#ifndef __EJ_PORT_UART_H__
#define __EJ_PORT_UART_H__
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/stats.h>
#include <lwip/icmp.h>
#include <lwip/ip.h>
#include <lwip/inet_chksum.h>
#include "ej_porting_layer.h"


#define REASON_USER_REBOOT                  (5)
#define WIFI_MAC_ADDRESS_LENGTH             (6)

typedef struct {

	uint8_t ssidLength;

	uint8_t ssid[32];

	uint8_t passwordLength;

	uint8_t password[32];
	
}EJ_WifiModuleConfig;


typedef struct {
	uint8_t mac[13];
	uint8_t uuid[6];
}EJ_DevInfo_t;


void EJ_init_dev();
int    EJ_App_reboot(uint8_t reason);
int   EJ_App_network_configured(unsigned char state);
int   EJ_App_network_configured_status(unsigned char *state);

int  EJ_App_psm_erase();
void  EJ_App_reset();
int   EJ_Wlan_get_connection_state(unsigned char *state);
int   EJ_Wlan_set_connection_state(int state);
int  EJ_Wlan_get_mac_address(uint8_t  *mac);
int  EJ_Wlan_get_mac_address_posix(uint8_t  *buf);
int    EJ_Wlan_set_mac_address(uint8_t  *mac);
int EJ_Wlan_get_ip_address(uint8_t* sIpaddr);
int  EJ_Wlan_set_ip_address(uint8_t* sIpaddr);
int  EJ_Wlan_set_network(EJ_WifiModuleConfig *config);
int  EJ_Wlan_get_network();
int EJ_Wlan_sta_start();
int EJ_Wlan_sta_stop();
int EJ_Wlan_uap_start(char *ssid,char *passphrase);
int EJ_Wlan_uap_stop();
bool  EJ_Wlan_is_uap_started();
bool EJ_Wlan_is_sta_connected();
int EJ_Wlan_set_time(uint8_t *pTimeArray);
int EJ_Wlan_get_time(uint8_t *pTimeArray);
int  EJ_Device_get_uuid(uint8_t *pUuid);



#endif

