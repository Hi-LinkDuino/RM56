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
#ifndef NUMBER_FORMAT_MODULE_TDD_TEST_H
#define NUMBER_FORMAT_MODULE_TDD_TEST_H
#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

#include "acelite_config.h"
#if (FEATURE_NUMBER_FORMAT == 1)
#include "test_common.h"
#include "js_fwk_common.h"
namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class NumberFormatModuleTddTest : public testing::Test {
#else
class NumberFormatModuleTddTest {
#endif
public:
    NumberFormatModuleTddTest() {}
    ~NumberFormatModuleTddTest() {}
    void SetUp();
    void TearDown();

    void RunTests();

    void NumberFormat_Locale_Test001();

    void NumberFormat_Style_Test002();

    void NumberFormat_Style_Test003();

    void NumberFormat_Group_Test004();

    void NumberFormat_Group_Test005();

    void NumberFormat_Group_Test006();

    void NumberFormat_Group_Test007();

    void NumberFormat_MinimumFractionDigit_Test008();

    void NumberFormat_MinimumFractionDigit_Test009();

    void NumberFormat_MinimumFractionDigit_Test010();

    void NumberFormat_MinimumFractionDigit_Test011();

    void NumberFormat_MaxmumFractionDigit_Test012();

    void NumberFormat_MaxmumFractionDigit_Test013();

    void NumberFormat_MaxmumFractionDigit_Test014();

    void NumberFormat_MaxmumFractionDigit_Test015();

    jerry_value_t RunJs(const char* jsContent);

    char* FormatNumber(const jerry_value_t numberFormat, const jerry_value_t args[], const jerry_length_t argsNum);
};
}
}

#endif // FEATURE_NUMBER_FORMAT
#endif // NUMBER_FORMAT_MODULE_TDD_TEST_H
