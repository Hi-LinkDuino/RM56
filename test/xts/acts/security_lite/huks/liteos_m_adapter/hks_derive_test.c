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

#include "iot_watchdog.h"
#include <hctest.h>

#include "hks_derive_test.h"

#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"
#include "cmsis_os2.h"
#include "ohos_types.h"

#define DEFAULT_DERIVE_SIZE 32
#define DEFAULT_INFO_SIZE 55
#define DEFAULT_SALT_SIZE 16
#define TEST_TASK_STACK_SIZE      0x2000
#define WAIT_TO_TEST_DONE         4

static osPriority_t g_setPriority;

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(security, securityData, HksDeriveTest);

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
static BOOL HksDeriveTestSetUp()
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
    LiteTestPrint("HksDeriveTestSetUp End2!\n");
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksDeriveTestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
}


static const struct HksTestDeriveParams g_testDeriveParams[] = {
    /* hkdf-sha256-salt-info */
    { 0, HKS_SUCCESS, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_DERIVE,
            true, HKS_DIGEST_SHA256,
            false, 0,
            false, 0,
            false, 0 },
        { 0 },
        {
            true, /* derive params */
            true, HKS_ALG_HKDF,
            true, HKS_KEY_PURPOSE_DERIVE,
            true, HKS_DIGEST_SHA256,
            false, 0,
            true, DEFAULT_SALT_SIZE,
            true, DEFAULT_INFO_SIZE,
            false, true },
        {
            true, DEFAULT_DERIVE_SIZE, true, DEFAULT_DERIVE_SIZE },
        {
            false, 0, false, 0 }
    },

    /* local: hkdf-sha256-salt-info */
    { 1, HKS_SUCCESS, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_DERIVE,
            true, HKS_DIGEST_SHA256,
            false, 0,
            false, 0,
            true, HKS_STORAGE_TEMP },
        { 0 },
        {
            true, /* derive params */
            true, HKS_ALG_HKDF,
            true, HKS_KEY_PURPOSE_DERIVE,
            true, HKS_DIGEST_SHA256,
            false, 0,
            true, DEFAULT_SALT_SIZE,
            true, DEFAULT_INFO_SIZE,
            true, false },
        {
            true, DEFAULT_DERIVE_SIZE, true, DEFAULT_DERIVE_SIZE },
        {
            true, DEFAULT_LOCAL_KEY_SIZE, true, DEFAULT_LOCAL_KEY_SIZE }
    },
};

static int32_t DeriveKey(const struct HksTestDeriveParamSet *deriveParamSetParams, const struct HksBlob *masterKey,
    struct HksBlob *derivedKey, struct HksBlob **saltData, struct HksBlob **infoData)
{
    struct HksParamSet *deriveParamSet = NULL;
    uint32_t saltSize = deriveParamSetParams->saltSize;
    uint32_t infoSize = deriveParamSetParams->infoSize;
    if (saltSize != 0) {
        HKS_TEST_ASSERT(TestConstuctBlob(saltData, true, saltSize, true, saltSize) == 0);
    }
    if (infoSize != 0) {
        HKS_TEST_ASSERT(TestConstuctBlob(infoData, true, infoSize, true, infoSize) == 0);
    }
    struct TestDeriveParamSetStructure paramStruct = {
        &deriveParamSet,
        deriveParamSetParams->paramSetExist,
        deriveParamSetParams->setAlg, deriveParamSetParams->alg,
        deriveParamSetParams->setPurpose, deriveParamSetParams->purpose,
        deriveParamSetParams->setDigest, deriveParamSetParams->digest,
        deriveParamSetParams->setIteration, deriveParamSetParams->iteration,
        deriveParamSetParams->setSalt, *saltData,
        deriveParamSetParams->setInfo, *infoData,
        deriveParamSetParams->setIsKeyAlias, deriveParamSetParams->isKeyAlias
    };
    int32_t ret = TestConstructDeriveParamSet(&paramStruct);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksDeriveKeyRun(deriveParamSet, masterKey, derivedKey, 1);
    HksFreeParamSet(&deriveParamSet);
    return ret;
}

static int32_t BaseTestDerive(uint32_t index)
{
    /* 1. generate key */
    struct HksBlob *keyAlias = NULL;
    int32_t ret;
    if (g_testDeriveParams[index].genKeyParamSetParams.setKeyStorageFlag &&
        (g_testDeriveParams[index].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) {
        ret = GenerateLocalRandomKey(&keyAlias, &g_testDeriveParams[index].localKeyParams);
    } else {
        if (g_testDeriveParams[index].keyAliasParams.blobExist) {
            ret = GenerateKey(&keyAlias, &g_testDeriveParams[index].keyAliasParams,
                &g_testDeriveParams[index].genKeyParamSetParams, NULL);
        } else {
            ret = TestConstuctBlob(&keyAlias,
                g_testDeriveParams[index].masterKeyParams.blobExist,
                g_testDeriveParams[index].masterKeyParams.blobSize,
                g_testDeriveParams[index].masterKeyParams.blobDataExist,
                g_testDeriveParams[index].masterKeyParams.blobDataSize);
        }
    }
    TEST_ASSERT_TRUE(ret == 0);

    /* 2. derive */
    struct HksBlob *derivedKey = NULL;
    ret = TestConstuctBlob(&derivedKey,
        g_testDeriveParams[index].derivedKeyParams.blobExist,
        g_testDeriveParams[index].derivedKeyParams.blobSize,
        g_testDeriveParams[index].derivedKeyParams.blobDataExist,
        g_testDeriveParams[index].derivedKeyParams.blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);

    struct HksBlob *saltData = NULL;
    struct HksBlob *infoData = NULL;
    ret = DeriveKey(&g_testDeriveParams[index].deriveParamSetParams, keyAlias, derivedKey, &saltData, &infoData);
    if (ret != g_testDeriveParams[index].expectResult) {
        HKS_TEST_LOG_I("failed, ret[%u] = %d", g_testDeriveParams[index].testId, ret);
    }
    TEST_ASSERT_TRUE(ret == g_testDeriveParams[index].expectResult);

    /* 3. delete key */
    if (!(g_testDeriveParams[index].genKeyParamSetParams.setKeyStorageFlag &&
        (g_testDeriveParams[index].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) &&
        (g_testDeriveParams[index].keyAliasParams.blobExist)) {
        TEST_ASSERT_TRUE(HksDeleteKey(keyAlias, NULL) == 0);
    }
    TestFreeBlob(&keyAlias);
    TestFreeBlob(&derivedKey);
    TestFreeBlob(&saltData);
    TestFreeBlob(&infoData);

    return ret;
}

static void ExecHksDeriveTest001(void const *argument)
{
    LiteTestPrint("HksDeriveTest001 Begin!\n");
    int32_t ret = BaseTestDerive(0);
    TEST_ASSERT_TRUE(ret == 0);
    LiteTestPrint("HksDeriveTest001 End!\n");
    osThreadExit();
}

static void ExecHksDeriveTest002(void const *argument)
{
    LiteTestPrint("HksDeriveTest002 Begin!\n");
    int32_t ret = BaseTestDerive(1);
    TEST_ASSERT_TRUE(ret == 0);
    LiteTestPrint("HksDeriveTest002 End!\n");
    osThreadExit();
}

#ifndef _CUT_AUTHENTICATE_
/**
 * @tc.name: HksDeriveTest.HksDeriveTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksDeriveTest, HksDeriveTest001, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksDeriveTest001, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksDeriveTest001 End2!\n");
}
#endif /* _CUT_AUTHENTICATE_ */

/**
 * @tc.name: HksDeriveTest.HksDeriveTest002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksDeriveTest, HksDeriveTest002, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksDeriveTest002, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksDeriveTest002 End2!\n");
}
RUN_TEST_SUITE(HksDeriveTest);
