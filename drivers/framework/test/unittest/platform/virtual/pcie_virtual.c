/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "device_resource_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "pcie_core.h"

#define HDF_LOG_TAG pcie_virtual_c

#define PCIE_VIRTUAL_ADAPTER_ONE_BYTE 1
#define PCIE_VIRTUAL_ADAPTER_TWO_BYTE 2
#define PCIE_VIRTUAL_ADAPTER_FOUR_BYTE 4
#define PCIE_VIRTUAL_ADAPTER_READ_DATA_1 0x95
#define PCIE_VIRTUAL_ADAPTER_READ_DATA_2 0x27
#define PCIE_VIRTUAL_ADAPTER_READ_DATA_3 0x89

struct PcieVirtualAdapterHost {
    struct PcieCntlr cntlr;
};

static int32_t PcieVirtualAdapterRead(struct PcieCntlr *cntlr, uint32_t pos, uint8_t *data, uint32_t len)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (len == PCIE_VIRTUAL_ADAPTER_ONE_BYTE) {
        *data = PCIE_VIRTUAL_ADAPTER_READ_DATA_1;
    } else if (len == PCIE_VIRTUAL_ADAPTER_TWO_BYTE) {
        *data = PCIE_VIRTUAL_ADAPTER_READ_DATA_1;
        data++;
        *data = PCIE_VIRTUAL_ADAPTER_READ_DATA_2;
    } else {
        *data = PCIE_VIRTUAL_ADAPTER_READ_DATA_1;
        data++;
        *data = PCIE_VIRTUAL_ADAPTER_READ_DATA_2;
        data++;
        *data = PCIE_VIRTUAL_ADAPTER_READ_DATA_2;
        data++;
        *data = PCIE_VIRTUAL_ADAPTER_READ_DATA_3;
    }
    return HDF_SUCCESS;
}

static int32_t PcieVirtualAdapterWrite(struct PcieCntlr *cntlr, uint32_t pos, uint8_t *data, uint32_t len)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return HDF_SUCCESS;
}

static struct PcieCntlrOps g_pcieVirtualAdapterHostOps = {
    .read = PcieVirtualAdapterRead,
    .write = PcieVirtualAdapterWrite,
};

static int32_t PcieVirtualAdapterBind(struct HdfDeviceObject *obj)
{
    struct PcieVirtualAdapterHost *host = NULL;
    int32_t ret;

    if (obj == NULL) {
        HDF_LOGE("PcieVirtualAdapterBind: Fail, device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    host = (struct PcieVirtualAdapterHost *)OsalMemCalloc(sizeof(struct PcieVirtualAdapterHost));
    if (host == NULL) {
        HDF_LOGE("PcieVirtualAdapterBind: no mem for PcieAdapterHost.");
        return HDF_ERR_MALLOC_FAIL;
    }
    host->cntlr.ops = &g_pcieVirtualAdapterHostOps;
    host->cntlr.hdfDevObj = obj;
    obj->service = &(host->cntlr.service);

    ret = PcieCntlrParse(&(host->cntlr), obj);
    if (ret != HDF_SUCCESS) {
        goto ERR;
    }

    ret = PcieCntlrAdd(&(host->cntlr));
    if (ret != HDF_SUCCESS) {
        goto ERR;
    }

    HDF_LOGD("PcieVirtualAdapterBind: success.");
    return HDF_SUCCESS;
ERR:
    PcieCntlrRemove(&(host->cntlr));
    OsalMemFree(host);
    HDF_LOGD("PcieAdapterBind: fail, err = %d.", ret);
    return ret;
}

static int32_t PcieVirtualAdapterInit(struct HdfDeviceObject *obj)
{
    (void)obj;

    HDF_LOGD("PcieVirtualAdapterInit: success.");
    return HDF_SUCCESS;
}

static void PcieVirtualAdapterRelease(struct HdfDeviceObject *obj)
{
    struct PcieCntlr *cntlr = NULL;
    struct PcieVirtualAdapterHost *host = NULL;

    if (obj == NULL) {
        return;
    }

    cntlr = (struct PcieCntlr *)obj->service;
    if (cntlr == NULL) {
        return;
    }
    PcieCntlrRemove(cntlr);
    host = (struct PcieVirtualAdapterHost *)cntlr;
    OsalMemFree(host);
    HDF_LOGD("PcieAdapterRelease: success.");
}

struct HdfDriverEntry g_pcieVirtualDriverEntry = {
    .moduleVersion = 1,
    .Bind = PcieVirtualAdapterBind,
    .Init = PcieVirtualAdapterInit,
    .Release = PcieVirtualAdapterRelease,
    .moduleName = "PLATFORM_PCIE_VIRTUAL",
};
HDF_INIT(g_pcieVirtualDriverEntry);
