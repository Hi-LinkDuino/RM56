/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "dac_core.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_spinlock.h"
#include "osal_time.h"
#include "platform_core.h"

#define DAC_HANDLE_SHIFT    0xFF00U
#define HDF_LOG_TAG dac_core_c

struct DacManager {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct DacDevice *devices[DAC_DEVICES_MAX];
    OsalSpinlock spin;
};

static struct DacManager *g_dacManager = NULL;

static int32_t DacDeviceLockDefault(struct DacDevice *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return OsalSpinLock(&device->spin);
}

static void DacDeviceUnlockDefault(struct DacDevice *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    (void)OsalSpinUnlock(&device->spin);
}

static const struct DacLockMethod g_dacLockOpsDefault = {
    .lock = DacDeviceLockDefault,
    .unlock = DacDeviceUnlockDefault,
};

static inline int32_t DacDeviceLock(struct DacDevice *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (device->lockOps == NULL || device->lockOps->lock == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return device->lockOps->lock(device);
}

static inline void DacDeviceUnlock(struct DacDevice *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    if (device->lockOps != NULL && device->lockOps->unlock != NULL) {
        device->lockOps->unlock(device);
    }
}

static int32_t DacManagerAddDevice(struct DacDevice *device)
{
    int32_t ret;
    struct DacManager *manager = g_dacManager;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (device->devNum >= DAC_DEVICES_MAX) {
        HDF_LOGE("%s: devNum:%u exceed", __func__, device->devNum);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (manager == NULL) {
        HDF_LOGE("%s: get dac manager fail", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalSpinLockIrq(&manager->spin) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock dac manager fail", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    if (manager->devices[device->devNum] != NULL) {
        HDF_LOGE("%s: dac device num:%u alwritey exits", __func__, device->devNum);
        ret = HDF_FAILURE;
    } else {
        manager->devices[device->devNum] = device;
        ret = HDF_SUCCESS;
    }

    (void)OsalSpinUnlockIrq(&manager->spin);
    return ret;
}

static void DacManagerRemoveDevice(struct DacDevice *device)
{
    struct DacManager *manager = g_dacManager;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    if (device->devNum < 0 || device->devNum >= DAC_DEVICES_MAX) {
        HDF_LOGE("%s: invalid devNum:%u", __func__, device->devNum);
        return;
    }

    if (manager == NULL) {
        HDF_LOGE("%s: get dac manager fail", __func__);
        return;
    }

    if (OsalSpinLockIrq(&manager->spin) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock dac manager fail", __func__);
        return;
    }

    if (manager->devices[device->devNum] != device) {
        HDF_LOGE("%s: dac device(%u) not in manager", __func__, device->devNum);
    } else {
        manager->devices[device->devNum] = NULL;
    }

    (void)OsalSpinUnlockIrq(&manager->spin);
}

static struct DacDevice *DacManagerFindDevice(uint32_t number)
{
    struct DacDevice *device = NULL;
    struct DacManager *manager = g_dacManager;

    if (number >= DAC_DEVICES_MAX) {
        HDF_LOGE("%s: invalid devNum:%u", __func__, number);
        return NULL;
    }

    if (manager == NULL) {
        HDF_LOGE("%s: get dac manager fail", __func__);
        return NULL;
    }

    if (OsalSpinLockIrq(&manager->spin) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock dac manager fail", __func__);
        return NULL;
    }

    device = manager->devices[number];
    (void)OsalSpinUnlockIrq(&manager->spin);

    return device;
}

int32_t DacDeviceAdd(struct DacDevice *device)
{
    int32_t ret;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->ops == NULL) {
        HDF_LOGE("%s: no ops supplied", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->lockOps == NULL) {
        HDF_LOGI("%s: use default lockOps!", __func__);
        device->lockOps = &g_dacLockOpsDefault;
    }

    if (OsalSpinInit(&device->spin) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock failed", __func__);
        return HDF_FAILURE;
    }

    ret = DacManagerAddDevice(device);
    if (ret != HDF_SUCCESS) {
        (void)OsalSpinDestroy(&device->spin);
    }
    return ret;
}

void DacDeviceRemove(struct DacDevice *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    DacManagerRemoveDevice(device);
    (void)OsalSpinDestroy(&device->spin);
}

struct DacDevice *DacDeviceGet(uint32_t number)
{
    return DacManagerFindDevice(number);
}

void DacDevicePut(struct DacDevice *device)
{
    (void)device;
}

static struct DacDevice *DacDeviceOpen(uint32_t number)
{
    int32_t ret;
    struct DacDevice *device = NULL;

    device = DacDeviceGet(number);
    if (device == NULL) {
        HDF_LOGE("%s: Get device failed!", __func__);
        return NULL;
    }

    ret = DacDeviceStart(device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: start device failed!", __func__);
        return NULL;
    }

    return device;
}

static void DacDeviceClose(struct DacDevice *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: close dac device fail", __func__);
        return;
    }

    (void)DacDeviceStop(device);
    DacDevicePut(device);
}

int32_t DacDeviceWrite(struct DacDevice *device, uint32_t channel, uint32_t val)
{
    int32_t ret;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->ops == NULL || device->ops->write == NULL) {
        HDF_LOGE("%s: ops or write is null", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (DacDeviceLock(device) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock add device failed", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = device->ops->write(device, channel, val);
    DacDeviceUnlock(device);
    return ret;
}

int32_t DacDeviceStart(struct DacDevice *device)
{
    int32_t ret;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->ops == NULL || device->ops->start == NULL) {
        HDF_LOGE("%s: ops or start is null", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (DacDeviceLock(device) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock add device failed", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = device->ops->start(device);
    DacDeviceUnlock(device);
    return ret;
}

int32_t DacDeviceStop(struct DacDevice *device)
{
    int32_t ret;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->ops == NULL || device->ops->stop == NULL) {
        HDF_LOGE("%s: ops or stop is null", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (DacDeviceLock(device) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock add device failed", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = device->ops->stop(device);
    DacDeviceUnlock(device);
    return ret;
}

static int32_t DacManagerIoOpen(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t number;

    if (data == NULL || reply == NULL) {
        HDF_LOGE("%s: invalid data or reply", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &number)) {
        HDF_LOGE("%s: read number failed!", __func__);
        return HDF_ERR_IO;
    }

    if (number >= DAC_DEVICES_MAX || reply == NULL) {
        HDF_LOGE("%s: invalid number %u", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }

    if (DacDeviceOpen(number) == NULL) {
        HDF_LOGE("%s: get device %u failed!", __func__, number);
        return HDF_ERR_NOT_SUPPORT;
    }

    number = (uint32_t)(number + DAC_HANDLE_SHIFT);
    if (!HdfSbufWriteUint32(reply, (uint32_t)(uintptr_t)number)) {
        HDF_LOGE("%s: write number failed!", __func__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t DacManagerIoClose(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t number;

    if (data == NULL) {
        HDF_LOGE("%s: invalid data", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &number)) {
        HDF_LOGE("%s: read number failed!", __func__);
        return HDF_ERR_IO;
    }

    number  = (uint32_t)(number - DAC_HANDLE_SHIFT);
    if (number >= DAC_DEVICES_MAX) {
        HDF_LOGE("%s: invalid number %u", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }

    DacDeviceClose(DacDeviceGet(number));
    return HDF_SUCCESS;
}

static int32_t DacManagerIoWrite(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t channel;
    uint32_t val;
    uint32_t number;

    if (data == NULL) {
        HDF_LOGE("%s: invalid data", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &number)) {
        HDF_LOGE("%s: read number failed!", __func__);
        return HDF_ERR_IO;
    }

    number  = (uint32_t)(number - DAC_HANDLE_SHIFT);
    if (number >= DAC_DEVICES_MAX) {
        HDF_LOGE("%s: invalid number %u", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &channel)) {
        HDF_LOGE("%s: read dac channel failed", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadUint32(data, &val)) {
        HDF_LOGE("%s: read dac value failed", __func__);
        return HDF_ERR_IO;
    }

    ret = DacDeviceWrite(DacDeviceGet(number), channel, val);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: write dac failed:%d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t DacManagerDispatch(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    switch (cmd) {
        case DAC_IO_OPEN:
            return DacManagerIoOpen(data, reply);
        case DAC_IO_CLOSE:
            return DacManagerIoClose(data, reply);
        case DAC_IO_WRITE:
            return DacManagerIoWrite(data, reply);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}
static int32_t DacManagerBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}
static int32_t DacManagerInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct DacManager *manager = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    manager = (struct DacManager *)OsalMemCalloc(sizeof(*manager));
    if (manager == NULL) {
        HDF_LOGE("%s: alloc manager failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = OsalSpinInit(&manager->spin);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: spinlock init failed", __func__);
        OsalMemFree(manager);
        return HDF_FAILURE;
    }

    manager->device = device;
    g_dacManager = manager;
    device->service = &manager->service;
    device->service->Dispatch = DacManagerDispatch;
    return HDF_SUCCESS;
}

static void DacManagerRelease(struct HdfDeviceObject *device)
{
    struct DacManager *manager = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }

    manager = (struct DacManager *)device->service;
    if (manager == NULL) {
        HDF_LOGI("%s: no service bind", __func__);
        return;
    }

    g_dacManager = NULL;
    OsalMemFree(manager);
}

struct HdfDriverEntry g_dacManagerEntry = {
    .moduleVersion = 1,
    .Bind = DacManagerBind,
    .Init = DacManagerInit,
    .Release = DacManagerRelease,
    .moduleName = "HDF_PLATFORM_DAC_MANAGER",
};
HDF_INIT(g_dacManagerEntry);
