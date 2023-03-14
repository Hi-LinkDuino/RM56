/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "platform_device.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "platform_core.h"
#include "securec.h"
#include "stdarg.h"

#define PLATFORM_DEV_NAME_DEFAULT "platform_device"

static void PlatformDeviceOnFirstGet(struct HdfSRef *sref)
{
    (void)sref;
}

static void PlatformDeviceOnLastPut(struct HdfSRef *sref)
{
    struct PlatformDevice *device = NULL;
    int32_t ret;

    if (sref == NULL) {
        return;
    }

    device = CONTAINER_OF(sref, struct PlatformDevice, ref);
    if (device == NULL) {
        PLAT_LOGE("PlatformDeviceOnLastPut: get device is NULL!");
        return;
    }

    ret = PlatformDevicePostEvent(device, PLAT_EVENT_DEVICE_DEAD);
    if (ret != HDF_SUCCESS) {
        PLAT_LOGE("PlatformDeviceOnLastPut: post event failed:%d", ret);
        return;
    }
    PLAT_LOGI("PlatformDeviceOnLastPut: device:%s(%d) life cycle end", device->name, device->number);
}

struct IHdfSRefListener g_platObjListener = {
    .OnFirstAcquire = PlatformDeviceOnFirstGet,
    .OnLastRelease = PlatformDeviceOnLastPut,
};

#define PLATFORM_DEV_NAME_LEN 64
int32_t PlatformDeviceSetName(struct PlatformDevice *device, const char *fmt, ...)
{
    int ret;
    char tmpName[PLATFORM_DEV_NAME_LEN + 1] = {0};
    size_t realLen;
    va_list vargs;
    char *realName = NULL;

    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (fmt == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    va_start(vargs, fmt);
    ret = vsnprintf_s(tmpName, PLATFORM_DEV_NAME_LEN, PLATFORM_DEV_NAME_LEN, fmt, vargs);
    va_end(vargs);
    if (ret  <= 0) {
        PLAT_LOGE("PlatformDeviceSetName: failed to format device name:%d", ret);
        return HDF_PLT_ERR_OS_API;
    }

    realLen = strlen(tmpName);
    if (realLen <= 0 || realLen > PLATFORM_DEV_NAME_LEN) {
        PLAT_LOGE("PlatformDeviceSetName: invalid name len:%zu", realLen);
        return HDF_ERR_INVALID_PARAM;
    }
    realName = OsalMemCalloc(realLen + 1);
    if (realName == NULL) {
        PLAT_LOGE("PlatformDeviceSetName: alloc name mem failed");
        return HDF_ERR_MALLOC_FAIL;
    }

    PLAT_LOGD("PlatformDeviceSetName: tmpName:%s, realLen:%zu", tmpName, realLen);
    ret = strncpy_s(realName, realLen + 1, tmpName, realLen);
    if (ret != EOK) {
        OsalMemFree(realName);
        PLAT_LOGE("PlatformDeviceSetName: copy name failed:%d", ret);
        return HDF_ERR_IO;
    }

    device->name = (const char *)realName;
    return HDF_SUCCESS;
}

void PlatformDeviceClearName(struct PlatformDevice *device)
{
    if (device != NULL && device->name != NULL) {
        OsalMemFree((char *)device->name);
        device->name = NULL;
    }
}

int32_t PlatformDeviceInit(struct PlatformDevice *device)
{
    int32_t ret;
    if (device == NULL) {
        PLAT_LOGW("PlatformDeviceInit: device is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }

    if ((ret = OsalSpinInit(&device->spin)) != HDF_SUCCESS) {
        return ret;
    }
    if ((ret = OsalSemInit(&device->released, 0)) != HDF_SUCCESS) {
        (void)OsalSpinDestroy(&device->spin);
        return ret;
    }
    if ((ret = PlatformEventInit(&device->event)) != HDF_SUCCESS) {
        (void)OsalSemDestroy(&device->released);
        (void)OsalSpinDestroy(&device->spin);
        return ret;
    }
    DListHeadInit(&device->node);
    HdfSRefConstruct(&device->ref, &g_platObjListener);

    return HDF_SUCCESS;
}

void PlatformDeviceUninit(struct PlatformDevice *device)
{
    if (device == NULL) {
        PLAT_LOGW("PlatformDevice: device is NULL");
        return;
    }

    (void)PlatformEventUninit(&device->event);
    (void)OsalSemDestroy(&device->released);
    (void)OsalSpinDestroy(&device->spin);
}

int32_t PlatformDeviceGet(struct PlatformDevice *device)
{
    if (device == NULL) {
        PLAT_LOGE("PlatformDeviceGet: device is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }

    HdfSRefAcquire(&device->ref);
    return HDF_SUCCESS;
}

void PlatformDevicePut(struct PlatformDevice *device)
{
    if (device != NULL) {
        HdfSRefRelease(&device->ref);
    }
}

int32_t PlatformDeviceRefCount(struct PlatformDevice *device)
{
    if (device != NULL) {
        return HdfSRefCount(&device->ref);
    }
    return HDF_ERR_INVALID_OBJECT;
}

int32_t PlatformDeviceAdd(struct PlatformDevice *device)
{
    int32_t ret;
    struct PlatformManager *manager = NULL;

    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    manager = device->manager;
    if (manager == NULL) {
        manager = PlatformManagerGet(PLATFORM_MODULE_DEFAULT);
        if (manager == NULL) {
            PLAT_LOGE("PlatformDeviceAdd: get default manager failed");
            return HDF_PLT_ERR_INNER;
        }
    }

    if ((ret = PlatformDeviceInit(device)) != HDF_SUCCESS) {
        return ret;
    }

    ret = PlatformManagerAddDevice(manager, device);
    if (ret == HDF_SUCCESS) {
        device->manager = manager;
        PLAT_LOGD("PlatformDeviceAdd: add dev:%s(%d) success", device->name, device->number);
    } else {
        PLAT_LOGE("PlatformDeviceAdd: add %s(%d) failed:%d", device->name, device->number, ret);
        PlatformDeviceUninit(device);
    }
    return ret;
}

void PlatformDeviceDel(struct PlatformDevice *device)
{
    int32_t ret;
    struct PlatformManager *manager = NULL;

    if (device == NULL) {
        return;
    }

    manager = device->manager;
    if (manager == NULL) {
        manager = PlatformManagerGet(PLATFORM_MODULE_DEFAULT);
        if (manager == NULL) {
            PLAT_LOGE("PlatformDeviceAdd: get default manager failed");
            return;
        }
    }
    ret = PlatformManagerDelDevice(manager, device);
    if (ret != HDF_SUCCESS) {
        PLAT_LOGW("PlatformDeviceDel: failed to remove device:%s from manager:%s",
            device->name, manager->device.name);
        return; // it's dagerous to continue ...
    }

    /* make sure no reference anymore before exit. */
    ret = PlatformDeviceWaitEvent(device, PLAT_EVENT_DEVICE_DEAD, HDF_WAIT_FOREVER, NULL);
    if (ret == HDF_SUCCESS) {
        PLAT_LOGD("PlatformDeviceDel: remove dev:%s(%d) success", device->name, device->number);
    } else {
        PLAT_LOGE("PlatformDeviceDel: wait %s(%d) dead failed:%d", device->name, device->number, ret);
    }
    PlatformDeviceUninit(device);
}

int32_t PlatformDeviceCreateService(struct PlatformDevice *device,
    int32_t (*dispatch)(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply))
{
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->service != NULL) {
        PLAT_LOGE("PlatformDeviceCreateService: service already creatted!");
        return HDF_FAILURE;
    }

    device->service = (struct IDeviceIoService *)OsalMemCalloc(sizeof(*(device->service)));
    if (device->service == NULL) {
        PLAT_LOGE("PlatformDeviceCreateService: alloc service failed!");
        return HDF_ERR_MALLOC_FAIL;
    }

    device->service->Dispatch = dispatch;
    return HDF_SUCCESS;
}

void PlatformDeviceDestroyService(struct PlatformDevice *device)
{
    if (device == NULL) {
        return;
    }

    if (device->service == NULL) {
        return;
    }

    OsalMemFree(device->service);
    device->service = NULL;
}

int32_t PlatformDeviceBind(struct PlatformDevice *device, struct HdfDeviceObject *hdfDevice)
{
    if (device == NULL || hdfDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (device->hdfDev != NULL) {
        PLAT_LOGE("PlatformDeviceBind: already bind a hdf device!");
        return HDF_FAILURE;
    }

    device->hdfDev = hdfDevice;
    hdfDevice->service = device->service;
    hdfDevice->priv = device;
    return HDF_SUCCESS;
}

void PlatformDeviceUnbind(struct PlatformDevice *device, struct HdfDeviceObject *hdfDev)
{
    if (device == NULL) {
        return;
    }
    if (device->hdfDev == NULL) {
        return;
    }
    if (device->hdfDev != hdfDev) {
        PLAT_LOGW("PlatformDeviceUnbind: hdf device not match!");
        return;
    }

    device->hdfDev->service = NULL;
    device->hdfDev->priv = NULL;
    device->hdfDev = NULL;
}

struct PlatformDevice *PlatformDeviceFromHdfDev(struct HdfDeviceObject *hdfDev)
{
    if (hdfDev == NULL || hdfDev->priv == NULL) {
        PLAT_LOGE("PlatformDeviceFromHdfDev: hdf device or priv null");
        return NULL;
    }

    return (struct PlatformDevice *)hdfDev->priv;
}

int32_t PlatformDevicePostEvent(struct PlatformDevice *device, uint32_t events)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return PlatformEventPost(&device->event, events);
}

int32_t PlatformDeviceWaitEvent(struct PlatformDevice *device, uint32_t mask, uint32_t tms, uint32_t *events)
{
    int32_t ret;
    uint32_t eventsRead;

    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = PlatformEventWait(&device->event, mask, PLAT_EVENT_MODE_CLEAR, tms, &eventsRead);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (eventsRead == 0) {
        return HDF_FAILURE;
    }
    if (events != NULL) {
        *events = eventsRead;
    }
    return HDF_SUCCESS;
}

int32_t PlatformDeviceListenEvent(struct PlatformDevice *device, struct PlatformEventListener *listener)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (listener == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return PlatformEventListen(&device->event, listener);
}

void PlatformDeviceUnListenEvent(struct PlatformDevice *device, struct PlatformEventListener *listener)
{
    if (device != NULL && listener != NULL) {
        PlatformEventUnlisten(&device->event, listener);
    }
}
