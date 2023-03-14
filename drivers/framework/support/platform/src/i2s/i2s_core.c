/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "i2s_core.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG i2s_core

int32_t I2sCntlrOpen(struct I2sCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->Open == NULL) {
        HDF_LOGE("%s: Open not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->Open(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t I2sCntlrClose(struct I2sCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->Close == NULL) {
        HDF_LOGE("%s: Open not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->Close(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t I2sCntlrEnable(struct I2sCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->Enable == NULL) {
        HDF_LOGE("%s: Open not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->Enable(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t I2sCntlrDisable(struct I2sCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->Disable == NULL) {
        HDF_LOGE("%s: Open not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->Disable(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t I2sCntlrStartRead(struct I2sCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->StartRead == NULL) {
        HDF_LOGE("%s: Open not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->StartRead(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}


int32_t I2sCntlrStopRead(struct I2sCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->StopRead == NULL) {
        HDF_LOGE("%s: Open not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->StopRead(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t I2sCntlrStartWrite(struct I2sCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->StartWrite == NULL) {
        HDF_LOGE("%s: Open not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->StartWrite(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t I2sCntlrStopWrite(struct I2sCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->StopWrite == NULL) {
        HDF_LOGE("%s: Open not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->StopWrite(cntlr);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t I2sCntlrSetCfg(struct I2sCntlr *cntlr, struct I2sCfg *cfg)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr->method == NULL || cntlr->method->SetCfg == NULL) {
        HDF_LOGE("%s: not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->SetCfg(cntlr, cfg);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t I2sCntlrGetCfg(struct I2sCntlr *cntlr, struct I2sCfg *cfg)
{
    int32_t ret;

    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr->method == NULL || cntlr->method->GetCfg == NULL) {
        HDF_LOGE("%s: not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->GetCfg(cntlr, cfg);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

int32_t I2sCntlrTransfer(struct I2sCntlr *cntlr, struct I2sMsg *msg)
{
    int32_t ret;

    if (cntlr == NULL || msg == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->method == NULL || cntlr->method->Transfer == NULL) {
        HDF_LOGE("%s: transfer not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    (void)OsalMutexLock(&(cntlr->lock));
    ret = cntlr->method->Transfer(cntlr, msg);
    (void)OsalMutexUnlock(&(cntlr->lock));
    return ret;
}

struct I2sCntlr *I2sCntlrCreate(struct HdfDeviceObject *device)
{
    struct I2sCntlr *cntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return NULL;
    }

    cntlr = (struct I2sCntlr *)OsalMemCalloc(sizeof(*cntlr));
    if (cntlr == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return NULL;
    }
    cntlr->device = device;
    device->service = &(cntlr->service);
    (void)OsalMutexInit(&cntlr->lock);
    cntlr->priv = NULL;
    cntlr->method = NULL;
    return cntlr;
}

void I2sCntlrDestroy(struct I2sCntlr *cntlr)
{
    if (cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return;
    }

    (void)OsalMutexDestroy(&(cntlr->lock));
    cntlr->device = NULL;
    cntlr->method = NULL;
    cntlr->priv = NULL;
    OsalMemFree(cntlr);
}
