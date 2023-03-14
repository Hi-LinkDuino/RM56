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

#include <functional>

#include "gtest/gtest.h"

#define protected public
#define private public
#include "core/pipeline/base/composed_element.h"
#include "core/pipeline/base/render_element.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string KEY_CALLBACK = "key_callback";
const std::string CLICK_CALLBACK = "click_callback";
const std::string FOCUS_CALLBACK = "focus_callback";
const std::string BLUR_CALLBACK = "blur_callback";

} // namespace

class MockRenderNode : public RenderNode {
    DECLARE_ACE_TYPE(MockRenderNode, RenderNode);

public:
    explicit MockRenderNode(bool takeBoundary = false) : RenderNode(takeBoundary) {}
    ~MockRenderNode() override = default;

    void PerformLayout() override {}
    void Update(const RefPtr<Component>& component) override {}
};

class MockElement : public FocusNode, public RenderElement {
    DECLARE_ACE_TYPE(MockElement, FocusNode, RenderElement);

public:
    void Create()
    {
        RefPtr<MockRenderNode> temp = AceType::MakeRefPtr<MockRenderNode>();
        renderNode_ = AceType::DynamicCast<RenderNode>(temp);
        renderNode_->SetParent(nullptr);
    }
};

class RowNode : public FocusGroup, public RenderElement {
    DECLARE_ACE_TYPE(RowNode, FocusGroup, RenderElement);

public:
    void Create()
    {
        RefPtr<MockRenderNode> temp = AceType::MakeRefPtr<MockRenderNode>();
        renderNode_ = AceType::DynamicCast<RenderNode>(temp);
        renderNode_->SetParent(nullptr);
    }

    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
};

class ColNode : public FocusGroup, public RenderElement {
    DECLARE_ACE_TYPE(ColNode, FocusGroup, RenderElement);

public:
    void Create()
    {
        RefPtr<MockRenderNode> temp = AceType::MakeRefPtr<MockRenderNode>();
        renderNode_ = AceType::DynamicCast<RenderNode>(temp);
        renderNode_->SetParent(nullptr);
    }

    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
};

bool RowNode::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    if (vertical) {
        return false;
    }

    return GoToNextFocus(reverse);
}

bool ColNode::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    if (vertical) {
        return GoToNextFocus(reverse);
    }

    return false;
}

void ComposedElement::ApplyChildren() {}

void PipelineContext::AddToHoverList(const RefPtr<RenderNode>& node) {}
void PipelineContext::AddDirtyLayoutNode(RefPtr<RenderNode> const& renderNode) {}
void PipelineContext::AddDirtyRenderNode(const RefPtr<RenderNode>& renderNode, bool overlay) {}
void PipelineContext::AddDirtyElement(const RefPtr<Element>& dirtyElement) {}
void PipelineContext::PushFocusAnimation(const RefPtr<Element>& element) const {}
void PipelineContext::PopFocusAnimation() const {}
void PipelineContext::PopRootFocusAnimation() const {}
void PipelineContext::PushShadow(const RefPtr<Element>& element) const {}
void PipelineContext::PopShadow() const {}
void PipelineContext::AddPredictLayoutNode(const RefPtr<RenderNode>& renderNode) {}
void PipelineContext::RemoveScheduleTask(uint32_t id) {}
void PipelineContext::AddDeactivateElement(int, RefPtr<Element> const&) {}
void PipelineContext::AddNeedRebuildFocusElement(RefPtr<Element> const&) {}
void PipelineContext::AddNeedRenderFinishNode(RefPtr<RenderNode> const& renderNode) {}
void PipelineContext::StoreNode(int32_t restoreId, const WeakPtr<RenderElement>& node) {}

std::string PipelineContext::GetRestoreInfo(int32_t restoreId)
{
    return "";
}

bool PipelineContext::IsVisibleChangeNodeExists(NodeId index) const
{
    return false;
}

RefPtr<Element> PipelineContext::GetDeactivateElement(int) const
{
    return nullptr;
}

const AnimationOption PipelineContext::GetExplicitAnimationOption() const
{
    return AnimationOption();
}

double PipelineContext::NormalizeToPx(const Dimension& dimension) const
{
    return dimension.Value();
}

bool PipelineContext::GetIsDeclarative() const
{
    return false;
}

uint64_t PipelineContext::GetTimeFromExternalTimer()
{
    return 0;
}

uint32_t PipelineContext::AddScheduleTask(const RefPtr<ScheduleTask>& task)
{
    return 0;
}

RefPtr<AccessibilityManager> PipelineContext::GetAccessibilityManager() const
{
    return nullptr;
}

RefPtr<PageElement> PipelineContext::GetLastPage() const
{
    return nullptr;
}

bool PipelineContext::Animate(const AnimationOption& option, const RefPtr<Curve>& curve,
    const std::function<void()>& propertyCallback, const std::function<void()>& finishCallBack)
{
    return true;
}

void PipelineContext::OpenImplicitAnimation(
    const AnimationOption& option, const RefPtr<Curve>& curve, const std::function<void()>& finishCallBack)
{}

bool PipelineContext::CloseImplicitAnimation()
{
    return true;
}

void PipelineContext::AddGeometryChangedNode(const RefPtr<RenderNode>& renderNode) {}

void PipelineContext::ForceLayoutForImplicitAnimation() {}

void PipelineContext::PostAsyncEvent(const std::function<void()>&) {}

void PipelineContext::PostAsyncEvent(std::function<void()>&&) {}

class FocusTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void InitFocusTree();

protected:
    RefPtr<MockElement> firstNodeElement_ = AceType::MakeRefPtr<MockElement>();
    RefPtr<MockElement> secondNodeElement_ = AceType::MakeRefPtr<MockElement>();
    RefPtr<MockElement> thirdNodeElement_ = AceType::MakeRefPtr<MockElement>();
    RefPtr<MockElement> fourthNodeElement_ = AceType::MakeRefPtr<MockElement>();

    RefPtr<ColNode> root_ = AceType::MakeRefPtr<ColNode>();
    RefPtr<RowNode> firstRow_ = AceType::MakeRefPtr<RowNode>();
    RefPtr<RowNode> secondRow_ = AceType::MakeRefPtr<RowNode>();

    RefPtr<FocusGroup> rootScope_ = AceType::DynamicCast<FocusGroup>(root_);
    RefPtr<FocusGroup> firstRowScope_ = AceType::DynamicCast<FocusGroup>(firstRow_);
    RefPtr<FocusGroup> secondRowScope_ = AceType::DynamicCast<FocusGroup>(secondRow_);

    RefPtr<FocusNode> firstNode_ = AceType::DynamicCast<FocusNode>(firstNodeElement_);
    RefPtr<FocusNode> secondNode_ = AceType::DynamicCast<FocusNode>(secondNodeElement_);
    RefPtr<FocusNode> thirdNode_ = AceType::DynamicCast<FocusNode>(thirdNodeElement_);
    RefPtr<FocusNode> fourthNode_ = AceType::DynamicCast<FocusNode>(fourthNodeElement_);
};

void FocusTest::SetUpTestCase() {}

void FocusTest::TearDownTestCase() {}

void FocusTest::SetUp()
{
    InitFocusTree();
}

void FocusTest::TearDown() {}

void FocusTest::InitFocusTree()
{
    firstRowScope_->AddChild(firstNode_);
    firstRowScope_->AddChild(secondNode_);
    secondRowScope_->AddChild(thirdNode_);
    secondRowScope_->AddChild(fourthNode_);
    rootScope_->AddChild(firstRowScope_);
    rootScope_->AddChild(secondRowScope_);
    root_->RequestFocusImmediately();

    firstNodeElement_->Create();
    secondNodeElement_->Create();
    thirdNodeElement_->Create();
    fourthNodeElement_->Create();
    root_->Create();
    firstRow_->Create();
    secondRow_->Create();
}

/**
 * @tc.name: FocusTest001
 * @tc.desc: Verify the state after the focus moves left and right.
 * @tc.type: FUNC
 * @tc.require: AR000DAUU9 AR000DAUUB
 */
HWTEST_F(FocusTest, FocusTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. focus moves to the right.
     */
    KeyEvent keyEvent(KeyCode::TV_CONTROL_RIGHT, KeyAction::UP);
    root_->HandleKeyEvent(keyEvent);

    /**
     * @tc.steps: step2. get focus status of each node.
     * @tc.expected: step2. the state of focus is correct.
     */
    ASSERT_TRUE(secondNode_->IsCurrentFocus());
    ASSERT_TRUE(firstRow_->IsCurrentFocus());
    ASSERT_TRUE(root_->IsCurrentFocus());
    ASSERT_FALSE(firstNode_->IsCurrentFocus());
    ASSERT_FALSE(secondRow_->IsCurrentFocus());
    ASSERT_FALSE(thirdNode_->IsCurrentFocus());
    ASSERT_FALSE(fourthNode_->IsCurrentFocus());

    /**
     * @tc.steps: step3. focus moves to the left.
     */
    keyEvent.code = KeyCode::TV_CONTROL_LEFT;
    keyEvent.action = KeyAction::UP;
    root_->HandleKeyEvent(keyEvent);

    /**
     * @tc.steps: step4. get focus status of each node.
     * @tc.expected: step4. the state of focus is correct.
     */
    ASSERT_TRUE(firstNode_->IsCurrentFocus());
    ASSERT_TRUE(firstRow_->IsCurrentFocus());
    ASSERT_TRUE(root_->IsCurrentFocus());
    ASSERT_FALSE(secondNode_->IsCurrentFocus());
    ASSERT_FALSE(secondRow_->IsCurrentFocus());
    ASSERT_FALSE(thirdNode_->IsCurrentFocus());
    ASSERT_FALSE(fourthNode_->IsCurrentFocus());
}

/**
 * @tc.name: FocusTest002
 * @tc.desc: Verify the state after the focus moves up and down.
 * @tc.type: FUNC
 * @tc.require: AR000DAUU9 AR000DAUUB
 */
HWTEST_F(FocusTest, FocusTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. focus moves down.
     */
    KeyEvent keyEvent(KeyCode::TV_CONTROL_DOWN, KeyAction::UP);
    root_->HandleKeyEvent(keyEvent);

    /**
     * @tc.steps: step2. get focus status of each node.
     * @tc.expected: step2. the state of focus is correct.
     */
    ASSERT_TRUE(thirdNode_->IsCurrentFocus());
    ASSERT_TRUE(secondRow_->IsCurrentFocus());
    ASSERT_TRUE(root_->IsCurrentFocus());
    ASSERT_FALSE(firstNode_->IsCurrentFocus());
    ASSERT_FALSE(firstRow_->IsCurrentFocus());
    ASSERT_FALSE(secondNode_->IsCurrentFocus());
    ASSERT_FALSE(fourthNode_->IsCurrentFocus());

    /**
     * @tc.steps: step3. focus moves up.
     */
    keyEvent.code = KeyCode::TV_CONTROL_UP;
    keyEvent.action = KeyAction::UP;
    root_->HandleKeyEvent(keyEvent);

    /**
     * @tc.steps: step4. get focus status of each node.
     * @tc.expected: step4. the state of focus is correct.
     */
    ASSERT_TRUE(firstNode_->IsCurrentFocus());
    ASSERT_TRUE(firstRow_->IsCurrentFocus());
    ASSERT_TRUE(root_->IsCurrentFocus());
    ASSERT_FALSE(secondNode_->IsCurrentFocus());
    ASSERT_FALSE(secondRow_->IsCurrentFocus());
    ASSERT_FALSE(thirdNode_->IsCurrentFocus());
    ASSERT_FALSE(fourthNode_->IsCurrentFocus());
}

/**
 * @tc.name: FocusTest003
 * @tc.desc: Verify the key callback interface.
 * @tc.type: FUNC
 * @tc.require: AR000DAUUA AR000DAIHU
 */
HWTEST_F(FocusTest, FocusTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set the key callback function.
     */
    KeyEvent keyEvent(KeyCode::TV_CONTROL_RIGHT, KeyAction::UP);
    std::string keyCallback;
    firstNode_->SetOnKeyCallback([&keyCallback](const KeyEvent& keyEvent) {
        keyCallback = KEY_CALLBACK;
        return false;
    });

    /**
     * @tc.steps: step2. handle key events.
     * @tc.expected: step2. key callback gets executed.
     */
    root_->HandleKeyEvent(keyEvent);
    ASSERT_TRUE(keyCallback == KEY_CALLBACK);
}

/**
 * @tc.name: FocusTest004
 * @tc.desc: Verify the click callback interface.
 * @tc.type: FUNC
 * @tc.require: AR000DAIHU AR000DA11V
 */
HWTEST_F(FocusTest, FocusTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set the click callback function.
     */
    KeyEvent keyEvent(KeyCode::KEY_ENTER, KeyAction::UP);
    std::string clickCallback;
    firstNode_->SetOnClickCallback([&clickCallback]() { clickCallback = CLICK_CALLBACK; });

    /**
     * @tc.steps: step2. handle key events.
     * @tc.expected: step2. click callback gets executed.
     */
    root_->HandleKeyEvent(keyEvent);
    ASSERT_TRUE(clickCallback == CLICK_CALLBACK);
}

/**
 * @tc.name: FocusTest005
 * @tc.desc: Verify the focus callback interface.
 * @tc.type: FUNC
 * @tc.require: AR000DAIHV AR000DAUU9
 */
HWTEST_F(FocusTest, FocusTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set the focus callback function.
     */
    std::string focusCallback;
    secondNode_->SetOnFocusCallback([&focusCallback]() { focusCallback = FOCUS_CALLBACK; });

    /**
     * @tc.steps: step2. secondNode_ requests focus.
     * @tc.expected: step2. focus callback gets executed.
     */
    secondNode_->RequestFocusImmediately();
    ASSERT_TRUE(focusCallback == FOCUS_CALLBACK);
}

/**
 * @tc.name: FocusTest006
 * @tc.desc: Verify the blur callback interface.
 * @tc.type: FUNC
 * @tc.require: AR000DAIHU AR000DA11V
 */
HWTEST_F(FocusTest, FocusTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set the blur callback function.
     */
    std::string blurCallback;
    firstNode_->SetOnBlurCallback([&blurCallback]() { blurCallback = BLUR_CALLBACK; });

    /**
     * @tc.steps: step2. secondNode_ requests focus.
     * @tc.expected: step2. blur callback gets executed.
     */
    secondNode_->RequestFocusImmediately();
    ASSERT_TRUE(blurCallback == BLUR_CALLBACK);
}

/**
 * @tc.name: FocusTest007
 * @tc.desc: Verify the request and lost focus interface.
 * @tc.type: FUNC
 * @tc.require: AR000DAIHU AR000DA11V
 */
HWTEST_F(FocusTest, FocusTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. thirdNode_ request focus.
     * @tc.expected: step1. the state of focus is correct.
     */
    thirdNode_->RequestFocusImmediately();
    ASSERT_TRUE(thirdNode_->IsCurrentFocus());
    ASSERT_TRUE(secondRow_->IsCurrentFocus());
    ASSERT_TRUE(root_->IsCurrentFocus());
    ASSERT_FALSE(firstNode_->IsCurrentFocus());
    ASSERT_FALSE(firstRow_->IsCurrentFocus());
    ASSERT_FALSE(secondNode_->IsCurrentFocus());
    ASSERT_FALSE(fourthNode_->IsCurrentFocus());

    /**
     * @tc.steps: step2. thirdNode_ lost focus.
     * @tc.expected: step2. the state of focus is correct.
     */
    thirdNode_->LostFocus();
    ASSERT_TRUE(secondRow_->IsCurrentFocus());
    ASSERT_TRUE(root_->IsCurrentFocus());
    ASSERT_FALSE(thirdNode_->IsCurrentFocus());
    ASSERT_FALSE(firstNode_->IsCurrentFocus());
    ASSERT_FALSE(firstRow_->IsCurrentFocus());
    ASSERT_FALSE(secondNode_->IsCurrentFocus());
    ASSERT_FALSE(fourthNode_->IsCurrentFocus());
}

/**
 * @tc.name: FocusTest008
 * @tc.desc: Verify the interface of adding and deleting nodes.
 * @tc.type: FUNC
 * @tc.require: AR000DAIHU AR000DA11V
 */
HWTEST_F(FocusTest, FocusTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parent node delete focus child node.
     * @tc.expected: step1. next child node gets focus.
     */
    firstRowScope_->RemoveChild(firstNode_);
    ASSERT_TRUE(secondNode_->IsCurrentFocus());

    /**
     * @tc.steps: step2. add a child node to the parent node.
     * @tc.expected: step2. the number of child nodes plus one.
     */
    RefPtr<FocusNode> newNode = AceType::MakeRefPtr<FocusNode>();
    firstRowScope_->AddChild(newNode);
    ASSERT_TRUE(firstRow_->GetChildrenList().size() == 2);

    /**
     * @tc.steps: step3. new node requests focus.
     * @tc.expected: step3. new node gets focus.
     */
    newNode->RequestFocusImmediately();
    ASSERT_TRUE(newNode->IsCurrentFocus());

    /**
     * @tc.steps: step4. delete new node.
     * @tc.expected: step4. The previous node gets focus and the number of child nodes is reduced by one.
     */
    firstRowScope_->RemoveChild(newNode);
    ASSERT_TRUE(secondNode_->IsCurrentFocus());
    ASSERT_TRUE(firstRow_->GetChildrenList().size() == 1);
}

/**
 * @tc.name: FocusTest009
 * @tc.desc: Test focus movement in complex scenes.
 * @tc.type: FUNC
 * @tc.require: AR000DAIHT AR000DAIHV
 */
HWTEST_F(FocusTest, FocusTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. initialize the focus tree.
     */
    RefPtr<MockElement> fifthNodeElement = AceType::MakeRefPtr<MockElement>();
    RefPtr<MockElement> sixthNodeElement = AceType::MakeRefPtr<MockElement>();
    RefPtr<MockElement> seventhNodeElement = AceType::MakeRefPtr<MockElement>();
    RefPtr<MockElement> eighthNodeElement = AceType::MakeRefPtr<MockElement>();

    RefPtr<RowNode> thirdRow = AceType::MakeRefPtr<RowNode>();
    RefPtr<ColNode> secondCol = AceType::MakeRefPtr<ColNode>();

    RefPtr<FocusNode> fifthNode = AceType::DynamicCast<FocusNode>(fifthNodeElement);
    RefPtr<FocusNode> sixthNode = AceType::DynamicCast<FocusNode>(sixthNodeElement);
    RefPtr<FocusNode> seventhNode = AceType::DynamicCast<FocusNode>(seventhNodeElement);
    RefPtr<FocusNode> eighthNode = AceType::DynamicCast<FocusNode>(eighthNodeElement);

    RefPtr<FocusGroup> thirdRowScope = AceType::DynamicCast<FocusGroup>(thirdRow);
    RefPtr<FocusGroup> secondColScope = AceType::DynamicCast<FocusGroup>(secondCol);

    fifthNodeElement->Create();
    sixthNodeElement->Create();
    seventhNodeElement->Create();
    eighthNodeElement->Create();
    thirdRow->Create();
    secondCol->Create();

    thirdRowScope->AddChild(fifthNode);
    thirdRowScope->AddChild(sixthNode);
    secondColScope->AddChild(seventhNode);
    secondColScope->AddChild(eighthNode);
    thirdRowScope->AddChild(secondColScope);
    firstRowScope_->AddChild(thirdRowScope);

    /**
     * @tc.steps: step2. focus moves to the right.
     * @tc.expected: step2. the state of focus is correct.
     */
    secondNode_->RequestFocusImmediately();
    KeyEvent keyEvent(KeyCode::TV_CONTROL_RIGHT, KeyAction::UP);
    root_->HandleKeyEvent(keyEvent);
    ASSERT_TRUE(fifthNode->IsCurrentFocus());

    root_->HandleKeyEvent(keyEvent);
    ASSERT_TRUE(sixthNode->IsCurrentFocus());

    root_->HandleKeyEvent(keyEvent);
    ASSERT_TRUE(seventhNode->IsCurrentFocus());

    root_->HandleKeyEvent(keyEvent);
    ASSERT_TRUE(seventhNode->IsCurrentFocus());

    /**
     * @tc.steps: step3. focus moves down.
     * @tc.expected: step3. the state of focus is correct.
     */
    keyEvent.code = KeyCode::TV_CONTROL_DOWN;
    keyEvent.action = KeyAction::UP;
    root_->HandleKeyEvent(keyEvent);
    ASSERT_TRUE(eighthNode->IsCurrentFocus());

    root_->HandleKeyEvent(keyEvent);
    ASSERT_TRUE(thirdNode_->IsCurrentFocus());

    /**
     * @tc.steps: step4. focus moves up.
     * @tc.expected: step4. the state of focus is correct.
     */
    keyEvent.code = KeyCode::TV_CONTROL_UP;
    keyEvent.action = KeyAction::UP;
    root_->HandleKeyEvent(keyEvent);
    ASSERT_TRUE(eighthNode->IsCurrentFocus());
}

} // namespace OHOS::Ace
