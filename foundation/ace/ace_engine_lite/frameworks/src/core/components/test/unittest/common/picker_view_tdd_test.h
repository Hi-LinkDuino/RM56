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

#ifndef ACELITE_PICKER_VIEW_TDD_TEST_H
#define ACELITE_PICKER_VIEW_TDD_TEST_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#endif

#include "acelite_config.h"
#include "component_factory.h"
#include "root_component_mock.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
static jerry_value_t g_globObj;
static jerry_value_t g_attrsObj;
static jerry_value_t g_styleObj;
static uint16_t g_componentNameId;
static int32_t g_width = 400;
static int32_t g_height = 400;
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class PickerViewTddTest : public testing::Test {
#else
class PickerViewTddTest {
#endif
public:

    PickerViewTddTest();
    ~PickerViewTddTest(){}
    void SetUp();
    void TearDown();
    Component* GetRenderedComponent(uint16_t componentKeyId) const;
    static uint16_t SetCompnentNameId(const char* componentName);
    static void SetTextPickerRange();
    static void SetCharAttribute(const char* keyName, const char* attribute);
    void SetNumAttribute(const char* keyName, const int32_t attribute);

    void SetNumStyle(const char* keyName, const int32_t style);
    void ComponentPickerviewAttributeSetTest001();
    void ComponentPickerviewAttributeSetTest002();
    void ComponentPickerviewAttributeSetTest003();
    void ComponentPickerviewAttributeSetTest004();
    void ComponentPickerviewAttributeSetTest005();
    void RunTests();

private:
    RootComponentMock rootComponentMock_;
};
} // namespace ACELite
} // namesapce OHOS
#endif // ACELITE_PICKER_VIEW_TDD_TEST_H
