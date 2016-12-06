#include "nolock-list.h"


nolock_list *cloud2wifiList, *wifi2cloudList, *wifi2deviceList, *device2wifiList, *cloud2deviceList, *device2cloudList;

nolock_list *lan2deviceList, *device2lanList, *lan2wifiList, *wifi2lanList;

nolock_list *udp2wifiList, *wifi2udpList;

nolock_list *wifi2deviceAckList, *device2wifiAckList,  *device2wifiFirmwareOtaAckList, *device2wifiFirmwareOtaactiveAckList;

uint8_t InitNolockLists()
{
	uint8_t ret = 0x00;

	cloud2wifiList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!cloud2wifiList) {

		return ret;
	}
	wifi2cloudList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!wifi2cloudList) {

		return ret;
	}
	wifi2deviceList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!wifi2deviceList) {

		return ret;
	}
	device2wifiList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!device2wifiList) {

		return ret;
	}
	cloud2deviceList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!cloud2deviceList) {

		return ret;
	}
	device2cloudList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!device2cloudList) {

		return ret;
	}
	lan2deviceList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!lan2deviceList) {

		return ret;
	}
	device2lanList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!device2lanList) {

		return ret;
	}
	lan2wifiList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!lan2wifiList) {

		return ret;
	}
	wifi2lanList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!wifi2lanList) {

		return ret;
	}

	udp2wifiList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!udp2wifiList) {

		return ret;
	}
	wifi2udpList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!wifi2udpList) {

		return ret;
	}

	wifi2deviceAckList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!wifi2deviceAckList) {

		return ret;
	}
	device2wifiAckList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!device2wifiAckList) {

		return ret;
	}

	device2wifiFirmwareOtaAckList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!device2wifiFirmwareOtaAckList) {

		return ret;
	}

	device2wifiFirmwareOtaactiveAckList = (nolock_list *)EJ_mem_malloc(sizeof(nolock_list));
	if (!device2wifiFirmwareOtaactiveAckList) {

		return ret;
	}

	new_nolock_list(cloud2wifiList, 10);
	new_nolock_list(wifi2cloudList, 10);
	new_nolock_list(wifi2deviceList, 10);
	new_nolock_list(device2wifiList, 10);
	new_nolock_list(cloud2deviceList, 10);
	new_nolock_list(device2cloudList, 10);
	new_nolock_list(lan2deviceList, 10);
	new_nolock_list(device2lanList, 10);
	new_nolock_list(lan2wifiList, 10);
	new_nolock_list(wifi2lanList, 10);

	new_nolock_list(udp2wifiList, 10);
	new_nolock_list(wifi2udpList, 10);

	new_nolock_list(wifi2deviceAckList, 10);
	new_nolock_list(device2wifiAckList, 10);

	new_nolock_list(device2wifiFirmwareOtaAckList, 10);
	new_nolock_list(device2wifiFirmwareOtaactiveAckList, 10);

	return 0x01;
}

uint8_t new_nolock_list(nolock_list *list, int length)
{
	uint8_t ret = 0x00;

	list->data = EJ_mem_malloc(sizeof(void *)* length);

	if (list->data != NULL)
	{
		list->r_cursor = 0;
		list->w_cursor = 0;
		list->length = length;

		ret = 0x01;
	}

	return ret;
}

uint8_t free_nolock_list(nolock_list *list)
{
	uint8_t ret = 0x00;

	if (list->data != NULL)
	{
		EJ_mem_free(list->data);

		list->data = NULL;
	}

	return ret;
}

uint8_t nolock_list_push(nolock_list *list, void *data)
{
	int w_cursor = 0;
	uint8_t ret = 0x00;

	w_cursor = (list->w_cursor + 1) % list->length;

	if (w_cursor != list->r_cursor)
	{
		((void **)list->data)[list->w_cursor] = data;
		list->w_cursor = w_cursor;

		ret = 0x01;
	}

	return ret;

}

uint8_t nolock_list_pop(nolock_list *list, void **data)
{
	uint8_t ret = 0x00;


	if (list->r_cursor != list->w_cursor)
	{
		*data = ((void **)list->data)[list->r_cursor];
		list->r_cursor = (list->r_cursor + 1) % list->length;

		ret = 0x01;
	}

	return ret;

}

nolock_list * GetCloud2wifiList()
{
	return cloud2wifiList;
}
nolock_list * GetWifi2cloudList()
{
	return wifi2cloudList;
}
nolock_list * GetWifi2deviceList()
{
	return wifi2deviceList;
}
nolock_list * GetDevice2wifiList()
{
	return device2wifiList;
}
nolock_list * GetCloud2deviceList()
{
	return cloud2deviceList;
}
nolock_list * GetDevice2cloudList()
{
	return device2cloudList;
}

nolock_list * GetWifi2lanList()
{
	return wifi2lanList;
}
nolock_list * GetLan2wifiList()
{
	return lan2wifiList;
}
nolock_list * GetLan2deviceList()
{
	return lan2deviceList;
}
nolock_list * GetDevice2lanList()
{
	return device2lanList;
}

nolock_list * GetUdp2wifiList()
{
	return udp2wifiList;
}
nolock_list * GetWifi2udpList()
{
	return wifi2udpList;
}

nolock_list * GetWifi2deviceAckList()
{
	return wifi2deviceAckList;
}
nolock_list * GetDevice2wifiAckList()
{
	return device2wifiAckList;
}

nolock_list * GetDevice2FirmwareOtaAckList()
{
	return device2wifiFirmwareOtaAckList;
}

nolock_list * GetDevice2FirmwareOtaActiveAckList()
{
	return device2wifiFirmwareOtaactiveAckList;
}


