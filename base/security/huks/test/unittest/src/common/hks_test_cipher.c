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
#include "hks_test_cipher.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"

#define DEFAULT_AES_CIPHER_PLAIN_SIZE 1000
#define AES_DEFAULT_GCM_NONCE_LENGTH 12
#define AES_DEFAULT_AAD_LEN 4
#define DEFAULT_AES_LOCAL_PARAM_SET_OUT_SIZE 256
#define AES_LOCAL_KEY_SIZE_128 16
#define AES_LOCAL_KEY_SIZE_256 32

static const struct HksTestCipherParams g_testCipherParams[] = {
    /* success: local aes256-gcm-none */
    { 0, HKS_SUCCESS, { false, 0, false, 0 },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            true, HKS_STORAGE_TEMP
        },
        { false, 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES,  true, /* encrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_ENCRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            true, AES_DEFAULT_AAD_LEN,
            true, false
        },
        {
            HKS_TEST_CIPHER_TYPE_AES,  true, /* decrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            true, AES_DEFAULT_AAD_LEN,
            true, false
        },
        {
            true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        {
            true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16 },
        {
            true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        {
            true, AES_LOCAL_KEY_SIZE_256, true, AES_LOCAL_KEY_SIZE_256 }
    },

    /* success: aes256-gcm-none */
    { 1, HKS_SUCCESS, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0
        },
        { false, 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES,  true, /* encrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_ENCRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            true, AES_DEFAULT_AAD_LEN
        },
        {
            HKS_TEST_CIPHER_TYPE_AES,  true, /* decrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            true, AES_DEFAULT_AAD_LEN
        },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16 },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { false, 0, false, 0 }
    },
};

static const struct HksTestEncryptParams g_testEncryptParams[] = {
    /* aes gcm nonce not set */
    { 0, HKS_ERROR_CHECK_GET_NONCE_FAIL, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM
        },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES, true, /* encrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_ENCRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            false, 0,
            true, AES_DEFAULT_AAD_LEN,
            false, true
        },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16 },
        { false, 0, false, 0 }
    },

    /* aes gcm nonce size invalid */
    { 1, HKS_ERROR_INVALID_NONCE, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM
        },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES, true, /* encrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_ENCRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH - 1,
            true, AES_DEFAULT_AAD_LEN,
            false, true
        },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16 },
        { false, 0, false, 0 }
    },

    /* aes gcm aad not set */
    { 2, HKS_ERROR_CHECK_GET_AAD_FAIL, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM
        },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES, true, /* encrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_ENCRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            false, 0,
            false, true
        },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16 },
        { false, 0, false, 0 }
    },
    /* aes gcm inSize == 0 */
    { 3, HKS_ERROR_INVALID_ARGUMENT, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM
        },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES, true, /* encrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_ENCRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            true, AES_DEFAULT_AAD_LEN,
            false, true
        },
        { true, 0, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16 },
        { false, 0, false, 0 }
    },

    /* aes gcm outSize too small */
    { 4, HKS_ERROR_BUFFER_TOO_SMALL, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES, true, /* encrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_ENCRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            true, AES_DEFAULT_AAD_LEN,
            false, true
        },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16 - 1, true, DEFAULT_AES_CIPHER_PLAIN_SIZE + 16 - 1 },
        { false, 0, false, 0 }
    },
};

static const struct HksTestDecryptParams g_testDecryptParams[] = {
    /* aes gcm nonce not set */
    { 0, HKS_ERROR_CHECK_GET_NONCE_FAIL, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM
        },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES,  true, /* decrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            false, AES_DEFAULT_GCM_NONCE_LENGTH,
            true, AES_DEFAULT_AAD_LEN,
            false, true
        },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16 },
        { false, 0, false, 0 }
    },

    /* aes gcm nonce size invalid */
    { 1, HKS_ERROR_INVALID_NONCE, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM
        },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES,  true, /* decrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH - 1,
            true, AES_DEFAULT_AAD_LEN,
            false, true
        },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16 },
        { false, 0, false, 0 }
    },

    /* aes gcm aad not set */
    { 2, HKS_ERROR_CHECK_GET_AAD_FAIL, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM
        },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES,  true, /* decrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            false, 0,
            false, true
        },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16 },
        { false, 0, false, 0 }
    },
    /* aes gcm inSize <= 16  */
    { 3, HKS_ERROR_INVALID_ARGUMENT, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM
        },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES,  true, /* decrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            true, AES_DEFAULT_AAD_LEN,
            false, true
        },
        { true, 16, true, 16 },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16, true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16 },
        { false, 0, false, 0 }
    },

    /* aes gcm outSize too small */
    { 4, HKS_ERROR_BUFFER_TOO_SMALL, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        {
            true, /* genKey params */
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_256,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM
        },
        { 0 },
        {
            HKS_TEST_CIPHER_TYPE_AES, true, /* decrypt params */
            true, HKS_ALG_AES,
            true, HKS_KEY_PURPOSE_DECRYPT,
            false, 0,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_GCM,
            false, 0,
            true, AES_DEFAULT_GCM_NONCE_LENGTH,
            true, AES_DEFAULT_AAD_LEN,
            false, true
        },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE, true, DEFAULT_AES_CIPHER_PLAIN_SIZE },
        { true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16 - 1, true, DEFAULT_AES_CIPHER_PLAIN_SIZE - 16 -1 },
        { false, 0, false, 0 }
    },
};

static int32_t ConstructDataToBlob(struct HksBlob **inData, struct HksBlob **outData,
    const struct HksTestBlobParams *inTextParams, const struct HksTestBlobParams *outTextParams)
{
    int32_t ret = TestConstuctBlob(inData,
        inTextParams->blobExist,
        inTextParams->blobSize,
        inTextParams->blobDataExist,
        inTextParams->blobDataSize);
    HKS_TEST_ASSERT(ret == 0);

    ret = TestConstuctBlob(outData,
        outTextParams->blobExist,
        outTextParams->blobSize,
        outTextParams->blobDataExist,
        outTextParams->blobDataSize);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

static int32_t Encrypt(struct CipherEncryptStructure *encryptStruct)
{
    int32_t ret;
    struct HksParamSet *encryptParamSet = NULL;

    if (encryptStruct->cipherParms->cipherType == HKS_TEST_CIPHER_TYPE_RSA) {
        struct TestRsaCipherParamSet rsaCipherParamStructure = {
            &encryptParamSet,
            encryptStruct->cipherParms->paramSetExist,
            encryptStruct->cipherParms->setAlg, encryptStruct->cipherParms->alg,
            encryptStruct->cipherParms->setPurpose, encryptStruct->cipherParms->purpose,
            encryptStruct->cipherParms->setDigest, encryptStruct->cipherParms->digest,
            encryptStruct->cipherParms->setPadding, encryptStruct->cipherParms->padding
        };
        ret = TestConstructRsaCipherParamSet(&rsaCipherParamStructure);
    } else {
        uint32_t ivSize = encryptStruct->cipherParms->ivSize;
        uint32_t nonceSize = encryptStruct->cipherParms->nonceSize;
        uint32_t aadSize = encryptStruct->cipherParms->aadSize;
        if (ivSize != 0) {
            ret = TestConstuctBlob(encryptStruct->ivData, true, ivSize, true, ivSize);
            HKS_TEST_ASSERT(ret == 0);
        }
        if (nonceSize != 0) {
            ret = TestConstuctBlob(encryptStruct->nonceData, true, nonceSize, true, nonceSize);
            HKS_TEST_ASSERT(ret == 0);
        }
        if (aadSize != 0) {
            ret = TestConstuctBlob(encryptStruct->aadData, true, aadSize, true, aadSize);
            HKS_TEST_ASSERT(ret == 0);
        }
        struct AesCipherParamSetStructure enParamStruct = {
            &encryptParamSet,
            encryptStruct->cipherParms->paramSetExist,
            encryptStruct->cipherParms->setAlg, encryptStruct->cipherParms->alg,
            encryptStruct->cipherParms->setPurpose, encryptStruct->cipherParms->purpose,
            encryptStruct->cipherParms->setPadding, encryptStruct->cipherParms->padding,
            encryptStruct->cipherParms->setBlockMode, encryptStruct->cipherParms->mode,
            encryptStruct->cipherParms->setIv, *(encryptStruct->ivData),
            encryptStruct->cipherParms->setNonce, *(encryptStruct->nonceData),
            encryptStruct->cipherParms->setAad, *(encryptStruct->aadData),
            encryptStruct->cipherParms->setIsKeyAlias, encryptStruct->cipherParms->isKeyAlias
        };
        ret = TestConstructAesCipherParamSet(&enParamStruct);
        HKS_TEST_ASSERT(ret == 0);
    }

    ret = HksEncryptRun(encryptStruct->keyAlias, encryptParamSet, encryptStruct->plainData, encryptStruct->cipherData,
        encryptStruct->performTimes);
    HksFreeParamSet(&encryptParamSet);
    return ret;
}

static int32_t DecryptCipher(struct CipherDecryptStructure *decryptStruct)
{
    int32_t ret = TestConstuctBlob(decryptStruct->decryptedData,
        decryptStruct->cipherParms->decryptedTextParams.blobExist,
        decryptStruct->cipherParms->decryptedTextParams.blobSize,
        decryptStruct->cipherParms->decryptedTextParams.blobDataExist,
        decryptStruct->cipherParms->decryptedTextParams.blobDataSize);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParamSet *decryptParamSet = NULL;
    if (decryptStruct->cipherParms->decryptParamSetParams.cipherType == HKS_TEST_CIPHER_TYPE_RSA) {
        struct TestRsaCipherParamSet rsaDeCipherParamStructure = {
            &decryptParamSet,
            decryptStruct->cipherParms->decryptParamSetParams.paramSetExist,
            decryptStruct->cipherParms->decryptParamSetParams.setAlg,
            decryptStruct->cipherParms->decryptParamSetParams.alg,
            decryptStruct->cipherParms->decryptParamSetParams.setPurpose,
            decryptStruct->cipherParms->decryptParamSetParams.purpose,
            decryptStruct->cipherParms->decryptParamSetParams.setDigest,
            decryptStruct->cipherParms->decryptParamSetParams.digest,
            decryptStruct->cipherParms->decryptParamSetParams.setPadding,
            decryptStruct->cipherParms->decryptParamSetParams.padding
        };
        ret = TestConstructRsaCipherParamSet(&rsaDeCipherParamStructure);
    } else {
        struct AesCipherParamSetStructure deParamStruct = {
            &decryptParamSet,
            decryptStruct->cipherParms->decryptParamSetParams.paramSetExist,
            decryptStruct->cipherParms->decryptParamSetParams.setAlg,
            decryptStruct->cipherParms->decryptParamSetParams.alg,
            decryptStruct->cipherParms->decryptParamSetParams.setPurpose,
            decryptStruct->cipherParms->decryptParamSetParams.purpose,
            decryptStruct->cipherParms->decryptParamSetParams.setPadding,
            decryptStruct->cipherParms->decryptParamSetParams.padding,
            decryptStruct->cipherParms->decryptParamSetParams.setBlockMode,
            decryptStruct->cipherParms->decryptParamSetParams.mode,
            decryptStruct->cipherParms->decryptParamSetParams.setIv, decryptStruct->ivData,
            decryptStruct->cipherParms->decryptParamSetParams.setNonce, decryptStruct->nonceData,
            decryptStruct->cipherParms->decryptParamSetParams.setAad, decryptStruct->aadData,
            decryptStruct->cipherParms->decryptParamSetParams.setIsKeyAlias,
            decryptStruct->cipherParms->decryptParamSetParams.isKeyAlias
        };
        ret = TestConstructAesCipherParamSet(&deParamStruct);
        HKS_TEST_ASSERT(ret == 0);
    }

    ret = HksDecryptRun(decryptStruct->keyAlias, decryptParamSet, decryptStruct->cipherData,
        *(decryptStruct->decryptedData), decryptStruct->performTimes);
    HksFreeParamSet(&decryptParamSet);
    return ret;
}

static int32_t Decrypt(struct OnlyDecryptStructure *onlyDecryptStruct)
{
    struct HksParamSet *decryptParamSet = NULL;
    int32_t ret;
    if (onlyDecryptStruct->cipherParms->cipherType == HKS_TEST_CIPHER_TYPE_RSA) {
        struct TestRsaCipherParamSet rsaCipherParamStructure = {
            &decryptParamSet,
            onlyDecryptStruct->cipherParms->paramSetExist,
            onlyDecryptStruct->cipherParms->setAlg, onlyDecryptStruct->cipherParms->alg,
            onlyDecryptStruct->cipherParms->setPurpose, onlyDecryptStruct->cipherParms->purpose,
            onlyDecryptStruct->cipherParms->setDigest, onlyDecryptStruct->cipherParms->digest,
            onlyDecryptStruct->cipherParms->setPadding, onlyDecryptStruct->cipherParms->padding
        };
        ret = TestConstructRsaCipherParamSet(&rsaCipherParamStructure);
    } else {
        uint32_t ivSize = onlyDecryptStruct->cipherParms->ivSize;
        uint32_t nonceSize = onlyDecryptStruct->cipherParms->nonceSize;
        uint32_t aadSize = onlyDecryptStruct->cipherParms->aadSize;
        if (ivSize != 0) {
            ret = TestConstuctBlob(onlyDecryptStruct->ivData, true, ivSize, true, ivSize);
            HKS_TEST_ASSERT(ret == 0);
        }
        if (nonceSize != 0) {
            ret = TestConstuctBlob(onlyDecryptStruct->nonceData, true, nonceSize, true, nonceSize);
            HKS_TEST_ASSERT(ret == 0);
        }
        if (aadSize != 0) {
            ret = TestConstuctBlob(onlyDecryptStruct->aadData, true, aadSize, true, aadSize);
            HKS_TEST_ASSERT(ret == 0);
        }
        struct AesCipherParamSetStructure onlyDeParamStruct = {
            &decryptParamSet,
            onlyDecryptStruct->cipherParms->paramSetExist,
            onlyDecryptStruct->cipherParms->setAlg, onlyDecryptStruct->cipherParms->alg,
            onlyDecryptStruct->cipherParms->setPurpose, onlyDecryptStruct->cipherParms->purpose,
            onlyDecryptStruct->cipherParms->setPadding, onlyDecryptStruct->cipherParms->padding,
            onlyDecryptStruct->cipherParms->setBlockMode, onlyDecryptStruct->cipherParms->mode,
            onlyDecryptStruct->cipherParms->setIv, *(onlyDecryptStruct->ivData),
            onlyDecryptStruct->cipherParms->setNonce, *(onlyDecryptStruct->nonceData),
            onlyDecryptStruct->cipherParms->setAad, *(onlyDecryptStruct->aadData),
            onlyDecryptStruct->cipherParms->setIsKeyAlias, onlyDecryptStruct->cipherParms->isKeyAlias
        };
        ret = TestConstructAesCipherParamSet(&onlyDeParamStruct);
    }
    HKS_TEST_ASSERT(ret == 0);

    ret = HksDecryptRun(onlyDecryptStruct->keyAlias, decryptParamSet, onlyDecryptStruct->cipherData,
        onlyDecryptStruct->decryptedData, onlyDecryptStruct->performTimes);
    HksFreeParamSet(&decryptParamSet);
    return ret;
}


int32_t BaseTestCipher(uint32_t times, uint32_t index, uint32_t performTimes)
{
    /* 1. generate key */
    struct HksBlob *keyAlias = NULL;
    int32_t ret;
    if ((g_testCipherParams[index].genKeyParamSetParams.setKeyStorageFlag) &&
        g_testCipherParams[index].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP) {
        ret = GenerateLocalRandomKey(&keyAlias, &g_testCipherParams[index].localKeyParams);
    } else {
        ret = GenerateKey(&keyAlias, &g_testCipherParams[index].keyAliasParams,
            &g_testCipherParams[index].genKeyParamSetParams, &g_testCipherParams[index].genKeyParamSetParamsOut);
    }
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob *plainData = NULL;
    struct HksBlob *cipherData = NULL;
    ret = ConstructDataToBlob(&plainData, &cipherData,
        &g_testCipherParams[index].plainTextParams, &g_testCipherParams[index].cipherTextParams);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob *ivData = NULL;
    struct HksBlob *nonceData = NULL;
    struct HksBlob *aadData = NULL;
    /* 2. encrypt */
    struct CipherEncryptStructure testEncryptStruct = {
        keyAlias, &g_testCipherParams[index].encryptParamSetParams,
        plainData, cipherData, &ivData, &nonceData, &aadData, performTimes
    };
    ret = Encrypt(&testEncryptStruct);
    HKS_TEST_ASSERT(ret == g_testCipherParams[index].expectResult);

    /* 3. decrypt */
    struct HksBlob *decryptedData = NULL;
    struct CipherDecryptStructure testDecryptStruct = {
        keyAlias, &g_testCipherParams[index], cipherData,
        &decryptedData, ivData, nonceData, aadData, performTimes
    };
    ret = DecryptCipher(&testDecryptStruct);
    HKS_TEST_ASSERT(ret == g_testCipherParams[index].expectResult);

    HKS_TEST_ASSERT(plainData->size == decryptedData->size);
    HKS_TEST_ASSERT(memcmp(plainData->data, decryptedData->data, plainData->size) == 0);
    if (!((g_testCipherParams[index].genKeyParamSetParams.setKeyStorageFlag) &&
        g_testCipherParams[index].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) {
        HKS_TEST_ASSERT(HksDeleteKey(keyAlias, NULL) == 0);
    }
    TestFreeBlob(&keyAlias);
    TestFreeBlob(&plainData);
    TestFreeBlob(&cipherData);
    TestFreeBlob(&decryptedData);
    TestFreeBlob(&ivData);
    TestFreeBlob(&nonceData);
    TestFreeBlob(&aadData);
    return (ret != g_testCipherParams[index].expectResult);
}

int32_t BaseTestEncrypt(uint32_t times, uint32_t index, uint32_t performTimes)
{
    /* 1. generate key */
    struct HksBlob *keyAlias = NULL;
    int32_t ret;
    if (g_testEncryptParams[index].encryptParamSetParams.setIsKeyAlias &&
        !g_testEncryptParams[index].encryptParamSetParams.isKeyAlias) {
        ret = GenerateLocalRandomKey(&keyAlias, &g_testEncryptParams[index].localKeyParams);
    } else {
        if (g_testEncryptParams[index].keyAliasParams.blobExist) {
            ret = GenerateKey(&keyAlias, &g_testEncryptParams[index].keyAliasParams,
                &g_testEncryptParams[index].genKeyParamSetParams, NULL);
        } else {
            ret = TestConstuctBlob(&keyAlias,
                g_testEncryptParams[index].encryptAliasParams.blobExist,
                g_testEncryptParams[index].encryptAliasParams.blobSize,
                g_testEncryptParams[index].encryptAliasParams.blobDataExist,
                g_testEncryptParams[index].encryptAliasParams.blobDataSize);
        }
    }
    HKS_TEST_ASSERT(ret == 0);

    struct HksBlob *plainData = NULL;
    struct HksBlob *cipherData = NULL;
    ret = ConstructDataToBlob(&plainData, &cipherData,
        &g_testEncryptParams[index].inDataParams, &g_testEncryptParams[index].outDataParams);
    HKS_TEST_ASSERT(ret == 0);

    struct HksBlob *ivData = NULL;
    struct HksBlob *nonceData = NULL;
    struct HksBlob *aadData = NULL;
    /* 2. encrypt */
    struct CipherEncryptStructure encryptStruct = {
        keyAlias, &g_testEncryptParams[index].encryptParamSetParams, plainData, cipherData, &ivData,
        &nonceData, &aadData, performTimes
    };
    ret = Encrypt(&encryptStruct);
    HKS_TEST_ASSERT(ret == g_testEncryptParams[index].expectResult);

    /* 4. delete key */
    if (g_testEncryptParams[index].keyAliasParams.blobExist) {
        HKS_TEST_ASSERT(HksDeleteKey(keyAlias, NULL) == 0);
    }
    TestFreeBlob(&keyAlias);
    TestFreeBlob(&plainData);
    TestFreeBlob(&cipherData);
    TestFreeBlob(&ivData);
    TestFreeBlob(&nonceData);
    TestFreeBlob(&aadData);
    return (ret != g_testEncryptParams[index].expectResult);
}

int32_t BaseTestDecrypt(uint32_t times, uint32_t index, uint32_t performTimes)
{
    /* 1. generate key */
    struct HksBlob *keyAlias = NULL;
    int32_t ret;
    if (g_testDecryptParams[index].decryptParamSetParams.setIsKeyAlias &&
        !g_testDecryptParams[index].decryptParamSetParams.isKeyAlias) {
        ret = GenerateLocalRandomKey(&keyAlias, &g_testDecryptParams[index].localKeyParams);
    } else {
        if (g_testDecryptParams[index].keyAliasParams.blobExist) {
            ret = GenerateKey(&keyAlias, &g_testDecryptParams[index].keyAliasParams,
                &g_testDecryptParams[index].genKeyParamSetParams, NULL);
        } else {
            ret = TestConstuctBlob(&keyAlias,
                g_testDecryptParams[index].decryptAliasParams.blobExist,
                g_testDecryptParams[index].decryptAliasParams.blobSize,
                g_testDecryptParams[index].decryptAliasParams.blobDataExist,
                g_testDecryptParams[index].decryptAliasParams.blobDataSize);
        }
    }
    HKS_TEST_ASSERT(ret == 0);

    struct HksBlob *cipherData = NULL;
    struct HksBlob *decryptedData = NULL;
    ret = ConstructDataToBlob(&cipherData, &decryptedData,
        &g_testDecryptParams[index].inDataParams, &g_testDecryptParams[index].outDataParams);
    HKS_TEST_ASSERT(ret == 0);

    struct HksBlob *ivData = NULL;
    struct HksBlob *nonceData = NULL;
    struct HksBlob *aadData = NULL;
    /* 3. encrypt */
    struct OnlyDecryptStructure onlyDecryptStruct = {
        keyAlias, &g_testDecryptParams[index].decryptParamSetParams, cipherData, decryptedData, &ivData,
        &nonceData, &aadData, performTimes
    };
    ret = Decrypt(&onlyDecryptStruct);
    HKS_TEST_ASSERT(ret == g_testDecryptParams[index].expectResult);

    /* 4. delete key */
    if (g_testDecryptParams[index].keyAliasParams.blobExist) {
        HKS_TEST_ASSERT(HksDeleteKey(keyAlias, NULL) == 0);
    }
    TestFreeBlob(&keyAlias);
    TestFreeBlob(&decryptedData);
    TestFreeBlob(&cipherData);
    TestFreeBlob(&ivData);
    TestFreeBlob(&nonceData);
    TestFreeBlob(&aadData);
    return (ret != g_testDecryptParams[index].expectResult);
}