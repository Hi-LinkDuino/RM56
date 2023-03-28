/*
 * Copyright (c) 2013-2017 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ----------------------------------------------------------------------
 *
 * $Date:        10. January 2017
 * $Revision:    V1.2
 *
 * Project:      CMSIS-RTOS API V1
 * Title:        cmsis_os_v1.c V1 module file
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "cmsis_os.h"
#include "task.h"

#if (osCMSIS >= 0x20000U) && !defined(os1_Disable)

// Thread
#if !defined(os1_Disable_Thread)
osThreadId osThreadCreate (const osThreadDef_t *thread_def, void *argument) {

  if (thread_def == NULL) {
    return NULL;
  }
  return osThreadNew((osThreadFunc_t)thread_def->pthread, argument, &thread_def->attr);
}
#endif


// Signals

#if !defined(os1_Disable_Signal)

#define SignalMask ((1U<<osFeature_Signals)-1U)

int32_t osSignalSet (osThreadId thread_id, int32_t signals) {
  uint32_t flags;

  flags = osThreadFlagsSet(thread_id, (uint32_t)signals);
  if ((flags & 0x80000000U) != 0U) {
    return ((int32_t)0x80000000U);
  }
  return ((int32_t)(flags & ~((uint32_t)signals)));
}

int32_t osSignalClear (osThreadId thread_id, int32_t signals) {
  uint32_t flags;

  if (thread_id != osThreadGetId()) {
    return ((int32_t)0x80000000U);
  }
  flags = osThreadFlagsClear((uint32_t)signals);
  if ((flags & 0x80000000U) != 0U) {
    return ((int32_t)0x80000000U);
  }
  return ((int32_t)flags);
}

os_InRegs osEvent osSignalWait (int32_t signals, uint32_t millisec) {
  osEvent  event;
  uint32_t flags;

  if (signals != 0) {
    flags = osThreadFlagsWait((uint32_t)signals, osFlagsWaitAll, millisec);
  } else {
    flags = osThreadFlagsWait(SignalMask,        osFlagsWaitAny, millisec);
  }
  if ((flags > 0U) && (flags < 0x80000000U)) {
    event.status = osEventSignal;
    event.value.signals = (int32_t)flags;
  } else {
    switch ((int32_t)flags) {
      case osErrorResource:
        event.status = osOK;
        break;
      case osErrorTimeout:
        event.status = osEventTimeout;
        break;
      case osErrorParameter:
        event.status = osErrorValue;
        break;
      default:
        event.status = (osStatus)flags;
        break;
    }
  }
  return event;
}

#endif  // Signal


// Timer
#if !defined(os1_Disable_Timer)
osTimerId osTimerCreate (const osTimerDef_t *timer_def, os_timer_type type, void *argument) {

  if (timer_def == NULL) {
    return NULL;
  }
  return osTimerNew((osTimerFunc_t)timer_def->ptimer, type, argument, &timer_def->attr);
}
#endif


// Mutex
#if !defined(os1_Disable_Mutex)
osMutexId osMutexCreate (const osMutexDef_t *mutex_def) {

  if (mutex_def == NULL) {
    return NULL;
  }
  return osMutexNew(mutex_def);
}
#endif


// Semaphore

#if (defined (osFeature_Semaphore) && (osFeature_Semaphore != 0U)) && !defined(os1_Disable_Semaphore)

osSemaphoreId osSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count) {

  if (semaphore_def == NULL) {
    return NULL;
  }
  return osSemaphoreNew((uint32_t)osFeature_Semaphore, (uint32_t)count, semaphore_def);
}

int32_t osSemaphoreWait (osSemaphoreId semaphore_id, uint32_t millisec) {
  osStatus_t status;
  uint32_t   count;

  status = osSemaphoreAcquire(semaphore_id, millisec);
  switch (status) {
    case osOK:
      count = osSemaphoreGetCount(semaphore_id);
      return ((int32_t)count + 1);
    case osErrorResource:
    case osErrorTimeout:
      return 0;
    default:
      break;
  }
  return -1;
}

#endif  // Semaphore


// Memory Pool

#if (defined(osFeature_Pool) && (osFeature_Pool != 0))&& !defined(os1_Disable_Pool)
#include "rt_MemBox.h"

// Clear Memory Box (Zero init)
static void rt_clr_box (void *box_mem, void *box) {
  uint32_t *p, n;

  if ((box_mem != NULL) && (box != NULL)) {
    p = box;
    for (n = ((P_BM)box_mem)->blk_size; n; n -= 4) {
      *p++ = 0;
    }
  }
}

/// Create and Initialize memory pool
static osPoolId svcPoolCreate (const osPoolDef_t *pool_def) {
  uint32_t blk_sz;

  if ((pool_def == NULL) ||
      (pool_def->pool_sz == 0) ||
      (pool_def->item_sz == 0) ||
      (pool_def->pool == NULL)) {
    configASSERT(0);
    return NULL;
  }

  blk_sz = (pool_def->item_sz + 3) & ~3;
  _init_box(pool_def->pool, sizeof(struct OS_BM) + pool_def->pool_sz * blk_sz, blk_sz);

  return pool_def->pool;
}

/// Allocate a memory block from a memory pool
void *sysPoolAlloc (osPoolId pool_id, uint32_t clr) {
  void *ptr;

  if (pool_id == NULL) {
      configASSERT(0);
      return NULL;
  }

  __disable_irq();
  ptr = rt_alloc_box(pool_id);
  if (clr) {
    rt_clr_box(pool_id, ptr);
  }

  if (!ptr || (uint32_t)ptr < (uint32_t)pool_id) {
      TRACE(1,"cannot alloc pool for id %p", pool_id);
      configASSERT(0);
  }
  __enable_irq();

  return ptr;
}

/// Return an allocated memory block back to a specific memory pool
osStatus sysPoolFree (osPoolId pool_id, void *block) {
  int32_t res;

  if (pool_id == NULL) {
      configASSERT(0);
      return osErrorParameter;
  }

  res = rt_free_box(pool_id, block);
  if (res != 0) return osErrorValue;

  return osOK;
}

// Memory Management Public API
osPoolId osPoolCreate (const osPoolDef_t *pool_def) {

  if (__get_IPSR() != 0) {
      TRACE(0,"not allow create pool in ISR");
      configASSERT(0);
      return NULL;           // Not allowed in ISR
  }
  if (((__get_CONTROL() & 1) == 0) && (osKernelGetState() != osKernelRunning)) {
    // Privileged and not running
    return   svcPoolCreate(pool_def);
  } else {
    return   svcPoolCreate(pool_def);
  }
}

void *osPoolAlloc (osPoolId pool_id) {
    return  sysPoolAlloc(pool_id, 0);
}

void *osPoolCAlloc (osPoolId pool_id) {

    return   sysPoolAlloc(pool_id, 1);
}

osStatus osPoolFree (osPoolId pool_id, void *block) {

    return sysPoolFree(pool_id, block);
}

#endif  // Memory Pool


// Message Queue

#if (defined(osFeature_MessageQ) && (osFeature_MessageQ != 0)) && !defined(os1_Disable_MessageQ)

osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id) {
  (void)thread_id;

  if (queue_def == NULL) {
    return NULL;
  }
  return osMessageQueueNew(queue_def->queue_sz, sizeof(uint32_t), &queue_def->attr);
}

osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec) {
  return osMessageQueuePut(queue_id, &info, 0U, millisec);
}

os_InRegs osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec) {
  osStatus_t status;
  osEvent    event;
  uint32_t   message;

  status = osMessageQueueGet(queue_id, &message, NULL, millisec);
  switch (status) {
    case osOK:
      event.status = osEventMessage;
      event.value.v = message;
      break;
    case osErrorResource:
      event.status = osOK;
      break;
    case osErrorTimeout:
      event.status = osEventTimeout;
      break;
    default:
      event.status = status;
      break;
  }
  return event;
}

uint32_t osMessageGetSpace (osMessageQId queue_id)
{
    return osMessageQueueGetSpace(queue_id);
}

#endif  // Message Queue


// Mail Queue

#if (defined(osFeature_MailQ) && (osFeature_MailQ != 0)) && !defined(os1_Disable_MailQ)

typedef struct os_mail_queue_s {
  osPoolId   mp_id;
  osMessageQueueId_t mq_id;
} os_mail_queue_t;

osMailQId osMailCreate (osMailQDef_t *queue_def, osThreadId thread_id) {

  os_mail_queue_t *ptr;
  (void)thread_id;

  if (queue_def == NULL) {
    configASSERT(0);
    return NULL;
  }

  ptr = pvPortMalloc (sizeof(os_mail_queue_t));
  if (ptr == NULL) {
    configASSERT(0);
    return NULL;
  }

  queue_def->mail = (void *)ptr;

  ptr->mp_id = osPoolCreate  (&queue_def->mq_pool);
  ptr->mq_id = osMessageQueueNew(queue_def->mq_pool.pool_sz, sizeof(void *), &queue_def->mq_attr);
  if ((ptr->mp_id == NULL) || (ptr->mq_id == NULL)) {
    if (ptr->mp_id != NULL) {
      configASSERT(0);
    }
    if (ptr->mq_id != NULL) {
      configASSERT(0);
      osMessageQueueDelete(ptr->mq_id);
    }
    return NULL;
  }

  return ptr;
}

void *osMailAlloc (osMailQId queue_id, uint32_t millisec) {
    void *mail;

    os_mail_queue_t *ptr = (os_mail_queue_t *)queue_id;
    if (ptr == NULL) {
        configASSERT(0);
        return NULL;
    }
    if(millisec != 0) {
        TRACE(0,"currently dont support sleep if there is no mail available");
        configASSERT(0);
        return NULL;
    }

    mail = osPoolAlloc(ptr->mp_id);
    return mail;
}

void *osMailCAlloc (osMailQId queue_id, uint32_t millisec) {
    return osMailAlloc(queue_id, millisec);
}

osStatus osMailPut (osMailQId queue_id, const void *mail) {
  os_mail_queue_t *ptr = (os_mail_queue_t *)queue_id;

  if (ptr == NULL) {
    configASSERT(0);
    return osErrorParameter;
  }

  if (mail == NULL) {
    configASSERT(0);
    return osErrorValue;
  }
  return osMessageQueuePut(ptr->mq_id, &mail, 0U, 0U);
}

os_InRegs osEvent osMailGet (osMailQId queue_id, uint32_t millisec) {
  os_mail_queue_t *ptr = (os_mail_queue_t *)queue_id;
  osStatus_t       status;
  osEvent          event;
  void            *mail = NULL;

  if (ptr == NULL) {
    configASSERT(0);
    event.status = osErrorParameter;
    return event;
  }

  status = osMessageQueueGet(ptr->mq_id, &mail, NULL, millisec);
  switch (status) {
    case osOK:
      if ((uint32_t)mail < (uint32_t)ptr->mp_id)
        configASSERT(0);
      event.status = osEventMail;
      event.value.p = mail;
      break;
    case osErrorResource:
      event.status = osOK;
      break;
    case osErrorTimeout:
      event.status = osEventTimeout;
      break;
    default:
      event.status = status;
      break;
  }
  return event;
}

osStatus osMailFree (osMailQId queue_id, void *mail) {
  os_mail_queue_t *ptr = (os_mail_queue_t *)queue_id;

  if (ptr == NULL) {
    configASSERT(0);
    return osErrorParameter;
  }
  if (mail == NULL) {
    configASSERT(0);
    return osErrorValue;
  }
  osPoolFree(ptr->mp_id, mail);
  return osOK;
}

uint32_t osMailGetCount (osMailQId queue_id) {
  os_mail_queue_t *ptr = (os_mail_queue_t *)queue_id;

  if (ptr == NULL) {
    return 0;
  }
  return osMessageQueueGetCount(ptr->mq_id);
}

#endif  // Mail Queue

int task_idle_health_check(void)
{
    return 0;
}

#endif  // osCMSIS
