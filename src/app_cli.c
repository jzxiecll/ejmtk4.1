#include "ej_porting_layer.h"
#include "ej_wificonfig.h"
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

unsigned char ej_cli_read_item(unsigned char *keyname)
{
	char tmp[256];
    int  nvdm_len = sizeof(tmp);
    int  status;
    status = EJ_read_psm_item(keyname,
                                     (unsigned char *)tmp,
                                     nvdm_len);
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

unsigned char ej_cli_write_item(unsigned char *keyname)
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

