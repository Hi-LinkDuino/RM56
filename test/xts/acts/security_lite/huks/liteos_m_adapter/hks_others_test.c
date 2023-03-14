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

#include "hks_others_test.h"

#include <hctest.h>
#include "iot_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
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
LITE_TEST_SUIT(security, securityData, HksOthersTest);

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
static BOOL HksOthersTestSetUp()
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
    LiteTestPrint("HksOthersTestSetUp End2!\n");
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksOthersTestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
}

static void ExcHksOthersTest001(void const *argument)
{
    LiteTestPrint("HksOthersTest001 Begin!\n");
    int32_t ret = HksGetKeyParamSet(NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NULL_POINTER);
    LiteTestPrint("HksOthersTest001 End!\n");
    osThreadExit();
}

static void ExcHksOthersTest002(void const *argument)
{
    LiteTestPrint("HksOthersTest002 Begin!\n");
    int32_t ret = HksGetKeyInfoList(NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NULL_POINTER);
    LiteTestPrint("HksOthersTest002 End!\n");
    osThreadExit();
}

static void ExcHksOthersTest003(void const *argument)
{
    LiteTestPrint("HksOthersTest003 Begin!\n");
    int32_t ret = HksAttestKey(NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
    LiteTestPrint("HksOthersTest003 End!\n");
    osThreadExit();
}

static void ExcHksOthersTest004(void const *argument)
{
    LiteTestPrint("HksOthersTest004 Begin!\n");
    int32_t ret = HksGetCertificateChain(NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
    LiteTestPrint("HksOthersTest004 End!\n");
    osThreadExit();
}

static void ExcHksOthersTest005(void const *argument)
{
    LiteTestPrint("HksOthersTest005 Begin!\n");
    int32_t ret = HksGetCertificateChain(NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
    LiteTestPrint("HksOthersTest005 End!\n");
    osThreadExit();
}

static void ExcHksOthersTest006(void const *argument)
{
    LiteTestPrint("HksOthersTest006 Begin!\n");
    int32_t ret = HksUnwrapKey(NULL, NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
    LiteTestPrint("HksOthersTest006 End!\n");
    osThreadExit();
}

static void ExcHksOthersTest007(void const *argument)
{
    LiteTestPrint("HksOthersTest007 Begin!\n");
    int32_t ret = HksGetSdkVersion(NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NULL_POINTER);
    LiteTestPrint("HksOthersTest007 End!\n");
    osThreadExit();
}

static void ExcHksOthersTest008(void const *argument)
{
    LiteTestPrint("HksOthersTest008 Begin!\n");
    int32_t ret = HksInitialize();
    TEST_ASSERT_TRUE(ret == HKS_SUCCESS);
    LiteTestPrint("HksOthersTest008 End!\n");
    osThreadExit();
}

/**
 * @tc.name: HksOthersTest.HksOthersTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest001, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExcHksOthersTest001, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksOthersTest001 End2!\n");
}

/**
 * @tc.name: HksOthersTest.HksOthersTest002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest002, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExcHksOthersTest002, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksOthersTest002 End2!\n");  
}

/**
 * @tc.name: HksOthersTest.HksOthersTest003
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest003, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExcHksOthersTest003, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksOthersTest003 End2!\n");
}

/**
 * @tc.name: HksOthersTest.HksOthersTest004
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest004, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExcHksOthersTest004, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksOthersTest004 End2!\n");
}

/**
 * @tc.name: HksOthersTest.HksOthersTest005
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest005, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExcHksOthersTest005, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksOthersTest005 End2!\n"); 
}

/**
 * @tc.name: HksOthersTest.HksOthersTest006
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest006, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExcHksOthersTest006, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksOthersTest006 End2!\n");
}

/**
 * @tc.name: HksOthersTest.HksOthersTest006
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest007, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExcHksOthersTest007, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksOthersTest007 End2!\n");
}

/**
 * @tc.name: HksOthersTest.HksOthersTest008
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest008, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExcHksOthersTest008, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksOthersTest008 End2!\n");
}
RUN_TEST_SUITE(HksOthersTest);

#endif /* _CUT_AUTHENTICATE_ */
