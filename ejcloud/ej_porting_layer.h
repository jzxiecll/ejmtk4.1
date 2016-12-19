#ifndef  __EJ_PORTING_LAYER_H__
#define  __EJ_PORTING_LAYER_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
//#include <wm_os.h>
//#include <wmerrno.h>
//#include <wm_utils.h>
//#include <wmstdio.h>
//#include <wmlog.h>
//#include <semdbg.h>
//#include <board.h>
#include "ej_log.h"
#include "ej_error.h"


//psm port inc
#include "nvdm.h"
#include "hal.h"
#include "hal_sys.h"
//uart port inc
//#include <mdev_uart.h>
//rtc port inc
//#include <rtc.h>

typedef unsigned char uint8_t;
//typedef unsigned int uint32_t;
typedef unsigned int size_t;
typedef long EJ_BaseType_t;

#define ejPASS			( ( EJ_BaseType_t ) 1 )
#define ejFAIL			( ( EJ_BaseType_t ) 0 )
#define ejFALSE			( ( EJ_BaseType_t ) 0 )
#define ejTRUE			( ( EJ_BaseType_t ) 1 )
#define  EJ_Printf  printf
#define  app_d      printf

#ifndef PACK_START
#define PACK_START
#endif
#ifndef PACK_END
#define PACK_END __attribute__((packed))
#endif
/** Wait Forever */
#define EJ_WAIT_FOREVER    portMAX_DELAY
/** Do Not Wait */
#define EJ_NO_WAIT         0
#define EJ_PRIO_0     4	 /** High **/
#define EJ_PRIO_1     3
#define EJ_PRIO_2     2
#define EJ_PRIO_3     1
#define EJ_PRIO_4     0	 /** Low **/


typedef xTaskHandle ej_thread_t;
typedef void *ej_thread_arg_t;
typedef struct ej_thread_stack {
	/** Total stack size */
	int size;
} ej_thread_stack_t;
#define ej_thread_stack_define(stackname, stacksize)		\
	ej_thread_stack_t stackname =				\
		{(stacksize) / (sizeof(portSTACK_TYPE))}
int EJ_thread_create(ej_thread_t *thandle, const char *name,
		     void (*main_func)(ej_thread_arg_t arg),
		     void *arg, ej_thread_stack_t *stack, int prio);
void EJ_thread_Suspend(ej_thread_t *thandle);

/*
************************************************************************
* Queue Management 
*
************************************************************************
*/


typedef void *ej_timer_arg_t;
typedef xTimerHandle ej_timer_t;
typedef portTickType ej_timer_tick;

/** OS Timer reload Options
 *
 */
typedef enum ej_timer_reload {
	/**
	 * Create one shot timer. Timer will be in the dormant state after
	 * it expires.
	 */
	EJ_TIMER_ONE_SHOT,
	/**
	 * Create a periodic timer. Timer will auto-reload after it expires.
	 */
	EJ_TIMER_PERIODIC,
} ej_timer_reload_t;

/**
 * OS Timer Activate Options
 */
typedef enum ej_timer_activate {
	/** Start the timer on creation. */
	EJ_TIMER_AUTO_ACTIVATE,
	/** Do not start the timer on creation. */
	EJ_TIMER_NO_ACTIVATE,
} ej_timer_activate_t;


/** Create timer**********************************************************
 **
 ***This function creates a timer.
 ***********************************************************************/
int EJ_timer_create(ej_timer_t *timer_t, const char *name, ej_timer_tick ticks,
		    void (*call_back) (ej_timer_arg_t), void *cb_arg,
		    ej_timer_reload_t reload, ej_timer_activate_t activate);
int EJ_timer_change(ej_timer_t *timer_t, ej_timer_tick ntime,
				ej_timer_tick block_time);
int EJ_timer_deactivate(ej_timer_t *timer_t);
bool EJ_timer_is_running(ej_timer_t *timer_t);
int EJ_timer_activate(ej_timer_t *timer_t);

/*
************************************************************************
* Queue Management 
*
************************************************************************
*/

typedef xQueueHandle ej_queue_t;
/** Structure used for queue definition */
typedef struct ej_queue_pool {
	/** Size of the queue */
	int size;
} ej_queue_pool_t;

#define ej_queue_pool_define(poolname, poolsize)	\
	ej_queue_pool_t poolname = { poolsize };

int EJ_queue_create(ej_queue_t *qhandle, const char *name, int msgsize, ej_queue_pool_t *poolname);
int EJ_queue_send(ej_queue_t *qhandle, const void *msg,
				unsigned long wait);
int EJ_queue_send_Overwrite(ej_queue_t *qhandle, const void *msg,
				unsigned long wait);

int EJ_queue_recv(ej_queue_t *qhandle, void *msg,
			   unsigned long wait);
int EJ_queue_delete(ej_queue_t *qhandle);
/*
************************************************************************
* Semaphore Manager 
*
************************************************************************
*/
typedef xSemaphoreHandle ej_semaphore_t;

#if 0
typedef QueueHandle_t ej_semaphore_t;


#define xSemaphoreGiveFromISR( xSemaphore, pxHigherPriorityTaskWoken )	xQueueGiveFromISR( ( QueueHandle_t ) ( xSemaphore ), ( pxHigherPriorityTaskWoken ) )
#define xSemaphoreTakeFromISR( xSemaphore, pxHigherPriorityTaskWoken )	xQueueReceiveFromISR( ( QueueHandle_t ) ( xSemaphore ), NULL, ( pxHigherPriorityTaskWoken ) )

#define xSemaphoreGive( xSemaphore )		xQueueGenericSend( ( QueueHandle_t ) ( xSemaphore ), NULL, semGIVE_BLOCK_TIME, queueSEND_TO_BACK )
#define xSemaphoreTake( xSemaphore, xBlockTime )		xQueueGenericReceive( ( QueueHandle_t ) ( xSemaphore ), NULL, ( xBlockTime ), pdFALSE )

#define vSemaphoreCreateBinary( xSemaphore )																							\
	{																																	\
		( xSemaphore ) = xQueueGenericCreate( ( UBaseType_t ) 1, semSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_BINARY_SEMAPHORE );	\
		if( ( xSemaphore ) != NULL )																									\
		{																																\
			( void ) xSemaphoreGive( ( xSemaphore ) );																					\
		}																																\
	}



static inline int EJ_semaphore_create(ej_semaphore_t *mhandle, const char *name)
{
	vSemaphoreCreateBinary(*mhandle);
	if (*mhandle) {
		//sem_debug_add((const xSemaphoreHandle)*mhandle,name, 1);
		return EJ_SUCCESS;
	}
	else
		return -EJ_FAIL;
}

static inline int EJ_semaphore_get(ej_semaphore_t *mhandle, unsigned long wait)
{
	int ret;
	signed portBASE_TYPE xHigherPriorityTaskWoken = ejFALSE;
	if (!mhandle || !(*mhandle))
		return -EJ_E_INVAL;
	
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xSemaphoreTakeFromISR(*mhandle,
					    &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else
		ret = xSemaphoreTake(*mhandle, wait);
	return ret == ejTRUE ? EJ_SUCCESS : -EJ_FAIL;
}



static inline int EJ_semaphore_put(ej_semaphore_t *mhandle)
{
	int ret;
	signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if (!mhandle || !(*mhandle))
		return -EJ_E_INVAL;

	
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xSemaphoreGiveFromISR(*mhandle,
					    &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else
		ret = xSemaphoreGive(*mhandle);
	return ret == ejTRUE ? EJ_SUCCESS : -EJ_FAIL;
}

#endif
/*
************************************************************************
* Memmoy Manager 
*
************************************************************************
*/

void* EJ_mem_malloc(size_t sz);
void EJ_mem_free(void *ptr);
void EJ_thread_sleep(int ticks);
unsigned long EJ_msec_to_ticks(unsigned long msecs);
/*
************************************************************************
* Mutex Manager 
*
************************************************************************
*/
typedef xSemaphoreHandle ej_mutex_t;
#define xSemaphoreCreateRecursiveMutex() xQueueCreateMutex( queueQUEUE_TYPE_RECURSIVE_MUTEX )
#define xSemaphoreTakeRecursive( xMutex, xBlockTime )	xQueueTakeMutexRecursive( ( xMutex ), ( xBlockTime ) )
#define xSemaphoreGiveRecursive( xMutex )	xQueueGiveMutexRecursive( ( xMutex ) )
#define vSemaphoreDelete( xSemaphore ) vQueueDelete( ( QueueHandle_t ) ( xSemaphore ) )
	
	
typedef struct Mutex
{
   	ej_mutex_t mtx;

} Mutex;
void MutexInit(Mutex* m);
void MutexDeinit(Mutex* m);
int MutexLock(Mutex* m);
int MutexUnlock(Mutex* m);


#endif
