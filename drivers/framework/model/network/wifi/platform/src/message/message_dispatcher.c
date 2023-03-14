/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "securec.h"
#include "osal/osal_thread.h"
#include "osal/osal_time.h"
#include "osal/osal_mutex.h"
#include "utils/hdf_log.h"
#include "message_dispatcher.h"
#include "hdf_wlan_priority_queue.h"
#include "message_router_inner.h"

#ifdef USERSPACE_CLIENT_SUPPORT
#define HDF_LOG_TAG UMsgEngine
#else
#define HDF_LOG_TAG KMsgEngine
#endif

typedef struct {
    INHERT_MESSAGE_DISPATCHER;
    OSAL_DECLARE_THREAD(dispatcherThread);
} LocalMessageDispatcher;

void ReleaseMessageContext(MessageContext *context)
{
    if (context == NULL) {
        return;
    }
    if (context->rspData != NULL) {
        HdfSbufRecycle(context->rspData);
        context->rspData = NULL;
    }

    if (context->crossNode ||
        (context->requestType != MESSAGE_TYPE_SYNC_REQ && context->requestType != MESSAGE_TYPE_SYNC_RSP)) {
        // Sync request message may use stack mem.Memory is managed by user
        if (context->reqData != NULL) {
            HdfSbufRecycle(context->reqData);
            context->reqData = NULL;
        }
        OsalMemFree(context);
    }
}

void ReleaseMessageMapper(struct ServiceDef *mapper)
{
    if (mapper == NULL) {
        return;
    }
    if (mapper->messages != NULL) {
        OsalMemFree(mapper->messages);
        mapper->messages = NULL;
    }
    OsalMemFree(mapper);
}

struct MessageDef *GetMsgDef(const struct ServiceDef *serviceDef, uint32_t commandId)
{
    struct MessageDef *msgDef = NULL;
    if (serviceDef == NULL || serviceDef->messages == NULL) {
        HDF_LOGE("%s:input is NULL!", __func__);
        return NULL;
    }
    if (commandId >= serviceDef->messagesLength) {
        HDF_LOGE("%s:commandId exceed service def!", __func__);
        return NULL;
    }

    msgDef = serviceDef->messages + commandId;
    if (msgDef->handler == NULL) {
        HDF_LOGE("%s:command has no handler!", __func__);
        return NULL;
    }
    return msgDef;
}

ErrorCode AppendToLocalDispatcher(MessageDispatcher *dispatcher, const uint8_t priority, MessageContext *context)
{
    if (context == NULL) {
        HDF_LOGE("%s:Input context is NULL!", __func__);
        return ME_ERROR_NULL_PTR;
    }
    if (dispatcher == NULL) {
        HDF_LOGE("%s:Input dispatcher is NULL!", __func__);
        return ME_ERROR_NULL_PTR;
    }

    if (dispatcher->messageQueue == NULL) {
        HDF_LOGE("MessageQueue is NULL.");
        return ME_ERROR_NULL_PTR;
    }

    if (dispatcher->status != ME_STATUS_RUNNING) {
        HDF_LOGE("%s:dispatcher is not running", __func__);
        return ME_ERROR_DISPATCHER_NOT_RUNNING;
    }
    return PushPriorityQueue(dispatcher->messageQueue, priority, context);
}

void SetToResponse(MessageContext *context)
{
    ServiceId senderId;
    if (context->requestType != MESSAGE_TYPE_ASYNC_REQ && context->requestType != MESSAGE_TYPE_SYNC_REQ) {
        HDF_LOGE("Only sync and async message can send response!type=%u", context->requestType);
        return;
    }
    senderId = context->senderId;
    context->senderId = context->receiverId;
    context->receiverId = senderId;
    context->requestType = MESSAGE_RSP_START + context->requestType - MESSAGE_REQ_START;
}

static void HandleAsyncResponse(MessageContext *context)
{
    if (context == NULL) {
        return;
    }

    if (context->callback != NULL) {
        context->callback((RequestContext *)context, context->reqData, context->rspData, context->responseStatus);
    }

    ReleaseMessageContext(context);
}

static void HandleSyncResponse(MessageContext *context)
{
    HDF_STATUS status;
    if (context == NULL) {
        HDF_LOGE("Input context is NULL!");
        return;
    }
    status = OsalSemPost(&context->rspSemaphore);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Send semaphore failed!CMD=%u,Sender=%u,Receiver=%u", context->commandId, context->senderId,
            context->receiverId);
    }
    return;
}

static void HandleRequestMessage(MessageContext *context)
{
    RemoteService *targetService = RefRemoteService(context->receiverId);
    ErrorCode errCode = ME_SUCCESS;
    RemoteService *rspService = NULL;
    do {
        if (targetService == NULL) {
            HDF_LOGE("%s:Service %u is not available!", __func__, context->receiverId);
            errCode = ME_ERROR_NULL_PTR;
            break;
        }

        if (targetService->ExecRequestMsg == NULL) {
            HDF_LOGE("%s:Service %u has no ExecMsg method!", __func__, context->receiverId);
            errCode = ME_ERROR_NULL_PTR;
            break;
        }
        targetService->ExecRequestMsg(targetService, context);

        // Convert to response message
        SetToResponse(context);

        if (context->requestType == MESSAGE_TYPE_ASYNC_RSP && context->callback == NULL) {
            ReleaseMessageContext(context);
            break;
        }

        rspService = RefRemoteService(context->receiverId);
        if (rspService == NULL) {
            errCode = ME_ERROR_NO_SUCH_SERVICE;
            break;
        }
        if (rspService->SendMessage == NULL) {
            errCode = ME_ERROR_BAD_SERVICE;
            break;
        }
        errCode = rspService->SendMessage(rspService, context);
    } while (false);

    if (errCode != ME_SUCCESS) {
        if (context->requestType == MESSAGE_TYPE_SYNC_RSP || context->requestType == MESSAGE_TYPE_SYNC_REQ) {
            (void)OsalSemPost(&context->rspSemaphore);
        } else {
            ReleaseMessageContext(context);
        }
    }

    if (targetService != NULL && targetService->Disref != NULL) {
        targetService->Disref(targetService);
        targetService = NULL;
    }

    if (rspService != NULL && rspService->Disref != NULL) {
        rspService->Disref(rspService);
        rspService = NULL;
    }
}

static void HandleMessage(MessageContext *context)
{
    if (context != NULL) {
        switch (context->requestType) {
            case MESSAGE_TYPE_SYNC_REQ:
            case MESSAGE_TYPE_ASYNC_REQ:
                HandleRequestMessage(context);
                break;
            case MESSAGE_TYPE_SYNC_RSP:
                HandleSyncResponse(context);
                break;
            case MESSAGE_TYPE_ASYNC_RSP:
                HandleAsyncResponse(context);
                break;
            default:
                HDF_LOGE("Unsupported message type %u", context->requestType);
        }
    }
}

static void ReleaseAllMessage(MessageDispatcher *dispatcher)
{
    MessageContext *context = NULL;
    do {
        context = PopPriorityQueue(dispatcher->messageQueue, 0);
        ReleaseMessageContext(context);
    } while (context != NULL);
}

static int RunDispatcher(void *para)
{
    MessageDispatcher *dispatcher = NULL;
    MessageContext *context = NULL;
    if (para == NULL) {
        HDF_LOGE("Start dispatcher failed! cause:%s\n", "input para is NULL");
        return ME_ERROR_NULL_PTR;
    }
    dispatcher = (MessageDispatcher *)para;
    if (dispatcher->messageQueue == NULL) {
        HDF_LOGE("Start dispatcher failed! cause:%s\n", "message queue is NULL");
        return ME_ERROR_NULL_PTR;
    }

    if (dispatcher->Ref != NULL) {
        dispatcher = dispatcher->Ref(dispatcher);
    }

    if (dispatcher->status != ME_STATUS_STARTTING) {
        if (dispatcher->Disref != NULL) {
            dispatcher->Disref(dispatcher);
        }
        HDF_LOGE("Start dispatcher failed! cause:%s\n", "dispatcher is not stopped");
        return ME_ERROR_WRONG_STATUS;
    } else {
        dispatcher->status = ME_STATUS_RUNNING;
    }
    while (dispatcher->status == ME_STATUS_RUNNING) {
        context = PopPriorityQueue(dispatcher->messageQueue, QUEUE_OPER_TIMEOUT);
        if (context == NULL) {
            continue;
        }
        HandleMessage(context);
    }

    ReleaseAllMessage(dispatcher);
    dispatcher->status = ME_STATUS_TODESTROY;
    if (dispatcher->Disref != NULL) {
        dispatcher->Disref(dispatcher);
        dispatcher = NULL;
    }

    HDF_LOGW("Dispatcher shutdown!");
    return ME_SUCCESS;
}

static ErrorCode StartDispatcher(MessageDispatcher *dispatcher)
{
    HDF_STATUS status;
    ErrorCode errCode;
    LocalMessageDispatcher *localDispatcher = NULL;
    struct OsalThreadParam config;
    if (dispatcher == NULL) {
        return ME_ERROR_NULL_PTR;
    }

    status = OsalMutexTimedLock(&dispatcher->mutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        return ME_ERROR_OPER_MUTEX_FAILED;
    }

    errCode = ME_SUCCESS;
    do {
        if (dispatcher->status != ME_STATUS_STOPPED) {
            errCode = ME_ERROR_WRONG_STATUS;
            break;
        }
        dispatcher->status = ME_STATUS_STARTTING;
        config.name = "MessageDispatcher";
        config.priority = OSAL_THREAD_PRI_DEFAULT;
        config.stackSize = 0x2000;
        localDispatcher = (LocalMessageDispatcher *)dispatcher;
        status = OsalThreadCreate(&localDispatcher->dispatcherThread, RunDispatcher, localDispatcher);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("%s:OsalThreadCreate failed!status=%d", __func__, status);
            dispatcher->status = ME_STATUS_STOPPED;
            errCode = ME_ERROR_CREATE_THREAD_FAILED;
            break;
        }

        status = OsalThreadStart(&localDispatcher->dispatcherThread, &config);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("%s:OsalThreadStart failed!status=%d", __func__, status);
            dispatcher->status = ME_STATUS_STOPPED;
            OsalThreadDestroy(&localDispatcher->dispatcherThread);
            errCode = ME_ERROR_CREATE_THREAD_FAILED;
            break;
        }
    } while (false);

    status = OsalMutexUnlock(&dispatcher->mutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s:Destroy mutex failed!", __func__);
    }

    if (errCode != ME_SUCCESS) {
        return errCode;
    }

    do {
        OsalMSleep(1);
    } while (dispatcher->status == ME_STATUS_STARTTING);
    return (dispatcher->status == ME_STATUS_RUNNING) ? ME_SUCCESS : ME_ERROR_WRONG_STATUS;
}

static void ShutdownDispatcher(MessageDispatcher *dispatcher)
{
    HDF_STATUS status;
    if (dispatcher == NULL) {
        return;
    }
    status = OsalMutexTimedLock(&dispatcher->mutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Get lock failed!status=%d", status);
        return;
    }

    do {
        if (dispatcher->status != ME_STATUS_RUNNING && dispatcher->status != ME_STATUS_STARTTING) {
            HDF_LOGE("%s:wrong status.status=%d", __func__, dispatcher->status);
            break;
        }
        dispatcher->status = ME_STATUS_STOPPING;
    } while (false);

    status = OsalMutexUnlock(&dispatcher->mutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s:Destroy mutex failed!", __func__);
    }
}

IMPLEMENT_SHARED_OBJ(MessageDispatcher);
static void DestroyLocalDispatcher(MessageDispatcher *dispatcher)
{
    int32_t ret;
    if (dispatcher == NULL) {
        return;
    }

    ReleaseAllMessage(dispatcher);

    if (dispatcher->messageQueue != NULL) {
        DestroyPriorityQueue(dispatcher->messageQueue);
        dispatcher->messageQueue = NULL;
    }

    ret = OsalMutexDestroy(&dispatcher->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:Release mutex failed.ret=%d", __func__, ret);
    }

    DEINIT_SHARED_OBJ(MessageDispatcher, dispatcher);
}

ErrorCode CreateLocalDispatcher(MessageDispatcher **dispatcher, const DispatcherConfig *config)
{
    LocalMessageDispatcher *localDispatcher = NULL;
    int32_t ret;
    ErrorCode errCode;
    if (dispatcher == NULL || config == NULL) {
        return ME_ERROR_NULL_PTR;
    }

    localDispatcher = (LocalMessageDispatcher *)OsalMemCalloc(sizeof(LocalMessageDispatcher));
    if (localDispatcher == NULL) {
        return ME_ERROR_RES_LAKE;
    }
    do {
        localDispatcher->status = ME_STATUS_STOPPED;
        localDispatcher->AppendMessage = AppendToLocalDispatcher;
        localDispatcher->Shutdown = ShutdownDispatcher;
        localDispatcher->Start = StartDispatcher;

        localDispatcher->messageQueue = CreatePriorityQueue(config->queueSize, config->priorityLevelCount);
        if (localDispatcher->messageQueue == NULL) {
            errCode = ME_ERROR_OPER_QUEUE_FAILED;
            break;
        }

        ret = OsalMutexInit(&localDispatcher->mutex);
        if (ret != HDF_SUCCESS) {
            errCode = ME_ERROR_OPER_MUTEX_FAILED;
            break;
        }

        errCode = INIT_SHARED_OBJ(MessageDispatcher, (MessageDispatcher *)localDispatcher, DestroyLocalDispatcher);
        if (errCode != ME_SUCCESS) {
            break;
        }
    } while (false);

    if (errCode == ME_SUCCESS) {
        *dispatcher = (MessageDispatcher *)localDispatcher;
    } else {
        DestroyLocalDispatcher((MessageDispatcher *)localDispatcher);
        OsalMemFree(localDispatcher);
    }
    return errCode;
}
