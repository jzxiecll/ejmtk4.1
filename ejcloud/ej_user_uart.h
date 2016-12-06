
#ifndef __H_EJ_USER_UART_H__
#define __H_EJ_USER_UART_H__



#include "ej_porting_layer.h"
#include "uart/ej_port_uart.h"
#include "nolock-list.h"
#include "ej_packet.h"

typedef enum {

	EJ_UART_MSG_HEAD = 1,
	EJ_UART_MSG_LENGTH,
	EJ_UART_MSG_CONTENT,

}EJ_UART_MSG_RECEIVE_STATUS;

typedef enum {

	EJ_UARTTHREAD_RXTX_SEND,
	EJ_UARTTHREAD_RXTX_WAIT_FOR_ACK,

}EJ_UARTTHREAD_RXTX_STATE_MACHINE;
void EJ_PublishUart2WifiPacket(uart2WifiPacket *pPacket);

#endif

