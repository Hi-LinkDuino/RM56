/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "devsvc_listener_holder.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mutex.h"
#include "osal_mem.h"

struct KServStatListenerHolder {
    struct DListHead node;
    struct ServStatListenerHolder holder;
    struct HdfDeviceIoClient *listenerClient;
};

struct SvcStatListenerHolderList {
    struct OsalMutex mutex;
    struct DListHead list;
};

static struct SvcStatListenerHolderList g_holoderList;

static void KServStatListenerHolderListInit(void)
{
    OsalMutexInit(&g_holoderList.mutex);
    DListHeadInit(&g_holoderList.list);
}

void ServStatListenerHolderinit(void)
{
    KServStatListenerHolderListInit();
}

int32_t KServStatListenerHolderNotifyStatus(struct ServStatListenerHolder *holder,
    struct ServiceStatus *status)
{
    if (holder == NULL || status == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct KServStatListenerHolder *holderInst = CONTAINER_OF(holder, struct KServStatListenerHolder, holder);
    if (holderInst->listenerClient == NULL) {
        HDF_LOGE("failed to notify service status, invalid holder");
        return HDF_ERR_INVALID_PARAM;
    }

    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("failed to notify service status, oom");
        return HDF_ERR_MALLOC_FAIL;
    }

    if (ServiceStatusMarshalling(status, data) != HDF_SUCCESS) {
        HDF_LOGE("failed to marshalling service status");
        HdfSbufRecycle(data);
        return HDF_ERR_INVALID_PARAM;
    }

    if (HdfDeviceSendEventToClient(holderInst->listenerClient, 0, data) != HDF_SUCCESS) {
        HDF_LOGE("failed to notify service status, send error");
        return HDF_FAILURE;
    }

    HdfSbufRecycle(data);
    return HDF_SUCCESS;
}

void KServStatListenerHolderRecycle(struct ServStatListenerHolder *holder)
{
    if (holder == NULL) {
        return;
    }

    ServStatListenerHolderRelease(holder);
    HDF_LOGD("KServStatListenerHolderRecycle success");
    return;
}

struct ServStatListenerHolder *ServStatListenerHolderCreate(uintptr_t listener, uint16_t listenClass)
{
    struct HdfDeviceIoClient *client = (struct HdfDeviceIoClient *)listener;
    struct KServStatListenerHolder *holder = NULL;

    if (listener == 0) {
        return NULL;
    }

    holder = OsalMemCalloc(sizeof(struct KServStatListenerHolder));
    if (holder == NULL) {
        return NULL;
    }
    holder->listenerClient = client;
    holder->holder.index = listener;
    holder->holder.listenClass = listenClass;
    holder->holder.NotifyStatus = KServStatListenerHolderNotifyStatus;
    holder->holder.Recycle = KServStatListenerHolderRecycle;

    OsalMutexLock(&g_holoderList.mutex);
    DListInsertTail(&holder->node, &g_holoderList.list);
    OsalMutexUnlock(&g_holoderList.mutex);
    return &holder->holder;
}

struct ServStatListenerHolder *ServStatListenerHolderGet(uint64_t index)
{
    struct KServStatListenerHolder *it = NULL;
    struct KServStatListenerHolder *holder = NULL;

    OsalMutexLock(&g_holoderList.mutex);
    DLIST_FOR_EACH_ENTRY(it, &g_holoderList.list, struct KServStatListenerHolder, node) {
        if (it->holder.index == index) {
            holder = it;
            break;
        }
    }
    OsalMutexUnlock(&g_holoderList.mutex);
    return holder != NULL ? &holder->holder : NULL;
}

void ServStatListenerHolderRelease(struct ServStatListenerHolder *holder)
{
    struct KServStatListenerHolder *holderInst = NULL;
    if (holder == NULL) {
        return;
    }

    holderInst = CONTAINER_OF(holder, struct KServStatListenerHolder, holder);
    OsalMutexLock(&g_holoderList.mutex);
    if (holderInst->node.next != NULL) {
        DListRemove(&holderInst->node);
    }
    OsalMutexUnlock(&g_holoderList.mutex);

    holderInst->listenerClient = NULL;
    OsalMemFree(holderInst);
}

