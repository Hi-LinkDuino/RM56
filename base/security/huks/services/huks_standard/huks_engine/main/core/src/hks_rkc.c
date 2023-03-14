/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "hks_rkc.h"

#include "hks_crypto_hal.h"
#include "hks_get_udid.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_rkc_rw.h"

#define HKS_RKC_RMK_LEN 64                              /* the length of root main key */
#define HKS_RKC_RMK_EK_LEN 32                           /* the encryption key length of root main key */
#define HKS_RKC_RAW_KEY_LEN 64                          /* the raw key length of root key component */
#define HKS_HARDWARE_UDID_LEN 32                        /* the length of hardware UDID */
#define HKS_RKC_MK_ADD_DATA_LEN 8                       /* the additional data length of main key */
#define HKS_RKC_KSF_NAME_LEN_MAX 256                    /* the max length of keystore filename */

/* the initialization parameter of root key component */
struct HksRkcInitParam {
    uint16_t version;                                   /* the version of root key component */
    uint8_t storageType;                                /* the storage type of root key component */
    struct HksRkcKsfAttr ksfAttr;                       /* the attribute of keystore file */
    uint32_t rmkIter;                                   /* the iterator number of times which derive Root Main Key */
    uint32_t rmkHashAlg;                                /* the hash algorithm which derive Root Main Key */
    uint32_t mkEncryptAlg;                              /* the encrption algorithm of main key */
};

/* main key */
struct HksRkcMk {
    bool valid;                                         /* whether main key is valid */
    struct HksTime mkCreatedTime;                       /* the created time of main key */
    struct HksTime mkExpiredTime;                       /* the expired time of main key */
    uint8_t mkWithMask[HKS_RKC_MK_LEN];                 /* the main key with mask */
};

/* the default initialized parameter of root key component */
const struct HksRkcInitParam g_hksRkcDefaultInitParam = {
    .version = HKS_RKC_VER,
    .storageType = HKS_RKC_STORAGE_FILE_SYS,
    .ksfAttr = { HKS_RKC_KSF_NUM, { "info1.data", "info2.data" } },
    .rmkIter = HKS_RKC_RMK_ITER,
    .rmkHashAlg = HKS_RKC_RMK_HMAC_SHA256,
    .mkEncryptAlg = HKS_RKC_MK_CRYPT_ALG_AES256_GCM,
};

/* the data of main key */
struct HksRkcMk g_hksRkcMk = { false, { 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 }, {0} };

/* the additional data of main key. 'H', 'K', 'S', 'R', 'K', 'C', 'M', 'K' */
const uint8_t g_hksRkcMkAddData[HKS_RKC_MK_ADD_DATA_LEN] = { 0x48, 0x4B, 0x53, 0x52, 0x4B, 0x43, 0x4D, 0x4B };

static int32_t RkcReadAllKsf(int32_t *allKsfRet, struct HksRkcKsfData *allKsfData, uint32_t ksfCount,
    struct HksRkcKsfData **validKsfData, uint32_t *validKsfIndex)
{
    if (ksfCount > g_hksRkcCfg.ksfAttr.num) {
        HKS_LOG_E("Invalid ksf count!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    /* Read all ksf */
    bool someCaseSuccess = false;
    for (uint32_t i = 0; i < g_hksRkcCfg.ksfAttr.num; ++i) {
        allKsfRet[i] = HksRkcReadKsf(g_hksRkcCfg.ksfAttr.name[i], &(allKsfData[i]));
        if (allKsfRet[i] != HKS_SUCCESS) {
            continue;
        }

        /* the first valid ksf is found, save data and index */
        if (*validKsfData == NULL) {
            *validKsfData = &(allKsfData[i]);
            *validKsfIndex = i;
            someCaseSuccess = true;
        }
    }

    return (someCaseSuccess ? HKS_SUCCESS : HKS_ERROR_INVALID_KEY_FILE);
}

static int32_t RkcRecoverRkTime(const struct HksRkcKsfData *ksfData)
{
    if (memcpy_s(&(g_hksRkcCfg.rkCreatedTime), sizeof(g_hksRkcCfg.rkCreatedTime),
        &(ksfData->rkCreatedTime), sizeof(ksfData->rkCreatedTime)) != EOK) {
        HKS_LOG_E("Memcpy rkCreatedTime failed!");
        return HKS_ERROR_BAD_STATE;
    }

    if (memcpy_s(&(g_hksRkcCfg.rkExpiredTime), sizeof(g_hksRkcCfg.rkExpiredTime),
        &(ksfData->rkExpiredTime), sizeof(ksfData->rkExpiredTime)) != EOK) {
        HKS_LOG_E("Memcpy rkExpiredTime failed!");
        return HKS_ERROR_BAD_STATE;
    }

    return HKS_SUCCESS;
}

static int32_t RkcGetFixedMaterial(struct HksBlob *material)
{
    /* consistent with the old hks */
    const uint8_t fixedMaterial[HKS_RKC_MATERIAL_LEN] = {
        0xB2, 0xA1, 0x0C, 0x73, 0x52, 0x73, 0x76, 0xA1,
        0x60, 0x62, 0x2E, 0x08, 0x52, 0x08, 0x2E, 0xA9,
        0x60, 0xBC, 0x2E, 0x73, 0x52, 0x0B, 0x0C, 0xBC,
        0xEE, 0x0A, 0x2E, 0x08, 0x52, 0x9C, 0x76, 0xA9
    };

    if (memcpy_s(material->data, material->size, fixedMaterial, HKS_RKC_MATERIAL_LEN) != EOK) {
        HKS_LOG_E("Memcpy fiexd material failed!");
        return HKS_ERROR_BAD_STATE;
    }

    return HKS_SUCCESS;
}

static int32_t RkcGetRmkRawKey(const struct HksRkcKsfData *ksfData, struct HksBlob *rawKey)
{
    uint8_t material3Data[HKS_RKC_MATERIAL_LEN] = {0};
    struct HksBlob material3 = { HKS_RKC_MATERIAL_LEN, material3Data };

    /* Get the fixed material */
    int32_t ret = RkcGetFixedMaterial(&material3);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Get fixed material failed! ret = 0x%X", ret);
        return ret;
    }

    /* materials xor */
    for (uint32_t i = 0; i < HKS_RKC_MATERIAL_LEN; ++i) {
        rawKey->data[i] = ksfData->rkMaterial1[i] ^ ksfData->rkMaterial2[i] ^ material3.data[i];
    }

    /* append hardware UDID */
    ret = HksGetHardwareUdid(rawKey->data + HKS_RKC_MATERIAL_LEN, HKS_HARDWARE_UDID_LEN);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Get hardware udid failed! ret = 0x%X", ret);
        return ret;
    }

    return HKS_SUCCESS;
}

static uint32_t RkcDigestToHks(const uint32_t rkcDigest)
{
    if (rkcDigest == HKS_RKC_RMK_HMAC_SHA256) {
        return HKS_DIGEST_SHA256;
    }

    /* if digest is invalid, will use default digest */
    return HKS_DIGEST_SHA256;
}

static int32_t RkcPbkdf2Hmac(const uint32_t hashAlg, const struct HksBlob *rawKey,
    const struct HksBlob *salt, const uint32_t iterNum, struct HksBlob *dk)
{
    struct HksKeyDerivationParam derParam = {
        .salt = *salt,
        .iterations = iterNum,
        .digestAlg = RkcDigestToHks(hashAlg),
    };
    const struct HksKeySpec derivationSpec = { HKS_ALG_PBKDF2, dk->size, &derParam };
    int32_t ret = HksCryptoHalDeriveKey(rawKey, &derivationSpec, dk);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Crypto hal derive key failed! ret = 0x%X", ret);
    }

    return ret;
}

static int32_t RkcDeriveRmk(const struct HksRkcKsfData *ksfData, struct HksBlob *rmk)
{
    struct HksBlob rawKey;
    rawKey.data = (uint8_t *)HksMalloc(HKS_RKC_RAW_KEY_LEN);
    if (rawKey.data == NULL) {
        HKS_LOG_E("Malloc rawKey failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    rawKey.size = HKS_RKC_RAW_KEY_LEN;
    (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);

    int32_t ret;
    do {
        /* get the raw key */
        ret = RkcGetRmkRawKey(ksfData, &rawKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Get rmk raw key failed! ret = 0x%X", ret);
            break;
        }

        /* PBKDF2-HMAC */
        const struct HksBlob salt = { HKS_RKC_SALT_LEN, (uint8_t *)(ksfData->rmkSalt) };
        ret = RkcPbkdf2Hmac(ksfData->rmkHashAlg, &rawKey, &salt, ksfData->rmkIter, rmk);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Pbkdf2 failed! ret = 0x%X", ret);
        }
    } while (0);

    (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
    HKS_FREE_BLOB(rawKey);
    return ret;
}

static int32_t InitMkCryptUsageSpec(uint8_t *iv, const uint32_t ivSize, struct HksUsageSpec *usageSpec)
{
    usageSpec->mode = HKS_MODE_GCM;
    usageSpec->padding = HKS_PADDING_NONE;
    usageSpec->digest = HKS_DIGEST_NONE;
    usageSpec->algType = HKS_ALG_AES;

    struct HksAeadParam *aeadParam = (struct HksAeadParam *)usageSpec->algParam;
    aeadParam->aad.size = HKS_RKC_MK_ADD_DATA_LEN;
    aeadParam->aad.data = (uint8_t *)&g_hksRkcMkAddData;
    aeadParam->nonce.size = ivSize;
    aeadParam->nonce.data = iv;
    aeadParam->payloadLen = HKS_RKC_RMK_EK_LEN;

    return HKS_SUCCESS;
}

static int32_t RkcMkCrypt(const struct HksRkcKsfData *ksfData,
    struct HksBlob *plainText, struct HksBlob *cipherText, const bool encrypt)
{
    struct HksBlob rmk;
    rmk.data = (uint8_t *)HksMalloc(HKS_RKC_RMK_LEN);
    if (rmk.data == NULL) {
        HKS_LOG_E("Malloc rmk failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    rmk.size = HKS_RKC_RMK_LEN;
    (void)memset_s(rmk.data, rmk.size, 0, rmk.size);

    int32_t ret;
    do {
        ret = RkcDeriveRmk(ksfData, &rmk);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Derive rmk failed! ret = 0x%X", ret);
            break;
        }

        struct HksAeadParam aeadParam = {0};
        struct HksUsageSpec usageSpec = { .algParam = (void *)(&aeadParam) };
        ret = InitMkCryptUsageSpec((uint8_t *)ksfData->mkIv, HKS_RKC_MK_IV_LEN, &usageSpec);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Init mk crypt usageSpec failed! ret = 0x%X", ret);
            break;
        }

        const struct HksBlob key = { HKS_RKC_RMK_EK_LEN, rmk.data };
        if (encrypt) {
            aeadParam.tagLenEnc = HKS_AE_TAG_LEN;
            struct HksBlob tag = { HKS_AE_TAG_LEN, cipherText->data + key.size };
            ret = HksCryptoHalEncrypt(&key, &usageSpec, plainText, cipherText, &tag);
        } else {
            aeadParam.tagDec.size = HKS_AE_TAG_LEN;
            aeadParam.tagDec.data = cipherText->data + cipherText->size - HKS_AE_TAG_LEN;
            cipherText->size -= HKS_AE_TAG_LEN; /* the decrypt len should remove the tag len */
            ret = HksCryptoHalDecrypt(&key, &usageSpec, cipherText, plainText);
        }
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Crypto mk failed! ret = 0x%X", ret);
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR; /* need return this error code for hichian call refresh func */
        }
    } while (0);

    /* the data of root key should be cleared after use */
    (void)memset_s(rmk.data, rmk.size, 0, rmk.size);
    HKS_FREE_BLOB(rmk);
    return ret;
}

static void RkcMaskMk(const struct HksBlob *mk)
{
    for (uint32_t i = 0; i < HKS_RKC_MK_LEN; ++i) {
        g_hksRkcMk.mkWithMask[i] = mk->data[i] ^ g_hksRkcCfg.mkMask[i];
    }

    g_hksRkcMk.valid = true;
}

static int32_t RkcRecoverMkTime(const struct HksRkcKsfData *ksfData)
{
    if (memcpy_s(&(g_hksRkcMk.mkCreatedTime), sizeof(g_hksRkcMk.mkCreatedTime),
        &(ksfData->mkCreatedTime), sizeof(ksfData->mkCreatedTime)) != EOK) {
        HKS_LOG_E("Memcpy mkCreatedTime failed!");
        return HKS_ERROR_BAD_STATE;
    }

    if (memcpy_s(&(g_hksRkcMk.mkExpiredTime), sizeof(g_hksRkcMk.mkExpiredTime),
        &(ksfData->mkExpiredTime), sizeof(ksfData->mkExpiredTime)) != EOK) {
        HKS_LOG_E("Memcpy mkExpiredTime failed!");
        return HKS_ERROR_BAD_STATE;
    }

    struct HksBlob mk;
    mk.data = (uint8_t *)HksMalloc(HKS_RKC_MK_LEN);
    if (mk.data == NULL) {
        HKS_LOG_E("Malloc mk failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    mk.size = HKS_RKC_MK_LEN;

    int32_t ret;
    do {
        struct HksBlob mkMaskBlob = { HKS_RKC_MK_LEN, g_hksRkcCfg.mkMask };
        ret = HksCryptoHalFillRandom(&mkMaskBlob);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Generate mk failed! ret = 0x%X", ret);
            break;
        }

        struct HksBlob mkCipherText = { HKS_RKC_MK_CIPHER_TEXT_LEN, (uint8_t *)ksfData->mkCiphertext };
        ret = RkcMkCrypt(ksfData, &mk, &mkCipherText, false); /* false: decrypt */
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Main key crypt failed! ret = 0x%X", ret);
            break;
        }

        /* the main key in memory should be masked */
        (void)RkcMaskMk(&mk);
    } while (0);

    (void)memset_s(mk.data, mk.size, 0, mk.size);
    HKS_FREE_BLOB(mk);
    return ret;
}

static int32_t RkcCheckKsf(const char *ksfName, int32_t ret,
    const struct HksRkcKsfData *ksfData, const struct HksRkcKsfData *validKsfData)
{
    /* If this ksf is different from the first valid ksf, try to overwrite it by the first valid ksf. */
    if ((ret != HKS_SUCCESS) || (HksMemCmp(validKsfData, ksfData, sizeof(struct HksRkcKsfData)) != 0)) {
        HKS_LOG_E("Repair ksf[%s]", ksfName);
        return HksRkcWriteKsf(ksfName, validKsfData);
    }

    return HKS_SUCCESS;
}

static int32_t RkcCheckAllKsf(const int32_t *allKsfRet, const struct HksRkcKsfData *allKsfData,
    uint32_t ksfCount, const struct HksRkcKsfData *validKsfData, const uint32_t validKsfIndex)
{
    if (ksfCount > g_hksRkcCfg.ksfAttr.num) {
        HKS_LOG_E("Invalid ksf count!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    for (uint32_t i = 0; i < g_hksRkcCfg.ksfAttr.num; ++i) {
        if (i == validKsfIndex) {
            continue;
        }

        /* if fail, continue */
        int32_t ret = RkcCheckKsf(g_hksRkcCfg.ksfAttr.name[i], allKsfRet[i], allKsfData + i, validKsfData);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Check 0x%X ksf failed! ret = 0x%X", i, ret);
        }
    }

    return HKS_SUCCESS;
}

static int32_t RkcLoadKsf(void)
{
    const uint32_t allKsfDataSize = sizeof(struct HksRkcKsfData) * HKS_RKC_KSF_NUM;
    struct HksRkcKsfData *allKsfData = (struct HksRkcKsfData *)HksMalloc(allKsfDataSize);
    if (allKsfData == NULL) {
        HKS_LOG_E("Malloc all ksf data failed! malloc size = 0x%X", allKsfDataSize);
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(allKsfData, allKsfDataSize, 0, allKsfDataSize);

    int32_t ret;
    do {
        int32_t allKsfRet[HKS_RKC_KSF_NUM] = {0};
        uint32_t validKsfIndex = 0;
        struct HksRkcKsfData *validKsfData = NULL;

        ret = RkcReadAllKsf(allKsfRet, allKsfData, HKS_RKC_KSF_NUM, &validKsfData, &validKsfIndex);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("All ksf file are invalid! ret = 0x%X", ret);
            break;
        }

        ret = RkcRecoverRkTime(validKsfData);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Recover rook key memory failed! ret = 0x%X", ret);
            break;
        }

        ret = RkcRecoverMkTime(validKsfData);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Recover main key memory failed! ret = 0x%X", ret);
            break;
        }

        ret = RkcCheckAllKsf(allKsfRet, allKsfData, HKS_RKC_KSF_NUM, validKsfData, validKsfIndex);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Check all ksf failed! ret = 0x%X", ret);
        }
    } while (0);

    (void)memset_s(allKsfData, allKsfDataSize, 0, allKsfDataSize);
    HKS_FREE_PTR(allKsfData);
    return ret;
}

static int32_t RkcCalculateMaterial(const struct HksBlob *random1, const struct HksBlob *random2,
    struct HksRkcKsfData *ksfData)
{
    /*
     *  calculate material
     *  material1: SHA256(0x1 + R1 + R2 + E)
     *  material2: SHA256(0x2 + R1 + R2 + E)
     */
    struct HksBlob hashSrc;
    hashSrc.data = (uint8_t *)HksMalloc(1 + HKS_RKC_MATERIAL_LEN + HKS_RKC_MATERIAL_LEN);
    if (hashSrc.data == NULL) {
        HKS_LOG_E("Malloc hash src data failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    hashSrc.size = 1 + HKS_RKC_MATERIAL_LEN + HKS_RKC_MATERIAL_LEN;

    struct HksBlob hashResult1 = { HKS_RKC_MATERIAL_LEN, ksfData->rkMaterial1 };
    struct HksBlob hashResult2 = { HKS_RKC_MATERIAL_LEN, ksfData->rkMaterial2 };

    int32_t ret;
    do {
        /* 1: sn is 0x01 or 0x02 */
        if (memcpy_s(hashSrc.data + 1, HKS_RKC_MATERIAL_LEN, random1->data, random1->size) != EOK) {
            HKS_LOG_E("Memcpy random1 failed!");
            ret = HKS_ERROR_BAD_STATE;
            break;
        }

        if (memcpy_s(hashSrc.data + 1 + HKS_RKC_MATERIAL_LEN, HKS_RKC_MATERIAL_LEN,
            random2->data, random2->size) != EOK) {
            HKS_LOG_E("Memcpy random2 failed!");
            ret = HKS_ERROR_BAD_STATE;
            break;
        }

        hashSrc.data[0] = 0x01;
        ret = HksCryptoHalHash(HKS_DIGEST_SHA256, &hashSrc, &hashResult1);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Hash rkMaterial1 failed!");
            break;
        }

        hashSrc.data[0] = 0x02;
        ret = HksCryptoHalHash(HKS_DIGEST_SHA256, &hashSrc, &hashResult2);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Hash rkMaterial2 failed!");
        }
    } while (0);

    (void)memset_s(hashSrc.data, hashSrc.size, 0, hashSrc.size);
    HKS_FREE_BLOB(hashSrc);
    return ret;
}

static int32_t RkcMakeRandomMaterial(struct HksRkcKsfData *ksfData)
{
    /* two random number */
    uint8_t random1Data[HKS_RKC_MATERIAL_LEN] = {0};
    uint8_t random2Data[HKS_RKC_MATERIAL_LEN] = {0};
    struct HksBlob random1 = { HKS_RKC_MATERIAL_LEN, random1Data };
    struct HksBlob random2 = { HKS_RKC_MATERIAL_LEN, random2Data };

    int32_t ret;
    do {
        /* Generate 32 * 2 random number: R1 + R2 */
        ret = HksCryptoHalFillRandom(&random1);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Generate random1 failed! ret = 0x%X", ret);
            break;
        }

        ret = HksCryptoHalFillRandom(&random2);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Generate random2 failed! ret = 0x%X", ret);
            break;
        }

        /* calculate material */
        ret = RkcCalculateMaterial(&random1, &random2, ksfData);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Calc material failed! ret = 0x%X", ret);
        }
    } while (0);

    (void)memset_s(random1Data, HKS_RKC_MATERIAL_LEN, 0, HKS_RKC_MATERIAL_LEN);
    (void)memset_s(random2Data, HKS_RKC_MATERIAL_LEN, 0, HKS_RKC_MATERIAL_LEN);
    return ret;
}

static int32_t RkcMakeMk(struct HksRkcKsfData *ksfData)
{
    struct HksBlob mk;
    mk.data = (uint8_t *)HksMalloc(HKS_RKC_MK_LEN);
    if (mk.data == NULL) {
        HKS_LOG_E("Malloc mk failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    mk.size = HKS_RKC_MK_LEN;

    int32_t ret;
    do {
        ksfData->mkEncryptAlg = g_hksRkcCfg.mkEncryptAlg;

        /* generate the mask of main key */
        struct HksBlob mkMaskBlob = { HKS_RKC_MK_LEN, g_hksRkcCfg.mkMask };
        ret = HksCryptoHalFillRandom(&mkMaskBlob);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Generate mkMask failed! ret = 0x%X", ret);
            break;
        }

        /* generate main key */
        ret = HksCryptoHalFillRandom(&mk);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Generate mk failed! ret = 0x%X", ret);
            break;
        }

        /* generate the IV of main key */
        struct HksBlob mkIvBlob = { HKS_RKC_MK_IV_LEN, ksfData->mkIv };
        ret = HksCryptoHalFillRandom(&mkIvBlob);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Generate mkIv failed! ret = 0x%X", ret);
            break;
        }

        struct HksBlob cipherTextBlob = { HKS_RKC_MK_CIPHER_TEXT_LEN, ksfData->mkCiphertext };
        ret = RkcMkCrypt(ksfData, &mk, &cipherTextBlob, true); /* true: encrypt */
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Encrypt mk failed! ret = 0x%X", ret);
            break;
        }

        /* the main key in memory should be masked */
        (void)RkcMaskMk(&mk);
    } while (0);

    (void)memset_s(mk.data, mk.size, 0, mk.size);
    HKS_FREE_BLOB(mk);
    return ret;
}

static int32_t RkcWriteAllKsf(const struct HksRkcKsfData *ksfData)
{
    bool isSuccess = false;
    for (uint32_t i = 0; i < g_hksRkcCfg.ksfAttr.num; ++i) {
        int32_t ret = HksRkcWriteKsf(g_hksRkcCfg.ksfAttr.name[i], ksfData);
        if (ret == HKS_SUCCESS) {
            isSuccess = true;
        }
    }

    /* If all keystore file were written fail, return error code, otherwise, return success code. */
    return (isSuccess ? HKS_SUCCESS : HKS_ERROR_WRITE_FILE_FAIL);
}

static int32_t RkcCreateKsf(void)
{
    struct HksRkcKsfData *newKsfData = (struct HksRkcKsfData *)HksMalloc(sizeof(struct HksRkcKsfData));
    if (newKsfData == NULL) {
        HKS_LOG_E("Malloc ksf data failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(newKsfData, sizeof(struct HksRkcKsfData), 0, sizeof(struct HksRkcKsfData));

    int32_t ret;
    do {
        /* Fill some fixed field. */
        newKsfData->version = g_hksRkcCfg.version;
        newKsfData->rmkIter = g_hksRkcCfg.rmkIter;
        newKsfData->rmkHashAlg = g_hksRkcCfg.rmkHashAlg;

        /* Two material are generated by random number. */
        ret = RkcMakeRandomMaterial(newKsfData);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Generate material failed! ret = 0x%X", ret);
            break;
        }

        /* The salt value is generated by random number. */
        struct HksBlob salt = { HKS_RKC_SALT_LEN, newKsfData->rmkSalt };
        ret = HksCryptoHalFillRandom(&salt);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Generate salt failed! ret = 0x%X", ret);
            break;
        }

        /* make main key. */
        ret = RkcMakeMk(newKsfData);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("make mk failed! ret = 0x%X", ret);
            break;
        }

        /* Write the root key component data into all keystore files */
        ret = RkcWriteAllKsf(newKsfData);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Write ksf failed! ret = 0x%X", ret);
        }
    } while (0);

    /* the data of root key should be cleared after use */
    (void)memset_s(newKsfData, sizeof(struct HksRkcKsfData), 0, sizeof(struct HksRkcKsfData));
    HKS_FREE_PTR(newKsfData);
    return ret;
}

static int32_t RkcInitKsf(void)
{
    int32_t ret;
    if (RkcKsfExist()) {
        HKS_LOG_I("Rkc ksf is exist, start to load ksf");
        ret = RkcLoadKsf();
    } else {
        HKS_LOG_I("Rkc ksf doesn't exist, start to creat ksf");
        ret = RkcCreateKsf();
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Load/Create ksf failed! ret = 0x%X", ret);
    }

    return ret;
}

static char *CloneNewStr(const char *srcStr, const uint32_t strLenMax)
{
    if (srcStr == NULL) {
        HKS_LOG_E("Invalid input string!");
        return NULL;
    }

    const size_t strLen = strlen(srcStr);
    if ((strLen == 0) || (strLen > strLenMax)) {
        HKS_LOG_E("Invalid input string! len = 0x%X, maxLen = 0x%X", strLen, strLenMax);
        return NULL;
    }

    char *newBuf = (char *)HksMalloc(strLen + 1); /* 1: end char */
    if (newBuf == NULL) {
        HKS_LOG_E("Malloc new buffer failed!");
        return NULL;
    }

    if (memcpy_s(newBuf, strLen, srcStr, strLen) != EOK) {
        HKS_LOG_E("Memcpy new buffer failed!");
        HKS_FREE_PTR(newBuf);
        return NULL;
    }
    newBuf[strLen] = '\0';

    return newBuf;
}

static int32_t RkcInitKsfAttr(const struct HksRkcKsfAttr *ksfAttr)
{
    /* clone keystore filename from parameter. */
    for (uint8_t i = 0; i < ksfAttr->num; ++i) {
        char *fileName = CloneNewStr(ksfAttr->name[i], HKS_RKC_KSF_NAME_LEN_MAX);
        if (fileName == NULL) {
            /* the memory will be freed by hksRkcDestroy() */
            return HKS_ERROR_MALLOC_FAIL;
        }

        g_hksRkcCfg.ksfAttr.name[i] = fileName;
    }

    g_hksRkcCfg.ksfAttr.num = ksfAttr->num;
    return HKS_SUCCESS;
}

int32_t HksRkcInit(void)
{
    if (g_hksRkcCfg.state == HKS_RKC_STATE_VALID) {
        HKS_LOG_I("Hks rkc is running!");
        return HKS_SUCCESS;
    }

    int32_t ret;
    const struct HksRkcInitParam *initParamInner = &g_hksRkcDefaultInitParam;
    do {
        g_hksRkcCfg.version = initParamInner->version;
        g_hksRkcCfg.storageType = initParamInner->storageType;
        g_hksRkcCfg.rmkIter = initParamInner->rmkIter;
        g_hksRkcCfg.rmkHashAlg = initParamInner->rmkHashAlg;
        g_hksRkcCfg.mkEncryptAlg = initParamInner->mkEncryptAlg;

        /* Initialize the attribute of keystore file */
        ret = RkcInitKsfAttr(&(initParamInner->ksfAttr));
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Init attribute of keystore file failed! ret = 0x%X", ret);
            break;
        }

        /* Initialize the keystore file of root key component. */
        ret = RkcInitKsf();
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Init root key component failed! ret = 0x%X", ret);
        }
    } while (0);

    if (ret != HKS_SUCCESS) {
        (void)HksRkcDestroy();
        return ret;
    }

    g_hksRkcCfg.state = HKS_RKC_STATE_VALID;
    return HKS_SUCCESS;
}

void HksRkcDestroy(void)
{
    g_hksRkcCfg.state = HKS_RKC_STATE_INVALID;
    HksRkcClearMem();
}

void HksRkcClearMem(void)
{
    for (uint32_t i = 0; i < HKS_RKC_KSF_NUM; ++i) {
        HKS_FREE_PTR(g_hksRkcCfg.ksfAttr.name[i]);
    }

    (void)memset_s(&g_hksRkcCfg, sizeof(g_hksRkcCfg), 0, sizeof(g_hksRkcCfg));
    (void)memset_s(&g_hksRkcMk, sizeof(g_hksRkcMk), 0, sizeof(g_hksRkcMk));
}

int32_t HksRkcGetMainKey(struct HksBlob *mainKey)
{
    if (!g_hksRkcMk.valid) {
        HKS_LOG_E("Main key is invalid now, initialization is required before Getting main key!");
        return HKS_FAILURE;
    }

    if (mainKey->size != HKS_RKC_MK_LEN) {
        HKS_LOG_E("Invalid mainKey size! size = 0x%X", mainKey->size);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    /* remove mask */
    for (uint32_t i = 0; i < HKS_RKC_MK_LEN; ++i) {
        mainKey->data[i] = g_hksRkcMk.mkWithMask[i] ^ g_hksRkcCfg.mkMask[i];
    }

    return HKS_SUCCESS;
}
#endif /* _CUT_AUTHENTICATE_ */
