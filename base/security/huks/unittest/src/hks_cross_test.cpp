/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "hks_cross_test.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::CrossTest {
class HksCrossTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksCrossTest::SetUpTestCase(void)
{
}

void HksCrossTest::TearDownTestCase(void)
{
}

void HksCrossTest::SetUp()
{
}

void HksCrossTest::TearDown()
{
}

/**
 * @tc.name: HksCrossTest.HksCrossTestAesCipher001
 * @tc.desc: Encrypt-oneStage Decrypt-threeStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestAesCipher001, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestAesCipher001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_aesGenParams,
        sizeof(g_aesGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *encryptParamSet = nullptr;
    struct HksParamSet *decryptParamSet = nullptr;
    ret = InitParamSet(&encryptParamSet, g_aesEncryParams, sizeof(g_aesEncryParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(en) failed.";
    ret = InitParamSet(&decryptParamSet, g_aesDecryParams, sizeof(g_aesDecryParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(de) failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t cipher[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::CrossTest::CROSS_COMMON_SIZE, cipher };
    uint8_t plain[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::CrossTest::CROSS_COMMON_SIZE, plain };

    ret = HksEncrypt(&keyAlias, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksEncrypt failed.";
    EXPECT_NE(HksMemCmp(inData.data, cipherText.data, cipherText.size), HKS_SUCCESS) << "inData equals cipherText.";

    ret = HksCrossTestAesDecrypt(&keyAlias, decryptParamSet, &cipherText, &plainText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAesDecrypt failed.";
    EXPECT_EQ(HksMemCmp(inData.data, plainText.data, plainText.size), HKS_SUCCESS) << "inData not equals plainText.";

    HksDeleteKey(&keyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
    HksFreeParamSet(&decryptParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestAesCipher002
 * @tc.desc: Encrypt-threeStage Decrypt-oneStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestAesCipher002, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestAesCipher002";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_aesGenParams,
        sizeof(g_aesGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *encryptParamSet = nullptr;
    struct HksParamSet *decryptParamSet = nullptr;
    ret = InitParamSet(&encryptParamSet, g_aesEncryParams, sizeof(g_aesEncryParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(en) failed.";
    ret = InitParamSet(&decryptParamSet, g_aesDecryParams, sizeof(g_aesDecryParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(de) failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t cipher[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::CrossTest::CROSS_COMMON_SIZE, cipher };
    uint8_t plain[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::CrossTest::CROSS_COMMON_SIZE, plain };

    ret = HksCrossTestAesEncrypt(&keyAlias, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAesEncrypt failed.";
    EXPECT_NE(HksMemCmp(inData.data, cipherText.data, cipherText.size), HKS_SUCCESS) << "inData equals cipherText.";

    ret = HksDecrypt(&keyAlias, decryptParamSet, &cipherText, &plainText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksDecrypt failed.";
    EXPECT_EQ(HksMemCmp(inData.data, plainText.data, plainText.size), HKS_SUCCESS) << "inData not equals plainText.";

    HksDeleteKey(&keyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
    HksFreeParamSet(&decryptParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestRsaCipher001
 * @tc.desc: Encrypt-oneStage Decrypt-threeStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestRsaCipher001, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestRsaCipher001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_rsaGenParams,
        sizeof(g_rsaGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *encryptParamSet = nullptr;
    struct HksParamSet *decryptParamSet = nullptr;
    ret = InitParamSet(&encryptParamSet, g_rsaEncryParams, sizeof(g_rsaEncryParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(en) failed.";
    ret = InitParamSet(&decryptParamSet, g_rsaDecryParams, sizeof(g_rsaDecryParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(de) failed.";

    char tmpKey[] = "HksCrossTestRsaCipher001_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, encryptParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData_32.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData_32.c_str() };
    uint8_t cipher[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::CrossTest::CROSS_COMMON_SIZE, cipher };
    uint8_t plain[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::CrossTest::CROSS_COMMON_SIZE, plain };

    ret = HksEncrypt(&newKeyAlias, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksEncrypt failed.";
    EXPECT_NE(HksMemCmp(inData.data, cipherText.data, cipherText.size), HKS_SUCCESS) << "inData equals cipherText.";

    ret = HksCrossTestRsaDecrypt(&keyAlias, decryptParamSet, &cipherText, &plainText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestRsaDecrypt failed.";
    EXPECT_EQ(HksMemCmp(inData.data, plainText.data, plainText.size), HKS_SUCCESS) << "inData not equals plainText.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
    HksFreeParamSet(&decryptParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestRsaCipher002
 * @tc.desc: Encrypt-threeStage Decrypt-oneStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestRsaCipher002, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestRsaCipher002";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_rsaGenParams,
        sizeof(g_rsaGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *encryptParamSet = nullptr;
    struct HksParamSet *decryptParamSet = nullptr;
    ret = InitParamSet(&encryptParamSet, g_rsaEncryParams, sizeof(g_rsaEncryParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(en) failed.";
    ret = InitParamSet(&decryptParamSet, g_rsaDecryParams, sizeof(g_rsaDecryParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(de) failed.";

    char tmpKey[] = "HksCrossTestRsaCipher002_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, encryptParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData_32.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData_32.c_str() };
    uint8_t cipher[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::CrossTest::CROSS_COMMON_SIZE, cipher };
    uint8_t plain[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::CrossTest::CROSS_COMMON_SIZE, plain };

    ret = HksCrossTestRsaEncrypt(&newKeyAlias, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestRsaEncrypt failed.";
    EXPECT_NE(HksMemCmp(inData.data, cipherText.data, cipherText.size), HKS_SUCCESS) << "inData equals cipherText.";

    ret = HksDecrypt(&keyAlias, decryptParamSet, &cipherText, &plainText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksDecrypt failed.";
    EXPECT_EQ(HksMemCmp(inData.data, plainText.data, plainText.size), HKS_SUCCESS) << "inData not equals plainText.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
    HksFreeParamSet(&decryptParamSet);
}

#ifdef _USE_OPENSSL_
/**
 * @tc.name: HksCrossTest.HksCrossTestDsaSignVerify001
 * @tc.desc: Sign-oneStage Verify-threeStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestDsaSignVerify001, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestDsaSignVerify001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_dsaSVGenParams,
        sizeof(g_dsaSVGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_dsaSVSignParams, sizeof(g_dsaSVSignParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(S) failed.";
    ret = InitParamSet(&verifyParamSet, g_dsaSVVerParams, sizeof(g_dsaSVVerParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(V) failed.";

    char tmpKey[] = "HksCrossTestDsaSignVerify001_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, verifyParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t sign[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob signedData = { Unittest::CrossTest::CROSS_COMMON_SIZE, sign };

    ret = HksSign(&keyAlias, signParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksSign failed.";

    ret = HksCrossTestSignVerify(&newKeyAlias, verifyParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestSignVerify(V) failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestDsaSignVerify002
 * @tc.desc: Sign-threeStage Verify-oneStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestDsaSignVerify002, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestDsaSignVerify002";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_dsaSVGenParams,
        sizeof(g_dsaSVGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_dsaSVSignParams, sizeof(g_dsaSVSignParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(S) failed.";
    ret = InitParamSet(&verifyParamSet, g_dsaSVVerParams, sizeof(g_dsaSVVerParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(V) failed.";

    char tmpKey[] = "HksCrossTestDsaSignVerify002_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, verifyParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t sign[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob signedData = { Unittest::CrossTest::CROSS_COMMON_SIZE, sign };

    ret = HksCrossTestSignVerify(&keyAlias, signParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestSignVerify(S) failed.";

    ret = HksVerify(&newKeyAlias, verifyParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksVerify failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}
#endif

/**
 * @tc.name: HksCrossTest.HksCrossTestEccSignVerify001
 * @tc.desc: Sign-oneStage Verify-threeStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestEccSignVerify001, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestEccSignVerify001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_eccSVGenParams,
        sizeof(g_eccSVGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_eccSVSignParams, sizeof(g_eccSVSignParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(S) failed.";
    ret = InitParamSet(&verifyParamSet, g_eccSVVerParams, sizeof(g_eccSVVerParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(V) failed.";

    char tmpKey[] = "HksCrossTestEccSignVerify001_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, verifyParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t sign[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob signedData = { Unittest::CrossTest::CROSS_COMMON_SIZE, sign };

    ret = HksSign(&keyAlias, signParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksSign failed.";

    ret = HksCrossTestSignVerify(&newKeyAlias, verifyParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestSignVerify(V) failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestEccSignVerify002
 * @tc.desc: Sign-threeStage Verify-oneStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestEccSignVerify002, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestEccSignVerify002";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_eccSVGenParams,
        sizeof(g_eccSVGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_eccSVSignParams, sizeof(g_eccSVSignParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(S) failed.";
    ret = InitParamSet(&verifyParamSet, g_eccSVVerParams, sizeof(g_eccSVVerParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(V) failed.";

    char tmpKey[] = "HksCrossTestEccSignVerify002_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, verifyParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t sign[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob signedData = { Unittest::CrossTest::CROSS_COMMON_SIZE, sign };

    ret = HksCrossTestSignVerify(&keyAlias, signParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestSignVerify(S) failed.";

    ret = HksVerify(&newKeyAlias, verifyParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksVerify failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestEd25519SignVerify001
 * @tc.desc: Sign-oneStage Verify-threeStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestEd25519SignVerify001, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestEd25519SignVerify001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_ed25519SVGenParams,
        sizeof(g_ed25519SVGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_ed25519SVSignParams, sizeof(g_ed25519SVSignParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(S) failed.";
    ret = InitParamSet(&verifyParamSet, g_ed25519SVVerParams, sizeof(g_ed25519SVVerParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(V) failed.";

    char tmpKey[] = "HksCrossTestEd25519SignVerify001_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, verifyParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t sign[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob signedData = { Unittest::CrossTest::CROSS_COMMON_SIZE, sign };

    ret = HksSign(&keyAlias, signParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksSign failed.";

    ret = HksCrossTestSignVerify(&newKeyAlias, verifyParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestSignVerify(V) failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestEd25519SignVerify002
 * @tc.desc: Sign-threeStage Verify-oneStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestEd25519SignVerify002, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestEd25519SignVerify002";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_ed25519SVGenParams,
        sizeof(g_ed25519SVGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_ed25519SVSignParams, sizeof(g_ed25519SVSignParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(S) failed.";
    ret = InitParamSet(&verifyParamSet, g_ed25519SVVerParams, sizeof(g_ed25519SVVerParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(V) failed.";

    char tmpKey[] = "HksCrossTestEd25519SignVerify002_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, verifyParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t sign[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob signedData = { Unittest::CrossTest::CROSS_COMMON_SIZE, sign };

    ret = HksCrossTestSignVerify(&keyAlias, signParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestSignVerify(S) failed.";

    ret = HksVerify(&newKeyAlias, verifyParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksVerify failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestRsaSignVerify001
 * @tc.desc: Sign-oneStage Verify-threeStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestRsaSignVerify001, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestRsaSignVerify001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_rsaSVGenParams,
        sizeof(g_rsaSVGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_rsaSVSignParams, sizeof(g_rsaSVSignParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(S) failed.";
    ret = InitParamSet(&verifyParamSet, g_rsaSVVerParams, sizeof(g_rsaSVVerParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(V) failed.";

    char tmpKey[] = "HksCrossTestRsaSignVerify001_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, verifyParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t sign[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob signedData = { Unittest::CrossTest::CROSS_COMMON_SIZE, sign };

    ret = HksSign(&keyAlias, signParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksSign failed.";

    ret = HksCrossTestSignVerify(&newKeyAlias, verifyParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestSignVerify(V) failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestRsaSignVerify002
 * @tc.desc: Sign-threeStage Verify-oneStage
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestRsaSignVerify002, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestRsaSignVerify002";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_rsaSVGenParams,
        sizeof(g_rsaSVGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_rsaSVSignParams, sizeof(g_rsaSVSignParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(S) failed.";
    ret = InitParamSet(&verifyParamSet, g_rsaSVVerParams, sizeof(g_rsaSVVerParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(V) failed.";

    char tmpKey[] = "HksCrossTestRsaSignVerify002_new";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksCrossTestImportKey(&keyAlias, genParamSet, verifyParamSet, &newKeyAlias);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestImportKey failed.";

    struct HksBlob inData = { Unittest::CrossTest::g_inData.length(),
                              (uint8_t *)Unittest::CrossTest::g_inData.c_str() };
    uint8_t sign[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob signedData = { Unittest::CrossTest::CROSS_COMMON_SIZE, sign };

    ret = HksCrossTestSignVerify(&keyAlias, signParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestSignVerify(S) failed.";

    ret = HksVerify(&newKeyAlias, verifyParamSet, &inData, &signedData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksVerify failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&newKeyAlias, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestDhAgree001
 * @tc.desc: Agree
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestDhAgree001, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *finishParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    struct HksParamSet *finishParamSet02 = nullptr;
    int32_t ret = InitParamSet(&initParamSet01, g_dhInitParams01, sizeof(g_dhInitParams01)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&finishParamSet01, g_dhFinishParams01, sizeof(g_dhFinishParams01)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)01 failed.";
    ret = InitParamSet(&initParamSet02, g_dhInitParams02, sizeof(g_dhInitParams02)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";
    ret = InitParamSet(&finishParamSet02, g_dhFinishParams02, sizeof(g_dhFinishParams02)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)02 failed.";

    ret = HksCrossTestAgreeGenerate(&g_dhKeyAlias01, &g_dhKeyAlias02, genParamSet,
        g_dhGenParams, sizeof(g_dhGenParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAgreeGenerate failed.";

    uint8_t tmpPub01[HKS_DH_KEY_SIZE_4096] = {0};
    uint8_t tmpPub02[HKS_DH_KEY_SIZE_4096] = {0};
    struct HksBlob publicKey01 = { HKS_DH_KEY_SIZE_4096, tmpPub01 };
    struct HksBlob publicKey02 = { HKS_DH_KEY_SIZE_4096, tmpPub02 };
    ret = HksCrossTestAgreeExport(&g_dhKeyAlias01, &g_dhKeyAlias02, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAgreeExport failed.";

    uint8_t tmpOut01[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    uint8_t tmpOut02[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob outData01 = { Unittest::CrossTest::CROSS_COMMON_SIZE, tmpOut01 };
    struct HksBlob outData02 = { Unittest::CrossTest::CROSS_COMMON_SIZE, tmpOut02 };
    ret = HksCrossTestAgree(&g_dhKeyAlias01, &publicKey02, initParamSet01, finishParamSet01, &outData01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAgree01 failed.";
    ret = HksAgreeKey(initParamSet02, &g_dhKeyAlias02, &publicKey01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAgreeKey02 failed.";

    ret = TestCmpKeyAliasHash(&outData01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "outData01 not equals outData02";

    HksCrossTestAgreeDeleteKey(&g_dhKeyAlias01, &g_dhKeyAlias02);
    HksCrossTestAgreeFreeParamSet(genParamSet, initParamSet01, finishParamSet01, initParamSet02, finishParamSet02);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestEcdhAgree001
 * @tc.desc: Agree
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestEcdhAgree001, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *finishParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    struct HksParamSet *finishParamSet02 = nullptr;
    int32_t ret = InitParamSet(&initParamSet01, g_ecdhInitParams01, sizeof(g_ecdhInitParams01)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&finishParamSet01, g_ecdhFinishParams01, sizeof(g_ecdhFinishParams01)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)01 failed.";
    ret = InitParamSet(&initParamSet02, g_ecdhInitParams02, sizeof(g_ecdhInitParams02)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";
    ret = InitParamSet(&finishParamSet02, g_ecdhFinishParams02, sizeof(g_ecdhFinishParams02)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)02 failed.";

    ret = HksCrossTestAgreeGenerate(&g_ecdhKeyAlias01, &g_ecdhKeyAlias02, genParamSet,
        g_ecdhGenParams, sizeof(g_ecdhGenParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAgreeGenerate failed.";

    uint8_t tmpPub01[HKS_ECC_KEY_SIZE_224] = {0};
    uint8_t tmpPub02[HKS_ECC_KEY_SIZE_224] = {0};
    struct HksBlob publicKey01 = { HKS_ECC_KEY_SIZE_224, tmpPub01 };
    struct HksBlob publicKey02 = { HKS_ECC_KEY_SIZE_224, tmpPub02 };
    ret = HksCrossTestAgreeExport(&g_ecdhKeyAlias01, &g_ecdhKeyAlias02, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAgreeExport failed.";

    uint8_t tmpOut01[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    uint8_t tmpOut02[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob outData01 = { Unittest::CrossTest::CROSS_COMMON_SIZE, tmpOut01 };
    struct HksBlob outData02 = { Unittest::CrossTest::CROSS_COMMON_SIZE, tmpOut02 };
    ret = HksCrossTestAgree(&g_ecdhKeyAlias01, &publicKey02, initParamSet01, finishParamSet01, &outData01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAgree01 failed.";
    ret = HksAgreeKey(initParamSet02, &g_ecdhKeyAlias02, &publicKey01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAgreeKey02 failed.";

    ret = TestCmpKeyAliasHash(&outData01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "outData01 not equals outData02";

    HksCrossTestAgreeDeleteKey(&g_ecdhKeyAlias01, &g_ecdhKeyAlias02);
    HksCrossTestAgreeFreeParamSet(genParamSet, initParamSet01, finishParamSet01, initParamSet02, finishParamSet02);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestX25519Agree001
 * @tc.desc: Agree
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestX25519Agree001, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *finishParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    struct HksParamSet *finishParamSet02 = nullptr;
    int32_t ret = InitParamSet(&initParamSet01, g_x25519InitParams01, sizeof(g_x25519InitParams01)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&finishParamSet01, g_x25519FinishParams01, sizeof(g_x25519FinishParams01)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)01 failed.";
    ret = InitParamSet(&initParamSet02, g_x25519InitParams02, sizeof(g_x25519InitParams02)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";
    ret = InitParamSet(&finishParamSet02, g_x25519FinishParams02, sizeof(g_x25519FinishParams02)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)02 failed.";

    ret = HksCrossTestAgreeGenerate(&g_x25519KeyAlias01, &g_x25519KeyAlias02, genParamSet,
        g_x25519GenParams, sizeof(g_x25519GenParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAgreeGenerate failed.";

    uint8_t tmpPub01[HKS_CURVE25519_KEY_SIZE_256] = {0};
    uint8_t tmpPub02[HKS_CURVE25519_KEY_SIZE_256] = {0};
    struct HksBlob publicKey01 = { HKS_CURVE25519_KEY_SIZE_256, tmpPub01 };
    struct HksBlob publicKey02 = { HKS_CURVE25519_KEY_SIZE_256, tmpPub02 };
    ret = HksCrossTestAgreeExport(&g_x25519KeyAlias01, &g_x25519KeyAlias02, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAgreeExport failed.";

    uint8_t tmpOut01[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    uint8_t tmpOut02[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob outData01 = { Unittest::CrossTest::CROSS_COMMON_SIZE, tmpOut01 };
    struct HksBlob outData02 = { Unittest::CrossTest::CROSS_COMMON_SIZE, tmpOut02 };
    ret = HksCrossTestAgree(&g_x25519KeyAlias01, &publicKey02, initParamSet01, finishParamSet01, &outData01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestAgree01 failed.";
    ret = HksAgreeKey(initParamSet02, &g_x25519KeyAlias02, &publicKey01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAgreeKey02 failed.";

    ret = TestCmpKeyAliasHash(&outData01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "outData01 not equals outData02";

    HksCrossTestAgreeDeleteKey(&g_x25519KeyAlias01, &g_x25519KeyAlias02);
    HksCrossTestAgreeFreeParamSet(genParamSet, initParamSet01, finishParamSet01, initParamSet02, finishParamSet02);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestHkdfDerive001
 * @tc.desc: Derive
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestHkdfDerive001, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet = nullptr;
    struct HksParamSet *finishParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_hkdfGenParams, sizeof(g_hkdfGenParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet, g_hkdfInitParams, sizeof(g_hkdfInitParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update) failed.";
    ret = InitParamSet(&finishParamSet, g_hkdfFinishParams, sizeof(g_hkdfFinishParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish) failed.";

    char tmpKeyAlias[] = "HksCrossTestHkdfDerive001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    ret = HksCrossTestDerive(&keyAlias, genParamSet, initParamSet, finishParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestDerive failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&g_hkdfKeyAliasFinal, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&initParamSet);
    HksFreeParamSet(&finishParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestPbkdf2Derive001
 * @tc.desc: Derive
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestPbkdf2Derive001, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet = nullptr;
    struct HksParamSet *finishParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_pbkdf2GenParams, sizeof(g_pbkdf2GenParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet, g_pbkdf2InitParams, sizeof(g_pbkdf2InitParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update) failed.";
    ret = InitParamSet(&finishParamSet, g_pbkdf2FinishParams, sizeof(g_pbkdf2FinishParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish) failed.";

    struct HksParam *saltParam = nullptr;
    HksGetParam(initParamSet, HKS_TAG_SALT, &saltParam);
    ret = HksGenerateRandom(NULL, &(saltParam->blob));
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateRandom failed.";

    char tmpKeyAlias[] = "HksCrossTestPbkdf2Derive001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    ret = HksCrossTestDerive(&keyAlias, genParamSet, initParamSet, finishParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestDerive failed.";

    HksDeleteKey(&keyAlias, nullptr);
    HksDeleteKey(&g_pbkdf2KeyAliasFinal, nullptr);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&initParamSet);
    HksFreeParamSet(&finishParamSet);
}

/**
 * @tc.name: HksCrossTest.HksCrossTestHmac001
 * @tc.desc: HMAC
 * @tc.type: FUNC
 */
HWTEST_F(HksCrossTest, HksCrossTestHmac001, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksCrossTestHmac001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = HksCrossTestGenerateKey(&keyAlias, g_hmacGenParams,
        sizeof(g_hmacGenParams)/sizeof(HksParam), genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestGenerateKey failed.";

    struct HksParamSet *hmacParamSet = nullptr;
    ret = InitParamSet(&hmacParamSet, g_hmacParams, sizeof(g_hmacParams)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(HMAC) failed.";

    ret = HksCrossTestHmac(&keyAlias, hmacParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksCrossTestHmac failed.";

    HksDeleteKey(&keyAlias, genParamSet);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hmacParamSet);
}
} // namespace Unittest::CrossTest