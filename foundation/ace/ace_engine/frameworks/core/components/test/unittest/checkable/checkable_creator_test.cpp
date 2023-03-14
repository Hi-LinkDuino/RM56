/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "core/components/checkable/checkable_component.h"
#include "core/components/test/json/checkable_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {
const Color TEST_COLOR(123456789);
const std::string CHECKABLE_PROPERTIES = "  \"onChanged\": \"changeId001\",            "
                                         "  \"width\": 100,                            "
                                         "  \"height\": 100,                           "
                                         "  \"disabled\": true,                        "
                                         "  \"pointColor\": {                          "
                                         "  \"className\": \"Color\",                  "
                                         "  \"value\":  123456789                      "
                                         "  },                                         "
                                         "  \"activeColor\": {                          "
                                         "  \"className\": \"Color\",                  "
                                         "  \"value\":  123456789                      "
                                         "  },                                         "
                                         "  \"inactiveColor\": {                       "
                                         "  \"className\": \"Color\",                  "
                                         "  \"value\":  123456789                      "
                                         "  },                                         "
                                         "  \"focusColor\": {                          "
                                         "  \"className\": \"Color\",                  "
                                         "  \"value\":  123456789                      "
                                         "  },                                         ";
const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();

RefPtr<Component> CreateCheckableComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (!component) {
        return nullptr;
    }
    return component;
}

RefPtr<CheckboxComponent> CreateCheckboxComponent(const std::string& testJson)
{
    const auto& component = CreateCheckableComponent(testJson);
    return AceType::DynamicCast<CheckboxComponent>(component);
}

RefPtr<SwitchComponent> CreateSwitchComponent(const std::string& testJson)
{
    const auto& component = CreateCheckableComponent(testJson);
    return AceType::DynamicCast<SwitchComponent>(component);
}

RefPtr<RadioComponent<std::string>> CreateRadioComponent(const std::string& testJson)
{
    const auto& component = CreateCheckableComponent(testJson);
    return AceType::DynamicCast<RadioComponent<std::string>>(component);
}

void CheckCheckableColor(const RefPtr<CheckableComponent>& checkable)
{
    EXPECT_EQ(checkable->GetActiveColor().GetValue(), TEST_COLOR.GetValue());
    EXPECT_TRUE(checkable->GetInactiveColor() == TEST_COLOR);
    EXPECT_TRUE(checkable->GetFocusColor() == TEST_COLOR);
    EXPECT_EQ(checkable->GetPointColor(), TEST_COLOR);
}

void CheckCheckableDefaultColor(const RefPtr<CheckableComponent>& checkable, const RefPtr<CheckableTheme>& theme)
{
    EXPECT_TRUE(checkable->GetActiveColor() == theme->GetActiveColor());
    EXPECT_TRUE(checkable->GetInactiveColor() == theme->GetInactiveColor());
    EXPECT_TRUE(checkable->GetFocusColor() == theme->GetFocusColor());
    EXPECT_EQ(checkable->GetPointColor(), theme->GetPointColor());
}

void CheckCheckboxDefaultValue(const RefPtr<CheckboxComponent>& checkbox)
{
    EXPECT_TRUE(checkbox);
    RefPtr<CheckboxTheme> theme = THEME_MANAGER->GetTheme<CheckboxTheme>();
    EXPECT_EQ(checkbox->GetWidth().Value(), theme->GetWidth().Value());
    EXPECT_EQ(checkbox->GetHeight().Value(), theme->GetHeight().Value());
    CheckCheckableDefaultColor(checkbox, theme);
    EXPECT_EQ(checkbox->GetValue(), false);
    EXPECT_FALSE(checkbox->IsDisabled());
    EXPECT_TRUE(checkbox->GetChangeEvent().IsEmpty());
}

void CheckSwitchDefaultValue(const RefPtr<SwitchComponent>& switchComponent)
{
    EXPECT_TRUE(switchComponent);
    RefPtr<SwitchTheme> theme = THEME_MANAGER->GetTheme<SwitchTheme>();
    EXPECT_EQ(switchComponent->GetWidth().Value(), theme->GetWidth().Value());
    EXPECT_EQ(switchComponent->GetHeight().Value(), theme->GetHeight().Value());
    CheckCheckableDefaultColor(switchComponent, theme);
    EXPECT_EQ(switchComponent->GetValue(), false);
    EXPECT_FALSE(switchComponent->IsDisabled());
    EXPECT_TRUE(switchComponent->GetChangeEvent().IsEmpty());
}

void CheckRadioDefaultValue(const RefPtr<RadioComponent<std::string>>& radio)
{
    EXPECT_TRUE(radio);
    RefPtr<RadioTheme> theme = THEME_MANAGER->GetTheme<RadioTheme>();
    EXPECT_EQ(radio->GetWidth().Value(), theme->GetWidth().Value());
    EXPECT_EQ(radio->GetHeight().Value(), theme->GetHeight().Value());
    CheckCheckableDefaultColor(radio, theme);
    EXPECT_TRUE(radio->GetValue().empty());
    EXPECT_FALSE(radio->IsDisabled());
    EXPECT_TRUE(radio->GetGroupValue().empty());
    EXPECT_TRUE(radio->GetChangeEvent().IsEmpty());
}

} // namespace

class CheckableCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CheckableCreatorTest::SetUpTestCase() {}

void CheckableCreatorTest::TearDownTestCase() {}

void CheckableCreatorTest::SetUp() {}

void CheckableCreatorTest::TearDown() {}

/**
 * @tc.name: CheckableCreator001
 * @tc.desc: Test parse checkbox's field successfully from the right fields dsl json of checkbox component.
 * @tc.type: FUNC
 */
HWTEST_F(CheckableCreatorTest, CheckableCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with right fields, then create checkbox component with the json.
     * @tc.expected: step1. Checkbox component successfully created, and the fields value same with the json fields.
     */
    std::string dslJsonStart = "{ \"className\": \"Checkbox\",               ";
    std::string dslJsonCommon = CHECKABLE_PROPERTIES;
    std::string dslJsonEnd = "  \"value\": true                            "
                             "}                                            ";
    std::string dslJson = dslJsonStart.append(dslJsonCommon.append(dslJsonEnd));

    RefPtr<CheckboxComponent> checkbox = CreateCheckboxComponent(dslJson);

    EXPECT_TRUE(checkbox);
    EXPECT_EQ(checkbox->GetWidth().Value(), 100.0);
    EXPECT_EQ(checkbox->GetHeight().Value(), 100.0);
    CheckCheckableColor(checkbox);
    EXPECT_TRUE(checkbox->IsDisabled());
    EXPECT_TRUE(checkbox->GetValue());
    EXPECT_EQ(checkbox->GetChangeEvent(), "changeId001");
}

/**
 * @tc.name: CheckableCreator002
 * @tc.desc: Test parse checkbox's field successfully from no field dsl json of checkbox component.
 * @tc.type: FUNC
 */
HWTEST_F(CheckableCreatorTest, CheckableCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with no fields, then create checkbox component with the json.
     * @tc.expected: step1. Checkbox component successfully created, and the fields has default values.
     */
    std::string dslJson = "{ \"className\": \"Checkbox\" }";
    RefPtr<CheckboxComponent> checkbox = CreateCheckboxComponent(dslJson);
    CheckCheckboxDefaultValue(checkbox);
}

/**
 * @tc.name: CheckableCreator003
 * @tc.desc: Test parse checkbox's field successfully from the type error fields dsl json of checkbox component.
 * @tc.type: FUNC
 */
HWTEST_F(CheckableCreatorTest, CheckableCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with type error fields, then create checkbox component with the json.
     * @tc.expected: step1. Checkbox component successfully created, and the fields has default values.
     */
    std::string dslJson = "{                                            "
                          "  \"className\": \"Checkbox\",               "
                          "  \"onChanged\": 1,                          "
                          "  \"width\": true,                           "
                          "  \"height\": true,                          "
                          "  \"disabled\": 123,                         "
                          "  \"value\": 1                               "
                          "}                                            ";
    RefPtr<CheckboxComponent> checkbox = CreateCheckboxComponent(dslJson);
    CheckCheckboxDefaultValue(checkbox);
}

/**
 * @tc.name: CheckableCreator004
 * @tc.desc: Test parse switch's field successfully from the right fields dsl json of switch component.
 * @tc.type: FUNC
 */
HWTEST_F(CheckableCreatorTest, CheckableCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with right fields, then create switch component with the json.
     * @tc.expected: step1. Switch component successfully created, and the fields value same with the json fields.
     */
    std::string dslJsonStart = "{ \"className\": \"Switch\",                 ";
    std::string dslJsonCommon = CHECKABLE_PROPERTIES;
    std::string dslJsonEnd = "  \"value\": true                            "
                             "}                                            ";
    std::string dslJson = dslJsonStart.append(dslJsonCommon.append(dslJsonEnd));
    RefPtr<SwitchComponent> switchComponent = CreateSwitchComponent(dslJson);

    EXPECT_TRUE(switchComponent);
    EXPECT_EQ(switchComponent->GetWidth().Value(), 100.0);
    EXPECT_EQ(switchComponent->GetHeight().Value(), 100.0);
    CheckCheckableColor(switchComponent);
    EXPECT_TRUE(switchComponent->IsDisabled());
    EXPECT_TRUE(switchComponent->GetValue());
    EXPECT_EQ(switchComponent->GetChangeEvent(), "changeId001");
}

/**
 * @tc.name: CheckableCreator005
 * @tc.desc: Test parse switch's field successfully from no field dsl json of switch component.
 * @tc.type: FUNC
 */
HWTEST_F(CheckableCreatorTest, CheckableCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with no fields, then create switch component with the json.
     * @tc.expected: step1. Switch component successfully created, and the fields has default values.
     */
    std::string dslJson = "{ \"className\": \"Switch\" }";
    RefPtr<SwitchComponent> switchComponent = CreateSwitchComponent(dslJson);
    CheckSwitchDefaultValue(switchComponent);
}

/**
 * @tc.name: CheckableCreator006
 * @tc.desc: Test parse switch's field successfully from the type error fields dsl json of switch component.
 * @tc.type: FUNC
 */
HWTEST_F(CheckableCreatorTest, CheckableCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with type error fields, then create switch component with the json.
     * @tc.expected: step1. Switch component successfully created, and the fields has default values.
     */
    std::string dslJson = "{                                            "
                          "  \"className\": \"Switch\",                 "
                          "  \"onChanged\": 1,                          "
                          "  \"width\": true,                           "
                          "  \"height\": true,                          "
                          "  \"disabled\": 123,                         "
                          "  \"value\": 1                               "
                          "}                                            ";
    RefPtr<SwitchComponent> switchComponent = CreateSwitchComponent(dslJson);
    CheckSwitchDefaultValue(switchComponent);
}

/**
 * @tc.name: CheckableCreator007
 * @tc.desc: Test parse radio's field successfully from the right fields dsl json of radio component.
 * @tc.type: FUNC
 */
HWTEST_F(CheckableCreatorTest, CheckableCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with right fields, then create radio component with the json.
     * @tc.expected: step1. Radio component successfully created, and the fields value same with the json fields.
     */
    std::string dslJsonStart = "  { \"className\": \"Radio\",                  ";
    std::string dslJsonCommon = CHECKABLE_PROPERTIES;
    std::string dslJsonEnd = "  \"value\": \"red\",                        "
                             "  \"groupValue\": \"red\"                    "
                             "}                                            ";
    std::string dslJson = dslJsonStart.append(dslJsonCommon.append(dslJsonEnd));
    RefPtr<RadioComponent<std::string>> radio = CreateRadioComponent(dslJson);

    EXPECT_TRUE(radio);
    EXPECT_EQ(radio->GetWidth().Value(), 100.0);
    EXPECT_EQ(radio->GetHeight().Value(), 100.0);
    CheckCheckableColor(radio);
    EXPECT_TRUE(radio->IsDisabled());
    EXPECT_EQ(radio->GetValue(), "red");
    EXPECT_EQ(radio->GetGroupValue(), "red");
    EXPECT_EQ(radio->GetChangeEvent(), "changeId001");
}

/**
 * @tc.name: CheckableCreator008
 * @tc.desc: Test parse radio's field successfully from no field dsl json of radio component.
 * @tc.type: FUNC
 */
HWTEST_F(CheckableCreatorTest, CheckableCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with no fields, then create radio component with the json.
     * @tc.expected: step1. Radio component successfully created, and the fields has default values.
     */
    std::string dslJson = "{ \"className\": \"Radio\" }";
    RefPtr<RadioComponent<std::string>> radio = CreateRadioComponent(dslJson);
    CheckRadioDefaultValue(radio);
}

/**
 * @tc.name: CheckableCreator009
 * @tc.desc: Test parse radio's field successfully from the type error fields dsl json of radio component.
 * @tc.type: FUNC
 */
HWTEST_F(CheckableCreatorTest, CheckableCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with type error fields, then create radio component with the json.
     * @tc.expected: step1. Radio component successfully created, and the fields has default values.
     */
    std::string dslJson = "{                                            "
                          "  \"className\": \"Radio\",                  "
                          "  \"onChanged\": 1,                          "
                          "  \"width\": true,                           "
                          "  \"height\": true,                          "
                          "  \"disabled\": 123,                         "
                          "  \"value\": 1,                              "
                          "  \"groupValue\": 1                          "
                          "}                                            ";
    RefPtr<RadioComponent<std::string>> radio = CreateRadioComponent(dslJson);
    CheckRadioDefaultValue(radio);
}

} // namespace OHOS::Ace