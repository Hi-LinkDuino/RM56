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

#ifndef OHOS_ACELITE_PERCENT_TDD_TEST_H
#define OHOS_ACELITE_PERCENT_TDD_TEST_H

#include "acelite_config.h"
#include "component.h"
#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#endif
#include "test_common.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class PercentTddTest : public testing::Test {
#else
class PercentTddTest {
#endif
public:
    PercentTddTest() = default;
    ~PercentTddTest() = default;
    void SetUp();
    void TearDown();
    void PercentParse001();
    void PercentParse002();
    void PercentParse003();
    void PercentParse004();
    void PercentParse005();
    void PercentParse006();
    void PercentParse007();
    void PercentParse008();
    void PercentParse009();
    void PercentParse010();
    void PercentParse011();
    void PercentSupport001();
    void PercentSupport002();
    void PercentSupport003();
    void PercentSupport004();
    void RunTests();
    void PrepareOptions(JSValue &option, JSValue &staticStyle) const;
    // following functions are used to setup the specific style items in percent format
    void SetStyleWithValue(JSValue target, const char *key, const char *value) const;
    void ConfigStylesWithRect(JSValue staticStyle, const char *widthValue, const char *heightValue) const;
    void ConfigStylesWithPosition(JSValue staticStyle, const char *topValue, const char *leftValue) const;
    void ConfigStylesWithMargin(JSValue staticStyle, const char *marginValue) const;
    void ConfigStylesWithMargins(JSValue staticStyle,
                                 const char *marginTopValue,
                                 const char *marginLeftValue,
                                 const char *marginRightValue,
                                 const char *marginBottomValue) const;
    void VerifyDimension(JSValue element, uint16_t keyID, int16_t targetPX) const;
    void DumpJSObjectJson(JSValue value) const;
    Component *BuildViewTree(JSValue component, int16_t parentWidth, int16_t parentHeight) const;

    static const char * const ATTR_STATIC_STYLE_NAME;
    static const char * const STYLE_WIDTH_NAME;
    static const char * const STYLE_HEIGHT_NAME;
    static const char * const STYLE_TOP_NAME;
    static const char * const STYLE_LEFT_NAME;
    static const char * const STYLE_MARGINTOP_NAME;
    static const char * const STYLE_MARGINLEFT_NAME;
    static const char * const STYLE_MARGINRIGHT_NAME;
    static const char * const STYLE_MARGINBOTTOM_NAME;
    static const char * const FUNC_CREATE_ELEMENT;
    static const uint8_t ARG_LENGTH_1 = 1;
    static const uint8_t ARG_LENGTH_2 = 2;
    static const uint8_t ARG_LENGTH_3 = 3;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_PERCENT_TDD_TEST_H
