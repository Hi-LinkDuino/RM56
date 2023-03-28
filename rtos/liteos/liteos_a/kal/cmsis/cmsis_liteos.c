/***************************************************************************
 *
 * Copyright 2021-2031 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 * 
 ****************************************************************************/

#include "cmsis_os.h"
#include "los_config.h"
#include "string.h"
#include "stdio.h"
#include "los_oom.h"
#include "los_sys.h"
#include "los_tick.h"
#include "los_task_pri.h"
#include "los_printf.h"
#include "los_swtmr.h"
#include "los_swtmr_pri.h"
#include "los_memory_pri.h"
#include "los_sem_pri.h"
#include "los_mux_pri.h"
#include "los_rwlock_pri.h"
#include "los_queue_pri.h"
#include "los_memstat_pri.h"
#include "los_hwi_pri.h"
#include "los_spinlock.h"
#include "los_mp.h"
#include "los_atomic.h"
#include "los_exc_pri.h"
#include "gic_common.h"
#include "los_vm_boot.h"
#include "string.h"
#include "securec.h"
#include "hal_trace.h"

#define UNUSED(var) do { (void)var; } while (0)

// Thread
osThreadId osThreadCreate (const osThreadDef_t *thread_def, void *argument) {
    osThreadId thread_Id;

    if (thread_def == NULL) {
        return (osThreadId)NULL;
    }
    thread_Id = osThreadNew((osThreadFunc_t)thread_def->pthread, argument, &thread_def->attr);

    return thread_Id;
}


// Signals
#if 1
#define SignalMask ((1U<<osFeature_Signals)-1U)

int32_t osSignalSet (osThreadId thread_id, int32_t signals) {
    uint32_t flags;

    if (!thread_id) {
        TRACE(0, "%s=>%s   %d thread_Id=%p \n", __FILE__,__func__, __LINE__, thread_id);
        return ((int32_t)0x80000000U);
    }

    flags = osEventFlagsSet(thread_id->ef_id, (uint32_t)signals);
    if (flags == osFlagsErrorParameter) {
        return ((int32_t)0x80000000U);
    }

    return (int32_t)flags;
}

int32_t osSignalClear (osThreadId thread_id, int32_t signals) {
    uint32_t flags;

    if (!thread_id) {
        TRACE(0, "%s=>%s   %d thread_Id=%p \n", __FILE__,__func__, __LINE__, thread_id);
        return ((int32_t)0x80000000U);
    }

    if (thread_id != osThreadGetId()) {
        return ((int32_t)0x80000000U);
    }

    flags = osEventFlagsClear(thread_id->ef_id, (uint32_t)signals);
    if (flags == osFlagsErrorParameter) {
        return ((int32_t)0x80000000U);
    }
  return ((int32_t)flags);
}

osEvent osSignalWait (int32_t signals, uint32_t millisec) {
    osEvent  event = {0};
    int32_t flags;
    osThreadId thread_id = osThreadGetId();

    if (!thread_id) {
        TRACE(0, "%s=>%s   %d thread_Id=%p \n", __FILE__,__func__, __LINE__, thread_id);
        event.status = osErrorValue;
        return event;
    }

    if (signals != 0) {
        flags = (int32_t) osEventFlagsWait(thread_id->ef_id, (uint32_t)signals, osFlagsWaitAll, millisec);
    } else {
        flags = (int32_t) osEventFlagsWait(thread_id->ef_id, SignalMask, osFlagsWaitAny, millisec);
    }

    if (flags > 0) {
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
#endif

#if 0
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

osEvent osSignalWait (int32_t signals, uint32_t millisec) {
  osEvent  event = {0};
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
#endif
#if 0
// can only use 31 bits at most
#if (osFeature_Signals > 0x1F)
#error exceed max event bit count
#endif

#define VALID_EVENT_MASK   ((UINT32)((1U << osFeature_Signals) - 1))
#define INVALID_EVENT_MASK ((UINT32)(~VALID_EVENT_MASK))

int32_t osSignalSet(osThreadId thread_id, int32_t signals)
{
    LosTaskCB *taskCB = (LosTaskCB *)thread_id;
    UINT32 events = (UINT32)signals;
    UINT32 intSave;
    UINT32 ret;
    EVENT_CB_S *eventCB = NULL;
    UINT32 eventSave;

    if ((taskCB == NULL) || (events & INVALID_EVENT_MASK)) {
        return (int32_t)0x80000000;
    }
    if (OS_TASK_ID_CHECK_INVALID(taskCB->taskID) || (taskCB->taskStatus & OS_TASK_STATUS_UNUSED)) {
        return (int32_t)0x80000000;
    }

    eventCB = &(taskCB->event);
    intSave = LOS_IntLock();
    eventSave = eventCB->uwEventID;
    LOS_IntRestore(intSave);

    ret = LOS_EventWrite(eventCB, events);
    if (ret == LOS_OK) {
        return (int32_t)eventSave;
    } else {
        return (int32_t)0x80000000;
    }
}

int32_t osSignalClear(osThreadId thread_id, int32_t signals)
{
    LosTaskCB *taskCB = (LosTaskCB *)thread_id;
    UINT32 events = (UINT32)signals;
    UINT32 intSave;
    UINT32 ret;
    EVENT_CB_S *eventCB = NULL;
    UINT32 eventSave;

    if ((taskCB == NULL) || (events & INVALID_EVENT_MASK) || OS_INT_ACTIVE) {
        return (int32_t)0x80000000;
    }
    if (OS_TASK_ID_CHECK_INVALID(taskCB->taskID) || (taskCB->taskStatus & OS_TASK_STATUS_UNUSED)) {
        return (int32_t)0x80000000;
    }

    eventCB = &(taskCB->event);
    intSave = LOS_IntLock();
    eventSave = eventCB->uwEventID;
    LOS_IntRestore(intSave);

    ret = LOS_EventClear(eventCB, ~events);
    if (ret == LOS_OK) {
        return (int32_t)eventSave;
    } else {
        return (int32_t)0x80000000;
    }
}

osEvent osSignalWait(int32_t signals, uint32_t millisec)
{
    UINT32 events;
    UINT32 ret;
    osEvent evt = {0};
    UINT32 flags = 0;
    LosTaskCB *runTask = NULL;

    if (OS_INT_ACTIVE) {
        evt.status = osErrorISR;
        return evt;
    }
    if ((UINT32)signals & INVALID_EVENT_MASK) {
        evt.status = osErrorValue;
        return evt;
    }

    if (signals != 0) {
        events = (UINT32)signals;
        flags |= LOS_WAITMODE_AND;
    } else {
        events = VALID_EVENT_MASK;
        flags |= LOS_WAITMODE_OR;
    }

    
    UINT32 tsk_id = LOS_CurTaskIDGet();
	TSK_INFO_S tskInfo = {0};

	ret = LOS_TaskInfoGet(tsk_id, &tskInfo);
	if(ret != LOS_OK)
	{
        evt.status = osErrorValue;
        TRACE(0, "%s=> %s %d ERROR==================(%s)\n", __FILE__, __func__, __LINE__, LOS_CurTaskNameGet());
	    return evt;
	}
    ret = LOS_EventRead(&(tskInfo.uwEvent), events, flags | LOS_WAITMODE_CLR, LOS_MS2Tick(millisec));
    if (ret & LOS_ERRTYPE_ERROR) {
        if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT) {
            evt.status = osEventTimeout;
        } else {
            evt.status = osErrorResource;
        }
    } else {
        if (ret == 0) {
            evt.status = osOK;
        } else {
            evt.status = osEventSignal;
            evt.value.signals = (int32_t)ret;
        }
    }

    return evt;
}
#endif


// Timer
osTimerId osTimerCreate (const osTimerDef_t *timer_def, os_timer_type type, void *argument) {

  if (timer_def == NULL) {
    return (osTimerId)NULL;
  }
  return osTimerNew((osTimerFunc_t)timer_def->ptimer, type, argument, &timer_def->attr);
}


// Mutex
osMutexId osMutexCreate (const osMutexDef_t *mutex_def) {

  if (mutex_def == NULL) {
    return (osMutexId)NULL;
  }
  return osMutexNew(mutex_def);
}


// Semaphore

osSemaphoreId osSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count) {

  if (semaphore_def == NULL) {
    return (osSemaphoreId)NULL;
  }
  return osSemaphoreNew(osRtxSemaphoreTokenLimit, (uint32_t)count, semaphore_def);
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

#if 0
// Memory Pool
/**
 * @ingroup los_membox
 * Static memory pool information structure
 */
typedef struct {
    UINT32 uwBlkSize;           /**< The memory block size of the static memory pool */
    UINT32 uwBlkNum;            /**< The total memory block number of the static memory pool */
    UINT32 uwBlkCnt;            /**< The number of allocated memory blocks in the static memory pool */
#ifdef LOSCFG_KERNEL_MEMBOX_STATIC
    LOS_MEMBOX_NODE stFreeList; /**< The list of free memory block node in the static memory pool. This
                                     structure member is available only LOSCFG_KERNEL_MEMBOX_STATIC is
                                     defined. */
#endif
} LOS_MEMBOX_INFO;
#endif
#define LOS_MEMBOX_ALLIGNED(memAddr) (((UINTPTR)(memAddr) + sizeof(UINTPTR) - 1) & (~(sizeof(UINTPTR) - 1)))

/* internal function for osPool and osMail */
static void *CreateAndInitMemBox(UINT32 blkNum, UINT32 blkSize)
{
    VOID *memBox = NULL;
    UINT32 poolSize;
    UINT32 ret;
    UINT32 blkSizeAlign;

    if ((blkNum == 0) || (blkSize == 0)) {
        return NULL;
    }

    blkSizeAlign = LOS_MEMBOX_ALLIGNED(blkSize + OS_MEMBOX_NODE_HEAD_SIZE);
    poolSize = blkSizeAlign * blkNum + sizeof(LOS_MEMBOX_INFO);
    memBox = LOS_MemAlloc(m_aucSysMem0, poolSize);
    if (memBox == NULL) {
        return NULL;
    }

    ret = LOS_MemboxInit(memBox, poolSize, blkSize);
    if (ret != LOS_OK) {
        LOS_MemFree(m_aucSysMem0, memBox);
        return NULL;
    }

    return memBox;
}

osPoolId osPoolCreate(const osPoolDef_t *pool_def)
{
    if ((pool_def == NULL) || OS_INT_ACTIVE) {
        return NULL;
    }

    return (osPoolId)CreateAndInitMemBox(pool_def->pool_sz, pool_def->item_sz);
}

void *osPoolAlloc(osPoolId pool_id)
{
    if (pool_id == NULL) {
        return NULL;
    }

    return LOS_MemboxAlloc(pool_id);
}

void *osPoolCAlloc(osPoolId pool_id)
{
    void *ptr = NULL;

    if (pool_id == NULL) {
        return NULL;
    }

    ptr = LOS_MemboxAlloc(pool_id);
    if (ptr != NULL) {
        LOS_MemboxClr(pool_id, ptr);
    }

    return ptr;
}

osStatus osPoolFree(osPoolId pool_id, void *block)
{
    UINT32 ret;

    if (pool_id == NULL) {
        return osErrorParameter;
    }

    ret = LOS_MemboxFree(pool_id, block);
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osErrorValue;
    }
}

osStatus osPoolDelete(osPoolId pool_id)
{
    LOS_MEMBOX_INFO *memBox = (LOS_MEMBOX_INFO *)pool_id;
    UINT32 ret;

    if (memBox == NULL) {
        return osErrorParameter;
    }
    if (OS_INT_ACTIVE) {
        return osErrorISR;
    }
    if (memBox->uwBlkCnt != 0) {
        return osErrorResource;
    }

    ret = LOS_MemFree(m_aucSysMem0, memBox);
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osErrorValue;
    }
}


// Message Queue

osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id) {
  (void)thread_id;

  if (queue_def == NULL) {
    return (osMessageQId)NULL;
  }
  return ((osMessageQId)(osMessageQueueNew(queue_def->queue_sz, sizeof(uint32_t), &queue_def->attr)));
}

osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec) {
  return osMessageQueuePut((osMessageQueueId_t)queue_id, &info, 0U, millisec);
}

osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec) {
  osStatus_t status;
  osEvent    event;
  uint32_t   message;

  status = osMessageQueueGet((osMessageQueueId_t)queue_id, &message, NULL, millisec);
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



// Mail Queue
static inline osStatus MappingQueueWriteRet(UINT32 ret)
{
    switch (ret) {
        case LOS_OK:
            return osOK;
        case LOS_ERRNO_QUEUE_TIMEOUT:
        case LOS_ERRNO_QUEUE_ISFULL:
            return osErrorTimeoutResource;
        case LOS_ERRNO_QUEUE_INVALID:
        case LOS_ERRNO_QUEUE_NOT_CREATE:
        case LOS_ERRNO_QUEUE_WRITE_PTR_NULL:
        case LOS_ERRNO_QUEUE_WRITESIZE_ISZERO:
        case LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG:
            return osErrorParameter;
        default:
            return osErrorResource;
    }
}

static inline osStatus MappingQueueReadRet(UINT32 ret)
{
    switch (ret) {
        case LOS_OK:
            return osEventMessage;
        case LOS_ERRNO_QUEUE_TIMEOUT:
        case LOS_ERRNO_QUEUE_ISEMPTY:
            return osEventTimeout;
        case LOS_ERRNO_QUEUE_INVALID:
        case LOS_ERRNO_QUEUE_NOT_CREATE:
        case LOS_ERRNO_QUEUE_READ_PTR_NULL:
        case LOS_ERRNO_QUEUE_READ_SIZE_TOO_SMALL:
            return osErrorParameter;
        default:
            return osErrorResource;
    }
}

osMailQId osMailCreate(const osMailQDef_t *queue_def, osThreadId thread_id)
{
    (VOID)thread_id;
    UINT32 ret;
    UINT32 queueId;
    struct osMailQ *mailQ = NULL;

    if ((queue_def == NULL) || (queue_def->pool == NULL) || OS_INT_ACTIVE) {
        return NULL;
    }

    ret = LOS_QueueCreate(NULL, (UINT16)(queue_def->queue_sz), &queueId, 0, sizeof(UINT32));
    if (ret != LOS_OK) {
        return NULL;
    }

    mailQ = (struct osMailQ *)(queue_def->pool);
    mailQ->pool = CreateAndInitMemBox(queue_def->queue_sz, queue_def->item_sz);
    if (mailQ->pool == NULL) {
        LOS_QueueDelete(queueId);
        return NULL;
    }

    mailQ->queue = queueId;
    return (osMailQId)mailQ;
}

void *osMailAlloc(osMailQId queue_id, uint32_t millisec)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    UNUSED(millisec);
    if (mailQ == NULL) {
        return NULL;
    }
    return LOS_MemboxAlloc(mailQ->pool);
}

void *osMailCAlloc(osMailQId queue_id, uint32_t millisec)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    void *mem = NULL;

    mem = osMailAlloc(queue_id, millisec);
    if (mem != NULL) {
        LOS_MemboxClr(mailQ->pool, mem);
    }

    return mem;
}

osStatus osMailFree(osMailQId queue_id, void *mail)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    UINT32 ret;

    if (mailQ == NULL) {
        return osErrorParameter;
    }

    ret = LOS_MemboxFree(mailQ->pool, mail);
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osErrorOS;
    }
}

osStatus osMailPutHead(osMailQId queue_id, void *mail)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    UINT32 ret;

    if (mailQ == NULL) {
        return osErrorParameter;
    }
    if (mail == NULL) {
        return osErrorValue;
    }

    ret = LOS_QueueWriteHead(mailQ->queue, mail, sizeof(UINT32), 0);
    return MappingQueueWriteRet(ret);
}

osStatus osMailPut(osMailQId queue_id, const void *mail)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    UINT32 ret;

    if (mailQ == NULL) {
        return osErrorParameter;
    }
    if (mail == NULL) {
        return osErrorValue;
    }

    ret = LOS_QueueWrite(mailQ->queue, (void *)mail, sizeof(UINT32), 0);
    return MappingQueueWriteRet(ret);
}

osEvent osMailGet(osMailQId queue_id, uint32_t millisec)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    osEvent event = {0};
    osStatus status;
    UINT32 ret;

    if ((mailQ == NULL) || (OS_INT_ACTIVE && (millisec != 0))) {
        event.status = osErrorParameter;
        return event;
    }

    ret = LOS_QueueRead(mailQ->queue, &(event.value.p), sizeof(UINT32), LOS_MS2Tick(millisec));
    status = MappingQueueReadRet(ret);
    event.status = (status == osEventMessage) ? osEventMail : status;
    return event;
}

osStatus osMailClear(osMailQId queue_id)
{
    osEvent evt;
    UINTPTR intSave;
    intSave = LOS_IntLock();
    while (1) {
        evt = osMailGet(queue_id, 0);
        if (evt.status == osEventMail) {
            (VOID)osMailFree(queue_id, evt.value.p);
        } else if (evt.status == osEventTimeout) {
            LOS_IntRestore(intSave);
            return osOK;
        } else {
            LOS_IntRestore(intSave);
            return evt.status;
        }
    }
}

osStatus osMailDelete(osMailQId queue_id)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    osStatus ret1;
    UINT32 ret2;

    if (mailQ == NULL) {
        return osErrorParameter;
    }
    if (OS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret1 = osPoolDelete(mailQ->pool);
    ret2 = LOS_QueueDelete(mailQ->queue);
    if ((ret1 == osOK) && (ret2 == LOS_OK)) {
        return osOK;
    } else {
        return osErrorOS;
    }
}

void osThreadExitPub (void) {
    osThreadExit();
}

int osGetThreadIntId (void) {
#if 1 // compatibility with posix thread or los thread
    LosTaskCB *pstTaskCB = NULL;

    pstTaskCB = OsCurrTaskGet();

    if (pstTaskCB)
        return pstTaskCB->taskID;
    else
        return -1;
#else
    osThreadId thread_id = osThreadGetId();
    if (thread_id)
        return thread_id->tid;
    return -1;
#endif
}

osStatus_t osThreadBindCpu(osThreadId_t thread_id, uint8_t cpu_id)
{
    if (!thread_id)
        return osErrorParameter;

    if (LOS_TaskCpuAffiSet(thread_id->tid, CPUID_TO_AFFI_MASK(cpu_id)) != LOS_OK)
        return osErrorValue;
    else
        return osOK;
}

uint8_t osThreadGetCpu(void)
{
    return (uint8_t)ArchCurrCpuid();
}