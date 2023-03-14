/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef I2C_TEST_H
#define I2C_TEST_H

#include "i2c_if.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t I2cTestExecute(int cmd);
void I2cTestExecuteAll(void);

enum I2cTestCmd {
    I2C_TEST_CMD_TRANSFER = 0,
    I2C_TEST_CMD_WRITE_READ = 1,
    I2C_TEST_CMD_MULTI_THREAD = 2,
    I2C_TEST_CMD_RELIABILITY = 3,
    I2C_TEST_CMD_PERFORMANCE = 4,
    I2C_TEST_CMD_SETUP_ALL = 5,
    I2C_TEST_CMD_TEARDOWN_ALL = 6,
    I2C_TEST_CMD_SETUP_SINGLE = 7,
    I2C_TEST_CMD_TEARDOWN_SINGLE = 8,
    I2C_TEST_CMD_MAX = 9,
};

struct I2cTestConfig {
    uint16_t busNum;
    uint16_t devAddr;
    uint16_t regAddr;
    uint16_t regLen;
    uint16_t bufSize;
};

struct I2cTester {
    struct I2cTestConfig config;
    DevHandle handle;
    uint16_t total;
    uint16_t fails;
};
#ifdef __cplusplus
}
#endif
#endif /* I2C_TEST_H */
