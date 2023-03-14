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
};

const TestCaseParams HKS_CRYPTO_HAL_DH_001_PARAMS = {
    .spec = { .algType = HKS_ALG_DH, .keyLen = HKS_DH_KEY_SIZE_2048, },
    .generateKeyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_DH_002_PARAMS = {
    .spec = { .algType = HKS_ALG_DH, .keyLen = HKS_DH_KEY_SIZE_3072, },
    .generateKeyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_DH_003_PARAMS = {
    .spec = { .algType = HKS_ALG_DH, .keyLen = HKS_DH_KEY_SIZE_4096, },
    .generateKeyResult = HKS_SUCCESS,
};
}  // namespace

class HksCryptoHalDh : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    void RunTestCase(const TestCaseParams &testCaseParams) const
    {
        HksBlob key = { 0, NULL };
        EXPECT_EQ(HksCryptoHalGenerateKey(&testCaseParams.spec, &key), testCaseParams.generateKeyResult);
        HKS_FREE_BLOB(key);
    }
};

void HksCryptoHalDh::SetUpTestCase(void)
{
}

void HksCryptoHalDh::TearDownTestCase(void)
{
}

void HksCryptoHalDh::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalDh::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalDh_001
 * @tc.name      : HksCryptoHalDh_001
 * @tc.desc      : Generate Dh-2048 key pair
 */
HWTEST_F(HksCryptoHalDh, HksCryptoHalDh_001, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DH_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalDh_002
 * @tc.name      : HksCryptoHalDh_002
 * @tc.desc      : Generate Dh-3072 key pair
 */
HWTEST_F(HksCryptoHalDh, HksCryptoHalDh_002, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DH_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalDh_003
 * @tc.name      : HksCryptoHalDh_003
 * @tc.desc      : Generate Dh-4096 key pair
 */
HWTEST_F(HksCryptoHalDh, HksCryptoHalDh_003, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DH_003_PARAMS);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS
#endif