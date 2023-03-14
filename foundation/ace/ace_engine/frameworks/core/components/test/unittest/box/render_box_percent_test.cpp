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

#include <array>
#include <list>

#include "gtest/gtest.h"

#define private public
#define protected public
#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/bubble/render_bubble.h"
#include "core/components/flex/render_flex.h"
#include "core/components/flex/render_flex_item.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/render_grid_layout.h"
#include "core/components/grid_layout/render_grid_layout_item.h"
#include "core/components/list/list_component.h"
#include "core/components/list/render_list.h"
#include "core/components/scroll/render_single_child_scroll.h"
#include "core/components/stack/render_stack.h"
#include "core/components/swiper/flutter_render_swiper.h"
#include "core/components/swiper/render_swiper.h"
#include "core/components/swiper/swiper_component.h"
#include "core/components/tab_bar/render_tab_bar.h"
#include "core/components/tab_bar/render_tab_content.h"
#include "core/components/test/unittest/box/box_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/wrap/wrap_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Dimension PERCENT_HALF = Dimension(0.5, DimensionUnit::PERCENT);
const Dimension PERCENT_NEGATIVE = Dimension(-0.5, DimensionUnit::PERCENT);
const Dimension PERCENT_OVER_LIMIT = Dimension(1.2, DimensionUnit::PERCENT);
const Dimension PARENT_SIZE = Dimension(100.0, DimensionUnit::PX);
const Dimension HALF_SIZE = Dimension(50.0, DimensionUnit::PX);
const size_t WRAP_CHILD_SIZE = 4;
const size_t FLEX_CHILD_SIZE = 2;
const size_t SWIPER_CHILD_SIZE = 2;
const Dimension BUBBLE_MARGIN = Dimension(48.0, DimensionUnit::PX);
const Dimension INDICATOR_POINT_PADDING(8.0, DimensionUnit::VP);

void MockPerformLayout(
    const RefPtr<PipelineContext>& context, const RefPtr<RenderNode>& parendNode, bool infinitRoot = false)
{
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    if (infinitRoot) {
        renderRoot->SetPaintRect(Rect(0.0, 0.0, Size::INFINITE_SIZE, Size::INFINITE_SIZE));
    }
    renderRoot->AddChild(parendNode);
    renderRoot->PerformLayout();
}

RefPtr<RenderBox> ConstructRenderBox(
    const RefPtr<PipelineContext>& context, const Dimension& width, const Dimension& height)
{
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width.Value(), width.Unit());
    boxComponent->SetHeight(height.Value(), height.Unit());
    boxComponent->SetPercentFlag(0);
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(context);
    renderBox->Update(boxComponent);
    return renderBox;
}

void TestBoxInWrap(const RefPtr<PipelineContext>& context, const WrapDirection& direction)
{
    /**
     * @tc.steps: step1. Construct a wrap and 4 child box with percent value(2 box in a line).
     */
    RefPtr<WrapComponent> wrap = AceType::MakeRefPtr<WrapComponent>(std::list<RefPtr<Component>>());
    wrap->SetDirection(direction);
    RefPtr<RenderWrap> renderWrap = AceType::MakeRefPtr<RenderWrap>();
    renderWrap->Attach(context);
    renderWrap->Update(wrap);
    std::array<RefPtr<RenderBox>, WRAP_CHILD_SIZE> childRenderBox;
    for (size_t i = 0; i < childRenderBox.size(); ++i) {
        childRenderBox[i] = ConstructRenderBox(context, PERCENT_HALF, PERCENT_HALF);
    }

    /**
     * @tc.steps: step2. Add child box to wrap, add wrap to box of 100x100, perform layout.
     * @tc.expected: step2. All child box's size is 50x50.
     */
    auto parentRenderBox = ConstructRenderBox(context, PARENT_SIZE, PARENT_SIZE);
    parentRenderBox->AddChild(renderWrap);
    for (size_t i = 0; i < childRenderBox.size(); ++i) {
        renderWrap->AddChild(childRenderBox[i]);
    }

    MockPerformLayout(context, AceType::DynamicCast<RenderNode>(parentRenderBox));

    for (size_t i = 0; i < childRenderBox.size(); ++i) {
        ASSERT_TRUE(childRenderBox[i]->GetLayoutSize() == Size(HALF_SIZE.Value(), HALF_SIZE.Value()));
    }
}

void TestBoxInFlexItem(const RefPtr<PipelineContext>& context, double flexGrow, double flexShrink, double flexBasis)
{
    /**
     * @tc.steps: step1. Construct a renderFlex and children.
     */
    RefPtr<RenderFlex> renderFlex = AceType::MakeRefPtr<RenderFlex>();
    renderFlex->Attach(context);
    std::array<RefPtr<RenderFlexItem>, FLEX_CHILD_SIZE> renderChildren;
    for (size_t i = 0; i < renderChildren.size(); ++i) {
        RefPtr<RenderFlexItem> renderFlexItem = AceType::MakeRefPtr<RenderFlexItem>();
        renderFlexItem->SetFlexGrow(flexGrow);
        renderFlexItem->SetFlexShrink(flexShrink);
        renderFlexItem->SetFlexBasis(flexBasis);
        renderFlexItem->Attach(context);
        auto renderBox = ConstructRenderBox(context, PERCENT_HALF, PERCENT_HALF);
        renderFlexItem->AddChild(renderBox);
        renderChildren[i] = renderFlexItem;
    }

    /**
     * @tc.steps: step2. Add children to renderFlex, add renderFlex to box of 100x100, perform layout.
     * @tc.expected: step2. All child box's size is 50x50.
     */
    auto parentRenderBox = ConstructRenderBox(context, PARENT_SIZE, PARENT_SIZE);
    parentRenderBox->AddChild(renderFlex);
    for (size_t i = 0; i < renderChildren.size(); ++i) {
        renderFlex->AddChild(renderChildren[i]);
    }
    MockPerformLayout(context, AceType::DynamicCast<RenderNode>(parentRenderBox));
    for (size_t i = 0; i < renderChildren.size(); ++i) {
        ASSERT_TRUE(renderChildren[i]->GetLayoutSize() == Size(HALF_SIZE.Value(), HALF_SIZE.Value()));
    }
}

RefPtr<FlutterRenderSwiper> TestSwiperIndicator(
    const RefPtr<PipelineContext>& context, const RefPtr<SwiperIndicator>& indicator)
{
    auto parentRenderBox = ConstructRenderBox(context, PARENT_SIZE, PARENT_SIZE);
    auto renderSwiper = AceType::DynamicCast<FlutterRenderSwiper>(RenderSwiper::Create());
    renderSwiper->Attach(context);
    std::list<RefPtr<Component>> swiperChildren;
    for (size_t i = 0; i < SWIPER_CHILD_SIZE; ++i) {
        auto box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(PARENT_SIZE.Value(), PARENT_SIZE.Unit());
        box->SetHeight(PARENT_SIZE.Value(), PARENT_SIZE.Unit());
        swiperChildren.push_back(box);
        auto renderBox = RenderBox::Create();
        renderBox->Update(box);
        renderSwiper->AddChild(renderBox);
    }
    RefPtr<SwiperComponent> swiper = AceType::MakeRefPtr<SwiperComponent>(swiperChildren);
    swiper->SetIndicator(indicator);
    renderSwiper->Update(swiper);
    parentRenderBox->AddChild(renderSwiper);
    MockPerformLayout(context, AceType::DynamicCast<RenderNode>(parentRenderBox));
    return renderSwiper;
}

} // namespace

class RenderBoxPercentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    RefPtr<PipelineContext> context_;
};

void RenderBoxPercentTest::SetUpTestCase() {}
void RenderBoxPercentTest::TearDownTestCase() {}

void RenderBoxPercentTest::SetUp()
{
    context_ = MockRenderCommon::GetMockContext();
}

void RenderBoxPercentTest::TearDown()
{
    context_ = nullptr;
}

/**
 * @tc.name: RenderBoxHandlePercentTest001
 * @tc.desc: RenderBox could handle width and height in percent.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxHandlePercentTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box and mount child box, perform layout.
     * @tc.expected: step1. Child render box's size calculate correctly.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    parentRenderBox->AddChild(childBox);
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));

    ASSERT_TRUE(childBox->GetLayoutSize() == Size(HALF_SIZE.Value(), HALF_SIZE.Value()));
}

/**
 * @tc.name: RenderBoxHandlePercentTest002
 * @tc.desc: RenderBox could handle width and height in negative percent.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxHandlePercentTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step2. Construct parent box and mount child box, perform layout.
     * @tc.expected: step2. Child render box's size is zero.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto childBox = ConstructRenderBox(context_, PERCENT_NEGATIVE, PERCENT_NEGATIVE);
    parentRenderBox->AddChild(childBox);
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));

    ASSERT_TRUE(childBox->GetLayoutSize() == Size(0.0, 0.0));
}

/**
 * @tc.name: RenderBoxHandlePercentTest003
 * @tc.desc: RenderBox could handle width and height in percent over 100%.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxHandlePercentTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step2. Construct parent box and mount child box, perform layout.
     * @tc.expected: step2. Child render box's size is same with parent.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto childBox = ConstructRenderBox(context_, PERCENT_OVER_LIMIT, PERCENT_OVER_LIMIT);
    parentRenderBox->AddChild(childBox);
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));

    ASSERT_TRUE(childBox->GetLayoutSize() == Size(PARENT_SIZE.Value(), PARENT_SIZE.Value()));
}

/**
 * @tc.name: RenderBoxHandlePercentTest004
 * @tc.desc: RenderBox could handle width and height in percent, parent size is INFINITE.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxHandlePercentTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step2. Construct parent box and mount child box, perform layout.
     * @tc.expected: step2. Child render box's size is zero.
     */
    auto parentRenderBox = ConstructRenderBox(context_, Dimension(Size::INFINITE_SIZE), Dimension(Size::INFINITE_SIZE));
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    parentRenderBox->AddChild(childBox);
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox), true);

    ASSERT_TRUE(childBox->GetLayoutSize() == Size(0.0, 0.0));
}

/**
 * @tc.name: RenderBoxPercentBoxInWrap001
 * @tc.desc: RenderBox could handle width and height in percent, parent is wrap, horizontal direction.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInWrap001, TestSize.Level1)
{
    TestBoxInWrap(context_, WrapDirection::HORIZONTAL);
}

/**
 * @tc.name: RenderBoxPercentBoxInWrap002
 * @tc.desc: RenderBox could handle width and height in percent, parent is wrap, vertical direction.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInWrap002, TestSize.Level1)
{
    TestBoxInWrap(context_, WrapDirection::VERTICAL);
}

/**
 * @tc.name: RenderBoxPercentBoxInFLexItem001
 * @tc.desc: RenderBox could handle width and height in percent, parent is flexItem(flexGrow=0, flexShrink=0,
 *           flexBasis=0).
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInFLexItem001, TestSize.Level1)
{
    TestBoxInFlexItem(context_, 0.0, 0.0, 0.0);
}

/**
 * @tc.name: RenderBoxPercentBoxInFLexItem002
 * @tc.desc: RenderBox could handle width and height in percent, parent is flexItem(flexGrow=1, flexShrink=0,
 *           flexBasis=0).
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInFLexItem002, TestSize.Level1)
{
    TestBoxInFlexItem(context_, 1.0, 0.0, 0.0);
}

/**
 * @tc.name: RenderBoxPercentBoxInFLexItem003
 * @tc.desc: RenderBox could handle width and height in percent, parent is flexItem(flexGrow=0, flexShrink=1,
 *           flexBasis=0).
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInFLexItem003, TestSize.Level1)
{
    TestBoxInFlexItem(context_, 0.0, 1.0, 0.0);
}

/**
 * @tc.name: RenderBoxPercentBoxInFLexItem004
 * @tc.desc: RenderBox could handle width and height in percent, parent is flexItem(flexGrow=1, flexShrink=0,
 *           flexBasis=10).
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInFLexItem004, TestSize.Level1)
{
    TestBoxInFlexItem(context_, 1.0, 0.0, 10.0);
}

/**
 * @tc.name: RenderBoxPercentBoxInFLexItem005
 * @tc.desc: RenderBox could handle width and height in percent, parent is flexItem(flexGrow=0, flexShrink=1,
 *           flexBasis=100).
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInFLexItem005, TestSize.Level1)
{
    TestBoxInFlexItem(context_, 0.0, 1.0, 100.0);
}

/**
 * @tc.name: RenderBoxPercentBoxInGridLayout001
 * @tc.desc: RenderBox could handle width and height in percent, parent is GridLayout.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInGridLayout001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box with 100x100, and grid layout, grid item, child box with 50%x50%.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto renderGrid = AceType::MakeRefPtr<RenderGridLayout>();
    auto gridComponent = AceType::MakeRefPtr<GridLayoutComponent>(std::list<RefPtr<Component>>());
    renderGrid->Attach(context_);
    renderGrid->Update(gridComponent);
    parentRenderBox->AddChild(renderGrid);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    auto renderGridItem = AceType::MakeRefPtr<RenderGridLayoutItem>();
    renderGridItem->Attach(context_);
    renderGridItem->AddChild(childBox);
    renderGrid->AddChild(renderGridItem);

    /**
     * @tc.steps: step2. Perform layout.
     * @tc.expected: step2. Child box's size is 50x50.
     */
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));
    ASSERT_TRUE(childBox->GetLayoutSize() == Size(HALF_SIZE.Value(), HALF_SIZE.Value()));
}

/**
 * @tc.name: RenderBoxPercentBoxInScroll001
 * @tc.desc: RenderBox could handle width and height in percent, parent is scroll.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInScroll001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box with 100x100, and scroll layout, child box with 50%x50%.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto renderScroll = RenderSingleChildScroll::Create();
    renderScroll->Attach(context_);
    parentRenderBox->AddChild(renderScroll);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    renderScroll->AddChild(childBox);

    /**
     * @tc.steps: step2. Perform layout.
     * @tc.expected: step2. Child box's size is 50x0.
     */
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));
    ASSERT_TRUE(childBox->GetLayoutSize() == Size(0.0, HALF_SIZE.Value()));
}

/**
 * @tc.name: RenderBoxPercentBoxInList001
 * @tc.desc: RenderBox could handle width and height in percent, parent is list.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInList001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box with 100x100, and list, child box with 50%x50%.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto renderList = AceType::MakeRefPtr<RenderList>();
    renderList->Attach(context_);
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    renderList->Update(listComponent);
    parentRenderBox->AddChild(renderList);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    auto renderListItem = RenderListItem::Create();
    renderListItem->Attach(context_);
    renderListItem->AddChild(childBox);
    renderList->AddChild(renderListItem);
    renderList->AddListItem(0, renderListItem);

    /**
     * @tc.steps: step2. Perform layout.
     * @tc.expected: step2. Child box's size is 50x0.
     */
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));
    ASSERT_TRUE(childBox->GetLayoutSize() == Size(HALF_SIZE.Value(), 0.0));
}

/**
 * @tc.name: RenderBoxPercentBoxInListItem001
 * @tc.desc: RenderBox could handle width and height in percent, parent is listItem.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInListItem001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box with 100x100, and listItem, child box with 50%x50%.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto renderListItem = RenderListItem::Create();
    parentRenderBox->AddChild(renderListItem);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    renderListItem->Attach(context_);
    renderListItem->AddChild(childBox);

    /**
     * @tc.steps: step2. Perform layout.
     * @tc.expected: step2. Child box's size is 50x50.
     */
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));
    ASSERT_TRUE(childBox->GetLayoutSize() == Size(HALF_SIZE.Value(), HALF_SIZE.Value()));
}

/**
 * @tc.name: RenderBoxPercentBoxInStack001
 * @tc.desc: RenderBox could handle width and height in percent, parent is stack.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInStack001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box with 100x100, and stack, child box with 50%x50%.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto renderStack = RenderStack::Create();
    parentRenderBox->AddChild(renderStack);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    renderStack->Attach(context_);
    renderStack->AddChild(childBox);

    /**
     * @tc.steps: step2. Perform layout.
     * @tc.expected: step2. Child box's size is 50x50.
     */
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));
    ASSERT_TRUE(childBox->GetLayoutSize() == Size(HALF_SIZE.Value(), HALF_SIZE.Value()));
}

/**
 * @tc.name: RenderBoxPercentBoxInSwiper001
 * @tc.desc: RenderBox could handle width and height in percent, parent is swiper.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInSwiper001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box with 100x100, and swiper, child box with 50%x50%.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto renderSwiper = RenderSwiper::Create();
    parentRenderBox->AddChild(renderSwiper);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    renderSwiper->Attach(context_);
    renderSwiper->AddChild(childBox);

    /**
     * @tc.steps: step2. Perform layout.
     * @tc.expected: step2. Child box's size is 50x50.
     */
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));
    GTEST_LOG_(INFO) << "RenderBoxPercentBoxInSwiper001 " << childBox->GetLayoutSize().ToString();
}

#ifndef WEARABLE_PRODUCT
/**
 * @tc.name: RenderBoxPercentBoxInTabBar001
 * @tc.desc: RenderBox could handle width and height in percent, parent is tabBar.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInTabBar001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box with 100x100, and tabBar, child box with 50%x50%.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto renderTabBar = RenderTabBar::Create();
    const auto& controller = TabController::GetController(0);
    auto tabBarIndicator = AceType::MakeRefPtr<TabBarIndicatorComponent>();
    auto tabBarComponent =
        AceType::MakeRefPtr<TabBarComponent>(std::list<RefPtr<Component>>(), controller, tabBarIndicator);
    renderTabBar->Update(tabBarComponent);
    parentRenderBox->AddChild(renderTabBar);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    renderTabBar->Attach(context_);
    renderTabBar->AddChild(childBox);

    /**
     * @tc.steps: step2. Perform layout.
     * @tc.expected: step2. Child box's size is 50x50.
     */
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));
    ASSERT_TRUE(childBox->GetLayoutSize() == Size(HALF_SIZE.Value(), HALF_SIZE.Value()));
}

/**
 * @tc.name: RenderBoxPercentBoxInTabContent001
 * @tc.desc: RenderBox could handle width and height in percent, parent is tab content.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInTabContent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box with 100x100, and tab content, child box with 50%x50%.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto renderTabContent = AceType::DynamicCast<RenderTabContent>(RenderTabContent::Create());
    const auto& controller = TabController::GetController(0);
    auto tabContentComponent = AceType::MakeRefPtr<TabContentComponent>(std::list<RefPtr<Component>>(), controller);
    renderTabContent->Update(tabContentComponent);
    parentRenderBox->AddChild(renderTabContent);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    renderTabContent->Attach(context_);
    renderTabContent->AddChild(childBox);
    renderTabContent->AddChildContent(0, childBox);

    /**
     * @tc.steps: step2. Perform layout.
     * @tc.expected: step2. Child box's size is 50x50.
     */
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));
    ASSERT_TRUE(childBox->GetLayoutSize() == Size(HALF_SIZE.Value(), HALF_SIZE.Value()));
}
#endif

/**
 * @tc.name: RenderBoxPercentBoxInBubble001
 * @tc.desc: RenderBox could handle width and height in percent, parent is bubble (popup).
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentBoxInBubble001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct parent box with 100x100, and bubble, child box with 50%x50%.
     */
    auto parentRenderBox = ConstructRenderBox(context_, PARENT_SIZE, PARENT_SIZE);
    auto renderBubble = RenderBubble::Create();
    parentRenderBox->AddChild(renderBubble);
    auto childBox = ConstructRenderBox(context_, PERCENT_HALF, PERCENT_HALF);
    renderBubble->Attach(context_);
    renderBubble->AddChild(childBox);

    /**
     * @tc.steps: step2. Perform layout.
     * @tc.expected: step2. Child box's size is 50x50.
     */
    MockPerformLayout(context_, AceType::DynamicCast<RenderNode>(parentRenderBox));
    double expectWidth = (PARENT_SIZE.Value() - BUBBLE_MARGIN.Value()) * PERCENT_HALF.Value();
    ASSERT_TRUE(childBox->GetLayoutSize() == Size(expectWidth, HALF_SIZE.Value()));
}

/**
 * @tc.name: RenderBoxPercentSwiperIndicator001
 * @tc.desc: Swiper indicator could handle position in percent, left = 50%, top = 50%.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentSwiperIndicator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a swiper component, set 50% to indicator left and top.
     */
    auto indicator = AceType::MakeRefPtr<SwiperIndicator>();
    indicator->SetTop(PERCENT_HALF);
    indicator->SetLeft(PERCENT_HALF);
    auto renderSwiper = TestSwiperIndicator(context_, indicator);

    /**
     * @tc.steps: step2. Check indicator position.
     * @tc.expected: step2. Indicator position is x = 50, y = 50.
     */
    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetX(), PARENT_SIZE.Value() * PERCENT_HALF.Value()));
    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetY(), PARENT_SIZE.Value() * PERCENT_HALF.Value()));
}

/**
 * @tc.name: RenderBoxPercentSwiperIndicator002
 * @tc.desc: Swiper indicator could handle position in percent, left = -50%, top = -50%.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentSwiperIndicator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a swiper component, set -50% to indicator left and top.
     */
    auto indicator = AceType::MakeRefPtr<SwiperIndicator>();
    indicator->SetTop(PERCENT_NEGATIVE);
    indicator->SetLeft(PERCENT_NEGATIVE);
    auto renderSwiper = TestSwiperIndicator(context_, indicator);

    /**
     * @tc.steps: step2. Check indicator position.
     * @tc.expected: step2. Indicator position is x = 0, y = 0.
     */
    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetX(), 0.0));
    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetY(), 0.0));
}

/**
 * @tc.name: RenderBoxPercentSwiperIndicator003
 * @tc.desc: Swiper indicator could handle position in percent, left = 100%, top = 100%.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentSwiperIndicator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a swiper component, set 100% to indicator left and top.
     */
    auto indicator = AceType::MakeRefPtr<SwiperIndicator>();
    indicator->SetTop(Dimension(1.0, DimensionUnit::PERCENT));
    indicator->SetLeft(Dimension(1.0, DimensionUnit::PERCENT));
    auto renderSwiper = TestSwiperIndicator(context_, indicator);

    /**
     * @tc.steps: step2. Check indicator position.
     * @tc.expected: step2. Indicator position is x = width - indicatorWidth, y = height - selectHeight.
     */
    double indicatorSize = renderSwiper->NormalizeToPx(indicator->GetSize());
    double indicatorSelectSize = renderSwiper->NormalizeToPx(indicator->GetSelectedSize());
    double indicatorWidth = indicatorSelectSize + indicatorSize * (SWIPER_CHILD_SIZE - 1) +
                            renderSwiper->NormalizeToPx(INDICATOR_POINT_PADDING) * (SWIPER_CHILD_SIZE - 1);
    double expectX = renderSwiper->swiperWidth_ - indicatorWidth;
    double expectY = renderSwiper->swiperHeight_ - indicatorSelectSize;

    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetX(), expectX));
    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetY(), expectY));
}

/**
 * @tc.name: RenderBoxPercentSwiperIndicator004
 * @tc.desc: Swiper indicator could handle position in percent, right = 50%, bottom = 50%.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentSwiperIndicator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a swiper component, set 50% to indicator right and bottom.
     */
    auto indicator = AceType::MakeRefPtr<SwiperIndicator>();
    indicator->SetRight(PERCENT_HALF);
    indicator->SetBottom(PERCENT_HALF);
    auto renderSwiper = TestSwiperIndicator(context_, indicator);

    /**
     * @tc.steps: step2. Check indicator position.
     * @tc.expected: step2. Indicator position is x = 50% * width - indicatorWidth, y = 50% * height - selectHeight.
     */
    double indicatorSize = renderSwiper->NormalizeToPx(indicator->GetSize());
    double indicatorSelectSize = renderSwiper->NormalizeToPx(indicator->GetSelectedSize());
    double indicatorWidth = indicatorSelectSize + indicatorSize * (SWIPER_CHILD_SIZE - 1) +
                            renderSwiper->NormalizeToPx(INDICATOR_POINT_PADDING) * (SWIPER_CHILD_SIZE - 1);
    double expectX = renderSwiper->swiperWidth_ * PERCENT_HALF.Value() - indicatorWidth;
    double expectY = renderSwiper->swiperHeight_ * PERCENT_HALF.Value() - indicatorSelectSize;

    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetX(), expectX));
    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetY(), expectY));
}

/**
 * @tc.name: RenderBoxPercentSwiperIndicator005
 * @tc.desc: Swiper indicator could handle position in percent, left, top, right, bottom = 50%.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PV
 * @tc.author: liruokun
 */
HWTEST_F(RenderBoxPercentTest, RenderBoxPercentSwiperIndicator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a swiper component, set 50% to indicator left, top, right and bottom.
     */
    auto indicator = AceType::MakeRefPtr<SwiperIndicator>();
    indicator->SetTop(PERCENT_HALF);
    indicator->SetLeft(PERCENT_HALF);
    indicator->SetRight(PERCENT_HALF);
    indicator->SetBottom(PERCENT_HALF);
    auto renderSwiper = TestSwiperIndicator(context_, indicator);

    /**
     * @tc.steps: step2. Check indicator position.
     * @tc.expected: step2. Indicator position is x = 50, y = 50.
     */
    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetX(), PARENT_SIZE.Value() * PERCENT_HALF.Value()));
    ASSERT_TRUE(NearEqual(renderSwiper->indicatorPosition_.GetY(), PARENT_SIZE.Value() * PERCENT_HALF.Value()));
}

} // namespace OHOS::Ace
