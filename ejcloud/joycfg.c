#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"
#include "ej_wificonfig.h"
#include "joycfg.h"
#include "semphr.h"
#include "syslog.h"
#include "wifi_api.h"
#include "wifi_scan.h"
#include "nvdm.h"
log_create_module(joycfg, PRINT_LEVEL_INFO);

#define jy_debug_log 1
#ifdef  PRINTF_HIGH
#define printf_high printf
#else
#define printf_high
#endif

#define PAYLOAD_MIN		6
#define PAYLOAD_MAX		80+1


static jy_status_t jy_status;
static joycfg_context_t* jy_context;
TimerHandle_t joy_lock_timer = NULL;
static uint8_t cmpdata[5] = {0};


extern smt_info   saved_smt_info;
extern void atomic_write_smt_flag(uint8_t flag_value);

static joycfg_config_t jy_conf = {
    .memset = memset,
    .memcpy = memcpy,
    .memcmp = memcmp,
//    .memfree =vPortFree,
    .printf = NULL,
};

static uint8_t getCurrentChannel(); 


static void dump8(uint8_t* p, int split, int len)
{
	int i;
	char buf[512];
	int index = 0;
	for (i = 0; i < len; i++)
	{
		if (split != 0 && ((i + 1) % split) == 0)
		{
			index += sprintf(buf + index, "%02x,", p[i]);
		}
		else
			index += sprintf(buf + index, "%02x ", p[i]);
	}
	printf_high("Addr=%d,Len=%d:%s\r\n", p, len, buf);
}


static uint16_t CalcSum(uint8_t* addr, uint16_t len)
{
	uint16_t i;
	uint16_t sum = 0;
	for(i = 0; i < len; i++)
	{
		sum += *addr++;
	}
	return sum;
}

static uint8_t getCrc(uint8_t *ptr, uint8_t len)
{
	unsigned char crc;
	unsigned char i;
	crc = 0;
	while (len--)
	{
		crc ^= *ptr++;
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x01)
			{
				crc = (crc >> 1) ^ 0x8C;
			}
			else
				crc >>= 1;
		}
	}
	return crc;
}


static signed char jy_cfg_50msTimer(void)
{
	jy_context->chCurrentIndex = getCurrentChannel()-1;
	
	if (jy_context->directTimerSkip)
	{
		jy_context->directTimerSkip--;
		return 50;
	}
	if (jy_context->state == JOYCFG_STATUS_COMPLETE)
	{
		jy_context->directTimerSkip = 10000/50;
		return 50;
	}
	if (jy_context->isProbeReceived >0 )
	{
		printf_high("-------------------->Probe Stay(CH:%d) %d\n", getCurrentChannel(), jy_context->isProbeReceived);
		jy_context->isProbeReceived = 0;
		jy_context->directTimerSkip = 5000 / 50;
		return 50;
	}
	if (jy_context->chCurrentProbability > 0)	// 如果中间收到特征数据包,继续停留在当前频道
	{
		jy_context->chCurrentProbability--;
		printf_high("------------------->SYNC (CH:%d) %d\n", getCurrentChannel(), jy_context->chCurrentProbability);
		return 50;
	}

	jy_context->chCurrentIndex = (jy_context->chCurrentIndex + 1) % 13;
	//adp_changeCh(jy_context->chCurrentIndex +1);
	
	smt_continue_switch(); 
	jy_context->state = JOYCFG_STATUS_CONTINUE;
	jy_context->syncStepPoint = 0;
	jy_context->syncCount = 0;
	jy_context->chCurrentProbability = 0;
	
	
	return 50;
}

static  void joycfg_stop_switch(void)
{	
	xTimerStop(joy_lock_timer, tmr_nodelay);
    LOG_I(joycfg, "channel locked at %d, scaned times\n",
          getCurrentChannel());
}


static void jy_lock_timeout( TimerHandle_t tmr )
{
   
    
	int8_t msTimer=0;
	msTimer = jy_cfg_50msTimer();

	xTimerStart(joy_lock_timer, tmr_nodelay);
//	if(xTimerChangePeriod(joy_lock_timer,msTimer/portTICK_PERIOD_MS,tmr_nodelay) == pdPASS)
//	{
//			//printf("jy_lock_timeout joy_lock_timer xTimerChangePeriod !\n");		
//	}
//	else
//	{
//			printf("jy_lock_timeout joy_lock_timer xTimerChangePeriod is faied!\n");
//		
//	}
	
}


static int joycfg_init(joycfg_context_t* context )
{
		//memset 0 上下文
		
		memset(cmpdata,0,sizeof(cmpdata));
		memset(context,0,sizeof(joycfg_context_t));
		
		return 0;
}


static int jy_init(const unsigned char *key, const unsigned char key_length)
{
    int ret = 0;
	LOG_I(joycfg, "<INF>joycfg malloc jy_context !</INF>\n");
    jy_context = (joycfg_context_t *) pvPortMalloc(sizeof(joycfg_context_t));
    if(jy_context == NULL){
        LOG_E(joycfg, "<ERR>joycfg malloc jy_context failed!</ERR>\n");
        return -1;
    }
	
    ret = joycfg_init(jy_context);
	
    if(ret < 0){
        LOG_E(joycfg, "<ERR>joycfg init failed!</ERR>\n");
        return -1;
    }

    joy_lock_timer = xTimerCreate( "jy_lock_timer",
                                    (50/portTICK_PERIOD_MS), /*the period being used.*/
                                    pdFALSE,
                                    NULL,
                                    jy_lock_timeout);
    if (joy_lock_timer == NULL) {
		printf_high("joy_lock_timer is null\n");
        LOG_E(joycfg, "<ERR>jy_lock_timer create fail.</ERR>\n");
        return -1;
    }

	jy_status = JY_RCV_PRE;

	//xTimerStart(joy_lock_timer, tmr_nodelay);
    return ret;
}

static void jy_cleanup(void)
{
    if (joy_lock_timer != NULL) {
        xTimerDelete(joy_lock_timer, tmr_nodelay);
        joy_lock_timer = NULL;
    }

	
    if(jy_context != NULL){
        vPortFree(jy_context);
        jy_context = NULL;
    }
}

static int joy_change_channel()
{	
	return 0;
}


static int jy_channel_rst(void)
{
   	//airkiss_init(ak_context, &ak_conf);
   	int ret=0;
	ret = joycfg_init(jy_context);
	if(ret<0)
	{
		return 0;
	}
    jy_status = JY_RCV_PRE;
    joy_change_channel();

    return 0;
}

static int jy_get_result_NoEncode(joycfg_context_t* context, joycfg_result_t* result)
{
		//将取得的结果翻译为result
	uint8_t ssidLen;
	uint8_t passwordLen;

	memset(result,0,sizeof(joycfg_result_t));
    result->ssid_length= jy_context->payload[5];
	
	result->pwd_length = jy_context->payload[6];
	
    jy_conf.memcpy(result->ssid, &(jy_context->payload[0])+7, result->ssid_length);			
    
    jy_conf.memcpy(result->pwd, &(jy_context->payload[0])+7+result->ssid_length, result->pwd_length);
	

	LOG_I(joycfg,"passwordLen:%02d,ssid_length:%02d\n",result->pwd_length,result->ssid_length);

	LOG_I(joycfg,"password:%s,ssid:%s\r\n",result->pwd,result->ssid);


   	return 0;
	
		

}





static int jy_get_result(joycfg_context_t* context, joycfg_result_t* result)
{
		//将取得的结果翻译为result

	uint8_t ssidLen;
    uint8_t ssid[32] = {0};
    uint8_t password[64];
    uint8_t passwordLen;
	
    ssidLen = jy_context->payload[0];
    jy_conf.memcpy(result->ssid, jy_context->payload[1], ssidLen);			
    passwordLen = jy_context->payload[ssidLen + 1];
    jy_conf.memcpy(result->pwd, jy_context->payload[ssidLen + 1 + 1], passwordLen);
	result->pwd_length =passwordLen;
	result->ssid_length =ssidLen;

	LOG_I(joycfg,"passwordLen:%02d,ssid_length:%02d\n",passwordLen,ssidLen);
	LOG_I(joycfg,"password:%s,ssid:%s\n",result->ssid,result->ssid);

   return 0;
	
		

}

static int jy_get_info_NoEncode(void)
{
    joycfg_result_t result;

    xTimerStop(joy_lock_timer, tmr_nodelay);
    if(jy_get_result(jy_context, &result) < 0){
        LOG_E(joycfg, "<ERR>joycfg get result failed.</ERR>\n");
        atomic_write_smtcn_flag(SMT_FLAG_FAIL);
        return -1;
    }

    /*SSID*/
    saved_smt_info.ssid_len = result.ssid_length;
    if(saved_smt_info.ssid_len > WIFI_MAX_LENGTH_OF_SSID)
        saved_smt_info.ssid_len = WIFI_MAX_LENGTH_OF_SSID;
    memcpy(saved_smt_info.ssid, result.ssid, saved_smt_info.ssid_len);

    /*password*/
    saved_smt_info.pwd_len = result.pwd_length;
    if(saved_smt_info.pwd_len > WIFI_LENGTH_PASSPHRASE)
        saved_smt_info.pwd_len = WIFI_LENGTH_PASSPHRASE;
    memcpy(saved_smt_info.pwd, result.pwd, saved_smt_info.pwd_len);


    atomic_write_smt_flag(SMT_FLAG_FIN);
    return 0;
}


static int jy_get_info(void)
{
    joycfg_result_t result;

    //xTimerStop(joy_lock_timer, tmr_nodelay);
    if(jy_get_result_NoEncode(jy_context, &result) < 0){
        LOG_E(joycfg, "<ERR>joycfg get result failed.</ERR>\n");
        atomic_write_smtcn_flag(SMT_FLAG_FAIL);
        return -1;
    }

    /*SSID*/
    saved_smt_info.ssid_len = result.ssid_length;
    if(saved_smt_info.ssid_len > WIFI_MAX_LENGTH_OF_SSID)
        saved_smt_info.ssid_len = WIFI_MAX_LENGTH_OF_SSID;
    memcpy(saved_smt_info.ssid, result.ssid, saved_smt_info.ssid_len);

    /*password*/
    saved_smt_info.pwd_len = result.pwd_length;
    if(saved_smt_info.pwd_len > WIFI_LENGTH_PASSPHRASE)
        saved_smt_info.pwd_len = WIFI_LENGTH_PASSPHRASE;
    memcpy(saved_smt_info.pwd, result.pwd, saved_smt_info.pwd_len);

#if (jy_debug_log == 1)
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID+1] = {0};
    uint8_t passwd[WIFI_LENGTH_PASSPHRASE+1] = {0};

    memcpy(ssid, saved_smt_info.ssid, saved_smt_info.ssid_len);
    memcpy(passwd, saved_smt_info.pwd, saved_smt_info.pwd_len);

    LOG_I(joycfg, "ssid:%s/%d, passwd:%s/%d\n",
            ssid, saved_smt_info.ssid_len,
            passwd, saved_smt_info.pwd_len);
#endif

	//joycfg_profile_set_configfree(1);

	atomic_write_smt_flag(SMT_FLAG_FIN);
    return 0;
}




static int payLoadCheckNoEncode(uint8_t index)
{
	uint8_t crcNoEncode = 0;
	uint8_t sumNoEncode = 0;
	crcNoEncode =jy_context->payload[index];
	if(!crcNoEncode){
		return 1;
	}

	dump8(jy_context->payload,1,40);
	for(int i=7;i<index;i++)
	{
		if(jy_context->payload[i]==0)
			return 1;
	}
	sumNoEncode = ((CalcSum(jy_context->payload+5, index-5) + 10)& 0xFF);

	printf_high("[payLoadCheckNoEncode]:Sum->%02d:Crc->%02d\r\n", sumNoEncode ,crcNoEncode);
	
	if(sumNoEncode == (jy_context->payload[index])){
			return 0;
	}

	return 1;

}

static int payLoadCheck(void)
{
	uint8_t crc = getCrc(jy_context->payload+ 1, jy_context->payload[1]+1);
	printf_high("[payLoadCheck]:%02d:%02d\r\n", jy_context->payload[0],jy_context->payload[1]);
	if ((jy_context->payload[1] > PAYLOAD_MIN) &&
		(jy_context->payload[1] < PAYLOAD_MAX) &&
        (jy_context->payload[0] == crc))
	{
				printf_high("*******************CRC=%d**************\n", crc);
				joylinkResult_t* pRet = &jy_context->result;
				memcpy(pRet->encData, jy_context->payload+1, jy_context->payload[1]+1);
                jy_context->state = JOYCFG_STATUS_COMPLETE;
				jy_context->result.type= 3;
                return 0;

	}
	

	return 1;
}




static void BubbleSort(uint8_t *a, uint16_t n)
{
	uint16_t i,j;
	uint16_t temp = 0;
	uint16_t flag = 0;
	// 大循环，用于控制程序不再对已经排好序的数进行比较
	 for(i = 0; i < n-1; i++) 
	 {
		 flag = 0;
		 // 小循环，用于从后往前依次比较相邻数的大小，如>	 果后者小于前者，交换相邻元素
		for(j = n-1; j > i; j--) 
		{
				if(a[j] < a[j-1])
				{
					temp = a[j];
					 a[j] = a[j-1];
					 a[j-1] = temp;
					flag = 1; // 当此轮比较有交换发生，则继续比较
				 }
		}
		// 当此轮比较没有交换发生，则说明比较已经结束
		 if(flag == 0) 
		 break;
	 }
 }
static uint8_t CmpListNoEncode(uint8_t No)
{	
	//static uint8_t cmpdata[5] = {0};
	uint8_t dstdata[5] = {0};
	printf_high("********* CmpListNoEncode start *********\r\n");
	if(No != cmpdata[4] &&No != cmpdata[3] && No != cmpdata[2] && No != cmpdata[1] && No != cmpdata[0])
	{
		cmpdata[4] = cmpdata[3];
		cmpdata[3] = cmpdata[2];
		cmpdata[2] = cmpdata[1];
		cmpdata[1] = cmpdata[0];
		cmpdata[0] = No;

		memcpy(dstdata,cmpdata,sizeof(cmpdata));
		BubbleSort(dstdata,sizeof(dstdata));
		printf_high("********* Lock Ch Info *********\r\n");
		dump8(dstdata,1,5);

			if((dstdata[4] - dstdata[3]) == 1&&
			(dstdata[3] - dstdata[2]) == 1 && 
			(dstdata[2] - dstdata[1]) == 1 && 
			(dstdata[1] - dstdata[0]) == 1)
		{
				
				return 1;
		}

			

	}
	return 0;
}


static uint8_t CmpList(uint8_t No)
{	
	
	uint8_t dstdata[5] = {0};
	printf_high("********* CmpList start *********\r\n");
	if(No != cmpdata[4]&&No != cmpdata[3] && No != cmpdata[2] && No != cmpdata[1] && No != cmpdata[0])
	{
		cmpdata[4] = cmpdata[3];
		cmpdata[3] = cmpdata[2];
		cmpdata[2] = cmpdata[1];
		cmpdata[1] = cmpdata[0];
		cmpdata[0] = No;
		memcpy(dstdata,cmpdata,sizeof(cmpdata));
		BubbleSort(dstdata,sizeof(dstdata));
		printf_high("********* Lock Ch Info *********\r\n");
		dump8(dstdata,1,5);
		
			if((dstdata[4] - dstdata[3]) == 1 && 
			(dstdata[3] - dstdata[2]) == 1 && 
			(dstdata[2] - dstdata[1]) == 1 && 
			(dstdata[1] - dstdata[0]) == 1)
		{
				
				return 1;
		}

	}
	return 0;
}


static uint8_t getCurrentChannel()
{
		uint8_t currChannel=0;
		wifi_config_get_channel(WIFI_PORT_STA, &currChannel);
		return currChannel;
}

static  void  muticastAddNoEncode(uint8_t *pAddr)
{
		
	switch(jy_context->state)
	{
		case JOYCFG_STATUS_CONTINUE:
				{
					if(pAddr[3] == 118 && pAddr[5] ==0)
					{
						if (jy_context->chCurrentProbability < 20) 
									jy_context->chCurrentProbability += 3;
						printf_high("********* 118 start *********\r\n");
						jy_context->payload[pAddr[4]-1]=pAddr[5];
						if(CmpListNoEncode(pAddr[4]) == 1)
						{
							jy_context->state = JOYCFG_STATUS_CHANNEL_LOCKED;
							memset(jy_context->payload+5,0,sizeof(jy_context->payload)-5);
							joycfg_stop_switch();
						}
					}
					//dump8(pAddr,1,6);
					break;
				}
		case JOYCFG_STATUS_CHANNEL_LOCKED:
				{
						
					if(pAddr[3]== 119 || pAddr[3]== 120 || 
					   pAddr[3]== 121 || pAddr[3]== 122)
					{
						if (jy_context->chCurrentProbability < 20) 
									jy_context->chCurrentProbability += 3;		
						jy_context->payload[pAddr[4]-1]=pAddr[5];
						
						if(pAddr[3]==122){
							jy_context->indexCrcNoEncode= pAddr[4]-1;
							jy_context->crcNoEncode= pAddr[5]-10;
						}
						if(payLoadCheckNoEncode(jy_context->indexCrcNoEncode)==0)
						{
							jy_context->state = JOYCFG_STATUS_COMPLETE;
							return;
						}
					}

				break;
			}

	}

	
	return ;
	


	
}

static  void  muticastAdd(uint8_t *pAddr)
{
		
	switch(jy_context->state)
	{
		case JOYCFG_STATUS_CONTINUE:
				{
					if(pAddr[3]==0&&pAddr[4]==0x01)
					{
						if(pAddr[5]<5&&pAddr[5]>0)
						{
//								if(pAddr[5]==0x04){
//									printf("recive 01 00 5e 00 01 04\n");
//									jy_context->state = JOYCFG_STATUS_CHANNEL_LOCKED;
//								}
								printf("enter special data\n");
								if(CmpList(pAddr[5]) == 1)
									{
										jy_context->state = JOYCFG_STATUS_CHANNEL_LOCKED;
										joycfg_stop_switch();
									}
								
						}		

					}
					dump8(pAddr,1,6);
					break;
				}
		case JOYCFG_STATUS_CHANNEL_LOCKED:
				{
					int8_t index = 0;
					if (pAddr[3] == 0)
					{
						index = 0 - pAddr[3];
					}
					else if ((pAddr[3] >> 6) == ((pAddr[4] ^ pAddr[5]) & 0x1))
					{
						index = pAddr[3] & 0x3F;
					}
					
					else 
					{	
						printf("no he fa index!\n");
						return;
					}
					int expectLength = 1 + jy_context->mutiLastIndex;
					jy_context->mutiLastIndex = index;
					printf_high("Muticast Add Probability %d \n", jy_context->chCurrentProbability);
					if (expectLength == index)
					{
						printf_high("Muticast Add Probability %d \n", jy_context->chCurrentProbability);
						uint8_t payloadIndex = index - 1;
						if (payloadIndex > 64)
							return;
						if (jy_context->chCurrentProbability < 20) jy_context->chCurrentProbability += 3;		
						jy_context->payload[payloadIndex * 2]     = pAddr[4];
						jy_context->payload[payloadIndex * 2 + 1] = pAddr[5];
						printf_high("M%02d(CH=%d)--%02X:(%02X,%02X)\n", index, getCurrentChannel(), pAddr[3], pAddr[4], pAddr[5]); 
						if (payLoadCheck() == 0)
						{
							jy_context->state = JOYCFG_STATUS_COMPLETE;
							return;
						}
					}


				
				break;
			}

	}

	
	return ;
	


	
}

static int jy_recv_NoEncode(joycfg_context_t* context,  wlan_frame_t* frame, unsigned short length)
{
	joycfg_status_t status;			// 是否上行线路
	uint8_t  packetType = 0;			// 0=普通数据包, 1=组播数据包, 2=广播数据包
	//1.	先做上下行数转换
	uint8_t *pDest, *pSrc, *pBssid;
	if (frame->frame_type==0x88)
	{
		// 上行88, 下行08, 下行长2字节
		// 对于数据包: Addr1=Dest,Addr2=BSSID,Addr3=Src
		pBssid = frame->frame_data.data_info.Addr1;// 88 上行
		pSrc = frame->frame_data.data_info.Addr2;
		pDest = frame->frame_data.data_info.Addr3;
		length -= 2;

	}
	else
	{
		
		pDest = frame->frame_data.data_info.Addr1;	// 08 下行
		pBssid = frame->frame_data.data_info.Addr2;
		pSrc  = frame->frame_data.data_info.Addr3;

	}

	if (memcmp(pDest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0)
	{
		packetType = 2;
	}
	else if (memcmp(pDest, "\x01\x00\x5E", 3) == 0)
	{
		packetType = 1;
	}

	
	if (packetType == 0) 
		return jy_context->state;

	
	
	//printf("context->state :0x%02x\n",jy_context->state);
	if(jy_context->state == JOYCFG_STATUS_CONTINUE)
	{

				if (packetType == 1)
				{
					// 239.0.{Version}.4  +239.{(0*1bit)((byte[i]^byte[i+1])*1bit)(Index*6bit)}.{byte[i]}.{byte[i+1]}
					  //LOG_HEXDUMP_I(joycfg, "wlan_frame_t", frame, length);
					  muticastAddNoEncode(pDest);	// Internal state machine could delay the ch switching
					  return jy_context->state;
				}
				
				if (packetType == 2)
				{
					//return broadcastAdd(ascii);
					//  return context->multicastContext.state;

				}
			
				
		
	}
	else if (jy_context->state == JOYCFG_STATUS_CHANNEL_LOCKED)
	{

		if (packetType == 1)
		{// Muticast"01:00:5E: (0*1bit)((byte[i]^byte[i+1])*1bit)(Index*6bit): byte[i] : byte[i+1]"
		     //LOG_HEXDUMP_I(joycfg, "wlan_frame_t", frame, length);
			 muticastAddNoEncode(pDest);
		}else if ( packetType == 2)
		{
			
		}
		
	}

	return jy_context->state;
}



static int joycfg_recv(joycfg_context_t* context,  wlan_frame_t* frame, unsigned short length)
{
	joycfg_status_t status;			// 是否上行线路
	uint8_t  packetType = 0;			// 0=普通数据包, 1=组播数据包, 2=广播数据包
	//1.	先做上下行数转换
	uint8_t *pDest, *pSrc, *pBssid;
	//	if ((length > 100) && (context->state != JOYCFG_STATUS_CHANNEL_LOCKED))	// 在锁定之前收到大包不用处理
	//			return context->state;
	//LOG_HEXDUMP_I(joycfg, "wlan_frame_t", frame, length);
	if (frame->frame_type==0x88)
	{
		// 上行88, 下行08, 下行长2字节
		// 对于数据包: Addr1=Dest,Addr2=BSSID,Addr3=Src
		pBssid = frame->frame_data.data_info.Addr1;// 88 上行
		pSrc = frame->frame_data.data_info.Addr2;
		pDest = frame->frame_data.data_info.Addr3;
		length -= 2;

	}
	else
	{
		
		pDest = frame->frame_data.data_info.Addr1;	// 08 下行
		pBssid = frame->frame_data.data_info.Addr2;
		pSrc  = frame->frame_data.data_info.Addr3;

	}

	if (memcmp(pDest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0)
	{
		packetType = 2;
	}
	else if (memcmp(pDest, "\x01\x00\x5E", 3) == 0)
	{
		packetType = 1;
	}

	
	if (packetType == 0) 
		return jy_context->state;

	
	
	//printf("context->state :0x%02x\n",jy_context->state);
	if(jy_context->state == JOYCFG_STATUS_CONTINUE)
	{
				if (packetType == 1)
				{
					  LOG_HEXDUMP_I(joycfg, "wlan_frame_t", frame, length);
					// 239.0.{Version}.4  +239.{(0*1bit)((byte[i]^byte[i+1])*1bit)(Index*6bit)}.{byte[i]}.{byte[i+1]}
					  muticastAdd(pDest);	// Internal state machine could delay the ch switching
					  return jy_context->state;
				}
				
				if (packetType == 2)
				{
					//return broadcastAdd(ascii);
					//  return context->multicastContext.state;

				}
			
				
		
	}
	else if (jy_context->state == JOYCFG_STATUS_CHANNEL_LOCKED)
	{

		if (packetType == 1)
		{// Muticast"01:00:5E: (0*1bit)((byte[i]^byte[i+1])*1bit)(Index*6bit): byte[i] : byte[i+1]"
			 LOG_HEXDUMP_I(joycfg, "wlan_frame_t", frame, length);
			 muticastAdd(pDest);
		}else if ( packetType == 2)
		{
			
		}
		
	}

	return jy_context->state;
}


static int jy_rx_handler(char* phdr, int len)
{
   
    int ret;
    ret = jy_recv_NoEncode(&jy_context, phdr, len);
   
    switch(jy_status){
    case JY_RCV_PRE:
        if(ret == JOYCFG_STATUS_CHANNEL_LOCKED){
            LOG_I(joycfg, "<INFO>joycfg channel locked</INFO>\n");
            //smtcn_stop_switch();
			//joycfg_stop_switch();
            //xTimerStart(joy_lock_timer, tmr_nodelay);
            jy_status = JY_RCV_DATA;
        }
        break;

    case JY_RCV_DATA:
        if(ret == JOYCFG_STATUS_COMPLETE){
		    
            jy_status = JY_FIN;
		    jy_get_info();
			LOG_I(joycfg, "<INFO>joycfg Finished</INFO>\n");
	    }
        break;

	default:
		break;
//    case JY_FIN:
//        break;
    }

    return ret;
}

smt_proto_ops jy_proto_ops = {
    .init               =   &jy_init,
    .cleanup            =   &jy_cleanup,
    .switch_channel_rst =   &jy_channel_rst,
    .rx_handler         =   &jy_rx_handler,
};




