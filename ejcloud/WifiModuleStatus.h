#ifndef _H_WIFIMODULE_STATUS_H_
#define _H_WIFIMODULE_STATUS_H_

#include "ej_porting_layer.h"
#include "ej_platform_conf.h"

//#include <wlan.h>
////#include <wm_os.h>
//#include <psm.h>
//#include <cli.h>
//#include <partition.h>


typedef enum {

	NO_SIGNAL = 0x00,
	SIGNAL_ONE_GRADES,
	SIGNAL_TWO_GRADES,
	SIGNAL_THREE_GRADES,
	SIGNAL_FOUR_GRADES,

}WIFIMODULE_SIGNALINTENSITY;

typedef enum {

	ROUTER_CONNECTED = 0x00,
	ROUTER_NOT_CONNECTED,
	ROUTER_CONNECTTING,
	ROUTER_AUTH_PASSWORD_ERROR,
	ROUTER_NOT_FIND,
	ROUTER_CANNOT_GET_IP,
	ROUTER_NOT_STABLE,
	WIFIMODULE_ERROR,

}WIFIMODULE_ROUTER_STATUS;

typedef enum {
	CLOUD_CONNECTED = 0x00,
	CLOUD_NOT_CONNECTED,
	CLOUD_CONNECT_NOT_STABLE,
	CLOUD_ADDRESS_PARSER_ERROR,
	CLOUD_CONNECT_REFUSE,
	CLOUD_MAINTAINNING,
	CLOUD_FAULT,

}WIFIMODULE_CLOUD_SERVICE_STATUS;

typedef enum {

	NO_CONNECTED = 0x00,
	CONNECTED,
	
}WIFIMODULE_CONNECT_STATUS;

typedef enum {

	MODULE_NOT_CONFIGURED,
	MODULE_CONFIGURED,
	
}WIFIMODULE_CONFIG_STATUS;

typedef enum {

	HOMEAP_NOT_CONFIGURED,
	HOMEAP_CONFIGURED,
	
}WIFIMODULE_HOMEAPCONFIG_STATUS;

typedef enum {

	LANMODULE_NOT_RUNNING,
	LANMODULE_RUNNING,

}WIFIMODULE_LANMODULERUNNING_STATUS;

typedef enum {

	UDPBROADCASTMODULE_NOT_RUNNING = 0x00,
	UDPBROADCASTMODULE_RUNNING,

}WIFIMODULE_UDPBROADCASTMODULERUNNING_STATUS;

typedef enum {
	
	DEVICEID_REQUEST_SUCCESS = 0x00,
	DEVICEID_REQUEST_FAILED,

}WIFIMODULE_DEVICEID_REQUEST_STATUS;

typedef enum {

	MODULE_TYPE_UNKNOWN = 0x00,
	MODULE_TYPE_WIFI,

}MODULE_TYPE;

typedef enum {

	MODULE_MODEL_UNKNOWN = 0x00,
	MODULE_MODEL_CCD_U_MW200,
	MODULE_MODEL_CCD_U_MW300,

}MODULE_MODEL;

typedef enum {

	WIFIMODULE_EASYCONNECT_MODE = 0x00,
	WIFIMODULE_SOFTAP_MODE,
	WIFIMODULE_AIRKISS_MODE,
	WIFIMODULE_JOYLINK_MODE,
	WIFIMODULE_ALI_MODE,
	WIFIMODULE_CLIENT_MODE,
	WIFIMODULE_UNKNOW_MODE,

}WIFIMODULE_CONFIG_MODE;

typedef struct {

	uint8_t configStatus;

	uint8_t deviceIDRequestStatus;

	uint8_t deviceConnectStatus;

	uint8_t workMode;

	uint8_t signalIntensity;

	uint8_t routerStatus;

	uint8_t cloudServiceStatus;

	uint8_t connectedStatus;

	uint8_t tcpConnectedCount;

	uint8_t ipaddr[4];

	uint8_t deviceID[6];

	uint8_t isHomeAPConfig;

	uint8_t isLanModuleRunning;

	uint8_t isUdpBroadcastRunning;

}WifiModuleStatus;

typedef struct {

	uint8_t brand[3];

	uint8_t manufacturers[3];

	uint8_t type[3];

	uint8_t modelID[3];

	uint8_t NO[6];

	uint8_t time[4];

}WIFIModuleSN;



typedef struct {

	uint8_t brand[3];

	uint8_t manufacturers[3];

	uint8_t type[3];

	uint8_t modelID[3];

	uint8_t NO[4];

	uint8_t time[4];

	uint8_t addr[2];

}DeviceSN;

typedef struct {

	uint8_t uuid[6];

}DeviceUuid;


typedef struct {

	//DeviceSN SN;
	DeviceUuid UUID;

	uint8_t manufacturers[6];

	uint8_t deviceType;

	uint8_t qos;

	uint8_t acktimeout;

	uint8_t crypt;

	uint8_t defaultConfigMode;

	uint8_t *deviceBrandString;

	uint8_t *deviceTypeString;

	uint8_t *deviceModelString;

	uint8_t *deviceVersionString;

}DeviceInfo;

typedef struct {

	uint8_t versionRule;
	
	uint8_t functionCode;

	uint8_t hardwareVersion;

	uint8_t softwareVersionMsb;

	uint8_t softwareVersionLsb;

	uint8_t publishYear;

	uint8_t publishWeek;

}WIFIModuleVersion;

typedef struct {

	uint8_t versionRule;

	uint8_t deviceBrandcode;

	uint8_t deviceModel;

	uint8_t hardwareVersion;

	uint8_t softwareVersion;

	uint8_t publishYear;

	uint8_t publishWeek;

	uint8_t reserved;

}DeviceVersion;

typedef struct {

	WIFIModuleSN SN;

	uint8_t type;

	uint8_t model;

	WIFIModuleVersion version;

	uint8_t *brandStr;

	uint8_t *modelStr;

}WIFIModuleInfo;

typedef struct {

	uint8_t todo;

	char *renameSSID;

	char *cloudDomainAddr;

	uint8_t configMode;

	char *masterServerAddr;

	int  masterServerPort;

	char *backupServer1Addr;

	int  backupServer1Port;

	char *backupServer2Addr;

	int  backupServer2Port;

	uint8_t defaultConnectServer;

}WifiModuleConfiguration;


typedef struct {

	DeviceInfo deviceInfo;

	WifiModuleStatus wifiStatus;

	WIFIModuleInfo   wifiInfo;

	WifiModuleConfiguration wifiConfig;

}InfoManagement;



uint8_t loadInfoFromFlash();
uint8_t GetWifiModuleStatusConfigStatus();
uint8_t GetWifiModuleStatusDeviceIDRequestStatus();
uint8_t GetWifiModuleStatusDeviceConnectStatus();
uint8_t GetWifiModuleStatusWorkMode();
uint8_t GetWifiModuleStatusSignalIntensity();
uint8_t GetWifiModuleStatusRouterStatus();
uint8_t GetWifiModuleStatusCloudServiceStatus();
uint8_t GetWifiModuleStatusConnectedStatus();
uint8_t GetWifiModuleStatusTcpConnectedCount();
uint8_t GetWifiModuleStatusIsHomeAPConfig();
uint8_t GetWifiModuleStatusIsLanModuleRunning();
uint8_t GetWifiModuleStatusIsUdpBroadcastRunning();


void SetWifiModuleStatusConfigStatus(uint8_t status);
void SetWifiModuleStatusDeviceIDRequestStatus(uint8_t status);
void SetWifiModuleStatusDeviceConnectStatus(uint8_t status);
void SetWifiModuleStatusWorkMode(uint8_t mode);
void SetWifiModuleStatusSignalIntensity(uint8_t Intensity);
void SetWifiModuleStatusRouterStatus(uint8_t status);
void SetWifiModuleStatusCloudServiceStatus(uint8_t status);
void SetWifiModuleStatusConnectedStatus(uint8_t status);
void SetWifiModuleStatusTcpConnectedCount(uint8_t count);
void SetWifiModuleStatusIsHomeAPConfig(uint8_t isConfig);
void SetWifiModuleStatusIsLanModuleRunning(uint8_t isRunning);
void SetWifiModuleStatusIsUdpBroadcastRunning(uint8_t isRunning);

WIFIModuleSN GetWifiModuleInfoSN();
uint8_t GetWifiModuleInfoType();
uint8_t GetWifiModuleInfoModel();
WIFIModuleVersion GetWifiModuleInfoVersion();
uint8_t *GetWifiModuleInfoBrandStr();
uint8_t *GetWifiModuleInfoModelStr();




void SetWifiModuleInfoSN(WIFIModuleSN *SN);
void SetWifiModuleInfoType(uint8_t type);
void SetWifiModuleInfoModel(uint8_t model);
void SetWifiModuleInfoVersion(WIFIModuleVersion *version);
uint8_t storeWifiModuleInfoVersion(WIFIModuleVersion *version);
void SetWifiModuleInfoBrandStr(uint8_t *brand);
void SetWifiModuleInfoModelStr(uint8_t *model);



DeviceUuid GetDeviceInfoDeviceUuid();
uint8_t GetDeviceInfoDeviceType();
void GetDeviceInfoManufacturers(uint8_t *manufacturers);
uint8_t GetDeviceInfoQos();
uint8_t GetDeviceInfoAckTimeout();
uint8_t GetDeviceInfoCrypt();
uint8_t GetDeviceInfoDefaultConfigMode();
uint8_t * GetDeviceInfoDeviceBrandString();
uint8_t * GetDeviceInfoDeviceTypeString();
uint8_t * GetDeviceInfoDeviceModelString();
uint8_t * GetDeviceInfoDeviceTVersionString();



void SetDeviceInfoDeviceType(uint8_t deviceType);
void SetDeviceInfoManufacturers(uint8_t *manufacturers);
void SetDeviceInfoQos(uint8_t qos);
void SetDeviceInfoAckTimeout(uint8_t acktimeout);
void SetDeviceInfoCrypt(uint8_t crypt);
void SetDeviceInfoDefaultConfigMode(uint8_t defaultConfigMode);
void SetDeviceInfoDeviceBrandString(uint8_t *deviceBrandString);
void SetDeviceInfoDeviceTypeString(uint8_t *deviceTypeString);
void SetDeviceInfoDeviceModelString(uint8_t *deviceModelString);
void SetDeviceInfoDeviceTVersionString(uint8_t *deviceVersionString);



void GetWifiStatusDeviceID(uint8_t *deviceID);
void SetWifiStatusDeviceID(uint8_t *deviceID);
void GetWifiStatusDeviceUuid(uint8_t *deviceUUID);


uint8_t GetWifiConfigConfigMode();
void SetWifiConfigConfigMode(uint8_t configMode);
uint8_t storeWifiConfigConfigMode(uint8_t configMode);
void loadWifiConfigConfigMode();

char * GetWifiConfigRenameSSID();
void SetWifiConfigRenameSSID(char *renameSSID);
uint8_t storeWifiConfigRenameSSID(char *renameSSID);
void loadWifiConfigRenameSSID();


char * GetWifiConfigCloudDomainAddr();

void  SetWifiConfigCloudDomainAddr(char *cloudDomainAddr);

uint8_t storeWifiConfigCloudDomainAddr(char *cloudDomainAddr);

void loadWifiConfigCloudDomainAddr();


char * GetWifiConfigMasterServerAddr();

void SetWifiConfigMasterServerAddr(char *masterServerAddr);

uint8_t storeWifiConfigMasterServerAddr(char *masterServerAddr);

void loadWifiConfigMasterServerAddr();



int GetWifiConfigMasterServerPort();

void SetWifiConfigMasterServerPort(int masterServerPort);

uint8_t storeWifiConfigMasterServerPort(int masterServerPort);

void loadWifiConfigMasterServerPort();



char * GetWifiConfigBackupServer1Addr();

void SetWifiConfigBackupServer1Addr(char *backupServer1Addr);

uint8_t storeWifiConfigBackupServer1Addr(char *backupServer1Addr);

void loadWifiConfigBackupServer1Addr();


int GetWifiConfigBackupServer1Port();

void SetWifiConfigBackupServer1Port(int backupServer1Port);

uint8_t storeWifiConfigBackupServer1Port(int backupServer1Port);

void loadWifiConfigBackupServer1Port();



char * GetWifiConfigBackupServer2Addr();

void SetWifiConfigBackupServer2Addr(char *backupServer2Addr);

uint8_t storeWifiConfigBackupServer2Addr(char *backupServer2Addr);

void loadWifiConfigBackupServer2Addr();


int GetWifiConfigBackupServer2Port();

void SetWifiConfigBackupServer2Port(int backupServer2Port);

uint8_t storeWifiConfigBackupServer2Port(int backupServer2Port);

void loadWifiConfigBackupServer2Port();



uint8_t GetWifiConfigDefaultConnectServer();

void SetWifiConfigDefaultConnectServer(uint8_t defaultConnectServer);

uint8_t storeWifiConfigDefaultConnectServer(uint8_t defaultConnectServer);

void loadWifiConfigDefaultConnectServer();


int  GetWifiConfigSSID(char *ssid,int len);

bool DeviceInfo_save_uuid(char* value);


bool DeviceInfo_load_uuid(char* value);


uint8_t storeDeviceInfoDeviceid(uint8_t *Deviceid);


void GetWifiModuleStatusSignalStrength();

#endif //_H_WIFIMODULE_STATUS_H_
