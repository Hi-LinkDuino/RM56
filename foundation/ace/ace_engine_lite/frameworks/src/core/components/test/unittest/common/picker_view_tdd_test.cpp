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

#include "picker_view_tdd_test.h"
#include "js_app_environment.h"
#include "js_debugger_config.h"
#include "picker_view_component.h"
#include "root_view.h"
#include "stylemgr/app_style_item.h"
#include "stylemgr/app_style_manager.h"
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
#include "ui_date_picker.h"
#endif
#include "ui_picker.h"
#include "ui_time_picker.h"

namespace OHOS {
namespace ACELite {
PickerViewTddTest::PickerViewTddTest()
{}

void PickerViewTddTest::SetUp()
{
    Debugger::GetInstance().SetupJSContext();
    jerry_init(JERRY_INIT_EMPTY);

    g_globObj = jerry_get_global_object();
    jerry_value_t keyAttrs = jerry_create_string(reinterpret_cast<const jerry_char_t *>("attrs"));
    g_attrsObj = jerry_create_object();
    jerry_release_value(jerry_set_property(g_globObj, keyAttrs, g_attrsObj));
    jerry_release_value(keyAttrs);

    jerry_value_t keyStaticStyle = jerry_create_string(reinterpret_cast<const jerry_char_t *>("staticStyle"));
    g_styleObj = jerry_create_object();
    jerry_release_value(jerry_set_property(g_globObj, keyStaticStyle, g_styleObj));
    jerry_release_value(keyStaticStyle);
    rootComponentMock_.PrepareRootContainer();
}

void PickerViewTddTest::TearDown()
{
    JsAppContext::GetInstance()->ReleaseStyles();
    jerry_release_value(g_globObj);
    jerry_release_value(g_attrsObj);
    jerry_release_value(g_styleObj);
    jerry_cleanup();
}

Component* PickerViewTddTest::GetRenderedComponent(uint16_t componentKeyId) const
{
    jerry_value_t children = jerry_create_null();
    Component* component = ComponentFactory::CreateComponent(componentKeyId, g_globObj, children);
    rootComponentMock_.RenderComponent(*component);
    jerry_release_value(children);
    return component;
}

uint16_t PickerViewTddTest::SetCompnentNameId(const char* componentName)
{
    if (!strcmp(componentName, "picker-view")) {
        uint8_t len = 11;
        return g_componentNameId = KeyParser::ParseKeyId("picker-view", len);
    }
    return -1;
}

void PickerViewTddTest::SetTextPickerRange()
{
    uint32_t length = 2;
    jerry_value_t rangeValue = jerry_create_array(length);
    jerry_value_t firstValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>("AA"));
    jerry_value_t secValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>("BB"));
    jerry_set_property_by_index(rangeValue, 0, firstValue);
    jerry_set_property_by_index(rangeValue, 1, secValue);
    jerry_value_t rangeKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>("range"));
    jerry_release_value(jerry_set_property(g_attrsObj, rangeKey, rangeValue));
    jerry_release_value(rangeKey);
    jerry_release_value(firstValue);
    jerry_release_value(secValue);
    jerry_release_value(rangeValue);
}

void PickerViewTddTest::SetCharAttribute(const char* keyName, const char* attribute)
{
    jerry_value_t attrName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(keyName));
    jerry_value_t attrValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(attribute));
    jerry_release_value(jerry_set_property(g_attrsObj, attrName, attrValue));
    jerry_release_value(attrName);
    jerry_release_value(attrValue);
}

void PickerViewTddTest::SetNumAttribute(const char* keyName, const int32_t attribute)
{
    jerry_value_t attrName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(keyName));
    jerry_value_t attrValue = jerry_create_number(attribute);
    jerry_release_value(jerry_set_property(g_attrsObj, attrName, attrValue));
    jerry_release_value(attrName);
    jerry_release_value(attrValue);
}

void PickerViewTddTest::SetNumStyle(const char* keyName, const int32_t style)
{
    jerry_value_t styleName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(keyName));
    jerry_value_t styleValue = jerry_create_number(style);
    jerry_release_value(jerry_set_property(g_styleObj, styleName, styleValue));
    jerry_release_value(styleName);
    jerry_release_value(styleValue);
}

void PickerViewTddTest::ComponentPickerviewAttributeSetTest001()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("picker-view");

    /**
     * @tc.steps: step1. set invalid type attribute
     */
    SetCharAttribute("type", "unknown");

    Component* pickerComponent = reinterpret_cast<PickerViewComponent *>(GetRenderedComponent(g_componentNameId));
    if (!pickerComponent) {
        printf("ComponentPickerviewAttributeSetTest001 fail\n");
        return;
    }
    EXPECT_TRUE(pickerComponent != nullptr);
    UIPicker* textPicker = reinterpret_cast<UIPicker *>(pickerComponent->GetComponentRootView());
    UITimePicker* timePicker = reinterpret_cast<UITimePicker *>(pickerComponent->GetComponentRootView());
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
    UIDatePicker* datePicker = reinterpret_cast<UIDatePicker *>(pickerComponent->GetComponentRootView());
#endif

    /**
     * @tc.expected: step1. native picker object is nullptr
     */
    if (!textPicker && !timePicker
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
        && !datePicker
#endif
        ) {
        printf("ComponentPickerviewAttributeSetTest001 pass\n");
    } else {
        printf("ComponentPickerviewAttributeSetTest001 fail\n");
    }
    EXPECT_TRUE((textPicker == nullptr) && (timePicker == nullptr)
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
    && (datePicker != nullptr)
#endif
    );

    pickerComponent->Release();
    delete pickerComponent;
    pickerComponent = nullptr;

    TDD_CASE_END();
}

void PickerViewTddTest::ComponentPickerviewAttributeSetTest002()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("picker-view");

    /**
     * @tc.steps: step1. set type attribute to text
     */
    SetCharAttribute("type", "text");
    SetNumStyle("width", g_width);
    SetNumStyle("height", g_height);

    /**
     * @tc.steps: step2. set range attribute
     */
    SetTextPickerRange();

    /**
     * @tc.steps: step3. set selected attribute
     */
    SetNumAttribute("selected", 1);

    /**
     * @tc.expected: step4. render native textPicker object
     */
    Component* pickerComponent = reinterpret_cast<PickerViewComponent *>(GetRenderedComponent(g_componentNameId));
    if (!pickerComponent) {
        printf("ComponentPickerviewAttributeSetTest002 fail\n");
        return;
    }
    EXPECT_TRUE(pickerComponent != nullptr);
    UIPicker* textPicker = reinterpret_cast<UIPicker *>(pickerComponent->GetComponentRootView());

    /**
     * @tc.expected: step4. native textPicker object is not nullptr
     */
    if (!textPicker) {
        printf("ComponentPickerviewAttributeSetTest002 fail\n");
        delete pickerComponent;
        pickerComponent = nullptr;
        return;
    }
    EXPECT_TRUE(textPicker != nullptr);

    /**
     * @tc.expected: step4. GetSelected return 1
     */
    if (textPicker->GetSelected() == 1) {
        printf("ComponentPickerviewAttributeSetTest002 pass\n");
    } else {
        printf("ComponentPickerviewAttributeSetTest002 fail\n");
    }
    EXPECT_EQ(textPicker->GetSelected(), 1);
    pickerComponent->Release();
    delete pickerComponent;
    pickerComponent = nullptr;

    TDD_CASE_END();
}

void PickerViewTddTest::ComponentPickerviewAttributeSetTest003()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("picker-view");

    /**
     * @tc.steps: step1. set type attribute to text
     */
    SetCharAttribute("type", "text");
    SetNumStyle("width", g_width);
    SetNumStyle("height", g_height);

    /**
     * @tc.steps: step2. set invalid range attribute
     */
    uint32_t length = 2;
    jerry_value_t rangeValue = jerry_create_array(length);
    jerry_value_t firstValue = jerry_create_null();
    jerry_value_t secValue = jerry_create_undefined();
    jerry_set_property_by_index(rangeValue, 0, firstValue);
    jerry_set_property_by_index(rangeValue, 1, secValue);
    jerry_value_t rangeKey = jerry_create_string(reinterpret_cast<const jerry_char_t *>("range"));
    jerry_release_value(jerry_set_property(g_attrsObj, rangeKey, rangeValue));
    jerry_release_value(rangeKey);
    jerry_release_value(firstValue);
    jerry_release_value(secValue);
    jerry_release_value(rangeValue);

    /**
     * @tc.steps: step3. set selected attribute
     */
    SetNumAttribute("selected", 1);

    /**
     * @tc.expected: step4. render native textPicker object
     */
    Component* pickerComponent = reinterpret_cast<PickerViewComponent *>(GetRenderedComponent(g_componentNameId));
    if (!pickerComponent) {
        printf("ComponentPickerviewAttributeSetTest003 fail\n");
        return;
    }
    EXPECT_TRUE(pickerComponent != nullptr);
    UIPicker* textPicker = reinterpret_cast<UIPicker *>(pickerComponent->GetComponentRootView());

    /**
     * @tc.expected: step4. native textPicker object is not nullptr
     */
    if (!textPicker) {
        printf("ComponentPickerviewAttributeSetTest003 fail\n");
        delete pickerComponent;
        pickerComponent = nullptr;
        return;
    }
    EXPECT_TRUE(textPicker != nullptr);

    /**
     * @tc.expected: step4. GetSelected return 0
     */
    if (textPicker->GetSelected() == 0) {
        printf("ComponentPickerviewAttributeSetTest003 pass\n");
    } else {
        printf("ComponentPickerviewAttributeSetTest003 fail\n");
    }
    EXPECT_EQ(textPicker->GetSelected(), 0);
    pickerComponent->Release();
    delete pickerComponent;
    pickerComponent = nullptr;

    TDD_CASE_END();
}

void PickerViewTddTest::ComponentPickerviewAttributeSetTest004()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("picker-view");
    /**
     * @tc.steps: step1. set range attribute
     */
    SetTextPickerRange();

    /**
     * @tc.steps: step2. set invalid selected attribute
     */
    SetNumStyle("width", g_width);
    SetNumStyle("height", g_height);
    SetCharAttribute("selected", "invalid");

    /**
     * @tc.steps: step3. render native textPicker object
     */
    Component* pickerComponent = reinterpret_cast<PickerViewComponent *>(GetRenderedComponent(g_componentNameId));
    if (!pickerComponent) {
        printf("ComponentPickerviewAttributeSetTest004 fail\n");
        return;
    }
    EXPECT_TRUE(pickerComponent != nullptr);

    UIPicker* textPicker = reinterpret_cast<UIPicker *>(pickerComponent->GetComponentRootView());

    /**
     * @tc.expected: step3. native textPicker object is not nullptr
     */
    if (!textPicker) {
        printf("ComponentPickerviewAttributeSetTest004 fail\n");
        delete pickerComponent;
        pickerComponent = nullptr;
        return;
    }
    EXPECT_TRUE(textPicker != nullptr);

    /**
     * @tc.expected: step3. GetSelected return 0
     */
    if (textPicker->GetSelected() == 0) {
        printf("ComponentPickerviewAttributeSetTest004 pass\n");
    } else {
        printf("ComponentPickerviewAttributeSetTest004 fail\n");
    }
    EXPECT_EQ(textPicker->GetSelected(), 0);
    pickerComponent->Release();
    delete pickerComponent;
    pickerComponent = nullptr;

    TDD_CASE_END();
}

void PickerViewTddTest::ComponentPickerviewAttributeSetTest005()
{
    TDD_CASE_BEGIN();
    SetCompnentNameId("picker-view");
    /**
     * @tc.steps: step1. set range attribute
     */
    SetTextPickerRange();

    /**
     * @tc.steps: step2. set out of bound selected attribute
     */
    SetNumStyle("width", g_width);
    SetNumStyle("height", g_height);
    SetNumAttribute("selected", -1);

    /**
     * @tc.steps: step3. render native textPicker object
     */
    Component* pickerComponent = reinterpret_cast<PickerViewComponent *>(GetRenderedComponent(g_componentNameId));
    if (!pickerComponent) {
        printf("ComponentPickerviewAttributeSetTest005 fail\n");
        return;
    }
    EXPECT_TRUE(pickerComponent != nullptr);
    UIPicker* textPicker = reinterpret_cast<UIPicker *>(pickerComponent->GetComponentRootView());

    /**
     * @tc.expected: step3. native textPicker object is not nullptr
     */
    if (!textPicker) {
        printf("ComponentPickerviewAttributeSetTest005 fail\n");
        delete pickerComponent;
        pickerComponent = nullptr;
        return;
    }
    EXPECT_TRUE(textPicker != nullptr);

    /**
     * @tc.expected: step3. GetSelected return 0
     */
    if (textPicker->GetSelected() == 0) {
        printf("ComponentPickerviewAttributeSetTest005 pass\n");
    } else {
        printf("ComponentPickerviewAttributeSetTest005 fail\n");
    }
    EXPECT_EQ(textPicker->GetSelected(), 0);
    pickerComponent->Release();
    delete pickerComponent;
    pickerComponent = nullptr;

    TDD_CASE_END();
}

void PickerViewTddTest::RunTests()
{
    ComponentPickerviewAttributeSetTest001();
    ComponentPickerviewAttributeSetTest002();
    ComponentPickerviewAttributeSetTest003();
    ComponentPickerviewAttributeSetTest004();
    ComponentPickerviewAttributeSetTest005();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: ComponentPickerviewAttributeSetTest001
 * @tc.desc: Verify picker-view invalid type attribute.
 */
HWTEST_F(PickerViewTddTest, PickerviewAttr001, TestSize.Level1)
{
    PickerViewTddTest::ComponentPickerviewAttributeSetTest001();
}

/**
 * @tc.name: ComponentPickerviewAttributeSetTest002
 * @tc.desc: Verify text picker-view valid attributes.
 */
HWTEST_F(PickerViewTddTest, PickerviewAttr002, TestSize.Level0)
{
    PickerViewTddTest::ComponentPickerviewAttributeSetTest002();
}

/**
 * @tc.name: ComponentPickerviewAttributeSetTest003
 * @tc.desc: Verify text picker-view invalid range attribute.
 */
HWTEST_F(PickerViewTddTest, PickerviewAttr003, TestSize.Level1)
{
    PickerViewTddTest::ComponentPickerviewAttributeSetTest003();
}

/**
 * @tc.name: ComponentPickerviewAttributeSetTest004
 * @tc.desc: Verify text picker-view invalid selected attribute.
 */
HWTEST_F(PickerViewTddTest, PickerviewAttr004, TestSize.Level1)
{
    PickerViewTddTest::ComponentPickerviewAttributeSetTest004();
}

/**
 * @tc.name: ComponentPickerviewAttributeSetTest005
 * @tc.desc: Verify text picker-view with out of bound selected attribute.
 */
HWTEST_F(PickerViewTddTest, PickerviewAttr005, TestSize.Level1)
{
    PickerViewTddTest::ComponentPickerviewAttributeSetTest005();
}
#endif
} // ACELite
} // OHOS
