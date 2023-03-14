/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "jsfwk_tdd_test.h"
#include <cstdio>
#include "ace_version.h"
#include "acelite_config.h"
#include "js_ability.h"
#include "js_app_context.h"
#include "js_config.h"
#include "js_debugger_config.h"

namespace OHOS {
namespace ACELite {
JSfwkTddTest::JSfwkTddTest() {}

static JSAbility *g_targetJSAbility = nullptr;
static char *g_currentTestCaseName = nullptr;
static uint8_t g_assertRecord = 0;

void JSfwkTddTest::TestCaseB(const char *name) const
{
    g_assertRecord = 0;
    g_currentTestCaseName = const_cast<char *>(name);
}

void JSfwkTddTest::JudeTestCaseResult() const
{
    if (g_assertRecord > 0) {
        printf("[Test Case] [%s]   FAILED \n", g_currentTestCaseName);
    } else {
        printf("[Test Case] [%s]   PASSED \n", g_currentTestCaseName);
    }
}

void JSfwkTddTest::SetUp()
{
    g_targetJSAbility = new JSAbility();
}

void JSfwkTddTest::TearDown()
{
    if (g_targetJSAbility != nullptr) {
        delete g_targetJSAbility;
        g_targetJSAbility = nullptr;
    }
}

void JSfwkTddTest::StartUpEngine()
{
    // free the external JS context, only can be called after clean up engine
    Debugger::GetInstance().SetupJSContext();
    jerry_init(JERRY_INIT_EMPTY);
}

void JSfwkTddTest::TearDownEngine()
{
    jerry_cleanup();
    // free the external JS context, only can be called after clean up engine
    Debugger::GetInstance().ReleaseJSContext();
    JudeTestCaseResult();
}

void JSfwkTddTest::ACELiteFrameworkInitialization001()
{
    TestCaseB("ACELiteFrameworkInitialization001");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step2. call Launch method and give invalid input paramaters
     */
    g_targetJSAbility->Launch("", "", 0);

    /**
     * @tc.steps: step3. check if the top JS ability is initialized
     */
    JSAbilityImpl *jsAbilityImpl = const_cast<JSAbilityImpl *>(JsAppContext::GetInstance()->GetTopJSAbilityImpl());
    EXPECT_TRUE(jsAbilityImpl == nullptr);
    /**
     * @tc.steps: step4. clean up test environment
     */
    TDD_CASE_END();
}

void JSfwkTddTest::ACELiteFrameworkInitialization002()
{
    TestCaseB("ACELiteFrameworkInitialization002");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step2. call Launch method and give invalid input paramaters
     */
    g_targetJSAbility->Launch("user/data/demo/", "", 0);

    /**
     * @tc.steps: step3. check if the top JS ability is initialized
     */
    JSAbilityImpl *jsAbilityImpl = const_cast<JSAbilityImpl *>(JsAppContext::GetInstance()->GetTopJSAbilityImpl());
    EXPECT_TRUE(jsAbilityImpl == nullptr);
    /**
     * @tc.steps: step4. clean up test environment
     */
    TDD_CASE_END();
}

void JSfwkTddTest::ACELiteFrameworkInitialization003()
{
    TestCaseB("ACELiteFrameworkInitialization003");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step2. call Launch method and give invalid input paramaters
     */
    constexpr char packageName[] = "test";
    g_targetJSAbility->Launch("user/data/test/", packageName, 0);

    /**
     * @tc.steps: step3. check if the top JS ability is initialized
     */
    JSAbilityImpl *jsAbilityImpl = const_cast<JSAbilityImpl *>(JsAppContext::GetInstance()->GetTopJSAbilityImpl());
    EXPECT_TRUE(jsAbilityImpl != nullptr);

    /**
     * @tc.steps: step4. try to call back-pressed to trigger terminating
     */
    g_targetJSAbility->BackPressed();

    /**
     * @tc.steps: step5. check the package name is same with the one we passed
     */
    if (strcmp(g_targetJSAbility->GetPackageName(), packageName)) {
        g_assertRecord++;
    }
    EXPECT_TRUE(!strcmp(g_targetJSAbility->GetPackageName(), packageName));

    /**
     * @tc.steps: step5. clean up test environment
     */
    g_targetJSAbility->TransferToDestroy();
    g_targetJSAbility = nullptr;
    TDD_CASE_END();
}

void JSfwkTddTest::ACELiteFrameworkInitialization009()
{
    TestCaseB("ACELiteFrameworkInitialization009");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();
#if IS_ENABLED(ACE_LITE_VERSION_JS_API)
    /**
     * @tc.steps: step2. check version code
     */
    EXPECT_TRUE(ACEVersion::GetStr() != nullptr);
    /**
     * @tc.steps: step3. check commit code
     */
    EXPECT_TRUE(ACEVersion::GetCommit() != nullptr);
    /**
     * @tc.steps: step4. check build timestamp
     */
    const char *buildStamp = ACEVersion::GetTimeStamp();
    EXPECT_TRUE(buildStamp != nullptr);
    constexpr char firstVersionBuildStamp[] = "2020-05-09 16:31:41";
    if (strcmp(buildStamp, firstVersionBuildStamp) == 0) {
        // the version code header file is not updated, make the case failed
        g_assertRecord++;
    }
    EXPECT_TRUE(strcmp(buildStamp, firstVersionBuildStamp) != 0);
#endif
    TDD_CASE_END();
}

void JSfwkTddTest::ACELiteFrameworkInitialization011()
{
    TestCaseB("ACELiteFrameworkInitialization011");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step2. call launch
     */
    constexpr char packageName[] = "com.example.test";
    g_targetJSAbility->Launch("user/data/test/", packageName, 0);

    /**
     * @tc.steps: step3. call show
     */
    g_targetJSAbility->Show();

    /**
     * @tc.steps: step4. verify
     */
    if (strcmp(g_targetJSAbility->GetPackageName(), packageName)) {
        g_assertRecord++;
    }
    EXPECT_TRUE(!strcmp(g_targetJSAbility->GetPackageName(), packageName));
    TDD_CASE_END();
}

void JSfwkTddTest::ACELiteFrameworkInitialization012()
{
    TestCaseB("ACELiteFrameworkInitialization012");
    /**
     * @tc.steps: step1. init test environment
     */
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step2. call launch
     */
    constexpr char packageName[] = "com.example.test";
    g_targetJSAbility->Launch("user/data/test/", packageName, 0);

    /**
     * @tc.steps: step3. call show then hide
     */
    g_targetJSAbility->Show();
    g_targetJSAbility->Hide();

    /**
     * @tc.steps: step4. verify
     */
    if (strcmp(g_targetJSAbility->GetPackageName(), packageName)) {
        g_assertRecord++;
    }
    EXPECT_TRUE(!strcmp(g_targetJSAbility->GetPackageName(), packageName));
    TDD_CASE_END();
}

void JSfwkTddTest::AceliteFrameworkEngineIntegrating01()
{
    TestCaseB("AceliteFrameworkEngineIntegrating01");
    /**
     * @tc.steps: step1. init test environment
     */
    StartUpEngine();
    /**
     * @tc.steps: step2. check the jerry engine's configuration parser
     */
    bool featureOn = jerry_is_feature_enabled(JERRY_FEATURE_JS_PARSER);

    /**
     * @tc.steps: step3. check JS parser feature is on
     */
    EXPECT_TRUE(featureOn);
    /**
     * @tc.steps: step4. clean up test environment
     */
    TearDownEngine();
}

void JSfwkTddTest::AceliteFrameworkEngineIntegrating02()
{
    TestCaseB("AceliteFrameworkEngineIntegrating02");
    /**
     * @tc.steps: step1. init test environment
     */
    StartUpEngine();
    /**
     * @tc.steps: step1. check the jerry engine's configuration promise(which is part of es2005)
     */
    bool featureOn = jerry_is_feature_enabled(JERRY_FEATURE_PROMISE);

    /**
     * @tc.steps: step2. check JS promise feature is off
     */
    EXPECT_FALSE(featureOn);

    /**
     * @tc.steps: step3. check the jerry engine's configuration typedarray (which is part of es2005)
     */
    featureOn = jerry_is_feature_enabled(JERRY_FEATURE_TYPEDARRAY);

    /**
     * @tc.steps: step4. check JS typedarray is off
     */
    EXPECT_TRUE(featureOn);
    /**
     * @tc.steps: step5. cleanup test environment
     */
    TearDownEngine();
}

void JSfwkTddTest::AceliteFrameworkEngineIntegrating03()
{
    TestCaseB("AceliteFrameworkEngineIntegrating03");
    /**
     * @tc.steps: step1. init test environment
     */
    StartUpEngine();

    /**
     * @tc.steps: step2. check the jerry engine's configuration snapshot
     */
    bool featureOn = jerry_is_feature_enabled(JERRY_FEATURE_SNAPSHOT_EXEC);

    /**
     * @tc.steps: step3. check JS snapshot feature is on
     */
    EXPECT_TRUE(featureOn);
    TearDownEngine();
}

void JSfwkTddTest::AceliteFrameworkEngineIntegrating04()
{
    TestCaseB("AceliteFrameworkEngineIntegrating04");
    StartUpEngine();
    /**
     * @tc.steps: step1. check the jerry engine's configuration regexp
     */
    bool featureOn = jerry_is_feature_enabled(JERRY_FEATURE_REGEXP);

    /**
     * @tc.steps: step2. check JS regexp feature is on
     */
    EXPECT_FALSE(featureOn);
    TearDownEngine();
}

void JSfwkTddTest::RunTests()
{
    ACELiteFrameworkInitialization001();
    ACELiteFrameworkInitialization002();
    ACELiteFrameworkInitialization003();
    ACELiteFrameworkInitialization009();
    AceliteFrameworkEngineIntegrating01();
    AceliteFrameworkEngineIntegrating02();
    AceliteFrameworkEngineIntegrating03();
    AceliteFrameworkEngineIntegrating04();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: ACELiteFrameworkInitialization001
 * @tc.desc: Verify if the invalid input will lead exception .
 */
HWTEST_F(JSfwkTddTest, init001, TestSize.Level1)
{
    JSfwkTddTest::ACELiteFrameworkInitialization001();
}

/**
 * @tc.name: ACELiteFrameworkInitialization002
 * @tc.desc: Verify if the invalid input will lead exception .
 */
HWTEST_F(JSfwkTddTest, init002, TestSize.Level1)
{
    JSfwkTddTest::ACELiteFrameworkInitialization002();
}

/**
 * @tc.name: ACELiteFrameworkInitialization003
 * @tc.desc: Verify if the invalid input will lead exception .
 */
HWTEST_F(JSfwkTddTest, init003, TestSize.Level1)
{
    JSfwkTddTest::ACELiteFrameworkInitialization003();
}

/**
 * @tc.name: ACELiteFrameworkInitialization009
 * @tc.desc: Verify the version API.
 */
HWTEST_F(JSfwkTddTest, init009, TestSize.Level1)
{
    JSfwkTddTest::ACELiteFrameworkInitialization009();
}

/**
 * @tc.name: ACELiteFrameworkInitialization011
 * @tc.desc: Verify hide and show.
 */
HWTEST_F(JSfwkTddTest, init011, TestSize.Level1)
{
    JSfwkTddTest::ACELiteFrameworkInitialization011();
}

/**
 * @tc.name: ACELiteFrameworkInitialization012
 * @tc.desc: Verify hide and show.
 */
HWTEST_F(JSfwkTddTest, init012, TestSize.Level1)
{
    JSfwkTddTest::ACELiteFrameworkInitialization012();
}

/**
 * @tc.name: AceliteFrameworkEngineIntegrating01
 * @tc.desc: Verify the configuration of jerry engine.
 */
HWTEST_F(JSfwkTddTest, engineIntegrating001, TestSize.Level1)
{
    JSfwkTddTest::AceliteFrameworkEngineIntegrating01();
}

/**
 * @tc.name: AceliteFrameworkEngineIntegrating02
 * @tc.desc: Verify the configuration of jerry engine.
 */
HWTEST_F(JSfwkTddTest, engineIntegrating002, TestSize.Level1)
{
    JSfwkTddTest::AceliteFrameworkEngineIntegrating02();
}

/**
 * @tc.name: AceliteFrameworkEngineIntegrating03
 * @tc.desc: Verify the configuration of jerry engine.
 */
HWTEST_F(JSfwkTddTest, engineIntegrating003, TestSize.Level1)
{
    JSfwkTddTest::AceliteFrameworkEngineIntegrating03();
}

/**
 * @tc.name: AceliteFrameworkEngineIntegrating04
 * @tc.desc: Verify the configuration of jerry engine.
 */
HWTEST_F(JSfwkTddTest, engineIntegrating004, TestSize.Level1)
{
    JSfwkTddTest::AceliteFrameworkEngineIntegrating04();
}
#endif
} // namespace ACELite
} // namespace OHOS
