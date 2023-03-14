/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "timer_core.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"

#define HDF_LOG_TAG timer_core

struct TimerManager {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct DListHead timerListHead;
    struct OsalMutex lock;
};

static struct TimerManager *g_timerManager = NULL;
#define TIMER_HANDLE_SHIFT    ((uintptr_t)(-1) << 16)

struct TimerCntrl *TimerCntrlOpen(const uint32_t number)
{
    struct TimerCntrl *pos = NULL;
    struct TimerManager *manager = g_timerManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, NULL);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock failed", __func__);
        return NULL;
    }

    DLIST_FOR_EACH_ENTRY(pos, &manager->timerListHead, struct TimerCntrl, node) {
        if (number == pos->info.number) {
            (void)OsalMutexUnlock(&manager->lock);
            return pos;
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGE("%s: open %u failed", __func__, number);
    return NULL;
}

int32_t TimerCntrlClose(struct TimerCntrl *cntrl)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);
    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %u failed", __func__, cntrl->info.number);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->Close != NULL) && (cntrl->ops->Close(cntrl) != HDF_SUCCESS)) {
        HDF_LOGE("%s: close %u failed", __func__, cntrl->info.number);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }
    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlSet(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(cb, HDF_ERR_INVALID_OBJECT);
    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %u failed", __func__, cntrl->info.number);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->Set != NULL) && (cntrl->ops->Set(cntrl, useconds, cb) != HDF_SUCCESS)) {
        HDF_LOGE("%s: set %u failed", __func__, cntrl->info.number);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }

    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlSetOnce(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(cb, HDF_ERR_INVALID_OBJECT);

    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %u failed", __func__, cntrl->info.number);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->SetOnce != NULL) && (cntrl->ops->SetOnce(cntrl, useconds, cb) != HDF_SUCCESS)) {
        HDF_LOGE("%s: setOnce %u failed", __func__, cntrl->info.number);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }

    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlGet(struct TimerCntrl *cntrl, uint32_t *useconds, bool *isPeriod)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(useconds, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(isPeriod, HDF_ERR_INVALID_OBJECT);

    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %u failed", __func__, cntrl->info.number);
        return HDF_ERR_DEVICE_BUSY;
    }
    *useconds = cntrl->info.useconds;
    *isPeriod = cntrl->info.isPeriod;
    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlStart(struct TimerCntrl *cntrl)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);

    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %u failed", __func__, cntrl->info.number);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->Start != NULL) && (cntrl->ops->Start(cntrl) != HDF_SUCCESS)) {
        HDF_LOGE("%s: start %u failed", __func__, cntrl->info.number);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }
    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlStop(struct TimerCntrl *cntrl)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);

    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %u failed", __func__, cntrl->info.number);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->Stop != NULL) && (cntrl->ops->Stop(cntrl) != HDF_SUCCESS)) {
        HDF_LOGE("%s: stop %u failed", __func__, cntrl->info.number);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }

    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

static int32_t TimerIoOpen(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int16_t number;
    uint32_t handle;

    if ((data == NULL) || (reply == NULL)) {
        HDF_LOGE("%s: param invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint16(data, (uint16_t *)&number)) {
        HDF_LOGE("%s: HdfSbufReadUint16 failed", __func__);
        return HDF_ERR_IO;
    }

    if (number < 0) {
        HDF_LOGE("%s: info read failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (TimerCntrlOpen(number) == NULL) {
        HDF_LOGE("%s: TimerCntrlOpen %d failed", __func__, number);
        return HDF_FAILURE;
    }

    handle = (uint32_t)(number + TIMER_HANDLE_SHIFT);
    if (!HdfSbufWriteUint32(reply, handle)) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t TimerIoClose(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t handle;
    int16_t number;
    if (data == NULL) {
        HDF_LOGE("%s: param invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%s: HdfSbufReadUint32 failed", __func__);
        return HDF_ERR_IO;
    }

    number = (int16_t)(handle - TIMER_HANDLE_SHIFT);
    if (number < 0) {
        HDF_LOGE("%s: number[%d] invalid", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }
    return TimerCntrlClose(TimerCntrlOpen(number));
}

static int32_t TimerIoStart(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t handle;
    int16_t number;
    if (data == NULL) {
        HDF_LOGE("%s: param invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%s: HdfSbufReadUint32 failed", __func__);
        return HDF_ERR_IO;
    }

    number = (int16_t)(handle - TIMER_HANDLE_SHIFT);
    if (number < 0) {
        HDF_LOGE("%s: number[%d] invalid", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }
    return TimerCntrlStart(TimerCntrlOpen(number));
}

static int32_t TimerIoStop(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t handle;
    int16_t number;
    if (data == NULL) {
        HDF_LOGE("%s: param invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%s: HdfSbufReadUint32 failed", __func__);
        return HDF_ERR_IO;
    }

    number = (int16_t)(handle - TIMER_HANDLE_SHIFT);
    if (number < 0) {
        HDF_LOGE("%s: number[%d] invalid", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }
    return TimerCntrlStop(TimerCntrlOpen(number));
}

static int32_t TimerIoCb()
{
    return HDF_SUCCESS;
}

static int32_t TimerIoSet(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t len;
    uint32_t handle;
    int16_t number;
    struct TimerConfig *cfg = NULL;
    if (data == NULL) {
        HDF_LOGE("%s: param invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%s: read handle failed!", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadBuffer(data, (const void **)&cfg, &len) || cfg == NULL) {
        HDF_LOGE("%s: read buffer failed!", __func__);
        return HDF_ERR_IO;
    }

    number = (int16_t)(handle - TIMER_HANDLE_SHIFT);
    if (number < 0) {
        HDF_LOGE("%s: number[%d] invalid", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }
    return TimerCntrlSet(TimerCntrlOpen(number), cfg->useconds, TimerIoCb);
}

static int32_t TimerIoSetOnce(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t len;
    uint32_t handle;
    int16_t number;
    struct TimerConfig *cfg = NULL;
    if (data == NULL) {
        HDF_LOGE("%s: param invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%s: read handle failed!", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadBuffer(data, (const void **)&cfg, &len) || cfg == NULL) {
        HDF_LOGE("%s: read buffer failed!", __func__);
        return HDF_ERR_IO;
    }

    number = (int16_t)(handle - TIMER_HANDLE_SHIFT);
    if (number < 0) {
        HDF_LOGE("%s: number[%d] invalid", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }
    return TimerCntrlSetOnce(TimerCntrlOpen(number), cfg->useconds, TimerIoCb);
}

static int32_t TimerIoGet(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret = HDF_SUCCESS;
    struct TimerConfig cfg;
    uint32_t handle;
    int16_t number;
    if ((data == NULL) || (reply == NULL)) {
        HDF_LOGE("%s: param null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%s: HdfSbufReadUint32 failed", __func__);
        return HDF_ERR_IO;
    }

    number = (int16_t)(handle - TIMER_HANDLE_SHIFT);
    if (number < 0) {
        HDF_LOGE("%s: number[%d] invalid", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }
    cfg.number = number;
    ret = TimerCntrlGet(TimerCntrlOpen(number), &cfg.useconds, &cfg.isPeriod);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerCntrlGet failed!", __func__);
        return ret;
    }

    if (!HdfSbufWriteBuffer(reply, &cfg, sizeof(cfg))) {
        HDF_LOGE("%s: write buffer failed!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t TimerIoDispatch(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;

    switch (cmd) {
        case TIMER_IO_OPEN:
            ret = TimerIoOpen(data, reply);
            break;
        case TIMER_IO_CLOSE:
            ret = TimerIoClose(data, reply);
            break;
        case TIMER_IO_START:
            ret = TimerIoStart(data, reply);
            break;
        case  TIMER_IO_STOP:
            ret = TimerIoStop(data, reply);
            break;
        case  TIMER_IO_SET:
            ret = TimerIoSet(data, reply);
            break;
        case  TIMER_IO_SETONCE:
            ret = TimerIoSetOnce(data, reply);
            break;
        case  TIMER_IO_GET:
            ret = TimerIoGet(data, reply);
            break;
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            HDF_LOGE("%s: cmd[%d] not support!", __func__, cmd);
            break;
    }
    return ret;
}

int32_t TimerListRemoveAll(void)
{
    struct TimerCntrl *pos = NULL;
    struct TimerCntrl *tmp = NULL;
    struct TimerManager *manager = g_timerManager;

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock regulator manager fail", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->timerListHead, struct TimerCntrl, node) {
        if ((pos->ops->Remove != NULL) && (pos->ops->Remove(pos) != HDF_SUCCESS)) {
            HDF_LOGE("%s: remove %u failed", __func__, pos->info.number);
        }
        DListRemove(&pos->node);
        (void)OsalMutexDestroy(&pos->lock);
        OsalMemFree(pos);
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("%s: remove all regulator success", __func__);
    return HDF_SUCCESS;
}

int32_t TimerCntrlAdd(struct TimerCntrl *cntrl)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(cntrl->ops, HDF_ERR_INVALID_PARAM);
    struct TimerCntrl *pos = NULL;
    struct TimerCntrl *tmp = NULL;
    struct TimerManager *manager = g_timerManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->timerListHead, struct TimerCntrl, node) {
        if (cntrl->info.number == pos->info.number) {
            HDF_LOGE("%s: timer[%u] existed", __func__, cntrl->info.number);
            return HDF_FAILURE;
        }
    }

    // init info
    if (OsalMutexInit(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexInit %u failed", __func__, cntrl->info.number);
        return HDF_FAILURE;
    }

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %u failed", __func__, cntrl->info.number);
        return HDF_ERR_DEVICE_BUSY;
    }
    DListInsertTail(&cntrl->node, &manager->timerListHead);
    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("%s: add timer number[%u] success", __func__, cntrl->info.number);

    return HDF_SUCCESS;
}

int32_t TimerCntrlRemoveByNumber(const uint32_t number)
{
    struct TimerCntrl *pos = NULL;
    struct TimerCntrl *tmp = NULL;
    struct TimerManager *manager = g_timerManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock failed", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->timerListHead, struct TimerCntrl, node) {
        if (number == pos->info.number) {
            if ((pos->ops->Remove != NULL) && (pos->ops->Remove(pos) != HDF_SUCCESS)) {
                HDF_LOGE("%s: remove %u failed", __func__, pos->info.number);
            }
            (void)OsalMutexDestroy(&pos->lock);
            DListRemove(&pos->node);
            OsalMemFree(pos);
            break;
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("%s: remove timer %u success", __func__, number);
    return HDF_SUCCESS;
}

static int32_t TimerManagerBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct TimerManager *manager = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_OBJECT);

    manager = (struct TimerManager *)OsalMemCalloc(sizeof(*manager));
    if (manager == NULL) {
        HDF_LOGE("%s: malloc manager fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = OsalMutexInit(&manager->lock);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mutex init fail:%d", __func__, ret);
        OsalMemFree(manager);
        manager = NULL;
        return HDF_FAILURE;
    }

    manager->device = device;
    device->service = &manager->service;
    device->service->Dispatch = TimerIoDispatch;
    DListHeadInit(&manager->timerListHead);
    g_timerManager = manager;

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t TimerManagerInit(struct HdfDeviceObject *device)
{
    (void)device;

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

static void TimerManagerRelease(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: Enter!", __func__);
    CHECK_NULL_PTR_RETURN(device);

    if (TimerListRemoveAll() != HDF_SUCCESS) {
        HDF_LOGE("%s: failed", __func__);
    }

    struct TimerManager *manager = (struct TimerManager *)device->service;
    CHECK_NULL_PTR_RETURN(manager);
    OsalMutexDestroy(&manager->lock);
    OsalMemFree(manager);
    g_timerManager = NULL;
}

struct HdfDriverEntry g_timerManagerEntry = {
    .moduleVersion = 1,
    .Bind = TimerManagerBind,
    .Init = TimerManagerInit,
    .Release = TimerManagerRelease,
    .moduleName = "HDF_PLATFORM_TIMER_MANAGER",
};

HDF_INIT(g_timerManagerEntry);
