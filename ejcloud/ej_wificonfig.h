#ifndef __EJ_WIFICONFIG_H__
#define __EJ_WIFICONFIG_H__

#include "ej_porting_layer.h"

typedef enum {

	WIFICONFIG_START,
	WIFICONFIG_WAIT,
	WIFICONFIG_END,

};

typedef enum {

	WIFICONFIG_EZCONNECT_MODE,
	WIFICONFIG_AP_MODE,
	WIFICONFIG_AIRKISS_MODE,
	WIFICONFIG_NULL_MODE,

};

static void WifiConfigMainLoopThread(void* data);

void InitWifiConfigManager();

void SendAirkissConfigResult();

#endif //_H_

