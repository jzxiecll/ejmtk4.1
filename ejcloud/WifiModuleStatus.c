
#include "ej_port_psm.h"
#include "WifiModuleStatus.h"
#include "ej_platform_conf.h"
#include "ej_wificonfig.h"


InfoManagement h_InfoManagement;



uint8_t GetWifiModuleStatusConfigStatus()
{
  return h_InfoManagement.wifiStatus.configStatus;
}
uint8_t GetWifiModuleStatusDeviceIDRequestStatus()
{
  return h_InfoManagement.wifiStatus.deviceIDRequestStatus;
}
uint8_t GetWifiModuleStatusDeviceConnectStatus()
{
  return h_InfoManagement.wifiStatus.deviceConnectStatus;
}
uint8_t GetWifiModuleStatusWorkMode()
{
  return h_InfoManagement.wifiStatus.workMode;
}
uint8_t GetWifiModuleStatusSignalIntensity()
{
  return h_InfoManagement.wifiStatus.signalIntensity;
}
uint8_t GetWifiModuleStatusRouterStatus()
{
  return h_InfoManagement.wifiStatus.routerStatus;
}
uint8_t GetWifiModuleStatusCloudServiceStatus()
{
  return h_InfoManagement.wifiStatus.cloudServiceStatus;
}
uint8_t GetWifiModuleStatusConnectedStatus()
{
  return h_InfoManagement.wifiStatus.connectedStatus;
}
uint8_t GetWifiModuleStatusTcpConnectedCount()
{
  return h_InfoManagement.wifiStatus.tcpConnectedCount;
}

uint8_t GetWifiModuleStatusIsHomeAPConfig()
{
  return h_InfoManagement.wifiStatus.isHomeAPConfig;
}

uint8_t GetWifiModuleStatusIsLanModuleRunning()
{
  return h_InfoManagement.wifiStatus.isLanModuleRunning;
}

uint8_t GetWifiModuleStatusIsUdpBroadcastRunning()
{
  return h_InfoManagement.wifiStatus.isUdpBroadcastRunning;
}


void SetWifiModuleStatusConfigStatus(uint8_t status)
{
  h_InfoManagement.wifiStatus.configStatus = status;
}
void SetWifiModuleStatusDeviceIDRequestStatus(uint8_t status)
{
  h_InfoManagement.wifiStatus.deviceIDRequestStatus = status;
}
void SetWifiModuleStatusDeviceConnectStatus(uint8_t status)
{
  h_InfoManagement.wifiStatus.deviceConnectStatus = status;
}

void SetWifiModuleStatusWorkMode(uint8_t mode)
{
  h_InfoManagement.wifiStatus.workMode = mode;
}
void SetWifiModuleStatusSignalIntensity(uint8_t Intensity)
{
  h_InfoManagement.wifiStatus.signalIntensity = Intensity;
}
void SetWifiModuleStatusRouterStatus(uint8_t status)
{
  h_InfoManagement.wifiStatus.routerStatus = status;
}
void SetWifiModuleStatusCloudServiceStatus(uint8_t status)
{
  h_InfoManagement.wifiStatus.cloudServiceStatus = status;
}
void SetWifiModuleStatusConnectedStatus(uint8_t status)
{
  h_InfoManagement.wifiStatus.connectedStatus = status;
}
void SetWifiModuleStatusTcpConnectedCount(uint8_t count)
{
  h_InfoManagement.wifiStatus.tcpConnectedCount = count;
}

void SetWifiModuleStatusIsHomeAPConfig(uint8_t isConfig)
{
  h_InfoManagement.wifiStatus.isHomeAPConfig = isConfig;
}

void SetWifiModuleStatusIsLanModuleRunning(uint8_t isRunning)
{
  h_InfoManagement.wifiStatus.isLanModuleRunning = isRunning;
}

void SetWifiModuleStatusIsUdpBroadcastRunning(uint8_t isRunning)
{
  h_InfoManagement.wifiStatus.isUdpBroadcastRunning = isRunning;
}

WIFIModuleSN GetWifiModuleInfoSN()
{
  return h_InfoManagement.wifiInfo.SN;
}
uint8_t GetWifiModuleInfoType() 
{
  return h_InfoManagement.wifiInfo.type;
}
uint8_t GetWifiModuleInfoModel() 
{
  return h_InfoManagement.wifiInfo.model;
}

WIFIModuleVersion GetWifiModuleInfoVersion()
{
  return h_InfoManagement.wifiInfo.version;
}

uint8_t *GetWifiModuleInfoBrandStr()
{
  return h_InfoManagement.wifiInfo.brandStr;
}
uint8_t *GetWifiModuleInfoModelStr()
{
  return h_InfoManagement.wifiInfo.modelStr;
}

void SetWifiModuleInfoType(uint8_t type)
{
  h_InfoManagement.wifiInfo.type = type;
}
void SetWifiModuleInfoModel(uint8_t model)
{
  h_InfoManagement.wifiInfo.model = model;
}
void SetWifiModuleInfoVersion(WIFIModuleVersion *version)
{
  memcpy((uint8_t *)(&(h_InfoManagement.wifiInfo.version)), (uint8_t *)version, sizeof(WIFIModuleVersion));

  storeWifiModuleInfoVersion(version);
}
uint8_t storeWifiModuleInfoVersion(WIFIModuleVersion *version)
{
  int ret = 0;

  char strval[7] = { '\0' };

  sprintf(strval, "%d", version->versionRule);
  ret = EJ_write_psm_item("wifimodule-version-versionRule", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiModuleInfoVersion][ERROR]: Error set wifimodule-version-versionRule.\r\n"));
  }



  sprintf(strval, "%d", version->functionCode);
  ret = EJ_write_psm_item( "wifimodule-version-functionCode", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiModuleInfoVersion][ERROR]: Error set wifimodule-version-functionCode.\r\n"));
  }



  sprintf(strval, "%d", version->hardwareVersion);
  ret = EJ_write_psm_item("wifimodule-version-hardwareVersion", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiModuleInfoVersion][ERROR]: Error set wifimodule-version-hardwareVersion.\r\n"));
  }



  sprintf(strval, "%d", version->softwareVersionMsb);
  ret = EJ_write_psm_item( "wifimodule-version-softwareVersionMsb", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiModuleInfoVersion][ERROR]: Error set wifimodule-version-softwareVersionMsb.\r\n"));
  }



  sprintf(strval, "%d", version->softwareVersionLsb);
  ret = EJ_write_psm_item("wifimodule-version-softwareVersionLsb", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiModuleInfoVersion][ERROR]: Error set wifimodule-version-softwareVersionLsb.\r\n"));
  }



  sprintf(strval, "%d", version->publishYear);
  ret = EJ_write_psm_item( "wifimodule-version-publishYear", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiModuleInfoVersion][ERROR]: Error set wifimodule-version-publishYear.\r\n"));
  }



  sprintf(strval, "%d", version->publishWeek);
  ret = EJ_write_psm_item( "wifimodule-version-publishWeek", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiModuleInfoVersion][ERROR]: Error set wifimodule-version-publishWeek.\r\n"));
  }

	return ret;
	
}
uint8_t loadWifiModuleInfoVersion()
{
  uint8_t ret = 0;

  WIFIModuleVersion version;

  char strval[7];

  ret = EJ_read_psm_item("wifimodule-version-versionRule", strval,7);
  if (ret != 0) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiModuleInfoVersion][DEBUG]: Error get wifimodule-version-versionRule.\r\n"));
		ret = 1;
  }
  else {		
    version.versionRule= atoi(strval);
  }

  ret = EJ_read_psm_item( "wifimodule-version-functionCode", strval,7);
  if (ret != 0) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiModuleInfoVersion][DEBUG]: Error get wifimodule-version-functionCode.\r\n"));
		ret = 1;
  }
  else {
    version.functionCode= atoi(strval);
  }

  ret = EJ_read_psm_item("wifimodule-version-hardwareVersion", strval,7);
  if (ret != 0) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiModuleInfoVersion][DEBUG]: Error get wifimodule-version-hardwareVersion.\r\n"));

		ret = 1;
  }
  else {
    version.hardwareVersion= atoi(strval);
  }

  ret = EJ_read_psm_item( "wifimodule-version-softwareVersionMsb", strval,7);
  if (ret != 0) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiModuleInfoVersion][DEBUG]: Error get wifimodule-version-softwareVersionMsb.\r\n"));
		ret = 1;
  }
  else {
    version.softwareVersionMsb= atoi(strval);
  }

  ret = EJ_read_psm_item("wifimodule-version-softwareVersionLsb", strval,7);
  if (ret != 0) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiModuleInfoVersion][DEBUG]: Error get wifimodule-version-softwareVersionLsb.\r\n"));
		ret = 1;
  }
  else {
    version.softwareVersionLsb= atoi(strval);
  }

  ret = EJ_read_psm_item( "wifimodule-version-publishYear", strval,7);
  if (ret != 0) {

    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiModuleInfoVersion][DEBUG]: Error get wifimodule-version-publishYear.\r\n"));
		ret = 1;
  }
  else {
    version.publishYear= atoi(strval);
  }

  ret = EJ_read_psm_item( "wifimodule-version-publishWeek", strval,7);
  if (ret != 0) {
    EJ_DebugPrintf(("[WifiModuleStatus.c][loadWifiModuleInfoVersion][DEBUG]: Error get wifimodule-version-publishWeek.\r\n"));
		ret = 1;
  }
  else {
    version.publishWeek= atoi(strval);
    SetWifiModuleInfoVersion(&version);
  }

	
  return ret;
	
}

void SetWifiModuleInfoBrandStr(uint8_t *brand)
{
  h_InfoManagement.wifiInfo.brandStr = brand;
}
void SetWifiModuleInfoModelStr(uint8_t *model)
{
  h_InfoManagement.wifiInfo.modelStr = model;
}

DeviceUuid GetDeviceInfoDeviceUuid()
{
	return h_InfoManagement.deviceInfo.UUID;
}

uint8_t GetDeviceInfoDeviceType()
{
  return h_InfoManagement.deviceInfo.deviceType;
}
void GetDeviceInfoManufacturers(uint8_t *manufacturers)
{
  if (manufacturers) {

    memcpy(manufacturers, h_InfoManagement.deviceInfo.manufacturers, 6);
  }
}

uint8_t GetDeviceInfoQos()
{
  return h_InfoManagement.deviceInfo.qos;
}

uint8_t GetDeviceInfoAckTimeout()
{
  return h_InfoManagement.deviceInfo.acktimeout;
}

uint8_t GetDeviceInfoCrypt()
{
  return h_InfoManagement.deviceInfo.crypt;
}

uint8_t GetDeviceInfoDefaultConfigMode()
{
  return h_InfoManagement.deviceInfo.defaultConfigMode;
}

uint8_t * GetDeviceInfoDeviceBrandString()
{
  return h_InfoManagement.deviceInfo.deviceBrandString;
	
} 


uint8_t * GetDeviceInfoDeviceTypeString()
{
  return h_InfoManagement.deviceInfo.deviceTypeString;

}


uint8_t * GetDeviceInfoDeviceModelString()
{

  return h_InfoManagement.deviceInfo.deviceModelString;

}


uint8_t * GetDeviceInfoDeviceTVersionString()
{

  return h_InfoManagement.deviceInfo.deviceVersionString;
}



void SetDeviceInfoDeviceUuid(uint8_t *UUID)
{
	memcpy((uint8_t *)(&(h_InfoManagement.deviceInfo.UUID)),UUID,6);
}

void SetDeviceInfoDeviceType(uint8_t deviceType)
{
  h_InfoManagement.deviceInfo.deviceType = deviceType;
}


void SetDeviceInfoManufacturers(uint8_t *manufacturers)
{
  if (manufacturers) {

    memcpy(h_InfoManagement.deviceInfo.manufacturers, manufacturers, 6);
  }
}


void SetDeviceInfoQos(uint8_t qos)
{
  h_InfoManagement.deviceInfo.qos = qos;
}

void SetDeviceInfoAckTimeout(uint8_t acktimeout)
{
  h_InfoManagement.deviceInfo.acktimeout = acktimeout;
}

void SetDeviceInfoCrypt(uint8_t crypt)
{
  h_InfoManagement.deviceInfo.crypt = crypt;
}

void SetDeviceInfoDefaultConfigMode(uint8_t defaultConfigMode)
{
    h_InfoManagement.deviceInfo.defaultConfigMode = defaultConfigMode;
}

void SetDeviceInfoDeviceBrandString(uint8_t *deviceBrandString)
{
  if (deviceBrandString) {

    h_InfoManagement.deviceInfo.deviceBrandString = deviceBrandString;
  }
} 

void SetDeviceInfoDeviceTypeString(uint8_t *deviceTypeString)
{
  if (deviceTypeString) {

    h_InfoManagement.deviceInfo.deviceTypeString = deviceTypeString;
  }
}

void SetDeviceInfoDeviceModelString(uint8_t *deviceModelString)
{
  if (deviceModelString) {

    h_InfoManagement.deviceInfo.deviceModelString = deviceModelString;
  }
}


void SetDeviceInfoDeviceTVersionString(uint8_t *deviceVersionString)
{
  if (deviceVersionString) {

    h_InfoManagement.deviceInfo.deviceVersionString = deviceVersionString;
  }
  
  
}



void GetWifiStatusDeviceID(uint8_t *deviceID)
{
  if (deviceID) {

    memcpy(deviceID, h_InfoManagement.wifiStatus.deviceID, 6);
  }
}

void GetWifiStatusDeviceUuid(uint8_t *deviceUUID)
{
  if (deviceUUID) {

    memcpy(deviceUUID, h_InfoManagement.deviceInfo.UUID.uuid, 6);
  }
}


void SetWifiStatusDeviceID(uint8_t *deviceID)
{
  if (deviceID) {

    memcpy(h_InfoManagement.wifiStatus.deviceID, deviceID, 6);

	//storeDeviceInfoDeviceid(deviceID);
  }
}

uint8_t GetWifiConfigConfigMode()
{
  return h_InfoManagement.wifiConfig.configMode;
}

void SetWifiConfigConfigMode(uint8_t configMode)
{
  h_InfoManagement.wifiConfig.configMode = configMode;

  storeWifiConfigConfigMode(configMode);
}

uint8_t storeWifiConfigConfigMode(uint8_t configMode)
{
  int ret = 0;

  char strval[7] = { '\0' };

  if (configMode == WIFICONFIG_EZCONNECT_MODE) {

    ret = EJ_write_psm_item( "configMode", "WIFICONFIG_EZCONNECT_MODE");
		
  }else if (configMode == WIFICONFIG_AP_MODE) {

    ret = EJ_write_psm_item( "configMode", "WIFICONFIG_AP_MODE");
  }else if (configMode == WIFICONFIG_AIRKISS_MODE) {

    ret = EJ_write_psm_item( "configMode", "WIFICONFIG_AIRKISS_MODE");
  }else{
	
	ret = EJ_write_psm_item( "configMode", "WIFICONFIG_NULL_MODE");
  }
	
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigConfigMode][ERROR]: Error set configMode.\r\n"));
  }
	return ret;

}

void loadWifiConfigConfigMode()
{
  int ret = 0;

  char strval[44+1];

  ret = EJ_read_psm_item( "configMode", strval,45);
  if (ret != 0) {

    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiConfigConfigMode][DEBUG]: Error get configMode.\r\n"));
  }
  else {

    if (strcmp(strval, "WIFICONFIG_EZCONNECT_MODE") == 0) {

      SetWifiConfigConfigMode(WIFICONFIG_EZCONNECT_MODE);
			
    }else if (strcmp(strval, "WIFICONFIG_AP_MODE") == 0) {

      SetWifiConfigConfigMode(WIFICONFIG_AP_MODE);
			
    }else if (strcmp(strval, "WIFICONFIG_AIRKISS_MODE") == 0) {

      SetWifiConfigConfigMode(WIFICONFIG_AIRKISS_MODE);
    }else {
    
	  SetWifiConfigConfigMode(WIFICONFIG_NULL_MODE);
	  
	}
  }

//  /* reset the configMode to NULL. */
//  ret = EJ_write_psm_item("configMode", "WIFICONFIG_NULL_MODE");

//  if (ret) {
//    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiConfigConfigMode][ERROR]: Error set configMode to default WIFICONFIG_NULL_MODE.\r\n"));
//  }


}


char * GetWifiConfigRenameSSID()
{
  return h_InfoManagement.wifiConfig.renameSSID;
}

int  GetWifiConfigSSID(char *ssid,int len)
{
	int rv=0;
	memset(ssid, 0x00, len);
	rv =  EJ_read_sysitem("network.ssid",
				    ssid, len);	
	
	
	return rv;
}

void SetWifiConfigRenameSSID(char *renameSSID)
{
  if(renameSSID) {

    if (!(h_InfoManagement.wifiConfig.renameSSID)) {

      h_InfoManagement.wifiConfig.renameSSID = (char *)EJ_mem_malloc(strlen(renameSSID) + 1);

      memset(h_InfoManagement.wifiConfig.renameSSID, 0, strlen(renameSSID) + 1);
    }

    strcpy(h_InfoManagement.wifiConfig.renameSSID, renameSSID);

    storeWifiConfigRenameSSID(h_InfoManagement.wifiConfig.renameSSID);
  }
}

uint8_t storeWifiConfigRenameSSID(char *renameSSID)
{
  int ret = 0;

  char strval[7] = { '\0' };

  ret = EJ_write_psm_item("renameSSID", renameSSID);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigRenameSSID][ERROR]: Error set renameSSID.\r\n"));
  }
	return ret;
}

void loadWifiConfigRenameSSID()
{
  int ret = 0;

  char strval[44+1];

  ret = EJ_read_psm_item("renameSSID", strval, 45);
  if (ret != 0) {

    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiConfigRenameSSID][DEBUG]: Error get renameSSID.\r\n"));
  }
  else {

    SetWifiConfigRenameSSID(strval);
  }
}

char * GetWifiConfigCloudDomainAddr()
{
  return h_InfoManagement.wifiConfig.cloudDomainAddr;
}

void SetWifiConfigCloudDomainAddr(char *cloudDomainAddr)
{
  if(cloudDomainAddr) {

    if (!(h_InfoManagement.wifiConfig.cloudDomainAddr)) {

      h_InfoManagement.wifiConfig.cloudDomainAddr = (char *)EJ_mem_malloc(strlen(cloudDomainAddr) + 1);

      memset(h_InfoManagement.wifiConfig.cloudDomainAddr, 0, strlen(cloudDomainAddr) + 1);
    }

    strcpy(h_InfoManagement.wifiConfig.cloudDomainAddr, cloudDomainAddr);

    storeWifiConfigCloudDomainAddr(h_InfoManagement.wifiConfig.cloudDomainAddr);
  }
}

uint8_t storeWifiConfigCloudDomainAddr(char *cloudDomainAddr)
{
  int ret = 0;
  ret = EJ_write_psm_item( "cloudDomainAddr", cloudDomainAddr);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigCloudDomainAddr][ERROR]: Error set cloudDomainAddr.\r\n"));
  }

	return ret;
}

void loadWifiConfigCloudDomainAddr()
{
  int ret = 0;

  char strval[44+1];

  ret = EJ_read_psm_item( "cloudDomainAddr", strval, 45);
  if (ret != 0) {

    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiConfigCloudDomainAddr][DEBUG]: Error get cloudDomainAddr.\r\n"));
  }
  else {

    SetWifiConfigCloudDomainAddr(strval);
  }
}

char * GetWifiConfigMasterServerAddr()
{
  return h_InfoManagement.wifiConfig.masterServerAddr;
}

void SetWifiConfigMasterServerAddr(char *masterServerAddr)
{
  if(masterServerAddr) {

    if (!(h_InfoManagement.wifiConfig.masterServerAddr)) {

      h_InfoManagement.wifiConfig.masterServerAddr = (char *)EJ_mem_malloc(strlen(masterServerAddr) + 1);

      memset(h_InfoManagement.wifiConfig.masterServerAddr, 0, strlen(masterServerAddr) + 1);
    }

    strcpy(h_InfoManagement.wifiConfig.masterServerAddr, masterServerAddr);

    storeWifiConfigMasterServerAddr(h_InfoManagement.wifiConfig.masterServerAddr);
  }
}

uint8_t storeWifiConfigMasterServerAddr(char *masterServerAddr)
{
  int ret = 0;

  ret = EJ_write_psm_item( "masterServerAddr", masterServerAddr);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigMasterServerAddr][ERROR]: Error set masterServerAddr.\r\n"));
  }

  return ret;
}

void loadWifiConfigMasterServerAddr()
{
  int ret = 0;

  char strval[44+1];

  ret = EJ_read_psm_item( "masterServerAddr", strval, 45);
  if (ret != 0) {

    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiConfigMasterServerAddr][DEBUG]: Error get masterServerAddr.\r\n"));
  }
  else {

    SetWifiConfigMasterServerAddr(strval);
  }
}


int GetWifiConfigMasterServerPort()
{
  return (int)h_InfoManagement.wifiConfig.masterServerPort;
}

void SetWifiConfigMasterServerPort(int masterServerPort)
{

  h_InfoManagement.wifiConfig.masterServerPort = masterServerPort;

  storeWifiConfigMasterServerPort(h_InfoManagement.wifiConfig.masterServerPort);

}

uint8_t storeWifiConfigMasterServerPort(int masterServerPort)
{
  int ret = 0;

  char strval[7] = { '\0' };

  sprintf(strval, "%d", masterServerPort);

  ret = EJ_write_psm_item( "masterServerPort", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigMasterServerPort][ERROR]: Error set masterServerPort.\r\n"));
  }

	return ret;

}

void loadWifiConfigMasterServerPort()
{
  int ret = 0;

  char strval[7];

  ret = EJ_read_psm_item("masterServerPort", strval, 7);
  if (ret != 0) {

    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiConfigMasterServerPort][DEBUG]: Error get masterServerPort.\r\n"));
  }
  else {

    int port = atoi(strval);
	
    SetWifiConfigMasterServerPort(port);
  }
}


char * GetWifiConfigBackupServer1Addr()
{
  return h_InfoManagement.wifiConfig.backupServer1Addr;
}

void SetWifiConfigBackupServer1Addr(char *backupServer1Addr)
{
  if(backupServer1Addr) {

    if (!(h_InfoManagement.wifiConfig.backupServer1Addr)) {

      h_InfoManagement.wifiConfig.backupServer1Addr = (char *)EJ_mem_malloc(strlen(backupServer1Addr) + 1);

      memset(h_InfoManagement.wifiConfig.backupServer1Addr, 0, strlen(backupServer1Addr) + 1);
    }

    strcpy(h_InfoManagement.wifiConfig.backupServer1Addr, backupServer1Addr);

    storeWifiConfigBackupServer1Addr(h_InfoManagement.wifiConfig.backupServer1Addr);
  }
}

uint8_t storeWifiConfigBackupServer1Addr(char *backupServer1Addr)
{
  int ret = 0;

  char strval[7] = { '\0' };

  ret = EJ_write_psm_item("backupServer1Addr", backupServer1Addr);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigBackupServer1Addr][ERROR]: Error set backupServer1Addr.\r\n"));
  }
  return ret;

}

void loadWifiConfigBackupServer1Addr()
{
  int ret = 0;

  char strval[44+1];

  ret = EJ_read_psm_item( "backupServer1Addr", strval, 45);
  if (ret != 0) {

    EJ_DebugPrintf(("[WifiModuleStatus.c][loadWifiConfigBackupServer1Addr][DEBUG]: Error get backupServer1Addr.\r\n"));
  }
  else {

    SetWifiConfigBackupServer1Addr(strval);
  }
}


int GetWifiConfigBackupServer1Port()
{
  return h_InfoManagement.wifiConfig.backupServer1Port;
}

void SetWifiConfigBackupServer1Port(int backupServer1Port)
{

  h_InfoManagement.wifiConfig.backupServer1Port = backupServer1Port;

  storeWifiConfigMasterServerPort(h_InfoManagement.wifiConfig.backupServer1Port);

}

uint8_t storeWifiConfigBackupServer1Port(int backupServer1Port)
{
  int ret = 0;

  char strval[7] = { '\0' };

  sprintf(strval, "%d", backupServer1Port);

  ret = EJ_write_psm_item( "backupServer1Port", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigBackupServer1Port][ERROR]: Error set backupServer1Port.\r\n"));
  }
  return ret;

}

void loadWifiConfigBackupServer1Port()
{
  int ret = 0;

  char strval[7];

  ret = EJ_read_psm_item( "backupServer1Port", strval, 7);
  if (ret != 0) {

    EJ_ErrPrintf(("[WifiModuleStatus.c][loadWifiConfigBackupServer1Port][DEBUG]: Error get backupServer1Port.\r\n"));
  }
  else {

    int port = atoi(strval);
		
    SetWifiConfigMasterServerPort(port);
  }
}


char * GetWifiConfigBackupServer2Addr()
{
  return h_InfoManagement.wifiConfig.backupServer2Addr;
}

void SetWifiConfigBackupServer2Addr(char *backupServer2Addr)
{
  if(backupServer2Addr) {

    if (!(h_InfoManagement.wifiConfig.backupServer2Addr)) {

      h_InfoManagement.wifiConfig.backupServer2Addr = (char *)EJ_mem_malloc(strlen(backupServer2Addr) + 1);

      memset(h_InfoManagement.wifiConfig.backupServer2Addr, 0, strlen(backupServer2Addr) + 1);
    }

    strcpy(h_InfoManagement.wifiConfig.backupServer2Addr, backupServer2Addr);

    storeWifiConfigMasterServerAddr(h_InfoManagement.wifiConfig.backupServer2Addr);
  }
}

uint8_t storeWifiConfigBackupServer2Addr(char *backupServer2Addr)
{
  int ret = 0;

  char strval[7] = { '\0' };

  ret = EJ_write_psm_item( "backupServer2Addr", backupServer2Addr);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigBackupServer2Addr][ERROR]: Error set backupServer2Addr.\r\n"));
  }
	return ret;

}

void loadWifiConfigBackupServer2Addr()
{
  int ret = 0;

  char strval[44+1];

  ret = EJ_read_psm_item( "masterServerAddr", strval, 45);
  if (ret != 0) {

    EJ_DebugPrintf(("[WifiModuleStatus.c][loadWifiConfigMasterServerAddr][DEBUG]: Error get masterServerAddr.\r\n"));
  }
  else {

    SetWifiConfigMasterServerAddr(strval);
  }
}


int GetWifiConfigBackupServer2Port()
{
  return h_InfoManagement.wifiConfig.backupServer2Port;
}

void SetWifiConfigBackupServer2Port(int backupServer2Port)
{

  h_InfoManagement.wifiConfig.backupServer2Port = backupServer2Port;

  storeWifiConfigMasterServerPort(h_InfoManagement.wifiConfig.backupServer2Port);

}

uint8_t storeWifiConfigBackupServer2Port(int backupServer2Port)
{
  int ret = 0;

  char strval[7] = { '\0' };

  sprintf(strval, "%d", backupServer2Port);

  ret = EJ_write_psm_item( "backupServer2Port", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigBackupServer2Port][ERROR]: Error set backupServer2Port.\r\n"));
  }

	return ret;
}

void loadWifiConfigBackupServer2Port()
{
  int ret = 0;

  char strval[7];

  ret = EJ_read_psm_item("backupServer2Port", strval, 7);
  if (ret != 0) {

    EJ_DebugPrintf(("[WifiModuleStatus.c][loadWifiConfigBackupServer2Port][DEBUG]: Error get backupServer2Port.\r\n"));
  }
  else {

    int port = atoi(strval);
		
    SetWifiConfigMasterServerPort(port);
  }
}




uint8_t GetWifiConfigDefaultConnectServer()
{
  return h_InfoManagement.wifiConfig.defaultConnectServer;
}

void SetWifiConfigDefaultConnectServer(uint8_t defaultConnectServer)
{

  h_InfoManagement.wifiConfig.defaultConnectServer = defaultConnectServer;

  storeWifiConfigDefaultConnectServer(h_InfoManagement.wifiConfig.defaultConnectServer);

}

uint8_t storeWifiConfigDefaultConnectServer(uint8_t defaultConnectServer)
{
  int ret = 0;

  char strval[7] = { '\0' };

  sprintf(strval, "%d", defaultConnectServer);

  ret = EJ_write_psm_item( "defaultConnectServer", strval);
  if (ret) {
    EJ_ErrPrintf(("[WifiModuleStatus.c][storeWifiConfigDefaultConnectServer][ERROR]: Error set defaultConnectServer.\r\n"));
  }

	return ret;
}

void loadWifiConfigDefaultConnectServer()
{
  int ret = 0;

  char strval[7];

  ret = EJ_read_psm_item( "defaultConnectServer", strval, 7);
  if (ret != 0) {

    EJ_DebugPrintf(("[WifiModuleStatus.c][loadWifiConfigDefaultConnectServer][DEBUG]: Error get defaultConnectServer.\r\n"));
  }
  else {

    int defaultConnectServer = atoi(strval);
		
    SetWifiConfigDefaultConnectServer(defaultConnectServer);
  }
}


uint8_t Init_InfoManagement()
{
  memset(&h_InfoManagement, 0, sizeof(InfoManagement));

  SetWifiModuleStatusSignalIntensity(NO_SIGNAL);
  SetWifiModuleStatusRouterStatus(ROUTER_NOT_CONNECTED);
  SetWifiModuleStatusCloudServiceStatus(CLOUD_NOT_CONNECTED);
  SetWifiModuleStatusConnectedStatus(NO_CONNECTED);
  SetWifiModuleStatusTcpConnectedCount(0x00);
  SetWifiModuleStatusIsHomeAPConfig(HOMEAP_NOT_CONFIGURED);
  SetWifiModuleStatusIsLanModuleRunning(LANMODULE_NOT_RUNNING);
  SetWifiModuleStatusIsUdpBroadcastRunning(UDPBROADCASTMODULE_NOT_RUNNING);
  SetWifiModuleStatusDeviceIDRequestStatus(DEVICEID_REQUEST_FAILED);
  SetWifiModuleStatusWorkMode(WIFIMODULE_UNKNOW_MODE);

  h_InfoManagement.wifiInfo.version.versionRule = 0x01;
  h_InfoManagement.wifiInfo.version.functionCode = FUNCTION_CODE;
  h_InfoManagement.wifiInfo.version.hardwareVersion = HARDWARE_VERSION;
  h_InfoManagement.wifiInfo.version.softwareVersionMsb = SOFTWARE_VERSION_MSB; 
  h_InfoManagement.wifiInfo.version.softwareVersionLsb = SOFTWARE_VERSION_LSB; 
  h_InfoManagement.wifiInfo.version.publishYear = PUBLISH_YEAR;
  h_InfoManagement.wifiInfo.version.publishWeek = PUBLISH_WEEK;

  h_InfoManagement.wifiInfo.type = MODULE_TYPE_WIFI;

#ifdef CONFIG_CPU_MW300
  h_InfoManagement.wifiInfo.model = MODULE_MODEL_CCD_U_MW300;
  SetWifiModuleInfoBrandStr("CCD");
  SetWifiModuleInfoModelStr("MW300");
#else
  h_InfoManagement.wifiInfo.model = MODULE_MODEL_CCD_U_MT7687;
  SetWifiModuleInfoBrandStr(STR_WIFI_MODULE_BRAND);
  SetWifiModuleInfoModelStr(STR_WIFI_MODULE_MODE);
#endif

  int ret = 0;

  loadInfoFromFlash();
	
  return 0x00;
}


static int ChangeStr2Hex(char s[],char bits[]) {
    int i,n = 0;
    for(i = 0; s[i]; i += 2) {
        if(s[i] >= 'A' && s[i] <= 'F')
            bits[n] = s[i] - 'A' + 10;
        else bits[n] = s[i] - '0';
        if(s[i + 1] >= 'A' && s[i + 1] <= 'F')
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'A' + 10);
        else bits[n] = (bits[n] << 4) | (s[i + 1] - '0');

		
        ++n;
    }
    return n;
}



uint8_t loadInfoFromFlash()
{	
  int ret = 0;

  char strval[44+1];

  ret = EJ_read_psm_item( "UUID", strval, 45);

  if (ret != 0) {

    EJ_DebugPrintf(("[WifiModuleStatus.c][loadInfoFromFlash][ERROR]: Error get deviceInfo SN.\r\n"));
  }
  else {
	
	
    SetDeviceInfoDeviceUuid(strval);
  }


  ret = EJ_read_psm_item( "Deviceid", strval,45);
  if (ret != 0) {

    EJ_DebugPrintf(("[WifiModuleStatus.c][loadInfoFromFlash][ERROR]: Error get deviceInfo manufacturers.\r\n"));
  }
  else {

	char bits[10];
	int  n = ChangeStr2Hex(strval, bits);
	int i =0;

	EJ_Printf ("loadInfoFromFlash  DID IS %s\r\n",strval);

    SetWifiStatusDeviceID(bits);
  }

  ret = EJ_read_psm_item( "manufacturers", strval, 8);
  if (ret != 0) {

    EJ_DebugPrintf(("[WifiModuleStatus.c][loadInfoFromFlash][ERROR]: Error get deviceInfo manufacturers.\r\n"));
  }
  else {
    SetDeviceInfoManufacturers(strval);
  }

  ret = EJ_read_psm_item( "deviceType", strval, 2);
  if (ret!=0) {

    EJ_DebugPrintf(("[WifiModuleStatus.c][loadInfoFromFlash][ERROR]: Error get devieType.\r\n"));
  }
  else {
    SetDeviceInfoDeviceType(strval[0]);
  }

  ret = EJ_read_psm_item( "deviceTypeString", strval, 8);
  if (ret!=0) {

    EJ_DebugPrintf(("[WifiModuleStatus.c][loadInfoFromFlash][ERROR]: Error get deviceInfo deviceTypeString.\r\n"));
  }
  else {
    SetDeviceInfoDeviceTypeString(strval);
  }



  loadWifiConfigConfigMode();

  loadWifiConfigRenameSSID();//SSIDÖØÃüÃû

  loadWifiConfigMasterServerAddr();//Ö÷µØÖ·

  loadWifiConfigMasterServerPort();

  loadWifiConfigBackupServer1Addr(); //±¸ÓÃµØÖ·1

  loadWifiConfigBackupServer1Port();

  loadWifiConfigBackupServer2Addr();

  loadWifiConfigBackupServer2Port();

  loadWifiConfigDefaultConnectServer();

  if (!GetWifiConfigMasterServerAddr()) {

    SetWifiConfigMasterServerAddr(MASTER_SERVER_ADDR);//ÉèÖÃÖ÷·þÎñÆ÷µØÖ·ºÍ¶Ë¿Ú

    SetWifiConfigMasterServerPort(MASTER_SERVER_PORT);
  }

	loadWifiConfigCloudDomainAddr();

  if (!GetWifiConfigCloudDomainAddr(CLOUD_DOMAIN_ADDR)) {

    SetWifiConfigCloudDomainAddr(CLOUD_DOMAIN_ADDR);

  }

  if (loadWifiModuleInfoVersion() != 0) {

    WIFIModuleVersion version;

    version.versionRule = 0x01;
    version.functionCode = FUNCTION_CODE;
    version.hardwareVersion = HARDWARE_VERSION;
    version.softwareVersionMsb = SOFTWARE_VERSION_MSB;
    version.softwareVersionLsb = SOFTWARE_VERSION_LSB;
    version.publishYear = PUBLISH_YEAR;
    version.publishWeek = PUBLISH_WEEK;

    SetWifiModuleInfoVersion(&version);
  }
	
  return 0x00;
}

bool DeviceInfo_save_uuid(char* value)
{
	
	int ret;


	if (EJ_write_psm_item( "uuid", value) != 0) {
		
		return false;
	}

	return true;
}

bool DeviceInfo_load_uuid(char* value)
{
	char psm_val[65];
	
	int ret;



	if (EJ_read_psm_item("uuid", psm_val, sizeof(psm_val)) != 0) {
		
		return false;
	}

	memcpy(value, psm_val, strlen(psm_val));

	
	return true;
}


uint8_t storeDeviceInfoDeviceUuid()
{
	int ret = 0;
	DeviceUuid id = GetDeviceInfoDeviceUuid();

	uint8_t newDeviceUuidStr[7] = {'\0'};

	sprintf(newDeviceUuidStr, "%s",id.uuid);
	
	ret = EJ_write_psm_item( "UUID", newDeviceUuidStr);
  	if (ret) {
    	EJ_ErrPrintf(("[WifiModuleStatus.c][storeDeviceInfoDeviceSN][ERROR]: Error set deviceinfo Uuid.\r\n"));
  	}else{
	    
		EJ_Printf("storeDeviceInfoDeviceUuid ======>%s\r\n",newDeviceUuidStr);
  }

	return 0;
}

uint8_t storeDeviceInfoDeviceid(uint8_t *Deviceid)
{
	int ret = 0;
	uint8_t temp[13]={0};

	sprintf(temp,"%02X%02X%02X%02X%02X%02X",Deviceid[0],Deviceid[1],Deviceid[2],Deviceid[3],Deviceid[4],Deviceid[5]);

	ret = EJ_write_psm_item( "Deviceid",temp);
	if (ret) {
    	EJ_ErrPrintf(("[WifiModuleStatus.c][storeDeviceInfoDeviceSN][ERROR]: Error set deviceinfo Uuid.\r\n"));
  	}else{
		EJ_Printf("storeDeviceInfoDeviceid ======>%s\r\n",temp);
  	}

	return ret;
}



uint8_t storeDeviceInfoNetwork()
{
	int ret = 0;
	
	return ret;
}

uint8_t storeDeviceInfoManufacturers()
{
  int ret = 0;

  char strval[7] = { '\0' };

  GetDeviceInfoManufacturers((uint8_t *)(strval));

  ret = EJ_write_psm_item( "manufacturers", strval);
  if (ret) {
    /*error*/
  }

  return 0x00;
}

uint8_t storeDeviceInfoDeviceType()
{
  int ret = 0;

  char strval[2] = { '\0' };

  strval[0] = GetDeviceInfoDeviceType();

  ret = EJ_write_psm_item( "deviceType", strval);
  if (ret) {
    EJ_Printf("Error set deviceinfo deviceType.\r\n");
  }

  return 0x00;
}




uint8_t storeInfoToFlash()
{

  return 0x00;
}

static int EJ_PROVISION_AP_NUM;
static int scan_cb(unsigned int count)
{
	EJ_PROVISION_AP_NUM = count;
	return 0;
}

void  GetWifiModuleStatusSignalStrength()
{

#if 0
	int i=0;
	char ssid[32] = {0};
	GetWifiConfigSSID(ssid, sizeof(ssid));
	EJ_PROVISION_AP_NUM = -1;
	wlan_scan(scan_cb);
	while(EJ_PROVISION_AP_NUM <0){
		EJ_thread_sleep(10);
	}
	struct wlan_scan_result res;
	
	for (i = 0; i < EJ_PROVISION_AP_NUM && i<20; i++) {
		wlan_get_scan_result(i, &res);
		if(!strcmp(res.ssid,ssid)){
			EJ_Printf("%s %d %d\r\n", res.ssid, res.channel,res.rssi);
			if(res.rssi>0)
				SetWifiModuleStatusSignalIntensity(SIGNAL_FOUR_GRADES);
			if(res.rssi>40)
				SetWifiModuleStatusSignalIntensity(SIGNAL_THREE_GRADES);
			if(res.rssi>60)
				SetWifiModuleStatusSignalIntensity(SIGNAL_TWO_GRADES);
			if(res.rssi>86)
				SetWifiModuleStatusSignalIntensity(SIGNAL_ONE_GRADES);

			break;
		}

	}

#else


	char curRSSI = 0;
	short rssi =-50;
	int snr;
	//wlan_get_current_signal_strength(&rssi, &snr);
	if(rssi < -90) {
		curRSSI = 0;
	} else if(rssi < -84) {
		curRSSI = 1;
	} else if(rssi < -76) {
		curRSSI = 2;
	} else {
		curRSSI = 3;
	}

	SetWifiModuleStatusSignalIntensity(curRSSI);


#endif

}

