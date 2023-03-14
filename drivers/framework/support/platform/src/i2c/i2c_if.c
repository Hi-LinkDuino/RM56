/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "i2c_if.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "i2c_core.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG i2c_if

#define I2C_SERVICE_NAME "HDF_PLATFORM_I2C_MANAGER"

DevHandle I2cOpen(int16_t number)
{
    return (DevHandle)I2cCntlrGet(number);
}

void I2cClose(DevHandle handle)
{
    if (handle != NULL) {
        I2cCntlrPut((struct I2cCntlr *)handle);
    }
}

int32_t I2cTransfer(DevHandle handle, struct I2cMsg *msgs, int16_t count)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (msgs == NULL || count <= 0) {
        HDF_LOGE("I2cTransfer: err params! msgs:%s, count:%d",
            (msgs == NULL) ? "0" : "x", count);
        return HDF_ERR_INVALID_PARAM;
    }

    return I2cCntlrTransfer((struct I2cCntlr *)handle, msgs, count);
}

