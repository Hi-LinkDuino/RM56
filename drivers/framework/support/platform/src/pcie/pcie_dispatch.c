/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pcie_dispatch.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "pcie_core.h"
#include "pcie_if.h"

#define HDF_LOG_TAG pcie_dispatch_c

enum PcieIoCmd {
    PCIE_CMD_READ,
    PCIE_CMD_WRITE,
    PCIE_CMD_BUTT,
};

struct PcieDispatchFunc {
    uint32_t cmd;
    int32_t (*func)(struct PcieCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply);
};

static int32_t PcieCmdRead(struct PcieCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t len, pos;
    uint8_t *buf = NULL;
    int32_t ret;

    if (!HdfSbufReadUint32(data, &len)) {
        HDF_LOGE("PcieCmdRead: read len fail");
        return HDF_ERR_IO;
    }
    if (len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &pos)) {
        HDF_LOGE("PcieCmdRead: read pos fail");
        return HDF_ERR_IO;
    }

    buf = (uint8_t *)OsalMemCalloc(sizeof(*buf) * len);
    if (buf == NULL) {
        HDF_LOGE("PcieCmdRead: OsalMemCalloc error");
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = PcieCntlrRead(cntlr, pos, buf, len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("PcieCntlrRead: error, ret is %d", ret);
        goto EXIT;
    }
    if (!HdfSbufWriteBuffer(reply, buf, len)) {
        HDF_LOGE("PcieCntlrRead: sbuf write buffer failed");
        ret = HDF_ERR_IO;
        goto EXIT;
    }
    ret = HDF_SUCCESS;
EXIT:
    OsalMemFree(buf);
    return ret;
}

static int32_t PcieCmdWrite(struct PcieCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t size, pos;
    uint8_t *buf = NULL;
    (void)reply;

    if (!HdfSbufReadUint32(data, &pos)) {
        HDF_LOGE("PcieCmdWrite: read pos fail");
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadBuffer(data, (const void **)&buf, &size)) {
        HDF_LOGE("PcieCmdWrite: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return PcieCntlrWrite(cntlr, pos, buf, size);
}

int32_t PcieIoDispatch(struct HdfDeviceIoClient *client, int32_t cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct PcieCntlr *cntlr = NULL;
    uint32_t i;
    uint32_t len;
    struct PcieDispatchFunc dispatchFunc[] = {
        { PCIE_CMD_READ, PcieCmdRead },
        { PCIE_CMD_WRITE, PcieCmdWrite },
    };

    if (client == NULL || client->device == NULL) {
        HDF_LOGE("PcieIoDispatch: client or hdf dev obj is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct PcieCntlr *)client->device->service;
    if (cntlr == NULL) {
        HDF_LOGE("PcieIoDispatch: service is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }

    len = sizeof(dispatchFunc) / sizeof(dispatchFunc[0]);
    for (i = 0; i < len; i++) {
        if (dispatchFunc[i].cmd == cmd) {
            return dispatchFunc[i].func(cntlr, data, reply);
        }
    }

    HDF_LOGE("PcieIoDispatch: cmd %d is not support", cmd);
    return HDF_ERR_NOT_SUPPORT;
}
