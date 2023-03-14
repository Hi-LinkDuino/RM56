/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef ADC_TEST_H
#define ADC_TEST_H

#include "adc_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct AdcTestConfig {
    uint32_t devNum;
    uint32_t channel;
    uint32_t maxChannel;
    uint32_t dataWidth;
    uint32_t rate;
};

struct AdcTester {
    struct AdcTestConfig config;
    DevHandle handle;
};

enum AdcTestCmd {
    ADC_TEST_CMD_READ = 0,
    ADC_TEST_CMD_MULTI_THREAD,
    ADC_TEST_CMD_RELIABILITY,
    ADC_IF_PERFORMANCE_TEST,
    ADC_TEST_CMD_MAX,
};

int32_t AdcTestExecute(int cmd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* ADC_TEST_H */
