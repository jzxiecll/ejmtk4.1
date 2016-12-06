#ifndef __EJ_USER_TIME_H__
#define __EJ_USER_TIME_H__

#include "ej_port_time.h"

/**
 * Init date and time
 */

int EJ_time_init();

/**
 * Set date and time
 */
int EJ_time_set(const struct ej_tm *tm);

/**
 * Get date and time
 */
int EJ_time_get(struct ej_tm *tm);
int EJ_Wlan_set_time(uint8_t *pTimeArray);
int EJ_Wlan_get_time(uint8_t *pTimeArray);


#endif
