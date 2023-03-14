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

#ifndef OHOS_ACELITE_CONSOLE_LOG_MODULE_TDD_TEST_H
#define OHOS_ACELITE_CONSOLE_LOG_MODULE_TDD_TEST_H

#include "acelite_config.h"
#include "console_log_impl.h"
#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#endif
#include "test_common.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
using OHOS::ACELite::LogLevel;

// use this struture for holding test contents
struct TestObjectWrapper {
    size_t codeLength;
    const char *code;
    size_t targetLogLen;
    const char *targetLogContent;
    size_t targetLogRound;
};

#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class ConsoleModuleTest : public testing::Test {
#else
class ConsoleModuleTest {
#endif
public:
    ConsoleModuleTest() = default;
    ~ConsoleModuleTest() = default;
    void SetUp();
    void TearDown();
    void ConsoleLog001();
    void ConsoleLog002();
    void ConsoleLog003();
    void ConsoleLog004();
    void ConsoleLog005();
    void EvalAndVerify(TestObjectWrapper object);
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_CONSOLE_LOG_MODULE_TDD_TEST_H
