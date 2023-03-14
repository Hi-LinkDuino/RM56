/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "hctest.h"
#include "iot_errno.h"
#ifdef CONFIG_PWM_SUPPORT
#include "iot_pwm.h"
#endif

const unsigned int COMPILABILITY_TEST_PWM_PORT = 0xFFFFFFFF;

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is wifiiot
 * @param        : module name is wifiiotlite
 * @param        : test suit name is UtilsFileFuncTestSuite
 */
LITE_TEST_SUIT(wifiiot, wifiiotlite, IotPwmTestSuite);

#ifdef CONFIG_PWM_SUPPORT
/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL IotPwmTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL IotPwmTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_4000
 * @tc.name      : PWM operation with IoTPwmInit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotPwmTestSuite, testIotPwm001, Function | MediumTest | Level1)
{
    unsigned int ret;
    ret = IoTPwmInit(COMPILABILITY_TEST_PWM_PORT);
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_4100
 * @tc.name      : PWM operation with IoTPwmStart
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotPwmTestSuite, testIotPwm002, Function | MediumTest | Level1)
{
    unsigned int ret;
    unsigned short duty = 20;
    unsigned short freq = 2;

    ret = IoTPwmStart(COMPILABILITY_TEST_PWM_PORT, duty, freq);
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_4200
 * @tc.name      : PWM operation with IoTPwmStop
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotPwmTestSuite, testIotPwm003, Function | MediumTest | Level1)
{
    unsigned int ret;
    ret = IoTPwmStop(COMPILABILITY_TEST_PWM_PORT);
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_4300
 * @tc.name      : PWM operation with IoTPwmDeinit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotPwmTestSuite, testIotPwm004, Function | MediumTest | Level1)
{
    unsigned int ret;
    ret = IoTPwmDeinit(COMPILABILITY_TEST_PWM_PORT);
};
#endif

RUN_TEST_SUITE(IotPwmTestSuite);
