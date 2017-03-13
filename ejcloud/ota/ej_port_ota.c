#include "ej_port_ota.h"
#include "ej_cmd_mqtt.h"


static unsigned int ej_device_firmware_len;

int ej_wifi_fota_http(char *url)
{
	int ret = 1 ;
	if(!url)
	{
		EJ_Printf("ej_fota_download_package, ret = %d \r\n", ret);
		return ret;
	}
	ret = fota_download_by_http(url);
	EJ_Printf ("fota_download_by_http, ret = %d \r\n", ret);

	if(!ret)
	{
		reportWifiUpgradeInfoToCloud();
	}
	///sleep 3s
	EJ_thread_sleep(3000);
	if(!ret)
	{
		// download success
		fota_trigger_update();		
		// TODO: reboot device
		hal_wdt_config_t wdt_config;
		wdt_config.mode = HAL_WDT_MODE_RESET;
		wdt_config.seconds = 30;
		hal_wdt_init(&wdt_config);	
		hal_wdt_software_reset();
	}

    EJ_Printf("fota_result, ret = %d \r\n", ret);
    return ret;
}

int EJ_wifi_fota_task(void *arg)
{
#if 0
    EJ_thread_create(ej_wifi_fota_http, "FOTADownloadTask", 5*1024/sizeof(unsigned int), arg, 3, NULL);
#endif
	EJ_Printf("EJ wifi fota start, url=%s\r\n",arg);
	return ej_wifi_fota_http(arg);
}


int EJ_wifi_firmware_download(const char *url_str, uint32_t datasize)
{
	return  EJ_SUCCESS;
}

int EJ_device_firmware_download(const char *url_str, uint32_t datasize)
{
	int ret = 1 ;
	if(!url_str)
	{
		EJ_Printf("ej_fota_download_package, ret = %d \r\n", ret);
		return ret;
	}
	ret = fota_download_by_http(url_str);
	EJ_Printf("fota_download_by_http, ret = %d \r\n", ret);
	if(ret!=0)
	{	
		return  -EJ_FAIL;
	}
	ej_device_firmware_len = datasize;
	return  EJ_SUCCESS;
}

#define DEVICE_BLOCK_SIZE 64
static int ej_read_frame_from_flash(unsigned int offset,unsigned char *buffer,unsigned int length)
{
	memset(buffer,0,length);
	int ret1 = fota_seek(FOTA_PARITION_TMP, offset);
	if(0!=ret1) {
		return -3;
	}
	int ret2 = fota_read(FOTA_PARITION_TMP, buffer, length );
	if(0!=ret2)
	{
		return -EJ_FAIL;
	}
	return EJ_SUCCESS;
	
}

static int ej_write_frame_from_flash(unsigned int offset,unsigned char *buffer,unsigned int length)
{
	if (!fota_seek(FOTA_PARITION_TMP, offset)) {
		return -3;
	}

	if(!fota_write(FOTA_PARITION_TMP, buffer, DEVICE_BLOCK_SIZE ))
	{
		return EJ_SUCCESS;
	}

	return -EJ_FAIL;
}


static int ej_init_frame_from_flash()
{
	if (fota_init(&fota_flash_default_config) != FOTA_STATUS_OK) {
        return -2;
    }

	return EJ_SUCCESS;
}


static bool deviceOtaFail = false;
static void deviceota_timer_cb(ej_timer_arg_t arg)
{
	deviceOtaFail = true;
}

static int ej_device_fota(unsigned int image_len,unsigned int frame_size)
{

	signed   int ret = 0 ;
	unsigned char buffer[frame_size];
	unsigned int  frame_num = 0;
	unsigned int  file_size = 0;	
	
	int dived = (ej_device_firmware_len - 4)/frame_size;
	frame_num = dived?(dived+1):(dived);
	
	static ej_timer_t deviceota_timer;
	if(EJ_timer_create(&deviceota_timer,
                          "deviceota-timer",
                          EJ_msec_to_ticks(2000),
                          &deviceota_timer_cb,
                          NULL,
                          EJ_TIMER_ONE_SHOT,
                          EJ_TIMER_NO_ACTIVATE)!=0)
	{
		EJ_Printf("[ej_device_fota]EJ_timer_create fail\r\n");
		return -EJ_FAIL;
	}

    //2. send to device throught uart
	//2.1  send  first  frame
	EJ_Printf("[ej_device_fota]send first frame!\r\n");
	unsigned int offset = 0;
	unsigned int frame_no = 1 ;
	if (ej_init_frame_from_flash()!=0) {
			
            return -2;
    }
	if (ej_read_frame_from_flash(offset,buffer,frame_size)!=0) {
            return -3;
    }
	uart2WifiPacket *pRequestPacket = getDeviceUpdateFramePacket(frame_num, frame_no, buffer);
	if (nolock_list_push(GetWifi2deviceList(), pRequestPacket) != 0x01)
	{
		EJ_Printf("[ej_device_fota]add packet to wifi2devicelist failed.\r\n");
		EJ_PacketUartFree(pRequestPacket);
		pRequestPacket = NULL;
		return -EJ_FAIL;
	}

	//2.2  wait for frame ack then send 
	uart2WifiPacket *pReceiveAckPacket = NULL;	

	while((((frame_no + 1)* DEVICE_BLOCK_SIZE)<(ej_device_firmware_len - 4))&&!deviceOtaFail)
	{
		nolock_list_pop(GetDevice2FirmwareOtaAckList(),(void **)&pReceiveAckPacket);
		if(pReceiveAckPacket)
		{
			EJ_PrintUart2WifiPacket(pReceiveAckPacket,"[OtaAck Packet]");
			EJ_timer_reset(&deviceota_timer);						
			if(pReceiveAckPacket->data[0] == 0x00)
			{
				//frame_no = pReceiveAckPacket->data[1]+1;
				frame_no++;
			}
			EJ_PacketUartFree(pReceiveAckPacket);
			pReceiveAckPacket = NULL;			
			offset = frame_no * DEVICE_BLOCK_SIZE;
			if (!ej_read_frame_from_flash(offset,buffer,frame_size)) 
			{					
	   			uart2WifiPacket * pRequestPacket = getDeviceUpdateFramePacket(frame_num, frame_no, buffer);
				if (nolock_list_push(GetWifi2deviceList(), pRequestPacket) != 0x01)
				{
					EJ_mem_free(pRequestPacket);
					pRequestPacket = NULL;
					EJ_Printf("[ej_device_fota]add packet to wifi2devicelist failed.\r\n");
					return -EJ_FAIL;
				}
			}	
		}
	};

	return EJ_SUCCESS;
}

void EJ_device_fota_task(unsigned int  frame_size)
{
	int ret =  ej_device_fota(ej_device_firmware_len,frame_size);
	EJ_Printf("[EJ_device_fota_task]ej_device_fota imagelen = %d ,framesize = %d ,ret = %d\r\n",ej_device_firmware_len,frame_size,ret);
	//EJ_App_reboot(1);
}




