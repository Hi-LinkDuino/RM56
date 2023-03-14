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

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#ifdef HKS_SUPPORT_DH_C

#include "hks_openssl_dh.h"

#include <openssl/evp.h>
#include <openssl/dh.h>

#include "hks_log.h"
#include "hks_mem.h"
#include "hks_openssl_engine.h"
#include "hks_type_inner.h"

static int32_t HksOpensslGetNid(uint32_t keySize, int *nid)
{
    switch (keySize) {
        case HKS_DH_KEY_SIZE_2048:
            *nid = NID_ffdhe2048;
            return HKS_SUCCESS;
        case HKS_DH_KEY_SIZE_3072:
            *nid = NID_ffdhe3072;
            return HKS_SUCCESS;
        case HKS_DH_KEY_SIZE_4096:
            *nid = NID_ffdhe4096;
            return HKS_SUCCESS;
        default:
            HKS_LOG_E("invalid key size, keySize = %u", keySize);
            return HKS_ERROR_INVALID_KEY_SIZE;
    }
}

static DH *InitDhStruct(const struct HksBlob *key, const bool needPrivateExponent)
{
    const struct KeyMaterialDh *keyMaterial = (struct KeyMaterialDh *)(key->data);
    if (key->size != sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize + keyMaterial->priKeySize) {
        return NULL;
    }

    int nid = 0;
    int32_t ret = HksOpensslGetNid(keyMaterial->keySize, &nid);
    if (ret != HKS_SUCCESS) {
        return NULL;
    }

    DH *dh = DH_new_by_nid(nid);
    if (dh == NULL) {
        HksLogOpensslError();
        return NULL;
    }

    uint32_t offset = sizeof(struct KeyMaterialDh);
    BIGNUM *pubKey = BN_bin2bn(key->data + offset, keyMaterial->pubKeySize, NULL);
    offset += keyMaterial->pubKeySize;
    BIGNUM *privKey = BN_bin2bn(key->data + offset, keyMaterial->priKeySize, NULL);

    if (DH_set0_key(dh, pubKey, privKey) != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        DH_free(dh);
        return NULL;
    }

    return dh;
}

#ifdef HKS_SUPPORT_DH_GENERATE_KEY
static int32_t DhSaveKeyMaterial(const DH *dh, const uint32_t keySize, struct HksBlob *key)
{
    const BIGNUM *pubKey = NULL;
    const BIGNUM *privKey = NULL;
    DH_get0_key(dh, &pubKey, &privKey);
    const uint32_t rawMaterialLen = sizeof(struct KeyMaterialDh) + (uint32_t)BN_num_bytes(pubKey)
        + (uint32_t)BN_num_bytes(privKey);
    uint8_t *rawMaterial = (uint8_t *)HksMalloc(rawMaterialLen);
    if (rawMaterial == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);

    struct KeyMaterialDh *keyMaterial = (struct KeyMaterialDh *)rawMaterial;
    keyMaterial->keyAlg = HKS_ALG_DH;
    keyMaterial->keySize = keySize;
    keyMaterial->pubKeySize = (uint32_t)BN_num_bytes(pubKey);
    keyMaterial->priKeySize = (uint32_t)BN_num_bytes(privKey);
    keyMaterial->reserved = 0;

    uint32_t offset = sizeof(struct KeyMaterialDh);
    BN_bn2bin(pubKey, rawMaterial + offset);
    offset += keyMaterial->pubKeySize;
    BN_bn2bin(privKey, rawMaterial + offset);
    offset += keyMaterial->priKeySize;

    key->size = rawMaterialLen;
    key->data = rawMaterial;

    return HKS_SUCCESS;
}

int32_t HksOpensslDhGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key)
{
    int32_t ret;
    int nid = 0;
    ret = HksOpensslGetNid(spec->keyLen, &nid);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    DH *dh = DH_new_by_nid(nid);
    if (dh == NULL) {
        HksLogOpensslError();
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    if (DH_generate_key(dh) != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        DH_free(dh);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    ret = DhSaveKeyMaterial(dh, spec->keyLen, key);

    DH_free(dh);

    return ret;
}
#endif /* HKS_SUPPORT_DH_GENERATE_KEY */

#ifdef HKS_SUPPORT_DH_GET_PUBLIC_KEY
int32_t HksOpensslGetDhPubKey(const struct HksBlob *input, struct HksBlob *output)
{
    struct KeyMaterialDh *keyMaterial = (struct KeyMaterialDh *)input->data;
    if (input->size < sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (output->size < sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (memcpy_s(output->data, output->size, input->data, sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize) !=
        EOK) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    ((struct KeyMaterialDh *)output->data)->priKeySize = 0;
    ((struct KeyMaterialDh *)output->data)->reserved = 0;
    output->size = sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize;

    return HKS_SUCCESS;
}
#endif /* HKS_SUPPORT_DH_GET_PUBLIC_KEY */

#ifdef HKS_SUPPORT_DH_AGREE_KEY
int32_t HksOpensslDhAgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey)
{
    int32_t ret;
    if (HKS_KEY_BYTES(spec->keyLen) > sharedKey->size) {
        return HKS_ERROR_INVALID_KEY_SIZE;
    }

    struct KeyMaterialDh *pubKeyMaterial = (struct KeyMaterialDh *)pubKey->data;
    BIGNUM *pub = BN_bin2bn(pubKey->data + sizeof(struct KeyMaterialDh), pubKeyMaterial->pubKeySize, NULL);
    if (pub == NULL) {
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    DH *dh = InitDhStruct(nativeKey, true);
    if (dh == NULL) {
        BN_free(pub);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    uint8_t *computeKey = HksMalloc(DH_size(dh));
    if (computeKey == NULL) {
        BN_free(pub);
        DH_free(dh);
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (DH_compute_key_padded(computeKey, pub, dh) <= 0) {
        HksLogOpensslError();
        BN_free(pub);
        DH_free(dh);
        HksFree(computeKey);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    if (HKS_KEY_BYTES(spec->keyLen) > (uint32_t)DH_size(dh)) {
        ret = HKS_ERROR_INVALID_KEY_SIZE;
    } else {
        if (memcpy_s(sharedKey->data, sharedKey->size, computeKey, HKS_KEY_BYTES(spec->keyLen)) != EOK) {
            ret = HKS_ERROR_INVALID_OPERATION;
        } else {
            sharedKey->size = (uint32_t)DH_size(dh);
            ret = HKS_SUCCESS;
        }
    }

    (void)memset_s(computeKey, DH_size(dh), 0, DH_size(dh));
    BN_free(pub);
    DH_free(dh);
    HksFree(computeKey);
    return ret;
}
#endif /* HKS_SUPPORT_DH_AGREE_KEY */

#endif /* HKS_SUPPORT_DH_C */