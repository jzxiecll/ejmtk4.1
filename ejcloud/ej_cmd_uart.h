#ifndef __EJ_CMD_UART_H__
#define __EJ_CMD_UART_H__


#include "ej_packet.h"
#include "ej_porting_layer.h"
#include "nolock-list.h"
#include "ej_event_manager.h"

uart2WifiPacket * getDeviceInfo();
uart2WifiPacket * queryDeviceVersion();
uart2WifiPacket * queryDeviceUpdate(int size, char *softwareVersion);

void initUARTCommands();

#endif //_H_WIFIMODULE_UART_COMMANDS_H_

