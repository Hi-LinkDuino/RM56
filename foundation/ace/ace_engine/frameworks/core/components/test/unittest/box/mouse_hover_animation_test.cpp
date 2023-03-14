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

#include "core/accessibility/accessibility_node.h"
#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/root/root_element.h"
#include "core/components/stage/stage_element.h"
#include "core/components/test/unittest/box/box_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const double BOX_WIDTH_PARENT = 500.0;
const double BOX_HEIGHT_PARENT = 500.0;
const double BOX_WIDTH_CHILD = 300.0;
const double BOX_HEIGHT_CHILD = 300.0;
std::vector<Rect> BASE_VECTOR = {Rect(0.0, 0.0, 1080.0, 1920.0)};
std::vector<Rect> PARENT_VECTOR = {Rect(0.0, 0.0, BOX_WIDTH_PARENT, BOX_HEIGHT_PARENT)};
std::vector<Rect> CHILD_VECTOR = {Rect(0.0, 0.0, BOX_WIDTH_CHILD, BOX_HEIGHT_CHILD)};

} // namespace

class MouseHoverAnimationTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "MouseHoverAnimationTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "MouseHoverAnimationTest TearDownTestCase";
    }

    void SetUp() override {}

    void TearDown() override {}
};

/**
 * @tc.name: MouseHoverAnimationTest001
 * @tc.desc: test mouse enter animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest001";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    stackRender->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->SetAccessibilityNode(accessNode);
    renderBox->SetTouchRect(Rect(0.0, 0.0, BOX_WIDTH_PARENT, BOX_HEIGHT_PARENT));
    renderBox->Attach(mockContext);
    stackRender->AddChild(renderBox);

    /**
     * @tc.steps: step3. Verify that the mouse enter animation callback called correctly.
     * @tc.expected: step3. mouse enter animation callback called correctly.
     */
    MouseEvent event { .x = 600, .y = 600 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBox->enterAnimationCount_, 0);
}

/**
 * @tc.name: MouseHoverAnimationTest002
 * @tc.desc: test mouse enter animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest002";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->ChangeTouchRectList(BASE_VECTOR);
    stackRender->ChangeTouchRectList(BASE_VECTOR);
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->SetAccessibilityNode(accessNode);
    renderBox->ChangeTouchRectList(PARENT_VECTOR);
    renderBox->Attach(mockContext);
    stackRender->AddChild(renderBox);

    /**
     * @tc.steps: step3. Verify that the mouse enter animation callback called correctly.
     * @tc.expected: step3. mouse enter animation callback called correctly.
     */
    MouseEvent event { .x = 200, .y = 200 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBox->enterAnimationCount_, 1);
}

/**
 * @tc.name: MouseHoverAnimationTest003
 * @tc.desc: test mouse exit animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest003";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->ChangeTouchRectList(BASE_VECTOR);
    stackRender->ChangeTouchRectList(BASE_VECTOR);
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->SetAccessibilityNode(accessNode);
    renderBox->ChangeTouchRectList(PARENT_VECTOR);
    renderBox->Attach(mockContext);
    stackRender->AddChild(renderBox);

    /**
     * @tc.steps: step3. Verify that the mouse enter animation callback called correctly.
     * @tc.expected: step3. mouse enter animation callback called correctly.
     */
    MouseEvent event { .x = 700, .y = 700 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBox->enterAnimationCount_, 0);
    MouseEvent eventTwo { .x = 500, .y = 500 };
    mockContext->OnMouseEvent(eventTwo);
    EXPECT_EQ(renderBox->enterAnimationCount_, 1);
    MouseEvent eventThree { .x = 300, .y = 300 };
    mockContext->OnMouseEvent(eventThree);
    EXPECT_EQ(renderBox->enterAnimationCount_, 1);
}

/**
 * @tc.name: MouseHoverAnimationTest004
 * @tc.desc: test mouse exit animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest004";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->ChangeTouchRectList(BASE_VECTOR);
    stackRender->ChangeTouchRectList(BASE_VECTOR);
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->SetAccessibilityNode(accessNode);
    renderBox->ChangeTouchRectList(PARENT_VECTOR);
    renderBox->Attach(mockContext);
    stackRender->AddChild(renderBox);

    /**
     * @tc.steps: step3. Verify that the mouse enter animation callback called correctly.
     * @tc.expected: step3. mouse enter animation callback called correctly.
     */
    MouseEvent event { .x = 300, .y = 300 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBox->enterAnimationCount_, 1);
    MouseEvent eventTwo { .x = 500, .y = 500 };
    mockContext->OnMouseEvent(eventTwo);
    EXPECT_EQ(renderBox->enterAnimationCount_, 1);
    MouseEvent eventThree { .x = 700, .y = 700 };
    mockContext->OnMouseEvent(eventThree);
    EXPECT_EQ(renderBox->enterAnimationCount_, 1);
}

/**
 * @tc.name: MouseHoverAnimationTest005
 * @tc.desc: test mouse exit animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest005";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    stackRender->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->SetAccessibilityNode(accessNode);
    renderBox->SetTouchRect(Rect(0.0, 0.0, BOX_WIDTH_PARENT, BOX_HEIGHT_PARENT));
    renderBox->Attach(mockContext);
    stackRender->AddChild(renderBox);

    /**
     * @tc.steps: step3. Verify that the mouse exit animation callback called correctly.
     * @tc.expected: step3. mouse exit animation callback called correctly.
     */
    MouseEvent event { .x = 600, .y = 600 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBox->exitAnimationCount_, 0);
}

/**
 * @tc.name: MouseHoverAnimationTest006
 * @tc.desc: test mouse exit animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest006";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    stackRender->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->SetAccessibilityNode(accessNode);
    renderBox->SetTouchRect(Rect(0.0, 0.0, BOX_WIDTH_PARENT, BOX_HEIGHT_PARENT));
    renderBox->Attach(mockContext);
    stackRender->AddChild(renderBox);

    /**
     * @tc.steps: step3. Verify that the mouse exit animation callback called correctly.
     * @tc.expected: step3. mouse exit animation callback called correctly.
     */
    MouseEvent event { .x = 300, .y = 300 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBox->exitAnimationCount_, 0);
}

/**
 * @tc.name: MouseHoverAnimationTest007
 * @tc.desc: test mouse exit animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest007";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->ChangeTouchRectList(BASE_VECTOR);
    stackRender->ChangeTouchRectList(BASE_VECTOR);
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->SetAccessibilityNode(accessNode);
    renderBox->ChangeTouchRectList(PARENT_VECTOR);
    renderBox->Attach(mockContext);
    stackRender->AddChild(renderBox);

    /**
     * @tc.steps: step3. Verify that the mouse exit animation callback called correctly.
     * @tc.expected: step3. mouse exit animation callback called correctly.
     */
    MouseEvent event { .x = 300, .y = 300 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBox->exitAnimationCount_, 0);
    MouseEvent eventTwo { .x = 500, .y = 500 };
    mockContext->OnMouseEvent(eventTwo);
    EXPECT_EQ(renderBox->exitAnimationCount_, 0);
    MouseEvent eventThree { .x = 700, .y = 700 };
    mockContext->OnMouseEvent(eventThree);
    EXPECT_EQ(renderBox->exitAnimationCount_, 1);
}

/**
 * @tc.name: MouseHoverAnimationTest008
 * @tc.desc: test mouse exit animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest008";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    stackRender->SetTouchRect(Rect(0.0, 0.0, 1080.0, 1920.0));
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    renderBox->SetAccessibilityNode(accessNode);
    renderBox->SetTouchRect(Rect(0.0, 0.0, BOX_WIDTH_PARENT, BOX_HEIGHT_PARENT));
    renderBox->Attach(mockContext);
    stackRender->AddChild(renderBox);

    /**
     * @tc.steps: step3. Verify that the mouse exit animation callback called correctly.
     * @tc.expected: step3. mouse exit animation callback called correctly.
     */
    MouseEvent event { .x = 700, .y = 700 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBox->exitAnimationCount_, 0);
    MouseEvent eventTwo { .x = 500, .y = 500 };
    mockContext->OnMouseEvent(eventTwo);
    EXPECT_EQ(renderBox->exitAnimationCount_, 0);
    MouseEvent eventThree { .x = 300, .y = 300 };
    mockContext->OnMouseEvent(eventThree);
    EXPECT_EQ(renderBox->exitAnimationCount_, 0);
}

/**
 * @tc.name: MouseHoverAnimationTest009
 * @tc.desc: test parent and child mouse enter/exit animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest009";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->ChangeTouchRectList(BASE_VECTOR);
    stackRender->ChangeTouchRectList(BASE_VECTOR);
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBoxParent = AceType::MakeRefPtr<MockRenderBox>();
    renderBoxParent->SetAccessibilityNode(accessNode);
    RefPtr<AccessibilityNode> accessNodeTwo = AceType::MakeRefPtr<AccessibilityNode>(2, "box");
    RefPtr<MockRenderBox> renderBoxChild = AceType::MakeRefPtr<MockRenderBox>();
    renderBoxChild->SetAccessibilityNode(accessNodeTwo);
    renderBoxParent->ChangeTouchRectList(PARENT_VECTOR);
    renderBoxChild->ChangeTouchRectList(CHILD_VECTOR);
    renderBoxParent->Attach(mockContext);
    renderBoxChild->Attach(mockContext);
    stackRender->AddChild(renderBoxParent);
    renderBoxParent->AddChild(renderBoxChild);

    /**
     * @tc.steps: step3. Verify that the mouse enter/exit animation callback called correctly.
     * @tc.expected: step3. mouse enter/exit animation callback called correctly.
     */
    MouseEvent event { .x = 700, .y = 700 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBoxParent->enterAnimationCount_, 0);
    EXPECT_EQ(renderBoxParent->exitAnimationCount_, 0);
    EXPECT_EQ(renderBoxChild->enterAnimationCount_, 0);
    EXPECT_EQ(renderBoxChild->exitAnimationCount_, 0);
    MouseEvent eventTwo { .x = 500, .y = 500 };
    mockContext->OnMouseEvent(eventTwo);
    EXPECT_EQ(renderBoxParent->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxParent->exitAnimationCount_, 0);
    EXPECT_EQ(renderBoxChild->enterAnimationCount_, 0);
    EXPECT_EQ(renderBoxChild->exitAnimationCount_, 0);
    MouseEvent eventThree { .x = 300, .y = 300 };
    mockContext->OnMouseEvent(eventThree);
    EXPECT_EQ(renderBoxParent->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxParent->exitAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->exitAnimationCount_, 0);
    MouseEvent eventFour { .x = 100, .y = 100 };
    mockContext->OnMouseEvent(eventFour);
    EXPECT_EQ(renderBoxParent->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxParent->exitAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->exitAnimationCount_, 0);
    MouseEvent eventFive { .x = 400, .y = 400 };
    mockContext->OnMouseEvent(eventFive);
    EXPECT_EQ(renderBoxParent->enterAnimationCount_, 2);
    EXPECT_EQ(renderBoxParent->exitAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->exitAnimationCount_, 1);
}

/**
 * @tc.name: MouseHoverAnimationTest010
 * @tc.desc: test parent and child mouse enter/exit animation.
 * @tc.type: FUNC
 */
HWTEST_F(MouseHoverAnimationTest, MouseHoverAnimationTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MouseHoverAnimationTest MouseHoverAnimationTest010";
    /**
     * @tc.steps: step1. construct RenderBox and RenderRoot.
     * @tc.expected: step1. properties set correctly.
     */
    auto mockContext = MockRenderCommon::GetMockContext();
    auto rootElemet = mockContext->SetupRootElement();
    auto renderRoot = rootElemet->GetRenderNode();
    auto stackRender = renderRoot->GetFirstChild();

    /**
     * @tc.steps: step2. set RenderBox and RenderRoot TouchRect.
     * @tc.expected: step2. properties set correctly.
     */
    renderRoot->ChangeTouchRectList(BASE_VECTOR);
    stackRender->ChangeTouchRectList(BASE_VECTOR);
    RefPtr<AccessibilityNode> accessNode = AceType::MakeRefPtr<AccessibilityNode>(1, "box");
    RefPtr<MockRenderBox> renderBoxParent = AceType::MakeRefPtr<MockRenderBox>();
    renderBoxParent->SetAccessibilityNode(accessNode);
    RefPtr<AccessibilityNode> accessNodeTwo = AceType::MakeRefPtr<AccessibilityNode>(2, "box");
    RefPtr<MockRenderBox> renderBoxChild = AceType::MakeRefPtr<MockRenderBox>();
    renderBoxChild->SetAccessibilityNode(accessNodeTwo);
    renderBoxParent->ChangeTouchRectList(PARENT_VECTOR);
    renderBoxChild->ChangeTouchRectList(CHILD_VECTOR);
    renderBoxParent->Attach(mockContext);
    renderBoxChild->Attach(mockContext);
    stackRender->AddChild(renderBoxParent);
    renderBoxParent->AddChild(renderBoxChild);

    /**
     * @tc.steps: step3. Verify that the mouse enter/exit animation callback called correctly.
     * @tc.expected: step3. mouse entet/exit animation callback called correctly.
     */
    MouseEvent event { .x = 100, .y = 100 };
    mockContext->OnMouseEvent(event);
    EXPECT_EQ(renderBoxParent->enterAnimationCount_, 0);
    EXPECT_EQ(renderBoxParent->exitAnimationCount_, 0);
    EXPECT_EQ(renderBoxChild->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->exitAnimationCount_, 0);
    MouseEvent eventTwo { .x = 300, .y = 300 };
    mockContext->OnMouseEvent(eventTwo);
    EXPECT_EQ(renderBoxParent->enterAnimationCount_, 0);
    EXPECT_EQ(renderBoxParent->exitAnimationCount_, 0);
    EXPECT_EQ(renderBoxChild->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->exitAnimationCount_, 0);
    MouseEvent eventThree { .x = 500, .y = 500 };
    mockContext->OnMouseEvent(eventThree);
    EXPECT_EQ(renderBoxParent->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxParent->exitAnimationCount_, 0);
    EXPECT_EQ(renderBoxChild->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->exitAnimationCount_, 1);
    MouseEvent eventFour { .x = 700, .y = 700 };
    mockContext->OnMouseEvent(eventFour);
    EXPECT_EQ(renderBoxParent->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxParent->exitAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->enterAnimationCount_, 1);
    EXPECT_EQ(renderBoxChild->exitAnimationCount_, 1);
}

} // namespace OHOS::Ace