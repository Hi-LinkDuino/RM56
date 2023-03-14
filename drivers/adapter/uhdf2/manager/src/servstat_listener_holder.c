/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "devsvc_listener_holder.h"
#include "hdf_remote_service.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "hdf_log.h"

#define HDF_LOG_TAG servstat

struct UServStatListenerHolder {
    struct DListHead node;
    struct ServStatListenerHolder holder;
    struct HdfRemoteService *listenerRemote;
};

struct SvcStatListenerHolderList {
    struct OsalMutex mutex;
    struct DListHead list;
    bool inited;
};

static struct SvcStatListenerHolderList g_holoderList = { 0 };

static void UServStatListenerHolderListInit(void)
{
    OsalMutexInit(&g_holoderList.mutex);
    DListHeadInit(&g_holoderList.list);
}

void ServStatListenerHolderinit(void)
{
    UServStatListenerHolderListInit();
}

static void UServStatListenerHolderListListAdd(struct UServStatListenerHolder *holder)
{
    OsalMutexLock(&g_holoderList.mutex);
    DListInsertTail(&holder->node, &g_holoderList.list);
    OsalMutexUnlock(&g_holoderList.mutex);
}

int32_t UServStatListenerHolderNotifyStatus(struct ServStatListenerHolder *holder,
    struct ServiceStatus *status)
{
    if (holder == NULL || status == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct UServStatListenerHolder *holderInst = CONTAINER_OF(holder, struct UServStatListenerHolder, holder);
    if (holderInst->listenerRemote == NULL) {
        HDF_LOGE("failed to notify service status, invalid holder");
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("notify service status %{public}s", status->serviceName);
    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL) {
        HDF_LOGE("failed to notify service status, oom");
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(holderInst->listenerRemote, data)) {
        HDF_LOGE("failed to write interface token");
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    if (ServiceStatusMarshalling(status, data) != HDF_SUCCESS) {
        HDF_LOGE("failed to marshalling service status");
        HdfSbufRecycle(data);
        return HDF_ERR_INVALID_PARAM;
    }

    int ret = holderInst->listenerRemote->dispatcher->DispatchAsync(holderInst->listenerRemote,
        SERVIE_STATUS_LISTENER_NOTIFY, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to notify service status, dispatch error");
        return HDF_FAILURE;
    } else {
        HDF_LOGD("notify service status success");
        return HDF_SUCCESS;
    }
    HdfSbufRecycle(data);
    return HDF_SUCCESS;
}

void UServStatListenerHolderRecycle(struct ServStatListenerHolder *holder)
{
    if (holder == NULL) {
        return;
    }

    ServStatListenerHolderRelease(holder);
    HDF_LOGD("UServStatListenerHolderRecycle success");
    return;
}

struct ServStatListenerHolder *ServStatListenerHolderGet(uint64_t index)
{
    struct UServStatListenerHolder *it = NULL;
    struct UServStatListenerHolder *holder = NULL;

    OsalMutexLock(&g_holoderList.mutex);
    DLIST_FOR_EACH_ENTRY(it, &g_holoderList.list, struct UServStatListenerHolder, node) {
        if (it->holder.index == index) {
            holder = it;
            break;
        }
    }
    OsalMutexUnlock(&g_holoderList.mutex);
    return holder != NULL ? &holder->holder : NULL;
}

struct ServStatListenerHolder *ServStatListenerHolderCreate(uintptr_t listener, uint16_t listenClass)
{
    if (!listener) {
        return NULL;
    }

    struct UServStatListenerHolder *holder = (struct UServStatListenerHolder *)OsalMemCalloc(
        sizeof(struct UServStatListenerHolder));
    if (holder == NULL) {
        return NULL;
    }
    struct HdfRemoteService *listenerRemote = (struct HdfRemoteService *)listener;
    if (!HdfRemoteServiceSetInterfaceDesc(listenerRemote, "HDI.IServiceStatusListener.V1_0")) {
        HDF_LOGE("failed to set interface desc to listener");
        OsalMemFree(holder);
        return NULL;
    }
    holder->holder.listenClass = listenClass;
    holder->holder.NotifyStatus = UServStatListenerHolderNotifyStatus;
    holder->holder.Recycle = UServStatListenerHolderRecycle;
    holder->listenerRemote = listenerRemote;
    holder->holder.index = listenerRemote->index;
    UServStatListenerHolderListListAdd(holder);
    return &holder->holder;
}

void ServStatListenerHolderRelease(struct ServStatListenerHolder *holder)
{
    if (holder == NULL) {
        return;
    }

    struct UServStatListenerHolder *holderInst = CONTAINER_OF(holder, struct UServStatListenerHolder, holder);

    if (holderInst->node.next != NULL) {
        OsalMutexLock(&g_holoderList.mutex);
        DListRemove(&holderInst->node);
        OsalMutexUnlock(&g_holoderList.mutex);
    }

    HdfRemoteServiceRecycle(holderInst->listenerRemote);
    holderInst->listenerRemote = NULL;
    OsalMemFree(holderInst);
}

