#include "ej_user_uart.h"



extern nolock_list  *wifi2deviceList, *device2wifiList;
static ej_thread_t UARTReceiveThread_thread;
static ej_thread_stack_define(UARTReceiveThread_stack, 2048);
static ej_thread_t UARTSendThread_thread;
static ej_thread_stack_define(UARTSendThread_stack, 2048);
ej_timer_t wifi2deviceTimer, cloud2deviceTimer, lan2deviceTimer;


void EJ_timerCB()
{

}

int EJ_InitUARTTimer()
{
	if (EJ_timer_create(&wifi2deviceTimer,
			    "wifi2deviceTimer",
			    EJ_msec_to_ticks(10000),
			    &EJ_timerCB,
			    NULL,
			    EJ_TIMER_ONE_SHOT,
			    EJ_TIMER_NO_ACTIVATE) != EJ_SUCCESS) {
		EJ_ErrPrintf(("[UARTThread.c][InitUARTTimer][ERROR]: Failed to create wifi2deviceTimer timer.\r\n"));
		return -EJ_FAIL;
	}

	if (EJ_timer_create(&cloud2deviceTimer,
			    "cloud2deviceTimer",
			    EJ_msec_to_ticks(100),
			    &EJ_timerCB,
			    NULL,
			    EJ_TIMER_ONE_SHOT,
			    EJ_TIMER_NO_ACTIVATE) != EJ_SUCCESS) {
		EJ_ErrPrintf(("[UARTThread.c][InitUARTTimer][ERROR]: Failed to create cloud2deviceTimer timer.\r\n"));
		return -EJ_FAIL;
	}


	if (EJ_timer_create(&lan2deviceTimer,
			    "lan2deviceTimer",
			    EJ_msec_to_ticks(100),
			    &EJ_timerCB,
			    NULL,
			    EJ_TIMER_ONE_SHOT,
			    EJ_TIMER_NO_ACTIVATE) != EJ_SUCCESS) {
		EJ_ErrPrintf(("[UARTThread.c][InitUARTTimer][ERROR]: Failed to create lan2deviceTimer timer.\r\n"));
		return -EJ_FAIL;
	}

	return EJ_SUCCESS;
}



static uart2WifiPacket *pWifi2UartPacket = NULL;
/**
 * Defines 
 * 
 *
 */

void EJ_UartSendThread(void* n)
{
	static EJ_UARTTHREAD_RXTX_STATE_MACHINE wifi2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_SEND;
	static EJ_UARTTHREAD_RXTX_STATE_MACHINE cloud2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_SEND;
	static EJ_UARTTHREAD_RXTX_STATE_MACHINE lan2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_SEND;

	for (;;)
	{
		
		uart2WifiPacket *pdevice2wifiAckPacket = NULL;		
		nolock_list_pop(GetDevice2wifiAckList(), (void **)&pdevice2wifiAckPacket);
		/* process wifi to device.*/	
		switch(wifi2deviceRxTxStateMachine) {
			case EJ_UARTTHREAD_RXTX_SEND:
			{
				if (!pWifi2UartPacket) {
					nolock_list_pop(wifi2deviceList, (void **)&pWifi2UartPacket);
				}
				
				if (pWifi2UartPacket) {
					EJ_PublishUart2WifiPacket(pWifi2UartPacket);
					EJ_PrintUart2WifiPacket(pWifi2UartPacket, "[Uart2WifiSend]");
					if (((pWifi2UartPacket->crypt & QOS_BITS) && (GetDeviceInfoQos())) || (GetDeviceInfoQos() == 0x01)) {
						/* start timer... */
						if (EJ_timer_activate(&wifi2deviceTimer) != EJ_SUCCESS) {

							EJ_ErrPrintf(("[UARTThread.c][UARTSendThread][ERROR]: start wifi2deviceTimer failed.\r\n"));
						}
						wifi2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_WAIT_FOR_ACK;
					}else {
						EJ_PacketUartFree(pWifi2UartPacket);
						pWifi2UartPacket = NULL;
					}
				}
				break;
			}
			case EJ_UARTTHREAD_RXTX_WAIT_FOR_ACK:
			{
				if (((pWifi2UartPacket->crypt & QOS_BITS) && (GetDeviceInfoQos())) || (GetDeviceInfoQos() == 0x01)) {
					pWifi2UartPacket->crypt |= QOS_BITS;
					if (EJ_timer_is_running(&wifi2deviceTimer) == true) {
						if (pdevice2wifiAckPacket) {
							if (AckPacketCompare(pWifi2UartPacket, pdevice2wifiAckPacket)) {							
								EJ_timer_deactivate(&wifi2deviceTimer);
								EJ_PacketUartFree(pWifi2UartPacket);			
								pWifi2UartPacket = NULL;
								/* change the state machine to */
								wifi2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_SEND;
								EJ_PacketUartFree(pdevice2wifiAckPacket);
								pdevice2wifiAckPacket = NULL;
							}
						}

					}else {
						/* modify thr DUP bit to indicate this packet is re-send. */
						pWifi2UartPacket->crypt |= (1 << DUP_BITS);
						wifi2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_SEND;
					}
				}		
				break;
			}
			default:
				break;

		}


		/* process cloud to device.*/	
		wifi2CloudPacket *pCloud2DevicePacket = NULL;
		static uart2WifiPacket *pUart2WifiPacket = NULL;
		switch(cloud2deviceRxTxStateMachine) {
			case EJ_UARTTHREAD_RXTX_SEND:
			{
				if (!pUart2WifiPacket) {
					nolock_list_pop(GetCloud2deviceList(), (void **)&pCloud2DevicePacket);
					if (pCloud2DevicePacket) {			
						pUart2WifiPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));
						if (pUart2WifiPacket) {
							if (convertWifi2CloudPacketToUart2WifiPacket(pCloud2DevicePacket, pUart2WifiPacket) == 0x00) {							
								EncodeUart2WifiPacket(pUart2WifiPacket);													
								int EtherCommandsDataID = (pCloud2DevicePacket->dataID[0] | (pCloud2DevicePacket->dataID[1] << 8) | (pCloud2DevicePacket->dataID[2] << 16) | (pCloud2DevicePacket->dataID[3] << 24));							
//								if (SetEtherCommandDataID2UARTCommandIDMap(pUart2WifiPacket->dataID, EtherCommandsDataID, COMMANDTYPE_SOURCE_MQTT) < 0) {

//									EJ_ErrPrintf(("[UARTThread.c][UARTSendThread][ERROR]: SetEtherCommandDataID2UARTCommandIDMap failed!\r\n"));
//								}
							}else {

								EJ_PacketUartFree(pUart2WifiPacket);
								pUart2WifiPacket = NULL;
								EJ_ErrPrintf(("[UARTThread.c][UARTSendThread][ERROR]: convertwifi2CloudPacketToUart2WifiPacket failed!\r\n"));
							}
							EJ_PacketCloudFree(pCloud2DevicePacket);
						}

					}
				}
				if (pUart2WifiPacket) {		
					EJ_PublishUart2WifiPacket(pUart2WifiPacket);
					EJ_PrintUart2WifiPacket(pUart2WifiPacket, "[Uart2WifiSend]");
					if (((pUart2WifiPacket->crypt & QOS_BITS) && (GetDeviceInfoQos())) || (GetDeviceInfoQos() == 0x01)) {
						pUart2WifiPacket->crypt |= QOS_BITS;
						/* start timer... */
						if (EJ_timer_activate(&cloud2deviceTimer) != EJ_SUCCESS) {
							EJ_ErrPrintf(("[UARTThread.c][UARTSendThread][ERROR]: start cloud2deviceTimer failed.\r\n"));
						}
						cloud2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_WAIT_FOR_ACK;
					}else {
						EJ_PacketUartFree(pUart2WifiPacket);
						pUart2WifiPacket = NULL;
					}
				}	
				break;
			}

			case EJ_UARTTHREAD_RXTX_WAIT_FOR_ACK:
			{
				if (((pUart2WifiPacket->crypt & QOS_BITS) && (GetDeviceInfoQos())) || (GetDeviceInfoQos() == 0x01)) {
					if (EJ_timer_is_running(&cloud2deviceTimer) == true) {
						if (pdevice2wifiAckPacket) {
							if (AckPacketCompare(pUart2WifiPacket, pdevice2wifiAckPacket)) {
								EJ_timer_deactivate(&cloud2deviceTimer);
								EJ_PacketUartFree(pUart2WifiPacket);
								pUart2WifiPacket = NULL;
								/* change the state machine to */
								wifi2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_SEND;
								EJ_PacketUartFree(pdevice2wifiAckPacket);
								pdevice2wifiAckPacket = NULL;
								//EJ_DebugPrintf(("[UARTThread.c][UARTSendThread][INFO]: receive ack.\r\n"));
							}
						}
					}else {
						/* modify thr DUP bit to indicate this packet is re-send. */
						pUart2WifiPacket->crypt |= (1 << DUP_BITS);
						cloud2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_SEND;
					}
				}
				break;
			}
			default :
				break;
		}


		/* process lan to device.*/
		wifi2CloudPacket *pLan2DevicePacket = NULL;
		static uart2WifiPacket *pLanUart2WifiPacket = NULL;
		switch(lan2deviceRxTxStateMachine) {
			case EJ_UARTTHREAD_RXTX_SEND:
			{
				if (!pLanUart2WifiPacket) {
					nolock_list_pop(GetLan2deviceList(), (void **)&pLan2DevicePacket);
					if (pLan2DevicePacket) {				
						pLanUart2WifiPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));
						if (pLanUart2WifiPacket) {
							if (convertWifi2CloudPacketToUart2WifiPacket(pLan2DevicePacket, pLanUart2WifiPacket) == 0x00) {
								int LanCommandDataID = (pLan2DevicePacket->dataID[0] | (pLan2DevicePacket->dataID[1] << 8) | (pLan2DevicePacket->dataID[2] << 16) | (pLan2DevicePacket->dataID[3] << 24));
//								if (setLANCommandDataID2UARTCommandDataID(LanCommandDataID, pLanUart2WifiPacket->dataID) < 0) {

//									EJ_ErrPrintf(("[UARTThread.c][UARTSendThread][ERROR]: setLANCommandDataID2UARTCommandDataID failed!\r\n"));
//								}
								
							}else {
								EJ_PacketUartFree(pLanUart2WifiPacket);
								pLanUart2WifiPacket = NULL;
								EJ_ErrPrintf(("[UARTThread.c][UARTSendThread][ERROR]: convertwifi2CloudPacketToUart2WifiPacket failed!\r\n"));
							}
							EJ_PacketCloudFree(pLan2DevicePacket);
						}
					}
				}
				if (pLanUart2WifiPacket) {
					EJ_PublishUart2WifiPacket(pLanUart2WifiPacket);
					EJ_PrintUart2WifiPacket(pLanUart2WifiPacket, "[Uart2WifiSend]");
					if (((pLanUart2WifiPacket->crypt & QOS_BITS) && (GetDeviceInfoQos())) || (GetDeviceInfoQos() == 0x01)){
						pLanUart2WifiPacket->crypt |= QOS_BITS;
						/* start timer... */
						if (EJ_timer_activate(&lan2deviceTimer) != EJ_SUCCESS) {
							EJ_ErrPrintf(("[UARTThread.c][UARTSendThread][ERROR]: start cloud2deviceTimer failed.\r\n"));
						}
						//EJ_DebugPrintf(("[UARTThread.c][UARTSendThread][INFO]: send an packet from lan2deviceList and wait for ack.\r\n"));
						/**/
						lan2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_WAIT_FOR_ACK;
					}else {
						EJ_PacketUartFree(pLanUart2WifiPacket);
						pLanUart2WifiPacket = NULL;
					}
				}
				break;
			}

			case EJ_UARTTHREAD_RXTX_WAIT_FOR_ACK:
			{
				if (((pLanUart2WifiPacket->crypt & QOS_BITS) && (GetDeviceInfoQos())) || (GetDeviceInfoQos() == 0x01)) {
					if (EJ_timer_is_running(&lan2deviceTimer) == true) {
						if (pdevice2wifiAckPacket) {
							if (AckPacketCompare(pLanUart2WifiPacket, pdevice2wifiAckPacket)) {
								EJ_timer_deactivate(&lan2deviceTimer);
								EJ_PacketUartFree(pLanUart2WifiPacket);
								pLanUart2WifiPacket = NULL;

								/* change the state machine to */
								lan2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_SEND;
								EJ_PacketUartFree(pdevice2wifiAckPacket);
								pdevice2wifiAckPacket = NULL;
								//EJ_DebugPrintf(("[UARTThread.c][UARTSendThread][INFO]: receive ack.\r\n"));
							}
						}
					}else {
						/* modify thr DUP bit to indicate this packet is re-send. */
						pLanUart2WifiPacket->crypt |= (1 << DUP_BITS);
						lan2deviceRxTxStateMachine = EJ_UARTTHREAD_RXTX_SEND;
					}
				}

				break;
			}
			default :
				break;
		}

		if (pdevice2wifiAckPacket) {
			EJ_PacketUartFree(pdevice2wifiAckPacket);
		}
		uart2WifiPacket *pwifi2deviceAckPacket = NULL; 
		nolock_list_pop(GetWifi2deviceAckList(), (void **)&pwifi2deviceAckPacket);
		if (pwifi2deviceAckPacket) {
			EJ_PublishUart2WifiPacket(pwifi2deviceAckPacket);
			EJ_PrintUart2WifiPacket(pwifi2deviceAckPacket, "[UARTThread.c][UARTSendThread][INFO]");
			EJ_PacketUartFree(pwifi2deviceAckPacket);
			//EJ_DebugPrintf(("[UARTThread.c][UARTSendThread][INFO]: publish wifi2device ack packet.\r\n"));
		}	
		EJ_thread_sleep(EJ_msec_to_ticks(10));

	}

}


static int querydid()
{
	int ret =0;
	uint8_t did[6] = { 0 };
	GetWifiStatusDeviceID(did);
	if(did[0]|did[1]|did[2]|did[3]|did[4]|did[5])
	{
		ret = 1;
	}else{
		ret = 0;
	}		
	return ret;
}



uint8_t receiveData[100];

void EJ_UartReceiveThread(void *arg)
{
	static EJ_UART_MSG_RECEIVE_STATUS receiveStatus = EJ_UART_MSG_HEAD;
	int len = 0;
	int alreadyReadCnt = 0;
	uint8_t head = 0x00;
	uint8_t *lengthArray = receiveData;
	uint8_t *messageContent = (uint8_t *)(receiveData + 4);
	uint8_t messageContentLength = 0x00;
	int dataLen = 0;
	
	for(;;) {
		switch (receiveStatus)
		{
			case EJ_UART_MSG_HEAD:
			{
				len = EJ_uart_read((uint8_t *)&head, 1);
				if ((len == 1) && (head == 0xAA)){
					receiveStatus = EJ_UART_MSG_LENGTH;
					len = 0;		
				}
				break;
			}

			case EJ_UART_MSG_LENGTH:
			{
				alreadyReadCnt = EJ_uart_read(lengthArray + len, 4 - len);
				len += alreadyReadCnt;
				if (len == 4) {
					dataLen = (lengthArray[2] | lengthArray[3] << 8);
					messageContentLength = 5 + (dataLen - 9);
					len = 0;
					receiveStatus = EJ_UART_MSG_CONTENT;
				}
				break;

			}
			case EJ_UART_MSG_CONTENT:
			{
				alreadyReadCnt = EJ_uart_read( messageContent + len, messageContentLength - len);			
				len += alreadyReadCnt;
				if (len == messageContentLength) {
					receiveStatus = EJ_UART_MSG_HEAD;
					alreadyReadCnt = 0;
					len = 0;
					uint8_t crc = makeCrc(receiveData, dataLen - 1);
					if (crc == messageContent[messageContentLength - 1]) {
						receiveStatus = EJ_UART_MSG_HEAD;
						uart2WifiPacket *pUart2WifiPacket = (uart2WifiPacket *)EJ_mem_malloc(sizeof(uart2WifiPacket));
						if (pUart2WifiPacket) {

							pUart2WifiPacket->head = head;
							pUart2WifiPacket->version = lengthArray[0];
							pUart2WifiPacket->crypt = lengthArray[1];
							pUart2WifiPacket->dataLen[0] = lengthArray[2];
							pUart2WifiPacket->dataLen[1] = lengthArray[3];

							pUart2WifiPacket->dataType = messageContent[0];
							pUart2WifiPacket->dataTypeCrc[0] = messageContent[1];
							pUart2WifiPacket->dataTypeCrc[1] = messageContent[2];
							pUart2WifiPacket->dataID = messageContent[3];
							pUart2WifiPacket->crc = crc;

							if ((dataLen - 9) > 0) {
								pUart2WifiPacket->data = (uint8_t *)EJ_mem_malloc(dataLen - 9);
								if (pUart2WifiPacket->data) {
									memcpy(pUart2WifiPacket->data, messageContent + 4, dataLen - 9);
									if (pUart2WifiPacket->crypt & DATA_WITH_XOR_CRYPT) {
										xor(pUart2WifiPacket->data, dataLen - 9);
									}
								}
								else {
									EJ_PacketUartFree(pUart2WifiPacket);
									EJ_ErrPrintf(("[ERROR]: os_mem_alloc uart2WifiPacket->data failed.\r\n"));
								}
							}else {
								
								pUart2WifiPacket->data = NULL;
							}

							EJ_PrintUart2WifiPacket(pUart2WifiPacket, "[Uart2WifiRecv]");
							if (pUart2WifiPacket->crypt & ACK_BITS) {
								if(nolock_list_push(GetDevice2wifiAckList(), pUart2WifiPacket)!=0x01)
								{
									EJ_PacketUartFree(pUart2WifiPacket);
								}								
							}else { 
								if (pUart2WifiPacket->crypt & QOS_BITS){
									/* */
									uart2WifiPacket * pResponsePacket = MakeUart2WifiResponsePacket(pUart2WifiPacket);
									if (pResponsePacket) {
										if(nolock_list_push(GetWifi2deviceAckList(), pResponsePacket)!=0x01)
										{
											EJ_PacketUartFree(pResponsePacket);
										}
									}else {
										EJ_ErrPrintf(("[ERROR]: reponse for device2wifi failed.\r\n"));
									}
								}
								if ((querydid() && pUart2WifiPacket->dataType == 0x20) || (querydid() && pUart2WifiPacket->dataType == 0x21) || (pUart2WifiPacket->dataType == 0x15)) {
								
									/*add this packet to device2cloud*/
									if (EJ_queue_send(GetDevice2cloudQueue(), &pUart2WifiPacket, EJ_NO_WAIT) != EJ_SUCCESS) {
										EJ_DebugPrintf(("[INFO]: add an packet to device2cloudQueue failed!\r\n"));
										EJ_PacketUartFree(pUart2WifiPacket);
									}else {

										//EJ_DebugPrintf(("[INFO]: add an packet to device2cloudQueue sccuess!\r\n"));
									}
									/*if lan connected.*/
//									if (IsLanModuleConnected()) {

//										uart2WifiPacket *pUart2WifiPacketCopy = copyUart2WifiPacket(pUart2WifiPacket);

//										/*add this packet to device2cloud*/
//										nolock_list_push(GetDevice2lanList(), pUart2WifiPacketCopy);

//									}
								}
								else if(pUart2WifiPacket->dataType == 0x63){
									EJ_Printf("0x63  process.....\r\n");
									if(nolock_list_push((nolock_list *)GetDevice2FirmwareOtaAckList(), pUart2WifiPacket) != 0x01)
									{
										EJ_Printf("0x63  push failed!.....\r\n");
										EJ_PacketUartFree(pUart2WifiPacket);
									}

								}else {

									if (pUart2WifiPacket->crypt & DUP_BITS) {

										//EJ_DebugPrintf(("[UARTThread.c][UARTReceiveThread][INFO]: an re-send packet !\r\n"));
									}		

									/*add this packet to device2wifi*/
									if(nolock_list_push(GetDevice2wifiList(), pUart2WifiPacket)!=0x01)
										{
											EJ_PacketUartFree(pUart2WifiPacket);
										}
								
									//EJ_InfoPrintf(("[INFO]: add an packet to device2wifiList!\r\n"));
								
								}
							}			
						}
						else {

							EJ_ErrPrintf(("[ERROR]: EJ_mem_malloc uart2WifiPacket failed.\r\n"));
						}
					}else {
						EJ_ErrPrintf(("[ERROR]: crc failed!\r\n"));
					}
					receiveStatus = EJ_UART_MSG_HEAD;
				}

				break;
			}
		default:
			break;
		}
		EJ_thread_sleep(EJ_msec_to_ticks(10));
	}

}


void EJ_PublishUart2WifiPacket(uart2WifiPacket *pPacket)
{
	if (pPacket)
	{
		if (GetDeviceInfoQos() == 0x01) {
			pPacket->crypt |= QOS_BITS;
		}
		uint8_t *buf = (uint8_t *)EJ_mem_malloc(pPacket->dataLen[0] + 1);
		memcpy(buf, pPacket, 9);
		if (pPacket->data) {
			memcpy(buf + 9, pPacket->data, pPacket->dataLen[0] - 9);
		}
		uint8_t crc = makeCrc(buf + 1, pPacket->dataLen[0] - 1);
		buf[pPacket->dataLen[0]] = crc;
		int needWirte = pPacket->dataLen[0] + 1;
		int actulWirte = EJ_uart_write(buf, needWirte);
		EJ_mem_free(buf);
	}
	
}



/**
 * Defines 
 * 
 *
 */
uint8_t EJ_UartThreadStart()
{
	int ret = 0;
	EJ_InitUARTTimer();
	if(EJ_uart_open(EJ_UART1_ID, 9600)!=0)
	{
		return -1;
	}
	ret = EJ_thread_create(&UARTReceiveThread_thread,
		"UARTReceiveThread",
		(void *)EJ_UartReceiveThread, 0,
		&UARTReceiveThread_stack, EJ_PRIO_3);
	if (ret)
		EJ_ErrPrintf(("[ERROR]:unable to create UARTReceiveThread.\r\n"));
	ret = EJ_thread_create(&UARTSendThread_thread,
		"UARTSendThread",
		(void *)EJ_UartSendThread, 0,
		&UARTSendThread_stack, EJ_PRIO_3);
	if (ret)
		EJ_ErrPrintf(("[ERROR]: unable to create UARTSendThread.\r\n"));
	return ret;

}





