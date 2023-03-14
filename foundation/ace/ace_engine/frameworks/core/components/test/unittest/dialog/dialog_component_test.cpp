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

#include "base/geometry/dimension.h"
#include "base/utils/system_properties.h"
#include "core/components/dialog/dialog_component.h"
#include "core/components/dialog_tween/dialog_tween_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/focusable/focusable_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/scroll/scroll_component.h"
#include "core/components/text/text_component.h"
#include "core/components/transition/transition_component.h"
#include "core/components/wrap/wrap_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string TEXT_VALUE = "HelloWorld";
const std::string TEXT_VALUE_DEFAULT = "HiAce";
const Edge PADDING_VALUE = Edge(24.0, 24.0, 24.0, 0.0, DimensionUnit::VP);
const Edge PADDING_TITLE = Edge(Dimension(20.0, DimensionUnit::VP));
const Edge PADDING_CONTENT = Edge(Dimension(10.0, DimensionUnit::VP));
const Edge PADDING_DEFAULT = Edge(Dimension(24.0, DimensionUnit::VP));
const Edge PADDING_ACTIONS = Edge(24.0, 18.0, 24.0, 10.0, DimensionUnit::VP);
const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();

} // namespace

class DialogComponentTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}

protected:
    RefPtr<ColumnComponent> BuildAndGetColumn(const RefPtr<DialogComponent>& dialog);
    RefPtr<ColumnComponent> BuildAndGetColumnForWatch(const RefPtr<DialogComponent>& dialog);
    std::list<RefPtr<ButtonComponent>> BuildActions(int32_t actionsCount);
};

RefPtr<ColumnComponent> DialogComponentTest::BuildAndGetColumn(const RefPtr<DialogComponent>& dialog)
{
    auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
    dialog->SetDeviceType(DeviceType::PHONE);
    dialog->SetDialogTweenBox(boxComponent);
    dialog->BuildChild(THEME_MANAGER);
    auto dialogTween = AceType::DynamicCast<DialogTweenComponent>(boxComponent->GetChild());
    EXPECT_TRUE(dialogTween);
    auto transition = AceType::DynamicCast<TransitionComponent>(dialogTween->GetChild());
    EXPECT_TRUE(transition);
    auto box = AceType::DynamicCast<BoxComponent>(transition->GetChild());
    EXPECT_TRUE(box);
    auto focusCollaboration = AceType::DynamicCast<FocusCollaborationComponent>(box->GetChild());
    EXPECT_TRUE(focusCollaboration);
    RefPtr<ColumnComponent> column;
    if (dialog->GetActions().empty()) {
        auto focusable = AceType::DynamicCast<FocusableComponent>(focusCollaboration->GetChildren().front());
        EXPECT_TRUE(focusable);
        column = AceType::DynamicCast<ColumnComponent>(focusable->GetChild());
    } else {
        column = AceType::DynamicCast<ColumnComponent>(focusCollaboration->GetChildren().front());
    }
    return column;
}

RefPtr<ColumnComponent> DialogComponentTest::BuildAndGetColumnForWatch(const RefPtr<DialogComponent>& dialog)
{
    auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
    dialog->SetDeviceType(DeviceType::WATCH);
    dialog->SetDialogTweenBox(boxComponent);
    dialog->BuildChild(THEME_MANAGER);
    auto dialogTween = AceType::DynamicCast<DialogTweenComponent>(boxComponent->GetChild());
    EXPECT_TRUE(dialogTween);
    auto transition = AceType::DynamicCast<TransitionComponent>(dialogTween->GetChild());
    EXPECT_TRUE(transition);
    auto box = AceType::DynamicCast<BoxComponent>(transition->GetChild());
    EXPECT_TRUE(box);
    auto scroll = AceType::DynamicCast<ScrollComponent>(box->GetChild());
    EXPECT_TRUE(scroll);
    auto focusCollaboration = AceType::DynamicCast<FocusCollaborationComponent>(scroll->GetChild());
    EXPECT_TRUE(focusCollaboration);
    RefPtr<ColumnComponent> column;
    if (dialog->GetActions().empty()) {
        auto focusable = AceType::DynamicCast<FocusableComponent>(focusCollaboration->GetChildren().front());
        EXPECT_TRUE(focusable);
        column = AceType::DynamicCast<ColumnComponent>(focusable->GetChild());
    } else {
        column = AceType::DynamicCast<ColumnComponent>(focusCollaboration->GetChildren().front());
    }
    return column;
}

std::list<RefPtr<ButtonComponent>> DialogComponentTest::BuildActions(int32_t actionsCount)
{
    std::list<RefPtr<Component>> buttonChildren;
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    buttonChildren.emplace_back(text);
    RefPtr<ButtonComponent> button = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    std::list<RefPtr<ButtonComponent>> actions;
    for (int32_t i = 0; i < actionsCount; ++i) {
        actions.emplace_back(button);
    }
    return actions;
}

/**
 * @tc.name: DialogComponentBuild001
 * @tc.desc: Verify the Build Interface of DialogComponent work correctly with title.
 * @tc.type: FUNC
 */
HWTEST_F(DialogComponentTest, DialogComponentBuild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent with title.
     */
    RefPtr<DialogComponent> dialog = AceType::MakeRefPtr<DialogComponent>();
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    dialog->SetTitle(text);

    /**
     * @tc.steps: step2. call the Build interface of DialogComponent.
     * @tc.expected: step2. child of DialogComponent is built correctly.
     */
    RefPtr<ColumnComponent> column = BuildAndGetColumn(dialog);
    EXPECT_TRUE(column->GetChildren().size() == 1);
    RefPtr<FlexItemComponent> flexItem = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().front());
    EXPECT_TRUE(flexItem != nullptr);
    RefPtr<PaddingComponent> padding;
    if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
        RefPtr<RowComponent> brow = AceType::DynamicCast<RowComponent>(flexItem->GetChild());
        EXPECT_TRUE(brow != nullptr);
        RefPtr<BoxComponent> row = AceType::DynamicCast<BoxComponent>(brow->GetChildren().front());
        EXPECT_TRUE(row != nullptr);
        padding = AceType::DynamicCast<PaddingComponent>(row->GetChild());
    } else {
        RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(flexItem->GetChild());
        EXPECT_TRUE(row != nullptr);
        padding = AceType::DynamicCast<PaddingComponent>(row->GetChildren().front());
    }
    EXPECT_TRUE(padding != nullptr);
    RefPtr<BoxComponent> box = AceType::DynamicCast<BoxComponent>(padding->GetChild());
    EXPECT_TRUE(box != nullptr);
    RefPtr<TextComponent> textComponent = AceType::DynamicCast<TextComponent>(box->GetChild());
    EXPECT_TRUE(textComponent != nullptr);
    EXPECT_EQ(textComponent->GetData(), TEXT_VALUE_DEFAULT);
}

/**
 * @tc.name: DialogComponentBuild002
 * @tc.desc: Verify the Build Interface of DialogComponent work correctly with content.
 * @tc.type: FUNC
 */
HWTEST_F(DialogComponentTest, DialogComponentBuild002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent with content.
     */
    RefPtr<DialogComponent> dialog = AceType::MakeRefPtr<DialogComponent>();
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    dialog->SetContent(text);

    /**
     * @tc.steps: step2. call the Build interface of DialogComponent.
     * @tc.expected: step2. child of DialogComponent is built correctly.
     */
    RefPtr<ColumnComponent> column = BuildAndGetColumn(dialog);
    EXPECT_TRUE(column->GetChildren().size() == 1);
    RefPtr<FlexItemComponent> flexItem = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().front());
    EXPECT_TRUE(flexItem != nullptr);
    RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(flexItem->GetChild());
    EXPECT_TRUE(padding != nullptr);
    RefPtr<ScrollComponent> scroll = AceType::DynamicCast<ScrollComponent>(padding->GetChild());
    EXPECT_TRUE(scroll != nullptr);
    RefPtr<TextComponent> textComponent = AceType::DynamicCast<TextComponent>(scroll->GetChild());
    EXPECT_TRUE(textComponent != nullptr);
    EXPECT_EQ(textComponent->GetData(), TEXT_VALUE_DEFAULT);
}

/**
 * @tc.name: DialogComponentBuild003
 * @tc.desc: Verify the Build Interface of DialogComponent work correctly with actions.
 * @tc.type: FUNC
 */
HWTEST_F(DialogComponentTest, DialogComponentBuild003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent with actions.
     */
    RefPtr<DialogComponent> dialog = AceType::MakeRefPtr<DialogComponent>();
    std::list<RefPtr<ButtonComponent>> actions = BuildActions(1);
    dialog->SetActions(actions);

    /**
     * @tc.steps: step2. call the Build interface of DialogComponent.
     * @tc.expected: step2. child of DialogComponent is built correctly.
     */
    RefPtr<ColumnComponent> column = BuildAndGetColumn(dialog);
    EXPECT_TRUE(column->GetChildren().size() == 1);
    RefPtr<FlexItemComponent> flexItem = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().front());
    EXPECT_TRUE(flexItem != nullptr);
    RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(flexItem->GetChild());
    EXPECT_TRUE(padding != nullptr);
    RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(padding->GetChild());
    EXPECT_TRUE(row != nullptr);
    EXPECT_TRUE(row->GetChildren().size() == 1);
}

/**
 * @tc.name: DialogComponentBuild004
 * @tc.desc: Verify the Build Interface of DialogComponent work correctly with title and content.
 * @tc.type: FUNC
 */
HWTEST_F(DialogComponentTest, DialogComponentBuild004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent with title and content.
     */
    RefPtr<DialogComponent> dialog = AceType::MakeRefPtr<DialogComponent>();
    RefPtr<TextComponent> title = AceType::MakeRefPtr<TextComponent>("HiAce");
    dialog->SetTitle(title);
    RefPtr<TextComponent> content = AceType::MakeRefPtr<TextComponent>("HelloWorld");
    dialog->SetContent(content);

    /**
     * @tc.steps: step2. call the Build interface of DialogComponent.
     * @tc.expected: step2. child of DialogComponent is built correctly.
     */
    RefPtr<ColumnComponent> column = BuildAndGetColumn(dialog);
    EXPECT_TRUE(column->GetChildren().size() == 2);
    RefPtr<FlexItemComponent> titleFlex = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().front());
    EXPECT_TRUE(titleFlex != nullptr);
    RefPtr<PaddingComponent> titlePadding;
    if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
        RefPtr<RowComponent> brow = AceType::DynamicCast<RowComponent>(titleFlex->GetChild());
        EXPECT_TRUE(brow != nullptr);
        RefPtr<BoxComponent> row = AceType::DynamicCast<BoxComponent>(brow->GetChildren().front());
        EXPECT_TRUE(row != nullptr);
        titlePadding = AceType::DynamicCast<PaddingComponent>(row->GetChild());
    } else {
        RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(titleFlex->GetChild());
        EXPECT_TRUE(row != nullptr);
        titlePadding = AceType::DynamicCast<PaddingComponent>(row->GetChildren().front());
    }
    EXPECT_TRUE(titlePadding != nullptr);
    RefPtr<BoxComponent> box = AceType::DynamicCast<BoxComponent>(titlePadding->GetChild());
    EXPECT_TRUE(box != nullptr);
    RefPtr<TextComponent> titleComponent = AceType::DynamicCast<TextComponent>(box->GetChild());
    EXPECT_TRUE(titleComponent != nullptr);
    EXPECT_EQ(titleComponent->GetData(), TEXT_VALUE_DEFAULT);
    RefPtr<FlexItemComponent> contentFlex = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().back());
    EXPECT_TRUE(contentFlex != nullptr);
    RefPtr<PaddingComponent> contentPadding = AceType::DynamicCast<PaddingComponent>(contentFlex->GetChild());
    EXPECT_TRUE(contentPadding != nullptr);
    RefPtr<ScrollComponent> scroll = AceType::DynamicCast<ScrollComponent>(contentPadding->GetChild());
    EXPECT_TRUE(scroll != nullptr);
    RefPtr<TextComponent> contentComponent = AceType::DynamicCast<TextComponent>(scroll->GetChild());
    EXPECT_TRUE(contentComponent != nullptr);
    EXPECT_EQ(contentComponent->GetData(), TEXT_VALUE);
}

/**
 * @tc.name: DialogComponentBuild005
 * @tc.desc: Verify the Build Interface of DialogComponent work correctly with title and actions.
 * @tc.type: FUNC
 */
HWTEST_F(DialogComponentTest, DialogComponentBuild005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent with title and actions.
     */
    RefPtr<DialogComponent> dialog = AceType::MakeRefPtr<DialogComponent>();
    RefPtr<TextComponent> title = AceType::MakeRefPtr<TextComponent>("HiAce");
    dialog->SetTitle(title);
    std::list<RefPtr<ButtonComponent>> actions = BuildActions(2);
    dialog->SetActions(actions);

    /**
     * @tc.steps: step2. call the Build interface of DialogComponent.
     * @tc.expected: step2. child of DialogComponent is built correctly.
     */
    RefPtr<ColumnComponent> column = BuildAndGetColumn(dialog);
    EXPECT_TRUE(column->GetChildren().size() == 2);
    RefPtr<FlexItemComponent> titleFlex = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().front());
    EXPECT_TRUE(titleFlex != nullptr);
    RefPtr<PaddingComponent> titlePadding;
    if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
        RefPtr<RowComponent> brow = AceType::DynamicCast<RowComponent>(titleFlex->GetChild());
        EXPECT_TRUE(brow != nullptr);
        RefPtr<BoxComponent> row = AceType::DynamicCast<BoxComponent>(brow->GetChildren().front());
        EXPECT_TRUE(row != nullptr);
        titlePadding = AceType::DynamicCast<PaddingComponent>(row->GetChild());
    } else {
        RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(titleFlex->GetChild());
        EXPECT_TRUE(row != nullptr);
        titlePadding = AceType::DynamicCast<PaddingComponent>(row->GetChildren().front());
    }
    EXPECT_TRUE(titlePadding != nullptr);
    RefPtr<BoxComponent> box = AceType::DynamicCast<BoxComponent>(titlePadding->GetChild());
    EXPECT_TRUE(box != nullptr);
    RefPtr<TextComponent> titleComponent = AceType::DynamicCast<TextComponent>(box->GetChild());
    EXPECT_TRUE(titleComponent != nullptr);
    EXPECT_EQ(titleComponent->GetData(), TEXT_VALUE_DEFAULT);
    RefPtr<FlexItemComponent> actionsFlex = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().back());
    EXPECT_TRUE(actionsFlex != nullptr);
    RefPtr<PaddingComponent> actionsPadding = AceType::DynamicCast<PaddingComponent>(actionsFlex->GetChild());
    EXPECT_TRUE(actionsPadding != nullptr);
    RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(actionsPadding->GetChild());
    EXPECT_TRUE(row != nullptr);
    EXPECT_TRUE(row->GetChildren().size() == 3);
}

/**
 * @tc.name: DialogComponentBuild006
 * @tc.desc: Verify the Build Interface of DialogComponent work correctly with content and actions.
 * @tc.type: FUNC
 */
HWTEST_F(DialogComponentTest, DialogComponentBuild006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent with content and actions.
     */
    RefPtr<DialogComponent> dialog = AceType::MakeRefPtr<DialogComponent>();
    RefPtr<TextComponent> content = AceType::MakeRefPtr<TextComponent>("HiAce");
    dialog->SetContent(content);
    std::list<RefPtr<ButtonComponent>> actions = BuildActions(3);
    dialog->SetActions(actions);

    /**
     * @tc.steps: step2. call the Build interface of DialogComponent.
     * @tc.expected: step2. child of DialogComponent is built correctly.
     */
    RefPtr<ColumnComponent> column = BuildAndGetColumn(dialog);
    EXPECT_TRUE(column->GetChildren().size() == 2);
    RefPtr<FlexItemComponent> contentFlex = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().front());
    EXPECT_TRUE(contentFlex != nullptr);
    RefPtr<PaddingComponent> contentPadding = AceType::DynamicCast<PaddingComponent>(contentFlex->GetChild());
    EXPECT_TRUE(contentPadding != nullptr);
    RefPtr<ScrollComponent> scroll = AceType::DynamicCast<ScrollComponent>(contentPadding->GetChild());
    EXPECT_TRUE(scroll != nullptr);
    RefPtr<TextComponent> contentComponent = AceType::DynamicCast<TextComponent>(scroll->GetChild());
    EXPECT_TRUE(contentComponent != nullptr);
    EXPECT_EQ(contentComponent->GetData(), TEXT_VALUE_DEFAULT);
    RefPtr<FlexItemComponent> actionsFlex = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().back());
    EXPECT_TRUE(actionsFlex != nullptr);
    RefPtr<PaddingComponent> actionsPadding = AceType::DynamicCast<PaddingComponent>(actionsFlex->GetChild());
    EXPECT_TRUE(actionsPadding != nullptr);
    RefPtr<WrapComponent> wrapActions = AceType::DynamicCast<WrapComponent>(actionsPadding->GetChild());
    EXPECT_TRUE(wrapActions != nullptr);
    EXPECT_TRUE(wrapActions->GetChildren().size() == 3);
}

/**
 * @tc.name: DialogComponentBuild007
 * @tc.desc: Verify the Build Interface of DialogComponent work correctly with title, content and actions.
 * @tc.type: FUNC
 */
HWTEST_F(DialogComponentTest, DialogComponentBuild007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent with title, content and actions.
     */
    RefPtr<DialogComponent> dialog = AceType::MakeRefPtr<DialogComponent>();
    RefPtr<TextComponent> title = AceType::MakeRefPtr<TextComponent>("HiAce");
    dialog->SetTitle(title);
    RefPtr<TextComponent> content = AceType::MakeRefPtr<TextComponent>("HelloWorld");
    dialog->SetContent(content);
    std::list<RefPtr<ButtonComponent>> actions = BuildActions(4);
    dialog->SetActions(actions);

    /**
     * @tc.steps: step2. call the Build interface of DialogComponent.
     * @tc.expected: step2. child of DialogComponent is built correctly.
     */
    RefPtr<ColumnComponent> column = BuildAndGetColumn(dialog);
    EXPECT_TRUE(column->GetChildren().size() == 3);
    int32_t index = 0;
    for (const auto& child : column->GetChildren()) {
        ++index;
        if (index == 1) {
            RefPtr<FlexItemComponent> titleFlex = AceType::DynamicCast<FlexItemComponent>(child);
            EXPECT_TRUE(titleFlex != nullptr);
            RefPtr<PaddingComponent> titlePadding;
            if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
                RefPtr<RowComponent> brow = AceType::DynamicCast<RowComponent>(titleFlex->GetChild());
                EXPECT_TRUE(brow != nullptr);
                RefPtr<BoxComponent> row = AceType::DynamicCast<BoxComponent>(brow->GetChildren().front());
                EXPECT_TRUE(row != nullptr);
                titlePadding = AceType::DynamicCast<PaddingComponent>(row->GetChild());
            } else {
                RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(titleFlex->GetChild());
                EXPECT_TRUE(row != nullptr);
                titlePadding = AceType::DynamicCast<PaddingComponent>(row->GetChildren().front());
            }
            EXPECT_TRUE(titlePadding != nullptr);
            RefPtr<BoxComponent> box = AceType::DynamicCast<BoxComponent>(titlePadding->GetChild());
            EXPECT_TRUE(box != nullptr);
            RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(box->GetChild());
            EXPECT_TRUE(text != nullptr);
            EXPECT_EQ(text->GetData(), TEXT_VALUE_DEFAULT);
        } else if (index == 2) {
            RefPtr<FlexItemComponent> contentFlex = AceType::DynamicCast<FlexItemComponent>(child);
            EXPECT_TRUE(contentFlex != nullptr);
            RefPtr<PaddingComponent> contentPadding = AceType::DynamicCast<PaddingComponent>(contentFlex->GetChild());
            EXPECT_TRUE(contentPadding != nullptr);
            RefPtr<ScrollComponent> contentScroll = AceType::DynamicCast<ScrollComponent>(contentPadding->GetChild());
            EXPECT_TRUE(contentScroll != nullptr);
            RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(contentScroll->GetChild());
            EXPECT_TRUE(text != nullptr);
            EXPECT_EQ(text->GetData(), TEXT_VALUE);
        } else if (index == 3) {
            RefPtr<FlexItemComponent> actionFlex = AceType::DynamicCast<FlexItemComponent>(child);
            EXPECT_TRUE(actionFlex != nullptr);
            RefPtr<PaddingComponent> actionsPadding = AceType::DynamicCast<PaddingComponent>(actionFlex->GetChild());
            EXPECT_TRUE(actionsPadding != nullptr);
            RefPtr<WrapComponent> wrapColumn = AceType::DynamicCast<WrapComponent>(actionsPadding->GetChild());
            EXPECT_TRUE(wrapColumn != nullptr);
            EXPECT_TRUE(wrapColumn->GetChildren().size() == 3);
        }
    }
}

/**
 * @tc.name: DialogComponentBuild008
 * @tc.desc: Verify the Build Interface of DialogComponent work correctly with
 *           title, titlePadding, content, contentPadding and actions.
 * @tc.type: FUNC
 */
HWTEST_F(DialogComponentTest, DialogComponentBuild008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent with title, titlePadding, content, contentPadding and actions.
     */
    RefPtr<DialogComponent> dialog = AceType::MakeRefPtr<DialogComponent>();
    RefPtr<TextComponent> title = AceType::MakeRefPtr<TextComponent>("HiAce");
    dialog->SetTitle(title);
    dialog->SetTitlePadding(PADDING_TITLE);
    RefPtr<TextComponent> content = AceType::MakeRefPtr<TextComponent>("HelloWorld");
    dialog->SetContent(content);
    dialog->SetContentPadding(PADDING_CONTENT);
    std::list<RefPtr<ButtonComponent>> actions = BuildActions(2);
    dialog->SetActions(actions);

    /**
     * @tc.steps: step2. call the Build interface of DialogComponent.
     * @tc.expected: step2. child of DialogComponent is built correctly.
     */
    RefPtr<ColumnComponent> column = BuildAndGetColumn(dialog);
    EXPECT_TRUE(column->GetChildren().size() == 3);
    int32_t index = 0;
    for (auto child : column->GetChildren()) {
        ++index;
        if (index == 1) {
            RefPtr<FlexItemComponent> titleFlex = AceType::DynamicCast<FlexItemComponent>(child);
            EXPECT_TRUE(titleFlex != nullptr);
            RefPtr<PaddingComponent> titlePadding;
            if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
                RefPtr<RowComponent> brow = AceType::DynamicCast<RowComponent>(titleFlex->GetChild());
                EXPECT_TRUE(brow != nullptr);
                RefPtr<BoxComponent> row = AceType::DynamicCast<BoxComponent>(brow->GetChildren().front());
                EXPECT_TRUE(row != nullptr);
                titlePadding = AceType::DynamicCast<PaddingComponent>(row->GetChild());
            } else {
                RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(titleFlex->GetChild());
                EXPECT_TRUE(row != nullptr);
                titlePadding = AceType::DynamicCast<PaddingComponent>(row->GetChildren().front());
            }
            EXPECT_TRUE(titlePadding != nullptr);
            RefPtr<BoxComponent> box = AceType::DynamicCast<BoxComponent>(titlePadding->GetChild());
            EXPECT_TRUE(box != nullptr);
            RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(box->GetChild());
            EXPECT_TRUE(text != nullptr);
            EXPECT_EQ(text->GetData(), TEXT_VALUE_DEFAULT);
        } else if (index == 2) {
            RefPtr<FlexItemComponent> contentFlex = AceType::DynamicCast<FlexItemComponent>(child);
            EXPECT_TRUE(contentFlex != nullptr);
            RefPtr<PaddingComponent> contentPadding = AceType::DynamicCast<PaddingComponent>(contentFlex->GetChild());
            EXPECT_TRUE(contentPadding != nullptr);
            RefPtr<ScrollComponent> contentScroll = AceType::DynamicCast<ScrollComponent>(contentPadding->GetChild());
            EXPECT_TRUE(contentScroll != nullptr);
            RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(contentScroll->GetChild());
            EXPECT_TRUE(text != nullptr);
            EXPECT_EQ(text->GetData(), TEXT_VALUE);
        } else if (index == 3) {
            RefPtr<FlexItemComponent> actionsFlex = AceType::DynamicCast<FlexItemComponent>(child);
            EXPECT_TRUE(actionsFlex != nullptr);
            RefPtr<PaddingComponent> actionsPadding = AceType::DynamicCast<PaddingComponent>(actionsFlex->GetChild());
            EXPECT_TRUE(actionsPadding != nullptr);
            RefPtr<RowComponent> actionsRow = AceType::DynamicCast<RowComponent>(actionsPadding->GetChild());
            EXPECT_TRUE(actionsRow != nullptr);
            EXPECT_TRUE(actionsRow->GetChildren().size() == 3);
        }
    }
}

/**
 * @tc.name: DialogComponentBuild009
 * @tc.desc: Verify the Build Interface of DialogComponent work correctly with content and actions.
 * @tc.type: FUNC
 */
HWTEST_F(DialogComponentTest, DialogComponentBuild009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent with content and actions.
     */
    RefPtr<DialogComponent> dialog = AceType::MakeRefPtr<DialogComponent>();
    RefPtr<TextComponent> content = AceType::MakeRefPtr<TextComponent>("HiAce");
    dialog->SetContent(content);
    std::list<RefPtr<ButtonComponent>> actions = BuildActions(2);
    dialog->SetActions(actions);

    /**
     * @tc.steps: step2. call the Build interface of DialogComponent.
     * @tc.expected: step2. child of DialogComponent is built correctly.
     */
    RefPtr<ColumnComponent> column = BuildAndGetColumnForWatch(dialog);
    EXPECT_TRUE(column->GetChildren().size() == 2);
    RefPtr<FlexItemComponent> contentFlex = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().front());
    EXPECT_TRUE(contentFlex != nullptr);
    RefPtr<PaddingComponent> contentPadding = AceType::DynamicCast<PaddingComponent>(contentFlex->GetChild());
    EXPECT_TRUE(contentPadding != nullptr);
    RefPtr<TextComponent> contentComponent = AceType::DynamicCast<TextComponent>(contentPadding->GetChild());
    EXPECT_TRUE(contentComponent != nullptr);
    EXPECT_EQ(contentComponent->GetData(), TEXT_VALUE_DEFAULT);
    RefPtr<FlexItemComponent> actionsFlex = AceType::DynamicCast<FlexItemComponent>(column->GetChildren().back());
    EXPECT_TRUE(actionsFlex != nullptr);
    RefPtr<PaddingComponent> actionsPadding = AceType::DynamicCast<PaddingComponent>(actionsFlex->GetChild());
    EXPECT_TRUE(actionsPadding != nullptr);
    RefPtr<RowComponent> rowComponent = AceType::DynamicCast<RowComponent>(actionsPadding->GetChild());
    EXPECT_TRUE(rowComponent != nullptr);
    EXPECT_TRUE(rowComponent->GetChildren().size() == 2);
}

} // namespace OHOS::Ace
