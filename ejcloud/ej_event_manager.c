#include "ej_event_manager.h"

ej_queue_t *ejEventQueue;

static ej_queue_pool_define(ejEventQueueData, 10*sizeof(uint8_t*));

int  EJ_initEventManager()
{

	int  ret = 0;	
	ejEventQueue  =  (ej_queue_t *)EJ_mem_malloc(sizeof(ej_queue_t));
	if(ejEventQueue != NULL){		
		if (EJ_queue_create(ejEventQueue, "ejEventQueue", sizeof(void *) , &ejEventQueueData) != EJ_SUCCESS) {	
		  ret = 1;	
		  EJ_ErrPrintf(("EJ_initEventManager failed.\r\n"));
		}else{
		  EJ_DebugPrintf(("EJ_initEventManager success.\r\n"));
		}
	}else{
		EJ_DebugPrintf(("EJ_initEventManager failed for malloc.\r\n"));
		ret = -1 ;
	}
	return ret;
}



int  EJ_PutEventSem( ej_event_t EventItem)
{
	//ej_event_t Item = EventItem ;
	int ret = 0 ;
	uint8_t msg = EventItem ; 
	ret = EJ_queue_send(ejEventQueue, &msg, EJ_msec_to_ticks(1));	
	EJ_DebugPrintf(("[EJ_PutEventSem][EJ_queue_send] is %d ret=%d\r\n",msg,ret));		
	return  ret;	
}




