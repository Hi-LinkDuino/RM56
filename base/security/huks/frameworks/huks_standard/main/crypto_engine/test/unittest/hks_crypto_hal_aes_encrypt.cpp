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
    HksUsageSpec usageSpec = {0};
    std::string keyData;
    std::string hexData;
    HksStageType runStage = HksStageType::HKS_STAGE_THREE;

    HksErrorCode encryptResult = HksErrorCode::HKS_SUCCESS;
};

const uint32_t IV_SIZE = 16;
const uint32_t AEAD_SIZE = 16;
uint8_t IV[IV_SIZE] = {0};
HksCipherParam TAG_IV = { .iv = { .size = IV_SIZE, .data = IV } };
HksAeadParam AEAD_PARAM = {
    .nonce = { .size = IV_SIZE, .data = IV },
    .aad = { .size = 0, .data = nullptr },
    .tagLenEnc = IV_SIZE,
    .payloadLen = 0,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_001_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_002_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_003_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CTR,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_004_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_005_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "00112233445566778899aabbccddeeff",
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_006_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_GCM,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &AEAD_PARAM,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_007_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_008_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_009_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CTR,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_010_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_011_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "00112233445566778899aabbccddeeff",
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_012_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_GCM,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &AEAD_PARAM,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_013_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_014_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_015_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CTR,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_016_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_017_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_AES_ENCRYPT_018_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_GCM,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &AEAD_PARAM,
    },
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .encryptResult = HKS_SUCCESS,
};
}  // namespace

class HksCryptoHalAesEncrypt : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    void RunTestEncrypt(struct HksBlob *key, const TestCaseParams &testCaseParams, struct HksBlob *encryptMsg,
        struct HksBlob *encryptOut, struct HksBlob *tagAead) const
    {
        void *encryptCtx = (void *)HksMalloc(HKS_CONTEXT_DATA_MAX);
        ASSERT_EQ(encryptCtx == nullptr, false) << "encryptCtx malloc failed.";
        uint32_t inLen = encryptMsg->size;

        EXPECT_EQ(HksCryptoHalEncryptInit(key, &testCaseParams.usageSpec, &encryptCtx), testCaseParams.encryptResult);
        if (testCaseParams.encryptResult != HKS_SUCCESS) {
            return;
        }
        uint32_t point = 0;
        if (inLen > HKS_UPDATE_DATA_MAX) {
            HksBlob messageUpdate = { .size = HKS_UPDATE_DATA_MAX, .data = (uint8_t *)HksMalloc(HKS_UPDATE_DATA_MAX) };
            HksBlob out = { .size = HKS_UPDATE_DATA_MAX, .data = (uint8_t *)HksMalloc(HKS_UPDATE_DATA_MAX) };
            ASSERT_EQ(messageUpdate.data == nullptr, false) << "messageUpdate malloc failed.";
            ASSERT_EQ(out.data == nullptr, false) << "out malloc failed.";

            while (point < inLen - HKS_UPDATE_DATA_MAX) {
                EXPECT_EQ(memcpy_s(messageUpdate.data, messageUpdate.size,
                    (encryptMsg->data + point), HKS_UPDATE_DATA_MAX), EOK) << "memcpy fail";
                EXPECT_EQ(HksCryptoHalEncryptUpdate(&messageUpdate, encryptCtx, &out, testCaseParams.usageSpec.algType),
                    testCaseParams.encryptResult) << "HksCryptoHalEncryptUpdate failed.";
                point = point + HKS_UPDATE_DATA_MAX;
            }
            HksFree(out.data);
            HksFree(messageUpdate.data);

            uint32_t lastLen = inLen - point;
            HksBlob enMessageLast = { .size = lastLen, .data = (uint8_t *)HksMalloc(lastLen) };
            HksBlob enMessageLastOut = {
                .size = HKS_UPDATE_DATA_MAX + HKS_PADDING_SUPPLENMENT,
                .data = (uint8_t *)HksMalloc(HKS_UPDATE_DATA_MAX + HKS_PADDING_SUPPLENMENT)
            };
            ASSERT_EQ(enMessageLast.data == nullptr, false) << "enMessageLast malloc failed.";
            ASSERT_EQ(enMessageLastOut.data == nullptr, false) << "enMessageLastOut malloc failed.";
            (void)memcpy_s(enMessageLast.data, lastLen, (encryptMsg->data + point), lastLen);
            EXPECT_EQ(HksCryptoHalEncryptFinal(&enMessageLast, &encryptCtx, &enMessageLastOut, tagAead,
                testCaseParams.usageSpec.algType), testCaseParams.encryptResult) << "HksCryptoHalEncryptFinal failed.";

            HksFree(enMessageLast.data);
            HksFree(enMessageLastOut.data);
        } else {
            HksBlob out = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
            ASSERT_EQ(out.data == nullptr, false) << "out malloc failed.";
            EXPECT_EQ(HksCryptoHalEncryptUpdate(encryptMsg, encryptCtx, &out, testCaseParams.usageSpec.algType),
                testCaseParams.encryptResult) << "HksCryptoHalEncryptUpdate failed.";
            HksFree(out.data);

            HksBlob enMessageLast = { .size = 0, .data = nullptr };
            EXPECT_EQ(HksCryptoHalEncryptFinal(&enMessageLast, &encryptCtx, encryptOut, tagAead,
                testCaseParams.usageSpec.algType), testCaseParams.encryptResult) << "HksCryptoHalEncryptFinal failed.";
        }
    }

    void RunTestCase(const TestCaseParams &testCaseParams) const
    {
        uint32_t keyLen = testCaseParams.keyData.length() / HKS_COUNT_OF_HALF;
        HksBlob key = { .size = keyLen, .data = (uint8_t *)HksMalloc(keyLen) };
        ASSERT_EQ(key.data == nullptr, false) << "key malloc failed.";
        for (uint32_t ii = 0; ii < keyLen; ii++) {
            key.data[ii] = ReadHex((const uint8_t *)&testCaseParams.keyData[HKS_COUNT_OF_HALF * ii]);
        }

        uint32_t inLen = testCaseParams.hexData.length() / HKS_COUNT_OF_HALF;
        uint32_t outLen = inLen;
        if (testCaseParams.usageSpec.padding == HKS_PADDING_PKCS7) {
            outLen = (inLen + HKS_PADDING_SUPPLENMENT) / HKS_PADDING_SUPPLENMENT * HKS_PADDING_SUPPLENMENT;
        }
        HksBlob message = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_EQ(message.data == nullptr, false) << "message malloc failed.";
        for (uint32_t ii = 0; ii < inLen; ii++) {
            message.data[ii] = ReadHex((const uint8_t *)&testCaseParams.hexData[HKS_COUNT_OF_HALF * ii]);
        }
        HksBlob cipherText = { .size = outLen, .data = (uint8_t *)HksMalloc(outLen + HKS_PADDING_SUPPLENMENT) };
        HksBlob tagAead = { .size = AEAD_SIZE, .data = (uint8_t *)HksMalloc(AEAD_SIZE) };
        ASSERT_EQ(cipherText.data == nullptr, false) << "cipherText malloc failed.";
        ASSERT_EQ(tagAead.data == nullptr, false) << "tagAead malloc failed.";

        if (testCaseParams.runStage == HksStageType::HKS_STAGE_THREE) {
            RunTestEncrypt(&key, testCaseParams, &message, &cipherText, &tagAead);
        } else {
            EXPECT_EQ(HksCryptoHalEncrypt(&key, &testCaseParams.usageSpec, &message, &cipherText, &tagAead),
                testCaseParams.encryptResult);
        }

        HksFree(key.data);
        HksFree(message.data);
        HksFree(cipherText.data);
        HksFree(tagAead.data);
    }
};

void HksCryptoHalAesEncrypt::SetUpTestCase(void)
{
}

void HksCryptoHalAesEncrypt::TearDownTestCase(void)
{
}

void HksCryptoHalAesEncrypt::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalAesEncrypt::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_001
 * @tc.name      : HksCryptoHalAesEncrypt_001
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-128-CBC-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_001, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_002
 * @tc.name      : HksCryptoHalAesEncrypt_002
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-128-CBC-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_002, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_003
 * @tc.name      : HksCryptoHalAesEncrypt_003
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-128-CTR-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_003, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_003_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_004
 * @tc.name      : HksCryptoHalAesEncrypt_004
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-128-ECB-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_004, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_004_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_005
 * @tc.name      : HksCryptoHalAesEncrypt_005
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-128-ECB-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_005, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_005_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_006
 * @tc.name      : HksCryptoHalAesEncrypt_006
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-128-GCM-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_006, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_006_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_007
 * @tc.name      : HksCryptoHalAesEncrypt_007
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-192-CBC-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_007, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_007_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_008
 * @tc.name      : HksCryptoHalAesEncrypt_008
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-192-CBC-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_008, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_008_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_009
 * @tc.name      : HksCryptoHalAesEncrypt_009
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-192-CTR-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_009, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_009_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_010
 * @tc.name      : HksCryptoHalAesEncrypt_010
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-192-ECB-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_010, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_010_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_011
 * @tc.name      : HksCryptoHalAesEncrypt_011
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-192-ECB-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_011, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_011_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_012
 * @tc.name      : HksCryptoHalAesEncrypt_012
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-192-GCM-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_012, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_012_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_013
 * @tc.name      : HksCryptoHalAesEncrypt_013
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-256-CBC-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_013, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_013_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_014
 * @tc.name      : HksCryptoHalAesEncrypt_014
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-256-CBC-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_014, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_014_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_015
 * @tc.name      : HksCryptoHalAesEncrypt_015
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-256-CTR-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_015, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_015_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_016
 * @tc.name      : HksCryptoHalAesEncrypt_016
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-256-ECB-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_016, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_016_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_017
 * @tc.name      : HksCryptoHalAesEncrypt_017
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-256-ECB-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_017, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_017_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesEncrypt_018
 * @tc.name      : HksCryptoHalAesEncrypt_018
 * @tc.desc      : Using HksCryptoHalEncrypt Encrypt AES-256-GCM-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesEncrypt, HksCryptoHalAesEncrypt_018, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_ENCRYPT_018_PARAMS);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS