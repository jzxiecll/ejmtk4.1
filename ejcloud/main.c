


#include <wm_os.h>
#include <app_framework.h>
#include <wmtime.h>
#include <partition.h>
#include "appln_cb.h"
#include "appln_dbg.h"
#include <cli.h>
#include <wmstdio.h>
#include <wm_net.h>


#include <psm.h>
#include <psm-utils.h>
#include <ftfs.h>
#include <rfget.h>
#include <diagnostics.h>
#include <mdev_gpio.h>

#include "ej_main.h"
#include "ej_event_manager.h"
#include "WifiModuleStatus.h"
#include "ej_porting_layer.h"
#include "ej_error.h"
#include "ej_log.h"
//#include "ntpc.h"



static bool is_time_sync_started;


/*-----------------------Global declarations----------------------*/
static char g_uap_ssid[IEEEtypes_SSID_SIZE + 1];
appln_config_t appln_cfg = {
	.passphrase = "marvellwm",
	.wps_pb_gpio = -1,
	.reset_prov_pb_gpio = -1
};

int ftfs_api_version = 100;
char *ftfs_part_name = "ftfs";

struct fs *fs;

static ej_timer_t uap_down_timer;

#define UAP_DOWN_TIMEOUT (30 * 1000)

#define NETWORK_MOD_NAME	"network"
#define VAR_UAP_SSID		"uap_ssid"
#define VAR_PROV_KEY            "prov_key"



void appln_critical_error_handler(void *data)
{
	while (1)
		;
	/* do nothing -- stall */
}

/** Provisioning done timer call back function
 * Once the provisioning is done, we wait for provisioning client to send
 * AF_EVT_PROV_CLIENT_DONE which stops uap and dhcp server. But if any case
 * client doesn't send AF_EVT_PROV_CLIENT_DONE event, then we wait for
 * 60seconds(timer) to shutdown UAP.
 */
static void uap_down_timer_cb(os_timer_arg_t arg)
{
	/*
	if (is_uap_started()) {
		hp_mdns_deannounce(net_get_uap_handle());
		app_uap_stop();
	}
	*/
}

/* This function initializes the SSID with the PSM variable network.uap_ssid
 * If the variable is not found, the default value is used.
 * To change the ssid, please set the network.uap_ssid variable
 * from the console.
 */
void appln_init_ssid()
{
	if (psm_get_single(NETWORK_MOD_NAME, VAR_UAP_SSID, g_uap_ssid,
			sizeof(g_uap_ssid)) == EJ_SUCCESS) {
		dbg("Using %s as the uAP SSID", g_uap_ssid);
		appln_cfg.ssid = g_uap_ssid;
		appln_cfg.hostname = g_uap_ssid;
	} else {
			uint8_t my_mac[6];

			memset(g_uap_ssid, 0, sizeof(g_uap_ssid));
			wlan_get_mac_address(my_mac);
			/* Provisioning SSID */
			snprintf(g_uap_ssid, sizeof(g_uap_ssid),
				 "eg_uAP_%02X%02X%02X%02X%02X%02X", my_mac[0], my_mac[1],my_mac[2], my_mac[3],my_mac[4], my_mac[5]);
			dbg("Using %s as the uAP SSID", g_uap_ssid);
			appln_cfg.ssid = g_uap_ssid;
			appln_cfg.hostname = g_uap_ssid;
	}
}
/*
#define KEY_LEN 16
uint8_t prov_key[KEY_LEN + 1]; // One extra length to store \0" 

int wmdemo_get_prov_key(uint8_t *prov_key)
{

	if (psm_get_single(NETWORK_MOD_NAME, VAR_PROV_KEY,
			   (char *)prov_key,
			   KEY_LEN + 1) == EJ_SUCCESS) {
		if (strlen((char *)prov_key) == KEY_LEN) {
			dbg("Using key from psm %s", prov_key);
			prov_ezconn_set_device_key(prov_key, KEY_LEN);
			return KEY_LEN;
		} else {
			dbg("Found incorrect prov_key. Starting provisioning"
			    " without key");
			dbg("You can set 16byte key using below command and "
			    "reboot the board");
			dbg("psm-set network prov_key <16byte key>");
			memset(prov_key, 0 , KEY_LEN);
			return 0;
		}
	} else {
		dbg("No prov_key found. Starting provisioning without key");
		return 0;
	}
}
*/
/* This function must initialize the variables required (network name,
 * passphrase, etc.) It should also register all the event handlers that are of
 * interest to the application.
 */
int appln_config_init()
{
	/* Initialize service name for mdns */
	snprintf(appln_cfg.servname, MAX_SRVNAME_LEN, "wm_demo");
	appln_cfg.wps_pb_gpio = board_button_1();
	/* Initialize reset to provisioning push button settings */
	appln_cfg.reset_prov_pb_gpio = board_button_2();
	/* Initialize power management */
	return 0;
}

/*-----------------------Local declarations----------------------*/
static int provisioned;

/*
 * Event: INIT_DONE
 *
 * The application framework is initialized.
 *
 * The data field has information passed by boot2 bootloader
 * when it loaded the application.
 *
 * ?? What happens if app is loaded via jtag
 */
static void event_init_done(void *data)
{


}

extern InfoManagement h_InfoManagement;

/*
 * Handler invoked on WLAN_INIT_DONE event.
 *
 * When WLAN is started, the application framework looks to
 * see whether a home network information is configured
 * and stored in PSM (persistent storage module).
 *
 * The data field returns whether a home network is provisioned
 * or not, which is used to determine what network interfaces
 * to start (station, micro-ap, or both).
 *
 * If provisioned, the station interface of the device is
 * connected to the configured network.
 *
 * Else, Micro-AP network is configured.
 *
 * (If desired, the Micro-AP network can also be started
 * along with the station interface.)
 *
 * We also start all the services which don't need to be
 * restarted between provisioned and non-provisioned mode
 * or between connected and disconnected state.
 *
 * Accordingly:
 *      -- Start mDNS and advertize services
 *	-- Start HTTP Server
 *	-- Register WSGI handlers for HTTP server
 */
static void event_wlan_init_done(void *data)
{
	int ret;

	wlan_set_country(COUNTRY_CN);

	wlan_enable_11d();

	/* We receive provisioning status in data */
	provisioned = (int)data;

	

	/* Initialize ssid to be used for uAP mode */
	appln_init_ssid();

	if (provisioned) {
		app_sta_start();
		//h_InfoManagement.wifiStatus.isHomeAPConfig = HOMEAP_CONFIGURED;
	} else {
		//h_InfoManagement.wifiStatus.isHomeAPConfig = HOMEAP_NOT_CONFIGURED;
	}



	/*
	 * Initialize CLI Commands for some of the modules:
	 *
	 * -- psm:  allows user to check data in psm partitions
	 * -- ftfs: allows user to see contents of ftfs
	 * -- wlan: allows user to explore basic wlan functions
	 */

	ret = psm_cli_init(sys_psm_get_handle(), NULL);
	if (ret != EJ_SUCCESS)
		dbg("Error: psm_cli_init failed");
	ret = ftfs_cli_init(fs);
	if (ret != EJ_SUCCESS)
		dbg("Error: ftfs_cli_init failed");
	ret = rfget_cli_init();
	if (ret != EJ_SUCCESS)
		dbg("Error: rfget_cli_init failed");
	ret = wlan_cli_init();
	if (ret != EJ_SUCCESS)
		dbg("Error: wlan_cli_init failed");


	initMainLoop(provisioned);
	EJ_Printf("provisioned: [%d] \n\r",provisioned);
	//start_smart_config();

}

/*
 * Event: Micro-AP Started
 *
 * If we are not provisioned, then start provisioning on
 * the Micro-AP network.
 *
 * Also, enable WPS.
 *
 * Since Micro-AP interface is UP, announce mDNS service
 * on the Micro-AP interface.
 */
static void event_uap_started(void *data)
{
#ifndef APPCONFIG_PROV_EZCONNECT
	void *iface_handle = net_get_uap_handle();

	dbg("Event: Micro-AP Started");
#endif
	
	SetWifiModuleStatusWorkMode(WIFIMODULE_SOFTAP_MODE);
	//PutUapStartedSem();
	EJ_PutEventSem(EJ_EVENT_uapStartedSem);
	//Wifi_SocketIo();
}

/*
 * Event: PROV_DONE
 *
 * Provisioning is complete. We can stop the provisioning
 * service.
 *
 * Stop WPS.
 *
 * Enable Reset to Prov Button.
 */
static void event_prov_done(void *data)
{
	EJ_PutEventSem(EJ_EVENT_ezconnectCompletedSem);
	//PutEzConnectCompletedSem();
}

/* Event: PROV_CLIENT_DONE
 *
 * Provisioning Client has terminated session.
 *
 * We can now safely stop the Micro-AP network.
 *
 * Note: It is possible to keep the Micro-AP network alive even
 * when the provisioning client is done.
 */
static void event_prov_client_done(void *data)
{

}

/*
 * Event UAP_STOPPED
 *
 * Normally, we will get here when provisioning is complete,
 * and the Micro-AP network is brought down.
 *
 * If we are connected to an AP, we can enable IEEE Power Save
 * mode here.
 */
static void event_uap_stopped(void *data)
{
	SetWifiModuleStatusWorkMode(WIFIMODULE_UNKNOW_MODE);
	dbg("Event: Micro-AP Stopped");
	//hp_pm_wifi_ps_enable();
}

/*
 * Event: PROV_WPS_SSID_SELECT_REQ
 *
 * An SSID with active WPS session is found and WPS negotiation will
 * be started with this AP.
 *
 * Since WPS take a lot of memory resources (on the heap), we
 * temporarily stop http server (and, the Micro-AP provisioning
 * along with it).
 *
 * The HTTP server will be restarted when WPS session is over.
 */
static void event_prov_wps_ssid_select_req(void *data)
{

}

/*
 * Event: PROV_WPS_SUCCESSFUL
 *
 * WPS session completed successfully.
 *
 * Restart the HTTP server that was stopped when WPS session attempt
 * began.
 */
static void event_prov_wps_successful(void *data)
{

	return;
}

/*
 * Event: PROV_WPS_UNSUCCESSFUL
 *
 * WPS session completed unsuccessfully.
 *
 * Restart the HTTP server that was stopped when WPS session attempt
 * began.
 */
static void event_prov_wps_unsuccessful(void *data)
{

	return;
}

/*
 * Event: CONNECTING
 *
 * We are attempting to connect to the Home Network
 *
 * Note: We can come here:
 *
 *   1. After boot -- if already provisioned.
 *   2. After provisioning
 *   3. After link loss
 *
 * This is just a transient state as we will either get
 * CONNECTED or have a CONNECTION/AUTH Failure.
 *
 */
static void event_normal_connecting(void *data)
{
	uint8_t my_mac[6];


	wlan_get_mac_address(my_mac);

	char pm_mcu_wifi_demo_hostname[30];

	snprintf(pm_mcu_wifi_demo_hostname,
		 sizeof(pm_mcu_wifi_demo_hostname),
		 "wmdemo-%02X%02X", my_mac[4], my_mac[5]);
	net_dhcp_hostname_set("pm_mcu_wifi_demo_hostname");
 	dbg("Connecting to Home Network...");
	EJ_PutEventSem(EJ_EVENT_routerConnectingSem);

}

/* Event: AF_EVT_NORMAL_CONNECTED
 *
 * Station interface connected to home AP.
 *
 * Network dependent services can be started here. Note that these
 * services need to be stopped on disconnection and
 * reset-to-provisioning event.
 */
static void event_normal_connected(void *data)
{
	char ip[16];


	app_network_ip_get(ip);

	EJ_Printf("sync network time :%ld!\r\n",wmtime_time_get_posix());
	SetWifiModuleStatusWorkMode(WIFIMODULE_CLIENT_MODE);

	GetWifiModuleStatusSignalStrength();
	//PutRouterConnectedSem();
	EJ_PutEventSem(EJ_EVENT_routerConnectedSem);
	
	
}
/*
 * Event: CONNECT_FAILED
 *
 * We attempted to connect to the Home AP, but the connection did
 * not succeed.
 *
 * This typically indicates:
 *
 * -- Authentication failed.
 * -- The access point could not be found.
 * -- We did not get a valid IP address from the AP
 *
 */
static void event_connect_failed(void *data)
{
	char failure_reason[32];
	static int iReconn=0;
	if (*(app_conn_failure_reason_t *)data == AUTH_FAILED) {
		
		strcpy(failure_reason, "Authentication failure");
		if(iReconn++>2){
			EJ_PutEventSem(EJ_EVENT_routerAuthFailedSem);
		}
		
	}
	if (*(app_conn_failure_reason_t *)data == NETWORK_NOT_FOUND)
		strcpy(failure_reason, "Network not found");
	if (*(app_conn_failure_reason_t *)data == DHCP_FAILED)
		strcpy(failure_reason, "DHCP failure");

	
	SetWifiModuleStatusWorkMode(WIFIMODULE_UNKNOW_MODE);
	EJ_thread_sleep(EJ_msec_to_ticks(2000));
	dbg("Application Error: Connection Failed: %s", failure_reason);
}

/*
 * Event: USER_DISCONNECT
 *
 * This means that the application has explicitly requested a network
 * disconnect
 *
 */
static void event_normal_user_disconnect(void *data)
{
	
	dbg("User disconnect");
}

/*
 * Event: LINK LOSS
 *
 * We lost connection to the AP.
 *
 * The App Framework will attempt to reconnect. We dont
 * need to do anything here.
 */
static void event_normal_link_lost(void *data)
{
	SetWifiModuleStatusWorkMode(WIFIMODULE_UNKNOW_MODE);
	pm_ieeeps_hs_cfg(false, 0);
	dbg("Link Lost");
}

static void event_normal_pre_reset_prov(void *data)
{
	//hp_mdns_deannounce(net_get_mlan_handle());
}

static void event_normal_dhcp_renew(void *data)
{
	void *iface_handle = net_get_mlan_handle();
	//hp_mdns_announce(iface_handle, REANNOUNCE);
}

static void event_normal_reset_prov(void *data)
{

	/* Cancel the UAP down timer timer */
	EJ_timer_deactivate(&uap_down_timer);


	/* Reset to provisioning */
	provisioned = 0;
#ifndef APPCONFIG_PROV_EZCONNECT
	if (is_uap_started() == false) {
		app_uap_start_with_dhcp(appln_cfg.ssid, appln_cfg.passphrase);
	} else {
#ifdef APPCONFIG_WPS_ENABLE
		
#else
		
#endif /* APPCONFIG_WPS_ENABLE */
	}
#else

#endif
}
void ps_state_to_desc(char *ps_state_desc, int ps_state)
{
	switch (ps_state) {
	case WLAN_IEEE:
		strcpy(ps_state_desc, "IEEE PS");
		break;
	case WLAN_DEEP_SLEEP:
		strcpy(ps_state_desc, "Deep sleep");
		break;
	case WLAN_PDN:
		strcpy(ps_state_desc, "Power down");
		break;
	case WLAN_ACTIVE:
		strcpy(ps_state_desc, "WLAN Active");
		break;
	default:
		strcpy(ps_state_desc, "Unknown");
		break;
	}
}

/*
 * Event: PS ENTER
 *
 * Application framework event to indicate the user that WIFI
 * has entered Power save mode.
 */

static void event_ps_enter(void *data)
{
	int ps_state = (int) data;
	char ps_state_desc[32];
	if (ps_state == WLAN_PDN) {
		dbg("NOTE: Due to un-availability of "
		    "enough dynamic memory for ");
		dbg("de-compression of WLAN Firmware, "
		    "exit from PDn will not\r\nwork with wm_demo.");
		dbg("Instead of wm_demo, pm_mc200_wifi_demo"
		    " application demonstrates\r\nthe seamless"
		    " exit from PDn functionality today.");
		dbg("This will be fixed in subsequent "
		    "software release.\r\n");
	}
	ps_state_to_desc(ps_state_desc, ps_state);
	dbg("Power save enter : %s", ps_state_desc);

}

/*
 * Event: PS EXIT
 *
 * Application framework event to indicate the user that WIFI
 * has exited Power save mode.
 */

static void event_ps_exit(void *data)
{
	int ps_state = (int) data;
	char ps_state_desc[32];
	ps_state_to_desc(ps_state_desc, ps_state);
	dbg("Power save exit : %s", ps_state_desc);
}

/* This is the main event handler for this project. The application framework
 * calls this function in response to the various events in the system.
 */
int common_event_handler(int event, void *data)
{
	switch (event) {
	case AF_EVT_INIT_DONE:
		event_init_done(data);
		break;
	case AF_EVT_WLAN_INIT_DONE:
		event_wlan_init_done(data);
		break;
	case AF_EVT_NORMAL_CONNECTING:
		event_normal_connecting(data);
		break;
	case AF_EVT_NORMAL_CONNECTED:
		event_normal_connected(data);
		break;
	case AF_EVT_NORMAL_CONNECT_FAILED:
		event_connect_failed(data);
		break;
	case AF_EVT_NORMAL_LINK_LOST:
		event_normal_link_lost(data);
		break;
	case AF_EVT_NORMAL_USER_DISCONNECT:
		event_normal_user_disconnect(data);
		break;
	case AF_EVT_NORMAL_DHCP_RENEW:
		event_normal_dhcp_renew(data);
		break;
	case AF_EVT_PROV_WPS_SSID_SELECT_REQ:
		event_prov_wps_ssid_select_req(data);
		break;
	case AF_EVT_PROV_WPS_SUCCESSFUL:
		event_prov_wps_successful(data);
		break;
	case AF_EVT_PROV_WPS_UNSUCCESSFUL:
		event_prov_wps_unsuccessful(data);
		break;
	case AF_EVT_NORMAL_PRE_RESET_PROV:
		event_normal_pre_reset_prov(data);
		break;
	case AF_EVT_NORMAL_RESET_PROV:
		event_normal_reset_prov(data);
		break;
	case AF_EVT_UAP_STARTED:
		event_uap_started(data);
		break;
	case AF_EVT_UAP_STOPPED:
		event_uap_stopped(data);
		break;
	case AF_EVT_PROV_DONE:
		event_prov_done(data);
		break;
	case AF_EVT_PROV_CLIENT_DONE:
		event_prov_client_done(data);
		break;
	case AF_EVT_PS_ENTER:
		event_ps_enter(data);
		break;
	case AF_EVT_PS_EXIT:
		event_ps_exit(data);
		break;
	default:
		break;
	}

	return 0;
}

#define VAR_NET_LOWPOWER	"network.lowpower"

void pre_module_init()
{

}


void module_init()
{
	int ret;

	EJ_Printf("Build Time: " __DATE__ " " __TIME__ "\r\n");
	ret = sys_psm_init();
	if (ret != EJ_SUCCESS) {
		EJ_Printf("sys psm init failed\r\n");
		return;
	}

	/* Initialize WM core modules */
	ret = wm_core_and_wlan_init();
	if (ret) {
		EJ_Printf("Error initializing WLAN subsystem. Reason: %d\r\n",
				ret);
		/* System core init failed, just wait forever here */
		while (1)
			;
	}
	ret = psm_cli_init(sys_psm_get_handle(), NULL);
	if (ret) {
		EJ_Printf("Error registering PSM commands. Reason: %d\r\n",
				ret);
		/* System core init failed, just wait forever here */
		while (1)
			;
	}

	ret = healthmon_start();
	if (ret != EJ_SUCCESS) {
		EJ_Printf("Error: healthmon_start failed");
		//appln_critical_error_handler((void *) -WM_FAIL);
	}

	/*
	 * Register Power Management CLI Commands
	 */
	ret = pm_cli_init();
	if (ret != EJ_SUCCESS) {
		EJ_Printf("Error: pm_cli_init failed");
		while(1);
	}

	ret = nw_utils_cli_init();
	if (ret != EJ_SUCCESS) {
		EJ_Printf("nw_utils cli init failed: %d\r\n", ret);
		return ret;
	}
	

	sysinfo_init();

	wlan_configure_null_pkt_interval(-1);
	
}

int main()
{
	wmstdio_init(UART0_ID, 0);
	module_init();
	if (app_framework_start(common_event_handler) != EJ_SUCCESS) {
		EJ_Printf("Failed to start application framework\r\n");
		while (1) ;
	}

	
	return 0;
}


