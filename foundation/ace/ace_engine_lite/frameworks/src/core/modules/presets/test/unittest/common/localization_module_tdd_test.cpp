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
#if (FEATURE_LOCALIZATION_MODULE == 1)
#include "ace_log.h"
#include "global.h"
#include "js_app_context.h"
#include "js_app_environment.h"
#include "test_common.h"
namespace OHOS {
namespace ACELite {
void LocalizationModuleTddTest::SetUp()
{
#if (TARGET_SIMULATOR == 1)
    constexpr char testAppPath[] = "..\\moduletest\\unittest";
#elif defined(__linux__)
    constexpr char testAppPath[] = "/storage/data";
#else
    constexpr char testAppPath[] = "/user/data";
#endif
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    if (env == nullptr) {
        return;
    }
    env->InitJsFramework();
    const int defaultToken = 1000;
    JsAppContext::GetInstance()->SetCurrentAbilityInfo(testAppPath, "com.localization.test", defaultToken);
    const char *content = "new ViewModel({});";
    const jerry_char_t *jScript = reinterpret_cast<const jerry_char_t *>(content);
    viewModel_ = jerry_eval(jScript, strlen(content), JERRY_PARSE_NO_OPTS);
    if (jerry_value_is_error(viewModel_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create ViewModel failed");
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "Pass");
    }
    // get the attribute $t
    localization_ = jerryx_get_property_str(viewModel_, "$t");
}

void LocalizationModuleTddTest::TearDown()
{
    if (viewModel_ != 0) {
        jerry_release_value(viewModel_);
    }
    if (localization_ != 0) {
        jerry_release_value(localization_);
    }
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    if (env == nullptr) {
        return;
    }
    env->Cleanup();
}

/**
 * @tc.name: TestNullValue009
 * @tc.desc: Test null object value in resource file
 */
HWTEST_F(LocalizationModuleTddTest, TestNullValue009, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.execute js this.$t('test01.value')
     * @tc.expected: step1.the result of execute js is undefined
     */
    jerry_value_t args[1];
    args[0] = jerry_create_string(reinterpret_cast<const jerry_char_t *>("test01.value"));
    JSValue result = CallJSFunction(localization_, viewModel_, args, 1);
    jerry_release_value(args[0]);
    jerry_release_value(result);
    TDD_CASE_END();
}

#ifndef TDD_ASSERTIONS
void LocalizationModuleTddTest::RunTests()
{
    TestNullValue009();
}
#endif
}
}
#endif
