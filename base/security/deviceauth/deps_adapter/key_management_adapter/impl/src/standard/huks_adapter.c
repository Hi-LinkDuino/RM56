/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "huks_adapter.h"
#include "crypto_hash_to_point.h"
#include "hc_log.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_type.h"
#include "string_util.h"

#define BASE_IMPORT_PARAMS_LEN 7
#define EXT_IMPORT_PARAMS_LEN 2

static enum HksKeyPurpose g_purposeToHksKeyPurpose[] = {
    HKS_KEY_PURPOSE_MAC,
    HKS_KEY_PURPOSE_DERIVE
};

static enum HksKeyAlg g_algToHksAlgorithm[] = {
    HKS_ALG_ED25519,
    HKS_ALG_X25519,
    HKS_ALG_ECC
};

static int32_t BaseCheckParams(const Uint8Buff **inParams, const char **paramTags, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        CHECK_PTR_RETURN_HAL_ERROR_CODE(inParams[i], paramTags[i]);
        CHECK_PTR_RETURN_HAL_ERROR_CODE(inParams[i]->val, paramTags[i]);
        CHECK_LEN_ZERO_RETURN_ERROR_CODE(inParams[i]->length, paramTags[i]);
    }
    return HAL_SUCCESS;
}

static int32_t ConstructParamSet(struct HksParamSet **out, const struct HksParam *inParam,
    const uint32_t inParamNum)
{
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    if (ret != HKS_SUCCESS) {
        LOGE("init param set failed, ret = %d", ret);
        return HAL_ERR_INIT_PARAM_SET_FAILED;
    }

    ret = HksAddParams(paramSet, inParam, inParamNum);
    if (ret != HKS_SUCCESS) {
        LOGE("add param failed, ret = %d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_ERR_ADD_PARAM_FAILED;
    }

    ret = HksBuildParamSet(&paramSet);
    if (ret != HKS_SUCCESS) {
        LOGE("build param set failed, ret = %d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_ERR_BUILD_PARAM_SET_FAILED;
    }

    *out = paramSet;
    return HAL_SUCCESS;
}

static int32_t InitHks(void)
{
    return HksInitialize();
}

static int32_t Sha256(const Uint8Buff *message, Uint8Buff *hash)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(message, "message");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(message->val, "message->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(message->length, "message->length");

    CHECK_PTR_RETURN_HAL_ERROR_CODE(hash, "hash");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(hash->val, "hash->val");
    CHECK_LEN_EQUAL_RETURN(hash->length, SHA256_LEN, "hash->length");

    struct HksBlob srcBlob = { message->length, message->val };
    struct HksBlob hashBlob = { hash->length, hash->val };
    struct HksParamSet *paramSet = NULL;
    struct HksParam digestParam[] = {
        {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }
    };
    int32_t ret = ConstructParamSet(&paramSet, digestParam, CAL_ARRAY_SIZE(digestParam));
    if (ret != HAL_SUCCESS) {
        LOGE("construct param set failed, ret = %d", ret);
        return ret;
    }

    ret = HksHash(paramSet, &srcBlob, &hashBlob);
    if (ret != HKS_SUCCESS || hashBlob.size != SHA256_LEN) {
        HksFreeParamSet(&paramSet);
        return HAL_FAILED;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static int32_t GenerateRandom(Uint8Buff *rand)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(rand, "rand");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(rand->val, "rand->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(rand->length, "rand->length");

    struct HksBlob randBlob = { rand->length, rand->val };
    int32_t ret = HksGenerateRandom(NULL, &randBlob);
    if (ret != HKS_SUCCESS) {
        LOGE("Generate random failed, ret: %d", ret);
        return HAL_FAILED;
    }

    return HAL_SUCCESS;
}

static int32_t CheckKeyExist(const Uint8Buff *keyAlias)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias, "keyAlias");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias->val, "keyAlias->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(keyAlias->length, "keyAlias->length");

    struct HksBlob keyAliasBlob = { keyAlias->length, keyAlias->val };
    int32_t ret = HksKeyExist(&keyAliasBlob, NULL);
    if (ret != HKS_SUCCESS) {
        LOGI("Hks check key exist or not, ret = %d", ret);
        return HAL_FAILED;
    }

    return HAL_SUCCESS;
}

static int32_t DeleteKey(const Uint8Buff *keyAlias)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias, "keyAlias");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias->val, "keyAlias->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(keyAlias->length, "keyAlias->length");

    struct HksBlob keyAliasBlob = { keyAlias->length, keyAlias->val };
    int32_t ret = HksDeleteKey(&keyAliasBlob, NULL);
    if (ret == HKS_ERROR_NOT_EXIST) {
        LOGI("Key not exists.");
        return HAL_SUCCESS;
    }
    if (ret != HKS_SUCCESS) {
        LOGE("Delete key failed, ret = %d", ret);
        return HAL_FAILED;
    }

    return HAL_SUCCESS;
}

static int32_t ComputeHmac(const Uint8Buff *key, const Uint8Buff *message, Uint8Buff *outHmac, bool isAlias)
{
    const Uint8Buff *inParams[] = { key, message, outHmac };
    const char *paramTags[] = {"key", "message", "outHmac"};
    int32_t ret = BaseCheckParams(inParams, paramTags, CAL_ARRAY_SIZE(inParams));
    if (ret != HAL_SUCCESS) {
        return ret;
    }
    CHECK_LEN_EQUAL_RETURN(outHmac->length, HMAC_LEN, "outHmac->length");

    struct HksBlob keyBlob = { key->length, key->val };
    struct HksBlob srcBlob = { message->length, message->val };
    struct HksBlob hmacBlob = { outHmac->length, outHmac->val };
    struct HksParamSet *paramSet = NULL;
    struct HksParam hmacParam[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_MAC
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = isAlias
        }
    };
    ret = ConstructParamSet(&paramSet, hmacParam, CAL_ARRAY_SIZE(hmacParam));
    if (ret != HAL_SUCCESS) {
        LOGE("construct param set failed, ret = %d", ret);
        return ret;
    }

    ret = HksMac(&keyBlob, paramSet, &srcBlob, &hmacBlob);
    if (ret != HKS_SUCCESS  || hmacBlob.size != HMAC_LEN) {
        LOGE("Hmac failed, ret: %d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_FAILED;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static int32_t ComputeHkdf(const Uint8Buff *baseKey, const Uint8Buff *salt, const Uint8Buff *keyInfo,
    Uint8Buff *outHkdf, bool isAlias)
{
    const Uint8Buff *inParams[] = { baseKey, salt, outHkdf };
    const char *paramTags[] = { "baseKey", "salt", "outHkdf" };
    int32_t ret = BaseCheckParams(inParams, paramTags, CAL_ARRAY_SIZE(inParams));
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    struct HksBlob srcKeyBlob = { baseKey->length, baseKey->val };
    struct HksBlob saltBlob = { salt->length, salt->val };
    struct HksBlob keyInfoBlob = { 0, NULL };
    if (keyInfo != NULL) {
        keyInfoBlob.size = keyInfo->length;
        keyInfoBlob.data = keyInfo->val;
    }
    struct HksBlob derivedKeyBlob = { outHkdf->length, outHkdf->val };

    struct HksParamSet *paramSet = NULL;
    struct HksParam hkdfParam[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_DERIVE
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_HKDF
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_SALT,
            .blob = saltBlob
        }, {
            .tag = HKS_TAG_INFO,
            .blob = keyInfoBlob
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = isAlias
        }
    };

    ret = ConstructParamSet(&paramSet, hkdfParam, CAL_ARRAY_SIZE(hkdfParam));
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    ret = HksDeriveKey(paramSet, &srcKeyBlob, &derivedKeyBlob);
    if (ret != HKS_SUCCESS) {
        LOGE("Key derivation failed, ret: %d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_FAILED;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static int32_t CheckAesGcmEncryptParam(const Uint8Buff *key, const Uint8Buff *plain, const GcmParam *encryptInfo,
    Uint8Buff *outCipher)
{
    const Uint8Buff *inParams[] = { key, plain, outCipher };
    const char* paramTags[] = { "key", "plain", "outCipher" };
    int32_t ret = BaseCheckParams(inParams, paramTags, CAL_ARRAY_SIZE(inParams));
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    CHECK_PTR_RETURN_HAL_ERROR_CODE(encryptInfo, "encryptInfo");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(encryptInfo->aad, "aad");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(encryptInfo->aadLen, "aadLen");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(encryptInfo->nonce, "nonce");
    CHECK_LEN_LOWER_RETURN(encryptInfo->nonceLen, HKS_AE_NONCE_LEN, "nonceLen");
    CHECK_LEN_LOWER_RETURN(outCipher->length, plain->length + HKS_AE_TAG_LEN, "outCipher");

    return HAL_SUCCESS;
}

static int32_t AesGcmEncrypt(const Uint8Buff *key, const Uint8Buff *plain,
    const GcmParam *encryptInfo, bool isAlias, Uint8Buff *outCipher)
{
    int32_t ret = CheckAesGcmEncryptParam(key, plain, encryptInfo, outCipher);
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    struct HksBlob keyBlob = { key->length, key->val };
    struct HksBlob plainBlob = { plain->length, plain->val };
    struct HksBlob cipherBlob = { outCipher->length, outCipher->val };

    struct HksParamSet *paramSet = NULL;
    struct HksParam encryptParam[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_ENCRYPT
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES
        }, {
            .tag = HKS_TAG_BLOCK_MODE,
            .uint32Param = HKS_MODE_GCM
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_NONCE,
            .blob = { encryptInfo->nonceLen, encryptInfo->nonce }
        }, {
            .tag = HKS_TAG_ASSOCIATED_DATA,
            .blob = { encryptInfo->aadLen, encryptInfo->aad }
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = isAlias
        }
    };

    ret = ConstructParamSet(&paramSet, encryptParam, CAL_ARRAY_SIZE(encryptParam));
    if (ret != HAL_SUCCESS) {
        LOGE("construct param set failed, ret = %d", ret);
        return ret;
    }

    ret = HksEncrypt(&keyBlob, paramSet, &plainBlob, &cipherBlob);
    if (ret != HKS_SUCCESS) {
        LOGE("Aes-gcm encrypt failed, ret: %d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_FAILED;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static int32_t CheckAesGcmDecryptParam(const Uint8Buff *key, const Uint8Buff *cipher, const GcmParam *decryptInfo,
    Uint8Buff *outPlain)
{
    const Uint8Buff *inParams[] = { key, cipher, outPlain };
    const char *paramTags[] = { "key", "cipher", "outPlain" };
    int32_t ret = BaseCheckParams(inParams, paramTags, CAL_ARRAY_SIZE(inParams));
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    CHECK_PTR_RETURN_HAL_ERROR_CODE(decryptInfo, "decryptInfo");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(decryptInfo->aad, "aad");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(decryptInfo->aadLen, "aadLen");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(decryptInfo->nonce, "nonce");
    CHECK_LEN_LOWER_RETURN(decryptInfo->nonceLen, HKS_AE_NONCE_LEN, "nonceLen");
    CHECK_LEN_LOWER_RETURN(outPlain->length, cipher->length - HKS_AE_TAG_LEN, "outPlain");

    return HAL_SUCCESS;
}

static int32_t AesGcmDecrypt(const Uint8Buff *key, const Uint8Buff *cipher,
    const GcmParam *decryptInfo, bool isAlias, Uint8Buff *outPlain)
{
    int32_t ret = CheckAesGcmDecryptParam(key, cipher, decryptInfo, outPlain);
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    struct HksBlob keyBlob = { key->length, key->val };
    struct HksBlob cipherBlob = { cipher->length, cipher->val };
    struct HksBlob plainBlob = { outPlain->length, outPlain->val };

    struct HksParamSet *paramSet = NULL;
    struct HksParam decryptParam[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_DECRYPT
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES
        }, {
            .tag = HKS_TAG_BLOCK_MODE,
            .uint32Param = HKS_MODE_GCM
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_NONCE,
            .blob = { decryptInfo->nonceLen, decryptInfo->nonce }
        }, {
            .tag = HKS_TAG_ASSOCIATED_DATA,
            .blob = { decryptInfo->aadLen, decryptInfo->aad }
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = isAlias
        }
    };

    ret = ConstructParamSet(&paramSet, decryptParam, CAL_ARRAY_SIZE(decryptParam));
    if (ret != HAL_SUCCESS) {
        LOGE("construct param set failed, ret = %d", ret);
        return ret;
    }

    ret = HksDecrypt(&keyBlob, paramSet, &cipherBlob, &plainBlob);
    if (ret != HKS_SUCCESS) {
        LOGE("Aes-gcm decrypt failed, ret: %d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_FAILED;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static int32_t HashToPoint(const Uint8Buff *hash, Algorithm algo, Uint8Buff *outEcPoint)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(hash, "hash");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(hash->val, "hash->va");
    CHECK_LEN_EQUAL_RETURN(hash->length, SHA256_LEN, "hash->length");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(outEcPoint, "outEcPoint");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(outEcPoint->val, "outEcPoint->va");
    CHECK_LEN_EQUAL_RETURN(outEcPoint->length, SHA256_LEN, "outEcPoint->length");

    if (algo != X25519) {
        LOGE("Invalid algo: %d.", algo);
        return HAL_ERR_INVALID_PARAM;
    }

    struct HksBlob hashBlob = { hash->length, hash->val };
    struct HksBlob pointBlob = { outEcPoint->length, outEcPoint->val };

    int32_t ret = OpensslHashToPoint(&hashBlob, &pointBlob);
    if (ret != HAL_SUCCESS || pointBlob.size != SHA256_LEN) {
        LOGE("Hks hashToPoint failed, ret: %d", ret);
        return HAL_FAILED;
    }

    return HAL_SUCCESS;
}

static int32_t ConstructAgreeWithStorageParams(struct HksParamSet **paramSet, uint32_t keyLen, Algorithm algo,
    const KeyBuff *priKey, const KeyBuff *pubKey)
{
    struct HksBlob priKeyBlob = { priKey->keyLen, priKey->key };
    struct HksBlob pubKeyBlob = { pubKey->keyLen, pubKey->key };
    struct HksParam agreeParam[] = {
        {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = keyLen * BITS_PER_BYTE
        }, {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_DERIVE
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }, {
            .tag = HKS_TAG_KEY_GENERATE_TYPE,
            .uint32Param = HKS_KEY_GENERATE_TYPE_AGREE
        }, {
            .tag = HKS_TAG_AGREE_ALG,
            .uint32Param = g_algToHksAlgorithm[algo]
        }, {
            .tag = HKS_TAG_AGREE_PRIVATE_KEY_ALIAS,
            .blob = priKeyBlob
        }, {
            .tag = HKS_TAG_AGREE_PUBLIC_KEY,
            .blob = pubKeyBlob
        }, {
            .tag = HKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS,
            .boolParam = pubKey->isAlias
        }
    };

    int32_t ret = ConstructParamSet(paramSet, agreeParam, CAL_ARRAY_SIZE(agreeParam));
    if (ret != HAL_SUCCESS) {
        LOGE("Construct param set failed, ret = %d", ret);
        return ret;
    }
    return ret;
}

static int32_t AgreeSharedSecretWithStorage(const KeyBuff *priKey, const KeyBuff *pubKey, Algorithm algo,
    uint32_t sharedKeyLen, const Uint8Buff *sharedKeyAlias)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(priKey, "priKey");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(priKey->key, "priKey->key");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(priKey->keyLen, "priKey->keyLen");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(pubKey, "pubKey");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(pubKey->key, "pubKey->key");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(pubKey->keyLen, "pubKey->keyLen");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(sharedKeyAlias, "sharedKeyAlias");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(sharedKeyAlias->val, "sharedKeyAlias->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(sharedKeyAlias->length, "sharedKeyAlias->length");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(sharedKeyLen, "sharedKeyLen");

    struct HksBlob sharedKeyAliasBlob = { sharedKeyAlias->length, sharedKeyAlias->val };
    struct HksParamSet *paramSet = NULL;
    int32_t ret = ConstructAgreeWithStorageParams(&paramSet, sharedKeyLen, algo, priKey, pubKey);
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    ret = HksGenerateKey(&sharedKeyAliasBlob, paramSet, NULL);
    if (ret != HKS_SUCCESS) {
        LOGE("Hks agree key with storage failed, ret = %d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_FAILED;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static int32_t AgreeSharedSecret(const KeyBuff *priKey, const KeyBuff *pubKey, Algorithm algo, Uint8Buff *sharedKey)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(priKey, "priKey");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(priKey->key, "priKey->key");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(priKey->keyLen, "priKey->keyLen");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(pubKey, "pubKey");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(pubKey->key, "pubKey->key");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(pubKey->keyLen, "pubKey->keyLen");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(sharedKey, "sharedKey");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(sharedKey->val, "sharedKey->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(sharedKey->length, "sharedKey->length");

    struct HksBlob priKeyBlob = { priKey->keyLen, priKey->key };
    struct HksBlob pubKeyBlob = { pubKey->keyLen, pubKey->key };
    struct HksBlob sharedKeyBlob = { sharedKey->length, sharedKey->val };

    struct HksParamSet *paramSet = NULL;
    struct HksParam agreeParam[] = {
        {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = g_algToHksAlgorithm[algo] // only support HKS_ALG_X25519 now
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = sharedKey->length * BITS_PER_BYTE
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = priKey->isAlias
        }
    };

    int32_t ret = ConstructParamSet(&paramSet, agreeParam, CAL_ARRAY_SIZE(agreeParam));
    if (ret != HAL_SUCCESS) {
        LOGE("Construct param set failed, ret = %d", ret);
        return ret;
    }

    ret = HksAgreeKey(paramSet, &priKeyBlob, &pubKeyBlob, &sharedKeyBlob);
    if (ret != HKS_SUCCESS) {
        LOGE("Agree key failed, ret = %d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_FAILED;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static int32_t BigNumExpMod(const Uint8Buff *base, const Uint8Buff *exp, const char *bigNumHex, Uint8Buff *outNum)
{
    const Uint8Buff *inParams[] = { base, exp, outNum };
    const char *paramTags[] = { "base", "exp", "outNum" };
    int32_t ret = BaseCheckParams(inParams, paramTags, CAL_ARRAY_SIZE(inParams));
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    CHECK_PTR_RETURN_HAL_ERROR_CODE(bigNumHex, "bigNumHex");
    uint32_t primeLen = strlen(bigNumHex) / BYTE_TO_HEX_OPER_LENGTH;
    if ((primeLen != BIG_PRIME_LEN_384) && (primeLen != BIG_PRIME_LEN_256)) {
        LOGE("Not support big number len %d", outNum->length);
        return HAL_FAILED;
    }
    CHECK_LEN_EQUAL_RETURN(outNum->length, primeLen, "outNum->length");

    struct HksBlob baseBlob = { base->length, base->val };
    struct HksBlob expBlob = { exp->length, exp->val };
    struct HksBlob outNumBlob = { outNum->length, outNum->val };
    struct HksBlob bigNumBlob = { 0, NULL };
    bigNumBlob.size = outNum->length;
    bigNumBlob.data = (uint8_t *)HcMalloc(bigNumBlob.size, 0);
    if (bigNumBlob.data == NULL) {
        LOGE("malloc bigNumBlob.data failed.");
        return HAL_ERR_BAD_ALLOC;
    }
    ret = HexStringToByte(bigNumHex, bigNumBlob.data, bigNumBlob.size);
    if (ret != HAL_SUCCESS) {
        LOGE("HexStringToByte for bigNumHex failed.");
        HcFree(bigNumBlob.data);
        return ret;
    }

    ret = HksBnExpMod(&outNumBlob, &baseBlob, &expBlob, &bigNumBlob);
    if (ret != HKS_SUCCESS) {
        LOGE("Huks calculate big number exp mod failed, ret = %d", ret);
        HcFree(bigNumBlob.data);
        return HAL_FAILED;
    }
    outNum->length = outNumBlob.size;

    HcFree(bigNumBlob.data);
    return HAL_SUCCESS;
}

static int32_t ConstructGenerateKeyPairWithStorageParams(struct HksParamSet **paramSet, Algorithm algo,
    uint32_t keyLen, const struct HksBlob *authIdBlob)
{
    struct HksParam keyParam[] = {
        {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = g_algToHksAlgorithm[algo]
        }, {
            .tag = HKS_TAG_KEY_STORAGE_FLAG,
            .uint32Param = HKS_STORAGE_PERSISTENT
        }, {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = keyLen * BITS_PER_BYTE
        }, {
            .tag = HKS_TAG_KEY_AUTH_ID,
            .blob = *authIdBlob
        }
    };

    int32_t ret = ConstructParamSet(paramSet, keyParam, CAL_ARRAY_SIZE(keyParam));
    if (ret != HAL_SUCCESS) {
        LOGE("Construct param set failed, ret = %d", ret);
        return ret;
    }
    return ret;
}

static int32_t GenerateKeyPairWithStorage(const Uint8Buff *keyAlias, uint32_t keyLen, Algorithm algo,
    const ExtraInfo *exInfo)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias, "keyAlias");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias->val, "keyAlias->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(keyAlias->length, "keyAlias->length");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(exInfo, "exInfo");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(exInfo->authId.val, "authId->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(exInfo->authId.length, "authId->length");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(keyLen, "keyLen");

    struct HksBlob keyAliasBlob = { keyAlias->length, keyAlias->val };
    struct HksBlob authIdBlob = { exInfo->authId.length, exInfo->authId.val };
    struct HksParamSet *paramSet = NULL;
    int32_t ret = ConstructGenerateKeyPairWithStorageParams(&paramSet, algo, keyLen, &authIdBlob);
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    ret = HksGenerateKey(&keyAliasBlob, paramSet, NULL);
    if (ret != HKS_SUCCESS) {
        LOGE("Hks generate failed, ret=%d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_FAILED;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static int32_t GetKeyPair(struct HksParamSet *outParamSet, Uint8Buff *outPriKey, Uint8Buff *outPubKey)
{
    int32_t ret = HksFreshParamSet(outParamSet, false); /* false means fresh by local, not through IPC */
    if (ret != HKS_SUCCESS) {
        LOGE("fresh param set failed, ret:%d", ret);
        return HAL_ERR_FRESH_PARAM_SET_FAILED;
    }

    struct HksParam *pubKeyParam = NULL;
    ret = HksGetParam(outParamSet, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubKeyParam);
    if (ret != HKS_SUCCESS) {
        LOGE("get pub key from param set failed, ret:%d", ret);
        return HAL_ERR_GET_PARAM_FAILED;
    }

    struct HksParam *priKeyParam = NULL;
    ret = HksGetParam(outParamSet, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priKeyParam);
    if (ret != HKS_SUCCESS) {
        LOGE("get priv key from param set failed, ret:%d", ret);
        return HAL_ERR_GET_PARAM_FAILED;
    }

    if (memcpy_s(outPubKey->val, outPubKey->length, pubKeyParam->blob.data, pubKeyParam->blob.size) != EOK) {
        LOGE("parse x25519 output param set memcpy public key failed!");
        return HAL_ERR_MEMORY_COPY;
    }
    outPubKey->length = pubKeyParam->blob.size;

    if (memcpy_s(outPriKey->val, outPriKey->length, priKeyParam->blob.data, priKeyParam->blob.size) != EOK) {
        LOGE("parse x25519 output param set memcpy private key failed!");
        return HAL_ERR_MEMORY_COPY;
    }
    outPriKey->length = priKeyParam->blob.size;

    return HAL_SUCCESS;
}

static int32_t ConstructGenerateKeyPairParams(struct HksParamSet **paramSet, Algorithm algo, uint32_t keyLen)
{
    struct HksParam keyParam[] = {
        {
            .tag = HKS_TAG_KEY_STORAGE_FLAG,
            .uint32Param = HKS_STORAGE_TEMP
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = g_algToHksAlgorithm[algo]
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = keyLen * BITS_PER_BYTE
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .uint32Param = false
        }
    };

    int32_t ret = ConstructParamSet(paramSet, keyParam, CAL_ARRAY_SIZE(keyParam));
    if (ret != HAL_SUCCESS) {
        LOGE("Construct param set failed, ret = %d", ret);
        return ret;
    }
    return ret;
}

static int32_t GenerateKeyPair(Algorithm algo, Uint8Buff *outPriKey, Uint8Buff *outPubKey)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(outPriKey, "outPriKey");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(outPriKey->val, "outPriKey->key");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(outPriKey->length, "outPriKey->keyLen");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(outPubKey, "outPubKey");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(outPubKey->val, "outPubKey->key");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(outPubKey->length, "outPubKey->keyLen");

    if (outPriKey->length != outPubKey->length) {
        LOGE("key len not equal.");
        return HAL_ERR_INVALID_LEN;
    }
    uint32_t keyLen = outPriKey->length;

    struct HksParamSet *paramSet = NULL;
    struct HksParamSet *outParamSet = NULL;
    int32_t ret = ConstructGenerateKeyPairParams(&paramSet, algo, keyLen);
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    /* need 2 HksParam struct for outPriKey and outPubKey */
    uint32_t outParamSetSize = sizeof(struct HksParamSet) +
        2 * (sizeof(struct HksParam)) + outPriKey->length + outPubKey->length;
    outParamSet = (struct HksParamSet *)HcMalloc(outParamSetSize, 0);
    if (outParamSet == NULL) {
        LOGE("allocate buffer for output param set failed");
        ret = HAL_ERR_BAD_ALLOC;
        goto ERR;
    }
    outParamSet->paramSetSize = outParamSetSize;

    ret = HksGenerateKey(NULL, paramSet, outParamSet);
    if (ret != HKS_SUCCESS) {
        LOGE("generate x25519 key failed, ret:%d", ret);
        ret = HAL_FAILED;
        goto ERR;
    }

    ret = GetKeyPair(outParamSet, outPriKey, outPubKey);
    if (ret != HAL_SUCCESS) {
        LOGE("parse x25519 output param set failed, ret:%d", ret);
        goto ERR;
    }
ERR:
    HksFreeParamSet(&paramSet);
    HcFree(outParamSet);
    return ret;
}

static int32_t ExportPublicKey(const Uint8Buff *keyAlias, Uint8Buff *outPubKey)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias, "keyAlias");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias->val, "keyAlias->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(keyAlias->length, "keyAlias->length");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(outPubKey, "outPubKey");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(outPubKey->val, "outPubKey->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(outPubKey->length, "outPubKey->length");

    struct HksBlob keyAliasBlob = { keyAlias->length, keyAlias->val };
    struct HksBlob keyBlob = { outPubKey->length, outPubKey->val };

    int32_t ret = HksExportPublicKey(&keyAliasBlob, NULL, &keyBlob);
    if (ret != HKS_SUCCESS) {
        LOGE("Export public key failed, ret=%d", ret);
        return HAL_FAILED;
    }
    outPubKey->length = keyBlob.size;

    return HAL_SUCCESS;
}

static int32_t ConstructSignParams(struct HksParamSet **paramSet, Algorithm algo)
{
    struct HksParam signParam[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_SIGN
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = g_algToHksAlgorithm[algo] // only support HKS_ALG_ED25519
        }
    };

    int32_t ret = ConstructParamSet(paramSet, signParam, CAL_ARRAY_SIZE(signParam));
    if (ret != HAL_SUCCESS) {
        LOGE("Construct param set failed, ret = %d", ret);
        return ret;
    }
    return ret;
}

static int32_t Sign(const Uint8Buff *keyAlias, const Uint8Buff *message, Algorithm algo,
    Uint8Buff *outSignature, bool isAlias)
{
    struct HksParamSet *paramSet = NULL;
    const Uint8Buff *inParams[] = { keyAlias, message, outSignature };
    const char *paramTags[] = { "keyAlias", "message", "outSignature" };
    int32_t ret = BaseCheckParams(inParams, paramTags, CAL_ARRAY_SIZE(inParams));
    if (ret != HAL_SUCCESS) {
        return ret;
    }
    CHECK_LEN_EQUAL_RETURN(outSignature->length, SIGNATURE_LEN, "outSignature->length");

    struct HksBlob keyAliasBlob = { keyAlias->length, keyAlias->val };
    Uint8Buff messageHash = { NULL, 0 };
    messageHash.length = SHA256_LEN;
    messageHash.val = (uint8_t *)HcMalloc(messageHash.length, 0);
    if (messageHash.val == NULL) {
        LOGE("malloc messageHash.data failed.");
        ret = HAL_ERR_BAD_ALLOC;
        goto ERR;
    }
    ret = Sha256(message, &messageHash);
    if (ret != HAL_SUCCESS) {
        LOGE("Sha256 failed.");
        goto ERR;
    }
    struct HksBlob messageBlob = { messageHash.length, messageHash.val };
    struct HksBlob signatureBlob = { outSignature->length, outSignature->val };

    ret = ConstructSignParams(&paramSet, algo);
    if (ret != HAL_SUCCESS) {
        goto ERR;
    }

    ret = HksSign(&keyAliasBlob, paramSet, &messageBlob, &signatureBlob);
    if ((ret != HKS_SUCCESS) || (signatureBlob.size != SIGNATURE_LEN)) {
        LOGE("Hks sign failed.");
        ret = HAL_FAILED;
        goto ERR;
    }
    ret = HAL_SUCCESS;
ERR:
    HksFreeParamSet(&paramSet);
    HcFree(messageHash.val);
    return ret;
}

static int32_t ConstructVerifyParams(struct HksParamSet **paramSet, Algorithm algo, bool isAlias)
{
    struct HksParam verifyParam[] = {
        {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = g_algToHksAlgorithm[algo] // only support HKS_ALG_ED25519
        }, {
            .tag = HKS_TAG_IS_KEY_ALIAS,
            .boolParam = isAlias
        }
    };

    int32_t ret = ConstructParamSet(paramSet, verifyParam, CAL_ARRAY_SIZE(verifyParam));
    if (ret != HAL_SUCCESS) {
        LOGE("Construct param set failed, ret = %d", ret);
        return ret;
    }
    return ret;
}

static int32_t Verify(const Uint8Buff *key, const Uint8Buff *message, Algorithm algo,
    const Uint8Buff *signature, bool isAlias)
{
    struct HksParamSet *paramSet = NULL;
    const Uint8Buff *inParams[] = { key, message, signature };
    const char *paramTags[] = { "key", "message", "signature" };
    int32_t ret = BaseCheckParams(inParams, paramTags, CAL_ARRAY_SIZE(inParams));
    if (ret != HAL_SUCCESS) {
        return ret;
    }
    CHECK_LEN_EQUAL_RETURN(signature->length, SIGNATURE_LEN, "signature");

    struct HksBlob keyAliasBlob = { key->length, key->val };
    Uint8Buff messageHash = { NULL, 0 };
    messageHash.length = SHA256_LEN;
    messageHash.val = (uint8_t *)HcMalloc(messageHash.length, 0);
    if (messageHash.val == NULL) {
        LOGE("malloc messageHash.data failed.");
        ret = HAL_ERR_BAD_ALLOC;
        goto ERR;
    }
    ret = Sha256(message, &messageHash);
    if (ret != HAL_SUCCESS) {
        LOGE("Sha256 failed.");
        goto ERR;
    }
    struct HksBlob messageBlob = { messageHash.length, messageHash.val };
    struct HksBlob signatureBlob = { signature->length, signature->val };

    ret = ConstructVerifyParams(&paramSet, algo, isAlias);
    if (ret != HAL_SUCCESS) {
        goto ERR;
    }

    ret = HksVerify(&keyAliasBlob, paramSet, &messageBlob, &signatureBlob);
    if ((ret != HKS_SUCCESS)) {
        LOGE("HksVerify failed, ret: %d", ret);
        ret = HAL_FAILED;
        goto ERR;
    }
    ret = HAL_SUCCESS;
ERR:
    HksFreeParamSet(&paramSet);
    HcFree(messageHash.val);
    return ret;
}

static int32_t ConstructImportPublicKeyParams(struct HksParamSet **paramSet, Algorithm algo, uint32_t keyLen,
    const struct HksBlob *authIdBlob, const union KeyRoleInfoUnion *roleInfoUnion)
{
    struct HksParam importParam[] = {
        {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = g_algToHksAlgorithm[algo]
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = keyLen * BITS_PER_BYTE
        }, {
            .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE
        }, {
            .tag = HKS_TAG_KEY_AUTH_ID,
            .blob = *authIdBlob
        }, {
            .tag = HKS_TAG_IS_ALLOWED_WRAP,
            .boolParam = true
        }, {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_VERIFY
        }, {
            .tag = HKS_TAG_KEY_ROLE,
            .uint32Param = roleInfoUnion->roleInfo
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA256
        }
    };

    int32_t ret = ConstructParamSet(paramSet, importParam, CAL_ARRAY_SIZE(importParam));
    if (ret != HAL_SUCCESS) {
        LOGE("Construct param set failed, ret = %d", ret);
        return ret;
    }
    return ret;
}

static int32_t ImportPublicKey(const Uint8Buff *keyAlias, const Uint8Buff *pubKey, Algorithm algo,
    const ExtraInfo *exInfo)
{
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias, "keyAlias");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(keyAlias->val, "keyAlias->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(keyAlias->length, "keyAlias->length");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(pubKey, "pubKey");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(pubKey->val, "pubKey->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(pubKey->length, "pubKey->length");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(exInfo, "exInfo");
    CHECK_PTR_RETURN_HAL_ERROR_CODE(exInfo->authId.val, "authId->val");
    CHECK_LEN_ZERO_RETURN_ERROR_CODE(exInfo->authId.length, "authId->length");
    CHECK_LEN_HIGHER_RETURN(exInfo->pairType, PAIR_TYPE_END - 1, "pairType");

    struct HksBlob keyAliasBlob = { keyAlias->length, keyAlias->val };
    struct HksBlob pubKeyBlob = { pubKey->length, pubKey->val };

    struct HksBlob authIdBlob = { exInfo->authId.length, exInfo->authId.val };
    union KeyRoleInfoUnion roleInfoUnion;
    roleInfoUnion.roleInfoStruct.userType = (uint8_t)exInfo->userType;
    roleInfoUnion.roleInfoStruct.pairType = (uint8_t)exInfo->pairType;
    roleInfoUnion.roleInfoStruct.reserved1 = (uint8_t)0;
    roleInfoUnion.roleInfoStruct.reserved2 = (uint8_t)0;

    struct HksParamSet *paramSet = NULL;

    int32_t ret = ConstructImportPublicKeyParams(&paramSet, algo, pubKey->length, &authIdBlob, &roleInfoUnion);
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    ret = HksImportKey(&keyAliasBlob, paramSet, &pubKeyBlob);
    if (ret != HKS_SUCCESS) {
        LOGE("Hks importKey failed, ret: %d", ret);
        HksFreeParamSet(&paramSet);
        return HAL_FAILED;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static bool CheckBigNumCompareParams(const Uint8Buff *a, const Uint8Buff *b, int *res)
{
    if ((a == NULL || a->val == NULL) && (b == NULL || b->val == NULL)) {
        *res = 0; // a = b
        return false;
    }
    if ((a == NULL || a->val == NULL) && (b != NULL && b->val != NULL)) {
        *res = 1; // a < b
        return false;
    }
    if ((a != NULL && a->val != NULL) && (b == NULL || b->val == NULL)) {
        *res = -1; // a > b
        return false;
    }
    return true;
}

static int32_t BigNumCompare(const Uint8Buff *a, const Uint8Buff *b)
{
    int res = 0;
    if (!CheckBigNumCompareParams(a, b, &res)) {
        return res;
    }
    const uint8_t *tmpA = a->val;
    const uint8_t *tmpB = b->val;
    uint32_t len = a->length;
    if (a->length < b->length) {
        for (uint32_t i = 0; i < b->length - a->length; i++) {
            if (b->val[i] > 0) {
                return 1; // a < b
            }
        }
        tmpA = a->val;
        tmpB = b->val + b->length - a->length;
        len = a->length;
    }
    if (a->length > b->length) {
        for (uint32_t i = 0; i < a->length - b->length; i++) {
            if (a->val[i] > 0) {
                return -1; // a > b
            }
        }
        tmpA = a->val + a->length - b->length;
        tmpB = b->val;
        len = b->length;
    }
    for (uint32_t i = 0; i < len; i++) {
        if (*(tmpA + i) > *(tmpB + i)) {
            return -1; // a > b
        }
        if (*(tmpA + i) < *(tmpB + i)) {
            return 1; // a < b
        }
    }
    return 0; // a == b
}

static bool CheckDlPublicKey(const Uint8Buff *key, const char *primeHex)
{
    if (key == NULL || key->val == NULL || primeHex == NULL) {
        LOGE("Params is null.");
        return false;
    }
    uint8_t min = 1;

    uint32_t innerKeyLen = HcStrlen(primeHex) / BYTE_TO_HEX_OPER_LENGTH;
    if (key->length > innerKeyLen) {
        LOGE("Key length > prime number length.");
        return false;
    }
    uint8_t *primeByte = (uint8_t *)HcMalloc(innerKeyLen, 0);
    if (primeByte == NULL) {
        LOGE("Malloc for primeByte failed.");
        return false;
    }
    if (HexStringToByte(primeHex, primeByte, innerKeyLen) != HAL_SUCCESS) {
        LOGE("Convert prime number from hex string to byte failed.");
        HcFree(primeByte);
        return false;
    }
    /*
     * P - 1, since the last byte of large prime number must be greater than 1,
     * needn't to think about borrowing forward
     */
    primeByte[innerKeyLen - 1] -= 1;

    Uint8Buff minBuff = { &min, sizeof(uint8_t) };
    if (BigNumCompare(key, &minBuff) >= 0) {
        LOGE("Pubkey is invalid, key <= 1.");
        HcFree(primeByte);
        return false;
    }

    Uint8Buff primeBuff = { primeByte, innerKeyLen };
    if (BigNumCompare(key, &primeBuff) <= 0) {
        LOGE("Pubkey is invalid, key >= p - 1.");
        HcFree(primeByte);
        return false;
    }

    HcFree(primeByte);
    return true;
}

static int32_t CheckImportSymmetricKeyParam(const Uint8Buff *keyAlias, const Uint8Buff *authToken)
{
    const Uint8Buff *inParams[] = { keyAlias, authToken };
    const char *paramTags[] = { "keyAlias", "authToken" };
    return BaseCheckParams(inParams, paramTags, CAL_ARRAY_SIZE(inParams));
}

static int32_t ConstructImportSymmetricKeyParam(struct HksParamSet **paramSet, uint32_t keyLen, KeyPurpose purpose,
    const ExtraInfo *exInfo)
{
    struct HksParam *importParam = NULL;
    struct HksBlob authIdBlob = { 0, NULL };
    union KeyRoleInfoUnion roleInfoUnion;
    (void)memset_s(&roleInfoUnion, sizeof(roleInfoUnion), 0, sizeof(roleInfoUnion));
    uint32_t idx = 0;
    if (exInfo != NULL) {
        CHECK_PTR_RETURN_HAL_ERROR_CODE(exInfo->authId.val, "authId");
        CHECK_LEN_ZERO_RETURN_ERROR_CODE(exInfo->authId.length, "authId");
        CHECK_LEN_HIGHER_RETURN(exInfo->pairType, PAIR_TYPE_END - 1, "pairType");
        importParam = (struct HksParam *)HcMalloc(sizeof(struct HksParam) *
            (BASE_IMPORT_PARAMS_LEN + EXT_IMPORT_PARAMS_LEN), 0);
        if (importParam == NULL) {
            LOGE("Malloc for importParam failed.");
            return HAL_ERR_BAD_ALLOC;
        }
        authIdBlob.size = exInfo->authId.length;
        authIdBlob.data = exInfo->authId.val;
        roleInfoUnion.roleInfoStruct.userType = (uint8_t)exInfo->userType;
        roleInfoUnion.roleInfoStruct.pairType = (uint8_t)exInfo->pairType;
        importParam[idx].tag = HKS_TAG_KEY_AUTH_ID;
        importParam[idx++].blob = authIdBlob;
        importParam[idx].tag = HKS_TAG_KEY_ROLE;
        importParam[idx++].uint32Param = roleInfoUnion.roleInfo;
    } else {
        importParam = (struct HksParam *)HcMalloc(sizeof(struct HksParam) * BASE_IMPORT_PARAMS_LEN, 0);
        if (importParam == NULL) {
            LOGE("Malloc for importParam failed.");
            return HAL_ERR_BAD_ALLOC;
        }
    }

    importParam[idx].tag = HKS_TAG_ALGORITHM;
    importParam[idx++].uint32Param = HKS_ALG_AES;
    importParam[idx].tag = HKS_TAG_KEY_SIZE;
    importParam[idx++].uint32Param = keyLen * BITS_PER_BYTE;
    importParam[idx].tag = HKS_TAG_PADDING;
    importParam[idx++].uint32Param = HKS_PADDING_NONE;
    importParam[idx].tag = HKS_TAG_IS_ALLOWED_WRAP;
    importParam[idx++].boolParam = false;
    importParam[idx].tag = HKS_TAG_PURPOSE;
    importParam[idx++].uint32Param = g_purposeToHksKeyPurpose[purpose];
    importParam[idx].tag = HKS_TAG_BLOCK_MODE;
    importParam[idx++].uint32Param = HKS_MODE_GCM;
    importParam[idx].tag = HKS_TAG_DIGEST;
    importParam[idx++].uint32Param = HKS_DIGEST_SHA256;

    int ret = ConstructParamSet(paramSet, importParam, idx);
    if (ret != HAL_SUCCESS) {
        LOGE("Construct decrypt param set failed, ret = %d.", ret);
    }

    HcFree(importParam);
    return ret;
}

static int32_t ImportSymmetricKey(const Uint8Buff *keyAlias, const Uint8Buff *authToken, KeyPurpose purpose,
    const ExtraInfo *exInfo)
{
    int32_t ret = CheckImportSymmetricKeyParam(keyAlias, authToken);
    if (ret != HAL_SUCCESS) {
        return ret;
    }

    struct HksBlob keyAliasBlob = { keyAlias->length, keyAlias->val };
    struct HksBlob symKeyBlob = { authToken->length, authToken->val };
    struct HksParamSet *paramSet = NULL;
    ret = ConstructImportSymmetricKeyParam(&paramSet, authToken->length, purpose, exInfo);
    if (ret != HAL_SUCCESS) {
        LOGE("construct param set failed, ret = %d", ret);
        return ret;
    }

    ret = HksImportKey(&keyAliasBlob, paramSet, &symKeyBlob);
    if (ret != HKS_SUCCESS) {
        LOGE("HksImportKey failed, ret: %d", ret);
        HksFreeParamSet(&paramSet);
        return ret;
    }

    HksFreeParamSet(&paramSet);
    return HAL_SUCCESS;
}

static const AlgLoader g_huksLoader = {
    .initAlg = InitHks,
    .sha256 = Sha256,
    .generateRandom = GenerateRandom,
    .computeHmac = ComputeHmac,
    .computeHkdf = ComputeHkdf,
    .importSymmetricKey = ImportSymmetricKey,
    .checkKeyExist = CheckKeyExist,
    .deleteKey = DeleteKey,
    .aesGcmEncrypt = AesGcmEncrypt,
    .aesGcmDecrypt = AesGcmDecrypt,
    .hashToPoint = HashToPoint,
    .agreeSharedSecretWithStorage = AgreeSharedSecretWithStorage,
    .agreeSharedSecret = AgreeSharedSecret,
    .bigNumExpMod = BigNumExpMod,
    .generateKeyPairWithStorage = GenerateKeyPairWithStorage,
    .generateKeyPair = GenerateKeyPair,
    .exportPublicKey = ExportPublicKey,
    .sign = Sign,
    .verify = Verify,
    .importPublicKey = ImportPublicKey,
    .checkDlPublicKey = CheckDlPublicKey,
    .checkEcPublicKey = NULL,
    .bigNumCompare = BigNumCompare
};

const AlgLoader *GetRealLoaderInstance()
{
    return &g_huksLoader;
}