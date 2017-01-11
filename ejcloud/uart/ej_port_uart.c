#include "ej_port_uart.h"
#include "hal_uart.h"

#define EJ_UART_DMA 1

static hal_uart_port_t g_ejcloud_uart_port;
static SemaphoreHandle_t mutex_tx, mutex_rx;
static char rx_vfifo_buffer[512] __attribute__ ((section(".noncached_zidata")));
static char tx_vfifo_buffer[512] __attribute__ ((section(".noncached_zidata")));

static void user_uart_callback(hal_uart_callback_event_t status, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken;

    if (status == HAL_UART_EVENT_READY_TO_WRITE) {
        xSemaphoreGiveFromISR(mutex_tx, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    else if (status == HAL_UART_EVENT_READY_TO_READ) {
        xSemaphoreGiveFromISR(mutex_rx, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

}

//static int joylink_port_uart_dma_init(uint8_t port)
//{
//  

//}


//bool joylink_port_uart_dma_send(char *buf, int len)
//{
//    
//}


//int joylink_port_uart_dma_receive(char *buf, int len)
//{
//   
//}


static int EJ_port_uart_dma_init( EJ_UART_ID_Type port, uint32_t baud)
{
	/*Need to do */
	int ret=0;
	hal_uart_config_t uart_config;
	hal_uart_dma_config_t dma_config;
 	g_ejcloud_uart_port = port;

    hal_uart_deinit(g_ejcloud_uart_port);

	if(port == HAL_UART_0 ){
			hal_gpio_init(HAL_GPIO_2);
			hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
			hal_gpio_init(HAL_GPIO_3);
			hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);

	}else{
			hal_gpio_init(HAL_GPIO_36);
			hal_pinmux_set_function(HAL_GPIO_36, HAL_GPIO_36_UART2_RX_CM4);
			hal_gpio_init(HAL_GPIO_37);
			hal_pinmux_set_function(HAL_GPIO_37, HAL_GPIO_37_UART2_TX_CM4);

	}
   uart_config.baudrate = HAL_UART_BAUDRATE_9600;
   uart_config.parity = HAL_UART_PARITY_NONE;
   uart_config.stop_bit = HAL_UART_STOP_BIT_1;
   uart_config.word_length = HAL_UART_WORD_LENGTH_8;
   
   ret = hal_uart_init(g_ejcloud_uart_port, &uart_config);
   if(ret<0)
   {	
		printf("[%s : %d]\n",__FUNCTION__,__LINE__);
		return ret;
   }
	mutex_tx = xSemaphoreCreateBinary();
	mutex_rx = xSemaphoreCreateBinary();
   
   dma_config.receive_vfifo_alert_size = 36;
   dma_config.receive_vfifo_buffer = rx_vfifo_buffer;
   dma_config.receive_vfifo_buffer_size = 512;
   dma_config.receive_vfifo_threshold_size = 36;
   dma_config.send_vfifo_buffer = tx_vfifo_buffer;
   dma_config.send_vfifo_buffer_size = 512;
   dma_config.send_vfifo_threshold_size = 36;
   if(hal_uart_set_dma(g_ejcloud_uart_port, &dma_config))
   {
		printf("[%s : %d]\n",__FUNCTION__,__LINE__);
		return -1;
   }
   if(hal_uart_register_callback(g_ejcloud_uart_port, user_uart_callback, NULL))
   {
		printf("[%s : %d]\n",__FUNCTION__,__LINE__);
		return -1;
   }

   return HAL_UART_STATUS_OK;
}

static int EJ_port_uart_poll_init(EJ_UART_ID_Type port_id, uint32_t baud)
{
	/*Need to do */	
	return 0;
}



static int EJ_port_uart_dma_send(uint8_t *buf, int len)
{
		/*Need to do */
	uint32_t avail_space, left_size, real_byte, sent_byte;
    char *pbuf;

    pbuf = buf;
    left_size = len;
    while(1){
        avail_space = hal_uart_get_available_send_space(g_ejcloud_uart_port);
        if(avail_space > left_size) {
            real_byte = left_size;
        }
        else {
            real_byte = avail_space;
        }
        sent_byte = hal_uart_send_dma(g_ejcloud_uart_port, (const uint8_t *)pbuf, real_byte);
        left_size -= sent_byte;
        pbuf += sent_byte;
        if(sent_byte != real_byte) {
            return false;
        }
        if(!left_size) {
            break;
        }
        xSemaphoreTake(mutex_tx, portMAX_DELAY);
    }
    return true;

}

static int EJ_port_uart_poll_send(uint8_t *buf, int len)
{

	return 0;
	/*Need to do */
   
}

static int EJ_port_uart_dma_receive(uint8_t *buf, int len)
{
	uint32_t left_size, avail_bytes, rcv_bytes;
    char *pbuf;

    pbuf = buf;
    left_size = len;
    while (1) {
        avail_bytes = hal_uart_get_available_receive_bytes(g_ejcloud_uart_port);
        avail_bytes = (left_size < avail_bytes) ? left_size : avail_bytes;
        rcv_bytes = hal_uart_receive_dma(g_ejcloud_uart_port, (uint8_t *)buf, avail_bytes);
        left_size -= rcv_bytes;
        pbuf += rcv_bytes;
        if (rcv_bytes != avail_bytes) {
            return 0;
        }
        if (!left_size) {
            return len;
        }
        xSemaphoreTake(mutex_rx, portMAX_DELAY);
    }
	/*Need to do */


}

static int EJ_port_uart_poll_receive(uint8_t *buf, int len)
{
	return 0;
	/*Need to do */
}
static int EJ_port_uart_close()
{
	/*Need to do */	
	return hal_uart_deinit(g_ejcloud_uart_port);
}


/*
 *	 EJ_UART_READ	  
 *
 */
 uint32_t EJ_uart_read( uint8_t *buf, uint32_t len)
{
#if EJ_UART_DMA
	  return EJ_port_uart_dma_receive(buf,len);
#else
	  return EJ_port_uart_poll_receive(buf,len);
#endif
}


/*
 *	 EJ_UART_WRITE	  
 *
 */
 bool EJ_uart_write( uint8_t *buf, uint32_t len)
{	  	
#if EJ_UART_DMA
	  return EJ_port_uart_dma_send(buf,  len);
#else
	  return EJ_port_uart_poll_send(buf,len);
#endif
}

/*
*   EJ_UART_OPEN     
*
*/
static int EJ_port_uart_open(EJ_UART_ID_Type port_id, uint32_t baud)
{
#if EJ_UART_DMA
	return EJ_port_uart_dma_init(port_id,baud);
#else
	return EJ_port_uart_poll_init(port_id,baud);
#endif	
}


int EJ_uart_open(EJ_UART_ID_Type port_id, uint32_t baud)
{

	if(EJ_port_uart_open(port_id,baud)!=0)
	{
		EJ_DebugPrintf(("[EJ_uart_open] uart_open failed!\r\n"));
		return -1;
	}else
	{
		EJ_DebugPrintf(("[EJ_uart_open] uart_open success!\r\n"));
		return 0;
	}

}


/*
*   EJ_UART_OPEN     
*
*/
int EJ_uart_close(EJ_UART_ID_Type uartX)
{
	EJ_port_uart_close();
	return 0;
}


 int ejcloud_port_uart_open(uint8_t port)
{
#if 1
	return EJ_port_uart_dma_init(port,9600);
#else
	return ejcloud_port_uart_poll_init(port);
#endif	
}


