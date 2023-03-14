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

#include "text_tdd_test.h"
#include "acelite_config.h"
#include "component_factory.h"
#include "js_app_environment.h"
#include "root_view.h"
#include "text_component.h"
#include "ui_label.h"
#include "ui_font.h"

namespace OHOS {
namespace ACELite {
TextTddTest::TextTddTest():BaseTest()
{
    componentNameId_ = KeyParser::ParseKeyId("text");
}

void TextTddTest::ComponentTextAttributeSetValueTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set text attribute value = "hello world"
     */
    const char* expectTextValue = "hello world";
    const char* valueStr = "value";
    jerry_value_t textKey = jerry_create_string((const jerry_char_t*)valueStr);
    jerry_value_t textValue = jerry_create_string((const jerry_char_t*)expectTextValue);
    jerry_set_property(attrsObj_, textKey, textValue);
    Component* textComponent = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    UILabel* uilabelView = reinterpret_cast<UILabel *>(textComponent->GetComponentRootView());
    jerry_release_value(textKey);
    jerry_release_value(textValue);

    /**
     * @tc.expected: step2. value == expectTextValue
     */
    if (uilabelView->GetText() != nullptr && !strcmp(uilabelView->GetText(), expectTextValue)) {
        printf("%s pass\n", __FUNCTION__);
    } else {
        printf("%s fail\n", __FUNCTION__);
    }
    EXPECT_STREQ(uilabelView->GetText(), expectTextValue);

    /**
     * @tc.expected: step3. update value, check value == expectTextValue2
     */
    const char* expectTextValue2 = "1234567890";
    UpdateCharAttributeOrStyleValue(textComponent, "value", expectTextValue2, true);
    if ((uilabelView->GetText() != nullptr) && !strcmp(uilabelView->GetText(), expectTextValue2)) {
        printf("%s update test value pass\n", __FUNCTION__);
    } else {
        printf("%s update test value fail\n", __FUNCTION__);
    }
    EXPECT_STREQ(uilabelView->GetText(), expectTextValue2);

    /**
     * @tc.expected: step4. update value witch excpetion expectTextIntValue , check value != expectTextIntValue
     */
    int expectTextIntValue = 100;
    UpdateNumAttributeOrStyleValue(textComponent, "value", expectTextIntValue, true);
    if ((uilabelView->GetText() != nullptr) && !strcmp(uilabelView->GetText(), "100")) {
        printf("%s update test value 100 pass\n", __FUNCTION__);
    } else {
        printf("%s update test value 100 fail\n", __FUNCTION__);
    }
    EXPECT_TRUE(uilabelView->GetText() != nullptr);
    EXPECT_STREQ(uilabelView->GetText(), "100");

    TDD_CASE_END();
}

void TextTddTest::ComponentTextStyleSetColorTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set text color value = 16777215
     */
    int expectColorValue = 16711680;
    const char* colorStr = "color";
    jerry_value_t textKey = jerry_create_string((const jerry_char_t*)colorStr);
    jerry_value_t textColorValue = jerry_create_number(expectColorValue);

    jerry_set_property(styleObj_, textKey, textColorValue);

    Component* textComponent = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    UILabel* uilabelView = reinterpret_cast<UILabel *>(textComponent->GetComponentRootView());

    jerry_release_value(textKey);
    jerry_release_value(textColorValue);

    /**
     * @tc.expected: step2. check text color == 16711680 (red color)
     */
    if (uilabelView->GetStyle(STYLE_TEXT_COLOR) == GetRGBColor(expectColorValue).full) {
        printf("%s pass\n", __FUNCTION__);
    } else {
        printf("%s fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetStyle(STYLE_TEXT_COLOR), GetRGBColor(expectColorValue).full);

    /**
     * @tc.expected: step3. update text color with normal expectColorValue2,check text color == expectColorValue2
     */
    int32_t expectColorValue2 = 16777215;
    UpdateNumAttributeOrStyleValue(textComponent, colorStr, expectColorValue2, false);
    if (uilabelView->GetStyle(STYLE_TEXT_COLOR) == GetRGBColor(expectColorValue2).full) {
        printf("%s update color pass\n", __FUNCTION__);
    } else {
        printf("%s update color fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetStyle(STYLE_TEXT_COLOR), GetRGBColor(expectColorValue2).full);

    /**
     * @tc.expected: step4. update text color with overflow expectColorValue3,check text color == expectColorValue3
     */
    int32_t expectColorValue3 = 16777216;
    UpdateNumAttributeOrStyleValue(textComponent, colorStr, expectColorValue3, false);
    if (uilabelView->GetStyle(STYLE_TEXT_COLOR) == GetRGBColor(expectColorValue3).full) {
        printf("%s update overflow color pass\n", __FUNCTION__);
    } else {
        printf("%s update overflow color fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetStyle(STYLE_TEXT_COLOR), GetRGBColor(expectColorValue3).full);

    TDD_CASE_END();
}

void TextTddTest::ComponentTextStyleSetOverflowTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set text overflow = clip
     */
    const char* expectOverflowClip = "clip";
    const char* textOverflow = "textOverflow";
    const char* height = "height";
    const char* width = "width";
    int textHeight = 0;
    int textWidth = 0;
    JerrySetNumberProperty(styleObj_, height, textHeight);
    JerrySetNumberProperty(styleObj_, width, textWidth);
    JerrySetStringProperty(styleObj_, textOverflow, expectOverflowClip);
    Component* textComponent = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    UILabel* uilabelView = reinterpret_cast<UILabel *>(textComponent->GetComponentRootView());
    /**
     * @tc.expected: step2. check uikit uilabelView  == UILabel::LINE_BREAK_CLIP
     */
    if (uilabelView->GetLineBreakMode() == UILabel::LINE_BREAK_CLIP) {
        printf("%s set OverFlow clip pass\n", __FUNCTION__);
    } else {
        printf("%s set OverFlow clip fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetLineBreakMode(), UILabel::LINE_BREAK_CLIP);

    /**
     * @tc.expected: step3. update text Overflow = "ellipsis"
     * check uikit uilabelView  == UILabel::LINE_BREAK_ELLIPSIS
     */
    const char* expectOverflowEllipsis = "ellipsis";
    UpdateCharAttributeOrStyleValue(textComponent, textOverflow, expectOverflowEllipsis, false);
    if (uilabelView->GetLineBreakMode() == UILabel::LINE_BREAK_ELLIPSIS) {
        printf("%s update OverFlow ellipsis pass\n", __FUNCTION__);
    } else {
        printf("%s update OverFlow ellipsis fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetLineBreakMode(), UILabel::LINE_BREAK_ELLIPSIS);

    /**
     * @tc.expected: step4. update text Overflow = "break"
     * check uikit uilabelView  == UILabel::LINE_BREAK_WRAP
     */
    const char* expectOverflowBreak = "break";
    UpdateCharAttributeOrStyleValue(textComponent, textOverflow, expectOverflowBreak, false);
    if (uilabelView->GetLineBreakMode() == UILabel::LINE_BREAK_WRAP) {
        printf("%s update OverFlow break pass\n", __FUNCTION__);
    } else {
        printf("%s update OverFlow break fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetLineBreakMode(), UILabel::LINE_BREAK_WRAP);

    /**
     * @tc.expected: step5. update text Overflow = "expand"
     * check uikit uilabelView  == UILabel::LINE_BREAK_ADAPT
     */
    const char* expectOverflowExpand = "expand";
    UpdateCharAttributeOrStyleValue(textComponent, textOverflow, expectOverflowExpand, false);
    if (uilabelView->GetLineBreakMode() == UILabel::LINE_BREAK_ADAPT) {
        printf("%s update OverFlow expand pass\n", __FUNCTION__);
    } else {
        printf("%s update OverFlow expand fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetLineBreakMode(), UILabel::LINE_BREAK_ADAPT);

    TDD_CASE_END();
}

void TextTddTest::ComponentTextStyleSetLetterSpacingTest004()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set text letterSpacing value = 10
     */
    int expectLetterSpacingValue = 10;
    const char* letterSpacingStr = "letterSpacing";
    jerry_value_t textKey = jerry_create_string((const jerry_char_t*)letterSpacingStr);
    jerry_value_t textLetterSpacingValue = jerry_create_number(expectLetterSpacingValue);
    jerry_set_property(styleObj_, textKey, textLetterSpacingValue);
    Component* textComponent = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    UILabel* uilabelView = reinterpret_cast<UILabel *>(textComponent->GetComponentRootView());

    /**
     * @tc.expected: step2. check uikit uilabel letterSpace == expectLetterSpacingValue
     */
    if (uilabelView->GetStyle(STYLE_LETTER_SPACE) == expectLetterSpacingValue) {
        printf("%s update letterSpace pass\n", __FUNCTION__);
    } else {
        printf("%s update letterSpace fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetStyle(STYLE_LETTER_SPACE), expectLetterSpacingValue);
    jerry_release_value(textKey);
    jerry_release_value(textLetterSpacingValue);

    /**
     * @tc.expected: step3. update letterSpace, check uikit uilabel letterSpace == expectLetterSpacingValue2
     */
    int32_t expectLetterSpacingValue2 = 255;
    UpdateNumAttributeOrStyleValue(textComponent, letterSpacingStr, expectLetterSpacingValue2, false);
    if (uilabelView->GetStyle(STYLE_LETTER_SPACE) == expectLetterSpacingValue2) {
        printf("%s update normal letterSpace pass\n", __FUNCTION__);
    } else {
        printf("%s update normal letterSpace fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetStyle(STYLE_LETTER_SPACE), expectLetterSpacingValue2);

    /**
     * @tc.expected: step4. update letterSpace with exception value,
     * check uikit uilabel letterSpace != expectLetterSpacingValue3
     */
    int32_t expectLetterSpacingValue3 = 32768;
    UpdateNumAttributeOrStyleValue(textComponent, letterSpacingStr, expectLetterSpacingValue3, false);
    if (uilabelView->GetStyle(STYLE_LETTER_SPACE) != expectLetterSpacingValue3) {
        printf("%s update overflow letterSpace  pass\n", __FUNCTION__);
    } else {
        printf("%s update overflow letterSpace fail\n", __FUNCTION__);
    }
    EXPECT_NE(uilabelView->GetStyle(STYLE_LETTER_SPACE), expectLetterSpacingValue3);

    TDD_CASE_END();
}

void TextTddTest::ComponentTextStyleSetAlignTest005()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set text Align  = "left"
     */
    const char* expectLeftAlign = "left";
    const char* textAlign = "textAlign";
    jerry_value_t textKey = jerry_create_string((const jerry_char_t*)textAlign);
    jerry_value_t textAlignValue = jerry_create_string((const jerry_char_t*)expectLeftAlign);
    jerry_set_property(styleObj_, textKey, textAlignValue);
    Component* textComponent = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    UILabel* uilabelView = reinterpret_cast<UILabel *>(textComponent->GetComponentRootView());
    jerry_release_value(textKey);
    jerry_release_value(textAlignValue);
    /**
     * @tc.expected: step2. jsfwk render, check text align == UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT
     */
    if (uilabelView->GetHorAlign() == UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT) {
        printf("%s update text ALign left pass\n", __FUNCTION__);
    } else {
        printf("%s update text ALign left fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetHorAlign(), UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT);

    /**
     * @tc.expected: step2. update align = right, check text align == UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT
     */
    const char* expectRightAlign = "right";
    UpdateCharAttributeOrStyleValue(textComponent, textAlign, expectRightAlign, false);
    if (uilabelView->GetHorAlign() == UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT) {
        printf("%s update text ALign right pass\n", __FUNCTION__);
    } else {
        printf("%s update text ALign right fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetHorAlign(), UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT);

    /**
     * @tc.expected: step3. update align = center, check text align == UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER
     */
    const char* expectCenterAlign = "center";
    UpdateCharAttributeOrStyleValue(textComponent, textAlign, expectCenterAlign, false);
    if (uilabelView->GetHorAlign() == UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER) {
        printf("%s update text ALign center pass\n", __FUNCTION__);
    } else {
        printf("%s update text ALign center fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetHorAlign(), UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);

    TDD_CASE_END();
}

void TextTddTest::ComponentTextStyleSetSizeFamilyTest006()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set text fontSize = 30, fontFamily = HYQiHei-65S
     */
    const char *textSize = "fontSize";
    int expectSize = 30;
    jerry_value_t textSizetKey = jerry_create_string((const jerry_char_t *)textSize);
    jerry_value_t textSizeValue = jerry_create_number(expectSize);
    jerry_set_property(styleObj_, textSizetKey, textSizeValue);
    const char *textFamily = "fontFamily";
    const char *expectFamily = "HYQiHei-65S";
    jerry_value_t textFamilyKey = jerry_create_string((const jerry_char_t *)textFamily);
    jerry_value_t textFamilyValue = jerry_create_string((const jerry_char_t *)expectFamily);
    const char *expectTextValue = "hello world";
    const char *valueTag = "value";
    JerrySetStringProperty(attrsObj_, valueTag, expectTextValue);
    Component *textComponent = reinterpret_cast<TextComponent *>(GetRenderedComponent(componentNameId_));
    UILabel *uilabelView = reinterpret_cast<UILabel *>(textComponent->GetComponentRootView());
    jerry_release_value(textSizetKey);
    jerry_release_value(textSizeValue);
    jerry_release_value(textFamilyKey);
    jerry_release_value(textFamilyValue);

    /**
     * @tc.expected: step2. check text fontid == UITextLanguageFontId::F_HYQIHEI_65S_30_4
     */
    UIFont *font = UIFont::GetInstance();
    if (uilabelView->GetFontId() == font->GetFontId(expectFamily, expectSize)) {
        printf("%s set text size&family pass\n", __FUNCTION__);
    } else {
        printf("%s set text size&family fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetFontId(), font->GetFontId(expectFamily, expectSize));

    /**
     * @tc.expected: step3. update fontSize =38, check text fontid == UITextLanguageFontId::F_HYQIHEI_65S_38_4
     */
    int expectVidaLargerSize = 38;
    UpdateNumAttributeOrStyleValue(textComponent, textSize, expectVidaLargerSize, false);
    const char *expectVidaLargerFamily = "HYQiHei-65S";
    UpdateCharAttributeOrStyleValue(textComponent, textFamily, expectVidaLargerFamily, false);
    if (uilabelView->GetFontId() == font->GetFontId(expectVidaLargerFamily, expectVidaLargerSize)) {
        printf("%s update text size&family pass\n", __FUNCTION__);
    } else {
        printf("%s update text size&family fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetFontId(), font->GetFontId(expectVidaLargerFamily, expectVidaLargerSize));

    /**
     * @tc.expected: step4. update fontFamily with exception value "helloWorld",
     * check text fontid == UITextLanguageFontId::F_HYQIHEI_65S_38_4
     */
    /* set last available fontid instead of exceptionFamily */
    const char *expectExceptionFamily = "helloWorld";
    UpdateCharAttributeOrStyleValue(textComponent, textFamily, expectExceptionFamily, false);

    if (uilabelView->GetFontId() == font->GetFontId(expectVidaLargerFamily, expectVidaLargerSize)) {
        printf("%s  update text exception size&family pass\n", __FUNCTION__);
    } else {
        printf("%s update text  exception size&family fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uilabelView->GetFontId(), font->GetFontId(expectVidaLargerFamily, expectVidaLargerSize));

    TDD_CASE_END();
}

void TextTddTest::RunTests()
{
    ComponentTextAttributeSetValueTest001();
    ComponentTextStyleSetColorTest002();
    ComponentTextStyleSetOverflowTest003();
    ComponentTextStyleSetLetterSpacingTest004();
    ComponentTextStyleSetAlignTest005();
    ComponentTextStyleSetSizeFamilyTest006();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name:ComponentTextAttributeSetValueTest001
 * @tc.desc: Verify text value can set normally.
 */
HWTEST_F(TextTddTest, textAttr001, TestSize.Level1)
{
    TextTddTest::ComponentTextAttributeSetValueTest001();
}

/**
 * @tc.name:ComponentTextStyleSetColorTest002
 * @tc.desc: Verify text color value.
 */
HWTEST_F(TextTddTest, textStyle002, TestSize.Level1)
{
    TextTddTest::ComponentTextStyleSetColorTest002();
}

/**
 * @tc.name:ComponentTextStyleSetOverflowTest003
 * @tc.desc: Verify text color value.
 */
HWTEST_F(TextTddTest, textStyle003, TestSize.Level0)
{
    TextTddTest::ComponentTextStyleSetOverflowTest003();
}

/**
 * @tc.name:ComponentTextStyleSetLetterSpacingTest004
 * @tc.desc: Verify text letterSpacing value.
 */
HWTEST_F(TextTddTest, textStyle004, TestSize.Level1)
{
    TextTddTest::ComponentTextStyleSetLetterSpacingTest004();
}

/**
 * @tc.name:ComponentTextStyleSetAlignTest005
 * @tc.desc: Verify text color value.
 */
HWTEST_F(TextTddTest, textStyle005, TestSize.Level1)
{
    TextTddTest::ComponentTextStyleSetAlignTest005();
}
#endif
} // namespace ACELite
} // namespace OHOS
