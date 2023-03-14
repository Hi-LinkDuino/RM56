/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DAC_TEST_H
#define DAC_TEST_H

#include "dac_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct DacTestConfig {
    uint32_t devNum;
    uint32_t channel;
    uint32_t maxChannel;
    uint32_t dataWidth;
    uint32_t rate;
};

struct DacTester {
    struct DacTestConfig config;
    DevHandle handle;
};

enum DacTestCmd {
    DAC_TEST_CMD_WRITE = 0,
    DAC_TEST_CMD_MULTI_THREAD,
    DAC_TEST_CMD_RELIABILITY,
    DAC_TEST_CMD_IF_PERFORMANCE,
    DAC_TEST_CMD_MAX,
};

int32_t DacTestExecute(int cmd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DAC_TEST_H */
