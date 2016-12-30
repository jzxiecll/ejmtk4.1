#include "ej_packet.h"

void EJ_PacketCloudFree(wifi2CloudPacket *pPacket)
{
	if (pPacket) {
		if (pPacket->data) {
			EJ_mem_free(pPacket->data);
		}
		EJ_mem_free(pPacket);
	}
}


void EJ_PacketUartFree(uart2WifiPacket *pPacket)
{
	if (pPacket) {
		if (pPacket->data) {
			EJ_mem_free(pPacket->data);
		}
		EJ_mem_free(pPacket);
	}
}



uart2WifiPacket * MakeUart2WifiResponsePacket(uart2WifiPacket *pPacket)
{
	uart2WifiPacket *responsePacket = NULL;
	if (pPacket) {
		responsePacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));
		memcpy((uint8_t *)responsePacket, (uint8_t *)pPacket, sizeof(uart2WifiPacket));
		responsePacket->dataLen[0] = 9;
		responsePacket->dataLen[1] = 0;
		responsePacket->crypt = ACK_BITS | DATA_WITH_XOR_CRYPT;
		responsePacket->data = NULL;
		unsigned short dataLength = 9;
		unsigned short dataTypeCrc = ((pPacket->dataType << 8 | pPacket->dataType)) ^ dataLength;		
		responsePacket->dataTypeCrc[0] = (uint8_t)(dataTypeCrc & 0xff);
		responsePacket->dataTypeCrc[1] = (uint8_t)((dataTypeCrc & 0xff00) >> 8);
	}	
	return responsePacket;
}

bool AckPacketCompare(uart2WifiPacket *pPacket, uart2WifiPacket *pAckPacket)
{
	bool ret = false;
	if ((pPacket) && (pAckPacket)) {
		if ((pPacket->dataType == pAckPacket->dataType) && (pPacket->dataID == pAckPacket->dataID)) {
			ret = true;
		}
	}
	return ret;
}


uart2WifiPacket *copyUart2WifiPacket(uart2WifiPacket *pPacket)
{
	uart2WifiPacket *pPacketCopy = NULL;
	if (pPacket) {
		pPacketCopy = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));
		if (pPacketCopy) {
			memcpy(pPacketCopy, pPacket, sizeof(uart2WifiPacket));
			if (pPacket->data){
				unsigned short length = (pPacket->dataLen[0] | (pPacket->dataLen[1] << 8));
				pPacketCopy->data = (uint8_t *)EJ_mem_malloc(length - UART2WIFIPACKET_FIXED_LENGTH);
				if (pPacketCopy->data) {
					memcpy(pPacketCopy->data, pPacket->data, length - UART2WIFIPACKET_FIXED_LENGTH);
				}			
			}else {
				EJ_PacketUartFree(pPacketCopy);				
				pPacketCopy = NULL;
			}
		}
	}

	return pPacketCopy;
}




uint8_t convertWifi2CloudPacketToUart2WifiPacket(wifi2CloudPacket *pWifi2CloudPacket, uart2WifiPacket *pUart2WifiPacket)
{
	uint8_t ret = 0x00;
	if ((pWifi2CloudPacket) && (pUart2WifiPacket)) {
		memcpy((uint8_t *)pUart2WifiPacket, pWifi2CloudPacket->data, UART2WIFIPACKET_FIXED_LENGTH);
		unsigned short length = (pUart2WifiPacket->dataLen[0] | (pUart2WifiPacket->dataLen[1] << 8));
		pUart2WifiPacket->data = NULL;
		if ((length - UART2WIFIPACKET_FIXED_LENGTH) > 0) {
			pUart2WifiPacket->data = (uint8_t *)EJ_mem_malloc(length - UART2WIFIPACKET_FIXED_LENGTH);
			if (pUart2WifiPacket->data) {
				memcpy(pUart2WifiPacket->data, pWifi2CloudPacket->data + UART2WIFIPACKET_FIXED_LENGTH, length - UART2WIFIPACKET_FIXED_LENGTH);
			}else {
				ret = 0x02;
			}
		}
	}else {
		ret = 0x01;
	}
	return ret;

}
uint8_t convertUart2WifiPacketToWifi2CloudPacket(uart2WifiPacket *pUart2WifiPacket, wifi2CloudPacket *pWifi2CloudPacket)
{
	uint8_t ret = 0x00;
	if ((pUart2WifiPacket) && (pWifi2CloudPacket)) {
		pWifi2CloudPacket->head[0] = 0x5A;
		pWifi2CloudPacket->head[1] = 0x5A;
		pWifi2CloudPacket->version = 0x04;
		pWifi2CloudPacket->crypt = 0x11;	
		if (pUart2WifiPacket->dataType == 0x20) {
			pWifi2CloudPacket->dataType[0] = 0x20;
			pWifi2CloudPacket->dataType[1] = 0x00;
		}else {
			pWifi2CloudPacket->dataType[0] = 0x21;
			pWifi2CloudPacket->dataType[1] = 0x00;
		}
		
		/**/
		fillTimeStampToPackt(pWifi2CloudPacket);
		/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
		//memset(pWifi2CloudPacket->deviceID, 0, 6);
		GetWifiStatusDeviceID(pWifi2CloudPacket->deviceID);
		unsigned short length = (pUart2WifiPacket->dataLen[0] | (pUart2WifiPacket->dataLen[1] << 8));
		pWifi2CloudPacket->data = (uint8_t *)EJ_mem_malloc(length + 1);
		if (pWifi2CloudPacket->data) {
			memcpy(pWifi2CloudPacket->data, pUart2WifiPacket, UART2WIFIPACKET_FIXED_LENGTH);
			memcpy(pWifi2CloudPacket->data + UART2WIFIPACKET_FIXED_LENGTH, pUart2WifiPacket->data, length - UART2WIFIPACKET_FIXED_LENGTH);		
			pWifi2CloudPacket->data[length] = pUart2WifiPacket->crc;		
			int datalen = 44 + (length + 1);
			pWifi2CloudPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pWifi2CloudPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pWifi2CloudPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pWifi2CloudPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);
		}else {
			ret = 0x02;
		}

	}else {
		ret = 0x01;
	}

	return ret;
}

void fillTimeStampToPackt(wifi2CloudPacket *pPacket)
{
	uint8_t ret = 0;
	uint8_t pTimeArray[7] = {0};
	if (pPacket) {

		    if(!EJ_Wlan_get_time(pTimeArray))
			{
			 
			 pPacket->timeStamp[0] = 0;
			 pPacket->timeStamp[1] = pTimeArray[0] & 0x3F;
			 pPacket->timeStamp[2] = pTimeArray[1];
			 pPacket->timeStamp[3] = pTimeArray[2];
			 pPacket->timeStamp[4] = pTimeArray[4];
			 pPacket->timeStamp[5] = pTimeArray[5];
			 pPacket->timeStamp[6] = 0x0F;
			 pPacket->timeStamp[7] = 0x14;
			 
		    }else{

			 pPacket->timeStamp[0] = 0;
			 pPacket->timeStamp[1] = 0 & 0x3F;
			 pPacket->timeStamp[2] = 0;
			 pPacket->timeStamp[3] = 0;
			 pPacket->timeStamp[4] = 0;
			 pPacket->timeStamp[5] = 0;
			 pPacket->timeStamp[6] = 0x0F;
			 pPacket->timeStamp[7] = 0x14;

			}
	}
	
	return ;
}



void fillDataIDToPacket(wifi2CloudPacket *pPacket, int dataID)
{
	pPacket->dataID[3] = (uint8_t)((dataID & 0xff000000) >> 24);
	pPacket->dataID[2] = (uint8_t)((dataID & 0x00ff0000) >> 16);
	pPacket->dataID[1] = (uint8_t)((dataID & 0x0000ff00) >> 8);
	pPacket->dataID[0] = (uint8_t)((dataID & 0x000000ff) >> 0);
}


uint8_t makeCrc(uint8_t *data, uint8_t length)
{
	uint8_t i = 0;
	int crc32 = 0;
	uint8_t crc = 0;
	for (i = 0; i < length; i++) {
		crc32 += data[i];
	}
	crc = crc32 & 0x000000FF;
	crc = ~crc;
	return crc + 1;
}

typedef uint32_t EJ_DebugUartLevel;
static EJ_DebugUartLevel EJ_PacketDbgLevel = 4;

#define LEVEL_WIFI_UART_ON   (3)
#define LEVEL_WIFI_CLOUD_ON   (4)

void  EJ_SetPacketDbgLevel(int level)
{
	EJ_PacketDbgLevel = level;
}

void EJ_PrintUart2WifiPacket(uart2WifiPacket *pPacket, const char *prefix)
{
	if (EJ_PacketDbgLevel < EJ_DEBUG_INFO+1) {
	    return;
	}
	if (pPacket) {
		EJ_Printf("%s PT:%s DT:%02X DL:%02X", prefix, ((pPacket->crypt & ACK_BITS)? "ack": "ctrl"), pPacket->dataType, pPacket->dataLen[0]);
		EJ_Printf(" Packet:");	
		if (pPacket) {		
			uint8_t i = 0;
			uint8_t *buf = (uint8_t *)pPacket;
			uint8_t *dt  =  (uint8_t *)pPacket->data;		
			for(i = 0; i < 9; i++) {
				EJ_Printf("%02X ", buf[i]);
			}
			EJ_Printf("%,");
			for(i = 0; i < pPacket->dataLen[0]-9; i++) {
				EJ_Printf("%02X ", dt[i]);
			}		
		}else {
			EJ_Printf(" NULL");
		}
		EJ_Printf("\r\n");
	}

}


void EJ_PrintWifi2CloudPacket(wifi2CloudPacket *pPacket, const char *prefix)
{
	if (EJ_PacketDbgLevel < EJ_DEBUG_INFO) {
	    return;
	}
	if (pPacket) {
		EJ_Printf("%s DT:0x%02X%02X DL:%02X", prefix,  pPacket->dataType[1], pPacket->dataType[0],pPacket->dataLen[0]);
		if (pPacket) {
			EJ_Printf((" Packet:"));
			uint8_t i = 0;
			uint8_t *buf = (uint8_t *)pPacket->data;		
			for(i = 0; i < pPacket->dataLen[0] - 44; i++) {
				EJ_Printf("%02X ", buf[i]);
			}
		}
		EJ_Printf("\r\n");
	}

}

/*
*************************************************************************
*Uartcommand  and Cloudcommand CallBack Manager   register by ID 
*
*
*
*************************************************************************
*/


UartCommandProcessCB uartCommands[MAX_UART_COMMANDS_COUNT];

MQTTCommandProcessCB mqttCommands[MAX_MQTT_COMMANDS_COUNT];

void initCommandsCallbackManager()
{
	uint8_t i = 0;

	for (i = 0; i < MAX_UART_COMMANDS_COUNT; i++)
	{
		uartCommands[i].commandID = 0x00;
		uartCommands[i].func_cb = NULL;
	}

	for (i = 0; i < MAX_MQTT_COMMANDS_COUNT; i++)
	{
		mqttCommands[i].commandID = 0x00;
		mqttCommands[i].func_cb = NULL;
	}
}

UartPacketProcessCB getCallbackByUartCommandID(uint8_t commandID)
{
	uint8_t i = 0;

	for (i = 0; i < MAX_UART_COMMANDS_COUNT; i++)
	{
		if (uartCommands[i].commandID == commandID)
		{
			return uartCommands[i].func_cb;
		}
	}

	return NULL;
}

MQTTPacketProcessCB getCallbackByMQTTCommandID(uint32_t commandID)
{
	uint8_t i = 0;

	for (i = 0; i < MAX_UART_COMMANDS_COUNT; i++)
	{
		if (mqttCommands[i].commandID == commandID)
		{
			return mqttCommands[i].func_cb;
		}
	}

	return NULL;
}

uint8_t registerUartCommandCallback(uint8_t commandID, UartPacketProcessCB func)
{
	uint8_t i = 0;
	uint8_t ret = 0;

	for (i = 0; i < MAX_UART_COMMANDS_COUNT; i++)
	{
		if (uartCommands[i].commandID == 0x00)
		{
			break;
		}
	}

	if (i < (MAX_UART_COMMANDS_COUNT - 1))
	{
		uartCommands[i].commandID = commandID;
		uartCommands[i].func_cb = func;

		ret = 0x01;
	}

	return ret;
}
uint8_t registerMQTTCommandCallback(uint32_t commandID, MQTTPacketProcessCB func)
{
	uint8_t i = 0;
	uint8_t ret = 0;

	for (i = 0; i < MAX_UART_COMMANDS_COUNT; i++)
	{
		if (mqttCommands[i].commandID == 0x0000)
		{
			break;
		}
	}

	if (i < (MAX_UART_COMMANDS_COUNT - 1))
	{
		mqttCommands[i].commandID = commandID;
		mqttCommands[i].func_cb = func;

		ret = 0x01;
	}
	return ret;
}


ej_queue_t *cloud2deviceQueue, *device2cloudQueue;

static ej_queue_pool_define(cloud2deviceQueueData, 10 * sizeof(wifi2CloudPacket *));
static ej_queue_pool_define(device2cloudQueueData, 10 * sizeof(uart2WifiPacket *));

uint8_t InitPacketQUEUE()
{
  uint8_t ret = 0;

  cloud2deviceQueue = (ej_queue_t *)EJ_mem_malloc(sizeof(ej_queue_t));

  device2cloudQueue = (ej_queue_t *)EJ_mem_malloc(sizeof(ej_queue_t));

  if (EJ_queue_create(cloud2deviceQueue, "cloud2deviceQueue", sizeof(wifi2CloudPacket *) , &cloud2deviceQueueData) != EJ_SUCCESS) {

    ret = 1;

    EJ_ErrPrintf(("[PacketQueue.c][InitPacketQUEUE][ERROR]: os_queue_create cloud2deviceQueue failed.\r\n"));
  }

  if (EJ_queue_create(device2cloudQueue, "device2cloudQueue", sizeof(uart2WifiPacket *), &device2cloudQueueData) != EJ_SUCCESS) {

    ret = 1;

    EJ_ErrPrintf(("[PacketQueue.c][InitPacketQUEUE][ERROR]: os_queue_create device2cloudQueue failed.\r\n"));
  }

  return ret;
}


ej_queue_t *GetCloud2deviceQueue()
{
  return cloud2deviceQueue;
}

ej_queue_t *GetDevice2cloudQueue()
{
  return device2cloudQueue;
}

