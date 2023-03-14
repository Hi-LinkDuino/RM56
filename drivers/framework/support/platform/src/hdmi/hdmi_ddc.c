/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdmi_core.h"

#define HDF_LOG_TAG hdmi_ddc_c

int32_t HdmiCntlrDdcTransfer(struct HdmiCntlr *cntlr, struct HdmiDdcCfg *ddcCfg)
{
    int32_t ret;

    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->ddcTransfer == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (ddcCfg == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    HdmiCntlrLock(cntlr);
    ret = cntlr->ops->ddcTransfer(cntlr, ddcCfg);
    HdmiCntlrUnlock(cntlr);
    return ret;
}

int32_t HdmiDdcTransfer(struct HdmiDdc *ddc, struct HdmiDdcCfg *cfg)
{
    int32_t ret;

    if (ddc == NULL || cfg == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (cfg->mode >= HDMI_DDC_MODE_BUTT) {
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalMutexLock(&(ddc->ddcMutex));
    ret = HdmiCntlrDdcTransfer(ddc->priv, cfg);
    (void)OsalMutexUnlock(&(ddc->ddcMutex));
    return ret;
}
