#ifndef _H_WIFIMODULE_MQTT_CONFIGURATION_H_
#define _H_WIFIMODULE_MQTT_CONFIGURATION_H_

typedef struct 
{
	char* clientid;
	int nodelimiter;
	char* delimiter;
	int qos;
	char* username;
	char* password;
	char* host;
	int port;
	int showtopics;
	char* topic;
	int retained;
	char* pubTopic;
	char* subTopic;
#if defined(WIN32) || defined(WIN64)
	MQTTClient client;
#else
	Client client;
	Network network;
#endif
}opts_struct/* opts =
{
	"0010010010010000031510", 0, "\n", 1, "u000003", "p000003", "120.25.66.18", 1883, 1, "#", 1,  "$USR/manager", "001/001/001"
}*/;

#endif //_H_WIFIMODULE_MQTT_CONFIGURATION_H_
