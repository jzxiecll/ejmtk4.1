#include "app_cli.h"

unsigned char ej_smart_config_test(unsigned char len, unsigned char *param[])
{
    unsigned char status = 0;
    signed int ret = 0;

    if (len == 1) {
        if (!strcmp(param[0], "connect")) {
            ret = joylink_smart_connect();
        } else if (!strcmp(param[0], "stop")) {
            ret = joylink_smart_stop();
        } else {
            EJ_Printf("Not supported cmd\n");
            return 1;
        }
        if (ret < 0) {
            status = 1;
        }
        EJ_Printf("ej_smart_config_test(), ret:%s, Code=%ld\n", EJ_CLI_RETURN_STRING(ret), ret);
        return status;
    } 
    return 0;
}

static unsigned char ej_cli_read_item(unsigned char *keyname)
{
	char tmp[256];
    int  nvdm_len = sizeof(tmp);
    int  status;
    status = EJ_read_psm_item(keyname,(unsigned char *)tmp,nvdm_len);
    if (status == 0) {
        printf("%s",keyname);
        printf(" = ");
        printf("%s",tmp);
        printf("\r\n");
    } else {
        printf("the data item is not exist");
        printf("\r\n");
    }
    return 0;
}

static unsigned char ej_cli_write_item(unsigned char *keyname)
{

    return 0;
}


unsigned char ej_cli_uuid()
{
  	ej_cli_read_item("UUID");
    return 0;
}


unsigned char ej_cli_Deviceid()
{
  	ej_cli_read_item("Deviceid");
    return 0;
}


 void ej_cli_modify_server(int argc, char *argv[])
{
	if (argc == 3) {
		char *serverAddr = argv[1];		
		int port = atoi(argv[2]);
		SetWifiConfigCloudDomainAddr(serverAddr);
		SetWifiConfigMasterServerPort(port);
		EJ_AlwaysPrintf(("[INFO]: modify server success!\r\n"));
	}else {
		EJ_ErrPrintf(("[ERROR]: modify server error argc!\r\n"));
	}
}

 void ej_cli_modify_clouddomain_addr(int argc, char *argv[])
{
	if (argc == 3) {
		char *cloudDomainAddr = argv[1];		
		SetWifiConfigCloudDomainAddr(cloudDomainAddr);
		EJ_AlwaysPrintf(("[INFO]: modify cloud domain addr success!\r\n"));
	}else {
		EJ_ErrPrintf(("[ERROR]: modify cloud domain addr error argc!\r\n"));
	}
}

 void ej_cli_set_log_level(int argc, char *argv[])
{
  if (argc == 2) {
    char *pLogLevel = argv[1];
    int level = atoi(pLogLevel);
    EJ_DbgLevel = level;
    EJ_AlwaysPrintf(("[INFO]: set log level success!\r\n"));		
  }else {
	EJ_ErrPrintf(("[ERROR]: set log level error argc!\r\n"));    
  }
}

void ej_cli_get_softwareversion(int argc, char *argv[])
{
	char  data[15]={0};
	char  moduleverinfo[10] = {0};
	char s_month[5];
    int  month, day, year;
    char mVersion[50] = "WF_MTK_7687_3.3V_EJ_0101";
		
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    sscanf(__DATE__, "%s %d %d", s_month, &day, &year);

    month = (strstr(month_names, s_month)-month_names)/3+1;

	sprintf(data,"%04d%02d%02d",year,month,day);

	WIFIModuleVersion Version = GetWifiModuleInfoVersion();

	sprintf(moduleverinfo,"%d%d%d%d%d",Version.hardwareVersion,Version.softwareVersionMsb,Version.softwareVersionLsb,Version.publishYear,Version.publishWeek);

	printf("[SoftWare_Version]: %s%s_%s_%s\r\n",mVersion,moduleverinfo,"00",data);	
}


 void ej_cli_wifi_fota_http(int argc, char *argv[])
{

	//EJ_Printf("xxxxxxxx fota argc=%d,argv[0]=%s\r\n",argc,argv[0]);
	if (argc == 1) {
		EJ_wifi_fota_task(argv[0]);
	}else {
		EJ_ErrPrintf(("[ERROR]: wifi fota http error argc!\r\n"));
	}
}


