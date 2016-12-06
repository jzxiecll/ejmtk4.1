#include "ej_port_time.h"
#include "hal_rtc.h"


#define rtc_time_set  
#define rtc_time_get
#define rtc_init  
/*
*
*  RTC  interface  Time  porting 
*
*
*
*
*
*
*/
int EJ_time_set_posix(time_t time)
{

	/*
	*Need to Porting ...
	*/
	return 0;//rtc_time_set(time);
}

time_t EJ_time_get_posix(void)
{
	/*
	*Need to Porting ...
	*/
	return 0;//rtc_time_get();
}

int  EJ_time_init_posix()
{
		/*
	*Need to Porting ...
	*/
//	rtc_init();
	return 0 ;
}




