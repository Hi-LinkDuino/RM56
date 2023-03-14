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
#ifndef LOCALIZATION_MODULE_TEST_H
#define LOCALIZATION_MODULE_TEST_H
#include "acelite_config.h"
#if (FEATURE_LOCALIZATION_MODULE == 1)
#include "basic_tdd_test.h"
#include "test_common.h"
#include "js_fwk_common.h"
namespace OHOS {
namespace ACELite {
class LocalizationModuleTddTest : public BasicTddTest {
public:

    void SetUp();

    void TearDown();

    void TestNullValue009();

#ifndef TDD_ASSERTIONS
    void RunTests();
#endif
    jerry_value_t localization_;
    jerry_value_t viewModel_;
};
#endif
} // namespace ACELite
} // namespace OHOS
#endif

