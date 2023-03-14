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

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/test/unittest/flex/flex_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double SMALL_BOX = 100.0;
constexpr double MEDIUM_BOX = 200.0;
constexpr double LARGE_BOX = 300.0;
constexpr double RECT_WIDTH = 1080.0;
constexpr double RECT_HEIGHT = 2244.0;

} // namespace

class RenderMagicLayoutTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "RenderMagicLayoutTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "RenderMagicLayoutTest TearDownTestCase";
    }

    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: FlexWeight001
 * @tc.desc: Verify the row component with 1 weight child works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F2BCC
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    row->Attach(mockContext);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateWeightedBox(1, SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH, SMALL_BOX));
    EXPECT_TRUE(row->GetLayoutSize() == Size(RECT_WIDTH, SMALL_BOX));
}

/**
 * @tc.name: FlexWeight002
 * @tc.desc: Verify the row component with 3 same weight child works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAS
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    row->Attach(mockContext);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateWeightedBox(1, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateWeightedBox(2, SMALL_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateWeightedBox(3, SMALL_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(180, 0.0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(540, 0.0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(180.0, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(360.0, SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(540.0, SMALL_BOX));
    EXPECT_TRUE(row->GetLayoutSize() == Size(RECT_WIDTH, SMALL_BOX));
}

/**
 * @tc.name: FlexWeight003
 * @tc.desc: Verify the column component with 3 same weight child works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F2BCC
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::CENTER);
    row->Attach(mockContext);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateWeightedBox(1, SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateWeightedBox(2, MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateWeightedBox(3, LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(100, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(50, 374.0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 1122.0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, 374.0));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, 748.0));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, 1122.0));
    EXPECT_TRUE(row->GetLayoutSize() == Size(LARGE_BOX, RECT_HEIGHT));
}

/**
 * @tc.name: FlexWeight004
 * @tc.desc: Verify the row component with 3 different weight and aspect ratio children works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAI
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    row->Attach(mockContext);
    root->AddChild(row);
    auto firstItem = FlexTestUtils::CreateMagicNode(2, 1, 1, LayoutParam(Size(), Size()), mockContext);
    firstItem->Attach(mockContext);
    row->AddChild(firstItem);
    auto secondItem = FlexTestUtils::CreateMagicNode(1, 2, 1, LayoutParam(Size(), Size()), mockContext);
    secondItem->Attach(mockContext);
    row->AddChild(secondItem);
    auto thirdItem = FlexTestUtils::CreateMagicNode(0.5, 3, 1, LayoutParam(Size(), Size()), mockContext);
    thirdItem->Attach(mockContext);
    row->AddChild(thirdItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstItem->GetPosition().IsZero());
    EXPECT_TRUE(secondItem->GetPosition() == Offset(180, 0.0));
    EXPECT_TRUE(thirdItem->GetPosition() == Offset(540, 0.0));
    EXPECT_TRUE(firstItem->GetLayoutSize() == Size(180.0, 90));
    EXPECT_TRUE(secondItem->GetLayoutSize() == Size(360.0, 360));
    EXPECT_TRUE(thirdItem->GetLayoutSize() == Size(540.0, 1080));
    EXPECT_TRUE(row->GetLayoutSize() == Size(RECT_WIDTH, 1080));
}

/**
 * @tc.name: FlexWeight005
 * @tc.desc: Verify the column component with 3 different weight and aspect ratio children works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAR
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::CENTER);
    column->Attach(mockContext);
    root->AddChild(column);
    auto firstItem = FlexTestUtils::CreateMagicNode(2, 1, 1, LayoutParam(Size(), Size()), mockContext);
    firstItem->Attach(mockContext);
    column->AddChild(firstItem);
    auto secondItem = FlexTestUtils::CreateMagicNode(1, 2, 1, LayoutParam(Size(), Size()), mockContext);
    secondItem->Attach(mockContext);
    column->AddChild(secondItem);
    auto thirdItem = FlexTestUtils::CreateMagicNode(0.5, 3, 1, LayoutParam(Size(), Size()), mockContext);
    thirdItem->Attach(mockContext);
    column->AddChild(thirdItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstItem->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondItem->GetPosition() == Offset(0.0, 374.0));
    EXPECT_TRUE(thirdItem->GetPosition() == Offset(93.5, 1122.0));
    EXPECT_TRUE(firstItem->GetLayoutSize() == Size(748, 374.0));
    EXPECT_TRUE(secondItem->GetLayoutSize() == Size(748.0, 748.0));
    EXPECT_TRUE(thirdItem->GetLayoutSize() == Size(561.0, 1122));
    EXPECT_TRUE(column->GetLayoutSize() == Size(748.0, 2244.0));
}

/**
 * @tc.name: FlexWeight006
 * @tc.desc: Verify the row component with 3 different weight and constraint children works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAJ
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_END);
    row->Attach(mockContext);
    root->AddChild(row);
    LayoutParam constraint = LayoutParam(Size(1500.0, 500.0), Size(500.0, 100.0));
    auto firstItem = FlexTestUtils::CreateMagicNode(1, 1, 1, constraint, mockContext);
    firstItem->Attach(mockContext);
    row->AddChild(firstItem);
    auto secondItem = FlexTestUtils::CreateMagicNode(1, 2, 1, constraint, mockContext);
    secondItem->Attach(mockContext);
    row->AddChild(secondItem);
    auto thirdItem = FlexTestUtils::CreateMagicNode(1, 3, 3, constraint, mockContext);
    thirdItem->Attach(mockContext);
    row->AddChild(thirdItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstItem->GetPosition() == Offset(0.0, 500.0));
    EXPECT_TRUE(secondItem->GetPosition() == Offset(0.0, 500.0));
    EXPECT_TRUE(thirdItem->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(firstItem->GetLayoutSize() == Size(0, 0.0));
    EXPECT_TRUE(secondItem->GetLayoutSize() == Size(0.0, 0.0));
    EXPECT_TRUE(thirdItem->GetLayoutSize() == Size(1080.0, 500));
    EXPECT_TRUE(row->GetLayoutSize() == Size(1080.0, 500.0));
}

/**
 * @tc.name: FlexWeight007
 * @tc.desc: Verify the column component with 3 different weight and displayIndex children works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAK
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_END);
    column->Attach(mockContext);
    root->AddChild(column);
    LayoutParam constraint = LayoutParam(Size(150.0, 1200.0), Size(50.0, 500.0));
    auto firstItem = FlexTestUtils::CreateMagicNode(0, 1, 3, constraint, mockContext);
    firstItem->Attach(mockContext);
    column->AddChild(firstItem);
    auto secondItem = FlexTestUtils::CreateMagicNode(1, 2, 2, constraint, mockContext);
    secondItem->Attach(mockContext);
    column->AddChild(secondItem);
    auto thirdItem = FlexTestUtils::CreateMagicNode(0, 3, 1, constraint, mockContext);
    thirdItem->Attach(mockContext);
    column->AddChild(thirdItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstItem->GetPosition() == Offset(50.0, 0.0));
    EXPECT_TRUE(secondItem->GetPosition() == Offset(0.0, 748.0));
    EXPECT_TRUE(thirdItem->GetPosition() == Offset(150.0, 1948.0));
    EXPECT_TRUE(firstItem->GetLayoutSize() == Size(100.0, 748.0));
    EXPECT_TRUE(secondItem->GetLayoutSize() == Size(150.0, 1200.0));
    EXPECT_TRUE(thirdItem->GetLayoutSize() == Size(0.0, 0.0));
    EXPECT_TRUE(column->GetLayoutSize() == Size(150.0, 2244.0));
}

/**
 * @tc.name: FlexWeight008
 * @tc.desc: Verify the row component in weight and stretch works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F7J2H
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-stretch.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::STRETCH);
    row->Attach(mockContext);
    root->AddChild(row);
    LayoutParam constraint = LayoutParam(Size(1500.0, 500.0), Size(500.0, 100.0));
    auto firstItem = FlexTestUtils::CreateMagicNode(1, 5, 3, constraint, mockContext);
    firstItem->Attach(mockContext);
    row->AddChild(firstItem);
    auto secondItem = FlexTestUtils::CreateMagicNode(1, 2, 1, constraint, mockContext);
    secondItem->Attach(mockContext);
    row->AddChild(secondItem);
    auto thirdItem = FlexTestUtils::CreateMagicNode(1, 3, 3, constraint, mockContext);
    thirdItem->Attach(mockContext);
    row->AddChild(thirdItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstItem->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondItem->GetPosition() == Offset(675.0, 0.0));
    EXPECT_TRUE(thirdItem->GetPosition() == Offset(675, 0.0));
    EXPECT_TRUE(firstItem->GetLayoutSize() == Size(675, 500.0));
    EXPECT_TRUE(secondItem->GetLayoutSize() == Size(0.0, 0.0));
    EXPECT_TRUE(thirdItem->GetLayoutSize() == Size(500.0, 500.0));
    EXPECT_TRUE(row->GetLayoutSize() == Size(1080.0, 500.0));
}

/**
 * @tc.name: FlexWeight009
 * @tc.desc: Verify the column component with 3 different weight and stretch-mode works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F7JGL
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as start-stretch.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::STRETCH);
    row->Attach(mockContext);
    root->AddChild(row);
    LayoutParam constraint = LayoutParam(Size(1500.0, 500.0), Size(500.0, 100.0));
    auto firstItem = FlexTestUtils::CreateMagicNode(1, 1, 3, constraint, mockContext);
    firstItem->Attach(mockContext);
    row->AddChild(firstItem);
    auto secondItem = FlexTestUtils::CreateMagicNode(1, 2, 1, constraint, mockContext);
    secondItem->Attach(mockContext);
    row->AddChild(secondItem);
    auto thirdItem = FlexTestUtils::CreateMagicNode(1, 3, 3, constraint, mockContext);
    thirdItem->Attach(mockContext);
    row->AddChild(thirdItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstItem->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondItem->GetPosition() == Offset(0.0, 374.0));
    EXPECT_TRUE(thirdItem->GetPosition() == Offset(0, 874.0));
    EXPECT_TRUE(firstItem->GetLayoutSize() == Size(500, 374.0));
    EXPECT_TRUE(secondItem->GetLayoutSize() == Size(500.0, 500.0));
    EXPECT_TRUE(thirdItem->GetLayoutSize() == Size(500.0, 500));
    EXPECT_TRUE(row->GetLayoutSize() == Size(500.0, 2244.0));
}

/**
 * @tc.name: FlexWeight010
 * @tc.desc: Verify the row component with illegal weight works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F7JGL
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, FlexWeight010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-stretch.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::STRETCH);
    row->Attach(mockContext);
    root->AddChild(row);
    LayoutParam constraint = LayoutParam(Size(1500.0, 500.0), Size(500.0, 100.0));
    auto firstItem = FlexTestUtils::CreateMagicNode(1, 0, 3, constraint, mockContext);
    firstItem->Attach(mockContext);
    row->AddChild(firstItem);
    auto secondItem = FlexTestUtils::CreateMagicNode(1, -1, 1, constraint, mockContext);
    secondItem->Attach(mockContext);
    row->AddChild(secondItem);
    auto thirdItem = FlexTestUtils::CreateMagicNode(1, 3, 3, constraint, mockContext);
    thirdItem->Attach(mockContext);
    row->AddChild(thirdItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstItem->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondItem->GetPosition() == Offset(500.0, 0.0));
    EXPECT_TRUE(thirdItem->GetPosition() == Offset(1000, 0.0));
    EXPECT_TRUE(firstItem->GetLayoutSize() == Size(500, 500.0));
    EXPECT_TRUE(secondItem->GetLayoutSize() == Size(500.0, 500.0));
    EXPECT_TRUE(thirdItem->GetLayoutSize() == Size(500.0, 500));
    EXPECT_TRUE(row->GetLayoutSize() == Size(1080.0, 500.0));
}

/**
 * @tc.name: DisplayIndexTest001
 * @tc.desc: Verify the row component with 3 constrainted and displayIndex children with no aspect-ratio works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAI
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, DisplayIndexTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    row->Attach(mockContext);
    root->AddChild(row);
    LayoutParam constraint = LayoutParam(Size(1000.0, 1000.0), Size(500.0, 500.0));
    auto firstItem = FlexTestUtils::CreateMagicNode(0, 0, 3, constraint, mockContext);
    firstItem->Attach(mockContext);
    row->AddChild(firstItem);
    auto secondItem = FlexTestUtils::CreateMagicNode(0, 0, 2, constraint, mockContext);
    secondItem->Attach(mockContext);
    row->AddChild(secondItem);
    auto thirdItem = FlexTestUtils::CreateMagicNode(0, 0, 1, constraint, mockContext);
    thirdItem->Attach(mockContext);
    row->AddChild(thirdItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstItem->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondItem->GetPosition() == Offset(500.0, 0.0));
    EXPECT_TRUE(thirdItem->GetPosition() == Offset(1000, 0.0));
    EXPECT_TRUE(firstItem->GetLayoutSize() == Size(500.0, 500.0));
    EXPECT_TRUE(secondItem->GetLayoutSize() == Size(500.0, 500.0));
    EXPECT_TRUE(thirdItem->GetLayoutSize() == Size(0.0, 0.0));
    EXPECT_TRUE(row->GetLayoutSize() == Size(1080.0, 500.0));
}

/**
 * @tc.name: DisplayIndexTest002
 * @tc.desc: Verify the row component with 3 constrainted and displayIndex children with aspect-ratio works fine.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAS
 * @tc.author: yangfan
 */
HWTEST_F(RenderMagicLayoutTest, DisplayIndexTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, row are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::STRETCH);
    row->Attach(mockContext);
    root->AddChild(row);
    LayoutParam constraint = LayoutParam(Size(1000.0, 1000.0), Size(500.0, 500.0));
    auto firstItem = FlexTestUtils::CreateMagicNode(2, 0, 3, constraint, mockContext);
    firstItem->Attach(mockContext);
    row->AddChild(firstItem);
    auto secondItem = FlexTestUtils::CreateMagicNode(2, 0, 2, constraint, mockContext);
    secondItem->Attach(mockContext);
    row->AddChild(secondItem);
    auto thirdItem = FlexTestUtils::CreateMagicNode(1.5, 0, 1, constraint, mockContext);
    thirdItem->Attach(mockContext);
    row->AddChild(thirdItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstItem->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondItem->GetPosition() == Offset(1000.0, 0.0));
    EXPECT_TRUE(thirdItem->GetPosition() == Offset(1000, 0.0));
    EXPECT_TRUE(firstItem->GetLayoutSize() == Size(1000.0, 500.0));
    EXPECT_TRUE(secondItem->GetLayoutSize() == Size(0.0, 0.0));
    EXPECT_TRUE(thirdItem->GetLayoutSize() == Size(0.0, 0.0));
    EXPECT_TRUE(row->GetLayoutSize() == Size(1080.0, 500.0));
}

} // namespace OHOS::Ace