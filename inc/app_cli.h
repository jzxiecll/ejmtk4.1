#ifndef __APP_CLI_H__
#define __APP_CLI_H__


#ifdef __cplusplus
 extern "C" {
#endif



#define EJ_CLI_RETURN_STRING(ret) ((ret>=0)?"Success":"Error")



unsigned char ej_smart_config_test(unsigned char len, unsigned char *param[]);


#define EJ_CLI_SMNT_ENTRY     { "ejsmart",   "Ejcloud Smart Connection", ej_smart_config_test },




#ifdef __cplusplus
 }
#endif



#endif /* __APP_CLI_TABLE_H__ */

