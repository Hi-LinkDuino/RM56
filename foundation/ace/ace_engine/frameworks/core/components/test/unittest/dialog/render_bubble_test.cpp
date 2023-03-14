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

#define protected public
#include "core/components/bubble/bubble_component.h"
#undef protected

#include "core/components/test/unittest/dialog/dialog_test_utils.h"
#include "core/components/test/unittest/display/display_test_utils.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Placement BUBBLE_PLACEMENT_VALUE = Placement::LEFT;
const Color BUBBLE_MASK_COLOR_VALUE = Color(0xffff0000);
const std::string ON_VISIBILITY_CHANGE = "testCallback";
const std::string TEXT_VALUE = "HiAce";
const double BOX_SIZE_MIN = 100;
const double BOX_SIZE_MAX = 400;
const Size LAYOUT_SIZE = Size(100, 100);
const Size LAYOUT_SIZE_MAX = Size(400, 400);
const Offset POSITION_VALUE_LEFT = Offset(92, 250);
const Offset POSITION_VALUE_RIGHT = Offset(408, 250);
const Offset POSITION_VALUE_TOP = Offset(250, 84);
const Offset POSITION_VALUE_BOTTOM = Offset(250, 416);
const Offset POSITION_VALUE_TOP_LEFT = Offset(100, 100);
const Offset POSITION_VALUE_TOP_RIGHT = Offset(400, 100);
const Offset POSITION_VALUE_BOTTOM_LEFT = Offset(100, 400);
const Offset POSITION_VALUE_BOTTOM_RIGHT = Offset(400, 400);
const Offset POSITION_VALUE_DEFAULT = Offset(200, 200);
const Offset POSITION_BOTTOM_ERROR_LEFT = Offset(300, 516);
const Offset POSITION_BOTTOM_ERROR_RIGHT = Offset(674, 416);
const Offset POSITION_ERROR_FULLSCREEN = Offset(-208, 1000);

} // namespace

class RenderBubbleTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: RenderBubbleUpdate001
 * @tc.desc: Verify the Update Interface of RenderBubble work correctly with bubble component.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubbleUpdate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct BubbleComponent and RenderBubble.
     */
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    RefPtr<BubbleComponent> bubble = AceType::MakeRefPtr<BubbleComponent>(text);
    auto popupParam = AceType::MakeRefPtr<PopupParam>();
    popupParam->SetPlacement(Placement::LEFT);
    popupParam->SetMaskColor(Color(0xffff0000));
    bubble->SetPopupParam(popupParam);
    RefPtr<MockRenderBubble> renderBubble = AceType::MakeRefPtr<MockRenderBubble>();

    /**
     * @tc.steps: step2. call the Update interface of RenderBubble.
     * @tc.expected: step2. renderBubble is set as needLayout, properties are set correctly.
     */
    renderBubble->Update(bubble);
    EXPECT_TRUE(renderBubble->NeedLayout());
    EXPECT_EQ(renderBubble->GetPlacement(), BUBBLE_PLACEMENT_VALUE);
    EXPECT_EQ(renderBubble->GetMaskColor(), BUBBLE_MASK_COLOR_VALUE);
}

/**
 * @tc.name: RenderBubblePerformLayout001
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with left placement.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::LEFT, Offset(200, 200), Size(200, 200));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MIN, BOX_SIZE_MIN);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE);
    EXPECT_EQ(child->GetPosition(), POSITION_VALUE_LEFT);
}

/**
 * @tc.name: RenderBubblePerformLayout002
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with right placement.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::RIGHT, Offset(200, 200), Size(200, 200));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MIN, BOX_SIZE_MIN);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE);
    EXPECT_EQ(child->GetPosition(), POSITION_VALUE_RIGHT);
}

/**
 * @tc.name: RenderBubblePerformLayout003
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with top placement.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::TOP, Offset(200, 200), Size(200, 200));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MIN, BOX_SIZE_MIN);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE);
    EXPECT_EQ(child->GetPosition(), POSITION_VALUE_TOP);
}

/**
 * @tc.name: RenderBubblePerformLayout004
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with bottom placement.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::BOTTOM, Offset(200, 200), Size(200, 200));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MIN, BOX_SIZE_MIN);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE);
    EXPECT_EQ(child->GetPosition(), POSITION_VALUE_BOTTOM);
}

/**
 * @tc.name: RenderBubblePerformLayout005
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with topLeft placement.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::TOP_LEFT, Offset(200, 200), Size(200, 200));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MIN, BOX_SIZE_MIN);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE);
    EXPECT_EQ(child->GetPosition(), POSITION_VALUE_TOP_LEFT);
}

/**
 * @tc.name: RenderBubblePerformLayout006
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with topRight placement.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::TOP_RIGHT, Offset(200, 200), Size(200, 200));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MIN, BOX_SIZE_MIN);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE);
    EXPECT_EQ(child->GetPosition(), POSITION_VALUE_TOP_RIGHT);
}

/**
 * @tc.name: RenderBubblePerformLayout007
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with bottomLeft placement.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::BOTTOM_LEFT, Offset(200, 200), Size(200, 200));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MIN, BOX_SIZE_MIN);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE);
    EXPECT_EQ(child->GetPosition(), POSITION_VALUE_BOTTOM_LEFT);
}

/**
 * @tc.name: RenderBubblePerformLayout008
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with bottomRight placement.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::BOTTOM_RIGHT, Offset(200, 200), Size(200, 200));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MIN, BOX_SIZE_MIN);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE);
    EXPECT_EQ(child->GetPosition(), POSITION_VALUE_BOTTOM_RIGHT);
}

/**
 * @tc.name: RenderBubblePerformLayout009
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with negative dx.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::LEFT, Offset(100, 100), Size(800, 400));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MAX, BOX_SIZE_MAX);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE_MAX);
    EXPECT_EQ(child->GetPosition(), POSITION_BOTTOM_ERROR_LEFT);
}

/**
 * @tc.name: RenderBubblePerformLayout010
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with dx over screen.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::RIGHT, Offset(800, 200), Size(200, 200));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MAX, BOX_SIZE_MAX);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE_MAX);
    EXPECT_EQ(child->GetPosition(), POSITION_BOTTOM_ERROR_RIGHT);
}

/**
 * @tc.name: RenderBubblePerformLayout011
 * @tc.desc: Verify PerformLayout interface of RenderBubble works correctly with bottomRight placement.
 * @tc.type: FUNC
 */
HWTEST_F(RenderBubbleTest, RenderBubblePerformLayout011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderBubble and RenderBox.
     */
    auto mockContext = DisplayTestUtils::GetMockContext();
    RefPtr<RenderRoot> root = DisplayTestUtils::CreateRenderRoot();
    RefPtr<MockRenderBubble> bubble =
        DialogTestUtils::CreateRenderBubble(Placement::LEFT, Offset(200, 200), Size(800, 2000));
    bubble->Attach(mockContext);
    RefPtr<RenderBox> box = DisplayTestUtils::CreateRenderBox(BOX_SIZE_MAX, BOX_SIZE_MAX);
    box->Attach(mockContext);
    bubble->AddChild(box);
    root->AddChild(bubble);

    /**
     * @tc.steps: step2. call the PerformLayout interface of RenderRoot.
     * @tc.expected: step2. layoutSize and position of child is correct.
     */
    root->PerformLayout();
    ASSERT_TRUE(!(bubble->GetChildren().empty()));
    const auto child = bubble->GetChildren().front();
    ASSERT_TRUE(child != nullptr);
    EXPECT_EQ(child->GetLayoutSize(), LAYOUT_SIZE_MAX);
    EXPECT_EQ(child->GetPosition(), POSITION_ERROR_FULLSCREEN);
}

} // namespace OHOS::Ace