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

#include "base/subwindow/subwindow.h"
#include "base/utils/utils.h"
#include "core/components/menu/menu_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string TEST_TITLE = "Title of test.";
const uint32_t TEST_INVALIDATE_INDEX = 0xffffffff;
const uint32_t TEST_OPTION_COUNT = 10u;

} // namespace

RefPtr<Subwindow> Subwindow::CreateSubwindow(int32_t instanceId)
{
    return nullptr;
}

class MenuComponentTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: MenuComponent001
 * @tc.desc: Verify MenuComponent can get and set title correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1QT
 * @tc.author: huangzhangmin
 */
HWTEST_F(MenuComponentTest, MenuComponent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component and set title.
     */
    auto menu = AceType::MakeRefPtr<MenuComponent>("", "");
    menu->SetTitle(TEST_TITLE);

    /**
     * @tc.steps: step2. call get title interface and check it's value.
     * @tc.expected: step2. the value is correct.
     */
    auto title = menu->GetTitle();
    EXPECT_EQ(title, TEST_TITLE);
}

/**
 * @tc.name: MenuComponent002
 * @tc.desc: Verify MenuComponent can get option with invalidate index.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1QT
 * @tc.author: huangzhangmin
 */
HWTEST_F(MenuComponentTest, MenuComponent002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component.
     */
    auto menu = AceType::MakeRefPtr<MenuComponent>("", "");

    /**
     * @tc.steps: step2. call get option with invalidate index.
     * @tc.expected: step2. can get option, and is nullptr.
     */
    auto option = menu->GetOption(TEST_INVALIDATE_INDEX);
    EXPECT_EQ(option, nullptr);
}

/**
 * @tc.name: MenuComponent003
 * @tc.desc: Verify MenuComponent can append option correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1QT
 * @tc.author: huangzhangmin
 */
HWTEST_F(MenuComponentTest, MenuComponent003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component with options.
     */
    auto menu = AceType::MakeRefPtr<MenuComponent>("", "");
    for (uint32_t index = 0; index < TEST_OPTION_COUNT; index++) {
        auto option = AceType::MakeRefPtr<OptionComponent>();
        option->SetIndex(index);
        menu->AppendOption(option);
    }

    /**
     * @tc.steps: step2. call get option count, and check it's value.
     * @tc.expected: step2. it's value is correct.
     */
    auto count = menu->GetOptionCount();
    EXPECT_EQ(count, TEST_OPTION_COUNT);
}

/**
 * @tc.name: MenuComponent004
 * @tc.desc: Verify MenuComponent can clear all option correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1QT
 * @tc.author: huangzhangmin
 */
HWTEST_F(MenuComponentTest, MenuComponent004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component with options.
     */
    auto menu = AceType::MakeRefPtr<MenuComponent>("", "");
    for (uint32_t index = 0; index < TEST_OPTION_COUNT; index++) {
        auto option = AceType::MakeRefPtr<OptionComponent>();
        option->SetIndex(index);
        menu->AppendOption(option);
    }

    /**
     * @tc.steps: step2. call clear all option, and check it's result.
     * @tc.expected: step2. it's result is correct.
     */
    menu->ClearOptions();
    auto result = menu->GetOptionCount();
    EXPECT_EQ(result, 0u);
}

/**
 * @tc.name: MenuComponent005
 * @tc.desc: Verify MenuComponent can set text color correctly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1QT
 * @tc.author: huangzhangmin
 */
HWTEST_F(MenuComponentTest, MenuComponent005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the component with text color green.
     */
    auto menu = AceType::MakeRefPtr<MenuComponent>("", "");
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    menu->InitTheme(themeManager);
    TextStyle style;
    style.SetTextColor(Color::GREEN);
    menu->SetTitleStyle(style);

    /**
     * @tc.steps: step2. call get style, and check it's text color.
     * @tc.expected: step2. it's text color is correct.
     */
    style = menu->GetTitleStyle();
    EXPECT_EQ(style.GetTextColor(), Color::GREEN);
}

} // namespace OHOS::Ace
