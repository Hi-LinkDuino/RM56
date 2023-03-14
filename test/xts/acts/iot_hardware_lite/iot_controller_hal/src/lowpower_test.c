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
#include "lowpower.h"

unsigned int (*g_lpcInit)(void);
unsigned int (*g_lpcSetType)(LpcType type);

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is wifiiot
 * @param        : module name is wifiiotlite
 * @param        : test suit name is UtilsFileFuncTestSuite
 */
LITE_TEST_SUIT(wifiiot, wifiiotlite, IotLowPowerTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL IotLowPowerTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL IotLowPowerTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_7000
 * @tc.name      : UART operation with LpcInit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotLowPowerTestSuite, testIotLowPower001, Function | MediumTest | Level1)
{
    g_lpcInit =LpcInit;
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_7100
 * @tc.name      : UART operation with IoTWatchDogKick
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotLowPowerTestSuite, testIotLowPower002, Function | MediumTest | Level1)
{
    g_lpcSetType = LpcSetType;
};

RUN_TEST_SUITE(IotLowPowerTestSuite);

