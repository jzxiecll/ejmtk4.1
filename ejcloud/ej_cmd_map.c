
#include "ej_cmd_map.h"


#define MAX_COMMANDDATAID_MAPS	20

UARTCommandDataID2NetCommandDataIDMap EtherCommandDataIDMap[MAX_COMMANDDATAID_MAPS];

UARTCommandDataID2NetCommandDataIDMap LANCommandDataIDMap[MAX_COMMANDDATAID_MAPS];

Mutex UARTCommandDataID2MQTTCommandDataIDMutex;
Mutex UARTCommandDataID2LANCommandDataIDMutex;


void InitUARTCommandDataID2NetCommandDataIDMaps()
{

	/* init mutex... */
	MutexInit(&UARTCommandDataID2MQTTCommandDataIDMutex);

	MutexInit(&UARTCommandDataID2LANCommandDataIDMutex);
	
	int i = 0;

	for(i = 0; i < MAX_COMMANDDATAID_MAPS; i++) {

		EtherCommandDataIDMap[i].IsUsed = 0;
		EtherCommandDataIDMap[i].CommandType = 0;
		EtherCommandDataIDMap[i].LANCommandSocketfd = 0;
		EtherCommandDataIDMap[i].EtherCommandDataID = 0;
		EtherCommandDataIDMap[i].UARTCommandDataID = 0;

		LANCommandDataIDMap[i].IsUsed = 0;
		LANCommandDataIDMap[i].CommandType = 0;
		LANCommandDataIDMap[i].LANCommandSocketfd = 0;
		LANCommandDataIDMap[i].EtherCommandDataID = 0;
		LANCommandDataIDMap[i].UARTCommandDataID = 0;
	}
}


int GetEtherCommandDataIDByUARTCommandDataID(uint8_t UARTCommandDataID, uint8_t commandType)
{
	int ret = -1;
	
	/**/
	MutexLock(&UARTCommandDataID2MQTTCommandDataIDMutex);

	int i = 0;

	for (i = 0; i < MAX_COMMANDDATAID_MAPS; i++) {

		if (EtherCommandDataIDMap[i].IsUsed) {

			if ((EtherCommandDataIDMap[i].UARTCommandDataID == UARTCommandDataID) && (EtherCommandDataIDMap[i].CommandType == commandType)){

				ret = EtherCommandDataIDMap[i].EtherCommandDataID;

				if (EtherCommandDataIDMap[i].CommandType == COMMANDTYPE_SOURCE_MQTT) {

					EtherCommandDataIDMap[i].IsUsed = 0;	
				}
				
				break;
			}
		}

	}
	
	/**/
	MutexUnlock(&UARTCommandDataID2MQTTCommandDataIDMutex);

	return ret;
}

int SetEtherCommandDataID2UARTCommandIDMap(uint8_t UARTCommandDataID, int EtherCommandDataID, uint8_t CommandType)
{
	int ret = -1;
	
	/**/
	MutexLock(&UARTCommandDataID2MQTTCommandDataIDMutex);

	int i = 0;

	for (i = 0; i < MAX_COMMANDDATAID_MAPS; i++) {

		if (EtherCommandDataIDMap[i].IsUsed == 0) {

			EtherCommandDataIDMap[i].IsUsed = 1;
			EtherCommandDataIDMap[i].EtherCommandDataID = EtherCommandDataID;
			EtherCommandDataIDMap[i].UARTCommandDataID = UARTCommandDataID;
			EtherCommandDataIDMap[i].CommandType = CommandType;

			ret = 1;

			break;
		}

	}
	
	/**/
	MutexUnlock(&UARTCommandDataID2MQTTCommandDataIDMutex);

	return ret;
}




int setLANCommandDataID2LANCommandSocketfd(int LanCommandDataID, int LanCommandSocketfd)
{
	int ret = -1;
	
	/**/
	//MutexLock(&UARTCommandDataID2LANCommandDataIDMutex);

	int i = 0;

	for (i = 0; i < MAX_COMMANDDATAID_MAPS; i++) {

		if (LANCommandDataIDMap[i].IsUsed == 0) {

			LANCommandDataIDMap[i].IsUsed = 1;
			LANCommandDataIDMap[i].EtherCommandDataID = LanCommandDataID;
			LANCommandDataIDMap[i].LANCommandSocketfd = LanCommandSocketfd;

			ret = 1;

			break;
		}

	}
	
	/**/
	//MutexUnlock(&UARTCommandDataID2LANCommandDataIDMutex);

	return ret;
}

int getLanCommandSocketfdByLANCommandDataID(int LANCommandDataID)
{
	int ret = -1;
	
	/**/
	//MutexLock(&UARTCommandDataID2LANCommandDataIDMutex);

	int i = 0;

	for (i = 0; i < MAX_COMMANDDATAID_MAPS; i++) {

		if (LANCommandDataIDMap[i].IsUsed) {

			if (LANCommandDataIDMap[i].EtherCommandDataID == LANCommandDataID) {

				ret = LANCommandDataIDMap[i].LANCommandSocketfd;

				LANCommandDataIDMap[i].IsUsed = 0;
				
				break;
			}
		}

	}
	
	/**/
	//MutexUnlock(&UARTCommandDataID2LANCommandDataIDMutex);

	return ret;
}

int setLANCommandDataID2UARTCommandDataID(int LanCommandDataID, int UARTCommandDataID)
{

	int ret = -1;
	
	/**/
	MutexLock(&UARTCommandDataID2LANCommandDataIDMutex);

	int i = 0;

	for (i = 0; i < MAX_COMMANDDATAID_MAPS; i++) {

		if ((LANCommandDataIDMap[i].IsUsed == 1) && (LANCommandDataIDMap[i].EtherCommandDataID == LanCommandDataID)) {

			LANCommandDataIDMap[i].UARTCommandDataID = UARTCommandDataID;
			
			ret = 1;

			break;
		}

	}
	
	/**/
	MutexUnlock(&UARTCommandDataID2LANCommandDataIDMutex);

	return ret;
}

int getLANCommandDataIDByUARTCommandDataID(int UARTCommandDataID)
{
	int ret = -1;
	
	/**/
	MutexLock(&UARTCommandDataID2LANCommandDataIDMutex);

	int i = 0;

	for (i = 0; i < MAX_COMMANDDATAID_MAPS; i++) {

		if (LANCommandDataIDMap[i].IsUsed) {

			if (LANCommandDataIDMap[i].UARTCommandDataID == UARTCommandDataID) {

				ret = LANCommandDataIDMap[i].EtherCommandDataID;
				
				break;
			}
		}

	}
	
	/**/
	MutexUnlock(&UARTCommandDataID2LANCommandDataIDMutex);

	return ret;
}



