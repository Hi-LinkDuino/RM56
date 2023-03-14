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

#include "hks_keyblob.h"

#include "hks_crypto_adapter.h"
#include "hks_crypto_hal.h"
#include "hks_file_operator.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type_inner.h"

#ifndef _CUT_AUTHENTICATE_

#define HKS_KEY_BLOB_DUMMY_KEY_VERSION 1
#define HKS_KEY_BLOB_DUMMY_OS_VERSION 1
#define HKS_KEY_BLOB_DUMMY_OS_PATCHLEVEL 1

struct HksKeyBlobInfo {
    uint8_t salt[HKS_KEY_BLOB_DERIVE_SALT_SIZE];
    uint8_t nonce[HKS_KEY_BLOB_NONCE_SIZE];
    uint8_t tag[HKS_KEY_BLOB_TAG_SIZE];
    uint32_t keySize;
};

static void CleanKey(const struct HksParamSet *paramSet)
{
    struct HksParam *keyParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_KEY, &keyParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get key param failed!");
        return;
    }
    (void)memset_s(keyParam->blob.data, keyParam->blob.size, 0, keyParam->blob.size);
}

void HksFreeKeyNode(struct HksKeyNode **keyNode)
{
    if ((keyNode == NULL) || (*keyNode == NULL) || ((*keyNode)->refCnt == 0)) {
        return;
    }

    (*keyNode)->refCnt--;
    if (((*keyNode)->status == HKS_KEYNODE_INACTIVE) && ((*keyNode)->refCnt == 0)) {
        CleanKey((*keyNode)->paramSet);
        HksFreeParamSet(&(*keyNode)->paramSet);
        HKS_FREE_PTR(*keyNode);
        *keyNode = NULL;
    }
}

#ifndef _STORAGE_LITE_
#ifdef L2_STANDARD
#define HKS_ENCRYPT_KEY_NAME "root_encrypt_key"

static int32_t WriteEncryptKey(struct HksBlob *key)
{
    int32_t ret = HksCryptoHalFillRandom(key);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("generate encrypt key failed!");
        return ret;
    }

    ret = HksFileWrite(HKS_KEY_STORE_PATH, HKS_ENCRYPT_KEY_NAME, 0, key->data, key->size);
    if (ret != 0) {
        HKS_LOG_E("write encrypt key fail, ret:%x", ret);
        return ret;
    }
    return HKS_SUCCESS;
}

static int32_t GetEncryptKey(struct HksBlob *key)
{
    uint32_t fileSize = HksFileSize(HKS_KEY_STORE_PATH, HKS_ENCRYPT_KEY_NAME);
    if (fileSize != HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256)) {
        return WriteEncryptKey(key);
    } else {
        fileSize = HksFileRead(HKS_KEY_STORE_PATH, HKS_ENCRYPT_KEY_NAME, 0, key->data, key->size);
        if (fileSize != HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256)) {
            HKS_LOG_E("read encrypt key fail\n");
            return HKS_ERROR_BAD_STATE;
        }
        return HKS_SUCCESS;
    }
}
#else
static int32_t GetEncryptKey(struct HksBlob *mainKey)
{
    return HksCryptoHalGetMainKey(NULL, mainKey);
}
#endif

static int32_t GetSalt(const struct HksParamSet *paramSet, const struct HksKeyBlobInfo *keyBlobInfo,
    struct HksBlob *salt)
{
    struct HksParam *appIdParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_PROCESS_NAME, &appIdParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get app id param failed!");
        return ret;
    }

    if (appIdParam->blob.size > HKS_MAX_PROCESS_NAME_LEN) {
        HKS_LOG_E("invalid app id size: %u", appIdParam->blob.size);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    salt->size = appIdParam->blob.size + HKS_KEY_BLOB_DERIVE_SALT_SIZE;
    salt->data = (uint8_t *)HksMalloc(salt->size);
    if (salt->data == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (memcpy_s(salt->data, salt->size, appIdParam->blob.data, appIdParam->blob.size) != EOK) {
        HKS_LOG_E("memcpy appid failed");
        HKS_FREE_PTR(salt->data);
        return HKS_ERROR_BAD_STATE;
    }

    if (memcpy_s(salt->data + appIdParam->blob.size, salt->size - appIdParam->blob.size,
        keyBlobInfo->salt, HKS_KEY_BLOB_DERIVE_SALT_SIZE) != EOK) {
        HKS_LOG_E("memcpy salt failed");
        HKS_FREE_PTR(salt->data);
        return HKS_ERROR_BAD_STATE;
    }
    return ret;
}

static int32_t GetDeriveKey(const struct HksParamSet *paramSet, const struct HksKeyBlobInfo *keyBlobInfo,
    struct HksBlob *derivedKey)
{
    struct HksBlob salt = { 0, NULL };
    int32_t ret = GetSalt(paramSet, keyBlobInfo, &salt);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    struct HksKeyDerivationParam derParam = {
        .salt = salt,
        .iterations = HKS_KEY_BLOB_DERIVE_CNT,
        .digestAlg = HKS_DIGEST_SHA256,
    };

    struct HksKeySpec derivationSpec = { HKS_ALG_PBKDF2, HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256), &derParam };

    uint8_t encryptKeyData[HKS_KEY_BLOB_MAIN_KEY_SIZE] = {0};
    struct HksBlob encryptKey = { HKS_KEY_BLOB_MAIN_KEY_SIZE, encryptKeyData };
    ret = GetEncryptKey(&encryptKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Hks get encrypt key failed! ret = 0x%X", ret);
        HKS_FREE_BLOB(salt);
        return ret;
    }

    derivedKey->size = HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256);
    derivedKey->data = (uint8_t *)HksMalloc(derivedKey->size);
    if (derivedKey->data == NULL) {
        HKS_LOG_E("malloc failed");
        HKS_FREE_BLOB(salt);
        (void)memset_s(encryptKeyData, HKS_KEY_BLOB_MAIN_KEY_SIZE, 0, HKS_KEY_BLOB_MAIN_KEY_SIZE);
        return HKS_ERROR_MALLOC_FAIL;
    }

    ret = HksCryptoHalDeriveKey(&encryptKey, &derivationSpec, derivedKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keyblob derived key failed!");
        HKS_FREE_PTR(derivedKey->data);
    }

    (void)memset_s(encryptKeyData, HKS_KEY_BLOB_MAIN_KEY_SIZE, 0, HKS_KEY_BLOB_MAIN_KEY_SIZE);
    HKS_FREE_BLOB(salt);

    return ret;
}

static int32_t BuildKeyBlobUsageSpec(const struct HksBlob *aad, const struct HksParam *keyParam,
    bool isEncrypt, struct HksUsageSpec *usageSpec)
{
    usageSpec->mode = HKS_MODE_GCM;
    usageSpec->padding = HKS_PADDING_NONE;
    usageSpec->digest = HKS_DIGEST_NONE;
    usageSpec->algType = HKS_ALG_AES;

    struct HksAeadParam *aeadParam = (struct HksAeadParam *)HksMalloc(sizeof(struct HksAeadParam));
    if (aeadParam == NULL) {
        HKS_LOG_E("aeadParam malloc failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }

    struct HksKeyBlobInfo *keyBlobInfo = (struct HksKeyBlobInfo *)keyParam->blob.data;
    aeadParam->aad = *aad;
    aeadParam->payloadLen = keyBlobInfo->keySize;
    aeadParam->nonce.data = keyBlobInfo->nonce;
    aeadParam->nonce.size = HKS_KEY_BLOB_NONCE_SIZE;
    if (isEncrypt) {
        aeadParam->tagLenEnc = HKS_AE_TAG_LEN;
    } else {
        aeadParam->tagDec.data = keyBlobInfo->tag;
        aeadParam->tagDec.size = HKS_KEY_BLOB_TAG_SIZE;
    }
    usageSpec->algParam = aeadParam;
    return HKS_SUCCESS;
}

static int32_t EncryptAndDecryptKeyBlob(const struct HksBlob *aad, struct HksParamSet *paramSet, bool isEncrypt)
{
    struct HksParam *keyParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_KEY, &keyParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher keyBlob get key param failed!");
        return ret;
    }

    if (keyParam->blob.size <= sizeof(struct HksKeyBlobInfo)) {
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    struct HksUsageSpec *usageSpec = (struct HksUsageSpec *)HksMalloc(sizeof(struct HksUsageSpec));
    if (usageSpec == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memset_s(usageSpec, sizeof(struct HksUsageSpec), 0, sizeof(struct HksUsageSpec));
    ret = BuildKeyBlobUsageSpec(aad, keyParam, isEncrypt, usageSpec);
    if (ret != HKS_SUCCESS) {
        HksFreeUsageSpec(&usageSpec);
        return ret;
    }

    struct HksKeyBlobInfo *keyBlobInfo = (struct HksKeyBlobInfo *)keyParam->blob.data;
    if ((keyParam->blob.size - sizeof(*keyBlobInfo)) != keyBlobInfo->keySize) {
        HKS_LOG_E("invalid key size in keyBlob, keySize: %u, blobSize: %u", keyBlobInfo->keySize, keyParam->blob.size);
        HksFreeUsageSpec(&usageSpec);
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    /* encrypt/decrypt will override the srcData, so encKey and decKey point to the same buffer */
    struct HksBlob srcKey = { keyBlobInfo->keySize, keyParam->blob.data + sizeof(*keyBlobInfo) };
    struct HksBlob encKey = srcKey;

    struct HksBlob derivedKey = { 0, NULL };
    ret = GetDeriveKey(paramSet, keyBlobInfo, &derivedKey);
    if (ret != HKS_SUCCESS) {
        HksFreeUsageSpec(&usageSpec);
        return ret;
    }

    if (isEncrypt) {
        struct HksBlob tag = { HKS_KEY_BLOB_TAG_SIZE, keyBlobInfo->tag };
        ret = HksCryptoHalEncrypt(&derivedKey, usageSpec, &srcKey, &encKey, &tag);
    } else {
        ret = HksCryptoHalDecrypt(&derivedKey, usageSpec, &encKey, &srcKey);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher key[0x%x] failed!", isEncrypt);
    }

    (void)memset_s(derivedKey.data, derivedKey.size, 0, derivedKey.size);
    HKS_FREE_BLOB(derivedKey);
    HksFreeUsageSpec(&usageSpec);
    return ret;
}

/*
 * [input]
 * paramSet: |-inParamSet-|-version-|-osVersion-|-patchLevel-|-struct HksKeyBlobInfo-|-srcKey-|,
 * which use |-inParamSet-|-version-|-osVersion-|-patchLevel-| as aad
 *
 * [output]
 * paramSet: |-inParamSet-|-version-|-osVersion-|-patchLevel-|-struct HksKeyBlobInfo-|-encKey-|
 */
static int32_t EncryptKeyBlob(const struct HksBlob *aad, struct HksParamSet *paramSet)
{
    return EncryptAndDecryptKeyBlob(aad, paramSet, true);
}

/*
 * [input]
 * paramSet: |-inParamSet-|-version-|-osVersion-|-patchLevel-|-struct HksKeyBlobInfo-|-encKey-|,
 * which use |-inParamSet-|-version-|-osVersion-|-patchLevel-| as aad
 *
 * [output]
 * paramSet: |-inParamSet-|-version-|-osVersion-|-patchLevel-|-struct HksKeyBlobInfo-|-srcKey-|
 */
static int32_t DecryptKeyBlob(const struct HksBlob *aad, struct HksParamSet *paramSet)
{
    return EncryptAndDecryptKeyBlob(aad, paramSet, false);
}

static int32_t InitKeyBlobInfo(const struct HksBlob *key, struct HksBlob *keyInfo)
{
    keyInfo->size = key->size + sizeof(struct HksKeyBlobInfo);
    keyInfo->data = (uint8_t *)HksMalloc(keyInfo->size);
    if (keyInfo->data == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    int32_t ret;
    do {
        struct HksKeyBlobInfo *keyBlobInfo = (struct HksKeyBlobInfo *)keyInfo->data;
        keyBlobInfo->keySize = key->size;

        struct HksBlob salt = { HKS_KEY_BLOB_DERIVE_SALT_SIZE, keyBlobInfo->salt };
        ret = HksCryptoHalFillRandom(&salt);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get salt randomly failed, ret = %d", ret);
            break;
        }

        struct HksBlob nonce = { HKS_KEY_BLOB_NONCE_SIZE, keyBlobInfo->nonce };
        ret = HksCryptoHalFillRandom(&nonce);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get nonce randomly failed, ret = %d", ret);
            break;
        }

        if (memcpy_s(keyInfo->data + sizeof(*keyBlobInfo), keyInfo->size - sizeof(*keyBlobInfo),
            key->data, key->size) != EOK) {
            ret = HKS_ERROR_BAD_STATE;
            HKS_LOG_E("memcpy failed");
        }
    } while (0);

    if (ret != HKS_SUCCESS) {
        HKS_FREE_PTR(keyInfo->data);
    }
    return ret;
}

static int32_t AddCoreServiceParams(const struct HksBlob *keyInfo, enum HksKeyFlag keyFlag,
    struct HksParamSet *paramSet)
{
    struct HksParam tmpParam[] = {
        {
            .tag = HKS_TAG_KEY_VERSION,
            .uint32Param = HKS_KEY_BLOB_DUMMY_KEY_VERSION
        }, {
            .tag = HKS_TAG_OS_VERSION,
            .uint32Param = HKS_KEY_BLOB_DUMMY_OS_VERSION
        }, {
            .tag = HKS_TAG_OS_PATCHLEVEL,
            .uint32Param = HKS_KEY_BLOB_DUMMY_OS_PATCHLEVEL
        }, {
            .tag = HKS_TAG_KEY_FLAG,
            .uint32Param = keyFlag
        }, {
            .tag = HKS_TAG_KEY,
            .blob = *keyInfo
        },
    };

    int32_t ret = HksAddParams(paramSet, tmpParam, sizeof(tmpParam) / sizeof(tmpParam[0]));
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("add sys params failed");
    }
    return ret;
}

static int32_t BuildClearKeyBlob(const struct HksBlob *key, enum HksKeyFlag keyFlag,
    const struct HksParamSet *inParamSet, struct HksParamSet **outParamSet)
{
    struct HksParamSet *newParamSet = NULL;
    struct HksBlob tmpKey = { 0, NULL };

    int32_t ret = HksInitParamSet(&newParamSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("init param set failed");
        return ret;
    }

    do {
        ret = HksAddParams(newParamSet, inParamSet->params, inParamSet->paramsCnt);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("add in params failed");
            break;
        }

        ret = InitKeyBlobInfo(key, &tmpKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("InitKeyBlobInfo failed");
            break;
        }

        ret = AddCoreServiceParams(&tmpKey, keyFlag, newParamSet);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("add Params failed");
            break;
        }

        /* need not clean key here */
        ret = HksBuildParamSet(&newParamSet);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("build paramset failed!");
        }
    } while (0);

    if (tmpKey.data != NULL) {
        (void)memset_s(tmpKey.data, tmpKey.size, 0, tmpKey.size);
        HKS_FREE_PTR(tmpKey.data);
    }
    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(&newParamSet);
        return ret;
    }

    *outParamSet = newParamSet;
    return HKS_SUCCESS;
}

static int32_t GetAadAndParamSet(const struct HksBlob *inData, struct HksBlob *aad, struct HksParamSet **paramSet)
{
    uint8_t *keyBlob = (uint8_t *)HksMalloc(inData->size);
    if (keyBlob == NULL) {
        HKS_LOG_E("malloc keyBlob failed");
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memcpy_s(keyBlob, inData->size, inData->data, inData->size);

    struct HksParamSet *keyBlobParamSet = NULL;
    int32_t ret = HksGetParamSet((const struct HksParamSet *)keyBlob, inData->size, &keyBlobParamSet);
    if (ret != HKS_SUCCESS) {
        HKS_FREE_PTR(keyBlob);
        HKS_LOG_E("get keyBlobParamSet failed");
        return ret;
    }

    struct HksParam *keyParam = NULL;
    ret = HksGetParam(keyBlobParamSet, HKS_TAG_KEY, &keyParam);
    if (ret != HKS_SUCCESS) {
        HKS_FREE_PTR(keyBlob);
        HksFreeParamSet(&keyBlobParamSet);
        HKS_LOG_E("aad get key param failed!");
        return ret;
    }

    if (keyParam->blob.data + keyParam->blob.size != (uint8_t *)keyBlobParamSet + keyBlobParamSet->paramSetSize) {
        HKS_FREE_PTR(keyBlob);
        HksFreeParamSet(&keyBlobParamSet);
        HKS_LOG_E("invalid keyblob, keyParam should be the last param!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    *paramSet = keyBlobParamSet;
    /* the aad is the whole keyBlob content without the keyParam blob part */
    aad->data = keyBlob;
    aad->size = keyBlobParamSet->paramSetSize - keyParam->blob.size;
    return HKS_SUCCESS;
}

struct HksKeyNode *HksGenerateKeyNode(const struct HksBlob *key)
{
    if (key->size > MAX_KEY_SIZE) {
        HKS_LOG_E("invalid key blob size %x", key->size);
        return NULL;
    }

    struct HksBlob aad = { 0, NULL };
    struct HksParamSet *keyBlobParamSet = NULL;
    int32_t ret = GetAadAndParamSet(key, &aad, &keyBlobParamSet);
    if (ret != HKS_SUCCESS) {
        return NULL;
    }

    ret = DecryptKeyBlob(&aad, keyBlobParamSet);
    HKS_FREE_BLOB(aad);
    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(&keyBlobParamSet);
        HKS_LOG_E("decrypt keyBlob failed");
        return NULL;
    }

    struct HksKeyNode *keyNode = (struct HksKeyNode *)HksMalloc(sizeof(struct HksKeyNode));
    if (keyNode == NULL) {
        CleanKey(keyBlobParamSet);
        HksFreeParamSet(&keyBlobParamSet);
        HKS_LOG_E("malloc keynode failed");
        return NULL;
    }

    keyNode->refCnt = 1;
    keyNode->status = HKS_KEYNODE_INACTIVE;
    keyNode->handle = 0;
    keyNode->paramSet = keyBlobParamSet;
    return keyNode;
}

int32_t HksGetRawKey(const struct HksParamSet *paramSet, struct HksBlob *rawKey)
{
    struct HksParam *keyParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_KEY, &keyParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get key param failed!");
        return ret;
    }

    if (keyParam->blob.size <= sizeof(struct HksKeyBlobInfo)) {
        HKS_LOG_E("invalid key size in keyBlob!");
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    struct HksKeyBlobInfo *keyBlobInfo = (struct HksKeyBlobInfo *)keyParam->blob.data;
    if ((keyParam->blob.size - sizeof(*keyBlobInfo)) != keyBlobInfo->keySize) {
        HKS_LOG_E("invalid key size in keyBlob, keySize: %u, blobSize: %u", keyBlobInfo->keySize, keyParam->blob.size);
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    uint8_t *data = HksMalloc(keyBlobInfo->keySize);
    if (data == NULL) {
        HKS_LOG_E("fail to malloc raw key");
        return HKS_ERROR_BAD_STATE;
    }

    (void)memcpy_s(data, keyBlobInfo->keySize, keyParam->blob.data + sizeof(*keyBlobInfo), keyBlobInfo->keySize);
    rawKey->size = keyBlobInfo->keySize;
    rawKey->data = data;
    return HKS_SUCCESS;
}

int32_t HksBuildKeyBlob(const struct HksBlob *keyAlias, uint8_t keyFlag, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    (void)keyAlias;
    struct HksParamSet *keyBlobParamSet = NULL;
    int32_t ret = BuildClearKeyBlob(key, keyFlag, paramSet, &keyBlobParamSet);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    struct HksParam *keyParam = NULL;
    ret = HksGetParam(keyBlobParamSet, HKS_TAG_KEY, &keyParam);
    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(&keyBlobParamSet);
        HKS_LOG_E("get key param when building keyBlob failed!");
        return ret;
    }

    /* the aad is the whole keyBlob content without the keyParam blob part */
    struct HksBlob aad = { keyBlobParamSet->paramSetSize - keyParam->blob.size, (uint8_t *)keyBlobParamSet };
    ret = EncryptKeyBlob(&aad, keyBlobParamSet);
    if (ret != HKS_SUCCESS) {
        /* should clean the clear key if fail to encrypt key */
        (void)memset_s(keyParam->blob.data, keyParam->blob.size, 0, keyParam->blob.size);
        HksFreeParamSet(&keyBlobParamSet);
        return ret;
    }

    if (memcpy_s(keyOut->data, keyOut->size, keyBlobParamSet, keyBlobParamSet->paramSetSize) != EOK) {
        HksFreeParamSet(&keyBlobParamSet);
        HKS_LOG_E("copy keyblob out failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }

    keyOut->size = keyBlobParamSet->paramSetSize;
    HksFreeParamSet(&keyBlobParamSet);
    return HKS_SUCCESS;
}

int32_t HksGetAadAndParamSet(const struct HksBlob *inData, struct HksBlob *aad, struct HksParamSet **paramSet)
{
    return GetAadAndParamSet(inData, aad, paramSet);
}

int32_t HksDecryptKeyBlob(const struct HksBlob *aad, struct HksParamSet *paramSet)
{
    return DecryptKeyBlob(aad, paramSet);
}
#endif

#endif /* _CUT_AUTHENTICATE_ */
