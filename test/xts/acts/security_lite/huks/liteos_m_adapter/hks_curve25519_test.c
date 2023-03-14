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

#ifndef _CUT_AUTHENTICATE_
#ifndef _CUT_ED25519_
#include "hks_curve25519_test.h"

#include "cmsis_os2.h"
#include "hctest.h"
#include "iot_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_curve25519.h"
#include "los_config.h"
#include "ohos_types.h"
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
LITE_TEST_SUIT(security, securityData, HksCurve25519Test);

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
static BOOL HksCurve25519TestSetUp()
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
    LiteTestPrint("HksCurve25519TestSetUp End2!\n");
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksCurve25519TestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
}

static void LocalHksCurve25519Test001(void const *argument)
{
    LiteTestPrint("HksCurve25519Test001 Begin!\n");
    TEST_ASSERT_TRUE(TestCurve25519All() == 0);
    LiteTestPrint("HksCurve25519Test001 End!\n");
    osThreadExit();
}

static void LocalHksCurve25519Test002(void const *argument)
{
    LiteTestPrint("HksCurve25519Test002 Begin!\n");
    TEST_ASSERT_TRUE(TestEd25519SignTeeVerifyLocal() == 0);
    LiteTestPrint("HksCurve25519Test002 End!\n");
    osThreadExit();
}

static void LocalHksCurve25519Test003(void const *argument)
{
    LiteTestPrint("HksCurve25519Test003 Begin!\n");
    TEST_ASSERT_TRUE(TestCurve25519SignWrong() == 0);
    LiteTestPrint("HksCurve25519Test003 End!\n");
    osThreadExit();
}

static void LocalHksCurve25519Test004(void const *argument)
{
    LiteTestPrint("HksCurve25519Test004 Begin!\n");
    TEST_ASSERT_TRUE(TestCurve25519verifyWrong() == 0);
    LiteTestPrint("HksCurve25519Test004 End!\n");
    osThreadExit();
}

/**
 * @tc.name: HksCurve25519Test.HksCurve25519Test001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksCurve25519Test, HksCurve25519Test001, Level1)
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
    id = osThreadNew((osThreadFunc_t)LocalHksCurve25519Test001, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCurve25519Test001 End2!\n");    
}

/**
 * @tc.name: HksCurve25519Test.HksCurve25519Test002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksCurve25519Test, HksCurve25519Test002, Level1)
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
    id = osThreadNew((osThreadFunc_t)LocalHksCurve25519Test002, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCurve25519Test002 End2!\n");
}

/**
 * @tc.name: HksCurve25519Test.HksCurve25519Test003
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksCurve25519Test, HksCurve25519Test003, Level1)
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
    id = osThreadNew((osThreadFunc_t)LocalHksCurve25519Test003, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCurve25519Test003 End2!\n");
}

/**
 * @tc.name: HksCurve25519Test.HksCurve25519Test004
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksCurve25519Test, HksCurve25519Test004, Level1)
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
    id = osThreadNew((osThreadFunc_t)LocalHksCurve25519Test004, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCurve25519Test004 End2!\n");
}

RUN_TEST_SUITE(HksCurve25519Test);
#endif
#endif /* _CUT_AUTHENTICATE_ */

