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

#include "console_module_test.h"

#include "ace_log.h"
#include "console_log_impl.h"
#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_fwk_common.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
namespace {
// keep sync with the defination in console_log_impl.cpp
#ifdef CONSOLE_LOG_LINE_MAX_LENGTH
static const int16_t LOG_BUFFER_SIZE = CONSOLE_LOG_LINE_MAX_LENGTH;
#else
static const int16_t LOG_BUFFER_SIZE = 256; // use 256 as default if it's not config
#endif // CONSOLE_LOG_LINE_MAX_LENGTH
// the verifing buffer length is bigger than the max
static const size_t VERIFY_BUFFER_LENGTH = 2 * LOG_BUFFER_SIZE;
} // namespace

// used for holding verifing
static char *g_verifyBuffer = nullptr;
static size_t g_logContentLength = 0;
static size_t g_logRound = 0;
// this is the extra handler to get output string and fill into one buffer for following verifing buffer
static void VerifyBufferFillHanlder(OHOS::ACELite::LogLevel level, const char *logContent, size_t length)
{
    UNUSED(level);
    if (g_verifyBuffer == nullptr || logContent == nullptr || length == 0 || length >= VERIFY_BUFFER_LENGTH) {
        return;
    }

    if (EOK != strcpy_s(g_verifyBuffer, VERIFY_BUFFER_LENGTH, logContent)) {
        return;
    }
    g_logContentLength = length;
    g_logRound++;
}

void ConsoleModuleTest::SetUp()
{
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    if (env == nullptr) {
        // should never happen
        return;
    }
    env->InitJsFramework();
    // initialize the verifing buffer
    g_verifyBuffer = static_cast<char *>(ace_malloc(VERIFY_BUFFER_LENGTH));
    if (g_verifyBuffer == nullptr) {
        return;
    }
    if (EOK != memset_s(g_verifyBuffer, VERIFY_BUFFER_LENGTH, 0, VERIFY_BUFFER_LENGTH)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "prepare verify buffer failed");
    }
    RegisterJSLogOutputHandler(VerifyBufferFillHanlder);
    g_logContentLength = 0;
    g_logRound = 0;
}

void ConsoleModuleTest::TearDown()
{
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    JsAppContext *context = JsAppContext::GetInstance();
    if (env == nullptr || context == nullptr) {
        // should never happen
        return;
    }
    context->ReleaseStyles();
    env->Cleanup();
    ACE_FREE(g_verifyBuffer);
    RegisterJSLogOutputHandler(nullptr);
    g_logContentLength = 0;
    g_logRound = 0;
}

void ConsoleModuleTest::EvalAndVerify(TestObjectWrapper object)
{
    if (object.code == nullptr || object.targetLogContent == nullptr || object.codeLength == 0 ||
        object.targetLogLen == 0) {
        return;
    }
    jerry_value_t evalResult =
        jerry_eval(reinterpret_cast<const jerry_char_t *>(object.code), object.codeLength, JERRY_PARSE_NO_OPTS);
    jerry_release_value(evalResult);
    int res = (g_verifyBuffer == nullptr) ? -1 : strcmp(object.targetLogContent, g_verifyBuffer);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(g_logContentLength, object.targetLogLen);
    EXPECT_EQ(g_logRound, object.targetLogRound);
}

/**
 * @tc.name: AceliteFrameworkConsoleLog001
 * @tc.desc: Verify the Console module API output
 */
HWTEST_F(ConsoleModuleTest, ConsoleLog001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare data
     */
    const char targetLog[] = "[Console Debug] This is a console log.";
    const size_t targetLogLength = strlen(targetLog);
    const char codeToEval[] = "console.log(\"This is a console log.\");";
    const size_t evalCodeLen = strlen(codeToEval);
    const size_t targetRound = 1;

    /**
     * @tc.steps: step2. eval and verify
     */
    TestObjectWrapper testObject = {evalCodeLen, codeToEval, targetLogLength, targetLog, targetRound};
    EvalAndVerify(testObject);
}

/**
 * @tc.name: AceliteFrameworkConsoleLog002
 * @tc.desc: Verify the Console module API output
 */
HWTEST_F(ConsoleModuleTest, ConsoleLog002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare data
     */
    const char targetLog[] = "[Console Debug] This is a console log.";
    const size_t targetLogLength = strlen(targetLog);
    const char codeToEval[] = "console.debug(\"This is a console log.\");";
    const size_t evalCodeLen = strlen(codeToEval);
    const size_t targetRound = 1;

    /**
     * @tc.steps: step2. eval and verify
     */
    TestObjectWrapper testObject = {evalCodeLen, codeToEval, targetLogLength, targetLog, targetRound};
    EvalAndVerify(testObject);
}

/**
 * @tc.name: AceliteFrameworkConsoleLog003
 * @tc.desc: Verify the Console module API output
 */
HWTEST_F(ConsoleModuleTest, ConsoleLog003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare data
     */
    const char targetLog[] = "[Console Error] This is a console error log.";
    const size_t targetLogLength = strlen(targetLog);
    const char codeToEval[] = "console.error(\"This is a console error log.\");";
    const size_t evalCodeLen = strlen(codeToEval);
    const size_t targetRound = 1;

    /**
     * @tc.steps: step2. eval and verify
     */
    TestObjectWrapper testObject = {evalCodeLen, codeToEval, targetLogLength, targetLog, targetRound};
    EvalAndVerify(testObject);
}

/**
 * @tc.name: AceliteFrameworkConsoleLog004
 * @tc.desc: Verify the Console module API output
 */
HWTEST_F(ConsoleModuleTest, ConsoleLog004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare data
     */
    const char targetLog[] = "[Console Warn] This is a console warning log.";
    const size_t targetLogLength = strlen(targetLog);
    const char codeToEval[] = "console.warn(\"This is a console warning log.\");";
    const size_t evalCodeLen = strlen(codeToEval);
    const size_t targetRound = 1;

    /**
     * @tc.steps: step2. eval and verify
     */
    TestObjectWrapper testObject = {evalCodeLen, codeToEval, targetLogLength, targetLog, targetRound};
    EvalAndVerify(testObject);
}

/**
 * @tc.name: AceliteFrameworkConsoleLog005
 * @tc.desc: Verify the Console module API output
 */
HWTEST_F(ConsoleModuleTest, ConsoleLog005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare data
     */
    const char targetLog[] = "[Console Info] This is a console info log.";
    const size_t targetLogLength = strlen(targetLog);
    const char codeToEval[] = "console.info(\"This is a console info log.\");";
    const size_t evalCodeLen = strlen(codeToEval);
    const size_t targetRound = 1;

    /**
     * @tc.steps: step2. eval and verify
     */
    TestObjectWrapper testObject = {evalCodeLen, codeToEval, targetLogLength, targetLog, targetRound};
    EvalAndVerify(testObject);
}

/**
 * @tc.name: AceliteFrameworkConsoleLog006
 * @tc.desc: Verify the Console module API output
 */
HWTEST_F(ConsoleModuleTest, ConsoleLog006, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare data
     */
    const char logContent[] =
        "This is very long log content. This is very long log content. This is very long log content. This is very "
        "long log content. This is very long log content. This is very long log content. This is very long log "
        "content. This is very long log content. This is very long log content. This is very long log content. This is "
        "very long log content.";
    const size_t targetLogLength = strlen(logContent);
    constexpr size_t bufferLen = 512;
    char codeToEval[bufferLen];
    if (EOK != memset_s(codeToEval, bufferLen, 0, bufferLen)) {
        return;
    }
    if (EOK != sprintf_s(codeToEval, bufferLen, "console.log(\"%s\");", logContent)) {
        return;
    }
    const size_t evalCodeLen = strlen(codeToEval);
    const size_t targetRound = ((targetLogLength % LOG_BUFFER_SIZE) != 0) ? (targetLogLength / LOG_BUFFER_SIZE + 1)
                                                                          : (targetLogLength / LOG_BUFFER_SIZE);

    /**
     * @tc.steps: step2. eval and verify
     */
    jerry_release_value(
        jerry_eval(reinterpret_cast<const jerry_char_t *>(codeToEval), evalCodeLen, JERRY_PARSE_NO_OPTS));
    EXPECT_EQ(g_logRound, targetRound);
}
} // namespace ACELite
} // namespace OHOS
