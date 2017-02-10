#include  "ej_cmd_mqtt.h"
#include  "ej_port_wlan.h"
#include  "ej_event_manager.h"
#include  "ej_ota.h"


#define WIFIMODULE_SN_LENGTH 22
#define DEVICE_SN_LENGTH 22

//void fillDataIDToPacket(wifi2CloudPacket *pPacket, int dataID)
//{
//	pPacket->dataID[3] = (uint8_t)((dataID & 0xff000000) >> 24);
//	pPacket->dataID[2] = (uint8_t)((dataID & 0x00ff0000) >> 16);
//	pPacket->dataID[1] = (uint8_t)((dataID & 0x0000ff00) >> 8);
//	pPacket->dataID[0] = (uint8_t)((dataID & 0x000000ff) >> 0);
//}

wifi2CloudPacket * requestDeviceIDFromCloud(uint8_t *Mac, uint8_t *Uuid)
{
	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pPacket) {

		pPacket->head[0] = 0x5A;
		pPacket->head[1] = 0x5A;

		pPacket->version = 0x04;

		pPacket->crypt = 0x11;

		pPacket->dataType[0] = 0x15;
		pPacket->dataType[1] = 0x00;

		fillDataIDToPacket(pPacket, 0x55);

		/**/
		fillTimeStampToPackt(pPacket);

		/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
		memset(pPacket->deviceID, 0, 6);

		pPacket->data = (uint8_t *)EJ_mem_malloc(12 + 6);

		memcpy(pPacket->data, Mac, 12);
		memcpy(pPacket->data + 12, Uuid, 6);

		int datalen = 44 + (12 + 6);
		//ÕûÐÍ×ª×Ö½ÚÐÍ
		pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
		pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
		pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
		pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

		memset(pPacket->signature, 0xA5, 16);
	}

	return pPacket;
}


uint8_t Process_RequestDeviceIDFromCloudResponseCB(wifi2CloudPacket *pPacket)
{
	if (pPacket) {

		if (pPacket->data) {

			uint8_t result = pPacket->data[0];

			if (result == 0) {
				
				uint8_t tmp[6]={0};
				memcpy(tmp,(uint8_t *)(pPacket->data + 2),6);

				EJ_Printf("0x8015 report did = %02X %02X %02X %02X %02X %02X \r\n",tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
				

				SetWifiStatusDeviceID(tmp);

				storeDeviceInfoDeviceid(tmp);

				SetWifiModuleStatusDeviceIDRequestStatus(DEVICEID_REQUEST_SUCCESS);

				}else {

				EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_RequestDeviceIDFromCloudResponseCB][ERROR]: request deviceID failed.\r\n"));


			}
		}
	}

	return 0x00;
}


typedef struct {

	uint8_t Type;

	uint8_t Model;

	uint8_t workmode;

	uint8_t signalIntensity;

	uint8_t routerStatus;

	uint8_t cloudServiceStatus;

	uint8_t connectedStatus;

	uint8_t tcpConnectedCount;

	uint8_t ipaddr[4];

}PacketWifiModuleStatus;

wifi2CloudPacket * reportWifiModuleNetworkStateToCloud()
{

	PacketWifiModuleStatus moduleStatus;

	moduleStatus.Type = GetWifiModuleInfoType();
	moduleStatus.Model = GetWifiModuleInfoModel();
	moduleStatus.workmode = GetWifiModuleStatusWorkMode();
	moduleStatus.signalIntensity = GetWifiModuleStatusSignalIntensity();
	moduleStatus.routerStatus = GetWifiModuleStatusRouterStatus();
	moduleStatus.cloudServiceStatus = GetWifiModuleStatusCloudServiceStatus();
	moduleStatus.connectedStatus = GetWifiModuleStatusConnectedStatus();
	moduleStatus.tcpConnectedCount = GetWifiModuleStatusTcpConnectedCount();


	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pPacket) {

		pPacket->head[0] = 0x5A;
		pPacket->head[1] = 0x5A;

		pPacket->version = 0x04;

		pPacket->crypt = 0x11;

		pPacket->dataType[0] = 0x22;
		pPacket->dataType[1] = 0x00;

		fillDataIDToPacket(pPacket, 0x55);

		/**/
		fillTimeStampToPackt(pPacket);

		GetWifiStatusDeviceID(pPacket->deviceID);

		pPacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketWifiModuleStatus));

		if (pPacket->data) {

			memcpy(pPacket->data, (uint8_t *)&moduleStatus, sizeof(PacketWifiModuleStatus));

			int datalen = 44 + sizeof(PacketWifiModuleStatus);

			pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

			memset(pPacket->signature, 0xA5, 16);
		}
	}

	return pPacket;
}

uint8_t Process_WifiModuleNetworkStateRequestCB(wifi2CloudPacket *pPacket)
{
	wifi2CloudPacket *pResponsePacket = NULL;

	pResponsePacket = reportWifiModuleNetworkStateToCloud();

	/**/
	if (pResponsePacket) {

		if (nolock_list_push(GetWifi2cloudList(), pResponsePacket) != 0x01)
		{

		}
	}

	return 0x00;
	
}


wifi2CloudPacket *getSystemTimeFromCloud()
{
	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pPacket) {

		pPacket->head[0] = 0x5A;
		pPacket->head[1] = 0x5A;

		pPacket->version = 0x04;

		pPacket->crypt = 0x11;

		pPacket->dataType[0] = 0x41;
		pPacket->dataType[1] = 0x00;

		fillDataIDToPacket(pPacket, 0x55);

		/**/
		fillTimeStampToPackt(pPacket);

		GetWifiStatusDeviceID(pPacket->deviceID);

		pPacket->data = NULL;

		int datalen = 44;

		pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
		pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
		pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
		pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

		memset(pPacket->signature, 0xA5, 16);
	}

	return pPacket;
}

typedef struct {

	uint8_t sec;

	uint8_t min;

	uint8_t hour;

	uint8_t week;

	uint8_t day;

	uint8_t month;

	uint8_t year;

	uint8_t zone;
	
}PacketWifiModuleGetSystemTimeFromCloudResponse;

uint8_t Process_getSystemTimeFromCloudResponseCB(wifi2CloudPacket *pPacket)
{
	uint8_t ret = 0x00;
	uint8_t  pTimeArray[7]={0};
	if (pPacket) {

		if (pPacket->data) {

			if (pPacket->dataLen[0] == (44 + sizeof(PacketWifiModuleGetSystemTimeFromCloudResponse))) {
				 memset(pTimeArray,(int)pPacket->data,7);
				 EJ_Wlan_set_time(pTimeArray);
			}else {
			
				ret = 0x01;
			}
		}
	}

	return ret;
}

typedef struct {

	uint8_t functionCode;

	uint8_t hardwareVersion;

	uint8_t softwareVersionMsb;

	uint8_t softwareVersionLsb;

	uint8_t publishYear;

	uint8_t publishWeek;

}PacketWifiModuleUpgraeRequest;


typedef struct {
	
	uint8_t updateRule;

	uint8_t updateType;

	uint8_t ruleVersion;

	uint8_t functionCode;

	uint8_t hardwareVersion;

	uint8_t softwareVersionMsb;

	uint8_t softwareVersionLsb;

	uint8_t publishYear;

	uint8_t publishWeek;

	uint8_t totalFrame[4];

	uint8_t *httpAddr;

}PacketWifiModuleUpgraeResponse;

uint8_t CompareWifiModuleVersion(WIFIModuleVersion currentVersion, PacketWifiModuleUpgraeResponse *upgradeVersion)
{
	uint8_t ret = 0x00;

	if (currentVersion.hardwareVersion == upgradeVersion->hardwareVersion) {

		unsigned int currentTemp, upgradeTemp;

		currentTemp = (unsigned int)(currentVersion.softwareVersionMsb << 24 |
			currentVersion.softwareVersionLsb << 16 |
			currentVersion.publishYear << 8 |
			currentVersion.publishWeek);

		upgradeTemp = (unsigned int)(upgradeVersion->softwareVersionMsb << 24 |
			upgradeVersion->softwareVersionLsb << 16 |
			upgradeVersion->publishYear << 8 |
			upgradeVersion->publishWeek);

		if (currentTemp < upgradeTemp) {
			ret = 0x01;
		}

	}

	return ret;
}

wifi2CloudPacket * requestWifiModuleUpgradeInfoFromCloud()
{
	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	WIFIModuleVersion currentVersion =  GetWifiModuleInfoVersion();

	
	if (pPacket) {

		pPacket->head[0] = 0x5A;
		pPacket->head[1] = 0x5A;

		pPacket->version = 0x04;

		pPacket->crypt = 0x11;

		pPacket->dataType[0] = 0x50;
		pPacket->dataType[1] = 0x00;

		fillDataIDToPacket(pPacket, 0x55);

		/**/
		fillTimeStampToPackt(pPacket);

		GetWifiStatusDeviceID(pPacket->deviceID);

		pPacket->data = (uint8_t *)EJ_mem_malloc(sizeof(WIFIModuleVersion));

		if (pPacket->data) {

			memcpy(pPacket->data, (uint8_t *)&currentVersion, sizeof(WIFIModuleVersion));

			int datalen = 44 + sizeof(WIFIModuleVersion);

			pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

			memset(pPacket->signature, 0xA5, 16);
		}
	}

	return pPacket;
}

wifi2CloudPacket *reportWifiUpgradeInfoToCloud()
{
	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pPacket) {

		uint8_t dataLength = 45; //+ sizeof(WIFIModuleVersion) + 6 + 1 + strlen(GetWifiModuleInfoBrandStr()) + 1 + strlen(GetWifiModuleInfoModelStr());

		pPacket->data = (uint8_t *)EJ_mem_malloc(dataLength);

		if (pPacket->data) {

			pPacket->head[0] = 0x5A;
			pPacket->head[1] = 0x5A;

			pPacket->version = 0x04;

			pPacket->crypt = 0x11;

			pPacket->dataType[0] = 0x58;
			pPacket->dataType[1] = 0x80;

			fillDataIDToPacket(pPacket, 0x55);

			/**/
			fillTimeStampToPackt(pPacket);

			/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
			GetWifiStatusDeviceID(pPacket->deviceID);

			int datalen = 45;

			pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

			pPacket->data = 0x00;
			memset(pPacket->signature, 0xA5, 16);
			if (nolock_list_push(GetWifi2cloudList(), pPacket) != 0x01)
			{
				EJ_mem_free(pPacket->data);
				EJ_mem_free(pPacket);
				EJ_ErrPrintf(("[ej_cmd_mqtt.c][reportWifiUpgradeInfoToCloud][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
			}
				
		}
	}

	return pPacket;
}

uint8_t Process_GetWifiModuleUpgradeResponseCB(wifi2CloudPacket *pPacket)
{
	WIFIModuleVersion currentVersion = GetWifiModuleInfoVersion();

	PacketWifiModuleUpgraeResponse *upgradeVersion;

	if (pPacket) {

		char *httpAddr = NULL;
		httpAddr = (char *)(pPacket->data + 13);
	

		if (pPacket->data[0] == 0x02) {		
			upgradeVersion = (PacketWifiModuleUpgraeResponse *)pPacket->data;
			if (upgradeVersion->ruleVersion != 0x01) {
				EJ_ErrPrintf(("[Process_GetWifiModuleUpgradeResponseCB][ERROR]: version rule is not match.\r\n"));
			}

			uint8_t ret = 0x00;
			if (upgradeVersion->updateType == 1) {
				ret = 0x01;				
			}else if (upgradeVersion->updateType == 0) {
				ret = CompareWifiModuleVersion(currentVersion, upgradeVersion);
			}			

			if (ret && httpAddr != NULL) {
				if(CompareWifiModuleVersion(currentVersion, upgradeVersion) == 0x00){
					return 0x00;
				}else if(CompareWifiModuleVersion(currentVersion, upgradeVersion) == 0x01 && httpAddr != NULL){
					EJ_AlwaysPrintf(("[Process_GetWifiModuleUpgradeResponseCB][INFO]: version is not match.\r\n"));
				}

				EJ_Printf("wifi ota httpAddr: %s\r\n", httpAddr);
				EJ_Printf("version->hardwareVersion: %d.\r\n", currentVersion.hardwareVersion);	
				EJ_Printf("version->softwareVersionMsb: %d.\r\n", currentVersion.softwareVersionMsb);	
				EJ_Printf("version->softwareVersionLsb: %d.\r\n", currentVersion.softwareVersionLsb);	
				EJ_Printf("version->publishYear: %d.\r\n", currentVersion.publishYear);	
				EJ_Printf("version->publishWeek: %d.\r\n", currentVersion.publishWeek);
				
				/* upgrade software...*/
			//	struct partition_entry *p = rfget_get_passive_firmware();

				if (EJ_wifi_fota_task(httpAddr) == EJ_SUCCESS) {
					
					EJ_AlwaysPrintf(("[Process_GetWifiModuleUpgradeResponseCB][INFO]: Firmware update succeeded.\r\n"));

					WIFIModuleVersion version;
					version.versionRule = upgradeVersion->ruleVersion;
					version.hardwareVersion= upgradeVersion->hardwareVersion;
					version.softwareVersionMsb = upgradeVersion->softwareVersionMsb;
					version.softwareVersionLsb = upgradeVersion->softwareVersionLsb;
					version.publishYear = upgradeVersion->publishYear;
					version.publishWeek = upgradeVersion->publishWeek;
					version.functionCode = upgradeVersion->functionCode;

					SetWifiModuleInfoVersion(&version);

					#if 0
					wifi2CloudPacket *pReportWifiModuleInfo = reportWifiUpgradeInfoToCloud();
					EJ_PrintWifi2CloudPacket(pReportWifiModuleInfo,"wifi2CloudPacket:0x8058");
					
					if (nolock_list_push(GetWifi2cloudList(), pReportWifiModuleInfo) != 0x01)
					{
						EJ_ErrPrintf(("[MQTTThread.c][Init_MQTTThread][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
					}

					os_thread_sleep(500);
					#endif
				} else {
				
					EJ_ErrPrintf(("[Process_GetWifiModuleUpgradeResponseCB][INFO]: Firmware update failed.\r\n"));
				}

				}
			}

			
		}

	return 0x00;
}


typedef struct {

	uint8_t type;

	WIFIModuleVersion version;

	uint8_t mac[6];

	uint8_t brandStrLength;

	uint8_t * brandStr;

	uint8_t modelStrLength;

	uint8_t * modelStr;
	
}PacketWifiModuleInfoResponse;

wifi2CloudPacket *reportWifiModuleInfoToCloud()
{
	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pPacket) {

		uint8_t dataLength = 1 + sizeof(WIFIModuleVersion) + 6 + 1 + strlen(GetWifiModuleInfoBrandStr()) + 1 + strlen(GetWifiModuleInfoModelStr());

		pPacket->data = (uint8_t *)EJ_mem_malloc(dataLength);

		if (pPacket->data) {

			pPacket->head[0] = 0x5A;
			pPacket->head[1] = 0x5A;

			pPacket->version = 0x04;

			pPacket->crypt = 0x11;

			pPacket->dataType[0] = 0x51;
			pPacket->dataType[1] = 0x00;

			fillDataIDToPacket(pPacket, 0x55);

			/**/
			fillTimeStampToPackt(pPacket);

			/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
			GetWifiStatusDeviceID(pPacket->deviceID);

			int datalen = 44 + dataLength;

			pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

			PacketWifiModuleInfoResponse response;

			response.type = GetWifiModuleInfoType();
			response.version = GetWifiModuleInfoVersion();
					
			EJ_Wlan_get_mac_address_posix(response.mac);

			response.brandStr = GetWifiModuleInfoBrandStr();
			response.brandStrLength = strlen(response.brandStr);
			
			response.modelStr = GetWifiModuleInfoModelStr();
			response.modelStrLength = strlen(response.modelStr);
			
			memcpy(pPacket->data, (uint8_t *)&response, 14);

			pPacket->data[14] = response.brandStrLength;
			memcpy(pPacket->data + 15, response.brandStr, response.brandStrLength);

			pPacket->data[15 + response.brandStrLength] = response.modelStrLength;
			memcpy(pPacket->data + 15 + response.brandStrLength + 1, response.modelStr, response.modelStrLength);

			memset(pPacket->signature, 0xA5, 16);
		}
	}

	return pPacket;
}


uint8_t Process_ReportWifiModuleInfoToCloudRequest(wifi2CloudPacket *pPacket)
{
	wifi2CloudPacket *pResponsePacket = NULL;

	pResponsePacket = reportWifiModuleInfoToCloud();

	/**/
	if (pResponsePacket) {

		if (nolock_list_push(GetWifi2cloudList(), pResponsePacket) != 0x01)
		{
			EJ_ErrPrintf(("[Process_ReportWifiModuleInfoToCloudRequest][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
		}
	}else {

		EJ_ErrPrintf(("[Process_ReportWifiModuleInfoToCloudRequest][ERROR]: reportWifiModuleInfoToCloud failed.\r\n"));
	}

	return 0x00;
}

typedef struct {

	uint8_t reserved;

	uint8_t deviceBrandLength;

	uint8_t *deviceBrand;

	uint8_t deviceTypeLength;

	uint8_t *deviceType;

	uint8_t deviceModelLength;

	uint8_t *deviceModel;

	uint8_t deviceVersionLength;
	
	uint8_t *deviceVersion;

	DeviceSN sn;
	
}PacketWifiModuleGetDeviceInfoResponse;

#if 0
wifi2CloudPacket *reportDeviceInfoToCloud()
{
	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pPacket) {

		int dataLength = 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1 + strlen(GetDeviceInfoDeviceModelString()) + 1 + strlen(GetDeviceInfoDeviceTVersionString()) + 22;

		pPacket->data = (uint8_t *)EJ_mem_malloc(dataLength);

		if (pPacket->data) {

			pPacket->head[0] = 0x5A;
			pPacket->head[1] = 0x5A;

			pPacket->version = 0x04;

			pPacket->crypt = 0x11;

			pPacket->dataType[0] = 0x60;
			pPacket->dataType[1] = 0x00;

			fillDataIDToPacket(pPacket, 0x55);

			/**/
			fillTimeStampToPackt(pPacket);

			/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
			GetWifiStatusDeviceID(pPacket->deviceID);

			int datalen = 44 + dataLength;

			pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

			pPacket->data[0] = 0;
			
			pPacket->data[0 + 1] = strlen(GetDeviceInfoDeviceBrandString());
			memcpy(pPacket->data + 1 + 1, GetDeviceInfoDeviceBrandString(), strlen(GetDeviceInfoDeviceBrandString()));

			pPacket->data[0 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString())] = strlen(GetDeviceInfoDeviceTypeString());
			memcpy(pPacket->data + 0 + 1 + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1, GetDeviceInfoDeviceTypeString(), strlen(GetDeviceInfoDeviceTypeString()));

			pPacket->data[0 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString())] = strlen(GetDeviceInfoDeviceModelString());
			memcpy(pPacket->data + 0 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1, GetDeviceInfoDeviceModelString(), strlen(GetDeviceInfoDeviceModelString()));

			pPacket->data[0 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1 +  strlen(GetDeviceInfoDeviceTypeString())] = strlen(GetDeviceInfoDeviceTVersionString());
			memcpy(pPacket->data + 0 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1, GetDeviceInfoDeviceTVersionString(), strlen(GetDeviceInfoDeviceTVersionString()));

			//DeviceSN sn = GetDeviceInfoDeviceSN();
			DeviceUuid id = GetDeviceInfoDeviceUuid();

			uint8_t *data = (uint8_t *)&id;

			memcpy(pPacket->data + 0 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1 + strlen(GetDeviceInfoDeviceTVersionString()), data, 6);

			memset(pPacket->signature, 0xA5, 16);
		}else {

			EJ_mem_free(pPacket);

			pPacket = NULL;
		}
		
	}

	return pPacket;
}
#else

wifi2CloudPacket *reportDeviceInfoToCloud()
{
	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pPacket) {

		int dataLength = 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1 + strlen(GetDeviceInfoDeviceModelString()) + 1 + strlen(GetDeviceInfoDeviceTVersionString()) + 22;

		pPacket->data = (uint8_t *)EJ_mem_malloc(dataLength);

		if (pPacket->data) {

			pPacket->head[0] = 0x5A;
			pPacket->head[1] = 0x5A;

			pPacket->version = 0x04;
			pPacket->crypt = 0x11;
			pPacket->dataType[0] = 0x60;
			pPacket->dataType[1] = 0x00;

			fillDataIDToPacket(pPacket, 0x55);

			/**/
			fillTimeStampToPackt(pPacket);

			/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
			GetWifiStatusDeviceID(pPacket->deviceID);

			int datalen = 44 + dataLength;

			pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

			pPacket->data[0] = 0;

			DeviceUuid id = GetDeviceInfoDeviceUuid();
			uint8_t *data = (uint8_t *)&id;
			memcpy(pPacket->data + 1, data, 6);

			//品牌名长度以及名称(7,8~n)
			pPacket->data[7] = strlen(GetDeviceInfoDeviceBrandString());
			memcpy(pPacket->data + 7 + 1, GetDeviceInfoDeviceBrandString(), strlen(GetDeviceInfoDeviceBrandString()));

			//类型名长度以及名称(n+1) (n+2~m)
			pPacket->data[7 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString())] = strlen(GetDeviceInfoDeviceTypeString());
			memcpy(pPacket->data + 7 + 1 + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1, GetDeviceInfoDeviceTypeString(), strlen(GetDeviceInfoDeviceTypeString()));

			//型号名长度以及名称(m+1) (m+2~k)
			pPacket->data[7 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString())] = strlen(GetDeviceInfoDeviceModelString());
			memcpy(pPacket->data + 7 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1, GetDeviceInfoDeviceModelString(), strlen(GetDeviceInfoDeviceModelString()));

			//家电版本信息(k+1~x)
			pPacket->data[7 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1 +  strlen(GetDeviceInfoDeviceTypeString())] = strlen(GetDeviceInfoDeviceTVersionString());
			memcpy(pPacket->data + 7 + 1 + 1 + strlen(GetDeviceInfoDeviceBrandString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1 + strlen(GetDeviceInfoDeviceTypeString()) + 1, GetDeviceInfoDeviceTVersionString(), strlen(GetDeviceInfoDeviceTVersionString()));

			memset(pPacket->signature, 0xA5, 16);
		}else {

			EJ_mem_free(pPacket);
			pPacket = NULL;
		}
		
	}

	return pPacket;
}
#endif

typedef struct {

	uint8_t defaultCloudServiceAddr;

	uint8_t needModifyCloudServiceAddr;

	uint8_t cloudServiceAddrIP[8];

	uint8_t *dominName;

}PacketWifiModuleCloudAddrModifyRequest;


uint8_t Process_ModifyCloudServiceAddrRequest(wifi2CloudPacket *pPacket)
{
	if (pPacket) {

		if ((pPacket->data) && (pPacket->dataLen[0] > (44 + 12))) {

			PacketWifiModuleCloudAddrModifyRequest request;

			request.defaultCloudServiceAddr = pPacket->data[0];
			request.needModifyCloudServiceAddr = pPacket->data[1];			
			memcpy(request.cloudServiceAddrIP, pPacket->data + 2, 4);

			request.dominName = (uint8_t *)EJ_mem_malloc(pPacket->dataLen[0] - 44 - 6);
			memcpy(request.dominName, pPacket->data + 7, pPacket->dataLen[0] - 44 - 6);

			EJ_Printf("defaultCloudServiceAddr: %d\r\n", request.defaultCloudServiceAddr);
			EJ_Printf("needModifyCloudServiceAddr: %d\r\n", request.needModifyCloudServiceAddr);
			return 0;
		}else {
			EJ_Printf("[Process_ModifyCloudServiceAddrRequest][ERROR]: data or dataLen error.\r\n");
			return 1;
		}
	}


	return 0;
}

typedef struct {

	uint8_t response;
	
}PacketWifiModuleRebootResponse;

typedef struct {

	uint8_t reserved;
	
}PacketWifiModuleRebootRequest;


uint8_t Process_WifiModuleRebootRequest(wifi2CloudPacket *pPacket)
{
	wifi2CloudPacket *pResponsePacket = NULL;

	pResponsePacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pResponsePacket) {
		
		pResponsePacket->head[0] = 0x5A;
		pResponsePacket->head[1] = 0x5A;

		pResponsePacket->version = 0x04;
		pResponsePacket->crypt = 0x11;
		pResponsePacket->dataType[0] = 0xB1;
		pResponsePacket->dataType[1] = 0x80;

		fillDataIDToPacket(pResponsePacket, 0x55);

		/**/
		fillTimeStampToPackt(pResponsePacket);
		GetWifiStatusDeviceID(pResponsePacket->deviceID);
		PacketWifiModuleRebootResponse response;
		response.response = 0x00;
		pResponsePacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketWifiModuleRebootResponse));

		if (pResponsePacket->data) {
			memcpy(pResponsePacket->data, (uint8_t *)&response, sizeof(PacketWifiModuleRebootResponse));
			if (nolock_list_push(GetWifi2cloudList(), pResponsePacket) != 0x01)
			{
				EJ_ErrPrintf(("[Process_WifiModuleRebootRequest][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
			}
		}

	}

	return 0x00;
}

typedef struct {

	uint8_t reserved;
	
}PacketWifiModuleResetToFactoryRequest;

typedef struct {

	uint8_t response;

}PacketWifiModuleResetToFactoryResponse;


uint8_t Process_WifiModuleResetToFactoryRequest(wifi2CloudPacket *pPacket)
{
	wifi2CloudPacket *pResponsePacket = NULL;

	pResponsePacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pResponsePacket) {
		
		pResponsePacket->head[0] = 0x5A;
		pResponsePacket->head[1] = 0x5A;

		pResponsePacket->version = 0x04;
		pResponsePacket->crypt = 0x11;
		pResponsePacket->dataType[0] = 0xB2;
		pResponsePacket->dataType[1] = 0x80;

		fillDataIDToPacket(pResponsePacket, 0x55);

		/**/
		fillTimeStampToPackt(pResponsePacket);
		GetWifiStatusDeviceID(pResponsePacket->deviceID);
		PacketWifiModuleResetToFactoryResponse response;

		response.response = 0x00;
		pResponsePacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketWifiModuleResetToFactoryResponse));

		if (pResponsePacket->data) {
			memcpy(pResponsePacket->data, (uint8_t *)&response, sizeof(PacketWifiModuleResetToFactoryResponse));
			if (nolock_list_push(GetWifi2cloudList(), pResponsePacket) != 0x01)
			{
				EJ_ErrPrintf(("[Process_WifiModuleRebootRequest][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
			}
		}

	}

	return 0x00;
}


typedef struct {

	uint8_t mode;

	uint8_t deviceSN[22];

	uint8_t ssidLength;

	uint8_t *ssid;

	uint8_t passwordLength;

	uint8_t *password;
	
}PacketWifiModuleConfigRequest;

typedef struct {

	uint8_t status;

	uint8_t errCode;

}PacketWifiModuleConfigResponse;

void Process_WifiModuleConfigRequest(wifi2CloudPacket *pPacket)
{
	if (pPacket) {

		if (pPacket->data) {

			EJ_WifiModuleConfig config;
			config.ssidLength = pPacket->data[23];
			memcpy(config.ssid, pPacket->data + 24, config.ssidLength);
			config.passwordLength = pPacket->data[24 + config.ssidLength];
			memcpy(config.password, pPacket->data + 25 + config.ssidLength, config.passwordLength);

			EJ_Wlan_set_network(&config);	
			EJ_Wlan_sta_start();


			EJ_mem_free(config.ssid);
			EJ_mem_free(config.password);

			/* send response.*/
			PacketWifiModuleConfigResponse configResponse;

			configResponse.status = 0;
			configResponse.errCode = 0;

			wifi2CloudPacket *pResponsePacket = NULL;
			pResponsePacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

			if (pResponsePacket) {

				pResponsePacket->head[0] = 0x5A;
				pResponsePacket->head[1] = 0x5A;

				pResponsePacket->version = 0x04;
				pResponsePacket->crypt = 0x11;

				pResponsePacket->dataType[0] = 0x90;
				pResponsePacket->dataType[1] = 0x80;

				memcpy(pResponsePacket->dataID, pPacket->dataID, 4);

				int datalen = 44 + sizeof(PacketWifiModuleConfigResponse);

				pResponsePacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
				pResponsePacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
				pResponsePacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
				pResponsePacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

				pResponsePacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketWifiModuleConfigResponse));

				memcpy(pResponsePacket->data, (uint8_t *)&configResponse, sizeof(PacketWifiModuleConfigResponse));

				/* add this packet to wifi2lan list.*/
				if (nolock_list_push(GetWifi2lanList(), pResponsePacket) != 0x01)
				{
					EJ_ErrPrintf(("[Process_WifiModuleConfigRequest][ERROR]: add packet to wifi2udplist failed.\r\n"));
				}

				EJ_thread_sleep(EJ_msec_to_ticks(1000));

				/* if wifi uap is running, then shutdown it. */
				if (EJ_Wlan_is_uap_started()) {
					
					//app_uap_stop();
					EJ_Wlan_uap_stop();
				}
			}

		}
	}
}


typedef struct {

	uint8_t deviceType[3];

	uint8_t deviceID[6];

}PakcetWifiModuleDiscoverRequest;

typedef struct {

	uint8_t deviceUuid[10];

	uint8_t did[10];

	uint8_t mac[20];

	uint8_t ipaddr[20];
	
}PakcetWifiModuleDiscoverResponse;


#if 1

void FillWifiModuleResponseDataPacket(int udpBroadcastType, wifi2AppPacket *pPacket,char *responseData)
{
	PakcetWifiModuleDiscoverResponse discoverResponse;
	memset(&discoverResponse,0,sizeof(PakcetWifiModuleDiscoverResponse));

	int numTest=1;
	numTest = strsplinum(pPacket->data,":"); 
	char *arrRecvdata[numTest]; 
	split(arrRecvdata,pPacket->data,":");

	if(udpBroadcastType==3)
	{

		// UUID,MAC,Beacon
		//strcpy(discoverResponse.deviceUuid,arrRecvdata[0]);
		//sprintf(discoverResponse.deviceUuid,"%s",arrRecvdata[0]);
		//SetDeviceInfoDeviceUuid(discoverResponse.deviceUuid);
		//storeDeviceInfoDeviceUuid();
	}

	if(udpBroadcastType==2)
	{

				//SSID,PWD,UUID,beacon

				EJ_WifiModuleConfig config;

				memset(&config, 0, sizeof(config));

				config.ssidLength = strlen(*(arrRecvdata))-1;

				memcpy(config.ssid, *(arrRecvdata), config.ssidLength);

				config.passwordLength = strlen(*(arrRecvdata+1))-1;

				memcpy(config.password,*(arrRecvdata+1) , config.passwordLength);

				EJ_Wlan_set_network(&config);
			
				EJ_Wlan_sta_start();
		
	}

	

	if(udpBroadcastType==1)
	{
		
		//IP,Port,UUID,beacon
		
	}

	//EJ_Printf("uuid set packet data2 is = %s\r\n",discoverResponse.deviceUuid);

	//GetWifiStatusDeviceUuid(discoverResponse.deviceUuid);
	//EJ_Printf("uuid set packet data is = %s\r\n",discoverResponse.deviceUuid);
	
	//did
	GetWifiStatusDeviceID(discoverResponse.did);

	//mac
	EJ_Wlan_get_mac_address(discoverResponse.mac);
	//GetStationNetworkMacaddr(discoverResponse.mac);
	
	//ipaddr
	EJ_Wlan_get_ip_address(discoverResponse.ipaddr);
	//GetStationNetworkIPaddr(discoverResponse.ipaddr);
	//Beacon == 0xFF
	//sprintf(discoverResponse->beacon ,"%s","0XFF");
	switch(udpBroadcastType)
	{

		case 1:
			
			sprintf(responseData,"%s,%s,%s,%s",discoverResponse.deviceUuid,discoverResponse.did,discoverResponse.mac,discoverResponse.ipaddr);
			break;

//			case 2:
//				sprintf(responseData,"%s,%s,%s",discoverResponse.deviceUuid,discoverResponse.did,discoverResponse.mac);
//				break;

//			case 3:
//				sprintf(responseData,"%s,%s,%s,%s","xxxxxa",discoverResponse.did,discoverResponse.mac,discoverResponse.ipaddr);
//				break;

		default:
			break;

	}
			
	EJ_Printf("%s responseData = %s\r\n",__FUNCTION__,responseData);
		
}
			
void FillWifiModuleResponseHeadPacket(int udpBroadcastType,wifi2AppPacket *pPacket, wifi2AppPacket *pResponsePacket)
{
			
	// head:   cmd , version,type,did,msgID,Time,Lenth,
	//content:	SSID,PWD,UUID,Beacon
	// head:   cmd , version,type,did,msgID,Time,Lenth,
	memcpy(pResponsePacket->cmd, "Response", strlen("Response"));
	memcpy(pResponsePacket->version, "2.0", strlen("2.0"));
	memcpy(pResponsePacket->PacketType,pPacket->PacketType, strlen(pPacket->PacketType));

	//DeviceUuid id = GetDeviceInfoDeviceUuid();
	
	GetWifiStatusDeviceID(pResponsePacket->deviceID);	
	//pResponsePacket->PacketID = "Request";
	memcpy(pResponsePacket->PacketID, pPacket->PacketID, strlen(pPacket->PacketID));
   //current time
	unsigned int  time;
	time = EJ_time_get_posix();
	EJ_Printf("EJ_time_get_posix is %d\r\n",time);
	ej_itoa(time,pResponsePacket->timeStamp,10);	
	

}

void WifiModuleUdpBroadcastRequest(int udpBroadcastType,wifi2AppPacket *pPacket)
{
	if (pPacket) {

		wifi2AppPacket *pResponsePacket = NULL;
		pResponsePacket = (wifi2AppPacket *)EJ_mem_malloc(sizeof(wifi2AppPacket));
		if (pResponsePacket) {
			memset(pResponsePacket,0,sizeof(wifi2AppPacket));

			// 填充头部
			EJ_Printf("%s-->%d  FillWifiModuleResponse   HeadPacket !\r\n",__FUNCTION__,__LINE__);
			FillWifiModuleResponseHeadPacket(udpBroadcastType,pPacket,pResponsePacket);


			//  填充数据部
			uint8_t responseData[128]={0};
			EJ_Printf("%s-->%d  FillWifiModuleResponse   DataPacket !\r\n",__FUNCTION__,__LINE__);
			FillWifiModuleResponseDataPacket(udpBroadcastType,pPacket,responseData);	
			
			pResponsePacket->data = (uint8_t *)EJ_mem_malloc(strlen(responseData));
			if(pResponsePacket->data)
			{
					
					memset(pResponsePacket->data,0,strlen(responseData));	
					sprintf(pResponsePacket->dataLen, "%s", strlen(responseData));

					strcpy(pResponsePacket->data,responseData);
					//sprintf(pResponsePacket->data, "%s", responseData);
					EJ_Printf("===responseData %d,content is %s\r\n",strlen(pResponsePacket->data),pResponsePacket->data);

			}else{
					EJ_mem_free(pResponsePacket);
					EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_WifiModuleDiscoverRequest][ERROR]: EJ_mem_malloc responseData failed.\r\n"));
					return ;
			}
			// 加入发送 队列
		   	/* add this packet to wifi2lan list.*/
			if (nolock_list_push(GetWifi2udpList(), pResponsePacket) != 0x01)
			{
				EJ_mem_free(pResponsePacket->data);
				EJ_mem_free(pResponsePacket);
				EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_WifiModuleDiscoverRequest][ERROR]: add packet to wifi2udplist failed.\r\n"));
			}else{

				EJ_Printf("push a Wifi2udp message!\r\n");
			}

		}

		}

}

void Process_Wifi2AppPacket(wifi2AppPacket *pPacket)
{
	uint8_t  typePacket = GetUdpPacketType(pPacket->PacketType);
	WifiModuleUdpBroadcastRequest(typePacket,pPacket);
}




//void Process_WifiModuleDiscoverRequest(wifi2AppPacket *pPacket)
//{
//	if (pPacket) {



//		wifi2AppPacket *pResponsePacket = NULL;

//		pResponsePacket = (wifi2AppPacket *)EJ_mem_malloc(sizeof(wifi2AppPacket));

//		memset(pResponsePacket,0,sizeof(wifi2AppPacket));


//		if (pResponsePacket) {
//			PakcetWifiModuleDiscoverResponse discoverResponse;
//			memset(&discoverResponse,0,sizeof(PakcetWifiModuleDiscoverResponse));
//			
//			


//			//pResponsePacket.cmd = "Response";
//			memcpy(pResponsePacket->cmd, "Response", strlen("Response"));

//			//pResponsePacket->version = "2.0";
//			memcpy(pResponsePacket->version, "2.0", strlen("2.0"));
//			//pResponsePacket->crypt = 0x11;
//			//pResponsePacket.PacketType = "T_call";
//			memcpy(pResponsePacket->PacketType, "T_call", strlen("T_call"));
//			DeviceUuid id = GetDeviceInfoDeviceUuid();
//			memcpy(pResponsePacket->deviceID, &id.uuid, 6);

//			//pResponsePacket->PacketID = "Request";
//			memcpy(pResponsePacket->PacketID, "Request", strlen("Request"));

//			time_t time;
//			time = wmtime_time_get_posix();
//			EJ_Printf("wmtime_time_get_posix is %d\r\n",time);
//			ej_itoa(time,pResponsePacket->timeStamp,32);

//			int datalen = sizeof(PakcetWifiModuleDiscoverResponse);
//			ej_itoa(datalen,pResponsePacket->dataLen,32);


//			PacketWifiModuleInfoResponse response;
//			memset(&response,0,sizeof(response));

//	
//				memset(discoverResponse.deviceUuid,0,sizeof(discoverResponse.deviceUuid));
//				memcpy(discoverResponse.deviceUuid,"123456",strlen("123456"));
//		
//			
//			EJ_Printf("discoverResponse.deviceUuid = %s\r\n",discoverResponse.deviceUuid);
//			 //memcpy(discoverResponse.deviceUuid, h_InfoManagement.wifiStatus.deviceID, 6);
//					
//			wlan_get_mac_address(response.mac);
//			
//			//memcpy(discoverResponse.mac, "11:22:33:44:55:66", strlen("11:22:33:44:55:66"));
//			//memcpy(discoverResponse.mac, _g_dev->mac,6);
//			GetStationNetworkMacaddr(discoverResponse.mac);
//			
//			char *ip = GetStationNetworkIpaddr();
//			memset(discoverResponse.ipaddr,0,20);
//			//memcpy(discoverResponse.ipaddr,ip, strlen(ip));
//			memcpy(discoverResponse.ipaddr,"192.168.2.103", strlen("192.168.2.103"));
//			//sprintf(discoverResponse.ipaddr,"%s",ip);
//			EJ_Printf("Get Station IP is %s\r\n",discoverResponse.ipaddr);

//			memcpy(discoverResponse.did,"666666",strlen("666666"));
//		
//			uint8_t  dataTmp[256]={0};
//			sprintf(dataTmp,"%s,%s,%s,%s",discoverResponse.deviceUuid,discoverResponse.did,discoverResponse.mac,discoverResponse.ipaddr);
//			//EJ_Printf("===dataTmp is %d,content is %s\r\n",strlen(dataTmp),dataTmp);
//			pResponsePacket->data = (uint8_t *)EJ_mem_malloc(strlen(dataTmp));
//			memset(pResponsePacket->data,0,strlen(dataTmp));
//			
//			//memcpy(pResponsePacket->data, dataTmp, strlen(dataTmp));
//			sprintf(pResponsePacket->data, "%s", dataTmp);
//			EJ_Printf("===dataTmp 11111is %d,content is %s\r\n",strlen(pResponsePacket->data),dataTmp);



//		   			/* add this packet to wifi2lan list.*/
//			if (nolock_list_push(GetWifi2udpList(), pResponsePacket) != 0x01)
//			{
//				EJ_ErrPrintf(("[MQTTCommands.c][Process_WifiModuleDiscoverRequest][ERROR]: add packet to wifi2udplist failed.\r\n"));
//			}else{

//				EJ_DebugPrintf(("push a Wifi2udp message!\r\n"));
//			}

//			EJ_PutEventSem(EJ_EVENT_broadcastRequestSem);
//		}



//		}



//}
#endif

typedef struct {

	uint8_t updateRule;

	uint8_t updateType;

	uint8_t deviceVersion;

	uint8_t totalFrame[4];

	uint8_t framemd5[16];

	uint8_t *httpAddr;

	
}PacketGetDeviceInfo2CloudResponse;

uint8_t CompareDeviceVersion(DeviceVersion currentVersion,PacketGetDeviceInfo2CloudResponse *upgradeVersion)
{
	uint8_t ret;

	if(currentVersion.versionRule == upgradeVersion->updateRule)
	{
		unsigned int currentTemp, upgradeTemp;

		//currentTemp = (unsigned int)(currentVersion.publishYear|currentVersion.publishWeek);

		//upgradeTemp = (unsigned int)(upgradeVersion->version.publishYear|upgradeVersion->version.publishWeek);
		if(currentTemp == upgradeTemp)
		{
			ret = 0x00;
		}else if(currentTemp < upgradeTemp)
		{
			ret = 0x01;
		}else if(currentTemp > upgradeTemp)
		{
			ret = 0x02;
		}
	
	}
	return ret;
}
#if 0
uint8_t Process_QueryDeviceFirmwareVersionRequest(wifi2CloudPacket *pPacket)
{
	if (pPacket) {

		wifi2CloudPacket *pResponsePacket = NULL;

		pResponsePacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

		if (pResponsePacket) {

			pResponsePacket->head[0] = 0x5A;
			pResponsePacket->head[1] = 0x5A;

			pResponsePacket->version = 0x04;

			pResponsePacket->crypt = 0x11;
	
			pResponsePacket->dataType[0] = 0x61;
			pResponsePacket->dataType[1] = 0x80;

			//fillDataIDToPacket(pPacket, 0x55);
			memcpy(pResponsePacket->dataID, pPacket->dataID, 4);

			/**/
			fillTimeStampToPackt(pResponsePacket);

			GetWifiStatusDeviceID(pResponsePacket->deviceID);

			uint8_t * version = GetDeviceInfoDeviceTVersionString();

			if (version == NULL) {
				EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_QueryDeviceFirmwareVersionRequest][ERROR]: version is empty."));
				
			}

			uint8_t size = version[0];

			int datalen = 44 /*+ size*/;

			pResponsePacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pResponsePacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pResponsePacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pResponsePacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);
			pResponsePacket->data = NULL;
			/*
			pResponsePacket->data = (uint8_t *)EJ_mem_malloc(size);
			
			if (size > 0) {
				memcpy(pResponsePacket->data, version + 1, size);
			}
			*/
			/* add this packet to wifi2cloud list.*/
			if (nolock_list_push(GetWifi2cloudList(), pResponsePacket) != 0x01)
			{
				EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_QueryDeviceFirmwareVersionRequest][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
			}
		}else {

			EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_QueryDeviceFirmwareVersionRequest][ERROR]:EJ_mem_malloc failed.\r\n"));
		}

	}

	return 0x00;
}
#else

//模块把云端下发的设备固件信息发送给家电板
uint8_t Process_QueryDeviceFirmwareVersionRequest(wifi2CloudPacket *pPacket)
{

	if (pPacket) {
		uart2WifiPacket *pUart2WifiPacket = NULL;	
		pUart2WifiPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

		if (pPacket->data) {

			FillCommonUart2WifiPacket(pUart2WifiPacket, 0x61, UART2WIFIPACKET_FIXED_LENGTH + 13, DATA_WITH_NO_CRYPT | 0);
			pUart2WifiPacket->data= (uint8_t *)EJ_mem_malloc(13);
			if(pUart2WifiPacket->data){
				memcpy(pUart2WifiPacket->data,pPacket->data,13);

				if (nolock_list_push(GetWifi2deviceList(), pUart2WifiPacket) != 0x01)
				{
					EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_WifiModuleDiscoverRequest][ERROR]: add packet to wifi2udplist failed.\r\n"));
					EJ_PacketUartFree(pUart2WifiPacket);
					pUart2WifiPacket = NULL;
				}
			}

		}else{

			EJ_PacketUartFree(pUart2WifiPacket);
			pUart2WifiPacket = NULL;

		}
		
	}

	return 0x01;
}

#endif

wifi2CloudPacket *ReportDeviceUpgradeInfoCloud(uint8_t data)
{
	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));
	pPacket->data = (uint8_t *)EJ_mem_malloc(4);

	if(pPacket)
	{
		pPacket->head[0] = 0x5A;
		pPacket->head[1] = 0x5A;

		pPacket->version = 0x04;
		pPacket->crypt = 0x11;

		pPacket->dataType[0] = 0x63;
		pPacket->dataType[1] = 0x80;

		//fillDataIDToPacket(pPacket, dataID);
		fillDataIDToPacket(pPacket, 0x55);
		fillTimeStampToPackt(pPacket);
		GetWifiStatusDeviceID(pPacket->deviceID);

		int datalen = 45;
		pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
		pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
		pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
		pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

		pPacket->data[0] = data;
		memset(pPacket->signature, 0xA5, 16);
	}

	return pPacket;
}


static int upgrade_crc_check(unsigned int sumlength)
{
	return 1;

}

#define Data2 2

void Process_DeviceFirmwareVersionUpdateRequest(wifi2CloudPacket *pPacket)
{
	if (pPacket) {
		
		char *httpAddr = NULL;
		httpAddr = (char *)(pPacket->data + 35);		
		EJ_Printf("device ota httpAddr: %s\r\n", httpAddr);

		uint32_t size = (uint32_t)(pPacket->data[15] | pPacket->data[16] << 8 | pPacket->data[17] << 16 | pPacket->data[18] << 24);	
		EJ_Printf("size :[%X]\r\n",size);
		
		if (pPacket->data) {
			int readCount = 0;

			char *version = EJ_mem_malloc(1);
			version = (char *)(pPacket->data + 2);
			
			/* down the url_str image and write the flash addr to temp.*/
			if(!EJ_device_firmware_download(httpAddr, size))
			{
				
				//Compare bin file CRC
				if(upgrade_crc_check(size))
				{
					//EJ_Printf("size :[7777777777777777777777777]\r\n");
					/* make an uart commands to query whether need to update. */
					uart2WifiPacket * pRequestPacket = queryDeviceUpdate((size - 4), version);
					EJ_PrintUart2WifiPacket(pRequestPacket,"queryDeviceUpdate 0x62 :wifi2UartPacket");
					/* add this packet to wifi2cloud list.*/
					if (nolock_list_push(GetWifi2deviceList(), pRequestPacket) != 0x01)
					{
						EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_QueryDeviceFirmwareVersionRequest][ERROR]: add packet to wifi2devicelist failed.\r\n"));
					}
				}else{
				
					wifi2CloudPacket *pReportDeviceUpgradeInfo = ReportDeviceUpgradeInfoCloud(Data2);
					EJ_PrintWifi2CloudPacket(pReportDeviceUpgradeInfo,"0x62:wifi2CloudPacket:0x8063");
					
					if (nolock_list_push(GetWifi2cloudList(), pReportDeviceUpgradeInfo) != 0x01)
					{
						EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_QueryDeviceFirmwareVersionRequest][ERROR]: add packet to wifi2cloudlist failed.\r\n"));
					}	
				}
			}
			
		}
	}
}


wifi2CloudPacket * responseHeartBeatToClient(int dataID)
{
	wifi2CloudPacket *pPacket = NULL;

	pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pPacket) {

		pPacket->head[0] = 0x5A;
		pPacket->head[1] = 0x5A;

		pPacket->version = 0x04;
		pPacket->crypt = 0x11;

		pPacket->dataType[0] = 0x7B;
		pPacket->dataType[1] = 0x80;

		fillDataIDToPacket(pPacket, dataID);

		/**/
		fillTimeStampToPackt(pPacket);

		/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
		GetWifiStatusDeviceID(pPacket->deviceID);

		int datalen = 44;
		pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
		pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
		pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
		pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

		pPacket->data = NULL;
		memset(pPacket->signature, 0xA5, 16);
	}

	return pPacket;
}

void Process_heartBeatRequest(wifi2CloudPacket *pPacket)
{
	if (pPacket) {

		int LANCommandDataID = (pPacket->dataID[0] | (pPacket->dataID[1] << 8) | (pPacket->dataID[2] << 16) | (pPacket->dataID[3] << 24));

		wifi2CloudPacket * pResponsePacket = responseHeartBeatToClient(LANCommandDataID);

		memcpy(pResponsePacket->dataID, pPacket->dataID, 4);

		if (pResponsePacket) {

			if (nolock_list_push(GetWifi2lanList(), pResponsePacket) != 0x01)
			{
		
				EJ_ErrPrintf(("[ej_cmd_mqtt.c][Process_heartBeatRequest][ERROR]: add packet to wifi2lanlist failed.\r\n"));
			}
		}
		
	}
}

void initMQTTCommands()
{
	registerMQTTCommandCallback(0x8015, Process_RequestDeviceIDFromCloudResponseCB); //回复设备ID
	registerMQTTCommandCallback(0x8022, Process_WifiModuleNetworkStateRequestCB); //WiFi状态回复
	registerMQTTCommandCallback(0x8041, Process_getSystemTimeFromCloudResponseCB); //系统时间回复
	registerMQTTCommandCallback(0x0058, Process_GetWifiModuleUpgradeResponseCB); //推送WiFi升级包回复
	registerMQTTCommandCallback(0x8051, Process_ReportWifiModuleInfoToCloudRequest);//模块信息上报
	registerMQTTCommandCallback(0x0061, Process_QueryDeviceFirmwareVersionRequest);//0x61:服务器请求查询设备版本
	registerMQTTCommandCallback(0x0062, Process_DeviceFirmwareVersionUpdateRequest);
	registerMQTTCommandCallback(0x0070, Process_ModifyCloudServiceAddrRequest);
	registerMQTTCommandCallback(0x007B, Process_heartBeatRequest);	
	registerMQTTCommandCallback(0x0090, Process_WifiModuleConfigRequest);
	registerMQTTCommandCallback(0x80B1, Process_WifiModuleRebootRequest);
	registerMQTTCommandCallback(0x80B2, Process_WifiModuleResetToFactoryRequest);
	//registerMQTTCommandCallback(0x00E1, Process_WifiModuleDiscoverRequest);	
	
}




