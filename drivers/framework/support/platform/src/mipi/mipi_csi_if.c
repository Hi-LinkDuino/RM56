/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mipi_csi_if.h"
#include "mipi_csi_core.h"

#define HDF_LOG_TAG mipi_csi_if

DevHandle MipiCsiOpen(uint8_t id)
{
    return (DevHandle)MipiCsiCntlrGet(id);
}

void MipiCsiClose(DevHandle handle)
{
    MipiCsiCntlrPut((struct MipiCsiCntlr *)handle);
}

int32_t MipiCsiSetComboDevAttr(DevHandle handle, ComboDevAttr *pAttr)
{
    return MipiCsiCntlrSetComboDevAttr((struct MipiCsiCntlr *)handle, pAttr);
}

int32_t MipiCsiSetPhyCmvmode(DevHandle handle, uint8_t devno, PhyCmvMode cmvMode)
{
    return MipiCsiCntlrSetPhyCmvmode((struct MipiCsiCntlr *)handle, devno, cmvMode);
}

int32_t MipiCsiSetExtDataType(DevHandle handle, ExtDataType* dataType)
{
    return MipiCsiCntlrSetExtDataType((struct MipiCsiCntlr *)handle, dataType);
}

int32_t MipiCsiSetHsMode(DevHandle handle, LaneDivideMode laneDivideMode)
{
    return MipiCsiCntlrSetHsMode((struct MipiCsiCntlr *)handle, laneDivideMode);
}

int32_t MipiCsiEnableClock(DevHandle handle, uint8_t comboDev)
{
    return MipiCsiCntlrEnableClock((struct MipiCsiCntlr *)handle, comboDev);
}

int32_t MipiCsiDisableClock(DevHandle handle, uint8_t comboDev)
{
    return MipiCsiCntlrDisableClock((struct MipiCsiCntlr *)handle, comboDev);
}

int32_t MipiCsiResetRx(DevHandle handle, uint8_t comboDev)
{
    return MipiCsiCntlrResetRx((struct MipiCsiCntlr *)handle, comboDev);
}

int32_t MipiCsiUnresetRx(DevHandle handle, uint8_t comboDev)
{
    return MipiCsiCntlrUnresetRx((struct MipiCsiCntlr *)handle, comboDev);
}

int32_t MipiCsiEnableSensorClock(DevHandle handle, uint8_t snsClkSource)
{
    return MipiCsiCntlrEnableSensorClock((struct MipiCsiCntlr *)handle, snsClkSource);
}

int32_t MipiCsiDisableSensorClock(DevHandle handle, uint8_t snsClkSource)
{
    return MipiCsiCntlrDisableSensorClock((struct MipiCsiCntlr *)handle, snsClkSource);
}

int32_t MipiCsiResetSensor(DevHandle handle, uint8_t snsResetSource)
{
    return MipiCsiCntlrResetSensor((struct MipiCsiCntlr *)handle, snsResetSource);
}

int32_t MipiCsiUnresetSensor(DevHandle handle, uint8_t snsResetSource)
{
    return MipiCsiCntlrUnresetSensor((struct MipiCsiCntlr *)handle, snsResetSource);
}

int32_t MipiCsiSetDrvData(DevHandle handle, void *drvData)
{
    return MipiCsiCntlrSetDrvData((struct MipiCsiCntlr *)handle, drvData);
}
