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

#ifndef OHOS_ACELITE_STACK_TDD_TEST_H
#define OHOS_ACELITE_STACK_TDD_TEST_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#endif

#include "component_factory.h"
#include "root_component_mock.h"
#include "test_common.h"

#define CASE_END_IF_NULLPTR(ptr) \
    do {                         \
        if (ptr == nullptr) {    \
            TDD_CASE_END();      \
            return;              \
        }                        \
    } while (0)

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class StackTddTest : public testing::Test {
#else
class StackTddTest {
#endif
public:
    StackTddTest();
    ~StackTddTest(){}
    void SetUp();
    void TearDown();
    void ComponentStackTest001();
    void ComponentStackTest002();
    void ComponentStackTest003();
    void ComponentStackTest004();
    void ComponentStackTest005();
    void ComponentStackTest006();
    void RunTests();

private:
    Component *GetRenderedComponent(uint16_t componentKeyId, jerry_value_t children) const;
    Component *CreateChildComponent(jerry_value_t childOptions) const;
    void ReleaseComponent(Component *&component) const;
    jerry_value_t optionsObj_;
    jerry_value_t attrsObj_;
    jerry_value_t styleObj_;
    RootComponentMock rootComponentMock_;
    uint16_t componentNameId_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_STACK_TDD_TEST_H
