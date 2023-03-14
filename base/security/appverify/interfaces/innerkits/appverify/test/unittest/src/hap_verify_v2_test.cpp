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

#include "hap_verify_v2_test.h"

#include <string>

#include <gtest/gtest.h>

#include "init/trusted_source_manager.h"
#include "provision/provision_info.h"
#include "verify/hap_verify_v2.h"

#include "test_hap_file_data.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;

namespace {
class HapVerifyV2Test : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapVerifyV2Test::SetUpTestCase(void)
{
}

void HapVerifyV2Test::TearDownTestCase(void)
{
}

void HapVerifyV2Test::SetUp()
{
}

void HapVerifyV2Test::TearDown()
{
}

/**
 * @tc.name: Test CheckFilePath function
 * @tc.desc: The static function test whether input is a valid filepath;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyV2Test, CheckFilePathTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. input an too long filepath.
     * @tc.expected: step1. the return will be false.
     */
    HapVerifyV2 v2;
    std::string filePath = HAP_FILE_ECC_SIGN_BASE64;
    std::string standardFilePath;
    ASSERT_FALSE(v2.CheckFilePath(filePath, standardFilePath));
}

/**
 * @tc.name: Test GenerateAppId function
 * @tc.desc: The static function will return whether generate appid successfully;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyV2Test, GenerateAppIdTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. input a null ProvisionInfo.
     * @tc.expected: step1. the return will be false.
     */
    HapVerifyV2 v2;
    ProvisionInfo provisionInfo;
    ASSERT_FALSE(v2.GenerateAppId(provisionInfo));
}

/**
 * @tc.name: Test VerifyProfileInfo function
 * @tc.desc: The static function will return result of verify profile info;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyV2Test, VerifyProfileInfoTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. profile match with debug and profile type is release.
     * @tc.expected: step1. the return will be false.
     */
    HapVerifyV2 v2;
    Pkcs7Context pkcs7Context;
    Pkcs7Context profileContext;
    ProvisionInfo provisionInfo;
    profileContext.matchResult.matchState = MATCH_WITH_PROFILE_DEBUG;
    provisionInfo.type = ProvisionType::RELEASE;
    ASSERT_FALSE(v2.VerifyProfileInfo(pkcs7Context, profileContext, provisionInfo));
    /*
     * @tc.steps: step2. profile debug cert is null.
     * @tc.expected: step2. the return will be false.
     */
    provisionInfo.type = ProvisionType::DEBUG;
    CertChain test;
    pkcs7Context.certChains.push_back(test);
    X509* certX509 = HapCertVerifyOpensslUtils::GetX509CertFromPemString(ECC_TEST_CERT);
    ASSERT_TRUE(certX509 != nullptr);
    pkcs7Context.certChains[0].push_back(certX509);
    ASSERT_FALSE(v2.VerifyProfileInfo(pkcs7Context, profileContext, provisionInfo));
    /*
     * @tc.steps: step3. app distributed type is NONE_TYPE.
     * @tc.expected: step3. the return will be false.
     */
    profileContext.matchResult.matchState = MATCH_WITH_PROFILE;
    provisionInfo.type = ProvisionType::RELEASE;
    provisionInfo.distributionType = AppDistType::NONE_TYPE;
    ASSERT_FALSE(v2.VerifyProfileInfo(pkcs7Context, profileContext, provisionInfo));
    /*
     * @tc.steps: step4. profile distributed cert is null.
     * @tc.expected: step4. the return will be false.
     */
    provisionInfo.distributionType = AppDistType::ENTERPRISE;
    ASSERT_FALSE(v2.VerifyProfileInfo(pkcs7Context, profileContext, provisionInfo));
    X509_free(certX509);
}

/**
 * @tc.name: Test ParseAndVerifyProfileIfNeed function
 * @tc.desc: The static function will return result of ParseAndVerifyProfileIfNeed;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyV2Test, ParseAndVerifyProfileIfNeedTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. input a null profile.
     * @tc.expected: step1. the return will be false.
     */
    HapVerifyV2 v2;
    std::string profile;
    ProvisionInfo provisionInfo;
    ASSERT_FALSE(v2.ParseAndVerifyProfileIfNeed(profile, provisionInfo, false));
    /*
     * @tc.steps: step1. input no need parse and verify profile.
     * @tc.expected: step1. the return will be true.
     */
    ASSERT_TRUE(v2.ParseAndVerifyProfileIfNeed(profile, provisionInfo, true));
}

/**
 * @tc.name: Test GetDigestAndAlgorithm function
 * @tc.desc: The static function will return result of GetDigestAndAlgorithm;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyV2Test, GetDigestAndAlgorithmTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. input an error pkcs7 content.
     * @tc.expected: step1. the return will be false.
     */
    HapVerifyV2 v2;
    Pkcs7Context digest;
    digest.content.SetCapacity(TEST_FILE_BLOCK_LENGTH);
    ASSERT_FALSE(v2.GetDigestAndAlgorithm(digest));
}
}
