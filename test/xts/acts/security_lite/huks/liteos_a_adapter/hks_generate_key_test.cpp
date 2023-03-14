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

#include <gtest/gtest.h>

#include "hks_generate_key_test.h"

#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"

using namespace testing::ext;
namespace {
#ifndef _CUT_AUTHENTICATE_
class HksGenerateKeyTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksGenerateKeyTest::SetUpTestCase(void)
{
}

void HksGenerateKeyTest::TearDownTestCase(void)
{
}

void HksGenerateKeyTest::SetUp()
{
    EXPECT_EQ(HksInitialize(), 0);
}

void HksGenerateKeyTest::TearDown()
{
}

const int DEFAULT_X25519_PARAM_SET_OUT = 104;

const struct HksTestGenKeyParams g_testGenKeyParams[] = {
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

/**
 * @tc.name: HksGenerateKeyTest.HksGenerateKeyTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksGenerateKeyTest, HksGenerateKeyTest001, TestSize.Level1)
{
    uint32_t times = 1;
    uint32_t index = 0;
    uint32_t performTimes = 1;
    struct HksBlob *keyAlias = NULL;
    int32_t ret = TestConstuctBlob(&keyAlias,
        g_testGenKeyParams[index].keyAliasParams.blobExist,
        g_testGenKeyParams[index].keyAliasParams.blobSize,
        g_testGenKeyParams[index].keyAliasParams.blobDataExist,
        g_testGenKeyParams[index].keyAliasParams.blobDataSize);
    HKS_TEST_ASSERT(ret == 0);

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
    HKS_TEST_ASSERT(ret == 0);

    struct HksParamSet *paramSetOut = NULL;
    ret = TestConstructGenerateKeyParamSetOut(&paramSetOut,
        g_testGenKeyParams[index].paramSetParamsOut.paramSetExist,
        g_testGenKeyParams[index].paramSetParamsOut.paramSetSize);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksGenerateKeyRun(keyAlias, paramSet, paramSetOut, performTimes);
    if (ret != g_testGenKeyParams[index].expectResult) {
        HKS_TEST_LOG_I("failed, ret[%u] = %d", g_testGenKeyParams[index].testId, ret);
    }
    HKS_TEST_ASSERT(ret == g_testGenKeyParams[index].expectResult);

    if ((ret == HKS_SUCCESS) &&
        !(g_testGenKeyParams[index].paramSetParams.setKeyStorageFlag == true) &&
        (g_testGenKeyParams[index].paramSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) {
        HKS_TEST_ASSERT(HksDeleteKey(keyAlias, NULL) == 0);
    }
    TestFreeBlob(&keyAlias);
    HksFreeParamSet(&paramSet);
    HksFreeParamSet(&paramSetOut);
    HKS_TEST_LOG_I("[%u]TestGenerateKey, Testcase_GenerateKey_[%03u] pass!", times, g_testGenKeyParams[index].testId);
    ASSERT_TRUE(ret == 0);
}
#endif /* _CUT_AUTHENTICATE_ */
}