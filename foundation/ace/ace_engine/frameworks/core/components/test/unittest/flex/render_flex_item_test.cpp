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

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/test/unittest/flex/flex_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double TEST_FLEX_BASIS = 400.0;
constexpr double TEST_FLEX_GROW = 2.0;
constexpr double TEST_FLEX_SHRINK = 2.0;
constexpr double SMALL_BOX = 100.0;
constexpr double MEDIUM_BOX = 200.0;
constexpr double THIRD_LAEGEST_BOX = 500.0;
constexpr double LARGE_BOX = 1000.0;
constexpr double SUPER_BOX = 3000.0;
constexpr double RECT_WIDTH = 1080.0;
constexpr double RECT_HEIGHT = 2244.0;
constexpr double GROW_WIDTH = 490.0;
constexpr double GROW_HEIGHT = 1072.0;
constexpr double ONE_THIRD_HEIGHT = 748.0;
constexpr double SHRINK_HEIGHT = 1022.0;

} // namespace

class RenderFlexItemTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "RenderFlexItemTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "RenderFlexItemTest TearDownTestCase";
    }

    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: FlexItemBasis001
 * @tc.desc: verify the row with two flex children works fine, where the flex basis are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasis001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasis001 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex basis property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    row->AddChild(secondFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set as basis, the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(TEST_FLEX_BASIS, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(TEST_FLEX_BASIS, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(TEST_FLEX_BASIS, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasis001 stop";
}

/**
 * @tc.name: FlexItemBasis002
 * @tc.desc: verify the row with one flex child and one normal child works fine, where the flex basis are set in the
 *           flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasis002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasis002 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex basis property to the flex item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(TEST_FLEX_BASIS, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(TEST_FLEX_BASIS, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasis002 stop";
}

/**
 * @tc.name: FlexItemBasis003
 * @tc.desc: verify the column with two flex children works fine, where the flex basis are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasis003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasis003 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex basis property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    column->AddChild(secondFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set as basis, the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, TEST_FLEX_BASIS));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, TEST_FLEX_BASIS));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, TEST_FLEX_BASIS));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasis003 stop";
}

/**
 * @tc.name: FlexItemBasis004
 * @tc.desc: verify the column with one flex child and one normal child works fine, where the flex basis are set in the
 *           flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasis004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasis004 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex basis property to the flex item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, TEST_FLEX_BASIS));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, TEST_FLEX_BASIS));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasis004 stop";
}

/**
 * @tc.name: FlexItemGrow001
 * @tc.desc: verify the row with two flex children works fine, where the flex grow are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemGrow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemGrow001 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex grow property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    row->AddChild(secondFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set correctly, the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(GROW_WIDTH, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(GROW_WIDTH, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(RECT_WIDTH - GROW_WIDTH, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemGrow001 stop";
}

/**
 * @tc.name: FlexItemGrow002
 * @tc.desc: verify the row with one flex child and one normal child works fine, where the flex grow are set in the flex
 *           children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemGrow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemGrow002 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex grow property to the single flex item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(RECT_WIDTH - MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH - MEDIUM_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemGrow002 stop";
}

/**
 * @tc.name: FlexItemGrow003
 * @tc.desc: verify the column with two flex children works fine, where the flex grow are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemGrow003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemGrow003 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex grow property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    column->AddChild(secondFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set correctly, the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, GROW_HEIGHT));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, GROW_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, RECT_HEIGHT - GROW_HEIGHT));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemGrow003 stop";
}

/**
 * @tc.name: FlexItemGrow004
 * @tc.desc: verify the column with one flex child and one normal child works fine, where the flex grow are set in the
 *           flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemGrow004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemGrow004 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex grow property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, RECT_HEIGHT - MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, RECT_HEIGHT - MEDIUM_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemGrow004 stop";
}

/**
 * @tc.name: FlexItemShrink001
 * @tc.desc: verify the row with two flex children works fine, where the flex shrink are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemShrink001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemShrink001 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex shrink property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    row->AddChild(secondFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set correctly, the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(RECT_WIDTH / 2.0, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH / 2.0, LARGE_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(RECT_WIDTH / 2.0, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemShrink001 stop";
}

/**
 * @tc.name: FlexItemShrink002
 * @tc.desc: verify the row with one flex child and one normal child works fine, where the flex shrink are set in the
 *           flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemShrink002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemShrink002 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex shrink property to the single flex item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(RECT_WIDTH - MEDIUM_BOX, 0));
    EXPECT_TRUE(firstFlexItem->GetLayoutSize() == Size(RECT_WIDTH - MEDIUM_BOX, LARGE_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH - MEDIUM_BOX, LARGE_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemShrink002 stop";
}

/**
 * @tc.name: FlexItemShrink003
 * @tc.desc: verify the column with three flex children works fine, where the flex shrink are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemShrink003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemShrink003 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex shrink property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set correctly, the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, ONE_THIRD_HEIGHT));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, ONE_THIRD_HEIGHT + ONE_THIRD_HEIGHT));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(LARGE_BOX, ONE_THIRD_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(LARGE_BOX, ONE_THIRD_HEIGHT));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, ONE_THIRD_HEIGHT));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemShrink003 stop";
}

/**
 * @tc.name: FlexItemShrink004
 * @tc.desc: verify the column with two flex children and one normal child works fine, where the flex shrink are set in
 *           the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemShrink004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemShrink004 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex shrink property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SUPER_BOX, SUPER_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(SUPER_BOX, SUPER_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, SHRINK_HEIGHT));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SHRINK_HEIGHT + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH, SHRINK_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(RECT_WIDTH, RECT_HEIGHT - SHRINK_HEIGHT - MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemShrink004 stop";
}

/**
 * @tc.name: FlexItemBasisGrow001
 * @tc.desc: verify the row with two flex children works fine, where the flex basis and grow are set in the flex
 *           children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasisGrow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisGrow001 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex basis and grow property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    row->AddChild(secondFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set correctly, the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(RECT_WIDTH / 2.0, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH / 2.0, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(RECT_WIDTH / 2.0, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisGrow001 stop";
}

/**
 * @tc.name: FlexItemBasisGrow002
 * @tc.desc: verify the row with one flex child and one normal child works fine, where the flex grow and basis are set
 *           in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasisGrow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisGrow002 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex grow and basis property to the single flex item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(RECT_WIDTH - MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH - MEDIUM_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisGrow002 stop";
}

/**
 * @tc.name: FlexItemBasisGrow003
 * @tc.desc: verify the column with two flex children works fine, where the flex grow are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasisGrow003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisGrow003 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex grow property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    column->AddChild(secondFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set correctly, the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, RECT_HEIGHT / 2.0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, RECT_HEIGHT / 2.0));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, RECT_HEIGHT / 2.0));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisGrow003 stop";
}

/**
 * @tc.name: FlexItemBasisGrow004
 * @tc.desc: verify the column with one flex child and one normal child works fine, where the flex grow are set in the
 *           flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasisGrow004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisGrow004 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex grow property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(TEST_FLEX_BASIS, TEST_FLEX_GROW, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, RECT_HEIGHT - MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, RECT_HEIGHT - MEDIUM_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisGrow004 stop";
}

/**
 * @tc.name: FlexItemBasisShrink001
 * @tc.desc: verify the row with two flex children works fine, where the flex shrink are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasisShrink001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisShrink001 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex shrink property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(SUPER_BOX, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(SUPER_BOX, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    row->AddChild(secondFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set correctly, the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(RECT_WIDTH / 2.0, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH / 2.0, LARGE_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(RECT_WIDTH / 2.0, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisShrink001 stop";
}

/**
 * @tc.name: FlexItemBasisShrink002
 * @tc.desc: verify the row with one flex child and one normal child works fine, where the flex shrink are set in the
 *           flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasisShrink002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisShrink002 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex shrink property to the single flex item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(SUPER_BOX, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    row->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(RECT_WIDTH - MEDIUM_BOX, 0));
    EXPECT_TRUE(firstFlexItem->GetLayoutSize() == Size(RECT_WIDTH - MEDIUM_BOX, LARGE_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH - MEDIUM_BOX, LARGE_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisShrink002 stop";
}

/**
 * @tc.name: FlexItemBasisShrink003
 * @tc.desc: verify the column with three flex children works fine, where the flex shrink are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasisShrink003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisShrink003 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex shrink property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(SUPER_BOX, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(SUPER_BOX, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(SUPER_BOX, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the size of the flex items are set correctly, the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, ONE_THIRD_HEIGHT));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, ONE_THIRD_HEIGHT + ONE_THIRD_HEIGHT));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(LARGE_BOX, ONE_THIRD_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(LARGE_BOX, ONE_THIRD_HEIGHT));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, ONE_THIRD_HEIGHT));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisShrink003 stop";
}

/**
 * @tc.name: FlexItemBasisShrink004
 * @tc.desc: verify the column with two flex children and one normal child works fine, where the flex shrink are set in
 *           the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemBasisShrink004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisShrink004 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex shrink property to the flex items
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(SUPER_BOX, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    column->AddChild(firstFlexItem);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(SUPER_BOX, 0, TEST_FLEX_SHRINK);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(SUPER_BOX, SUPER_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the sizes are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, (RECT_HEIGHT - MEDIUM_BOX) / 2.0));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, ((RECT_HEIGHT - MEDIUM_BOX) / 2.0 + MEDIUM_BOX)));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(LARGE_BOX, (RECT_HEIGHT - MEDIUM_BOX) / 2.0));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(RECT_WIDTH, (RECT_HEIGHT - MEDIUM_BOX) / 2.0));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemBasisShrink004 stop";
}

/**
 * @tc.name: RenderFlexItemUpdate001
 * @tc.desc: Verify the Update Interface of RenderFlexItem does not work for other components
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, RenderFlexItemUpdate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest RenderFlexItemUpdate001 start";
    /**
     * @tc.steps: step1. construct Text Component and RenderFlex.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("Hi Ace");
    RefPtr<RenderFlexItem> renderFlex = AceType::MakeRefPtr<RenderFlexItem>();
    /**
     * @tc.steps: step2. call the Update interface of RenderFlex
     * @tc.expected: step2. renderFlex are not set need layout
     */
    renderFlex->Update(text);
    EXPECT_TRUE(!renderFlex->NeedLayout());
    GTEST_LOG_(INFO) << "RenderFlexItemTest RenderFlexItemUpdate001 stop";
}

/**
 * @tc.name: RenderFlexItemUpdate002
 * @tc.desc: Verify that RenderFlexItem works for flex item component.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, RenderFlexItemUpdate002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest RenderFlexItemUpdate002 start";
    /**
     * @tc.steps: step1. construct flex item component and RenderFlexItem.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("Hi Ace");
    RefPtr<FlexItemComponent> flex = AceType::MakeRefPtr<FlexItemComponent>(1, 1, 1, text);
    RefPtr<RenderFlexItem> renderFlex = AceType::MakeRefPtr<RenderFlexItem>();
    /**
     * @tc.steps: step2. call Update interface of renderFlexItem
     * @tc.expected: step2. renderFlex are set needLayout
     */
    renderFlex->Update(flex);
    EXPECT_TRUE(renderFlex->NeedLayout());
    GTEST_LOG_(INFO) << "RenderFlexItemTest RenderFlexItemUpdate002 stop";
}

/**
 * @tc.name: FlexItemAlignSelf001
 * @tc.desc: verify the column with four flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf001 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    thirdFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    column->AddChild(thirdFlexItem);
    RefPtr<RenderFlexItem> forthFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> forthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    forthBox->Attach(mockContext);
    forthFlexItem->AddChild(forthBox);
    column->AddChild(forthFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    forthFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX/2 - MEDIUM_BOX/2, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(LARGE_BOX - THIRD_LAEGEST_BOX, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(forthFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX + THIRD_LAEGEST_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    EXPECT_TRUE(forthBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf001 stop";
}

/**
 * @tc.name: FlexItemAlignSelf002
 * @tc.desc: verify the column with two flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf002 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    thirdFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    column->AddChild(thirdFlexItem);
    RefPtr<RenderFlexItem> forthFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> forthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    forthBox->Attach(mockContext);
    forthFlexItem->AddChild(forthBox);
    column->AddChild(forthFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    forthFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(0, 222));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX/2 - MEDIUM_BOX/2, 222 + SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(LARGE_BOX - THIRD_LAEGEST_BOX, 222 + SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(forthFlexItem->GetPosition() == Offset(0, 222 + SMALL_BOX + MEDIUM_BOX + THIRD_LAEGEST_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    EXPECT_TRUE(forthBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf002 stop";
}

/**
 * @tc.name: FlexItemAlignSelf003
 * @tc.desc: verify the column with two flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf003 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::FLEX_END);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    thirdFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(thirdFlexItem);
    RefPtr<RenderFlexItem> forthFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> forthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    forthBox->Attach(mockContext);
    forthFlexItem->AddChild(forthBox);
    column->AddChild(forthFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    forthFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(0, 222));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX/2 - MEDIUM_BOX/2, 222 + SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, 222 + SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(forthFlexItem->GetPosition() == Offset(0, 222 + SMALL_BOX + MEDIUM_BOX + THIRD_LAEGEST_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    EXPECT_TRUE(forthBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf003 stop";
}

/**
 * @tc.name: FlexItemAlignSelf004
 * @tc.desc: verify the column with two flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf004 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_END, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    thirdFlexItem->SetAlignSelf(FlexAlign::STRETCH);
    column->AddChild(thirdFlexItem);
    RefPtr<RenderFlexItem> forthFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> forthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    forthBox->Attach(mockContext);
    forthFlexItem->AddChild(forthBox);
    column->AddChild(forthFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    forthFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(LARGE_BOX/2 - SMALL_BOX/2, 444));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX - MEDIUM_BOX, 444 + SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, 444 + SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(forthFlexItem->GetPosition() == Offset(0, 444 + SMALL_BOX + MEDIUM_BOX + THIRD_LAEGEST_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, THIRD_LAEGEST_BOX));
    EXPECT_TRUE(forthBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf004 stop";
}

/**
 * @tc.name: FlexItemAlignSelf005
 * @tc.desc: verify the column with two flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf005 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_END, FlexAlign::CENTER);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::STRETCH);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    thirdFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(thirdFlexItem);
    RefPtr<RenderFlexItem> forthFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> forthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    forthBox->Attach(mockContext);
    forthFlexItem->AddChild(forthBox);
    column->AddChild(forthFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    forthFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(0, 444));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX - MEDIUM_BOX, 444 + SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, 444 + SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(forthFlexItem->GetPosition() == Offset(0, 444 + SMALL_BOX + MEDIUM_BOX + THIRD_LAEGEST_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(LARGE_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    EXPECT_TRUE(forthBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf005 stop";
}

/**
 * @tc.name: FlexItemAlignSelf006
 * @tc.desc: verify the column with two flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf006 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_END);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::STRETCH);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    thirdFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(thirdFlexItem);
    RefPtr<RenderFlexItem> forthFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> forthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    forthBox->Attach(mockContext);
    forthFlexItem->AddChild(forthBox);
    column->AddChild(forthFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    forthFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX/2 - MEDIUM_BOX/2, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(forthFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX + THIRD_LAEGEST_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(LARGE_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    EXPECT_TRUE(forthBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf006 stop";
}

/**
 * @tc.name: FlexItemAlignSelf007
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf007 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(0, THIRD_LAEGEST_BOX - SMALL_BOX));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(SMALL_BOX, THIRD_LAEGEST_BOX/2 - MEDIUM_BOX/2));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf007 stop";
}

/**
 * @tc.name: FlexItemAlignSelf008
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf008 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_END);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(SMALL_BOX, THIRD_LAEGEST_BOX/2-MEDIUM_BOX/2));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf008 stop";
}

/**
 * @tc.name: FlexItemAlignSelf009
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf009 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::CENTER, FlexAlign::FLEX_END);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::CENTER);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(140, THIRD_LAEGEST_BOX/2-SMALL_BOX/2));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(140 + SMALL_BOX, 0));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(140 + SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf009 stop";
}

/**
 * @tc.name: FlexItemAlignSelf010
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf010 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_END, FlexAlign::FLEX_END);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::STRETCH);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(280, 0));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(280 + SMALL_BOX, 0));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(280 + SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, THIRD_LAEGEST_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf010 stop";
}

/**
 * @tc.name: FlexItemAlignSelf011
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf011 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_END, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::STRETCH);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(280, 0));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(280 + SMALL_BOX, THIRD_LAEGEST_BOX/2-MEDIUM_BOX/2));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(280 + SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, THIRD_LAEGEST_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf011 stop";
}

/**
 * @tc.name: FlexItemAlignSelf012
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf012 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_END, FlexAlign::STRETCH);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(280, THIRD_LAEGEST_BOX - SMALL_BOX));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(280 + SMALL_BOX, THIRD_LAEGEST_BOX/2-MEDIUM_BOX/2));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(280 + SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf012 stop";
}

/**
 * @tc.name: FlexItemAlignSelf013
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf013 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::STRETCH);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX/2 - MEDIUM_BOX/2, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf013 stop";
}

/**
 * @tc.name: FlexItemAlignSelf014
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf014 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_END);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(LARGE_BOX/2 - SMALL_BOX/2, 0));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf014 stop";
}

/**
 * @tc.name: FlexItemAlignSelf015
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf015 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::STRETCH);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX - MEDIUM_BOX, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf015 stop";
}

/**
 * @tc.name: FlexItemAlignSelf016
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf016 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::STRETCH);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::STRETCH);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(LARGE_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(LARGE_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf016 stop";
}

/**
 * @tc.name: FlexItemAlignSelf017
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf017 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(SMALL_BOX, THIRD_LAEGEST_BOX/2 - MEDIUM_BOX/2));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf017 stop";
}

/**
 * @tc.name: FlexItemAlignSelf018
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf018 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(SMALL_BOX, THIRD_LAEGEST_BOX - MEDIUM_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf018 stop";
}

/**
 * @tc.name: FlexItemAlignSelf019
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf019 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::STRETCH);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(SMALL_BOX, 0));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, THIRD_LAEGEST_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf019 stop";
}

/**
 * @tc.name: FlexItemAlignSelf020
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf020 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::CENTER);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(0, THIRD_LAEGEST_BOX/2 - SMALL_BOX/2));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(SMALL_BOX, THIRD_LAEGEST_BOX - MEDIUM_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf020 stop";
}

/**
 * @tc.name: FlexItemAlignSelf021
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf021, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf021 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::CENTER);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(0, THIRD_LAEGEST_BOX/2 - SMALL_BOX/2));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(SMALL_BOX, 0));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf021 stop";
}

/**
 * @tc.name: FlexItemAlignSelf022
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf022, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf022 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(0, THIRD_LAEGEST_BOX - SMALL_BOX));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(SMALL_BOX, 0));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf022 stop";
}

/**
 * @tc.name: FlexItemAlignSelf023
 * @tc.desc: verify the row with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf023, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf023 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    row->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    row->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    row->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(0, THIRD_LAEGEST_BOX - SMALL_BOX));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(SMALL_BOX, THIRD_LAEGEST_BOX/2 - MEDIUM_BOX/2));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(THIRD_LAEGEST_BOX, THIRD_LAEGEST_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf023 stop";
}

/**
 * @tc.name: FlexItemAlignSelf024
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf024, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf024 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX/2 - MEDIUM_BOX/2, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf024 stop";
}

/**
 * @tc.name: FlexItemAlignSelf025
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf025, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf025 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX - MEDIUM_BOX, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf025 stop";
}

/**
 * @tc.name: FlexItemAlignSelf026
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf026, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf026 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::STRETCH);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition().IsZero());
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(LARGE_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf026 stop";
}

/**
 * @tc.name: FlexItemAlignSelf027
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf027, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf027 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(LARGE_BOX/2 - SMALL_BOX/2, 0));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX - MEDIUM_BOX, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf027 stop";
}

/**
 * @tc.name: FlexItemAlignSelf028
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf028, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf028 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(LARGE_BOX/2 - SMALL_BOX/2, 0));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf028 stop";
}

/**
 * @tc.name: FlexItemAlignSelf029
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf029, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf029 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::FLEX_START);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(LARGE_BOX - SMALL_BOX, 0));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(0, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf029 stop";
}

/**
 * @tc.name: FlexItemAlignSelf030
 * @tc.desc: verify the column with three flex children works fine, where the flex alignSelf are set in the flex children.
 * @tc.type: FUNC
 */
HWTEST_F(RenderFlexItemTest, FlexItemAlignSelf030, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf030 start";
    /**
     * @tc.steps: step1. construct the render node tree, set flex alignSelf property to the flex items.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlexItem> firstFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    firstFlexItem->AddChild(firstBox);
    firstFlexItem->SetAlignSelf(FlexAlign::FLEX_END);
    column->AddChild(firstFlexItem);
    RefPtr<RenderFlexItem> secondFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    secondFlexItem->AddChild(secondBox);
    secondFlexItem->SetAlignSelf(FlexAlign::CENTER);
    column->AddChild(secondFlexItem);
    RefPtr<RenderFlexItem> thirdFlexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    thirdFlexItem->AddChild(thirdBox);
    column->AddChild(thirdFlexItem);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    firstFlexItem->Attach(mockContext);
    secondFlexItem->Attach(mockContext);
    thirdFlexItem->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstFlexItem->GetPosition() == Offset(LARGE_BOX - SMALL_BOX, 0));
    EXPECT_TRUE(secondFlexItem->GetPosition() == Offset(LARGE_BOX/2 - MEDIUM_BOX/2, SMALL_BOX));
    EXPECT_TRUE(thirdFlexItem->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
    GTEST_LOG_(INFO) << "RenderFlexItemTest FlexItemAlignSelf030 stop";
}

} // namespace OHOS::Ace
