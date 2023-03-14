/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "hks_rsa_common_mt.h"

#include <gtest/gtest.h>
#include <securec.h>

#include "hks_api.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "openssl_rsa_helper.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace MT {
namespace {
const int SET_SIZE_4096 = 4096;
}

void HksRsaCommonMt::GenerateKeyTestCase(const GenerateKeyCaseParams &testCaseParams)
{
    struct HksParamSet *paramInSet = NULL;
    HksInitParamSet(&paramInSet);
    ASSERT_NE(paramInSet, nullptr);

    struct HksParamSet *paramSetOut = (struct HksParamSet *)HksMalloc(SET_SIZE_4096);
    ASSERT_NE(paramSetOut, nullptr);
    (void)memset_s(paramSetOut, SET_SIZE_4096, 0, SET_SIZE_4096);
    paramSetOut->paramSetSize = SET_SIZE_4096;

    EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);
    EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);
    EXPECT_EQ(HksGenerateKey(NULL, paramInSet, paramSetOut), testCaseParams.generateKeyResult);

    HksParam *pubKeyExport = NULL;
    EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubKeyExport), HKS_SUCCESS);
    HksBlob publicKey = { .size = pubKeyExport->blob.size, .data = (uint8_t *)HksMalloc(pubKeyExport->blob.size) };
    ASSERT_NE(publicKey.data, nullptr);
    (void)memcpy_s(publicKey.data, pubKeyExport->blob.size, pubKeyExport->blob.data, pubKeyExport->blob.size);

    HksParam *priKeyExport = NULL;
    EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priKeyExport), HKS_SUCCESS);
    HksBlob privateKey = { .size = priKeyExport->blob.size, .data = (uint8_t *)HksMalloc(priKeyExport->blob.size) };
    ASSERT_NE(privateKey.data, nullptr);
    (void)memcpy_s(privateKey.data, priKeyExport->blob.size, priKeyExport->blob.data, priKeyExport->blob.size);

    HksBlob plainText = {
        .size = (uint32_t)testCaseParams.hexData.length(),
        .data = (uint8_t *)&testCaseParams.hexData[0]
    };

    HksParam *cipherLenBit = NULL;
    HksGetParam(paramInSet, HKS_TAG_KEY_SIZE, &cipherLenBit);
    uint32_t inLength = (cipherLenBit->uint32Param) / BIT_NUM_OF_UINT8;
    HksBlob cipherText = { .size = inLength, .data = (uint8_t *)HksMalloc(inLength) };
    ASSERT_NE(cipherText.data, nullptr);

    EXPECT_EQ(EncryptRsa(&plainText, &cipherText, &publicKey, testCaseParams.padding, testCaseParams.keyDigest),
        testCaseParams.encryptResult);

    if (testCaseParams.encryptResult == HKS_SUCCESS) {
        HksBlob decryptedText = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
        ASSERT_NE(decryptedText.data, nullptr);

        EXPECT_EQ(
            DecryptRsa(&cipherText, &decryptedText, &privateKey, testCaseParams.padding, testCaseParams.keyDigest),
            testCaseParams.decryptResult);
        EXPECT_EQ((memcmp(plainText.data, decryptedText.data, decryptedText.size)), 0);

        HksFree(decryptedText.data);
    }

    HksFree(publicKey.data);
    HksFree(privateKey.data);
    HksFree(paramSetOut);
    HksFree(cipherText.data);
    HksFreeParamSet(&paramInSet);
}

void HksRsaCommonMt::EncryptLocalTestCase(const EncryptLocalCaseParams &testCaseParams)
{
    struct HksParamSet *paramInSet = NULL;
    HksInitParamSet(&paramInSet);
    ASSERT_NE(paramInSet, nullptr);

    struct HksParamSet *paramSetOut = (struct HksParamSet *)HksMalloc(SET_SIZE_4096);
    ASSERT_NE(paramSetOut, nullptr);
    (void)memset_s(paramSetOut, SET_SIZE_4096, 0, SET_SIZE_4096);
    paramSetOut->paramSetSize = SET_SIZE_4096;

    EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

    EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

    EXPECT_EQ(HksGenerateKey(NULL, paramInSet, paramSetOut), HKS_SUCCESS);

    HksParam *pubKeyExport = NULL;
    EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubKeyExport), HKS_SUCCESS);

    HksBlob publicKey = { .size = pubKeyExport->blob.size, .data = (uint8_t *)HksMalloc(pubKeyExport->blob.size) };
    ASSERT_NE(publicKey.data, nullptr);
    (void)memcpy_s(publicKey.data, pubKeyExport->blob.size, pubKeyExport->blob.data, pubKeyExport->blob.size);

    HksParam *priKeyExport = NULL;
    EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priKeyExport), HKS_SUCCESS);

    HksBlob privateKey = { .size = priKeyExport->blob.size, .data = (uint8_t *)HksMalloc(priKeyExport->blob.size) };
    ASSERT_NE(privateKey.data, nullptr);
    (void)memcpy_s(privateKey.data, priKeyExport->blob.size, priKeyExport->blob.data, priKeyExport->blob.size);

    HksBlob plainText = {
        .size = (uint32_t)testCaseParams.hexData.length(),
        .data = (uint8_t *)&testCaseParams.hexData[0]
    };

    HksParam *cipherLenBit = NULL;
    HksGetParam(paramInSet, HKS_TAG_KEY_SIZE, &cipherLenBit);
    uint32_t inLength = (cipherLenBit->uint32Param) / BIT_NUM_OF_UINT8;
    HksBlob cipherText = { .size = inLength, .data = (uint8_t *)HksMalloc(inLength) };
    ASSERT_NE(cipherText.data, nullptr);

    EXPECT_EQ(HksEncrypt(&publicKey, paramInSet, &plainText, &cipherText), testCaseParams.encryptResult);

    if (testCaseParams.encryptResult == HKS_SUCCESS) {
        HksBlob decryptedText = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
        ASSERT_NE(decryptedText.data, nullptr);

        EXPECT_EQ(
            DecryptRsa(&cipherText, &decryptedText, &privateKey, testCaseParams.padding, testCaseParams.keyDigest),
            testCaseParams.decryptResult);

        EXPECT_EQ((memcmp(plainText.data, decryptedText.data, decryptedText.size)), 0);

        HksFree(decryptedText.data);
    }
    HksFree(paramSetOut);
    HksFree(publicKey.data);
    HksFree(privateKey.data);
    HksFree(cipherText.data);
    HksFreeParamSet(&paramInSet);
}

void HksRsaCommonMt::EncryptServiceTestCase(const EncryptServiceCaseParams &testCaseParams)
{
    struct HksBlob authId = {
        .size = (uint32_t)testCaseParams.alias.length(),
        .data = (uint8_t *)&testCaseParams.alias[0]
    };

    struct HksParamSet *paramInSet = NULL;
    HksInitParamSet(&paramInSet);
    ASSERT_NE(paramInSet, nullptr);

    EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

    EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

    HksBlob plainText = {
        .size = (uint32_t)testCaseParams.hexData.length(),
        .data = (uint8_t *)&testCaseParams.hexData[0]
    };

    HksParam *cipherLenBit = NULL;
    HksGetParam(paramInSet, HKS_TAG_KEY_SIZE, &cipherLenBit);
    uint32_t inLen = (cipherLenBit->uint32Param) / BIT_NUM_OF_UINT8;
    HksBlob cipherText = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
    ASSERT_NE(cipherText.data, nullptr);

    struct HksBlob opensslRsaKeyInfo = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
    ASSERT_NE(opensslRsaKeyInfo.data, nullptr);

    struct HksBlob x509Key = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
    ASSERT_NE(x509Key.data, nullptr);

    EVP_PKEY *pkey = GenerateRsaKey(testCaseParams.keySize);
    ASSERT_NE(pkey, nullptr);

    OpensslGetx509PubKey(pkey, &x509Key);

    EXPECT_EQ(HksImportKey(&authId, paramInSet, &x509Key), HKS_SUCCESS);

    SaveRsaKeyToHksBlob(pkey, testCaseParams.keySize, &opensslRsaKeyInfo);

    EXPECT_EQ(HksEncrypt(&authId, paramInSet, &plainText, &cipherText), testCaseParams.encryptResult);
    if (testCaseParams.encryptResult == HKS_SUCCESS) {
        HksBlob decryptedText = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
        ASSERT_NE(decryptedText.data, nullptr);

        EXPECT_EQ(
            DecryptRsa(
                &cipherText, &decryptedText, &opensslRsaKeyInfo, testCaseParams.padding, testCaseParams.keyDigest),
            testCaseParams.decryptResult);

        EXPECT_EQ((memcmp(plainText.data, decryptedText.data, decryptedText.size)), 0);

        HksFree(decryptedText.data);
    }

    EVP_PKEY_free(pkey);
    HksFree(cipherText.data);
    HksFree(x509Key.data);
    HksFree(opensslRsaKeyInfo.data);
    HksFreeParamSet(&paramInSet);
}

void HksRsaCommonMt::DecryptLocalTestCase(const DecryptLocalCaseParams &testCaseParams)
{
    struct HksParamSet *paramInSet = NULL;
    HksInitParamSet(&paramInSet);
    ASSERT_NE(paramInSet, nullptr);

    struct HksParamSet *paramSetOut = (struct HksParamSet *)HksMalloc(SET_SIZE_4096);
    ASSERT_NE(paramSetOut, nullptr);
    (void)memset_s(paramSetOut, SET_SIZE_4096, 0, SET_SIZE_4096);
    paramSetOut->paramSetSize = SET_SIZE_4096;

    EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

    EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

    EXPECT_EQ(HksGenerateKey(NULL, paramInSet, paramSetOut), HKS_SUCCESS);

    HksParam *pubKeyExport = NULL;
    EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubKeyExport), HKS_SUCCESS);

    HksBlob publicKey = { .size = pubKeyExport->blob.size, .data = (uint8_t *)HksMalloc(pubKeyExport->blob.size) };
    ASSERT_NE(publicKey.data, nullptr);
    (void)memcpy_s(publicKey.data, pubKeyExport->blob.size, pubKeyExport->blob.data, pubKeyExport->blob.size);

    HksParam *priKeyExport = NULL;
    EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priKeyExport), HKS_SUCCESS);

    HksBlob privateKey = { .size = priKeyExport->blob.size, .data = (uint8_t *)HksMalloc(priKeyExport->blob.size) };
    ASSERT_NE(privateKey.data, nullptr);
    (void)memcpy_s(privateKey.data, priKeyExport->blob.size, priKeyExport->blob.data, priKeyExport->blob.size);

    HksBlob plainText = {
        .size = (uint32_t)testCaseParams.hexData.length(),
        .data = (uint8_t *)&testCaseParams.hexData[0]
    };

    HksParam *cipherLenBit = NULL;
    HksGetParam(paramInSet, HKS_TAG_KEY_SIZE, &cipherLenBit);
    uint32_t inLength = (cipherLenBit->uint32Param) / BIT_NUM_OF_UINT8;
    HksBlob cipherText = { .size = inLength, .data = (uint8_t *)HksMalloc(inLength) };
    ASSERT_NE(cipherText.data, nullptr);

    EXPECT_EQ(EncryptRsa(&plainText, &cipherText, &publicKey, testCaseParams.padding, testCaseParams.keyDigest),
        testCaseParams.encryptResult);

    if (testCaseParams.encryptResult == HKS_SUCCESS) {
        HksBlob decryptedText = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
        ASSERT_NE(decryptedText.data, nullptr);
        EXPECT_EQ(HksDecrypt(&privateKey, paramInSet, &cipherText, &decryptedText), testCaseParams.decryptResult);
        if (testCaseParams.decryptResult == HKS_SUCCESS) {
            EXPECT_EQ((memcmp(plainText.data, decryptedText.data, decryptedText.size)), 0);
        }
        HksFree(decryptedText.data);
    }
    HksFree(paramSetOut);
    HksFree(publicKey.data);
    HksFree(privateKey.data);
    HksFree(cipherText.data);
    HksFreeParamSet(&paramInSet);
}

void HksRsaCommonMt::DecryptServiceTestCase(const DecryptServiceCaseParams &testCaseParams)
{
    struct HksBlob authId = {
        .size = (uint32_t)testCaseParams.alias.length(),
        .data = (uint8_t *)&testCaseParams.alias[0]
    };
    struct HksParamSet *paramInSet = NULL;
    HksInitParamSet(&paramInSet);
    ASSERT_NE(paramInSet, nullptr);

    struct HksParamSet *paramSetOut = (struct HksParamSet *)HksMalloc(SET_SIZE_4096);
    ASSERT_NE(paramSetOut, nullptr);
    (void)memset_s(paramSetOut, SET_SIZE_4096, 0, SET_SIZE_4096);
    paramSetOut->paramSetSize = SET_SIZE_4096;

    EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);
    EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);
    EXPECT_EQ(HksGenerateKey(&authId, paramInSet, paramSetOut), HKS_SUCCESS);
    uint8_t opensslRsaKey[SET_SIZE_4096] = {0};
    uint32_t opensslRsaKeyLen = SET_SIZE_4096;
    struct HksBlob opensslRsaKeyInfo = { opensslRsaKeyLen, opensslRsaKey };
    EXPECT_EQ(HksExportPublicKey(&authId, paramInSet, &opensslRsaKeyInfo), HKS_SUCCESS);

    uint8_t rsaPublicKey[SET_SIZE_4096] = {0};
    uint32_t rsaPublicKeyLen = SET_SIZE_4096;
    struct HksBlob rsaPublicKeyInfo = { rsaPublicKeyLen, rsaPublicKey };
    EXPECT_EQ(X509ToRsaPublicKey(&opensslRsaKeyInfo, &rsaPublicKeyInfo), 0);
    HksBlob publicKey = { .size = rsaPublicKeyInfo.size, .data = (uint8_t *)HksMalloc(rsaPublicKeyInfo.size) };
    ASSERT_NE(publicKey.data, nullptr);
    ASSERT_EQ(memcpy_s(publicKey.data, publicKey.size, rsaPublicKeyInfo.data, rsaPublicKeyInfo.size), 0);

    HksBlob plainText = {
        .size = (uint32_t)testCaseParams.hexData.length(),
        .data = (uint8_t *)&testCaseParams.hexData[0]
    };
    HksParam *cipherLenBit = NULL;
    HksGetParam(paramInSet, HKS_TAG_KEY_SIZE, &cipherLenBit);
    uint32_t inLength = (cipherLenBit->uint32Param) / BIT_NUM_OF_UINT8;
    HksBlob cipherText = { .size = inLength, .data = (uint8_t *)HksMalloc(inLength) };
    ASSERT_NE(cipherText.data, nullptr);
    EXPECT_EQ(EncryptRsa(&plainText, &cipherText, &publicKey, testCaseParams.padding, testCaseParams.keyDigest),
        testCaseParams.encryptResult);

    if (testCaseParams.encryptResult == HKS_SUCCESS) {
        HksBlob decryptedText = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
        ASSERT_NE(decryptedText.data, nullptr);
        EXPECT_EQ(HksDecrypt(&authId, paramInSet, &cipherText, &decryptedText), testCaseParams.decryptResult);

        if (testCaseParams.decryptResult == HKS_SUCCESS) {
            EXPECT_EQ((memcmp(plainText.data, decryptedText.data, decryptedText.size)), 0);
        }
        HksFree(decryptedText.data);
    }

    HksFree(paramSetOut);
    HksFree(publicKey.data);
    HksFree(cipherText.data);
    HksFreeParamSet(&paramInSet);
}

void HksRsaCommonMt::SignLocalTestCase(const SignLocalCaseParams &testCaseParams)
{
    struct HksParamSet *paramInSet = NULL;
    HksInitParamSet(&paramInSet);
    ASSERT_NE(paramInSet, nullptr);

    struct HksParamSet *paramSetOut = (struct HksParamSet *)HksMalloc(SET_SIZE_4096);
    ASSERT_NE(paramSetOut, nullptr);
    (void)memset_s(paramSetOut, SET_SIZE_4096, 0, SET_SIZE_4096);
    paramSetOut->paramSetSize = SET_SIZE_4096;

    EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

    EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

    EXPECT_EQ(HksGenerateKey(NULL, paramInSet, paramSetOut), HKS_SUCCESS);

    HksParam *priKeyExport = NULL;
    EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priKeyExport), HKS_SUCCESS);

    HksBlob privateKey = { .size = priKeyExport->blob.size, .data = (uint8_t *)HksMalloc(priKeyExport->blob.size) };
    ASSERT_NE(privateKey.data, nullptr);
    (void)memcpy_s(privateKey.data, priKeyExport->blob.size, priKeyExport->blob.data, priKeyExport->blob.size);

    const char *hexData = "00112233445566778899aabbccddeeff";

    HksBlob plainText = { .size = (uint32_t)strlen(hexData), .data = (uint8_t *)hexData };

    HksBlob signData = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
    ASSERT_NE(signData.data, nullptr);

    EXPECT_EQ(HksSign(&privateKey, paramInSet, &plainText, &signData), testCaseParams.signResult);
    if (testCaseParams.signResult == HKS_SUCCESS) {
        HksParam *pubKeyExport = NULL;
        EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubKeyExport), HKS_SUCCESS);

        HksBlob publicKey = { .size = pubKeyExport->blob.size, .data = (uint8_t *)HksMalloc(pubKeyExport->blob.size) };
        ASSERT_NE(publicKey.data, nullptr);
        (void)memcpy_s(publicKey.data, pubKeyExport->blob.size, pubKeyExport->blob.data, pubKeyExport->blob.size);

        EXPECT_EQ(OpensslVerifyRsa(&plainText, &signData, &publicKey, testCaseParams.padding, testCaseParams.keyDigest),
            testCaseParams.verifyResult);
        HksFree(publicKey.data);
    }

    HksFree(paramSetOut);
    HksFree(privateKey.data);
    HksFree(signData.data);
    HksFreeParamSet(&paramInSet);
}

void HksRsaCommonMt::SignServiceTestCase(const SignServiceCaseParams &testCaseParams)
{
    struct HksBlob authId = {
        .size = (uint32_t)testCaseParams.alias.length(),
        .data = (uint8_t *)&testCaseParams.alias[0]
    };
    struct HksParamSet *paramInSet = NULL;
    HksInitParamSet(&paramInSet);
    ASSERT_NE(paramInSet, nullptr);

    struct HksParamSet *paramSetOut = (struct HksParamSet *)HksMalloc(SET_SIZE_4096);
    ASSERT_NE(paramSetOut, nullptr);
    (void)memset_s(paramSetOut, SET_SIZE_4096, 0, SET_SIZE_4096);
    paramSetOut->paramSetSize = SET_SIZE_4096;

    EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

    EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

    EXPECT_EQ(HksGenerateKey(&authId, paramInSet, paramSetOut), HKS_SUCCESS);

    const char *hexData = "00112233445566778899aabbccddeeff";

    HksBlob plainText = { .size = (uint32_t)strlen(hexData), .data = (uint8_t *)hexData };

    HksBlob signData = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
    ASSERT_NE(signData.data, nullptr);

    EXPECT_EQ(HksSign(&authId, paramInSet, &plainText, &signData), testCaseParams.signResult);
    if (testCaseParams.signResult == HKS_SUCCESS) {
        uint8_t opensslRsaKey[SET_SIZE_4096] = {0};
        uint32_t opensslRsaKeyLen = SET_SIZE_4096;
        struct HksBlob opensslRsaKeyInfo = { opensslRsaKeyLen, opensslRsaKey };
        EXPECT_EQ(HksExportPublicKey(&authId, paramInSet, &opensslRsaKeyInfo), HKS_SUCCESS);

        uint8_t rsaPublicKey[SET_SIZE_4096] = {0};
        uint32_t rsaPublicKeyLen = SET_SIZE_4096;
        struct HksBlob rsaPublicKeyInfo = { rsaPublicKeyLen, rsaPublicKey };
        EXPECT_EQ(X509ToRsaPublicKey(&opensslRsaKeyInfo, &rsaPublicKeyInfo), 0);

        HksBlob publicKey = { .size = rsaPublicKeyInfo.size, .data = (uint8_t *)HksMalloc(rsaPublicKeyInfo.size) };
        ASSERT_NE(publicKey.data, nullptr);
        ASSERT_EQ(memcpy_s(publicKey.data, publicKey.size, rsaPublicKeyInfo.data, rsaPublicKeyInfo.size), 0);

        EXPECT_EQ(OpensslVerifyRsa(&plainText, &signData, &publicKey, testCaseParams.padding, testCaseParams.keyDigest),
            testCaseParams.verifyResult);
        HksFree(publicKey.data);
    }
    HksFree(paramSetOut);
    HksFree(signData.data);
    HksFreeParamSet(&paramInSet);
}

void HksRsaCommonMt::VerifyLocalTestCase(const VerifyLocalCaseParams &testCaseParams)
{
    struct HksParamSet *paramInSet = NULL;
    HksInitParamSet(&paramInSet);
    ASSERT_NE(paramInSet, nullptr);

    struct HksParamSet *paramSetOut = (struct HksParamSet *)HksMalloc(SET_SIZE_4096);
    ASSERT_NE(paramSetOut, nullptr);
    (void)memset_s(paramSetOut, SET_SIZE_4096, 0, SET_SIZE_4096);
    paramSetOut->paramSetSize = SET_SIZE_4096;

    EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

    EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

    EXPECT_EQ(HksGenerateKey(NULL, paramInSet, paramSetOut), HKS_SUCCESS);

    HksParam *priKeyExport = NULL;
    EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priKeyExport), HKS_SUCCESS);

    HksBlob privateKey = { .size = priKeyExport->blob.size, .data = (uint8_t *)HksMalloc(priKeyExport->blob.size) };
    ASSERT_NE(privateKey.data, nullptr);
    (void)memcpy_s(privateKey.data, priKeyExport->blob.size, priKeyExport->blob.data, priKeyExport->blob.size);

    const char *hexData = "00112233445566778899aabbccddeeff";

    HksBlob plainText = { .size = (uint32_t)strlen(hexData), .data = (uint8_t *)hexData };

    HksBlob signData = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
    ASSERT_NE(signData.data, nullptr);

    EXPECT_EQ(OpensslSignRsa(&plainText, &signData, &privateKey, testCaseParams.padding, testCaseParams.keyDigest),
        testCaseParams.signResult);
    if (testCaseParams.signResult == HKS_SUCCESS) {
        HksParam *pubKeyExport = NULL;
        EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubKeyExport), HKS_SUCCESS);

        HksBlob publicKey = { .size = pubKeyExport->blob.size, .data = (uint8_t *)HksMalloc(pubKeyExport->blob.size) };
        ASSERT_NE(publicKey.data, nullptr);
        (void)memcpy_s(publicKey.data, pubKeyExport->blob.size, pubKeyExport->blob.data, pubKeyExport->blob.size);
        EXPECT_EQ(HksVerify(&publicKey, paramInSet, &plainText, &signData), testCaseParams.verifyResult);
        HksFree(publicKey.data);
    }
    HksFree(paramSetOut);
    HksFree(privateKey.data);
    HksFree(signData.data);
    HksFreeParamSet(&paramInSet);
}

void HksRsaCommonMt::VerifyServiceTestCase(const VerifyServiceCaseParams &testCaseParams)
{
    struct HksBlob authId = {
        .size = (uint32_t)testCaseParams.alias.length(),
        .data = (uint8_t *)&testCaseParams.alias[0]
    };

    struct HksParamSet *paramInSet = NULL;
    HksInitParamSet(&paramInSet);
    ASSERT_NE(paramInSet, nullptr);

    EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

    EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

    const char *hexData = "00112233445566778899aabbccddeeff";

    HksBlob plainText = { .size = (uint32_t)strlen(hexData), .data = (uint8_t *)hexData };

    HksBlob signData = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
    ASSERT_NE(signData.data, nullptr);

    struct HksBlob opensslRsaKeyInfo = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
    ASSERT_NE(opensslRsaKeyInfo.data, nullptr);

    struct HksBlob x509Key = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
    ASSERT_NE(x509Key.data, nullptr);

    EVP_PKEY *pkey = GenerateRsaKey(testCaseParams.keySize);
    ASSERT_NE(pkey, nullptr);

    OpensslGetx509PubKey(pkey, &x509Key);

    EXPECT_EQ(HksImportKey(&authId, paramInSet, &x509Key), HKS_SUCCESS);

    SaveRsaKeyToHksBlob(pkey, testCaseParams.keySize, &opensslRsaKeyInfo);
    EXPECT_EQ(
        OpensslSignRsa(&plainText, &signData, &opensslRsaKeyInfo, testCaseParams.padding, testCaseParams.keyDigest),
        testCaseParams.signResult);
    if (testCaseParams.signResult == HKS_SUCCESS) {
        EXPECT_EQ(HksVerify(&authId, paramInSet, &plainText, &signData), testCaseParams.verifyResult);
    }
    EVP_PKEY_free(pkey);
    HksFree(signData.data);
    HksFree(x509Key.data);
    HksFree(opensslRsaKeyInfo.data);
    HksFreeParamSet(&paramInSet);
}
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS
