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

#include "marquee_tdd_test.h"
#include "marquee_component.h"
#include "themes/theme.h"
#include "themes/theme_manager.h"

namespace OHOS {
namespace ACELite {
void MarqueeTddTest::ComponentMarqueeStyleSetTest006()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set color = 0
     */
    const char *color = "color";
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(color));
    jerry_value_t value = jerry_create_number(0);
    jerry_set_property(styleObj_, key, value);
    Component *component = GetRenderedComponent(componentNameId_);
    UIView *marqueeView = component->GetComponentRootView();

    /**
     * @tc.expected: step2. get color = 0
     */
    ColorType colorRGB = GetRGBColor(0);
    ColorType colorRGBSet;
    colorRGBSet.full = marqueeView->GetStyle(STYLE_TEXT_COLOR);
    if (colorRGB.red != colorRGBSet.red || colorRGB.green != colorRGBSet.green || colorRGB.blue != colorRGBSet.blue ||
        colorRGB.full != colorRGBSet.full) {
        printf("Component_Marquee_StyleSet_Test_006 fail\n");
    } else {
        printf("Component_Marquee_StyleSet_Test_006 pass\n");
    }
    EXPECT_TRUE((colorRGB.red == colorRGBSet.red) && (colorRGB.green == colorRGBSet.green) &&
                (colorRGB.blue == colorRGBSet.blue) && (colorRGB.full == colorRGBSet.full));
    jerry_release_value(value);
    delete component;
    component = nullptr;

    /**
     * @tc.expected: step3. set color = '0'
     */
    const char *colorValue = "0";
    value = jerry_create_string(reinterpret_cast<const jerry_char_t *>(colorValue));
    jerry_set_property(styleObj_, key, value);
    component = GetRenderedComponent(componentNameId_);
    marqueeView = component->GetComponentRootView();
    jerry_release_value(key);
    jerry_release_value(value);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void MarqueeTddTest::ComponentMarqueeStyleSetTest007()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set color = 16777215
     */
    const int colorSize = 16777215;
    const char color[] = "color";
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(color));
    jerry_value_t value = jerry_create_number(colorSize);
    jerry_set_property(styleObj_, key, value);
    Component *component = GetRenderedComponent(componentNameId_);
    UIView *marqueeView = component->GetComponentRootView();

    /**
     * @tc.expected: step2. get color = 16777215
     */
    ColorType colorRGB = GetRGBColor(colorSize);
    ColorType colorRGBSet;
    colorRGBSet.full = marqueeView->GetStyle(STYLE_TEXT_COLOR);
    if (colorRGB.red != colorRGBSet.red || colorRGB.green != colorRGBSet.green || colorRGB.blue != colorRGBSet.blue ||
        colorRGB.full != colorRGBSet.full) {
        printf("Component_Marquee_StyleSet_Test_007 fail\n");
    } else {
        printf("Component_Marquee_StyleSet_Test_007 pass\n");
    }
    EXPECT_TRUE((colorRGB.red == colorRGBSet.red) && (colorRGB.green == colorRGBSet.green) &&
                (colorRGB.blue == colorRGBSet.blue) && (colorRGB.full == colorRGBSet.full));
    jerry_release_value(value);
    delete component;
    component = nullptr;

    /**
     * @tc.expected: step3. set color = '16777215'
     */
    const char *colorVal = "16777215";
    value = jerry_create_string(reinterpret_cast<const jerry_char_t *>(colorVal));
    jerry_set_property(styleObj_, key, value);
    component = GetRenderedComponent(componentNameId_);
    marqueeView = component->GetComponentRootView();
    jerry_release_value(key);
    jerry_release_value(value);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void MarqueeTddTest::ComponentMarqueeStyleSetTest008()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set color = 65280
     */
    const int colorSize = 65280;
    const char color[] = "color";
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(color));
    jerry_value_t value = jerry_create_number(colorSize);
    jerry_set_property(styleObj_, key, value);
    Component *component = GetRenderedComponent(componentNameId_);
    UIView *marqueeView = component->GetComponentRootView();

    /**
     * @tc.expected: step2. get color = 65280
     */
    ColorType colorRGB = GetRGBColor(colorSize);
    ColorType colorRGBSet;
    colorRGBSet.full = marqueeView->GetStyle(STYLE_TEXT_COLOR);
    if (colorRGB.red != colorRGBSet.red || colorRGB.green != colorRGBSet.green || colorRGB.blue != colorRGBSet.blue ||
        colorRGB.full != colorRGBSet.full) {
        printf("Component_Marquee_StyleSet_Test_008 fail\n");
    } else {
        printf("Component_Marquee_StyleSet_Test_008 pass\n");
    }
    EXPECT_TRUE((colorRGB.red == colorRGBSet.red) && (colorRGB.green == colorRGBSet.green) &&
                (colorRGB.blue == colorRGBSet.blue) && (colorRGB.full == colorRGBSet.full));
    jerry_release_value(value);
    delete component;
    component = nullptr;

    /**
     * @tc.expected: step3. set color = '65280'
     */
    const char *colorVal = "65280";
    value = jerry_create_string(reinterpret_cast<const jerry_char_t *>(colorVal));
    jerry_set_property(styleObj_, key, value);
    component = GetRenderedComponent(componentNameId_);
    marqueeView = component->GetComponentRootView();
    jerry_release_value(key);
    jerry_release_value(value);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void MarqueeTddTest::ComponentMarqueeStyleSetTest009()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set color = -1
     */
    const char *color = "color";
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(color));
    int colorVal = -1;
    jerry_value_t value = jerry_create_number(colorVal);
    jerry_set_property(styleObj_, key, value);
    Component *component = GetRenderedComponent(componentNameId_);
    UIView *marqueeView = component->GetComponentRootView();

    /**
     * @tc.expected: step2. get color = -1
     */
    ColorType colorRGB = GetRGBColor(-1);
    ColorType colorRGBSet;
    colorRGBSet.full = marqueeView->GetStyle(STYLE_TEXT_COLOR);
    if (colorRGB.red != colorRGBSet.red || colorRGB.green != colorRGBSet.green || colorRGB.blue != colorRGBSet.blue ||
        colorRGB.full != colorRGBSet.full) {
        printf("Component_Marquee_StyleSet_Test_009 fail\n");
    } else {
        printf("Component_Marquee_StyleSet_Test_009 pass\n");
    }
    EXPECT_TRUE((colorRGB.red == colorRGBSet.red) && (colorRGB.green == colorRGBSet.green) &&
                (colorRGB.blue == colorRGBSet.blue) && (colorRGB.full == colorRGBSet.full));
    jerry_release_value(value);
    delete component;
    component = nullptr;

    /**
     * @tc.expected: step3. set color = '-1'
     */
    const char *strVal = "-1";
    value = jerry_create_string(reinterpret_cast<const jerry_char_t *>(strVal));
    jerry_set_property(styleObj_, key, value);
    component = GetRenderedComponent(componentNameId_);
    marqueeView = component->GetComponentRootView();
    jerry_release_value(key);
    jerry_release_value(value);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void MarqueeTddTest::ComponentMarqueeStyleSetTest010()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set color = 16777216
     */
    const int colorSize = 16777216;
    const char *color = "color";
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(color));
    jerry_value_t value = jerry_create_number(colorSize);
    jerry_set_property(styleObj_, key, value);
    Component *component = GetRenderedComponent(componentNameId_);
    UIView *marqueeView = component->GetComponentRootView();

    /**
     * @tc.expected: step2. get color = 16777216
     */
    ColorType colorRGB = GetRGBColor(colorSize);
    ColorType colorRGBSet;
    colorRGBSet.full = marqueeView->GetStyle(STYLE_TEXT_COLOR);
    if (colorRGB.red != colorRGBSet.red || colorRGB.green != colorRGBSet.green || colorRGB.blue != colorRGBSet.blue ||
        colorRGB.full != colorRGBSet.full) {
        printf("Component_Marquee_StyleSet_Test_010 fail\n");
    } else {
        printf("Component_Marquee_StyleSet_Test_010 pass\n");
    }
    EXPECT_TRUE((colorRGB.red == colorRGBSet.red) && (colorRGB.green == colorRGBSet.green) &&
                (colorRGB.blue == colorRGBSet.blue) && (colorRGB.full == colorRGBSet.full));
    jerry_release_value(value);
    delete component;
    component = nullptr;

    /**
     * @tc.expected: step3. set color = '16777216'
     */
    const char *strVal = "16777216";
    value = jerry_create_string(reinterpret_cast<const jerry_char_t *>(strVal));
    jerry_set_property(styleObj_, key, value);
    component = GetRenderedComponent(componentNameId_);
    marqueeView = component->GetComponentRootView();
    jerry_release_value(key);
    jerry_release_value(value);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void MarqueeTddTest::ComponentMarqueeStyleSetTest011()
{
    TDD_CASE_BEGIN();
    const char *value = "value";
    const char *textVal = "hello world";
    jerry_value_t textKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    jerry_value_t textValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(textVal));
    jerry_release_value(jerry_set_property(attrsObj_, textKey, textValue));

    /**
     * @tc.steps: step1. set font size = 30
     */
    const int initFontSize = 30;
    const char *fontSizeName = "fontSize";
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(fontSizeName));
    jerry_value_t fontSizeVal = jerry_create_number(initFontSize);
    jerry_release_value(jerry_set_property(styleObj_, key, fontSizeVal));

    Component *component = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    UILabel *uiLabel = reinterpret_cast<UILabel *>(component->GetComponentRootView());
    /**
     * @tc.expected: step2. get font size = 16
     */
    uint16_t targetFontSize = 16;
    uint16_t fontSize = uiLabel->GetFontId();
    if (targetFontSize == fontSize) {
        printf("Component_Marquee_StyleSet_Test_011 pass\n");
    } else {
        printf("Component_Marquee_StyleSet_Test_011 fail\n");
    }
    EXPECT_TRUE(targetFontSize == fontSize);

    jerry_release_value(textKey);
    jerry_release_value(textValue);
    jerry_release_value(key);
    jerry_release_value(fontSizeVal);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: ComponentMarqueeStyleSetTest012
 * @tc.desc: Verify Marquee style font size set correct value.
 */
void MarqueeTddTest::ComponentMarqueeStyleSetTest012()
{
    TDD_CASE_BEGIN();
    const char *value = "value";
    const char *textVal = "hello world";
    jerry_value_t textKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    jerry_value_t textValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(textVal));
    jerry_release_value(jerry_set_property(attrsObj_, textKey, textValue));

    /**
     * @tc.steps: step1. set font size = 38
     */
    const int initFontSize = 38;
    const char *fontSizeName = "fontSize";
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(fontSizeName));
    jerry_value_t fontSizeVal = jerry_create_number(initFontSize);
    jerry_release_value(jerry_set_property(styleObj_, key, fontSizeVal));
    Component *component = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    UILabel *uiLabel = reinterpret_cast<UILabel *>(component->GetComponentRootView());

    /**
     * @tc.expected: step2. get font size = 18
     */
    uint16_t targetFontSize = 18;
    uint16_t fontSize = uiLabel->GetFontId();
    if (targetFontSize == fontSize) {
        printf("Component_Marquee_StyleSet_Test_012 pass\n");
    } else {
        printf("Component_Marquee_StyleSet_Test_012 fail\n");
    }
    EXPECT_TRUE(targetFontSize == fontSize);

    jerry_release_value(textKey);
    jerry_release_value(textValue);
    jerry_release_value(key);
    jerry_release_value(fontSizeVal);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: ComponentMarqueeStyleSetTest013
 * @tc.desc: Verify Marquee style font size set incorrect value.
 */
void MarqueeTddTest::ComponentMarqueeStyleSetTest013()
{
    TDD_CASE_BEGIN();
    const char *value = "value";
    const char *textVal = "hello world";
    jerry_value_t textKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    jerry_value_t textValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(textVal));
    jerry_release_value(jerry_set_property(attrsObj_, textKey, textValue));

    /**
     * @tc.steps: step1. set font size = 40
     */
    Theme *theme = ThemeManager::GetInstance().GetCurrent();
    Style style;
    if (theme != nullptr) {
        style = theme->GetLabelStyle();
    } else {
        style = StyleDefault::GetLabelStyle();
    }
    const int initFontSize = 40;
    const char *fontSizeName = "fontSize";
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(fontSizeName));
    jerry_value_t fontSizeVal = jerry_create_number(initFontSize);
    jerry_release_value(jerry_set_property(styleObj_, key, fontSizeVal));
    Component *component = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    UILabel *uiLabel = reinterpret_cast<UILabel *>(component->GetComponentRootView());

    /**
     * @tc.expected: step2. get font size = 67
     */
    uint16_t fontSize = uiLabel->GetFontId();
    if (fontSize == style.font_) {
        printf("Component_Marquee_StyleSet_Test_013 pass\n");
    } else {
        printf("Component_Marquee_StyleSet_Test_013 fail\n");
    }
    EXPECT_TRUE(fontSize == style.font_);

    jerry_release_value(textKey);
    jerry_release_value(textValue);
    jerry_release_value(key);
    jerry_release_value(fontSizeVal);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void MarqueeTddTest::RunTests()
{
    ComponentMarqueeStyleSetTest006();
    ComponentMarqueeStyleSetTest007();
    ComponentMarqueeStyleSetTest008();
    ComponentMarqueeStyleSetTest009();
    ComponentMarqueeStyleSetTest010();
    ComponentMarqueeStyleSetTest011();
    ComponentMarqueeStyleSetTest012();
    ComponentMarqueeStyleSetTest013();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: ComponentMarqueeStyleSetTest006
 * @tc.desc: Verify Marquee style color set boundary value.
 */
HWTEST_F(MarqueeTddTest, marStyle006, TestSize.Level0)
{
    MarqueeTddTest::ComponentMarqueeStyleSetTest006();
}

/**
 * @tc.name: ComponentMarqueeStyleSetTest007
 * @tc.desc: Verify Marquee style color set boundary value.
 */
HWTEST_F(MarqueeTddTest, marStyle007, TestSize.Level1)
{
    MarqueeTddTest::ComponentMarqueeStyleSetTest007();
}

/**
 * @tc.name: ComponentMarqueeStyleSetTest008
 * @tc.desc: Verify Marquee style color set normal value.
 */
HWTEST_F(MarqueeTddTest, marStyle008, TestSize.Level1)
{
    MarqueeTddTest::ComponentMarqueeStyleSetTest008();
}

/**
 * @tc.name: ComponentMarqueeStyleSetTest009
 * @tc.desc: Verify Marquee style color set value out of boundary.
 */
HWTEST_F(MarqueeTddTest, marStyle009, TestSize.Level1)
{
    MarqueeTddTest::ComponentMarqueeStyleSetTest009();
}

/**
 * @tc.name: ComponentMarqueeStyleSetTest010
 * @tc.desc: Verify Marquee style color set value out of boundary.
 */
HWTEST_F(MarqueeTddTest, marStyle010, TestSize.Level1)
{
    MarqueeTddTest::ComponentMarqueeStyleSetTest010();
}
#endif // TDD_ASSERTIONS
} // namespace ACELite
} // namespace OHOS
