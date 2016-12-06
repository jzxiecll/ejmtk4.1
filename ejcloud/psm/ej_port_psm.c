#include "ej_port_psm.h"
#include "nvdm.h"

/**
*
*need youself porting flash read/wirte function
*
***/
int EJ_write_psm_item(const char *keyname,const uint8_t *value)
{

		
		return nvdm_write_data_item(
        "EJCLOUD", 
        keyname, 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (uint8_t *)value, 
        strlen(value));
		  
}



int EJ_read_psm_item(const char *keyname, uint8_t *value,unsigned int value_len)
{


	 int status = nvdm_read_data_item("EJCLOUD", keyname,value,&value_len);
	 if('\0' == *value)//???????
        return 1;
	return status;
}


int EJ_read_sysitem(const char *keyname, uint8_t *value,unsigned int value_len)
{

	 int read_len = nvdm_read_data_item("EJCLOUD", keyname,
					value, &value_len);
	if (read_len >= 0) {
		value[read_len] = 0;
		return EJ_SUCCESS;
	}

	return -EJ_FAIL;
}


int EJ_psm_init()
{

	return nvdm_init();	
}




