/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pcie_core.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "pcie_dispatch.h"
#include "securec.h"

#define HDF_LOG_TAG pcie_core_c

static int32_t PcieCntlrInit(struct PcieCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->hdfDevObj == NULL) {
        HDF_LOGE("PcieCntlrInit: no HdfDeviceObject attached!");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = OsalMutexInit(&cntlr->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("PcieCntlrInit: mutex init fail!");
        return ret;
    }

    cntlr->service.Dispatch = PcieIoDispatch;
    cntlr->hdfDevObj->service = &(cntlr->service);
    cntlr->device.number = (int32_t)cntlr->devInfo.busNum;
    cntlr->device.hdfDev = cntlr->hdfDevObj;
    return HDF_SUCCESS;
}

static void PcieCntlrUninit(struct PcieCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexDestroy(&cntlr->mutex);
    }
}

int32_t PcieCntlrAdd(struct PcieCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = PcieCntlrInit(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    cntlr->device.manager = PlatformManagerGet(PLATFORM_MODULE_PCIE);
    ret = PlatformDeviceAdd(&cntlr->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("PcieCntlrAdd: device add fail!");
        PcieCntlrUninit(cntlr);
        return ret;
    }
    return HDF_SUCCESS;
}

void PcieCntlrRemove(struct PcieCntlr *cntlr)
{
    if (cntlr != NULL) {
        PlatformDeviceDel(&cntlr->device);
        PcieCntlrUninit(cntlr);
    }
}

int32_t PcieCntlrParse(struct PcieCntlr *cntlr, struct HdfDeviceObject *obj)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (obj == NULL || cntlr == NULL) {
        HDF_LOGE("PcieCntlrParse: input param is NULL.");
        return HDF_FAILURE;
    }

    node = obj->property;
    if (node == NULL) {
        HDF_LOGE("PcieCntlrParse: drs node is NULL.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("PcieCntlrParse: invalid drs ops fail.");
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "busNum", &(cntlr->devInfo.busNum), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("PcieCntlrParse: read busNum fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "vendorId", &(cntlr->devInfo.vendorId), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("PcieCntlrParse: read vendorId fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "devId", &(cntlr->devInfo.devId), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("PcieCntlrParse: read devId fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t PcieCntlrRead(struct PcieCntlr *cntlr, uint32_t pos, uint8_t *data, uint32_t len)
{
    int32_t ret;

    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->read == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    PcieCntlrLock(cntlr);
    ret = cntlr->ops->read(cntlr, pos, data, len);
    PcieCntlrUnlock(cntlr);
    return ret;
}

int32_t PcieCntlrWrite(struct PcieCntlr *cntlr, uint32_t pos, uint8_t *data, uint32_t len)
{
    int32_t ret;
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->write == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    PcieCntlrLock(cntlr);
    ret = cntlr->ops->write(cntlr, pos, data, len);
    PcieCntlrUnlock(cntlr);
    return ret;
}
