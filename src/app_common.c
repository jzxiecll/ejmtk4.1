/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "wifi_api.h"
#include "syslog.h"
//#include "lwip_network.h"
//#include "network_init.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ej_main.h"
#include "syslog.h"
#include "ej_event_manager.h"
#include "ej_wificonfig.h"
#include "app_common.h"
log_create_module(EJCLOUD, PRINT_LEVEL_INFO);




bool g_wifi_init_ready = false;
ej_queue_t* ej_iot_event_queue = NULL;


 int32_t event_normal_connect_failed(wifi_event_t event,
                                      uint8_t *payload,
                                      uint32_t length)
{

	EJ_Printf("Application Error: Connection Failed!\r\n");
	return 0;
}

 int32_t event_normal_disconnected(wifi_event_t event,
                                      uint8_t *payload,
                                      uint32_t length)
{
	EJ_Printf("Application Error: disconnected !\n");
	return 0;
}



 int32_t event_normal_connected(wifi_event_t event,
                                      uint8_t *payload,
                                      uint32_t length)
{
	
	return 0;
}


int32_t user_wifi_init_complete_handler(wifi_event_t event,
                                      uint8_t *payload,
                                      uint32_t length)
{
	EJ_Send_IotEvent(EJ_IOT_EVENT_MAINLOOP);
    EJ_Printf("EJCLOUD :WiFi Init Done: port = %d\r\n", payload[6]);
    return 1;
}

int32_t user_wifi_status_ipready(void)
{
	//EJ_Send_IotEvent(EJ_IOT_EVENT_MAINLOOP);
	EJ_PutEventSem(EJ_EVENT_routerConnectedSem);
	EJ_App_network_configured(1);
    return 1;
}


static ej_queue_pool_define(EJ_IOT_EventQueueData, 10*sizeof(uint8_t*));

int  EJ_IOT_EventManager_Init()
{

	int  ret = 0;	
	ej_iot_event_queue  =  (ej_queue_t *)EJ_mem_malloc(sizeof(ej_queue_t));
	if(ej_iot_event_queue != NULL){		
		if (EJ_queue_create(ej_iot_event_queue, "User_App_EventQueueData", sizeof(void *) , &EJ_IOT_EventQueueData) != EJ_SUCCESS) {	
		  ret = 1;	
		  EJ_ErrPrintf(("EJ_IOT_EventManager_Init failed.\r\n"));
		}else{
		  EJ_DebugPrintf(("EJ_IOT_EventManager_Init success.\r\n"));
		}
	}else{
		EJ_DebugPrintf(("EJ_IOT_EventManager_Init failed for malloc.\r\n"));
		ret = -1 ;
	}
	return ret;
}



int  EJ_Send_IotEvent( EJ_Iot_event_t EventItem)
{
	
	int ret = 0 ;
	EJ_Iot_event_t  msg = EventItem ; 

	if(!ej_iot_event_queue)
	{
		EJ_IOT_EventManager_Init();
	}	
	ret = EJ_queue_send(ej_iot_event_queue, &msg, EJ_msec_to_ticks(1));
	EJ_DebugPrintf(("[EJ_Send_IotEvent][EJ_queue_send] is %d ret=%d\r\n",msg,ret));		
	return  ret;	
}



static signed int  EJ_Iot_Init()
{

		EJ_IOT_EventManager_Init();
		EJ_time_init();
		InitPacketQUEUE();//Init cloud<-->device Queue
		EJ_init_dev();	
		InitUARTCommandDataID2NetCommandDataIDMaps();
		EJ_EventManagerInit(); 
		Init_InfoManagement(); 
		initCommandsCallbackManager();
		initMQTTCommands();
		initUARTCommands();


		int ret = EJ_Aes_init();
		if (ret != EJ_SUCCESS) {
			EJ_ErrPrintf(("[initMainLoop][ERROR]: Unable to initialize AES engine.\r\n"));
			return  1;
		}
		if (InitNolockLists() != 0x01)
		{
			EJ_ErrPrintf(("[initMainLoop][ERROR]: Unable to init nolock list.\r\n"));
			return  1;
		}


		uart2WifiPacket *pUart2WifiPacket = (uart2WifiPacket*)readDeviceQos();
		if (!pUart2WifiPacket) {
			return  1;
		}
		if (nolock_list_push(GetWifi2deviceList(), pUart2WifiPacket) != 0x01)
		{	
			EJ_PacketUartFree(pUart2WifiPacket);
			EJ_ErrPrintf(("[initMainLoop][ERROR]: add packet to wifi2devicelist failed.\r\n"));
		}

		
		EJ_UartThreadStart();

		return 0;


}


static void EJ_Iot_Check_Configured(unsigned char   *isHomeAPConfig)
{

	//unsigned char ej_configured =0 ;
	//joycfg_profile_get_configfree(&ej_configured);	
	EJ_App_network_configured_status(isHomeAPConfig);
	EJ_Printf("ej_configured = %d\r\n",*isHomeAPConfig);

//	*isHomeAPConfig = ej_configured;
//	if(!ej_configured)
//	{
//		EJ_Send_IotEvent(EJ_IOT_EVENT_UNCONFIGED);
//		//EJ_PutEventSem(EJ_EVENT_easylinkRequestSem);
//	}

	//EJ_Send_IotEvent(EJ_IOT_EVENT_MAINLOOP);

}

ej_timer_t   app_event_timer;


static void EJ_Iot_TaskTimerCB()
{

	EJ_Iot_Status_t ist = 0 ;

	switch(ist)
	{
		


	}


}

void EJ_Iot_EventTimerCreate()
{
	if (EJ_timer_create(&app_event_timer,
			    "app_event_timer",
			    EJ_msec_to_ticks(500),
			    &EJ_Iot_TaskTimerCB,
			    NULL,
			    EJ_TIMER_PERIODIC,
			    EJ_TIMER_AUTO_ACTIVATE) != EJ_SUCCESS) {
		EJ_ErrPrintf(("[app_common.c][User_App_EventTimerCreate][ERROR]: Failed to create app_event_timer timer.\r\n"));
	}
}

void user_wifi_app_entry(void *args)
{
   
	EJ_Printf("EJCLOUD :%s:%d\r\n",__FUNCTION__,__LINE__);	
	//1.	 init platform
	if(EJ_Iot_Init()!=0)
	{
		EJ_App_reboot(0);
	}


	//2.	 check  configured
	unsigned char   isHomeAPConfig = 0;
	EJ_App_network_configured_status(&isHomeAPConfig);
	//EJ_Iot_Check_Configured(&isHomeAPConfig);


	EJ_Printf("Platform Event Process!\r\n");
	while(1)
	{
		EJ_Iot_event_t event=0;
		if(EJ_queue_recv(ej_iot_event_queue, &event, EJ_msec_to_ticks(10))==EJ_SUCCESS)
		{
			EJ_Printf("Get Platform Event = %d !\r\n", event);
			switch(event)
			{
				case EJ_IOT_EVENT_MAINLOOP:
					initMainLoop(isHomeAPConfig);
					break;
				case EJ_IOT_EVENT_UNCONFIGED:
					joylink_smart_connect(WIFICONFIG_EZCONNECT_MODE);
					break;
				default:				
					break;

			}		
		}
		
			
	}
		
}


