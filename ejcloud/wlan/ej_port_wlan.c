#include "ej_port_wlan.h"
#include "wifi_api.h"
#include "ej_wificonfig.h"
EJ_DevInfo_t _g_dev;

static  int ej_get_mac_address(uint8_t *mac)
{
	return wifi_config_get_mac_address(WIFI_PORT_STA, mac);
}

void EJ_init_dev()
{
	char mac[13]; 
  char buf[6];
  ej_get_mac_address(buf);

	sprintf(mac,"%02X%02X%02X%02X%02X%02X", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

 	memcpy(_g_dev.mac, mac, strlen(mac));
	char uuid[]="XBMBJA";
	memcpy(_g_dev.uuid, uuid, strlen(uuid));

	printf("EJ_init_dev :mac=%s,uuid=%s\r\n",_g_dev.mac,_g_dev.uuid);
}

/*
*  WIFI  MODULE  MANAGENER  FUNCTION   SERILES
*
*/
int    EJ_App_reboot(uint8_t reason)
{

	hal_cache_disable();
	hal_cache_deinit();
	hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
	return EJ_SUCCESS;

}



int   EJ_App_network_configured(unsigned char state)
{

	joycfg_profile_set_configfree(state);
	
	return 0;

}

int   EJ_App_network_configured_status(unsigned char *state)
{

	joycfg_profile_get_configfree(state);
	EJ_Printf("ej_configured = %d\r\n",*state);
	return 0;

}


int  EJ_App_psm_erase()
{	
	//psm_erase(1,"psm_erase");	
	return 0 ;

}


void  EJ_App_reset()
{
	EJ_App_psm_erase();

	
	EJ_App_reboot(0);
}






int   EJ_Wlan_get_connection_state(int *state)
{
	if(state == NULL)
			return -EJ_E_INVAL;
	int cur = 0 ;
	
	switch(cur){
			case  1:

			case  2:

			case  3:

			default: 

				*state  = cur ;

			break;
	}


	return EJ_SUCCESS;

}


int   EJ_Wlan_set_connection_state(int state)
{
	return EJ_SUCCESS;
}


int  EJ_Wlan_get_mac_address(uint8_t  *mac)
{
	if(mac == NULL)
		return -EJ_E_INVAL; 
	{
		
				char *sMacaddr = NULL;
			    char buf[6];
			    ej_get_mac_address(buf);
				
				sprintf(mac,"%02X%02X%02X%02X%02X%02X",
						       buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

 	}
	return EJ_SUCCESS;


}


int  EJ_Wlan_get_mac_address_posix(uint8_t  *buf)
{
	if(buf == NULL)
		return -EJ_E_INVAL; 
	{
		
			    ej_get_mac_address(buf);

 	}
	return EJ_SUCCESS;


}


int    EJ_Wlan_set_mac_address(uint8_t  *mac)
{
	 if(mac == NULL)
		return -EJ_E_INVAL; 
	 {
	 
	 
	 
	 }

	return EJ_SUCCESS;

}


int EJ_Wlan_get_ip_address(uint8_t* sIpaddr)
	
{
	int len = 0 ;

	return nvdm_read_data_item("common", "IpAddr", sIpaddr, &len);

	
}

int  EJ_Wlan_set_ip_address(uint8_t* sIpaddr)
{
	if(sIpaddr == NULL)
		return -EJ_E_INVAL; 
	{
	
	
	
	}

	return EJ_SUCCESS;
}





int  EJ_Wlan_set_network(EJ_WifiModuleConfig *config)
{

	wifi_config_set_ssid(WIFI_PORT_STA,config->ssid,config->ssidLength);

	wifi_config_set_wpa_psk_key(WIFI_PORT_STA,config->password,config->passwordLength);

	return 0 ;

}




int  EJ_Wlan_get_network()
{

	int ret = 0 ;

	return ret ;


}


int EJ_Wlan_sta_start()
{	
	wifi_set_opmode(0x01);
	wifi_config_reload_setting();
	return EJ_SUCCESS; 

}


int EJ_Wlan_sta_stop()
{
	wifi_config_set_opmode(0x01);
	wifi_profile_set_opmode(0x01);
	return EJ_SUCCESS;

}


int EJ_Wlan_uap_start(char *ssid,char *passphrase)
{
	wifi_config_set_opmode(0x02);
	wifi_config_reload_setting();
	return EJ_SUCCESS;

	
}


int EJ_Wlan_uap_stop()
{
	wifi_config_set_opmode(0x01);
	wifi_profile_set_opmode(0x01);
	return EJ_SUCCESS;

	
}




bool  EJ_Wlan_is_uap_started()
{

	uint8_t mode;
	wifi_config_get_opmode(&mode);
	if(mode==0x02)
		return true;
	else
		return false;
}


bool EJ_Wlan_is_sta_connected()
{
	uint8_t mode ;

	wifi_config_get_opmode(&mode);

	if(mode==0x01)
		return true;
	else	
		return false;
}






int  EJ_Device_get_uuid(uint8_t *pUuid)
{
	memcpy(pUuid,_g_dev.uuid,6);
	return 0 ;
}
