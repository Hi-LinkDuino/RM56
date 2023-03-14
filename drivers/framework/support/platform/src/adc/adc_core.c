/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "adc_core.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_spinlock.h"
#include "osal_time.h"
#include "platform_core.h"

#define HDF_LOG_TAG adc_core_c
#define LOCK_WAIT_SECONDS_M 1
#define ADC_BUFF_SIZE 4

#define ADC_HANDLE_SHIFT    0xFF00U

struct AdcManager {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct AdcDevice *devices[ADC_DEVICES_MAX];
    OsalSpinlock spin;
};

static struct AdcManager *g_adcManager = NULL;

static int32_t AdcDeviceLockDefault(struct AdcDevice *device)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return OsalSpinLock(&device->spin);
}

static void AdcDeviceUnlockDefault(struct AdcDevice *device)
{
    if (device == NULL) {
        return;
    }
    (void)OsalSpinUnlock(&device->spin);
}

static const struct AdcLockMethod g_adcLockOpsDefault = {
    .lock = AdcDeviceLockDefault,
    .unlock = AdcDeviceUnlockDefault,
};

static inline int32_t AdcDeviceLock(struct AdcDevice *device)
{
    if (device->lockOps == NULL || device->lockOps->lock == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return device->lockOps->lock(device);
}

static inline void AdcDeviceUnlock(struct AdcDevice *device)
{
    if (device->lockOps != NULL && device->lockOps->unlock != NULL) {
        device->lockOps->unlock(device);
    }
}

int32_t AdcDeviceStart(struct AdcDevice *device)
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

    if (AdcDeviceLock(device) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock add device failed", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = device->ops->start(device);
    AdcDeviceUnlock(device);
    return ret;
}

int32_t AdcDeviceStop(struct AdcDevice *device)
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

    if (AdcDeviceLock(device) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock add device failed", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = device->ops->stop(device);
    AdcDeviceUnlock(device);
    return ret;
}

static int32_t AdcManagerAddDevice(struct AdcDevice *device)
{
    int32_t ret;
    struct AdcManager *manager = g_adcManager;

    if (device->devNum >= ADC_DEVICES_MAX) {
        HDF_LOGE("%s: devNum:%u exceed", __func__, device->devNum);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (manager == NULL) {
        HDF_LOGE("%s: get adc manager fail", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalSpinLockIrq(&manager->spin) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock adc manager fail", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    if (manager->devices[device->devNum] != NULL) {
        HDF_LOGE("%s: adc device num:%u already exits", __func__, device->devNum);
        ret = HDF_FAILURE;
    } else {
        manager->devices[device->devNum] = device;
        ret = HDF_SUCCESS;
    }

    (void)OsalSpinUnlockIrq(&manager->spin);
    return ret;
}

static void AdcManagerRemoveDevice(struct AdcDevice *device)
{
    struct AdcManager *manager = g_adcManager;

    if (device->devNum < 0 || device->devNum >= ADC_DEVICES_MAX) {
        HDF_LOGE("%s: invalid devNum:%u", __func__, device->devNum);
        return;
    }

    if (manager == NULL) {
        HDF_LOGE("%s: get adc manager fail", __func__);
        return;
    }

    if (OsalSpinLockIrq(&manager->spin) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock adc manager fail", __func__);
        return;
    }

    if (manager->devices[device->devNum] != device) {
        HDF_LOGE("%s: adc device(%u) not in manager", __func__, device->devNum);
    } else {
        manager->devices[device->devNum] = NULL;
    }

    (void)OsalSpinUnlockIrq(&manager->spin);
}

static struct AdcDevice *AdcManagerFindDevice(uint32_t number)
{
    struct AdcDevice *device = NULL;
    struct AdcManager *manager = g_adcManager;

    if (number >= ADC_DEVICES_MAX) {
        HDF_LOGE("%s: invalid devNum:%u", __func__, number);
        return NULL;
    }

    if (manager == NULL) {
        HDF_LOGE("%s: get adc manager fail", __func__);
        return NULL;
    }

    if (OsalSpinLockIrq(&manager->spin) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock adc manager fail", __func__);
        return NULL;
    }

    device = manager->devices[number];
    (void)OsalSpinUnlockIrq(&manager->spin);

    return device;
}

struct AdcDevice *AdcDeviceGet(uint32_t number)
{
    return AdcManagerFindDevice(number);
}

void AdcDevicePut(struct AdcDevice *device)
{
    (void)device;
}

static struct AdcDevice *AdcDeviceOpen(uint32_t number)
{
    int32_t ret;
    struct AdcDevice *device = NULL;

    device = AdcDeviceGet(number);
    if (device == NULL) {
        return NULL;
    }

    ret = AdcDeviceStart(device);
    if (ret != HDF_SUCCESS) {
        return NULL;
    }

    return device;
}

static void AdcDeviceClose(struct AdcDevice *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: close adc device fail", __func__);
        return;
    }

    (void)AdcDeviceStop(device);
    AdcDevicePut(device);
}

int32_t AdcDeviceAdd(struct AdcDevice *device)
{
    int32_t ret;

    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->ops == NULL) {
        HDF_LOGE("%s: no ops supplied", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->lockOps == NULL) {
        HDF_LOGI("%s: use default lockOps!", __func__);
        device->lockOps = &g_adcLockOpsDefault;
    }

    if (OsalSpinInit(&device->spin) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock failed", __func__);
        return HDF_FAILURE;
    }

    ret = AdcManagerAddDevice(device);
    if (ret != HDF_SUCCESS) {
        (void)OsalSpinDestroy(&device->spin);
    }
    return ret;
}

void AdcDeviceRemove(struct AdcDevice *device)
{
    if (device == NULL) {
        return;
    }
    AdcManagerRemoveDevice(device);
    (void)OsalSpinDestroy(&device->spin);
}

int32_t AdcDeviceRead(struct AdcDevice *device, uint32_t channel, uint32_t *val)
{
    int32_t ret;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->ops == NULL || device->ops->read == NULL) {
        HDF_LOGE("%s: ops or read is null", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (val == NULL) {
        HDF_LOGE("%s: invalid val pointer!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (AdcDeviceLock(device) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock add device failed", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = device->ops->read(device, channel, val);
    AdcDeviceUnlock(device);
    return ret;
}

static int32_t AdcManagerIoOpen(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t number;

    if (data == NULL || reply == NULL) {
        HDF_LOGE("%s: invalid data or reply!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &number)) {
        HDF_LOGE("%s: read data failed!", __func__);
        return HDF_ERR_IO;
    }

    if (number >= ADC_DEVICES_MAX || reply == NULL) {
        HDF_LOGE("%s: invalid number!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (AdcDeviceOpen(number) == NULL) {
        HDF_LOGE("%s: get device %u failed", __func__, number);
        return HDF_ERR_NOT_SUPPORT;
    }

    number = (uint32_t)(number + ADC_HANDLE_SHIFT);
    if (!HdfSbufWriteUint32(reply, (uint32_t)(uintptr_t)number)) {
        HDF_LOGE("%s: write reply failed!", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t AdcManagerIoClose(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t number;

    (void)reply;
    if (data == NULL) {
        HDF_LOGE("%s: invalid data!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &number)) {
        HDF_LOGE("%s: read data failed!", __func__);
        return HDF_ERR_IO;
    }

    number = (uint32_t)(number - ADC_HANDLE_SHIFT);
    if (number >= ADC_DEVICES_MAX) {
        HDF_LOGE("%s: get device %u failed", __func__, number);
        return HDF_ERR_INVALID_PARAM;
    }

    AdcDeviceClose(AdcManagerFindDevice(number));
    return HDF_SUCCESS;
}

static int32_t AdcManagerIoRead(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t number;
    uint32_t channel;
    uint32_t val;

    if (data == NULL || reply == NULL) {
        HDF_LOGE("%s: invalid data or reply!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &number)) {
        HDF_LOGE("AdcManagerIoRead: read handle failed!");
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadUint32(data, &channel)) {
        HDF_LOGE("AdcManagerIoRead: read handle failed!");
        return HDF_ERR_IO;
    }

    number  = (uint32_t)(number - ADC_HANDLE_SHIFT);
    ret = AdcDeviceRead(AdcManagerFindDevice(number), channel, &val);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AdcManagerIoRead: read val failed!");
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteUint32(reply, val)) {
        HDF_LOGE("%s: write val fail!", __func__);
        return HDF_ERR_IO;
    }

    return ret;
}

static int32_t AdcManagerDispatch(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    switch (cmd) {
        case ADC_IO_OPEN:
            return AdcManagerIoOpen(data, reply);
        case ADC_IO_CLOSE:
            return AdcManagerIoClose(data, reply);
        case ADC_IO_READ:
            return AdcManagerIoRead(data, reply);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static int32_t AdcManagerBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static int32_t AdcManagerInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct AdcManager *manager = NULL;

    HDF_LOGI("%s: Enter", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    manager = (struct AdcManager *)OsalMemCalloc(sizeof(*manager));
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
    g_adcManager = manager;
    device->service = &manager->service;
    device->service->Dispatch = AdcManagerDispatch;
    return HDF_SUCCESS;
}

static void AdcManagerRelease(struct HdfDeviceObject *device)
{
    struct AdcManager *manager = NULL;

    HDF_LOGI("%s: Enter", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }

    manager = (struct AdcManager *)device->service;
    if (manager == NULL) {
        HDF_LOGI("%s: no service bind", __func__);
        return;
    }

    g_adcManager = NULL;
    OsalMemFree(manager);
}

struct HdfDriverEntry g_adcManagerEntry = {
    .moduleVersion = 1,
    .Bind = AdcManagerBind,
    .Init = AdcManagerInit,
    .Release = AdcManagerRelease,
    .moduleName = "HDF_PLATFORM_ADC_MANAGER",
};
HDF_INIT(g_adcManagerEntry);
