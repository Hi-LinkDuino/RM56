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

#include "gtest/gtest.h"

#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/select/select_component.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/test/json/select_creator.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr std::size_t SELECT_TEST_OPTION_INDEX = 0;
constexpr std::size_t SELECT_TEST_OPTION_COUNT = 1;
const std::string SELECT_TEST_OPTION_VALUE = "item1";
const std::string SELECT_TEST_OPTION_TEXT = "menu1";
const std::string SELECT_TEST_OPTION_ICON = "icon.jpg";
const Dimension SELECT_TEST_FONT_SIZE(100.0);
constexpr std::size_t SELECT_TEST_OPTION_SIZE = 10;
const std::string SELECT_TEST_TIP_TEXT = "please select!";
const std::string SELECT_TEST_ON_CHANGED = "onChangedTest";
const std::string SELECT_TEST_FONT_FAMILY = "Kaushan,Italiana";

} // namespace

class SelectCreatorTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: SelectCreator001
 * @tc.desc: Verify SelectCreator can create SelectComponent with default value.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"options\" : ["
                                "    {"
                                "      \"value\": \"item1\","
                                "      \"text\" : \"menu1\""
                                "    }"
                                "  ]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);

    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);

    EXPECT_EQ(select->GetDisabled(), false);
    EXPECT_EQ(select->GetClicked(), false);
}

/**
 * @tc.name: SelectCreator002
 * @tc.desc: Verify SelectCreator can create SelectComponent with all values.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"disabled\": \"true\","
        "  \"selectedColor\": { \"className\": \"Color\", \"red\" : 255, \"green\" : 0, \"blue\" : 0 },"
        "  \"clickedColor\":{ \"className\": \"Color\", \"red\": 0, \"green\": 255, \"blue\": 0 },"
        "  \"fontSize\": 100,"
        "  \"optionSize\": 10,"
        "  \"onChanged\": \"handleChanged\","
        "  \"tipText\" : \"please select!\","
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);

    EXPECT_EQ(select->GetDisabled(), true);
    EXPECT_EQ(select->GetClicked(), false);
    EXPECT_EQ(select->GetClickedColor(), Color::GREEN);
    EXPECT_EQ(select->GetSelectedColor(), Color::RED);
    EXPECT_EQ(select->GetFontSize(), SELECT_TEST_FONT_SIZE);
    EXPECT_EQ(select->GetOptionSize(), SELECT_TEST_OPTION_SIZE);
    auto tipText = select->GetTipText();
    ASSERT_TRUE(tipText != nullptr);
    EXPECT_EQ(tipText->GetData(), SELECT_TEST_OPTION_TEXT);

    EXPECT_EQ(select->GetSelectOptionCount(), SELECT_TEST_OPTION_COUNT);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    EXPECT_EQ(option->GetSelected(), true);
    EXPECT_EQ(option->GetValue(), SELECT_TEST_OPTION_VALUE);
    auto text = option->GetText();
    ASSERT_TRUE(text != nullptr);
    EXPECT_EQ(text->GetData(), SELECT_TEST_OPTION_TEXT);
    auto icon = option->GetIcon();
    ASSERT_TRUE(icon != nullptr);
    EXPECT_EQ(icon->GetSrc(), SELECT_TEST_OPTION_ICON);
}

/**
 * @tc.name: SelectCreator003
 * @tc.desc: Verify SelectCreator can create SelectComponent with enabled.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"disabled\": \"false\","
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. disabled are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    EXPECT_EQ(select->GetDisabled(), false);
}

/**
 * @tc.name: SelectCreator004
 * @tc.desc: Verify SelectCreator can create SelectComponent with disabled.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"disabled\": \"true\","
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. disabled are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    EXPECT_EQ(select->GetDisabled(), true);
}

/**
 * @tc.name: SelectCreator005
 * @tc.desc: Verify SelectCreator can create SelectComponent with clicked color.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"clickedColor\": {\"className\":\"Color\",\"red\":255,\"green\":255,\"blue\":255},"
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. clickedColor are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    EXPECT_EQ(select->GetClickedColor(), Color::WHITE);
}

/**
 * @tc.name: SelectCreator006
 * @tc.desc: Verify SelectCreator can create SelectComponent with selected color.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"selectedColor\": {\"className\":\"Color\",\"red\":0,\"green\":0,\"blue\":0},"
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. selectedColor are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    EXPECT_EQ(select->GetSelectedColor(), Color::BLACK);
}

/**
 * @tc.name: SelectCreator007
 * @tc.desc: Verify SelectCreator can create SelectComponent with disabled color.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"disabledColor\": {\"className\":\"Color\",\"red\":255,\"green\":0,\"blue\":0},"
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. disabledColor are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    EXPECT_EQ(select->GetDisabledColor(), Color::RED);
}

/**
 * @tc.name: SelectCreator008
 * @tc.desc: Verify SelectCreator can create SelectComponent with onchange event.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"onChanged\": \"onChangedTest\","
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. onChanged event are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    EXPECT_EQ(select->GetOnChanged(), SELECT_TEST_ON_CHANGED);
}

/**
 * @tc.name: SelectCreator009
 * @tc.desc: Verify SelectCreator can create SelectComponent with tipText.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"tipText\": \"please select!\","
                                "  \"options\" :"
                                "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"icon\": \"icon.jpg\" }]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. tipText are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    ASSERT_TRUE(select->GetTipText() != nullptr);
    EXPECT_EQ(select->GetTipText()->GetData(), SELECT_TEST_TIP_TEXT);
}

/**
 * @tc.name: SelectCreator010
 * @tc.desc: Verify SelectCreator can create SelectComponent with fontSize.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"fontSize\": 100,"
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. fontSize are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    EXPECT_EQ(select->GetFontSize(), SELECT_TEST_FONT_SIZE);
}

/**
 * @tc.name: SelectCreator011
 * @tc.desc: Verify SelectCreator can create SelectComponent with fontFamily.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"fontFamilies\": \"Kaushan,Italiana\","
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. fontFamily are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    EXPECT_EQ(select->GetFontFamily(), SELECT_TEST_FONT_FAMILY);
}

/**
 * @tc.name: SelectCreator012
 * @tc.desc: Verify SelectCreator can create SelectComponent with optionSize.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPS
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"optionSize\": 10,"
        "  \"options\" :"
        "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\": \"true\", \"icon\": \"icon.jpg\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select component.
     * @tc.expected: step2. optionSize are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    EXPECT_EQ(select->GetOptionSize(), SELECT_TEST_OPTION_SIZE);
}

/**
 * @tc.name: SelectCreator013
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with icon.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"options\" :"
                                "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"icon\": \"icon.jpg\" }]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. icon are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    ASSERT_TRUE(option->GetIcon() != nullptr);
    EXPECT_EQ(option->GetIcon()->GetSrc(), SELECT_TEST_OPTION_ICON);
}

/**
 * @tc.name: SelectCreator014
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with value.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"options\" :"
                                "    [{ \"value\": \"item1\", \"text\" : \"menu1\" }]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. value are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    EXPECT_EQ(option->GetValue(), SELECT_TEST_OPTION_VALUE);
}

/**
 * @tc.name: SelectCreator015
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with text.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"options\" :"
                                "    [{ \"value\": \"item1\", \"text\" : \"menu1\" }]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. text are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    ASSERT_TRUE(option->GetText() != nullptr);
    EXPECT_EQ(option->GetText()->GetData(), SELECT_TEST_OPTION_TEXT);
}

/**
 * @tc.name: SelectCreator016
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with selected.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"options\" :"
                                "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\":\"true\" }]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. selected are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    EXPECT_EQ(option->GetSelected(), true);
}

/**
 * @tc.name: SelectCreator017
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with unselected.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"options\" :"
                                "    [{ \"value\": \"item1\", \"text\" : \"menu1\", \"selected\":\"false\" }]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. unselected are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    EXPECT_EQ(option->GetSelected(), false);
}

/**
 * @tc.name: SelectCreator018
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with fontColor.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"options\" : [{ \"text\" : \"t\", \"value\" : \"v\", "
        "    \"color\" : { \"className\" : \"Color\", \"red\" : 0, \"green\" : 255, \"blue\" : 0 } }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. fontColor are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    EXPECT_EQ(option->GetFontColor(), Color::GREEN);
}

/**
 * @tc.name: SelectCreator019
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with fontWeight.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"options\" :"
                                "    [{ \"text\" : \"t1\", \"value\" : \"v1\", \"fontWeight\" : 12 }]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. fontWeight are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    EXPECT_EQ(option->GetFontWeight(), FontWeight::LIGHTER);
}

/**
 * @tc.name: SelectCreator020
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with fontFamily.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator020, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr =
        ""
        "{"
        "  \"className\": \"Select\","
        "  \"options\" :"
        "    [{ \"text\" : \"t1\", \"value\" : \"v1\", \"fontFamilies\" : \"Kaushan,Italiana\" }]"
        "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. fontFamily are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    EXPECT_EQ(option->GetFontFamily(), SELECT_TEST_FONT_FAMILY);
}

/**
 * @tc.name: SelectCreator021
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with textDecoration.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator021, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"options\" :"
                                "    [{ \"text\" : \"t1\", \"value\" : \"v1\", \"textDecoration\" : 3 }]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. textDecoration are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    EXPECT_EQ(option->GetTextDecoration(), TextDecoration::LINE_THROUGH);
}

/**
 * @tc.name: SelectCreator022
 * @tc.desc: Verify SelectCreator can create SelectComponent has OptionComponent with fontSize.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPT
 * @tc.author: huangzhangmin
 */
HWTEST_F(SelectCreatorTest, SelectCreator022, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string.
     */
    const std::string jsonStr = ""
                                "{"
                                "  \"className\": \"Select\","
                                "  \"options\" :"
                                "    [{ \"text\" : \"t1\", \"value\" : \"v1\", \"fontSize\" : 100 }]"
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get select option component.
     * @tc.expected: step2. fontSize are set correctly.
     */
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(
        reinterpret_cast<const uint8_t*>(jsonStr.c_str()), jsonStr.size());
    ASSERT_TRUE(component != nullptr);
    auto select = AceType::DynamicCast<SelectComponent>(component);
    ASSERT_TRUE(select != nullptr);
    auto option = select->GetSelectOption(SELECT_TEST_OPTION_INDEX);
    ASSERT_TRUE(option != nullptr);
    EXPECT_EQ(option->GetFontSize(), SELECT_TEST_FONT_SIZE);
}

} // namespace OHOS::Ace
