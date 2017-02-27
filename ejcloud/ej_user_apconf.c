#include "ej_user_apconf.h"


typedef enum {

	EJ_INIT_SOCKET_SUCCESS = 0x00,
	EJ_SOCKET_BIND_ERROR,
	EJ_SOCKET_LISTEN_ERROR,
	EJ_OS_THREAD_CREATE_ERROR,
};

typedef enum {

	EJ_LAN_MESSAGE_HEAD = 1,
	EJ_LAN_MESSAGE_LENGTH,
	EJ_LAN_MESSAGE_CONTENT,

}LAN_MESSAGE_RECEIVE_STATUS;



static ej_thread_t LANServerReceiveThread_thread;
static os_thread_stack_define(LANServerReceiveThread_stack, 2048);

static ej_thread_t LANServerAcceptThread_thread;
static os_thread_stack_define(LANServerAcceptThread_stack, 2048);

static ej_thread_t LANServerSendThread_thread;
static os_thread_stack_define(LANServerSendThread_stack, 2048);

static int listenfd = -1;
static struct sockaddr_in local_addr, remote_addr;
#define  EJ_SOCKET_RECVIVE_LEN   256
static char socket_recvive_data[EJ_SOCKET_RECVIVE_LEN];


static void ej_server_accept_thread(void *arg)
{
		int remoteAddrLen = sizeof(remote_addr);
		int remotefd = 0;
		remotefd = accept(listenfd, (struct sockaddr *)&remote_addr, &remoteAddrLen);

		int nb = 0;
		int timeout = 500;
		nb = setsockopt(remotefd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(int));
		if(nb == -1)
		{
			EJ_ErrPrintf(("[LANServer.c][init_LANServer][ERROR]: rxsocket set broadcast.\r\n"));
		
		}
		EJ_thread_sleep(EJ_msec_to_ticks(300));
	
}
static void ej_server_receive_thread(void *arg)
{
		fd_set rfd;
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		
		while(1)
		{

			//1.set  fd_set
			FD_ZERO(&rfd);
			FD_SET(listenfd,&rfd)

			int nRet  = select(listenfd+1,*rfd,NULL,NULL,&timeout);
			if(nRet <=0 )
			{
				continue;
			}

			if(FD_ISSET(listenfd,&rfd))
			{	memset(socket_recvive_data,0,EJ_SOCKET_RECVIVE_LEN);
				int ret = recvfrom(listenfd,socket_recvive_data,EJ_SOCKET_RECVIVE_LEN,0,(struct sockaddr *)&local_addr,sizeof(struct sockaddr));

				if(ret>0)
				{
					
				}
			}
			

		}


}
static void ej_server_send_thread(void *arg)
{

}

int EJ_init_apconf_task()
{
	int ret = EJ_SUCCESS;

	//1.	create socket
	listenfd = socket(AF_INET,SOCK_STREAM,0);
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(4321);
    local_addr.sin_len = sizeof(local_addr);
    local_addr.sin_addr.s_addr = INADDR_ANY;

	//2.	set socket opt
	int on = 1;
	int nb = 0;
	nb = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );


	//3.	bind socket
	if(bind(listenfd,(struct sockaddr *)&local_addr,sizeof(struct sockaddr))==-1)
	{
		close(listenfd);
		listenfd = -1;
		EJ_ErrPrintf(("[LANServer.c][EJ_init_apconf_task][ERROR]: Socket bind failed.\r\n"));
      	return EJ_SOCKET_BIND_ERROR; 
	}
	
	//4.	

//	if (listen(listenfd, 1) == -1)
//    {
//    	EJ_ErrPrintf(("[LANServer.c][EJ_init_apconf_task][ERROR]: Socket listen failed.\r\n"));

//        return EJ_SOCKET_LISTEN_ERROR;
//    }

//	ret = EJ_thread_create(&LANServerAcceptThread_thread,
//								"ej_server_accept_thread",
//								(void *)ej_server_accept_thread, 0,
//								&LANServerAcceptThread_stack, EJ_PRIO_3);

//	if (ret) {
//		EJ_ErrPrintf(("[ej_user_apconf.c][Init_LANThread][ERROR]: Unable to create LANServerAcceptThread.\r\n"));
//		return EJ_OS_THREAD_CREATE_ERROR;
//    }

	ret = EJ_thread_create(&LANServerSendThread_thread,
								"ej_server_send_thread",
								(void *)ej_server_send_thread, 0,
								&LANServerSendThread_stack, EJ_PRIO_3);

	if (ret) {
		EJ_ErrPrintf(("[ej_user_apconf.c][Init_LANThread][ERROR]: Unable to create LANServerSendThread.\r\n"));
		return EJ_OS_THREAD_CREATE_ERROR;
    }

	ret = EJ_thread_create(&LANServerReceiveThread_thread,
								"ej_server_receive_thread",
								(void *)ej_server_receive_thread, 0,
								&LANServerReceiveThread_stack, EJ_PRIO_3);

	if (ret) {
		EJ_ErrPrintf(("[ej_user_apconf.c][EJ_init_apconf_task][ERROR]: Unable to create LANServerReceiveThread.\r\n"));
		return EJ_OS_THREAD_CREATE_ERROR;
    }

	
	return ret;
	
}





int  EJ_apconf_task()
{

	//1. init task


	//2. start task


	//3. uinit task

}


