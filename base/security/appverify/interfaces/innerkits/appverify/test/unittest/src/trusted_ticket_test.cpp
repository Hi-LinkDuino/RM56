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

#include "trusted_ticket_test.h"

#include <gtest/gtest.h>

#include "init/trusted_ticket_manager.h"

using namespace testing::ext;
using namespace OHOS::Security::Verify;

namespace {
class TrustedTicketTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void TrustedTicketTest::SetUpTestCase(void)
{
}

void TrustedTicketTest::TearDownTestCase(void)
{
}

void TrustedTicketTest::SetUp()
{
}

void TrustedTicketTest::TearDown()
{
}

/**
 * @tc.name: Test trusted root init and debug mode
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F (TrustedTicketTest, CreatTrustedTicketCA001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. run init and recovery.
     * @tc.expected: step1. the return will true.
     */
    TrustedTicketManager& trustedticketsource = TrustedTicketManager::GetInstance();
    ASSERT_TRUE(trustedticketsource.Init());
    trustedticketsource.Recovery();
    ASSERT_TRUE(trustedticketsource.Init());
}

/**
 * @tc.name: Test IsTrustedSource function
 * @tc.desc: The static function will return object of matched cert;
 * @tc.type: FUNC
 */
HWTEST_F (TrustedTicketTest, IsTrustedSourceTest001, TestSize.Level1)
{
    /*
     * @tc.steps: step1. input nullptr.
     * @tc.expected: step1. the return will be DO_NOT_MATCH, because certSubject and certIssuer are not matched.
     */
    TrustedTicketManager& trustedticketsource = TrustedTicketManager::GetInstance();
    std::string certSubject = "";
    std::string certIssuer = "";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject, certIssuer, 3).matchState == DO_NOT_MATCH);
    /*
     * @tc.steps: step2.  input invalid certSubject and valid certIssuer.
     * @tc.expected: step2. the return will be DO_NOT_MATCH, because invalid certSubject.
     */
    certSubject = "C=CN, O=Huawei, OU=MemberCenter, CN=MemberShip Ticket Release V1";
    certIssuer = "C=CN, O=Huawei, OU=Huawei CBG, CN=Huawei CBG Software Signing Service CA";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject, certIssuer, 3).matchState == DO_NOT_MATCH);
    /*
     * @tc.steps: step3.  input valid certSubject and invalid certIssuer.
     * @tc.expected: step3. the return will be DO_NOT_MATCH, because invalid certIssuer.
     */
    certSubject = "C=CN, O=Huawei, OU=MemberCenter, CN=MemberShip ticket Release V1";
    certIssuer = "C=CN, O=Huawei, OU=Huawei CBG, CN=Huawei CBG Software Signing Service";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject, certIssuer, 3).matchState == DO_NOT_MATCH);
    /*
     * @tc.steps: step4. input valid certSubject and certIssuer.
     * @tc.expected: step4. the return will be MATCH_WITH_TICKET.
     */
    certSubject = "C=CN, O=Huawei, OU=MemberCenter, CN=MemberShip ticket Release V1";
    certIssuer = "C=CN, O=Huawei, OU=Huawei CBG, CN=Huawei CBG Software Signing Service CA";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject, certIssuer, 3).matchState == MATCH_WITH_TICKET);
    /*
     * @tc.steps: step5. input invalid certListpath.
     * @tc.expected: step5. the return will be DO_NOT_MATCH, because certListpath is not matched.
     */
    certSubject = "C=CN, O=Huawei, OU=MemberCenter, CN=MemberShip ticket Release V1";
    certIssuer = "C=CN, O=Huawei, OU=Huawei CBG, CN=Huawei CBG Software Signing Service CA";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject, certIssuer, 2).matchState == DO_NOT_MATCH);
}

HWTEST_F (TrustedTicketTest, IsTrustedSourceTest002, TestSize.Level1)
{
    /*
     * @tc.steps: step1. input invalid certsubject.
     * @tc.expected: step1. the return will be DO_NOT_MATCH, because invalid certSubject.
     */
    TrustedTicketManager& trustedticketsource = TrustedTicketManager::GetInstance();
    std::string certSubject1 = "C=CN, O=Huawei, OU=MemberCenter, CN=MemberShip ticket Release V0";
    std::string certIssuer = "C=CN, O=Huawei, OU=Huawei CBG, CN=Huawei CBG Software Signing Service CA";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject1, certIssuer, 3).matchState == DO_NOT_MATCH);
    /*
     * @tc.steps: step2. input invalid certsubject.
     * @tc.expected: step2. the return will be DO_NOT_MATCH, because invalid certSubject.
     */
    std::string certSubject2 = "C=CN, O=Huawei, OU=MemberCenter, CN=MemberShip ticket Release V01";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject2, certIssuer, 3).matchState == DO_NOT_MATCH);
    /*
     * @tc.steps: step3. input valid certsubject.
     * @tc.expected: step3. the return will be MATCH_WITH_TICKET.
     */
    std::string certSubject3 = "C=CN, O=Huawei, OU=MemberCenter, CN=MemberShip ticket Release V2";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject3, certIssuer, 3).matchState == MATCH_WITH_TICKET);
    /*
     * @tc.steps: step4. input valid certsubject.
     * @tc.expected: step4. the return will be MATCH_WITH_TICKET.
     */
    std::string certSubject4 = "C=CN, O=Huawei, OU=MemberCenter, CN=MemberShip ticket Release V109";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject4, certIssuer, 3).matchState == MATCH_WITH_TICKET);
    /*
     * @tc.steps: step5. input valid and invalid certsubject.
     * @tc.expected: step5. the return will be DO_NOT_MATCH, because invalid certSubject.
     */
    std::string certSubject5 = "C=CN, O=Huawei, OU=Member Center, CN=MemberShip ticket Release V6";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject5, certIssuer, 3).matchState == DO_NOT_MATCH);
    /*
     * @tc.steps: step6. input invalid certsubject.
     * @tc.expected: step6. the return will be DO_NOT_MATCH, because invalid certSubject.
     */
    std::string certSubject6 = "C=CN, O=Other, OU=MemberCenter, CN=MemberShip ticket Release V8";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject6, certIssuer, 3).matchState == DO_NOT_MATCH);
    /*
     * @tc.steps: step7. input invalid certsubject.
     * @tc.expected: step6. the return will be DO_NOT_MATCH, because invalid certSubject.
     */
    std::string certSubject7 = "1C=CN, O=Other, OU=MemberCenter, CN=MemberShip ticket Release V9";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject7, certIssuer, 3).matchState == DO_NOT_MATCH);
    /*
     * @tc.steps: step8. input invalid certsubject.
     * @tc.expected: step6. the return will be DO_NOT_MATCH, because invalid certSubject.
     */
    std::string certSubject8 = "C=CN, O=Other, OU=MemberCenter, CN=MemberShip ticket Release V9 9";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject8, certIssuer, 3).matchState == DO_NOT_MATCH);
    /*
     * @tc.steps: step9. input invalid certsubject.
     * @tc.expected: step6. the return will be DO_NOT_MATCH, because invalid certSubject.
     */
    std::string certSubject9 = "C=CN, O=Other, OU=MemberCenter, CN=MemberShip ticket Release V9a";
    ASSERT_TRUE(trustedticketsource.IsTrustedSource(certSubject9, certIssuer, 3).matchState == DO_NOT_MATCH);
}
}
