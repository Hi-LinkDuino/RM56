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

#include "hks_generate_key_test.h"

#include <hctest.h>
#include "iot_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"
#include "cmsis_os2.h"
#include "ohos_types.h"

#define DEFAULT_X25519_PARAM_SET_OUT 104
#define TEST_TASK_STACK_SIZE      0x2000
#define WAIT_TO_TEST_DONE         4

static osPriority_t g_setPriority;

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(security, securityData, HksGenerateKeyTest);

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
static BOOL HksGenerateKeyTestSetUp()
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
    LiteTestPrint("HksGenerateKeyTestSetUp End2!\n");
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksGenerateKeyTestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
}


static const struct HksTestGenKeyParams g_testGenKeyParams[] = {
    /* x25519: ree sign/verify */
    { 0, HKS_SUCCESS, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE }, {
        true,
        true, HKS_ALG_X25519,
        true, HKS_CURVE25519_KEY_SIZE_256,
        true, HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
        false, 0,
        false, 0,
        false, 0,
        true, HKS_STORAGE_TEMP },
        { true, DEFAULT_X25519_PARAM_SET_OUT },
    },
};

static void ExecHksGenerateKeyTest001(void const *argument)
{
    LiteTestPrint("HksGenerateKeyTest001 Begin!\n");  

    uint32_t times = 1;
    uint32_t index = 0;
    uint32_t performTimes = 1;
    struct HksBlob *keyAlias = NULL;
    int32_t ret = TestConstuctBlob(&keyAlias,
        g_testGenKeyParams[index].keyAliasParams.blobExist,
        g_testGenKeyParams[index].keyAliasParams.blobSize,
        g_testGenKeyParams[index].keyAliasParams.blobDataExist,
        g_testGenKeyParams[index].keyAliasParams.blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);

    struct HksParamSet *paramSet = NULL;
    struct GenerateKeyParamSetStructure paramStruct = { &paramSet,
        g_testGenKeyParams[index].paramSetParams.paramSetExist,
        g_testGenKeyParams[index].paramSetParams.setAlg, g_testGenKeyParams[index].paramSetParams.alg,
        g_testGenKeyParams[index].paramSetParams.setKeySize, g_testGenKeyParams[index].paramSetParams.keySize,
        g_testGenKeyParams[index].paramSetParams.setPurpose, g_testGenKeyParams[index].paramSetParams.purpose,
        g_testGenKeyParams[index].paramSetParams.setDigest, g_testGenKeyParams[index].paramSetParams.digest,
        g_testGenKeyParams[index].paramSetParams.setPadding, g_testGenKeyParams[index].paramSetParams.padding,
        g_testGenKeyParams[index].paramSetParams.setBlockMode, g_testGenKeyParams[index].paramSetParams.mode,
        g_testGenKeyParams[index].paramSetParams.setKeyStorageFlag,
        g_testGenKeyParams[index].paramSetParams.keyStorageFlag };
    ret = TestConstructGenerateKeyParamSet(&paramStruct);
    TEST_ASSERT_TRUE(ret == 0);

    struct HksParamSet *paramSetOut = NULL;
    ret = TestConstructGenerateKeyParamSetOut(&paramSetOut,
        g_testGenKeyParams[index].paramSetParamsOut.paramSetExist,
        g_testGenKeyParams[index].paramSetParamsOut.paramSetSize);
    TEST_ASSERT_TRUE(ret == 0);

    ret = HksGenerateKeyRun(keyAlias, paramSet, paramSetOut, performTimes);
    if (ret != g_testGenKeyParams[index].expectResult) {
        HKS_TEST_LOG_I("failed, ret[%u] = %d", g_testGenKeyParams[index].testId, ret);
    }
    TEST_ASSERT_TRUE(ret == g_testGenKeyParams[index].expectResult);

    if ((ret == HKS_SUCCESS) &&
        !(g_testGenKeyParams[index].paramSetParams.setKeyStorageFlag == true) &&
        (g_testGenKeyParams[index].paramSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) {
        HKS_TEST_ASSERT(HksDeleteKey(keyAlias, NULL) == 0);
    }
    TestFreeBlob(&keyAlias);
    HksFreeParamSet(&paramSet);
    HksFreeParamSet(&paramSetOut);
    HKS_TEST_LOG_I("[%u]TestGenerateKey, Testcase_GenerateKey_[%03u] pass!", times, g_testGenKeyParams[index].testId);
    TEST_ASSERT_TRUE(ret == 0);
    
    LiteTestPrint("HksGenerateKeyTest001 End!\n");
    osThreadExit();
}

/**
 * @tc.name: HksGenerateKeyTest.HksGenerateKeyTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksGenerateKeyTest, HksGenerateKeyTest001, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksGenerateKeyTest001, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksGenerateKeyTest001 End2!\n");
}

RUN_TEST_SUITE(HksGenerateKeyTest);
#endif /* _CUT_AUTHENTICATE_ */

