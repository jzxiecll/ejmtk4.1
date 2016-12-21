#ifndef __APP_CLI_H__
#define __APP_CLI_H__

#include "ej_porting_layer.h"
#include "ej_wificonfig.h"
#include "WifiModuleStatus.h"

#ifdef __cplusplus
 extern "C" {
#endif



#define EJ_CLI_RETURN_STRING(ret) ((ret>=0)?"Success":"Error")



unsigned char ej_smart_config_test(unsigned char len, unsigned char *param[]);
unsigned char ej_cli_Deviceid();
unsigned char ej_cli_uuid();
void ej_cli_modify_server(int argc, char *argv[]);
void ej_cli_modify_clouddomain_addr(int argc, char *argv[]);
void ej_cli_set_log_level(int argc, char *argv[]);
void ej_cli_get_softwareversion(int argc, char *argv[]);



#define EJ_CLI_SMNT_ENTRY     { "ejsmart",   "<Ejcloud Smart Connection>", ej_smart_config_test },
#define EJ_CLI_DID      { "did",   "<Ejcloud Get Deviceid>", ej_cli_Deviceid},
#define EJ_CLI_UUID     { "uuid",   "<Ejcloud Get Uuid>", ej_cli_uuid},

#define EJ_CLI_MSA 		{"ms", "<Ejcloud modify server addr>", ej_cli_modify_server},
#define EJ_CLI_MDA		{"md", "<Ejcloud modify cloud domain addr>", ej_cli_modify_clouddomain_addr},
#define EJ_CLI_MLL		{"mll", "<Ejcloud modify log level>", ej_cli_set_log_level},
#define EJ_CLI_SOFTVER  {"version", "<Ejcloud Get Software Version>", ej_cli_get_softwareversion},



#define EJ_CLI_CMDS     EJ_CLI_SMNT_ENTRY\
						EJ_CLI_DID\
						EJ_CLI_UUID\
						EJ_CLI_MSA\
						EJ_CLI_MDA\
						EJ_CLI_MLL\


#ifdef __cplusplus
 }
#endif



#endif /* __APP_CLI_TABLE_H__ */

