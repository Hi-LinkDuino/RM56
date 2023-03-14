/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pcie_if.h"
#ifndef __USER__
#include "devsvc_manager_clnt.h"
#include "pcie_core.h"
#endif
#include "hdf_base.h"
#include "hdf_log.h"
#ifdef __USER__
#include "hdf_io_service_if.h"
#endif
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG pcie_if_c

#define PCIE_SERVICE_NAME_LEN 32

#ifdef __USER__
enum PcieIoCmd {
    PCIE_CMD_READ,
    PCIE_CMD_WRITE,
    PCIE_CMD_BUTT,
};

static int32_t PcieGetDataFromReply(struct HdfSBuf *reply, uint8_t *data, uint32_t size)
{
    uint32_t rLen;
    const void *rBuf = NULL;

    if (HdfSbufReadBuffer(reply, &rBuf, &rLen) == false) {
        HDF_LOGE("PcieGetDataFromReply: read rBuf fail!");
        return HDF_ERR_IO;
    }
    if (size != rLen) {
        HDF_LOGE("PcieGetDataFromReply: err len:%u, rLen:%u", size, rLen);
        if (rLen > size) {
            rLen = size;
        }
    }

    if (memcpy_s(data, size, rBuf, rLen) != EOK) {
        HDF_LOGE("PcieGetDataFromReply: memcpy rBuf fail!");
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t PcieUserRead(DevHandle handle, uint32_t pos, uint8_t *data, uint32_t len)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("PcieUserRead: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    if (data == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    buf = HdfSbufObtainDefaultSize();
    if (buf == NULL) {
        HDF_LOGE("PcieUserRead: failed to obtain buf");
        ret = HDF_ERR_MALLOC_FAIL;
        goto EXIT;
    }
    if (!HdfSbufWriteUint32(buf, len)) {
        HDF_LOGE("PcieUserRead: sbuf write uint32 failed");
        ret = HDF_ERR_IO;
        goto EXIT;
    }
    if (!HdfSbufWriteUint32(buf, pos)) {
        HDF_LOGE("PcieUserRead: sbuf write uint32 failed");
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("PcieUserRead: failed to obtain reply");
        ret = HDF_ERR_MALLOC_FAIL;
        goto EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, PCIE_CMD_READ, buf, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("PcieUserRead: failed to write, ret %d", ret);
    } else {
        ret = PcieGetDataFromReply(reply, data, len);
    }

EXIT :
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
    if (buf != NULL) {
        HdfSbufRecycle(buf);
    }
    return ret;
}

static int32_t PcieUserWrite(DevHandle handle, uint32_t pos, uint8_t *data, uint32_t len)
{
    int32_t ret;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("PcieUserWrite: service is invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    buf = HdfSbufObtainDefaultSize();
    if (buf == NULL) {
        HDF_LOGE("PcieUserWrite: failed to obtain buf");
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteUint32(buf, pos)) {
        HDF_LOGE("PcieUserWrite: sbuf write uint32 failed");
        ret = HDF_ERR_IO;
        goto EXIT;
    }
    if (!HdfSbufWriteBuffer(buf, data, len)) {
        HDF_LOGE("PcieUserWrite: sbuf write buffer failed");
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, PCIE_CMD_WRITE, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("PcieUserWrite: failed to write, ret %d", ret);
    }
EXIT:
    HdfSbufRecycle(buf);
    return ret;
}
#endif

static void *PcieCntlrObjGet(uint16_t busNum)
{
    char *serviceName = NULL;
    void *obj = NULL;

    serviceName = (char *)OsalMemCalloc(PCIE_SERVICE_NAME_LEN + 1);
    if (serviceName == NULL) {
        HDF_LOGE("HDMI service name malloc fail.");
        return NULL;
    }
    if (snprintf_s(serviceName, (PCIE_SERVICE_NAME_LEN + 1),
        PCIE_SERVICE_NAME_LEN, "HDF_PLATFORM_PCIE_%hu", busNum) < 0) {
        HDF_LOGE("get PCIE service name fail.");
        goto EXIT;
    }
#ifdef __USER__
    obj = (void *)HdfIoServiceBind(serviceName);
#else
    obj = (void *)PcieCntlrGetByBusNum(busNum);
#endif
EXIT:
    OsalMemFree(serviceName);
    return obj;
}

DevHandle PcieOpen(uint16_t busNum)
{
    return (DevHandle)PcieCntlrObjGet(busNum);
}

int32_t PcieRead(DevHandle handle, uint32_t pos, uint8_t *data, uint32_t len)
{
#ifdef __USER__
    return PcieUserRead(handle, pos, data, len);
#else
    return PcieCntlrRead((struct PcieCntlr *)handle, pos, data, len);
#endif
}

int32_t PcieWrite(DevHandle handle, uint32_t pos, uint8_t *data, uint32_t len)
{
#ifdef __USER__
    return PcieUserWrite(handle, pos, data, len);
#else
    return PcieCntlrWrite((struct PcieCntlr *)handle, pos, data, len);
#endif
}

void PcieClose(DevHandle handle)
{
    if (handle != NULL) {
#ifdef __USER__
        HdfIoServiceRecycle((struct HdfIoService *)handle);
#endif
    }
}
