/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "hks_generate_random_test.h"
#include "hctest.h"
#include "iot_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"
#include "hks_type.h"

#include "cmsis_os2.h"
#include "ohos_types.h"

#define TEST_TASK_STACK_SIZE      0x2000
#define WAIT_TO_TEST_DONE         4

static osPriority_t g_setPriority;

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(security, securityData, HksGenerateRandomTest);

static void ExecHksInitialize(void const *argument)
{
    LiteTestPrint("HksInitialize Begin!\n");
    TEST_ASSERT_TRUE(HksInitialize() == 0);
    LiteTestPrint("HksInitialize End!\n");
    osThreadExit();
}

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksGenerateRandomTestSetUp()
{
    LiteTestPrint("setup\n");
    IoTWatchDogDisable();
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityAboveNormal6;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)ExecHksInitialize, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksGenerateRandomTestSetUp End2!\n");
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksGenerateRandomTestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
}

static const struct HksTestGenRandomParams g_testGenRandomParams[] = {
    /* normal case */
    { 0, HKS_SUCCESS, { true, HKS_MAX_RANDOM_LEN, true, HKS_MAX_RANDOM_LEN } },
};

static void ExecHksGenerateRandomTest001(void const *argument)
{
    int32_t ret;
    struct HksBlob *random = NULL;
    LiteTestPrint("HksGenerateRandomTest001 Begin!\n");  

    ret = TestConstructBlobOut(&random,
        g_testGenRandomParams[0].randomParams.blobExist,
        g_testGenRandomParams[0].randomParams.blobSize,
        g_testGenRandomParams[0].randomParams.blobDataExist,
        g_testGenRandomParams[0].randomParams.blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);

    ret = HksGenerateRandomRun(random, 1);
    if (ret != g_testGenRandomParams[0].expectResult) {
        HKS_TEST_LOG_I("HksGenerateRandomRun failed, ret[%u] = %d", g_testGenRandomParams[0].testId, ret);
    }
    TEST_ASSERT_TRUE(ret == g_testGenRandomParams[0].expectResult);

    TestFreeBlob(&random);
    TEST_ASSERT_TRUE(ret == 0);
    
    LiteTestPrint("HksGenerateRandomTest001 End!\n");
    osThreadExit();
}

/**
 * @tc.name: HksGenerateRandomTest.HksGenerateRandomTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksGenerateRandomTest, HksGenerateRandomTest001, Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityAboveNormal6;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)ExecHksGenerateRandomTest001, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksGenerateRandomTest001 End2!\n");
}

RUN_TEST_SUITE(HksGenerateRandomTest);
