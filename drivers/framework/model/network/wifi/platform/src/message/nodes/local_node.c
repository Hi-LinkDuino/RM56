/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "securec.h"
#include "hdf_wlan_priority_queue.h"
#include "utils/hdf_log.h"
#include "message/message_types.h"
#include "../message_router_inner.h"
#include "sidecar.h"

#ifdef USERSPACE_CLIENT_SUPPORT
#define HDF_LOG_TAG UMsgEngine
#else
#define HDF_LOG_TAG KMsgEngine
#endif

typedef struct LocalMessageNode {
    INHERT_MESSAGE_NODE;
} LocalMessageNode;

typedef struct LocalNodeService {
    INHERT_REMOTE_SERVICE;
    MessageDispatcher *dispatcher;
    struct ServiceDef *mapper;
} LocalNodeService;

static void HandleRequestMessage(const RemoteService *service, MessageContext *context)
{
    LocalNodeService *localNodeService = NULL;
    localNodeService = (LocalNodeService *)service;
    struct MessageDef messageDef = { NULL, 0 };

    if (context == NULL || service == NULL) {
        HDF_LOGE("%s:input is NULL", __func__);
        return;
    }
    if (localNodeService->mapper != NULL && context->commandId < localNodeService->mapper->messagesLength) {
        messageDef = localNodeService->mapper->messages[context->commandId];
    }
    if (messageDef.handler == NULL) {
        context->responseStatus = ME_ERROR_NO_SUCH_COMMAND;
    } else {
        context->responseStatus = messageDef.handler((RequestContext *)context, context->reqData, context->rspData);
    }
}

static void HandleResponseMessage(const RemoteService *service, MessageContext *context)
{
    HDF_STATUS status;
    (void)service;
    if (context->requestType < MESSAGE_RSP_START) {
        return;
    }

    if (context->requestType == MESSAGE_TYPE_SYNC_RSP) {
        status = OsalSemPost(&context->rspSemaphore);
        if (status != HDF_SUCCESS) {
            ReleaseMessageContext(context);
        }
    } else if (context->requestType == MESSAGE_TYPE_ASYNC_RSP) {
        if (context->callback != NULL) {
            context->callback((const RequestContext *)context, context->reqData, context->rspData,
                context->responseStatus);
        }
        ReleaseMessageContext(context);
    } else {
        HDF_LOGE("%s:Response type not supported!type=%u", __func__, context->requestType);
    }
}

ErrorCode SendMessageLocalNode(const RemoteService *service, MessageContext *context)
{
    LocalNodeService *localService = NULL;
    uint8_t pri = HIGHEST_PRIORITY;
    if (service == NULL || context == NULL) {
        HDF_LOGE("%s:Input is NULL!", __func__);
        return ME_ERROR_NULL_PTR;
    }

    if (!context->crossNode && context->requestType == MESSAGE_TYPE_SYNC_REQ) {
        HandleRequestMessage(service, context);
        SetToResponse(context);
        return context->responseStatus;
    } else if (context->requestType == MESSAGE_TYPE_SYNC_RSP) {
        (void)OsalSemPost(&context->rspSemaphore);
        return ME_SUCCESS;
    } else {
        localService = (LocalNodeService *)service;
        if (localService->dispatcher == NULL || localService->dispatcher->AppendMessage == NULL) {
            HDF_LOGE("This service has no dispatcher!");
            return ME_ERROR_NOT_SUPPORTED;
        }
        if (context->requestType < MESSAGE_RSP_START) {
            if (localService->mapper == NULL || localService->mapper->messages == NULL) {
                HDF_LOGE("%s:Bad message mapper!", __func__);
                return ME_ERROR_NULL_PTR;
            }

            if (context->commandId >= localService->mapper->messagesLength ||
                localService->mapper->messages[context->commandId].handler == NULL) {
                HDF_LOGE("%s:Request command not found!", __func__);
                return ME_ERROR_NO_SUCH_COMMAND;
            }

            pri = localService->mapper->messages[context->commandId].pri;
        }
        return localService->dispatcher->AppendMessage(localService->dispatcher, pri, context);
    }
}

static void ShutdownLocalService(RemoteService *service)
{
    service->status = ME_STATUS_TODESTROY;
}

static void DestroyLocalNodeRemoteService(RemoteService *service)
{
    LocalNodeService *localService = NULL;
    if (service == NULL) {
        return;
    }
    localService = (LocalNodeService *)service;
    if (localService->dispatcher != NULL && localService->dispatcher->Disref != NULL) {
        localService->dispatcher->Disref(localService->dispatcher);
    }
    localService->mapper = NULL;
    localService->dispatcher = NULL;
    DEINIT_SHARED_OBJ(RemoteService, service);
}

RemoteService *CreateLocalNodeService(MessageNode *node, MessageDispatcher *dispatcher, struct ServiceDef *mapper)
{
    LocalNodeService *service = NULL;
    ErrorCode errCode;
    (void)node;
    if (mapper == NULL) {
        return NULL;
    }
    if (dispatcher == NULL || dispatcher->Ref == NULL) {
        HDF_LOGE("%s:Bad dispatcher found!", __func__);
        return NULL;
    }
    service = (LocalNodeService *)OsalMemCalloc(sizeof(LocalNodeService));
    if (service == NULL) {
        return NULL;
    }
    do {
        service->status = ME_STATUS_RUNNING;
        service->ExecRequestMsg = HandleRequestMessage;
        service->ExecResponseMsg = HandleResponseMessage;
        service->SendMessage = SendMessageLocalNode;
        service->Shutdown = ShutdownLocalService;
        service->serviceId = mapper->serviceId;
        service->mapper = mapper;
        service->dispatcher = dispatcher->Ref(dispatcher);
        if (service->dispatcher == NULL) {
            errCode = ME_ERROR_NO_SUCH_DISPATCHER;
            break;
        }

        errCode = INIT_SHARED_OBJ(RemoteService, (RemoteService *)service, DestroyLocalNodeRemoteService);
        if (errCode != ME_SUCCESS) {
            break;
        }
    } while (false);

    if (errCode != ME_SUCCESS) {
        DestroyLocalNodeRemoteService((RemoteService *)service);
        OsalMemFree(service);
        return NULL;
    }
    return (RemoteService *)service;
}

static ErrorCode InitLocalNode(MessageNode *node)
{
    HDF_STATUS status;
    ErrorCode errCode;
    if (node == NULL) {
        return ME_ERROR_NULL_PTR;
    }

    status = OsalMutexTimedLock(&node->mutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        return ME_ERROR_OPER_MUTEX_FAILED;
    }
    errCode = ME_SUCCESS;
    do {
        if (node->status != ME_STATUS_STOPPED) {
            HDF_LOGE("%s:unexpected status %d", __func__, node->status);
            errCode = ME_ERROR_MUTI_INIT_NOT_ALLOWED;
            break;
        }

        node->status = ME_STATUS_STARTTING;
    } while (false);

    status = OsalMutexUnlock(&node->mutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s:unlock mutex failed!", __func__);
    }

    if (errCode != ME_SUCCESS) {
        return errCode;
    }

    status = OsalMutexTimedLock(&node->mutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s:lock mutex failed!", __func__);
    }
    if (errCode == ME_SUCCESS) {
        node->status = ME_STATUS_RUNNING;
    } else {
        node->status = ME_STATUS_STOPPED;
    }

    status = OsalMutexUnlock(&node->mutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s:unlock mutex failed!", __func__);
    }
    return errCode;
}

static void DestroyLocalNode(MessageNode *node)
{
    int32_t ret;
    if (node == NULL) {
        return;
    }
    ret = OsalMutexDestroy(&node->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:Release mutex failed!ret=%d", __func__, ret);
    }
    DEINIT_SHARED_OBJ(MessageNode, node);
}

ErrorCode CreateLocalNode(MessageNode **node)
{
    int32_t ret;
    LocalMessageNode *newNode = NULL;
    ErrorCode errCode;
    if (node == NULL) {
        return ME_ERROR_NULL_PTR;
    }
    HDF_LOGI("Creating local node...");
    newNode = (LocalMessageNode *)OsalMemCalloc(sizeof(LocalMessageNode));
    if (newNode == NULL) {
        return ME_ERROR_RES_LAKE;
    }
    do {
        newNode->status = ME_STATUS_STOPPED;
        newNode->Init = InitLocalNode;
        newNode->CreateRemoteService = CreateLocalNodeService;
        newNode->SyncService = NULL;
        newNode->NotifyServiceAdd = NULL;
        newNode->NotifyServiceDel = NULL;

        ret = OsalMutexInit(&newNode->mutex);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:Init mutex failed!err=%d", __func__, ret);
            errCode = ME_ERROR_OPER_MUTEX_FAILED;
            break;
        }

        errCode = INIT_SHARED_OBJ(MessageNode, (MessageNode *)newNode, DestroyLocalNode);
        if (errCode != ME_SUCCESS) {
            break;
        }
    } while (false);

    if (errCode != ME_SUCCESS) {
        DestroyLocalNode((MessageNode *)newNode);
        OsalMemFree(newNode);
    } else {
        *node = (MessageNode *)newNode;
    }
    return errCode;
}