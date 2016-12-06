#ifndef __EJ_PORT_UART_H__
#define __EJ_PORT_UART_H__

#include "../ej_porting_layer.h"


/** Flag to enable UART in 8bit mode */
#define EJ_UART_8BIT 0
/** Flag to enable UART in 9bit mode */
#define EJ_UART_9BIT 1

typedef enum
{
  EJ_UART0_ID = 0,	                                 /*!< UART0 port define */
  EJ_UART1_ID = 1,                                      /*!< UART1 port define */
  EJ_UART2_ID,                                      /*!< UART2 port define */
  EJ_UART3_ID,                                      /*!< UART3 port define */
}EJ_UART_ID_Type;

int EJ_uart_open(EJ_UART_ID_Type port_id, uint32_t baud);
int EJ_uart_close(EJ_UART_ID_Type uartX);
bool EJ_uart_write( uint8_t *buf, uint32_t len);
uint32_t EJ_uart_read( uint8_t *buf, uint32_t len);



#endif
