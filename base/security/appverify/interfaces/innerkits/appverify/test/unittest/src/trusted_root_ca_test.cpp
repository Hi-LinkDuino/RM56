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

#include "trusted_root_ca_test.h"

#include <gtest/gtest.h>

#include "init/trusted_root_ca.h"
#include "util/hap_cert_verify_openssl_utils.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;

namespace {
class TrustedRootCaTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void TrustedRootCaTest::SetUpTestCase(void)
{
}

void TrustedRootCaTest::TearDownTestCase(void)
{
}

void TrustedRootCaTest::SetUp()
{
}

void TrustedRootCaTest::TearDown()
{
}

/**
 * @tc.name: Test trusted root init and debug mode
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F (TrustedRootCaTest, CreatTrustedRootCA001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. run init and recovery.
     * @tc.expected: step1. the return will true.
     */
    TrustedRootCa& rootCertsObj = TrustedRootCa::GetInstance();
    ASSERT_TRUE(rootCertsObj.Init());
    rootCertsObj.Recovery();
    ASSERT_TRUE(rootCertsObj.Init());
    /*
     * @tc.steps: step2. run EableDebug and DisableDebug.
     * @tc.expected: step2. the return will true.
     */
    ASSERT_TRUE(rootCertsObj.EnableDebug());
    rootCertsObj.DisableDebug();
    ASSERT_TRUE(rootCertsObj.EnableDebug());
}

/**
 * @tc.name: Test FindMatchedRoot function
 * @tc.desc: The static function will return object of matched cert;
 * @tc.type: FUNC
 */
HWTEST_F (TrustedRootCaTest, FindMatchedRootTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. input nullptr.
     * @tc.expected: step1. the return will be nullptr.
     */
    TrustedRootCa& rootCertsObj = TrustedRootCa::GetInstance();
    X509* cert = nullptr;
    ASSERT_TRUE(rootCertsObj.FindMatchedRoot(cert) == nullptr);
    /*
     * @tc.steps: step2. input a self sign cert.
     * @tc.expected: step2. the return will be nullptr, because no trusted root is matched.
     */
    cert = HapCertVerifyOpensslUtils::GetX509CertFromPemString(ECC_TEST_CERT);
    ASSERT_TRUE(cert != nullptr);
    ASSERT_TRUE(rootCertsObj.FindMatchedRoot(cert) == nullptr);
    X509_free(cert);
    /*
     * @tc.steps: step2. input a slft ca sign cert.
     * @tc.expected: step2. the return will be ptr of trusted root.
     */
    cert = HapCertVerifyOpensslUtils::GetX509CertFromPemString(SOFT_CA_CERT);
    ASSERT_TRUE(cert != nullptr);
    X509* matchCert = rootCertsObj.FindMatchedRoot(cert);
    ASSERT_TRUE(matchCert != nullptr);
    X509_free(cert);
    X509_free(matchCert);
}

/**
 * @tc.name: Test GetTrustedRootCAFromJson function.
 * @tc.desc: The static function will return false;
 * @tc.type: FUNC
 */
HWTEST_F (TrustedRootCaTest, GetTrustedRootCAFromJsonTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. use an error filePath as input.
     * @tc.expected: step1. the return will false.
     */
    TrustedRootCa& rootCertsObj = TrustedRootCa::GetInstance();
    StringCertMap rootCertMap;
    std::string filePath = "./trusted_root_ca.json";
    ASSERT_FALSE(rootCertsObj.GetTrustedRootCAFromJson(rootCertMap, filePath));
}
}
