#include "ej_port_mqtt.h"
/*void InitTimer(Timer* t)
{
    if (!t)
    {
        return;
    }
	t->xTicksToWait = 0;
	memset(&t->xTimeOut, '\0', sizeof(t->xTimeOut));
}
*/


#define CLK32_TICK_TO_MS (32)

static unsigned int mqtt_current_time_ms(void)
{
    unsigned int current_ms = 0;

    // time in unit of 32k clock,need transfer to ms
    current_ms = get_current_count() / CLK32_TICK_TO_MS;
    return current_ms;
}


void InitTimer(Timer *timer)
{
    timer->end_time = 0;
}


void TimerDeinit(Timer* t)
{
    if (!t)
    {
        return;
    }
}


/*
char expired(Timer* t)
{
    if (!t)
    {
        return -1;
    }
	return xTaskCheckForTimeOut(&t->xTimeOut, &t->xTicksToWait) == ejTRUE;
}
*/

char expired(Timer *timer)
{
    unsigned int cur_time = 0;
    cur_time = mqtt_current_time_ms();
    if (timer->end_time < cur_time || timer->end_time == cur_time) {
        MQTT_DBG("MQTT expired enter");
        return 1;
    } else {
        MQTT_DBG("MQTT not expired");
        return 0;
    }
}
/*
void countdown_ms(Timer* t, unsigned int ms)
{
    if (!t)
    {
        return;
    }
    t->xTicksToWait = (ms / (portTICK_RATE_MS));
    vTaskSetTimeOutState(&t->xTimeOut); // Record the time at which this function was entered. 
}
*/

void countdown_ms(Timer *timer, unsigned int timeout)
{
    timer->end_time = mqtt_current_time_ms() + timeout;
}

/*
int left_ms(Timer* t)
{
    if (!t)
    {
        return 0;
    }
	if (xTaskCheckForTimeOut(&t->xTimeOut, &t->xTicksToWait) == ejTRUE)
        return 0;
	return (t->xTicksToWait <= 0) ? 0 : (t->xTicksToWait * (portTICK_RATE_MS));
}
*/

int left_ms(Timer *timer)
{
    unsigned int cur_time = 0;
    cur_time = mqtt_current_time_ms();
    if (timer->end_time < cur_time || timer->end_time == cur_time) {
        return 0;
    } else {
        return timer->end_time - cur_time;
    }
}

/*
static int ej_mqtt_read(Network* n, unsigned char* buffer, int len, int tv)
{
	int bytes = 0;
	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(int));
	if (tv == 0) {
		return bytes;
	}
	int rc = recv(n->my_socket, &buffer[bytes], (size_t)(len - bytes), 0);
	if (rc == 0)
	{
		bytes = 0;
	}
	else if (rc == -1)
	{
		if (errno != ENOTCONN && errno != ECONNRESET)
		{
			bytes = -1;
		}
	}
	else
		bytes += rc;

	return bytes;
}

*/
static int ej_mqtt_read(Network* n, unsigned char* buffer, int len, int timeout_ms)

{
    int rc = 0;
    int recvlen = 0;
    int ret = -1;
    fd_set fdset;
    struct timeval tv;

    FD_ZERO(&fdset);
    FD_SET(n->my_socket, &fdset);


    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    ret = select(n->my_socket + 1, &fdset, NULL, NULL, &tv);
    MQTT_DBG("mqtt read timer=%d ms", timeout_ms);
    if (ret < 0) {
        MQTT_DBG("mqtt read(select) fail ret=%d", ret);
        return -1;
    } else if (ret == 0) {
        MQTT_DBG("mqtt read(select) timeout");
        return -2;
    } else if (ret == 1) {
        do {
            MQTT_DBG("mqtt read recv len = %d, recvlen = %d", len, recvlen);
            rc = recv(n->my_socket, buffer + recvlen, len - recvlen, 0);
            if (rc > 0) {
            recvlen += rc;
                MQTT_DBG("mqtt read ret=%d, rc = %d, recvlen = %d", ret, rc, recvlen);
            } else {
                MQTT_DBG("mqtt read fail: ret=%d, rc = %d, recvlen = %d", ret, rc, recvlen);
                return -3;
            }
        } while (recvlen < len);
    }
    return recvlen;
}

/*
static int ej_mqtt_write(Network* n, unsigned char* buffer, int len, int tv)
{
	if (tv == 0) EJ_Printf("error1.\r\n");
	setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(int));
	int	rc = send(n->my_socket, buffer, len, 0);
	return rc;
}
*/
	int ej_mqtt_write(Network *n, unsigned char *buffer, int len, int timeout_ms)
	{
		int rc = 0;
		int ret = -1;
		fd_set fdset;
		struct timeval tv;
	
		FD_ZERO(&fdset);
		FD_SET(n->my_socket, &fdset);
	
	
		tv.tv_sec = timeout_ms / 1000;
		tv.tv_usec = (timeout_ms % 1000) * 1000;
		MQTT_DBG("mqtt write timer=%d ms", timeout_ms);
		ret = select(n->my_socket + 1, NULL, &fdset, NULL, &tv);
	
		if (ret < 0) {
			MQTT_DBG("mqtt write fail");
			return -1;
		} else if (ret == 0) {
			MQTT_DBG("mqtt write timeout");
			return -2;
		} else if (ret == 1) {
			rc = write(n->my_socket, buffer, len);
		}
		return rc;
	
	}


static void ej_mqtt_disconnect(Network* n)
{
	close(n->my_socket);
}


int NetworkConnect(Network *n, char *addr,  int port)
{
#if 1



    int type = SOCK_STREAM;
    struct sockaddr_in address;
    int rc = -1;
    sa_family_t family = AF_INET;
    struct addrinfo *result = NULL;
    struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

    if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0) {
        struct addrinfo *res = result;

        /* prefer ip4 addresses */
        while (res) {
            if (res->ai_family == AF_INET) {
                result = res;
                break;
            }
            res = res->ai_next;
        }

        if (result->ai_family == AF_INET) {
            //address.sin_port = htons(atoi(port));
            address.sin_port = htons(port);
            address.sin_family = family = AF_INET;
            address.sin_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;
#if 1
            int a = 0, b = 0, c = 0, d = 0;
            unsigned int temp = 0;

            temp = address.sin_addr.s_addr;
            a = temp & 0x000000FF;
            b = (temp >> 8) & 0x000000FF;
            c = (temp >> 16) & 0x000000FF;
            d = (temp >> 24) & 0x000000FF;
            MQTT_DBG("mqtt connect find addrinfo:%d.%d.%d.%d", a, b, c, d);
#endif
        } else {
            rc = -1;
        }
        freeaddrinfo(result);
    }

    /* create client socket */
    if (rc == 0) {
        n->my_socket = socket(family, type, 0);
        if (n->my_socket < 0) {
            MQTT_DBG("mqtt socket create fail");
            return -1;
        }
        /* connect remote servers*/
        rc = connect(n->my_socket, (struct sockaddr *)&address, sizeof(address));

        if (rc < 0) {
            close(n->my_socket);
            MQTT_DBG("mqtt socket connect fail:rc=%d,socket = %d", rc, n->my_socket);
            return -2;
        }
    }

    return rc;





#else 

	struct sockaddr_in addr;
	n->my_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (n->my_socket < 0) {
		EJ_ErrPrintf(("[wmsdk_port.c][ConnectNetwork][ERROR]:  create socket failed.\r\n"));		
		return -EJ_FAIL;
	}else{
		EJ_ErrPrintf(("[wmsdk_port.c][ConnectNetwork][INFO]: create socket success.\r\n"));		
	}	  

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host);
	memset(&(addr.sin_zero), '\0', 8);
	EJ_ErrPrintf(("[ConnectNettwork][port:%d][host:%s].\r\n",port,host));
	if (connect(n->my_socket, (const struct sockaddr *)&addr, sizeof(addr)) != 0) 
	{
		EJ_ErrPrintf(("[ConnectNettwork][ERROR]: socket connect failed.\r\n"));
		close(n->my_socket); 
		n->my_socket = 0;
		return -EJ_FAIL;
	}else
	{
		EJ_ErrPrintf(("[ConnectNetwork][INFO]: socket connect success!\r\n"));
		return EJ_SUCCESS;
	}


#endif
}


void NetworkDisconnect(Network* n)
{

	EJ_Printf("enter NetworkDisconnect\r\n");
	if(!n)
	{
		EJ_Printf("NetworkDisconnect\r\n");
		return;
	}
	shutdown(n->my_socket,SHUT_RDWR);
	close(n->my_socket); 
	n->my_socket = -1;	


}

void NewNetwork(Network* n)
{
	n->my_socket = 0;
	n->mqttread = ej_mqtt_read;
	n->mqttwrite = ej_mqtt_write;
	n->disconnect = ej_mqtt_disconnect;
}

