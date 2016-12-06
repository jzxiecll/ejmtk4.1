#ifndef __EJ_CMD_MAP_H__
#define __EJ_CMD_MAP_H__


#include <FreeRTOS.h>
#include <task.h>

#include "ej_packet.h"
#include "nolock-list.h"



typedef enum {

	COMMANDTYPE_SOURCE_MQTT = 0,
	COMMANDTYPE_SOURCE_LAN,
};

typedef struct {

	uint8_t IsUsed;

	uint8_t CommandType;

	uint8_t UARTCommandDataID;

	int EtherCommandDataID;

	int LANCommandSocketfd;
	
}UARTCommandDataID2NetCommandDataIDMap;

void InitUARTCommandDataID2NetCommandDataIDMaps();

int GetEtherCommandDataIDByUARTCommandDataID(uint8_t UARTCommandDataID, uint8_t commandType);

int SetEtherCommandDataID2UARTCommandIDMap(uint8_t UARTCommandDataID, int EtherCommandsDataID, uint8_t CommandType);

int setLANCommandDataID2LANCommandSocketfd(int LanCommandDataID, int LanCommandSocketfd);

int getLanCommandSocketfdByLANCommandDataID(int LANCommandDataID);

int setLANCommandDataID2UARTCommandDataID(int LanCommandDataID, int UARTCommandDataID);

int getLANCommandDataIDByUARTCommandDataID(int UARTCommandDataID);

#endif //_H_WIFIMODULE_MAP_H_

