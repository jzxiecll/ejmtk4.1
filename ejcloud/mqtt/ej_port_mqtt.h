#ifndef __EJ_PORT_MQTT_H__
#define __EJ_PORT_MQTT_H__

#include "ej_port_wlan.h"
#include "ej_porting_layer.h"

typedef struct Timer
{

//	portTickType xTicksToWait;
//	xTimeOutType xTimeOut;
	unsigned int systick_period;
    unsigned int end_time;

} Timer;

char expired(Timer* timer);
void countdown_ms(Timer* timer, unsigned int timeout);
void countdown(Timer* timer, unsigned int timeout);
int  left_ms(Timer* timer);
void InitTimer(Timer* timer);

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};


int NetworkConnect(Network *n, char *addr,  int port);

void NewNetwork(Network* n);

#if 0
#define MQTT_DBG(x, ...) printf("[MQTT:DBG]"x"\r\n", ##__VA_ARGS__)
#define MQTT_WARN(x, ...) printf("[MQTT:WARN]"x"\r\n", ##__VA_ARGS__)
#define MQTT_ERR(x, ...) printf("[MQTT:ERR]"x"\r\n", ##__VA_ARGS__)
#else
#define MQTT_DBG(x, ...)
#define MQTT_WARN(x, ...)
#define MQTT_ERR(x, ...)
#endif



#endif //_H_WMSDK_PORT_H_

