/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <errno.h>
#include <limits.h>
#include <osal_thread.h>
#include <osal_time.h>
#include <poll.h>
#include <securec.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "osal_mem.h"

#include "hdf_syscall_adapter.h"

#define HDF_LOG_TAG                 hdf_syscall_adapter
#define EPOLL_MAX_EVENT_SIZE        4
#define HDF_DEFAULT_BWR_READ_SIZE   1024
#define EVENT_READ_BUFF_GROWTH_RATE 2
#define EVENT_READ_BUFF_MAX         (20 * 1024) // 20k
#define SYSCALL_INVALID_FD          (-1)
#define HDF_PFD_GROW_SIZE           4
#define TIMEOUT_US                  100000 // 100ms
#define LOAD_IOSERVICE_WAIT_TIME    10     // ms
#define LOAD_IOSERVICE_WAIT_COUNT   20     // ms

static bool HaveOnlyOneElement(const struct DListHead *head)
{
    if (head->next != head && head->next->next == head) {
        return true;
    }

    return false;
}

static int32_t HdfDevEventGrowReadBuffer(struct HdfWriteReadBuf *buffer)
{
    size_t newSize = buffer->readSize;

    if (newSize > EVENT_READ_BUFF_MAX) {
        HDF_LOGE("%s: report event size out of max limit", __func__);
        return HDF_DEV_ERR_NORANGE;
    }

    void *newBuff = OsalMemAlloc(newSize);
    if (newBuff == NULL) {
        HDF_LOGE("%s:oom,%d", __func__, (int)newSize);
        return HDF_DEV_ERR_NO_MEMORY;
    }

    OsalMemFree((void *)(uintptr_t)buffer->readBuffer);
    buffer->readBuffer = (uintptr_t)newBuff;
    return HDF_SUCCESS;
}

static struct HdfSyscallAdapter *HdfFdToAdapterLocked(const struct HdfDevListenerThread *thread, int32_t fd)
{
    if (thread->adapter != NULL && thread->adapter->fd == fd) {
        return thread->adapter;
    }

    if (thread->adapterListPtr == NULL) {
        return NULL;
    }

    struct HdfSyscallAdapter *adapter = NULL;
    DLIST_FOR_EACH_ENTRY(adapter, thread->adapterListPtr, struct HdfSyscallAdapter, listNode) {
        if (adapter->fd == fd) {
            return adapter;
        }
    }

    return NULL;
}

static int32_t HdfDevEventDispatchLocked(
    const struct HdfDevListenerThread *thread, struct HdfSyscallAdapter *adapter, const struct HdfWriteReadBuf *bwr)
{
    struct HdfDevEventlistener *listener = NULL;
    struct HdfSBuf *sbuf = NULL;

    if (bwr->readConsumed > 0) {
        sbuf = HdfSbufBind(bwr->readBuffer, bwr->readConsumed);
    } else {
        sbuf = HdfSbufObtain(sizeof(int));
    }

    if (sbuf == NULL) {
        HDF_LOGE("%s:sbuf oom", __func__);
        return HDF_DEV_ERR_NO_MEMORY;
    }

    /* Dispatch events to the service group listener */
    if (thread->listenerListPtr != NULL) {
        DLIST_FOR_EACH_ENTRY(listener, thread->listenerListPtr, struct HdfDevEventlistener, listNode) {
            if (listener->onReceive != NULL) {
                (void)listener->onReceive(listener, &adapter->super, bwr->cmdCode, sbuf);
            } else if (listener->callBack != NULL) {
                (void)listener->callBack(listener->priv, bwr->cmdCode, sbuf);
            }
            HdfSbufSetDataSize(sbuf, bwr->readConsumed);
        }
    }

    OsalMutexLock(&adapter->mutex);
    /* Dispatch events to the service (SyscallAdapter) listener */
    DLIST_FOR_EACH_ENTRY(listener, &adapter->listenerList, struct HdfDevEventlistener, listNode) {
        if (listener->onReceive != NULL) {
            (void)listener->onReceive(listener, &adapter->super, bwr->cmdCode, sbuf);
        } else if (listener->callBack != NULL) {
            (void)listener->callBack(listener->priv, bwr->cmdCode, sbuf);
        }
        HdfSbufSetDataSize(sbuf, bwr->readConsumed);
    }
    OsalMutexUnlock(&adapter->mutex);

    HdfSbufRecycle(sbuf);
    return HDF_SUCCESS;
}

static int32_t HdfDevEventReadAndDispatch(struct HdfDevListenerThread *thread, int32_t fd)
{
    struct HdfWriteReadBuf bwr = {0};
    int32_t ret = HDF_SUCCESS;

    bwr.readBuffer = (uintptr_t)OsalMemAlloc(HDF_DEFAULT_BWR_READ_SIZE);
    if (bwr.readBuffer == (uintptr_t)NULL) {
        HDF_LOGE("%s: oom", __func__);
        return HDF_DEV_ERR_NO_MEMORY;
    }
    bwr.cmdCode = -1;
    bwr.readConsumed = 0;
    bwr.readSize = HDF_DEFAULT_BWR_READ_SIZE;

    OsalMutexLock(&thread->mutex);

    struct HdfSyscallAdapter *adapter = HdfFdToAdapterLocked(thread, fd);
    if (adapter == NULL) {
        HDF_LOGI("%s: invalid adapter", __func__);
        OsalMSleep(1); // yield to sync adapter list
        goto FINISH;
    }

    while (true) {
        ret = ioctl(adapter->fd, HDF_READ_DEV_EVENT, &bwr);
        if (ret == 0) {
            break;
        }
        ret = errno;
        if (ret == -HDF_DEV_ERR_NORANGE) {
            if (HdfDevEventGrowReadBuffer(&bwr) == HDF_SUCCESS) {
                /* read buffer may not enough, grow read buffer and try again--The read buffere is insufficient.
                Expand the buffer and try again. */
                continue;
            }
        }
        if (ret == -HDF_DEV_ERR_NODATA) {
            ret = HDF_SUCCESS;
        } else {
            HDF_LOGE("%s:ioctl failed, errno=%d", __func__, ret);
        }

        goto FINISH;
    }

    ret = HdfDevEventDispatchLocked(thread, adapter, &bwr);

FINISH:
    OsalMemFree((void *)(uintptr_t)bwr.readBuffer);
    OsalMutexUnlock(&thread->mutex);
    return ret;
}

static int32_t AssignPfds(struct HdfDevListenerThread *thread, struct pollfd **pfds, uint16_t *pfdSize)
{
    struct pollfd *pfdPtr = *pfds;
    uint16_t pfdCount = 0;

    OsalMutexLock(&thread->mutex);
    if (*pfdSize < thread->pfdSize) {
        pfdPtr = OsalMemAlloc(sizeof(struct pollfd) * thread->pfdSize);
        if (pfdPtr == NULL) {
            HDF_LOGE("%s: oom", __func__);
            OsalMutexUnlock(&thread->mutex);
            return HDF_ERR_MALLOC_FAIL;
        }

        *pfdSize = thread->pfdSize;
        OsalMemFree(*pfds);
        *pfds = pfdPtr;
    }

    for (uint32_t i = 0; i < thread->pfdSize; i++) {
        if (thread->pfds[i].fd != SYSCALL_INVALID_FD) {
            pfdPtr[pfdCount].fd = thread->pfds[i].fd;
            pfdPtr[pfdCount].events = thread->pfds[i].events;
            pfdPtr[pfdCount].revents = 0;
            pfdCount++;
        }
    }

    thread->pollChanged = false;
    OsalMutexUnlock(&thread->mutex);
    return pfdCount;
}

#define POLL_WAIT_TIME_MS 100
static int32_t HdfDevEventListenTask(void *para)
{
    struct HdfDevListenerThread *thread = (struct HdfDevListenerThread *)para;
    struct pollfd *pfds = NULL;
    uint16_t pfdSize = 0;
    int32_t pollCount = 0;

    thread->status = LISTENER_RUNNING;
    while (!thread->shouldStop) {
        if (thread->pollChanged) {
            pollCount = AssignPfds(thread, &pfds, &pfdSize);
        }
        if (pollCount <= 0) {
            goto EXIT;
        }
        int32_t pollSize = poll(pfds, pollCount, -1);
        if (pollSize <= 0) {
            HDF_LOGE("%s: poll fail (%d)%s", __func__, errno, strerror(errno));
            OsalMSleep(POLL_WAIT_TIME_MS);
            continue;
        }
        for (uint32_t i = 0; i < (uint32_t)pollCount; i++) {
            if (pfds[i].fd == SYSCALL_INVALID_FD) {
                continue;
            }
            if ((((uint32_t)pfds[i].revents) & POLLIN) &&
                HdfDevEventReadAndDispatch(thread, pfds[i].fd) != HDF_SUCCESS) {
                goto EXIT;
            } else if (((uint32_t)pfds[i].revents) & POLLHUP) {
                HDF_LOGI("event listener task received exit event");
                goto EXIT;
            } else if (((uint32_t)pfds[i].revents) & POLLNVAL) {
                OsalMSleep(1); // polled closed fd, yield to sync
            }
        }
    }

EXIT:
    HDF_LOGI("event listener task exit");

    thread->status = LISTENER_EXITED;
    OsalMemFree(pfds);

    if (thread->shouldStop) {
        /* Exit due to async call and free the thread struct. */
        OsalMutexDestroy(&thread->mutex);
        OsalThreadDestroy(&thread->thread);
        OsalMemFree(thread->pfds);
        OsalMemFree(thread);
    }

    return HDF_SUCCESS;
}

static int32_t HdfAdapterStartListenIoctl(int fd)
{
    int32_t ret = ioctl(fd, HDF_LISTEN_EVENT_START, 0);
    if (ret) {
        HDF_LOGE("%s: failed to notify drv(%d) of start %d %{public}s", __func__, fd, errno, strerror(errno));
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t HdfAdapterStopListenIoctl(int fd)
{
    int32_t ret = ioctl(fd, HDF_LISTEN_EVENT_STOP, 0);
    if (ret) {
        HDF_LOGE("%s: failed to notify drv(%d) of stop %d %{public}s", __func__, fd, errno, strerror(errno));
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t HdfAdapterExitListenIoctl(int fd)
{
    int32_t ret = ioctl(fd, HDF_LISTEN_EVENT_EXIT, 0);
    if (ret) {
        HDF_LOGE("%s: failed to notify drv(%d) of exit %d %{public}s", __func__, fd, errno, strerror(errno));
        return HDF_ERR_IO;
    }
    HDF_LOGD("ioctl send poll thread(%d) exit event, ret=%d", fd, ret);
    return HDF_SUCCESS;
}

static int32_t HdfDevListenerThreadDoInit(struct HdfDevListenerThread *thread)
{
    if (OsalMutexInit(&thread->mutex) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to create thread lock", __func__);
        return HDF_FAILURE;
    }

    int32_t ret = OsalThreadCreate(&thread->thread, HdfDevEventListenTask, thread);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to create thread", __func__);
        thread->status = LISTENER_UNINITED;
        OsalMutexDestroy(&thread->mutex);
        return HDF_ERR_THREAD_CREATE_FAIL;
    }

    thread->status = LISTENER_INITED;
    thread->shouldStop = false;
    thread->pollChanged = true;

    return HDF_SUCCESS;
}

static int32_t HdfDevListenerThreadInit(struct HdfDevListenerThread *thread)
{
    switch (thread->status) {
        case LISTENER_STARTED: // fall-through
        case LISTENER_RUNNING: // fall-through
        case LISTENER_INITED:  // fall-through
        case LISTENER_WAITING:
            return HDF_SUCCESS;
        case LISTENER_EXITED:
            thread->status = LISTENER_INITED;
            thread->shouldStop = false;
            return HDF_SUCCESS;
        case LISTENER_UNINITED:
            return HdfDevListenerThreadDoInit(thread);
        default:
            break;
    }

    return HDF_FAILURE;
}

static int32_t GetValidPfdIndexLocked(struct HdfDevListenerThread *thread, struct HdfSyscallAdapter *adapter)
{
    uint32_t index = 0;
    struct pollfd *pfds = thread->pfds;
    for (; index < thread->pfdSize; index++) {
        if (pfds[index].fd == SYSCALL_INVALID_FD) {
            break;
        }

        if (pfds[index].fd == adapter->fd) {
            return index;
        }
    }

    if (index >= thread->pfdSize) {
        uint32_t newSize = thread->pfdSize + HDF_PFD_GROW_SIZE;
        struct pollfd *newPfds = OsalMemCalloc(sizeof(struct pollfd) * newSize);
        if (newPfds == NULL) {
            return HDF_ERR_MALLOC_FAIL;
        }
        if (thread->pfdSize != 0) {
            (void)memcpy_s(
                newPfds, sizeof(struct pollfd) * newSize, thread->pfds, sizeof(struct pollfd) * thread->pfdSize);
        }

        for (uint32_t i = index; i < newSize; i++) {
            newPfds[i].fd = SYSCALL_INVALID_FD;
        }

        OsalMemFree(thread->pfds);
        thread->pfds = newPfds;
        thread->pfdSize = newSize;
    }

    return index;
}

static int32_t HdfAddAdapterToPfds(struct HdfDevListenerThread *thread, struct HdfSyscallAdapter *adapter)
{
    int32_t index = GetValidPfdIndexLocked(thread, adapter);
    if (index < 0) {
        return HDF_ERR_MALLOC_FAIL;
    }

    thread->pfds[index].fd = adapter->fd;
    thread->pfds[index].events = POLLIN;
    thread->pfds[index].revents = 0;

    return HDF_SUCCESS;
}

static int32_t HdfListenThreadInitPollFds(struct HdfDevListenerThread *thread)
{
    struct HdfSyscallAdapter *adapter = NULL;
    if (thread->adapterListPtr != NULL) {
        DLIST_FOR_EACH_ENTRY(adapter, thread->adapterListPtr, struct HdfSyscallAdapter, listNode) {
            if (HdfAddAdapterToPfds(thread, adapter) != HDF_SUCCESS) {
                return HDF_ERR_MALLOC_FAIL;
            }
        }
    }

    if (thread->adapter != NULL) {
        return HdfAddAdapterToPfds(thread, thread->adapter);
    }

    return HDF_SUCCESS;
}

static int32_t HdfDevListenerThreadStart(struct HdfDevListenerThread *thread)
{
    if (thread->status >= LISTENER_STARTED) {
        return HDF_SUCCESS;
    }

    if (thread->status != LISTENER_INITED) {
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ret = HdfListenThreadInitPollFds(thread);
    if (ret != HDF_SUCCESS || thread->pfdSize <= 0) {
        HDF_LOGE("%s:invalid poll list", __func__);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    do {
        for (uint16_t i = 0; i < thread->pfdSize; i++) {
            if (thread->pfds[i].fd == SYSCALL_INVALID_FD) {
                continue;
            }
            if (HdfAdapterStartListenIoctl(thread->pfds[i].fd)) {
                return HDF_ERR_IO;
            }
        }

        struct OsalThreadParam config = {
            .name = "hdf_event_listener",
            .priority = OSAL_THREAD_PRI_DEFAULT,
            .stackSize = 0,
        };

        thread->status = LISTENER_STARTED;
        if (OsalThreadStart(&thread->thread, &config) != HDF_SUCCESS) {
            HDF_LOGE("%s:OsalThreadStart failed", __func__);
            ret = HDF_FAILURE;
            break;
        }
        return HDF_SUCCESS;
    } while (0);

    return ret;
}

static struct HdfDevListenerThread *HdfDevListenerThreadObtain(void)
{
    struct HdfDevListenerThread *thread = OsalMemCalloc(sizeof(struct HdfDevListenerThread));
    if (thread == NULL) {
        return NULL;
    }
    thread->status = LISTENER_UNINITED;
    if (HdfDevListenerThreadInit(thread) != HDF_SUCCESS) {
        OsalMemFree(thread);
        return NULL;
    }
    return thread;
}

static int32_t HdfIoServiceGroupThreadInit(struct HdfSyscallAdapterGroup *group)
{
    if (group->thread == NULL) {
        struct HdfDevListenerThread *listenerThread = HdfDevListenerThreadObtain();
        if (listenerThread == NULL) {
            return HDF_ERR_THREAD_CREATE_FAIL;
        }
        group->thread = listenerThread;
    }
    group->thread->adapterListPtr = &group->adapterList;
    group->thread->listenerListPtr = &group->listenerList;
    return HdfDevListenerThreadInit(group->thread);
}

static int32_t HdfIoServiceGroupThreadStart(struct HdfSyscallAdapterGroup *group)
{
    OsalMutexLock(&group->mutex);
    if (HdfIoServiceGroupThreadInit(group) != HDF_SUCCESS) {
        OsalMutexUnlock(&group->mutex);
        return HDF_FAILURE;
    }
    int32_t ret = HdfDevListenerThreadStart(group->thread);
    OsalMutexUnlock(&group->mutex);
    return ret;
}

static int32_t HdfListenThreadPollAdd(struct HdfDevListenerThread *thread, struct HdfSyscallAdapter *adapter)
{
    /* If thread is not bound to a service group, you do not need to add a poll. */
    if (thread->adapterListPtr == NULL) {
        return HDF_SUCCESS;
    }

    OsalMutexLock(&thread->mutex);
    struct HdfSyscallAdapter *headAdapter = DListIsEmpty(thread->adapterListPtr) ?
        NULL :
        DLIST_FIRST_ENTRY(thread->adapterListPtr, struct HdfSyscallAdapter, listNode);

    DListInsertTail(&adapter->listNode, thread->adapterListPtr);

    if (thread->status < LISTENER_STARTED) {
        OsalMutexUnlock(&thread->mutex);
        return HDF_SUCCESS;
    }

    int32_t ret = HDF_SUCCESS;
    do {
        int32_t index = GetValidPfdIndexLocked(thread, adapter);
        if (index < 0) {
            ret = HDF_ERR_MALLOC_FAIL;
            break;
        }

        thread->pfds[index].fd = adapter->fd;
        thread->pfds[index].events = POLLIN;
        thread->pfds[index].revents = 0;

        if (headAdapter != NULL) {
            if (ioctl(headAdapter->fd, HDF_LISTEN_EVENT_WAKEUP, 0) != 0) {
                HDF_LOGE("%s: failed to wakeup drv to add poll %d %{public}s", __func__, errno, strerror(errno));
                thread->pfds[index].fd = SYSCALL_INVALID_FD;
                ret = HDF_ERR_IO;
                break;
            }
        }

        if (HdfAdapterStartListenIoctl(adapter->fd) != HDF_SUCCESS) {
            thread->pfds[index].fd = SYSCALL_INVALID_FD;
            ret = HDF_DEV_ERR_OP;
            break;
        }
        thread->pollChanged = true;
        OsalMutexUnlock(&thread->mutex);
        return ret;
    } while (false);

    DListRemove(&adapter->listNode);
    OsalMutexUnlock(&thread->mutex);
    return ret;
}

static void HdfListenThreadPollDel(struct HdfDevListenerThread *thread, struct HdfSyscallAdapter *adapter)
{
    if (thread == NULL) {
        DListRemove(&adapter->listNode);
        adapter->group = NULL;
        return;
    }
    OsalMutexLock(&thread->mutex);
    struct pollfd *pfds = thread->pfds;
    for (uint32_t index = 0; index < thread->pfdSize; index++) {
        if (pfds[index].fd == adapter->fd) {
            pfds[index].fd = SYSCALL_INVALID_FD;
            break;
        }
    }

    HdfAdapterStopListenIoctl(adapter->fd);
    if (ioctl(adapter->fd, HDF_LISTEN_EVENT_WAKEUP, 0) != 0) {
        HDF_LOGE("%s: failed to wakeup drv to del poll %d %s", __func__, errno, strerror(errno));
    }
    DListRemove(&adapter->listNode);
    adapter->group = NULL;
    thread->pollChanged = true;
    OsalMutexUnlock(&thread->mutex);
}

static void HdfDevListenerThreadFree(struct HdfDevListenerThread *thread)
{
    OsalMutexDestroy(&thread->mutex);
    OsalMemFree(thread->pfds);
    OsalThreadDestroy(&thread->thread);
    OsalMemFree(thread);
}

static void HdfDevListenerThreadDestroy(struct HdfDevListenerThread *thread)
{
    if (thread == NULL) {
        return;
    }

    switch (thread->status) {
        case LISTENER_RUNNING: {
            int count = 0;
            uint32_t stopCount = 0;
            OsalMutexLock(&thread->mutex);
            thread->adapter = NULL;
            thread->adapterListPtr = NULL;
            thread->listenerListPtr = NULL;
            OsalMutexUnlock(&thread->mutex);
            for (uint16_t i = 0; i < thread->pfdSize; i++) {
                if (thread->pfds[i].fd != SYSCALL_INVALID_FD &&
                    HdfAdapterExitListenIoctl(thread->pfds[i].fd) == HDF_SUCCESS) {
                    stopCount++;
                }
                thread->pfds[i].fd = SYSCALL_INVALID_FD;
            }

            if (stopCount == 0) {
                thread->shouldStop = true;
                HDF_LOGE("%s:failed to exit listener thread with ioctl, will go async way", __func__);
                return;
            }
            while (thread->status != LISTENER_EXITED && count <= TIMEOUT_US) {
                OsalUSleep(1);
                count++;
            }
            if (thread->status == LISTENER_EXITED) {
                HDF_LOGI("poll thread exited");
                HdfDevListenerThreadFree(thread);
            } else {
                thread->shouldStop = true;
                HDF_LOGE("wait poll thread exit timeout, async exit");
            }
            return;
        }
        case LISTENER_STARTED:
            thread->shouldStop = true;
            break;
        case LISTENER_EXITED: // fall-through
        case LISTENER_INITED:
            HdfDevListenerThreadFree(thread);
            break;
        default:
            break;
    }
}

static int32_t HdfSyscallAdapterDispatch(
    struct HdfObject *object, int32_t code, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (object == NULL) {
        HDF_LOGE("Input object is null");
        return HDF_FAILURE;
    }
    struct HdfSyscallAdapter *ioService = (struct HdfSyscallAdapter *)object;
    struct HdfWriteReadBuf wrBuf;
    if (reply != NULL) {
        wrBuf.readBuffer = (uintptr_t)HdfSbufGetData(reply);
        wrBuf.readSize = HdfSbufGetCapacity(reply);
    } else {
        wrBuf.readBuffer = 0;
        wrBuf.readSize = 0;
    }
    if (data != NULL) {
        wrBuf.writeBuffer = (uintptr_t)HdfSbufGetData(data);
        wrBuf.writeSize = HdfSbufGetDataSize(data);
    } else {
        wrBuf.writeBuffer = 0;
        wrBuf.writeSize = 0;
    }

    wrBuf.readConsumed = 0;
    wrBuf.writeConsumed = 0;
    wrBuf.cmdCode = code;
    int32_t ret = ioctl(ioService->fd, HDF_WRITE_READ, &wrBuf);
    if (ret < 0) {
        HDF_LOGE("Failed to dispatch serv call ioctl %{public}d", -errno);
        ret = -errno;
    }
    if (reply != NULL) {
        HdfSbufSetDataSize(reply, wrBuf.readConsumed);
    }
    return ret;
}

static int TrytoLoadIoService(const char *serviceName, char *devNodePath, char *realPath)
{
    if (HdfLoadDriverByServiceName(serviceName) != HDF_SUCCESS) {
        HDF_LOGE("%s: load %{public}s driver failed", __func__, serviceName);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    int waitCount = LOAD_IOSERVICE_WAIT_COUNT;
    while (realpath(devNodePath, realPath) == NULL && waitCount-- > 0) {
        OsalMSleep(LOAD_IOSERVICE_WAIT_TIME); // wait ueventd to crater dev
    }
    if (waitCount <= 0) {
        HDF_LOGE("%s: char dev %{public}s is invalid", __func__, devNodePath);
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    return HDF_SUCCESS;
}

struct HdfIoService *HdfIoServiceAdapterObtain(const char *serviceName)
{
    struct HdfSyscallAdapter *adapter = NULL;
    struct HdfIoService *ioService = NULL;
    char *devNodePath = NULL;
    char *realPath = NULL;

    const char *devPath = DEV_NODE_PATH;
    if (access(DEV_NODE_PATH, F_OK) != 0) {
        devPath = DEV_PATH;
    }

    devNodePath = OsalMemCalloc(PATH_MAX);
    realPath = OsalMemCalloc(PATH_MAX);
    if (devNodePath == NULL || realPath == NULL) {
        HDF_LOGE("%s: out of memory", __func__);
        goto OUT;
    }

    if (sprintf_s(devNodePath, PATH_MAX - 1, "%s%s", devPath, serviceName) < 0) {
        HDF_LOGE("Failed to get the node path");
        goto OUT;
    }

    if (realpath(devNodePath, realPath) == NULL &&
        TrytoLoadIoService(serviceName, devNodePath, realPath) != HDF_SUCCESS) {
        goto OUT;
    }

    adapter = (struct HdfSyscallAdapter *)OsalMemCalloc(sizeof(struct HdfSyscallAdapter));
    if (adapter == NULL) {
        HDF_LOGE("Failed to allocate SyscallAdapter");
        goto OUT;
    }

    DListHeadInit(&adapter->listenerList);
    if (OsalMutexInit(&adapter->mutex)) {
        HDF_LOGE("%s: Failed to create mutex", __func__);
        OsalMemFree(adapter);
        goto OUT;
    }

    adapter->fd = open(realPath, O_RDWR);
    if (adapter->fd < 0) {
        HDF_LOGE("Open file node %{public}s failed, (%d)%{public}s", realPath, errno, strerror(errno));
        OsalMutexDestroy(&adapter->mutex);
        OsalMemFree(adapter);
        goto OUT;
    }
    ioService = &adapter->super;
    static struct HdfIoDispatcher dispatch = {
        .Dispatch = HdfSyscallAdapterDispatch,
    };
    ioService->dispatcher = &dispatch;
OUT:
    OsalMemFree(devNodePath);
    OsalMemFree(realPath);
    return ioService;
}

void HdfIoServiceAdapterRecycle(struct HdfIoService *service)
{
    struct HdfSyscallAdapter *adapter = (struct HdfSyscallAdapter *)service;
    if (adapter != NULL) {
        HdfDevListenerThreadDestroy(adapter->thread);
        adapter->thread = NULL;
        if (adapter->fd >= 0) {
            close(adapter->fd);
            adapter->fd = -1;
        }
        OsalMutexDestroy(&adapter->mutex);
        OsalMemFree(adapter);
    }
}

static int32_t HdfIoServiceThreadBindLocked(struct HdfSyscallAdapter *adapter)
{
    if (adapter->thread == NULL) {
        struct HdfDevListenerThread *listenerthread = HdfDevListenerThreadObtain();
        if (listenerthread == NULL) {
            return HDF_ERR_THREAD_CREATE_FAIL;
        }
        adapter->thread = listenerthread;
    }
    adapter->thread->adapter = adapter;
    return HdfDevListenerThreadInit(adapter->thread);
}

static int32_t HdfIoServiceStartListen(struct HdfSyscallAdapter *adapter)
{
    if (HdfIoServiceThreadBindLocked(adapter) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to bind a thread to SyscallAdapter", __func__);
        return HDF_FAILURE;
    }

    return HdfDevListenerThreadStart(adapter->thread);
}

static bool AddListenerToAdapterLocked(struct HdfSyscallAdapter *adapter, struct HdfDevEventlistener *listener)
{
    struct HdfDevEventlistener *it = NULL;
    DLIST_FOR_EACH_ENTRY(it, &adapter->listenerList, struct HdfDevEventlistener, listNode) {
        if (it == listener) {
            HDF_LOGE("Add a listener for duplicate dev-event");
            return false;
        }
    }
    DListInsertTail(&listener->listNode, &adapter->listenerList);
    return true;
}

int32_t HdfDeviceRegisterEventListener(struct HdfIoService *target, struct HdfDevEventlistener *listener)
{
    if (target == NULL || listener == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (listener->callBack == NULL && listener->onReceive == NULL) {
        HDF_LOGE("Listener onReceive func not implemented");
        return HDF_ERR_INVALID_OBJECT;
    }

    struct HdfSyscallAdapter *adapter = CONTAINER_OF(target, struct HdfSyscallAdapter, super);
    int32_t ret = HDF_SUCCESS;

    OsalMutexLock(&adapter->mutex);
    if (!AddListenerToAdapterLocked(adapter, listener)) {
        OsalMutexUnlock(&adapter->mutex);
        return HDF_ERR_INVALID_PARAM;
    }

    if (adapter->group != NULL) {
        /* Do not bind any service in a service goup to its own thread or start the group thread. */
        ret = HdfIoServiceGroupThreadStart(adapter->group);
        OsalMutexUnlock(&adapter->mutex);
        return ret;
    }

    if (HdfIoServiceStartListen(adapter) != HDF_SUCCESS) {
        DListRemove(&listener->listNode);
        ret = HDF_FAILURE;
    }

    OsalMutexUnlock(&adapter->mutex);
    return ret;
}

int32_t HdfDeviceUnregisterEventListener(struct HdfIoService *target, struct HdfDevEventlistener *listener)
{
    if (target == NULL || listener == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (listener->listNode.next == NULL || listener->listNode.prev == NULL) {
        HDF_LOGE("%s: broken listener, may double unregister", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct HdfSyscallAdapter *adapter = (struct HdfSyscallAdapter *)target;
    OsalMutexLock(&adapter->mutex);

    DListRemove(&listener->listNode);

    if (DListIsEmpty(&adapter->listenerList)) {
        HdfDevListenerThreadDestroy(adapter->thread);
        adapter->thread = NULL;
    }
    OsalMutexUnlock(&adapter->mutex);

    return HDF_SUCCESS;
}

struct HdfIoServiceGroup *HdfIoServiceGroupObtain(void)
{
    struct HdfSyscallAdapterGroup *adapterGroup = OsalMemCalloc(sizeof(struct HdfSyscallAdapterGroup));
    if (adapterGroup == NULL) {
        return NULL;
    }

    if (OsalMutexInit(&adapterGroup->mutex)) {
        OsalMemFree(adapterGroup);
        return NULL;
    }
    DListHeadInit(&adapterGroup->adapterList);
    DListHeadInit(&adapterGroup->listenerList);
    return &adapterGroup->serviceGroup;
}

void HdfIoServiceGroupRecycle(struct HdfIoServiceGroup *group)
{
    if (group == NULL) {
        return;
    }

    struct HdfSyscallAdapterGroup *adapterGroup = CONTAINER_OF(group, struct HdfSyscallAdapterGroup, serviceGroup);
    OsalMutexLock(&adapterGroup->mutex);

    HdfDevListenerThreadDestroy(adapterGroup->thread);
    adapterGroup->thread = NULL;

    struct HdfSyscallAdapter *adapter = NULL;
    struct HdfSyscallAdapter *tmp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(adapter, tmp, &adapterGroup->adapterList, struct HdfSyscallAdapter, listNode) {
        DListRemove(&adapter->listNode);
        adapter->group = NULL;
    }

    OsalMutexUnlock(&adapterGroup->mutex);

    OsalMutexDestroy(&adapterGroup->mutex);
    OsalMemFree(adapterGroup);
}

int32_t HdfIoServiceGroupRegisterListener(struct HdfIoServiceGroup *group, struct HdfDevEventlistener *listener)
{
    if (group == NULL || listener == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (listener->callBack == NULL && listener->onReceive == NULL) {
        HDF_LOGE("Listener onReceive func not implemented");
        return HDF_ERR_INVALID_OBJECT;
    }
    struct HdfSyscallAdapterGroup *adapterGroup = CONTAINER_OF(group, struct HdfSyscallAdapterGroup, serviceGroup);

    OsalMutexLock(&adapterGroup->mutex);
    if (HdfIoServiceGroupThreadInit(adapterGroup) != HDF_SUCCESS) {
        HDF_LOGE("%s:failed to bind listener thread for service group", __func__);
        OsalMutexUnlock(&adapterGroup->mutex);
        return HDF_FAILURE;
    }

    int32_t ret = HDF_SUCCESS;
    struct HdfDevListenerThread *listenerThread = adapterGroup->thread;

    OsalMutexLock(&listenerThread->mutex);
    struct HdfDevEventlistener *it = NULL;
    DLIST_FOR_EACH_ENTRY(it, &adapterGroup->listenerList, struct HdfDevEventlistener, listNode) {
        if (it == listener) {
            HDF_LOGE("Failed to add group listener, repeated registration");
            ret = HDF_ERR_INVALID_PARAM;
            goto FINISH;
        }
    }
    DListInsertTail(&listener->listNode, &adapterGroup->listenerList);
    if (!DListIsEmpty(&adapterGroup->adapterList) && listenerThread->status < LISTENER_STARTED) {
        ret = HdfDevListenerThreadStart(listenerThread);
        if (ret != HDF_SUCCESS) {
            DListRemove(&listener->listNode);
        }
    }

FINISH:
    OsalMutexUnlock(&listenerThread->mutex);
    OsalMutexUnlock(&adapterGroup->mutex);
    return ret;
}

static int32_t GetListenerCount(struct HdfDevListenerThread *thread)
{
    struct HdfDevEventlistener *listener = NULL;
    int32_t count = 0;

    OsalMutexLock(&thread->mutex);
    if (thread->listenerListPtr != NULL) {
        DLIST_FOR_EACH_ENTRY(listener, thread->listenerListPtr, struct HdfDevEventlistener, listNode) {
            count++;
        }
    }

    struct HdfSyscallAdapter *adapter = NULL;
    DLIST_FOR_EACH_ENTRY(adapter, thread->adapterListPtr, struct HdfSyscallAdapter, listNode) {
        OsalMutexLock(&adapter->mutex);
        DLIST_FOR_EACH_ENTRY(listener, &adapter->listenerList, struct HdfDevEventlistener, listNode) {
            count++;
        }
        OsalMutexUnlock(&adapter->mutex);
    }
    OsalMutexUnlock(&thread->mutex);

    return count;
}

int32_t HdfIoServiceGroupUnregisterListener(struct HdfIoServiceGroup *group, struct HdfDevEventlistener *listener)
{
    if (group == NULL || listener == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (listener->listNode.next == NULL || listener->listNode.prev == NULL) {
        HDF_LOGE("%s:broken listener, may double unregister", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct HdfSyscallAdapterGroup *adapterGroup = CONTAINER_OF(group, struct HdfSyscallAdapterGroup, serviceGroup);

    OsalMutexLock(&adapterGroup->mutex);
    struct HdfDevListenerThread *listenerThread = adapterGroup->thread;

    DListRemove(&listener->listNode);

    if (listenerThread != NULL && GetListenerCount(listenerThread) == 0) {
        HdfDevListenerThreadDestroy(listenerThread);
        adapterGroup->thread = NULL;
    }
    OsalMutexUnlock(&adapterGroup->mutex);

    return HDF_SUCCESS;
}

int32_t HdfIoServiceGroupAddService(struct HdfIoServiceGroup *group, struct HdfIoService *service)
{
    if (group == NULL || service == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct HdfSyscallAdapter *adapter = CONTAINER_OF(service, struct HdfSyscallAdapter, super);
    struct HdfSyscallAdapterGroup *adapterGroup = CONTAINER_OF(group, struct HdfSyscallAdapterGroup, serviceGroup);

    if (adapter->group != NULL) {
        HDF_LOGE("service already in group");
        return HDF_ERR_DEVICE_BUSY;
    }

    if (adapter->thread != NULL) {
        HDF_LOGE("service already has independent thread");
        return HDF_ERR_DEVICE_BUSY;
    }

    OsalMutexLock(&adapterGroup->mutex);
    if (HdfIoServiceGroupThreadInit(adapterGroup) != HDF_SUCCESS) {
        HDF_LOGE("%s:failed to bind listener thread for service group", __func__);
        OsalMutexUnlock(&adapterGroup->mutex);
        return HDF_FAILURE;
    }

    struct HdfDevListenerThread *listenerThread = adapterGroup->thread;
    int32_t ret = HdfListenThreadPollAdd(listenerThread, adapter);
    if (ret != HDF_SUCCESS) {
        OsalMutexUnlock(&adapterGroup->mutex);
        return ret;
    }

    adapter->group = adapterGroup;

    OsalMutexLock(&listenerThread->mutex);
    if ((!DListIsEmpty(&adapterGroup->listenerList) || !DListIsEmpty(&adapter->listenerList)) &&
        listenerThread->status < LISTENER_STARTED) {
        ret = HdfDevListenerThreadStart(adapterGroup->thread);
        if (ret != HDF_SUCCESS) {
            HdfListenThreadPollDel(adapterGroup->thread, adapter);
        }
    }
    OsalMutexUnlock(&listenerThread->mutex);
    OsalMutexUnlock(&adapterGroup->mutex);
    return ret;
}

void HdfIoServiceGroupRemoveService(struct HdfIoServiceGroup *group, struct HdfIoService *service)
{
    if (group == NULL || service == NULL) {
        return;
    }
    struct HdfSyscallAdapter *adapter = CONTAINER_OF(service, struct HdfSyscallAdapter, super);
    struct HdfSyscallAdapterGroup *adapterGroup = CONTAINER_OF(group, struct HdfSyscallAdapterGroup, serviceGroup);
    if (adapterGroup->thread == NULL || adapter->group == NULL) {
        return;
    }

    OsalMutexLock(&adapterGroup->mutex);
    if (HaveOnlyOneElement(&adapterGroup->adapterList)) {
        HdfDevListenerThreadDestroy(adapterGroup->thread);
        adapterGroup->thread = NULL;
        DListRemove(&adapter->listNode);
        adapter->group = NULL;
    } else {
        HdfListenThreadPollDel(adapterGroup->thread, adapter);
    }
    OsalMutexUnlock(&adapterGroup->mutex);
    adapter->group = NULL;
}

int HdfIoserviceGetListenerCount(const struct HdfIoService *service)
{
    if (service == NULL) {
        return 0;
    }

    struct HdfSyscallAdapter *adapter = CONTAINER_OF(service, struct HdfSyscallAdapter, super);

    OsalMutexLock(&adapter->mutex);
    int count = DlistGetCount(&adapter->listenerList);
    OsalMutexUnlock(&adapter->mutex);
    return count;
}

int HdfIoserviceGroupGetListenerCount(const struct HdfIoServiceGroup *group)
{
    if (group == NULL) {
        return 0;
    }

    struct HdfSyscallAdapterGroup *adapterGroup = CONTAINER_OF(group, struct HdfSyscallAdapterGroup, serviceGroup);
    OsalMutexLock(&adapterGroup->mutex);
    if (adapterGroup->thread == NULL) {
        OsalMutexUnlock(&adapterGroup->mutex);
        return 0;
    }
    int count = GetListenerCount(adapterGroup->thread);
    OsalMutexUnlock(&adapterGroup->mutex);
    return count;
}

int HdfIoserviceGroupGetServiceCount(const struct HdfIoServiceGroup *group)
{
    if (group == NULL) {
        return 0;
    }

    struct HdfSyscallAdapterGroup *adapterGroup = CONTAINER_OF(group, struct HdfSyscallAdapterGroup, serviceGroup);

    OsalMutexLock(&adapterGroup->mutex);
    if (adapterGroup->thread == NULL) {
        OsalMutexUnlock(&adapterGroup->mutex);
        return 0;
    }

    OsalMutexLock(&adapterGroup->thread->mutex);
    int count = DlistGetCount(&adapterGroup->adapterList);
    OsalMutexUnlock(&adapterGroup->thread->mutex);

    OsalMutexUnlock(&adapterGroup->mutex);

    return count;
}
