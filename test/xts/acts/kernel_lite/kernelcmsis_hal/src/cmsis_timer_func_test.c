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

UINT16 g_cmsisTestTimeCount;
#define MILLISEC_NUM_INT10 10U
#define MILLISEC_NUM_INT4 4U
#define INVALID_TIMER_TYPE 10
#define TIMER_PERIODIC_COUNT 2


/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Cmsis, Cmsistimer, CmsisTimerFuncTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL CmsisTimerFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL CmsisTimerFuncTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

static void CmsisTimerFunc001(void const *argument)
{
    (void)argument;
    return;
}

static void CmsisTimerFunc002(void const *argument)
{
    (void)argument;
    g_cmsisTestTimeCount++;
    return;
}

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_0100
 * @tc.name      : timer operation for creat with parameter osTimerOnce
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerNew001, Function | MediumTest | Level1)
{
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);
    (void)osTimerDelete(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_0200
 * @tc.name      : timer operation for creat with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerNew002, Function | MediumTest | Level1)
{
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);
    (void)osTimerDelete(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_0300
 * @tc.name      : timer creat operation with NULL func and parameter osTimerOnce
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerNew003, Function | MediumTest | Level1)
{
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew(NULL, osTimerOnce, &value, NULL);
    TEST_ASSERT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_0400
 * @tc.name      : timer creat operation with NULL func and parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerNew004, Function | MediumTest | Level1)
{
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew(NULL, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_0500
 * @tc.name      : timer operation for invalid timer type
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerNew005, Function | MediumTest | Level1)
{
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, INVALID_TIMER_TYPE, &value, NULL);
    TEST_ASSERT_NULL(id);
    osDelay(DELAY_TICKS_5);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_0600
 * @tc.name      : timer operation for start with parameter osTimerOnce
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStart001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT4;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);
    uwRet = osTimerStart(id, millisec);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_0700
 * @tc.name      : timer operation for start with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStart002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT4;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);
    uwRet = osTimerStart(id, millisec);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_0800
 * @tc.name      : timer operation for start with callback func and parameter osTimerOnce
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStart003, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT4;
    g_cmsisTestTimeCount = 0;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc002, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);
    uwRet = osTimerStart(id, millisec);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);

    osDelay(DELAY_TICKS_10);
    TEST_ASSERT_EQUAL_INT(1, g_cmsisTestTimeCount);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_0900
 * @tc.name      : timer operation for start with callback func and parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStart004, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT4;
    g_cmsisTestTimeCount = 0;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc002, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);
    uwRet = osTimerStart(id, millisec);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);

    osDelay(DELAY_TICKS_10);
    TEST_ASSERT_EQUAL_INT(TIMER_PERIODIC_COUNT, g_cmsisTestTimeCount);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1000
 * @tc.name      : timer start operation with ticks = 0 and parameter osTimerOnce
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStart005, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);
    uwRet = osTimerStart(id, 0);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1100
 * @tc.name      : timer start operation with ticks = 0 and parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStart006, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);
    uwRet = osTimerStart(id, 0);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1200
 * @tc.name      : timer start operation with timer_id = NULL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStart007, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    UINT32 millisec = MILLISEC_NUM_INT4;
    uwRet = osTimerStart(NULL, millisec);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1300
 * @tc.name      : timer operation for delete with parameter osTimerOnce
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerDelete001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osDelay(1);
    uwRet = osTimerDelete(id);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1400
 * @tc.name      : timer operation for delete with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerDelete002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osDelay(1);
    uwRet = osTimerDelete(id);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1500
 * @tc.name      : timer operation for delete after osTimerStart with parameter osTimerOnce
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerDelete003, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    osDelay(1);
    uwRet = osTimerDelete(id);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1600
 * @tc.name      : timer operation for delete after osTimerStart with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerDelete004, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    osDelay(1);
    uwRet = osTimerDelete(id);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1700
 * @tc.name      : timer delete operation with timer_id = NULL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerDelete005, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osTimerDelete(NULL);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1800
 * @tc.name      : timer operation for stop after osTimerStart with parameter osTimerOnce
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStop001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    osDelay(1);
    uwRet = osTimerStop(id);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_1900
 * @tc.name      : timer operation for stop after osTimerStart with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStop002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    osDelay(1);
    uwRet = osTimerStop(id);
    TEST_ASSERT_EQUAL_INT(osOK, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2000
 * @tc.name      : timer operation for stop with parameter osTimerOnce
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStop003, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osDelay(1);
    uwRet = osTimerStop(id);
    TEST_ASSERT_EQUAL_INT(osErrorResource, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2100
 * @tc.name      : timer operation for stop with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStop004, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osDelay(1);
    uwRet = osTimerStop(id);
    TEST_ASSERT_EQUAL_INT(osErrorResource, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2200
 * @tc.name      : timer stop operation with timer_id = NULL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerStop005, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osTimerStop(NULL);
    TEST_ASSERT_EQUAL_INT(osErrorParameter, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2300
 * @tc.name      : timer operation for running
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerIsRunning001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew ((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    uwRet = osTimerIsRunning(id);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2400
 * @tc.name      : timer operation for running after osTimerStart
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerIsRunning002, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    uwRet = osTimerIsRunning(id);
    TEST_ASSERT_EQUAL_INT(1, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2500
 * @tc.name      : timer operation for running after osTimerStart and osTimerStop
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerIsRunning003, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew ((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    osTimerStop(id);

    uwRet = osTimerIsRunning(id);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2600
 * @tc.name      : timer operation for running after osTimerStart and osTimerDelete
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerIsRunning004, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew ((osTimerFunc_t)CmsisTimerFunc001, osTimerOnce, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    osTimerDelete(id);
    uwRet = osTimerIsRunning(id);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2700
 * @tc.name      : timer operation for running with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerIsRunning005, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    id = osTimerNew ((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    uwRet = osTimerIsRunning(id);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2800
 * @tc.name      : timer operation for running after osTimerStart with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerIsRunning006, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    uwRet = osTimerIsRunning(id);
    TEST_ASSERT_EQUAL_INT(1, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_2900
 * @tc.name      : timer operation for running after osTimerStart and osTimerStop with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerIsRunning007, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew ((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    osTimerStop(id);

    uwRet = osTimerIsRunning(id);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
    (void)osTimerDelete(id);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_3000
 * @tc.name      : timer operation for running after osTimerStart and osTimerDelete with parameter osTimerPeriodic
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerIsRunning008, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    osTimerId_t id;
    UINT32 value=0xffff;
    UINT32 millisec = MILLISEC_NUM_INT10;
    id = osTimerNew ((osTimerFunc_t)CmsisTimerFunc001, osTimerPeriodic, &value, NULL);
    TEST_ASSERT_NOT_NULL(id);

    osTimerStart(id, millisec);
    osTimerDelete(id);
    uwRet = osTimerIsRunning(id);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_3100
 * @tc.name      : timer running inquiry with timer_id = NULL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsTimerIsRunning009, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osTimerIsRunning(NULL);
    TEST_ASSERT_EQUAL_INT(0, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_3200
 * @tc.name      : os operation for get tick freq
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsKernelGetTickFreq001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osKernelGetTickFreq();
    TEST_ASSERT_EQUAL_INT(LOSCFG_BASE_CORE_TICK_PER_SECOND, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_3300
 * @tc.name      : os operation for get sys time freq
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsKernelGetSysTimerFreq001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osKernelGetSysTimerFreq();
    TEST_ASSERT_EQUAL_INT(OS_SYS_CLOCK, uwRet);
};

/**
 * @tc.number    : SUB_KERNEL_CMSIS_TIMER_OPERATION_3400
 * @tc.name      : os operation for get sys time count
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(CmsisTimerFuncTestSuite, testOsKernelGetSysTimerCount001, Function | MediumTest | Level1)
{
    UINT32 uwRet;
    uwRet = osKernelGetSysTimerCount();
    TEST_ASSERT_GREATER_THAN_UINT32(0, uwRet);
};

RUN_TEST_SUITE(CmsisTimerFuncTestSuite);
