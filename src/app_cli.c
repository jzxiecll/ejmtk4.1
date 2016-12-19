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


