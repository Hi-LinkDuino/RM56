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

#include <string>
#include <memory>

#include <gtest/gtest.h>

#include "hks_ability.h"
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

    HksErrorCode decryptResult = HksErrorCode::HKS_SUCCESS;
};
struct TestCaseParamsForGcm {
    std::string keyData;
    std::string hexData;
    std::string tagDec;
    HksStageType runStage = HksStageType::HKS_STAGE_THREE;

    HksErrorCode decryptResult = HksErrorCode::HKS_SUCCESS;
};

const uint32_t IV_SIZE = 16;
uint8_t IV[IV_SIZE] = {0};
HksCipherParam TAG_IV = { .iv = { .size = IV_SIZE, .data = IV } };

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_001_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "2665c033ca1d760d7fcfffce8eb09356a6a6e508584af3faf82fcce81b758934",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_002_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "6b6579306b6579306b6579306b657930",
    .hexData = "a043f74ce2620fc939811e9a949e9f0576c60fb964670ed6e626260f7e6d972aca0d0aeff876a8f03fd8ed8b91ff3f9a",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_003_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CTR,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "2665c033ca1d760d7fcfffce8eb09356a6a6e508584af3faf82fcce81b758934",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_004_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "2665c033ca1d760d7fcfffce8eb09356",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_005_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "2665c033ca1d760d7fcfffce8eb0935670f5531be24753f04e3200829ea2fd60",
#if defined(_USE_OPENSSL_)
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParamsForGcm HKS_CRYPTO_HAL_AES_DECRYPT_006_PARAMS = {
    .keyData = "933c213c1f8c844ffcc03f5f7e146a88",
    .hexData = "fd692103d931c663a0371bd250dd883aed48379774bfba64327cc4a3ea19afae",
    .tagDec = "43a345ff5f3af4bb383f999393949acc",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_007_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "6b5d0d7ac2df878c338f085edf3431116b5d0d7ac2df878c338f085edf343111",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_008_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "6b6579306b6579306b6579306b6579306b6579306b657930",
    .hexData = "7897f888bda5244c049da27f1533dc80accf7a0c8a9169fa64779d1f34be146dcb0f793048dee9efaa032c7901b07b64",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_009_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CTR,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "6b5d0d7ac2df878c338f085edf3431116b5d0d7ac2df878c338f085edf343111",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_010_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "6b5d0d7ac2df878c338f085edf343111",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_011_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "6b5d0d7ac2df878c338f085edf343111cf0afb8fc67acf62db781f324ecc361c",
#if defined(_USE_OPENSSL_)
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParamsForGcm HKS_CRYPTO_HAL_AES_DECRYPT_012_PARAMS = {
    .keyData = "7be3cb2c4c900fb318e9c89f828baf9b91783f47c6e8d088",
    .hexData = "06c34934467ad390f5f54410ac20c875feda920d514c3e43819ce16e1e2e3335",
    .tagDec = "b3ad99cd7951405f1eeefb4e54783f17",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_013_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "f905f3936288181cb1d3931ab26c0314056d1521dc8549013a9b829bebedc71b",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_014_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CBC,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "6b6579306b6579306b6579306b6579306b6579306b6579306b6579306b657930",
    .hexData = "acec21f1488248cdcf6f24a15124b1a68e401efd146cd944fabd84081b76dacf89989a52f8fc1e31dacb820ec8102365",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_015_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_CTR,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = &TAG_IV,
    },
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "f905f3936288181cb1d3931ab26c0314056d1521dc8549013a9b829bebedc71b",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_016_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "f905f3936288181cb1d3931ab26c0314",

    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_AES_DECRYPT_017_PARAMS = {
    .usageSpec = {
        .algType = HKS_ALG_AES,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS7,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .keyData = "6b6579306b6579306b6579306b6579306b6579306b6579306b6579306b657930",
    .hexData = "776a9f622d0978f89fff5c61ae5d4693776a9f622d0978f89fff5c61ae5d469379ea9dc7d7419ec0ea457aa041e1007a",
#if defined(_USE_OPENSSL_)
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParamsForGcm HKS_CRYPTO_HAL_AES_DECRYPT_018_PARAMS = {
    .keyData = "57095bd2ba60c34eaafaa77d694eb809af366810fba500ea660a5048b14b212f",
    .hexData = "da047651955c64daa5cac9b0082f1567235e682041061fef12f896ba83bb1fde",
    .tagDec = "35a956a070d5ed7472a9b66165bcada7",

    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksCryptoHalAesDecrypt : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
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
        ASSERT_EQ(cipherText.data == nullptr, false) << "cipherText malloc failed.";
        if (testCaseParams.runStage == HksStageType::HKS_STAGE_THREE) {
            HksBlob tmpTagAead = { .size = 0, .data = nullptr };
            RunTestAesDecrypt(&key, testCaseParams, &message, &cipherText, &tmpTagAead);
        } else {
            EXPECT_EQ(HksCryptoHalDecrypt(&key, &testCaseParams.usageSpec, &message, &cipherText),
                testCaseParams.decryptResult);
        }

        HksFree(key.data);
        HksFree(message.data);
        HksFree(cipherText.data);
    }

    void RunTestAesDecrypt(struct HksBlob *key, const TestCaseParams &testCaseParams, struct HksBlob *decryptMsg,
        struct HksBlob *decryptOut, struct HksBlob *tagAead) const
    {
        void* decryptCtx = (void *)HksMalloc(HKS_CONTEXT_DATA_MAX);
        ASSERT_EQ(decryptCtx == nullptr, false) << "decryptCtx malloc failed.";
        EXPECT_EQ(HksCryptoHalDecryptInit(key, &testCaseParams.usageSpec, &decryptCtx), testCaseParams.decryptResult);
        if (testCaseParams.decryptResult != HKS_SUCCESS) {
            return;
        }
        uint32_t decrytopoint = 0;
        uint32_t decrytoinLen = decryptMsg->size;

        if (decrytoinLen > HKS_UPDATE_DATA_MAX) {
            HksBlob messageUpdate = { .size = HKS_UPDATE_DATA_MAX, .data = (uint8_t *)HksMalloc(HKS_UPDATE_DATA_MAX) };
            HksBlob out = { .size = decrytoinLen, .data = (uint8_t *)HksMalloc(decrytoinLen) };
            ASSERT_EQ(messageUpdate.data == nullptr, false) << "messageUpdate malloc failed.";
            ASSERT_EQ(out.data == nullptr, false) << "out malloc failed.";
            while (decrytopoint < decrytoinLen - HKS_UPDATE_DATA_MAX) {
                EXPECT_EQ(memcpy_s(messageUpdate.data, messageUpdate.size, (decryptMsg->data + decrytopoint),
                    HKS_UPDATE_DATA_MAX), EOK) << "memcpy fail";
                EXPECT_EQ(HksCryptoHalDecryptUpdate(&messageUpdate, decryptCtx, &out,
                    testCaseParams.usageSpec.algType), testCaseParams.decryptResult);
                decrytopoint = decrytopoint + HKS_UPDATE_DATA_MAX;
            }
            HksFree(out.data);
            HksFree(messageUpdate.data);

            uint32_t lastLen = decrytoinLen - decrytopoint;
            HksBlob messageLast = { .size = lastLen, .data = (uint8_t *)HksMalloc(lastLen) };
            ASSERT_EQ(messageLast.data == nullptr, false) << "messageLast malloc failed.";
            (void)memcpy_s(messageLast.data, lastLen, (decryptMsg->data + decrytopoint), lastLen);
            HksBlob outLast = { .size = lastLen, .data = (uint8_t *)HksMalloc(lastLen + HKS_PADDING_SUPPLENMENT) };
            ASSERT_EQ(outLast.data == nullptr, false) << "outLast malloc failed.";
            EXPECT_EQ(HksCryptoHalDecryptFinal(&messageLast, &decryptCtx, &outLast, tagAead,
                testCaseParams.usageSpec.algType), testCaseParams.decryptResult) << "HksCryptoHalEncryptFinal failed.";
            HksFree(messageLast.data);
            HksFree(outLast.data);
        } else {
            HksBlob out = { .size = decrytoinLen, .data = (uint8_t *)HksMalloc(decrytoinLen) };
            ASSERT_EQ(out.data == nullptr, false) << "out malloc failed.";
            EXPECT_EQ(HksCryptoHalDecryptUpdate(decryptMsg, decryptCtx, &out,
                testCaseParams.usageSpec.algType), testCaseParams.decryptResult);

            HksBlob deMessageLast = { .size = 0, .data = nullptr };
            EXPECT_EQ(HksCryptoHalDecryptFinal(&deMessageLast, &decryptCtx, decryptOut, tagAead,
                testCaseParams.usageSpec.algType), testCaseParams.decryptResult);

            HksFree(out.data);
        }
    }

    void RunTestAesGcmDecrypt(struct HksBlob *key, const HksUsageSpec *usageSpec, struct HksBlob *decryptMsg,
        struct HksBlob *decryptOut, struct HksBlob *tagAead) const
    {
        void* decryptCtx = (void *)HksMalloc(HKS_CONTEXT_DATA_MAX);
        ASSERT_EQ(decryptCtx == nullptr, false) << "decryptCtx malloc failed.";
        EXPECT_EQ(HksCryptoHalDecryptInit(key, usageSpec, &decryptCtx), HKS_SUCCESS);

        uint32_t decrytopoint = 0;
        uint32_t decrytoinLen = decryptMsg->size;

        if (decrytoinLen > HKS_UPDATE_DATA_MAX) {
            HksBlob messageUpdate = { .size = HKS_UPDATE_DATA_MAX, .data = (uint8_t *)HksMalloc(HKS_UPDATE_DATA_MAX) };
            HksBlob out = { .size = decrytoinLen, .data = (uint8_t *)HksMalloc(decrytoinLen) };
            ASSERT_EQ(messageUpdate.data == nullptr, false) << "messageUpdate malloc failed.";
            ASSERT_EQ(out.data == nullptr, false) << "out malloc failed.";
            while (decrytopoint < decrytoinLen - HKS_UPDATE_DATA_MAX) {
                EXPECT_EQ(memcpy_s(messageUpdate.data, messageUpdate.size, (decryptMsg->data + decrytopoint),
                    HKS_UPDATE_DATA_MAX), EOK) << "memcpy fail";
                EXPECT_EQ(HksCryptoHalDecryptUpdate(&messageUpdate, decryptCtx, &out, usageSpec->algType), HKS_SUCCESS);
                decrytopoint = decrytopoint + HKS_UPDATE_DATA_MAX;
            }
            HksFree(out.data);
            HksFree(messageUpdate.data);

            uint32_t lastLen = decrytoinLen - decrytopoint;
            HksBlob messageLast = { .size = lastLen, .data = (uint8_t *)HksMalloc(lastLen) };
            ASSERT_EQ(messageLast.data == nullptr, false) << "messageLast malloc failed.";
            (void)memcpy_s(messageLast.data, lastLen, (decryptMsg->data + decrytopoint), lastLen);
            HksBlob outLast = { .size = lastLen, .data = (uint8_t *)HksMalloc(lastLen + HKS_PADDING_SUPPLENMENT) };
            ASSERT_EQ(outLast.data == nullptr, false) << "outLast malloc failed.";
            EXPECT_EQ(HksCryptoHalDecryptFinal(&messageLast, &decryptCtx, &outLast, tagAead,
                usageSpec->algType), HKS_SUCCESS) << "HksCryptoHalEncryptFinal failed.";
            HksFree(messageLast.data);
            HksFree(outLast.data);
        } else {
            HksBlob out = { .size = decrytoinLen, .data = (uint8_t *)HksMalloc(decrytoinLen) };
            ASSERT_EQ(out.data == nullptr, false) << "out malloc failed.";
            EXPECT_EQ(HksCryptoHalDecryptUpdate(decryptMsg, decryptCtx, &out, usageSpec->algType), HKS_SUCCESS);

            HksBlob deMessageLast = { .size = 0, .data = nullptr };
            EXPECT_EQ(HksCryptoHalDecryptFinal(&deMessageLast, &decryptCtx, decryptOut, tagAead,
                usageSpec->algType), HKS_SUCCESS);

            HksFree(out.data);
        }
    }

    void GcmRunTestCase(const TestCaseParamsForGcm &testCaseParamsForGcm)
    {
        uint32_t keyLen = testCaseParamsForGcm.keyData.length() / HKS_COUNT_OF_HALF;
        HksBlob key = { .size = keyLen, .data = (uint8_t *)HksMalloc(keyLen) };
        ASSERT_EQ(key.data == nullptr, false) << "key malloc failed.";
        for (uint32_t ii = 0; ii < keyLen; ii++) {
            key.data[ii] = ReadHex((const uint8_t *)&testCaseParamsForGcm.keyData[HKS_COUNT_OF_HALF * ii]);
        }
        uint32_t inLen = testCaseParamsForGcm.hexData.length() / HKS_COUNT_OF_HALF;
        uint32_t outLen = inLen;
        HksBlob message = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_EQ(message.data == nullptr, false) << "message malloc failed.";
        for (uint32_t ii = 0; ii < inLen; ii++) {
            message.data[ii] = ReadHex((const uint8_t *)&testCaseParamsForGcm.hexData[HKS_COUNT_OF_HALF * ii]);
        }
        HksBlob cipherText = { .size = outLen, .data = (uint8_t *)HksMalloc(outLen + HKS_PADDING_SUPPLENMENT) };
        uint32_t tagDecSize = testCaseParamsForGcm.tagDec.length() / HKS_COUNT_OF_HALF;
        HksBlob tagDecBlob = { .size = tagDecSize, .data = (uint8_t *)HksMalloc(tagDecSize) };
        ASSERT_EQ(cipherText.data == nullptr, false) << "cipherText malloc failed.";
        ASSERT_EQ(tagDecBlob.data == nullptr, false) << "tagDecBlob malloc failed.";
        for (uint32_t ii = 0; ii < tagDecSize; ii++) {
            tagDecBlob.data[ii] = ReadHex((const uint8_t *)&testCaseParamsForGcm.tagDec[HKS_COUNT_OF_HALF * ii]);
        }
        HksAeadParam AEAD_PARAM = {
            .nonce = { .size = IV_SIZE, .data = IV },
            .aad = { .size = 0, .data = nullptr },
            .tagDec = tagDecBlob,
            .payloadLen = 0,
        };
        HksUsageSpec usageSpec = {
            .algType = HKS_ALG_AES,
            .mode = HKS_MODE_GCM,
            .padding = HKS_PADDING_NONE,
            .digest = HKS_DIGEST_NONE,
            .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            .algParam = &AEAD_PARAM,
        };

        if (testCaseParamsForGcm.runStage == HksStageType::HKS_STAGE_THREE) {
            RunTestAesGcmDecrypt(&key, &usageSpec, &message, &cipherText, &tagDecBlob);
        } else {
            EXPECT_EQ(HksCryptoHalDecrypt(&key, &usageSpec, &message, &cipherText), testCaseParamsForGcm.decryptResult);
        }

        HksFree(key.data);
        HksFree(message.data);
        HksFree(cipherText.data);
        HksFree(tagDecBlob.data);
    }
};

void HksCryptoHalAesDecrypt::SetUpTestCase(void)
{
}

void HksCryptoHalAesDecrypt::TearDownTestCase(void)
{
}

void HksCryptoHalAesDecrypt::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalAesDecrypt::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_001
 * @tc.name      : HksCryptoHalAesDecrypt_001
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-128-CBC-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_001, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_002
 * @tc.name      : HksCryptoHalAesDecrypt_002
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-128-CBC-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_002, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_003
 * @tc.name      : HksCryptoHalAesDecrypt_003
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-128-CTR-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_003, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_003_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_004
 * @tc.name      : HksCryptoHalAesDecrypt_004
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-128-ECB-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_004, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_004_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_005
 * @tc.name      : HksCryptoHalAesDecrypt_005
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-128-ECB-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_005, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_005_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_006
 * @tc.name      : HksCryptoHalAesDecrypt_006
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-128-GCM-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_006, Function | SmallTest | Level0)
{
    GcmRunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_006_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_007
 * @tc.name      : HksCryptoHalAesDecrypt_007
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-192-CBC-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_007, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_007_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_008
 * @tc.name      : HksCryptoHalAesDecrypt_008
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-192-CBC-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_008, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_008_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_009
 * @tc.name      : HksCryptoHalAesDecrypt_009
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-192-CTR-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_009, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_009_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_010
 * @tc.name      : HksCryptoHalAesDecrypt_010
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-192-ECB-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_010, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_010_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_011
 * @tc.name      : HksCryptoHalAesDecrypt_011
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-192-ECB-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_011, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_011_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_012
 * @tc.name      : HksCryptoHalAesDecrypt_012
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-192-GCM-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_012, Function | SmallTest | Level0)
{
    GcmRunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_012_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_013
 * @tc.name      : HksCryptoHalAesDecrypt_013
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-256-CBC-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_013, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_013_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_014
 * @tc.name      : HksCryptoHalAesDecrypt_014
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-256-CBC-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_014, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_014_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_015
 * @tc.name      : HksCryptoHalAesDecrypt_015
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-256-CTR-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_015, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_015_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_016
 * @tc.name      : HksCryptoHalAesDecrypt_016
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-256-ECB-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_016, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_016_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_017
 * @tc.name      : HksCryptoHalAesDecrypt_017
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-256-ECB-PKCS7PADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_017, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_017_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesDecrypt_018
 * @tc.name      : HksCryptoHalAesDecrypt_018
 * @tc.desc      : Using HksCryptoHalDecrypt Decrypt AES-256-GCM-NOPADDING key.
 */
HWTEST_F(HksCryptoHalAesDecrypt, HksCryptoHalAesDecrypt_018, Function | SmallTest | Level0)
{
    GcmRunTestCase(HKS_CRYPTO_HAL_AES_DECRYPT_018_PARAMS);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS