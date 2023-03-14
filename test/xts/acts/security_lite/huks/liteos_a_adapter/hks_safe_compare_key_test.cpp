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

#include "hks_safe_compare_key_test.h"

#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_file_operator.h"
#include "hks_test_log.h"
#include "hks_test_mem.h"

using namespace testing::ext;
namespace {
#ifndef _CUT_AUTHENTICATE_
class HksSafeCompareKeyTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksSafeCompareKeyTest::SetUpTestCase(void)
{
}

void HksSafeCompareKeyTest::TearDownTestCase(void)
{
}

void HksSafeCompareKeyTest::SetUp()
{
    EXPECT_EQ(HksInitialize(), 0);
}

void HksSafeCompareKeyTest::TearDown()
{
}

const char *g_storePath = "/storage/maindata/hks_client/key/";
const char *g_testOne = "TestOne";
const char *g_testTwo = "TestTwo";

const struct HksTestGenKeyParams g_testGenKeyParams[] = {
    /* x25519: tee sign/verify */
    { 0, HKS_SUCCESS, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true,
            true, HKS_ALG_X25519,
            true, HKS_CURVE25519_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
            false, 0,
            false, 0,
            false, 0,
            false, 0
        },
        { false, 0 },
    },
};

static int32_t SafeTestGenerateKey(struct HksBlob *keyAlias)
{
    uint32_t index = 0;
    uint32_t performTimes = 1;

    struct HksParamSet *paramSet = NULL;
    struct GenerateKeyParamSetStructure paramStruct = {
        &paramSet,
        g_testGenKeyParams[index].paramSetParams.paramSetExist,
        g_testGenKeyParams[index].paramSetParams.setAlg, g_testGenKeyParams[index].paramSetParams.alg,
        g_testGenKeyParams[index].paramSetParams.setKeySize, g_testGenKeyParams[index].paramSetParams.keySize,
        g_testGenKeyParams[index].paramSetParams.setPurpose, g_testGenKeyParams[index].paramSetParams.purpose,
        g_testGenKeyParams[index].paramSetParams.setDigest, g_testGenKeyParams[index].paramSetParams.digest,
        g_testGenKeyParams[index].paramSetParams.setPadding, g_testGenKeyParams[index].paramSetParams.padding,
        g_testGenKeyParams[index].paramSetParams.setBlockMode, g_testGenKeyParams[index].paramSetParams.mode,
        g_testGenKeyParams[index].paramSetParams.setKeyStorageFlag,
        g_testGenKeyParams[index].paramSetParams.keyStorageFlag
    };
    int32_t ret = TestConstructGenerateKeyParamSet(&paramStruct);
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
    EXPECT_EQ(ret, g_testGenKeyParams[index].expectResult);

    if (ret == g_testGenKeyParams[index].expectResult) {
        ret = 0;
    } else {
        ret = 1;
    }
    HksFreeParamSet(&paramSet);
    HksFreeParamSet(&paramSetOut);
    return ret;
}

static int32_t CompareKeyData(struct HksBlob *keyAliasOne, struct HksBlob *keyAliasTwo)
{
    uint32_t sizeOne = HksTestFileSize(g_storePath, (char *)keyAliasOne->data);
    uint8_t *bufOne = (uint8_t *)HksTestMalloc(sizeOne);
    if (bufOne == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    uint32_t sizeRead = HksTestFileRead(g_storePath, (char *)keyAliasOne->data, 0, bufOne, sizeOne);

    uint32_t sizeTwo = HksTestFileSize(g_storePath, (char *)keyAliasTwo->data);
    uint8_t *bufTwo = (uint8_t *)HksTestMalloc(sizeTwo);
    if (bufTwo == NULL) {
        HksTestFree(bufOne);
        return HKS_ERROR_MALLOC_FAIL;
    }
    sizeRead = HksTestFileRead(g_storePath, (char *)keyAliasTwo->data, 0, bufTwo, sizeOne);
    int32_t ret = memcmp(bufOne, bufTwo, sizeOne);
    HksTestFree(bufOne);
    HksTestFree(bufTwo);
    return ret;
}

/**
 * @tc.name: HksSafeCompareKeyTest.HksSafeCompareKeyTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksSafeCompareKeyTest, HksSafeCompareKeyTest001, TestSize.Level1)
{
    struct HksBlob keyAliasOne = { strlen(g_testOne), (uint8_t *)g_testOne };
    int32_t ret = SafeTestGenerateKey(&keyAliasOne);
    EXPECT_EQ(ret, 0);

    struct HksBlob keyAliasTwo = { strlen(g_testTwo), (uint8_t *)g_testTwo };
    ret = SafeTestGenerateKey(&keyAliasTwo);
    EXPECT_EQ(ret, 0);

    ret = CompareKeyData(&keyAliasOne, &keyAliasTwo);
    EXPECT_NE(ret, 0);
}
#endif /* _CUT_AUTHENTICATE_ */
}