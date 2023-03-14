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

#include "slider_tdd_test.h"
#include "acelite_config.h"
#include "component_factory.h"
#include "js_app_environment.h"
#include "root_view.h"
#include "slider_component.h"
#include "ui_slider.h"

namespace OHOS {
namespace ACELite {
SliderTddTest::SliderTddTest():BaseTest()
{
    componentNameId_ = KeyParser::ParseKeyId("slider");
}

void SliderTddTest::ComponentSliderAttributeMinTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set Slider attribute min value = 0
     */
    const int expectMinValue = 0;
    const char* minStr = "min";
    jerry_value_t minKey = jerry_create_string((const jerry_char_t*)minStr);
    jerry_value_t minValue = jerry_create_number(expectMinValue);
    jerry_set_property(attrsObj_, minKey, minValue);
    Component* sliderComponent = reinterpret_cast<SliderComponent *>(GetRenderedComponent(componentNameId_));
    UISlider* sliderView = reinterpret_cast<UISlider *>(sliderComponent->GetComponentRootView());
    jerry_release_value(minKey);
    jerry_release_value(minValue);

    /**
     * @tc.expected: step2. uikit sliderView minvalue == 0
     */
    if (sliderView->GetRangeMin() == expectMinValue) {
        printf("%s pass\n", __FUNCTION__);
    } else {
        printf("%s fail\n", __FUNCTION__);
    }
    EXPECT_EQ(sliderView->GetRangeMin(), expectMinValue);

    /* minValue is [-32767,32767], set min value = 32768 */
    /**
     * @tc.expected: step4. update min value = 32768, check sliderView.minValue != 32768
     */
    const int expectMinValue3 = 32768;
    UpdateNumAttributeOrStyleValue(sliderComponent, minStr, expectMinValue3, true);
    if (sliderView->GetRangeMin() != expectMinValue3) {
        printf("%s  update min overflow value pass\n", __FUNCTION__);
    } else {
        printf("%s update min overflow value  fail\n", __FUNCTION__);
    }
    EXPECT_NE(sliderView->GetRangeMin(), expectMinValue3);

    TDD_CASE_END();
}

void SliderTddTest::ComponentSliderAttributeMaxTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set Slider attribute max = 0
     */
    const int expectMaxValue = 1;
    const char* maxStr = "max";
    jerry_value_t maxKey = jerry_create_string((const jerry_char_t*)maxStr);
    jerry_value_t maxValue = jerry_create_number(expectMaxValue);
    jerry_set_property(attrsObj_, maxKey, maxValue);
    Component* sliderComponent = reinterpret_cast<SliderComponent *>(GetRenderedComponent(componentNameId_));
    UISlider* sliderView = reinterpret_cast<UISlider *>(sliderComponent->GetComponentRootView());
    jerry_release_value(maxKey);
    jerry_release_value(maxValue);
    /**
     * @tc.expected: step2. uikit sliderView maxvalue == 0
     */
    if (sliderView->GetRangeMax() == expectMaxValue) {
        printf("%s set value pass\n", __FUNCTION__);
    } else {
        printf("%s set value fail\n", __FUNCTION__);
    }
    EXPECT_EQ(sliderView->GetRangeMax(), expectMaxValue);

    /**
     * @tc.expected: step4. update max value = 32768, check sliderView.maxValue != 32768
     */
    const int expectMaxValue3 = 32768;
    UpdateNumAttributeOrStyleValue(sliderComponent, maxStr, expectMaxValue3, true);
    if (sliderView->GetRangeMax() != expectMaxValue3) {
        printf("%s update max overflow value pass\n", __FUNCTION__);
    } else {
        printf("%s update max overflow value  fail\n", __FUNCTION__);
    }
    EXPECT_NE(sliderView->GetRangeMax(), expectMaxValue3);

    TDD_CASE_END();
}

void SliderTddTest::ComponentSliderAttributeValueTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set Slider attribute value = 0
     */
    const int expectValue = 100;
    const char* valueStr = "value";
    jerry_value_t valueKey = jerry_create_string((const jerry_char_t*)valueStr);
    jerry_value_t value = jerry_create_number(expectValue);
    jerry_set_property(attrsObj_, valueKey, value);
    Component* sliderComponent = reinterpret_cast<SliderComponent *>(GetRenderedComponent(componentNameId_));
    UISlider* sliderView = reinterpret_cast<UISlider *>(sliderComponent->GetComponentRootView());
    jerry_release_value(valueKey);
    jerry_release_value(value);
    /**
     * @tc.expected: step2. check uikit sliderView value == 0
     */
    if (sliderView->GetValue() == expectValue) {
        printf("%s pass\n", __FUNCTION__);
    } else {
        printf("%s fail\n", __FUNCTION__);
    }
    EXPECT_EQ(sliderView->GetValue(), expectValue);

    /**
     * @tc.expected: step3. update value = 32767, check sliderView.value == 32767
     */
    const int expectValue2 = 32767;
    UpdateNumAttributeOrStyleValue(sliderComponent, valueStr, expectValue2, true);
    if (sliderView->GetValue() != expectValue2) {
        printf("%s update value value pass\n", __FUNCTION__);
    } else {
        printf("%s update value value fail\n", __FUNCTION__);
    }
    EXPECT_NE(sliderView->GetValue(), expectValue2);

    /**
     * @tc.expected: step4. update value = 32768, check sliderView.value != 32768
     */
    const int expectValue3 = 32768;
    UpdateNumAttributeOrStyleValue(sliderComponent, valueStr, expectValue3, true);
    if (sliderView->GetValue() != expectValue3) {
        printf("%s update value overflow value pass\n", __FUNCTION__);
    } else {
        printf("%s update value overflow value fail\n", __FUNCTION__);
    }
    EXPECT_NE(sliderView->GetValue(), expectValue3);

    TDD_CASE_END();
}

void SliderTddTest::ComponentSliderStyleSetColorTest004()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set text color value = 16711680
     */
    const int expectColorValue = 16711680;
    const char* colorStr = "color";
    jerry_value_t textKey = jerry_create_string((const jerry_char_t*)colorStr);
    jerry_value_t textColorValue = jerry_create_number(expectColorValue);
    jerry_set_property(styleObj_, textKey, textColorValue);
    Component* sliderComponent = reinterpret_cast<SliderComponent *>(GetRenderedComponent(componentNameId_));
    UISlider* uiSliderView = reinterpret_cast<UISlider *>(sliderComponent->GetComponentRootView());
    jerry_release_value(textKey);
    jerry_release_value(textColorValue);
    /**
     * @tc.expected: step2. check uikit sliderView color = 16711680 (red color)
     */
    if (uiSliderView->GetBackgroundStyle(STYLE_BACKGROUND_COLOR) == GetRGBColor(expectColorValue).full) {
        printf("%s pass\n", __FUNCTION__);
    } else {
        printf("%s fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uiSliderView->GetBackgroundStyle(STYLE_BACKGROUND_COLOR), GetRGBColor(expectColorValue).full);

    /**
     * @tc.expected: step3. update color = expectColorValue2, check uikit sliderView color == expectColorValue2
     */
    const int32_t expectColorValue2 = 16777215;
    UpdateNumAttributeOrStyleValue(sliderComponent, colorStr, expectColorValue2, false);
    if (uiSliderView->GetBackgroundStyle(STYLE_BACKGROUND_COLOR) == GetRGBColor(expectColorValue2).full) {
        printf("%s update color pass\n", __FUNCTION__);
    } else {
        printf("%s update color fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uiSliderView->GetBackgroundStyle(STYLE_BACKGROUND_COLOR), GetRGBColor(expectColorValue2).full);

    /**
     * @tc.expected: step4. update color = expectColorValue3,
     * check uikit sliderView color == expectColorValue3,
     */
    const int32_t expectColorValue3 = 16777216;
    UpdateNumAttributeOrStyleValue(sliderComponent, colorStr, expectColorValue3, false);
    if (uiSliderView->GetBackgroundStyle(STYLE_BACKGROUND_COLOR) == GetRGBColor(expectColorValue3).full) {
        printf("%s update overflow color pass\n", __FUNCTION__);
    } else {
        printf("%s update overflow color fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uiSliderView->GetBackgroundStyle(STYLE_BACKGROUND_COLOR), GetRGBColor(expectColorValue3).full);

    /**
     * @tc.expected: step5. set exceptionColorStrValue with exception value  "helloWorld",
     *  program keep going
     */
    const char* exceptionColorStrValue = "helloWorld";
    UpdateCharAttributeOrStyleValue(sliderComponent, colorStr, exceptionColorStrValue, false);

    TDD_CASE_END();
}

void SliderTddTest::ComponentSliderStyleSetSelectColorTest005()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. set slider selected color value = 16711680
     */
    const int expectColorValue = 16711680;
    const char* selectedColorStr = "selectedColor";
    jerry_value_t selectedColorKey = jerry_create_string((const jerry_char_t*)selectedColorStr);
    jerry_value_t selectedColorValue = jerry_create_number(expectColorValue);

    jerry_set_property(styleObj_, selectedColorKey, selectedColorValue);

    Component* sliderComponent = reinterpret_cast<SliderComponent *>(GetRenderedComponent(componentNameId_));
    UISlider* uiSliderView = reinterpret_cast<UISlider *>(sliderComponent->GetComponentRootView());

    jerry_release_value(selectedColorKey);
    jerry_release_value(selectedColorValue);

    /**
     * @tc.expected: step2. check uikit sliderView select color = 16711680 (red color)
     */
    if (uiSliderView->GetForegroundStyle(STYLE_BACKGROUND_COLOR) == GetRGBColor(expectColorValue).full) {
        printf("%s pass\n", __FUNCTION__);
    } else {
        printf("%s fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uiSliderView->GetForegroundStyle(STYLE_BACKGROUND_COLOR), GetRGBColor(expectColorValue).full);

    /**
     * @tc.expected: step3. update slectedcolor = expectColorValue2, check uikit sliderView selectcolor ==
     * expectColorValue2
     */
    const int32_t expectColorValue2 = 16777215;
    UpdateNumAttributeOrStyleValue(sliderComponent, selectedColorStr, expectColorValue2, false);
    if (uiSliderView->GetForegroundStyle(STYLE_BACKGROUND_COLOR) == GetRGBColor(expectColorValue2).full) {
        printf("%s update selectedColor pass\n", __FUNCTION__);
    } else {
        printf("%s update selectedColor fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uiSliderView->GetForegroundStyle(STYLE_BACKGROUND_COLOR), GetRGBColor(expectColorValue2).full);

    /**
     * @tc.expected: step4. update  selectedcolor with overflow expectColorValue3,
     * check uikit sliderView color == expectColorValue3,
     */
    const int32_t expectColorValue3 = 16777216;
    UpdateNumAttributeOrStyleValue(sliderComponent, selectedColorStr, expectColorValue3, false);
    if (uiSliderView->GetForegroundStyle(STYLE_BACKGROUND_COLOR) == GetRGBColor(expectColorValue3).full) {
        printf("%s update overflow selectedColor pass\n", __FUNCTION__);
    } else {
        printf("%s update overflow selectedColor fail\n", __FUNCTION__);
    }
    EXPECT_EQ(uiSliderView->GetForegroundStyle(STYLE_BACKGROUND_COLOR), GetRGBColor(expectColorValue3).full);

    /**
     * @tc.expected: step5. set exceptionColorStrValue with exception value  "helloWorld",
     *  program keep going
     */
    const char* expectColorStrValue = "helloWorld";
    UpdateCharAttributeOrStyleValue(sliderComponent, selectedColorStr, expectColorStrValue, false);

    TDD_CASE_END();
}

void SliderTddTest::RunTests()
{
    ComponentSliderAttributeMinTest001();
    ComponentSliderAttributeMaxTest002();
    ComponentSliderAttributeValueTest003();
    ComponentSliderStyleSetColorTest004();
    ComponentSliderStyleSetSelectColorTest005();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name:ComponentSliderAttributeMinTest001
 * @tc.desc: Verify Slider min value can set normally.
 */
HWTEST_F(SliderTddTest, SliderAttr001, TestSize.Level1)
{
    SliderTddTest::ComponentSliderAttributeMinTest001();
}

/**
 * @tc.name:ComponentSliderAttributeMaxTest002
 * @tc.desc: Verify Slider max value can set normally.
 */
HWTEST_F(SliderTddTest, SliderAttr002, TestSize.Level1)
{
    SliderTddTest::ComponentSliderAttributeMaxTest002();
}

/**
 * @tc.name:ComponentSliderAttributeValueTest003
 * @tc.desc: Verify Slider value value can set normally.
 */
HWTEST_F(SliderTddTest, SliderAttr003, TestSize.Level0)
{
    SliderTddTest::ComponentSliderAttributeValueTest003();
}

/**
 * @tc.name:ComponentSliderStyleSetColorTest004
 * @tc.desc: Verify slider color value.
 */
HWTEST_F(SliderTddTest, SliderStyle004, TestSize.Level1)
{
    SliderTddTest::ComponentSliderStyleSetColorTest004();
}

/**
 * @tc.name:ComponentSliderStyleSetSelectColorTest005
 * @tc.desc: Verify slider color value.
 */
HWTEST_F(SliderTddTest, SliderStyle005, TestSize.Level1)
{
    SliderTddTest::ComponentSliderStyleSetSelectColorTest005();
}

#endif
} // namespace ACELite
} // namespace OHOS
