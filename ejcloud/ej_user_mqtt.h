#ifndef __EJ_USER_MQTT_H__
#define __EJ_USER_MQTT_H__

#include "ej_packet.h"
#include "ej_porting_layer.h"
typedef enum {
	MQTT_CONNECTED_SUCCESS,
	MQTT_CREATE_SOCKET_ERROR,
	MQTT_CONNECTED_ERROR,
	MQTT_CONNECT_ERROR_FINISHED,
};

typedef enum {

	INIT_MQTT_SUCCESS,
	INIT_MQTT_CONNECTION_ERROR,
	INIT_MQTT_OS_THREAD_CREATE_ERROR,
};

typedef enum {

  MECHANISM_3S = 0,

  MECHANISM_10S,

  MECHANISM_25S,
  
}CONNECT_MECHANISM;


int EJ_user_init_MQTTThread();

uint8_t EJ_user_uninit_MQTTThread();

//void MQTTSendThread(void* n);

//void MQTTReceiveThread(void *arg);

//uint8_t EJ_user_connectServer();

void EJ_user_pub_Wifi2CloudPacket(wifi2CloudPacket *pPacket, char *topicName);

#endif //_H_WIFIMODULE_MQTTTHREAD_H_





