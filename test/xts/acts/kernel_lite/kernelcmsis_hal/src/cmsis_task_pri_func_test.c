/**
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "ohos_types.h"
#include <securec.h>
#include "hctest.h"
#include "los_config.h"
#include "cmsis_os2.h"
#include "kernel_test.h"

osThreadId_t g_priTaskID01;
osPriority_t g_setPriority;

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskPriFuncTestSuite
 */
LITE_TEST_SUIT(Cmsis, Cmsistask, CmsisTaskPriFuncTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL CmsisTaskPriFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL CmsisTaskPriFuncTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

static void CmsisThreadGetPriorityFunc001(void const *argument)
{
    (void)argument;
    osThreadAttr_t attr;
    g_priTaskID01 = osThreadGetId();
    attr.priority = osThreadGetPriority(g_priTaskID01);
    TEST_ASSERT_EQUAL_INT(g_setPriority, attr.priority);
    osThreadExit();
}

static void CmsisThreadSetPriorityFunc001(void const *argument)
{
    (void)argument;
    osThreadAttr_t attr;
    UINT32 uwRet;
    g_priTaskID01 = osThreadGetId();
    uwRet = osThreadSetPriority(g_priTaskID01, g_setPriority);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    attr.priority = osThreadGetPriority(g_priTaskID01);
    TEST_ASSERT_EQUAL_INT(g_setPriority, attr.priority);
    osThreadExit();
}

static void CmsisThreadSetPriorityFunc002(void const *argument)
{
    (void)argument;
    UINT32 uwRet;
    g_priTaskID01 = osThreadGetId();
    uwRet = osThreadSetPriority(g_priTaskID01, g_setPriority);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
    osThreadExit();
}

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5120
 * @tc.name      : thread operation for get priority when Priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority001, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityLow1;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5160
 * @tc.name      : thread operation for get priority input exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osThreadGetPriority(NULL);
    TEST_ASSERT_EQUAL_INT(osPriorityError, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5200
 * @tc.name      : thread operation for get current priority
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority003, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    g_priTaskID01 =  osThreadGetId();
    uwRet = osThreadGetPriority(g_priTaskID01);
    TEST_ASSERT_EQUAL_INT(osPriorityNormal, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5240
 * @tc.name      : thread operation for get priority when Priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityLow7;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5280
 * @tc.name      : thread operation for get priority when Priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityBelowNormal;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5320
 * @tc.name      : thread operation for get priority when Priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityBelowNormal7;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5360
 * @tc.name      : thread operation for get priority when Priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority007, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityNormal;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5400
 * @tc.name      : thread operation for get priority when Priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityNormal7;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetPriorityFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5440
 * @tc.name      : thread operation for get priority when Priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority009, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityAboveNormal;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetPriorityFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5480
 * @tc.name      : thread operation for get priority when Priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadGetPriority010, Function | MediumTest | Level1)
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
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetPriorityFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5520
 * @tc.name      : thread operation for set priority input1 exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osThreadSetPriority(NULL, osPriorityNormal);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5560
 * @tc.name      : thread operation for set priority input2 exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    g_priTaskID01 =  osThreadGetId();
    uwRet = osThreadSetPriority(g_priTaskID01, osPriorityNone);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5600
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityNone
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority003, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityNone;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5640
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityIdle
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityIdle;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5680
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityLow
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityLow;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5720
 * @tc.name      : set priority when curPriority = osPriorityLow1 and setPriority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityLow1;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5760
 * @tc.name      : set priority when curPriority = osPriorityLow1 and setPriority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority007, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityLow7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5800
 * @tc.name      : set priority when curPriority = osPriorityLow1 and setPriority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityBelowNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5840
 * @tc.name      : set priority when curPriority = osPriorityLow1 and setPriority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority009, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityBelowNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5880
 * @tc.name      : set priority when curPriority = osPriorityLow1 and setPriority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority010, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5920
 * @tc.name      : set priority when curPriority = osPriorityLow1 and setPriority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority011, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5960
 * @tc.name      : set priority when curPriority = osPriorityLow1 and setPriority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority012, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityAboveNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6000
 * @tc.name      : set priority when curPriority = osPriorityLow1 and setPriority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority013, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityAboveNormal6;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6040
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityAboveNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority014, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityAboveNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6080
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityHigh
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority015, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityHigh;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6120
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityHigh7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority016, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityHigh7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6160
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityRealtime
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority017, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityRealtime;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6200
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityRealtime7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority018, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityRealtime7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6240
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityISR
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority019, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityISR;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6280
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityError
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority020, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityError;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6320
 * @tc.name      : set invalid priority when curPriority = osPriorityLow1 and setPriority = osPriorityReserved
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority021, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_setPriority = osPriorityReserved;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6360
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityNone
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority022, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityNone;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6400
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityIdle
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority023, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityIdle;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6440
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityLow
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority024, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityLow;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6480
 * @tc.name      : set priority when curPriority = osPriorityNormal and setPriority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority025, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityLow1;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6520
 * @tc.name      : set priority when curPriority = osPriorityNormal and setPriority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority026, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityLow7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6560
 * @tc.name      : set priority when curPriority = osPriorityNormal and setPriority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority027, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityBelowNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6600
 * @tc.name      : set priority when curPriority = osPriorityNormal and setPriority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority028, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityBelowNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6640
 * @tc.name      : set priority when curPriority = osPriorityNormal and setPriority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority029, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6680
 * @tc.name      : set priority when curPriority = osPriorityNormal and setPriority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority030, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6720
 * @tc.name      : set priority when curPriority = osPriorityNormal and setPriority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority031, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityAboveNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6760
 * @tc.name      : set priority when curPriority = osPriorityNormal and setPriority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority032, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityAboveNormal6;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6800
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityAboveNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority033, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityAboveNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6840
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityHigh
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority034, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityHigh;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6880
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityHigh7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority035, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityHigh7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6920
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityRealtime
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority036, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityRealtime;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_6960
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityRealtime7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority037, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityRealtime7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7000
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityISR
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority038, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityISR;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7040
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityError
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority039, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityError;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7080
 * @tc.name      : set invalid priority when curPriority = osPriorityNormal and setPriority = osPriorityReserved
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority040, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    g_setPriority = osPriorityReserved;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7120
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityNone
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority041, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityNone;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7160
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityIdle
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority042, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityIdle;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7200
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityLow
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority043, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityLow;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7240
 * @tc.name      : set priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority044, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityLow1;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7280
 * @tc.name      : set priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority045, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityLow7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7320
 * @tc.name      : set priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority046, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityBelowNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7360
 * @tc.name      : set priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority047, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityBelowNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7400
 * @tc.name      : set priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority048, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7440
 * @tc.name      : set priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority049, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7480
 * @tc.name      : set priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority050, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityAboveNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7520
 * @tc.name      : set priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority051, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityAboveNormal6;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7560
 * @tc.name      : set invalid priority when curPriority = PriorityAboveNormal6 and setPriority = PriorityAboveNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority052, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityAboveNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7600
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityHigh
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority053, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityHigh;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7640
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityHigh7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority054, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityHigh7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7680
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityRealtime
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority055, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityRealtime;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7720
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityRealtime7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority056, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityRealtime7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7760
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityISR
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority057, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityISR;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7800
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityError
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority058, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityError;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7840
 * @tc.name      : set invalid priority when curPriority = osPriorityAboveNormal6 and setPriority = osPriorityReserved
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsThreadSetPriority059, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_setPriority = osPriorityReserved;
    id = osThreadNew((osThreadFunc_t)CmsisThreadSetPriorityFunc002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7880
 * @tc.name      : delay operation for 5 ticks
 * @tc.desc      : [C- SOFTWARE -0200]

 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsDelay001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7920
 * @tc.name      : delay operation
 * @tc.desc      : [C- SOFTWARE -0200]

 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsDelay002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osDelay(0);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_7960
 * @tc.name      : delay until operation
 * @tc.desc      : [C- SOFTWARE -0200]

 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsDelayUntil001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    UINT32 uwTickCnt;
    UINT32 uwUntilTickCnt;
    uwTickCnt = osKernelGetTickCount();
    uwUntilTickCnt = uwTickCnt + DELAY_TICKS_10;
    uwRet = osDelayUntil(uwUntilTickCnt);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_8000
 * @tc.name      : delay until operation input exception for 1 tick
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsDelayUntil002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osDelayUntil(DELAY_TICKS_1);
    TEST_ASSERT_EQUAL_INT(osError, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_8040
 * @tc.name      : delay until operation input exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskPriFuncTestSuite, testOsDelayUntil003, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    UINT32 uwTickCnt;
    UINT32 uwUntilTickCnt;
    uwTickCnt = osKernelGetTickCount();
    uwUntilTickCnt = uwTickCnt - DELAY_TICKS_10;
    uwRet = osDelayUntil(uwUntilTickCnt);
    TEST_ASSERT_EQUAL_INT(osError, uwRet);
};

RUN_TEST_SUITE(CmsisTaskPriFuncTestSuite);
