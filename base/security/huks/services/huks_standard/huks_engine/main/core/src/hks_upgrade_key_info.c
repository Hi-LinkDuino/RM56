/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef _CUT_AUTHENTICATE_

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#ifdef HKS_SUPPORT_UPGRADE_STORAGE_DATA

#include "hks_upgrade_key_info.h"

#include "hks_common_check.h"
#include "hks_crypto_hal.h"
#include "hks_get_process_info.h"
#include "hks_keyblob.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"

#define HKS_SEALING_NONCE_SIZE 16
#define HKS_KEY_PAIR_CIPHER_ED25519 80

#define HKS_SLOT_PUBLIC_KEY_OCCUPY ((uint8_t)0x01) /* ed25519 public key */
#define HKS_SLOT_KEYPAIR_OCCUPY ((uint8_t)0x03) /* ed25519 private and public key pair */
#define HKS_SLOT_DERIVE_OCCUPY ((uint8_t)0x04) /* aes key: generate by ed25519 agreement */

#define HKS_KEY_TYPE_PUBLIC_KEY_ED25519 ((uint32_t)0xe001001d)
#define HKS_KEY_TYPE_KEY_PAIR_ED25519 ((uint32_t)0xf001001d)

struct HksKeyStoreInfo {
    uint32_t keyType;
    uint32_t keyUsage;
    uint32_t keyMode;
    uint32_t keyRole;
    uint16_t keyLen;
    uint16_t keyDomain;
    uint8_t keyPadding;
    uint8_t authIdType;
    uint8_t authIdSize;
    uint8_t rsv;
    uint8_t *authIdData;
};

struct HksNewVersionValue {
    uint32_t algType;
    uint32_t keySize;
    uint32_t purpose;
    uint32_t digest;
};

/*
 *          +------------------------------------------------------------------------------------------+
 * KeyInfo: | flags   | sealingNonce | aliasSize | keyAlias   | keyType    | keyLen     | usage        |
 *          | 1bytes  | 16bytes      | 1bytes    | max64bytes | 4bytes     | 2bytes     | 4bytes       |
 *          +------------------------------------------------------------------------------------------+
 *          | padding | keyMode      | key_role  | domain     | authIdType | authIdSize | auth_id_data |
 *          | 1bytes  | 4bytes       | 4bytes    | 2bytes     | 1bytes     | 1bytes     | max64bytes   |
 *          +------------------------------------------------------------------------------------------+
 *          | keySize | key          |
 *          | 1bytes  | max80bytes   |
 *          +------------------------+
*/
static uint32_t GetKeySlotLen(void)
{
    return (sizeof(uint8_t) + HKS_SEALING_NONCE_SIZE + sizeof(uint8_t) + HKS_MAX_KEY_ALIAS_LEN +
        (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) +
        sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) + HKS_MAX_KEY_AUTH_ID_LEN) +
        sizeof(uint8_t) + HKS_KEY_PAIR_CIPHER_ED25519);
}

static int32_t GetParamPosition(const struct HksBlob *keyInfo, uint32_t *paramPosition)
{
    /* keyInfo size has beed checked */
    uint32_t offset = sizeof(uint8_t) + HKS_SEALING_NONCE_SIZE; /* offset: flag and sealing nonce */
    uint8_t keyAliasSize = *(keyInfo->data + offset);
    if (keyAliasSize > HKS_MAX_KEY_ALIAS_LEN) {
        HKS_LOG_E("invalid keyAlias size, size = %u", keyAliasSize);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    offset += sizeof(uint8_t) + keyAliasSize; /* offset: add keyAlias size and real size */
    *paramPosition = offset;
    return HKS_SUCCESS;
}

static int32_t GetKeySizePosition(const struct HksBlob *keyInfo, uint32_t *keySizePosition)
{
    /* keyInfo size has beed checked */
    uint32_t offset = 0;
    int32_t ret = GetParamPosition(keyInfo, &offset);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* offset keyType + keyLen + keyUsage + padding + mode + role + domain + authIdType */
    offset += sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint32_t) +
        sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t);

    uint8_t authIdSize = *(keyInfo->data + offset);
    if (authIdSize > HKS_MAX_KEY_AUTH_ID_LEN) {
        HKS_LOG_E("invalid auth id size, size = %u", authIdSize);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    offset += sizeof(uint8_t) + authIdSize; /* offset: add key authId size and real size */
    *keySizePosition = offset;
    return HKS_SUCCESS;
}

static int32_t GetKeyInfo(const struct HksBlob *keyInfo, struct HksKeyStoreInfo *keyStoreInfo)
{
    /* keyInfo size has beed checked */
    uint32_t offset = 0;
    int32_t ret = GetParamPosition(keyInfo, &offset);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    uint8_t *tmp = keyInfo->data;
    keyStoreInfo->rsv = 0;
    keyStoreInfo->keyType = *((uint32_t *)(tmp + offset));
    offset += sizeof(uint32_t);

    keyStoreInfo->keyLen = *((uint16_t *)(tmp + offset));
    offset += sizeof(uint16_t);

    keyStoreInfo->keyUsage = *((uint32_t *)(tmp + offset));
    offset += sizeof(uint32_t);

    keyStoreInfo->keyPadding = *(tmp + offset);
    offset += sizeof(uint8_t);

    keyStoreInfo->keyMode = *((uint32_t *)(tmp + offset));
    offset += sizeof(uint32_t);

    keyStoreInfo->keyRole = *((uint32_t *)(tmp + offset));
    offset += sizeof(uint32_t);

    keyStoreInfo->keyDomain = *((uint16_t *)(tmp + offset));
    offset += sizeof(uint16_t);

    keyStoreInfo->authIdType = *(tmp + offset);
    offset += sizeof(uint8_t);

    keyStoreInfo->authIdSize = *(tmp + offset);
    if (keyStoreInfo->authIdSize > HKS_MAX_KEY_AUTH_ID_LEN) {
        HKS_LOG_E("invalid auth id size, size = %u", keyStoreInfo->authIdSize);
        return HKS_ERROR_INVALID_KEY_FILE;
    }
    offset += sizeof(uint8_t);

    if (keyStoreInfo->authIdSize != 0) {
        uint8_t *tmpAuthId = (uint8_t *)HksMalloc(keyStoreInfo->authIdSize);
        if (tmpAuthId == NULL) {
            HKS_LOG_E("malloc failed");
            return HKS_ERROR_MALLOC_FAIL;
        }
        (void)memcpy_s(tmpAuthId, keyStoreInfo->authIdSize, tmp + offset, keyStoreInfo->authIdSize);
        keyStoreInfo->authIdData = tmpAuthId;
    }
    return HKS_SUCCESS;
}

static int32_t GetNonce(const struct HksBlob *keyInfo, struct HksBlob *nonce)
{
    uint8_t *tmp = (uint8_t *)HksMalloc(HKS_SEALING_NONCE_SIZE);
    if (tmp == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    uint32_t offset = sizeof(uint8_t); /* offset: flags */
    /* keyInfo size has beed checked */
    if (memcpy_s(tmp, HKS_SEALING_NONCE_SIZE, keyInfo->data + offset, HKS_SEALING_NONCE_SIZE) != EOK) {
        HKS_LOG_E("memcpy nonce failed");
        HKS_FREE_PTR(tmp);
        return HKS_ERROR_BAD_STATE;
    }

    nonce->data = tmp;
    nonce->size = HKS_SEALING_NONCE_SIZE;
    return HKS_SUCCESS;
}

static int32_t BuildDecryptMeterail(const struct HksBlob *keyInfo, const struct HksBlob *nonce,
    struct HksUsageSpec *usageSpec, struct HksBlob *cipherKey)
{
    uint32_t keySizePosition = 0;
    int32_t ret = GetKeySizePosition(keyInfo, &keySizePosition);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    uint32_t keyPos = keySizePosition + sizeof(uint8_t);
    struct HksBlob aad = { keyPos, keyInfo->data }; /* aad: from flags to keySize */

    usageSpec->mode = HKS_MODE_GCM;
    usageSpec->padding = HKS_PADDING_NONE;
    usageSpec->digest = HKS_DIGEST_NONE;
    usageSpec->algType = HKS_ALG_AES;

    uint8_t encryptedKeySize = *(keyInfo->data + keySizePosition);
    if ((encryptedKeySize > HKS_KEY_PAIR_CIPHER_ED25519) || (encryptedKeySize <= HKS_AE_TAG_LEN)) {
        HKS_LOG_E("invalid key size, size = %u", encryptedKeySize);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    struct HksAeadParam *aeadParam = (struct HksAeadParam *)usageSpec->algParam;
    aeadParam->nonce = *nonce;
    aeadParam->aad = aad;
    aeadParam->payloadLen = encryptedKeySize - HKS_AE_TAG_LEN;

    aeadParam->tagDec.data = keyInfo->data + keyPos + encryptedKeySize - HKS_AE_TAG_LEN;
    aeadParam->tagDec.size = HKS_AE_TAG_LEN;

    cipherKey->size = encryptedKeySize - HKS_AE_TAG_LEN;
    cipherKey->data = keyInfo->data + keyPos;
    return HKS_SUCCESS;
}

static int32_t GetKek(const struct HksBlob *salt, struct HksBlob *kek)
{
    uint8_t mainKeyData[HKS_KEY_BLOB_MAIN_KEY_SIZE] = {0};
    struct HksBlob mainKey = { HKS_KEY_BLOB_MAIN_KEY_SIZE, mainKeyData };
    int32_t ret = HksCryptoHalGetMainKey(NULL, &mainKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get main key failed, ret = %d", ret);
        return ret;
    }

    struct HksKeyDerivationParam derParam = {
        .salt = *salt,
        .iterations = 1, /* old version only derive 1 time */
        .digestAlg = HKS_DIGEST_SHA256,
    };
    struct HksKeySpec derivationSpec = { HKS_ALG_PBKDF2, HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256), &derParam };

    kek->size = HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256);
    kek->data = (uint8_t *)HksMalloc(kek->size);
    if (kek->data == NULL) {
        HKS_LOG_E("malloc failed");
        (void)memset_s(mainKeyData, HKS_KEY_BLOB_MAIN_KEY_SIZE, 0, HKS_KEY_BLOB_MAIN_KEY_SIZE);
        return HKS_ERROR_MALLOC_FAIL;
    }

    ret = HksCryptoHalDeriveKey(&mainKey, &derivationSpec, kek);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keyblob derive key failed!");
        HKS_FREE_PTR(kek->data);
    }
    (void)memset_s(mainKeyData, HKS_KEY_BLOB_MAIN_KEY_SIZE, 0, HKS_KEY_BLOB_MAIN_KEY_SIZE);

    return ret;
}

static int32_t DecryptKey(const struct HksBlob *kek, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *cipherKey, struct HksBlob *key)
{
    key->data = HksMalloc(cipherKey->size);
    if (key->data == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }
    key->size = cipherKey->size;

    int32_t ret = HksCryptoHalDecrypt(kek, usageSpec, cipherKey, key);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("crypto hal decrypt failed, ret = %d", ret);
        HKS_FREE_PTR(key->data); /* decrypted failed, no need clear memory */
        key->size = 0;
        return HKS_ERROR_CRYPTO_ENGINE_ERROR; /* need return this error code for hichian call refresh func */
    }
    return ret;
}

static int32_t CopyToNewBlob(const struct HksBlob *key, struct HksBlob *outKey)
{
    uint8_t *keyBuffer = (uint8_t *)HksMalloc(key->size);
    if (keyBuffer == NULL) {
        HKS_LOG_E("public key to inner key format malloc keyBuffer failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memcpy_s(keyBuffer, key->size, key->data, key->size);
    outKey->data = keyBuffer;
    outKey->size = key->size;
    return HKS_SUCCESS;
}

static int32_t ConvertEd25519ToNewFormat(uint8_t flag, const struct HksBlob *pubKey, const struct HksBlob *priKey,
    struct HksBlob *keyOut)
{
    uint32_t totalSize = sizeof(struct KeyMaterial25519) + pubKey->size + priKey->size;
    uint8_t *buffer = (uint8_t *)HksMalloc(totalSize);
    if (buffer == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)buffer;
    keyMaterial->keyAlg = HKS_ALG_ED25519;
    keyMaterial->keySize = HKS_CURVE25519_KEY_SIZE_256;
    keyMaterial->pubKeySize = pubKey->size;
    keyMaterial->priKeySize = priKey->size;
    keyMaterial->reserved = 0;

    uint32_t offset = sizeof(struct KeyMaterial25519);
    if (memcpy_s(buffer + offset, pubKey->size, pubKey->data, pubKey->size) != EOK) {
        HKS_LOG_E("copy ed25519 public key failed");
        (void)memset_s(buffer, totalSize, 0, totalSize);
        HKS_FREE_PTR(buffer);
        return HKS_ERROR_BAD_STATE;
    }

    if (flag == HKS_SLOT_KEYPAIR_OCCUPY) {
        offset += pubKey->size;
        if (memcpy_s(buffer + offset, priKey->size, priKey->data, priKey->size) != EOK) {
            HKS_LOG_E("copy ed25519 private key failed");
            (void)memset_s(buffer, totalSize, 0, totalSize);
            HKS_FREE_PTR(buffer);
            return HKS_ERROR_BAD_STATE;
        }
    }

    keyOut->data = buffer;
    keyOut->size = totalSize;
    return HKS_SUCCESS;
}

static int32_t ConvertToNewFormat(const struct HksBlob *keyInfo, const struct HksBlob *rawKey, struct HksBlob *key)
{
    uint8_t flag = *(keyInfo->data);
    struct HksBlob priKey = { 0, NULL };
    struct HksBlob pubKey = { 0, NULL };
    switch (flag) {
        case HKS_SLOT_PUBLIC_KEY_OCCUPY:
            if (rawKey->size != HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256)) {
                return HKS_ERROR_INVALID_KEY_FILE;
            }

            pubKey.size = rawKey->size;
            pubKey.data = rawKey->data;

            return ConvertEd25519ToNewFormat(flag, &pubKey, &priKey, key);
        case HKS_SLOT_KEYPAIR_OCCUPY:
            if (rawKey->size != (HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256) << 1)) {
                return HKS_ERROR_INVALID_KEY_FILE;
            }

            /* old version: the first 32 bytes is private key; next 32 bytes is public key */
            priKey.size = HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256);
            priKey.data = rawKey->data;
            pubKey.size = HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256);
            pubKey.data = rawKey->data + priKey.size;

            return ConvertEd25519ToNewFormat(flag, &pubKey, &priKey, key);
        case HKS_SLOT_DERIVE_OCCUPY:
            if (rawKey->size != HKS_KEY_BYTES(HKS_AES_KEY_SIZE_256)) {
                return HKS_ERROR_INVALID_KEY_FILE;
            }

            return CopyToNewBlob(rawKey, key);
        default:
            return HKS_ERROR_INVALID_KEY_FILE;
    }
}

static int32_t GetNewFormatKey(const struct HksBlob *keyInfo, struct HksBlob *key)
{
    struct HksBlob kek = { 0, NULL };
    struct HksBlob nonce = { 0, NULL };
    struct HksBlob rawKey = { 0, NULL };
    int32_t ret = GetNonce(keyInfo, &nonce);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    do {
        ret = GetKek(&nonce, &kek);
        if (ret != HKS_SUCCESS) {
            break;
        }

        struct HksAeadParam aeadParam;
        struct HksUsageSpec usageSpec;
        (void)memset_s(&aeadParam, sizeof(aeadParam), 0, sizeof(aeadParam));
        (void)memset_s(&usageSpec, sizeof(usageSpec), 0, sizeof(usageSpec));
        usageSpec.algParam = (void *)&aeadParam;

        struct HksBlob cipherKey = { 0, NULL };
        ret = BuildDecryptMeterail(keyInfo, &nonce, &usageSpec, &cipherKey);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = DecryptKey(&kek, &usageSpec, &cipherKey, &rawKey);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = ConvertToNewFormat(keyInfo, &rawKey, key);
        if (ret != HKS_SUCCESS) {
            break;
        }
    } while (0);

    HKS_FREE_BLOB(nonce);
    if (kek.data != NULL) {
        (void)memset_s(kek.data, kek.size, 0, kek.size);
        HKS_FREE_BLOB(kek);
    }
    if (rawKey.data != NULL) {
        (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
        HKS_FREE_BLOB(rawKey);
    }
    return ret;
}

static int32_t AddKeyAuthIdParam(const struct HksKeyStoreInfo *keyStoreInfo, struct HksParamSet *paramSet)
{
    if (keyStoreInfo->authIdSize == 0) {
        return HKS_SUCCESS;
    }

    struct HksBlob keyAuthId = { keyStoreInfo->authIdSize, keyStoreInfo->authIdData };
    struct HksParam keyAuthIdParam = {
        .tag = HKS_TAG_KEY_AUTH_ID,
        .blob = keyAuthId
    };

    int32_t ret = HksAddParams(paramSet, &keyAuthIdParam, 1);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("add param keyAuthId failed!");
    }
    return ret;
}

static int32_t TranslateToNewValue(uint8_t flag, const struct HksKeyStoreInfo *keyStoreInfo,
    struct HksNewVersionValue *spec)
{
    if (((flag == HKS_SLOT_KEYPAIR_OCCUPY) && (keyStoreInfo->keyType != HKS_KEY_TYPE_KEY_PAIR_ED25519)) ||
        ((flag == HKS_SLOT_PUBLIC_KEY_OCCUPY) && (keyStoreInfo->keyType != HKS_KEY_TYPE_PUBLIC_KEY_ED25519))) {
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    spec->algType = HKS_ALG_ED25519;
    spec->keySize = HKS_CURVE25519_KEY_SIZE_256;
    spec->digest = HKS_DIGEST_SHA256; /* upgrade DERIVE_OCCUPY type key: set to default sha256 */
    if (flag == HKS_SLOT_KEYPAIR_OCCUPY) {
        spec->purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY;
    } else if (flag == HKS_SLOT_PUBLIC_KEY_OCCUPY) {
        spec->purpose = HKS_KEY_PURPOSE_VERIFY;
    } else if (flag == HKS_SLOT_DERIVE_OCCUPY) {
        spec->algType = HKS_ALG_AES;
        spec->keySize = HKS_AES_KEY_SIZE_256;
        spec->purpose = HKS_KEY_PURPOSE_DERIVE;
    } else {
        return HKS_ERROR_INVALID_KEY_FILE;
    }
    return HKS_SUCCESS;
}

static int32_t AddKeyInfoParams(uint8_t flag, const struct HksKeyStoreInfo *keyStoreInfo, struct HksParamSet *paramSet)
{
    struct HksNewVersionValue spec;
    (void)memset_s(&spec, sizeof(spec), 0, sizeof(spec));
    int32_t ret = TranslateToNewValue(flag, keyStoreInfo, &spec);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("translate to new value failed, ret = %d", ret);
        return ret;
    }

    struct HksParam params[] = {
        {
            .tag = HKS_TAG_ALGORITHM,
            .uint32Param = spec.algType
        }, {
            .tag = HKS_TAG_PURPOSE,
            .uint32Param = spec.purpose
        }, {
            .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = spec.keySize
        }, {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = spec.digest
        }, {
            .tag = HKS_TAG_KEY_ROLE,
            .uint32Param = keyStoreInfo->keyRole
        }, {
            .tag = HKS_TAG_KEY_DOMAIN,
            .uint32Param = keyStoreInfo->keyDomain
        },
    };

    ret = HksAddParams(paramSet, params, sizeof(params) / sizeof(params[0]));
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("add params failed!");
        return ret;
    }

    return AddKeyAuthIdParam(keyStoreInfo, paramSet);
}

static int32_t AddProcessNameParam(struct HksParamSet *paramSet)
{
    char *processName = NULL;
    int32_t ret = HksGetProcessName(&processName);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get process name failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }
    struct HksBlob processNameBlob = { strlen(processName), (uint8_t *)processName };
    struct HksParam processNameParam = {
        .tag = HKS_TAG_PROCESS_NAME,
        .blob = processNameBlob
    };

    ret = HksAddParams(paramSet, &processNameParam, 1);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("add param proceseName failed!");
    }
    return ret;
}

static int32_t AddParams(const struct HksBlob *keyInfo, struct HksParamSet *paramSet)
{
    int32_t ret = AddProcessNameParam(paramSet);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* add params from key storage file */
    struct HksKeyStoreInfo keyStoreInfo;
    (void)memset_s(&keyStoreInfo, sizeof(keyStoreInfo), 0, sizeof(keyStoreInfo));
    ret = GetKeyInfo(keyInfo, &keyStoreInfo);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    uint8_t flag = *(keyInfo->data);
    ret = AddKeyInfoParams(flag, &keyStoreInfo, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("add key info params failed, ret = %d", ret);
    }
    if (keyStoreInfo.authIdData != NULL) {
        HKS_FREE_PTR(keyStoreInfo.authIdData);
    }
    return ret;
}

static int32_t GetParamSet(const struct HksBlob *keyInfo, struct HksParamSet **paramSet)
{
    struct HksParamSet *outputParamSet = NULL;
    int32_t ret = HksInitParamSet(&outputParamSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksInitParamSet failed!");
        return ret;
    }

    do {
        ret = AddParams(keyInfo, outputParamSet);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("AddParams failed!");
            break;
        }

        ret = HksBuildParamSet(&outputParamSet);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("HksBuildParamSet failed!");
            break;
        }
    } while (0);

    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("ConstructParamSet failed.");
        HksFreeParamSet(&outputParamSet);
        return ret;
    }

    *paramSet = outputParamSet;
    return ret;
}

static int32_t GetKeyFlag(const struct HksBlob *keyInfo, uint8_t *keyFlag)
{
    uint8_t flag = *(keyInfo->data);
    switch (flag) {
        case HKS_SLOT_PUBLIC_KEY_OCCUPY:
            *keyFlag = HKS_KEY_FLAG_IMPORT_KEY;
            break;
        case HKS_SLOT_KEYPAIR_OCCUPY:
        case HKS_SLOT_DERIVE_OCCUPY:
            *keyFlag = HKS_KEY_FLAG_GENERATE_KEY;
            break;
        default:
            return HKS_ERROR_INVALID_KEY_FILE;
    }
    return HKS_SUCCESS;
}

int32_t HksUpgradeKeyInfo(const struct HksBlob *keyAlias, const struct HksBlob *keyInfo, struct HksBlob *keyOut)
{
    int32_t ret = HksCheckBlob3(keyAlias, keyInfo, keyOut);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    if (keyInfo->size < GetKeySlotLen()) {
        HKS_LOG_E("invalid key info size, size = %u", keyInfo->size);
        return ret;
    }

    struct HksBlob key = { 0, NULL };
    struct HksParamSet *paramSet = NULL;

    do {
        ret = GetNewFormatKey(keyInfo, &key);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = GetParamSet(keyInfo, &paramSet);
        if (ret != HKS_SUCCESS) {
            break;
        }

        uint8_t keyFlag = 0;
        ret = GetKeyFlag(keyInfo, &keyFlag);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = HksBuildKeyBlob(keyAlias, keyFlag, &key, paramSet, keyOut);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("generate key blob failed, ret = %d", ret);
            break;
        }
    } while (0);

    if (key.data != NULL) {
        (void)memset_s(key.data, key.size, 0, key.size);
        HKS_FREE_BLOB(key);
    }
    HksFreeParamSet(&paramSet);
    return ret;
}
#endif /* HKS_SUPPORT_UPGRADE_STORAGE_DATA */
#endif /* _CUT_AUTHENTICATE_ */
