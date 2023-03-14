/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef __USER__
#include "mmc_emmc.h"
#endif
#include "emmc_if.h"
#ifdef __USER__
#include "hdf_io_service_if.h"
#endif
#include "hdf_log.h"
#include "securec.h"

#define HDF_LOG_TAG emmc_if_c

#ifdef __USER__
static int32_t EmmcGetCidReadReplyData(struct HdfSBuf *reply, uint8_t *cid, uint32_t size)
{
    uint32_t rLen;
    const void *rBuf = NULL;

    if (HdfSbufReadBuffer(reply, &rBuf, &rLen) == false) {
        HDF_LOGE("EmmcGetCidReadReplyData: read rBuf fail!");
        return HDF_ERR_IO;
    }
    if (size != rLen) {
        HDF_LOGE("EmmcGetCidReadReplyData: err len:%u, rLen:%u", size, rLen);
        if (rLen > size) {
            rLen = size;
        }
    }

    if (memcpy_s(cid, size, rBuf, rLen) != EOK) {
        HDF_LOGE("EmmcGetCidReadReplyData: memcpy rBuf fail!");
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

int32_t EmmcServiceGetCid(struct HdfIoService *service, uint8_t *cid, uint32_t size)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("EmmcServiceGetCid: failed to obtain reply!");
        ret = HDF_ERR_MALLOC_FAIL;
        goto __EXIT;
    }

    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("EmmcServiceGetCid: dispatcher or Dispatch is NULL!");
        ret = HDF_ERR_NOT_SUPPORT;
        goto __EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, EMMC_CMD_GET_CID, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EmmcServiceGetCid: failed to send service call:%d", ret);
        goto __EXIT;
    }

    ret = EmmcGetCidReadReplyData(reply, cid, size);
    if (ret != HDF_SUCCESS) {
        goto __EXIT;
    }
    HDF_LOGD("EmmcServiceGetCid: success");

__EXIT:
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
    return ret;
}

#else
static int32_t EmmcDeviceGetFromHandle(DevHandle handle, struct EmmcDevice **emmc)
{
    struct MmcDevice *mmc = NULL;

    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (emmc == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    mmc = MmcCntlrGetDevice((struct MmcCntlr *)handle);
    if (mmc == NULL) {
        return HDF_PLT_ERR_NO_DEV;
    }
    if (mmc->type != MMC_DEV_EMMC) {
        MmcDevicePut(mmc);
        return HDF_PLT_ERR_DEV_TYPE;
    }

    *emmc = (struct EmmcDevice *)mmc;
    return HDF_SUCCESS;
}

#endif

int32_t EmmcGetCid(DevHandle handle, uint8_t *cid, uint32_t size)
{
#ifndef __USER__
    struct EmmcDevice *emmc = NULL;
    int32_t ret;
#endif
    if (handle == NULL) {
        HDF_LOGE("EmmcGetCid: handle is NULL!");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cid == NULL || size < EMMC_CID_LEN) {
        HDF_LOGE("EmmcGetCid: error parms!");
        return HDF_ERR_INVALID_PARAM;
    }
#ifdef __USER__
    return EmmcServiceGetCid((struct HdfIoService *)handle, cid, size);
#else
    if (EmmcDeviceGetFromHandle(handle, &emmc) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = EmmcDeviceGetCid(emmc, cid, size);
    MmcDevicePut((struct MmcDevice *)emmc);
    return ret;
#endif
}

void EmmcGetHuid(uint8_t *cid, uint32_t size)
{
    DevHandle handle = NULL;
    if (cid == NULL || size == 0) {
        HDF_LOGE("EmmcGetUdid: error parms!");
        return;
    }
    if (memset_s(cid, sizeof(uint8_t) * size, 0, sizeof(uint8_t) * size) != EOK) {
        HDF_LOGE("EmmcGetUdid: memset_s fail!");
        return;
    }
    handle = EmmcOpen(0);
    if (handle == NULL) {
        HDF_LOGD("EmmcGetUdid: open fail, use default value!");
        return;
    }
    if (EmmcGetCid(handle, cid, size) != HDF_SUCCESS) {
        HDF_LOGD("EmmcGetUdid: get fail, use default value!");
    }
    EmmcClose(handle);
}
