#include "ej_porting_layer.h"


/*
* *****************************************************          
*   MEMMERY   MODULE
*******************************************************
*/

void* EJ_mem_malloc(size_t sz)
{  
    void *buf = NULL;
    buf = pvPortMalloc(sz);
    if(NULL == buf)
        EJ_Printf("joylink_malloc() Error! size:%d", sz);
    return buf;
}

void EJ_mem_free(void *ptr)
{   
    if(ptr)
        vPortFree(ptr);
}



/*
* *****************************************************          
*   THREAD   MODULE
*******************************************************
*/


 void EJ_thread_sleep(int ticks)
{

	vTaskDelay(ticks);
	return;
}

 unsigned long EJ_msec_to_ticks(unsigned long msecs)
{
	return (msecs) / (portTICK_RATE_MS);
}



int EJ_thread_create(ej_thread_t *thandle, const char *name,
		     void (*main_func)(ej_thread_arg_t arg),
		     void *arg, ej_thread_stack_t *stack, int prio)
{
	int ret;
	ret = xTaskCreate(main_func, name, stack->size, arg, prio, thandle);
	return ret == ejPASS ? EJ_SUCCESS : -EJ_FAIL;
}


void EJ_thread_Suspend(ej_thread_t *thandle)
{
	/* Suspend self until someone calls delete. This is required because in
	 * freeRTOS, main functions of a thread cannot return.
	 */
	if (thandle != NULL) {		
		vTaskSuspend(*thandle);
	} else {		
		vTaskSuspend(NULL);
	}

	/*
	 * We do not want this function to return ever.
	 */
}

int EJ_thread_delete(ej_thread_t *thandle)
{

	if (thandle == NULL) {		
		vTaskDelete(NULL);
	} else {		
		vTaskDelete(*thandle);
	}
	*thandle = NULL;
	return EJ_SUCCESS;
}


void EJ_task_Resume(ej_thread_t xTaskToResume)
{
	vTaskResume(xTaskToResume);
}


/*
* *****************************************************          
*   TIMER   MODULE
*******************************************************
*/
#define is_isr_context() (__get_IPSR() != 0)
int EJ_timer_change(ej_timer_t *timer_t, ej_timer_tick ntime,
				  ej_timer_tick block_time)
{
	int ret;
	portBASE_TYPE xHigherPriorityTaskWoken = ejFALSE;
	if (!timer_t || !(*timer_t))
		return -EJ_E_INVAL;
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xTimerChangePeriodFromISR(*timer_t, ntime,
						&xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else
		/* Fixme: What should be value of xBlockTime? */
		ret = xTimerChangePeriod(*timer_t, ntime, 100);
	return ret == ejPASS ? EJ_SUCCESS : -EJ_FAIL;
}


 int EJ_timer_deactivate(ej_timer_t *timer_t)
{
	int ret;
	portBASE_TYPE xHigherPriorityTaskWoken = ejFALSE;
	if (!timer_t || !(*timer_t))
		return -EJ_E_INVAL;
	/* Note:
	 * XTimerStop, seconds argument is xBlockTime which means, the time,
	 * in ticks, that the calling task should be held in the Blocked
	 * state, until timer command succeeds.
	 * We are giving as 0, to be consistent with threadx logic.
	 */
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xTimerStopFromISR(*timer_t, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else
		ret = xTimerStop(*timer_t, 0);
	return ret == ejPASS ? EJ_SUCCESS : -EJ_FAIL;
}



 bool EJ_timer_is_running(ej_timer_t *timer_t)
{
	int ret;
	if (!timer_t || !(*timer_t))
		return false;
	ret = xTimerIsTimerActive(*timer_t);
	return ret == ejPASS ? true : false;
}



int EJ_timer_activate(ej_timer_t *timer_t)
{
	int ret;
	portBASE_TYPE xHigherPriorityTaskWoken = ejFALSE;
	if (!timer_t || !(*timer_t))
		return -EJ_E_INVAL;
	/* Note:
	 * XTimerStart, seconds argument is xBlockTime which means, the time,
	 * in ticks, that the calling task should be held in the Blocked
	 * state, until timer command succeeds.
	 * We are giving as 0, to be consistent with threadx logic.
	 */
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xTimerStartFromISR(*timer_t, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else
		ret = xTimerStart(*timer_t, 0);
	return ret == ejPASS ? EJ_SUCCESS : -EJ_FAIL;
}



int EJ_timer_create(ej_timer_t *timer_t, const char *name, ej_timer_tick ticks,
		    void (*call_back)(ej_timer_arg_t), void *cb_arg,
		    ej_timer_reload_t reload,
		    ej_timer_activate_t activate)
{
	int auto_reload = (reload == EJ_TIMER_ONE_SHOT) ? ejFALSE : ejTRUE;
	*timer_t = xTimerCreate(name, ticks, auto_reload, cb_arg, call_back);
	if (*timer_t == NULL)
		return -EJ_FAIL;
	if (activate == EJ_TIMER_AUTO_ACTIVATE)
		return EJ_timer_activate(timer_t);
	return EJ_SUCCESS;
}


int EJ_queue_create(ej_queue_t *qhandle, const char *name, int msgsize, ej_queue_pool_t *poolname)
{

	/** The size of the pool divided by the max. message size gives the
	 * max. number of items in the queue. */	
	*qhandle = xQueueCreate(poolname->size/msgsize, msgsize);	
	if (*qhandle) {
		return EJ_SUCCESS;
	}
	return -EJ_FAIL;
}


/** Receive an item from queue
 *
 *
 */

int EJ_queue_send_Overwrite(ej_queue_t *qhandle, const void *msg,
				unsigned long wait)
{
	int ret;
	signed portBASE_TYPE xHigherPriorityTaskWoken = ejFALSE;
	if (!qhandle || !(*qhandle))
		return -EJ_E_INVAL;
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xQueueOverwriteFromISR(*qhandle, msg,
					      &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else{
		ret = xQueueOverwrite(*qhandle, msg);
		
	}
	return ret == ejTRUE ? EJ_SUCCESS : -EJ_FAIL;
}



int EJ_queue_send(ej_queue_t *qhandle, const void *msg,
				unsigned long wait)
{
	int ret;
	signed portBASE_TYPE xHigherPriorityTaskWoken = ejFALSE;
	if (!qhandle || !(*qhandle))
		return -EJ_E_INVAL;
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xQueueSendToBackFromISR(*qhandle, msg,
					      &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else{
		ret = xQueueSendToBack(*qhandle, msg, wait);
		
	}
	return ret == ejTRUE ? EJ_SUCCESS : -EJ_FAIL;
}


/** Receive an item from queue
 *
 * 
 */
 int EJ_queue_recv(ej_queue_t *qhandle, void *msg,
				unsigned long wait)
{
	int ret;
	if (!qhandle || !(*qhandle))
		return -EJ_E_INVAL;
	ret = xQueueReceive(*qhandle, msg, wait);
	return ret == ejTRUE ? EJ_SUCCESS : -EJ_FAIL;
}

/** Delete queue
 *
 * 
 */
 int EJ_queue_delete(ej_queue_t *qhandle)
{
	
	vQueueDelete(*qhandle);
	//sem_debug_delete((const xSemaphoreHandle)*qhandle);
	*qhandle = NULL;
	return EJ_SUCCESS;
}


 int EJ_queue_get_msgs_waiting(ej_queue_t *qhandle)
{
	int nmsg = 0;
	if (!qhandle || !(*qhandle))
		return -EJ_E_INVAL;
	nmsg = uxQueueMessagesWaiting(*qhandle);
	return nmsg;
}


/*
*
*
*
*
*/
static  int ej_recursive_mutex_create(ej_mutex_t *mhandle,
					    const char *name)
{
	if (!mhandle)
		return -EJ_E_INVAL;
	*mhandle = xSemaphoreCreateRecursiveMutex();
	if (!*mhandle)
		return -EJ_FAIL;
	return EJ_SUCCESS;
}


static  int ej_recursive_mutex_get(ej_mutex_t *mhandle,
					 unsigned long wait)
{	
	int ret = xSemaphoreTakeRecursive(*mhandle, wait);
	return ret == ejTRUE ? EJ_SUCCESS : -EJ_FAIL;
}


static  int ej_recursive_mutex_put(ej_mutex_t *mhandle)
{
	
	int ret = xSemaphoreGiveRecursive(*mhandle);
	return ret == ejTRUE ? EJ_SUCCESS : -EJ_FAIL;
}


static  int ej_mutex_delete(ej_mutex_t *mhandle)
{
	vSemaphoreDelete(*mhandle);	
	*mhandle = NULL;
	return EJ_SUCCESS;
}

void MutexInit(Mutex* m)
{
    if (!m)
    {
        return;
    }
    ej_recursive_mutex_create(&(m->mtx), "mutex");	
}

void MutexDeinit(Mutex* m)
{
    if (!m)
    {
        return;
    }
	ej_mutex_delete(&(m->mtx));
}


int MutexLock(Mutex* m)
{
    if (!m)
    {
        return -1;
    }   
	ej_recursive_mutex_get(&(m->mtx), EJ_WAIT_FOREVER);
    return 0;
}


int MutexUnlock(Mutex* m)
{
    if (!m)
    {
        return -1;
    }
	ej_recursive_mutex_put(&(m->mtx));
    return 0;
}


