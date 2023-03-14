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

#include "core/components/flex/flex_component.h"
#include "core/components/focusable/focusable_component.h"
#include "core/components/tab_bar/tab_bar_element.h"
#include "core/components/tab_bar/tab_content_component.h"
#include "core/components/tab_bar/tab_content_element.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/tabbar/tab_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr int32_t ITEM_COUNT = 3;
constexpr int32_t CONTROLLER_ID = 9528;

bool CheckContent(const RefPtr<Element> child, int32_t focusIndex)
{
    if (!child) {
        printf("child == nullptr");
        return false;
    }

    const auto& items = child->GetChildren();
    int32_t count = 0;
    for (auto pos = items.begin(); pos != items.begin(); ++pos) {
        auto focus = AceType::DynamicCast<FocusNode>(*pos);
        if (!focus) {
            continue;
        }
        bool focused = focus->IsCurrentFocus();
        if (count == focusIndex) {
            if (!focused) {
                printf("index[%d] focus is not expected\n", count);
                return false;
            }
        } else {
            if (focused) {
                printf("index[%d] focus is not expected\n", count);
                return false;
            }
        }
        count++;
    }
    return true;
}

} // namespace

class TabContentElementTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDowntestCase();
    void SetUp() override;
    void TearDown() override;
    void InitFocusTree();

protected:
    RefPtr<PipelineContext> context_;
    RefPtr<TabController> controller_ = TabController::GetController(CONTROLLER_ID);
    RefPtr<ContainerElement> rootContainer_ = AceType::MakeRefPtr<ContainerElement>();
    RefPtr<TabBarElement> tabBar_ = AceType::MakeRefPtr<TabBarElement>();
    RefPtr<TabContentElement> tabContent_;

private:
    RefPtr<FlexComponent> CreateContent();
};

void TabContentElementTest::SetUpTestCase() {}

void TabContentElementTest::TearDowntestCase() {}

void TabContentElementTest::SetUp()
{
    InitFocusTree();
}

void TabContentElementTest::TearDown() {}

void TabContentElementTest::InitFocusTree()
{
    // create fake renderNode
    rootContainer_->Create();
    context_ = MockRenderCommon::GetMockContext();
    context_->SetupRootElement();
    rootContainer_->SetPipelineContext(context_);

    // create TabBar
    std::list<RefPtr<Component>> children;
    children.clear();
    for (int32_t i = 0; i < ITEM_COUNT; i++) {
        children.emplace_back(AceType::MakeRefPtr<TabBarItemComponent>(AceType::MakeRefPtr<FocusComponet>()));
    }
    tabBar_->SetNewComponent(AceType::MakeRefPtr<TabBarComponent>(children, controller_));

    // create TabContent
    children.clear();
    // first child is focusable
    children.push_back(CreateContent());
    // second child is focusable
    children.push_back(CreateContent());
    // third child is not focusable
    RefPtr<FocusableComponent> focusableComponent =
        AceType::MakeRefPtr<FocusableComponent>(AceType::MakeRefPtr<BoxComponent>());
    focusableComponent->SetFocusable(false);
    focusableComponent->SetFocusNode(true);
    children.push_back(focusableComponent);
    tabContent_ = AceType::MakeRefPtr<TabContentElement>(children);
    tabContent_->SetNewComponent(AceType::MakeRefPtr<TabContentComponent>(children, controller_));

    // add to rootContainer_
    tabBar_->Mount(rootContainer_);
    tabContent_->Mount(rootContainer_);

    rootContainer_->RequestFocusImmediately();
}

RefPtr<FlexComponent> TabContentElementTest::CreateContent()
{
    std::list<RefPtr<Component>> children;
    for (int32_t i = 0; i < ITEM_COUNT; i++) {
        children.emplace_back(AceType::MakeRefPtr<FocusComponet>());
    }

    return AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, children);
}

/**
 * @tc.name: TabContentElementTest001
 * @tc.desc: Verify the basic function of request focus and release focus.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5Q
 * @tc.author: jiangtao
 */
HWTEST_F(TabContentElementTest, TabContentElementTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with tabBar and tabContent.
     * @tc.expected: step1. RootContainer and tabBar acquire the focus.
     */
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());

    /**
     * @tc.steps: step2. Animation to the down ITEM_COUNT + 1 times.
     * @tc.expected: step2. tabContent acquire the focus. tabContent's latest child get the focus.
     */
    for (int32_t i = 0; i < ITEM_COUNT + 1; i++) {
        rootContainer_->HandleKeyEvent(DOWN_KEY);
    }
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBar_->IsCurrentFocus());
    ASSERT_TRUE(tabContent_->IsCurrentFocus());
    if (!CheckContent(tabContent_->GetTabContentChild(controller_->GetIndex()), 2)) {
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step2. Animation to the up.
     * @tc.expected: step2. tabContent acquire the focus. tabContent's second child get the focus.
     */
    rootContainer_->HandleKeyEvent(UP_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBar_->IsCurrentFocus());
    ASSERT_TRUE(tabContent_->IsCurrentFocus());
    if (!CheckContent(tabContent_->GetTabContentChild(controller_->GetIndex()), 1)) {
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step3. Animation to the right.
     * @tc.expected: step3. focus will not change.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBar_->IsCurrentFocus());
    ASSERT_TRUE(tabContent_->IsCurrentFocus());
    if (!CheckContent(tabContent_->GetTabContentChild(controller_->GetIndex()), 1)) {
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step4. Animation to the left.
     * @tc.expected: step4. focus will not change.
     */
    rootContainer_->HandleKeyEvent(LEFT_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
}

/**
 * @tc.name: TabContentElementTest002
 * @tc.desc: When tabContent got the focus, change to another focusable child.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5Q
 * @tc.author: jiangtao
 */
HWTEST_F(TabContentElementTest, TabContentElementTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with tabBar and tabContent.
     * @tc.expected: step1. RootContainer and tabBar acquire the focus.
     */
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());

    /**
     * @tc.steps: step2. Animation to the down ITEM_COUNT + 1 times.
     * @tc.expected: step2. tabContent acquire the focus. latest child get the focus.
     */
    for (int32_t i = 0; i < ITEM_COUNT + 1; i++) {
        rootContainer_->HandleKeyEvent(DOWN_KEY);
    }
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBar_->IsCurrentFocus());
    ASSERT_TRUE(tabContent_->IsCurrentFocus());
    if (!CheckContent(tabContent_->GetTabContentChild(controller_->GetIndex()), 2)) {
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step3. Change to another focusable child
     * @tc.expected: step3. tabContent acquire the focus. first child get the focus.
     */
    tabContent_->ChangeByContent(1);
    tabContent_->PrepareContent(1);
    tabContent_->PerformBuild();
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());
    if (!CheckContent(tabContent_->GetTabContentChild(controller_->GetIndex()), 0)) {
        ASSERT_TRUE(false);
    }
}

/**
 * @tc.name: TabContentElementTest003
 * @tc.desc: When tabContent got the focus, change to another can't not focusable child.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5Q
 * @tc.author: jiangtao
 */
HWTEST_F(TabContentElementTest, TabContentElementTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with tabBar and tabContent.
     * @tc.expected: step1. RootContainer and tabBar acquire the focus.
     */
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());

    /**
     * @tc.steps: step2. Animation to the down ITEM_COUNT + 1 times.
     * @tc.expected: step2. tabContent acquire the focus. latest child get the focus.
     */
    for (int32_t i = 0; i < ITEM_COUNT + 1; i++) {
        rootContainer_->HandleKeyEvent(DOWN_KEY);
    }
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBar_->IsCurrentFocus());
    ASSERT_TRUE(tabContent_->IsCurrentFocus());
    if (!CheckContent(tabContent_->GetTabContentChild(controller_->GetIndex()), 2)) {
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step3. Change to can't not focusable child.
     * @tc.expected: step3. all element lost focus, and tabContent is not focusable.
     */
    tabContent_->PrepareContent(2);
    tabContent_->PerformBuild();
    tabContent_->ChangeByContent(2);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsFocusable());

    /**
     * @tc.steps: step4. Animation to the up.
     * @tc.expected: step4. RootContainer and tabBar acquire the focus.
     */
    rootContainer_->HandleKeyEvent(UP_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());
}

/**
 * @tc.name: TabContentElementTest004
 * @tc.desc: When tabContent lost the focus, change to another focusable child.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5Q
 * @tc.author: jiangtao
 */
HWTEST_F(TabContentElementTest, TabContentElementTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with tabBar and tabContent.
     * @tc.expected: step1. RootContainer and tabBar acquire the focus.
     */
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());

    /**
     * @tc.steps: step2. Change to another focusable child
     * @tc.expected: step2. focus not change, tabContent can be focusable.
     */
    tabContent_->ChangeByContent(1);
    tabContent_->PrepareContent(1);
    tabContent_->PerformBuild();
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());
    ASSERT_TRUE(tabContent_->IsFocusable());

    /**
     * @tc.steps: step3. Animation to the down.
     * @tc.expected: step3. tabContent acquire the focus. first child get the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBar_->IsCurrentFocus());
    ASSERT_TRUE(tabContent_->IsCurrentFocus());
    if (!CheckContent(tabContent_->GetTabContentChild(controller_->GetIndex()), 0)) {
        ASSERT_TRUE(false);
    }
}

/**
 * @tc.name: TabContentElementTest005
 * @tc.desc: When tabContent lost the focus, change to another can't not focusable child.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5Q
 * @tc.author: jiangtao
 */
HWTEST_F(TabContentElementTest, TabContentElementTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with tabBar and tabContent.
     * @tc.expected: step1. RootContainer and tabBar acquire the focus.
     */
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());

    /**
     * @tc.steps: step2. Change to another focusable child
     * @tc.expected: step2. focus not change, tabContent can not be focusable.
     */
    tabContent_->ChangeByContent(2);
    tabContent_->PrepareContent(2);
    tabContent_->PerformBuild();
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsFocusable());

    /**
     * @tc.steps: step3. Animation to the down.
     * @tc.expected: step3. focus will not change.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBar_->IsCurrentFocus());
    ASSERT_FALSE(tabContent_->IsCurrentFocus());
}

} // namespace OHOS::Ace