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

#include "base/json/json_util.h"
#include "core/animation/curves.h"
#include "core/components/flex/flex_component.h"
#include "core/components/scroll/scroll_component.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/scroll/scroll_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

const Size LAYOUT_TEST_SIZE_DEFAULT = Size(0, 0);
const Size LAYOUT_TEST_SIZE_MAX = Size(300, 300);
const Edge SCROLL_TEST_PADDING = Edge(5.0, 10.0, 15.0, 20.0);
const Size SCROLL_TEST_PADDING_EXCEPT = Size(280, 570);
const Offset SCROLL_TEST_PADDING_OFFSET = Offset(5, 10);
const Size SCROLL_TEST_CHILD = Size(300.0, 600.0);
const double SCROLL_CHILD_BOX_WIDTH = 300.0;
const double SCROLL_CHILD_BOX_HEIGHT = 600.0;
const Size PARENT_LAYOUT_TEST_SIZE_MAX = Size(500, 700);
const double PARENT1_BOX_WIDTH = 300.0;
const double PARENT1_BOX_HEIGHT = 300.0;
const double PARENT2_BOX_WIDTH = 400.0;
const double PARENT2_BOX_HEIGHT = 1000.0;
const double PARENT3_BOX_WIDTH = 500.0;
const double PARENT3_BOX_HEIGHT = 700.0;
const double SCROLL_INITIAL_OFFSET = 50.0;
const double SCROLL_JUMP_OFFSET = 100.0;
const double SCROLL_ANIMATE_DURATION = 200.0;

class MockRenderScroll final : public RenderSingleChildScroll {
    DECLARE_ACE_TYPE(MockRenderScroll, RenderSingleChildScroll);

public:
    MockRenderScroll() = default;
    ~MockRenderScroll() override = default;

    Axis GetAxisDirection() const;
    Edge GetPadding() const;
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
};

Axis MockRenderScroll::GetAxisDirection() const
{
    return axis_;
}

Edge MockRenderScroll::GetPadding() const
{
    return padding_;
}

void MockRenderScroll::Update(const RefPtr<Component>& component)
{
    return RenderSingleChildScroll::Update(component);
}

void MockRenderScroll::PerformLayout()
{
    return RenderSingleChildScroll::PerformLayout();
}

class RenderScrollTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void BuildRenderTree();

    RefPtr<PipelineContext> mockContext_;
    RefPtr<MockRenderScroll> renderScroll_;
    RefPtr<MockRenderScroll> renderFocusScroll_;
    RefPtr<RenderBox> renderBox_;
    RefPtr<MockRenderScroll> parentRenderScroll_;
    RefPtr<RenderBox> parentRenderBox_;
    RefPtr<RenderBox> parentRenderBox1_;
    RefPtr<RenderBox> parentRenderBox2_;
};

void RenderScrollTest::BuildRenderTree()
{
    auto renderBox = ScrollTestUtils::CreateRenderBox(800.0, 4000.0);
    RefPtr<RenderFlex> renderFlex =
        ScrollTestUtils::CreateRenderFlex(FlexDirection::COLUMN, FlexAlign::FLEX_START, FlexAlign::STRETCH);

    RefPtr<RenderBox> firstBox = ScrollTestUtils::CreateRenderBox(400.0, 800.0);
    firstBox->Attach(mockContext_);
    renderFlex->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = ScrollTestUtils::CreateRenderBox(400.0, 800.0);
    secondBox->Attach(mockContext_);
    renderFlex->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = ScrollTestUtils::CreateRenderBox(400.0, 800.0);
    thirdBox->Attach(mockContext_);
    renderFlex->AddChild(thirdBox);
    RefPtr<RenderBox> forthBox = ScrollTestUtils::CreateRenderBox(400.0, 800.0);
    forthBox->Attach(mockContext_);
    renderFlex->AddChild(forthBox);
    renderFlex->Attach(mockContext_);
    renderBox->AddChild(renderFlex);
    renderBox->Attach(mockContext_);
    renderFocusScroll_->Attach(mockContext_);
    renderFocusScroll_->AddChild(renderBox);
}

void RenderScrollTest::SetUpTestCase() {}
void RenderScrollTest::TearDownTestCase() {}

void RenderScrollTest::SetUp()
{
    mockContext_ = MockRenderCommon::GetMockContext();
    renderBox_ = ScrollTestUtils::CreateRenderBox(SCROLL_CHILD_BOX_WIDTH, SCROLL_CHILD_BOX_HEIGHT);
    renderBox_->Attach(mockContext_);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(LAYOUT_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(LAYOUT_TEST_SIZE_MAX);

    renderFocusScroll_ = AceType::MakeRefPtr<MockRenderScroll>();
    renderFocusScroll_->Attach(mockContext_);
    renderScroll_ = AceType::MakeRefPtr<MockRenderScroll>();
    renderScroll_->Attach(mockContext_);
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->AddChild(renderBox_);

    parentRenderBox_ = ScrollTestUtils::CreateRenderBox(PARENT1_BOX_WIDTH, PARENT1_BOX_HEIGHT);
    parentRenderBox_->Attach(mockContext_);
    parentRenderBox_->AddChild(renderScroll_);

    parentRenderBox1_ = ScrollTestUtils::CreateRenderBox(PARENT2_BOX_WIDTH, PARENT2_BOX_HEIGHT, false);
    parentRenderBox1_->Attach(mockContext_);
    parentRenderBox1_->AddChild(parentRenderBox_);

    LayoutParam parentLayoutParam;
    parentLayoutParam.SetMinSize(LAYOUT_TEST_SIZE_DEFAULT);
    parentLayoutParam.SetMaxSize(PARENT_LAYOUT_TEST_SIZE_MAX);
    parentRenderScroll_ = AceType::MakeRefPtr<MockRenderScroll>();
    parentRenderScroll_->Attach(mockContext_);
    parentRenderScroll_->SetLayoutParam(parentLayoutParam);
    parentRenderScroll_->AddChild(parentRenderBox1_);

    parentRenderBox2_ = ScrollTestUtils::CreateRenderBox(PARENT3_BOX_WIDTH, PARENT3_BOX_HEIGHT);
    parentRenderBox2_->Attach(mockContext_);
    parentRenderBox2_->AddChild(parentRenderScroll_);
}

void RenderScrollTest::TearDown()
{
    mockContext_ = nullptr;
    renderScroll_ = nullptr;
    renderFocusScroll_ = nullptr;
    parentRenderScroll_ = nullptr;
}

/**
 * @tc.name: RenderScrollTest001
 * @tc.desc: Verify PerformLayout can calculate scroll properties without setting direction.
 * @tc.type: FUNC
 * @tc.require: AR000DAR08 AR000DAR0B
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderScrollTest, RenderScrollTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a box child.
     */
    RefPtr<BoxComponent> box001 = AceType::MakeRefPtr<BoxComponent>();
    box001->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box001->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollComponent> scroll001 = AceType::MakeRefPtr<ScrollComponent>(box001);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    renderScroll_->Update(scroll001);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetAxisDirection() == Axis::VERTICAL);
}

/**
 * @tc.name: RenderScrollTest002
 * @tc.desc: Verify PerformLayout can calculate scroll properties where direction is set to horizontal.
 * @tc.type: FUNC
 * @tc.require: AR000DAR08 AR000DAR0B
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderScrollTest, RenderScrollTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a box child.
     */
    RefPtr<BoxComponent> box002 = AceType::MakeRefPtr<BoxComponent>();
    box002->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box002->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollComponent> scroll002 = AceType::MakeRefPtr<ScrollComponent>(box002);
    scroll002->SetAxisDirection(Axis::HORIZONTAL);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll002->GetAxisDirection() == Axis::HORIZONTAL);
    renderScroll_->Update(scroll002);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetAxisDirection() == Axis::HORIZONTAL);
}

/**
 * @tc.name: RenderScrollTest003
 * @tc.desc: Verify PerformLayout can calculate scroll properties when execute animate to method.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0C
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderScrollTest, RenderScrollTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a box child.
     */
    RefPtr<BoxComponent> box003 = AceType::MakeRefPtr<BoxComponent>();
    box003->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box003->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();

    RefPtr<ScrollComponent> scroll003 = AceType::MakeRefPtr<ScrollComponent>(box003);
    scroll003->SetAxisDirection(Axis::VERTICAL);
    scroll003->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll003->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll003);

    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();

    Size childSize = renderScroll_->GetChildren().front()->GetLayoutSize();
    ASSERT_TRUE(childSize == SCROLL_TEST_CHILD);

    renderScroll_->AnimateTo(SCROLL_JUMP_OFFSET, SCROLL_ANIMATE_DURATION, Curves::LINEAR);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearZero(renderScroll_->GetCurrentPosition()));
}

/**
 * @tc.name: RenderScrollTest004
 * @tc.desc: Verify scroll can jump to fixed offset where axis is set to vertical.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0D
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderScrollTest, RenderScrollTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a box child.
     */
    RefPtr<BoxComponent> box004 = AceType::MakeRefPtr<BoxComponent>();
    box004->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box004->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();

    RefPtr<ScrollComponent> scroll004 = AceType::MakeRefPtr<ScrollComponent>(box004);
    scroll004->SetAxisDirection(Axis::VERTICAL);
    scroll004->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll004->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll004);

    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();

    renderScroll_->JumpToPosition(SCROLL_JUMP_OFFSET);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_JUMP_OFFSET));
}

/**
 * @tc.name: RenderScrollTest005
 * @tc.desc: Verify scroll can jump to fixed offset where axis is set to horizontal.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0D
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderScrollTest, RenderScrollTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a box child.
     */
    RefPtr<BoxComponent> box005 = AceType::MakeRefPtr<BoxComponent>();
    box005->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box005->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();

    RefPtr<ScrollComponent> scroll005 = AceType::MakeRefPtr<ScrollComponent>(box005);
    scroll005->SetAxisDirection(Axis::VERTICAL);
    scroll005->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    renderScroll_->Update(scroll005);

    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();

    renderScroll_->JumpToPosition(SCROLL_JUMP_OFFSET);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_JUMP_OFFSET));
}

/**
 * @tc.name: RenderScrollTest006
 * @tc.desc: Verify PerformLayout can calculate scroll properties with a padding properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0E
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderScrollTest, RenderScrollTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a box child and padding properties.
     */
    RefPtr<BoxComponent> box006 = AceType::MakeRefPtr<BoxComponent>();
    box006->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box006->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();

    RefPtr<ScrollComponent> scroll006 = AceType::MakeRefPtr<ScrollComponent>(box006);
    scroll006->SetAxisDirection(Axis::VERTICAL);
    scroll006->SetScrollPositionController(positionController);
    scroll006->SetPadding(SCROLL_TEST_PADDING);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll006->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll006);

    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetPadding() == SCROLL_TEST_PADDING);
    Size childSize = renderScroll_->GetChildren().front()->GetLayoutSize();
    Offset childPosition = renderScroll_->GetChildren().front()->GetPosition();
    ASSERT_TRUE(childSize == SCROLL_TEST_PADDING_EXCEPT);
    ASSERT_TRUE(childPosition == SCROLL_TEST_PADDING_OFFSET);
}

/**
 * @tc.name: RenderScrollTest007
 * @tc.desc: Verify PerformLayout can calculate scroll properties with a initialOffset and direction is vertical.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0F AR000DAR0G
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderScrollTest, RenderScrollTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box007 = AceType::MakeRefPtr<BoxComponent>();
    box007->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box007->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll007 = AceType::MakeRefPtr<ScrollComponent>(box007);
    scroll007->SetAxisDirection(Axis::VERTICAL);
    scroll007->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll007->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll007);

    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET));
}

/**
 * @tc.name: RenderScrollTest008
 * @tc.desc: Verify PerformLayout can calculate scroll properties with a initialOffset and direction is horizontal.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0F AR000DAR0G
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderScrollTest, RenderScrollTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box008 = AceType::MakeRefPtr<BoxComponent>();
    box008->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box008->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll008 = AceType::MakeRefPtr<ScrollComponent>(box008);
    scroll008->SetAxisDirection(Axis::HORIZONTAL);
    scroll008->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    renderScroll_->Update(scroll008);

    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), 0.0));
}

/**
 * @tc.name: RenderScrollTest009
 * @tc.desc: Verify PerformLayout can calculate scroll and update offset.
 * @tc.type: FUNC
 * @tc.require: AR000DAR09
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderScrollTest, RenderScrollTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box009 = AceType::MakeRefPtr<BoxComponent>();
    box009->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box009->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);
    RefPtr<ScrollComponent> scroll009 = AceType::MakeRefPtr<ScrollComponent>(box009);
    scroll009->SetAxisDirection(Axis::VERTICAL);
    scroll009->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll009->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll009);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET));

    /**
     * @tc.steps: step3. Trigger a slide event with offset(100.0, 100.0) and offset(-10.0, -10.0)
     * @tc.expected: step3. Current Position will offset 100.0 and -10.0.
     */
    Offset offset1 = Offset(100.0, 100.0);
    renderScroll_->UpdateOffset(offset1, SCROLL_FROM_NONE);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET + 100.0));
    Offset offset2 = Offset(-10.0, -10.0);
    renderScroll_->UpdateOffset(offset2, SCROLL_FROM_NONE);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET + 90.0));
}

/**
 * @tc.name: RenderScrollScrollpageChangeTest001
 * @tc.desc: Verify Scroll UpdateOffset When scrollpage is true.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SR
 * @tc.author: chenlien
 */
HWTEST_F(RenderScrollTest, RenderScrollScrollpageChangeTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box001 = ScrollTestUtils::CreateBoxComponent(SCROLL_CHILD_BOX_WIDTH, SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);
    RefPtr<ScrollComponent> scroll001 = AceType::MakeRefPtr<ScrollComponent>(box001);
    scroll001->SetAxisDirection(Axis::VERTICAL);
    scroll001->SetScrollPositionController(positionController);
    scroll001->SetScrollPage(true);
    RefPtr<BoxComponent> box002 = ScrollTestUtils::CreateBoxComponent(PARENT1_BOX_WIDTH, PARENT1_BOX_HEIGHT);
    box002->SetChild(scroll001);
    RefPtr<BoxComponent> box003 = ScrollTestUtils::CreateBoxComponent(PARENT2_BOX_WIDTH, PARENT2_BOX_HEIGHT, false);
    box003->SetChild(box002);
    RefPtr<ScrollPositionController> parentPositionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);
    RefPtr<ScrollComponent> scroll002 = AceType::MakeRefPtr<ScrollComponent>(box003);
    scroll002->SetAxisDirection(Axis::VERTICAL);
    scroll002->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll002->GetAxisDirection() == Axis::VERTICAL);
    parentRenderScroll_->Update(scroll002);
    EXPECT_TRUE(parentRenderScroll_->NeedLayout());
    parentRenderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(parentRenderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET));
    ASSERT_TRUE(scroll001->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll001);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET));
    parentRenderBox2_->PerformLayout();

    /**
     * @tc.steps: step3. Trigger a slide event with offset(100.0, 100.0)
     * @tc.expected: step3. child scroll Current Position would not change, and parent Would offset 100.0.
     */
    Offset offset1 = Offset(100.0, 100.0);
    parentRenderScroll_->UpdateOffset(offset1, SCROLL_FROM_NONE);
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET));
    ASSERT_TRUE(NearEqual(parentRenderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET + 100.0));
}

/**
 * @tc.name: RenderScrollScrollpageChangeTest002
 * @tc.desc: Verify Scroll UpdateOffset When scrollpage is true.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SR
 * @tc.author: chenlien
 */
HWTEST_F(RenderScrollTest, RenderScrollScrollpageChangeTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box001 = ScrollTestUtils::CreateBoxComponent(SCROLL_CHILD_BOX_WIDTH, SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);
    RefPtr<ScrollComponent> scroll001 = AceType::MakeRefPtr<ScrollComponent>(box001);
    scroll001->SetAxisDirection(Axis::VERTICAL);
    scroll001->SetScrollPositionController(positionController);
    scroll001->SetScrollPage(true);
    RefPtr<BoxComponent> box002 = ScrollTestUtils::CreateBoxComponent(PARENT1_BOX_WIDTH, PARENT1_BOX_HEIGHT);
    box002->SetChild(scroll001);
    RefPtr<BoxComponent> box003 = ScrollTestUtils::CreateBoxComponent(PARENT2_BOX_WIDTH, PARENT2_BOX_HEIGHT, false);
    box003->SetChild(box002);
    RefPtr<ScrollPositionController> parentPositionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);
    RefPtr<ScrollComponent> scroll002 = AceType::MakeRefPtr<ScrollComponent>(box003);
    scroll002->SetAxisDirection(Axis::VERTICAL);
    scroll002->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll002->GetAxisDirection() == Axis::VERTICAL);
    parentRenderScroll_->Update(scroll002);
    EXPECT_TRUE(parentRenderScroll_->NeedLayout());
    parentRenderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(parentRenderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET));
    ASSERT_TRUE(scroll001->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll001);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET));
    parentRenderBox2_->PerformLayout();

    /**
     * @tc.steps: step3. Trigger a slide event with offset(-10.0, -10.0)
     * @tc.expected: step3. child scroll Current Position would offset -10.0, and parent scroll would not change
     */
    Offset offset1 = Offset(-10.0, -10.0);
    renderScroll_->UpdateOffset(offset1, SCROLL_FROM_NONE);
    ASSERT_TRUE(NearEqual(renderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET - 10.0));
    ASSERT_TRUE(NearEqual(parentRenderScroll_->GetCurrentPosition(), SCROLL_INITIAL_OFFSET));
}

/**
 * @tc.name: RenderScrollFocusTest001
 * @tc.desc: Verify scroll support move child into viewport.
 * @tc.type: FUNC
 * @tc.require: AR000DRS0L
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderScrollTest, RenderScrollFocusTest001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. construct the scroll component with a initialOffset.
    */
    RefPtr<BoxComponent> focusBox001 = AceType::MakeRefPtr<BoxComponent>();
    focusBox001->SetWidth(400.0);
    focusBox001->SetHeight(800.0);
    RefPtr<ScrollComponent> focusScroll001 = AceType::MakeRefPtr<ScrollComponent>(focusBox001);
    renderFocusScroll_->Update(focusScroll001);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(800.0, 1000.0));
    BuildRenderTree();
    renderFocusScroll_->SetLayoutParam(layoutParam);
    renderFocusScroll_->PerformLayout();

    /**
    * @tc.steps: step2. Verify that the properties are calculated correctly.
    * @tc.expected: step2. Properties are calculated correctly.
    */
    auto renderBox = AceType::DynamicCast<RenderBox>(renderFocusScroll_->GetChildren().front());
    ASSERT_TRUE(renderBox);

    auto viewRect = renderFocusScroll_->GetPaintRect();
    auto renderScroll1 = AceType::DynamicCast<RenderScroll>(renderFocusScroll_);
    auto renderFlex = AceType::DynamicCast<RenderFlex>(renderBox->GetChildren().front());
    ASSERT_TRUE(renderFlex);

    // get last child posotion and rect size
    auto lastChild = renderFlex->GetChildren().back();
    if (lastChild) {
        auto offset = lastChild->GetGlobalOffset();
        auto rect = lastChild->GetPaintRect();
        auto size = lastChild->GetLayoutSize();
        ASSERT_FALSE(rect.IsWrappedBy(viewRect));

        renderFocusScroll_->MoveChildToViewPort(size, offset, Offset());
        renderFocusScroll_->PerformLayout();
        rect = lastChild->GetPaintRect();
        Offset boxPostion = renderBox->GetPosition();
        auto lastChildRect = Rect(rect.GetOffset() + boxPostion, rect.GetSize());
        ASSERT_TRUE(lastChildRect.IsWrappedBy(viewRect));
    }
}

/**
 * @tc.name: RenderScrollScrollerTest001
 * @tc.desc: Verify renderScroll can get currentOffset.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UT
 * @tc.author: chenlien
 */
HWTEST_F(RenderScrollTest, RenderScrollScrollerTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box009 = AceType::MakeRefPtr<BoxComponent>();
    box009->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box009->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll009 = AceType::MakeRefPtr<ScrollComponent>(box009);
    scroll009->SetAxisDirection(Axis::VERTICAL);
    scroll009->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll009->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll009);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(positionController->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET));

    /**
     * @tc.steps: step3. Trigger a slide event with offset(100.0, 100.0)
     * @tc.expected: step3. Current Offset will be (0.0, 150.0).
     */
    Offset offset1 = Offset(100.0, 100.0);
    renderScroll_->UpdateOffset(offset1, SCROLL_FROM_NONE);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(positionController->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET + 100.0));
}

/**
 * @tc.name: RenderScrollScrollerTest002
 * @tc.desc: Verify renderScroll can scroll up one page.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UU
 * @tc.author: chenlien
 */
HWTEST_F(RenderScrollTest, RenderScrollScrollerTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box009 = AceType::MakeRefPtr<BoxComponent>();
    box009->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box009->SetHeight(800.0);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll009 = AceType::MakeRefPtr<ScrollComponent>(box009);
    scroll009->SetAxisDirection(Axis::VERTICAL);
    scroll009->SetScrollPositionController(positionController);

    renderBox_ = ScrollTestUtils::CreateRenderBox(SCROLL_CHILD_BOX_WIDTH, 800.0);
    renderBox_->Attach(mockContext_);
    LayoutParam layoutParam;
    layoutParam.SetMinSize(LAYOUT_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(LAYOUT_TEST_SIZE_MAX);
    renderScroll_ = AceType::MakeRefPtr<MockRenderScroll>();
    renderScroll_->Attach(mockContext_);
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->AddChild(renderBox_);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll009->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll009);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET));

    /**
     * @tc.steps: step3. Scroll up on page
     * @tc.expected: step3. Current Offset will be (0.0, 350.0).
     */
    renderScroll_->ScrollPage(false, false);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetViewPort().Height() == 300.0);
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, 350.0));
}

/**
 * @tc.name: RenderScrollScrollerTest003
 * @tc.desc: Verify renderScroll can scroll down one page.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UU
 * @tc.author: chenlien
 */
HWTEST_F(RenderScrollTest, RenderScrollScrollerTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box009 = AceType::MakeRefPtr<BoxComponent>();
    box009->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box009->SetHeight(800.0);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll009 = AceType::MakeRefPtr<ScrollComponent>(box009);
    scroll009->SetAxisDirection(Axis::VERTICAL);
    scroll009->SetScrollPositionController(positionController);

    renderBox_ = ScrollTestUtils::CreateRenderBox(SCROLL_CHILD_BOX_WIDTH, 800.0);
    renderBox_->Attach(mockContext_);
    LayoutParam layoutParam;
    layoutParam.SetMinSize(LAYOUT_TEST_SIZE_DEFAULT);
    layoutParam.SetMaxSize(LAYOUT_TEST_SIZE_MAX);
    renderScroll_ = AceType::MakeRefPtr<MockRenderScroll>();
    renderScroll_->Attach(mockContext_);
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->AddChild(renderBox_);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll009->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll009);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET));

    /**
     * @tc.steps: step3. Scroll to bottom and then Scroll up on page
     * @tc.expected: step3. Current Offset will be (0.0, 200.0).
     */
    renderScroll_->ScrollToEdge(ScrollEdgeType::SCROLL_BOTTOM, false);
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, 500.0));
    renderScroll_->ScrollPage(true, false);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetViewPort().Height() == 300.0);
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, 200.0));
}


/**
 * @tc.name: RenderScrollScrollerTest004
 * @tc.desc: Verify renderScroll can scroll to top.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UV
 * @tc.author: chenlien
 */
HWTEST_F(RenderScrollTest, RenderScrollScrollerTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box009 = AceType::MakeRefPtr<BoxComponent>();
    box009->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box009->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll009 = AceType::MakeRefPtr<ScrollComponent>(box009);
    scroll009->SetAxisDirection(Axis::VERTICAL);
    scroll009->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll009->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll009);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET));

    /**
     * @tc.steps: step3. Scroll to top
     * @tc.expected: step3. Current Offset will be (0.0, 0.0).
     */
    renderScroll_->ScrollToEdge(ScrollEdgeType::SCROLL_TOP, false);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, 0.0));
}

/**
 * @tc.name: RenderScrollScrollerTest005
 * @tc.desc: Verify renderScroll can scroll to bottom.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UV
 * @tc.author: chenlien
 */
HWTEST_F(RenderScrollTest, RenderScrollScrollerTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> box009 = AceType::MakeRefPtr<BoxComponent>();
    box009->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    box009->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll009 = AceType::MakeRefPtr<ScrollComponent>(box009);
    scroll009->SetAxisDirection(Axis::VERTICAL);
    scroll009->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll009->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll009);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET));

    /**
     * @tc.steps: step3. Scroll to bottom
     * @tc.expected: step3. Current Offset will be (0.0, 300.0).
     */
    renderScroll_->ScrollToEdge(ScrollEdgeType::SCROLL_BOTTOM, false);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, 300.0));
}

/**
 * @tc.name: RenderScrollRotateTest001
 * @tc.desc: Verify renderScroll by rotate.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T4
 * @tc.author: liujinwei
 */
HWTEST_F(RenderScrollTest, RenderScrollRotateTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> rotatebox001 = AceType::MakeRefPtr<BoxComponent>();
    rotatebox001->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    rotatebox001->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll = AceType::MakeRefPtr<ScrollComponent>(rotatebox001);
    scroll->SetAxisDirection(Axis::VERTICAL);
    scroll->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET));

    /**
     * @tc.steps: step3. rotate 10
     * @tc.expected: step3. Current Offset will be (0.0, SCROLL_INITIAL_OFFSET + 10).
     */
    renderScroll_->HandleRotate(10, true);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET - 10));

    /**
     * @tc.steps: step4. rotate 10
     * @tc.expected: step4. Current Offset will be (0.0, SCROLL_INITIAL_OFFSET + 20).
     */
    renderScroll_->HandleRotate(10, true);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET - 20));
}

/**
 * @tc.name: RenderScrollRotateTest002
 * @tc.desc: Verify renderScroll by rotate.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T4
 * @tc.author: liujinwei
 */
HWTEST_F(RenderScrollTest, RenderScrollRotateTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> rotatebox002 = AceType::MakeRefPtr<BoxComponent>();
    rotatebox002->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    rotatebox002->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll = AceType::MakeRefPtr<ScrollComponent>(rotatebox002);
    scroll->SetAxisDirection(Axis::VERTICAL);
    scroll->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET));

    /**
     * @tc.steps: step3. rotate -10
     * @tc.expected: step3. Current Offset will be (0.0, SCROLL_INITIAL_OFFSET - 10).
     */
    renderScroll_->HandleRotate(10, true);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET - 10));

    /**
     * @tc.steps: step4. rotate -10
     * @tc.expected: step4. Current Offset will be (0.0, SCROLL_INITIAL_OFFSET - 20), no more moving.
     */
    renderScroll_->HandleRotate(10, true);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET - 20));

    /**
    * @tc.steps: step5. rotate -30
    * @tc.expected: step5. Current Offset will be (0.0, SCROLL_INITIAL_OFFSET - 50).
    */
    renderScroll_->HandleRotate(30, true);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET - 50));

    /**
    * @tc.steps: step5. rotate -10
    * @tc.expected: step5. Current Offset will be (0.0, 0.0), no more moving.
    */
    renderScroll_->HandleRotate(10, true);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, 0.0));
}

/**
 * @tc.name: RenderScrollRotateTest003
 * @tc.desc: Verify renderScroll by rotate.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T4
 * @tc.author: liujinwei
 */
HWTEST_F(RenderScrollTest, RenderScrollRotateTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the scroll component with a initialOffset.
     */
    RefPtr<BoxComponent> rotatebox003 = AceType::MakeRefPtr<BoxComponent>();
    rotatebox003->SetWidth(SCROLL_CHILD_BOX_WIDTH);
    rotatebox003->SetHeight(SCROLL_CHILD_BOX_HEIGHT);
    RefPtr<ScrollPositionController> positionController = AceType::MakeRefPtr<ScrollPositionController>();
    positionController->SetInitialOffset(SCROLL_INITIAL_OFFSET);

    RefPtr<ScrollComponent> scroll = AceType::MakeRefPtr<ScrollComponent>(rotatebox003);
    scroll->SetAxisDirection(Axis::VERTICAL);
    scroll->SetScrollPositionController(positionController);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties are calculated correctly.
     */
    ASSERT_TRUE(scroll->GetAxisDirection() == Axis::VERTICAL);
    renderScroll_->Update(scroll);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET));

    /**
     * @tc.steps: step3. rotate 180
     * @tc.expected: step3. Current Offset will be (0.0, SCROLL_INITIAL_OFFSET + 180).
     */
    renderScroll_->HandleRotate(-180, true);
    renderScroll_->PerformLayout();
    GTEST_LOG_(INFO) <<renderScroll_->GetCurrentOffset().GetY();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, SCROLL_INITIAL_OFFSET + 180));

    /**
    * @tc.steps: step4. rotate 180
    * @tc.expected: step4. Current Offset will be (0.0, 300).
    */
    renderScroll_->HandleRotate(-180, true);
    renderScroll_->PerformLayout();
    GTEST_LOG_(INFO) <<renderScroll_->GetCurrentOffset().GetY();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, 300.0));

    /**
     * @tc.steps: step5. rotate -180
     * @tc.expected: step5. Current Offset will be (0.0, 300 - 180).
     */
    renderScroll_->HandleRotate(180, true);
    renderScroll_->PerformLayout();
    ASSERT_TRUE(renderScroll_->GetCurrentOffset() == Offset(0.0, 120.0));
}

} // namespace OHOS::Ace
