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

UINT32 g_threadCount;
UINT16 g_cmsisTestTaskCount;
UINT16 g_getStackSizeExit;
UINT16 g_threadCreateExit;
UINT16 g_getNameExit;
UINT16 g_getStackSpaceExit;
osThreadId_t g_puwTaskID01;
osThreadId_t g_puwTaskID02;
osPriority_t g_threadPriority;

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Cmsis, Cmsistask, CmsisTaskFuncTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL CmsisTaskFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL CmsisTaskFuncTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

static void CmsisThreadCreatFunc(void const *argument)
{
    (void)argument;
    printf(">> in CmsisThreadCreatFunc\n");
    g_threadCreateExit = TESTCOUNT_NUM_1;
    osThreadExit();
}

static void CmsisThreadCreat002Func001(void const *argument)
{
    (void)argument;
    g_cmsisTestTaskCount++;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_3, g_cmsisTestTaskCount);
    osThreadExit();
}

static void CmsisThreadCreat002Func002(void const *argument)
{
    (void)argument;
    g_cmsisTestTaskCount++;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    osThreadExit();
}

static void CmsisThreadCreat003Func001(void const *argument)
{
    (void)argument;
    TEST_ASSERT_EQUAL_INT(0, g_cmsisTestTaskCount);
    g_cmsisTestTaskCount++;
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_3, g_cmsisTestTaskCount);
    g_cmsisTestTaskCount++;
    osThreadExit();
}

static void CmsisThreadCreat003Func002(void const *argument)
{
    (void)argument;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    g_cmsisTestTaskCount++;
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_4, g_cmsisTestTaskCount);
    g_cmsisTestTaskCount++;
    osThreadExit();
}

static void CmsisThreadCreat004Func002(void const *argument)
{
    (void)argument;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
    g_cmsisTestTaskCount++;
    osThreadExit();
}

static void CmsisThreadCreat004Func001(void const *argument)
{
    (void)argument;
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    TEST_ASSERT_EQUAL_INT(0, g_cmsisTestTaskCount);
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreat004Func002, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    g_cmsisTestTaskCount++;
    osThreadExit();
}

static void KeepRunByTick(UINT32 tick)
{
    UINT32 tickA = osKernelGetTickCount();
    UINT32 ran = 0;
    UINT32 loop = 0;
    UINT32 tickB = 0;
    while (ran < tick) {
        loop++;
        tickB = osKernelGetTickCount();
        if (tickB >= tickA) {
            ran = tickB - tickA;
        } else {
            ran = tickB + (MAX_UINT32 - tickA);
        }
        if (loop % ALIVE_INFO_DIS == 0) {
            printf("ran:%u, tickB:%u, tickA:%u, loop:%u\t\n",
                ran, tickB, tickA, loop);
        }
    }
    printf("return ran:%u, tickB:%u, tickA:%u\t\n",
        ran, tickB, tickA);
    return;
}

static void WaitThreadExit(osThreadId_t id, UINT16 const *exitFlag)
{
    UINT32 uwRet = osThreadSetPriority(id, osPriorityAboveNormal6);
    printf("WaitThreadExit id = %d, uwRet = %d\n", id, uwRet);
    UINT32 loop = 0;
    while (*exitFlag != TESTCOUNT_NUM_1) {
        osDelay(DELAY_TICKS_10);
        if (loop % ALIVE_INFO_DIS == 0) {
            printf("WaitThreadExit id = %d, loop = %d\n", id, loop++);
        }
    }
    printf("WaitThreadExit exit\n");
}

static void CmsisThreadCreat005Func001(void const *argument)
{
    (void)argument;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
    while (g_cmsisTestTaskCount < TESTCOUNT_NUM_2) {
        KeepRunByTick(DELAY_TICKS_10);
    }
    g_cmsisTestTaskCount++;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_3, g_cmsisTestTaskCount);
    osThreadExit();
}

static void CmsisThreadGetIDFunc001(void const *argument)
{
    (void)argument;
    g_puwTaskID01 = osThreadGetId();
    TEST_ASSERT_NOT_NULL(g_puwTaskID01);
    osThreadExit();
}

static void CmsisThreadGetNameFunc001(void const *argument)
{
    (void)argument;
    osThreadAttr_t attr;
    printf(">> in CmsisThreadGetNameFunc001\n");
    g_puwTaskID01 = osThreadGetId();
    attr.name = osThreadGetName(g_puwTaskID01);
    TEST_ASSERT_EQUAL_STRING("testThreadGetName", attr.name);
    g_getNameExit = TESTCOUNT_NUM_1;
    osThreadExit();
}

static void CmsisThreadGetStateFunc001(void const *argument)
{
    (void)argument;
    osThreadState_t state;
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadBlocked, state);

    g_puwTaskID02 = osThreadGetId();
    state = osThreadGetState(g_puwTaskID02);
    TEST_ASSERT_EQUAL_INT(osThreadRunning, state);
    osThreadExit();
}

static void CmsisThreadGetStateFunc002(void const *argument)
{
    (void)argument;
    osThreadState_t state;
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadReady, state);

    g_puwTaskID02 = osThreadGetId();
    state = osThreadGetState(g_puwTaskID02);
    TEST_ASSERT_EQUAL_INT(osThreadRunning, state);
    osThreadExit();
}

static void CmsisThreadSuspendFunc001(void const *argument)
{
    (void)argument;
    osStatus_t uwRet;
    g_puwTaskID01 = osThreadGetId();
    uwRet = osThreadSuspend(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    osThreadExit();
}

static void CmsisThreadGetStackSizeFunc001(void const *argument)
{
    (void)argument;
    osThreadAttr_t attr;
    printf(">> in CmsisThreadGetStackSizeFunc001\n");
    g_puwTaskID01 = osThreadGetId();
    attr.stack_size = osThreadGetStackSize(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(TEST_TASK_STACK_SIZE, attr.stack_size);
    g_getStackSizeExit = TESTCOUNT_NUM_1;
    osThreadExit();
}

static void CmsisThreadGetStackSpaceFunc001(void const *argument)
{
    (void)argument;
    UINT32 uwCount;
    printf(">> in CmsisThreadGetStackSpaceFunc001\n");
    g_puwTaskID01 =  osThreadGetId();
    uwCount = osThreadGetStackSpace(g_puwTaskID01);
    TEST_ASSERT_GREATER_THAN_INT32(0, uwCount);
    g_getStackSpaceExit = TESTCOUNT_NUM_1;
    osThreadExit();
}

static void CmsisThreadYieldFunc002(void const *argument)
{
    (void)argument;
    osThreadState_t state;
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadReady, state);
    g_cmsisTestTaskCount++;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    osThreadExit();
}

static void CmsisThreadYieldFunc001(void const *argument)
{
    (void)argument;
    osStatus_t uwRet;
    osThreadId_t id;
    osThreadState_t state;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_threadPriority;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
    g_puwTaskID01 =  osThreadGetId();
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc002, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadRunning, state);
    uwRet = osThreadYield();
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    osThreadExit();
}

static void CmsisThreadYieldFunc003(void const *argument)
{
    (void)argument;
    osStatus_t uwRet;
    osThreadId_t id;
    osThreadState_t state;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;
    g_puwTaskID01 =  osThreadGetId();
    g_threadCreateExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadRunning, state);
    uwRet = osThreadYield();
    WaitThreadExit(id, &g_threadCreateExit);
    osThreadExit();
}

static void CmsisThreadResumeFunc002(void const *argument)
{
    (void)argument;
    osStatus_t uwRet;
    g_cmsisTestTaskCount++;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    osDelay(DELAY_TICKS_5);
    osThreadExit();
}

static void CmsisThreadResumeFunc001(void const *argument)
{
    (void)argument;
    osStatus_t uwRet;
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    g_puwTaskID01 = osThreadGetId();
    id = osThreadNew((osThreadFunc_t)CmsisThreadResumeFunc002, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    uwRet = osThreadSuspend(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    g_cmsisTestTaskCount++;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_3, g_cmsisTestTaskCount);
    osThreadExit();
}

static void CmsisThreadTerminateFunc001(void const *argument)
{
    (void)argument;
    osStatus_t uwRet;
    g_puwTaskID01 = osThreadGetId();
    uwRet = osThreadTerminate(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    g_cmsisTestTaskCount++;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
    osThreadExit();
}

static void CmsisThreadGetCountFunc002(void const *argument)
{
    (void)argument;
    UINT32 uwRet;
    uwRet = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(1, uwRet);
    osThreadExit();
}

static void CmsisThreadGetCountFunc001(void const *argument)
{
    (void)argument;
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc002, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osThreadExit();
}

static void CmsisThreadGetCountFunc003(void const *argument)
{
    (void)argument;
    UINT32 uwRet;
    uwRet = osThreadGetCount();
    TEST_ASSERT_EQUAL_INT(g_threadCount + 1, uwRet);
    osThreadExit();
}

static void CmsisOSKernelLockFunc002(void const *argument)
{
    (void)argument;
    g_cmsisTestTaskCount++;
    osThreadExit();
}

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0040
 * @tc.name      : thread operation for creat fail with invalid parameter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew001, Function | MediumTest | Level1)
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

    osDelay(DELAY_TICKS_5);
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, NULL);
    TEST_ASSERT_NULL(id);
    id = osThreadNew(NULL, NULL, NULL);
    TEST_ASSERT_NULL(id);
    id = osThreadNew(NULL, NULL, &attr);
    TEST_ASSERT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0080
 * @tc.name      : thread operation for creat success
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew002, Function | MediumTest | Level1)
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
    g_cmsisTestTaskCount = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreat002Func001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT(0, g_cmsisTestTaskCount);

    g_cmsisTestTaskCount++;
    attr.priority = osPriorityAboveNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreat002Func002, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_3, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0120
 * @tc.name      : thread operation for delay scheduler
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew003, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    osThreadAttr_t attr1;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadCreat003Func001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
    g_cmsisTestTaskCount++;
    attr1.name = "test1";
    attr1.attr_bits = 0U;
    attr1.cb_mem = NULL;
    attr1.cb_size = 0U;
    attr1.stack_mem = NULL;
    attr1.stack_size = TEST_TASK_STACK_SIZE;
    attr1.priority = osPriorityAboveNormal;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadCreat003Func002, NULL, &attr1);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_5, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0160
 * @tc.name      : thread operation for nesting schedule
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    g_cmsisTestTaskCount = 0;
    attr.priority = osPriorityAboveNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreat004Func001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_3, g_cmsisTestTaskCount);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0200
 * @tc.name      : thread operation for cycle schedule
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew005, Function | MediumTest | Level1)
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
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreat005Func001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_1);
    g_cmsisTestTaskCount++;
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    osDelay(DELAY_TICKS_5);
    while (g_cmsisTestTaskCount != TESTCOUNT_NUM_3) {
        KeepRunByTick(DELAY_TICKS_10);
    }
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0240
 * @tc.name      : thread operation for creat fail when priority = osPriorityNone
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNone;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0280
 * @tc.name      : thread operation for creat fail when priority = osPriorityIdle
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew007, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityIdle;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0320
 * @tc.name      : thread operation for creat fail when priority = osPriorityLow
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0360
 * @tc.name      : thread operation for creat success when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew009, Function | MediumTest | Level1)
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
    g_threadCreateExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_threadCreateExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0400
 * @tc.name      : thread operation for creat success when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew010, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow7;
    g_threadCreateExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_threadCreateExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0440
 * @tc.name      : thread operation for creat success when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew011, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal;
    g_threadCreateExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_threadCreateExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0480
 * @tc.name      : thread operation for creat success when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew012, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal7;
    g_threadCreateExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_threadCreateExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0520
 * @tc.name      : thread operation for creat success when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew013, Function | MediumTest | Level1)
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
    g_threadCreateExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_threadCreateExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0560
 * @tc.name      : thread operation for creat success when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew014, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;
    g_threadCreateExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_threadCreateExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0600
 * @tc.name      : thread operation for creat success when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew015, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    g_threadCreateExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_threadCreateExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0640
 * @tc.name      : thread operation for creat success when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew016, Function | MediumTest | Level1)
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
    g_threadCreateExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_threadCreateExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0680
 * @tc.name      : thread operation for creat fail when priority = osPriorityAboveNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew017, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0720
 * @tc.name      : thread operation for creat fail when priority = osPriorityHigh
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew018, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityHigh;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0760
 * @tc.name      : thread operation for creat fail when priority = osPriorityHigh7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew019, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityHigh7;
    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0800
 * @tc.name      : thread operation for creat fail when priority = osPriorityRealtime
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew020, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityRealtime;

    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0840
 * @tc.name      : thread operation for creat fail when priority = osPriorityRealtime7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew021, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityRealtime7;

    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0880
 * @tc.name      : thread operation for creat fail when priority = osPriorityISR
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew022, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityISR;

    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0920
 * @tc.name      : thread operation for creat fail when priority = osPriorityError
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew023, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityError;

    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_0960
 * @tc.name      : thread operation for creat fail when priority = osPriorityReserved
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew024, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityReserved;

    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1000
 * @tc.name      : thread creat operation with func = NULL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew025, Function | MediumTest | Level1)
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

    id = osThreadNew(NULL, NULL, &attr);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1040
 * @tc.name      : thread creat operation with attr = NULL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadNew026, Function | MediumTest | Level1)
{
    osThreadId_t id;

    id = osThreadNew((osThreadFunc_t)CmsisThreadCreatFunc, NULL, NULL);
    TEST_ASSERT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1080
 * @tc.name      : thread operation for get name when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName001, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "testThreadGetName";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;

    g_cmsisTestTaskCount = 0;
    g_getNameExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetNameFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    WaitThreadExit(id, &g_getNameExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1120
 * @tc.name      : thread operation for get state when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState001, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    g_puwTaskID01 = osThreadGetId();
    id = osThreadNew ((osThreadFunc_t)CmsisThreadGetStateFunc002, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1160
 * @tc.name      : thread operation for suspend when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadSuspend001, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osStatus_t uwRet;
    osThreadAttr_t attr;
    osThreadState_t state;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadSuspendFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadBlocked, state);
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1200
 * @tc.name      : thread operation for get current ID
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId001, Function | MediumTest | Level1)
{
    g_puwTaskID01 = osThreadGetId();
    TEST_ASSERT_NOT_NULL(g_puwTaskID01);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1240
 * @tc.name      : thread operation for get ID when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId002, Function | MediumTest | Level1)
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

    id = osThreadNew((osThreadFunc_t)CmsisThreadGetIDFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT((uintptr_t)id, (uintptr_t)g_puwTaskID01);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1280
 * @tc.name      : thread operation for get ID when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId003, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow7;

    id = osThreadNew((osThreadFunc_t)CmsisThreadGetIDFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT((uintptr_t)id, (uintptr_t)g_puwTaskID01);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1320
 * @tc.name      : thread operation for get ID when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal;

    id = osThreadNew((osThreadFunc_t)CmsisThreadGetIDFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT((uintptr_t)id, (uintptr_t)g_puwTaskID01);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1360
 * @tc.name      : thread operation for get ID when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal7;

    id = osThreadNew((osThreadFunc_t)CmsisThreadGetIDFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT((uintptr_t)id, (uintptr_t)g_puwTaskID01);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1400
 * @tc.name      : thread operation for get ID when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId006, Function | MediumTest | Level1)
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

    id = osThreadNew((osThreadFunc_t)CmsisThreadGetIDFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT((uintptr_t)id, (uintptr_t)g_puwTaskID01);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1440
 * @tc.name      : thread operation for get ID when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId007, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;

    id = osThreadNew((osThreadFunc_t)CmsisThreadGetIDFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT((uintptr_t)id, (uintptr_t)g_puwTaskID01);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1480
 * @tc.name      : thread operation for get ID when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;

    id = osThreadNew((osThreadFunc_t)CmsisThreadGetIDFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT((uintptr_t)id, (uintptr_t)g_puwTaskID01);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1520
 * @tc.name      : thread operation for get ID when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId009, Function | MediumTest | Level1)
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

    id = osThreadNew((osThreadFunc_t)CmsisThreadGetIDFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT((uintptr_t)id, (uintptr_t)g_puwTaskID01);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1560
 * @tc.name      : thread operation for get ID then exit thread
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetId010, Function | MediumTest | Level1)
{
    g_puwTaskID01 = osThreadGetId();
    TEST_ASSERT_NOT_NULL(g_puwTaskID01);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1600
 * @tc.name      : thread operation for get stack size when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize001, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_cmsisTestTaskCount = 0;
    g_puwTaskID01 = 0;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_getStackSizeExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSizeFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSizeExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1640
 * @tc.name      : thread operation for get stack space
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace001, Function | MediumTest | Level1)
{
    UINT32 uwCount;
    g_cmsisTestTaskCount = 0;
    g_puwTaskID01 =  osThreadGetId();
    uwCount = osThreadGetStackSpace(g_puwTaskID01);
    TEST_ASSERT_GREATER_THAN_INT32(0, uwCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1680
 * @tc.name      : thread operation for yield when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadYield001, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_threadPriority = osPriorityLow1;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_threadPriority;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1720
 * @tc.name      : thread operation for yield when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadYield002, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_threadPriority = osPriorityLow7;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_threadPriority;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1760
 * @tc.name      : thread operation for yield when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadYield003, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_threadPriority = osPriorityBelowNormal;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_threadPriority;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1800
 * @tc.name      : thread operation for yield when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadYield004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_threadPriority = osPriorityBelowNormal7;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_threadPriority;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1840
 * @tc.name      : thread operation for yield when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadYield005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_threadPriority = osPriorityNormal;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_threadPriority;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1880
 * @tc.name      : thread operation for yield when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadYield006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_threadPriority = osPriorityNormal7;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_threadPriority;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc001, NULL, &attr);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1920
 * @tc.name      : thread operation for yield when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadYield007, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_threadPriority = osPriorityAboveNormal;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_threadPriority;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc001, NULL, &attr);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_1960
 * @tc.name      : thread operation for yield when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadYield008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_threadPriority = osPriorityAboveNormal6;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_threadPriority;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc001, NULL, &attr);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_2, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2000
 * @tc.name      : thread yield operation for thread with different priority
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadYield009, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadYieldFunc003, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2040
 * @tc.name      : thread operation for resume
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadResume001, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadResumeFunc001, NULL, &attr);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_3, g_cmsisTestTaskCount);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2080
 * @tc.name      : thread operation for terminate when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadTerminate001, Function | MediumTest | Level1)
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
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadTerminateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2120
 * @tc.name      : get thread count with callback function when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount001, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2160
 * @tc.name      : thread operation for current count
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2200
 * @tc.name      : get thread count when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount003, Function | MediumTest | Level1)
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
    g_threadCount = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(0, g_threadCount);
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc003, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2240
 * @tc.name      : get thread count when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow7;
    g_threadCount = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(0, g_threadCount);
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc003, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2280
 * @tc.name      : get thread count when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal;
    g_threadCount = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(0, g_threadCount);
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc003, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2320
 * @tc.name      : get thread count when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal7;
    g_threadCount = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(0, g_threadCount);
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc003, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2360
 * @tc.name      : get thread count when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount007, Function | MediumTest | Level1)
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
    g_threadCount = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(0, g_threadCount);
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc003, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2400
 * @tc.name      : get thread count when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;
    g_threadCount = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(0, g_threadCount);
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc003, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2440
 * @tc.name      : get thread count when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount009, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    g_threadCount = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(0, g_threadCount);
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc003, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2480
 * @tc.name      : get thread count when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetCount010, Function | MediumTest | Level1)
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
    g_threadCount = osThreadGetCount();
    TEST_ASSERT_GREATER_THAN_INT32(0, g_threadCount);
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetCountFunc003, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2520
 * @tc.name      : thread operation for get name input exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName002, Function | MediumTest | Level1)
{
    osThreadAttr_t attr;
    attr.name = osThreadGetName(NULL);
    TEST_ASSERT_NULL(attr.name);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2560
 * @tc.name      : thread operation for get name when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName003, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "testThreadGetName";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow1;
    g_getNameExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetNameFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    WaitThreadExit(id, &g_getNameExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2600
 * @tc.name      : thread operation for get name when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "testThreadGetName";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow7;
    g_getNameExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetNameFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    WaitThreadExit(id, &g_getNameExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2640
 * @tc.name      : thread operation for get name when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "testThreadGetName";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal;
    g_getNameExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetNameFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    WaitThreadExit(id, &g_getNameExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2680
 * @tc.name      : thread operation for get name when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "testThreadGetName";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal7;
    g_getNameExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetNameFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    WaitThreadExit(id, &g_getNameExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2720
 * @tc.name      : thread operation for get name when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName007, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "testThreadGetName";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;
    g_getNameExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetNameFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getNameExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2760
 * @tc.name      : thread operation for get name when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "testThreadGetName";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    g_getNameExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetNameFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getNameExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2800
 * @tc.name      : thread operation for get name when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName009, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "testThreadGetName";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    g_getNameExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetNameFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getNameExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2840
 * @tc.name      : thread operation for get name
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetName010, Function | MediumTest | Level1)
{
    osThreadAttr_t attr;
    g_puwTaskID01 = osThreadGetId();
    attr.name = osThreadGetName(g_puwTaskID01);
    TEST_ASSERT_NOT_NULL(attr.name);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2880
 * @tc.name      : thread operation for get state input exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState002, Function | MediumTest | Level1)
{
    osStatus_t uwRet;
    uwRet = osThreadGetState(NULL);
    TEST_ASSERT_EQUAL_INT(osThreadError, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2920
 * @tc.name      : thread operation for get state when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState003, Function | MediumTest | Level1)
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

    g_puwTaskID01 = osThreadGetId();
    id = osThreadNew ((osThreadFunc_t)CmsisThreadGetStateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_2960
 * @tc.name      : thread operation for get state when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow7;

    g_puwTaskID01 = osThreadGetId();
    id = osThreadNew ((osThreadFunc_t)CmsisThreadGetStateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3000
 * @tc.name      : thread operation for get state when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal;

    g_puwTaskID01 = osThreadGetId();
    id = osThreadNew ((osThreadFunc_t)CmsisThreadGetStateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3040
 * @tc.name      : thread operation for get state when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal7;

    g_puwTaskID01 = osThreadGetId();
    id = osThreadNew ((osThreadFunc_t)CmsisThreadGetStateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3080
 * @tc.name      : thread operation for get state when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState007, Function | MediumTest | Level1)
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

    g_puwTaskID01 = osThreadGetId();
    id = osThreadNew ((osThreadFunc_t)CmsisThreadGetStateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3120
 * @tc.name      : thread operation for get state when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;

    g_puwTaskID01 = osThreadGetId();
    id = osThreadNew ((osThreadFunc_t)CmsisThreadGetStateFunc002, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3160
 * @tc.name      : thread operation for get state when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState009, Function | MediumTest | Level1)
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

    g_puwTaskID01 = osThreadGetId();
    id = osThreadNew ((osThreadFunc_t)CmsisThreadGetStateFunc002, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3200
 * @tc.name      : thread operation for get current state
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetState010, Function | MediumTest | Level1)
{
    osThreadState_t state;
    g_puwTaskID01 = osThreadGetId();
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadRunning, state);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3240
 * @tc.name      : thread operation for suspend input exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadSuspend002, Function | MediumTest | Level1)
{
    osStatus_t uwRet;
    uwRet = osThreadSuspend(NULL);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3280
 * @tc.name      : thread operation for suspend when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadSuspend003, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osStatus_t uwRet;
    osThreadAttr_t attr;
    osThreadState_t state;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow7;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadSuspendFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadBlocked, state);
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3320
 * @tc.name      : thread operation for suspend when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadSuspend004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osStatus_t uwRet;
    osThreadAttr_t attr;
    osThreadState_t state;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadSuspendFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadBlocked, state);
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3360
 * @tc.name      : thread operation for suspend when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadSuspend005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osStatus_t uwRet;
    osThreadAttr_t attr;
    osThreadState_t state;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal7;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadSuspendFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadBlocked, state);
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3400
 * @tc.name      : thread operation for suspend when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadSuspend006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osStatus_t uwRet;
    osThreadAttr_t attr;
    osThreadState_t state;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadSuspendFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadBlocked, state);
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3440
 * @tc.name      : thread operation for suspend when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadSuspend007, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osStatus_t uwRet;
    osThreadAttr_t attr;
    osThreadState_t state;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadSuspendFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadBlocked, state);
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3480
 * @tc.name      : thread operation for suspend when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadSuspend008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osStatus_t uwRet;
    osThreadAttr_t attr;
    osThreadState_t state;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadSuspendFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadBlocked, state);
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3520
 * @tc.name      : thread operation for suspend when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadSuspend009, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osStatus_t uwRet;
    osThreadAttr_t attr;
    osThreadState_t state;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal6;
    id = osThreadNew ((osThreadFunc_t)CmsisThreadSuspendFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    state = osThreadGetState(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osThreadBlocked, state);
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3560
 * @tc.name      : thread operation for get stack size input exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osThreadGetStackSize(NULL);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3600
 * @tc.name      : thread operation for get stack size when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize003, Function | MediumTest | Level1)
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
    g_getStackSizeExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSizeFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSizeExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3640
 * @tc.name      : thread operation for get stack size when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow7;
    g_getStackSizeExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSizeFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSizeExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3680
 * @tc.name      : thread operation for get stack size when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal;
    g_getStackSizeExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSizeFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSizeExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3720
 * @tc.name      : thread operation for get stack size when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal7;
    g_getStackSizeExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSizeFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSizeExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3760
 * @tc.name      : thread operation for get stack size when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize007, Function | MediumTest | Level1)
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
    g_getStackSizeExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSizeFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSizeExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3800
 * @tc.name      : thread operation for get stack size when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;
    g_getStackSizeExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSizeFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSizeExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3840
 * @tc.name      : thread operation for get stack size when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize009, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    g_getStackSizeExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSizeFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSizeExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3880
 * @tc.name      : thread operation for get stack size
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSize010, Function | MediumTest | Level1)
{
    osThreadAttr_t attr;
    g_puwTaskID01 = osThreadGetId();
    attr.stack_size = osThreadGetStackSize(g_puwTaskID01);
    TEST_ASSERT_GREATER_THAN_INT32(0, attr.stack_size);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3920
 * @tc.name      : thread operation for get stack space input exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osThreadGetStackSpace(NULL);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_3960
 * @tc.name      : thread operation for get stack space when priority = osPriorityLow1
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace003, Function | MediumTest | Level1)
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
    g_getStackSpaceExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSpaceFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSpaceExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4000
 * @tc.name      : thread operation for get stack space when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow7;
    g_getStackSpaceExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSpaceFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSpaceExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4040
 * @tc.name      : thread operation for get stack space when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal;
    g_getStackSpaceExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSpaceFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSpaceExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4080
 * @tc.name      : thread operation for get stack space when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace006, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal7;
    g_getStackSpaceExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSpaceFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSpaceExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4120
 * @tc.name      : thread operation for get stack space when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace007, Function | MediumTest | Level1)
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
    g_getStackSpaceExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSpaceFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSpaceExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4160
 * @tc.name      : thread operation for get stack space when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;
    g_getStackSpaceExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSpaceFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSpaceExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4200
 * @tc.name      : thread operation for get stack space when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace009, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;

    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    g_getStackSpaceExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSpaceFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSpaceExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4240
 * @tc.name      : thread operation for get stack space when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadGetStackSpace010, Function | MediumTest | Level1)
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
    g_getStackSpaceExit = 0;
    id = osThreadNew((osThreadFunc_t)CmsisThreadGetStackSpaceFunc001, NULL, &attr);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_NOT_NULL(id);
    WaitThreadExit(id, &g_getStackSpaceExit);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4280
 * @tc.name      : thread operation for resume input exception with NULL parameter
 * @tc.desc      : [C- SOFTWARE -0200]

 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadResume002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osThreadResume(NULL);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4320
 * @tc.name      : thread operation for resume input exception
 * @tc.desc      : [C- SOFTWARE -0200]

 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadResume003, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    g_puwTaskID01 = osThreadGetId();
    uwRet = osThreadResume(g_puwTaskID01);
    TEST_ASSERT_EQUAL_INT(osErrorResource, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4360
 * @tc.name      : thread operation for terminate input exception
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadTerminate002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osThreadTerminate(NULL);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4400
 * @tc.name      : thread operation for terminate when priority = osPriorityLow7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadTerminate003, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityLow7;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadTerminateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4440
 * @tc.name      : thread operation for terminate when priority = osPriorityBelowNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadTerminate004, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadTerminateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4480
 * @tc.name      : thread operation for terminate when priority = osPriorityBelowNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadTerminate005, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityBelowNormal7;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadTerminateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4520
 * @tc.name      : thread operation for terminate when priority = osPriorityNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadTerminate006, Function | MediumTest | Level1)
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
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadTerminateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    osDelay(DELAY_TICKS_5);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4560
 * @tc.name      : thread operation for terminate when priority = osPriorityNormal7
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadTerminate007, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal7;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadTerminateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4600
 * @tc.name      : thread operation for terminate when priority = osPriorityAboveNormal
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadTerminate008, Function | MediumTest | Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadTerminateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4640
 * @tc.name      : thread operation for terminate when priority = osPriorityAboveNormal6
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsThreadTerminate009, Function | MediumTest | Level1)
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
    g_cmsisTestTaskCount = 0;
    g_cmsisTestTaskCount++;
    id = osThreadNew((osThreadFunc_t)CmsisThreadTerminateFunc001, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4680
 * @tc.name      : kernel operation for get info
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelGetInfo001, Function | MediumTest | Level1)
{
    CHAR infobuf[100];
    osVersion_t osv;
    osStatus_t status;
    status = osKernelGetInfo(&osv, infobuf, sizeof(infobuf));
    TEST_ASSERT_EQUAL_INT(osOK, status);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4720
 * @tc.name      : kernel operation for get state
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelGetState001, Function | MediumTest | Level1)
{
    osKernelState_t uwRet;
    uwRet = osKernelGetState();
    TEST_ASSERT_EQUAL_INT(osKernelRunning, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4760
 * @tc.name      : kernel operation for get state after kernel lock
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelGetState002, Function | MediumTest | Level1)
{
    osKernelLock ();
    osKernelState_t uwRet;
    uwRet = osKernelGetState();
    TEST_ASSERT_EQUAL_INT(osKernelLocked, uwRet);
    osKernelUnlock ();
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4800
 * @tc.name      : kernel lock operation twice
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelLock001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osKernelLock();
    TEST_ASSERT_EQUAL_INT(0, uwRet);
    uwRet = osKernelLock();
    TEST_ASSERT_EQUAL_INT(1, uwRet);
    osKernelUnlock();
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4840
 * @tc.name      : kernel operation for lock
 * @tc.desc      : [C- SOFTWARE -0200]

 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelLock002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osThreadId_t id;
    osThreadAttr_t attr;
    g_cmsisTestTaskCount = 0;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = osPriorityAboveNormal;

    uwRet = osKernelLock();
    TEST_ASSERT_EQUAL_INT(0, uwRet);
    id = osThreadNew((osThreadFunc_t)CmsisOSKernelLockFunc002, NULL, &attr);
    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_INT(0, g_cmsisTestTaskCount);
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(1, uwRet);
    TEST_ASSERT_EQUAL_INT(TESTCOUNT_NUM_1, g_cmsisTestTaskCount);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4880
 * @tc.name      : kernel operation for unlock
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelUnLock001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4920
 * @tc.name      : kernel operation for unlock after kernel lock
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelUnLock002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    (void)osKernelLock();
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(1, uwRet);
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_4960
 * @tc.name      : kernel operation for unlock after kernel lock twice
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelUnLock003, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    (void)osKernelLock();
    (void)osKernelLock();
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(1, uwRet);
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5000
 * @tc.name      : kernel operation for restore lock after kernel lock
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelRestoreLock001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    (void)osKernelLock();
    uwRet = osKernelRestoreLock(0);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5040
 * @tc.name      : kernel operation for restore lock after kernel lock twice
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelRestoreLock002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    (void)osKernelLock();
    (void)osKernelLock();
    uwRet = osKernelRestoreLock(0);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TASK_OPERATION_5080
 * @tc.name      : kernel operation for restore lock
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTaskFuncTestSuite, testOsKernelRestoreLock003, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(0, uwRet);
    uwRet = osKernelRestoreLock(1);
    TEST_ASSERT_EQUAL_INT(1, uwRet);
    uwRet = osKernelUnlock();
    TEST_ASSERT_EQUAL_INT(1, uwRet);
};

RUN_TEST_SUITE(CmsisTaskFuncTestSuite);
