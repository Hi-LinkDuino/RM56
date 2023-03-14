/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_vnode_adapter.h"
#include <osal_cdev.h>
#include <osal_mem.h>
#include <osal_sem.h>
#include <osal_uaccess.h>
#include <securec.h>
#include "devsvc_manager_clnt.h"
#include "hdf_device_node_ext.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"

#define HDF_LOG_TAG hdf_vnode
#define VOID_DATA_SIZE 4
#define EVENT_QUEUE_MAX 100
#define MAX_RW_SIZE (1024 * 1204) // 1M

enum HdfVNodeClientStatus {
    VNODE_CLIENT_RUNNING,
    VNODE_CLIENT_LISTENING,
    VNODE_CLIENT_STOPPED,
    VNODE_CLIENT_EXITED,
};

struct HdfVNodeAdapterClient {
    struct HdfVNodeAdapter *adapter;
    struct HdfDeviceIoClient ioServiceClient;
    wait_queue_head_t pollWait;
    struct HdfIoService *serv;
    struct OsalMutex mutex;
    struct DListHead eventQueue;
    struct DListHead listNode;
    int32_t eventQueueSize;
    int32_t wakeup;
    uint32_t status;
};

struct HdfIoServiceKClient {
    struct HdfIoService ioService;
    struct HdfDeviceIoClient client;
};

int HdfKIoServiceDispatch (struct HdfObject *service, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct HdfIoService *ioService = (struct HdfIoService*)service;
    struct HdfIoServiceKClient *kClient = NULL;

    if (ioService == NULL || ioService->dispatcher == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    kClient = CONTAINER_OF(ioService, struct HdfIoServiceKClient, ioService);
    if (kClient->client.device == NULL || kClient->client.device->service == NULL ||
        kClient->client.device->service->Dispatch == NULL) {
            return HDF_ERR_INVALID_OBJECT;
    }

    return kClient->client.device->service->Dispatch(&kClient->client, cmdId, data, reply);
}

static struct HdfIoServiceKClient *HdfHdfIoServiceKClientInstance(struct HdfDeviceObject *deviceObject)
{
    static struct HdfIoDispatcher kDispatcher = {
        .Dispatch = HdfKIoServiceDispatch,
    };

    struct HdfIoServiceKClient *client = OsalMemCalloc(sizeof(struct HdfIoServiceKClient));
    if (client == NULL) {
        return NULL;
    }

    client->client.device = deviceObject;
    if (deviceObject->service != NULL && deviceObject->service->Open != NULL) {
        if (deviceObject->service->Open(&client->client) != HDF_SUCCESS) {
            OsalMemFree(client);
            return NULL;
        }
    }

    client->ioService.dispatcher = &kDispatcher;
    return client;
}

struct HdfIoService *HdfIoServiceAdapterObtain(const char *serviceName)
{
    struct DevSvcManagerClnt *svcMgr = NULL;
    struct HdfDeviceObject *deviceObject = NULL;
    struct HdfIoServiceKClient *kClient = NULL;

    if (serviceName == NULL) {
        return NULL;
    }

    svcMgr = DevSvcManagerClntGetInstance();
    if (svcMgr == NULL || svcMgr->devSvcMgrIf == NULL) {
        return NULL;
    }
    deviceObject = svcMgr->devSvcMgrIf->GetObject(svcMgr->devSvcMgrIf, serviceName);
    if (deviceObject == NULL) {
        return NULL;
    }

    kClient = HdfHdfIoServiceKClientInstance(deviceObject);
    if (kClient == NULL) {
        return NULL;
    }

    return &kClient->ioService;
}

void HdfIoServiceAdapterRecycle(struct HdfIoService *ioService)
{
    struct HdfIoServiceKClient *kClient = NULL;

    if (ioService == NULL) {
        return;
    }

    kClient = CONTAINER_OF(ioService, struct HdfIoServiceKClient, ioService);
    if (kClient->client.device != NULL && kClient->client.device->service != NULL &&
        kClient->client.device->service->Release != NULL) {
        kClient->client.device->service->Release(&kClient->client);
    }
    OsalMemFree(kClient);
}

static struct HdfSBuf *HdfSbufCopyFromUser(uintptr_t data, size_t size)
{
    uint8_t *kData = NULL;
    struct HdfSBuf *sbuf = NULL;

    if (size == 0) {
        return HdfSbufObtain(VOID_DATA_SIZE);
    }

    kData = OsalMemAlloc(size);
    if (kData == NULL) {
        HDF_LOGE("%s:oom", __func__);
        return NULL;
    }
    if (CopyFromUser((void*)kData, (void*)data, size) != 0) {
        HDF_LOGE("%s:failed to copy from user", __func__);
        OsalMemFree(kData);
        return NULL;
    }

    sbuf = HdfSbufBind((uintptr_t)kData, size);
    if (sbuf == NULL) {
        OsalMemFree(kData);
    }
    HdfSbufTransDataOwnership(sbuf);

    return sbuf;
}

static int HdfSbufCopyToUser(const struct HdfSBuf *sbuf, void *dstUser, size_t dstUserSize)
{
    size_t sbufSize = HdfSbufGetDataSize(sbuf);
    if (sbufSize == 0) {
        return HDF_SUCCESS;
    }
    if (dstUserSize < sbufSize) {
        HDF_LOGE("%s: readBuffer too small %zu", __func__, sbufSize);
        return HDF_DEV_ERR_NORANGE;
    }

    if (CopyToUser(dstUser, HdfSbufGetData(sbuf), sbufSize) != 0) {
        HDF_LOGE("%s: failed to copy buff data", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static void DevEventFree(struct HdfDevEvent *event)
{
    if (event == NULL) {
        return;
    }
    if (event->data != NULL) {
        HdfSbufRecycle(event->data);
        event->data = NULL;
    }
    OsalMemFree(event);
}

static int HdfVNodeAdapterServCall(const struct HdfVNodeAdapterClient *client, unsigned long arg)
{
    struct HdfWriteReadBuf bwr;
    struct HdfWriteReadBuf *bwrUser = (struct HdfWriteReadBuf *)((uintptr_t)arg);
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    int ret;

    if (client->serv == NULL || client->adapter == NULL ||
        client->adapter->ioService.dispatcher == NULL ||
        client->adapter->ioService.dispatcher->Dispatch == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (bwrUser == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CopyFromUser(&bwr, (void*)bwrUser, sizeof(bwr)) != 0) {
        HDF_LOGE("copy from user failed");
        return HDF_FAILURE;
    }
    if (bwr.writeSize > MAX_RW_SIZE || bwr.readSize > MAX_RW_SIZE) {
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufCopyFromUser(bwr.writeBuffer, bwr.writeSize);
    if (data == NULL) {
        HDF_LOGE("vnode adapter bind data is null");
        return HDF_FAILURE;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: oom", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    (void)HdfSbufWriteUint64(reply, (uintptr_t)&client->ioServiceClient);
    ret = client->adapter->ioService.dispatcher->Dispatch(client->adapter->ioService.target,
        bwr.cmdCode, data, reply);
    if (bwr.readSize != 0 && HdfSbufCopyToUser(reply, (void*)(uintptr_t)bwr.readBuffer, bwr.readSize) != HDF_SUCCESS) {
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }
    bwr.readConsumed = HdfSbufGetDataSize(reply);
    if (CopyToUser(bwrUser, &bwr, sizeof(struct HdfWriteReadBuf)) != 0) {
        HDF_LOGE("%s: fail to copy bwr", __func__);
        ret = HDF_FAILURE;
    }

    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

static int HdfVNodeAdapterReadDevEvent(struct HdfVNodeAdapterClient *client, unsigned long arg)
{
    struct HdfWriteReadBuf bwr;
    struct HdfWriteReadBuf *bwrUser = (struct HdfWriteReadBuf *)((uintptr_t)arg);
    struct HdfDevEvent *event = NULL;
    size_t eventSize;

    int ret = HDF_SUCCESS;
    if (bwrUser == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (CopyFromUser(&bwr, (void*)bwrUser, sizeof(bwr)) != 0) {
        HDF_LOGE("Copy from user failed");
        return HDF_FAILURE;
    }

    if (bwr.readSize > MAX_RW_SIZE) {
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&client->mutex);

    if (DListIsEmpty(&client->eventQueue)) {
        OsalMutexUnlock(&client->mutex);
        return HDF_DEV_ERR_NODATA;
    }

    event = CONTAINER_OF(client->eventQueue.next, struct HdfDevEvent, listNode);
    eventSize = HdfSbufGetDataSize(event->data);
    if (eventSize > bwr.readSize) {
        bwr.readSize = eventSize;
        ret = HDF_DEV_ERR_NORANGE;
    } else {
        if (HdfSbufCopyToUser(event->data, (void *)(uintptr_t)bwr.readBuffer, bwr.readSize) != HDF_SUCCESS) {
            OsalMutexUnlock(&client->mutex);
            return HDF_ERR_IO;
        }
        bwr.readConsumed = eventSize;
        bwr.cmdCode = (int32_t)event->id;
    }

    if (CopyToUser(bwrUser, &bwr, sizeof(struct HdfWriteReadBuf)) != 0) {
        HDF_LOGE("%s: failed to copy bwr", __func__);
        ret = HDF_ERR_IO;
    }
    if (ret == HDF_SUCCESS) {
        DListRemove(&event->listNode);
        DevEventFree(event);
        client->eventQueueSize--;
    }

    OsalMutexUnlock(&client->mutex);
    return ret;
}

static void HdfVnodeAdapterDropOldEventLocked(struct HdfVNodeAdapterClient *client)
{
    struct HdfDevEvent *dropEvent = CONTAINER_OF(client->eventQueue.next, struct HdfDevEvent, listNode);

    HDF_LOGE("dev event queue full, drop old one");
    DListRemove(&dropEvent->listNode);
    DevEventFree(dropEvent);
    client->eventQueueSize--;
}

static int VNodeAdapterSendDevEventToClient(struct HdfVNodeAdapterClient *vnodeClient,
    uint32_t id, const struct HdfSBuf *data)
{
    struct HdfDevEvent *event = NULL;

    OsalMutexLock(&vnodeClient->mutex);
    if (vnodeClient->status != VNODE_CLIENT_LISTENING) {
        OsalMutexUnlock(&vnodeClient->mutex);
        return HDF_SUCCESS;
    }
    if (vnodeClient->eventQueueSize >= EVENT_QUEUE_MAX) {
        HdfVnodeAdapterDropOldEventLocked(vnodeClient);
    }
    event = OsalMemAlloc(sizeof(struct HdfDevEvent));
    if (event == NULL) {
        OsalMutexUnlock(&vnodeClient->mutex);
        return HDF_DEV_ERR_NO_MEMORY;
    }
    event->id = id;
    event->data = HdfSbufCopy(data);
    if (event->data == NULL) {
        OsalMutexUnlock(&vnodeClient->mutex);
        HDF_LOGE("%s: sbuf oom", __func__);
        OsalMemFree(event);
        return HDF_DEV_ERR_NO_MEMORY;
    }
    DListInsertTail(&event->listNode, &vnodeClient->eventQueue);
    vnodeClient->eventQueueSize++;
    wake_up_interruptible(&vnodeClient->pollWait);
    OsalMutexUnlock(&vnodeClient->mutex);

    return HDF_SUCCESS;
}

static int HdfVNodeAdapterSendDevEvent(struct HdfVNodeAdapter *adapter, struct HdfVNodeAdapterClient *vnodeClient,
    uint32_t id, const struct HdfSBuf *data)
{
    struct HdfVNodeAdapterClient *client = NULL;
    int ret = HDF_FAILURE;

    if (adapter == NULL || data == NULL || HdfSbufGetDataSize(data) == 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&adapter->mutex);
    DLIST_FOR_EACH_ENTRY(client, &adapter->clientList, struct HdfVNodeAdapterClient, listNode) {
        if (vnodeClient != NULL && client != vnodeClient) {
            continue;
        }
        ret = VNodeAdapterSendDevEventToClient(client, id, data);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }
    OsalMutexUnlock(&adapter->mutex);
    return ret;
}

static void HdfVNodeAdapterClientStartListening(struct HdfVNodeAdapterClient *client)
{
    OsalMutexLock(&client->mutex);
    client->status = VNODE_CLIENT_LISTENING;
    OsalMutexUnlock(&client->mutex);
}

static void HdfVnodeCleanEventQueue(struct HdfVNodeAdapterClient *client)
{
    struct HdfDevEvent *event = NULL;
    struct HdfDevEvent *eventTemp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(event, eventTemp, &client->eventQueue, struct HdfDevEvent, listNode) {
        DListRemove(&event->listNode);
        DevEventFree(event);
    }
}

static void HdfVNodeAdapterClientStopListening(struct HdfVNodeAdapterClient *client)
{
    OsalMutexLock(&client->mutex);
    client->status = VNODE_CLIENT_STOPPED;
    HdfVnodeCleanEventQueue(client);
    wake_up_interruptible(&client->pollWait);
    OsalMutexUnlock(&client->mutex);
}

static void HdfVNodeAdapterClientExitListening(struct HdfVNodeAdapterClient *client)
{
    OsalMutexLock(&client->mutex);
    client->status = VNODE_CLIENT_EXITED;
    HdfVnodeCleanEventQueue(client);
    wake_up_interruptible(&client->pollWait);
    OsalMutexUnlock(&client->mutex);
}

static void HdfVNodeAdapterClientWakeup(struct HdfVNodeAdapterClient *client)
{
    OsalMutexLock(&client->mutex);
    if (client->status != VNODE_CLIENT_LISTENING) {
        OsalMutexUnlock(&client->mutex);
        return;
    }
    client->wakeup++;
    wake_up_interruptible(&client->pollWait);
    OsalMutexUnlock(&client->mutex);
}

static long HdfVNodeAdapterIoctl(struct file *filep,  unsigned int cmd, unsigned long arg)
{
    struct HdfVNodeAdapterClient *client = (struct HdfVNodeAdapterClient *)OsalGetFilePriv(filep);
    if (client == NULL) {
        return HDF_DEV_ERR_NO_DEVICE;
    }
    switch (cmd) {
        case HDF_WRITE_READ:
            return HdfVNodeAdapterServCall(client, arg);
        case HDF_READ_DEV_EVENT:
            return HdfVNodeAdapterReadDevEvent(client, arg);
        case HDF_LISTEN_EVENT_START:
            HdfVNodeAdapterClientStartListening(client);
            break;
        case HDF_LISTEN_EVENT_STOP:
            HdfVNodeAdapterClientStopListening(client);
            break;
        case HDF_LISTEN_EVENT_WAKEUP:
            HdfVNodeAdapterClientWakeup(client);
            break;
        case HDF_LISTEN_EVENT_EXIT:
            HdfVNodeAdapterClientExitListening(client);
            break;
        default:
            return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static struct HdfVNodeAdapterClient *HdfNewVNodeAdapterClient(struct HdfVNodeAdapter *adapter)
{
    struct HdfVNodeAdapterClient *client = OsalMemCalloc(sizeof(struct HdfVNodeAdapterClient));
    if (client == NULL) {
        HDF_LOGE("%s: oom", __func__);
        return NULL;
    }
    if (OsalMutexInit(&client->mutex) != HDF_SUCCESS) {
        OsalMemFree(client);
        HDF_LOGE("%s: no mutex", __func__);
        return NULL;
    }

    DListHeadInit(&client->eventQueue);
    client->eventQueueSize = 0;
    client->serv = &adapter->ioService;
    client->status = VNODE_CLIENT_RUNNING;
    client->adapter = adapter;
    client->ioServiceClient.device = (struct HdfDeviceObject *)adapter->ioService.target;
    client->ioServiceClient.priv = NULL;
    client->wakeup = 0;
    init_waitqueue_head(&client->pollWait);
    OsalMutexLock(&adapter->mutex);
    DListInsertTail(&client->listNode, &adapter->clientList);
    OsalMutexUnlock(&adapter->mutex);

    return client;
}

static void HdfDestoryVNodeAdapterClient(struct HdfVNodeAdapterClient *client)
{
    struct HdfDevEvent *event = NULL;
    struct HdfDevEvent *eventTemp = NULL;

    client->status = VNODE_CLIENT_STOPPED;

    OsalMutexLock(&client->adapter->mutex);
    DListRemove(&client->listNode);
    OsalMutexUnlock(&client->adapter->mutex);

    OsalMutexLock(&client->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(event, eventTemp, &client->eventQueue, struct HdfDevEvent, listNode) {
        DListRemove(&event->listNode);
        DevEventFree(event);
    }
    OsalMutexUnlock(&client->mutex);
    OsalMutexDestroy(&client->mutex);
    OsalMemFree(client);
}

int HdfVNodeAdapterOpen(struct OsalCdev *cdev, struct file *filep)
{
    struct HdfVNodeAdapter *adapter = (struct HdfVNodeAdapter *)OsalGetCdevPriv(cdev);
    struct HdfVNodeAdapterClient *client = NULL;
    int32_t ret;

    if (adapter == NULL) {
        HDF_LOGE("Vnode adapter dispatcher is null");
        return HDF_FAILURE;
    }
    client = HdfNewVNodeAdapterClient(adapter);
    if (client == NULL) {
        return ETXTBSY;
    }
    OsalSetFilePriv(filep, client);
    if (client->ioServiceClient.device != NULL && client->ioServiceClient.device->service != NULL &&
        client->ioServiceClient.device->service->Open != NULL) {
        ret = client->ioServiceClient.device->service->Open(&client->ioServiceClient);
        if (ret != HDF_SUCCESS) {
            HdfDestoryVNodeAdapterClient(client);
            return ret;
        }
    }

    return HDF_SUCCESS;
}

static unsigned int HdfVNodeAdapterPoll(struct file *filep, poll_table *wait)
{
    unsigned int mask = 0;
    struct HdfVNodeAdapterClient *client = (struct HdfVNodeAdapterClient *)OsalGetFilePriv(filep);
    if (client == NULL) {
        mask |= POLLERR;
        return mask;
    }
    poll_wait(filep, &client->pollWait, wait);
    OsalMutexLock(&client->mutex);
    if (client->status == VNODE_CLIENT_EXITED) {
        mask |= POLLHUP;
    } else if (!DListIsEmpty(&client->eventQueue)) {
        mask |= POLLIN;
    } else if (client->wakeup > 0) {
        mask |= POLLIN;
        client->wakeup--;
    }
    OsalMutexUnlock(&client->mutex);

    return mask;
}

static int HdfVNodeAdapterClose(struct OsalCdev *cdev, struct file *filep)
{
    struct HdfVNodeAdapterClient *client = NULL;
    (void)cdev;
    client = (struct HdfVNodeAdapterClient *)OsalGetFilePriv(filep);
    if (client->ioServiceClient.device != NULL && client->ioServiceClient.device->service != NULL &&
        client->ioServiceClient.device->service->Release != NULL) {
        client->ioServiceClient.device->service->Release(&client->ioServiceClient);
    }
    HdfDestoryVNodeAdapterClient(client);
    OsalSetFilePriv(filep, NULL);
    return HDF_SUCCESS;
}

struct HdfIoService *HdfIoServiceAdapterRegCdev(struct HdfVNodeAdapter *vnodeAdapter,
    const struct OsalCdevOps *fileOps, uint32_t mode)
{
    int32_t ret;
    DListHeadInit(&vnodeAdapter->clientList);
    if (OsalMutexInit(&vnodeAdapter->mutex) != HDF_SUCCESS) {
        HDF_LOGE("vnode adapter out of mutex");
        goto ERROR;
    }
    vnodeAdapter->cdev = OsalAllocCdev(fileOps);
    if (vnodeAdapter->cdev == NULL) {
        HDF_LOGE("fail to alloc osalcdev");
        OsalMutexDestroy(&vnodeAdapter->mutex);
        goto ERROR;
    }
    ret = OsalRegisterCdev(vnodeAdapter->cdev, vnodeAdapter->vNodePath, mode, vnodeAdapter);
    if (ret != 0) {
        HDF_LOGE("failed to register dev node %s, ret is: %d", vnodeAdapter->vNodePath, ret);
        OsalMutexDestroy(&vnodeAdapter->mutex);
        goto ERROR;
    }
    return &vnodeAdapter->ioService;
ERROR:
    OsalMemFree(vnodeAdapter->vNodePath);
    OsalMemFree(vnodeAdapter);
    return NULL;
}

struct HdfIoService *HdfIoServiceAdapterPublish(const char *serviceName, uint32_t mode)
{
    int nodePathLength;
    struct HdfVNodeAdapter *vnodeAdapter = NULL;
    static const struct OsalCdevOps fileOps = {
        .open = HdfVNodeAdapterOpen,
        .release = HdfVNodeAdapterClose,
        .ioctl = HdfVNodeAdapterIoctl,
        .poll = HdfVNodeAdapterPoll,
    };

    if ((serviceName == NULL) || (mode > MAX_MODE_SIZE)) {
        HDF_LOGE("input param is invalid, mode is %x", mode);
        return NULL;
    }

    vnodeAdapter = (struct HdfVNodeAdapter *)OsalMemCalloc(sizeof(struct HdfVNodeAdapter));
    if (vnodeAdapter == NULL) {
        HDF_LOGE("alloc remote service is null");
        return NULL;
    }

    nodePathLength = strlen(serviceName) + strlen(DEV_NODE_PATH) + 1;
    vnodeAdapter->vNodePath = (char *)OsalMemCalloc(nodePathLength);
    if (vnodeAdapter->vNodePath == NULL) {
        HDF_LOGE("alloc vnode path is null");
        OsalMemFree(vnodeAdapter);
        return NULL;
    }

    if (sprintf_s(vnodeAdapter->vNodePath, nodePathLength, "%s%s", DEV_NODE_PATH, serviceName) < 0) {
        HDF_LOGE("failed to get node path");
        OsalMemFree(vnodeAdapter->vNodePath);
        OsalMemFree(vnodeAdapter);
        return NULL;
    }
    return HdfIoServiceAdapterRegCdev(vnodeAdapter, &fileOps, mode);
}

void HdfIoServiceAdapterRemove(struct HdfIoService *service)
{
    if (service != NULL) {
        struct HdfVNodeAdapter *vnodeAdapter = (struct HdfVNodeAdapter *)service;
        if (vnodeAdapter->vNodePath != NULL) {
            OsalUnregisterCdev(vnodeAdapter->cdev);
            OsalFreeCdev(vnodeAdapter->cdev);
            OsalMemFree(vnodeAdapter->vNodePath);
        }
        OsalMutexDestroy(&vnodeAdapter->mutex);
        OsalMemFree(vnodeAdapter);
    }
}

int32_t HdfDeviceSendEvent(const struct HdfDeviceObject *deviceObject, uint32_t id, const struct HdfSBuf *data)
{
    struct HdfDeviceNode *deviceNode = NULL;
    struct HdfVNodeAdapter *adapter = NULL;

    if (deviceObject == NULL || data == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    deviceNode = CONTAINER_OF(deviceObject, struct HdfDeviceNode, deviceObject);
    if (deviceNode->policy != SERVICE_POLICY_CAPACITY) {
        return HDF_ERR_NOT_SUPPORT;
    }

    adapter = (struct HdfVNodeAdapter *)(((struct DeviceNodeExt*)deviceNode)->ioService);
    return HdfVNodeAdapterSendDevEvent(adapter, NULL, id, data);
}

int32_t HdfDeviceSendEventToClient(const struct HdfDeviceIoClient *client, uint32_t id, const struct HdfSBuf *data)
{
    struct HdfVNodeAdapterClient *vnodeClient = NULL;
    if (client == NULL || client->device == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    vnodeClient = CONTAINER_OF(client, struct HdfVNodeAdapterClient, ioServiceClient);
    if (vnodeClient->adapter == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    return HdfVNodeAdapterSendDevEvent(vnodeClient->adapter, vnodeClient, id, data);
}
