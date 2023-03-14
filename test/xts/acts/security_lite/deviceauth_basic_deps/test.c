/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hctest.h>
#include <ohos_types.h>
#include <securec.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hc_alg_test.h"
#include "hc_condition_test.h"
#include "hc_dev_info_test.h"
#include "hc_file_f_test.h"
#include "hc_file_iot_flash_test.h"
#include "hc_file_test.h"
#include "hc_file_utils_test.h"
#include "hc_mutex_test.h"
#include "hc_thread_test.h"
#include "hc_time_test.h"
#include "print_log.h"

LITE_TEST_SUIT(security, deviceAuth, DeviceAuthBasicDepsTestSuite)

static BOOL DeviceAuthBasicDepsTestSuiteSetUp(void)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    (void)(srand(time(NULL)));
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    return TRUE;
}

static BOOL DeviceAuthBasicDepsTestSuiteTearDown(void)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    return TRUE;
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseHcMutex
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseHcMutex, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcMutex();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseHcThread
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseHcThread, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcThread();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseHcGetUdid
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseHcGetUdid, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcGetUdid();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseHcFile
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseHcFile, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcFile();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseHcFileFApi
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseHcFileFApi, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcFileFApi();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseHcFileUtilsFile
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseHcFileUtilsFile, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcFileUtilsFile();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseSemCondition
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseSemCondition, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcSemCondition();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCasePthreadCondition
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCasePthreadCondition, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcPthreadCondition();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseHcFileIotFlash
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseHcFileIotFlash, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcFileIotFlash();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseHcTime
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseHcTime, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcTime();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

/**
 * @tc.name: DeviceAuthBasicDepsTestSuite.TestCaseHcAlg
 * @tc.desc:
 * @tc.type: FUNC
 */
LITE_TEST_CASE(DeviceAuthBasicDepsTestSuite, TestCaseHcAlg, Function | SmallTest | Level1)
{
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
    TestHcAlg();
    LOGI("++++++++++++++++++++++++++++++++++++++++\n");
}

RUN_TEST_SUITE(DeviceAuthBasicDepsTestSuite)
