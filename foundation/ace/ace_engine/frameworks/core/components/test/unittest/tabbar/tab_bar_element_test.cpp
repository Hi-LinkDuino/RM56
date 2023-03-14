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

#include "core/components/focusable/focusable_component.h"
#include "core/components/root/root_component.h"
#include "core/components/root/root_element.h"
#include "core/components/tab_bar/tab_bar_element.h"
#include "core/components/tab_bar/tab_bar_item_element.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/tabbar/tab_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr int32_t TAB_ITEM_COUNT = 3;
constexpr int32_t CONTROLLER_ID_ONE = 9526;
constexpr int32_t CONTROLLER_ID_TWO = 9527;

bool CheckFocus(const std::list<RefPtr<Element>>& children, int32_t index, int32_t& count)
{
    count = 0;
    for (auto pos = children.begin(); pos != children.end(); ++pos) {
        auto focus = AceType::DynamicCast<FocusNode>(*pos);
        if (!focus) {
            continue;
        }
        bool onFocus = focus->IsCurrentFocus();
        if (count == index) {
            if (!onFocus) {
                return false;
            }
        } else {
            if (onFocus) {
                return false;
            }
        }
        count++;
    }
    return true;
}

} // namespace

class TabBarElementTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void InitFocusTree();

protected:
    RefPtr<PipelineContext> context_;
    RefPtr<TabBarElement> tabBarOne_ = AceType::MakeRefPtr<TabBarElement>();
    RefPtr<TabBarElement> tabBarTwo_ = AceType::MakeRefPtr<TabBarElement>();
    RefPtr<ContainerElement> rootContainer_ = AceType::MakeRefPtr<ContainerElement>();
};

void TabBarElementTest::SetUpTestCase() {}

void TabBarElementTest::TearDownTestCase() {}

void TabBarElementTest::SetUp()
{
    InitFocusTree();
}

void TabBarElementTest::TearDown() {}

void TabBarElementTest::InitFocusTree()
{
    // create fake renderNode
    rootContainer_->Create();
    context_ = MockRenderCommon::GetMockContext();
    context_->SetupRootElement();
    rootContainer_->SetPipelineContext(context_);

    std::list<RefPtr<Component>> children;
    children.clear();

    // create first TabBar
    for (int32_t i = 0; i < TAB_ITEM_COUNT; i++) {
        auto child = AceType::MakeRefPtr<FocusComponet>();
        children.emplace_back(AceType::MakeRefPtr<TabBarItemComponent>(child));
    }
    auto controllerOne = TabController::GetController(CONTROLLER_ID_ONE);
    controllerOne->SetIndex(0);
    tabBarOne_->SetNewComponent(
        AceType::MakeRefPtr<TabBarComponent>(children, controllerOne));

    // create second TabBar
    children.clear();
    for (int32_t i = 0; i < TAB_ITEM_COUNT; i++) {
        auto child = AceType::MakeRefPtr<FocusComponet>();
        children.emplace_back(AceType::MakeRefPtr<TabBarItemComponent>(child));
    }
    auto controllerTwo = TabController::GetController(CONTROLLER_ID_TWO);
    controllerTwo->SetIndex(0);
    tabBarTwo_->SetNewComponent(AceType::MakeRefPtr<TabBarComponent>(children, controllerTwo));

    // mount on rootContainer_
    tabBarOne_->Mount(rootContainer_);
    tabBarTwo_->Mount(rootContainer_);

    rootContainer_->RequestFocusImmediately();
}

/**
 * @tc.name: TabBarElementTest001
 * @tc.desc: Verify the basic function of request focus and release focus.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5Q
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarElementTest, TabBarElementTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with two tabBars.
     * @tc.expected: step1. RootContainer and tabBarOne acquire the focus.
     */
    int32_t count = 0;
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step2. Animation to the right
     * @tc.expected: step2. RootContainer and tabBarOne acquire the focus. tabBarOne's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step3. Animation to the left
     * @tc.expected: step3. RootContainer and tabBarOne acquire the focus. tabBarOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(LEFT_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step4. Animation to the down
     * @tc.expected: step4. RootContainer and tabBarTwo acquire the focus. tabBarTwo's first child Get the focus.
     * tabBarOne lost the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
    ASSERT_TRUE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step5. Animation to the up
     * @tc.expected: step5. tabOne and tabOne's first child get the focus. tabTwo lost the focus.
     * tabBarOne lost the focus.
     */
    rootContainer_->HandleKeyEvent(UP_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
}

/**
 * @tc.name: TabBarElementTest002
 * @tc.desc: when focus lost and got again, TabBar should remember the latest on focused child.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5Q
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarElementTest, TabBarElementTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with two tabBars.
     * @tc.expected: step1. RootContainer and tabBarOne acquire the focus.
     */
    int32_t count = 0;
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step2. Animation to the right
     * @tc.expected: step2. RootContainer and tabBarOne acquire the focus. tabBarOne's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step3. Animation to the down
     * @tc.expected: step3. RootContainer and tabBarTwo acquire the focus. tabBarTwo's first child Get the focus.
     * tabBarOne lost the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
    ASSERT_TRUE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step4. Animation to the right
     * @tc.expected: step4. RootContainer and tabBarTwo acquire the focus. tabBarTwo's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
    ASSERT_TRUE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), 1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step5. Animation to the up
     * @tc.expected: step5. tabOne and tabOne's second child get the focus. tabTwo lost the focus.
     * tabBarOne lost the focus.
     */
    rootContainer_->HandleKeyEvent(UP_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step5. Animation to the down
     * @tc.expected: step5. tabTwo and tabTwo's second child get the focus. tabOne lost the focus.
     * tabBarOne lost the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
    ASSERT_TRUE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), 1, count)) {
        printf("index[%d] focus is not expected", count);
        ASSERT_TRUE(false);
    }
}

/**
 * @tc.name: TabBarElementTest003
 * @tc.desc: when focus at the edge of left, still left will not lost the focus.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5Q
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarElementTest, TabBarElementTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with two tabBars.
     * @tc.expected: step1. RootContainer and tabBarOne acquire the focus.
     */
    int32_t count = 0;
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step2. Animation to the left.
     * @tc.expected: step2. the focus will not change.
     */
    rootContainer_->HandleKeyEvent(LEFT_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }
}

/**
 * @tc.name: TabBarElementTest004
 * @tc.desc: when focus at the edge of child's right, still right will left the next component the focus.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5Q
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarElementTest, TabBarElementTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with two tabBars.
     * @tc.expected: step1. RootContainer and tabBarOne acquire the focus.
     */
    int32_t count = 0;
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step2. Animation to the right.
     * @tc.expected: step2. RootContainer and tabBarTwo acquire the focus. tabBarTwo's first child Get the focus.
     * tabBarOne lost the focus.
     */
    for (int32_t i = 0; i < TAB_ITEM_COUNT; i++) {
        // go right
        rootContainer_->HandleKeyEvent(RIGHT_KEY);
    }
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_FALSE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }
    ASSERT_TRUE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), 0, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }

    /**
     * @tc.steps: step3. Animation to the left.
     * @tc.expected: step3. RootContainer and tabBarOne acquire the focus.
     */
    rootContainer_->HandleKeyEvent(LEFT_KEY);
    ASSERT_TRUE(rootContainer_->IsCurrentFocus());
    ASSERT_TRUE(tabBarOne_->IsCurrentFocus());
    if (!CheckFocus(tabBarOne_->GetChildren(), 2, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }
    ASSERT_FALSE(tabBarTwo_->IsCurrentFocus());
    if (!CheckFocus(tabBarTwo_->GetChildren(), -1, count)) {
        printf("index[%d] focus is not expected\n", count);
        ASSERT_TRUE(false);
    }
}
} // namespace OHOS::Ace
