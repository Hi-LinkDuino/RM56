/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "los_queue_pri.h"
#include "los_queue_debug_pri.h"
#include "los_task_pri.h"
#include "los_sched_pri.h"
#include "los_spinlock.h"
#include "los_mp.h"
#include "los_percpu_pri.h"
#include "los_hook.h"

#ifdef LOSCFG_BASE_IPC_QUEUE
#if (LOSCFG_BASE_IPC_QUEUE_LIMIT <= 0)
#error "queue maxnum cannot be zero"
#endif /* LOSCFG_BASE_IPC_QUEUE_LIMIT <= 0 */

LITE_OS_SEC_BSS LosQueueCB *g_allQueue = NULL;
LITE_OS_SEC_BSS STATIC LOS_DL_LIST g_freeQueueList;

/*
 * Description : queue initial
 * Return      : LOS_OK on success or error code on failure
 */
LITE_OS_SEC_TEXT_INIT UINT32 OsQueueInit(VOID)
{
    LosQueueCB *queueNode = NULL;
    UINT32 index;
    UINT32 size;

    size = LOSCFG_BASE_IPC_QUEUE_LIMIT * sizeof(LosQueueCB);
    /* system resident memory, don't free */
    g_allQueue = (LosQueueCB *)LOS_MemAlloc(m_aucSysMem0, size);
    if (g_allQueue == NULL) {
        return LOS_ERRNO_QUEUE_NO_MEMORY;
    }
    (VOID)memset_s(g_allQueue, size, 0, size);
    LOS_ListInit(&g_freeQueueList);
    for (index = 0; index < LOSCFG_BASE_IPC_QUEUE_LIMIT; index++) {
        queueNode = ((LosQueueCB *)g_allQueue) + index;
        queueNode->queueID = index;
        LOS_ListTailInsert(&g_freeQueueList, &queueNode->readWriteList[OS_QUEUE_WRITE]);
    }

    if (OsQueueDbgInitHook() != LOS_OK) {
        return LOS_ERRNO_QUEUE_NO_MEMORY;
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_QueueCreate(CHAR *queueName, UINT16 len, UINT32 *queueID,
                                             UINT32 flags, UINT16 maxMsgSize)
{
    LosQueueCB *queueCB = NULL;
    UINT32 intSave;
    LOS_DL_LIST *unusedQueue = NULL;
    UINT8 *queue = NULL;
    UINT16 msgSize;

    (VOID)queueName;
    (VOID)flags;

    if (queueID == NULL) {
        return LOS_ERRNO_QUEUE_CREAT_PTR_NULL;
    }

    if (maxMsgSize > (OS_NULL_SHORT - sizeof(UINT32))) {
        return LOS_ERRNO_QUEUE_SIZE_TOO_BIG;
    }

    if ((len == 0) || (maxMsgSize == 0)) {
        return LOS_ERRNO_QUEUE_PARA_ISZERO;
    }

    msgSize = maxMsgSize + sizeof(UINT32);
    /*
     * Memory allocation is time-consuming, to shorten the time of disable interrupt,
     * move the memory allocation to here.
     */
    queue = (UINT8 *)LOS_MemAlloc(m_aucSysMem1, (UINT32)len * msgSize);
    if (queue == NULL) {
        return LOS_ERRNO_QUEUE_CREATE_NO_MEMORY;
    }

    SCHEDULER_LOCK(intSave);
    if (LOS_ListEmpty(&g_freeQueueList)) {
        SCHEDULER_UNLOCK(intSave);
        OsQueueCheckHook();
        (VOID)LOS_MemFree(m_aucSysMem1, queue);
        return LOS_ERRNO_QUEUE_CB_UNAVAILABLE;
    }

    unusedQueue = LOS_DL_LIST_FIRST(&g_freeQueueList);
    LOS_ListDelete(unusedQueue);
    queueCB = GET_QUEUE_LIST(unusedQueue);
    queueCB->queueLen = len;
    queueCB->queueSize = msgSize;
    queueCB->queueHandle = queue;
    queueCB->queueState = OS_QUEUE_INUSED;
    queueCB->readWriteableCnt[OS_QUEUE_READ] = 0;
    queueCB->readWriteableCnt[OS_QUEUE_WRITE] = len;
    queueCB->queueHead = 0;
    queueCB->queueTail = 0;
    LOS_ListInit(&queueCB->readWriteList[OS_QUEUE_READ]);
    LOS_ListInit(&queueCB->readWriteList[OS_QUEUE_WRITE]);
    LOS_ListInit(&queueCB->memList);

    OsQueueDbgUpdateHook(queueCB->queueID, OsCurrTaskGet()->taskEntry);
    SCHEDULER_UNLOCK(intSave);

    *queueID = queueCB->queueID;
    OsHookCall(LOS_HOOK_TYPE_QUEUE_CREATE, queueCB);
    return LOS_OK;
}

STATIC LITE_OS_SEC_TEXT UINT32 OsQueueReadParameterCheck(UINT32 queueID, const VOID *bufferAddr,
                                                         const UINT32 *bufferSize, UINT32 timeout)
{
    if (GET_QUEUE_INDEX(queueID) >= LOSCFG_BASE_IPC_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_INVALID;
    }
    if ((bufferAddr == NULL) || (bufferSize == NULL)) {
        return LOS_ERRNO_QUEUE_READ_PTR_NULL;
    }

    if ((*bufferSize == 0) || (*bufferSize > (OS_NULL_SHORT - sizeof(UINT32)))) {
        return LOS_ERRNO_QUEUE_READSIZE_IS_INVALID;
    }

    OsQueueDbgTimeUpdateHook(queueID);

    if (timeout != LOS_NO_WAIT) {
        if (OS_INT_ACTIVE) {
            return LOS_ERRNO_QUEUE_READ_IN_INTERRUPT;
        }
    }
    return LOS_OK;
}

STATIC LITE_OS_SEC_TEXT UINT32 OsQueueWriteParameterCheck(UINT32 queueID, const VOID *bufferAddr,
                                                          const UINT32 *bufferSize, UINT32 timeout)
{
    if (GET_QUEUE_INDEX(queueID) >= LOSCFG_BASE_IPC_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_INVALID;
    }

    if (bufferAddr == NULL) {
        return LOS_ERRNO_QUEUE_WRITE_PTR_NULL;
    }

    if (*bufferSize == 0) {
        return LOS_ERRNO_QUEUE_WRITESIZE_ISZERO;
    }

    OsQueueDbgTimeUpdateHook(queueID);

    if (timeout != LOS_NO_WAIT) {
        if (OS_INT_ACTIVE) {
            return LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT;
        }
    }
    return LOS_OK;
}

STATIC VOID OsQueueBufferOperate(LosQueueCB *queueCB, UINT32 operateType, VOID *bufferAddr, UINT32 *bufferSize)
{
    UINT8 *queueNode = NULL;
    UINT32 msgDataSize;
    UINT16 queuePosition;

    /* get the queue position */
    switch (OS_QUEUE_OPERATE_GET(operateType)) {
        case OS_QUEUE_READ_HEAD:
            queuePosition = queueCB->queueHead;
            ((queueCB->queueHead + 1) == queueCB->queueLen) ? (queueCB->queueHead = 0) : (queueCB->queueHead++);
            break;
        case OS_QUEUE_WRITE_HEAD:
            (queueCB->queueHead == 0) ? (queueCB->queueHead = queueCB->queueLen - 1) : (--queueCB->queueHead);
            queuePosition = queueCB->queueHead;
            break;
        case OS_QUEUE_WRITE_TAIL:
            queuePosition = queueCB->queueTail;
            ((queueCB->queueTail + 1) == queueCB->queueLen) ? (queueCB->queueTail = 0) : (queueCB->queueTail++);
            break;
        default:  /* read tail, reserved. */
            PRINT_ERR("invalid queue operate type!\n");
            return;
    }

    queueNode = &(queueCB->queueHandle[(queuePosition * (queueCB->queueSize))]);

    if (OS_QUEUE_IS_READ(operateType)) {
        if (memcpy_s(&msgDataSize, sizeof(UINT32), queueNode + queueCB->queueSize - sizeof(UINT32),
            sizeof(UINT32)) != EOK) {
            PRINT_ERR("get msgdatasize failed\n");
            return;
        }
        if (memcpy_s(bufferAddr, *bufferSize, queueNode, msgDataSize) != EOK) {
            PRINT_ERR("copy message to buffer failed\n");
            return;
        }

        *bufferSize = msgDataSize;
    } else {
        if (memcpy_s(queueNode, queueCB->queueSize, bufferAddr, *bufferSize) != EOK) {
            PRINT_ERR("store message failed\n");
            return;
        }
        if (memcpy_s(queueNode + queueCB->queueSize - sizeof(UINT32), sizeof(UINT32), bufferSize,
            sizeof(UINT32)) != EOK) {
            PRINT_ERR("store message size failed\n");
            return;
        }
    }
}

STATIC UINT32 OsQueueOperateParamCheck(const LosQueueCB *queueCB, UINT32 queueID,
                                       UINT32 operateType, const UINT32 *bufferSize)
{
    if ((queueCB->queueID != queueID) || (queueCB->queueState == OS_QUEUE_UNUSED)) {
        return LOS_ERRNO_QUEUE_NOT_CREATE;
    }

    if (OS_QUEUE_IS_READ(operateType) && (*bufferSize < (queueCB->queueSize - sizeof(UINT32)))) {
        return LOS_ERRNO_QUEUE_READ_SIZE_TOO_SMALL;
    } else if (OS_QUEUE_IS_WRITE(operateType) && (*bufferSize > (queueCB->queueSize - sizeof(UINT32)))) {
        return LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG;
    }
    return LOS_OK;
}

UINT32 OsQueueOperate(UINT32 queueID, UINT32 operateType, VOID *bufferAddr, UINT32 *bufferSize, UINT32 timeout)
{
    LosQueueCB *queueCB = NULL;
    LosTaskCB *resumedTask = NULL;
    UINT32 ret;
    UINT32 readWrite = OS_QUEUE_READ_WRITE_GET(operateType);
    UINT32 intSave;
    OsHookCall(LOS_HOOK_TYPE_QUEUE_READ, (LosQueueCB *)GET_QUEUE_HANDLE(queueID), operateType, *bufferSize, timeout);

    SCHEDULER_LOCK(intSave);
    queueCB = (LosQueueCB *)GET_QUEUE_HANDLE(queueID);
    ret = OsQueueOperateParamCheck(queueCB, queueID, operateType, bufferSize);
    if (ret != LOS_OK) {
        goto QUEUE_END;
    }

    if (queueCB->readWriteableCnt[readWrite] == 0) {
        if (timeout == LOS_NO_WAIT) {
            ret = OS_QUEUE_IS_READ(operateType) ? LOS_ERRNO_QUEUE_ISEMPTY : LOS_ERRNO_QUEUE_ISFULL;
            goto QUEUE_END;
        }

        if (!OsPreemptableInSched()) {
            ret = LOS_ERRNO_QUEUE_PEND_IN_LOCK;
            goto QUEUE_END;
        }

        OsTaskWaitSetPendMask(OS_TASK_WAIT_QUEUE, queueCB->queueID, timeout);
        ret = OsSchedTaskWait(&queueCB->readWriteList[readWrite], timeout, TRUE);
        if (ret == LOS_ERRNO_TSK_TIMEOUT) {
            ret = LOS_ERRNO_QUEUE_TIMEOUT;
            goto QUEUE_END;
        }
    } else {
        queueCB->readWriteableCnt[readWrite]--;
    }

    OsQueueBufferOperate(queueCB, operateType, bufferAddr, bufferSize);

    if (!LOS_ListEmpty(&queueCB->readWriteList[!readWrite])) {
        resumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&queueCB->readWriteList[!readWrite]));
        OsTaskWakeClearPendMask(resumedTask);
        OsSchedTaskWake(resumedTask);
        SCHEDULER_UNLOCK(intSave);
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
        return LOS_OK;
    } else {
        queueCB->readWriteableCnt[!readWrite]++;
    }

QUEUE_END:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueReadCopy(UINT32 queueID,
                                          VOID *bufferAddr,
                                          UINT32 *bufferSize,
                                          UINT32 timeout)
{
    UINT32 ret;
    UINT32 operateType;

    ret = OsQueueReadParameterCheck(queueID, bufferAddr, bufferSize, timeout);
    if (ret != LOS_OK) {
        return ret;
    }

    operateType = OS_QUEUE_OPERATE_TYPE(OS_QUEUE_READ, OS_QUEUE_HEAD);
    return OsQueueOperate(queueID, operateType, bufferAddr, bufferSize, timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueWriteHeadCopy(UINT32 queueID,
                                               VOID *bufferAddr,
                                               UINT32 bufferSize,
                                               UINT32 timeout)
{
    UINT32 ret;
    UINT32 operateType;

    ret = OsQueueWriteParameterCheck(queueID, bufferAddr, &bufferSize, timeout);
    if (ret != LOS_OK) {
        return ret;
    }

    operateType = OS_QUEUE_OPERATE_TYPE(OS_QUEUE_WRITE, OS_QUEUE_HEAD);
    return OsQueueOperate(queueID, operateType, bufferAddr, &bufferSize, timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueWriteCopy(UINT32 queueID,
                                           VOID *bufferAddr,
                                           UINT32 bufferSize,
                                           UINT32 timeout)
{
    UINT32 ret;
    UINT32 operateType;

    ret = OsQueueWriteParameterCheck(queueID, bufferAddr, &bufferSize, timeout);
    if (ret != LOS_OK) {
        return ret;
    }

    operateType = OS_QUEUE_OPERATE_TYPE(OS_QUEUE_WRITE, OS_QUEUE_TAIL);
    return OsQueueOperate(queueID, operateType, bufferAddr, &bufferSize, timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueRead(UINT32 queueID, VOID *bufferAddr, UINT32 bufferSize, UINT32 timeout)
{
    return LOS_QueueReadCopy(queueID, bufferAddr, &bufferSize, timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueWrite(UINT32 queueID, VOID *bufferAddr, UINT32 bufferSize, UINT32 timeout)
{
    if (bufferAddr == NULL) {
        return LOS_ERRNO_QUEUE_WRITE_PTR_NULL;
    }
    bufferSize = sizeof(CHAR *);
    return LOS_QueueWriteCopy(queueID, &bufferAddr, bufferSize, timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueWriteHead(UINT32 queueID,
                                           VOID *bufferAddr,
                                           UINT32 bufferSize,
                                           UINT32 timeout)
{
    if (bufferAddr == NULL) {
        return LOS_ERRNO_QUEUE_WRITE_PTR_NULL;
    }
    bufferSize = sizeof(CHAR *);
    return LOS_QueueWriteHeadCopy(queueID, &bufferAddr, bufferSize, timeout);
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_QueueDelete(UINT32 queueID)
{
    LosQueueCB *queueCB = NULL;
    UINT8 *queue = NULL;
    UINT32 intSave;
    UINT32 ret;

    if (GET_QUEUE_INDEX(queueID) >= LOSCFG_BASE_IPC_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_NOT_FOUND;
    }

    SCHEDULER_LOCK(intSave);
    queueCB = (LosQueueCB *)GET_QUEUE_HANDLE(queueID);
    if ((queueCB->queueID != queueID) || (queueCB->queueState == OS_QUEUE_UNUSED)) {
        ret = LOS_ERRNO_QUEUE_NOT_CREATE;
        goto QUEUE_END;
    }

    if (!LOS_ListEmpty(&queueCB->readWriteList[OS_QUEUE_READ])) {
        ret = LOS_ERRNO_QUEUE_IN_TSKUSE;
        goto QUEUE_END;
    }

    if (!LOS_ListEmpty(&queueCB->readWriteList[OS_QUEUE_WRITE])) {
        ret = LOS_ERRNO_QUEUE_IN_TSKUSE;
        goto QUEUE_END;
    }

    if (!LOS_ListEmpty(&queueCB->memList)) {
        ret = LOS_ERRNO_QUEUE_IN_TSKUSE;
        goto QUEUE_END;
    }

    if ((queueCB->readWriteableCnt[OS_QUEUE_WRITE] + queueCB->readWriteableCnt[OS_QUEUE_READ]) !=
        queueCB->queueLen) {
        ret = LOS_ERRNO_QUEUE_IN_TSKWRITE;
        goto QUEUE_END;
    }

    queue = queueCB->queueHandle;
    queueCB->queueHandle = NULL;
    queueCB->queueState = OS_QUEUE_UNUSED;
    queueCB->queueID = SET_QUEUE_ID(GET_QUEUE_COUNT(queueCB->queueID) + 1, GET_QUEUE_INDEX(queueCB->queueID));
    OsQueueDbgUpdateHook(queueCB->queueID, NULL);

    LOS_ListTailInsert(&g_freeQueueList, &queueCB->readWriteList[OS_QUEUE_WRITE]);
    SCHEDULER_UNLOCK(intSave);
    OsHookCall(LOS_HOOK_TYPE_QUEUE_DELETE, queueCB);
    ret = LOS_MemFree(m_aucSysMem1, (VOID *)queue);
    return ret;

QUEUE_END:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_QueueInfoGet(UINT32 queueID, QUEUE_INFO_S *queueInfo)
{
    UINT32 intSave;
    UINT32 ret = LOS_OK;
    LosQueueCB *queueCB = NULL;
    LosTaskCB *tskCB = NULL;

    if (queueInfo == NULL) {
        return LOS_ERRNO_QUEUE_PTR_NULL;
    }

    if (GET_QUEUE_INDEX(queueID) >= LOSCFG_BASE_IPC_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_INVALID;
    }

    (VOID)memset_s((VOID *)queueInfo, sizeof(QUEUE_INFO_S), 0, sizeof(QUEUE_INFO_S));
    SCHEDULER_LOCK(intSave);

    queueCB = (LosQueueCB *)GET_QUEUE_HANDLE(queueID);
    if ((queueCB->queueID != queueID) || (queueCB->queueState == OS_QUEUE_UNUSED)) {
        ret = LOS_ERRNO_QUEUE_NOT_CREATE;
        goto QUEUE_END;
    }

    queueInfo->uwQueueID = queueID;
    queueInfo->usQueueLen = queueCB->queueLen;
    queueInfo->usQueueSize = queueCB->queueSize;
    queueInfo->usQueueHead = queueCB->queueHead;
    queueInfo->usQueueTail = queueCB->queueTail;
    queueInfo->usReadableCnt = queueCB->readWriteableCnt[OS_QUEUE_READ];
    queueInfo->usWritableCnt = queueCB->readWriteableCnt[OS_QUEUE_WRITE];

    LOS_DL_LIST_FOR_EACH_ENTRY(tskCB, &queueCB->readWriteList[OS_QUEUE_READ], LosTaskCB, pendList) {
        queueInfo->uwWaitReadTask |= 1ULL << tskCB->taskID;
    }

    LOS_DL_LIST_FOR_EACH_ENTRY(tskCB, &queueCB->readWriteList[OS_QUEUE_WRITE], LosTaskCB, pendList) {
        queueInfo->uwWaitWriteTask |= 1ULL << tskCB->taskID;
    }

    LOS_DL_LIST_FOR_EACH_ENTRY(tskCB, &queueCB->memList, LosTaskCB, pendList) {
        queueInfo->uwWaitMemTask |= 1ULL << tskCB->taskID;
    }

QUEUE_END:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

#endif /* LOSCFG_BASE_IPC_QUEUE */

