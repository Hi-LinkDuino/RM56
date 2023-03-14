/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mipi_dsi_if.h"
#include "mipi_dsi_core.h"

#define HDF_LOG_TAG mipi_dsi_if

DevHandle MipiDsiOpen(uint8_t id)
{
    return (DevHandle)MipiDsiCntlrOpen(id);
}

void MipiDsiClose(DevHandle handle)
{
    MipiDsiCntlrClose((struct MipiDsiCntlr *)handle);
}

int32_t MipiDsiSetCfg(DevHandle handle, struct MipiCfg *cfg)
{
    return MipiDsiCntlrSetCfg((struct MipiDsiCntlr *)handle, cfg);
}

int32_t MipiDsiGetCfg(DevHandle handle, struct MipiCfg *cfg)
{
    return MipiDsiCntlrGetCfg((struct MipiDsiCntlr *)handle, cfg);
}

void MipiDsiSetLpMode(DevHandle handle)
{
    MipiDsiCntlrSetLpMode((struct MipiDsiCntlr *)handle);
}

void MipiDsiSetHsMode(DevHandle handle)
{
    MipiDsiCntlrSetHsMode((struct MipiDsiCntlr *)handle);
}

int32_t MipiDsiTx(DevHandle handle, struct DsiCmdDesc *cmd)
{
    return MipiDsiCntlrTx((struct MipiDsiCntlr *)handle, cmd);
}

int32_t MipiDsiRx(DevHandle handle, struct DsiCmdDesc *cmd, int32_t readLen, uint8_t *out)
{
    return MipiDsiCntlrRx((struct MipiDsiCntlr *)handle, cmd, readLen, out);
}

int32_t MipiDsiAttach(DevHandle handle, uint8_t *name)
{
    return MipiDsiCntlrAttach((struct MipiDsiCntlr *)handle, name);
}

int32_t MipiDsiSetDrvData(DevHandle handle, void *panelData)
{
    return MipiDsiCntlrSetDrvData((struct MipiDsiCntlr *)handle, panelData);
}
