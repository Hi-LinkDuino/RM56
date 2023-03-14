/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PWM_TEST_H
#define PWM_TEST_H

#include "pwm_if.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t PwmTestExecute(int cmd);

enum PwmTestCmd {
    PWM_SET_PERIOD_TEST = 0,
    PWM_SET_DUTY_TEST,
    PWM_SET_POLARITY_TEST,
    PWM_ENABLE_TEST,
    PWM_DISABLE_TEST,
    PWM_SET_GET_CONFIG_TEST,
    PWM_RELIABILITY_TEST,
    PWM_IF_PERFORMANCE_TEST,
    PWM_TEST_CMD_MAX,
};

struct PwmTestConfig {
    uint32_t num;
    struct PwmConfig cfg;
};

struct PwmTester {
    struct PwmTestConfig config;
    struct PwmConfig originCfg;
    DevHandle handle;
};

#ifdef __cplusplus
}
#endif

#endif /* PWM_TEST_H */
