/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include <mutex>
#include <random>
#include <string>
#include <thread>
#include "securec.h"

#include <gtest/gtest.h>

#include "hks_api.h"
#include "hks_config.h"
#include "hks_mem.h"
#include "hks_param.h"

using namespace testing::ext;
namespace {
namespace {
const char GENERATE_KEY[] = "This is for generate key";
const char IMPORT_KEY[] = "This is for import key";
const uint32_t TEST_HOURS = 6;
const uint32_t DSA_KEY_SIZE = 1024;
const uint32_t THREADS_NUM = 15;
const uint32_t RANDOM_KEY_SIZE = 512;
const uint32_t MAX_SDK_VERSION_SIZE = 64;
const uint32_t DERIVE_KEY_SIZE = 64;
const uint32_t IV_SIZE = 16;
const uint32_t AAD_SIZE = 16;
const uint32_t RSA_MESSAGE_SIZE = 6;
const uint32_t AES_MESSAGE_SIZE = 16;
const uint32_t COMPLEMENT_LEN = 16;
const uint32_t KEY_PARAMSET_SIZE = 1024;
const uint32_t MESSAGE_SIZE = 64;
const uint32_t TEST_KEY_SIZE = 4096;
const uint32_t CIPHER_SIZE = 4096;
const uint32_t MAX_STRING_LENTH = 513;

static const struct HksParam CIPHER_PARAMS[] = {
    { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
    { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
    { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
    { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
};

const struct HksParam g_PARAM_ALGORITHM_AES = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES };
const struct HksParam g_PARAM_ALGORITHM_RSA = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA };
const struct HksParam g_PARAM_ALGORITHM_ECC = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC };
#ifndef _USE_MBEDTLS_
const struct HksParam g_PARAM_ALGORITHM_DSA = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA };
#endif
#ifdef HKS_SUPPORT_DH_C
const struct HksParam g_PARAM_ALGORITHM_DH = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH };
#endif
const struct HksParam g_PARAM_ALGORITHM_ECDH = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH };

const struct HksParam g_PARAM_KEYSIZE_AES_128 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 };
const struct HksParam g_PARAM_KEYSIZE_AES_192 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 };
const struct HksParam g_PARAM_KEYSIZE_AES_256 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 };
const struct HksParam g_PARAM_KEYSIZE_RSA_512 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 };
const struct HksParam g_PARAM_KEYSIZE_RSA_768 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 };
const struct HksParam g_PARAM_KEYSIZE_RSA_1024 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 };
const struct HksParam g_PARAM_KEYSIZE_RSA_2048 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 };
const struct HksParam g_PARAM_KEYSIZE_RSA_3072 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 };
const struct HksParam g_PARAM_KEYSIZE_RSA_4096 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 };
const struct HksParam g_PARAM_KEYSIZE_ECC_224 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 };
const struct HksParam g_PARAM_KEYSIZE_ECC_256 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 };
const struct HksParam g_PARAM_KEYSIZE_ECC_384 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 };
const struct HksParam g_PARAM_KEYSIZE_ECC_521 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 };
#ifdef HKS_SUPPORT_DH_C
const struct HksParam g_PARAM_KEYSIZE_DH_2048 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_2048 };
const struct HksParam g_PARAM_KEYSIZE_DH_3072 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_3072 };
const struct HksParam g_PARAM_KEYSIZE_DH_4096 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_4096 };
#endif
const struct HksParam g_PARAM_KEYSIZE_DSA_1024 = { .tag = HKS_TAG_KEY_SIZE, .uint32Param = DSA_KEY_SIZE };

const struct HksParam g_PARAM_PADDING_NONE = { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE };
const struct HksParam g_PARAM_PADDING_OAEP = { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP };
const struct HksParam g_PARAM_PADDING_PKCS1_V1_5 = { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 };
const struct HksParam g_PARAM_PADDING_PKCS7 = { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 };
const struct HksParam g_PARAM_PADDING_PSS = { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS };

const struct HksParam g_PARAM_MODE_CBC = { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC };
const struct HksParam g_PARAM_MODE_CTR = { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CTR };
const struct HksParam g_PARAM_MODE_ECB = { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB };
const struct HksParam g_PARAM_MODE_GCM = { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_GCM };

const struct HksParam g_PARAM_DIGEST_NONE = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE };
const struct HksParam g_PARAM_DIGEST_MD5 = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_MD5 };
const struct HksParam g_PARAM_DIGEST_SHA1 = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 };
const struct HksParam g_PARAM_DIGEST_SHA224 = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 };
const struct HksParam g_PARAM_DIGEST_SHA256 = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 };
const struct HksParam g_PARAM_DIGEST_SHA384 = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 };
const struct HksParam g_PARAM_DIGEST_SHA512 = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 };

inline uint32_t GetRandom()
{
    std::random_device rd;
    return rd();
}

void AESModeRandForCiper(struct HksParamSet *paramInSet)
{
    struct HksParam tagIv;
    uint8_t iv[IV_SIZE] = {0};
    const struct HksParam *paramMode[] = {
        &g_PARAM_MODE_CBC,
        &g_PARAM_MODE_CTR,
        &g_PARAM_MODE_ECB,
        &g_PARAM_MODE_GCM,
    };
    const struct HksParam *paramPadding[] = {
        &g_PARAM_PADDING_NONE,
        &g_PARAM_PADDING_PKCS7,
    };
    const uint32_t countOfMode = sizeof(paramMode) / sizeof(struct HksParam *);
    const uint32_t countOfPadding = sizeof(paramPadding) / sizeof(struct HksParam *);
    const uint32_t indexOfMode = GetRandom() % countOfMode;
    HksAddParams(paramInSet, paramMode[indexOfMode], 1);
    if (paramMode[indexOfMode]->uint32Param == HKS_MODE_CBC) {
        tagIv = { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv } };
        HksAddParams(paramInSet, &tagIv, 1);
        HksAddParams(paramInSet, paramPadding[GetRandom() % countOfPadding], 1);
    } else if (paramMode[indexOfMode]->uint32Param == HKS_MODE_CTR) {
        HksAddParams(paramInSet, &g_PARAM_PADDING_NONE, 1);
        tagIv = { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv } };
        HksAddParams(paramInSet, &tagIv, 1);
    } else if (paramMode[indexOfMode]->uint32Param == HKS_MODE_ECB) {
        tagIv = { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv } };
        HksAddParams(paramInSet, &tagIv, 1);
#ifndef _USE_MBEDTLS_
        HksAddParams(paramInSet, paramPadding[GetRandom() % countOfPadding], 1);
#else
        HksAddParams(paramInSet, &g_PARAM_PADDING_NONE, 1);
#endif
    } else if (paramMode[indexOfMode]->uint32Param == HKS_MODE_GCM) {
        HksAddParams(paramInSet, &g_PARAM_PADDING_NONE, 1);
        uint8_t aadData[AAD_SIZE] = { 'a', 'a', 'd' };
        struct HksParam aad = { .tag = HKS_TAG_ASSOCIATED_DATA, .blob = { .size = sizeof(aadData), .data = aadData } };
        HksAddParams(paramInSet, &aad, 1);
        tagIv = { .tag = HKS_TAG_NONCE, .blob = { .size = IV_SIZE, .data = iv } };
        HksAddParams(paramInSet, &tagIv, 1);
    }
}

void ParamForAESCipher(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramKeySize[] = {
        &g_PARAM_KEYSIZE_AES_128,
        &g_PARAM_KEYSIZE_AES_192,
        &g_PARAM_KEYSIZE_AES_256,
    };
    const uint32_t count = sizeof(paramKeySize) / sizeof(struct HksParam *);
    HksAddParams(paramInSet, paramKeySize[GetRandom() % count], 1);
    AESModeRandForCiper(paramInSet);
}

void RSAKeySizeRand(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramKeySize[] = {
        &g_PARAM_KEYSIZE_RSA_512,
        &g_PARAM_KEYSIZE_RSA_768,
        &g_PARAM_KEYSIZE_RSA_1024,
        &g_PARAM_KEYSIZE_RSA_2048,
        &g_PARAM_KEYSIZE_RSA_3072,
        &g_PARAM_KEYSIZE_RSA_4096,
    };
    const uint32_t count = sizeof(paramKeySize) / sizeof(struct HksParam *);
    HksAddParams(paramInSet, paramKeySize[GetRandom() % count], 1);
}

void RSAKeySizeWithout512(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramKeySize[] = {
        &g_PARAM_KEYSIZE_RSA_768,
        &g_PARAM_KEYSIZE_RSA_1024,
        &g_PARAM_KEYSIZE_RSA_2048,
        &g_PARAM_KEYSIZE_RSA_3072,
        &g_PARAM_KEYSIZE_RSA_4096,
    };
    const uint32_t count = sizeof(paramKeySize) / sizeof(struct HksParam *);
    HksAddParams(paramInSet, paramKeySize[GetRandom() % count], 1);
}

void RSAKeySizeWithout768(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramKeySize[] = {
        &g_PARAM_KEYSIZE_RSA_1024,
        &g_PARAM_KEYSIZE_RSA_2048,
        &g_PARAM_KEYSIZE_RSA_3072,
        &g_PARAM_KEYSIZE_RSA_4096,
    };
    const uint32_t count = sizeof(paramKeySize) / sizeof(struct HksParam *);
    HksAddParams(paramInSet, paramKeySize[GetRandom() % count], 1);
}

void OaepSha512KeySize(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramKeySize[] = {
        &g_PARAM_KEYSIZE_RSA_2048,
        &g_PARAM_KEYSIZE_RSA_3072,
        &g_PARAM_KEYSIZE_RSA_4096,
    };
    const uint32_t count = sizeof(paramKeySize) / sizeof(struct HksParam *);
    HksAddParams(paramInSet, paramKeySize[GetRandom() % count], 1);
}

void ParamForRSACipher(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramPadding[] = {
#ifndef _USE_MBEDTLS_
        &g_PARAM_PADDING_NONE,
#endif
        &g_PARAM_PADDING_PKCS1_V1_5,
        &g_PARAM_PADDING_OAEP,
    };
    const struct HksParam *paramDigest[] = {
        &g_PARAM_DIGEST_NONE,
        &g_PARAM_DIGEST_SHA1,
        &g_PARAM_DIGEST_SHA224,
        &g_PARAM_DIGEST_SHA256,
        &g_PARAM_DIGEST_SHA384,
        &g_PARAM_DIGEST_SHA512,
    };
    const uint32_t countOfPadding = sizeof(paramPadding) / sizeof(struct HksParam *);
    const uint32_t countOfDigest = sizeof(paramDigest) / sizeof(struct HksParam *);
    const uint32_t indexOfPadding = GetRandom() % countOfPadding;
    HksAddParams(paramInSet, paramPadding[indexOfPadding], 1);
    if ((paramPadding[indexOfPadding]->uint32Param == HKS_PADDING_NONE) ||
        (paramPadding[indexOfPadding]->uint32Param == HKS_PADDING_PKCS1_V1_5)) {
        RSAKeySizeRand(paramInSet);
        HksAddParams(paramInSet, &g_PARAM_DIGEST_NONE, 1);
    } else if (paramPadding[indexOfPadding]->uint32Param == HKS_PADDING_OAEP) {
        const uint32_t indexOfDigest = GetRandom() % countOfDigest;
        HksAddParams(paramInSet, paramDigest[indexOfDigest], 1);
        if (paramDigest[indexOfDigest]->uint32Param == HKS_DIGEST_SHA256) {
            RSAKeySizeWithout512(paramInSet);
        } else if (paramDigest[indexOfDigest]->uint32Param == HKS_DIGEST_SHA384) {
            RSAKeySizeWithout768(paramInSet);
        } else if (paramDigest[indexOfDigest]->uint32Param == HKS_DIGEST_SHA512) {
            OaepSha512KeySize(paramInSet);
        } else {
            RSAKeySizeRand(paramInSet);
        }
    }
}

void ParamForCipher(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramAlgorithm[] = {
        &g_PARAM_ALGORITHM_AES,
        &g_PARAM_ALGORITHM_RSA,
    };
    const uint32_t count = sizeof(paramAlgorithm) / sizeof(struct HksParam *);
    const uint32_t index = GetRandom() % count;
    HksAddParams(paramInSet, paramAlgorithm[index], 1);
    if (paramAlgorithm[index]->uint32Param == HKS_ALG_AES) {
        ParamForAESCipher(paramInSet);
    } else if (paramAlgorithm[index]->uint32Param == HKS_ALG_RSA) {
        HksAddParams(paramInSet, &g_PARAM_MODE_ECB, 1);
        ParamForRSACipher(paramInSet);
    }
}

void ShaForPkcs1(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramDigest[] = {
        &g_PARAM_DIGEST_NONE,
        &g_PARAM_DIGEST_MD5,
        &g_PARAM_DIGEST_SHA1,
        &g_PARAM_DIGEST_SHA224,
        &g_PARAM_DIGEST_SHA256,
        &g_PARAM_DIGEST_SHA384,
        &g_PARAM_DIGEST_SHA512,
    };
    const uint32_t count = sizeof(paramDigest) / sizeof(struct HksParam *);
    const uint32_t index = GetRandom() % count;
    HksAddParams(paramInSet, paramDigest[index], 1);
    if ((paramDigest[index]->uint32Param == HKS_DIGEST_SHA384) ||
        (paramDigest[index]->uint32Param == HKS_DIGEST_SHA512)) {
        RSAKeySizeWithout512(paramInSet);
    } else {
        RSAKeySizeRand(paramInSet);
    }
}

void ShaForPss(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramDigest[] = {
        &g_PARAM_DIGEST_NONE,
        &g_PARAM_DIGEST_MD5,
        &g_PARAM_DIGEST_SHA1,
        &g_PARAM_DIGEST_SHA224,
        &g_PARAM_DIGEST_SHA256,
        &g_PARAM_DIGEST_SHA384,
        &g_PARAM_DIGEST_SHA512,
    };
    const uint32_t count = sizeof(paramDigest) / sizeof(struct HksParam *);
    const uint32_t index = GetRandom() % count;
    HksAddParams(paramInSet, paramDigest[index], 1);
    if (paramDigest[index]->uint32Param == HKS_DIGEST_SHA384) {
#ifndef _USE_MBEDTLS_
        RSAKeySizeRand(paramInSet);
#else
        RSAKeySizeWithout512(paramInSet);
#endif
    } else if (paramDigest[index]->uint32Param == HKS_DIGEST_SHA512) {
#ifndef _USE_MBEDTLS_
        RSAKeySizeWithout512(paramInSet);
#else
        RSAKeySizeWithout768(paramInSet);
#endif
    } else {
        RSAKeySizeRand(paramInSet);
    }
}

void ParamForRSASign(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramPadding[] = {
        &g_PARAM_PADDING_PKCS1_V1_5,
        &g_PARAM_PADDING_PSS,
    };
    const uint32_t count = sizeof(paramPadding) / sizeof(struct HksParam *);
    const uint32_t index = GetRandom() % count;
    HksAddParams(paramInSet, paramPadding[index], 1);
    if (paramPadding[index]->uint32Param == HKS_PADDING_PKCS1_V1_5) {
        ShaForPkcs1(paramInSet);
    } else if (paramPadding[index]->uint32Param == HKS_PADDING_PSS) {
        ShaForPss(paramInSet);
    }
}

void ParamForECCSign(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramKeySize[] = {
        &g_PARAM_KEYSIZE_ECC_224,
        &g_PARAM_KEYSIZE_ECC_256,
        &g_PARAM_KEYSIZE_ECC_384,
        &g_PARAM_KEYSIZE_ECC_521,
    };
    const uint32_t countOfKeySize = sizeof(paramKeySize) / sizeof(struct HksParam *);
    HksAddParams(paramInSet, paramKeySize[GetRandom() % countOfKeySize], 1);

    const struct HksParam *paramDigest[] = {
        &g_PARAM_DIGEST_NONE,
        &g_PARAM_DIGEST_SHA1,
        &g_PARAM_DIGEST_SHA224,
        &g_PARAM_DIGEST_SHA256,
        &g_PARAM_DIGEST_SHA384,
        &g_PARAM_DIGEST_SHA512,
    };
    const uint32_t countOfDigest = sizeof(paramDigest) / sizeof(struct HksParam *);
    HksAddParams(paramInSet, paramDigest[GetRandom() % countOfDigest], 1);
}

void ParamForDSASign(struct HksParamSet *paramInSet)
{
    HksAddParams(paramInSet, &g_PARAM_PADDING_PKCS1_V1_5, 1);
    HksAddParams(paramInSet, &g_PARAM_KEYSIZE_DSA_1024, 1);
    const struct HksParam *paramDigest[] = {
        &g_PARAM_DIGEST_SHA1,
        &g_PARAM_DIGEST_SHA224,
        &g_PARAM_DIGEST_SHA256,
        &g_PARAM_DIGEST_SHA384,
        &g_PARAM_DIGEST_SHA512,
    };
    const uint32_t countOfDigest = sizeof(paramDigest) / sizeof(struct HksParam *);
    HksAddParams(paramInSet, paramDigest[GetRandom() % countOfDigest], 1);
}

void ParamForSign(struct HksParamSet *paramInSet)
{
    const struct HksParam *paramAlgorithm[] = {
        &g_PARAM_ALGORITHM_RSA,
        &g_PARAM_ALGORITHM_ECC,
#ifndef _USE_MBEDTLS_
        &g_PARAM_ALGORITHM_DSA,
#endif
    };
    const uint32_t count = sizeof(paramAlgorithm) / sizeof(struct HksParam *);
    const uint32_t index = GetRandom() % count;
    HksAddParams(paramInSet, paramAlgorithm[index], 1);
    if (paramAlgorithm[index]->uint32Param == HKS_ALG_RSA) {
        ParamForRSASign(paramInSet);
    } else if (paramAlgorithm[index]->uint32Param == HKS_ALG_ECC) {
        ParamForECCSign(paramInSet);
    } else if (paramAlgorithm[index]->uint32Param == HKS_ALG_DSA) {
        ParamForDSASign(paramInSet);
    }
}

void ParamForAgree(struct HksParamSet *generateKeyParam, struct HksParamSet *agreeKeyParam)
{
    const struct HksParam *paramAlg[] = {
        &g_PARAM_ALGORITHM_ECDH,
#ifdef HKS_SUPPORT_DH_C
        &g_PARAM_ALGORITHM_DH,
#endif
    };
    const uint32_t countOfAlg = sizeof(paramAlg) / sizeof(struct HksParam *);
    const uint32_t indexOfAlg = GetRandom() % countOfAlg;

    HksAddParams(agreeKeyParam, paramAlg[indexOfAlg], 1);
    if (paramAlg[indexOfAlg]->uint32Param == HKS_ALG_ECDH) {
        HksAddParams(generateKeyParam, &g_PARAM_ALGORITHM_ECC, 1);
        const struct HksParam *paramKeySize[] = {
            &g_PARAM_KEYSIZE_ECC_224,
            &g_PARAM_KEYSIZE_ECC_256,
            &g_PARAM_KEYSIZE_ECC_384,
            &g_PARAM_KEYSIZE_ECC_521,
        };
        const uint32_t countOfKeySize = sizeof(paramKeySize) / sizeof(struct HksParam *);
        const uint32_t indexOfKeySize = GetRandom() % countOfKeySize;
        HksAddParams(generateKeyParam, paramKeySize[indexOfKeySize], 1);
        HksAddParams(agreeKeyParam, paramKeySize[indexOfKeySize], 1);
        HksAddParams(generateKeyParam, &g_PARAM_DIGEST_NONE, 1);
    }
#ifdef HKS_SUPPORT_DH_C
    if (paramAlg[indexOfAlg]->uint32Param == HKS_ALG_DH) {
        HksAddParams(generateKeyParam, &g_PARAM_ALGORITHM_DH, 1);
        const struct HksParam *paramKeySize[] = {
            &g_PARAM_KEYSIZE_DH_2048,
            &g_PARAM_KEYSIZE_DH_3072,
            &g_PARAM_KEYSIZE_DH_4096,
        };
        const uint32_t countOfKeySize = sizeof(paramKeySize) / sizeof(struct HksParam *);
        const uint32_t indexOfKeySize = GetRandom() % countOfKeySize;
        HksAddParams(generateKeyParam, paramKeySize[indexOfKeySize], 1);
        HksAddParams(agreeKeyParam, paramKeySize[indexOfKeySize], 1);
    }
#endif
}
}  // namespace

class ApiPressureTest : public testing::Test {
public:
    void SetThreadState(bool threadState)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        threadState_ = threadState;
    }
    bool GetThreadState()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return threadState_;
    }
    void GetSdkVersionTest();
    void InitializeTest();
    void RefreshKeyInfoTest();
    void GenerateKeyTest();
    void ImportKeyTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet);
    void ExportPublicKeyTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet);
    void DeleteKeyTest(uint32_t ii);
    void GetKeyParamSetTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet);
    void KeyExistTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet);
    void GenerateRandomTest();
    void SignTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet, const struct HksBlob *message);
    void VerifyTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet, const struct HksBlob *message,
        const struct HksBlob *signature);
    void EncryptTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet);
    void DecryptTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet,
        const struct HksBlob *cipherText, uint32_t *inLen);
    void AgreeKeyTest(const struct HksBlob *authId);
    void DeriveKeyTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet);
    void MacTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet);
    void HashTest();
    void CipherScene(uint32_t ii);
    void SignScene(uint32_t ii);
    void AgreeScene(uint32_t ii);

private:
    bool threadState_ = false;
    std::mutex mutex_;
};

void ApiPressureTest::GetSdkVersionTest()
{
    do {
        if (!GetThreadState()) {
            break;
        }
        struct HksBlob sdkVersion = {
            .size = MAX_SDK_VERSION_SIZE,
            .data = (uint8_t *)HksMalloc(MAX_SDK_VERSION_SIZE)
        };
        ASSERT_NE(sdkVersion.data, nullptr);

        EXPECT_EQ(HksGetSdkVersion(&sdkVersion), HKS_SUCCESS);

        HksFree(sdkVersion.data);
    } while (1);
}

void ApiPressureTest::InitializeTest()
{
    do {
        if (!GetThreadState()) {
            break;
        }
        EXPECT_EQ(HksInitialize(), HKS_SUCCESS);
    } while (1);
}

void ApiPressureTest::RefreshKeyInfoTest()
{
    do {
        if (!GetThreadState()) {
            break;
        }
        EXPECT_EQ(HksRefreshKeyInfo(), HKS_SUCCESS);
    } while (1);
}

void ApiPressureTest::GenerateKeyTest()
{
    do {
        if (!GetThreadState()) {
            break;
        }
        struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        struct HksParam tmpParams[] = {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        };
        HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
        HksBuildParamSet(&paramInSet);
        EXPECT_EQ(HksGenerateKey(&authId, paramInSet, NULL), HKS_SUCCESS);
        HksFreeParamSet(&paramInSet);
    } while (1);
}

void ApiPressureTest::ImportKeyTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet)
{
    struct HksBlob pubKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(pubKey.data, nullptr);
    HksExportPublicKey(authId, paramInSet, &pubKey);
    do {
        if (!GetThreadState()) {
            break;
        }
        struct HksBlob importId = { (uint32_t)strlen(IMPORT_KEY), (uint8_t *)IMPORT_KEY };
        EXPECT_EQ(HksImportKey(&importId, paramInSet, &pubKey), HKS_SUCCESS);
    } while (1);
    HksFree(pubKey.data);
}

void ApiPressureTest::ExportPublicKeyTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet)
{
    do {
        if (!GetThreadState()) {
            break;
        }
        struct HksBlob pubKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
        ASSERT_NE(pubKey.data, nullptr);

        EXPECT_EQ(HksExportPublicKey(authId, paramInSet, &pubKey), HKS_SUCCESS);

        HksFree(pubKey.data);
    } while (1);
}

void ApiPressureTest::DeleteKeyTest(uint32_t ii)
{
    do {
        if (!GetThreadState()) {
            break;
        }
        std::string key = "KeyForThread" + std::to_string(ii);
        struct HksBlob authId = { .size = (uint32_t)key.length(), .data = (uint8_t *)&key[0] };
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        struct HksParam tmpParams[] = {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        };
        HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
        HksBuildParamSet(&paramInSet);
        HksGenerateKey(&authId, paramInSet, NULL);
        EXPECT_EQ(HksDeleteKey(&authId, paramInSet), HKS_SUCCESS);
        HksFreeParamSet(&paramInSet);
    } while (1);
}

void ApiPressureTest::GetKeyParamSetTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet)
{
    do {
        struct HksParamSet *paramOutSet = nullptr;
        HksInitParamSet(&paramOutSet);
        struct HksParam localKey = {
            .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { .size = KEY_PARAMSET_SIZE, .data = (uint8_t *)HksMalloc(KEY_PARAMSET_SIZE) }
        };
        ASSERT_NE(localKey.blob.data, nullptr);
        HksAddParams(paramOutSet, &localKey, 1);
        HksBuildParamSet(&paramOutSet);
        if (!GetThreadState()) {
            break;
        }
        EXPECT_EQ(HksGetKeyParamSet(authId, paramInSet, paramOutSet), HKS_SUCCESS);
        HksFree(localKey.blob.data);
        HksFreeParamSet(&paramOutSet);
    } while (1);
}

void ApiPressureTest::KeyExistTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet)
{
    do {
        if (!GetThreadState()) {
            break;
        }
        EXPECT_EQ(HksKeyExist(authId, paramInSet), HKS_SUCCESS);
    } while (1);
}

void ApiPressureTest::GenerateRandomTest()
{
    struct HksBlob authId = { .size = RANDOM_KEY_SIZE, .data = (uint8_t *)HksMalloc(RANDOM_KEY_SIZE) };
    ASSERT_NE(authId.data, nullptr);
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    do {
        if (!GetThreadState()) {
            break;
        }
        EXPECT_EQ(HksGenerateRandom(paramInSet, &authId), HKS_SUCCESS);
    } while (1);
    HksFree(authId.data);
    HksFreeParamSet(&paramInSet);
}

void ApiPressureTest::SignTest(
    const struct HksBlob *authId, const struct HksParamSet *paramInSet, const struct HksBlob *message)
{
    do {
        if (!GetThreadState()) {
            break;
        }
        HksBlob signature = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
        ASSERT_NE(signature.data, nullptr);
        EXPECT_EQ(HksSign(authId, paramInSet, message, &signature), HKS_SUCCESS);
        HksFree(signature.data);
    } while (1);
}

void ApiPressureTest::VerifyTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet,
    const struct HksBlob *message, const struct HksBlob *signature)
{
    do {
        if (!GetThreadState()) {
            break;
        }
        EXPECT_EQ(HksVerify(authId, paramInSet, message, signature), HKS_SUCCESS);
    } while (1);
}

void ApiPressureTest::EncryptTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet)
{
    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob plainText = { .size = dataLen, .data = (uint8_t *)hexData };
    uint32_t inLen = dataLen + COMPLEMENT_LEN;
    do {
        if (!GetThreadState()) {
            break;
        }
        HksBlob cipherText = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_NE(cipherText.data, nullptr);
        EXPECT_EQ(HksEncrypt(authId, paramInSet, &plainText, &cipherText), HKS_SUCCESS);
        HksFree(cipherText.data);
    } while (1);
}

void ApiPressureTest::DecryptTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet,
    const struct HksBlob *cipherText, uint32_t *inLen)
{
    do {
        if (!GetThreadState()) {
            break;
        }
        HksBlob plainTextDecrypt = { .size = *inLen, .data = (uint8_t *)HksMalloc(*inLen) };
        ASSERT_NE(plainTextDecrypt.data, nullptr);
        EXPECT_EQ(HksDecrypt(authId, paramInSet, cipherText, &plainTextDecrypt), HKS_SUCCESS);
        HksFree(plainTextDecrypt.data);
    } while (1);
}

void ApiPressureTest::AgreeKeyTest(const struct HksBlob *authId)
{
    struct HksBlob pubKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(pubKey.data, nullptr);
    HksExportPublicKey(authId, NULL, &pubKey);
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    do {
        if (!GetThreadState()) {
            break;
        }
        HksBlob agreeKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
        ASSERT_NE(agreeKey.data, nullptr);
        EXPECT_EQ(HksAgreeKey(paramInSet, authId, &pubKey, &agreeKey), HKS_SUCCESS);
        HksFree(agreeKey.data);
    } while (1);
    HksFree(pubKey.data);
    HksFreeParamSet(&paramInSet);
}

void ApiPressureTest::DeriveKeyTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet)
{
    do {
        if (!GetThreadState()) {
            break;
        }
        HksBlob derivedKey = { .size = DERIVE_KEY_SIZE, .data = (uint8_t *)HksMalloc(DERIVE_KEY_SIZE) };
        ASSERT_NE(derivedKey.data, nullptr);
        EXPECT_EQ(HksDeriveKey(paramInSet, authId, &derivedKey), HKS_SUCCESS);
        HksFree(derivedKey.data);
    } while (1);
}

void ApiPressureTest::MacTest(const struct HksBlob *authId, const struct HksParamSet *paramInSet)
{
    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
    do {
        if (!GetThreadState()) {
            break;
        }
        HksBlob macMessage = { .size = MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(MESSAGE_SIZE) };
        ASSERT_NE(macMessage.data, nullptr);
        EXPECT_EQ(HksMac(authId, paramInSet, &message, &macMessage), HKS_SUCCESS);
        HksFree(macMessage.data);
    } while (1);
}

void ApiPressureTest::HashTest()
{
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam digest = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 };
    HksAddParams(paramInSet, &digest, 1);
    HksBuildParamSet(&paramInSet);
    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
    do {
        if (!GetThreadState()) {
            break;
        }
        HksBlob shaMessage = { .size = MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(MESSAGE_SIZE) };
        ASSERT_NE(shaMessage.data, nullptr);
        EXPECT_EQ(HksHash(paramInSet, &message, &shaMessage), HKS_SUCCESS);
        HksFree(shaMessage.data);
    } while (1);
    HksFreeParamSet(&paramInSet);
}

void ApiPressureTest::CipherScene(uint32_t ii)
{
    std::string key = "KeyForThread" + std::to_string(ii);
    struct HksBlob authId = { .size = (uint32_t)key.length(), .data = (uint8_t *)&key[0] };
    do {
        if (!GetThreadState()) {
            break;
        }
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        HksAddParams(paramInSet, CIPHER_PARAMS, sizeof(CIPHER_PARAMS) / sizeof(CIPHER_PARAMS[0]));
        ParamForCipher(paramInSet);
        HksBuildParamSet(&paramInSet);
        HksGenerateKey(&authId, paramInSet, NULL);
        struct HksParam *padding = nullptr;
        HksGetParam(paramInSet, HKS_TAG_PADDING, &padding);
        char hexData[MAX_STRING_LENTH] = {0};
        uint32_t dataLen;
        struct HksParam *algorithm = nullptr;
        HksGetParam(paramInSet, HKS_TAG_ALGORITHM, &algorithm);
        if (algorithm->uint32Param == HKS_ALG_RSA) {
            if (padding->uint32Param == HKS_PADDING_NONE) {
                struct HksParam *keySize = nullptr;
                HksGetParam(paramInSet, HKS_TAG_KEY_SIZE, &keySize);
                dataLen = HKS_KEY_BYTES(keySize->uint32Param);
            } else {
                dataLen = RSA_MESSAGE_SIZE;
            }
        } else {
            dataLen = AES_MESSAGE_SIZE;
        }
        HksBlob plainText = { .size = dataLen, .data = (uint8_t *)hexData };
        HksBlob cipherText = { .size = CIPHER_SIZE, .data = (uint8_t *)HksMalloc(CIPHER_SIZE) };
        ASSERT_NE(cipherText.data, nullptr);
        HksEncrypt(&authId, paramInSet, &plainText, &cipherText);
        HksBlob plainTextDecrypt = { .size = CIPHER_SIZE, .data = (uint8_t *)HksMalloc(CIPHER_SIZE) };
        ASSERT_NE(plainTextDecrypt.data, nullptr);
        EXPECT_EQ(HksDecrypt(&authId, paramInSet, &cipherText, &plainTextDecrypt), HKS_SUCCESS);
        HksFree(plainTextDecrypt.data);
        HksFree(cipherText.data);
        HksFreeParamSet(&paramInSet);
    } while (1);
}

void ApiPressureTest::SignScene(uint32_t ii)
{
    std::string key = "KeyForThread" + std::to_string(ii);
    struct HksBlob authId = { .size = (uint32_t)key.length(), .data = (uint8_t *)&key[0] };
    do {
        if (!GetThreadState()) {
            break;
        }
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        struct HksParam tmpParams[] = {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        };
        HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
        ParamForSign(paramInSet);
        HksBuildParamSet(&paramInSet);
        HksGenerateKey(&authId, paramInSet, NULL);
        const char *hexData = "0123456789abcdef";
        uint32_t dataLen = strlen(hexData);
        HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
        HksBlob signature = { .size = CIPHER_SIZE, .data = (uint8_t *)HksMalloc(CIPHER_SIZE) };
        ASSERT_NE(signature.data, nullptr);
        HksSign(&authId, paramInSet, &message, &signature);
        EXPECT_EQ(HksVerify(&authId, paramInSet, &message, &signature), HKS_SUCCESS);
        HksFree(signature.data);
        HksFreeParamSet(&paramInSet);
    } while (1);
}

void ApiPressureTest::AgreeScene(uint32_t ii)
{
    std::string key = "KeyForThread" + std::to_string(ii);
    struct HksBlob authId = { .size = (uint32_t)key.length(), .data = (uint8_t *)&key[0] };
    do {
        if (!GetThreadState()) {
            break;
        }
        struct HksParamSet *generateKeyParam = nullptr;
        HksInitParamSet(&generateKeyParam);
        struct HksParamSet *agreeKeyParam = nullptr;
        HksInitParamSet(&agreeKeyParam);
        struct HksParam tmpParams[] = {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        };
        HksAddParams(generateKeyParam, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
        HksAddParams(agreeKeyParam, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
        ParamForAgree(generateKeyParam, agreeKeyParam);
        HksBuildParamSet(&generateKeyParam);
        HksBuildParamSet(&agreeKeyParam);
        HksGenerateKey(&authId, generateKeyParam, NULL);
        struct HksBlob pubKey = { .size = TEST_KEY_SIZE, .data = (uint8_t *)HksMalloc(TEST_KEY_SIZE) };
        ASSERT_NE(pubKey.data, nullptr);
        HksExportPublicKey(&authId, generateKeyParam, &pubKey);
        HksBlob agreeKey = { .size = TEST_KEY_SIZE, .data = (uint8_t *)HksMalloc(TEST_KEY_SIZE) };
        ASSERT_NE(agreeKey.data, nullptr);
        EXPECT_EQ(HksAgreeKey(agreeKeyParam, &authId, &pubKey, &agreeKey), HKS_SUCCESS);
        HksFree(agreeKey.data);
        HksFree(pubKey.data);
        HksFreeParamSet(&generateKeyParam);
        HksFreeParamSet(&agreeKeyParam);
    } while (1);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest00100
 * @tc.name      : ApiPressureTest00100
 * @tc.desc      : HksGetSdkVersion
 */
HWTEST_F(ApiPressureTest, ApiPressureTest00100, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::GetSdkVersionTest, this));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest00200
 * @tc.name      : ApiPressureTest00200
 * @tc.desc      : HksInitialize
 */
HWTEST_F(ApiPressureTest, ApiPressureTest00200, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::InitializeTest, this));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest00300
 * @tc.name      : ApiPressureTest00300
 * @tc.desc      : HksRefreshKeyInfo
 */
HWTEST_F(ApiPressureTest, ApiPressureTest00300, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::RefreshKeyInfoTest, this));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest00400
 * @tc.name      : ApiPressureTest00400
 * @tc.desc      : HksGenerateKey
 */
HWTEST_F(ApiPressureTest, ApiPressureTest00400, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::GenerateKeyTest, this));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest00500
 * @tc.name      : ApiPressureTest00500
 * @tc.desc      : HksImportKey
 */
HWTEST_F(ApiPressureTest, ApiPressureTest00500, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::ImportKeyTest, this, &authId, paramInSet));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest00600
 * @tc.name      : ApiPressureTest00600
 * @tc.desc      : HksExportPublicKey
 */
HWTEST_F(ApiPressureTest, ApiPressureTest00600, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::ExportPublicKeyTest, this, &authId, paramInSet));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest00700
 * @tc.name      : ApiPressureTest00700
 * @tc.desc      : HksDeleteKey
 */
HWTEST_F(ApiPressureTest, ApiPressureTest00700, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::DeleteKeyTest, this, ii));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest00800
 * @tc.name      : ApiPressureTest00800
 * @tc.desc      : HksGetKeyParamSet
 */
HWTEST_F(ApiPressureTest, ApiPressureTest00800, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::GetKeyParamSetTest, this, &authId, paramInSet));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest00900
 * @tc.name      : ApiPressureTest00900
 * @tc.desc      : HksKeyExist
 */
HWTEST_F(ApiPressureTest, ApiPressureTest00900, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::KeyExistTest, this, &authId, paramInSet));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01000
 * @tc.name      : ApiPressureTest01000
 * @tc.desc      : HksGenerateRandom
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01000, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::GenerateRandomTest, this));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01100
 * @tc.name      : ApiPressureTest01100
 * @tc.desc      : HksSign
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01100, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::SignTest, this, &authId, paramInSet, &message));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01200
 * @tc.name      : ApiPressureTest01200
 * @tc.desc      : HksVerify
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01200, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
    HksBlob signature = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(signature.data, nullptr);
    HksSign(&authId, paramInSet, &message, &signature);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(
            std::thread(&ApiPressureTest::VerifyTest, this, &authId, paramInSet, &message, &signature));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFree(signature.data);
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01300
 * @tc.name      : ApiPressureTest01300
 * @tc.desc      : HksEncrypt
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01300, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
    };
    uint8_t iv[IV_SIZE] = {0};
    struct HksParam tagIv = { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv } };
    HksAddParams(paramInSet, &tagIv, 1);
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::EncryptTest, this, &authId, paramInSet));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01400
 * @tc.name      : ApiPressureTest01400
 * @tc.desc      : HksDecrypt
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01400, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
    };
    uint8_t iv[IV_SIZE] = {0};
    struct HksParam tagIv = { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv } };
    HksAddParams(paramInSet, &tagIv, 1);
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob plainText = { .size = dataLen, .data = (uint8_t *)hexData };
    uint32_t inLen = dataLen + COMPLEMENT_LEN;
    HksBlob cipherText = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
    ASSERT_NE(cipherText.data, nullptr);
    HksEncrypt(&authId, paramInSet, &plainText, &cipherText);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(
            std::thread(&ApiPressureTest::DecryptTest, this, &authId, paramInSet, &cipherText, &inLen));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFree(cipherText.data);
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01500
 * @tc.name      : ApiPressureTest01500
 * @tc.desc      : HksAgreeKey
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01500, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::AgreeKeyTest, this, &authId));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01600
 * @tc.name      : ApiPressureTest01600
 * @tc.desc      : HksDeriveKey
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01600, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_DERIVE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    struct HksParamSet *paramInSetHkdf = nullptr;
    HksInitParamSet(&paramInSetHkdf);
    struct HksParam tmpParamsHkdf[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HKDF },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_DERIVE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSetHkdf, tmpParamsHkdf, sizeof(tmpParamsHkdf) / sizeof(tmpParamsHkdf[0]));
    HksBuildParamSet(&paramInSetHkdf);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::DeriveKeyTest, this, &authId, paramInSetHkdf));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFreeParamSet(&paramInSet);
    HksFreeParamSet(&paramInSetHkdf);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01700
 * @tc.name      : ApiPressureTest01700
 * @tc.desc      : HksMac
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01700, TestSize.Level1)
{
    std::vector<std::thread> threads;
    struct HksBlob authId = { (uint32_t)strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = TEST_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);
    HksGenerateKey(&authId, paramInSet, NULL);
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::MacTest, this, &authId, paramInSet));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
    HksFreeParamSet(&paramInSet);
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01800
 * @tc.name      : ApiPressureTest01800
 * @tc.desc      : HksHash
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01800, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::HashTest, this));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest01900
 * @tc.name      : ApiPressureTest01900
 * @tc.desc      : Scenario 1: generate key and encrypt / decrypt
 */
HWTEST_F(ApiPressureTest, ApiPressureTest01900, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::CipherScene, this, ii));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest02000
 * @tc.name      : ApiPressureTest02000
 * @tc.desc      : Scenario 2: generate key and sign / verify
 */
HWTEST_F(ApiPressureTest, ApiPressureTest02000, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::SignScene, this, ii));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}

/**
 * @tc.number    : ApiPressureTest.ApiPressureTest02100
 * @tc.name      : ApiPressureTest02100
 * @tc.desc      : Scenario 3: generate key and agree
 */
HWTEST_F(ApiPressureTest, ApiPressureTest02100, TestSize.Level1)
{
    std::vector<std::thread> threads;
    SetThreadState(true);
    for (uint32_t ii = 0; ii < THREADS_NUM; ii++) {
        threads.emplace_back(std::thread(&ApiPressureTest::AgreeScene, this, ii));
    }
    std::this_thread::sleep_for(std::chrono::hours(TEST_HOURS));
    SetThreadState(false);
    for (auto &t : threads) {
        t.join();
    }
}
}  // namespace