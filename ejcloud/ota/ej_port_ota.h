#ifndef __EJ_PORT_OTA_H__
#define __EJ_PORT_OTA_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "ej_porting_layer.h"

#include "fota.h"
#include "fota_config.h"
#include "fota_76x7.h"
#include "fota_download_interface.h"
#ifdef MTK_FOTA_DUAL_IMAGE_ENABLE
#include "fota_dual_image.h"
#endif

int EJ_wifi_fota_task(void *arg);
void EJ_wifi_fota_trigger_update();
int EJ_wifi_firmware_download(const char *url_str, uint32_t datasize);
int EJ_device_firmware_download(const char *url_str, uint32_t datasize);
#ifdef __cplusplus
}
#endif

#endif  //__EJ_PORT_OTA_H__