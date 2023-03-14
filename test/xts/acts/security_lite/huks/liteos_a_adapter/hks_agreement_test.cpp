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

#include <gtest/gtest.h>

#include "hks_agreement_test.h"

#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"
#include "hks_type.h"

using namespace testing::ext;
namespace {
#ifndef _CUT_AUTHENTICATE_

class HksAgreementTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksAgreementTest::SetUpTestCase(void)
{
}

void HksAgreementTest::TearDownTestCase(void)
{
}

void HksAgreementTest::SetUp()
{
    EXPECT_EQ(HksInitialize(), 0);
}

void HksAgreementTest::TearDown()
{
}

const int TMP_SIZE = 512;
const int X25519_KEY_SIZE = 32;
const struct HksTestAgreeParams g_testAgreeParams[] = {
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
HWTEST_F(HksAgreementTest, HksAgreementTest001, TestSize.Level1)
{
     /* 1. generate key */
    struct HksBlob *privateKey = NULL;
    struct HksBlob *peerPubKeyAlias = NULL;
    struct HksBlob *peerPublicKey = NULL;
    int32_t ret;

    if (g_testAgreeParams[0].genKeyParamSetParams.setKeyStorageFlag &&
        (g_testAgreeParams[0].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) {
        ret = GenerateLocalX25519Key(&privateKey, NULL, &g_testAgreeParams[0].localPrivateKeyParams, NULL);
        ASSERT_TRUE(ret == 0);
        ret = GenerateLocalX25519Key(NULL, &peerPublicKey, NULL, &g_testAgreeParams[0].localPublicKeyParams);
        ASSERT_TRUE(ret == 0);
    }
    /* 2. agreeKey */
    struct HksBlob *agreeKey = NULL;
    ret = TestConstuctBlob(&agreeKey,
        g_testAgreeParams[0].agreedKeyParams.blobExist,
        g_testAgreeParams[0].agreedKeyParams.blobSize,
        g_testAgreeParams[0].agreedKeyParams.blobDataExist,
        g_testAgreeParams[0].agreedKeyParams.blobDataSize);
    ASSERT_TRUE(ret == 0);

    ret = AgreeKey(&g_testAgreeParams[0].agreeParamSetParams, privateKey, peerPublicKey, agreeKey);
    ASSERT_TRUE(ret == g_testAgreeParams[0].expectResult);

    /* 3. delete key */
    if (!(g_testAgreeParams[0].genKeyParamSetParams.setKeyStorageFlag &&
        (g_testAgreeParams[0].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) &&
        ((g_testAgreeParams[0].keyAlias1Params.blobExist) &&
        (g_testAgreeParams[0].keyAlias2Params.blobExist))) {
        ret = HksDeleteKey(privateKey, NULL);
        ASSERT_TRUE(ret == 0);
        ret = HksDeleteKey(peerPubKeyAlias, NULL);
        ASSERT_TRUE(ret == 0);
    }
    TestFreeBlob(&privateKey);
    TestFreeBlob(&peerPubKeyAlias);
    TestFreeBlob(&peerPublicKey);
    TestFreeBlob(&agreeKey);
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: HksDeleteTest.HksDeleteTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksAgreementTest, HksAgreementTest002, TestSize.Level1)
{
     /* 1. generate key */
    struct HksBlob *privateKey = NULL;
    struct HksBlob *peerPubKeyAlias = NULL;
    struct HksBlob *peerPublicKey = NULL;
    struct HksBlob *privateKey2 = NULL;
    struct HksBlob *peerPublicKey2 = NULL;
    int32_t ret;

    if (g_testAgreeParams[0].genKeyParamSetParams.setKeyStorageFlag &&
        (g_testAgreeParams[0].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) {
        ret = GenerateLocalX25519Key(&privateKey, &peerPublicKey, &g_testAgreeParams[0].localPrivateKeyParams, \
            &g_testAgreeParams[0].localPublicKeyParams);
        ASSERT_TRUE(ret == 0);
        ret = GenerateLocalX25519Key(&privateKey2, &peerPublicKey2, &g_testAgreeParams[0].localPrivateKeyParams, \
            &g_testAgreeParams[0].localPublicKeyParams);
        ASSERT_TRUE(ret == 0);
    }
    /* 2. agreeKey */
    struct HksBlob *agreeKey = NULL;
    ret = TestConstuctBlob(&agreeKey, g_testAgreeParams[0].agreedKeyParams.blobExist, \
        g_testAgreeParams[0].agreedKeyParams.blobSize, g_testAgreeParams[0].agreedKeyParams.blobDataExist, \
        g_testAgreeParams[0].agreedKeyParams.blobDataSize);
    ASSERT_TRUE(ret == 0);

    struct HksBlob *agreeKey2 = NULL;
    ret = TestConstuctBlob(&agreeKey2, g_testAgreeParams[0].agreedKeyParams.blobExist, \
        g_testAgreeParams[0].agreedKeyParams.blobSize, g_testAgreeParams[0].agreedKeyParams.blobDataExist, \
        g_testAgreeParams[0].agreedKeyParams.blobDataSize);
    ASSERT_TRUE(ret == 0);

    ret = AgreeKey(&g_testAgreeParams[0].agreeParamSetParams, privateKey, peerPublicKey2, agreeKey);
    ASSERT_TRUE(ret == g_testAgreeParams[0].expectResult);

    ret = AgreeKey(&g_testAgreeParams[0].agreeParamSetParams, privateKey2, peerPublicKey, agreeKey2);
    ASSERT_TRUE(ret == g_testAgreeParams[0].expectResult);
    EXPECT_EQ(agreeKey->size, agreeKey2->size);
    EXPECT_EQ(memcmp(agreeKey->data, agreeKey2->data, agreeKey->size), 0);

    /* 3. delete key */
    if (!(g_testAgreeParams[0].genKeyParamSetParams.setKeyStorageFlag && \
        (g_testAgreeParams[0].genKeyParamSetParams.keyStorageFlag == HKS_STORAGE_TEMP)) && \
        ((g_testAgreeParams[0].keyAlias1Params.blobExist) && (g_testAgreeParams[0].keyAlias2Params.blobExist))) {
        ASSERT_TRUE(HksDeleteKey(privateKey, NULL) == 0);
        ASSERT_TRUE(HksDeleteKey(peerPubKeyAlias, NULL) == 0);
    }
    TestFreeBlob(&privateKey);
    TestFreeBlob(&peerPubKeyAlias);
    TestFreeBlob(&peerPublicKey);
    TestFreeBlob(&agreeKey);
}


#endif /* _CUT_AUTHENTICATE_ */
}