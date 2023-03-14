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

#include "percent_tdd_test.h"

#include "ace_log.h"
#include "component.h"
#include "component_utils.h"
#include "js_app_context.h"
#include "js_app_environment.h"
#include "js_fwk_common.h"
#include "number_parser.h"

namespace OHOS {
namespace ACELite {
const char * const PercentTddTest::ATTR_STATIC_STYLE_NAME = "staticStyle";
const char * const PercentTddTest::STYLE_WIDTH_NAME = "width";
const char * const PercentTddTest::STYLE_HEIGHT_NAME = "height";
const char * const PercentTddTest::STYLE_TOP_NAME = "top";
const char * const PercentTddTest::STYLE_LEFT_NAME = "left";
const char * const PercentTddTest::STYLE_MARGINTOP_NAME = "marginTop";
const char * const PercentTddTest::STYLE_MARGINLEFT_NAME = "marginLeft";
const char * const PercentTddTest::STYLE_MARGINRIGHT_NAME = "marginRight";
const char * const PercentTddTest::STYLE_MARGINBOTTOM_NAME = "marginBottom";
const char * const PercentTddTest::FUNC_CREATE_ELEMENT = "_c";
void PercentTddTest::SetUp()
{
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    if (env == nullptr) {
        // should never happen
        return;
    }
    env->InitJsFramework();
}

void PercentTddTest::TearDown()
{
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    JsAppContext *context = JsAppContext::GetInstance();
    if (env == nullptr || context == nullptr) {
        // should never happen
        return;
    }
    context->ReleaseStyles();
    env->Cleanup();
}

void PercentTddTest::PrepareOptions(JSValue &options, JSValue &staticStyle) const
{
    // prepare component options
    options = JSObject::Create();
    staticStyle = JSObject::Create();
    JSObject::Set(options, ATTR_STATIC_STYLE_NAME, staticStyle);
}

void PercentTddTest::SetStyleWithValue(JSValue target, const char *key, const char *value) const
{
    if ((JSUndefined::Is(target)) || (key == nullptr) || (value == nullptr)) {
        return;
    }
    JSValue propValue = JSString::Create(value);
    JSObject::Set(target, key, propValue);
    ReleaseJerryValue(propValue, VA_ARG_END_FLAG);
}

void PercentTddTest::ConfigStylesWithRect(JSValue staticStyle, const char *widthValue, const char *heightValue) const
{
    SetStyleWithValue(staticStyle, STYLE_WIDTH_NAME, widthValue);
    SetStyleWithValue(staticStyle, STYLE_HEIGHT_NAME, heightValue);
}

void PercentTddTest::ConfigStylesWithPosition(JSValue staticStyle, const char *topValue, const char *leftValue) const
{
    SetStyleWithValue(staticStyle, STYLE_TOP_NAME, topValue);
    SetStyleWithValue(staticStyle, STYLE_LEFT_NAME, leftValue);
}

void PercentTddTest::ConfigStylesWithMargin(JSValue staticStyle, const char *marginValue) const
{
    ConfigStylesWithMargins(staticStyle, marginValue, marginValue, marginValue, marginValue);
}

void PercentTddTest::ConfigStylesWithMargins(JSValue staticStyle,
                                             const char *marginTopValue,
                                             const char *marginLeftValue,
                                             const char *marginRightValue,
                                             const char *marginBottomValue) const
{
    SetStyleWithValue(staticStyle, STYLE_MARGINTOP_NAME, marginTopValue);
    SetStyleWithValue(staticStyle, STYLE_MARGINLEFT_NAME, marginLeftValue);
    SetStyleWithValue(staticStyle, STYLE_MARGINRIGHT_NAME, marginRightValue);
    SetStyleWithValue(staticStyle, STYLE_MARGINBOTTOM_NAME, marginBottomValue);
}

void PercentTddTest::DumpJSObjectJson(JSValue value) const
{
    if (JSUndefined::Is(value)) {
        return;
    }
    char *jsonStr = MallocStringOf(jerry_json_stringify(value));
    HILOG_DEBUG(HILOG_MODULE_ACE, "JSObject: %{public}s", jsonStr);
    ACE_FREE(jsonStr);
}

Component *PercentTddTest::BuildViewTree(JSValue element, int16_t parentWidth, int16_t parentHeight) const
{
    if (JSUndefined::Is(element)) {
        return nullptr;
    }

    Component *component = ComponentUtils::GetComponentFromBindingObject(element);
    if (component != nullptr) {
        ConstrainedParameter parentParameter = {parentWidth, parentHeight};
        Component::BuildViewTree(component, nullptr, parentParameter);
    }
    return component;
}

void PercentTddTest::VerifyDimension(JSValue element, uint16_t keyID, int16_t targetPX) const
{
    EXPECT_FALSE(JSUndefined::Is(element));
    Component *component = ComponentUtils::GetComponentFromBindingObject(element);
    EXPECT_FALSE(component == nullptr);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "can not fetch component from element");
        return;
    }
    Dimension dimension = component->GetDimension(keyID);
    EXPECT_EQ(dimension.type, DimensionType::TYPE_PIXEL);
    EXPECT_EQ(dimension.value.pixel, targetPX);
}

/**
 * @tc.name: AceliteFrameworkPercentParse001
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse001, TestSize.Level1)
{
    uint16_t strLength = 5;
    float percentValue = 0;
    /**
     * @tc.steps: step1. pass nullptr for parsing
     */
    bool result = NumberParser::ParsePercentValue(nullptr, strLength, percentValue);
    /**
     * @tc.steps: step2. check if parse failed
     */
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AceliteFrameworkPercentParse002
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse002, TestSize.Level1)
{
    uint16_t strLength = 1;
    const char *strBuffer = "100%";
    float percentValue = 0;
    /**
     * @tc.steps: step1. give the length small than the min length
     */
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    /**
     * @tc.steps: step2. check if parse failed
     */
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AceliteFrameworkPercentParse003
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse003, TestSize.Level1)
{
    uint16_t strLength = 4;
    const char *strBuffer = "100%";
    float percentValue = 0;
    /**
     * @tc.steps: step1. give the corrent string and length
     */
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    EXPECT_TRUE(result);
    float correctValue = 100;
    /**
     * @tc.steps: step2. check if parse successfully
     */
    EXPECT_EQ(percentValue, correctValue);
}

/**
 * @tc.name: AceliteFrameworkPercentParse004
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse004, TestSize.Level1)
{
    uint16_t strLength = 7;
    /**
     * @tc.steps: step1. give one string not in digit format
     */
    const char *strBuffer = "abcdef%";
    float percentValue = 0;
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    /**
     * @tc.steps: step2. check if parse failed
     */
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AceliteFrameworkPercentParse005
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse005, TestSize.Level1)
{
    uint16_t strLength = 20;
    /**
     * @tc.steps: step1. give one string which is too long
     */
    const char *strBuffer = "1567890123343433434%";
    float percentValue = 0;
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    /**
     * @tc.steps: step2. check if parse failed
     */
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AceliteFrameworkPercentParse006
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse006, TestSize.Level1)
{
    uint16_t strLength = 4;
    /**
     * @tc.steps: step1. give one string which includes no-digit charater
     */
    const char *strBuffer = "50a%";
    float percentValue = 0;
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    /**
     * @tc.steps: step2. check if parse failed
     */
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AceliteFrameworkPercentParse007
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse007, TestSize.Level1)
{
    uint16_t strLength = 4;
    /**
     * @tc.steps: step1. give one negative number
     */
    const char *strBuffer = "-40%";
    float percentValue = 0;
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    /**
     * @tc.steps: step2. check if parse successfully and the result is negative number
     */
    EXPECT_TRUE(result);
    float correctValue = -40;
    EXPECT_EQ(percentValue, correctValue);
}

/**
 * @tc.name: AceliteFrameworkPercentParse008
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse008, TestSize.Level1)
{
    uint16_t strLength = 6;
    /**
     * @tc.steps: step1. give one string which is not number and not including %
     */
    const char *strBuffer = "abcdef";
    float percentValue = 0;
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    /**
     * @tc.steps: step2. check if parse failed
     */
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AceliteFrameworkPercentParse009
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse009, TestSize.Level1)
{
    uint16_t strLength = 100;
    /**
     * @tc.steps: step1. give one string which is not number and including %
     */
    const char *strBuffer = "abcdef%";
    float percentValue = 0;
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    /**
     * @tc.steps: step2. check if parse failed
     */
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AceliteFrameworkPercentParse010
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. give one string which is bigger than float range
     */
    const char *strBuffer = "123456789012345%";
    uint16_t strLength = 16;
    float percentValue = 0;
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    EXPECT_TRUE(result);
    /**
     * @tc.steps: step2. check the value is correct
     */
    float correctValue = static_cast<float>(123456789012345);
    EXPECT_EQ(percentValue, correctValue);
}

/**
 * @tc.name: AceliteFrameworkPercentParse011
 * @tc.desc: Verify the percent converting process
 */
HWTEST_F(PercentTddTest, PercentParse011, TestSize.Level1)
{
    float percentValue = 0;
    uint16_t strLength = 16;
    /**
     * @tc.steps: step1. give one very small value
     */
    const char *strBuffer = "-12345678901234%";
    bool result = NumberParser::ParsePercentValue(strBuffer, strLength, percentValue);
    EXPECT_TRUE(result);
    /**
     * @tc.steps: step2. check the value is correct
     */
    float correctValue = static_cast<float>(-12345678901234);
    EXPECT_EQ(percentValue, correctValue);
}

/**
 * @tc.name: AceliteFrameworkPercentSupport001
 * @tc.desc: Verify the percent processing on div.
 */
HWTEST_F(PercentTddTest, PercentSupport001, TestSize.Level1)
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. prepare child component
     */
    JSValue textTag = JSString::Create("text");
    JSValue textOption;
    JSValue textStaticStyle;
    PrepareOptions(textOption, textStaticStyle);
    ConfigStylesWithRect(textStaticStyle, "50%", "50%");
    JSValue textArgs[ARG_LENGTH_2] = {textTag, textOption};
    JSValue textEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, textArgs, ARG_LENGTH_2);
    /**
     * @tc.steps: step2. prepare parent component
     */
    JSValue divTag = JSString::Create("div");
    JSValue divOption;
    JSValue divStaticStyle;
    PrepareOptions(divOption, divStaticStyle);
    ConfigStylesWithRect(divStaticStyle, "100%", "100%");
    JSValue divChildren = JSArray::Create(0);
    JSArray::Push(divChildren, textEl);
    JSValue divArgs[ARG_LENGTH_3] = {divTag, divOption, divChildren};
    JSValue divEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, divArgs, ARG_LENGTH_3);
    /**
     * @tc.steps: step3. trigger render and build
     */
    const int16_t screenWidth = 454;
    const int16_t screenHeight = 454;
    Component *divComponent = BuildViewTree(divEl, screenWidth, screenHeight);
    /**
     * @tc.steps: step4. verify
     */
    VerifyDimension(divEl, K_WIDTH, screenWidth);
    const int16_t halfNum = 2;
    VerifyDimension(textEl, K_HEIGHT, (screenHeight / halfNum));
    /**
     * @tc.steps: step5. release resource
     */
    ComponentUtils::ReleaseComponents(divComponent);
    ReleaseJerryValue(textTag, textStaticStyle, textOption, textEl, VA_ARG_END_FLAG);
    ReleaseJerryValue(divTag, divStaticStyle, divOption, divEl, divChildren, VA_ARG_END_FLAG);
    TDD_CASE_END();
}

/**
 * @tc.name: AceliteFrameworkPercentSupport002
 * @tc.desc: Verify the percent processing on component.
 */
HWTEST_F(PercentTddTest, PercentSupport002, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. prepare child component
     */
    JSValue textTag = JSString::Create("text");
    JSValue textOption;
    JSValue textStaticStyle;
    PrepareOptions(textOption, textStaticStyle);
    ConfigStylesWithPosition(textStaticStyle, "10%", "10%");
    JSValue textArgs[ARG_LENGTH_2] = {textTag, textOption};
    JSValue textEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, textArgs, ARG_LENGTH_2);
    /**
     * @tc.steps: step2. prepare parent component
     */
    JSValue stackTag = JSString::Create("stack");
    JSValue stackOption;
    JSValue stackStaticStyle;
    PrepareOptions(stackOption, stackStaticStyle);
    ConfigStylesWithRect(stackStaticStyle, "50%", "50%");
    JSValue stackChildren = JSArray::Create(0);
    JSArray::Push(stackChildren, textEl);
    JSValue stackArgs[ARG_LENGTH_3] = {stackTag, stackOption, stackChildren};
    JSValue stackEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, stackArgs, ARG_LENGTH_3);
    /**
     * @tc.steps: step3. trigger render and build
     */
    const int16_t screenWidth = 1024;
    const int16_t screenHeight = 768;
    Component *stackComponent = BuildViewTree(stackEl, screenWidth, screenHeight);
    /**
     * @tc.steps: step4. verify
     */
    const int16_t halfNum = 2;
    const int16_t marginPercentNum = 10;
    VerifyDimension(stackEl, K_WIDTH, (screenWidth / halfNum));
    VerifyDimension(textEl, K_TOP, ((screenHeight / halfNum) / marginPercentNum));
    /**
     * @tc.steps: step5. release resource
     */
    ComponentUtils::ReleaseComponents(stackComponent);
    ReleaseJerryValue(textTag, textStaticStyle, textOption, textEl, VA_ARG_END_FLAG);
    ReleaseJerryValue(stackTag, stackStaticStyle, stackOption, stackEl, stackChildren, VA_ARG_END_FLAG);
    TDD_CASE_END();
}

/**
 * @tc.name: AceliteFrameworkPercentSupport003
 * @tc.desc: Verify the percent processing on component.
 */
HWTEST_F(PercentTddTest, PercentSupport003, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. prepare child component
     */
    JSValue labelTag = JSString::Create("text");
    JSValue labelOption;
    JSValue labelStaticStyle;
    PrepareOptions(labelOption, labelStaticStyle);
    ConfigStylesWithMargins(labelStaticStyle, "10%", "10%", "10%", "10%");
    JSValue labelArgs[ARG_LENGTH_2] = {labelTag, labelOption};
    JSValue textEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, labelArgs, ARG_LENGTH_2);
    /**
     * @tc.steps: step2. prepare parent component
     */
    JSValue containerTag = JSString::Create("div");
    JSValue containerOption;
    JSValue containerStaticStyle;
    PrepareOptions(containerOption, containerStaticStyle);
    ConfigStylesWithRect(containerStaticStyle, "100%", "100%");
    JSValue containerChildren = JSArray::Create(0);
    JSArray::Push(containerChildren, textEl);
    JSValue containerArgs[ARG_LENGTH_3] = {containerTag, containerOption, containerChildren};
    JSValue containerEl = JSGlobal::Call(FUNC_CREATE_ELEMENT, containerArgs, ARG_LENGTH_3);
    /**
     * @tc.steps: step3. trigger render and build
     */
    const int16_t screenWidth = 600;
    const int16_t screenHeight = 800;
    Component *containerComponent = BuildViewTree(containerEl, screenWidth, screenHeight);
    /**
     * @tc.steps: step4. verify
     */
    const int16_t marginPercent = 10;
    VerifyDimension(textEl, K_MARGIN_TOP, (screenHeight / marginPercent));
    /**
     * @tc.steps: step5. release resource
     */
    ComponentUtils::ReleaseComponents(containerComponent);
    ReleaseJerryValue(labelTag, labelStaticStyle, labelOption, textEl, VA_ARG_END_FLAG);
    ReleaseJerryValue(containerTag, containerStaticStyle, containerOption, containerEl, containerChildren,
                      VA_ARG_END_FLAG);
    TDD_CASE_END();
}

/**
 * used for debugging TDD implementation on simulator
 */
void PercentTddTest::RunTests()
{
#ifndef TDD_ASSERTIONS
    PercentParse001();
    PercentParse002();
    PercentParse003();
    PercentParse004();
    PercentParse005();
    PercentParse006();
    PercentParse007();
    PercentParse008();
    PercentParse009();
    PercentParse010();
    PercentParse011();
    PercentSupport001();
    PercentSupport002();
    PercentSupport003();
#endif // TDD_ASSERTIONS
}
} // namespace ACELite
} // namespace OHOS
