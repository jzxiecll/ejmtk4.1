#ifndef _H_EJ_UTILS_H_
#define _H_EJ_UTILS_H_

#include "ej_porting_layer.h"

#define     EJ_USER_UUID_STRTING_LEN  6
#define     EJ_USER_MAC_STRTING_LEN  12

void xor(uint8_t *buf, uint8_t size);
int strsplinum(char *str, const char*del) ;
void split( char **arr, char *str, const char *del);
int getweekofyear();
int getyearofyear();



#endif
