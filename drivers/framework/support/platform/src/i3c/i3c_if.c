/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "i3c_if.h"
#include "i3c_core.h"
#include "hdf_log.h"

#define HDF_LOG_TAG i3c_if_c

DevHandle I3cOpen(int16_t number)
{
    return (DevHandle)I3cCntlrGet(number);
}

void I3cClose(DevHandle handle)
{
    I3cCntlrPut((struct I3cCntlr *)handle);
}

int32_t I3cTransfer(DevHandle handle, struct I3cMsg *msgs, int16_t count, enum TransMode mode)
{
    int32_t ret = HDF_SUCCESS;

    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (msgs == NULL || count <= 0) {
        HDF_LOGE("I3cTransfer: err params! msgs:%s, count:%d",
            (msgs == NULL) ? "0" : "x", count);
        return HDF_ERR_INVALID_PARAM;
    }

    switch (mode) {
        case I2C_MODE:
            ret = I3cCntlrI2cTransfer((struct I3cCntlr *)handle, msgs, count);
            break;
        case I3C_MODE:
            ret = I3cCntlrTransfer((struct I3cCntlr *)handle, msgs, count);
            break;
        case CCC_CMD_MODE:
            ret = I3cCntlrSendCccCmd((struct I3cCntlr *)handle, msgs->ccc);
            break;
        default:
            break;
    }

    return ret;
}

int32_t I3cSetConfig(DevHandle handle, struct I3cConfig *config)
{
    return I3cCntlrSetConfig((struct I3cCntlr *)handle, config);
}

int32_t I3cGetConfig(DevHandle handle, struct I3cConfig *config)
{
    return I3cCntlrGetConfig((struct I3cCntlr *)handle, config);
}

int32_t I3cRequestIbi(DevHandle handle, uint16_t addr, I3cIbiFunc func, uint32_t payload)
{
    return I3cCntlrRequestIbi((struct I3cCntlr *)handle, addr, func, payload);
}

int32_t I3cFreeIbi(DevHandle handle, uint16_t addr)
{
    return I3cCntlrFreeIbi((struct I3cCntlr *)handle, addr);
}
