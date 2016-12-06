#ifndef __EJ_PORT_UART_H__
#define __EJ_PORT_UART_H__


#include "ej_porting_layer.h"

int EJ_write_psm_item(const char *keyname,const uint8_t *value);

int EJ_read_psm_item(const char *keyname, uint8_t *value,unsigned int value_len);

int EJ_read_sysitem(const char *keyname, uint8_t *value,unsigned int value_len);



#endif
