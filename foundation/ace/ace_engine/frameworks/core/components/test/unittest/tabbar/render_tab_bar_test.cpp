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

#include "core/components/root/render_root.h"
#include "core/components/tab_bar/render_tab_bar.h"
#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/tab_bar/tab_controller.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/tabbar/tabbar_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr int32_t CONTROLLER_ID = 12345;
constexpr int32_t SMALLTABSIZE = 3;
constexpr int32_t TABSIZE = 5;
constexpr double RECT_WIDTH = 1080.0;
const Size ITEM_SIZE = Size(250.0, 250.0);

void AddItem(const RefPtr<MockRenderTabBar>& tabBar, const RefPtr<PipelineContext>& context, int32_t size)
{
    auto flex = BoxFlex::FLEX_XY;
    if (tabBar->GetMode() == TabBarMode::SCROLLABEL) {
        flex = BoxFlex::FLEX_Y;
    }

    for (int32_t i = 0; i < size; i++) {
        auto renderBox = TabBarTestUtils::CreateRenderBox(ITEM_SIZE.Width(), ITEM_SIZE.Height(), flex);
        renderBox->Attach(context);
        tabBar->AddChild(renderBox);
    }
}

Offset GetItemOffset(TabBarMode mod, int index, double actualWidth)
{
    Offset offset;
    double unitWidth = ITEM_SIZE.Width();
    if (mod == TabBarMode::FIXED) {
        unitWidth = RECT_WIDTH / TABSIZE;
    } else if (actualWidth < RECT_WIDTH) {
        offset.SetX((RECT_WIDTH - actualWidth) / 2.0);
    }
    return Offset(index * unitWidth, 0.0) + offset;
}

void CheckItemPosition(const RefPtr<MockRenderTabBar>& tabBar, int size)
{
    auto mode = tabBar->GetMode();
    auto actualWidth = tabBar->GetActualWidth();
    const auto& children = tabBar->GetChildren();
    auto pos = children.begin();
    ++pos;
    for (int index = 0; index < size; ++index) {
        auto offset = (*pos)->GetPosition();
        ASSERT_EQ(offset, GetItemOffset(mode, index, actualWidth));
        ++pos;
    }
}

} // namespace

class RenderTabBarTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RenderTabBarTest::SetUpTestCase() {}
void RenderTabBarTest::TearDownTestCase() {}
void RenderTabBarTest::SetUp() {}
void RenderTabBarTest::TearDown() {}

/**
 * @tc.name: RenderTabBarTest001
 * @tc.desc: Verify TabBar PerformLayout when mod fix.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(RenderTabBarTest, RenderTabBarTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TabBarComponent with mod fixed.
     * @tc.expected: step1. properties set correctly.
     */
    TabBarMode mode = TabBarMode::FIXED;
    RefPtr<TabBarComponent> tabBarComponent = AceType::MakeRefPtr<TabBarComponent>(
        std::list<RefPtr<Component>>(), TabController::GetController(CONTROLLER_ID));
    tabBarComponent->SetMode(mode);

    /**
     * @tc.steps: step2. construct RenderTabBar with 5 child.
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = TabBarTestUtils::CreateRenderRoot();
    RefPtr<MockRenderTabBar> renderTabBar = AceType::MakeRefPtr<MockRenderTabBar>();
    renderRoot->Attach(mockContext);
    renderTabBar->Attach(mockContext);
    renderRoot->AddChild(renderTabBar);
    renderTabBar->Update(tabBarComponent);
    AddItem(renderTabBar, mockContext, TABSIZE);
    ASSERT_TRUE(renderTabBar->GetChildren().size() == TABSIZE + 1);
    ASSERT_TRUE(renderTabBar->NeedLayout());

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    CheckItemPosition(renderTabBar, TABSIZE);
}

/**
 * @tc.name: RenderTabBarTest002
 * @tc.desc: Verify TabBar PerformLayout when mod scrollable and children's width greater than TabBar.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(RenderTabBarTest, RenderTabBarTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TabBarComponent with mod scrollable.
     * @tc.expected: step1. properties set correctly.
     */
    TabBarMode mode = TabBarMode::SCROLLABEL;
    RefPtr<TabBarComponent> tabBarComponent = AceType::MakeRefPtr<TabBarComponent>(
        std::list<RefPtr<Component>>(), TabController::GetController(CONTROLLER_ID));
    tabBarComponent->SetMode(mode);

    /**
     * @tc.steps: step2. construct RenderTabBar with 5 child.
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = TabBarTestUtils::CreateRenderRoot();
    RefPtr<MockRenderTabBar> renderTabBar = AceType::MakeRefPtr<MockRenderTabBar>();
    renderRoot->Attach(mockContext);
    renderTabBar->Attach(mockContext);
    renderRoot->AddChild(renderTabBar);
    renderTabBar->Update(tabBarComponent);
    AddItem(renderTabBar, mockContext, TABSIZE);
    ASSERT_TRUE(renderTabBar->GetChildren().size() == TABSIZE + 1);
    ASSERT_TRUE(renderTabBar->NeedLayout());

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    CheckItemPosition(renderTabBar, TABSIZE);
}

/**
 * @tc.name: RenderTabBarTest003
 * @tc.desc: Verify TabBar PerformLayout when mod scrollable and children's width less than TabBar.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(RenderTabBarTest, RenderTabBarTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TabBarComponent with mod scrollable.
     * @tc.expected: step1. properties set correctly.
     */
    TabBarMode mode = TabBarMode::SCROLLABEL;
    RefPtr<TabBarComponent> tabBarComponent = AceType::MakeRefPtr<TabBarComponent>(
        std::list<RefPtr<Component>>(), TabController::GetController(CONTROLLER_ID));
    tabBarComponent->SetMode(mode);

    /**
     * @tc.steps: step2. construct RenderTabBar with 3 child.
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = TabBarTestUtils::CreateRenderRoot();
    RefPtr<MockRenderTabBar> renderTabBar = AceType::MakeRefPtr<MockRenderTabBar>();
    renderRoot->Attach(mockContext);
    renderTabBar->Attach(mockContext);
    renderRoot->AddChild(renderTabBar);
    renderTabBar->Update(tabBarComponent);
    AddItem(renderTabBar, mockContext, SMALLTABSIZE);
    ASSERT_TRUE(renderTabBar->GetChildren().size() == SMALLTABSIZE + 1);
    ASSERT_TRUE(renderTabBar->NeedLayout());

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    CheckItemPosition(renderTabBar, SMALLTABSIZE);
}

/**
 * @tc.name: RenderTabBarTest004
 * @tc.desc: Verify TabBar SetIndex with invalid value.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(RenderTabBarTest, RenderTabBarTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TabBarComponent with mod scrollable and set index less than zero.
     * @tc.expected: step1. properties set correctly.
     */
    TabBarMode mode = TabBarMode::SCROLLABEL;
    RefPtr<TabBarComponent> tabBarComponent = AceType::MakeRefPtr<TabBarComponent>(
        std::list<RefPtr<Component>>(), TabController::GetController(CONTROLLER_ID));
    tabBarComponent->SetMode(mode);
    for (int i = 0; i < SMALLTABSIZE; i++) {
        tabBarComponent->AppendChild(AceType::MakeRefPtr<BoxComponent>());
    }
    tabBarComponent->SetIndex(2);

    /**
     * @tc.steps: step2. construct RenderTabBar with 3 child.
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = TabBarTestUtils::CreateRenderRoot();
    RefPtr<MockRenderTabBar> renderTabBar = AceType::MakeRefPtr<MockRenderTabBar>();
    renderRoot->Attach(mockContext);
    renderTabBar->Attach(mockContext);
    renderRoot->AddChild(renderTabBar);
    renderTabBar->Update(tabBarComponent);
    AddItem(renderTabBar, mockContext, SMALLTABSIZE);
    ASSERT_TRUE(renderTabBar->GetChildren().size() == SMALLTABSIZE + 1);
    ASSERT_TRUE(renderTabBar->NeedLayout());
    ASSERT_EQ(renderTabBar->GetIndex(), 2);
    ASSERT_EQ(renderTabBar->GetTabsSize(), SMALLTABSIZE);

    renderTabBar->SetIndex(-1);
    ASSERT_EQ(renderTabBar->GetIndex(), 2);
    renderTabBar->SetIndex(1);
    ASSERT_EQ(renderTabBar->GetIndex(), 1);
    renderTabBar->SetIndex(SMALLTABSIZE);
    ASSERT_EQ(renderTabBar->GetIndex(), 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    renderTabBar->SetIndex(-1);
    ASSERT_EQ(renderTabBar->GetIndex(), 1);
    renderTabBar->SetIndex(1);
    ASSERT_EQ(renderTabBar->GetIndex(), 1);
    renderTabBar->SetIndex(SMALLTABSIZE);
    ASSERT_EQ(renderTabBar->GetIndex(), 1);
}

} // namespace OHOS::Ace
