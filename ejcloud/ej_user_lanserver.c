#include "ej_port_wlan.h"
#include "ej_packet.h"
#include "ej_utils.h"
#include "ej_user_lanserver.h"


unsigned char lanSignStr[] = "EnPntyOER8vx4vI67qACK2eC5Xl8TPntg3qe";

typedef enum {

	LAN_MESSAGE_HEAD = 1,
	LAN_MESSAGE_LENGTH,
	LAN_MESSAGE_CONTENT,

}LAN_MESSAGE_RECEIVE_STATUS;

int listenfd;

static ej_thread_t LANServerReceiveThread_thread;

static ej_thread_stack_define(LANServerReceiveThread_stack, 2048);

static ej_thread_t LANServerAcceptThread_thread;

static ej_thread_stack_define(LANServerAcceptThread_stack, 2048);


static ej_thread_t LANServerSendThread_thread;

static ej_thread_stack_define(LANServerSendThread_stack, 2048);

void LANServerAcceptThread(void *arg);
void LANServerReceiveThread(void *arg);
void LANServerSendThread(void *arg);

unsigned char ciphertext[256];
unsigned char plaintext[256];

struct sockaddr_in local_addr, remote_addr;


#define MAX_SOCKET_COUNTS 4

#define SOCKET_RECEIVE_BUF_SIZE	256

Mutex LANServerReceiveMxtex;

unsigned char currentSocketCounts = 0;

typedef struct {

	int remotefd;

	LAN_MESSAGE_RECEIVE_STATUS lanReceiveStatus;
	
	int len;

	int lengthInt;

	unsigned char * receiveData;

	int heartCounter;

}LANServerReceiveCtrlDesc;

LANServerReceiveCtrlDesc  receiveCtrlDesc[MAX_SOCKET_COUNTS];


ej_timer_t LanServerHeartTimer;

void LanServerHeartTimerProcessCB()
{
	int i = 0;

	MutexLock(&LANServerReceiveMxtex);

	for (i = 0; i < MAX_SOCKET_COUNTS; i++) {

		if (receiveCtrlDesc[i].remotefd != -1) {

			receiveCtrlDesc[i].heartCounter++;
			
			if (receiveCtrlDesc[i].heartCounter > 10) {

				close(receiveCtrlDesc[i].remotefd);
				
				receiveCtrlDesc[i].remotefd = -1;

				receiveCtrlDesc[i].heartCounter = 0;

				EJ_mem_free(receiveCtrlDesc[i].receiveData);
				
				receiveCtrlDesc[i].receiveData = NULL;

				receiveCtrlDesc[i].lanReceiveStatus = LAN_MESSAGE_HEAD;

				receiveCtrlDesc[i].len = 0;

				currentSocketCounts--;

				EJ_DebugPrintf(("[ej_user_lanserver.c][LanServerHeartTimerProcessCB][ERROR]: remove an client.\r\n"));
			}
		}
	}

	MutexUnlock(&LANServerReceiveMxtex);

}

int InitLanServerHeartTimer()
{
	if (EJ_timer_create(&LanServerHeartTimer,
			    "lanServerHeartTimer",
			    EJ_msec_to_ticks(1000),
			    &LanServerHeartTimerProcessCB,
			    NULL,
			    EJ_TIMER_PERIODIC,
			    EJ_TIMER_AUTO_ACTIVATE) != EJ_SUCCESS) {
		EJ_ErrPrintf(("[ej_user_lanserver.c][InitLanServerHeartTimer][ERROR]: Failed to create LanServerHeartTimer timer.\r\n"));
		return -EJ_FAIL;
	}

	return EJ_SUCCESS;
}


bool connected = false;

bool IsLanModuleConnected()
{
	return (currentSocketCounts > 0)?true:false;
}

unsigned char ConvertLengthToAesEncryptDataLength(unsigned char length)
{
	return (length/16 + 1)*16;
}

void InitLANServerReceiveCtrlDesc()
{
	int i = 0;

	for (i = 0; i < MAX_SOCKET_COUNTS; i++) {

		receiveCtrlDesc[i].remotefd = -1;

		receiveCtrlDesc[i].receiveData = NULL;

		receiveCtrlDesc[i].lanReceiveStatus = LAN_MESSAGE_HEAD;

		receiveCtrlDesc[i].len = 0;

		receiveCtrlDesc[i].heartCounter = 0;

	}
}


int registerLANServerReceiveCtrlDesc(int socketfd)
{
	int ret = -1;
	
	int i = 0;

	MutexLock(&LANServerReceiveMxtex);

	for (i = 0; i < MAX_SOCKET_COUNTS; i++) {

		if (receiveCtrlDesc[i].remotefd < 0) {

			receiveCtrlDesc[i].remotefd = socketfd;

			receiveCtrlDesc[i].receiveData = (unsigned char *)EJ_mem_malloc(SOCKET_RECEIVE_BUF_SIZE);

			receiveCtrlDesc[i].lanReceiveStatus = LAN_MESSAGE_HEAD;

			receiveCtrlDesc[i].len = 0;

			connected = true;

			currentSocketCounts++;

			ret = 0;

			break;
		}
	}

	MutexUnlock(&LANServerReceiveMxtex);

	return ret;
}

int unRegisterLANServerReceiveCtrlDesc(unsigned char index)
{
	//MutexLock(&LANServerReceiveMxtex);

	if (index < MAX_SOCKET_COUNTS) {

		close(receiveCtrlDesc[index].remotefd);

		receiveCtrlDesc[index].remotefd = -1;

		EJ_mem_free(receiveCtrlDesc[index].receiveData);

		receiveCtrlDesc[index].lanReceiveStatus = LAN_MESSAGE_HEAD;

		receiveCtrlDesc[index].len = 0;
	}


	return 0;

	//MutexUnlock(&LANServerReceiveMxtex);
}

int isLANServerReceiveCtrlDescAvailable(unsigned char index)
{
	return receiveCtrlDesc[index].remotefd;
}

int init_LANServer()
{

    int ret = EJ_SUCCESS;

    /* init mutex... */
	MutexInit(&LANServerReceiveMxtex);

    InitLANServerReceiveCtrlDesc();

	InitLanServerHeartTimer();

    listenfd = socket(AF_INET,SOCK_STREAM,0);
 
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(4321);
    local_addr.sin_len = sizeof(local_addr);
    local_addr.sin_addr.s_addr = INADDR_ANY;


    if (bind(listenfd, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0)
    {
      EJ_ErrPrintf(("[ej_user_lanserver.c][init_LANServer][ERROR]: Socket bind failed.\r\n"));

        return INIT_LANSERVER_SOCKET_BIND_ERROR;
    }
 
    if (listen(listenfd, 1) == -1)
    {
      EJ_ErrPrintf(("[ej_user_lanserver.c][init_LANServer][ERROR]: Socket listen failed.\r\n"));

        return INIT_LANSERVER_SOCKET_LISTEN_ERROR;
    }



	ret = EJ_thread_create(&LANServerAcceptThread_thread,
								"LANServerAcceptThread",
								(void *)LANServerAcceptThread, 0,
								&LANServerAcceptThread_stack, EJ_PRIO_3);

	if (ret) {
		EJ_ErrPrintf(("[ej_user_lanserver.c][Init_LANThread][ERROR]: Unable to create LANServerAcceptThread.\r\n"));

		return INIT_LANSERVER_OS_THREAD_CREATE_ERROR;
    }

	ret = EJ_thread_create(&LANServerSendThread_thread,
								"LANServerSendThread",
								(void *)LANServerSendThread, 0,
								&LANServerSendThread_stack, EJ_PRIO_3);

	if (ret) {
		EJ_ErrPrintf(("[ej_user_lanserver.c][Init_LANThread][ERROR]: Unable to create LANServerSendThread.\r\n"));

		return INIT_LANSERVER_OS_THREAD_CREATE_ERROR;
    }

	ret = EJ_thread_create(&LANServerReceiveThread_thread,
								"LANServerReceiveThread",
								(void *)LANServerReceiveThread, 0,
								&LANServerReceiveThread_stack, EJ_PRIO_3);

	if (ret) {
		EJ_ErrPrintf(("[ej_user_lanserver.c][Init_LANThread][ERROR]: Unable to create LANServerReceiveThread.\r\n"));

		return INIT_LANSERVER_OS_THREAD_CREATE_ERROR;
    }

	
	return ret;
}

int UnInit_LANServer()
{
	/**/
	EJ_thread_delete(&LANServerAcceptThread_thread);

	EJ_thread_delete(&LANServerSendThread_thread);

	EJ_thread_delete(&LANServerReceiveThread_thread);
	/*
	if (remotefd) {
		
		close(remotefd);

		remotefd = -1;
	}
	*/
	if (listenfd) {

		close(listenfd);

		listenfd = -1;
	}

	return 0;
	//connected = false;
}



void LANServerAcceptThread(void *arg)
{
	int remoteAddrLen = sizeof(remote_addr);

	for(;;) {

		int remotefd = 0;
		
		if (currentSocketCounts < MAX_SOCKET_COUNTS) {
			
			remotefd = accept(listenfd, (struct sockaddr *)&remote_addr, &remoteAddrLen);

			int nb = 0;
			int timeout = 500;
			nb = setsockopt(remotefd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(int));
			if(nb == -1)
			{
				EJ_ErrPrintf(("[ej_user_lanserver.c][init_LANServer][ERROR]: rxsocket set broadcast.\r\n"));
			
			}
	   

			/**/
			if (registerLANServerReceiveCtrlDesc(remotefd) < 0) {

				EJ_ErrPrintf(("[ej_user_lanserver.c][LANServerAcceptThread][ERROR]: registerLANServerReceiveCtrlDesc error.\r\n"));
			}else {
				EJ_ErrPrintf(("[ej_user_lanserver.c][LANServerAcceptThread][INFO]: registerLANServerReceiveCtrlDesc success.remotefd = %d\r\n",remotefd));
			}
			
		}

		//EJ_Printf("socketfd : %d\r\n", remotefd);
		
		EJ_thread_sleep(EJ_msec_to_ticks(300));
	}

}


void LANServerReceiveThread(void *arg)
{

		for (;;)
		{

			int receiveCtrlDescIndex = 0;
			
			int headInt = 0x00000000;
			int lengthInt = 0x00;
			int actulReadCnt = 0;
			bool shouldExit = false;

			MutexLock(&LANServerReceiveMxtex);
			
			for (receiveCtrlDescIndex = 0; (receiveCtrlDescIndex < MAX_SOCKET_COUNTS && isLANServerReceiveCtrlDescAvailable(receiveCtrlDescIndex)); ((receiveCtrlDescIndex++) % MAX_SOCKET_COUNTS)) {		
				

				unsigned char *headArray = receiveCtrlDesc[receiveCtrlDescIndex].receiveData;
							
				unsigned char *lengthArray = (unsigned char *)(receiveCtrlDesc[receiveCtrlDescIndex].receiveData + 4);
				
				unsigned char *messageContent = (unsigned char *)(receiveCtrlDesc[receiveCtrlDescIndex].receiveData + 8);
				
										
				switch (receiveCtrlDesc[receiveCtrlDescIndex].lanReceiveStatus)
				{
					case LAN_MESSAGE_HEAD:

						actulReadCnt = recv(receiveCtrlDesc[receiveCtrlDescIndex].remotefd, headArray + receiveCtrlDesc[receiveCtrlDescIndex].len, 4 - receiveCtrlDesc[receiveCtrlDescIndex].len, 0);

						if (actulReadCnt <= 0) {

							if ((errno != EINTR || errno != EWOULDBLOCK || errno != EAGAIN)) {
								shouldExit = true;
								break;
							}
							
						}

						receiveCtrlDesc[receiveCtrlDescIndex].len += actulReadCnt;

						if (receiveCtrlDesc[receiveCtrlDescIndex].len == 4) {

							if ((headArray[0] == 0x5A) && (headArray[1] == 0x5A)) {

								EJ_InfoPrintf(("[ej_user_lanserver.c][LANServerReceiveThread][INFO]: receive packet head.\r\n"));

								receiveCtrlDesc[receiveCtrlDescIndex].lanReceiveStatus = LAN_MESSAGE_LENGTH;
								
							}

							receiveCtrlDesc[receiveCtrlDescIndex].len = 0;
						}
						break;

					case LAN_MESSAGE_LENGTH:

						actulReadCnt = recv(receiveCtrlDesc[receiveCtrlDescIndex].remotefd, lengthArray + receiveCtrlDesc[receiveCtrlDescIndex].len, 4 - receiveCtrlDesc[receiveCtrlDescIndex].len, 0);

						if (actulReadCnt <= 0) {

							if ((errno != EINTR || errno != EWOULDBLOCK || errno != EAGAIN)) {
								shouldExit = true;
								break;
							}
							
						}

						receiveCtrlDesc[receiveCtrlDescIndex].len += actulReadCnt;

						if (receiveCtrlDesc[receiveCtrlDescIndex].len == 4) {

							int i = 0;
							for(i = 0; i < 4; i++) {
								EJ_Printf("%x ",lengthArray[i]);
							}

							receiveCtrlDesc[receiveCtrlDescIndex].lengthInt = (lengthArray[0] | (lengthArray[1] << 8) | (lengthArray[2] << 16) | (lengthArray[3] << 24));

							EJ_Printf("LAN_MESSAGE_LENGTH before: %d\r\n", receiveCtrlDesc[receiveCtrlDescIndex].lengthInt);

							receiveCtrlDesc[receiveCtrlDescIndex].lengthInt = ConvertLengthToAesEncryptDataLength(receiveCtrlDesc[receiveCtrlDescIndex].lengthInt - 8 - 16);

							EJ_Printf("LAN_MESSAGE_LENGTH after: %d\r\n", receiveCtrlDesc[receiveCtrlDescIndex].lengthInt);

							receiveCtrlDesc[receiveCtrlDescIndex].lanReceiveStatus = LAN_MESSAGE_CONTENT;

							receiveCtrlDesc[receiveCtrlDescIndex].len = 0;
						}

						break;

					case LAN_MESSAGE_CONTENT:

						actulReadCnt = recv(receiveCtrlDesc[receiveCtrlDescIndex].remotefd, messageContent + receiveCtrlDesc[receiveCtrlDescIndex].len, receiveCtrlDesc[receiveCtrlDescIndex].lengthInt + 16 - receiveCtrlDesc[receiveCtrlDescIndex].len, 0);
						EJ_Printf("LAN_MESSAGE_CONTENT after: %d\r\n", actulReadCnt);
						if (actulReadCnt <= 0) {

							if ((errno != EINTR || errno != EWOULDBLOCK || errno != EAGAIN)) {
								shouldExit = true;
								break;
							}
							
						}

						receiveCtrlDesc[receiveCtrlDescIndex].len += actulReadCnt;

						if (receiveCtrlDesc[receiveCtrlDescIndex].len == receiveCtrlDesc[receiveCtrlDescIndex].lengthInt + 16) {
							int i = 0;
							/*for(i = 0; i < lengthInt + 16; i++) {
								EJ_Printf("%x ",messageContent[i]);
							}*/
							unsigned char tk[16];
							unsigned char oldtk[16];

							memcpy(oldtk, messageContent + receiveCtrlDesc[receiveCtrlDescIndex].lengthInt, 16);

							memcpy(messageContent + receiveCtrlDesc[receiveCtrlDescIndex].lengthInt, lanSignStr, strlen(lanSignStr));

							EJ_hash_md5(receiveCtrlDesc[receiveCtrlDescIndex].receiveData,  receiveCtrlDesc[receiveCtrlDescIndex].lengthInt + 8 + strlen(lanSignStr), tk, 16);

							if (memcmp(tk, oldtk, 16) == 0) {

								wifi2CloudPacket *pPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

								if (pPacket) {
					
									pPacket->head[0] = headArray[0];
									pPacket->head[1] = headArray[1];

									pPacket->version = headArray[2];

									pPacket->crypt = headArray[3];

									pPacket->dataLen[0] = lengthArray[0];
									pPacket->dataLen[1] = lengthArray[1];
									pPacket->dataLen[2] = lengthArray[2];
									pPacket->dataLen[3] = lengthArray[3];

					
									if (EJ_Aes_lanMessageDecrypt((unsigned char *)(messageContent), receiveCtrlDesc[receiveCtrlDescIndex].lengthInt, plaintext) == 0)
									{
										EJ_Printf("error EJ_Aes_Decrypt\r\n");
										EJ_mem_free(pPacket);
										return ;
									}
								
									/*for(i = 0; i < (pPacket->dataLen[0] - 44); i++) {
										EJ_Printf("%x ", plaintext[i+20]);
									}*/

									pPacket->data = NULL;
								
									if ((pPacket->dataLen[0] - 44) > 0) {
	
											pPacket->data = (unsigned char *)EJ_mem_malloc(pPacket->dataLen[0] - 44);

											if (pPacket->data) {
												
												memcpy(pPacket->data, plaintext + 20, pPacket->dataLen[0] - 44);
											}else {

												EJ_ErrPrintf(("[ej_user_lanserver.c][LANReceiveThread][ERROR]: EJ_mem_malloc wifi2CloudPacket->data failed.\r\n"));
											}
									}
									
									pPacket->dataType[0] = plaintext[0];
									pPacket->dataType[1] = plaintext[1];

									pPacket->dataID[0] = plaintext[2];
									pPacket->dataID[1] = plaintext[3];
									pPacket->dataID[2] = plaintext[4];
									pPacket->dataID[3] = plaintext[5];

									memcpy(pPacket->timeStamp, plaintext + 6, 8);
									memcpy(pPacket->deviceID, plaintext + 14, 6);										

									memcpy(pPacket->signature, messageContent + receiveCtrlDesc[receiveCtrlDescIndex].lengthInt, 16);

									EJ_PrintWifi2CloudPacket(pPacket, NULL);
									
									int LanCommandDataID = (pPacket->dataID[0] | (pPacket->dataID[1] << 8) | (pPacket->dataID[2] << 16) | (pPacket->dataID[3] << 24));

									if (setLANCommandDataID2LANCommandSocketfd(LanCommandDataID, receiveCtrlDesc[receiveCtrlDescIndex].remotefd) < 0) {
									
										EJ_ErrPrintf(("[ej_user_lanserver.c][LANReceiveThread][ERROR]: setLANCommandDataID2LANCommandSocketfd failed.\r\n"));
									}

									if ((pPacket->dataType[0] == 0x20) && (pPacket->dataType[1] == 0x00)) {

										/* add this packet to cloud2deviceList. */
										nolock_list_push(GetLan2deviceList(), pPacket);										

										EJ_InfoPrintf(("[ej_user_lanserver.c][LANReceiveThread][INFO]: receive an lan2device packet.\r\n"));
									}
									else {

										if ((pPacket->dataType[0] == 0x7B) && (pPacket->dataType[1] == 0x00)) {

											receiveCtrlDesc[receiveCtrlDescIndex].heartCounter = 0;

											EJ_InfoPrintf(("[ej_user_lanserver.c][LANReceiveThread][INFO]: receive an heatbeat packet.\r\n"));
											
										}

										/* add this packet to cloud2wifiList. */
										nolock_list_push(GetLan2wifiList(), pPacket);
										EJ_Printf("[ej_user_lanserver.c][LANReceiveThread] after: %d\r\n", __LINE__);
									}
					
					
								}else{
									EJ_ErrPrintf(("[ej_user_lanserver.c][LANReceiveThread][ERROR]: EJ_mem_malloc wifi2CloudPacket failed.\r\n"));
								}

							}else {

					
								EJ_ErrPrintf(("[ej_user_lanserver.c][LANReceiveThread][ERROR]: sign failed.\r\n"));
							}

							
							receiveCtrlDesc[receiveCtrlDescIndex].lanReceiveStatus = LAN_MESSAGE_HEAD;

							receiveCtrlDesc[receiveCtrlDescIndex].len = 0;

						}

						break;

					default:

						break;

					
				}


				if (shouldExit) {

					unRegisterLANServerReceiveCtrlDesc(receiveCtrlDescIndex);
				}			
				
			}

			MutexUnlock(&LANServerReceiveMxtex);	

			EJ_thread_sleep(EJ_msec_to_ticks(10));
		}

		EJ_thread_delete(NULL);	

}

void LANServerSendThread(void *arg)
{
	int rc = 0;
	int dataLen = 0;

	EJ_InfoPrintf(("[ej_user_lanserver.c][LANServerSendThread][INFO]: LANServerSendThread start.\r\n"));

	for (;;)
	{
		/*read wifi2lan list and send */
		wifi2CloudPacket *pPacket = NULL;

		nolock_list_pop(GetWifi2lanList(), (void **)&pPacket);

		if (pPacket) {
			
			EJ_InfoPrintf(("[ej_user_lanserver.c][LAN_Thread][INFO]: process an wifi2CloudPacket that from wifi to lan.\r\n"));

			PubWifi2LANPacket(pPacket);

			EJ_PacketCloudFree(pPacket);
		}

		/*read device2cloud list and send */
		uart2WifiPacket *pDevice2CloudPacket = NULL;

		nolock_list_pop(GetDevice2lanList(), (void **)&pDevice2CloudPacket);

		if (pDevice2CloudPacket) {

			EJ_InfoPrintf(("[ej_user_lanserver.c][LAN_Thread][INFO]: process an uart2WifiPacket that from device to lan.\r\n"));

			/* decode uart2WifiPacket and encode wifi2CloudPacket ...*/

			wifi2CloudPacket *pWifi2CloudPacket = (wifi2CloudPacket *)EJ_mem_malloc(sizeof(wifi2CloudPacket));

			if (pWifi2CloudPacket) {

				if (convertUart2WifiPacketToWifi2CloudPacket(pDevice2CloudPacket, pWifi2CloudPacket) == 0x00) {
				
					int dataID = getLANCommandDataIDByUARTCommandDataID(pDevice2CloudPacket->dataID);

					if (dataID) {
			
						fillDataIDToPacket(pWifi2CloudPacket, dataID);
					}else {
						fillDataIDToPacket(pWifi2CloudPacket, 0x55);
					}

					PubWifi2LANPacket(pWifi2CloudPacket);							
				}else {

					EJ_ErrPrintf(("[ej_user_lanserver.c][LAN_Thread][ERROR]: convertUart2WifiPacketToWifi2CloudPacket failed!\r\n"));
				}

				EJ_PacketCloudFree(pWifi2CloudPacket);
				

				EJ_PacketUartFree(pDevice2CloudPacket);
				

			}else {
				EJ_ErrPrintf(("[ej_user_lanserver.c][LAN_Thread][ERROR]: EJ_mem_malloc wifi2CloudPacket failed!\r\n"));

				EJ_PacketUartFree(pDevice2CloudPacket);	
			}
		}


		EJ_thread_sleep(EJ_msec_to_ticks(10));

	}
}



int PublishLANMessage(int socketfd, void *payload, int dataLen)
{
	int rc = 0;
	int i;
	uint8_t *p = (uint8_t *)payload;
#if defined(WIN32) || defined(WIN64)

	
#else
	rc = send(socketfd, payload, dataLen, 0);
	for(int i=0;i<dataLen;i++)
	{
		EJ_Printf("%X ",p[i]);
	}
	EJ_Printf("\r\n");	
	return rc;
	

#endif	

}


void PubWifi2LANPacket(wifi2CloudPacket *pPacket)
{
	int rc = -1;
	if (pPacket) {
			
		unsigned char *buf = (unsigned char *)EJ_mem_malloc(pPacket->dataLen[0] - 16 -8);

		unsigned char *temp = (unsigned char *)pPacket;
		if (buf) {

			memcpy(buf, temp + 8, 20);

			if (pPacket->dataLen[0] > 44) {
				memcpy(buf + 20, pPacket->data, pPacket->dataLen[0] - 20 - 16 - 8);
			}

			int cipherSize = EJ_Aes_lanMessageEncrypt(buf, pPacket->dataLen[0] - 16 - 8, ciphertext);
			
			if (cipherSize) {
				
				unsigned char *payload = (unsigned char *)EJ_mem_malloc(8 + cipherSize + strlen(lanSignStr));

				if (payload) {

					memcpy(payload, pPacket, 8);
					memcpy(payload + 8, ciphertext, cipherSize);
					memcpy(payload + 8 + cipherSize, lanSignStr, strlen(lanSignStr));
				
					unsigned char tk[16];

					EJ_hash_md5(payload, 8 + cipherSize + strlen(lanSignStr), tk, 16);

					memcpy(payload + 8 + cipherSize, tk, 16);

					int LANCommandDataID = (pPacket->dataID[0] | (pPacket->dataID[1] << 8) | (pPacket->dataID[2] << 16) | (pPacket->dataID[3] << 24));

					int socketfd = getLanCommandSocketfdByLANCommandDataID(LANCommandDataID);

					EJ_DebugPrintf(("[ej_user_lanserver.c][PubWifi2LANPacket][INFO]: socketfd is: %d.\r\n", socketfd));

					if (socketfd != -1) {
						
						rc =PublishLANMessage(socketfd, payload, 8 + cipherSize + 16);
						if(rc > 0)
						{
							EJ_Printf("lan send data sucess rc =%d\r\n",rc);
						}
					}else {
						EJ_ErrPrintf(("[ej_user_lanserver.c][PubWifi2LANPacket][ERROR]: can't get socketfd by LANCommandDataID.\r\n"));
					}

					EJ_mem_free(payload);

				}else {
					EJ_ErrPrintf(("[ej_user_lanserver.c][PubWifi2LANPacket][ERROR]: EJ_mem_malloc failed.\r\n"));

				}

			}

			EJ_mem_free(buf);
			
		}else {
			EJ_ErrPrintf(("[ej_user_lanserver.c][PubWifi2LANPacket][ERROR]: EJ_mem_malloc failed.\r\n"));
		}
	}
	else {

		
	}
}





