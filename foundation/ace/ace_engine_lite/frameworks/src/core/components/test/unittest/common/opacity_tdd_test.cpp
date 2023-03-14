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

#include "opacity_tdd_test.h"
#include "acelite_config.h"
#include "component_factory.h"
#include "div_component.h"
#include "js_app_environment.h"
#include "root_view.h"
#include "text_component.h"
#include "ui_label.h"

namespace OHOS {
namespace ACELite {
OpacityTddTest::OpacityTddTest():BaseTest()
{
    componentNameId_ = KeyParser::ParseKeyId("div");
}

/**
 * @tc.name:SetOpacityTest001
 * @tc.desc: Verify div Opacity 0.6 set successful.
 * @tc.type FUNC
 */
void OpacityTddTest::SetOpacityTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set div opacity style value = 0.6,
     * check ui view opacityvalue = 0.6
     */
    const double expectOpacityValue = 0.6;
    SetOpaScale(styleObj_, expectOpacityValue);
    Component* divComponent = reinterpret_cast<DivComponent *>(GetRenderedComponent(componentNameId_));
    UIView* uiView = reinterpret_cast<UIView *>(divComponent->GetComponentRootView());
    EXPECT_EQ(uiView->GetOpaScale(), OPA_OPAQUE * expectOpacityValue);
    TDD_CASE_END();
}

/**
 * @tc.name:SetOpacityTest002
 * @tc.desc: Set exception div Opacity.
 * @tc.type FUNC
 */
void OpacityTddTest::SetOpacityTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set div exception opacity style value = 128,
     * check ui view opacityvalue = 1.0
     */
    const double exceptionOpacityValue1 = 128;
    const double maxOpacityValue = 1.0;
    const double minOpacityValue = 0.0;
    SetOpaScale(styleObj_, exceptionOpacityValue1);
    Component* divComponent = reinterpret_cast<DivComponent *>(GetRenderedComponent(componentNameId_));
    UIView* uiView = (divComponent->GetComponentRootView());
    EXPECT_NE(uiView->GetOpaScale(), OPA_OPAQUE * exceptionOpacityValue1);
    EXPECT_EQ(uiView->GetOpaScale(), OPA_OPAQUE * maxOpacityValue);

    /**
     * @tc.steps: step2. set div exception opacity style value = -123456,
     * check ui view opacityvalue = 0.0
     */
    const double exceptionOpacityValue2 = -123456;
    SetOpaScale(styleObj_, exceptionOpacityValue2);
    const char * const opacity = "opacity";
    UpdateNumAttributeOrStyleValue(divComponent, opacity, exceptionOpacityValue2, false);
    EXPECT_NE(uiView->GetOpaScale(), OPA_OPAQUE * exceptionOpacityValue2);
    EXPECT_EQ(uiView->GetOpaScale(), OPA_OPAQUE * minOpacityValue);

    TDD_CASE_END();
}


/**
 * @tc.name:SetOpacityTest003
 * @tc.desc: Set div & children text Opacity.
 * @tc.type FUNC
 */
void OpacityTddTest::SetOpacityTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set div opacity style value = 0.6,
     */
    const double divOpacityValue = 0.6;
    SetOpaScale(styleObj_, divOpacityValue);
    /**
     * @tc.steps: step2. set text opacity style value = 0.8,
     */
    jerry_value_t textObject = jerry_create_object();
    jerry_value_t  textStyleObj = jerry_create_object();
    JerrySetNamedProperty(textObject, "staticStyle", textStyleObj);
    const double textOpacityValue = 0.8;
    SetOpaScale(textStyleObj, textOpacityValue);
    /**
     * @tc.steps: step3. get div view, render and check.
     */
    UIView* uiView = GetContainerView(componentNameId_, globObj_, textObject);
    if (uiView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_NE(uiView->GetOpaScale() * divOpacityValue, (int)(OPA_OPAQUE * textOpacityValue * divOpacityValue));
    jerry_release_value(textObject);
    jerry_release_value(textStyleObj);
    TDD_CASE_END();
}

/**
 * @tc.name:SetOpacityTest004
 * @tc.desc: Set div & children text Opacity.
 * @tc.type FUNC
 */
void OpacityTddTest::SetOpacityTest004()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1. set text opacity style value = 0.8,
     */
    jerry_value_t textObject = jerry_create_object();
    jerry_value_t  textStyleObj = jerry_create_object();
    JerrySetNamedProperty(textObject, "staticStyle", textStyleObj);
    const double textOpacityValue = 0.8;
    SetOpaScale(textStyleObj, textOpacityValue);

    /**
     * @tc.steps: step2. set div opacity style value = 0.6,
     */
    jerry_value_t divObject = jerry_create_object();
    jerry_value_t  divStyleObj = jerry_create_object();
    JerrySetNamedProperty(divObject, "staticStyle", divStyleObj);
    const double divOpacityValue1 = 0.6;
    SetOpaScale(divStyleObj, divOpacityValue1);

    ComponentFactory::CreateComponent(componentNameId_, divObject, textObject);

    const double divOpacityValue2 = 0.5;
    SetOpaScale(styleObj_, divOpacityValue2);
    /**
     * @tc.steps: step3. get div view, render and check.
     */
    UIView* uiView = GetContainerView(componentNameId_, globObj_, divObject);
    if (uiView == nullptr) {
        TDD_CASE_END();
        return;
    }
    EXPECT_NE(uiView->GetOpaScale() * textOpacityValue * divOpacityValue2,
        (int)(OPA_OPAQUE * textOpacityValue * divOpacityValue1 * divOpacityValue2));
    jerry_release_value(textObject);
    jerry_release_value(textStyleObj);
    jerry_release_value(divObject);
    jerry_release_value(divStyleObj);

    TDD_CASE_END();
}

/**
 * @tc.name:SetOpacityTest005
 * @tc.desc: Set text & text color Opacity.
 * @tc.type FUNC
 */
void OpacityTddTest::SetOpacityTest005()
{
    uint16_t textComponentName = KeyParser::ParseKeyId("text");
    const double componentOpacityValue = 0.6;
    SetOpaScale(styleObj_, componentOpacityValue);
    const char * const colorKey = "color";
    const char * const colorValue = "#66666666";
    const uint8_t colorOpacity = 102;
    JerrySetStringProperty(styleObj_, colorKey, colorValue);
    Component* textComponent = reinterpret_cast<TextComponent *>(GetRenderedComponent(textComponentName));
    UILabel* uiLabel = reinterpret_cast<UILabel *>(textComponent->GetComponentRootView());
    EXPECT_EQ((int)(uiLabel->GetOpaScale() * (uiLabel->GetStyle(STYLE_TEXT_OPA)) / OPA_OPAQUE),
        (int)(componentOpacityValue * colorOpacity));
    TDD_CASE_END();
}

/**
 * @tc.name:SetOpacityTest006
 * @tc.desc: Set div & border color Opacity.
 * @tc.type FUNC
 */
void OpacityTddTest::SetOpacityTest006()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set div opacity style value = 0.6,
     * check ui view opacityvalue = 0.6
     */
    const double componentOpacityValue = 0.6;
    SetOpaScale(styleObj_, componentOpacityValue);
    const char * const borderColorKey = "borderColor";
    const char * const borderColorValue = "#66666666";
    const uint8_t colorOpacity = 102;
    JerrySetStringProperty(styleObj_, borderColorKey, borderColorValue);
    Component* divComponent = reinterpret_cast<DivComponent *>(GetRenderedComponent(componentNameId_));
    FlexLayout* uiView = reinterpret_cast<FlexLayout *>(divComponent->GetComponentRootView());
    EXPECT_EQ(uiView->GetOpaScale() * (uiView->GetStyle(STYLE_BORDER_OPA)) / OPA_OPAQUE,
        (int)(componentOpacityValue * colorOpacity));
    TDD_CASE_END();
}

/**
 * @tc.name:SetOpacityTest007
 * @tc.desc: Set div & background color Opacity.
 * @tc.type FUNC
 */
void OpacityTddTest::SetOpacityTest007()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set div opacity style value = 0.6,
     * check ui view opacityvalue = 0.6
     */
    const double componentOpacityValue = 0.6;
    SetOpaScale(styleObj_, componentOpacityValue);

    const char * const backgroundColorKey = "backgroundColor";
    const char * const backgroundColorValue = "#66666666";
    const uint8_t colorOpacity = 102;
    JerrySetStringProperty(styleObj_, backgroundColorKey, backgroundColorValue);

    Component* divComponent = reinterpret_cast<DivComponent *>(GetRenderedComponent(componentNameId_));
    FlexLayout* uiView = reinterpret_cast<FlexLayout *>(divComponent->GetComponentRootView());

    EXPECT_EQ((int)(uiView->GetOpaScale() * (uiView->GetStyle(STYLE_BACKGROUND_OPA)) / OPA_OPAQUE),
        (int)(componentOpacityValue * colorOpacity));
    TDD_CASE_END();
}

UIView* OpacityTddTest::GetContainerView(uint16_t componentKeyId, jerry_value_t options, jerry_value_t children) const
{
    Component* component = ComponentFactory::CreateComponent(componentKeyId, options, children);
    if (component == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create component error.");
        return nullptr;
    }
    component->Render();
    Component* divComponent = (reinterpret_cast<DivComponent *>(component));
    FlexLayout* uiView = reinterpret_cast<FlexLayout *>(divComponent->GetComponentRootView());
    return uiView;
}

void OpacityTddTest::SetOpaScale(jerry_value_t styleObj, double inputOpacityValue)
{
    const double expectOpacityValue = inputOpacityValue;
    const char * const opacity = "opacity";
    jerry_value_t opacityKey = jerry_create_string((const jerry_char_t*)opacity);
    jerry_value_t opacityValue = jerry_create_number(expectOpacityValue);
    jerry_set_property(styleObj, opacityKey, opacityValue);
    jerry_release_value(opacityKey);
    jerry_release_value(opacityValue);
}


void OpacityTddTest::RunTests()
{
    SetOpacityTest001();
    SetOpacityTest002();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name:SetOpacityTest001
 * @tc.desc: Verify div Opacity 0.6 set successful.
 * @tc.type FUNC
 */
HWTEST_F(OpacityTddTest, textOpacity001, TestSize.Level1)
{
    OpacityTddTest::SetOpacityTest001();
}

/**
 * @tc.name:SetOpacityTest002
 * @tc.desc: Set exception div Opacity.
 * @tc.type FUNC
 */
HWTEST_F(OpacityTddTest, textOpacity002, TestSize.Level1)
{
    OpacityTddTest::SetOpacityTest002();
}

/**
 * @tc.name:SetOpacityTest003
 * @tc.desc: Set div & children text Opacity.
 * @tc.type FUNC
 */
HWTEST_F(OpacityTddTest, textOpacity003, TestSize.Level0)
{
    OpacityTddTest::SetOpacityTest003();
}

/**
 * @tc.name:SetOpacityTest004
 * @tc.desc: Set div & children text Opacity.
 * @tc.type FUNC
 */
HWTEST_F(OpacityTddTest, textOpacity004, TestSize.Level1)
{
    OpacityTddTest::SetOpacityTest004();
}

/**
 * @tc.name:SetOpacityTest005
 * @tc.desc: Set text & text color Opacity.
 * @tc.type FUNC
 */
HWTEST_F(OpacityTddTest, textOpacity005, TestSize.Level1)
{
    OpacityTddTest::SetOpacityTest005();
}

/**
 * @tc.name:SetOpacityTest006
 * @tc.desc: Set div & border color Opacity.
 * @tc.type FUNC
 */
HWTEST_F(OpacityTddTest, textOpacity006, TestSize.Level1)
{
    OpacityTddTest::SetOpacityTest006();
}

/**
 * @tc.name:SetOpacityTest007
 * @tc.desc: Set div & background color Opacity.
 * @tc.type FUNC
 */
HWTEST_F(OpacityTddTest, textOpacity007, TestSize.Level0)
{
    OpacityTddTest::SetOpacityTest007();
}
#endif // TDD_ASSERTIONS
} // namespace ACELite
} // namespace OHOS
