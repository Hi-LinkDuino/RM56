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

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace UnitTest {
const uint32_t HMAC_KEY_SIZE = 256;
class HksCryptoHalHmacKey : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void HksCryptoHalHmacKey::SetUpTestCase(void)
{
}

void HksCryptoHalHmacKey::TearDownTestCase(void)
{
}

void HksCryptoHalHmacKey::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalHmacKey::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalHmacKey_001
 * @tc.name      : HksCryptoHalHmacKey_001
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate HMAC-256bit key.
 */
HWTEST_F(HksCryptoHalHmacKey, HksCryptoHalHmacKey_001, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_HMAC,
        .keyLen = HMAC_KEY_SIZE,
        .algParam = nullptr,
    };

    HksBlob key = { .size = 0, .data = nullptr };

    ret = HksCryptoHalGenerateKey(&spec, &key);
#if defined(HKS_SUPPORT_HMAC_C) && defined(HKS_SUPPORT_HMAC_GENERATE_KEY)
    ASSERT_EQ(HKS_SUCCESS, ret);
    ASSERT_NE((uint32_t)0, key.size);
    ASSERT_NE(nullptr, key.data);
    HksFree(key.data);
#else
    ASSERT_EQ(HKS_ERROR_NOT_SUPPORTED, ret);
#endif
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS