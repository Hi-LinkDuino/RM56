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

#include "hctest.h"
#include "iot_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "securec.h"
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
LITE_TEST_SUIT(security, securityData, HksAesTest);

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
static BOOL HksAesTestSetUp()
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
    LiteTestPrint("HksAgreementTestSetUp End2!\n");
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksAesTestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
}

#define TEST_TASK_STACK_SIZE      0x2000
#define WAIT_TO_TEST_DONE         4

static osPriority_t g_setPriority;

static void ExecHksAesTest001(void const *argument)
{
    LiteTestPrint("HksAesTest001 Begin!\n");
    TEST_ASSERT_TRUE(TestAes256ByAgree() == 0);
    LiteTestPrint("HksAesTest001 End!\n");
    osThreadExit();
}

static void ExecHksAesTest002(void const *argument)
{
    LiteTestPrint("HksAesTest002 Begin!\n");
    TEST_ASSERT_TRUE(TestAes256ByAgree1() == 0);
    LiteTestPrint("HksAesTest002 End!\n");
    osThreadExit();
}

static void ExecHksAesTest003(void const *argument)
{
    LiteTestPrint("HksAesTest003 Begin!\n");
    TEST_ASSERT_TRUE(TestAes256ByAgree2() == 0);
    LiteTestPrint("HksAesTest003 End!\n");
    osThreadExit();
}

static void ExecHksAesTest004(void const *argument)
{
    LiteTestPrint("HksAesTest004 Begin!\n");
    TEST_ASSERT_EQUAL(TestAes256ByLocal(), 0);
    LiteTestPrint("HksAesTest004 End!\n");
    osThreadExit();
}

#ifndef _CUT_AUTHENTICATE_
#ifndef _CUT_ED25519_
/**
 * @tc.name: HksCipherTest.HksCipherTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAesTest, HksAesTest001, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksAesTest001, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksAesTest001 End2!\n");
}

/**
 * @tc.name: HksCipherTest.HksCipherTest002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAesTest, HksAesTest002, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksAesTest002, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksAesTest002 End2!\n");
}

/**
 * @tc.name: HksCipherTest.HksCipherTest003
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAesTest, HksAesTest003, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksAesTest003, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksAesTest003 End2!\n");
    
}
#endif
#endif /* _CUT_AUTHENTICATE_ */

/**
 * @tc.name: HksCipherTest.HksCipherTest004
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAesTest, HksAesTest004, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksAesTest004, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksAesTest004 End2!\n");
}

RUN_TEST_SUITE(HksAesTest);
