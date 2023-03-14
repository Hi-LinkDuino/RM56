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
#include "radio_switch_tdd_test.h"
#include "ui_radio_button.h"
#include "ui_toggle_button.h"
namespace OHOS {
namespace ACELite {
RadioSwitchTddTest::RadioSwitchTddTest() : radioComponentId_(0), switchComponentId_(0)
{
    const int switchNameLen = 3;
    switchComponentId_ = KeyParser::ParseKeyId("switch", switchNameLen);
    const int inputNameLen = 5;
    radioComponentId_ = KeyParser::ParseKeyId("input", inputNameLen);
}

void RadioSwitchTddTest::WidthRadioValueTest001()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.set the input type radio
     */
    JerrySetStringProperty(attrsObj_, "type", "radio");

    /**
     * @tc.steps:step2.set the input width 25
     * @tc.expected:step2.the radio width is 25
     */
    int32_t width = 25;
    JerrySetNumberProperty(styleObj_, "width", width);
    Component *component = GetRenderedComponent(radioComponentId_);
    UIRadioButton *radio = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (radio->GetWidth() != width) {
        printf("[WidthRadioValueTest001] set the radio width 25 failed\n");
        delete component;
        return;
    }
    EXPECT_FALSE(radio->GetWidth() != width);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step3.set the input width -1
     * @tc.steps:step3.the radio width is the default length 46
     */
    width = -1;
    const uint8_t defaultLen = 46;
    JerrySetNumberProperty(styleObj_, "width", width);
    component = GetRenderedComponent(radioComponentId_);
    radio = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (radio->GetWidth() != defaultLen) {
        printf("[WidthRadioValueTest001] set the radio width 0 failed\n");
        delete component;
        return;
    }
    EXPECT_FALSE(radio->GetWidth() != defaultLen);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step4.set the input width INT16_MAX + 20
     * @tc.steps:step4.the width of radio is 19
     */
    width = MAX_VALUE;
    JerrySetNumberProperty(styleObj_, "width", width);
    component = GetRenderedComponent(radioComponentId_);
    radio = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (radio->GetWidth() != CHECK_LEN) {
        printf("[WidthRadioValueTest001] set the radio width INT16_MAX + 20 failed\n");
    } else {
        printf("[WidthRadioValueTest001] passed\n");
    }
    EXPECT_FALSE(radio->GetWidth() != CHECK_LEN);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void RadioSwitchTddTest::WidthRadioValueTest002()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.set the input type radio
     */
    JerrySetStringProperty(attrsObj_, "type", "radio");

    /**
     * @tc.steps:step2.set the radio width false
     * @tc.expected:step2.the width of radio is defaultLength 46
     */
    jerry_value_t widthValue = jerry_create_boolean(false);
    JerrySetNamedProperty(styleObj_, "width", widthValue);
    jerry_release_value(widthValue);
    Component *component = GetRenderedComponent(radioComponentId_);
    UIRadioButton *radio = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (radio->GetWidth() != 0) {
        printf("[WidthRadioValueTest002] set the width false failed\n");
        delete component;
        return;
    }
    EXPECT_FALSE(radio->GetWidth() != 0);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step3.set the radio width testaaa
     * @tc.expected:step3.the width of radio is defaultLen 46
     */
    JerrySetStringProperty(styleObj_, "width", "testaaa");
    component = GetRenderedComponent(radioComponentId_);
    radio = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (radio->GetWidth() != 0) {
        printf("[WidthRadioValueTest002] set the radio width testaaa failed\n");
    } else {
        printf("[WidthRadioValueTest002] passed\n");
    }
    EXPECT_FALSE(radio->GetWidth() != 0);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void RadioSwitchTddTest::WidthSwitchValueTest003()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.set the radio width 25
     * @tc.expected:step1.the width of toggleButton is 25
     */
    int32_t width = 25;
    JerrySetNumberProperty(styleObj_, "width", width);
    Component *component = GetRenderedComponent(switchComponentId_);
    UIToggleButton *toggleButton = reinterpret_cast<UIToggleButton *>(component->GetComponentRootView());
    if (toggleButton->GetWidth() != width) {
        printf("[WidthSwitchValueTest003] set the switch width 25 failed\n");
        delete component;
        return;
    }
    EXPECT_TRUE(toggleButton->GetWidth() == width);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step2.set the radio width 0
     * @tc.expected:step2.the radio width is the defaultLen 46
     */
    const uint8_t defaultLen = 46;
    width = -1;
    JerrySetNumberProperty(styleObj_, "width", width);
    component = GetRenderedComponent(switchComponentId_);
    toggleButton = reinterpret_cast<UIToggleButton *>(component->GetComponentRootView());
    if (toggleButton->GetWidth() != defaultLen) {
        printf("[WidthSwitchValueTest003] set the switch width -1 failed\n");
        delete component;
        return;
    }
    EXPECT_TRUE(toggleButton->GetWidth() == defaultLen);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step3.set the switch width 256
     * @tc.expected:step3.the width of switch is 256
     */
    width = MAX_VALUE;
    JerrySetNumberProperty(styleObj_, "width", width);
    component = GetRenderedComponent(switchComponentId_);
    toggleButton = reinterpret_cast<UIToggleButton *>(component->GetComponentRootView());
    if (toggleButton->GetWidth() != CHECK_LEN) {
        printf("[WidthSwitchValueTest003] set the switch width int16_max + 20 failed\n");
    } else {
        printf("[WidthSwitchValueTest003] passed\n");
    }
    EXPECT_TRUE(toggleButton->GetWidth() == CHECK_LEN);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void RadioSwitchTddTest::WidthSwitchValueTest004()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.set the width of switch false
     * @tc.expected:step1.the width of switch is defaultLen 46
     */
    jerry_value_t widthVal = jerry_create_boolean(false);
    JerrySetNamedProperty(styleObj_, "width", widthVal);
    jerry_release_value(widthVal);
    Component *component = GetRenderedComponent(switchComponentId_);
    UIToggleButton *toggleButton = reinterpret_cast<UIToggleButton *>(component->GetComponentRootView());
    if (toggleButton->GetWidth() != 0) {
        printf("[WidthSwitchValueTest004]set the width of switch false failed\n");
        delete component;
        return;
    }
    EXPECT_EQ(toggleButton->GetWidth(), 0);
    delete component;
    component = nullptr;

    JerrySetStringProperty(styleObj_, "width", "testaaa");
    component = GetRenderedComponent(switchComponentId_);
    toggleButton = reinterpret_cast<UIToggleButton *>(component->GetComponentRootView());
    if (toggleButton->GetWidth() == 0) {
        printf("[WidthSwitchValueTest004] passed\n");
    } else {
        printf("[WidthSwitchValueTest004] set the width of switch testaaa failed\n");
    }
    EXPECT_EQ(toggleButton->GetWidth(), 0);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void RadioSwitchTddTest::HeightRadioValueTest005()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.set the input type radio
     */
    JerrySetStringProperty(attrsObj_, "type", "radio");

    /**
     * @tc.steps:step2.set the width of radio 25
     * @tc.expected:step2.the width of radio is 25
     */
    int32_t height = 25;
    JerrySetNumberProperty(styleObj_, "height", height);
    Component *component = GetRenderedComponent(radioComponentId_);
    UIToggleButton *toggleButton = reinterpret_cast<UIToggleButton *>(component->GetComponentRootView());
    if (toggleButton->GetHeight() != height) {
        printf("[HeightRadioValueTest005] set the height of radio 25 failed\n");
        delete component;
        return;
    }
    EXPECT_TRUE(toggleButton->GetHeight() == height);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step3.set the height of radio 0
     * @tc.expected:step3.the height of radio is defaultLen 46
     */
    height = -1;
    const uint8_t defaultLen = 46;
    JerrySetNumberProperty(styleObj_, "height", height);
    component = GetRenderedComponent(radioComponentId_);
    toggleButton = reinterpret_cast<UIToggleButton *>(component->GetComponentRootView());
    if (toggleButton->GetHeight() != defaultLen) {
        printf("[HeightRadioValueTest005] set the height of radio 0 failed\n");
        delete component;
        return;
    }
    EXPECT_TRUE(toggleButton->GetHeight() == defaultLen);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step4.set the height of radio UINT16_MAX + 20
     * @tc.steps:step4.the height of radio is 19
     */
    height = MAX_VALUE;
    JerrySetNumberProperty(styleObj_, "height", height);
    component = GetRenderedComponent(radioComponentId_);
    toggleButton = reinterpret_cast<UIToggleButton *>(component->GetComponentRootView());
    if (toggleButton->GetHeight() != CHECK_LEN) {
        printf("[HeightRadioValueTest005] set the height of radio UINT16_MAX + 20 failed\n");
    } else {
        printf("[HeightRadioValueTest005] passed\n");
    }
    EXPECT_TRUE(toggleButton->GetHeight() == CHECK_LEN);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void RadioSwitchTddTest::HeightRadioValueTest006()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.set the input type radio
     */
    JerrySetStringProperty(attrsObj_, "type", "radio");

    /**
     * @tc.steps:step2.set the height of radio false
     * @tc.expected:step2.the height of radio is the defaultLen 46
     */
    jerry_value_t widthVal = jerry_create_boolean(false);
    JerrySetNamedProperty(styleObj_, "width", widthVal);
    jerry_release_value(widthVal);
    Component *component = GetRenderedComponent(switchComponentId_);
    UIRadioButton *radioButton = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (radioButton->GetWidth() != 0) {
        printf("[HeightRadioValueTest006] set the height of switch false");
        delete component;
        return;
    }
    EXPECT_EQ(radioButton->GetWidth(), 0);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step3.set the height of radio testaaa
     * @tc.expected:step3.the height of radio is defaultLen 46
     */
    JerrySetStringProperty(styleObj_, "width", "testaaa");
    component = GetRenderedComponent(switchComponentId_);
    radioButton = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (radioButton->GetWidth() != 0) {
        printf("[HeightRadioValueTest006] set the height of switch testaaa failed\n");
    } else {
        printf("[HeightRadioValueTest006] passed\n");
    }
    EXPECT_EQ(radioButton->GetWidth(), 0);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void RadioSwitchTddTest::HeightSwitchValueTest007()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.set the height of switch 25
     * @tc.steps:step1.the height of radio is 25
     */
    int32_t height = 25;
    JerrySetNumberProperty(styleObj_, "height", height);
    Component *component = GetRenderedComponent(switchComponentId_);
    UIRadioButton *button = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (button->GetHeight() != height) {
        printf("[HeightSwitchValueTest007]set the switch height 25 failed\n");
        delete component;
        return;
    }
    EXPECT_TRUE(button->GetHeight() == height);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step2.set the height of switch 0
     * @tc.expected:step2.the height of switch is the default length 46
     */
    height = -1;
    const uint8_t defaultLen = 46;
    JerrySetNumberProperty(styleObj_, "height", height);
    component = GetRenderedComponent(switchComponentId_);
    button = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (button->GetHeight() != defaultLen) {
        printf("[HeightSwitchValueTest007] set the height of switch -1 failed\n");
        delete component;
        return;
    }
    EXPECT_TRUE(button->GetHeight() == defaultLen);
    delete component;
    component = nullptr;

    /**
     * @tc.steps:step3.set the height of switch INT16_MAX + 20
     * @tc.expected:step3.the height of switch is the default length 19
     */
    height = MAX_VALUE;
    JerrySetNumberProperty(styleObj_, "height", height);
    component = GetRenderedComponent(switchComponentId_);
    button = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (button->GetHeight() != CHECK_LEN) {
        printf("[HeightSwitchValueTest007] set the height of switch 256 failed\n");
    } else {
        printf("[HeightSwitchValueTest007] passed\n");
    }
    EXPECT_TRUE(button->GetHeight() == CHECK_LEN);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void RadioSwitchTddTest::HeightSwitchValueTest008()
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps: step1.set the switch heifhr false
     * @tc.expected: step1.the height is the default height 46
     */
    jerry_value_t number = jerry_create_boolean(false);
    JerrySetNamedProperty(styleObj_, "height", number);
    jerry_release_value(number);
    Component *component = GetRenderedComponent(switchComponentId_);
    UIRadioButton *button = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (button->GetHeight() != 0) {
        printf("[HeightSwitchValueTest008] set the height of switch false failed\n");
        delete component;
        return;
    }
    EXPECT_EQ(button->GetHeight(), 0);
    delete component;
    component = nullptr;

    /**
     * @tc.steps: step2.set the switch height testaaa
     * @tc.expected: step2.the height of toggleButton is default height 46
     */
    JerrySetStringProperty(styleObj_, "height", "testaaa");
    component = GetRenderedComponent(switchComponentId_);
    button = reinterpret_cast<UIRadioButton *>(component->GetComponentRootView());
    if (button->GetHeight() != 0) {
        printf("[HeightSwitchValueTest008]set the height of switch testaaa failed\n");
    } else {
        printf("[HeightSwitchValueTest008] passed\n");
    }
    EXPECT_EQ(button->GetHeight(), 0);
    delete component;
    component = nullptr;
    TDD_CASE_END();
}

void RadioSwitchTddTest::RunTests()
{
    WidthRadioValueTest001();
    WidthRadioValueTest002();
    WidthSwitchValueTest003();
    WidthSwitchValueTest004();
    HeightRadioValueTest005();
    HeightRadioValueTest006();
    HeightSwitchValueTest007();
    HeightSwitchValueTest008();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: Width_Radio_Value_Test001
 * @tc.desc: test the radio width in different range
 */
HWTEST_F(RadioSwitchTddTest, radioWidthTest001, TestSize.Level1)
{
    WidthRadioValueTest001();
}

/**
 * @tc.name: Width_Radio_Value_Test002
 * @tc.desc: test the radio width in invalid value
 */
HWTEST_F(RadioSwitchTddTest, radioWidthTest002, TestSize.Level1)
{
    WidthRadioValueTest002();
}

/**
 * @tc.name: Width_Switch_Value_Test003
 * @tc.desc: test the switch width in different range
 */
HWTEST_F(RadioSwitchTddTest, widthSwitchTest003, TestSize.Level0)
{
    WidthSwitchValueTest003();
}

/**
 * @tc.name: Width_Switch_Value_Test004
 * @tc.desc: set the width of switch invalid value
 */
HWTEST_F(RadioSwitchTddTest, widthSwitchTest004, TestSize.Level1)
{
    WidthSwitchValueTest004();
}

/**
 * @tc.name: Height_Radio_Value_Test005
 * @tc.desc: set the height of radio in different range
 */
HWTEST_F(RadioSwitchTddTest, heightRadioTest005, TestSize.Level1)
{
    HeightRadioValueTest005();
}

/**
 * @tc.name: Height_Radio_Value_Test006
 * @tc.desc: set the height of radio invalid value
 */
HWTEST_F(RadioSwitchTddTest, heightRadioTest006, TestSize.Level1)
{
    HeightRadioValueTest006();
}

/**
 * @tc.name: Height_Switch_Value_Test007
 * @tc.desc: set the height of radio invalid value
 */
HWTEST_F(RadioSwitchTddTest, heightSwitchTest007, TestSize.Level1)
{
    HeightSwitchValueTest007();
}

/**
 * @tc.name: Height_Switch_Value_Test008
 * @tc.desc: set the height of radio invalid value
 */
HWTEST_F(RadioSwitchTddTest, heightSwitchTest008, TestSize.Level1)
{
    HeightSwitchValueTest008();
}
#endif
} // namespace ACELite
} // namespace OHOS
