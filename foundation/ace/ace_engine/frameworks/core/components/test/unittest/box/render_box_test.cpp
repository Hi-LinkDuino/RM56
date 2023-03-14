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

#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/test/unittest/box/box_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const double BOX_WIDTH = 1000.0;
const double BOX_HEIGHT = 1000.0;
const double ITEM_WIDTH = 100.0;
const double ITEM_HEIGHT = 100.0;

const Offset OFFSET_TOP_LEFT = Offset(0.0, 0.0);
const Offset OFFSET_TOP_CENTER = Offset(450.0, 0.0);
const Offset OFFSET_TOP_RIGHT = Offset(900.0, 0.0);
const Offset OFFSET_CENTER_LEFT = Offset(0.0, 450.0);
const Offset OFFSET_CENTER = Offset(450.0, 450.0);
const Offset OFFSET_CENTER_RIGHT = Offset(900.0, 450.0);
const Offset OFFSET_BOTTOM_LEFT = Offset(0.0, 900.0);
const Offset OFFSET_BOTTOM_CENTER = Offset(450.0, 900.0);
const Offset OFFSET_BOTTOM_RIGHT = Offset(900.0, 900.0);

const Size ZERO_SIZE = Size(0.0, 0.0);
const Size SMALL_SIZE = Size(50.0, 50.0);
const Size MIDDLE_SIZE = Size(100.0, 100.0);
const Size LARGE_SIZE = Size(200.0, 200.0);
const Size HUGE_SIZE = Size(500.0, 500.0);

const Color SHADOW_COLOR = Color(0xffff0000);
const double SHADOW_RADIUS = 10.0;
const Offset SHADOW_OFFSET = Offset(1.0, 1.0);
const Border BACK_BORDER = Border(BorderEdge(Color(0xff000000), Dimension(1.0, DimensionUnit::PX), BorderStyle::NONE));

} // namespace

class RenderBoxTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RenderBoxTest::SetUpTestCase() {}
void RenderBoxTest::TearDownTestCase() {}
void RenderBoxTest::SetUp() {}
void RenderBoxTest::TearDown() {}

/**
 * @tc.name: RenderBoxTest001
 * @tc.desc: Verify Box PerformLayout when align TOP_LEFT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::TOP_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::TOP_LEFT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBox->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest002
 * @tc.desc: Verify Box PerformLayout when align TOP_CENTER.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::TOP_CENTER.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::TOP_CENTER);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBox->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest003
 * @tc.desc: Verify Box PerformLayout when align TOP_RIGHT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::TOP_RIGHT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::TOP_RIGHT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBox->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_RIGHT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest004
 * @tc.desc: Verify Box PerformLayout when align CENTER_LEFT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::CENTER_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::CENTER_LEFT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBox->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest005
 * @tc.desc: Verify Box PerformLayout when align CENTER.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::CENTER.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::CENTER);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBox->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest006
 * @tc.desc: Verify Box PerformLayout when align CENTER_RIGHT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::CENTER_RIGHT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::CENTER_RIGHT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBox->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER_RIGHT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest007
 * @tc.desc: Verify Box PerformLayout when align BOTTOM_LEFT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::BOTTOM_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::BOTTOM_LEFT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBox->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_BOTTOM_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest008
 * @tc.desc: Verify Box PerformLayout when align BOTTOM_CENTER.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::BOTTOM_CENTER.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::BOTTOM_CENTER);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBox->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_BOTTOM_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest009
 * @tc.desc: Verify Box PerformLayout when align BOTTOM_RIGHT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::BOTTOM_RIGHT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetAlignment(Alignment::BOTTOM_RIGHT);
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBox->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_BOTTOM_RIGHT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest010
 * @tc.desc: Verify Box PerformLayout when same margin.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with same margin.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);
    boxComponent->SetMargin(Edge(10.0));

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH + 20.0, BOX_HEIGHT + 20.0));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER + Offset(10.0, 10.0));
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest011
 * @tc.desc: Verify Box PerformLayout when diff margin.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with diff margin.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);
    boxComponent->SetMargin(Edge(10.0, 20.0, 30.0, 40.0));

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH + 40.0, BOX_HEIGHT + 60.0));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER + Offset(10.0, 20.0));
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest012
 * @tc.desc: Verify Box PerformLayout when invalid margin.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with invalid margin.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);
    boxComponent->SetMargin(Edge(-10.0, -20.0, -30.0, -40.0));

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_FALSE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_FALSE(child->GetPosition() == OFFSET_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest013
 * @tc.desc: Verify Box PerformLayout when same padding.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with same padding.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);
    boxComponent->SetPadding(Edge(10.0));

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest014
 * @tc.desc: Verify Box PerformLayout when diff padding.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with diff padding.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);
    boxComponent->SetPadding(Edge(10.0, 20.0, 30.0, 40.0));

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER - Offset(10.0, 10.0));
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest015
 * @tc.desc: Verify Box PerformLayout when invalid padding.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with invalid padding.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);
    boxComponent->SetPadding(Edge(-10.0, -20.0, -30.0, -40.0));

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest016
 * @tc.desc: Verify Box PerformLayout when valid width and height.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with valid width and height.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest017
 * @tc.desc: Verify Box PerformLayout when invalid width and valid height.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with invalid width and valid height.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(-BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(LENGTH, BOX_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest018
 * @tc.desc: Verify Box PerformLayout when valid width and invalid height.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with valid width and invalid height.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(-BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(BOX_WIDTH, LENGTH));
}

/**
 * @tc.name: RenderBoxTest019
 * @tc.desc: Verify Box PerformLayout when invalid width and height.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with invalid width and height.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(-BOX_WIDTH);
    boxComponent->SetHeight(-BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == Size(LENGTH, LENGTH));
}

/**
 * @tc.name: RenderBoxTest020
 * @tc.desc: Verify Box PerformLayout when Constraints(min < max < item).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest020, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Constraints(min < max < item).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(ZERO_SIZE);
    constraints.SetMaxSize(SMALL_SIZE);
    boxComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == SMALL_SIZE);
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == SMALL_SIZE);
}

/**
 * @tc.name: RenderBoxTest021
 * @tc.desc: Verify Box PerformLayout when Constraints(item < min < max).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest021, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Constraints(item < min < max).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(LARGE_SIZE);
    constraints.SetMaxSize(HUGE_SIZE);
    boxComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == LARGE_SIZE);
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == LARGE_SIZE);
}

/**
 * @tc.name: RenderBoxTest022
 * @tc.desc: Verify Box PerformLayout when Constraints(item < min = max).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest022, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Constraints(min < max = item).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(SMALL_SIZE);
    constraints.SetMaxSize(MIDDLE_SIZE);
    boxComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == MIDDLE_SIZE);
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == MIDDLE_SIZE);
}

/**
 * @tc.name: RenderBoxTest023
 * @tc.desc: Verify Box PerformLayout when Constraints(item < min = max).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest023, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Constraints(item < min = max).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(LARGE_SIZE);
    constraints.SetMaxSize(LARGE_SIZE);
    boxComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == LARGE_SIZE);
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == LARGE_SIZE);
}

/**
 * @tc.name: RenderBoxTest024
 * @tc.desc: Verify Box PerformLayout when invalid Constraints(min > max).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest024, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Constraints(min > max).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(LARGE_SIZE);
    constraints.SetMaxSize(SMALL_SIZE);
    boxComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetLayoutSize() == MIDDLE_SIZE);
    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == MIDDLE_SIZE);
}

/**
 * @tc.name: RenderBoxTest025
 * @tc.desc: Verify Box PerformLayout when background decoration(shadow).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest025, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with background decoration(shadow).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetConstraints(LayoutParam(SMALL_SIZE, LARGE_SIZE));
    RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
    Shadow shadow;
    shadow.SetColor(SHADOW_COLOR);
    shadow.SetBlurRadius(SHADOW_RADIUS);
    shadow.SetOffset(SHADOW_OFFSET);
    backDecoration->AddShadow(shadow);
    boxComponent->SetBackDecoration(backDecoration);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    auto background = renderBox->GetBackDecoration();
    auto renderShadow = background->GetShadows().front();
    ASSERT_TRUE(renderShadow.GetColor() == SHADOW_COLOR);
    ASSERT_TRUE(renderShadow.GetBlurRadius() == SHADOW_RADIUS);
    ASSERT_TRUE(renderShadow.GetOffset() == SHADOW_OFFSET);
}

/**
 * @tc.name: RenderBoxTest026
 * @tc.desc: Verify Box PerformLayout when background decoration(color&image).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest026, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with background decoration(color&image).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetConstraints(LayoutParam(SMALL_SIZE, LARGE_SIZE));
    RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetBackgroundColor(SHADOW_COLOR);
    RefPtr<BackgroundImage> backgroundImage = AceType::MakeRefPtr<BackgroundImage>();
    backgroundImage->SetSrc("1.jpg", nullptr);
    backDecoration->SetImage(backgroundImage);
    boxComponent->SetBackDecoration(backDecoration);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    auto background = renderBox->GetBackDecoration();
    ASSERT_TRUE(background->GetBackgroundColor() == SHADOW_COLOR);
    ASSERT_TRUE(background->GetImage()->GetSrc() == "1.jpg");
}

/**
 * @tc.name: RenderBoxTest027
 * @tc.desc: Verify Box PerformLayout when background decoration(border).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest027, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with background decoration(border).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetConstraints(LayoutParam(SMALL_SIZE, LARGE_SIZE));
    RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetBorder(BACK_BORDER);
    boxComponent->SetBackDecoration(backDecoration);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    auto background = renderBox->GetBackDecoration();
    ASSERT_TRUE(background->GetBorder() == BACK_BORDER);
}

/**
 * @tc.name: RenderBoxTest028
 * @tc.desc: Verify Box PerformLayout when front decoration(shadow&border).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest028, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with front decoration(shadow&border).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetConstraints(LayoutParam(SMALL_SIZE, LARGE_SIZE));
    RefPtr<Decoration> frontDecoration = AceType::MakeRefPtr<Decoration>();
    Shadow shadow;
    shadow.SetColor(SHADOW_COLOR);
    shadow.SetBlurRadius(SHADOW_RADIUS);
    shadow.SetOffset(SHADOW_OFFSET);
    frontDecoration->AddShadow(shadow);
    frontDecoration->SetBorder(BACK_BORDER);
    boxComponent->SetFrontDecoration(frontDecoration);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    auto front = renderBox->GetFrontDecoration();
    ASSERT_TRUE(front->GetBorder() == BACK_BORDER);
    auto renderShadow = front->GetShadows().front();
    ASSERT_TRUE(renderShadow.GetColor() == SHADOW_COLOR);
    ASSERT_TRUE(renderShadow.GetBlurRadius() == SHADOW_RADIUS);
    ASSERT_TRUE(renderShadow.GetOffset() == SHADOW_OFFSET);
}

/**
 * @tc.name: RenderBoxTest029
 * @tc.desc: Verify Box PerformLayout when front decoration(color&image).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest029, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with front decoration(color&image).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetConstraints(LayoutParam(SMALL_SIZE, LARGE_SIZE));
    RefPtr<Decoration> frontDecoration = AceType::MakeRefPtr<Decoration>();
    frontDecoration->SetBackgroundColor(SHADOW_COLOR);
    RefPtr<BackgroundImage> backgroundImage = AceType::MakeRefPtr<BackgroundImage>();
    backgroundImage->SetSrc("1.jpg", nullptr);
    frontDecoration->SetImage(backgroundImage);
    boxComponent->SetFrontDecoration(frontDecoration);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    auto front = renderBox->GetFrontDecoration();
    ASSERT_TRUE(front->GetBackgroundColor() == SHADOW_COLOR);
    ASSERT_TRUE(front->GetImage()->GetSrc() == "1.jpg");
}

/**
 * @tc.name: RenderBoxTest030
 * @tc.desc: Verify Box PerformLayout when set background color.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest030, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with background color.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetConstraints(LayoutParam(SMALL_SIZE, LARGE_SIZE));
    boxComponent->SetColor(SHADOW_COLOR);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetColor() == SHADOW_COLOR);
}

/**
 * @tc.name: RenderBoxTest031
 * @tc.desc: Verify Box PerformLayout when default background color.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest031, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with default background color.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetConstraints(LayoutParam(SMALL_SIZE, LARGE_SIZE));

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBox->GetColor() == Color::TRANSPARENT);
}

/**
 * @tc.name: RenderBoxTest032
 * @tc.desc: Verify Box PerformLayout when set gradients color backDecoration.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest032, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with gradients color backDecoration.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetConstraints(LayoutParam(SMALL_SIZE, LARGE_SIZE));
    RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
    Gradient gradient;
    backDecoration->SetGradient(gradient);
    boxComponent->SetBackDecoration(backDecoration);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    auto background = renderBox->GetBackDecoration();
    ASSERT_TRUE(background != nullptr);
}

/**
 * @tc.name: RenderBoxTest033
 * @tc.desc: Verify Box PerformLayout when set gradients color frontDecoration.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest033, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with gradients color frontDecoration.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetConstraints(LayoutParam(SMALL_SIZE, LARGE_SIZE));
    RefPtr<Decoration> frontDecoration = AceType::MakeRefPtr<Decoration>();
    Gradient gradient;
    frontDecoration->SetGradient(gradient);
    boxComponent->SetFrontDecoration(frontDecoration);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    auto front = renderBox->GetFrontDecoration();
    ASSERT_TRUE(front != nullptr);
}

/**
 * @tc.name: RenderBoxTest034
 * @tc.desc: Verify BoxBase PerformLayout when align TOP_LEFT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest034, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Alignment::TOP_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetAlignment(Alignment::TOP_LEFT);
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBoxBase->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest035
 * @tc.desc: Verify BoxBase PerformLayout when align TOP_CENTER.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest035, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Alignment::TOP_CENTER.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetAlignment(Alignment::TOP_CENTER);
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBoxBase->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest036
 * @tc.desc: Verify BoxBase PerformLayout when align TOP_RIGHT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest036, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Alignment::TOP_RIGHT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetAlignment(Alignment::TOP_RIGHT);
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBoxBase->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_RIGHT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest037
 * @tc.desc: Verify BoxBase PerformLayout when align CENTER_LEFT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest037, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Alignment::CENTER_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetAlignment(Alignment::CENTER_LEFT);
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBoxBase->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest038
 * @tc.desc: Verify BoxBase PerformLayout when align CENTER.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest038, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Alignment::CENTER.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetAlignment(Alignment::CENTER);
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBoxBase->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest039
 * @tc.desc: Verify BoxBase PerformLayout when align CENTER_RIGHT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest039, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Alignment::CENTER_RIGHT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetAlignment(Alignment::CENTER_RIGHT);
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBoxBase->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER_RIGHT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest040
 * @tc.desc: Verify BoxBase PerformLayout when align BOTTOM_LEFT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest040, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Alignment::BOTTOM_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetAlignment(Alignment::BOTTOM_LEFT);
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBoxBase->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_BOTTOM_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest041
 * @tc.desc: Verify BoxBase PerformLayout when align BOTTOM_CENTER.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest041, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Alignment::BOTTOM_CENTER.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetAlignment(Alignment::BOTTOM_CENTER);
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBoxBase->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_BOTTOM_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest042
 * @tc.desc: Verify BoxBase PerformLayout when align BOTTOM_RIGHT.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest042, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Alignment::BOTTOM_RIGHT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetAlignment(Alignment::BOTTOM_RIGHT);
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));
    ASSERT_TRUE(renderBoxBase->GetChildren().size() == 1);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_BOTTOM_RIGHT);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest043
 * @tc.desc: Verify BoxBase PerformLayout when same margin.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest043, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with same margin.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);
    boxBaseComponent->SetMargin(Edge(10.0));

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH + 20.0, BOX_HEIGHT + 20.0));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER + Offset(10.0, 10.0));
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest044
 * @tc.desc: Verify BoxBase PerformLayout when diff margin.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest044, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with diff margin.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);
    boxBaseComponent->SetMargin(Edge(10.0, 20.0, 30.0, 40.0));

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH + 40.0, BOX_HEIGHT + 60.0));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER + Offset(10.0, 20.0));
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest045
 * @tc.desc: Verify BoxBase PerformLayout when invalid margin.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest045, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with invalid margin.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);
    boxBaseComponent->SetMargin(Edge(-10.0, -20.0, -30.0, -40.0));

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_FALSE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_FALSE(child->GetPosition() == OFFSET_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest046
 * @tc.desc: Verify BoxBase PerformLayout when same padding.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest046, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with same padding.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);
    boxBaseComponent->SetPadding(Edge(10.0));

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest047
 * @tc.desc: Verify BoxBase PerformLayout when diff padding.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest047, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with diff padding.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);
    boxBaseComponent->SetPadding(Edge(10.0, 20.0, 30.0, 40.0));

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER - Offset(10.0, 10.0));
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest048
 * @tc.desc: Verify BoxBase PerformLayout when invalid padding.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest048, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with invalid padding.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);
    boxBaseComponent->SetPadding(Edge(-10.0, -20.0, -30.0, -40.0));

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_CENTER);
    ASSERT_TRUE(child->GetLayoutSize() == Size(ITEM_WIDTH, ITEM_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest049
 * @tc.desc: Verify BoxBase PerformLayout when valid width and height.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest049, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with valid width and height.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest050
 * @tc.desc: Verify BoxBase PerformLayout when invalid width and valid height.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest050, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with invalid width and valid height.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(-BOX_WIDTH);
    boxBaseComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(LENGTH, BOX_HEIGHT));
}

/**
 * @tc.name: RenderBoxTest051
 * @tc.desc: Verify BoxBase PerformLayout when valid width and invalid height.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest051, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with valid width and invalid height.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(BOX_WIDTH);
    boxBaseComponent->SetHeight(-BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(BOX_WIDTH, LENGTH));
}

/**
 * @tc.name: RenderBoxTest052
 * @tc.desc: Verify BoxBase PerformLayout when invalid width and height.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest052, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with invalid width and height.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    boxBaseComponent->SetWidth(-BOX_WIDTH);
    boxBaseComponent->SetHeight(-BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == Size(LENGTH, LENGTH));
}

/**
 * @tc.name: RenderBoxTest053
 * @tc.desc: Verify BoxBase PerformLayout when Constraints(min < max < item).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest053, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Constraints(min < max < item).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(ZERO_SIZE);
    constraints.SetMaxSize(SMALL_SIZE);
    boxBaseComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == SMALL_SIZE);
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == SMALL_SIZE);
}

/**
 * @tc.name: RenderBoxTest054
 * @tc.desc: Verify BoxBase PerformLayout when Constraints(item < min < max).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest054, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Constraints(item < min < max).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(LARGE_SIZE);
    constraints.SetMaxSize(HUGE_SIZE);
    boxBaseComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == LARGE_SIZE);
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == LARGE_SIZE);
}

/**
 * @tc.name: RenderBoxTest055
 * @tc.desc: Verify BoxBase PerformLayout when Constraints(item < min = max).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest055, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Constraints(min < max = item).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(SMALL_SIZE);
    constraints.SetMaxSize(MIDDLE_SIZE);
    boxBaseComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0)..
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == MIDDLE_SIZE);
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == MIDDLE_SIZE);
}

/**
 * @tc.name: RenderBoxTest056
 * @tc.desc: Verify BoxBase PerformLayout when Constraints(item < min = max).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest056, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Constraints(item < min = max).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(LARGE_SIZE);
    constraints.SetMaxSize(LARGE_SIZE);
    boxBaseComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == LARGE_SIZE);
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == LARGE_SIZE);
}

/**
 * @tc.name: RenderBoxTest057
 * @tc.desc: Verify BoxBase PerformLayout when invalid Constraints(min > max).
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest057, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxBaseComponent with Constraints(min > max).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxBaseComponent> boxBaseComponent = AceType::MakeRefPtr<MockBoxBaseComponent>();
    LayoutParam constraints;
    constraints.SetMinSize(LARGE_SIZE);
    constraints.SetMaxSize(SMALL_SIZE);
    boxBaseComponent->SetConstraints(constraints);

    /**
     * @tc.steps: step2. construct RenderBoxBase with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBoxBase> renderBoxBase = AceType::MakeRefPtr<MockRenderBoxBase>();
    renderBoxBase->Attach(mockContext);
    renderRoot->AddChild(renderBoxBase);
    renderBoxBase->Update(boxBaseComponent);
    renderBoxBase->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(renderBoxBase->GetLayoutSize() == MIDDLE_SIZE);
    const RefPtr<RenderNode>& child = renderBoxBase->GetChildren().front();
    ASSERT_TRUE(child->GetPosition() == OFFSET_TOP_LEFT);
    ASSERT_TRUE(child->GetLayoutSize() == MIDDLE_SIZE);
}

/**
 * @tc.name: RenderBoxTest058
 * @tc.desc: Verify Box PerformLayout when user had set LayoutInBox.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest058, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Constraints(min < max = item).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(LENGTH);
    boxComponent->SetWidth(LENGTH);
    boxComponent->SetLayoutInBoxFlag(true);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();

    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(NearEqual(renderBox->GetLayoutSize().Width(), 100.0));
    ASSERT_TRUE(std::abs(child->GetLayoutSize().Width() - 70.7107) < 0.1);
    ASSERT_TRUE(std::abs(child->GetPosition().GetX() - 14.6447) < 0.1);
}

/**
 * @tc.name: RenderBoxTest059
 * @tc.desc: Verify Box PerformLayout when user had set LayoutInBox.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest059, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Constraints(min < max = item).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(LENGTH);
    boxComponent->SetWidth(LENGTH);
    boxComponent->SetPadding(Edge(10.0, 0.0, 0.0, 0.0));
    boxComponent->SetLayoutInBoxFlag(true);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();

    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(NearEqual(renderBox->GetLayoutSize().Width(), 100.0));
    ASSERT_TRUE(std::abs(child->GetLayoutSize().Width() - 70.7107) < 0.1);
    ASSERT_TRUE(std::abs(child->GetPosition().GetX() - 14.6447) < 0.1);
}

/**
 * @tc.name: RenderBoxTest060
 * @tc.desc: Verify Box PerformLayout when user had set LayoutInBox.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxTest060, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Constraints(min < max = item).
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(LENGTH);
    boxComponent->SetWidth(LENGTH);
    boxComponent->SetPadding(Edge(20.0, 0.0, 0.0, 0.0));
    boxComponent->SetLayoutInBoxFlag(true);

    /**
     * @tc.steps: step2. construct RenderBox with child text(100.0, 100.0).
     * @tc.expected: step2. properties and children are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    renderBox->AddChild(BoxTestUtils::CreateRenderText(mockContext));

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderRoot->PerformLayout();

    const RefPtr<RenderNode>& child = renderBox->GetChildren().front();
    ASSERT_TRUE(NearEqual(renderBox->GetLayoutSize().Width(), 100.0));
    ASSERT_TRUE(std::abs(child->GetLayoutSize().Width() - 65.3553) < 0.1);
    ASSERT_TRUE(std::abs(child->GetPosition().GetX() - 20.0) < 0.1);
}

/**
 * @tc.name: RenderBoxScrollpageChangeTest001
 * @tc.desc: Verify Box Layout When scrollpage is true.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxScrollpageChangeTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::TOP_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox .
     * @tc.expected: step2. Verify properties are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();

    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    /**
     * @tc.steps: step3. Verify that the box layout height are calculated correctly when scrollpage set true.
     * @tc.expected: step3. box layout height will equal with pipeline rootHeight.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(NearEqual(renderBox->GetLayoutSize().Height(), BOX_HEIGHT));
    boxComponent->SetScrollPage(true);
    renderBox->Update(boxComponent);
    renderRoot->PerformLayout();
    ASSERT_TRUE(NearEqual(renderBox->GetLayoutSize().Height(), mockContext->GetRootHeight()));
}

/**
 * @tc.name: RenderBoxScrollpageChangeTest002
 * @tc.desc: Verify Box Layout When scrollpage is true.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBoxTest, RenderBoxScrollpageChangeTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BoxComponent with Alignment::TOP_LEFT.
     * @tc.expected: step1. properties set correctly.
     */
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(BOX_WIDTH);
    boxComponent->SetHeight(BOX_HEIGHT);

    /**
     * @tc.steps: step2. construct RenderBox .
     * @tc.expected: step2. Verify properties are set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();

    RefPtr<RenderRoot> renderRoot = BoxTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->Attach(mockContext);
    renderRoot->AddChild(renderBox);
    renderBox->Update(boxComponent);
    /**
     * @tc.steps: step3. Verify that the box layout height are calculated correctly when scrollpage reset true.
     * @tc.expected: step3. box layout height will reset.
     */
    renderRoot->PerformLayout();
    ASSERT_TRUE(NearEqual(renderBox->GetLayoutSize().Height(), BOX_HEIGHT));
    boxComponent->SetScrollPage(true);
    renderBox->Update(boxComponent);
    renderRoot->PerformLayout();
    ASSERT_TRUE(NearEqual(renderBox->GetLayoutSize().Height(), mockContext->GetRootHeight()));

    boxComponent->SetScrollPage(false);
    renderBox->Update(boxComponent);
    renderRoot->PerformLayout();
    ASSERT_TRUE(NearEqual(renderBox->GetLayoutSize().Height(), BOX_HEIGHT));
}

} // namespace OHOS::Ace
