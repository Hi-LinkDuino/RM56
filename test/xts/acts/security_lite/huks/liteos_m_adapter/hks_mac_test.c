/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file Execept in compliance with the License.
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

#include "hks_mac_test.h"

#include <hctest.h>
#include "iot_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"

#include "cmsis_os2.h"
#include "ohos_types.h"

#define HKS_TEST_MAC_REE_KEY_SIZE_32 32
#define HKS_DEFAULT_MAC_SRCDATA_SIZE 253
#define HKS_DEFAULT_MAC_SHA256_SIZE 32

#define TEST_TASK_STACK_SIZE      0x2000
#define WAIT_TO_TEST_DONE         4

static osPriority_t g_setPriority;


static const struct HksTestMacParams g_testMacParams[] = {
    /* success: ree-sha256 */
    { 0, HKS_SUCCESS, HKS_TEST_MAC_TYPE_REE, { 0 }, { 0 },
        { true, HKS_TEST_MAC_REE_KEY_SIZE_32, true, HKS_TEST_MAC_REE_KEY_SIZE_32 },
        { true, true, HKS_KEY_PURPOSE_MAC, true, HKS_DIGEST_SHA256 },
        { true, HKS_DEFAULT_MAC_SRCDATA_SIZE, true, HKS_DEFAULT_MAC_SRCDATA_SIZE },
        { true, HKS_DEFAULT_MAC_SHA256_SIZE, true, HKS_DEFAULT_MAC_SHA256_SIZE }
    },

    /* success: tee-sha256 */
    { 1, HKS_SUCCESS, HKS_TEST_MAC_TYPE_TEE, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        { true, true, HKS_ALG_AES, true, HKS_AES_KEY_SIZE_256, true, HKS_KEY_PURPOSE_MAC,
            true, HKS_DIGEST_SHA256, false, 0, false, 0 },
        { 0 },
        { true,  true, HKS_KEY_PURPOSE_MAC, true, HKS_DIGEST_SHA256 },
        { true, HKS_DEFAULT_MAC_SRCDATA_SIZE, true, HKS_DEFAULT_MAC_SRCDATA_SIZE },
        { true, HKS_DEFAULT_MAC_SHA256_SIZE, true, HKS_DEFAULT_MAC_SHA256_SIZE }
    },
};

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(security, securityData, HksMacTest);

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
static BOOL HksMacTestSetUp()
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
    LiteTestPrint("HksMacTestSetUp End2!\n");
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksMacTestTearDown()
{
    LiteTestPrint("tearDown\n");
    IoTWatchDogEnable();
    return TRUE;
}

static int32_t ConstructDataToBlob(struct HksBlob **srcData, struct HksBlob **macData,
    const struct HksTestBlobParams *srcDataParams, const struct HksTestBlobParams *macDataParams)
{
    int32_t ret = TestConstuctBlob(srcData,
        srcDataParams->blobExist,
        srcDataParams->blobSize,
        srcDataParams->blobDataExist,
        srcDataParams->blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);

    ret = TestConstuctBlob(macData,
        macDataParams->blobExist,
        macDataParams->blobSize,
        macDataParams->blobDataExist,
        macDataParams->blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);
    return ret;
}

static int32_t Mac(const struct HksBlob *key, const struct HksBlob *srcData, struct HksBlob *macData,
    const struct HksTestMacParamSet *macParamSetParams, enum HksTestMacType macType)
{
    struct HksParamSet *macParamSet = NULL;
    int32_t ret;
    if (macType == HKS_TEST_MAC_TYPE_REE) {
        struct TestMacParamSetStructure paramStructTrue = {
            &macParamSet,
            macParamSetParams->paramSetExist,
            macParamSetParams->setPurpose, macParamSetParams->purpose,
            macParamSetParams->setDigest, macParamSetParams->digest, true, false
        };
        ret = TestConstructMacParamSet(&paramStructTrue);
    } else {
        struct TestMacParamSetStructure paramStructFalse = {
            &macParamSet,
            macParamSetParams->paramSetExist,
            macParamSetParams->setPurpose, macParamSetParams->purpose,
            macParamSetParams->setDigest, macParamSetParams->digest, false, false
        };
        ret = TestConstructMacParamSet(&paramStructFalse);
    }
    TEST_ASSERT_TRUE(ret == 0);

    ret = HksMacRun(key, macParamSet, srcData, macData, 1);
    HksFreeParamSet(&macParamSet);
    return ret;
}

static int32_t BaseTestMac(uint32_t index)
{
    /* 1. generate key */
    struct HksBlob *key = NULL;
    int32_t ret;

    if (g_testMacParams[index].macType == HKS_TEST_MAC_TYPE_REE) {
        ret = TestConstuctBlob(&key,
            g_testMacParams[index].keyParams.blobExist,
            g_testMacParams[index].keyParams.blobSize,
            g_testMacParams[index].keyParams.blobDataExist,
            g_testMacParams[index].keyParams.blobDataSize);
    } else {
        if (g_testMacParams[index].keyAliasParams.blobExist) {
            ret = GenerateKey(&key, &(g_testMacParams[index].keyAliasParams),
                &g_testMacParams[index].genKeyParamSetParams, NULL);
        } else {
            ret = TestConstuctBlob(&key,
                g_testMacParams[index].keyParams.blobExist,
                g_testMacParams[index].keyParams.blobSize,
                g_testMacParams[index].keyParams.blobDataExist,
                g_testMacParams[index].keyParams.blobDataSize);
        }
    }
    TEST_ASSERT_TRUE(ret == 0);

    /* 2. mac */
    struct HksBlob *srcData = NULL;
    struct HksBlob *macData = NULL;
    ret = ConstructDataToBlob(&srcData, &macData,
        &g_testMacParams[index].srcDataParams, &g_testMacParams[index].macParams);
    TEST_ASSERT_TRUE(ret == 0);

    ret = Mac(key, srcData, macData, &g_testMacParams[index].macParamSetParams, g_testMacParams[index].macType);
    if (ret != g_testMacParams[index].expectResult) {
        HKS_TEST_LOG_I("failed, ret[%u] = %d", g_testMacParams[index].testId, ret);
    }
    TEST_ASSERT_TRUE(ret == g_testMacParams[index].expectResult);

    /* 3. deletekey */
    if ((g_testMacParams[index].macType == HKS_TEST_MAC_TYPE_TEE) &&
        (g_testMacParams[index].keyAliasParams.blobExist)) {
        ret = HksDeleteKey(key, NULL);
        TEST_ASSERT_TRUE(ret == 0);
    }
    TestFreeBlob(&key);
    TestFreeBlob(&srcData);
    TestFreeBlob(&macData);
    return ret;
}

static void ExecHksMacTest001(void const *argument)
{
    LiteTestPrint("HksMacTest001 Begin!\n");
    int32_t ret = BaseTestMac(0);
    TEST_ASSERT_TRUE(ret == 0);
    LiteTestPrint("HksMacTest001 End!\n");
    osThreadExit();
}

static void ExecHksMacTest002(void const *argument)
{
    LiteTestPrint("HksMacTest002 Begin!\n");
    int32_t ret = BaseTestMac(1);
    TEST_ASSERT_TRUE(ret == 0);
    LiteTestPrint("HksMacTest002 End!\n");
    osThreadExit();
}

/**
 * @tc.name: HksMacTest.HksMacTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksMacTest, HksMacTest001, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksMacTest001, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksMacTest001 End2!\n");
}

#ifndef _CUT_AUTHENTICATE_
/**
 * @tc.name: HksMacTest.HksMacTest002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksMacTest, HksMacTest002, Level1)
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
    id = osThreadNew((osThreadFunc_t)ExecHksMacTest002, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksMacTest002 End2!\n");
}
#endif /* _CUT_AUTHENTICATE_ */

RUN_TEST_SUITE(HksMacTest);
