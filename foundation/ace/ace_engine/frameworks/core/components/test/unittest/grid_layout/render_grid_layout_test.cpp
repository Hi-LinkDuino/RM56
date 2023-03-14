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

#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/render_grid_layout.h"
#include "core/components/test/unittest/grid_layout/grid_layout_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class RenderGridLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    RefPtr<PipelineContext> mockContext_;
    RefPtr<RenderGridLayout> renderNode_;
    int32_t index_ = 0;
};

void RenderGridLayoutTest::SetUpTestCase() {}
void RenderGridLayoutTest::TearDownTestCase() {}

void RenderGridLayoutTest::SetUp()
{
    mockContext_ = MockRenderCommon::GetMockContext();
    renderNode_ = AceType::MakeRefPtr<RenderGridLayout>();
    renderNode_->Attach(mockContext_);
}

void RenderGridLayoutTest::TearDown()
{
    mockContext_ = nullptr;
    renderNode_ = nullptr;
}

/**
 * @tc.name: RenderGridLayoutTest001
 * @tc.desc: Verify Grid Layout PerformLayout can calculate four children with row direction.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SO
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 4 child by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 50%";
    std::string colArgs = "50% 1fr";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    int32_t count = 4;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 4);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 4);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        ASSERT_TRUE(item->GetPosition() == Offset(index / 2 * 540.0, index % 2 * 540.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest002
 * @tc.desc: Verify Grid Layout PerformLayout can calculate four children with column direction.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SO
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 4 child by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 1fr";
    std::string colArgs = "50% 50%";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs));
    int32_t count = 4;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(0, 0, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 4);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(800.0, 800.0));
    renderNode_->SetLayoutParam(layoutParam);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(800.0, 800.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 4);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        ASSERT_TRUE(item->GetPosition() == Offset(index % 2 * 400.0, index / 2 * 400.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 400.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest003
 * @tc.desc: Verify Grid Layout can Dynamic adjustment row count.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SO
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 4 child by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 540px";
    std::string colArgs = "540px 50%";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    int32_t count = 4;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(0, 0, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 4);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 4);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        ASSERT_TRUE(item->GetPosition() == Offset(index / 2 * 540.0, index % 2 * 540.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        index++;
    }

    /**
     * @tc.steps: step3. adjustment row count and trigger the relayout.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    rowArgs = "";
    colArgs = "540px 540px";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    index = 0;
    for (const auto& item : renderNode_->GetChildren()) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index < 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(index % 2 * 540.0, 270.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        } else {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(0.0, 0.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest004
 * @tc.desc: Verify Grid Layout can Dynamic adjustment column count.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SO
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 9 child by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 360px 1fr";
    std::string colArgs = "360px 1fr 360px";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs));
    int32_t count = 9;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 9);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 9);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        ASSERT_TRUE(item->GetPosition() == Offset(index % 3 * 360.0, index / 3 * 360.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 360.0));
        index++;
    }

    /**
     * @tc.steps: step3. adjustment column count and trigger the relayout.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    rowArgs = "1fr 1fr";
    colArgs = "216px 216px 216px 216px 216px";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    index = 0;
    for (const auto& item : renderNode_->GetChildren()) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        ASSERT_TRUE(item->GetPosition() == Offset(index / 2 * 216.0, index % 2 * 540.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(216.0, 540.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest005
 * @tc.desc: Verify Grid Layout can layout item with index and span when row.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SP
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 3 child by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 1fr 1fr";
    std::string colArgs = "1fr 1fr 1fr";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    RefPtr<RenderNode> item1 = GridLayoutTestUtils::CreateRenderItem(0, 0, 3, 1);
    item1->GetChildren().front()->Attach(mockContext_);
    item1->Attach(mockContext_);
    renderNode_->AddChild(item1);
    RefPtr<RenderNode> item2 = GridLayoutTestUtils::CreateRenderItem(1, 1, 2, 2);
    item2->GetChildren().front()->Attach(mockContext_);
    item2->Attach(mockContext_);
    renderNode_->AddChild(item2);
    RefPtr<RenderNode> item3 = GridLayoutTestUtils::CreateRenderItem(0, 1, 1, 2);
    item3->GetChildren().front()->Attach(mockContext_);
    item3->Attach(mockContext_);
    renderNode_->AddChild(item3);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 3);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 270.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 540.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(450.0, 450.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(450.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 360.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest006
 * @tc.desc: Verify Grid Layout can layout item with index and span when column.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SP
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 7 child by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 1fr 1fr 1fr";
    std::string colArgs = "1fr 1fr 1fr 1fr";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs));
    RefPtr<RenderNode> item1 = GridLayoutTestUtils::CreateRenderItem(2, 0, 2, 4);
    item1->GetChildren().front()->Attach(mockContext_);
    item1->Attach(mockContext_);
    renderNode_->AddChild(item1);
    RefPtr<RenderNode> item2 = GridLayoutTestUtils::CreateRenderItem(0, 2, 4, 2);
    item2->GetChildren().front()->Attach(mockContext_);
    item2->Attach(mockContext_);
    renderNode_->AddChild(item2);
    int32_t count = 5;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 7);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(270.0, 540.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        } else if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(540.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        } else if (index > 5) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(0.0, 0.0));
        } else {
            ASSERT_TRUE(item->GetPosition() == Offset((index - 2) % 2 * 270.0, (index - 2) / 2 * 270.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(270.0, 270.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest007
 * @tc.desc: Verify Grid Layout can layout item with index and span.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SM
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 4 child by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "50% 50%";
    std::string colArgs = "1fr 1fr 1fr";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs));
    RefPtr<RenderNode> item1 = GridLayoutTestUtils::CreateRenderItem(1, 0, 1, 2);
    item1->GetChildren().front()->Attach(mockContext_);
    item1->Attach(mockContext_);
    renderNode_->AddChild(item1);
    RefPtr<RenderNode> item2 = GridLayoutTestUtils::CreateRenderItem(0, 1, 1, 2);
    item2->GetChildren().front()->Attach(mockContext_);
    item2->Attach(mockContext_);
    renderNode_->AddChild(item2);
    int32_t count = 2;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 2, 2);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 4);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(90.0, 540.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(450.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 540.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(720.0, 540.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 540.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest008
 * @tc.desc: Verify Grid Layout can support focus obtain.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SN
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 9 child by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 1fr 1fr";
    std::string colArgs = "1fr 1fr 1fr";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    int32_t count = 9;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));

    /**
     * @tc.steps: step2. trigger focus event.
     * @tc.expected: step2. grid layout can obtain focus.
     */
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == 3); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 4); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, true) == 1); // LEFT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, true) == 0); // UP
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == 3); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 4); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 5); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == -1); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == 8); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == -1); // RIGHT
}

/**
 * @tc.name: RenderGridLayoutTest009
 * @tc.desc: Verify Grid Layout can support focus lost.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SN
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 9 child by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "360px 360px 360px";
    std::string colArgs = "1fr 1fr 1fr";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs));
    int32_t count = 9;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));

    /**
     * @tc.steps: step2. trigger focus event.
     * @tc.expected: step2. grid layout can obtain focus.
     */
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == 1); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 4); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, true) == 3); // LEFT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, true) == 0); // UP
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == 1); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 4); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 7); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == -1); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == 8); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == -1); // RIGHT
}

/**
 * @tc.name: RenderGridLayoutTest010
 * @tc.desc: Verify Grid Layout can support focus move.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SN
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 3 child by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 1fr 1fr";
    std::string colArgs = "360px 360px 360px";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    RefPtr<RenderNode> item1 = GridLayoutTestUtils::CreateRenderItem(1, 0, 1, 3);
    item1->GetChildren().front()->Attach(mockContext_);
    item1->Attach(mockContext_);
    renderNode_->AddChild(item1);
    RefPtr<RenderNode> item2 = GridLayoutTestUtils::CreateRenderItem(0, 0, 1, 2);
    item2->GetChildren().front()->Attach(mockContext_);
    item2->Attach(mockContext_);
    renderNode_->AddChild(item2);
    RefPtr<RenderNode> item3 = GridLayoutTestUtils::CreateRenderItem(2, 1, 1, 2);
    item3->GetChildren().front()->Attach(mockContext_);
    item3->Attach(mockContext_);
    renderNode_->AddChild(item3);
    int32_t count = 2;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 2, 2);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));

    /**
     * @tc.steps: step2. trigger focus event.
     * @tc.expected: step2. grid layout can obtain focus.
     */
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == -1); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 3); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, true) == -1); // LEFT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, true) == 0); // UP
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, true) == 1); // UP
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == 4); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 0); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 2); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == -1); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, true) == 3); // LEFT
}

/**
 * @tc.name: RenderGridLayoutTest011
 * @tc.desc: Verify Grid Layout can support focus move with empty grid.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SN
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 3 child by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 1fr 1fr";
    std::string colArgs = "25% 25% 25% 25%";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs));
    RefPtr<RenderNode> item1 = GridLayoutTestUtils::CreateRenderItem(0, 0, 1, 3);
    item1->GetChildren().front()->Attach(mockContext_);
    item1->Attach(mockContext_);
    renderNode_->AddChild(item1);
    RefPtr<RenderNode> item2 = GridLayoutTestUtils::CreateRenderItem(1, 0, 2, 1);
    item2->GetChildren().front()->Attach(mockContext_);
    item2->Attach(mockContext_);
    renderNode_->AddChild(item2);
    RefPtr<RenderNode> item3 = GridLayoutTestUtils::CreateRenderItem(1, 2, 2, 1);
    item3->GetChildren().front()->Attach(mockContext_);
    item3->Attach(mockContext_);
    renderNode_->AddChild(item3);
    RefPtr<RenderNode> item4 = GridLayoutTestUtils::CreateRenderItem(2, 1, 1, 2);
    item4->GetChildren().front()->Attach(mockContext_);
    item4->Attach(mockContext_);
    renderNode_->AddChild(item4);
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));

    /**
     * @tc.steps: step2. trigger focus event.
     * @tc.expected: step2. grid layout can obtain focus.
     */
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == -1); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 1); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, true) == -1); // LEFT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, true) == 0); // UP
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 1); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == 2); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == -1); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, true) == 0); // UP
}

/**
 * @tc.name: RenderGridLayoutTest012
 * @tc.desc: Verify Grid Layout PerformLayout can calculate children with gap
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SM
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 4 child by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "30% 70%";
    std::string colArgs = "1fr 2fr";
    auto component = GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs);
    auto gridcomponent = AceType::DynamicCast<GridLayoutComponent>(component);
    gridcomponent->SetRowGap(80.0_px);
    gridcomponent->SetColumnGap(60.0_px);
    renderNode_->Update(component);
    int32_t count = 4;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 4);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 4);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(340.0, 324.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 512.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(340.0, 540.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(470.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 324.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(470.0, 512.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest013
 * @tc.desc: Verify Grid Layout PerformLayout can calculate children with gap
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SM
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 4 child by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "20% 30% 70%";
    std::string colArgs = "600px 700px";
    auto component = GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs);
    auto gridcomponent = AceType::DynamicCast<GridLayoutComponent>(component);
    gridcomponent->SetRowGap(60.0_px);
    gridcomponent->SetColumnGap(80.0_px);
    renderNode_->Update(component);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 6);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(30.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 216.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(680.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 216.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(30.0, 276.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 324.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(680.0, 276.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 324.0));
        }
        if (index == 4) {
            ASSERT_TRUE(item->GetPosition() == Offset(30.0, 660.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        }
        if (index == 5) {
            ASSERT_TRUE(item->GetPosition() == Offset(680.0, 660.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 540.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest014
 * @tc.desc: Verify Grid Layout PerformLayout can calculate children when RTL
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SM
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 4 child by RTL.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "20% 30% 70%";
    std::string colArgs = "600px 700px";
    auto component = GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs);
    auto gridcomponent = AceType::DynamicCast<GridLayoutComponent>(component);
    gridcomponent->SetRowGap(60.0_px);
    gridcomponent->SetColumnGap(80.0_px);
    gridcomponent->SetRightToLeft(true);
    renderNode_->Update(component);
    int32_t count = 4;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 4);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 4);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(510.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 216.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 216.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(510.0, 276.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 324.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 276.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 324.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest015
 * @tc.desc: Verify Grid Layout can support focus move when RTL.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SN
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 3 child when RTL.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "1fr 1fr 1fr";
    std::string colArgs = "360px 360px 360px";
    auto component = GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs);
    auto gridcomponent = AceType::DynamicCast<GridLayoutComponent>(component);
    gridcomponent->SetRightToLeft(true);
    renderNode_->Update(gridcomponent);
    RefPtr<RenderNode> item1 = GridLayoutTestUtils::CreateRenderItem(1, 0, 1, 3);
    item1->GetChildren().front()->Attach(mockContext_);
    item1->Attach(mockContext_);
    renderNode_->AddChild(item1);
    RefPtr<RenderNode> item2 = GridLayoutTestUtils::CreateRenderItem(0, 0, 1, 2);
    item2->GetChildren().front()->Attach(mockContext_);
    item2->Attach(mockContext_);
    renderNode_->AddChild(item2);
    RefPtr<RenderNode> item3 = GridLayoutTestUtils::CreateRenderItem(2, 1, 1, 2);
    item3->GetChildren().front()->Attach(mockContext_);
    item3->Attach(mockContext_);
    renderNode_->AddChild(item3);
    int32_t count = 2;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 2, 2);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));

    /**
     * @tc.steps: step2. trigger focus event.
     * @tc.expected: step2. grid layout can obtain focus.
     */
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, true) == -1); // LEFT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 3); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == -1); // RIGHT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, true) == 0); // UP
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, true) == 1); // UP
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, true) == 4); // LEFT
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 0); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(true, false) == 2); // DOWN
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, true) == -1); // LEFT
    ASSERT_TRUE(renderNode_->RequestNextFocus(false, false) == 3); // RIGHT
}

/**
 * @tc.name: RenderGridLayoutTest016
 * @tc.desc: Verify Grid Layout PerformLayout can calculate children with invalid index.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SM
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with two children with invalid index.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "50% 50%";
    std::string colArgs = "600px 700px";
    auto component = GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs);
    auto gridcomponent = AceType::DynamicCast<GridLayoutComponent>(component);
    gridcomponent->SetRowGap(60.0_px);
    gridcomponent->SetColumnGap(80.0_px);
    renderNode_->Update(component);
    RefPtr<RenderNode> item1 = GridLayoutTestUtils::CreateRenderItem(2, 0, 1, 1);
    item1->GetChildren().front()->Attach(mockContext_);
    item1->Attach(mockContext_);
    renderNode_->AddChild(item1);
    RefPtr<RenderNode> item2 = GridLayoutTestUtils::CreateRenderItem(0, 3, 1, 1);
    item2->GetChildren().front()->Attach(mockContext_);
    item2->Attach(mockContext_);
    renderNode_->AddChild(item2);
    ASSERT_TRUE(renderNode_->GetChildren().size() == 2);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 2);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(30.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(680.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 540.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest017
 * @tc.desc: Verify Grid Layout can layout items with auto params.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SP
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 3 child by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string colArgs = "1fr auto 1fr";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, "", colArgs));
    RefPtr<RenderNode> item1 = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
    item1->GetChildren().front()->Attach(mockContext_);
    item1->Attach(mockContext_);
    renderNode_->AddChild(item1);
    RefPtr<RenderNode> item2 = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
    item2->GetChildren().front()->Attach(mockContext_);
    item2->Attach(mockContext_);
    renderNode_->AddChild(item2);
    RefPtr<RenderNode> item3 = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
    item3->GetChildren().front()->Attach(mockContext_);
    item3->Attach(mockContext_);
    renderNode_->AddChild(item3);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 3);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 270.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(270.0, 540.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(270.0, 270.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(810.0, 270.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(270.0, 540.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest018
 * @tc.desc: Verify Grid Layout can layout items with auto params.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SP
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 9 child by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "auto 1fr 200px";
    std::string colArgs = "270px 50% auto";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    int32_t count = 9;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 9);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 9);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(270.0, 540.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(270.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 540.0));
        }
        if (index == 6) {
            ASSERT_TRUE(item->GetPosition() == Offset(810.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(270.0, 540.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 540.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(270.0, 340.0));
        }
        if (index == 4) {
            ASSERT_TRUE(item->GetPosition() == Offset(270.0, 540.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 340.0));
        }
        if (index == 7) {
            ASSERT_TRUE(item->GetPosition() == Offset(810.0, 540.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(270.0, 340.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 880.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(270.0, 200.0));
        }
        if (index == 5) {
            ASSERT_TRUE(item->GetPosition() == Offset(270.0, 880.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(540.0, 200.0));
        }
        if (index == 8) {
            ASSERT_TRUE(item->GetPosition() == Offset(810.0, 880.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(270.0, 200.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest019
 * @tc.desc: Verify Grid Layout is useful for auto-fill in px mode.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAN
 * @tc.author: yangfan
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 9 child by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "auto-fill 360px 180px";
    std::string colArgs = "360px 1fr 360px";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    int32_t count = 9;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 9);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 9);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 360.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 360.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 180.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 540.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 360.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 900.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 180.0));
        }
        if (index == 4) {
            ASSERT_TRUE(item->GetPosition() == Offset(360.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 360.0));
        }
        if (index == 5) {
            ASSERT_TRUE(item->GetPosition() == Offset(360.0, 360.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 180.0));
        }
        if (index == 6) {
            ASSERT_TRUE(item->GetPosition() == Offset(360.0, 540.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 360.0));
        }
        if (index == 7) {
            ASSERT_TRUE(item->GetPosition() == Offset(360.0, 900.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 180.0));
        }
        if (index == 8) {
            ASSERT_TRUE(item->GetPosition() == Offset(720.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(360.0, 360.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest020
 * @tc.desc: Verify Grid Layout is useful for auto-fill in percent mode.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAO
 * @tc.author: yangfan
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest020, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 9 child by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "auto-fill 30% 25%";
    std::string colArgs = "auto-fill 10% 30%";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::COLUMN, rowArgs, colArgs));
    int32_t count = 9;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 9);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 9);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(108.0, 324.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(108.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(324.0, 324.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(432.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(108.0, 324.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(540.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(324.0, 324.0));
        }
        if (index == 4) {
            ASSERT_TRUE(item->GetPosition() == Offset(864.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(108.0, 324.0));
        }
        if (index == 5) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 324.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(108.0, 270.0));
        }
        if (index == 6) {
            ASSERT_TRUE(item->GetPosition() == Offset(108.0, 324.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(324.0, 270.0));
        }
        if (index == 7) {
            ASSERT_TRUE(item->GetPosition() == Offset(432.0, 324.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(108.0, 270.0));
        }
        if (index == 8) {
            ASSERT_TRUE(item->GetPosition() == Offset(540.0, 324.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(324.0, 270.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridLayoutTest021
 * @tc.desc: Verify Grid Layout is useful for auto-fill in percent mode when percent and px is mixed.
 * @tc.type: FUNC
 * @tc.require: AR000F3BAO
 * @tc.author: yangfan
 */
HWTEST_F(RenderGridLayoutTest, RenderGridLayoutTest021, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct component and render with 7 child by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::string rowArgs = "auto-fill 30% 200px";
    std::string colArgs = "auto-fill 400px 10%";
    renderNode_->Update(GridLayoutTestUtils::CreateComponent(FlexDirection::ROW, rowArgs, colArgs));
    int32_t count = 7;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderNode> item = GridLayoutTestUtils::CreateRenderItem(-1, -1, 1, 1);
        item->GetChildren().front()->Attach(mockContext_);
        item->Attach(mockContext_);
        renderNode_->AddChild(item);
    }
    ASSERT_TRUE(renderNode_->GetChildren().size() == 7);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderNode_->PerformLayout();
    ASSERT_TRUE(renderNode_->GetLayoutSize() == Size(1080.0, 1080.0));
    const std::list<RefPtr<RenderNode>>& items = renderNode_->GetChildren();
    ASSERT_TRUE(items.size() == 7);
    int32_t index = 0;
    for (const auto& item : items) {
        GridLayoutTestUtils::PrintNodeInfo(item);
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 324.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 324.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 200.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 524.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 324.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 848.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 200.0));
        }
        if (index == 4) {
            ASSERT_TRUE(item->GetPosition() == Offset(400.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(108.0, 324.0));
        }
        if (index == 5) {
            ASSERT_TRUE(item->GetPosition() == Offset(400.0, 324.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(108.0, 200.0));
        }
        if (index == 6) {
            ASSERT_TRUE(item->GetPosition() == Offset(400.0, 524.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(108.0, 324.0));
        }
        index++;
    }
}

} // namespace OHOS::Ace
