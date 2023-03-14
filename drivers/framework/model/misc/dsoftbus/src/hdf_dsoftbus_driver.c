/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "module_manager.h"

#define HDF_LOG_TAG "hdf_dsoftbus"

static struct HdfDeviceObject *g_hdfDevObj = NULL;

int32_t HdfSoftbusBroadcastEvent(uint32_t moudleId, const struct HdfSBuf *data)
{
    if (g_hdfDevObj == NULL) {
        return HDF_FAILURE;
    }
    return HdfDeviceSendEvent(g_hdfDevObj, moudleId, data);
}

static int32_t DispatchCommand(struct HdfDeviceIoClient *client, int moduleId,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    (void)client;
    SoftbusDispatchModuleCommand(moduleId, reqData, rspData);
    return HDF_SUCCESS;
}

static int32_t HdfSoftbusDriverBind(struct HdfDeviceObject *dev)
{
    static struct IDeviceIoService softbusService = {
        .object.objectId = 1,
        .Dispatch = DispatchCommand,
    };

    if (dev == NULL) {
        HDF_LOGE("hdf device object is null");
        return HDF_FAILURE;
    }
    dev->service = &softbusService;
    HDF_LOGE("softbus driver bind success");
    return HDF_SUCCESS;
}

static int32_t HdfSoftbusDriverInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("device object is null");
        return HDF_ERR_INVALID_PARAM;
    }
    g_hdfDevObj = device;
    return SoftbusModuleManagerInit(device);
}

static void HdfSoftbusDriverRelease(struct HdfDeviceObject *object)
{
    (void)object;
    HDF_LOGE("softbus driver release success");
}

static struct HdfDriverEntry g_hdfSoftbusEntry = {
    .moduleVersion = 1,
    .Bind = HdfSoftbusDriverBind,
    .Init = HdfSoftbusDriverInit,
    .Release = HdfSoftbusDriverRelease,
    .moduleName = "HDF_DSOFTBUS"
};

HDF_INIT(g_hdfSoftbusEntry);