/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef I3C_TEST_H
#define I3C_TEST_H

#include "i3c_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct I3cTestConfig {
    uint16_t busId;
    uint16_t devAddr;
    uint16_t regAddr;
    uint16_t regLen;
    uint16_t bufSize;
};

struct I3cTester {
    struct I3cTestConfig config;
    DevHandle handle;
    uint16_t total;
    uint16_t fails;
};

enum I3cTestCmd {
    I3C_TEST_CMD_TRANSFER = 0,
    I3C_TEST_CMD_SET_CONFIG,
    I3C_TEST_CMD_GET_CONFIG,
    I3C_TEST_CMD_REQUEST_IBI,
    I3C_TEST_CMD_FREE_IBI,
    I3C_TEST_CMD_MULTI_THREAD,
    I3C_TEST_CMD_RELIABILITY,
    I3C_TEST_CMD_SETUP_ALL,
    I3C_TEST_CMD_TEARDOWN_ALL,
    I3C_TEST_CMD_SETUP_SINGLE,
    I3C_TEST_CMD_TEARDOWN_SINGLE,
    I3C_TEST_CMD_MAX,
};

int32_t I3cTestExecute(int cmd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* I3C_TEST_H */
