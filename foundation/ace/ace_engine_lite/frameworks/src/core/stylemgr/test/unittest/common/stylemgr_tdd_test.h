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
#ifndef ACELITE_STYLEMGR_TDD_TEST_H
#define ACELITE_STYLEMGR_TDD_TEST_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

#include "acelite_config.h"
#include "component_factory.h"
#include "root_component_mock.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
static char* g_currentTestCaseName = nullptr;
static uint8_t g_assertRecord = 0;
static AppStyleManager* g_testStyleMgr = nullptr;
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class StyleMgrTddTest : public testing::Test {
#else
class StyleMgrTddTest {
#endif
public:
    StyleMgrTddTest();
    ~StyleMgrTddTest(){}
    void SetUp();
    void TearDown();
    void TestCaseB(const char* name) const;
    void JudeTestCaseResult() const;
    void ReleaseTestResources(const jerry_value_t* values,
                              uint8_t valueCount,
                              Component** components,
                              uint8_t componentCount) const;
    JSValue PrepareStyleSheet(const char* selectors,
                              const char* selectorName,
                              const char* propName,
                              uint16_t propValue) const;
    JSValue PrepareStyleSheetWithKeyValues(const char* selector,
                                       JSValue selectorObj,
                                       const char* selectorName,
                                       const uint8_t itemCount,
                                       char* keys[],
                                       const uint16_t values[]) const;
    JSValue PreparePesudoObjects(uint16_t value) const;

    JSValue PrepareStyleOption(const char* styleType, const char* propName, uint16_t propValue) const;
    JSValue PrepareAttrOption(const char* propName, const char* propValue) const;
    void AddValueToAttrOption(jerry_value_t attrOption, const char* propName, const char* propValue) const;
    void AddStrValueToOption(jerry_value_t option, const char* propName, const char* propValue) const;

    void ACELiteStyleManagerInlineStyle001();
    void ACELiteStyleManagerInlineStyle002();
    void ACELiteStyleManagerInlineStyle003();
    void ACELiteStyleManagerInlineStyle004();
    void ACELiteStyleManagerInlineStyle005();
    void ACELiteStyleManagerIDSelector001();
    void ACELiteStyleManagerIDSelector002();
    void ACELiteStyleManagerIDSelector003();
    void ACELiteStyleManagerIDSelector004();
    void ACELiteStyleManagerIDSelector005();
    void ACELiteStyleManagerClassSelector001();
    void ACELiteStyleManagerClassSelector002();
    void ACELiteStyleManagerClassSelector003();
    void ACELiteStyleManagerCSSPesudo001();
    void RunTests();

private:
    RootComponentMock rootComponentMock_;
};
} // namespace ACELite
} // namespace OHOS
#endif // ACELITE_STYLEMGR_TDD_TEST_H
