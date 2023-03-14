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
#include "core/components/test/unittest/flex/flex_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/stack/stack_test_utils.h"

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

class RenderStackTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "RenderStackTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "RenderStackTest TearDownTestCase";
    }

    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: RenderStackUpdate001
 * @tc.desc: Verify the Update Interface of RenderStack does not work for other components
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN AR000DAQTO AR000DAQTP
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackUpdate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct Box Component and RenderStack.
     */
    RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
    RefPtr<RenderStack> renderStack = AceType::MakeRefPtr<MockRenderStack>();
    /**
     * @tc.steps: step2. call the Update interface of RenderFlex
     * @tc.expected: step2. renderFlex are not set need layout
     */
    renderStack->Update(box);
    EXPECT_TRUE(!renderStack->NeedLayout());
}

/**
 * @tc.name: RenderStackUpdate002
 * @tc.desc: Verify that RenderStack works for stack component.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN AR000DAQTO AR000DAQTP
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackUpdate002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct Stack component and RenderStack.
     */
    RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
    Alignment alignment = Alignment::TOP_LEFT;
    Overflow overflow = Overflow::CLIP;
    StackFit stackFit = StackFit::KEEP;
    std::list<RefPtr<Component>> child;
    child.emplace_back(box);
    RefPtr<StackComponent> stack = AceType::MakeRefPtr<StackComponent>(alignment, stackFit, overflow, child);
    RefPtr<RenderStack> renderStack = AceType::MakeRefPtr<MockRenderStack>();
    /**
     * @tc.steps: step2. call Update interface of renderStack
     * @tc.expected: step2. renderStack are set needLayout
     */
    renderStack->Update(stack);
    EXPECT_TRUE(renderStack->NeedLayout());
}

/**
 * @tc.name: RenderStackLayout001
 * @tc.desc: Verify the stack with top-left alignment works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, stack are set as top-left, clip and keep.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack = StackTestUtils::CreateRenderStack(Alignment::TOP_LEFT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 0));
}

/**
 * @tc.name: RenderStackLayout002
 * @tc.desc: Verify the stack component with top-center works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, stack are set as top-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack =
        StackTestUtils::CreateRenderStack(Alignment::TOP_CENTER, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(SMALL_BOX, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX / 2, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 0));
}

/**
 * @tc.name: RenderStackLayout003
 * @tc.desc: Verify the stack component with top-right works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, stack are set as top-right.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack = StackTestUtils::CreateRenderStack(Alignment::TOP_RIGHT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(MEDIUM_BOX, 0.0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX, 0.0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 0));
}

/**
 * @tc.name: RenderStackLayout004
 * @tc.desc: Verify the stack component with center-left works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, flex are set as center-left.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack =
        StackTestUtils::CreateRenderStack(Alignment::CENTER_LEFT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, SMALL_BOX / 2));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 0));
}

/**
 * @tc.name: RenderStackLayout005
 * @tc.desc: Verify the stack component with center-center works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, stack are set as center-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack = StackTestUtils::CreateRenderStack(Alignment::CENTER, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX / 2, SMALL_BOX / 2));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 0));
}

/**
 * @tc.name: RenderStackLayout006
 * @tc.desc: Verify the stack component with center-right works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, stack are set as center-right.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack =
        StackTestUtils::CreateRenderStack(Alignment::CENTER_RIGHT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(MEDIUM_BOX, SMALL_BOX));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX, SMALL_BOX / 2));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 0));
}

/**
 * @tc.name: RenderStackLayout007
 * @tc.desc: Verify the stack component with bottom-left works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, stack are set as bottom-left.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack =
        StackTestUtils::CreateRenderStack(Alignment::BOTTOM_LEFT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, MEDIUM_BOX));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 0));
}

/**
 * @tc.name: RenderStackLayout008
 * @tc.desc: Verify the stack component with bottom-center works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RenderNode tree, stack are set as bottom-center.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack =
        StackTestUtils::CreateRenderStack(Alignment::BOTTOM_CENTER, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(SMALL_BOX, MEDIUM_BOX));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX / 2, SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 0));
}

/**
 * @tc.name: RenderStackLayout009
 * @tc.desc: Verify the stack component with bottom-right works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the node tree, stack are set as bottom-right.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack =
        StackTestUtils::CreateRenderStack(Alignment::BOTTOM_RIGHT, Overflow::CLIP, StackFit::KEEP);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(MEDIUM_BOX, MEDIUM_BOX));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SMALL_BOX, SMALL_BOX));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, 0));
}

/**
 * @tc.name: RenderStackLayout010
 * @tc.desc: Verify the stack component with stretch stackfit works fine.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTO
 * @tc.author: yangfan
 */
HWTEST_F(RenderStackTest, RenderStackLayout010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the node tree, stack is set stretch.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> root = FlexTestUtils::CreateRenderRoot();
    RefPtr<RenderStack> stack =
        StackTestUtils::CreateRenderStack(Alignment::TOP_LEFT, Overflow::CLIP, StackFit::STRETCH);
    root->AddChild(stack);
    RefPtr<RenderBox> firstBox = FlexTestUtils::CreateRenderBox(SMALL_BOX, SMALL_BOX);
    firstBox->Attach(mockContext);
    stack->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = FlexTestUtils::CreateRenderBox(MEDIUM_BOX, MEDIUM_BOX);
    secondBox->Attach(mockContext);
    stack->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = FlexTestUtils::CreateRenderBox(LARGE_BOX, LARGE_BOX);
    thirdBox->Attach(mockContext);
    stack->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     * @tc.expected: step2. the positions of three boxes are set correctly
     */
    root->Attach(mockContext);
    stack->Attach(mockContext);
    root->PerformLayout();
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0.0, 0.0));
    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(RECT_WIDTH, RECT_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(RECT_WIDTH, RECT_HEIGHT));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(RECT_WIDTH, RECT_HEIGHT));
}

} // namespace OHOS::Ace