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

#include "hks_cipher_test.h"

#include "hctest.h"
#include "iot_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_cipher.h"
#include "securec.h"
#include "cmsis_os2.h"
#include "ohos_types.h"

#define TEST_INDEX_0    0
#define TEST_INDEX_1    1
#define TEST_INDEX_2    2
#define TEST_INDEX_3    3
#define TEST_INDEX_4    4
#define TEST_TASK_STACK_SIZE      0x2000
#define WAIT_TO_TEST_DONE         4

static osPriority_t g_setPriority;

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(security, securityData, HksCipherTest);

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
static BOOL HksCipherTestSetUp()
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
    LiteTestPrint("HksCipherTestSetUp End2!\n");
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksCipherTestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
}

static void ExecHksCipherTest001(void const *argument)
{
    LiteTestPrint("HksCipherTest001 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestCipher(1, TEST_INDEX_0, 1) == 0);
    LiteTestPrint("HksCipherTest001 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest002(void const *argument)
{
    LiteTestPrint("HksCipherTest002 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestCipher(1, TEST_INDEX_0, 1) == 0);
    LiteTestPrint("HksCipherTest002 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest003(void const *argument)
{
    LiteTestPrint("HksCipherTest003 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_0, 1) == 0);
    LiteTestPrint("HksCipherTest003 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest004(void const *argument)
{
    LiteTestPrint("HksCipherTest004 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_1, 1) == 0);
    LiteTestPrint("HksCipherTest004 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest005(void const *argument)
{
    LiteTestPrint("HksCipherTest005 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_2, 1) == 0);
    LiteTestPrint("HksCipherTest005 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest006(void const *argument)
{
    LiteTestPrint("HksCipherTest006 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_3, 1) == 0);
    LiteTestPrint("HksCipherTest006 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest007(void const *argument)
{
    LiteTestPrint("HksCipherTest007 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_4, 1) == 0);
    LiteTestPrint("HksCipherTest007 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest008(void const *argument)
{
    LiteTestPrint("HksCipherTest008 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_0, 1) == 0);
    LiteTestPrint("HksCipherTest008 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest009(void const *argument)
{
    LiteTestPrint("HksCipherTest009 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_1, 1) == 0);
    LiteTestPrint("HksCipherTest009 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest010(void const *argument)
{
    LiteTestPrint("HksCipherTest010 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_2, 1) == 0);
    LiteTestPrint("HksCipherTest010 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest011(void const *argument)
{
    LiteTestPrint("HksCipherTest011 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_3, 1) == 0);
    LiteTestPrint("HksCipherTest011 End!\n");
    osThreadExit();
}

static void ExecHksCipherTest012(void const *argument)
{
    LiteTestPrint("HksCipherTest012 Begin!\n");
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_4, 1) == 0);
    LiteTestPrint("HksCipherTest012 End!\n");
    osThreadExit();
}


/**
 * @tc.name: HksCipherTest.HksCipherTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksCipherTest, HksCipherTest001, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest001, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest001 End2!\n");
}

#ifndef _CUT_AUTHENTICATE_

LITE_TEST_CASE(HksCipherTest, HksCipherTest002, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest002, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest002 End2!\n");
    
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest003, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest003, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest003 End2!\n");
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest004, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest004, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest004 End2!\n");
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest005, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest005, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest005 End2!\n");    
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest006, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest006, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest006 End2!\n");    
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest007, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest007, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest007 End2!\n");    
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest008, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest008, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest008 End2!\n");
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest009, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest009, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest009 End2!\n");
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest010, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest010, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest010 End2!\n");    
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest011, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest011, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest011 End2!\n");
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest012, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksCipherTest012, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksCipherTest012 End2!\n");
}
#endif /* _CUT_AUTHENTICATE_ */

RUN_TEST_SUITE(HksCipherTest);
