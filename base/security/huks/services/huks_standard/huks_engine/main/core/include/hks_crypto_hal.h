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

#ifndef HKS_CRYPTO_HAL_H
#define HKS_CRYPTO_HAL_H

#include "hks_type.h"

#ifdef __cplusplus
extern "C" {
#endif

enum HksKeyAlgMode {
    HKS_ALGORITHM_RSA_MODE_CRT = 1,
    HKS_ALGORITHM_RSA_MODE_NO_CRT = 2,
    HKS_ALGORITHM_EC_MODE_ECDH = 3,
    HKS_ALGORITHM_ED_MODE_SIG_VERIFY = 4,
    HKS_ALGORITHM_ED_MODE_VERIFY = 5,
    HKS_ALGORITHM_X25519_MODE = 6,
};

struct HksKeySpec {
    uint32_t algType;
    uint32_t keyLen;
    void *algParam; /* for example : struct HksKeyDerivationParam */
};

struct HksKeyDerivationParam {
    struct HksBlob salt;
    struct HksBlob info;
    uint32_t iterations;
    uint32_t digestAlg;
};

struct HksAeadParam {
    struct HksBlob nonce;
    struct HksBlob aad;
    union {
        struct HksBlob tagDec;
        uint32_t tagLenEnc;
    };
    uint32_t payloadLen;
};

struct HksCipherParam {
    struct HksBlob iv;
};

struct HksUsageSpec {
    uint32_t algType;
    uint32_t mode;
    uint32_t padding;
    uint32_t digest;
    uint32_t purpose;
    /*
     * Different algorithms correspond to different structures,for example:
     * struct HksAeadParam for aead;
     * struct HksCipherParam for cipher;
     */
    void *algParam;
};

struct KeyMaterialRsa {
    enum HksKeyAlg keyAlg;
    uint32_t keySize;
    uint32_t nSize;
    uint32_t eSize;
    uint32_t dSize;
};

struct KeyMaterialEcc {
    enum HksKeyAlg keyAlg;
    uint32_t keySize;
    uint32_t xSize;
    uint32_t ySize;
    uint32_t zSize;
};

struct KeyMaterial25519 {
    enum HksKeyAlg keyAlg;
    uint32_t keySize;
    uint32_t pubKeySize;
    uint32_t priKeySize;
    uint32_t reserved;
};

int32_t HksCryptoHalGetMainKey(const struct HksBlob *message, struct HksBlob *mainKey);

int32_t HksCryptoHalGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);

int32_t HksCryptoHalGetPubKey(const struct HksBlob *keyIn, struct HksBlob *keyOut);

int32_t HksCryptoHalDeriveKey(const struct HksBlob *mainKey, const struct HksKeySpec *derivationSpec,
    struct HksBlob *derivedKey);

int32_t HksCryptoHalFillRandom(struct HksBlob *randomData);

int32_t HksCryptoHalAddEntropy(const struct HksBlob *entropy);

int32_t HksCryptoHalAgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey);

int32_t HksCryptoHalSign(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *signature);

int32_t HksCryptoHalVerify(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const struct HksBlob *signature);

int32_t HksCryptoHalHmacInit(const struct HksBlob *key, uint32_t digestAlg, void **ctx);

int32_t HksCryptoHalHmacUpdate(const struct HksBlob *chunk, void *ctx);

int32_t HksCryptoHalHmacFinal(const struct HksBlob *msg, void **ctx, struct HksBlob *mac);

void HksCryptoHalHmacFreeCtx(void **ctx);

int32_t HksCryptoHalHmac(const struct HksBlob *key, uint32_t digestAlg, const struct HksBlob *msg,
    struct HksBlob *mac);

int32_t HksCryptoHalHashInit(uint32_t alg, void **ctx);

int32_t HksCryptoHalHashUpdate(const struct HksBlob *msg, void *ctx);

int32_t HksCryptoHalHashFinal(const struct HksBlob *msg, void **ctx, struct HksBlob *hash);

int32_t HksCryptoHalHash(uint32_t alg, const struct HksBlob *msg, struct HksBlob *hash);

void HksCryptoHalHashFreeCtx(void **ctx);

int32_t HksCryptoHalEncryptInit(const struct HksBlob *key, const struct HksUsageSpec *usageSpec, void **ctx);

int32_t HksCryptoHalEncryptUpdate(const struct HksBlob *message, void *ctx, struct HksBlob *out,
    const uint32_t algtype);

int32_t HksCryptoHalEncryptFinal(const struct HksBlob *message, void **ctx, struct HksBlob *cipherText,
    struct HksBlob *tagAead, const uint32_t algtype);

void HksCryptoHalEncryptFreeCtx(void **ctx, const uint32_t algtype);

int32_t HksCryptoHalEncrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText, struct HksBlob *tagAead);

int32_t HksCryptoHalDecryptInit(const struct HksBlob *key, const struct HksUsageSpec *usageSpec, void **ctx);

int32_t HksCryptoHalDecryptUpdate(const struct HksBlob *message, void *ctx, struct HksBlob *out,
    const uint32_t algtype);

int32_t HksCryptoHalDecryptFinal(const struct HksBlob *message, void **ctx, struct HksBlob *cipherText,
    struct HksBlob *tagAead, const uint32_t algtype);

void HksCryptoHalDecryptFreeCtx(void **ctx, const uint32_t algtype);

int32_t HksCryptoHalDecrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText);

int32_t HksCryptoHalBnExpMod(struct HksBlob *x, const struct HksBlob *a,
    const struct HksBlob *e, const struct HksBlob *n);

int32_t HksCryptoHalInit(void);

#ifdef __cplusplus
}
#endif

#endif /* HKS_CRYPTO_HAL_H */
