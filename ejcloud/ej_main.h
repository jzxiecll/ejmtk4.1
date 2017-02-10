#ifndef __EJ_MAIN_H__
#define __EJ_MAIN_H__

#include "ej_log.h"
#include "ej_porting_layer.h"
#include "WifiModuleStatus.h"
#include "nolock-list.h"
#include "ej_packet.h"
#include "ej_event_manager.h"
#include "ej_wificonfig.h"
#include "ej_user_udpbroadcast.h"
#include "ej_user_mqtt.h"
#include "ej_port_wlan.h"
#include "ej_utils.h"
#include "ej_aes.h"
#include "ej_user_lanserver.h"


typedef enum {

	WIFIMODULE_QUERY_QOS = 0,

	WIFIMODULE_VERIFY_DEVICEUUID,

	WIFIMODULE_NORAML_RUNNING,

}WifiModuleRunningStatus;

void initMainLoop(uint8_t isHomeAPConfig);
void mainLoop(void* n);

#endif
