#include "mqtt/MQTTClient.h"
#include "nolock-list.h"
#include "ej_user_mqtt.h"
#include "ej_cmd_map.h"
#include "ej_port_wlan.h"
#include "ej_event_manager.h"
#include "ej_utils.h"

static ej_thread_t MQTTSendThread_thread = 0;
static ej_thread_stack_define(MQTTSendThread_stack, 2048);
static ej_thread_t MQTTReceiveThread_thread = 0;
static ej_thread_stack_define(MQTTReceiveThread_stack, 2048);


uint8_t ethersignStr[] = "qHUnA0xB%`[a#},HZLkK.jQ|;6qxzb5NoJdZ";
unsigned char sendbuf[256], readbuf[256];
unsigned char cipher[256];
unsigned char text[256];
opts_struct *opts = NULL;
extern EJ_DevInfo_t _g_dev;

 void MQTTSendThread(void* n)
{
	int rc = 0;
	int dataLen = 0;
	EJ_DebugPrintf(("[MQTT_Thread][INFO]: MQTT_Thread start.\r\n"));

	for (;;)
	{
		/* process wifi to cloud.*/		
		/*read wifi2cloud list and send */
		wifi2CloudPacket *pPacket = NULL;
		nolock_list_pop(GetWifi2cloudList(), (void **)&pPacket);
		if (pPacket) {

			EJ_user_pub_Wifi2CloudPacket(pPacket, opts->pubTopic);
			EJ_PrintWifi2CloudPacket(pPacket, "[Wifi2Cloud  ]");
			EJ_PacketCloudFree(pPacket);
		}
		/* process device to cloud.*/
		/*read device2cloud list and send */
		int packetCounts = EJ_queue_get_msgs_waiting(GetDevice2cloudQueue());
		int i = 0;
		for(i = 0; i < packetCounts; i++) {
			uart2WifiPacket *pDevice2CloudPacket = NULL;
			if (EJ_queue_recv(GetDevice2cloudQueue(), &pDevice2CloudPacket, EJ_msec_to_ticks(100)) == EJ_SUCCESS) {			
				/* decode uart2WifiPacket and encode wifi2CloudPacket ...*/
				wifi2CloudPacket *pWifi2CloudPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));
				if (pWifi2CloudPacket) {
					if (convertUart2WifiPacketToWifi2CloudPacket(pDevice2CloudPacket, pWifi2CloudPacket) == 0x00) {					
						int dataID = GetEtherCommandDataIDByUARTCommandDataID(pDevice2CloudPacket->dataID, COMMANDTYPE_SOURCE_MQTT);
						if (dataID) {		
							fillDataIDToPacket(pWifi2CloudPacket, dataID);
						}else {
							fillDataIDToPacket(pWifi2CloudPacket, 0x55);
						}					
						EJ_user_pub_Wifi2CloudPacket(pWifi2CloudPacket, opts->pubTopic);	
						EJ_PrintWifi2CloudPacket(pWifi2CloudPacket, "[Device2Cloud]");
										
					}else {
						EJ_ErrPrintf(("[MQTT_Thread][ERROR]: convertUart2WifiPacketToWifi2CloudPacket failed!\r\n"));
					}
					EJ_PacketCloudFree(pWifi2CloudPacket);
				}else {
					EJ_ErrPrintf(("[MQTT_Thread][ERROR]: EJ_mem_malloc wifi2CloudPacket failed!\r\n"));	
				}				
				EJ_PacketUartFree(pDevice2CloudPacket);
			}
		}
		EJ_thread_sleep(EJ_msec_to_ticks(30));
	}

	
}


 void MQTTReceiveThread(void *arg)
{
	EJ_DebugPrintf(("[MQTTReceiveThread][INFO]: MQTTReceiveThread start.\r\n"));
	int rc = 0;
	for(;;)
	{
		if ((rc = MQTTYield(&opts->client, 200)) == FAILURE) {
			EJ_ErrPrintf(("[MQTTReceiveThread][ERROR]: MQTTYield failed.\r\n"));
		}
		else if (rc == CONNECTION_LOST) {	
			/**/
			EJ_thread_Suspend(MQTTSendThread_thread);
			EJ_thread_Suspend(MQTTReceiveThread_thread);		
			EJ_PutEventSem(EJ_EVENT_MQTTConnectionLostSem);
			EJ_ErrPrintf(("[MQTTReceiveThread][ERROR]: MQTT connection lost.\r\n"));
		}
		EJ_thread_sleep(EJ_msec_to_ticks(10));
	}
}


 void EJ_messageArrived(MessageData* m)

{
	EJ_Printf("EJ_messageArrived \r\n");
	uint8_t *data = (uint8_t *)(m->message->payload);
	wifi2CloudPacket *pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));
	if (pPacket) {		
		pPacket->head[0] = data[0];
		pPacket->head[1] = data[1];
		pPacket->version = data[2];
		pPacket->crypt = data[3];
		pPacket->dataLen[0] = data[4];
		pPacket->dataLen[1] = data[5];
		pPacket->dataLen[2] = data[6];
		pPacket->dataLen[3] = data[7];
		//EJ_Printf("EJ_messageArrived dataLen[0]=%d ,payloadlen =%d \r\n",pPacket->dataLen[0],m->message->payloadlen);
		memset(text,0,256);
		if (EJ_Aes_Decrypt((unsigned char *)(data + 8), m->message->payloadlen - 8 - 16, text) == 0)
		{
			EJ_Printf("error EJ_Aes_Decrypt\r\n");
			EJ_mem_free(pPacket);
			return ;
			
		}
		//EJ_Printf("success EJ_Aes_Decrypt\r\n");
		pPacket->data = (uint8_t *)EJ_mem_malloc(pPacket->dataLen[0] - 36);
		if (pPacket->data) {
			pPacket->dataType[0] = text[0];
			pPacket->dataType[1] = text[1];
			pPacket->dataID[0] = text[2];
			pPacket->dataID[1] = text[3];
			pPacket->dataID[2] = text[4];
			pPacket->dataID[3] = text[5];

			memcpy(pPacket->timeStamp, text + 6, 8);
			memcpy(pPacket->deviceID, text + 14, 6);
			memcpy(pPacket->data, text + 20, pPacket->dataLen[0] - 44);
			memcpy(pPacket->signature, (uint8_t*)m->message->payload + m->message->payloadlen - 16, 16);
			EJ_PrintWifi2CloudPacket(pPacket, "[MQTTmessArri]");
			if ((pPacket->dataType[0] == 0x20) && (pPacket->dataType[1] == 0x00)) {

				/* add this packet to cloud2deviceList. */
				if(nolock_list_push(GetCloud2deviceList(), pPacket)!=1){
					EJ_ErrPrintf(("nolock_list_push to Cloud2deviceList failed!\r\n"));	
					EJ_mem_free(pPacket->data);
					EJ_mem_free(pPacket);
				}
			}
			else {
				/* add this packet to cloud2wifiList. */
				if(nolock_list_push(GetCloud2wifiList(), pPacket)!=1){
					EJ_ErrPrintf(("nolock_list_push to Cloud2wifiList failed!\r\n"));
					EJ_mem_free(pPacket->data);
					EJ_mem_free(pPacket);
				}
			}			
		}else {
			EJ_ErrPrintf(("[messageArrived][ERROR]: EJ_mem_malloc wifi2CloudPacket->data failed.\r\n"));
		}
		
	}else{
		EJ_ErrPrintf(("[messageArrived][ERROR]: EJ_mem_malloc wifi2CloudPacket failed.\r\n"));
	}

}


static uint8_t EJ_ConnectToMQTTServer()
{
	int rc = 0;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	NewNetwork(&opts->network);
	MQTTClient(&opts->client, &opts->network, 3000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));
	if (NetworkConnect(&opts->network, opts->host, opts->port) != EJ_SUCCESS)
	{		
		MQTTClientDeinit(&opts->client);
		return MQTT_CREATE_SOCKET_ERROR;
	}
	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = opts->clientid;
	connectData.username.cstring = NULL;
	connectData.password.cstring = NULL;
	connectData.keepAliveInterval = 15;
	connectData.willFlag = 1;
	connectData.will.topicName.cstring = "ERR/ALL/ALL";
	connectData.will.message.cstring = opts->clientid;
	connectData.will.retained = 1;
	connectData.will.qos = 1;

	if ((rc = MQTTConnect(&opts->client, &connectData)) != 0) {
		EJ_ErrPrintf(("[MQTTThread.c][ConnectToMQTTServer][ERROR]: connnect MQTT server failed.\r\n"));
		if (opts->network.my_socket) {
			close(opts->network.my_socket);
			opts->network.my_socket = -1;
		}
		MQTTClientDeinit(&opts->client);
		return MQTT_CONNECTED_ERROR;
	}
	else
		EJ_AlwaysPrintf(("[MQTTThread.c][ConnectToMQTTServer][INFO]: connnect MQTT server success.\r\n"));
#if 0
	//if ((rc = MQTTSubscribe(&opts->client, opts->subTopic, 2, messageArrived)) != 0)
	if ((rc = MQTTSubscribe(&opts->client, "F0AD4E0335E9", 2, EJ_messageArrived)) != 0)
		EJ_ErrPrintf(("[MQTTThread.c][ConnectToMQTTServer][ERROR]: Unable to subscribe topic.\r\n"));

	//if ((rc = MQTTSubscribe(&opts->client, opts->clientid, 2, messageArrived)) != 0)
	if ((rc = MQTTSubscribe(&opts->client, "XBMBJA", 2, EJ_messageArrived)) != 0)
		EJ_ErrPrintf(("[MQTTThread.c][ConnectToMQTTServer][ERROR]: Unable to subscribe topic.\r\n"));
#else
		static  uint8_t  TopicUuid[EJ_USER_UUID_STRTING_LEN+1]={0};
		static  uint8_t  TopicMac[EJ_USER_MAC_STRTING_LEN+1]={0};
		EJ_Wlan_get_mac_address(TopicMac);
		EJ_Device_get_uuid(TopicUuid);		
		EJ_Printf("%s->uuid:%s,mac:%s\r\n",__FUNCTION__,TopicUuid,TopicMac);
		if ((rc = MQTTSubscribe(&opts->client, (const char*)TopicMac, 2, EJ_messageArrived)) != 0)
			EJ_ErrPrintf(("[MQTTThread.c][ConnectToMQTTServer][ERROR]: Unable to subscribe topic1.\r\n"));
		if ((rc = MQTTSubscribe(&opts->client, (const char*)TopicUuid, 2, EJ_messageArrived)) != 0)
			EJ_ErrPrintf(("[MQTTThread.c][ConnectToMQTTServer][ERROR]: Unable to subscribe topic2.\r\n"));

#endif
	return MQTT_CONNECTED_SUCCESS;
}






static void EJ_PublishMQTTMessage(char *topicName, int qos, int retained, void *payload, int dataLen)
{
	int rc = 0;
	MQTTMessage message;

	message.qos = qos;
	message.retained = retained;
	message.payload = payload;
	message.payloadlen = dataLen;

	//EJ_Printf("qos=%d,dataLen=%d,topicName=%s\r\n",qos,dataLen,topicName);
	if ((rc = MQTTPublish(&opts->client, topicName, &message)) != 0)
	{
		EJ_ErrPrintf(("[MQTTThread.c][PublishMQTTMessage][ERROR]: Unable to publish topic.\r\n"));
	}else{	
		//EJ_InfoPrintf(("[MQTTThread.c][PublishMQTTMessage][SUCCESS]: SUCCESS to publish topic.\r\n"));
	}



}

void EJ_user_pub_Wifi2CloudPacket(wifi2CloudPacket *pPacket, char *topicName)
{
	if (pPacket) {			
		uint8_t *buf = (uint8_t *)EJ_mem_malloc(pPacket->dataLen[0] - 16 -8);
		uint8_t *temp = (uint8_t *)pPacket;
		if (buf) {

			memcpy(buf, temp + 8, 20);
			memcpy(buf + 20, pPacket->data, pPacket->dataLen[0] - 20 - 16 - 8);	
			
			//aes_result_dump(buf, pPacket->dataLen[0] - 16 - 8);
			int cipherSize = EJ_Aes_Encrypt(buf, (unsigned int)pPacket->dataLen[0] - 16 - 8, cipher);
			//aes_result_dump(cipher, cipherSize);
			if (cipherSize) {
				
				uint8_t *payload = (uint8_t *)EJ_mem_malloc(8 + cipherSize + strlen(ethersignStr));

				if (payload) {

					memcpy(payload, pPacket, 8);
					memcpy(payload + 8, cipher, cipherSize);
					memcpy(payload + 8 + cipherSize, ethersignStr, strlen(ethersignStr));

					uint8_t tk[16];

					EJ_hash_md5(payload, 8 + cipherSize + strlen(ethersignStr), tk, 16);

					memcpy(payload + 8 + cipherSize, tk, 16);
					
					EJ_PublishMQTTMessage(topicName, opts->qos, opts->retained, payload, 8 + cipherSize + 16);

					EJ_mem_free(payload);

				}else {
					EJ_ErrPrintf(("[MQTTThread.c][PubWifi2CloudPacket][ERROR]: EJ_mem_malloc failed.\r\n"));
				}

			}

			EJ_mem_free(buf);
			
		}else {
			EJ_ErrPrintf(("[MQTTThread.c][PubWifi2CloudPacket][ERROR]: EJ_mem_malloc failed.\r\n"));
		}
	}
	else {

		
	}
}

static int EJ_user_connectServer()
{
	int ret = INIT_MQTT_CONNECTION_ERROR;
	static uint8_t connectTimes = 0;
	static CONNECT_MECHANISM mechanism = MECHANISM_3S;

	if (EJ_ConnectToMQTTServer() == MQTT_CONNECTED_SUCCESS) {		
		mechanism = MECHANISM_3S;
		connectTimes = 0;
	return MQTT_CONNECTED_SUCCESS;
	}

	/* */

	switch(mechanism) {

	case MECHANISM_3S: {
	connectTimes++;
	if (connectTimes >= 4) {
	  connectTimes = 0;
	  mechanism = MECHANISM_10S;
	  EJ_thread_sleep(EJ_msec_to_ticks(10000));
		}else {
		 EJ_thread_sleep(EJ_msec_to_ticks(3000));
		}  
		break;
	  }

	  case MECHANISM_10S: {
	    connectTimes++;    
	    if (connectTimes >= 2) {
	      connectTimes = 0;
	      mechanism = MECHANISM_25S;
	      EJ_thread_sleep(EJ_msec_to_ticks(25000));     
	    }else {      
	      EJ_thread_sleep(EJ_msec_to_ticks(10000));
	    }
	    break;
	  }

	  case MECHANISM_25S: {
	    connectTimes++;  
	    if (connectTimes >= 2) {
	      connectTimes = 0;
	      mechanism = MECHANISM_3S;
		  		EJ_App_reboot(0);

				ret = MQTT_CONNECT_ERROR_FINISHED;

	    }else {

	      EJ_thread_sleep(EJ_msec_to_ticks(25000));
	      
	    }
	    
	    break;

	  }

	  }

		return ret;
}


opts_struct * GenerateMQTTConnectionerOpts()
{

	opts_struct *opt = (opts_struct *)EJ_mem_malloc(sizeof(opts_struct));
	if(!opt){
		EJ_Printf("GenerateMQTTConnectionerOpts failed!\r\n");
		return NULL;
	}
	memset(opt, 0, sizeof(opts_struct));
	opt->clientid = (char*)EJ_mem_malloc(13);
	if(!opt->clientid){
		EJ_mem_free(opt);
		return NULL;
	}
	memset(opt->clientid,0,13);
	//memcpy(opt->clientid,"F0AD4E0335E9",12);
	//uint8_t clientid[12] = {0};
	//EJ_Wlan_get_mac_address(clientid);	
	EJ_Wlan_get_mac_address(opt->clientid);
	EJ_Printf("opt->clientid ==>%s\r\n",opt->clientid);
	opt->nodelimiter = 0;
	opt->delimiter = "\n";
	opt->qos = 1;	
	opt->username = NULL;
	opt->password = NULL;
#if 0
	bool userDomain = false;

	char *cloud_domain = GetWifiConfigCloudDomainAddr();

	if (cloud_domain) {

		int a, b, c, d;

		int ret=sscanf(cloud_domain,"%d.%d.%d.%d",&a,&b,&c,&d); 
 
		if(ret==4&&(a>=0&&a<=255)&&(b>=0&&b<=255)&&(c>=0&&c<=255)&&(d>=0&&d<=255))  
		{  
			opt->host = cloud_domain;
		}else {

			struct hostent *host;
			host = gethostbyname(cloud_domain);
		
			if (host != NULL) {

				if (opt->host == NULL) {

					opt->host = (char *)EJ_mem_malloc(16);
				}

				if (inet_ntop(AF_INET, (void *)(host->h_addr), opt->host, 16)) {
					
					userDomain = true;

				}else {
					EJ_ErrPrintf(("[GenerateMQTTConnectionerOpts]][ERROR]: inet_ntop failed!\r\n"));
				}

				opt->port = 1883;
			}else {

				EJ_ErrPrintf(("[GenerateMQTTConnectionerOpts]][ERROR]: gethostbyname failed!\r\n"));
			}
		}

	}else {
		EJ_ErrPrintf(("[GenerateMQTTConnectionerOpts]][ERROR]: null cloud domain addr failed!\r\n"));
	}

	if (!userDomain) {

		uint8_t defaultConnectServer = GetWifiConfigDefaultConnectServer();

		if (defaultConnectServer == 0) {
		
			opt->host = GetWifiConfigMasterServerAddr();
			opt->port = GetWifiConfigMasterServerPort();	
		
		}else if (defaultConnectServer == 1){

			opt->host = GetWifiConfigBackupServer1Addr();
			opt->port = GetWifiConfigBackupServer1Port();
		
		}else if (defaultConnectServer == 2){

			opt->host = GetWifiConfigBackupServer2Addr();
			opt->port = GetWifiConfigBackupServer2Port();
		}else {
		
		}

	}

#else

	opt->host = "120.24.170.30";
#endif
	opt->port = 1883;
	opt->showtopics = 1;
	opt->topic = "#";
	opt->retained = 1;
	opt->pubTopic = "$USR/manager";
	opt->subTopic = (char *)EJ_mem_malloc(11 + 1);

	if(!opt->subTopic){
		EJ_mem_free(opt->clientid);
		EJ_mem_free(opt);
		return NULL;
	}

	sprintf(opt->subTopic + 0, "%c", _g_dev.mac[0]);
	sprintf(opt->subTopic + 1, "%c", _g_dev.mac[1]);
	sprintf(opt->subTopic + 2, "%c", _g_dev.mac[2]);
	sprintf(opt->subTopic + 3, "%c", _g_dev.mac[3]);
	sprintf(opt->subTopic + 4, "%c", _g_dev.mac[4]);
	sprintf(opt->subTopic + 5, "%c", _g_dev.mac[5]);
	EJ_Printf("[MQTTConnectionerOpts]:mqtt conntect opts: ip= %s port= %d user=%s pass=%s subTopic=%s.\r\n", opt->host, opt->port, opt->username, opt->password,opt->subTopic);

	return opt;
}






int EJ_user_init_MQTTThread(){
	static int i = 0;
	if (i == 0) {

		opts = GenerateMQTTConnectionerOpts(NULL,NULL);

		if(!opts)
		{
			EJ_Printf("Call GenerateMQTTConnectionerOpts failed \r\n");
			return INIT_MQTT_CONNECTION_ERROR;
		}
		
		i = 1;
	}
	EJ_Printf("Call GenerateMQTTConnectionerOpts success \r\n");

	if (EJ_user_connectServer() != MQTT_CONNECTED_SUCCESS) {
		
		return INIT_MQTT_CONNECTION_ERROR;
	}


	
	if (MQTTSendThread_thread == 0) {

		int ret = EJ_thread_create(&MQTTSendThread_thread,
			"MQTTSendThread",
			(void *)MQTTSendThread, 0,
			&MQTTSendThread_stack, EJ_PRIO_3);

		if (ret) {
			EJ_ErrPrintf(("[MQTTThread.c][Init_MQTTThread][ERROR]: Unable to create MQTTSendThread.\r\n"));
		
			return INIT_MQTT_OS_THREAD_CREATE_ERROR;
		}
	}else {

		EJ_task_Resume(MQTTSendThread_thread);
	}

	if (MQTTReceiveThread_thread == 0) {
		int ret = EJ_thread_create(&MQTTReceiveThread_thread,
			"MQTTReceiveThread",
			(void *)MQTTReceiveThread, 0,
			&MQTTReceiveThread_stack, EJ_PRIO_3);

		if (ret) {
			EJ_ErrPrintf(("[MQTTThread.c][Init_MQTTThread][ERROR]: Unable to create MQTTReceiveThread.\r\n"));

			return INIT_MQTT_OS_THREAD_CREATE_ERROR;
		}
	}else {

		EJ_task_Resume(MQTTReceiveThread_thread);
	}
	EJ_Printf("Call GenerateMQTTConnectionerOpts success \r\n");

	return INIT_MQTT_SUCCESS;


}





uint8_t EJ_user_uninit_MQTTThread()
{

	/**/
	MQTTDisconnect(&opts->client);

	//os_thread_delete(&MQTTSendThread_thread);
	//os_thread_delete(&MQTTReceiveThread_thread);

	//EJ_thread_Suspend(&MQTTSendThread_thread);
	//EJ_thread_Suspend(&MQTTReceiveThread_thread);

	MQTTClientDeinit(&opts->client);
	
	if (opts->network.my_socket) {

		close(opts->network.my_socket);

		opts->network.my_socket = -1;
	}
	return 0;
}

