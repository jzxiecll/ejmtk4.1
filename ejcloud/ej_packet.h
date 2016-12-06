#ifndef __EJ_PACKET_H__
#define __EJ_PACKET_H__

//#include <wmtime.h>
#include "ej_porting_layer.h"


#define DUP_BITS (1 << 3)
#define QOS_BITS (1 << 2)
#define ACK_BITS (1 << 1)
#define RESPONSION_BITS (1 << 0)

#define DATA_WITH_NO_CRYPT	0
#define DATA_WITH_XOR_CRYPT (0x01 << 4)
#define DATA_WITH_AES_CRYPT (0x02 << 4)

#define UART2WIFIPACKET_FIXED_LENGTH 9

typedef struct {

  uint8_t head;

	uint8_t version;

	uint8_t crypt;

	uint8_t dataLen[2];

	uint8_t dataType;

	uint8_t dataTypeCrc[2];

	uint8_t dataID;

	uint8_t *data;

	uint8_t crc;

}uart2WifiPacket;




typedef struct {

	uint8_t head[2];

	uint8_t version;

	uint8_t crypt;

	uint8_t dataLen[4];

	uint8_t dataType[2];

	uint8_t dataID[4];

	uint8_t timeStamp[8];

	uint8_t deviceID[6];

	uint8_t *data;

	uint8_t signature[16];

}wifi2CloudPacket;


typedef struct {

	uint8_t cmd[10];

	uint8_t version[10];

	uint8_t crypt[10];

	uint8_t PacketType[20];
	
	uint8_t deviceID[10];

	uint8_t PacketID[10];

	uint8_t timeStamp[20];
	
	uint8_t dataLen[10];

	uint8_t *data;

}wifi2AppPacket;


#define MAX_UART_COMMANDS_COUNT 20
#define MAX_MQTT_COMMANDS_COUNT 20

typedef uint8_t(*MQTTPacketProcessCB)(wifi2CloudPacket *);
typedef uint8_t(*UartPacketProcessCB)(uart2WifiPacket *);

typedef struct {

  uint8_t commandID;

  uint8_t classID;

  uint8_t length;

  UartPacketProcessCB func_cb;

}UartCommandProcessCB;

typedef struct {

	uint32_t commandID;

	uint8_t classID;

	uint8_t length;

	MQTTPacketProcessCB func_cb;

}MQTTCommandProcessCB;


void initCommandsCallbackManager();
UartPacketProcessCB getCallbackByUartCommandID(uint8_t commandID);
MQTTPacketProcessCB getCallbackByMQTTCommandID(uint32_t commandID);
uint8_t registerUartCommandCallback(uint8_t commandID, UartPacketProcessCB func);
uint8_t registerMQTTCommandCallback(uint32_t  commandID, MQTTPacketProcessCB func);
uart2WifiPacket * MakeUart2WifiResponsePacket(uart2WifiPacket *pPacket);
bool AckPacketCompare(uart2WifiPacket *pPacket, uart2WifiPacket *pAckPacket);
void fillTimeStampToPackt(wifi2CloudPacket *pPacket);
void fillDataIDToPacket(wifi2CloudPacket *pPacket, int dataID);
uart2WifiPacket * MakeUart2WifiResponsePacket(uart2WifiPacket *pPacket);
void readDeviceAuthInfo();
void Process_readDeviceAuthInfoResponseCB();
uart2WifiPacket * readDeviceSN();
void Process_DataFromDeviceToCloud(uart2WifiPacket *pPacket);
void Process_GetWifiModuleNetworkStateRequestCB();
void Process_DeviceTimeSyncRequestCB();
void Process_DeviceWifiPowerRequestCB();
void Process_DeviceModifyWifiSSIDRequestCB();
void Process_DeviceConfigWifiModeRequestCB();
void Process_DeviceRebootWifiModuleRequestCB();
void Process_DeviceResetWifiToFactoryRequestCB();
void sendConfirmInfoToCloud();
void Process_DataFromCloudToDevice();
void reportWifiModuleStateToCloud();
void Process_CloudGetModuleInfoRequestCB();
void requestTimeSyncFromCloud();
void Process_TimeSyncFromCloudResponseCB();
void Process_wifiModuleUpgradeInfoFromCloudResponseCB();
void Process_CloudGetWifiModuleInfoRequestCB();
uint8_t InitPacketQUEUE();
ej_queue_t *GetCloud2deviceQueue();
ej_queue_t *GetDevice2cloudQueue();


#endif //_H_WIFIMODULE_PACKET_H_




