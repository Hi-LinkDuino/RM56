/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "i2s_if.h"
#include "los_vm_zone.h"
#include "devsvc_manager_clnt.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "i2s_core.h"

#define HDF_LOG_TAG i2s_if
#define HOST_NAME_LEN 32

static struct I2sCntlr *I2sGetCntlrByBusNum(uint32_t num)
{
    int ret;
    char *name = NULL;
    struct I2sCntlr *cntlr = NULL;

    name = (char *)OsalMemCalloc(HOST_NAME_LEN + 1);
    if (name == NULL) {
        return NULL;
    }
    ret = snprintf_s(name, HOST_NAME_LEN + 1, HOST_NAME_LEN, "HDF_PLATFORM_I2S_%u", num);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        OsalMemFree(name);
        return NULL;
    }
    cntlr = (struct I2sCntlr *)DevSvcManagerClntGetService(name);
    OsalMemFree(name);
    return cntlr;
}

void I2sEnable(DevHandle handle)
{
    struct I2sCntlr *cntlr = (struct I2sCntlr *)handle;
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return;
    }

    int ret = I2sCntlrEnable(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sCntlrEnable fail", __func__);
        return;
    }
}

void I2sDisable(DevHandle handle)
{
    struct I2sCntlr *cntlr = (struct I2sCntlr *)handle;
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return;
    }

    int ret = I2sCntlrDisable(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sCntlrDisable fail", __func__);
        return;
    }
}

void I2sStartWrite(DevHandle handle)
{
    struct I2sCntlr *cntlr = (struct I2sCntlr *)handle;
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return;
    }

    int ret = I2sCntlrStartWrite(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sCntlrStartWrite fail", __func__);
    }
}

void I2sStopWrite(DevHandle handle)
{
    struct I2sCntlr *cntlr = (struct I2sCntlr *)handle;
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return;
    }

    int ret = I2sCntlrStopWrite(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sCntlrStopWrite fail", __func__);
    }
}

void I2sStartRead(DevHandle handle)
{
    struct I2sCntlr *cntlr = (struct I2sCntlr *)handle;
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return;
    }

    int ret = I2sCntlrStartRead(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sStartRead fail", __func__);
    }
}

void I2sStopRead(DevHandle handle)
{
    struct I2sCntlr *cntlr = (struct I2sCntlr *)handle;
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return;
    }

    int ret = I2sCntlrStopRead(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sCntlrStopWrite fail", __func__);
    }
}

int32_t I2sWrite(DevHandle handle, uint8_t *buf, uint32_t len, uint32_t *pWlen)
{
    struct I2sMsg msg = {0};
    if (pWlen == NULL) {
        HDF_LOGE("%s: pWlen is null", __func__);
        return HDF_FAILURE;
    }
    *pWlen = 0;
    msg.wbuf = buf;
    msg.rbuf = NULL;
    msg.len = len;
    msg.pRlen = pWlen;
    do {
        OsalMSleep(I2S_DATA_TRANSFER_PERIOD);
        int ret = I2sCntlrTransfer((struct I2sCntlr *)handle, &msg);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: I2sCntlrTransfer fail", __func__);
        }
    } while (*pWlen == 0);

    return HDF_SUCCESS;
}

int32_t I2sRead(DevHandle handle, uint8_t *buf, uint32_t len, uint32_t *pRlen)
{
    struct I2sMsg msg = {0};
    if (pRlen == NULL) {
        HDF_LOGE("%s: pRlen is null", __func__);
        return HDF_FAILURE;
    }
    *pRlen = 0;
    msg.wbuf = NULL;
    msg.rbuf = buf;
    msg.len = len;
    msg.pRlen = pRlen;
    do {
        OsalMSleep(I2S_DATA_TRANSFER_PERIOD);
        int ret = I2sCntlrTransfer((struct I2sCntlr *)handle, &msg);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: I2sCntlrTransfer fail", __func__);
        }
    } while (*pRlen == 0);

    return HDF_SUCCESS;
}

DevHandle I2sOpen(int16_t number)
{
    struct I2sCntlr *cntlr = NULL;

    cntlr = I2sGetCntlrByBusNum(number);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return NULL;
    }

    int ret = I2sCntlrOpen(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sCntlrOpen fail", __func__);
        return NULL;
    }

    return (DevHandle)cntlr;
}

void I2sClose(DevHandle handle)
{
    struct I2sCntlr *cntlr = (struct I2sCntlr *)handle;
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return;
    }

    int ret = I2sCntlrClose(cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sClose fail", __func__);
    }
}

void I2sSetCfg(DevHandle handle, struct I2sCfg *cfg)
{
    if (handle == NULL || cfg == NULL) {
        HDF_LOGE("%s: cntlr or cfg is null", __func__);
        return;
    }

    struct I2sCntlr *cntlr = (struct I2sCntlr *)handle;

    int ret = I2sCntlrSetCfg(cntlr, cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sCntlrSetCfg fail", __func__);
    }
}
void I2sGetCfg(DevHandle handle, struct I2sCfg *cfg)
{
    struct I2sCntlr *cntlr = (struct I2sCntlr *)handle;
    if (cntlr == NULL || cfg == NULL) {
        HDF_LOGE("%s: cntlr or cfg is null", __func__);
        return;
    }

    int ret = I2sCntlrGetCfg(cntlr, cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: I2sCntlrSetCfg fail", __func__);
    }
}
