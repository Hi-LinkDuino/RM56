/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef INPUT_I2C_OPS_H
#define INPUT_I2C_OPS_H

#include "hdf_types.h"
#include "i2c_if.h"

typedef struct {
    uint16_t busNum;
    uint16_t addr;
} I2cConfig;

typedef struct {
    struct DevHandle *i2cHandle;
    I2cConfig i2cCfg;
} InputI2cClient;

int InputI2cRead(const InputI2cClient *client, uint8_t *writeBuf, uint32_t writeLen, uint8_t *readBuf,
    uint32_t readLen);
int InputI2cWrite(const InputI2cClient *client, uint8_t *writeBuf, uint32_t len);

#endif
