/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "dac/dac_core.h"
#include "asm/platform.h"
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "los_hwi.h"
#include "osal_io.h"
#include "osal_mem.h"
#include "osal_time.h"

#define HDF_LOG_TAG dac_virtual

struct VirtualDacDevice {
    struct DacDevice device;
    uint32_t deviceNum;
    uint32_t validChannel;
    uint32_t rate;
};

static int32_t VirtualDacWrite(struct DacDevice *device, uint32_t channel, uint32_t val)
{
    (void)device;
    (void)channel;
    (void)val;
    HDF_LOGI("%s: done!", __func__);

    return HDF_SUCCESS;
}

static inline int32_t VirtualDacStart(struct DacDevice *device)
{
    (void)device;
    HDF_LOGI("%s: done!", __func__);
    return HDF_SUCCESS;
}

static inline int32_t VirtualDacStop(struct DacDevice *device)
{
    (void)device;
    HDF_LOGI("%s: done!", __func__);
    return HDF_SUCCESS;
}

static inline void VirtualDacDeviceInit(struct VirtualDacDevice *virtual)
{
    HDF_LOGI("%s: device:%u init done!", __func__, virtual->deviceNum);
}

static const struct DacMethod g_method = {
    .write = VirtualDacWrite,
    .stop = VirtualDacStop,
    .start = VirtualDacStart,
};

static int32_t VirtualDacReadDrs(struct VirtualDacDevice *virtual, const struct DeviceResourceNode *node)
{
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL || drsOps->GetUint16 == NULL) {
        HDF_LOGE("%s: Invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint32(node, "deviceNum", &virtual->deviceNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: Read deviceNum fail!", __func__);
        return HDF_ERR_IO;
    }
    if (drsOps->GetUint32(node, "validChannel", &virtual->validChannel, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: Read validChannel fail!", __func__);
        return HDF_ERR_IO;
    }
    if (drsOps->GetUint32(node, "rate", &virtual->rate, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: Read rate fail!", __func__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t VirtualDacParseAndInit(struct HdfDeviceObject *device, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct VirtualDacDevice *virtual = NULL;
    (void)device;

    virtual = (struct VirtualDacDevice *)OsalMemCalloc(sizeof(*virtual));
    if (virtual == NULL) {
        HDF_LOGE("%s: Malloc virtual fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = VirtualDacReadDrs(virtual, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Read drs fail! ret:%d", __func__, ret);
        OsalMemFree(virtual);
        virtual = NULL;
        return ret;
    }

    VirtualDacDeviceInit(virtual);
    virtual->device.priv = (void *)node;
    virtual->device.devNum = virtual->deviceNum;
    virtual->device.ops = &g_method;
    ret = DacDeviceAdd(&virtual->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: add Dac controller failed! ret = %d", __func__, ret);
        OsalMemFree(virtual);
        virtual = NULL;
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t VirtualDacInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    const struct DeviceResourceNode *childNode = NULL;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = HDF_SUCCESS;
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        ret = VirtualDacParseAndInit(device, childNode);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }
    return ret;
}

static void VirtualDacRemoveByNode(const struct DeviceResourceNode *node)
{
    int32_t ret;
    int16_t devNum;
    struct DacDevice *device = NULL;
    struct VirtualDacDevice *virtual = NULL;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return;
    }

    ret = drsOps->GetUint16(node, "devNum", (uint16_t *)&devNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read devNum fail!", __func__);
        return;
    }

    device = DacDeviceGet(devNum);
    if (device != NULL && device->priv == node) {
        DacDevicePut(device);
        DacDeviceRemove(device);
        virtual = (struct VirtualDacDevice *)device;
        OsalMemFree(virtual);
    }
    return;
}

static void VirtualDacRelease(struct HdfDeviceObject *device)
{
    const struct DeviceResourceNode *childNode = NULL;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        VirtualDacRemoveByNode(childNode);
    }
}

struct HdfDriverEntry g_dacDriverEntry = {
    .moduleVersion = 1,
    .Init = VirtualDacInit,
    .Release = VirtualDacRelease,
    .moduleName = "virtual_dac_driver",
};
HDF_INIT(g_dacDriverEntry);
