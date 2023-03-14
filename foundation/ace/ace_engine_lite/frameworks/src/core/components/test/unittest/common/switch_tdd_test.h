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
#ifndef ACELITE_SWITCH_TDD_TEST_H
#define ACELITE_SWITCH_TDD_TEST_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

#include "acelite_config.h"
#include "component_factory.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class SwitchTddTest : public testing::Test {
#else
class SwitchTddTest {
#endif
public:
    SwitchTddTest();
    ~SwitchTddTest(){}
    void SetUp();
    void TearDown();
    void ComponentSwitchAttributeSetTest001();
    void ComponentSwitchAttributeSetTest002();
    void ComponentSwitchAttributeSetTest003();
    void RunTests();

    jerry_value_t optionsObj_;
    jerry_value_t attrsObj_;
    jerry_value_t styleObj_;
    jerry_value_t eventObj_;
    jerry_value_t childrenObj_;
    uint16_t componentKeyId_;
};
} // namespace ACELite
} // namespace OHOS
#endif // ACELITE_SWITCH_TDD_TEST_H
