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

#include "base/utils/utils.h"
#include "core/components/picker/picker_option_component.h"
#include "core/components/picker/render_picker_base.h"
#include "core/components/picker/render_picker_column.h"
#include "core/components/picker/render_picker_option.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr uint32_t TEST_LEAP_YEAR = 2020;
constexpr uint32_t TEST_LEAP_HUNDREDS = 2000;
constexpr uint32_t TEST_OTHER_YEAR = 2019;
constexpr uint32_t TEST_OTHER_HUNDREDS = 1900;
constexpr uint32_t TEST_MONTH = 2;
constexpr uint32_t TEST_DAY = 1;
constexpr uint32_t TEST_BIG_MONTH = 3;
constexpr uint32_t TEST_SMALL_MONTH = 11;
constexpr uint32_t TEST_BIT_MONTH_MAX_DAY = 31;
constexpr uint32_t TEST_SMALL_MONTH_MAX_DAY = 30;
constexpr uint32_t TEST_LEAP_YEAR_MAX_DAY = 29u;
constexpr uint32_t TEST_OTHER_YEAR_MAX_DAY = 28u;
const std::string TEST_OPTION_TEXT = "new option text";
const std::string TEST_COLUMN_TAG = "year";
const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();

} // namespace

class RenderPickerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RenderPicker001
 * @tc.desc: Verify the max day of february would be calculate correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DTE1R
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the leap year's february and other february.
     */
    PickerDate leapYearDate(TEST_LEAP_YEAR, TEST_MONTH, TEST_DAY);
    PickerDate otherYearDate(TEST_OTHER_YEAR, TEST_MONTH, TEST_DAY);

    /**
     * @tc.steps: step2. call max day calculating, and check it's value.
     * @tc.expected: step2. max day of leap year's february is 29 and other 28.
     */
    uint32_t leapMaxDay = PickerDate::GetMaxDay(leapYearDate.GetYear(), leapYearDate.GetMonth());
    uint32_t otherMaxDay = PickerDate::GetMaxDay(otherYearDate.GetYear(), otherYearDate.GetMonth());

    EXPECT_EQ(leapMaxDay, TEST_LEAP_YEAR_MAX_DAY);
    EXPECT_EQ(otherMaxDay, TEST_OTHER_YEAR_MAX_DAY);
}

/**
 * @tc.name: RenderPicker002
 * @tc.desc: Verify the flag of render mark after update function.
 * @tc.type: FUNC
 * @tc.require: AR000DTE1R
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component and the render.
     */
    auto component = AceType::MakeRefPtr<PickerBaseComponent>();
    auto render = AceType::DynamicCast<RenderPickerBase>(component->CreateRenderNode());
    EXPECT_TRUE(render != nullptr);

    /**
     * @tc.steps: step2. call the update function of render, and check it's render mark flag.
     * @tc.expected: step2. the need render mark flag is true.
     */
    render->Update(component);
    EXPECT_EQ(render->NeedLayout(), true);
}

/**
 * @tc.name: RenderPicker003
 * @tc.desc: Verify the flag of selected of render picker option.
 * @tc.type: FUNC
 * @tc.require: AR000DTE1R
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component and the render.
     */
    auto component = AceType::MakeRefPtr<PickerOptionComponent>();
    auto render = AceType::DynamicCast<RenderPickerOption>(component->CreateRenderNode());
    EXPECT_TRUE(render != nullptr);

    /**
     * @tc.steps: step2. call the update function of render, and check it's flag of selected.
     * @tc.expected: step2. the flag is the same with the component.
     */
    render->Update(component);
    EXPECT_EQ(component->GetSelected(), render->GetSelected());
}

/**
 * @tc.name: RenderPicker004
 * @tc.desc: Verify the text of option after calling of UpdateValue.
 * @tc.type: FUNC
 * @tc.require: AR000DTE1R
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component and the render.
     */
    auto theme = THEME_MANAGER->GetTheme<PickerTheme>();
    auto component = AceType::MakeRefPtr<PickerOptionComponent>();
    component->SetTheme(theme->clone());
    component->Initialize();
    auto render = AceType::DynamicCast<RenderPickerOption>(component->CreateRenderNode());
    auto text = component->GetTextComponent();
    EXPECT_TRUE(render != nullptr);
    EXPECT_TRUE(text != nullptr);

    /**
     * @tc.steps: step2. call the UpdateValue function of render, and check it's text.
     * @tc.expected: step2. the text is the same with the input text.
     */
    render->Update(component);
    render->UpdateValue(0, TEST_OPTION_TEXT);
    EXPECT_EQ(text->GetData(), TEST_OPTION_TEXT);
}

/**
 * @tc.name: RenderPicker005
 * @tc.desc: Verify the flag of render mark after call function of UpdateScrollDelta.
 * @tc.type: FUNC
 * @tc.require: AR000DTE1R
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component and the render.
     */
    auto theme = THEME_MANAGER->GetTheme<PickerTheme>();
    auto component = AceType::MakeRefPtr<PickerOptionComponent>();
    component->SetTheme(theme->clone());
    component->Initialize();
    auto render = AceType::DynamicCast<RenderPickerOption>(component->CreateRenderNode());
    auto text = component->GetTextComponent();
    EXPECT_TRUE(render != nullptr);
    EXPECT_TRUE(text != nullptr);

    /**
     * @tc.steps: step2. call the UpdateScrollDelta function of render, and check it's render flag.
     * @tc.expected: step2. it's render flag is true which means need layout.
     */
    render->Update(component);
    render->UpdateScrollDelta(0);
    EXPECT_EQ(render->NeedLayout(), true);
}

/**
 * @tc.name: RenderPicker006
 * @tc.desc: Verify the column tag of render picker column.
 * @tc.type: FUNC
 * @tc.require: AR000DTE1R
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component and the render.
     */
    auto theme = THEME_MANAGER->GetTheme<PickerTheme>();
    auto component = AceType::MakeRefPtr<PickerColumnComponent>();
    component->SetTheme(theme->clone());
    component->AppendOption(TEST_OPTION_TEXT);
    component->SetColumnTag(TEST_COLUMN_TAG);
    component->Initialize();
    auto render = AceType::DynamicCast<RenderPickerColumn>(component->CreateRenderNode());
    EXPECT_TRUE(render != nullptr);

    /**
     * @tc.steps: step2. call the UpdateScrollDelta function of render, and check it's render flag.
     * @tc.expected: step2. it's render flag is true which means need layout.
     */
    render->Update(component);
    EXPECT_EQ(render->GetColumnTag(), TEST_COLUMN_TAG);
}

/**
 * @tc.name: RenderPicker007
 * @tc.desc: Verify the max day of big month would be calculate correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1Q8
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the big month date.
     */
    PickerDate bigMonthDate(TEST_OTHER_YEAR, TEST_BIG_MONTH, TEST_DAY);

    /**
     * @tc.steps: step2. call max day calculating, and check it's value.
     * @tc.expected: step2. max day of big month is 31 days.
     */
    uint32_t maxDay = PickerDate::GetMaxDay(bigMonthDate.GetYear(), bigMonthDate.GetMonth());
    EXPECT_EQ(maxDay, TEST_BIT_MONTH_MAX_DAY);
}

/**
 * @tc.name: RenderPicker008
 * @tc.desc: Verify the max day of small month would be calculate correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1Q8
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the small month date.
     */
    PickerDate smallMonthDate(TEST_OTHER_YEAR, TEST_SMALL_MONTH, TEST_DAY);

    /**
     * @tc.steps: step2. call max day calculating, and check it's value.
     * @tc.expected: step2. max day of small month is 30 days.
     */
    uint32_t maxDay = PickerDate::GetMaxDay(smallMonthDate.GetYear(), smallMonthDate.GetMonth());
    EXPECT_EQ(maxDay, TEST_SMALL_MONTH_MAX_DAY);
}

/**
 * @tc.name: RenderPicker009
 * @tc.desc: Verify the leap year would be calculate correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1Q8
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the leap year date.
     */
    PickerDate leapYear(TEST_LEAP_YEAR, TEST_MONTH, TEST_DAY);
    PickerDate leapHundreds(TEST_LEAP_HUNDREDS, TEST_MONTH, TEST_DAY);

    /**
     * @tc.steps: step2. call leap year checking function, and check it's value.
     * @tc.expected: step2. both year is leap year.
     */
    bool checkLeapYear = PickerDate::IsLeapYear(leapYear.GetYear());
    bool checkLeapHundreds = PickerDate::IsLeapYear(leapHundreds.GetYear());
    EXPECT_EQ(checkLeapYear, true);
    EXPECT_EQ(checkLeapHundreds, true);
}

/**
 * @tc.name: RenderPicker010
 * @tc.desc: Verify the none leap year would be calculate correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1Q8
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the none leap year date.
     */
    PickerDate otherYear(TEST_OTHER_YEAR, TEST_MONTH, TEST_DAY);
    PickerDate otherHundreds(TEST_OTHER_HUNDREDS, TEST_MONTH, TEST_DAY);

    /**
     * @tc.steps: step2. call leap year checking function, and check it's value.
     * @tc.expected: step2. both year is none leap year.
     */
    bool checkOtherYear = PickerDate::IsLeapYear(otherYear.GetYear());
    bool checkOtherHundreds = PickerDate::IsLeapYear(otherHundreds.GetYear());
    EXPECT_EQ(checkOtherYear, false);
    EXPECT_EQ(checkOtherHundreds, false);
}

/**
 * @tc.name: RenderPicker011
 * @tc.desc: Verify the flags of picker would be setted correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1Q8
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the picker component and set it's flags to be false.
     */
    auto picker = AceType::MakeRefPtr<PickerBaseComponent>();
    picker->SetHasButtons(false);
    picker->SetHasLunar(false);
    picker->SetHasTitle(false);

    /**
     * @tc.steps: step2. get it's flags, and check it's value.
     * @tc.expected: step2. all flags is false.
     */
    auto hasButtons = picker->GetHasButtons();
    auto hasLunar = picker->GetHasLunar();
    auto hasTitle = picker->GetHasTitle();
    EXPECT_EQ(hasButtons, false);
    EXPECT_EQ(hasLunar, false);
    EXPECT_EQ(hasTitle, false);
}

/**
 * @tc.name: RenderPicker012
 * @tc.desc: Verify the column would be removed correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1Q8
 * @tc.author: huangzhangmin
 */
HWTEST_F(RenderPickerTest, RenderPicker012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the picker component and operate appending and removing to column.
     */
    auto picker = AceType::MakeRefPtr<PickerBaseComponent>();
    auto column = AceType::MakeRefPtr<PickerColumnComponent>();
    column->SetColumnTag(TEST_COLUMN_TAG);
    picker->AppendColumn(column);
    picker->RemoveColumn(TEST_COLUMN_TAG);

    /**
     * @tc.steps: step2. get the column, and check it's value.
     * @tc.expected: step2. the column is removed which is nullptr.
     */
    auto nullColumn = picker->GetColumn(TEST_COLUMN_TAG);
    EXPECT_EQ(nullColumn, nullptr);
}

} // namespace OHOS::Ace
