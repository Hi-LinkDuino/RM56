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
#include "core/components/box/box_component.h"
#include "core/components/positioned/positioned_component.h"
#include "core/components/stack/stack_component.h"
#include "core/components/test/unittest/flex/flex_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/stack/stack_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double TEST_HORIZONTAL_VALUE = 40.0;
constexpr double TEST_VERTICAL_VALUE = 40.0;
constexpr double TEST_WIDTH_VALUE = 50.0;
constexpr double TEST_HEIGHT_VALUE = 50.0;
constexpr double SMALL_BOX = 100.0;

} // namespace

class RenderPositionedTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "RenderPositionedTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "RenderPositionedTest TearDownTestCase";
    }

    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: HorizontalLayout001
 * @tc.desc: verify the stack with positioned children works fine, where the positioned are set left.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(RenderPositionedTest, HorizontalLayout001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the render node tree, set left property to the positioned item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack = StackTestUtils::CreateRenderStack(Alignment::TOP_LEFT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<PositionedComponent> positionedComponent = AceType::MakeRefPtr<PositionedComponent>(nullptr);
    positionedComponent->SetLeft(Dimension(TEST_HORIZONTAL_VALUE, DimensionUnit::PX));
    RefPtr<RenderPositioned> renderPositioned = StackTestUtils::CreateRenderPositioned(positionedComponent);
    stack->AddChild(renderPositioned);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    renderPositioned->AddChild(firstBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the position of the positioned items are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    renderPositioned->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(renderPositioned->GetPosition() == Offset(TEST_HORIZONTAL_VALUE, 0));
}

/**
 * @tc.name: HorizontalLayout002
 * @tc.desc: verify the stack with positioned children works fine, where the positioned are both set left and right.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(RenderPositionedTest, HorizontalLayout002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the render node tree, set left property to the positioned item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack = StackTestUtils::CreateRenderStack(Alignment::TOP_LEFT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<PositionedComponent> positioned = AceType::MakeRefPtr<PositionedComponent>(nullptr);
    positioned->SetLeft(Dimension(TEST_HORIZONTAL_VALUE, DimensionUnit::PX));
    positioned->SetRight(Dimension(TEST_HORIZONTAL_VALUE, DimensionUnit::PX));
    RefPtr<RenderPositioned> renderPositioned = StackTestUtils::CreateRenderPositioned(positioned);
    stack->AddChild(renderPositioned);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    renderPositioned->AddChild(firstBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the position of the positioned items are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    renderPositioned->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(renderPositioned->GetPosition() == Offset(TEST_HORIZONTAL_VALUE, 0));
}

/**
 * @tc.name: HorizontalLayout003
 * @tc.desc: verify the stack with positioned children works fine, where the positioned are set width.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(RenderPositionedTest, HorizontalLayout003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the render node tree, set width property to the positioned item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack = StackTestUtils::CreateRenderStack(Alignment::TOP_LEFT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<PositionedComponent> positioned = AceType::MakeRefPtr<PositionedComponent>(nullptr);
    positioned->SetWidth(TEST_WIDTH_VALUE);
    RefPtr<RenderPositioned> renderPositioned = StackTestUtils::CreateRenderPositioned(positioned);
    stack->AddChild(renderPositioned);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    renderPositioned->AddChild(firstBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the position of the positioned items are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    renderPositioned->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(renderPositioned->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(renderPositioned->GetLayoutSize() == Size(TEST_WIDTH_VALUE, SMALL_BOX));
}

/**
 * @tc.name: VerticalLayout001
 * @tc.desc: verify the stack with positioned children works fine, where the positioned are set top.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(RenderPositionedTest, VerticalLayout001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the render node tree, set top property to the positioned item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack = StackTestUtils::CreateRenderStack(Alignment::TOP_LEFT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<PositionedComponent> positionedComponent = AceType::MakeRefPtr<PositionedComponent>(nullptr);
    positionedComponent->SetTop(Dimension(TEST_VERTICAL_VALUE, DimensionUnit::PX));
    RefPtr<RenderPositioned> renderPositioned = StackTestUtils::CreateRenderPositioned(positionedComponent);
    stack->AddChild(renderPositioned);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    renderPositioned->AddChild(firstBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the position of the positioned items are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    renderPositioned->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(renderPositioned->GetPosition() == Offset(0, TEST_VERTICAL_VALUE));
}

/**
 * @tc.name: VerticalLayout002
 * @tc.desc: verify the stack with positioned children works fine, where the positioned are both set top and bottom.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(RenderPositionedTest, VerticalLayout002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the render node tree, set top and bottom property to the positioned item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack = StackTestUtils::CreateRenderStack(Alignment::TOP_LEFT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<PositionedComponent> positioned = AceType::MakeRefPtr<PositionedComponent>(nullptr);
    positioned->SetTop(Dimension(TEST_VERTICAL_VALUE, DimensionUnit::PX));
    positioned->SetBottom(Dimension(TEST_VERTICAL_VALUE, DimensionUnit::PX));
    RefPtr<RenderPositioned> renderPositioned = StackTestUtils::CreateRenderPositioned(positioned);
    stack->AddChild(renderPositioned);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    renderPositioned->AddChild(firstBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the position of the positioned items are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    renderPositioned->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(renderPositioned->GetPosition() == Offset(0, TEST_VERTICAL_VALUE));
}

/**
 * @tc.name: VerticalLayout003
 * @tc.desc: verify the stack with positioned children works fine, where the positioned are set height.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(RenderPositionedTest, VerticalLayout003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the render node tree, set height property to the positioned item
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack = StackTestUtils::CreateRenderStack(Alignment::TOP_LEFT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<PositionedComponent> positioned = AceType::MakeRefPtr<PositionedComponent>(nullptr);
    positioned->SetHeight(TEST_HEIGHT_VALUE);
    RefPtr<RenderPositioned> renderPositioned = StackTestUtils::CreateRenderPositioned(positioned);
    stack->AddChild(renderPositioned);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    renderPositioned->AddChild(firstBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the position of the positioned items are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    renderPositioned->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(renderPositioned->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(renderPositioned->GetLayoutSize() == Size(SMALL_BOX, TEST_HEIGHT_VALUE));
}

} // namespace OHOS::Ace
