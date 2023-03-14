/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <iostream>

#include "hks_ability.h"
#include "hks_config.h"
#include "hks_crypto_hal.h"
#include "hks_crypto_hal_common.h"
#include "hks_mem.h"

#ifdef HKS_SUPPORT_DH_C

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace UnitTest {
namespace {
struct TestCaseParams {
    HksKeySpec spec = {0};

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode agreeResult = HksErrorCode::HKS_SUCCESS;
};

const uint32_t ALISE_KEY_SIZE = 4096;
const uint32_t BOB_KEY_SIZE = 4096;
const TestCaseParams HKS_CRYPTO_HAL_DH_AGREE_001_PARAMS = {
    .spec = {
        .algType = HKS_ALG_DH,
        .keyLen = HKS_DH_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_DH_AGREE_002_PARAMS = {
    .spec = {
        .algType = HKS_ALG_DH,
        .keyLen = HKS_DH_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_DH_AGREE_003_PARAMS = {
    .spec = {
        .algType = HKS_ALG_DH,
        .keyLen = HKS_DH_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};
}  // namespace

class HksCryptoHalDhAgree : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    void RunTestCase(const TestCaseParams &testCaseParams) const
    {
        HksBlob alise = { .size = 0, .data = nullptr };
        HksBlob bob = { .size = 0, .data = nullptr };

        EXPECT_EQ(HksCryptoHalGenerateKey(&testCaseParams.spec, &alise), HKS_SUCCESS);
        EXPECT_EQ(HksCryptoHalGenerateKey(&testCaseParams.spec, &bob), HKS_SUCCESS);

        struct HksBlob pubKeyAlise = { .size = ALISE_KEY_SIZE, .data = (uint8_t *)HksMalloc(ALISE_KEY_SIZE) };
        ASSERT_NE(pubKeyAlise.data, nullptr);
        struct HksBlob pubKeyBob = { .size = BOB_KEY_SIZE, .data = (uint8_t *)HksMalloc(BOB_KEY_SIZE) };
        ASSERT_NE(pubKeyBob.data, nullptr);

        EXPECT_EQ(HksCryptoHalGetPubKey(&alise, &pubKeyAlise), HKS_SUCCESS);
        EXPECT_EQ(HksCryptoHalGetPubKey(&bob, &pubKeyBob), HKS_SUCCESS);

        struct HksBlob agreeKeyAlise = { .size = ALISE_KEY_SIZE, .data = (uint8_t *)HksMalloc(ALISE_KEY_SIZE) };
        ASSERT_NE(agreeKeyAlise.data, nullptr);
        struct HksBlob agreeKeyBob = { .size = BOB_KEY_SIZE, .data = (uint8_t *)HksMalloc(BOB_KEY_SIZE) };
        ASSERT_NE(agreeKeyBob.data, nullptr);

        EXPECT_EQ(HksCryptoHalAgreeKey(&alise, &pubKeyBob, &testCaseParams.spec, &agreeKeyAlise), HKS_SUCCESS);
        EXPECT_EQ(HksCryptoHalAgreeKey(&bob, &pubKeyAlise, &testCaseParams.spec, &agreeKeyBob), HKS_SUCCESS);

        EXPECT_EQ(agreeKeyAlise.size, agreeKeyBob.size);
        EXPECT_EQ(HksMemCmp(agreeKeyAlise.data, agreeKeyBob.data, agreeKeyAlise.size), HKS_SUCCESS);

        HksFree(alise.data);
        HksFree(bob.data);
        HksFree(pubKeyAlise.data);
        HksFree(pubKeyBob.data);
        HksFree(agreeKeyAlise.data);
        HksFree(agreeKeyBob.data);
    }
};

void HksCryptoHalDhAgree::SetUpTestCase(void)
{
}

void HksCryptoHalDhAgree::TearDownTestCase(void)
{
}

void HksCryptoHalDhAgree::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalDhAgree::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalDhAgree_001
 * @tc.name      : HksCryptoHalDhAgree_001
 * @tc.desc      : Using HksCryptoHalAgreeKey Agree DH-2048 key.
 */
HWTEST_F(HksCryptoHalDhAgree, HksCryptoHalDhAgree_001, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DH_AGREE_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalDhAgree_002
 * @tc.name      : HksCryptoHalDhAgree_002
 * @tc.desc      : Using HksCryptoHalAgreeKey Agree DH-3072 key.
 */
HWTEST_F(HksCryptoHalDhAgree, HksCryptoHalDhAgree_002, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DH_AGREE_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalDhAgree_003
 * @tc.name      : HksCryptoHalDhAgree_003
 * @tc.desc      : Using HksCryptoHalAgreeKey Agree DH-4096 key.
 */
HWTEST_F(HksCryptoHalDhAgree, HksCryptoHalDhAgree_003, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DH_AGREE_003_PARAMS);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS
#endif