#include "ej_port_wlan.h"
#include "ej_packet.h"
#include "ej_utils.h"
#include "ej_user_udpbroadcast.h"


static ej_thread_t UDPBroadcastThread_thread;
static ej_thread_stack_define(UDPBroadcastThread_stack, 4096);

unsigned char udpciphertext[256];
unsigned char udpplaintext[256];

int rxsock = -1;
int txsock = -1;
struct sockaddr_in from;
struct sockaddr_in addrto;
uint8_t sipaddr[22] = {0x00};

int GetUdpPacketType(char *PacketType)
{
	int ret = 0;
	if(strcmp(PacketType,"T_call") == 0) 
	{
		ret = 1;
	}else if(strcmp(PacketType,"T_gatewayInfo") == 0)
	{
		ret = 2;
	}else if(strcmp(PacketType,"T_getdid") == 0)
	{
		ret = 3;
	}
	return ret;
}


static char* UDPEncodeDatapart(char * pData)
{
	uint8_t pClitext[128];
	uint8_t pClitextHex[512];
	memset(pClitext,0,sizeof(pClitext));
	memset(pClitextHex,0,sizeof(pClitextHex));
	int cryptLength = EJ_Aes_udpBroadcast_Encrypt(pData,strlen(pData),pClitext);
	EJ_Printf("pClitextHex = %d\r\n",cryptLength);
	ChangeHex2Str(pClitext, pClitextHex,cryptLength);
	EJ_Printf("pClitextHex = %s\r\n",pClitextHex);
	return (char*)base64_encode(pClitextHex,cryptLength*2);
			
}

static uint8_t* UDPDecodeDatapart(char * pData)
{						
	char *retde64=(char*)base64_decode(pData,strlen(pData));
	EJ_Printf("decode64 :%s ,len=%d\r\n",retde64,strlen(retde64));
	char bits[128]={0};
	char decode[256]={0};
	memset(decode,0,sizeof(decode));
	ChangeStrHex(retde64,bits);			
	EJ_Aes_udpBroadcast_Decrypt(bits,strlen(retde64)/2,decode);
	EJ_mem_free(retde64);
	EJ_Printf("decode :%s ,len=%d\r\n",decode,strlen(decode));
	uint8_t padding_size = decode[strlen(decode) - 1];				
	uint8_t  *dataPart=(uint8_t *)EJ_mem_malloc(strlen(decode)-padding_size);
	memcpy(dataPart,decode,strlen(decode)-padding_size);
	EJ_Printf("dataPart :%s ,len=%d,padsize=%d\r\n",dataPart,strlen(dataPart),padding_size);
	return dataPart;
}



void PublishUDPMessage(void *payload, int dataLen)
{
	int rc = 0;
#if defined(WIN32) || defined(WIN64)
	
#else
	int ret = 0;
	int nlen=sizeof(addrto); 
	ret = sendto(txsock, payload, dataLen, 0, (struct sockaddr*)&addrto, nlen);  
	if(ret<0)  {  
		EJ_ErrPrintf(("[udpbroadcast.c][UDPBroadcastThread][ERROR]: sendto failed.\r\n"));  
	}  
	else  {		  
		//AJ_AlwaysPrintf(("[udpbroadcast.c][UDPBroadcastThread][INFO]: [sendto num :%d]send an broadcast success.\r\n",ret));
	} 

#endif	

}

uint8_t LanSignStr[] = "EnPntyOER8vx4vI67qACK2eC5Xl8TPntg3qe";

void PubWifi2UDPPacket(wifi2CloudPacket *pPacket)
{
	if (pPacket) {		
		uint8_t *buf = (uint8_t *)EJ_mem_malloc(pPacket->dataLen[0] - 16 -8);
		uint8_t *temp = (uint8_t *)pPacket;
		if (buf) {
			memcpy(buf, temp + 8, 20);
			memcpy(buf + 20, pPacket->data, pPacket->dataLen[0] - 20 - 16 - 8);

			int cipherSize = EJ_Aes_lanMessageEncrypt(buf, pPacket->dataLen[0] - 16 - 8, udpciphertext);
			if (cipherSize) {
				uint8_t *payload = (uint8_t *)EJ_mem_malloc(8 + cipherSize + strlen(LanSignStr));

				if (payload) {

					memcpy(payload, pPacket, 8);
					memcpy(payload + 8, udpciphertext, cipherSize);
					memcpy(payload + 8 + cipherSize, LanSignStr, strlen(LanSignStr));				
					uint8_t tk[16];
					EJ_hash_md5(payload, 8 + cipherSize + strlen(LanSignStr), tk, 16);
					memcpy(payload + 8 + cipherSize, tk, 16);
					PublishUDPMessage(payload, 8 + cipherSize + 16);
					EJ_mem_free(payload);

				}else {
					EJ_ErrPrintf(("[LANThread.c][PubWifi2LANPacket][ERROR]: EJ_mem_malloc failed.\r\n"));

				}

			}

			EJ_mem_free(buf);
			
		}else {
			EJ_ErrPrintf(("[LANThread.c][PubWifi2LANPacket][ERROR]: EJ_mem_malloc failed.\r\n"));
		}
	}
}



void PubWifi2UDPPacketNoEncypt(wifi2AppPacket *pPacket)
{
	uint8_t sendUdpbuf[256]={0};
	if (pPacket) {
			uint8_t type =  GetUdpPacketType(pPacket->PacketType);
			switch(type)
			{
					case 1:
						sprintf(sendUdpbuf,"%s,%s,%s,%s,%s,%s,%ld,%s,",pPacket->cmd,pPacket->version,
											pPacket->PacketType,pPacket->deviceID,
											pPacket->PacketID,pPacket->timeStamp,strlen(pPacket->data),pPacket->data);
						break;
					case 2:
						sprintf(sendUdpbuf,"%s,%s,%s,%s,%s,%s,%ld,%s,",pPacket->cmd,pPacket->version,
														pPacket->PacketType,pPacket->deviceID,
														pPacket->PacketID,pPacket->timeStamp,strlen(pPacket->data),pPacket->data);
						break;
					case 3:
						sprintf(sendUdpbuf,"%s,%s,%s,%s,%s,%s,%ld,%s,",pPacket->cmd,pPacket->version,
														pPacket->PacketType,pPacket->deviceID,
														pPacket->PacketID,pPacket->timeStamp,strlen(pPacket->data),pPacket->data);
						break;
					default:
						break;

			}			
		EJ_Printf("pPacket->sendUdpbuf=%s;len=%ld\r\n",sendUdpbuf,strlen(sendUdpbuf));
		PublishUDPMessage(sendUdpbuf, strlen(sendUdpbuf));		
	}

}




static void GetBroadcastResponsePacket(PakcetWifiModuleBroadcastResponse* discoverResponse)
{

	//uuid
	memset(discoverResponse,0,sizeof(PakcetWifiModuleBroadcastResponse));
	GetWifiStatusDeviceUuid(discoverResponse->deviceUuid);
	if(!strlen(discoverResponse->deviceUuid))
	{
		sprintf(discoverResponse->deviceUuid,"NULL");
	}
	//did
	uint8_t did[6]={0};
	GetWifiStatusDeviceID(did);
	if(did[0]|did[1]|did[2]|did[3]|did[4]|did[5])
	{
		snprintf(discoverResponse->did,13,"%02X%02X%02X%02X%02X%02X",did[0],did[1],did[2],did[3],did[4],did[5]);

	}else{
		sprintf(discoverResponse->did,"NULL");
	}	
	//mac					
	EJ_Wlan_get_mac_address(discoverResponse->mac);
	//ipaddr
	EJ_Wlan_get_ip_address(discoverResponse->ipaddr);



}


static void UDPBroadcastThread(void *arg)
{

	fd_set rfd;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	ip_addr_t addr;
	char *ip = NULL;
	char *p;
	char *delim = ":";
	EJ_DebugPrintf(("[UDPBroadcastThread][INFO]: started.\r\n"));  
	char receiveData[256] = {0};  
	char sendData[512] = {0};  
	for (;;) {

		FD_ZERO(&rfd);
		FD_SET(rxsock, &rfd); 
		int nRet = select(rxsock + 1, &rfd, NULL, NULL, &timeout);
		if (nRet < 0){
			EJ_Printf("Select ERR: %s!\r\n");
			continue;
		}else if (nRet == 0){
		   /*TimeOut*/
			continue;
		}
		if (FD_ISSET(rxsock, &rfd))
		{
						int nlen=sizeof(from); 
						memset(receiveData,0,sizeof(receiveData));
						memset(sendData,0,sizeof(receiveData));
						int ret = recvfrom(rxsock, receiveData, 256, 0, (struct sockaddr*)&from, (socklen_t*)&nlen); 
						if(ret > 0)  
						{
				
								int numTest=1;
								numTest = strsplinum(receiveData,delim); 
								char *arrRecvdata[numTest]; 
								split(arrRecvdata,receiveData,delim);
							
								uint8_t  dataPart[128]={0};
								PakcetWifiModuleBroadcastResponse discoverResponse;
								GetBroadcastResponsePacket(&discoverResponse);
								
								switch(GetUdpPacketType(arrRecvdata[2]))
								{
									case 1:
				
										sprintf(dataPart,"%s,%s,%s,%s,",discoverResponse.deviceUuid,discoverResponse.did,discoverResponse.mac,discoverResponse.ipaddr); 
				
										sprintf(sendData,"%s,%s,%s,%s,%s,%d,%d,","Response",arrRecvdata[1],arrRecvdata[2],
														discoverResponse.did,arrRecvdata[4],EJ_time_get_posix(),strlen(dataPart));
				
										strcat (sendData,dataPart); 
										PublishUDPMessage(sendData, strlen(sendData));	
										break;
									case 2:
										break;				
									case 3:
										if(strlen(discoverResponse.deviceUuid)!=6||discoverResponse.deviceUuid=="")
										{
												DeviceInfo_save_uuid(arrRecvdata[7]);
												DeviceInfo_load_uuid(discoverResponse.deviceUuid);

										}									
										sprintf(dataPart,"%s,%s,%s,%s,",arrRecvdata[7],discoverResponse.did,discoverResponse.mac,discoverResponse.ipaddr);		
										sprintf(sendData,"%s,%s,%s,%s,%s,%d,%d,","Response",arrRecvdata[1],arrRecvdata[2],
														discoverResponse.did,arrRecvdata[4],EJ_time_get_posix(),strlen(dataPart));

										strcat (sendData,dataPart); 
										PublishUDPMessage(sendData, strlen(sendData));
										break;			
									default:
										break;
								}
						}
				
				
		}
	
		/*read wifi2udp list and send */
		wifi2AppPacket *pWifi2AppPacket = NULL;
		nolock_list_pop(GetWifi2udpList(), (void **)&pWifi2AppPacket);
		if (pWifi2AppPacket) {
			PubWifi2UDPPacketNoEncypt(pWifi2AppPacket);
			EJ_mem_free(pWifi2AppPacket->data);
			EJ_mem_free(pWifi2AppPacket);
			EJ_Printf("[udpbroadcast.c][UDPBroadcastThread][INFO]: process an packet from wifi to udp.\r\n");  
		}
		EJ_thread_sleep(EJ_msec_to_ticks(500));
	}
			

}


int EJ_init_udpbroadcast()
{
	int ret = INIT_UDPBROADCAST_SUCCESS;
	//netif_add_udp_broadcast_filter(9888);
	memset(&addrto, 0,sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	addrto.sin_port = htons(9887);

	memset(&from, 0,sizeof(struct sockaddr_in));
	from.sin_family = AF_INET;
	//from.sin_addr.s_addr = IPADDR_ANY;
	from.sin_addr.s_addr = htonl(IPADDR_ANY);
	from.sin_port = htons(9888);
	
	if ((rxsock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		EJ_ErrPrintf(("[Init_udpbroadcast][ERROR]: rxsocket create failed.\r\n"));
		return INIT_UDPBROADCAST_SOCKET_CREATE_ERROR;
	}

	if ((txsock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		EJ_ErrPrintf(("[Init_udpbroadcast][ERROR]: txsocket create failed."));	
		close(rxsock);
		rxsock = -1;
		return INIT_UDPBROADCAST_SOCKET_CREATE_ERROR;
	}     

	const int opt = 1;
	int nb = 0;
	int timeout = 500;
	if(nb == -1)
	{
		EJ_ErrPrintf(("[udpbroadcast.c][Init_udpbroadcast][ERROR]: rxsocket set broadcast.\r\n"));
		return INIT_UDPBROADCAST_SOCKET_SET_BROADCAST_ERROR;
	}
	int on = 1;
	nb = setsockopt(rxsock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	nb = setsockopt(txsock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1)
	{
		app_d("[udpbroadcast.c][Init_udpbroadcast][ERROR]: txsocket set broadcast.");
		return INIT_UDPBROADCAST_SOCKET_SET_BROADCAST_ERROR;
	}

	if(bind(rxsock,(struct sockaddr *)&(from), sizeof(struct sockaddr_in)) == -1) 
	{   
		EJ_ErrPrintf(("[udpbroadcast.c][Init_udpbroadcast][ERROR]: socket bind failed.\r\n"));
		close(rxsock);
		rxsock = -1;
		close(txsock);
		txsock = -1;
		return INIT_UDPBROADCAST_SOCKET_BIND_ERROR;
	}


	ret = EJ_thread_create(&UDPBroadcastThread_thread,
								"UDPBroadcastThread",
								(void *)UDPBroadcastThread, 0,
								&UDPBroadcastThread_stack, EJ_PRIO_3);

	if (ret) {
		EJ_ErrPrintf(("[udpbroadcast.c][Init_udpbroadcast][ERROR]: Unable to create UDPBroadcastThread.\r\n"));
		close(rxsock);
		rxsock = -1;
		close(txsock);
		txsock = -1;
		return INIT_UDPBROADCAST_OS_THREAD_CREATE_ERROR;
    	}

	return ret;
}

int EJ_uninit_udpbroadcast()
{
	EJ_thread_delete(&UDPBroadcastThread_thread);
	if (rxsock) {
		close(rxsock);
		rxsock = -1;
	}	
	if (txsock) {
		close(txsock);
		txsock = -1;
	}	
	return 0;
}


