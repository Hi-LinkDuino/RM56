/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "spi_if.h"
#include "devsvc_manager_clnt.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "spi_core.h"

#define HDF_LOG_TAG spi_if
#define HOST_NAME_LEN 32

struct SpiObject {
    struct SpiCntlr *cntlr;
    uint32_t csNum;
};

static struct SpiCntlr *SpiGetCntlrByBusNum(uint32_t num)
{
    int ret;
    char *name = NULL;
    struct SpiCntlr *cntlr = NULL;

    name = (char *)OsalMemCalloc(HOST_NAME_LEN + 1);
    if (name == NULL) {
        return NULL;
    }
    ret = snprintf_s(name, HOST_NAME_LEN + 1, HOST_NAME_LEN, "HDF_PLATFORM_SPI_%u", num);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        OsalMemFree(name);
        return NULL;
    }
    cntlr = (struct SpiCntlr *)DevSvcManagerClntGetService(name);
    OsalMemFree(name);
    return cntlr;
}

int32_t SpiTransfer(DevHandle handle, struct SpiMsg *msgs, uint32_t count)
{
    struct SpiObject *obj = NULL;

    if (handle == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    obj = (struct SpiObject *)handle;
    return SpiCntlrTransfer(obj->cntlr, obj->csNum, msgs, count);
}

int32_t SpiRead(DevHandle handle, uint8_t *buf, uint32_t len)
{
    struct SpiMsg msg = {0};

    msg.wbuf = NULL;
    msg.rbuf = buf;
    msg.len = len;
    return SpiTransfer(handle, &msg, 1);
}

int32_t SpiWrite(DevHandle handle, uint8_t *buf, uint32_t len)
{
    struct SpiMsg msg = {0};

    msg.wbuf = buf;
    msg.rbuf = NULL;
    msg.len = len;
    return SpiTransfer(handle, &msg, 1);
}

int32_t SpiSetCfg(DevHandle handle, struct SpiCfg *cfg)
{
    struct SpiObject *obj = NULL;

    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    obj = (struct SpiObject *)handle;
    return SpiCntlrSetCfg(obj->cntlr, obj->csNum, cfg);
}

int32_t SpiGetCfg(DevHandle handle, struct SpiCfg *cfg)
{
    struct SpiObject *obj = NULL;

    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    obj = (struct SpiObject *)handle;
    return SpiCntlrGetCfg(obj->cntlr, obj->csNum, cfg);
}

void SpiClose(DevHandle handle)
{
    int32_t ret;
    struct SpiObject *obj = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return;
    }

    obj = (struct SpiObject *)handle;
    ret = SpiCntlrClose(obj->cntlr, obj->csNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error, ret is %d", __func__, ret);
    }
    OsalMemFree(handle);
}

DevHandle SpiOpen(const struct SpiDevInfo *info)
{
    int32_t ret;
    struct SpiObject *object = NULL;
    struct SpiCntlr *cntlr = NULL;

    if (info == NULL) {
        return NULL;
    }
    cntlr = SpiGetCntlrByBusNum(info->busNum);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return NULL;
    }

    object = (struct SpiObject *)OsalMemCalloc(sizeof(*object));
    if (object == NULL) {
        HDF_LOGE("%s: object malloc error", __func__);
        return NULL;
    }

    ret = SpiCntlrOpen(cntlr, info->csNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SpiCntlrOpen error, ret is %d", __func__, ret);
        OsalMemFree(object);
        return NULL;
    }

    object->cntlr = cntlr;
    object->csNum = info->csNum;
    return (DevHandle)object;
}
