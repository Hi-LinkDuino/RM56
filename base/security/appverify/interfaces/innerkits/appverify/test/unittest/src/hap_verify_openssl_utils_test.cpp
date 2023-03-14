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

#include "hap_verify_openssl_utils_test.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>
#include "openssl/x509.h"

#include "util/hap_verify_openssl_utils.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;

namespace {
class HapVerifyOpensslUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapVerifyOpensslUtilsTest::SetUpTestCase(void)
{
}

void HapVerifyOpensslUtilsTest::TearDownTestCase(void)
{
}
void HapVerifyOpensslUtilsTest::SetUp()
{
}

void HapVerifyOpensslUtilsTest::TearDown()
{
}

/**
 * @tc.name: Test VerifyPkcs7 functions
 * @tc.desc: use invalid input to verify pkcs7, The function will return false;
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyOpensslUtilsTest, VerifyPkcs7_001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. use a null input to run OpensslVerifyPkcs7
     * @tc.expected: step1. the return will be false.
     */
    Pkcs7Context digest;
    ASSERT_FALSE(HapVerifyOpensslUtils::VerifyPkcs7SignedData(digest));
    ASSERT_FALSE(HapVerifyOpensslUtils::VerifySignInfo(nullptr, nullptr, 0, digest));
    ASSERT_FALSE(HapVerifyOpensslUtils::VerifyShaWithRsaPss(nullptr, nullptr, true, nullptr, 0));
    std::vector<std::string> publicKeyVec;
    ASSERT_FALSE(HapVerifyOpensslUtils::GetPublickeyFromCertificate(nullptr, publicKeyVec));
}

/**
 * @tc.name: Test GetDigestAlgorithmId functions
 * @tc.desc: use different algorithm IDs to run this function, The function will return nid
 * @tc.type: FUNC
 */
HWTEST_F(HapVerifyOpensslUtilsTest, GetDigestAlgorithmId001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. use different algorithm IDs to run OpensslVerifyPkcs7
     * @tc.expected: step1. the return will be nID.
     */
    int nId = HapVerifyOpensslUtils::GetDigestAlgorithmId(ALGORITHM_SHA512_WITH_RSA_PSS);
    ASSERT_TRUE(nId == TEST_SHA512_NID);
    nId = HapVerifyOpensslUtils::GetDigestAlgorithmId(ALGORITHM_SHA384_WITH_RSA_PSS);
    ASSERT_TRUE(nId == TEST_SHA384_NID);
    nId = HapVerifyOpensslUtils::GetDigestAlgorithmId(ALGORITHM_SHA256_WITH_RSA_PSS);
    ASSERT_TRUE(nId == TEST_SHA256_NID);
    nId = HapVerifyOpensslUtils::GetDigestAlgorithmId(0);
    ASSERT_TRUE(nId == 0);
}
}
