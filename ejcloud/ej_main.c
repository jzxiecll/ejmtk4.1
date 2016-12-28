#include "ej_main.h"

static ej_thread_t mainLoop_thread;
extern InfoManagement h_InfoManagement;
extern ej_queue_t *ejEventQueue;
static ej_thread_stack_define(mainLoop_stack, 2048);
ej_timer_t atTaskTimer;

void atTaskTimerCB()
{
	static int timerCount = 0;
	static int timeSyncCount = 0;
	timerCount++;
	uint8_t deviceID[6] = {0};
	GetWifiStatusDeviceID(deviceID);	
	int total = deviceID[0] | deviceID[1] | deviceID[2] | deviceID[3] | deviceID[4] | deviceID[5];
	EJ_Printf("atTaskTimerCB total1=%d\r\n",total);
	if (total == 0) {
		EJ_Printf("atTaskTimerCB total2=%d\r\n",total);
		if (EJ_timer_change(&atTaskTimer, EJ_msec_to_ticks(5000), 0) != EJ_SUCCESS) {
      
          EJ_Printf("Failed to change jd50msTimer's period.\r\n");
    	}	
		EJ_PutEventSem(EJ_EVENT_reApplyIDSem);
	}		
	/* if timerCount is (n * 2)days, then sync time from server. */
	if ((timerCount % 360) == 0) {
		timeSyncCount++;
		timerCount = 0;
		if (timeSyncCount == 24) {
			EJ_PutEventSem(EJ_EVENT_timeSyncSem);
			timeSyncCount = 0;		
		}
	}
}

void setupAtTaskTimer()
{
	if (EJ_timer_create(&atTaskTimer,
			    "atTaskTimer",
			    EJ_msec_to_ticks(10000),
			    &atTaskTimerCB,
			    NULL,
			    EJ_TIMER_ONE_SHOT,
			    EJ_TIMER_AUTO_ACTIVATE) != EJ_SUCCESS) {
		EJ_ErrPrintf(("[mainLoop.c][setupAtTaskTimer][ERROR]: Failed to create atTaskTimer timer.\r\n"));
	}
}

void initMainLoop(uint8_t isHomeAPConfig)
{

	EJ_Printf("Build Time: "__DATE__" "__TIME__"\r\n");
	int ret =0;
	
	SetWifiModuleStatusIsHomeAPConfig(isHomeAPConfig);
	
	if (!isHomeAPConfig) {
		EJ_PutEventSem(EJ_EVENT_homeApNotConfiguredSem);
	}
	
	ret = EJ_thread_create(&mainLoop_thread,
		"mainLoop",
		(void *)mainLoop, 0,
		&mainLoop_stack, EJ_PRIO_3);

	if (ret)
		EJ_ErrPrintf(("[initMainLoop][ERROR]: Unable to create mainLoop thread.\r\n"));

	
/*	*/
}

/*
****************************************************************************
*
****************************************************************************
*/
static void EJ_event_shouldUARTThreadWorkProcess (void* data){		}
static void EJ_event_shouldMQTTThreadWorkProcess(void* data){		}
static void EJ_event_ezconnectCompletedProcess(void* data)
{		

}
static void EJ_event_routerConnectedProcess(void* data)
{		
	//EJ_InfoPrintf(("[mainloop.c][mainLoop][INFO]:receive an roter connectedSem event.\r\n"));
			/* */
			SetWifiModuleStatusRouterStatus(ROUTER_CONNECTED);
			/* if udpbroadcast module is not running. then run it.*/
			if (GetWifiModuleStatusIsUdpBroadcastRunning() == UDPBROADCASTMODULE_NOT_RUNNING) {
					if (EJ_init_udpbroadcast() == INIT_UDPBROADCAST_SUCCESS) {
						SetWifiModuleStatusIsUdpBroadcastRunning(UDPBROADCASTMODULE_RUNNING);
					}else{
						EJ_PutEventSem(EJ_EVENT_routerConnectedSem);
					}
			}
	
			/* if cloud services is not connected. */
			if (GetWifiModuleStatusCloudServiceStatus() == CLOUD_NOT_CONNECTED) {
				EJ_Printf("Call EJ_user_init_MQTTThread \r\n");
				/* if wifimodule is connected to home AP. */	
				if (EJ_user_init_MQTTThread() == INIT_MQTT_SUCCESS) {
						EJ_InfoPrintf(("[mainloop.c][mainLoop][INFO]: connect cloud services success.\r\n"));
						SetWifiModuleStatusCloudServiceStatus(CLOUD_CONNECTED);
						uint8_t  pArrayMac[12]={0};
						uint8_t  pArrayuuid[6]={0};
						EJ_Wlan_get_mac_address(pArrayMac);
						EJ_Device_get_uuid(pArrayuuid);
						wifi2CloudPacket * pWifi2CloudPacket = (wifi2CloudPacket *)requestDeviceIDFromCloud(pArrayMac,pArrayuuid);
						if (!pWifi2CloudPacket) {
							return;
						}
						if (nolock_list_push(GetWifi2cloudList(), pWifi2CloudPacket) != 0x01)
						{
							EJ_ErrPrintf(("[MQTTThread.c][Init_MQTTThread][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
						}
						setupAtTaskTimer();					
					}else{
					
						EJ_PutEventSem(EJ_EVENT_routerConnectedSem);
					}	

			}




	
}
static void EJ_event_broadcastRequestProcess(void* data)
{	  
		/*
	*Need to Do ...................................
	*/
	// if lan module is not running, then run it. 
	if (GetWifiModuleStatusIsLanModuleRunning() == LANMODULE_NOT_RUNNING) {
#if 0
		if (init_LANServer() == INIT_LANSERVER_SUCCESS) {
			SetWifiModuleStatusIsLanModuleRunning(LANMODULE_RUNNING);
		}
#endif
	}

	// send an response broadcast packet.
	//PutBroadcastResponseSem();	

	EJ_PutEventSem(EJ_EVENT_broadcastResponseSem);
}
static void EJ_event_broadcastResponseProcess	(void* data){	  }
static void EJ_event_uapStartedProcess(void* data)
{					
	/*
	*Need to Do ...................................
	*/
	/* if lan module is not running, then run it. */
	if (GetWifiModuleStatusIsLanModuleRunning() == LANMODULE_NOT_RUNNING) {

#if 0
		if (init_LANServer() == INIT_LANSERVER_SUCCESS) {

			SetWifiModuleStatusIsLanModuleRunning(LANMODULE_RUNNING);

		}else {

		}	
#endif
	}	


}


static void EJ_event_easylinkRequestProcess (void* data)
{		

	/*
	*Need to Do ...................................
	*/

	/* if cloud service is running, then shutdown it. */
	if (GetWifiModuleStatusCloudServiceStatus() == CLOUD_CONNECTED) {
		EJ_user_uninit_MQTTThread();
		SetWifiModuleStatusCloudServiceStatus(CLOUD_NOT_CONNECTED);
	}

	/* if lan service is running, then shutdown it. */
	if (GetWifiModuleStatusIsLanModuleRunning() == LANMODULE_RUNNING) {
		//UnInit_LANServer();
		SetWifiModuleStatusIsLanModuleRunning(LANMODULE_NOT_RUNNING);
	}

	/* if udp service is running, then shutdown it.*/
	if (GetWifiModuleStatusIsUdpBroadcastRunning() == UDPBROADCASTMODULE_RUNNING) {
		EJ_uninit_udpbroadcast();
		SetWifiModuleStatusIsUdpBroadcastRunning(UDPBROADCASTMODULE_NOT_RUNNING);
	}

	/* if wifi station is running, then shutdown it. */
	if(EJ_Wlan_is_sta_connected()){
		EJ_Wlan_sta_stop();
		SetWifiModuleStatusRouterStatus(ROUTER_NOT_CONNECTED);
	}
	/* if wifi uap is running, then shutdown it. */
	if (EJ_Wlan_is_uap_started()) {
		EJ_Wlan_uap_stop();
	}
	SetWifiModuleStatusIsHomeAPConfig(HOMEAP_NOT_CONFIGURED);
	/* set wificonfig flag to softAP, and reboot. */
	SetWifiConfigConfigMode(WIFICONFIG_EZCONNECT_MODE);
	/* before reboot, should delay some ms to wait uart send thread.*/
	EJ_thread_sleep(EJ_msec_to_ticks(100));	
	EJ_App_network_configured(0);	
	EJ_App_reboot(0);
	

}
static void EJ_event_softAPRequestProcess(void* data)
{	  
	/*
	*Need to Do ...................................
	*/
	/* set wificonfig flag to softAP, and reboot. */
	//SetWifiConfigConfigMode(WIFICONFIG_AP_MODE);
	/* before reboot, should delay some ms to wait uart send thread.*/
	EJ_thread_sleep(EJ_msec_to_ticks(100));
	EJ_App_network_configured(0);
	EJ_App_reboot(0);
}
static void EJ_event_airkissRequestProcess(void* data)
{		

	EJ_event_easylinkRequestProcess(data);
}
static void EJ_event_MQTTConnectionLostProcess(void* data)
{		
	/*
	*Need to Do ...................................
	*/
	EJ_ErrPrintf(("[mainloop.c][mainLoop][ERROR]: receive an mqtt connection lost event.\r\n"));
	//UnInit_MQTTThread();
	EJ_user_uninit_MQTTThread();
	SetWifiModuleStatusCloudServiceStatus(CLOUD_NOT_CONNECTED);

}
static void EJ_event_rebootRequestProcess(void* data)
{		
	//AJ_AlwaysPrintf(("[mainloop.c][mainLoop][ERROR]: receive an reboot request prepare to reboot.\r\n"));
	/* before reboot, should delay some ms to wait uart send thread.*/
	EJ_thread_sleep(EJ_msec_to_ticks(100));
	EJ_App_reboot(0);

}
static void EJ_event_Reset2FactoryRequestProcess(void* data)
{		
	//AJ_AlwaysPrintf(("[mainloop.c][mainLoop][ERROR]: prepare to reset and reboot.\r\n"));
	EJ_App_network_configured(0);
	/* before reboot, should delay some ms to wait uart send thread.*/
	EJ_thread_sleep(EJ_msec_to_ticks(100));
	EJ_App_reboot(0);

}
static void EJ_event_routerAuthFailedProcess(void* data)
{		
	//AJ_AlwaysPrintf(("[mainloop.c][mainLoop][ERROR]: wifi password error prepare to reboot and re-enter wifi config mode.\r\n"));	
	EJ_App_network_configured(0);	
	/* before reboot, should delay some ms to wait uart send thread.*/
	EJ_thread_sleep(EJ_msec_to_ticks(100));	
	EJ_App_reboot(0);

}
static void EJ_event_reApplyIDProcess(void* data)
{		

	/*
	*Need to Do ...................................
	*/

	uint8_t  StringUuid[EJ_USER_UUID_STRTING_LEN+1]={0};
	uint8_t  StringMac[EJ_USER_MAC_STRTING_LEN+1]={0};
	EJ_Wlan_get_mac_address(StringMac);
	EJ_Device_get_uuid(StringUuid);
	wifi2CloudPacket * pWifi2CloudPacket = (wifi2CloudPacket *)requestDeviceIDFromCloud(StringMac,StringUuid);
	if (!pWifi2CloudPacket) {
		EJ_ErrPrintf(("[ERROR]: create reApplyID packet failed.\r\n"));
		return;
	}
	
	if (nolock_list_push(GetWifi2cloudList(), pWifi2CloudPacket) != 0x01)
	{
		EJ_ErrPrintf(("[ERROR]: add reApplyID packet to wifi2cloudlist failed.\r\n"));
	}else{
		EJ_DebugPrintf(("[SUCCESS]: add reApplyID packet to wifi2cloudlist .\r\n"));
	}


				
}
static void EJ_event_timeSyncProcess(void* data)
{

	/*
	*Need to Do ...................................
	*/
	wifi2CloudPacket *pWifi2CloudPacket = (wifi2CloudPacket *)getSystemTimeFromCloud();	
	if (!pWifi2CloudPacket) {
		return;
	}
	if (nolock_list_push(GetWifi2cloudList(), pWifi2CloudPacket) != 0x01)
	{
		EJ_ErrPrintf(("[MQTTThread.c][atTaskTimerCB][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
	}

}

static void EJ_event_homeApNotConfiguredProcess(void* data)
{

	unsigned char configMode = GetWifiConfigConfigMode();

	if (configMode == WIFICONFIG_NULL_MODE) {

		configMode = GetDeviceInfoDefaultConfigMode();
	}
	
	EJ_WifiConfigProcess(configMode);
		

}


static void EJ_default_process()
{	
	/* if home AP is not configured. */
	if (GetWifiModuleStatusIsHomeAPConfig() == HOMEAP_NOT_CONFIGURED) {
		/* get config mode. */
		uint8_t configMode = GetWifiConfigConfigMode();
#if 0
		if (configMode == WIFICONFIG_NULL_MODE) {

			configMode = GetDeviceInfoDefaultConfigMode();
		}
		
		WifiConfigProcess(configMode);
#endif						
	}
	
}
	
	

static void EJ_Packet_Process(void* data)
{     
	//step 1:  Process  uart2wifiPacket
	uart2WifiPacket *pDevice2WifiPacket = NULL;
	nolock_list_pop(GetDevice2wifiList(), (void **)&pDevice2WifiPacket);
	if (pDevice2WifiPacket) {
		UartPacketProcessCB cb = getCallbackByUartCommandID(pDevice2WifiPacket->dataType);
		if (cb != NULL) {
			cb(pDevice2WifiPacket);
		}
		EJ_PacketUartFree(pDevice2WifiPacket);
	}

	//step 2: Process wifi2cloudPacket
	wifi2CloudPacket *pCloud2WifiPacket = NULL;
	nolock_list_pop(GetCloud2wifiList(), (void **)&pCloud2WifiPacket);

	if (pCloud2WifiPacket) {
		uint32_t commandID = (uint32_t)(pCloud2WifiPacket->dataType[1] << 8 | pCloud2WifiPacket->dataType[0]);
		MQTTPacketProcessCB cb = getCallbackByMQTTCommandID(commandID);
		if (cb != NULL) {			
			cb(pCloud2WifiPacket);
			/* if commandID is response of replyID and the reply is success. */
			if (commandID == 0x8015) {
				/* report wifimodule info to cloud. */
				wifi2CloudPacket *pReportWifiModuleInfo = (wifi2CloudPacket *)reportWifiModuleInfoToCloud();
				if (pReportWifiModuleInfo&&(nolock_list_push(GetWifi2cloudList(), pReportWifiModuleInfo) != 0x01))
				{
					EJ_ErrPrintf(("[MQTTThread.c][Init_MQTTThread][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
				}
				
				/* report wifimodule status to cloud. */
				wifi2CloudPacket *pReportWifiModuleStatus = (wifi2CloudPacket *)reportWifiModuleNetworkStateToCloud();
				if (pReportWifiModuleStatus&&(nolock_list_push(GetWifi2cloudList(), pReportWifiModuleStatus) != 0x01))
				{
					EJ_ErrPrintf(("[MQTTThread.c][Init_MQTTThread][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
				}

				/* report device info to cloud. */
				wifi2CloudPacket *pReportDeviceInfo = (wifi2CloudPacket *)reportDeviceInfoToCloud();
				if (pReportDeviceInfo&&(nolock_list_push(GetWifi2cloudList(), pReportDeviceInfo) != 0x01))
				{
					EJ_ErrPrintf(("[MQTTThread.c][Init_MQTTThread][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
				}

				/* report device status to cloud. */
				uart2WifiPacket * pUart2WifiPacket = (uart2WifiPacket *)queryDeviceStatus();
				if (pUart2WifiPacket&&(nolock_list_push(GetWifi2deviceList(), pUart2WifiPacket) != 0x01))
				{
					EJ_ErrPrintf(("[mainLoop.c][mainLoop][ERROR]: add packet to wifi2devicelist failed.\r\n"));
				}
			}
			
		}else {
			EJ_ErrPrintf(("[mainLoop.c][mainLoop][ERROR]: NULL MQTTCommand %x Callback.\r\n", commandID));
		}
		EJ_PacketCloudFree(pCloud2WifiPacket);
		
	}
	//step 3:Process wifi2appPacket
	wifi2AppPacket *wifi2AppPacket = NULL;
	nolock_list_pop(GetUdp2wifiList(), (void **)&wifi2AppPacket);
	if(wifi2AppPacket)
	{
		Process_Wifi2AppPacket(wifi2AppPacket);
		EJ_mem_free(wifi2AppPacket->data);
		EJ_mem_free(wifi2AppPacket);	
	}													

}



static void  EJ_event_handler(ej_event_t event,void *data)
{	
	EJ_DebugPrintf(("[EJ_PutEventSem][EJ_queue_recv] is %d\r\n",event));
	switch(event)
			{
				case  EJ_EVENT_shouldUARTThreadWorkSem:
					{	
						EJ_event_shouldUARTThreadWorkProcess(NULL);
						break;
					}
				case  EJ_EVENT_shouldMQTTThreadWorkSem:
					{	
						EJ_event_shouldMQTTThreadWorkProcess(NULL);
						break;
					}
				case  EJ_EVENT_ezconnectCompletedSem:
					{	
						EJ_event_ezconnectCompletedProcess(NULL);
						break;
					}
				case  EJ_EVENT_routerConnectedSem:
					{	
						EJ_event_routerConnectedProcess(NULL);
						break;
					}
				case  EJ_EVENT_broadcastRequestSem:
					{	
						EJ_event_broadcastRequestProcess(NULL);
						break;
					}
				case  EJ_EVENT_broadcastResponseSem:
					{	
						EJ_event_broadcastResponseProcess(NULL);
						break;
					}
				case  EJ_EVENT_uapStartedSem:
					{	
						EJ_event_uapStartedProcess(NULL);
						break;
					}
				case  EJ_EVENT_easylinkRequestSem:
					{	
						EJ_event_easylinkRequestProcess(NULL);
						break;
					}
				case  EJ_EVENT_softAPRequestSem:
					{	
						EJ_event_softAPRequestProcess(NULL);
						break;
					}
				case  EJ_EVENT_airkissRequestSem:
					{	
						EJ_event_airkissRequestProcess(NULL);
						break;
					}
				case  EJ_EVENT_MQTTConnectionLostSem:
					{	
						EJ_event_MQTTConnectionLostProcess(NULL);
						break;
					}
				case  EJ_EVENT_rebootRequestSem:
					{	
						EJ_event_rebootRequestProcess(NULL);
						break;
					}
				case  EJ_EVENT_Reset2FactoryRequestSem:
					{	
						EJ_event_Reset2FactoryRequestProcess(NULL);
						break;
					}
				case  EJ_EVENT_routerAuthFailedSem:
					{	
						EJ_event_routerAuthFailedProcess(NULL);
						break;
					}
				case  EJ_EVENT_reApplyIDSem:
					{	
						EJ_event_reApplyIDProcess(NULL);
						break;
					}
				case  EJ_EVENT_timeSyncSem:
					{	
						EJ_event_timeSyncProcess(NULL);
						break;
					}
				case  EJ_EVENT_homeApNotConfiguredSem:
					{	
						EJ_event_homeApNotConfiguredProcess(NULL);
						break;
					}
				default:
					//EJ_default_process();
					break;
	
			}
	


		
}


void  EJ_Event_Process(void *data)
{
	uint8_t msg;
	if(EJ_queue_recv(ejEventQueue, &msg, EJ_msec_to_ticks(10))==EJ_SUCCESS)
	{
		EJ_event_handler(msg,NULL);		
	}
	EJ_Packet_Process(NULL);	
}

void mainLoop(void* arg)
{

	static WifiModuleRunningStatus wifimoduleRunningStatus = WIFIMODULE_QUERY_QOS;
	static uint8_t readCount = 0;
	for (;;){
		switch(wifimoduleRunningStatus)
		{
			case WIFIMODULE_QUERY_QOS:
			{				
				uart2WifiPacket *pDevice2WifiPacket = NULL;
				nolock_list_pop(GetDevice2wifiList(), (void **)&pDevice2WifiPacket);
				if (pDevice2WifiPacket) {
					EJ_DebugPrintf(("[mainLoop][INFO]: receive an uart2wifi packet %x.\r\n", pDevice2WifiPacket->dataType));	
					UartPacketProcessCB cb = getCallbackByUartCommandID(pDevice2WifiPacket->dataType);
					if (cb != NULL) {
						cb(pDevice2WifiPacket);
					}
					EJ_PacketUartFree(pDevice2WifiPacket);
					if (pDevice2WifiPacket->dataType == 0x05) {
						wifimoduleRunningStatus = WIFIMODULE_VERIFY_DEVICEUUID;
						readCount = 0;
					}					
				}else {
					uart2WifiPacket * pUart2WifiPacket = (uart2WifiPacket *)readDeviceQos();
					if (!pUart2WifiPacket) {
						return;
					}
					if (nolock_list_push(GetWifi2deviceList(), pUart2WifiPacket) != 0x01)
					{
						EJ_ErrPrintf(("[mainLoop][ERROR]: add packet to wifi2devicelist failed.\r\n"));
					}
					readCount++;
					if (readCount >= 10) {
						/* set default qos level.*/
						SetDeviceInfoQos(0);
						EJ_UartThreadStart();
						EJ_thread_sleep(EJ_msec_to_ticks(1000));
						readCount = 0;
						wifimoduleRunningStatus = WIFIMODULE_VERIFY_DEVICEUUID;
					}else if(readCount >= 60){
						EJ_thread_sleep(EJ_msec_to_ticks(1000));
						EJ_App_reboot(REASON_USER_REBOOT);
					}
				}			
				EJ_thread_sleep(EJ_msec_to_ticks(500));				
				break;
			}
			case WIFIMODULE_VERIFY_DEVICEUUID:
			{
				uart2WifiPacket *pDevice2WifiPacket = NULL;
				nolock_list_pop(GetDevice2wifiList(), (void **)&pDevice2WifiPacket);
				if (pDevice2WifiPacket) {
					EJ_DebugPrintf(("[mainLoop][INFO]: receive an uart2wifi packet %x at WIFIMODULE_VERIFY_DEVICE_UUID.\r\n", pDevice2WifiPacket->dataType));		
					UartPacketProcessCB cb = getCallbackByUartCommandID(pDevice2WifiPacket->dataType);
					if (cb != NULL) {
						if ((cb(pDevice2WifiPacket) == 0x01) && (pDevice2WifiPacket->dataType == 0x07)) {
							//AJ_AlwaysPrintf(("[mainLoop.c][mainLoop][INFO]: prepare to enter wificonfig mode.\r\n"));
							EJ_App_network_configured(0);
							EJ_App_reboot(REASON_USER_REBOOT);
						}else if (pDevice2WifiPacket->dataType == 0x07){							
							uart2WifiPacket *pUart2WifiPacket =  (uart2WifiPacket *)getDeviceInfo();
							if (!pUart2WifiPacket) {
								return;
							}
							if (nolock_list_push(GetWifi2deviceList(), pUart2WifiPacket) != 0x01)
							{
								EJ_ErrPrintf(("[mainLoop][ERROR]: add packet to wifi2devicelist failed.\r\n"));
							}
							wifimoduleRunningStatus = WIFIMODULE_NORAML_RUNNING;							
						}
					}

					EJ_PacketUartFree(pDevice2WifiPacket);
				}else {
					uart2WifiPacket * pUart2WifiPacket = (uart2WifiPacket *)readDeviceUUID();
					if (!pUart2WifiPacket) {
						return;
					}
					if (nolock_list_push(GetWifi2deviceList(), pUart2WifiPacket) != 0x01)
					{
						EJ_ErrPrintf(("[mainLoop][ERROR]: add packet to wifi2devicelist failed.\r\n"));
					}
					readCount++;
					if (readCount >= 3) {
						EJ_thread_sleep(EJ_msec_to_ticks(1000));
						EJ_UartThreadStart();
						readCount = 0;
					}else if(readCount >= 5)
					{
						EJ_thread_sleep(EJ_msec_to_ticks(1000));
					#if 1
						DeviceUuid id = GetDeviceInfoDeviceUuid();
						uint8_t *data = (uint8_t *)&id;
						if(data = NULL)
						{
							EJ_App_network_configured(0);
							EJ_App_psm_erase();
							EJ_App_reboot(REASON_USER_REBOOT);
						}else
						{
							if(GetWifiModuleStatusIsHomeAPConfig() == HOMEAP_NOT_CONFIGURED)
							{
								EJ_App_network_configured(0);
								EJ_App_reboot(REASON_USER_REBOOT);
							}
						}
						readCount = 0;
						wifimoduleRunningStatus = WIFIMODULE_NORAML_RUNNING;
						EJ_DebugPrintf(("[INFO2]: receive an uart2wifi packet  at WIFIMODULE_NORAML_RUNNING.\r\n"));		
					}
					#endif
				}
				EJ_thread_sleep(EJ_msec_to_ticks(500));				
				break;
			}

			case WIFIMODULE_NORAML_RUNNING:
			{
				//EJ_Printf("process mainloop!!!!!!!\r\n");
				EJ_Event_Process(NULL);
			}
			default:
				break;

		}
		EJ_thread_sleep(EJ_msec_to_ticks(10));
	}

}


