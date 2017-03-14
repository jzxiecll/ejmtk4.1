#ifndef __EJ_PLATFORM_CONF_H__
#define __EJ_PLATFORM_CONF_H__

#include "ej_utils.h"

#define EJ_TEST_SERVER 1

/*
**************************************************************
*
*SERVER    DEFINE   CODE
**************************************************************
*/
//#if  EJ_TEST_SERVER
//#define MASTER_SERVER_ADDR	"120.24.170.30"
//#else
#define MASTER_SERVER_ADDR	"120.77.76.24"
//#endif


#define SERVER_ADDR_MASTER	"120.24.170.30"
#define SERVER_ADDR_TEST	"120.77.76.24"

#define SERVER_TOPIC_MASTER "$USR/manager"
#define SERVER_TOPIC_TEST   "$USR/manageriotdevice15"

#define MASTER_SERVER_PORT	1883
#define CLOUD_DOMAIN_ADDR   "ej-test.com"


/*
**************************************************************
*
*VERSION    DEFINE  CODE
**************************************************************
*/
#define _H_WIFIMODULE_VERSION_H_

#define FUNCTION_CODE	1

#define HARDWARE_VERSION	1

#define SOFTWARE_VERSION_MSB	1

#define SOFTWARE_VERSION_LSB	18

#define PUBLISH_YEAR	getyearofyear()

#define PUBLISH_WEEK	getweekofyear()



/*
**************************************************************
*
*MODULE    DEFINE  CODE
**************************************************************
*/
#define  STR_WIFI_MODULE_BRAND  "CCD" 
#define  STR_WIFI_MODULE_MODE   "MT7687"

#define  STR_WIFI_MODULE_FOTATOPIC  "100/1000/1003"
#endif

