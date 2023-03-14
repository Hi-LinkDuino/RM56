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
constexpr double CENTER_SPACE_SIZE = 822.0;
constexpr double END_SPACE_SIZE = 1644.0;
constexpr double BETWEEN_SPACE_SIZE = 822.0;
constexpr double AROUND_FRONT_SPACE_SIZE = 274.0;
constexpr double AROUND_SPACE_SIZE = 548.0;
constexpr double EVENLY_SPACE_SIZE = 411.0;
constexpr double CENTER_ALIGN_SIZE = 50.0;
constexpr double END_ALIGN_SIZE = 100.0;
constexpr double ROW_COL_CENTER_SIZE = 972.0;
constexpr double ROW_COL_SMALL_CENTER_SIZE = 872.0;

} // namespace

class RenderColumnTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "RenderColumnTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "RenderColumnTest TearDownTestCase";
    }

    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: RenderColumnLayout001
 * @tc.desc: Verify the column component with main(start) and cross(start) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout001 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout002
 * @tc.desc: Verify the column component with main(start) and cross(center) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout002 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as start-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::CENTER);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(CENTER_ALIGN_SIZE + CENTER_ALIGN_SIZE, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(CENTER_ALIGN_SIZE, SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout003
 * @tc.desc: Verify the column component with main(start) and cross(end) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout003 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as start-end.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_END);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(END_ALIGN_SIZE + END_ALIGN_SIZE, 0.0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(END_ALIGN_SIZE, SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout004
 * @tc.desc: Verify the column component with main(start) and cross(stretch) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout004 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as start-stretch.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::STRETCH);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the size of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, 0.0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0.0, SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(LARGE_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(LARGE_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
}

/**
 * @tc.name: RenderColumnLayout005
 * @tc.desc: Verify the column component with main(center) and cross(start) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout005 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as center-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::FLEX_START);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, CENTER_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, CENTER_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0.0, CENTER_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout006
 * @tc.desc: Verify the column component with main(end) and cross(start) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout006 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as end-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_END, FlexAlign::FLEX_START);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0.0, END_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, END_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0.0, END_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout007
 * @tc.desc: Verify the column component with main(space-between) and cross(start) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout007 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as space_between-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::SPACE_BETWEEN, FlexAlign::FLEX_START);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, BETWEEN_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(
        thirdBox->GetPosition() == Offset(0.0, BETWEEN_SPACE_SIZE + BETWEEN_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout008
 * @tc.desc: Verify the column component with main(space-around) and cross(start) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout008 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as space_around-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::SPACE_AROUND, FlexAlign::FLEX_START);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0.0, AROUND_FRONT_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, AROUND_FRONT_SPACE_SIZE + AROUND_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() ==
                Offset(0.0, AROUND_FRONT_SPACE_SIZE + AROUND_SPACE_SIZE + AROUND_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout009
 * @tc.desc: Verify the column component with main(center) and cross(center) under column component works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout009 start";
    /**
     * @tc.steps: step1. construct the node tree, column has a child column, child column are set as center-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderFlex> childCol =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::CENTER);
    childCol->Attach(mockContext);
    column->AddChild(childCol);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    childCol->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    childCol->AddChild(secondBox);
    RefPtr<RenderFlex> secondChildCol =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::CENTER);
    secondChildCol->Attach(mockContext);
    column->AddChild(secondChildCol);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    thirdBox->Attach(mockContext);
    secondChildCol->AddChild(thirdBox);
    RefPtr<RenderBox> fourthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    fourthBox->Attach(mockContext);
    secondChildCol->AddChild(fourthBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(childCol->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondChildCol->GetPosition() == Offset(0.0, 2244.0));
    EXPECT_TRUE(firstBox->GetPosition() == Offset(CENTER_ALIGN_SIZE, ROW_COL_CENTER_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, ROW_COL_CENTER_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(CENTER_ALIGN_SIZE, ROW_COL_SMALL_CENTER_SIZE));
    EXPECT_TRUE(fourthBox->GetPosition() == Offset(0.0, ROW_COL_SMALL_CENTER_SIZE + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout010
 * @tc.desc: Verify the row component with main(center) and cross(center) under column component works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout010 start";
    /**
     * @tc.steps: step1. construct the node tree, parent row has a child column, child column are set as center-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    row->Attach(mockContext);
    root->AddChild(row);
    RefPtr<RenderFlex> childCol =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::CENTER);
    childCol->Attach(mockContext);
    row->AddChild(childCol);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    childCol->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    childCol->AddChild(secondBox);
    RefPtr<RenderFlex> secondChildCol =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::CENTER);
    secondChildCol->Attach(mockContext);
    row->AddChild(secondChildCol);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    thirdBox->Attach(mockContext);
    secondChildCol->AddChild(thirdBox);
    RefPtr<RenderBox> fourthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    fourthBox->Attach(mockContext);
    secondChildCol->AddChild(fourthBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(childCol->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondChildCol->GetPosition() == Offset(MEDIUM_BOX, 0.0));
    EXPECT_TRUE(firstBox->GetPosition() == Offset(CENTER_ALIGN_SIZE, ROW_COL_CENTER_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, ROW_COL_CENTER_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(CENTER_ALIGN_SIZE, ROW_COL_SMALL_CENTER_SIZE));
    EXPECT_TRUE(fourthBox->GetPosition() == Offset(0.0, ROW_COL_SMALL_CENTER_SIZE + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout011
 * @tc.desc: Verify the column component with main(space-evenly) and cross(start) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as space_between-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::SPACE_EVENLY, FlexAlign::FLEX_START);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0.0, EVENLY_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, 2 * EVENLY_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0.0, 3 * EVENLY_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout012
 * @tc.desc: Verify the column component with main(center) and cross(center) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout012 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as center-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::CENTER);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(CENTER_ALIGN_SIZE + CENTER_ALIGN_SIZE, CENTER_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(CENTER_ALIGN_SIZE, CENTER_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, CENTER_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout013
 * @tc.desc: Verify the column component with main(center) and cross(end) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout013 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as center-end.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::FLEX_END);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(END_ALIGN_SIZE + END_ALIGN_SIZE, CENTER_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(END_ALIGN_SIZE, CENTER_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, CENTER_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout014
 * @tc.desc: Verify the column component with main(center) and cross(stretch) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout014 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as center-stretch.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::CENTER, FlexAlign::STRETCH);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, CENTER_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, CENTER_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0.0, CENTER_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(LARGE_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(LARGE_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
}

/**
 * @tc.name: RenderColumnLayout015
 * @tc.desc: Verify the column component with main(end) and cross(center) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout015 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as end-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_END, FlexAlign::CENTER);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(CENTER_ALIGN_SIZE + CENTER_ALIGN_SIZE, END_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(CENTER_ALIGN_SIZE, END_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0.0, END_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout016
 * @tc.desc: Verify the column component with main(end) and cross(end) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout016 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as end-end.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_END, FlexAlign::FLEX_END);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(END_ALIGN_SIZE + END_ALIGN_SIZE, END_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(END_ALIGN_SIZE, END_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0.0, END_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
}

/**
 * @tc.name: RenderColumnLayout017
 * @tc.desc: Verify the column component with main(end) and cross(stretch) works fine.
 * @tc.type: FUNC
 */
HWTEST_F(RenderColumnTest, RenderColumnLayout017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderColumnTest RenderColumnLayout017 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, column are set as end-stretch.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_END, FlexAlign::STRETCH);
    column->Attach(mockContext);
    root->AddChild(column);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    column->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    column->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    column->AddChild(thirdBox);

        /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, END_SPACE_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, END_SPACE_SIZE + SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0.0, END_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(LARGE_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(LARGE_BOX, MEDIUM_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
}


} // namespace OHOS::Ace