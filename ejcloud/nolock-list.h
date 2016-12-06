#ifndef __NOLOCK_LIST_H__
#define __NOLOCK_LIST_H__

#include "ej_porting_layer.h"


typedef enum {

	NOLOCK_OPERATION_FAILED = 0x00,
	NOLOCK_OPERATION_SUCCESS,
};

typedef struct {

	int length;

	int r_cursor;

	int w_cursor;

	void *data;

}nolock_list;

uint8_t new_nolock_list(nolock_list *list, int length);
uint8_t free_nolock_list(nolock_list *list);
uint8_t nolock_list_push(nolock_list *list, void *data);
uint8_t nolock_list_pop(nolock_list *list, void **data);

uint8_t InitNolockLists();

nolock_list * GetCloud2wifiList();
nolock_list * GetWifi2cloudList();
nolock_list * GetWifi2deviceList();
nolock_list * GetDevice2wifiList();
nolock_list * GetCloud2deviceList();
nolock_list * GetDevice2cloudList();
nolock_list * GetWifi2lanList();
nolock_list * GetLan2wifiList();
nolock_list * GetLan2deviceList();
nolock_list * GetDevice2lanList();

nolock_list * GetUdp2wifiList();
nolock_list * GetWifi2udpList();

nolock_list * GetWifi2deviceAckList();
nolock_list * GetDevice2wifiAckList();



#endif //_H_WIFIMODULE_NOLOCK_LIST_H_
