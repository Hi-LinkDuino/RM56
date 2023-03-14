/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_device_desc.h"
#include "platform_core.h"
#include "gpio_if.h"
#include "gpio/gpio_core.h"
#include "gpio/gpio_service.h"

#define HDF_LOG_TAG gpio_service

static int32_t GpioServiceIoRead(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint16_t gpio;
    uint16_t value;

    if (data == NULL || reply == NULL) {
        HDF_LOGE("%s: data or reply is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: read gpio number fail", __func__);
        return HDF_ERR_IO;
    }

    ret = GpioRead(gpio, &value);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read gpio fail:%d", __func__, ret);
        return ret;
    }

    if (!HdfSbufWriteUint16(reply, value)) {
        HDF_LOGE("%s: write subf fail:%d", __func__, ret);
        return ret;
    }

    return ret;
}

static int32_t GpioServiceIoWrite(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint16_t gpio;
    uint16_t value;

    (void)reply;
    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
    }

    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: read gpio number fail", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadUint16(data, &value)) {
        HDF_LOGE("%s: read gpio value fail", __func__);
        return HDF_ERR_IO;
    }

    ret = GpioWrite(gpio, value);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: write gpio fail:%d", __func__, ret);
        return ret;
    }

    return ret;
}

static int32_t GpioServiceIoGetDir(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint16_t gpio;
    uint16_t dir;

    if (data == NULL || reply == NULL) {
        HDF_LOGE("%s: data or reply is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: read gpio number fail", __func__);
        return HDF_ERR_IO;
    }

    ret = GpioGetDir(gpio, &dir);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get gpio dir fail:%d", __func__, ret);
        return ret;
    }

    if (!HdfSbufWriteUint16(reply, dir)) {
        HDF_LOGE("%s: write subf fail:%d", __func__, ret);
        return ret;
    }

    return ret;
}

static int32_t GpioServiceIoSetDir(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint16_t gpio;
    uint16_t dir;

    (void)reply;
    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: read gpio number fail", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadUint16(data, &dir)) {
        HDF_LOGE("%s: read gpio dir fail", __func__);
        return HDF_ERR_IO;
    }

    ret = GpioSetDir(gpio, dir);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set gpio dir fail:%d", __func__, ret);
        return ret;
    }

    return ret;
}

static int32_t GpioServiceIoSetIrq(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint16_t gpio;
    uint16_t mode;

    (void)reply;
    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: read gpio fail", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadUint16(data, &mode)) {
        HDF_LOGE("%s: read gpio mode fail", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t GpioServiceIoUnsetIrq(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint16_t gpio;

    (void)reply;
    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: read gpio number fail", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t GpioServiceIoEnableIrq(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint16_t gpio;

    (void)reply;
    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: read gpio number fail", __func__);
        return HDF_ERR_IO;
    }

    ret = GpioEnableIrq(gpio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable gpio irq fail:%d", __func__, ret);
        return ret;
    }

    return ret;
}

static int32_t GpioServiceIoDisableIrq(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint16_t gpio;

    (void)reply;
    if (data == NULL) {
        HDF_LOGE("%s: data is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint16(data, &gpio)) {
        HDF_LOGE("%s: read gpio number fail", __func__);
        return HDF_ERR_IO;
    }

    ret = GpioDisableIrq(gpio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: disable gpio irq fail:%d", __func__, ret);
        return ret;
    }

    return ret;
}


static int32_t GpioServiceDispatch(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;

    switch (cmd) {
        case GPIO_IO_READ:
            return GpioServiceIoRead(data, reply);
        case GPIO_IO_WRITE:
            return GpioServiceIoWrite(data, reply);
        case GPIO_IO_GETDIR:
            return GpioServiceIoGetDir(data, reply);
        case GPIO_IO_SETDIR:
            return GpioServiceIoSetDir(data, reply);
        case GPIO_IO_SETIRQ:
            return GpioServiceIoSetIrq(data, reply);
        case GPIO_IO_UNSETIRQ:
            return GpioServiceIoUnsetIrq(data, reply);
        case GPIO_IO_ENABLEIRQ:
            return GpioServiceIoEnableIrq(data, reply);
        case GPIO_IO_DISABLEIRQ:
            return GpioServiceIoDisableIrq(data, reply);
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }
    return ret;
}

static int32_t GpioServiceBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct PlatformManager *gpioMgr = NULL;

    PLAT_LOGI("GpioServiceBind: enter");
    if (device == NULL) {
        HDF_LOGE("GpioServiceBind: device is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }

    gpioMgr = GpioManagerGet();
    if (gpioMgr == NULL) {
        HDF_LOGE("GpioServiceBind: get gpio manager fail");
        return HDF_PLT_ERR_DEV_GET;
    }

    ret = PlatformDeviceCreateService(&gpioMgr->device, GpioServiceDispatch);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioServiceBind: create gpio service fail:%d", ret);
        return ret;
    }

    ret = PlatformDeviceBind(&gpioMgr->device, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioServiceBind: bind gpio device fail:%d", ret);
        (void)PlatformDeviceDestroyService(&gpioMgr->device);
        return ret;
    }

    PLAT_LOGI("GpioServiceBind: success");
    return HDF_SUCCESS;
}

static int32_t GpioServiceInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void GpioServiceRelease(struct HdfDeviceObject *device)
{
    struct PlatformManager *gpioMgr = NULL;

    PLAT_LOGI("GpioServiceRelease: enter");
    if (device == NULL) {
        PLAT_LOGI("GpioServiceRelease: device is null");
        return;
    }

    gpioMgr = GpioManagerGet();
    if (gpioMgr == NULL) {
        HDF_LOGE("GpioServiceBind: get gpio manager fail");
        return;
    }

    (void)PlatformDeviceUnbind(&gpioMgr->device, device);
    (void)PlatformDeviceDestroyService(&gpioMgr->device);
    PLAT_LOGI("GpioServiceRelease: done");
}

struct HdfDriverEntry g_gpioServiceEntry = {
    .moduleVersion = 1,
    .Bind = GpioServiceBind,
    .Init = GpioServiceInit,
    .Release = GpioServiceRelease,
    .moduleName = "HDF_PLATFORM_GPIO_MANAGER",
};
HDF_INIT(g_gpioServiceEntry);
