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

#include "osal_sysevent.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_mutex.h"

#define HDF_LOG_TAG usysevent

#define KEVENT_IOSERVICE_NAME "hdf_kevent"

#define KEVENT_COMPLETE_EVENT 1

struct HdfSysEventNotifier {
    struct OsalMutex mutex;
    struct DListHead notifyNodeList;
    struct DListHead waitList;
    struct HdfDevEventlistener ioServiceListener;
    struct HdfIoService *keventIoService;
};

struct HdfSysEventNotifier *g_hdfSysEventNotifier;

static struct HdfSysEventNotifier *HdfSysEventNotifierGetInstance(void)
{
    if (g_hdfSysEventNotifier != NULL) {
        return g_hdfSysEventNotifier;
    }

    struct HdfSysEventNotifier *notifier = OsalMemCalloc(sizeof(struct HdfSysEventNotifier));
    if (notifier == NULL) {
        return NULL;
    }

    int ret = OsalMutexInit(&notifier->mutex);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(notifier);
        return NULL;
    }

    DListHeadInit(&notifier->notifyNodeList);

    g_hdfSysEventNotifier = notifier;

    return notifier;
}

static int FinishEvent(struct HdfIoService *service, const struct HdfSysEvent *event)
{
    struct HdfSBuf *sbuf = HdfSbufObtain(sizeof(uint64_t));

    if (sbuf == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint64(sbuf, event->syncToken)) {
        HdfSbufRecycle(sbuf);
        return HDF_FAILURE;
    }

    int ret = service->dispatcher->Dispatch(&service->object, KEVENT_COMPLETE_EVENT, sbuf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to finish sysevent, %{public}d", ret);
    }

    HdfSbufRecycle(sbuf);
    return ret;
}

static int OnKEventReceived(
    struct HdfDevEventlistener *listener, struct HdfIoService *service, uint32_t id, struct HdfSBuf *data)
{
    struct HdfSysEventNotifier *notifier = (struct HdfSysEventNotifier *)listener->priv;
    if (notifier == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (id != HDF_SYSEVENT) {
        return HDF_ERR_INVALID_OBJECT;
    }

    struct HdfSysEvent *receivedEvent = NULL;
    uint32_t receivedEventLen = 0;

    if (!HdfSbufReadBuffer(data, (const void **)&receivedEvent, &receivedEventLen) ||
        receivedEventLen != sizeof(struct HdfSysEvent)) {
        HDF_LOGE("failed to read kevent object");
        return HDF_FAILURE;
    }

    receivedEvent->content = HdfSbufReadString(data);

    OsalMutexLock(&notifier->mutex);

    struct HdfSysEventNotifyNode *notifyNode = NULL;
    DLIST_FOR_EACH_ENTRY(notifyNode, &notifier->notifyNodeList, struct HdfSysEventNotifyNode, listNode) {
        if (receivedEvent->eventClass & notifyNode->classFilter) {
            (void)notifyNode->callback(
                notifyNode, receivedEvent->eventClass, receivedEvent->eventid, receivedEvent->content);
        }
    }

    if (receivedEvent->syncToken != 0) {
        (void)FinishEvent(service, receivedEvent);
    }

    OsalMutexUnlock(&notifier->mutex);

    return HDF_SUCCESS;
}

static int InitKeventIoServiceListenerLocked(struct HdfSysEventNotifier *notifier)
{
    if (notifier->keventIoService == NULL) {
        notifier->keventIoService = HdfIoServiceBind(KEVENT_IOSERVICE_NAME);
    }
    if (notifier->keventIoService == NULL) {
        HDF_LOGE(" ioservice %{public}s is invalid", KEVENT_IOSERVICE_NAME);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    notifier->ioServiceListener.onReceive = OnKEventReceived;
    notifier->ioServiceListener.priv = notifier;
    int ret = HdfDeviceRegisterEventListener(notifier->keventIoService, &notifier->ioServiceListener);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE(" ioservice %{public}s is invalid", KEVENT_IOSERVICE_NAME);
        HdfIoServiceRecycle(notifier->keventIoService);
        notifier->keventIoService = NULL;
    }

    return ret;
}

static void DeInitKeventIoServiceListenerLocked(struct HdfSysEventNotifier *notifier)
{
    if (notifier->keventIoService == NULL) {
        return;
    }

    (void)HdfDeviceUnregisterEventListener(notifier->keventIoService, &notifier->ioServiceListener);
    HdfIoServiceRecycle(notifier->keventIoService);
    notifier->keventIoService = NULL;
}

int32_t HdfSysEventNotifyRegister(struct HdfSysEventNotifyNode *notifierNode, uint64_t classSet)
{
    if (notifierNode == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct HdfSysEventNotifier *notifier = HdfSysEventNotifierGetInstance();

    if (notifier == NULL) {
        return HDF_DEV_ERR_NO_MEMORY;
    }

    OsalMutexLock(&notifier->mutex);
    DListInsertTail(&notifierNode->listNode, &notifier->notifyNodeList);
    notifierNode->classFilter = classSet;
    int32_t ret = InitKeventIoServiceListenerLocked(notifier);
    if (ret != HDF_SUCCESS) {
        DListRemove(&notifierNode->listNode);
    }
    OsalMutexUnlock(&notifier->mutex);

    return ret;
}

void HdfSysEventNotifyUnregister(struct HdfSysEventNotifyNode *notifierNode)
{
    if (notifierNode == NULL) {
        return;
    }

    struct HdfSysEventNotifier *notifier = HdfSysEventNotifierGetInstance();

    if (notifier == NULL) {
        return;
    }
    OsalMutexLock(&notifier->mutex);

    DListRemove(&notifierNode->listNode);
    if (DListIsEmpty(&notifier->notifyNodeList)) {
        DeInitKeventIoServiceListenerLocked(notifier);
    }
    OsalMutexUnlock(&notifier->mutex);
}
