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

#include "hctest.h"
#include "hi_watchdog.h"
#include "hks_agreement_test.h"

#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"
#include "hks_type.h"

#ifndef _CUT_AUTHENTICATE_

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(husk, huks_lite, HksAgreementTest);

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksAgreementTestSetUp()
{
    LiteTestPrint("setup\n");

    hi_watchdog_disable();
    TEST_ASSERT_TRUE(HksInitialize() == 0);
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksAgreementTestTearDown()
{
    LiteTestPrint("tearDown\n");

    hi_watchdog_enable();
    return TRUE;
}

#define TMP_SIZE 512
#define X25519_KEY_SIZE 32
static const struct HksTestAgreeParams g_testAgreeParams[] = {
    /* ree x25519 success */
    { 0, HKS_SUCCESS, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE },
        { true, true, HKS_ALG_X25519, true, HKS_CURVE25519_KEY_SIZE_256, true,
            HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY, true, HKS_DIGEST_SHA256,
            false, 0, false, 0, true, HKS_STORAGE_TEMP },
        { 0 },
        { true, true, HKS_ALG_X25519, true, HKS_CURVE25519_KEY_SIZE_256, true, false },
        { true, TMP_SIZE, true, TMP_SIZE },
        { true, X25519_KEY_SIZE, true, X25519_KEY_SIZE },
        { true, X25519_KEY_SIZE, true, X25519_KEY_SIZE },
        { true, X25519_KEY_SIZE, true, X25519_KEY_SIZE }
    },
};

static int32_t AgreeKey(const struct HksTestAgreeParamSet *agreeParamSetParams, struct HksBlob *privateKey,
    struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    struct HksParamSet *agreeParamSet = NULL;
    struct TestAgreeParamSetStructure paramStruct = {
        &agreeParamSet,
        agreeParamSetParams->paramSetExist,
        agreeParamSetParams->setAlg, agreeParamSetParams->alg,
        agreeParamSetParams->setKeySize, agreeParamSetParams->keySize,
        agreeParamSetParams->setIsKeyAlias, agreeParamSetParams->isKeyAlias
    };
    int32_t ret = TestConstructAgreeParamSet(&paramStruct);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksAgreeKeyRun(agreeParamSet, privateKey, peerPublicKey, agreedKey, 1);
    HksFreeParamSet(&agreeParamSet);
    return ret;
}

/**
 * @tc.name: HksDeleteTest.HksDeleteTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAgreementTest, HksAgreementTest001, Level1)
{
     /* 1. generate key */
    struct HksBlob *privateKey = NULL;
    struct HksBlob *peerPubKeyAlias = NULL;
    struct HksBlob *peerPublicKey = NULL;
    int32_t ret;

    if (g_testAgreeParams[0].genKeyParamSetParams.setKeyStorageFlag &&
        (g_testAgreeParams[0].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) {
        ret = GenerateLocalX25519Key(&privateKey, NULL, &g_testAgreeParams[0].localPrivateKeyParams, NULL);
        HKS_TEST_ASSERT(ret == 0);
        ret = GenerateLocalX25519Key(NULL, &peerPublicKey, NULL, &g_testAgreeParams[0].localPublicKeyParams);
        HKS_TEST_ASSERT(ret == 0);
    }
    /* 2. agreeKey */
    struct HksBlob *agreeKey = NULL;
    ret = TestConstuctBlob(&agreeKey,
        g_testAgreeParams[0].agreedKeyParams.blobExist,
        g_testAgreeParams[0].agreedKeyParams.blobSize,
        g_testAgreeParams[0].agreedKeyParams.blobDataExist,
        g_testAgreeParams[0].agreedKeyParams.blobDataSize);
    HKS_TEST_ASSERT(ret == 0);

    ret = AgreeKey(&g_testAgreeParams[0].agreeParamSetParams, privateKey, peerPublicKey, agreeKey);
    HKS_TEST_ASSERT(ret == g_testAgreeParams[0].expectResult);

    /* 3. delete key */
    if (!(g_testAgreeParams[0].genKeyParamSetParams.setKeyStorageFlag &&
        (g_testAgreeParams[0].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) &&
        ((g_testAgreeParams[0].keyAlias1Params.blobExist) &&
        (g_testAgreeParams[0].keyAlias2Params.blobExist))) {
        ret = HksDeleteKey(privateKey, NULL);
        HKS_TEST_ASSERT(ret == 0);
        ret = HksDeleteKey(peerPubKeyAlias, NULL);
        HKS_TEST_ASSERT(ret == 0);
    }
    TestFreeBlob(&privateKey);
    TestFreeBlob(&peerPubKeyAlias);
    TestFreeBlob(&peerPublicKey);
    TestFreeBlob(&agreeKey);
    TEST_ASSERT_TRUE(ret == 0);
}


/**
 * @tc.name: HksDeleteTest.HksDeleteTest002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAgreementTest, HksAgreementTest002, Level1)
{
     /* 1. generate key */
    struct HksBlob *privateKey = NULL;
    struct HksBlob *peerPubKeyAlias = NULL;
    struct HksBlob *peerPublicKey = NULL;
    struct HksBlob *privateKey1 = NULL;
    struct HksBlob *peerPubKeyAlias1 = NULL;
    struct HksBlob *peerPublicKey1 = NULL;
    int32_t ret;

    if (g_testAgreeParams[0].genKeyParamSetParams.setKeyStorageFlag &&
        (g_testAgreeParams[0].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) {
        ret = GenerateLocalX25519Key(&privateKey, &peerPublicKey, &g_testAgreeParams[0].localPrivateKeyParams, \
            &g_testAgreeParams[0].localPublicKeyParams);
        TEST_ASSERT_TRUE(ret == 0);
        ret = GenerateLocalX25519Key(&privateKey1, &peerPublicKey1, &g_testAgreeParams[0].localPrivateKeyParams, \
            &g_testAgreeParams[0].localPublicKeyParams);
        TEST_ASSERT_TRUE(ret == 0);
    }
    /* 2. agreeKey */
    struct HksBlob *agreeKey = NULL;
    ret = TestConstuctBlob(&agreeKey, g_testAgreeParams[0].agreedKeyParams.blobExist, \
        g_testAgreeParams[0].agreedKeyParams.blobSize, g_testAgreeParams[0].agreedKeyParams.blobDataExist, \
        g_testAgreeParams[0].agreedKeyParams.blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);

    ret = AgreeKey(&g_testAgreeParams[0].agreeParamSetParams, privateKey, peerPublicKey1, agreeKey);
    TEST_ASSERT_TRUE(ret == g_testAgreeParams[0].expectResult);

    struct HksBlob *agreeKey1 = NULL;
    ret = TestConstuctBlob(&agreeKey1, g_testAgreeParams[0].agreedKeyParams.blobExist, \
        g_testAgreeParams[0].agreedKeyParams.blobSize, g_testAgreeParams[0].agreedKeyParams.blobDataExist, \
        g_testAgreeParams[0].agreedKeyParams.blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);

    ret = AgreeKey(&g_testAgreeParams[0].agreeParamSetParams, privateKey1, peerPublicKey, agreeKey1);
    TEST_ASSERT_TRUE(ret == g_testAgreeParams[0].expectResult);
    TEST_ASSERT_EQUAL(agreeKey->size, agreeKey1->size);
    TEST_ASSERT_TRUE(memcmp(agreeKey->data, agreeKey1->data, agreeKey->size) == 0);

    /* 3. delete key */
    if (!(g_testAgreeParams[0].genKeyParamSetParams.setKeyStorageFlag &&
        (g_testAgreeParams[0].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) &&
        ((g_testAgreeParams[0].keyAlias1Params.blobExist) && (g_testAgreeParams[0].keyAlias2Params.blobExist))) {
        TEST_ASSERT_TRUE(HksDeleteKey(privateKey, NULL) == 0);
        TEST_ASSERT_TRUE(HksDeleteKey(peerPubKeyAlias, NULL) == 0);
        TEST_ASSERT_TRUE(HksDeleteKey(privateKey1, NULL) == 0);
        TEST_ASSERT_TRUE(HksDeleteKey(peerPubKeyAlias1, NULL) == 0);
    }
    TestFreeBlob(&privateKey);
    TestFreeBlob(&peerPubKeyAlias);
    TestFreeBlob(&peerPublicKey);
    TestFreeBlob(&agreeKey);
    TestFreeBlob(&privateKey1);
    TestFreeBlob(&peerPubKeyAlias1);
    TestFreeBlob(&peerPublicKey1);
    TestFreeBlob(&agreeKey1);
}

RUN_TEST_SUITE(HksAgreementTest);
#endif /* _CUT_AUTHENTICATE_ */

