#ifndef __EJ_CMD_MQTT_H__
#define __EJ_CMD_MQTT_H__


#include "ej_porting_layer.h"
#include "ej_packet.h"
#include  "ej_utils.h"
#include "WifiModuleStatus.h"
wifi2CloudPacket * responseHeartBeatToClient(int dataID);
wifi2CloudPacket *reportWifiUpgradeInfoToCloud();
wifi2CloudPacket *ReportDeviceUpgradeInfoCloud(uint8_t data);

#endif //__EJ_CMD_MQTT_H__




