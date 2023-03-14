/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <assert.h>

#include "hks_test_api_performance.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_log.h"

#include "securec.h"

#define USEC_MINT 1000
#define USEC_UINT 1000000

int32_t HksGenerateKeyRun(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn,
    struct HksParamSet *paramSetOut, uint32_t performTimes)
{
    int32_t ret;
    /* if keyAlias exist, the key will be generated and refreshed */
    for (uint32_t i = 0; i < performTimes; ++i) {
        ret = HksGenerateKey(keyAlias, paramSetIn, paramSetOut);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksEncryptRun(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText, uint32_t performTimes)
{
    int32_t ret;
    uint32_t oriCipherTestSize = 0;
    if (cipherText != NULL) {
        oriCipherTestSize = cipherText->size;
    }

    for (uint32_t i = 0; i < performTimes; ++i) {
        if (cipherText != NULL) {
            (void)memset_s(cipherText->data, oriCipherTestSize, 0, oriCipherTestSize);
            cipherText->size = oriCipherTestSize;
        }

        ret = HksEncrypt(key, paramSet, plainText, cipherText);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksDecryptRun(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText, uint32_t performTimes)
{
    int32_t ret;
    uint32_t oriPlainTextSize = 0;
    if (plainText != NULL) {
        oriPlainTextSize = plainText->size;
    }

    for (uint32_t i = 0; i < performTimes; ++i) {
        if (plainText != NULL) {
            (void)memset_s(plainText->data, oriPlainTextSize, 0, oriPlainTextSize);
            plainText->size = oriPlainTextSize;
        }
        ret = HksDecrypt(key, paramSet, cipherText, plainText);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksMacRun(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac, uint32_t performTimes)
{
    int32_t ret;
    uint32_t oriMacSize = 0;
    if (mac != NULL) {
        oriMacSize = mac->size;
    }

    for (uint32_t i = 0; i < performTimes; ++i) {
        if (mac != NULL) {
            (void)memset_s(mac->data, oriMacSize, 0, oriMacSize);
            mac->size = oriMacSize;
        }
        ret = HksMac(key, paramSet, srcData, mac);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksDeriveKeyRun(const struct HksParamSet *paramSet, const struct HksBlob *masterKey,
    struct HksBlob *derivedKey, uint32_t performTimes)
{
    int32_t ret;
    uint32_t oriDerivedKeySize = 0;
    if (derivedKey != NULL) {
        oriDerivedKeySize = derivedKey->size;
    }

    for (uint32_t i = 0; i < performTimes; ++i) {
        if (derivedKey != NULL) {
            (void)memset_s(derivedKey->data, oriDerivedKeySize, 0, oriDerivedKeySize);
            derivedKey->size = oriDerivedKeySize;
        }
        ret = HksDeriveKey(paramSet, masterKey, derivedKey);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksDeleteKeyRun(const struct HksBlob *keyAlias, uint32_t performTimes)
{
    (void)performTimes;
    return HksDeleteKey(keyAlias, NULL);
}

int32_t HksKeyExistRun(const struct HksBlob *keyAlias, uint32_t performTimes)
{
    int32_t ret;
    for (uint32_t i = 0; i < performTimes; ++i) {
        ret = HksKeyExist(keyAlias, NULL);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksGenerateRandomRun(struct HksBlob *random, uint32_t performTimes)
{
    int32_t ret;
    uint32_t oriRandomSize = 0;
    if (random != NULL) {
        oriRandomSize = random->size;
    }

    for (uint32_t i = 0; i < performTimes; ++i) {
        if (random != NULL) {
            (void)memset_s(random->data, oriRandomSize, 0, oriRandomSize);
            random->size = oriRandomSize;
        }
        ret = HksGenerateRandom(NULL, random);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksAgreeKeyRun(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey, uint32_t performTimes)
{
    int32_t ret;
    uint32_t oriAgreedKeySize = 0;
    if (agreedKey != NULL) {
        oriAgreedKeySize = agreedKey->size;
    }

    for (uint32_t i = 0; i < performTimes; ++i) {
        if (agreedKey != NULL) {
            (void)memset_s(agreedKey->data, oriAgreedKeySize, 0, oriAgreedKeySize);
            agreedKey->size = oriAgreedKeySize;
        }
        ret = HksAgreeKey(paramSet, privateKey, peerPublicKey, agreedKey);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksHashRun(const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *hash, uint32_t performTimes)
{
    int32_t ret;
    uint32_t oriHashSize = 0;
    if (hash != NULL) {
        oriHashSize = hash->size;
    }

    for (uint32_t i = 0; i < performTimes; ++i) {
        if (hash != NULL) {
            (void)memset_s(hash->data, oriHashSize, 0, oriHashSize);
            hash->size = oriHashSize;
        }

        ret = HksHash(paramSet, srcData, hash);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksBnExpModRun(struct HksBlob *x, const struct HksBlob *a,
    const struct HksBlob *e, const struct HksBlob *n, uint32_t performTimes)
{
    int32_t ret;
    uint32_t oriXSize = 0;
    if (x != NULL) {
        oriXSize = x->size;
    }

    for (uint32_t i = 0; i < performTimes; ++i) {
        if (x != NULL) {
            (void)memset_s(x->data, oriXSize, 0, oriXSize);
            x->size = oriXSize;
        }
        ret = HksBnExpMod(x, a, e, n);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }
    return HKS_SUCCESS;
}
