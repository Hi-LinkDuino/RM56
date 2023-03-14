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
#include "iot_watchdog.h"

void (*g_watchDogfunc)(void);

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is wifiiot
 * @param        : module name is wifiiotlite
 * @param        : test suit name is UtilsFileFuncTestSuite
 */
LITE_TEST_SUIT(wifiiot, wifiiotlite, IotWatchDogTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL IotWatchDogTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL IotWatchDogTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_6000
 * @tc.name      : UART operation with IoTWatchDogEnable
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotWatchDogTestSuite, testIotWatchDog001, Function | MediumTest | Level1)
{
    g_watchDogfunc = IoTWatchDogEnable;
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_6100
 * @tc.name      : UART operation with IoTWatchDogKick
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotWatchDogTestSuite, testIotWatchDog002, Function | MediumTest | Level1)
{
    g_watchDogfunc = IoTWatchDogKick;
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_6200
 * @tc.name      : UART operation with IoTWatchDogDisable
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotWatchDogTestSuite, testIotWatchDog003, Function | MediumTest | Level1)
{
    g_watchDogfunc = IoTWatchDogDisable;
};

RUN_TEST_SUITE(IotWatchDogTestSuite);

