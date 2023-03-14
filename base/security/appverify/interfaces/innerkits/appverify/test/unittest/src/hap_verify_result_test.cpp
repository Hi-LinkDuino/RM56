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

#include "hap_verify_result_test.h"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "interfaces/hap_verify_result.h"
#include "util/hap_signing_block_utils.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;
namespace {
class HapVerifyResultTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapVerifyResultTest::SetUpTestCase(void)
{
}

void HapVerifyResultTest::TearDownTestCase(void)
{
}

void HapVerifyResultTest::SetUp()
{
}

void HapVerifyResultTest::TearDown()
{
}

/**
 * @tc.name: Test GetPublicKey and GetSignature function
 * @tc.desc: The static function return publickey and signature will be null;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyResultTest, GetPublicKeyAndSignature001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. set a null buffer to HapVerifyResult as data package of pkcs7,
     *     and do GetPublicKey and GetSignature.
     * @tc.expected: step1. the return vector of string is null.
     */
    HapByteBuffer testBuffer;
    HapVerifyResult hapVerifyResult;
    hapVerifyResult.SetPkcs7SignBlock(testBuffer);
    std::vector<string> publicKeys = hapVerifyResult.GetPublicKey();
    ASSERT_TRUE(publicKeys.empty());
    std::vector<string> signatures = hapVerifyResult.GetSignature();
    ASSERT_TRUE(signatures.empty());
}

/**
 * @tc.name: Test SetOptionalBlocks and GetProfile GetProperty function
 * @tc.desc: The static function will return no profile and have a property;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyResultTest, GetProfileAndProperty001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. set an option with property to HapVerifyResult
     */
    std::vector<OptionalBlock> option;
    HapVerifyResult hapVerifyResult;
    OptionalBlock testblock;
    testblock.optionalType = PROPERTY_BLOB;
    testblock.optionalBlockValue.SetCapacity(TEST_HAPBYTEBUFFER_LENGTH);
    option.push_back(testblock);
    hapVerifyResult.SetOptionalBlocks(option);
    /*
     * @tc.steps: step2. GetProfile and detect it right.
     * @tc.expected: step2. the return code will be NO_THIS_BLOCK_IN_PACKAGE.
     */
    std::string getProfile;
    GetOptionalBlockResultCode targetResult = NO_THIS_BLOCK_IN_PACKAGE;
    int ret = hapVerifyResult.GetBlockFromOptionalBlocks(PROFILE_BLOB, getProfile);
    ASSERT_TRUE(ret == targetResult);
    /*
     * @tc.steps: step3. GetProperty.
     * @tc.expected: step3. the return code will be GET_SUCCESS.
     */
    std::string getProperty;
    ASSERT_TRUE(hapVerifyResult.GetProperty(getProperty) == GET_SUCCESS);
}

/**
 * @tc.name: Test SetVersion and GetVersion function
 * @tc.desc: The static function return Version which is set;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyResultTest, SetVersionAndGetVersion001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. set a target version to HapVerifyResult.
     * @tc.expected: step1. the return version is the value we set.
     */
    int targetVersion = TEST_HAP_SIGN_VERSION;
    HapVerifyResult hapVerifyResult;
    hapVerifyResult.SetVersion(targetVersion);
    int version = hapVerifyResult.GetVersion();
    ASSERT_TRUE(version == targetVersion);
}
}
