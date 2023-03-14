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

#include "openssl_aes_helper.h"

#include <stdlib.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <securec.h>

#include "hks_param.h"

#define BIT_NUM_OF_UINT8 8
#define HKS_AE_TAG_LEN 16
#define OPENSSL_CTX_PADDING_NONE 0
#define OPENSSL_CTX_PADDING_ENABLE 1

int32_t GenerateAesKey(const int keyLen, struct HksBlob *randomKey)
{
    uint32_t keySize = keyLen / BIT_NUM_OF_UINT8;
    uint8_t *key = (uint8_t *)malloc(keySize);
    if (key == NULL) {
        return HKS_FAILURE;
    }
    do {
        if (RAND_bytes(key, keySize) != 1) {
            free(key);
            return HKS_FAILURE;
        }
        randomKey->size = keySize;
        if (memcpy_s(randomKey->data, randomKey->size, key, keySize) != 0) {
            free(key);
            return HKS_FAILURE;
        }
    } while (0);

    free(key);
    return HKS_SUCCESS;
}

static const EVP_CIPHER *AesCbcCrypt(uint32_t keyLen)
{
    if (keyLen == HKS_AES_KEY_SIZE_128) {
        return EVP_aes_128_cbc();
    } else if (keyLen == HKS_AES_KEY_SIZE_192) {
        return EVP_aes_192_cbc();
    } else if (keyLen == HKS_AES_KEY_SIZE_256) {
        return EVP_aes_256_cbc();
    }
    return NULL;
}

static const EVP_CIPHER *AesEcbCrypt(uint32_t keyLen)
{
    if (keyLen == HKS_AES_KEY_SIZE_128) {
        return EVP_aes_128_ecb();
    } else if (keyLen == HKS_AES_KEY_SIZE_192) {
        return EVP_aes_192_ecb();
    } else if (keyLen == HKS_AES_KEY_SIZE_256) {
        return EVP_aes_256_ecb();
    }
    return NULL;
}

static const EVP_CIPHER *AesCtrCrypt(uint32_t keyLen)
{
    if (keyLen == HKS_AES_KEY_SIZE_128) {
        return EVP_aes_128_ctr();
    } else if (keyLen == HKS_AES_KEY_SIZE_192) {
        return EVP_aes_192_ctr();
    } else if (keyLen == HKS_AES_KEY_SIZE_256) {
        return EVP_aes_256_ctr();
    }
    return NULL;
}

static const EVP_CIPHER *AesGcmCrypt(uint32_t keyLen)
{
    if (keyLen == HKS_AES_KEY_SIZE_128) {
        return EVP_aes_128_gcm();
    } else if (keyLen == HKS_AES_KEY_SIZE_192) {
        return EVP_aes_192_gcm();
    } else if (keyLen == HKS_AES_KEY_SIZE_256) {
        return EVP_aes_256_gcm();
    }
    return NULL;
}

static uint32_t AesInit(EVP_CIPHER_CTX **ctx, const EVP_CIPHER **ciper, const struct HksParamSet *paramSetIn)
{
    struct HksParam *mode = NULL;
    HksGetParam(paramSetIn, HKS_TAG_BLOCK_MODE, &mode);
    struct HksParam *keyLen = NULL;
    HksGetParam(paramSetIn, HKS_TAG_KEY_SIZE, &keyLen);
    if (mode->uint32Param == HKS_MODE_GCM) {
        *ciper = AesGcmCrypt(keyLen->uint32Param);
    } else if (mode->uint32Param == HKS_MODE_CBC) {
        *ciper = AesCbcCrypt(keyLen->uint32Param);
    } else if (mode->uint32Param == HKS_MODE_ECB) {
        *ciper = AesEcbCrypt(keyLen->uint32Param);
    } else if (mode->uint32Param == HKS_MODE_CTR) {
        *ciper = AesCtrCrypt(keyLen->uint32Param);
    }

    *ctx = EVP_CIPHER_CTX_new();
    if (*ctx == NULL) {
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

int32_t AesEncrypt(const struct HksParamSet *paramSetIn, const struct HksBlob *inData, struct HksBlob *outData,
    const struct HksBlob *randomKey)
{
    if (inData == NULL || inData->data == NULL || inData->size == 0 || outData == NULL || outData->data == NULL ||
        outData->size == 0) {
        return HKS_FAILURE;
    }
    struct HksParam *padding = NULL;
    HksGetParam(paramSetIn, HKS_TAG_PADDING, &padding);
    struct HksParam *iv = NULL;
    HksGetParam(paramSetIn, HKS_TAG_IV, &iv);

    const EVP_CIPHER *ciper = NULL;
    EVP_CIPHER_CTX *ctx = NULL;
    if (AesInit(&ctx, &ciper, paramSetIn) != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    if (EVP_EncryptInit_ex(ctx, ciper, NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    if (EVP_EncryptInit_ex(ctx, NULL, NULL, randomKey->data, iv->blob.data) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    int ret = 1;
    if (padding->uint32Param == HKS_PADDING_PKCS7) {
        ret = EVP_CIPHER_CTX_set_padding(ctx, OPENSSL_CTX_PADDING_ENABLE);
    } else if (padding->uint32Param == HKS_PADDING_NONE) {
        ret = EVP_CIPHER_CTX_set_padding(ctx, OPENSSL_CTX_PADDING_NONE);
    }
    if (ret != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return ret;
    }

    int outLen = 0;
    if (EVP_EncryptUpdate(ctx, outData->data, &outLen, inData->data, inData->size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    outData->size = (uint32_t)outLen;
    if (EVP_EncryptFinal_ex(ctx, outData->data + outLen, &outLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    outData->size += outLen;
    EVP_CIPHER_CTX_free(ctx);
    return HKS_SUCCESS;
}

int32_t AesDecrypt(const struct HksParamSet *paramSetIn, const struct HksBlob *inData, struct HksBlob *outData,
    const struct HksBlob *randomKey)
{
    if (inData == NULL || inData->data == NULL || inData->size == 0 || outData == NULL || outData->data == NULL ||
        outData->size == 0) {
        return HKS_FAILURE;
    }
    struct HksParam *padding = NULL;
    HksGetParam(paramSetIn, HKS_TAG_PADDING, &padding);
    struct HksParam *iv = NULL;
    HksGetParam(paramSetIn, HKS_TAG_IV, &iv);

    const EVP_CIPHER *ciper = NULL;
    EVP_CIPHER_CTX *ctx = NULL;
    if (AesInit(&ctx, &ciper, paramSetIn) != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    if (EVP_DecryptInit_ex(ctx, ciper, NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    if (EVP_DecryptInit_ex(ctx, NULL, NULL, randomKey->data, iv->blob.data) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    int ret = 1;
    if (padding->uint32Param == HKS_PADDING_PKCS7) {
        ret = EVP_CIPHER_CTX_set_padding(ctx, OPENSSL_CTX_PADDING_ENABLE);
    } else if (padding->uint32Param == HKS_PADDING_NONE) {
        ret = EVP_CIPHER_CTX_set_padding(ctx, OPENSSL_CTX_PADDING_NONE);
    }
    if (ret != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return ret;
    }

    int outLen = 0;
    if (EVP_DecryptUpdate(ctx, outData->data, &outLen, inData->data, inData->size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    outData->size = outLen;
    if (EVP_DecryptFinal_ex(ctx, outData->data + outLen, &outLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    outData->size += (uint32_t)outLen;
    EVP_CIPHER_CTX_free(ctx);
    return HKS_SUCCESS;
}

int32_t AesGcmEncrypt(const struct HksParamSet *paramSetIn, const struct HksBlob *inData, struct HksBlob *outData,
    const struct HksBlob *randomKey, const struct HksBlob *tagAead)
{
    if (inData == NULL || inData->data == NULL || inData->size == 0 || outData == NULL || outData->data == NULL ||
        outData->size == 0) {
        return HKS_FAILURE;
    }
    struct HksParam *iv = NULL;
    HksGetParam(paramSetIn, HKS_TAG_NONCE, &iv);
    struct HksParam *aad = NULL;
    HksGetParam(paramSetIn, HKS_TAG_ASSOCIATED_DATA, &aad);

    const EVP_CIPHER *ciper = NULL;
    EVP_CIPHER_CTX *ctx = NULL;
    if (AesInit(&ctx, &ciper, paramSetIn) != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    if (EVP_EncryptInit_ex(ctx, ciper, NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, iv->blob.size, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    if (EVP_EncryptInit_ex(ctx, NULL, NULL, randomKey->data, iv->blob.data) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    int outLen = 0;
    if (EVP_EncryptUpdate(ctx, NULL, &outLen, aad->blob.data, aad->blob.size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    if (EVP_EncryptUpdate(ctx, outData->data, &outLen, inData->data, inData->size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    outData->size = (uint32_t)outLen;
    if (EVP_EncryptFinal_ex(ctx, outData->data, &outLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, HKS_AE_TAG_LEN, tagAead->data) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    EVP_CIPHER_CTX_free(ctx);
    return HKS_SUCCESS;
}

int32_t AesGcmDecrypt(const struct HksParamSet *paramSetIn, const struct HksBlob *inData, struct HksBlob *outData,
    const struct HksBlob *randomKey, const struct HksBlob *tagDec)
{
    if (inData == NULL || inData->data == NULL || inData->size == 0 || outData == NULL || outData->data == NULL ||
        outData->size == 0) {
        return HKS_FAILURE;
    }
    struct HksParam *iv = NULL;
    HksGetParam(paramSetIn, HKS_TAG_NONCE, &iv);
    struct HksParam *aad = NULL;
    HksGetParam(paramSetIn, HKS_TAG_ASSOCIATED_DATA, &aad);

    const EVP_CIPHER *ciper = NULL;
    EVP_CIPHER_CTX *ctx = NULL;
    if (AesInit(&ctx, &ciper, paramSetIn) != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    if (EVP_DecryptInit_ex(ctx, ciper, NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, iv->blob.size, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    if (EVP_DecryptInit_ex(ctx, NULL, NULL, randomKey->data, iv->blob.data) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }

    int outLen = 0;
    if (EVP_DecryptUpdate(ctx, NULL, &outLen, aad->blob.data, aad->blob.size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    if (EVP_DecryptUpdate(ctx, outData->data, &outLen, inData->data, inData->size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    outData->size = (uint32_t)outLen;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, tagDec->size, tagDec->data) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    if (EVP_DecryptFinal_ex(ctx, outData->data, &outLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return HKS_FAILURE;
    }
    EVP_CIPHER_CTX_free(ctx);
    return HKS_SUCCESS;
}
