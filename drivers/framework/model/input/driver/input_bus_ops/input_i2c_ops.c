/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "hdf_log.h"
#include "input_i2c_ops.h"

#define I2C_READ_MSG_NUM   2
#define I2C_WRITE_MSG_NUM  1

int32_t InputI2cRead(const InputI2cClient *client, uint8_t *writeBuf, uint32_t writeLen, uint8_t *readBuf,
    uint32_t readLen)
{
    struct I2cMsg msg[I2C_READ_MSG_NUM];
    (void)memset_s(msg, sizeof(msg), 0, sizeof(msg));

    msg[0].addr = client->i2cCfg.addr;
    msg[0].flags = 0;
    msg[0].len = writeLen;
    msg[0].buf = writeBuf;

    msg[1].addr = client->i2cCfg.addr;
    msg[1].flags = I2C_FLAG_READ;
    msg[1].len = readLen;
    msg[1].buf = readBuf;

    if (I2cTransfer(client->i2cHandle, msg, I2C_READ_MSG_NUM) != I2C_READ_MSG_NUM) {
        HDF_LOGE("%s: i2c read err", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InputI2cWrite(const InputI2cClient *client, uint8_t *writeBuf, uint32_t len)
{
    struct I2cMsg msg[I2C_WRITE_MSG_NUM];
    (void)memset_s(msg, sizeof(msg), 0, sizeof(msg));

    msg[0].addr = client->i2cCfg.addr;
    msg[0].flags = 0;
    msg[0].len = len;
    msg[0].buf = writeBuf;

    if (I2cTransfer(client->i2cHandle, msg, I2C_WRITE_MSG_NUM) != I2C_WRITE_MSG_NUM) {
        HDF_LOGE("%s: i2c write err", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
