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
namespace {
struct TestCaseParams {
    HksKeySpec spec = {0};
    HksUsageSpec usageSpec = {0};
    HksStageType runStage = HksStageType::HKS_STAGE_THREE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode signResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode verifyResult = HksErrorCode::HKS_SUCCESS;
};

const uint32_t SIGNATURE_SIZE = 521;
const uint32_t MAX_PUB_KEY_SIZE = 218;
const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_001_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_224,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_002_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_003_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_384,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_004_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_521,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_005_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_224,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_006_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_007_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_384,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_008_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_521,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_009_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_224,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_010_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_011_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_384,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_012_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_521,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_013_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_224,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_014_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_015_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_384,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_016_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_521,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_017_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_224,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_018_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_019_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_384,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_020_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_521,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_021_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_224,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_022_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_023_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_384,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_ECDSA_SIGN_024_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_521,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_ECC,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};
}  // namespace

class HksCryptoHalEcdsaSign : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    void RunTestCase(const TestCaseParams &testCaseParams) const
    {
        HksBlob key = { .size = 0, .data = nullptr };

        EXPECT_EQ(HksCryptoHalGenerateKey(&testCaseParams.spec, &key), testCaseParams.generateKeyResult);

        const char *hexData = "00112233445566778899aabbccddeeff";
        uint32_t dataLen = strlen(hexData) / HKS_COUNT_OF_HALF;

        HksBlob message = { .size = dataLen, .data = (uint8_t *)HksMalloc(dataLen) };
        ASSERT_NE(message.data, nullptr);
        for (uint32_t ii = 0; ii < dataLen; ii++) {
            message.data[ii] = ReadHex((const uint8_t *)&hexData[HKS_COUNT_OF_HALF * ii]);
        }

        struct HksBlob* pBlob = nullptr;
        uint8_t hashData[HKS_HMAC_DIGEST_SHA512_LEN] = {0};
        struct HksBlob hash = { HKS_HMAC_DIGEST_SHA512_LEN, hashData };
        struct HksUsageSpec usageSpecTmp = testCaseParams.usageSpec;
        if (testCaseParams.runStage == HksStageType::HKS_STAGE_THREE) {
            /* NONEwithECDSA default sha256: ec_pkey_ctrl default md nid */
            uint32_t inputDigest = usageSpecTmp.digest;
            usageSpecTmp.digest = (inputDigest == HKS_DIGEST_NONE) ? HKS_DIGEST_SHA256 : inputDigest;
            EXPECT_EQ(HksCryptoHalHash(usageSpecTmp.digest, &message, &hash), HKS_SUCCESS);
            pBlob = &hash;
        } else {
            pBlob = &message;
        }

        struct HksBlob signature = { .size = SIGNATURE_SIZE, .data = (uint8_t *)HksMalloc(SIGNATURE_SIZE) };
        ASSERT_NE(signature.data, nullptr);

        EXPECT_EQ(HksCryptoHalSign(&key, &usageSpecTmp, pBlob, &signature), testCaseParams.signResult);

        struct HksBlob pubKey = { .size = MAX_PUB_KEY_SIZE, .data = (uint8_t *)HksMalloc(MAX_PUB_KEY_SIZE) };
        ASSERT_NE(pubKey.data, nullptr);

        EXPECT_EQ(HksCryptoHalGetPubKey(&key, &pubKey), HKS_SUCCESS);

        EXPECT_EQ(
            HksCryptoHalVerify(&pubKey, &usageSpecTmp, pBlob, &signature), testCaseParams.verifyResult);

        HksFree(message.data);
        HksFree(signature.data);
        HksFree(pubKey.data);
        HksFree(key.data);
    }
};

void HksCryptoHalEcdsaSign::SetUpTestCase(void)
{
}

void HksCryptoHalEcdsaSign::TearDownTestCase(void)
{
}

void HksCryptoHalEcdsaSign::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalEcdsaSign::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_001
 * @tc.name      : HksCryptoHalEcdsaSign_001
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-224-NONE key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_001, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_002
 * @tc.name      : HksCryptoHalEcdsaSign_002
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-256-NONE key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_002, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_003
 * @tc.name      : HksCryptoHalEcdsaSign_003
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-384-NONE key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_003, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_003_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_004
 * @tc.name      : HksCryptoHalEcdsaSign_004
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-521-NONE key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_004, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_004_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_005
 * @tc.name      : HksCryptoHalEcdsaSign_005
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-224-SHA1 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_005, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_005_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_006
 * @tc.name      : HksCryptoHalEcdsaSign_006
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-256-SHA1 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_006, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_006_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_007
 * @tc.name      : HksCryptoHalEcdsaSign_007
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-384-SHA1 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_007, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_007_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_008
 * @tc.name      : HksCryptoHalEcdsaSign_008
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-521-SHA1 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_008, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_008_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_009
 * @tc.name      : HksCryptoHalEcdsaSign_009
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-224-SHA224 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_009, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_009_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_010
 * @tc.name      : HksCryptoHalEcdsaSign_010
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-256-SHA224 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_010, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_010_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_011
 * @tc.name      : HksCryptoHalEcdsaSign_011
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-384-SHA224 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_011, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_011_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_012
 * @tc.name      : HksCryptoHalEcdsaSign_012
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-521-SHA224 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_012, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_012_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_013
 * @tc.name      : HksCryptoHalEcdsaSign_013
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-224-SHA256 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_013, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_013_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_014
 * @tc.name      : HksCryptoHalEcdsaSign_014
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-256-SHA256 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_014, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_014_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_015
 * @tc.name      : HksCryptoHalEcdsaSign_015
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-384-SHA256 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_015, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_015_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_016
 * @tc.name      : HksCryptoHalEcdsaSign_016
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-521-SHA256 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_016, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_016_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_017
 * @tc.name      : HksCryptoHalEcdsaSign_017
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-224-SHA384 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_017, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_017_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_018
 * @tc.name      : HksCryptoHalEcdsaSign_018
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-256-SHA384 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_018, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_018_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_019
 * @tc.name      : HksCryptoHalEcdsaSign_019
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-384-SHA384 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_019, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_019_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_020
 * @tc.name      : HksCryptoHalEcdsaSign_020
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-521-SHA384 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_020, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_020_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_021
 * @tc.name      : HksCryptoHalEcdsaSign_021
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-224-SHA512 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_021, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_021_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_022
 * @tc.name      : HksCryptoHalEcdsaSign_022
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-256-SHA512 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_022, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_022_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_023
 * @tc.name      : HksCryptoHalEcdsaSign_023
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-384-SHA512 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_023, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_023_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEcdsaSign_024
 * @tc.name      : HksCryptoHalEcdsaSign_024
 * @tc.desc      : Using HksCryptoHalSign Sign ECC-521-SHA512 key.
 */
HWTEST_F(HksCryptoHalEcdsaSign, HksCryptoHalEcdsaSign_024, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECDSA_SIGN_024_PARAMS);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS