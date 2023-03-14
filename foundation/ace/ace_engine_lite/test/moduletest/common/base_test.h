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

#ifndef OHOS_ACELITE_TEST_CONSTANTS_H
#define OHOS_ACELITE_TEST_CONSTANTS_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

#include "component_factory.h"
#include "js_app_environment.h"
#include "root_component_mock.h"
#include "root_view.h"
#include "stylemgr/app_style_item.h"
#include "stylemgr/app_style_manager.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class BaseTest : public testing::Test {
#else
class BaseTest {
#endif
public:
    BaseTest();
    virtual ~BaseTest(){}
    void SetUp();
    void TearDown();

protected:
    jerry_value_t globObj_;
    jerry_value_t attrsObj_;
    jerry_value_t styleObj_;
    uint16_t componentNameId_;

    Component *GetRenderedComponent(uint16_t componentKeyId) const;
    ColorType GetRGBColor(int32_t colorIntValue) const;
    void UpdateNumAttributeOrStyleValue(Component *component,
                                        const char *attributeName,
                                        const int32_t newNumValue,
                                        bool isToSetAttribute) const;
    void UpdateCharAttributeOrStyleValue(Component *component,
                                         const char *attributeName,
                                         const char *newCharValue,
                                         bool isToSetAttribute) const;
    uint16_t SetCompnentNameId(const char *componentName);

private:
    RootComponentMock rootComponentMock_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_TEST_CONSTANTS_H
