#ifndef __APP_CLI_H__
#define __APP_CLI_H__

#include "ej_porting_layer.h"
#include "ej_wificonfig.h"
#include "WifiModuleStatus.h"
#include "ej_packet.h"
#ifdef __cplusplus
 extern "C" {
#endif



#define EJ_CLI_RETURN_STRING(ret) ((ret>=0)?"Success":"Error")



unsigned char ej_smart_config_ezconnect(unsigned char len, unsigned char *param[]);
unsigned char ej_smart_config_airkiss(unsigned char len, unsigned char *param[]);

unsigned char ej_cli_Deviceid();
unsigned char ej_cli_uuid();
void ej_cli_modify_server(int argc, char *argv[]);
void ej_cli_modify_clouddomain_addr(int argc, char *argv[]);
void ej_cli_set_log_level(int argc, char *argv[]);
void ej_cli_get_softwareversion(int argc, char *argv[]);
void ej_cli_wifi_fota_http(int argc, char *argv[]);
void ej_cli_ejcloud_item_read(int argc, char *argv[]);


#define EJ_CLI_AIRKISS_ENTRY     { "airkiss",   "<Ejcloud airkiss Connection>", ej_smart_config_airkiss },
#define EJ_CLI_EZCONNECT_ENTRY     { "ezconnect",   "<Ejcloud ezconnect Connection>", ej_smart_config_ezconnect },
#define EJ_CLI_DID      { "did",   "<Ejcloud Get Deviceid>", ej_cli_Deviceid},
#define EJ_CLI_UUID     { "uuid",   "<Ejcloud Get Uuid>", ej_cli_uuid},

#define EJ_CLI_MSA 		{"ms", "<Ejcloud modify server addr>", ej_cli_modify_server},
#define EJ_CLI_MDA		{"md", "<Ejcloud modify cloud domain addr>", ej_cli_modify_clouddomain_addr},
#define EJ_CLI_MLL		{"mll", "<Ejcloud modify log level: 4--cloud,5--uart and cloud>", ej_cli_set_log_level},
#define EJ_CLI_SOFTVER  {"version", "<Ejcloud Get Software Version>", ej_cli_get_softwareversion},
#define EJ_CLI_EJ_ITEM  {"ejcloud", "<Ejcloud Wifi fota http url>", ej_cli_ejcloud_item_read},

#define EJ_CLI_FOTA_HTTP  {"updatefw", "<Ejcloud Wifi fota http url>", ej_cli_wifi_fota_http},


#define EJ_CLI_CMDS     EJ_CLI_EZCONNECT_ENTRY\
						EJ_CLI_AIRKISS_ENTRY\
						EJ_CLI_DID\
						EJ_CLI_UUID\
						EJ_CLI_MSA\
						EJ_CLI_MDA\
						EJ_CLI_MLL\
						EJ_CLI_FOTA_HTTP\
						EJ_CLI_SOFTVER \
						EJ_CLI_EJ_ITEM \


#ifdef __cplusplus
 }
#endif



#endif /* __APP_CLI_TABLE_H__ */

