/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "message_router.h"
#include "hdf_base.h"
#ifdef USERSPACE_CLIENT_SUPPORT
#include <signal.h>
#include <unistd.h>
#endif
#include "utils/hdf_log.h"
#include "osal/osal_mutex.h"
#include "securec.h"
#include "message_router_inner.h"
#include "message_dispatcher.h"

#ifdef USERSPACE_CLIENT_SUPPORT
#define HDF_LOG_TAG UMsgEngine
#else
#define HDF_LOG_TAG KMsgEngine
#endif

#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif

#if MESSAGE_ENGINE_MAX_DISPATCHER > UINT8_MAX
#error Max MESSAGE_ENGINE_MAX_DISPATCHER is UINT8_MAX
#endif

IMPLEMENT_SHARED_OBJ(MessageNode);
IMPLEMENT_SHARED_OBJ(RemoteService);

typedef struct {
    uint8_t nodeIndex;
    DispatcherId dispatcherId;
    RemoteService *remoteService;
} ServiceInfo;

#define MAX_NODE_COUNT 2

OSAL_DECLARE_MUTEX(g_routerMutex) = {
    .realMutex = NULL
};

static ServiceInfo g_servicesIndex[MESSAGE_ENGINE_MAX_SERVICE] = {0};

static MessageNode *g_messageNodes[MAX_NODE_COUNT] = { 0, 0};

MessageDispatcher *g_dispatchers[MESSAGE_ENGINE_MAX_DISPATCHER] = {0};

static uint8_t g_routerStatus = ME_STATUS_STOPPED;

static void ReleaseRemoteService(RemoteService *service)
{
    if (service == NULL) {
        return;
    }
    if (service->Shutdown != NULL) {
        service->Shutdown(service);
    }
    if (service->Disref != NULL) {
        service->Disref(service);
    }
}

static MessageDispatcher *RefDispatcherInner(const DispatcherId dispatcherId, bool requireLock)
{
    MessageDispatcher *result = NULL;
    if (dispatcherId >= MESSAGE_ENGINE_MAX_DISPATCHER) {
        HDF_LOGE("%s:Input ID is too big.input=%u", __func__, dispatcherId);
        return NULL;
    }

    if (requireLock) {
        HDF_STATUS status = OsalMutexTimedLock(&g_routerMutex, HDF_WAIT_FOREVER);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("Unable to get lock!status=%d", status);
            return NULL;
        }
    }

    do {
        if (g_dispatchers[dispatcherId] == NULL) {
            break;
        }

        if (g_dispatchers[dispatcherId]->Ref == NULL) {
            break;
        }
        result = g_dispatchers[dispatcherId]->Ref(g_dispatchers[dispatcherId]);
    } while (false);

    if (requireLock) {
        HDF_STATUS status = OsalMutexUnlock(&g_routerMutex);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("Unable to unlock!status=%d", status);
        }
    }
    return result;
}

static ErrorCode RegDispatcher(DispatcherId dispatcherId, MessageDispatcher *dispatcher)
{
    HDF_STATUS status;
    ErrorCode errCode;
    if (dispatcherId >= MESSAGE_ENGINE_MAX_DISPATCHER) {
        HDF_LOGE("%s:dispatcher id is too big!id=%u", __func__, dispatcherId);
        return ME_ERROR_PARA_WRONG;
    }

    status = OsalMutexTimedLock(&g_routerMutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!status=%d", status);
        return ME_ERROR_OPER_MUTEX_FAILED;
    }
    errCode = ME_SUCCESS;
    do {
        if (g_routerStatus != ME_STATUS_RUNNING) {
            errCode = ME_ERROR_WRONG_STATUS;
            break;
        }
        if (g_dispatchers[dispatcherId] != NULL) {
            HDF_LOGE("%s:DispatcherId conflict!ID=%u", __func__, dispatcherId);
            errCode = ME_ERROR_DISPATCHERID_CONFLICT;
        } else {
            g_dispatchers[dispatcherId] = dispatcher;
        }
    } while (false);

    status = OsalMutexUnlock(&g_routerMutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to unlock!status=%d", status);
    }
    return errCode;
}

ErrorCode AddDispatcher(DispatcherConfig *config)
{
    ErrorCode errCode;
    MessageDispatcher *dispatcher = NULL;
    if (config == NULL) {
        return ME_ERROR_NULL_PTR;
    }
    errCode = CreateLocalDispatcher(&dispatcher, config);
    if (errCode != ME_SUCCESS) {
        return errCode;
    }
    if (dispatcher == NULL) {
        HDF_LOGE("%s:CreateDispatcher return NULL!", __func__);
        return ME_ERROR_NULL_PTR;
    }
    do {
        if (dispatcher->Start != NULL) {
            errCode = dispatcher->Start(dispatcher);
            if (errCode != ME_SUCCESS) {
                HDF_LOGE("%s:Start dispatcher %u failed!errCode=%d", __func__, config->dispatcherId, errCode);
                break;
            }
        }
        errCode = RegDispatcher(config->dispatcherId, dispatcher);
    } while (false);

    if (errCode != ME_SUCCESS && dispatcher->Shutdown != NULL) {
        dispatcher->Shutdown(dispatcher);
    }
    if (errCode != ME_SUCCESS && dispatcher->Disref != NULL) {
        dispatcher->Disref(dispatcher);
    }
    return errCode;
}

static void NotifyAllNodesServiceDel(const NodeId nodeId, ServiceId serviceId)
{
    uint8_t i;
    for (i = 0; i < MAX_NODE_COUNT; i++) {
        if (i == nodeId) {
            continue;
        }
        if (g_messageNodes[i] != NULL && g_messageNodes[i]->NotifyServiceDel != NULL) {
            ErrorCode subErrCode = g_messageNodes[i]->NotifyServiceDel(g_messageNodes[i], serviceId);
            if (subErrCode != ME_SUCCESS) {
                HDF_LOGE("%s:Rollback service add to node failed!nodeId=%hhu,serviceId=%d,errCode=%d", __func__, i,
                    serviceId, subErrCode);
            }
        }
    }
}

static ErrorCode NotifyAllNodesServiceAdd(const NodeId nodeId, struct ServiceDef *mapper)
{
    uint8_t i;
    uint8_t notifyNodeIndex;
    ErrorCode errCode;
    if (mapper == NULL) {
        return ME_ERROR_NULL_PTR;
    }

    errCode = ME_SUCCESS;
    for (notifyNodeIndex = 0; notifyNodeIndex < MAX_NODE_COUNT; notifyNodeIndex++) {
        if (notifyNodeIndex == nodeId) {
            continue;
        }
        if (g_messageNodes[notifyNodeIndex] != NULL && g_messageNodes[notifyNodeIndex]->NotifyServiceAdd != NULL) {
            errCode = g_messageNodes[notifyNodeIndex]->NotifyServiceAdd(g_messageNodes[notifyNodeIndex], mapper);
            if (errCode != ME_SUCCESS) {
                HDF_LOGE("%s:Notify service add to node failed!nodeId=%d,serviceId=%d,errCode=%d", __func__,
                    notifyNodeIndex, mapper->serviceId, errCode);
                break;
            }
        }
    }

    if (errCode == ME_SUCCESS) {
        return ME_SUCCESS;
    }
    for (i = 0; i < MAX_NODE_COUNT && i < notifyNodeIndex; i++) {
        if (i == nodeId) {
            continue;
        }
        if (g_messageNodes[i] != NULL && g_messageNodes[i]->NotifyServiceDel != NULL) {
            ErrorCode subErrCode = g_messageNodes[i]->NotifyServiceDel(g_messageNodes[i], mapper->serviceId);
            if (subErrCode != ME_SUCCESS) {
                HDF_LOGE("%s:Rollback service add to node failed!nodeId=%d,serviceId=%d,errCode=%d", __func__, i,
                    mapper->serviceId, subErrCode);
            }
        }
    }
    return errCode;
}

static ErrorCode DoRegistService(const NodeId nodeId, const DispatcherId dispatcherId, RemoteService *remoteService)
{
    if (remoteService == NULL) {
        return ME_ERROR_NULL_PTR;
    }

    if (remoteService->serviceId >= MESSAGE_ENGINE_MAX_SERVICE) {
        return ME_ERROR_PARA_WRONG;
    }

    if (g_routerStatus != ME_STATUS_RUNNING) {
        return ME_ERROR_WRONG_STATUS;
    }

    HDF_LOGW("%s:Register service Node:%d;Dispatcher:%u;Service:%u", __func__, nodeId, dispatcherId,
        remoteService->serviceId);

    if (g_servicesIndex[remoteService->serviceId].remoteService != NULL) {
        HDF_LOGE("%s:Router index find conflict serviceId!", __func__);
        return ME_ERROR_SERVICEID_CONFLICT;
    }

    g_servicesIndex[remoteService->serviceId].remoteService = remoteService;
    g_servicesIndex[remoteService->serviceId].nodeIndex = nodeId;
    g_servicesIndex[remoteService->serviceId].dispatcherId = dispatcherId;

    return ME_SUCCESS;
}

static ErrorCode RegistServiceInner(const NodeId nodeId, const DispatcherId dispatcherId, struct ServiceDef *mapper)
{
    MessageNode *node = NULL;
    RemoteService *remoteService = NULL;
    MessageDispatcher *dispatcher = NULL;
    ErrorCode errCode = ME_ERROR_NOT_SUPPORTED;
    if (mapper == NULL || mapper->serviceId >= MESSAGE_ENGINE_MAX_SERVICE) {
        return ME_ERROR_NULL_PTR;
    }

    if (OsalMutexTimedLock(&g_routerMutex, HDF_WAIT_FOREVER) != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!");
        return ME_ERROR_OPER_MUTEX_FAILED;
    }

    node = RefMessageNode(nodeId, false);
    if (node == NULL) {
        OsalMutexUnlock(&g_routerMutex);
        return ME_ERROR_NO_SUCH_NODE;
    }
    do {
        if (node->CreateRemoteService == NULL) {
            HDF_LOGE("%s:Can not reg service to node %d", __func__, nodeId);
            break;
        }
        dispatcher = RefDispatcherInner(dispatcherId, false);

        remoteService = node->CreateRemoteService(node, dispatcher, mapper);
        if (remoteService == NULL) {
            HDF_LOGE("%s:Node create service failed!node=%d", __func__, nodeId);
            break;
        }

        errCode = NotifyAllNodesServiceAdd(nodeId, mapper);
        if (errCode != ME_SUCCESS) {
            HDF_LOGE("%s:NotifyAllNodesServiceAdd failed!err=%d", __func__, errCode);
            break;
        }

        errCode = DoRegistService(nodeId, dispatcherId, remoteService);
        if (errCode != ME_SUCCESS) {
            HDF_LOGE("%s:DoRegistService failed!err=%d.", __func__, errCode);
            NotifyAllNodesServiceDel(nodeId, mapper->serviceId);
            break;
        }
    } while (false);

    OsalMutexUnlock(&g_routerMutex);
    if (dispatcher != NULL && dispatcher->Disref != NULL) {
        dispatcher->Disref(dispatcher);
    }
    if (node != NULL && node->Disref != NULL) {
        node->Disref(node);
    }

    if (errCode != ME_SUCCESS) {
        ReleaseRemoteService(remoteService);
    }

    return errCode;
}

ErrorCode RegistLocalService(const DispatcherId dispatcherId, struct ServiceDef *mapper)
{
    return RegistServiceInner(LOCAL_NODE_INDEX, dispatcherId, mapper);
}

ErrorCode RegistRemoteService(NodeId nodeId, RemoteService *service)
{
    HDF_STATUS status;
    ErrorCode errCode;
    if (service == NULL) {
        return ME_ERROR_NULL_PTR;
    }
    if (service->serviceId >= MESSAGE_ENGINE_MAX_SERVICE) {
        HDF_LOGE("%s:serviceId exceed max value! ServiceId=%d", __func__, service->serviceId);
        return ME_ERROR_PARA_WRONG;
    }
    if (nodeId >= MAX_NODE_COUNT) {
        HDF_LOGE("%s:NodeId exceed max value! NodeId=%d", __func__, nodeId);
        return ME_ERROR_PARA_WRONG;
    }
    status = OsalMutexTimedLock(&g_routerMutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!status=%d", status);
        return ME_ERROR_OPER_MUTEX_FAILED;
    }

    errCode = DoRegistService(nodeId, BAD_DISPATCHER_ID, service);
    if (errCode != ME_SUCCESS) {
        HDF_LOGE("%s:RegService failed! errCode=%d", __func__, errCode);
    }

    status = OsalMutexUnlock(&g_routerMutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to unlock!status=%d", status);
    }
    return errCode;
}
static ErrorCode UnregistServiceInner(const NodeId nodeId, const DispatcherId dispatcherId, const ServiceId serviceId)
{
    RemoteService *service = NULL;
    HDF_STATUS status;
    ErrorCode errCode;
    if (serviceId >= MESSAGE_ENGINE_MAX_SERVICE) {
        return ME_ERROR_PARA_WRONG;
    }

    status = OsalMutexTimedLock(&g_routerMutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!status=%d", status);
        return ME_ERROR_OPER_MUTEX_FAILED;
    }
    errCode = ME_SUCCESS;
    do {
        if (g_servicesIndex[serviceId].nodeIndex != nodeId || g_servicesIndex[serviceId].dispatcherId != dispatcherId) {
            errCode = ME_ERROR_NO_SUCH_SERVICE;
            break;
        }
        if (g_servicesIndex[serviceId].remoteService == NULL) {
            errCode = ME_ERROR_NO_SUCH_SERVICE;
            break;
        }
        service = g_servicesIndex[serviceId].remoteService;
        ReleaseRemoteService(service);
        g_servicesIndex[serviceId].remoteService = NULL;
        g_servicesIndex[serviceId].nodeIndex = NO_SUCH_NODE_INDEX;
        g_servicesIndex[serviceId].dispatcherId = BAD_DISPATCHER_ID;
        NotifyAllNodesServiceDel(nodeId, serviceId);
    } while (false);
    status = OsalMutexUnlock(&g_routerMutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to unlock!status=%d", status);
    }
    return errCode;
}

ErrorCode UnregistLocalService(const DispatcherId dispatcherId, ServiceId serviceId)
{
    return UnregistServiceInner(LOCAL_NODE_INDEX, dispatcherId, serviceId);
}

ErrorCode UnregistRemoteService(NodeId nodeId, ServiceId serviceId)
{
    (void)nodeId;
    return UnregistServiceInner(REMOTE_NODE_INDEX, BAD_DISPATCHER_ID, serviceId);
}
static bool CheckServiceID(ServiceId serviceId, bool allowSync)
{
    if (serviceId >= MESSAGE_ENGINE_MAX_SERVICE) {
        HDF_LOGE("receiverId exceed MaxServiceID.Route failed!");
        return false;
    }
    (void)allowSync;

    if (g_servicesIndex[serviceId].remoteService != NULL) {
        return true;
    }
#ifdef USERSPACE_CLIENT_SUPPORT
    if (!allowSync) {
        return false;
    }
    for (uint8_t i = 0; i < MAX_NODE_COUNT; i++) {
        if (g_messageNodes[i] != NULL && g_messageNodes[i]->SyncService != NULL) {
            (void)g_messageNodes[i]->SyncService(g_messageNodes[i]);
        }
    }
    return CheckServiceID(serviceId, false);
#else
    return false;
#endif
}

RemoteService *RefRemoteService(ServiceId serviceId)
{
    HDF_STATUS status;
    RemoteService *remoteService = NULL;
    RemoteService *service = NULL;
    if (serviceId >= MESSAGE_ENGINE_MAX_SERVICE) {
        return NULL;
    }

    if (!CheckServiceID(serviceId, true)) {
        return NULL;
    }
    status = OsalMutexTimedLock(&g_routerMutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!status=%d", status);
        return NULL;
    }

    do {
        remoteService = g_servicesIndex[serviceId].remoteService;
        if (remoteService != NULL && remoteService->Ref != NULL) {
            service = remoteService->Ref(remoteService);
        }
    } while (false);

    status = OsalMutexUnlock(&g_routerMutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!status=%d", status);
    }
    return service;
}

ErrorCode SendMessage(MessageContext *context)
{
    RemoteService *service = NULL;
    ErrorCode errCode;
    service = RefRemoteService(context->receiverId);
    if (service == NULL) {
        return ME_ERROR_NO_SUCH_SERVICE;
    }
    do {
        if (service->SendMessage == NULL) {
            errCode = ME_ERROR_NOT_SUPPORTED;
            break;
        }
        errCode = service->SendMessage(service, context);
    } while (false);

    if (service->Disref != NULL) {
        service->Disref(service);
    }
    return errCode;
}

#if defined(KERNEL_SERVER_SUPPORT) || defined(USERSPACE_CLIENT_SUPPORT)
#error("define both KERNEL_SERVER_SUPPORT and USERSPACE_CLIENT_SUPPORT is not allowed!")
#endif

#if defined(KERNEL_SERVER_SUPPORT) || defined(USERSPACE_CLIENT_SUPPORT)
static ErrorCode CreateRemoteMessageNode(uint8_t nodesConfig)
{
    ErrorCode errCode = ME_SUCCESS;
    (void)nodesConfig;

#ifdef KERNEL_SERVER_SUPPORT
    if ((nodesConfig & MESSAGE_NODE_REMOTE_KERNEL_SERVER) != 0) {
        HDF_LOGI("Creating kernel server node...");
        errCode = CreateKernelServerNode(&g_messageNodes[REMOTE_NODE_INDEX]);
    }
#endif

#ifdef USERSPACE_CLIENT_SUPPORT
    if ((nodesConfig & MESSAGE_NODE_REMOTE_USERSPACE_CLIENT) != 0) {
        HDF_LOGI("Creating UserspaceClient server node...");
        errCode = CreateUserspaceClientNode(&g_messageNodes[REMOTE_NODE_INDEX]);
    }
#endif
    return errCode;
}
#endif

static ErrorCode InitNodes(void)
{
    ErrorCode errCode = ME_SUCCESS;
    uint8_t i;
    for (i = 0; i < MAX_NODE_COUNT; i++) {
        MessageNode *node = g_messageNodes[i];
        if (node != NULL) {
            if (node->Init != NULL) {
                errCode = node->Init(node);
            }
            if (errCode != ME_SUCCESS) {
                HDF_LOGE("%s: init node failed!id=%d,ret=%d", __func__, i, errCode);
                return errCode;
            }
        }
    }
    return ME_SUCCESS;
}

static void ReleaseNodes(void)
{
    uint8_t i;
    for (i = 0; i < MAX_NODE_COUNT; i++) {
        MessageNode *node = g_messageNodes[i];
        if (node == NULL) {
            continue;
        }
        if (node->Disref != NULL) {
            node->Disref(node);
        }
        g_messageNodes[i] = NULL;
    }
}

static ErrorCode DoStartMessageRouter(uint8_t nodesConfig)
{
    uint8_t i;
    ErrorCode errCode;
    if (g_routerStatus != ME_STATUS_STOPPED) {
        HDF_LOGE("Router have already started!");
        return ME_ERROR_MUTI_INIT_NOT_ALLOWED;
    }

    for (i = 0; i < MESSAGE_ENGINE_MAX_SERVICE; i++) {
        g_servicesIndex[i].remoteService = NULL;
        g_servicesIndex[i].nodeIndex = NO_SUCH_NODE_INDEX;
        g_servicesIndex[i].dispatcherId = BAD_DISPATCHER_ID;
    }
    do {
        HDF_LOGE("%s:Create local node ...", __func__);
        errCode = CreateLocalNode(&g_messageNodes[LOCAL_NODE_INDEX]);
        if (errCode != ME_SUCCESS) {
            HDF_LOGE("%s:Create local node failed!ret=%d", __func__, errCode);
            break;
        }
#if defined(KERNEL_SERVER_SUPPORT) || defined(USERSPACE_CLIENT_SUPPORT)
        HDF_LOGE("%s:Create remote node ...", __func__);
        errCode = CreateRemoteMessageNode(nodesConfig);
        if (errCode != ME_SUCCESS) {
            HDF_LOGE("%s:Create remote node failed!ret=%d", __func__, errCode);
            break;
        }
#else
        (void)nodesConfig;
#endif
        errCode = InitNodes();
    } while (false);

    if (errCode == ME_SUCCESS) {
        g_routerStatus = ME_STATUS_RUNNING;
        return ME_SUCCESS;
    }

    ReleaseNodes();
    return ME_SUCCESS;
}

ErrorCode EnableDefaultDispatcher(void)
{
    ErrorCode errCode;
    DispatcherConfig config = {
        .dispatcherId = DEFAULT_DISPATCHER_ID,
        .queueSize = DEFAULT_DISPATCHER_QUEUE_SIZE,
        .priorityLevelCount = DEFAULT_DISPATCHER_PRIORITY_COUNT
    };
    HDF_LOGI("Register default dispatcher...");
    errCode = AddDispatcher(&config);
    if (errCode != ME_SUCCESS) {
        HDF_LOGE("Register default dispatcher failed!ret=%d", errCode);
    }
    return errCode;
}

ErrorCode StartMessageRouter(uint8_t nodesConfig)
{
    HDF_STATUS status;
    ErrorCode errCode;
    if (g_routerMutex.realMutex == NULL) {
        HDF_STATUS status = OsalMutexInit(&g_routerMutex);
        if (status != HDF_SUCCESS) {
            return ME_ERROR_OPER_MUTEX_FAILED;
        }
    }
    status = OsalMutexTimedLock(&g_routerMutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!status=%d", status);
        return ME_ERROR_OPER_MUTEX_FAILED;
    }
    errCode = DoStartMessageRouter(nodesConfig);
    status = OsalMutexUnlock(&g_routerMutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!status=%d", status);
    }
    return errCode;
}

static ErrorCode DoShutdownMessageRouter(void)
{
    uint8_t i;
    RemoteService *service = NULL;
    if (g_routerStatus == ME_STATUS_STOPPED) {
        return ME_SUCCESS;
    }

    g_routerStatus = ME_STATUS_STOPPING;
    for (i = 0; i < MESSAGE_ENGINE_MAX_SERVICE; i++) {
        if (g_servicesIndex[i].remoteService == NULL) {
            continue;
        }
        service = g_servicesIndex[i].remoteService;
        g_servicesIndex[i].remoteService = NULL;
        g_servicesIndex[i].nodeIndex = NO_SUCH_NODE_INDEX;
        g_servicesIndex[i].dispatcherId = BAD_DISPATCHER_ID;

        ReleaseRemoteService(service);
    }

    for (i = 0; i < MESSAGE_ENGINE_MAX_DISPATCHER; i++) {
        if (g_dispatchers[i] != NULL && g_dispatchers[i]->Shutdown != NULL) {
            g_dispatchers[i]->Shutdown(g_dispatchers[i]);
        }
        if (g_dispatchers[i] != NULL && g_dispatchers[i]->Disref != NULL) {
            g_dispatchers[i]->Disref(g_dispatchers[i]);
        }
        g_dispatchers[i] = NULL;
    }

    ReleaseNodes();
    g_routerStatus = ME_STATUS_STOPPED;
    return ME_SUCCESS;
}

ErrorCode ShutdownMessageRouter()
{
    HDF_STATUS status;
    ErrorCode errCode;
    HDF_LOGW("%s:Shutdown router...", __func__);
    status = OsalMutexTimedLock(&g_routerMutex, HDF_WAIT_FOREVER);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!status=%d", status);
        return ME_ERROR_OPER_MUTEX_FAILED;
    }
    errCode = DoShutdownMessageRouter();
    status = OsalMutexUnlock(&g_routerMutex);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("Unable to get lock!status=%d", status);
    }
    if (errCode == ME_SUCCESS) {
        HDF_LOGW("%s:Router down.", __func__);
    }
    return errCode;
}

MessageNode *RefMessageNode(const NodeId nodeId, bool isRequireLock)
{
    MessageNode *node = NULL;
    HDF_STATUS status;
    if (nodeId >= MAX_NODE_COUNT) {
        HDF_LOGE("Input nodeId >= MAX_NODE_COUNT");
        return NULL;
    }
    if (isRequireLock) {
        status = OsalMutexTimedLock(&g_routerMutex, HDF_WAIT_FOREVER);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("%s:require lock failed!", __func__);
            return NULL;
        }
    }

    if (g_messageNodes[nodeId] != NULL && g_messageNodes[nodeId]->Ref != NULL) {
        node = g_messageNodes[nodeId]->Ref(g_messageNodes[nodeId]);
    }

    if (isRequireLock) {
        status = OsalMutexUnlock(&g_routerMutex);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("%s:Unlock mutex failed!", __func__);
        }
    }
    return node;
}
