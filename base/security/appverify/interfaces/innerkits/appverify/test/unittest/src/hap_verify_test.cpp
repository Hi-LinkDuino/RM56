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

#include "hap_verify_test.h"

#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "init/trusted_root_ca.h"
#include "interfaces/hap_verify.h"
#include "interfaces/hap_verify_result.h"

#include "hap_cert_verify_openssl_utils_test.h"
#include "test_const.h"
#include "test_hap_file_data.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;
namespace {
class HapVerifyTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapVerifyTest::SetUpTestCase(void)
{
}

void HapVerifyTest::TearDownTestCase(void)
{
}

void HapVerifyTest::SetUp()
{
}

void HapVerifyTest::TearDown()
{
}

/**
 * @tc.name: HapVerifyTest.HapVerifyEnableDebug001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, HapVerifyEnableDebug001, TestSize.Level0)
{
    ASSERT_TRUE(EnableDebugMode());
}

/**
 * @tc.name: HapVerifyTest.HapVerify001
 * @tc.desc: The static function will return verify result;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, HapVerify001, TestSize.Level0)
{
    /*
     * @tc.steps: step1. input a invalid path to function of HapVerify.
     * @tc.expected: step1. the return will be FILE_PATH_INVALID.
     */
    HapVerifyResult hapVerifyResult;
    std::string errorFile = "./signed_test.app";
    int resultCode = HapVerify(errorFile, hapVerifyResult);
    HapVerifyResultCode targetResult = FILE_PATH_INVALID;
    ASSERT_TRUE(resultCode == targetResult);
    std::ofstream appFile;
    appFile.open(errorFile.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    ASSERT_TRUE(appFile.is_open());
    appFile.close();
    resultCode = HapVerify(errorFile, hapVerifyResult);
    ASSERT_TRUE(resultCode == targetResult);
    /*
     * @tc.steps: step2. create a hapfile and run HapVerify.
     * @tc.expected: step2. the return will be SIGNATURE_NOT_FOUND.
     */
    std::string rightFile = "./signed.hap";
    std::ofstream hapFile;
    hapFile.open(rightFile.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    ASSERT_TRUE(hapFile.is_open());
    hapFile.seekp(0, std::ios_base::beg);
    hapFile.write(miniHapFile, TEST_MINI_HAP_FILE_LENGTH);
    hapFile.close();
    resultCode = HapVerify(rightFile, hapVerifyResult);
    ASSERT_TRUE(resultCode == SIGNATURE_NOT_FOUND);
    /*
     * @tc.steps: step3. create an error hapfile and run HapVerify.
     * @tc.expected: step3. the return will be SIGNATURE_NOT_FOUND.
     */
    std::string rightFile1 = "./signed1.hap";
    std::ofstream hapFile1;
    hapFile1.open(rightFile1.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    ASSERT_TRUE(hapFile1.is_open());
    hapFile1.seekp(0, std::ios_base::beg);
    hapFile1.write(miniHapFile, sizeof(miniHapFile));
    hapFile1.seekp(TEST_MINI_HAP_FILE_LENGTH - sizeof(short), std::ios_base::beg);
    hapFile1.close();
    resultCode = HapVerify(rightFile1, hapVerifyResult);
    ASSERT_TRUE(resultCode == SIGNATURE_NOT_FOUND);
    /*
     * @tc.steps: step4. use an empty file to run HapVerify.
     * @tc.expected: step4. the return will be SIGNATURE_NOT_FOUND.
     */
    std::string invalidFile = "./signed2.hap";
    std::ofstream hapFile2;
    hapFile2.open(invalidFile.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    ASSERT_TRUE(hapFile2.is_open());
    hapFile2.close();
    resultCode = HapVerify(invalidFile, hapVerifyResult);
    ASSERT_TRUE(resultCode == SIGNATURE_NOT_FOUND);
}

/**
 * @tc.name: HapVerifyTest.HapVerifyOsApp001
 * @tc.desc: The static function will return verify result of signed file;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, HapVerifyOsApp001, TestSize.Level0)
{
    /*
     * @tc.steps: step1. input a signed file to verify.
     * @tc.expected: step1. the return will be VERIFY_SUCCESS.
     */
    HapByteBuffer hapInnerApp;
    std::string filePath = "./signed_inner.hap";
    ASSERT_TRUE(Base64StringDecode(HAP_FILE_INNER, hapInnerApp));
    std::ofstream hapFile;
    hapFile.open(filePath.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    ASSERT_TRUE(hapFile.is_open());
    hapFile.seekp(0, std::ios_base::beg);
    hapFile.write(hapInnerApp.GetBufferPtr(), hapInnerApp.GetCapacity());
    hapFile.close();
    HapVerifyResult hapVerifyResult;
    int ret = HapVerify(filePath, hapVerifyResult);
    ASSERT_EQ(ret, VERIFY_SUCCESS);
}

/**
 * @tc.name: HapVerifyTest.HapVerify002
 * @tc.desc: The static function will return verify result of signed file;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyTest, HapVerify002, TestSize.Level0)
{
    std::string fileContent[TEST_FILE_NUM] = {HAP_FILE_ECC_SIGN_BASE64, HAP_FILE_RSA_SIGN_BASE64};
    std::string filePath[TEST_FILE_NUM] = {"./signed_ecc.hap", "./signed_rsa.hap"};
    std::string errorfilePath[TEST_FILE_NUM] = {"./signed_ecc_error.hap", "./signed_rsa_error.hap"};
    for (int i = 0; i < TEST_FILE_NUM; i++) {
        /*
         * @tc.steps: step1. input a signed file to verify.
         * @tc.expected: step1. the return will be VERIFY_SUCCESS.
         */
        HapByteBuffer hapFileEccSign;
        ASSERT_TRUE(Base64StringDecode(fileContent[i], hapFileEccSign));
        std::ofstream hapFile;
        hapFile.open(filePath[i].c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        ASSERT_TRUE(hapFile.is_open());
        hapFile.seekp(0, std::ios_base::beg);
        hapFile.write(hapFileEccSign.GetBufferPtr(), hapFileEccSign.GetCapacity());
        hapFile.close();
        HapVerifyResult hapVerifyResult;
        ASSERT_TRUE(HapVerify(filePath[i], hapVerifyResult) == VERIFY_SUCCESS);
        /*
         * @tc.steps: step2. check verify result.
         * @tc.expected: step2. cert version is 1, certChains Len is 3.
         */
        ProvisionInfo profile = hapVerifyResult.GetProvisionInfo();
        ASSERT_TRUE(profile.type == ProvisionType::RELEASE);
        ASSERT_TRUE(hapVerifyResult.GetVersion() == TEST_SIGN_VERSION);
        std::vector<std::string> publicKeys = hapVerifyResult.GetPublicKey();
        ASSERT_TRUE(static_cast<int>(publicKeys.size()) == TEST_CERT_CHAIN_LEN);
        std::vector<std::string> signatures = hapVerifyResult.GetSignature();
        ASSERT_TRUE(static_cast<int>(signatures.size()) == TEST_CERT_CHAIN_LEN);
        /*
         * @tc.steps: step3. change comment data.
         * @tc.expected: step3. the return will be VERIFY_INTEGRITY_FAIL.
         */
        HapByteBuffer errorCommentFile = hapFileEccSign;
        char tmp = TEST_HAPBYTEBUFFER_CHAR_DATA;
        errorCommentFile.PutByte(0, tmp);
        std::ofstream errorFile;
        errorFile.open(errorfilePath[i].c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        ASSERT_TRUE(errorFile.is_open());
        errorFile.seekp(0, std::ios_base::beg);
        errorFile.write(errorCommentFile.GetBufferPtr(), errorCommentFile.GetCapacity());
        errorFile.close();
        HapVerifyResult verifyRet;
        ASSERT_TRUE(HapVerify(errorfilePath[i], verifyRet) == VERIFY_INTEGRITY_FAIL);
        /*
         * @tc.steps: step4. change profile pkcs7 data.
         * @tc.expected: step4. the return will be APP_SOURCE_NOT_TRUSTED.
         */
        errorCommentFile.PutByte(TEST_PFOFILE_PKCS7_DATA_INDEX, tmp);
        errorFile.open(errorfilePath[i].c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        ASSERT_TRUE(errorFile.is_open());
        errorFile.seekp(0, std::ios_base::beg);
        errorFile.write(errorCommentFile.GetBufferPtr(), errorCommentFile.GetCapacity());
        errorFile.close();
        ASSERT_TRUE(HapVerify(errorfilePath[i], verifyRet) == APP_SOURCE_NOT_TRUSTED);
        /*
         * @tc.steps: step5. change app pkcs7 data.
         * @tc.expected: step5. the return will be VERIFY_APP_PKCS7_FAIL.
         */
        errorCommentFile.PutByte(TEST_APP_PKCS7_DATA_INDEX, tmp);
        errorFile.open(errorfilePath[i].c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        ASSERT_TRUE(errorFile.is_open());
        errorFile.seekp(0, std::ios_base::beg);
        errorFile.write(errorCommentFile.GetBufferPtr(), errorCommentFile.GetCapacity());
        errorFile.close();
        ASSERT_TRUE(HapVerify(errorfilePath[i], verifyRet) == VERIFY_APP_PKCS7_FAIL);
    }
}
}
