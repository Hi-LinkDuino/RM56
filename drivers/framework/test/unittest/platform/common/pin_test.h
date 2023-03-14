/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PIN_TEST_H
#define PIN_TEST_H

#include "pin_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct PinTestConfig {
    const char *pinName;
    char funcNameBuf[30];
    const char *funcName;
    enum PinPullType PullTypeNum;
    uint32_t strengthNum;
};

struct PinTester {
    struct PinTestConfig config;
    DevHandle handle;
    uint16_t total;
    uint16_t fails;
};

enum PinTestCmd {
    PIN_TEST_CMD_SETGETPULL = 0,
    PIN_TEST_CMD_SETGETSTRENGTH = 1,
    PIN_TEST_CMD_SETGETFUNC = 2,
    PIN_TEST_CMD_RELIABILITY = 3,
    PIN_TEST_CMD_SETUP_ALL = 4,
    PIN_TEST_CMD_TEARDOWN_ALL = 5,
    PIN_TEST_CMD_SETUP_SINGLE = 6,
    PIN_TEST_CMD_TEARDOWN_SINGLE = 7,
    PIN_TEST_CMD_MAX = 8,
};

int32_t PinTestExecute(int cmd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PIN_TEST_H */
