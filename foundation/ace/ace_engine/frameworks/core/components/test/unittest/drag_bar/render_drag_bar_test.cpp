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

#include "core/components/drag_bar/drag_bar_component.h"
#include "core/components/test/unittest/drag_bar/drag_bar_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class RenderDragBarTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    RefPtr<PipelineContext> mockContext_;
    RefPtr<MockRenderDragBar> renderNode_;
    int32_t index_ = 0;
};

void RenderDragBarTest::SetUpTestCase() {}
void RenderDragBarTest::TearDownTestCase() {}

void RenderDragBarTest::SetUp()
{
    mockContext_ = MockRenderCommon::GetMockContext();
    renderNode_ = AceType::MakeRefPtr<MockRenderDragBar>();
    renderNode_->Attach(mockContext_);
}

void RenderDragBarTest::TearDown()
{
    mockContext_ = nullptr;
    renderNode_ = nullptr;
}

/**
 * @tc.name: RenderDragBarTest001
 * @tc.desc: Verify the prop value for drag_bar.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDragBarTest, RenderDragBarTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct drag_bar component.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<DragBarComponent> dragBar = AceType::MakeRefPtr<DragBarComponent>();
    ASSERT_TRUE(dragBar->GetPanelMode() == PanelMode::FULL);
    ASSERT_TRUE(dragBar->HasDragBar() == true);

    /**
     * @tc.steps: step2. Set prop to drag_bar component and update to render node.
     * @tc.expected: step2. The prop of render node is correct.
     */
    dragBar->SetPanelMode(PanelMode::MINI);
    dragBar->SetHasDragBar(false);
    renderNode_->Update(dragBar);
    ASSERT_TRUE(renderNode_->CheckPanelMode(PanelMode::MINI));
    ASSERT_TRUE(renderNode_->CheckHasDragBar(false));
}

/**
 * @tc.name: RenderDragBarTest002
 * @tc.desc: Verify the touch event for drag_bar.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDragBarTest, RenderDragBarTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set prop to drag_bar component and update to render node.
     * @tc.expected: step1. The prop of render node is correct.
     */
    RefPtr<DragBarComponent> dragBar = AceType::MakeRefPtr<DragBarComponent>();
    dragBar->SetPanelMode(PanelMode::HALF);
    dragBar->SetHasDragBar(true);
    renderNode_->Update(dragBar);
    ASSERT_TRUE(renderNode_->CheckPanelMode(PanelMode::HALF));
    ASSERT_TRUE(renderNode_->CheckHasDragBar(true));

    /**
     * @tc.steps: step2. Trigger layout for drag_bar.
     * @tc.expected: step2. The layout info is correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderNode_->SetLayoutParam(layoutParam);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->CheckScale());
    ASSERT_TRUE(renderNode_->CheckIconOffset(Offset(0.0, 0.0)));

    /**
     * @tc.steps: step3. Trigger touch event for drag_bar.
     * @tc.expected: step3. The drag offset info is correct.
     */
    renderNode_->HandleTouchDown(Offset(100.0, 100.0));
    renderNode_->HandleTouchMove(Offset(200.0, 200.0));
    ASSERT_TRUE(renderNode_->CheckDragOffset(true, true));
    renderNode_->HandleTouchMove(Offset(50.0, 200.0));
    ASSERT_TRUE(renderNode_->CheckDragOffset(false, true));
    renderNode_->HandleTouchMove(Offset(200.0, 50.0));
    ASSERT_TRUE(renderNode_->CheckDragOffset(true, false));
    renderNode_->HandleTouchMove(Offset(50.0, 50.0));
    ASSERT_TRUE(renderNode_->CheckDragOffset(false, false));
    renderNode_->HandleTouchMove(Offset(100.0, 100.0));
    ASSERT_TRUE(renderNode_->CheckDragOffset(true, true) == false);
}

} // namespace OHOS::Ace
