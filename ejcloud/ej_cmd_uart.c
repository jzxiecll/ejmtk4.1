#include "ej_user_time.h"
#include "ej_cmd_uart.h"
#include "WifiModuleStatus.h"



#define UART2WIFICOMMANDS_READ_QOS_REQUEST	0x05
#define UART2WIFICOMMANDS_READ_QOS_RESPONSE	0x05


#define UART2WIFICOMMANDS_READ_DEVICEMAC_REQUEST	0x06
#define UART2WIFICOMMANDS_READ_DEVICEMAC_RESPONSE	0x06


#define UART2WIFICOMMANDS_READ_DEVICESN_REQUEST		0x07
#define UART2WIFICOMMANDS_READ_DEVICESN_RESPONSE	0x07


#define UART2WIFICOMMANDS_READ_DEVICEINFO_REQUEST	0x08
#define UART2WIFICOMMANDS_READ_DEVICEINFO_RESPONSE	0x08


#define UART2WIFICOMMANDS_WRITE_DEVICESN_REQUEST	0x11
#define UART2WIFICOMMANDS_WRITE_DEVICESN_RESPONSE	0x11



#define UART2WIFICOMMANDS_CHECK_NETWORK_STATE_REQUEST	0x40
#define UART2WIFICOMMANDS_CHECK_NETWORK_STATE_RESPONSEE	0x40



#define UART2WIFICOMMANDS_SYNC_SYSTEMTIME_REQUEST	0x41
#define UART2WIFICOMMANDS_SYNC_SYSTEMTIME_RESPONSE	0x41


#define UART2WIFICOMMANDS_READ_WIFIMODULE_PROPERTY_REQUEST	0x50
#define UART2WIFICOMMANDS_READ_WIFIMODULE_PROPERTY_RESPONSE	0x50

#define UART2WIFICOMMANDS_QUERY_DEVICE_VERSION_REQUEST	0x61
#define UART2WIFICOMMANDS_QUERY_DEVICE_VERSION_RESPONSE	0x61

#define UART2WIFICOMMANDS_QUERY_DEVICE_UPDATE_REQUEST	0x62
#define UART2WIFICOMMANDS_QUERY_DEVICE_UPDATE_RESPONSE	0x62

#define UART2WIFICOMMANDS_PUSH_DEVICE_UPDATE_REQUEST	0x63
#define UART2WIFICOMMANDS_PUSH_DEVICE_UPDATE_RESPONSE	0x63
#define UART2WIFICOMMANDS_ACTIVE_DEVICE_UPDATE_RESPONSE	0x65


#define UART2WIFICOMMANDS_SET_UART_BAUD_REQUEST 	0x66
#define UART2WIFICOMMANDS_SET_UART_BAUD_RESPONSE 	0x66


#define UART2WIFICOMMANDS_CONFIG_WIFI_REQUEST 	0x67
#define UART2WIFICOMMANDS_CONFIG_WIFI_RESPONSE 	0x67


#define UART2WIFICOMMANDS_WIFI_POWER_REQUEST	0x68
#define UART2WIFICOMMANDS_WIFI_POWER_RESPONSE	0x68


#define UART2WIFICOMMANDS_RENAME_WIFISSID_REQUEST	0x70
#define UART2WIFICOMMANDS_RENAME_WIFISSID_RESPONSE	0x70


#define UART2WIFICOMMANDS_EASY_LINK_REQUEST		0x72
#define UART2WIFICOMMANDA_LINK_RESULT_RESPONSE	0x72

#define UART2WIFICOMMANDS_REBOOT_REQUEST	0x82
#define UART2WIFICOMMANDS_REBOOT_RESPONSE	0x82

#define UART2WIFICOMMANDS_RESET2FACTORY_REQUEST		0x83
#define UART2WIFICOMMANDS_RESET2FACTORY_RESPONSE	0x83

#define UART2WIFICOMMANDS_QUERY_DEVICE_STATUS_REQUEST	0x21
#define UART2WIFICOMMANDS_QUERY_DEVICE_STATUS_RESPONSE	0x21


extern ej_timer_t wifi2deviceTimer, cloud2deviceTimer, lan2deviceTimer;

static uint8_t GobaldataID = 0;

uint8_t GetUart2WifiPacketDataID()
{
  uint8_t dataID = 0;
	
  dataID = GobaldataID;

  GobaldataID = (GobaldataID + 1) % 256;

  return dataID;
}


void FillCommonUart2WifiPacket(uart2WifiPacket *pPacket, uint8_t dataType, unsigned short dataLength, uint8_t cryptAndFlag)
{
  if (pPacket) {

    pPacket->head = 0xAA;

    pPacket->version = 0x02;
		
    pPacket->crypt = cryptAndFlag;

    pPacket->dataLen[0] = (uint8_t)(dataLength & 0xff);
    pPacket->dataLen[1] = (uint8_t)((dataLength & 0xff00) >> 8);
		
    pPacket->dataType = dataType;

    unsigned short dataTypeCrc = ((dataType << 8 | dataType)) ^ dataLength;
		
    pPacket->dataTypeCrc[0] = (uint8_t)(dataTypeCrc & 0xff);
    pPacket->dataTypeCrc[1] = (uint8_t)((dataTypeCrc & 0xff00) >> 8);
		
    pPacket->dataID = GetUart2WifiPacketDataID();
  }

}

void EncodeUart2WifiPacket(uart2WifiPacket *pPacket)
{
  uint8_t crypt = GetDeviceInfoCrypt();

  pPacket->crypt |= (crypt << 4);

  if (pPacket->crypt & DATA_WITH_XOR_CRYPT) {

    xor(pPacket->data, pPacket->dataLen[0] - 9);
  }
}

typedef struct {

  uint8_t qos;

  uint8_t ackTimeout;

  uint8_t defaultConfigMode;
  
  uint8_t attributecode;

  uint8_t crypt;
	
}PacketGetDeviceQosResponse;


uart2WifiPacket * readDeviceQos()
{
  uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));


  if (pPacket) {

    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDS_READ_QOS_REQUEST, UART2WIFIPACKET_FIXED_LENGTH, DATA_WITH_NO_CRYPT);

    pPacket->data = NULL;

  }else {

    EJ_ErrPrintf(("[UARTCommands.c][readDeviceQos][ERROR]: EJ_mem_malloc pPacket failed.\r\n"));
  }

  return pPacket;
}

uint8_t Process_ReadDeviceQosResponseCB(uart2WifiPacket *pPacket)
{

  if (pPacket) {

    if (pPacket->data) {

      PacketGetDeviceQosResponse *pResponse = (PacketGetDeviceQosResponse *)pPacket->data;

      SetDeviceInfoQos(pResponse->qos);
			
      if ((pResponse->qos > 0) && (pResponse->ackTimeout > 0)) {

				if (EJ_timer_change(&wifi2deviceTimer, EJ_msec_to_ticks(/*pResponse->ackTimeout*10*/EJ_msec_to_ticks(200)),EJ_msec_to_ticks(1)) != EJ_SUCCESS) {

					EJ_ErrPrintf(("[UARTCommands.c][Process_ReadDeviceQosResponseCB][ERROR]: os_timer_change wifi2deviceTimer failed.\r\n"));
				}
				if (EJ_timer_change(&cloud2deviceTimer, EJ_msec_to_ticks(/*pResponse->ackTimeout*10*/EJ_msec_to_ticks(200)),EJ_msec_to_ticks(1)) != EJ_SUCCESS) {

					EJ_ErrPrintf(("[UARTCommands.c][Process_ReadDeviceQosResponseCB][ERROR]: os_timer_change cloud2deviceTimer failed.\r\n"));
				}
				if (EJ_timer_change(&lan2deviceTimer, EJ_msec_to_ticks(/*pResponse->ackTimeout*10*/EJ_msec_to_ticks(200)),EJ_msec_to_ticks(1)) != EJ_SUCCESS) {

					EJ_ErrPrintf(("[UARTCommands.c][Process_ReadDeviceQosResponseCB][ERROR]: os_timer_change lan2deviceTimer failed.\r\n"));
				}
      }
			
      SetDeviceInfoAckTimeout(pResponse->ackTimeout);
      SetDeviceInfoCrypt(pResponse->crypt);
      SetDeviceInfoDefaultConfigMode(pResponse->defaultConfigMode);

	 // EJ_Printf("%s,%d,%d,%d,%d\r\n",__FUNCTION__,pResponse->qos,pResponse->crypt,pResponse->ackTimeout,pResponse->defaultConfigMode);
			
    }

  }

  return 0x00;
}

uart2WifiPacket * readDeviceMAC()
{
  uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));


  if (pPacket) {

    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDS_READ_DEVICEMAC_REQUEST, UART2WIFIPACKET_FIXED_LENGTH, DATA_WITH_NO_CRYPT | QOS_BITS);

    pPacket->data = NULL;

  }else {

    EJ_ErrPrintf(("[UARTCommands.c][readDeviceMAC][ERROR]: EJ_mem_malloc pPacket failed.\r\n"));
  }

  return pPacket;

}

uint8_t Process_ReadDeviceMacResponseCB(uart2WifiPacket *pPacket)
{

  if (pPacket) {

    /*
      SetDeviceInfoManufacturers(pPacket->manufacturers);
      SetDeviceInfoDeviceType(pPacket->deviceType);
      SetDeviceInfoDeviceTypeString(pPacket->deviceTypeString);
    */

  }

  return 0x00;
}



uart2WifiPacket * readDeviceUUID()
{

  uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));


  if (pPacket) {

    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDS_READ_DEVICESN_REQUEST, UART2WIFIPACKET_FIXED_LENGTH, DATA_WITH_NO_CRYPT | QOS_BITS);

    pPacket->data = NULL;

  }else {

    EJ_ErrPrintf(("[UARTCommands.c][readDeviceSN][ERROR]: EJ_mem_malloc pPacket failed.\r\n"));
  }

  return pPacket;

}

uint8_t Process_readDeviceSNResponseCB(uart2WifiPacket *pPacket)
{
  uint8_t ret = 0x00;

  if (pPacket) {

    if (pPacket->dataType == UART2WIFICOMMANDS_READ_DEVICESN_RESPONSE) {

      unsigned short length = (pPacket->dataLen[0] | (pPacket->dataLen[1] << 8));

      //uint8_t DeviceSN_len = length - 9;
      uint8_t DeviceUuid_len = length - 9;

      uint8_t i = 0;

      for (i = 0; i < DeviceUuid_len; i++) {

		EJ_Printf("%x ", pPacket->data[i]);
      }

			

      if (DeviceUuid_len  == 6) {
				
		//DeviceSN *newSn = (DeviceSN *)(pPacket->data);
		DeviceUuid *newUuid = (DeviceUuid *)(pPacket->data);

		//DeviceSN lastSn = GetDeviceInfoDeviceSN();
		DeviceUuid lastUuid = GetDeviceInfoDeviceUuid();

		uint8_t *data = (uint8_t *)&lastUuid;
		EJ_Printf("\r\n");

		for (i = 0; i < DeviceUuid_len; i++) {

		  EJ_Printf("%x ", data[i]);
		}
		EJ_Printf("\r\n");

		if (memcmp(newUuid, &lastUuid, 6) != 0 || data == NULL) {
		  EJ_Printf("not equal.\r\n");
		  
		  SetDeviceInfoDeviceUuid(newUuid);
		  storeDeviceInfoDeviceUuid();

		  uint8_t clear[6]={0};
		  storeDeviceInfoDeviceid(clear);
		  //clear did 
		  
		  ret = 0x01;
		}
//		if(newUuid==NULL)
//		{
//			SetDeviceInfoDeviceUuid();	
//			storeDeviceInfoDeviceid("000000");
//		}
		
		//SetDeviceInfoDeviceSN(newSn);
		
		//storeDeviceInfoDeviceUuid();

        //storeDeviceInfoDeviceUuid();

						
      }
      else {

      }
    }
    else {

    }
  }

  return ret;
}

uart2WifiPacket * getDeviceInfo()
{

  uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));


  if (pPacket) {

    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDS_READ_DEVICEINFO_REQUEST, UART2WIFIPACKET_FIXED_LENGTH, DATA_WITH_NO_CRYPT | QOS_BITS);

    pPacket->data = NULL;

  }else {

    EJ_ErrPrintf(("[UARTCommands.c][getDeviceInfo][ERROR]: EJ_mem_malloc pPacket failed.\r\n"));
  }

  return pPacket;

}

typedef struct {

  uint8_t reserved;

  uint8_t deviceBrandLength;

  uint8_t *deviceBrand;

  uint8_t deviceTypeLength;

  uint8_t *deviceType;

  uint8_t deviceModelLength;

  uint8_t *deviceModel;

  uint8_t *deviceVersion;
	
}PacketGetDeviceInfoResponse;

uint8_t Process_GetDeviceInfoResponseCB(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    if (pPacket->data) {

      PacketGetDeviceInfoResponse response;

      response.deviceBrandLength = pPacket->data[1];
			
      response.deviceBrand = (uint8_t *)(pPacket->data + 2);

      response.deviceTypeLength = pPacket->data[2 + response.deviceBrandLength];

      response.deviceType = (uint8_t *)(pPacket->data + 2 + response.deviceBrandLength + 1);


      response.deviceModelLength = pPacket->data[2 + response.deviceBrandLength + 1 + response.deviceTypeLength];
      response.deviceModel = (uint8_t *)(pPacket->data + 2 + response.deviceBrandLength + 1 + response.deviceTypeLength + 1);

      response.deviceVersion = (uint8_t *)(pPacket->data + 2 + response.deviceBrandLength + 1 + response.deviceTypeLength + 1 + response.deviceModelLength);

      if (0 < response.deviceBrandLength < 20) {

	/* after re-invoke, this place maybe error.*/
	char *pBrandStr = (char *)EJ_mem_malloc(response.deviceBrandLength + 1);

	memcpy(pBrandStr, response.deviceBrand, response.deviceBrandLength);

	pBrandStr[response.deviceBrandLength] = '\0';

	SetDeviceInfoDeviceBrandString(pBrandStr);

	//EJ_Printf("[UARTCommands.c][Process_GetDeviceInfoResponseCB][INFO]: brand: %s\r\n", pBrandStr);
    }

      if (0 < response.deviceTypeLength < 20) {

	char *pTypeStr = (char *)EJ_mem_malloc(response.deviceTypeLength + 1);

	memcpy(pTypeStr, response.deviceType, response.deviceTypeLength);

	pTypeStr[response.deviceTypeLength] = '\0';

	SetDeviceInfoDeviceTypeString(pTypeStr);

	//EJ_Printf("[UARTCommands.c][Process_GetDeviceInfoResponseCB][INFO]: type: %s\r\n", pTypeStr);
    }

      if (0 < response.deviceModelLength < 20) {
				
	char *pModelStr = (char *)EJ_mem_malloc(response.deviceModelLength + 1);

	memcpy(pModelStr, response.deviceModel, response.deviceModelLength);

	pModelStr[response.deviceModelLength] = '\0';

	SetDeviceInfoDeviceModelString(pModelStr);

	//EJ_Printf("[UARTCommands.c][Process_GetDeviceInfoResponseCB][INFO]: model: %s\r\n", pModelStr);
    }

      if (strlen(response.deviceVersion) > 0) {

	char *pVersion = (char *)EJ_mem_malloc(pPacket->dataLen[0] - 4 - response.deviceBrandLength - response.deviceTypeLength - response.deviceModelLength + 1);

	memcpy(pVersion, response.deviceVersion, pPacket->dataLen[0] - 4 - response.deviceBrandLength - response.deviceTypeLength - response.deviceModelLength + 1);

	pVersion[pPacket->dataLen[0] - 4 - response.deviceBrandLength - response.deviceTypeLength - response.deviceModelLength] = '\0';

	SetDeviceInfoDeviceTVersionString(pVersion);

	//EJ_Printf("[UARTCommands.c][Process_GetDeviceInfoResponseCB][INFO]: version: %s\r\n", pVersion);
    }
    }
  }

  return 0x00;
}


typedef PACK_START struct {

  uint8_t configured;

  uint8_t deviceIDConfigured;

  uint8_t deviceConnected;

  uint8_t workmode;

  uint8_t signalIntensity;

  uint8_t routerStatus;

  uint8_t selfCloudServiceStatus;

  uint8_t mediaCloudServiceStatus;

  uint8_t jdCloudServiceStatus;

  uint8_t alibabaCloudServiceStatus;

  uint8_t extraCloudServiceStatus;

  uint8_t connectStatus;

  uint8_t connectCount;
	
  uint8_t ipaddr[4];

  uint8_t *connectedSSID;
	
}PACK_END PacketCheckNetworkStatusResponse;

uart2WifiPacket * noticeNetworkStatus(uint8_t dataID)
{
  PacketCheckNetworkStatusResponse networkStatus;

  memset(&networkStatus,0,sizeof(PacketCheckNetworkStatusResponse));

  networkStatus.configured = GetWifiModuleStatusIsHomeAPConfig();
	
  uint8_t deviceID[6];
  GetWifiStatusDeviceID(deviceID);
  uint8_t i = 0;
  int total = 0;
  for(i = 0; i < 6; i++) {
    total += deviceID[i];
  }
  if (total != 0) {
		
    networkStatus.deviceIDConfigured = 1;
  }else {
    networkStatus.deviceIDConfigured = 0;
  }

  networkStatus.deviceConnected = 0;
	
  networkStatus.workmode = GetWifiModuleStatusWorkMode();
  networkStatus.signalIntensity = GetWifiModuleStatusSignalIntensity();
  networkStatus.routerStatus = GetWifiModuleStatusRouterStatus();

  networkStatus.selfCloudServiceStatus = GetWifiModuleStatusCloudServiceStatus();
  networkStatus.mediaCloudServiceStatus = 0x01;
  networkStatus.jdCloudServiceStatus = 0x01;
  networkStatus.alibabaCloudServiceStatus = 0x01;
  networkStatus.extraCloudServiceStatus = 0x01;
	
  networkStatus.connectStatus = GetWifiModuleStatusConnectedStatus();
  networkStatus.connectCount = GetWifiModuleStatusTcpConnectedCount();

  char connectedSSID[32] = {0};

  GetWifiConfigSSID(connectedSSID,sizeof(connectedSSID));
 
  char ip[20] ={0} ;

  EJ_Wlan_get_ip_address(ip);

  if (ip) {

    sscanf(ip, "%d.%d.%d.%d", networkStatus.ipaddr, networkStatus.ipaddr + 1, networkStatus.ipaddr + 2, networkStatus.ipaddr + 3); 
  }

  uart2WifiPacket *pNoticeNetworkStatusPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

  if (pNoticeNetworkStatusPacket) {

    

    FillCommonUart2WifiPacket(pNoticeNetworkStatusPacket, UART2WIFICOMMANDS_CHECK_NETWORK_STATE_RESPONSEE, UART2WIFIPACKET_FIXED_LENGTH + sizeof(PacketCheckNetworkStatusResponse) - 4 + strlen(connectedSSID), DATA_WITH_NO_CRYPT | RESPONSION_BITS);

    pNoticeNetworkStatusPacket->dataID = dataID;

    pNoticeNetworkStatusPacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketCheckNetworkStatusResponse) - 4 + strlen(connectedSSID));

    if (pNoticeNetworkStatusPacket->data) {

    	  memcpy(pNoticeNetworkStatusPacket->data, &networkStatus, sizeof(PacketCheckNetworkStatusResponse) - 4);

      if (strlen(connectedSSID)) {

		  memcpy(pNoticeNetworkStatusPacket->data + sizeof(PacketCheckNetworkStatusResponse) - 4, connectedSSID, strlen(connectedSSID));
				
      }

    }
    else {

      EJ_mem_free(pNoticeNetworkStatusPacket);

      pNoticeNetworkStatusPacket = NULL;

      EJ_ErrPrintf(("[UARTCommands.c][noticeNetworkStatus][ERROR]: EJ_mem_malloc packet->data failed.\r\n"));
    }

  }else {

    EJ_ErrPrintf(("[UARTCommands.c][noticeNetworkStatus][ERROR]: EJ_mem_malloc packet failed.\r\n"));
  }

  return pNoticeNetworkStatusPacket;
}


uint8_t Process_CheckNetworkStatusRequestCB(uart2WifiPacket *pPacket)
{

  if (pPacket) {

    uart2WifiPacket *pResponsePacket = noticeNetworkStatus(pPacket->dataID);

    if (pResponsePacket) {

      if (nolock_list_push(GetWifi2deviceList(), pResponsePacket) != NOLOCK_OPERATION_SUCCESS)
	{
	  EJ_ErrPrintf(("[UARTCommands.c][Process_CheckNetworkStatusRequestCB][ERROR]: nolock_list_push failed.\r\n"));
	}
			
    }else {

      EJ_ErrPrintf(("[UARTCommands.c][Process_CheckNetworkStatusRequestCB][ERROR]: noticeNetworkStatus failed.\r\n"));
    }
				
  }

  return 0x00;
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
	
}PacketDeviceSyncSystemTimeResponse;

uint8_t Process_DeviceSyncSystemTimeRequestCB(uart2WifiPacket *pPacket)
{

  if (pPacket) {

    uart2WifiPacket *pResponsePacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

    if (pResponsePacket) {

      PacketDeviceSyncSystemTimeResponse response;

      struct ej_tm h_tm;

      if (EJ_time_get(&h_tm) == 0) {

					response.sec = h_tm.tm_sec;
					response.min = h_tm.tm_min;
					response.hour = h_tm.tm_hour;
					response.day = h_tm.tm_mday;
					response.month = h_tm.tm_mon;
					response.year = h_tm.tm_year;
					response.week = h_tm.tm_wday;

					response.zone = 0x08;
      }

      FillCommonUart2WifiPacket(pResponsePacket, UART2WIFICOMMANDS_SYNC_SYSTEMTIME_RESPONSE, UART2WIFIPACKET_FIXED_LENGTH + sizeof(PacketDeviceSyncSystemTimeResponse), DATA_WITH_XOR_CRYPT);

      pResponsePacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketDeviceSyncSystemTimeResponse));

      if (pResponsePacket->data) {

	memcpy(pResponsePacket->data, (uint8_t *)&response, sizeof(PacketDeviceSyncSystemTimeResponse));

	if (nolock_list_push(GetWifi2deviceList(), pResponsePacket) != NOLOCK_OPERATION_SUCCESS)
	  {
	    EJ_ErrPrintf(("[UARTCommands.c][Process_DeviceSyncSystemTimeRequestCB][ERROR]: nolock_list_push failed.\r\n"));
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

  uint8_t wifimoduleBrandLength;

  uint8_t *wifimoduleBrand;

  uint8_t wifimoduleModelLength;

  uint8_t *wifimoduleModel;
	
}PacketWifiModulePropertyResponse;
	
	
uint8_t Process_DeviceGetWifiModulePropertyRequestCB(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    uart2WifiPacket *pResponsePacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(PacketWifiModulePropertyResponse));

    if (pResponsePacket) {

      PacketWifiModulePropertyResponse response;

      uint8_t *brand = GetWifiModuleInfoBrandStr();

      if (brand) {

	response.wifimoduleBrandLength = strlen(brand);
      }

      uint8_t *model = GetWifiModuleInfoModelStr();

      if (model) {

	response.wifimoduleModelLength = strlen(model);
      }

      uint8_t dataLength = sizeof(PacketWifiModulePropertyResponse) - 4 - 4 + response.wifimoduleBrandLength + response.wifimoduleModelLength;

      pResponsePacket->data = (uint8_t *)EJ_mem_malloc(dataLength);

      FillCommonUart2WifiPacket(pResponsePacket, UART2WIFICOMMANDS_READ_WIFIMODULE_PROPERTY_RESPONSE, UART2WIFIPACKET_FIXED_LENGTH + dataLength, DATA_WITH_NO_CRYPT);
			
      if (pResponsePacket->data) {

	pResponsePacket->data[0] = GetWifiModuleInfoType();

	WIFIModuleVersion version = GetWifiModuleInfoVersion();
				
	memcpy(pResponsePacket->data + 1, (uint8_t *)&version, sizeof(WIFIModuleVersion));


	pResponsePacket->data[1 + sizeof(WIFIModuleVersion) + 6] = response.wifimoduleBrandLength;

	if (response.wifimoduleBrandLength > 0) {
					
	  memcpy(pResponsePacket->data + 1 + sizeof(WIFIModuleVersion) + 6 + 1, brand,  response.wifimoduleBrandLength);
	}

	pResponsePacket->data[1 + sizeof(WIFIModuleVersion) + 6 + response.wifimoduleBrandLength] = response.wifimoduleModelLength;

	if (response.wifimoduleModelLength > 0) {
					
	  memcpy(pResponsePacket->data + 1 + sizeof(WIFIModuleVersion) + 6 + 1 + response.wifimoduleBrandLength + 1, model,  response.wifimoduleModelLength);
	}
				
	if (nolock_list_push(GetWifi2deviceList(), pResponsePacket) != NOLOCK_OPERATION_SUCCESS)
	  {
	    EJ_ErrPrintf(("[UARTCommands.c][Process_DeviceGetWifiModulePropertyRequestCB][ERROR]: nolock_list_push failed.\r\n"));
	  }	
      }else {

	EJ_ErrPrintf(("[UARTCommands.c][Process_DeviceGetWifiModulePropertyRequestCB][ERROR]: EJ_mem_malloc packet->data failed.\r\n"));
      }
		
    }else {

      EJ_ErrPrintf(("[UARTCommands.c][Process_DeviceGetWifiModulePropertyRequestCB][ERROR]: EJ_mem_malloc packet failed.\r\n"));
    }
  }
	
  return 0x00;
}


uart2WifiPacket * queryDeviceVersion()
{
  uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));
	
	
  if (pPacket) {

    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDS_QUERY_DEVICE_VERSION_REQUEST, UART2WIFIPACKET_FIXED_LENGTH, DATA_WITH_NO_CRYPT | QOS_BITS);

    pPacket->data = NULL;

  }else {

    EJ_ErrPrintf(("[UARTCommands.c][queryDeviceVersion][ERROR]: EJ_mem_malloc pPacket failed.\r\n"));
  }

  return pPacket;

}


uint8_t Process_queryDeviceVersionResponseCB(uart2WifiPacket *pPacket)
{

	//EJ_Printf("[ENTER :%s]\r\n",__FUNCTION__);
	wifi2CloudPacket *pCloud2DevicePacket = NULL;
	pCloud2DevicePacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

	if (pPacket) {

		pCloud2DevicePacket->head[0] = 0x5A;
		pCloud2DevicePacket->head[1] = 0x5A;

		pCloud2DevicePacket->version = 0x04;

		pCloud2DevicePacket->crypt = 0x11;

		pCloud2DevicePacket->dataType[0] = 0x61;
		pCloud2DevicePacket->dataType[1] = 0x80;

		fillDataIDToPacket(pCloud2DevicePacket, (uint8_t)pCloud2DevicePacket->dataID);

		/**/
		fillTimeStampToPackt(pCloud2DevicePacket);

		/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
		GetWifiStatusDeviceID(pCloud2DevicePacket->deviceID);

		int datalen = 44 + 14;

		pCloud2DevicePacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
		pCloud2DevicePacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
		pCloud2DevicePacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
		pCloud2DevicePacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);
		//EJ_Printf("device->wifi->cloud:0x61:[0001]\r\n");
		if (pPacket->data) {

			pCloud2DevicePacket->data=(uint8_t *)EJ_mem_malloc(pPacket->dataLen[0]-UART2WIFIPACKET_FIXED_LENGTH);

			if(pCloud2DevicePacket->data)
			{
				//EJ_Printf("device->wifi->cloud:0x61:[0002]\r\n");
				memcpy(pCloud2DevicePacket->data, pPacket->data, pPacket->dataLen[0]- UART2WIFIPACKET_FIXED_LENGTH);
				//memcpy(pCloud2DevicePacket->data, pPacket->data, 14);
				EJ_PrintUart2WifiPacket(pCloud2DevicePacket,"Device2CloudPacket");

				if (nolock_list_push(GetWifi2cloudList(), pCloud2DevicePacket) != 0x01)
				{
					EJ_mem_free(pCloud2DevicePacket->data);
					EJ_mem_free(pCloud2DevicePacket);
					EJ_ErrPrintf(("[MQTTCommands.c][Process_heartBeatRequest][ERROR]: add packet to Device2wifiList failed.\r\n"));
				}

	      		//SetDeviceInfoDeviceTVersionString(version);
			}
			
    	}else {


			EJ_mem_free(pCloud2DevicePacket);

      		EJ_ErrPrintf(("[UARTCommands.c][Process_queryDeviceVersionResponseCB][ERROR]: device version is empty.\r\n"));
		}
		
  	}

  	return 0x00;
}


uint8_t Process_judgeDeviceOTAResponseCB(uart2WifiPacket *pPacket)
{
	uart2WifiPacket *pUart2WifiPacket = NULL;

	EJ_PrintUart2WifiPacket(pPacket,"judgeDeviceOTA");

	if(pPacket)
	{	

		if(pPacket->data)
		{	
			uint8_t framesize = 0;
			framesize = pPacket->data[1];
			EJ_Printf("framesize [%d]\r\n",framesize);
			
			if(pPacket->data[0] == 0x00){

				//EJ_Printf("DeviceFirmwareUpdate\r\n");
				//DeviceFirmwareUpdate(framesize);
				//DeviceFirmwareUpdate();
			}
		}else{
		  EJ_mem_free(pPacket->data);
				
		  EJ_ErrPrintf(("[UARTCommands.c][queryDeviceUpdate][ERROR]: EJ_mem_malloc packet->data failed.\r\n"));
		}
	}else{
		EJ_mem_free(pPacket);
	}
	
	return 0x00;
}


uint8_t Process_ActiveDeviceOtaResponseCB(uart2WifiPacket *pPacket)
{

	EJ_PrintUart2WifiPacket(pPacket,"ActiveDeviceOta");

	if(pPacket)
	{
		if(pPacket->data)
		{
			if(pPacket->data[1] == 0x01)
			{
				//获取最新的下载地址并下载(0x62)
				//char *httpAddr = NULL;
				//DeviceFirmwareDownload(httpAddr,NULL);
				//os_thread_sleep(200);
				
				//发送固件给家电板(0x63)
				//DeviceFirmwareUpdate();
				
			}else if(pPacket->data[0] == 0x02)
			{
				EJ_Printf("none of the latest firmware can be upgraded");
			}else if(pPacket->data[0] == 0x03)
			{
				EJ_Printf("no backup firmware can be restored");
			}
			
		}else{

			EJ_mem_free(pPacket->data);
		}
	}else{

		EJ_mem_free(pPacket);
	}

	return 0x00;
}



typedef struct {

  uint8_t size[4];

  uint8_t *deviceSoftwareVersion;

}PacketQueryDeviceUpdateRequest;


typedef struct {

  uint8_t ret;

  uint8_t frameSize[2];

}PacketQueryDeviceUpdateResponse;


uart2WifiPacket* queryDeviceUpdate(int size, char *softwareVersion)
{
  uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

  if (pPacket) {

    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDS_QUERY_DEVICE_UPDATE_REQUEST, UART2WIFIPACKET_FIXED_LENGTH + 19, DATA_WITH_NO_CRYPT | 0);

    pPacket->data = (uint8_t *)EJ_mem_malloc(4 + 13 + 2);

    if (pPacket->data) {

		pPacket->data[0] = 0x02;
		pPacket->data[1] = 0x00;
	#if 0
		pPacket->data[2] = 0x01;//softwareVersion[0];
		pPacket->data[3] = 0x01;//softwareVersion[1];
		pPacket->data[4] = 0x00;//softwareVersion[2];
		pPacket->data[5] = 0x01;//softwareVersion[3];
		pPacket->data[6] = 0x00;//softwareVersion[4];
		pPacket->data[7] = 0x01;//softwareVersion[5];
		pPacket->data[8] = 0x00;//softwareVersion[6];
		pPacket->data[9] = 0x02;//softwareVersion[7];
		pPacket->data[10] = 0x00;//softwareVersion[8];
		pPacket->data[11] = 0x16;//softwareVersion[9];
		pPacket->data[12] = 0x44;//softwareVersion[10];
		pPacket->data[13] = 0x00;//softwareVersion[11];
		pPacket->data[14] = 0x00;//softwareVersion[12];
	#else
		pPacket->data[2] = softwareVersion[0];
		pPacket->data[3] = softwareVersion[1];
		pPacket->data[4] = softwareVersion[2];
		pPacket->data[5] = softwareVersion[3];
		pPacket->data[6] = softwareVersion[4];
		pPacket->data[7] = softwareVersion[5];
		pPacket->data[8] = softwareVersion[6];
		pPacket->data[9] = softwareVersion[7];
		pPacket->data[10] = softwareVersion[8];
		pPacket->data[11] = softwareVersion[9];
		pPacket->data[12] = softwareVersion[10];
		pPacket->data[13] = softwareVersion[11];
		pPacket->data[14] = softwareVersion[12];
	#endif
		pPacket->data[15] = (size & 0xff000000)>>24;
		pPacket->data[16] = (size & 0x00ff0000)>>16;
		pPacket->data[17] = (size & 0x0000ff00)>>8;
		pPacket->data[18] = (size & 0x000000ff)>>0;
			
    }else {

      EJ_mem_free(pPacket);

		  pPacket = NULL;
				
		  EJ_ErrPrintf(("[UARTCommands.c][queryDeviceUpdate][ERROR]: EJ_mem_malloc packet->data failed.\r\n"));
	}
		
		
  }else {

    	EJ_ErrPrintf(("[UARTCommands.c][queryDeviceUpdate][ERROR]: EJ_mem_malloc packet failed.\r\n"));
  }

  return pPacket;
}

uart2WifiPacket * pushDeviceUpdate(uint32_t frameNum, uint32_t frameNo,uint8_t *sendbuf)
{
  	uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

  	if(pPacket){

	    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDS_PUSH_DEVICE_UPDATE_REQUEST, UART2WIFIPACKET_FIXED_LENGTH + 73, DATA_WITH_NO_CRYPT | 0);
		
	   	pPacket->data = (uint8_t *)EJ_mem_malloc(73);

	    if (pPacket->data) {

	      pPacket->data[0] = 0x00;

		  pPacket->data[4] = (frameNum & 0xff000000)>>24;
		  pPacket->data[3] = (frameNum & 0x00ff0000)>>16;
		  pPacket->data[2] = (frameNum & 0x0000ff00)>>8;
		  pPacket->data[1] = (frameNum & 0x000000ff)>>0;
		  
		  pPacket->data[8] = (frameNo & 0xff000000)>>24;
		  pPacket->data[7] = (frameNo & 0x00ff0000)>>16;
		  pPacket->data[6] = (frameNo & 0x0000ff00)>>8;
		  pPacket->data[5] = (frameNo & 0x000000ff)>>0;
		  //memcpy(pPacket->data + 9,sendbuf,sizeof(sendbuf));
		  memcpy(pPacket->data + 9,sendbuf,64);
				
	    }else {

	      EJ_mem_free(pPacket);

	      pPacket = NULL;
				
	      EJ_ErrPrintf(("[UARTCommands.c][pushDeviceUpdate][ERROR]: EJ_mem_malloc packet->data failed.\r\n"));
	    }
  	}else {
  	
    	EJ_ErrPrintf(("[UARTCommands.c][pushDeviceUpdate][ERROR]: EJ_mem_malloc packet failed.\r\n"));
  	}

  return pPacket;
}


typedef struct {

  uint8_t reserved;

  uint8_t totalFrames[4];

  uint8_t currentFrames[4];

  uint8_t * data;

}PacketUpdateFrameRequest;



#define DEVICE_FIRMWARE_BASE_ADDR	0x9D000

static uint32_t calcFlashReadAddr(uint32_t frameNum, uint32_t frameSize)
{

	return DEVICE_FIRMWARE_BASE_ADDR + frameNum*frameSize;
}

uint8_t Process_QueryDeviceUpdateResponseCB(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    if (pPacket->data) {

      PacketQueryDeviceUpdateResponse *response = (PacketQueryDeviceUpdateResponse *)pPacket->data;

      if (response->ret == 0) {

	/* prepare the first 0x63 command packet to start update process. */
	uart2WifiPacket *pResponsePacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

	if (pResponsePacket) {

	  pPacket->data = (uint8_t *)EJ_mem_malloc(9 +(size_t)response->frameSize);

	  if (pPacket->data) {

	    uint32_t addr = calcFlashReadAddr(0, (uint32_t)response->frameSize);

	    /* read data from flash...*/
	    int ret = 0;
					
	    //ret = flash_drv_read(fl_dev, pPacket->data + 9, response->frameSize, addr);

	    if (ret != EJ_SUCCESS) {

	      EJ_ErrPrintf(("[UARTCommands.c][Process_QueryDeviceUpdateResponseCB][ERROR]: flash_drv_read failed.\r\n"));
	    }

	    pPacket->data[0] = 0;
						
	  }
	}
				
      }
    }
  }

  return 0x00;

}

typedef struct {

  uint8_t ret;

  uint8_t currentFrames[4];

  uint8_t operation;

}PacketUpdateFrameResponse;

int totalFrames = 0;
int currentFrames = 0;

uint8_t Process_DeviceUpdateFrameResponse(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    if (pPacket->data) {

      PacketUpdateFrameResponse *response = (PacketUpdateFrameResponse *)pPacket->data;

      if (response->ret == 0) {

		currentFrames++;
				
      }else{

      }

      /* calc falsh read offset and size*/
      //uint32_t addr = calcFlashReadAddr(currentFrames, response.frameSize);

      /* prepare the first 0x63 command packet to start update process. */
      uart2WifiPacket *pResponsePacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

      if (pResponsePacket) {

	//pPacket->data = (uint8_t *)EJ_mem_malloc(9 + response.frameSize);

	if (pPacket->data) {

	  /* read data from flash...*/
	  int ret = 0;
					
	  //ret = flash_drv_read(fl_dev, pPacket->data + 9, response.frameSize, addr);

	  if (ret != EJ_SUCCESS) {

	    EJ_ErrPrintf(("[UARTCommands.c][Process_DeviceUpdateFrameResponse][ERROR]: flash_drv_read failed.\r\n"));
	  }

	  pPacket->data[0] = 0;
					
	}
      }
				

    }
  }
	
  return 0x00;
}

typedef struct {

  uint8_t ret;
	
  uint8_t baud;
	
}PacketSettingUartBaudResponse;

typedef struct {

  uint8_t baud;
	
}PacketSettingUartBaudRequest;


uart2WifiPacket * settingUartBaud(uint8_t baud)
{
  uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

  if (pPacket) {

    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDS_SET_UART_BAUD_RESPONSE, UART2WIFIPACKET_FIXED_LENGTH + sizeof(PacketSettingUartBaudResponse), DATA_WITH_XOR_CRYPT);

    pPacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketSettingUartBaudResponse));

    if (pPacket->data) {

      PacketSettingUartBaudResponse settingUartBaudResponse;

      settingUartBaudResponse.baud = baud;

      memcpy(pPacket->data, (uint8_t *)&settingUartBaudResponse, sizeof(PacketSettingUartBaudResponse));

      if (nolock_list_push(GetWifi2deviceList(), pPacket) != 0x01)
	{
	  EJ_ErrPrintf(("[UARTCommands.c][settingUartBaud][ERROR]: nolock_list_push failed.\r\n"));
	}
			
    }else {

      EJ_ErrPrintf(("[UARTCommands.c][settingUartBaud][ERROR]: EJ_mem_malloc pPacket->data failed.\r\n"));
    }
  }else {

    EJ_ErrPrintf(("[UARTCommands.c][settingUartBaud][ERROR]: EJ_mem_malloc pPacket failed.\r\n"));
  }

  return pPacket;
}


uint8_t Process_SettingUartBaudRequestCB(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    if (pPacket->data) {

      PacketSettingUartBaudRequest *response = (PacketSettingUartBaudRequest *)(pPacket->data);

      uart2WifiPacket *pResponsePacket = settingUartBaud(response->baud);

      if (pResponsePacket) {

	if (nolock_list_push(GetWifi2deviceList(), pPacket) != 0x01)
	  {
	    EJ_ErrPrintf(("[UARTCommands.c][Process_SettingUartBaudRequestCB][ERROR]: nolock_list_push failed.\r\n"));
	  }
				
      }else {

	EJ_ErrPrintf(("[UARTCommands.c][Process_SettingUartBaudRequestCB][ERROR]: settingUartBaud failed.\r\n"));
      }
    }
  }
	
  return 0x00;
}

typedef struct {

  uint8_t operation;

}PacketWifiPowerRequest;

typedef struct {

  uint8_t operation;

  uint8_t ret;

}PacketWifiPowerResponse;

uint8_t ProcessWifiPowerRequest(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    if (pPacket->data) {

      PacketWifiPowerRequest request;

      request.operation = pPacket->data[0];

      uart2WifiPacket *pResponsePacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

      if (pResponsePacket) {

	FillCommonUart2WifiPacket(pResponsePacket, UART2WIFICOMMANDS_WIFI_POWER_RESPONSE, UART2WIFIPACKET_FIXED_LENGTH + sizeof(PacketWifiPowerResponse), DATA_WITH_NO_CRYPT | RESPONSION_BITS);	

	pResponsePacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketWifiPowerResponse));

	if (pResponsePacket->data) {

	  PacketWifiPowerResponse response;

	  response.operation = request.operation;
	  response.ret = 0;

	  memcpy(pResponsePacket->data, (uint8_t *)&response, sizeof(PacketWifiPowerResponse));

	  if (nolock_list_push(GetWifi2deviceList(), pResponsePacket) != NOLOCK_OPERATION_SUCCESS)
	    {
	      EJ_ErrPrintf(("[UARTCommands.c][ProcessWifiPowerRequest][ERROR]: nolock_list_push failed.\r\n"));
	    }

	}
				
      }
    }
		
  }
  return 0x00;
}

typedef struct {

  uint8_t *ssid;
	
}PacketRenameWifiSSIDRequest;

typedef struct {

  uint8_t ret;
	
}PacketRenameWifiSSIDResponse;


uint8_t Process_RenameWifiSSIDRequestCB(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    if (pPacket->data) {

      PacketRenameWifiSSIDRequest request;
			
      request.ssid = (uint8_t *)pPacket->data;
			
      uart2WifiPacket *pResponsePacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

      if (pResponsePacket) {

	FillCommonUart2WifiPacket(pResponsePacket, UART2WIFICOMMANDS_RENAME_WIFISSID_RESPONSE, UART2WIFIPACKET_FIXED_LENGTH + sizeof(PacketRenameWifiSSIDResponse), DATA_WITH_NO_CRYPT | RESPONSION_BITS);	

	pResponsePacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketRenameWifiSSIDResponse));

	if (pResponsePacket->data) {

	  PacketRenameWifiSSIDResponse response;

	  response.ret = 0;

	  memcpy(pResponsePacket->data, (uint8_t *)&response, sizeof(PacketRenameWifiSSIDResponse));

	  if (nolock_list_push(GetWifi2deviceList(), pResponsePacket) != NOLOCK_OPERATION_SUCCESS)
	    {
	      EJ_ErrPrintf(("[UARTCommands.c][Process_RenameWifiSSIDRequestCB][ERROR]: nolock_list_push failed.\r\n"));
	    }

	  SetWifiConfigRenameSSID(request.ssid);

	}
      }
    }

  }

  return 0;

}

typedef struct {

  uint8_t configMode;

}PacketEasyLinkResquest;


typedef struct {

  uint8_t status;
	
}PacketLinkResultResponse;

uart2WifiPacket * noticeLinkResult(uint8_t linkresult)
{
  uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

  if (pPacket) {

    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDA_LINK_RESULT_RESPONSE, UART2WIFIPACKET_FIXED_LENGTH + sizeof(PacketLinkResultResponse), DATA_WITH_NO_CRYPT | RESPONSION_BITS);

    pPacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketLinkResultResponse));

    if (pPacket->data) {

      PacketLinkResultResponse linkresultResponse;

      linkresultResponse.status = linkresult;

      memcpy(pPacket->data, (uint8_t *)&linkresultResponse, sizeof(PacketLinkResultResponse));
			
    }else {

      EJ_mem_free(pPacket);

      pPacket = NULL;
			
      EJ_ErrPrintf(("[UARTCommands.c][noticeLinkResult][ERROR]: EJ_mem_malloc pPacket->data failed.\r\n"));
    }

  }else {

    EJ_ErrPrintf(("[UARTCommands.c][noticeLinkResult][ERROR]: EJ_mem_malloc pPacket failed.\r\n"));
  }

  return pPacket;

}



uint8_t Process_DeviceEasyLinkRequestCB(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    if (pPacket->data) {
			
      PacketEasyLinkResquest request;

      request.configMode = pPacket->data[0];

      uart2WifiPacket * pResponsePacket = noticeLinkResult(0);

      if (pResponsePacket)
	  {

		  if (nolock_list_push(GetWifi2deviceList(), pResponsePacket) != NOLOCK_OPERATION_SUCCESS)
			 {
			    EJ_ErrPrintf(("[UARTCommands.c][Process_DeviceEasyLinkRequestCB][ERROR]: nolock_list_push failed.\r\n"));
			 }
				
      }else {

			EJ_ErrPrintf(("[UARTCommands.c][Process_DeviceEasyLinkRequestCB][ERROR]: noticeNetworkStatus failed.\r\n"));
      }

      if (request.configMode == WIFIMODULE_EASYCONNECT_MODE) {

			//PutEasylinkRequestSem();
			EJ_PutEventSem(EJ_EVENT_easylinkRequestSem);	
      }else if (request.configMode == WIFIMODULE_SOFTAP_MODE) {
			
			//PutSoftApConfigRequestSem();
			EJ_PutEventSem(EJ_EVENT_softAPRequestSem);
				
      }else if (request.configMode == WIFIMODULE_AIRKISS_MODE) {

			EJ_PutEventSem(EJ_EVENT_airkissRequestSem);
      }
    }		
		
    //AJ_AlwaysPrintf(("[UARTCommands.c][Process_EasyLinkRequestCB][INFO]: receive an EasyLink Request Packet.\r\n"));
  }

  return 0;
}



typedef struct {

  uint8_t response;
	
}PacketWifiModuleRebootResponse;


uint8_t Process_DeviceRebootRequestCB(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    uart2WifiPacket *pResponsePacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

    if (pResponsePacket) {

      pResponsePacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketWifiModuleRebootResponse));

      if (pResponsePacket->data) {
				
	FillCommonUart2WifiPacket(pResponsePacket, UART2WIFICOMMANDS_REBOOT_RESPONSE, UART2WIFIPACKET_FIXED_LENGTH + sizeof(PacketWifiModuleRebootResponse), DATA_WITH_NO_CRYPT | RESPONSION_BITS);

	PacketWifiModuleRebootResponse response;
	response.response = 0;
				
	memcpy(pResponsePacket->data, (uint8_t *)&response, sizeof(PacketWifiModuleRebootResponse));

	if (nolock_list_push(GetWifi2deviceList(), pResponsePacket) != NOLOCK_OPERATION_SUCCESS)
	  {
	    EJ_ErrPrintf(("[UARTCommands.c][Process_DeviceRebootRequestCB][ERROR]: nolock_list_push failed.\r\n"));
	  }
      }
    }

  //  PutRebootRequestSem();

	EJ_PutEventSem(EJ_EVENT_rebootRequestSem);
  }


  return 0;
}

typedef struct {

  uint8_t response;

}PacketWifiModuleResetToFactoryResponse;

uint8_t Process_DeviceResetToFactoryRequestCB(uart2WifiPacket *pPacket)
{
  if (pPacket) {

    uart2WifiPacket *pResponsePacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

    if (pResponsePacket) {

      pResponsePacket->data = (uint8_t *)EJ_mem_malloc(sizeof(PacketWifiModuleResetToFactoryResponse));

      if (pResponsePacket->data) {
				
	FillCommonUart2WifiPacket(pResponsePacket, UART2WIFICOMMANDS_RESET2FACTORY_RESPONSE, UART2WIFIPACKET_FIXED_LENGTH + sizeof(PacketWifiModuleResetToFactoryResponse), DATA_WITH_NO_CRYPT | RESPONSION_BITS);

	PacketWifiModuleResetToFactoryResponse response;
	response.response = 0;
				
	memcpy(pResponsePacket->data, (uint8_t *)&response, sizeof(PacketWifiModuleResetToFactoryResponse));

	if (nolock_list_push(GetWifi2deviceList(), pResponsePacket) != NOLOCK_OPERATION_SUCCESS)
	  {
	    EJ_ErrPrintf(("[UARTCommands.c][Process_DeviceResetToFactoryRequestCB][ERROR]: nolock_list_push failed.\r\n"));
	  }
      }
    }

    //PutReset2FactoryRequestSem();

	EJ_PutEventSem(EJ_EVENT_Reset2FactoryRequestSem);

	
  }

	
  return 0x00;
}


uart2WifiPacket * queryDeviceStatus()
{
  uart2WifiPacket *pPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));

  if (pPacket) {

    FillCommonUart2WifiPacket(pPacket, UART2WIFICOMMANDS_QUERY_DEVICE_STATUS_REQUEST, UART2WIFIPACKET_FIXED_LENGTH, DATA_WITH_NO_CRYPT);

    pPacket->data = NULL;

  }else {

    EJ_ErrPrintf(("[UARTCommands.c][queryDeviceStatus][ERROR]: EJ_mem_malloc pPacket failed.\r\n"));
  }

  return pPacket;

}



void initUARTCommands() {
	
  registerUartCommandCallback(UART2WIFICOMMANDS_READ_QOS_RESPONSE, Process_ReadDeviceQosResponseCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_READ_DEVICESN_RESPONSE, Process_readDeviceSNResponseCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_READ_DEVICEINFO_RESPONSE, Process_GetDeviceInfoResponseCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_SET_UART_BAUD_RESPONSE, Process_SettingUartBaudRequestCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_CHECK_NETWORK_STATE_REQUEST, Process_CheckNetworkStatusRequestCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_SYNC_SYSTEMTIME_RESPONSE, Process_DeviceSyncSystemTimeRequestCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_QUERY_DEVICE_VERSION_RESPONSE, Process_queryDeviceVersionResponseCB);//0x61 uart?ìó|′|àí
  registerUartCommandCallback(UART2WIFICOMMANDS_QUERY_DEVICE_UPDATE_RESPONSE, Process_judgeDeviceOTAResponseCB);//0x62 uart?ìó|′|àí
  registerUartCommandCallback(UART2WIFICOMMANDS_ACTIVE_DEVICE_UPDATE_RESPONSE, Process_ActiveDeviceOtaResponseCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_WIFI_POWER_REQUEST, ProcessWifiPowerRequest);
  registerUartCommandCallback(UART2WIFICOMMANDS_RENAME_WIFISSID_REQUEST, Process_RenameWifiSSIDRequestCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_EASY_LINK_REQUEST, Process_DeviceEasyLinkRequestCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_REBOOT_RESPONSE, Process_DeviceRebootRequestCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_RESET2FACTORY_RESPONSE, Process_DeviceResetToFactoryRequestCB);
  registerUartCommandCallback(UART2WIFICOMMANDS_READ_WIFIMODULE_PROPERTY_REQUEST, Process_DeviceGetWifiModulePropertyRequestCB);
}







