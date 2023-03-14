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
#include "hks_bn_exp_mod_test.h"
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
LITE_TEST_SUIT(security, securityData, HksBnExpModTest);

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
static BOOL HksBnExpModTestSetUp()
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
    LiteTestPrint("HksBnExpModTestSetUp End2!\n");
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksBnExpModTestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
}


#define DEFAULT_X_SIZE 256
#define DEFAULT_A_SIZE 256
#define DEFAULT_E_SIZE 256
#define DEFAULT_N_SIZE 256
#define HKS_TEST_2 2
#define HKS_TEST_8 8

static const struct HksTestBnExpModParams g_testBnExpModParams[] = {
    /* normal case */
    { 0, HKS_SUCCESS, false,
        { true, DEFAULT_X_SIZE, true, DEFAULT_X_SIZE },
        { true, DEFAULT_A_SIZE, true, DEFAULT_A_SIZE },
        { true, DEFAULT_E_SIZE, true, DEFAULT_E_SIZE },
        { true, DEFAULT_N_SIZE, true, DEFAULT_N_SIZE }
    },
};

static int32_t TestValue()
{
    HKS_TEST_LOG_I("test value");
    uint8_t bufX[HKS_TEST_8] = { 0, 0, 0, 0, 0, 0, 0, 0x40 };
    uint8_t bufA[HKS_TEST_8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t bufE[HKS_TEST_2] = { 0, 2 };
    uint8_t bufN[HKS_TEST_8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    uint8_t tmpBufX[HKS_TEST_8] = {0};
    struct HksBlob tmpX = { HKS_TEST_8, tmpBufX };
    struct HksBlob tmpA = { HKS_TEST_8, bufA };
    struct HksBlob tmpE = { HKS_TEST_2, bufE };
    struct HksBlob tmpN = { HKS_TEST_8, bufN };
    int32_t ret = HksBnExpModRun(&tmpX, &tmpA, &tmpE, &tmpN, 1);
    for (int i = 0; i < HKS_TEST_8; ++i) {
        HKS_TEST_LOG_I("%x, %x", tmpBufX[i], bufX[i]);
        HKS_TEST_ASSERT(tmpBufX[i] == bufX[i]);
    }
    return ret;
}

/**
 * @tc.name: HksBnExpModTest.HksBnExpModTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksBnExpModTest, HksBnExpModTest001, Level1)
{
    int32_t ret;
    if (g_testBnExpModParams[0].isTestValue) {
        ret = TestValue();
        TEST_ASSERT_TRUE(ret == 0);
    } else {
        struct HksBlob *x = NULL;
        struct HksBlob *a = NULL;
        struct HksBlob *e = NULL;
        struct HksBlob *n = NULL;
        ret = TestConstuctBlob(&x, g_testBnExpModParams[0].xParams.blobExist,
            g_testBnExpModParams[0].xParams.blobSize, g_testBnExpModParams[0].xParams.blobDataExist,
            g_testBnExpModParams[0].xParams.blobDataSize);
        TEST_ASSERT_TRUE(ret == 0);

        ret = TestConstructBlobOut(&a, g_testBnExpModParams[0].aParams.blobExist,
            g_testBnExpModParams[0].aParams.blobSize,  g_testBnExpModParams[0].aParams.blobDataExist,
            g_testBnExpModParams[0].aParams.blobDataSize);
        TEST_ASSERT_TRUE(ret == 0);

        ret = TestConstuctBlob(&e, g_testBnExpModParams[0].eParams.blobExist,
            g_testBnExpModParams[0].eParams.blobSize, g_testBnExpModParams[0].eParams.blobDataExist,
            g_testBnExpModParams[0].eParams.blobDataSize);
        TEST_ASSERT_TRUE(ret == 0);

        ret = TestConstuctBlob(&n, g_testBnExpModParams[0].nParams.blobExist,
            g_testBnExpModParams[0].nParams.blobSize, g_testBnExpModParams[0].nParams.blobDataExist,
            g_testBnExpModParams[0].nParams.blobDataSize);
        TEST_ASSERT_TRUE(ret == 0);
        if ((n != NULL) && (n->data != NULL) && (n->size != 0)) {
            n->data[n->size - 1] = n->data[n->size - 1] | 0x00000001; /* make sure n is odd */
        }

        ret = HksBnExpModRun(x, a, e, n, 1);

        TEST_ASSERT_TRUE(ret == g_testBnExpModParams[0].expectResult);

        TestFreeBlob(&x);
        TestFreeBlob(&a);
        TestFreeBlob(&e);
        TestFreeBlob(&n);
        TEST_ASSERT_TRUE(ret == 0);
    }
}

RUN_TEST_SUITE(HksBnExpModTest);
