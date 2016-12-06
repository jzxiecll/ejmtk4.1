#ifndef __EJ_USER_UDPBROADCAST_H__
#define __EJ_USER_UDPBROADCAST_H__

#include "ej_porting_layer.h"


typedef enum {

	INIT_UDPBROADCAST_SUCCESS = 0x00,
	INIT_UDPBROADCAST_SOCKET_CREATE_ERROR,
	INIT_UDPBROADCAST_SOCKET_SET_BROADCAST_ERROR,
	INIT_UDPBROADCAST_SOCKET_BIND_ERROR,
	INIT_UDPBROADCAST_OS_THREAD_CREATE_ERROR,
};


typedef struct {

	uint8_t deviceUuid[10];

	uint8_t did[20];

	uint8_t mac[20];

	uint8_t ipaddr[20];
	
}PakcetWifiModuleBroadcastResponse;


int EJ_init_udpbroadcast();
int EJ_uninit_udpbroadcast();
int GetUdpPacketType(char *PacketType);



#endif //_H_WIFIMODULE_UDPBROADCAST_H_

