#ifndef __EJ_EVENT_MANAGER_H__
#define __EJ_EVENT_MANAGER_H__

#include "ej_porting_layer.h"

typedef enum
{

	EJ_EVENT_shouldUARTThreadWorkSem =1 ,
	EJ_EVENT_shouldMQTTThreadWorkSem,	
	EJ_EVENT_ezconnectCompletedSem,
	EJ_EVENT_routerConnectedSem,
	EJ_EVENT_broadcastRequestSem,	
	EJ_EVENT_broadcastResponseSem,	
	EJ_EVENT_uapStartedSem,	
	EJ_EVENT_easylinkRequestSem,	
	EJ_EVENT_softAPRequestSem,	
	EJ_EVENT_airkissRequestSem,	
	EJ_EVENT_MQTTConnectionLostSem,	
	EJ_EVENT_rebootRequestSem,	
	EJ_EVENT_Reset2FactoryRequestSem,	
	EJ_EVENT_routerAuthFailedSem,	
	EJ_EVENT_reApplyIDSem,
	EJ_EVENT_timeSyncSem,
	EJ_EVENT_routerConnectingSem,


}ej_event_t;

uint8_t  initEventManager();
int  EJ_initEventManager();


uint8_t putShouldUARTThreadWorkSem();
uint8_t getShouldUARTThreadWorkSem();
uint8_t putShouldMQTTThreadWorkSem();
uint8_t getShouldMQTTThreadWorkSem();
uint8_t GetEzConnectCompletedSem();
uint8_t PutEzConnectCompletedSem();
uint8_t GetRouterConnectedSem();
uint8_t PutRouterConnectedSem();

uint8_t GetBroadcastRequestSem();
uint8_t PutBroadcastRequestSem();

uint8_t GetBroadcastResponseSem();
uint8_t PutBroadcastResponseSem();

uint8_t GetUapStartedSem();
uint8_t PutUapStartedSem();

uint8_t GetEasylinkRequestSem();
uint8_t PutEasylinkRequestSem();

uint8_t GetMQTTConnectionLostSem();
uint8_t PuttMQTTConnectionLostSem();

uint8_t GetSoftApConfigRequestSem();
uint8_t PutSoftApConfigRequestSem();


uint8_t GetAirkissConfigRequestSem();
uint8_t PuttAirkissConfigRequestSem();

uint8_t GetRebootRequestSem();
uint8_t PutRebootRequestSem();

uint8_t GetReset2FactoryRequestSem();
uint8_t PutReset2FactoryRequestSem();

uint8_t GetRouterAuthFailedSem();
uint8_t PutRouterAuthFailedSem();

uint8_t GetReApplyIDSem();
uint8_t PutReApplyIDSem();

uint8_t GetTimeSyncSem();
uint8_t PutTimeSyncSem();



#endif //_H_WIFIMODULE_EVENT_MANAGER_H_


