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
constexpr double SMALL_TEXT = 50.0;
constexpr double MEDIUM_TEXT = 100.0;
constexpr double LARGE_TEXT = 150.0;
constexpr double SMALL_BASELINE = 30;
constexpr double MEDIUM_BASELINE = 60;
constexpr double RECT_WIDTH = 1080.0;
constexpr double CENTER_SPACE_SIZE = 240.0;
constexpr double END_SPACE_SIZE = 480.0;
constexpr double BETWEEN_SPACE_SIZE = 240.0;
constexpr double AROUND_FRONT_SPACE_SIZE = 80.0;
constexpr double EVENLY_SPACE_SIZE = 120.0;
constexpr double AROUND_SPACE_SIZE = 160.0;
constexpr double CENTER_ALIGN_SIZE = 50.0;
constexpr double END_ALIGN_SIZE = 100.0;
constexpr double ROW_COL_CENTER_SIZE = 390.0;
constexpr double ROW_COL_SMALL_CENTER_SIZE = 290.0;

} // namespace

class RenderRowTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "RenderRowTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "RenderRowTest TearDownTestCase";
    }

    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: RenderFlexUpdate001
 * @tc.desc: Verify the Update Interface of RenderFlex does not work for other components
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH4
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderFlexUpdate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderFlexUpdate001 start";
    /**
     * @tc.steps: step1. construct Text Component and RenderFlex.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("Hi Ace");
    RefPtr<RenderFlex> renderFlex = AceType::MakeRefPtr<RenderFlex>();

    /**
     * @tc.steps: step2. call the Update interface of RenderFlex
     * @tc.expected: step2. renderFlex are not set need layout
     */
    renderFlex->Update(text);
    EXPECT_TRUE(!renderFlex->NeedLayout());
}

/**
 * @tc.name: RenderFlexUpdate002
 * @tc.desc: Verify that RenderFlex works for flex component.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH4
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderFlexUpdate002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderFlexUpdate002 start";
    /**
     * @tc.steps: step1. construct flex component and RenderFlex.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("Hi Ace");
    FlexAlign main = FlexAlign::FLEX_START;
    FlexAlign cross = FlexAlign::FLEX_START;
    std::list<RefPtr<Component>> child;
    child.emplace_back(text);
    RefPtr<RowComponent> row = AceType::MakeRefPtr<RowComponent>(main, cross, child);
    RefPtr<RenderFlex> renderFlex = AceType::MakeRefPtr<RenderFlex>();

    /**
     * @tc.steps: step2. call Update interface of renderFlex
     * @tc.expected: step2. renderFlex are set needLayout
     */
    renderFlex->Update(row);
    EXPECT_TRUE(renderFlex->NeedLayout());
}

/**
 * @tc.name: RenderRowLayout001
 * @tc.desc: Verify the row component with main(start) and cross(start) works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout001 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as start-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
}

/**
 * @tc.name: RenderRowLayout002
 * @tc.desc: Verify the row component with main(start) and cross(center) works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout002 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as start-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::CENTER);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, CENTER_ALIGN_SIZE + CENTER_ALIGN_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX, CENTER_ALIGN_SIZE));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
}

/**
 * @tc.name: RenderRowLayout003
 * @tc.desc: Verify the row component with main(start) and cross(end) works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout003 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as start-end.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_END);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, END_ALIGN_SIZE + END_ALIGN_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX, END_ALIGN_SIZE));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0));
}

/**
 * @tc.name: RenderRowLayout004
 * @tc.desc: Verify the row component with main(start) and cross(stretch) works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout004 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as start-stretch.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::STRETCH);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly, the size of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, 0.0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX, 0.0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(SMALL_BOX + MEDIUM_BOX, 0.0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SMALL_BOX, LARGE_BOX));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(MEDIUM_BOX, LARGE_BOX));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(LARGE_BOX, LARGE_BOX));
}

/**
 * @tc.name: RenderRowLayout005
 * @tc.desc: Verify the row component with main(center) and cross(start) works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout005 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as center-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::CENTER, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(CENTER_SPACE_SIZE, 0.0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(CENTER_SPACE_SIZE + SMALL_BOX, 0.0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(CENTER_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX, 0.0));
}

/**
 * @tc.name: RenderRowLayout006
 * @tc.desc: Verify the row component with main(end) and cross(start) works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout006 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as end-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_END, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(END_SPACE_SIZE, 0.0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(END_SPACE_SIZE + SMALL_BOX, 0.0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(END_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX, 0));
}

/**
 * @tc.name: RenderRowLayout007
 * @tc.desc: Verify the row component with main(space-between) and cross(start) works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout007 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as space_between-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::SPACE_BETWEEN, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(BETWEEN_SPACE_SIZE + SMALL_BOX, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(BETWEEN_SPACE_SIZE + BETWEEN_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX, 0));
}

/**
 * @tc.name: RenderRowLayout008
 * @tc.desc: Verify the row component with main(space-around) and cross(start) works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout008 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as space_around-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::SPACE_AROUND, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(AROUND_FRONT_SPACE_SIZE, 0.0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(AROUND_FRONT_SPACE_SIZE + AROUND_SPACE_SIZE + SMALL_BOX, 0.0));
    EXPECT_TRUE(thirdBox->GetPosition() ==
                Offset(AROUND_FRONT_SPACE_SIZE + AROUND_SPACE_SIZE + AROUND_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX, 0));
}

/**
 * @tc.name: RenderRowLayout009
 * @tc.desc: Verify the row component with main(center) and cross(center) under row component works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout009 start";
    /**
     * @tc.steps: step1. construct the node tree, row has a child row, child row are set as center-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderFlex> childRow =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::CENTER, FlexAlign::CENTER);
    row->AddChild(childRow);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    childRow->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    childRow->AddChild(secondBox);
    RefPtr<RenderFlex> secondChildRow =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::CENTER, FlexAlign::CENTER);
    row->AddChild(secondChildRow);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    thirdBox->Attach(mockContext);
    secondChildRow->AddChild(thirdBox);
    RefPtr<RenderBox> fourthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    fourthBox->Attach(mockContext);
    secondChildRow->AddChild(fourthBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    childRow->Attach(mockContext);
    secondChildRow->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(childRow->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondChildRow->GetPosition() == Offset(RECT_WIDTH, 0.0));
    EXPECT_TRUE(firstBox->GetPosition() == Offset(ROW_COL_CENTER_SIZE, CENTER_ALIGN_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(ROW_COL_CENTER_SIZE + SMALL_BOX, 0.0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(ROW_COL_SMALL_CENTER_SIZE, CENTER_ALIGN_SIZE));
    EXPECT_TRUE(fourthBox->GetPosition() == Offset(ROW_COL_SMALL_CENTER_SIZE + MEDIUM_BOX, 0.0));
}

/**
 * @tc.name: RenderRowLayout010
 * @tc.desc: Verify the row component with main(center) and cross(center) under column component works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTH AR000DAIH5
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderRowLayout010 start";
    /**
     * @tc.steps: step1. construct the node tree, parent column has a child row, child column are set as center-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> column =
        FlexTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
    root->AddChild(column);
    RefPtr<RenderFlex> childRow =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::CENTER, FlexAlign::CENTER);
    column->AddChild(childRow);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    childRow->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    childRow->AddChild(secondBox);
    RefPtr<RenderFlex> secondChildRow =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::CENTER, FlexAlign::CENTER);
    column->AddChild(secondChildRow);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    thirdBox->Attach(mockContext);
    secondChildRow->AddChild(thirdBox);
    RefPtr<RenderBox> fourthBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    fourthBox->Attach(mockContext);
    secondChildRow->AddChild(fourthBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    column->Attach(mockContext);
    childRow->Attach(mockContext);
    secondChildRow->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(childRow->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondChildRow->GetPosition() == Offset(0.0, MEDIUM_BOX));
    EXPECT_TRUE(firstBox->GetPosition() == Offset(ROW_COL_CENTER_SIZE, CENTER_ALIGN_SIZE));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(ROW_COL_CENTER_SIZE + SMALL_BOX, 0.0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(ROW_COL_SMALL_CENTER_SIZE, CENTER_ALIGN_SIZE));
    EXPECT_TRUE(fourthBox->GetPosition() == Offset(ROW_COL_SMALL_CENTER_SIZE + MEDIUM_BOX, 0.0));
}

/**
 * @tc.name: RenderRowLayout011
 * @tc.desc: Verify the row component with main(space-evenly) and cross(start) works fine.
 * @tc.type: FUNC
 * @tc.require: SR000F3BA7 AR000F3BA9
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderRowLayout011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as space_between-start.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::SPACE_EVENLY, FlexAlign::FLEX_START);
    root->AddChild(row);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    row->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    row->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    row->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(EVENLY_SPACE_SIZE, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(2 * EVENLY_SPACE_SIZE + SMALL_BOX, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(3 * EVENLY_SPACE_SIZE + SMALL_BOX + MEDIUM_BOX, 0));
}


/**
 * @tc.name: RenderBaseline001
 * @tc.desc: Verify the row component with main(start) and cross(baseline) works fine when all the children are text.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTI
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderBaseline001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderBaseline001 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as start-baseline.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::BASELINE);
    root->AddChild(row);
    RefPtr<MockRenderText> firstText = FlexTestUtils::CreateRenderText(SMALL_TEXT);
    row->AddChild(firstText);
    RefPtr<MockRenderText> secondText = FlexTestUtils::CreateRenderText(MEDIUM_TEXT);
    row->AddChild(secondText);
    RefPtr<MockRenderText> thirdText = FlexTestUtils::CreateRenderText(LARGE_TEXT);
    row->AddChild(thirdText);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstText->Attach(mockContext);
    secondText->Attach(mockContext);
    thirdText->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstText->GetPosition() == Offset(0, MEDIUM_BASELINE));
    EXPECT_TRUE(secondText->GetPosition() == Offset(SMALL_TEXT, SMALL_BASELINE));
    EXPECT_TRUE(thirdText->GetPosition() == Offset(SMALL_TEXT + MEDIUM_TEXT, 0));
}

/**
 * @tc.name: RenderBaseline002
 * @tc.desc: Verify the row component with main(start) and cross(baseline) works fine when not all children are text.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTI
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderBaseline002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderBaseline002 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as start-baseline.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::BASELINE);
    root->AddChild(row);
    RefPtr<MockRenderText> firstText = FlexTestUtils::CreateRenderText(SMALL_TEXT);
    row->AddChild(firstText);
    RefPtr<MockRenderText> secondText = FlexTestUtils::CreateRenderText(MEDIUM_TEXT);
    row->AddChild(secondText);
    RefPtr<RenderBox> box = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    box->Attach(mockContext);
    row->AddChild(box);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstText->Attach(mockContext);
    secondText->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstText->GetPosition() == Offset(0, MEDIUM_BOX - SMALL_BASELINE));
    EXPECT_TRUE(secondText->GetPosition() == Offset(SMALL_TEXT, MEDIUM_BOX - MEDIUM_BASELINE));
    EXPECT_TRUE(box->GetPosition() == Offset(SMALL_TEXT + MEDIUM_TEXT, 0));
}

/**
 * @tc.name: RenderBaseline003
 * @tc.desc: Verify the row component with main(start) and cross(baseline) works fine when some children are flexItem.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTI
 * @tc.author: yangfan
 */
HWTEST_F(RenderRowTest, RenderBaseline003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RenderRowTest RenderBaseline003 start";
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as start-baseline.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderFlex> row =
        FlexTestUtils::CreateRenderFlex(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::BASELINE);
    root->AddChild(row);
    RefPtr<MockRenderText> firstText = FlexTestUtils::CreateRenderText(SMALL_TEXT);
    row->AddChild(firstText);
    RefPtr<RenderFlexItem> flexItem = FlexTestUtils::CreateRenderFlexItem(0, 0, 0);
    row->AddChild(flexItem);
    RefPtr<MockRenderText> secondText = FlexTestUtils::CreateRenderText(MEDIUM_TEXT);
    flexItem->AddChild(secondText);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    row->Attach(mockContext);
    firstText->Attach(mockContext);
    flexItem->Attach(mockContext);
    secondText->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstText->GetPosition() == Offset(0, SMALL_BASELINE));
    EXPECT_TRUE(flexItem->GetPosition() == Offset(SMALL_TEXT, 0));
}

} // namespace OHOS::Ace