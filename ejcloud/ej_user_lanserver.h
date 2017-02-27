#ifndef _H_EJ_USER_LANSERVER_H_
#define _H_EJ_USER_LANSERVER_H_

typedef enum {

	INIT_LANSERVER_SUCCESS = 0x00,
	INIT_LANSERVER_SOCKET_BIND_ERROR,
	INIT_LANSERVER_SOCKET_LISTEN_ERROR,
	INIT_LANSERVER_OS_THREAD_CREATE_ERROR,
};

int init_LANServer();

int UnInit_LANServer();

bool IsLanModuleConnected();
#endif //_H_WIFIMODULE_LAN_SERVER_H_
