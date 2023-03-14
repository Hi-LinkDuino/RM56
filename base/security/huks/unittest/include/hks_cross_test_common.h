/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HKS_CROSS_TEST_COMMON_H
#define HKS_CROSS_TEST_COMMON_H

#include <string>
#include "hks_test_common.h"
namespace Unittest::CrossTest {
static const std::string g_inData_32  = "Hks_Cross_Test_Rsa_000000_string";
static const std::string g_inData = "Hks_Cross_Test_00000000000000000000000000000000000000000000000000000000000000000"
                                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000"
                                    "0000000000000000000000000000000000000000000000000000000000000000000000000_string";
static const uint32_t CROSS_COMMON_SIZE = 2048;
static const uint32_t DSA_KEY_SIZE = 1024;
static const uint32_t COMMON_KEY_SIZE = 256;
static const uint32_t DERIVE_KEY_SIZE_32 = 32;
static const uint32_t DERIVE_KEY_SIZE_48 = 48;
static const uint32_t DERIVE_KEY_SIZE_64 = 64;
static const uint32_t IV_SIZE = 16;
static const uint32_t SALT_SIZE = 16;
static uint8_t IV[IV_SIZE] = {0};
static uint8_t SALT_DATA[SALT_SIZE] = {0};

int32_t HksCrossTestGenerateKey(const struct HksBlob *keyAlias, struct HksParam *genParams,
    const uint32_t paramCnt, struct HksParamSet *genParamSet);
int32_t HksCrossTestImportKey(const struct HksBlob *keyAlias, struct HksParamSet *firstParamSet,
    struct HksParamSet *secondParamSet, struct HksBlob *newKeyAlias);
int32_t HksCrossTestAesEncrypt(const struct HksBlob *keyAlias, const struct HksParamSet *encryptParamSet,
    const struct HksBlob *inData, struct HksBlob *cipherText);
int32_t HksCrossTestAesDecrypt(const struct HksBlob *keyAlias, const struct HksParamSet *decryptParamSet,
    const struct HksBlob *cipherTest, struct HksBlob *plainText);
int32_t HksCrossTestRsaEncrypt(const struct HksBlob *keyAlias, const struct HksParamSet *encryptParamSet,
    const struct HksBlob *inData, struct HksBlob *cipherText);
int32_t HksCrossTestRsaDecrypt(const struct HksBlob *keyAlias, const struct HksParamSet *decryptParamSet,
    const struct HksBlob *cipherTest, struct HksBlob *plainText);
int32_t HksCrossTestSignVerify(const struct HksBlob *keyAlias, const struct HksParamSet *signVerifyParamSet,
    const struct HksBlob *inData, struct HksBlob *signedData);
int32_t HksCrossTestHmac(const struct HksBlob *keyAlias, const struct HksParamSet* hmacParamSet);
int32_t HksCrossTestAgreeGenerate(const struct HksBlob *keyAlias01, const struct HksBlob *keyAlias02,
    struct HksParamSet *genParamSet, const struct HksParam *genParams, const uint32_t genParamsCnt);
int32_t HksCrossTestAgreeExport(const struct HksBlob *keyAlias01, const struct HksBlob *keyAlias02,
    struct HksBlob *publicKey01, struct HksBlob *publicKey02, const struct HksParamSet *genParamSet);
void HksCrossTestAgreeFreeParamSet(struct HksParamSet *paramSet1, struct HksParamSet *paramSet2,
    struct HksParamSet *paramSet3, struct HksParamSet *paramSet4, struct HksParamSet *paramSet5);
void HksCrossTestAgreeDeleteKey(const struct HksBlob *keyAlias1, const struct HksBlob *keyAlias2);
int32_t HksCrossTestAgree(const struct HksBlob *keyAlias, const struct HksBlob *publicKey,
    const struct HksParamSet *initParamSet, const struct HksParamSet *finishParamSet, struct HksBlob *outData);
int32_t HksCrossTestDerive(const struct HksBlob *keyAlias, const struct HksParamSet *genParamSet,
    struct HksParamSet *initParamSet, struct HksParamSet *finishParamSet);

static struct HksBlob g_ecdhKeyAlias01 = {
    strlen("HksCrossTestEcdhAgree001_01"),
    (uint8_t *)"HksCrossTestEcdhAgree001_01"
};
static struct HksBlob g_ecdhKeyAlias02 = {
    strlen("HksCrossTestEcdhAgree001_02"),
    (uint8_t *)"HksCrossTestEcdhAgree001_02"
};
static struct HksParam g_ecdhGenParams[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_224
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_NONE
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }
};
static struct HksParam g_ecdhInitParams01[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_224
    }
};
static struct HksParam g_ecdhFinishParams01[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};
static struct HksParam g_ecdhInitParams02[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_224
    }
};
static struct HksParam g_ecdhFinishParams02[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};

static struct HksBlob g_x25519KeyAlias01 = {
    strlen("HksCrossTestX25519Agree001_01"),
    (uint8_t *)"HksCrossTestX25519Agree001_01"
};
static struct HksBlob g_x25519KeyAlias02 = {
    strlen("HksCrossTestX25519Agree001_02"),
    (uint8_t *)"HksCrossTestX25519Agree001_02"
};
#ifdef _USE_MBEDTLS_
static struct HksParam g_x25519GenParams[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#else
static struct HksParam g_x25519GenParams[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ED25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#endif // _USE_MBEDTLS_
static struct HksParam g_x25519InitParams01[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
static struct HksParam g_x25519FinishParams01[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};
static struct HksParam g_x25519InitParams02[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
static struct HksParam g_x25519FinishParams02[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};

static struct HksBlob g_hkdfKeyAliasFinal = {
    strlen("HksCrossTestHkdfDerive001_finish"),
    (uint8_t *)"HksCrossTestHkdfDerive001_finish"
};
static struct HksParam g_hkdfGenParams[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_128
    }
};
static struct HksParam g_hkdfInitParams[] = {
    {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag =  HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = DERIVE_KEY_SIZE_32
    }
};
static struct HksParam g_hkdfFinishParams[] = {
    {
        .tag =  HKS_TAG_KEY_ALIAS,
        .blob = g_hkdfKeyAliasFinal
    }, {
        .tag =  HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HKDF
    }, {
        .tag =  HKS_TAG_KEY_SIZE,
        .uint32Param = DERIVE_KEY_SIZE_32
    }, {
        .tag =  HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag =  HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }
};

static struct HksBlob g_pbkdf2KeyAliasFinal = {
    strlen("HksCrossTestHkdfDerive001_finish"),
    (uint8_t *)"HksCrossTestHkdfDerive001_finish"
};
static struct HksParam g_pbkdf2GenParams[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = COMMON_KEY_SIZE
    }
};
static struct HksParam g_pbkdf2InitParams[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_PBKDF2
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = COMMON_KEY_SIZE
    }, {
        .tag = HKS_TAG_ITERATION,
        .int32Param = 1000
    }, {
        .tag = HKS_TAG_SALT,
        .blob = {
            sizeof(SALT_DATA),
            (uint8_t *)SALT_DATA
        }
    }, {
        .tag = HKS_TAG_DERIVE_KEY_SIZE,
        .uint32Param = DERIVE_KEY_SIZE_32
    }
};
static struct HksParam g_pbkdf2FinishParams[] = {
    {
        .tag = HKS_TAG_KEY_ALIAS,
        .blob = g_pbkdf2KeyAliasFinal
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = DERIVE_KEY_SIZE_32
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }
};
} // namespace Unittest::CrossTest
#endif // HKS_CROSS_TEST_COMMON_H