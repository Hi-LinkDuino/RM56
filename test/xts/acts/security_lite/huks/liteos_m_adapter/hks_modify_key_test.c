/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef _CUT_AUTHENTICATE_

#include "hks_modify_key_test.h"

#include <hctest.h>
#include "iot_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_cipher.h"
#include "hks_test_common.h"
#include "hks_test_log.h"
#include "stdlib.h"

#include "cmsis_os2.h"
#include "ohos_types.h"

#define DEFAULT_AES_CIPHER_PLAIN_SIZE 1000
#define AES_DEFAULT_GCM_NONCE_LENGTH 12
#define AES_DEFAULT_AAD_LEN 4
static const char *g_storePath = "./hks_store/";
static const char *g_testName = "TestName";

#define TEST_TASK_STACK_SIZE      0x2000
#define WAIT_TO_TEST_DONE         4

static osPriority_t g_setPriority;

static const struct HksTestCipherParams g_testCipherParams[] = {
    /* success: aes256-gcm-none */
    { 0, HKS_SUCCESS, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
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

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(security, securityData, HksModifyKeyTest);

static void ExecHksInitialize(void const *argument)
{
    LiteTestPrint("HksInitialize Begin!\n");
    TEST_ASSERT_TRUE(HksInitialize() == 0);
    LiteTestPrint("HksInitialize End!\n");
    osThreadExit();
}

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksModifyKeyTestSetUp()
{
    LiteTestPrint("setup\n");
    IoTWatchDogDisable();
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityAboveNormal6;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)ExecHksInitialize, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksModifyKeyTestSetUp End2!\n");
    return TRUE;
}

static void HksTestRemoveFile()
{
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksModifyKeyTestTearDown()
{
    LiteTestPrint("tearDown\n");
    HksTestRemoveFile();
    IoTWatchDogEnable();
    return TRUE;
}


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

    ret = HksDecryptRun(decryptStruct->keyAlias, decryptParamSet, decryptStruct->cipherData,
        *(decryptStruct->decryptedData), decryptStruct->performTimes);
    HksFreeParamSet(&decryptParamSet);
    return ret;
}

int32_t GenerateKeyTwo(const struct HksBlob *keyAlias, const struct HksTestBlobParams *keyAliasParams,
    const struct HksTestGenKeyParamsParamSet *genKeyParamSetParams,
    const struct HksTestGenKeyParamsParamSetOut *genKeyParamSetParamsOut)
{
    struct HksParamSet *paramSet = NULL;
    struct GenerateKeyParamSetStructure paramStruct = {
        &paramSet,
        genKeyParamSetParams->paramSetExist,
        genKeyParamSetParams->setAlg, genKeyParamSetParams->alg,
        genKeyParamSetParams->setKeySize, genKeyParamSetParams->keySize,
        genKeyParamSetParams->setPurpose, genKeyParamSetParams->purpose,
        genKeyParamSetParams->setDigest, genKeyParamSetParams->digest,
        genKeyParamSetParams->setPadding, genKeyParamSetParams->padding,
        genKeyParamSetParams->setBlockMode, genKeyParamSetParams->mode,
        genKeyParamSetParams->setKeyStorageFlag, genKeyParamSetParams->keyStorageFlag
    };
    int32_t ret = TestConstructGenerateKeyParamSet(&paramStruct);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParamSet *paramSetOut = NULL;
    if (genKeyParamSetParamsOut != NULL) {
        ret = TestConstructGenerateKeyParamSetOut(&paramSet,
            genKeyParamSetParamsOut->paramSetExist, genKeyParamSetParamsOut->paramSetSize);
        HKS_TEST_ASSERT(ret == 0);
    }

    ret = HksGenerateKey(keyAlias, paramSet, paramSetOut);
    HKS_TEST_ASSERT(ret == 0);

    HksFreeParamSet(&paramSet);
    return ret;
}

int32_t BaseTestCipherProcess(const struct HksBlob *keyAlias, uint32_t index)
{
    struct HksBlob *plainData = NULL;
    struct HksBlob *cipherData = NULL;
    int32_t ret = ConstructDataToBlob(&plainData, &cipherData,
        &g_testCipherParams[index].plainTextParams, &g_testCipherParams[index].cipherTextParams);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob *ivData = NULL;
    struct HksBlob *nonceData = NULL;
    struct HksBlob *aadData = NULL;
    struct HksBlob *decryptedData = NULL;
    /* 2. encrypt */
    do {
        struct CipherEncryptStructure testEncryptStruct = {
            keyAlias, &g_testCipherParams[index].encryptParamSetParams,
            plainData, cipherData, &ivData, &nonceData, &aadData, 1
        };
        ret = Encrypt(&testEncryptStruct);
        if (ret != g_testCipherParams[index].expectResult) {
            break;
        }
        /* 3. decrypt */
        struct CipherDecryptStructure testDecryptStruct = {
            keyAlias, &g_testCipherParams[index], cipherData,
            &decryptedData, ivData, nonceData, aadData, 1
        };
        ret = DecryptCipher(&testDecryptStruct);
        if (ret != g_testCipherParams[index].expectResult) {
            break;
        }

        if (ret == g_testCipherParams[index].expectResult) {
            if (plainData->size != decryptedData->size) {
                break;
            };
            ret = memcmp(plainData->data, decryptedData->data, plainData->size);
        }
    } while (0);
    TestFreeBlob(&plainData);
    TestFreeBlob(&cipherData);
    TestFreeBlob(&decryptedData);
    TestFreeBlob(&ivData);
    TestFreeBlob(&nonceData);
    TestFreeBlob(&aadData);
    return ret;
}

enum HksStorageType {
    HKS_STORAGE_TYPE_KEY = 0,
    HKS_STORAGE_TYPE_CERTCHAIN,
    HKS_STORAGE_TYPE_ROOT_KEY,
};

extern int32_t HksStoreKeyBlob(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    enum HksStorageType storageType, const struct HksBlob *keyBlob);
int32_t __attribute__((weak)) HksStoreKeyBlob(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    enum HksStorageType storageType, const struct HksBlob *keyBlob)
{
    (void)processName;
    (void)keyAlias;
    (void)storageType;
    (void)keyBlob;
}

static void ExecHksModifyKeyTest001(void const *argument)
{
    LiteTestPrint("HksModifyKeyTest001 Begin!\n");

    uint32_t index = 0;
    struct HksBlob keyAlias = { strlen(g_testName), (uint8_t *)g_testName };
    int32_t ret = GenerateKeyTwo(&keyAlias, &g_testCipherParams[index].keyAliasParams,
                  &g_testCipherParams[index].genKeyParamSetParams,
		  &g_testCipherParams[index].genKeyParamSetParamsOut);
    TEST_ASSERT_TRUE(ret == 0);

    ret = BaseTestCipherProcess(&keyAlias, 0);
    TEST_ASSERT_TRUE(ret == 0);

    struct HksBlob *plainData = NULL;
    struct HksBlob *cipherData = NULL;
    ret = ConstructDataToBlob(&plainData, &cipherData,
        &g_testCipherParams[index].plainTextParams, &g_testCipherParams[index].cipherTextParams);
    TEST_ASSERT_TRUE(ret == 0);
    struct HksBlob *ivData = NULL;
    struct HksBlob *nonceData = NULL;
    struct HksBlob *aadData = NULL;

    /* 2. encrypt */
    struct CipherEncryptStructure testEncryptStruct = {
        &keyAlias, &g_testCipherParams[index].encryptParamSetParams,
        plainData, cipherData, &ivData, &nonceData, &aadData, 1
    };
    ret = Encrypt(&testEncryptStruct);
    TEST_ASSERT_TRUE(ret == 0);

    ret = GenerateKeyTwo(&keyAlias, &g_testCipherParams[index].keyAliasParams,
                         &g_testCipherParams[index].genKeyParamSetParams,
			 &g_testCipherParams[index].genKeyParamSetParamsOut);
    TEST_ASSERT_TRUE(ret == 0);

    /* 3. decrypt */
    struct HksBlob *decryptedData = NULL;
    struct CipherDecryptStructure testDecryptStruct = {
        &keyAlias, &g_testCipherParams[index], cipherData,
        &decryptedData, ivData, nonceData, aadData, 1
    };
    ret = DecryptCipher(&testDecryptStruct);

    HKS_TEST_ASSERT(ret != g_testCipherParams[index].expectResult);
    TestFreeBlob(&plainData);
    TestFreeBlob(&cipherData);
    TestFreeBlob(&decryptedData);
    TestFreeBlob(&ivData);
    TestFreeBlob(&nonceData);
    TestFreeBlob(&aadData);
    TEST_ASSERT_TRUE(ret != 0);

    LiteTestPrint("HksModifyKeyTest001 End!\n");
    osThreadExit();
}

/**
 * @tc.name: HksModifyKeyTest.HksModifyKeyTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksModifyKeyTest, HksModifyKeyTest001, Level1)
{
    osThreadId_t id;
    osThreadAttr_t attr;
    g_setPriority = osPriorityAboveNormal6;
    attr.name = "test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TEST_TASK_STACK_SIZE;
    attr.priority = g_setPriority;
    id = osThreadNew((osThreadFunc_t)ExecHksModifyKeyTest001, NULL, &attr);
    sleep(WAIT_TO_TEST_DONE);
    LiteTestPrint("HksModifyKeyTest001 End2!\n");
}

RUN_TEST_SUITE(HksModifyKeyTest);
#endif /* _CUT_AUTHENTICATE_ */
