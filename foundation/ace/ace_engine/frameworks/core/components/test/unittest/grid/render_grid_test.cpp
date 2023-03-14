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

#include "core/animation/curves.h"
#include "core/components/grid/grid_component.h"
#include "core/components/grid/render_grid.h"
#include "core/components/test/unittest/grid/grid_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class RenderGridTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    RefPtr<PipelineContext> mockContext_;
    RefPtr<RenderGrid> renderGrid_;
    int32_t index_ = 0;
};

void RenderGridTest::SetUpTestCase() {}
void RenderGridTest::TearDownTestCase() {}

void RenderGridTest::SetUp()
{
    mockContext_ = MockRenderCommon::GetMockContext();
    renderGrid_ = AceType::MakeRefPtr<RenderGrid>();
    renderGrid_->Attach(mockContext_);
}

void RenderGridTest::TearDown()
{
    mockContext_ = nullptr;
    renderGrid_ = nullptr;
}

/**
 * @tc.name: RenderGridTest001
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row direction and one column.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row direction and one column.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW);
    gridComponent->SetFlexAlign(FlexAlign::FLEX_START);
    gridComponent->SetColumnCount(1);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 1);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(1000.0, 1000.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(index * 400.0, 0.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 400.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-500.0, -500.0) and offset(500.0, 500.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-500.0, -500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(index * 400.0, 0.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(500.0, 500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(index * 400.0 - 500.0, 0.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest002
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column direction and one column.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column direction and one column.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetFlexAlign(FlexAlign::CENTER);
    gridComponent->SetColumnCount(1);
    gridComponent->SetColumnExtent(200.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 1);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(500.0, 800.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(500.0, 800.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(50.0, index * 400.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 400.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(500.0, 500.0) and offset(-500.0, -500.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(500.0, 500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(50.0, index * 400.0 - 500.0));
        index++;
    }

    renderGrid_->UpdateOffset(Offset(-500.0, -500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(50.0, index * 400.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest003
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row reverse direction and one column.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row reverse direction and one column.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW_REVERSE);
    gridComponent->SetColumnCount(1);
    gridComponent->SetWidth(800.0);
    gridComponent->SetHeight(300.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 1);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(800.0, 300.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(800.0 - 400.0 - index * 400.0, 0.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 300.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-2000.0, -2000.0) and offset(2000.0, 2000.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-2000.0, -2000.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(800.0 - 400.0 - index * 400.0 + 1600.0, 0.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(2000.0, 2000.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(800.0 - 400.0 - index * 400.0, 0.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest004
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column reverse direction and one column.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column reverse direction and one column.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN_REVERSE);
    gridComponent->SetColumnCount(1);
    gridComponent->SetWidth(300.0);
    gridComponent->SetHeight(300.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 1);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(500.0, 500.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(300.0, 300.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(0.0, 300.0 - 400.0 - 400.0 * index));
        ASSERT_TRUE(item->GetLayoutSize() == Size(300.0, 400.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-1500.0, -1500.0) and offset(1500.0, 1500.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-1500.0, -1500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(0.0, 300.0 - 400.0 - 400.0 * index + 1500.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(1500.0, 1500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(0.0, 300.0 - 400.0 - 400.0 * index));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest005
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row direction and two columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row direction and two columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW);
    gridComponent->SetColumnCount(2);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 2);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(800.0, 800.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(800.0, 800.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(index / 2 * 400.0, (index % 2) * 400.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 400.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-200.0, -200.0) and offset(200.0, 200.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-200.0, -200.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(index / 2 * 400.0, (index % 2) * 400.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(200.0, 200.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(index / 2 * 400.0 - 200.0, (index % 2) * 400.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest006
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column direction and two columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column direction and two columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetColumnCount(2);
    gridComponent->SetColumnExtent(180.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 2);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(700.0, 700.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(700.0, 700.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 2) * 350.0, index / 2 * 400.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(350.0, 400.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(500.0, 500.0) and offset(-500.0, -500.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(500.0, 500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 2) * 350.0, index / 2 * 400.0 - 500.0));
        index++;
    }

    renderGrid_->UpdateOffset(Offset(-500.0, -500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 2) * 350.0, index / 2 * 400.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest007
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row reverse direction and two columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row reverse direction and two columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW_REVERSE);
    gridComponent->SetColumnCount(2);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(1200.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 2);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(200.0, 200.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(200.0, 200.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(200.0 - 400.0 - (index / 2) * 400.0, (index % 2) * 100.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 100.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-1000.0, -1000.0) and offset(1000.0, 1000.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-1000.0, -1000.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(-200.0 - (index / 2) * 400.0 + 1000.0, (index % 2) * 100.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(1000.0, 1000.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(-200.0 - (index / 2) * 400.0, (index % 2) * 100.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest008
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column reverse direction and two columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column reverse direction and two columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN_REVERSE);
    gridComponent->SetColumnCount(2);
    gridComponent->SetWidth(1000.0);
    gridComponent->SetHeight(350.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 2);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(350.0, 1000.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(350.0, 350.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 2) * 175, 350.0 - 400.0 - (index / 2) * 400.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(175, 400.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-2500.0, -2500.0) and offset(2500.0, 2500.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-2500.0, -2500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 2) * 175, -50.0 - (index / 2) * 400.0 + 850.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(2500.0, 2500.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 2) * 175, -50.0 - (index / 2) * 400.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest009
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row direction and 5 columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row direction and 5 columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW);
    gridComponent->SetFlexAlign(FlexAlign::FLEX_END);
    gridComponent->SetColumnCount(5);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 5);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(200.0, 100.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(600.0, 600.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(600.0, 600.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index / 5) * 200.0, 20.0 + (index % 5) * 120.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(200.0, 100.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-50.0, -50.0) and offset(50.0, 50.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-50.0, -50.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index / 5) * 200.0, 20.0 + (index % 5) * 120.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(50.0, 50.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index / 5) * 200.0, 20.0 + (index % 5) * 120.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest010
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column direction and 5 columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column direction and 5 columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetColumnCount(5);
    gridComponent->SetColumnExtent(2222);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 5);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(200.0, 100.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 150.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(1000.0, 150.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 5) * 200.0, index / 5 * 100.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(200.0, 100.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(50.0, 50.0) and offset(-50.0, -50.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(50.0, 50.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 5) * 200.0, index / 5 * 100.0 - 50.0));
        index++;
    }

    renderGrid_->UpdateOffset(Offset(-50.0, -50.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 5) * 200.0, index / 5 * 100.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest011
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row reverse direction and 5 columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row reverse direction and 5 columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW_REVERSE);
    gridComponent->SetColumnCount(5);
    gridComponent->SetWidth(999.0);
    gridComponent->SetHeight(888.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 5);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(100.0, 200.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(190.0, 850.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(190.0, 850.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(190.0 - 100.0 - (index / 5) * 100.0, (index % 5) * 170.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(100.0, 170.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-1111, -1111) and offset(1111, 1111).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-1111.0, -1111.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(90.0 - (index / 5) * 100.0 + 10.0, (index % 5) * 170.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(1111.0, 1111.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(190.0 - 100.0 - (index / 5) * 100.0, (index % 5) * 170.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest012
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column reverse direction and 5 columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column reverse direction and 5 columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN_REVERSE);
    gridComponent->SetColumnCount(5);
    gridComponent->SetWidth(800.0);
    gridComponent->SetHeight(50.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 5);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(100.0, 200.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(450.0, 777.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(450.0, 50.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 5) * 90.0, 50.0 - 200.0 - (index / 5) * 200.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(90.0, 200.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-300.0, -300.0) and offset(300.0, 300.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-300.0, -300.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 5) * 90.0, -150.0 - (index / 5) * 200.0 + 300.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(300.0, 300.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 5) * 90.0, -150.0 - (index / 5) * 200.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest013
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row direction and 999 extent.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row direction and 999 extent.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW);
    gridComponent->SetFlexAlign(FlexAlign::FLEX_START);
    gridComponent->SetColumnExtent(999);
    gridComponent->SetWidth(800.0);
    gridComponent->SetHeight(1000.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 999);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 20;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(300.0, 300.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 20.0);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(800.0, 1000.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 20.0);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index / 2) * 300.0, (index % 2) * 500.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(300.0, 300.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-150.0, -150.0) and offset(150.0, 150.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-150.0, -150.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index / 2) * 300.0, (index % 2) * 500.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(150.0, 150.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index / 2) * 300.0 - 150.0, (index % 2) * 500.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest014
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column direction and 700.0 extent.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column direction and 700.0 extent.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetColumnExtent(700.0);
    gridComponent->SetWidth(1000.0);
    gridComponent->SetHeight(900.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 700.0);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 10;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(800.0, 200.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 10.0);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(700.0, 900.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(700.0, 900.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 10.0);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(0.0, index * 200.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(700.0, 200.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(1100.0, 1100.0) and offset(-1100.0, -1100.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(1100.0, 1100.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(0.0, index * 200.0 - 1100.0));
        index++;
    }

    renderGrid_->UpdateOffset(Offset(-1100.0, -1100.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(0.0, index * 200.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest015
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row reverse direction and 301 extent.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row reverse direction and 301 extent.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW_REVERSE);
    gridComponent->SetFlexAlign(FlexAlign::STRETCH);
    gridComponent->SetColumnExtent(301);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(900.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 301);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 15;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(200.0, 200.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 15);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(900.0, 900.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 15);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(900.0 - 200.0 - index / 3 * 200.0, (index % 3) * 300.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(200.0, 300.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-200.0, -200.0) and offset(200.0, 200.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-200.0, -200.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(700.0 - index / 3 * 200.0 + 100.0, (index % 3) * 300.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(200.0, 200.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(700.0 - index / 3 * 200.0, (index % 3) * 300.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest016
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column reverse direction and 319 extent.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column reverse direction and 319 extent.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN_REVERSE);
    gridComponent->SetColumnExtent(319);
    gridComponent->SetWidth(960.0);
    gridComponent->SetHeight(960.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 319);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 16;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(240.0, 240.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 16);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(960.0, 960.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 16);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 4) * 240.0, 960.0 - 240.0 - 240.0 * (index / 4)));
        ASSERT_TRUE(item->GetLayoutSize() == Size(240.0, 240.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-9999, -9999) and offset(9999, 9999).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-9999.0, -9999.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 4) * 240.0, 720.0 - 240.0 * (index / 4)));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(9999.0, 9999.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 4) * 240.0, 720.0 - 240.0 * (index / 4)));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest017
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row direction and two columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row direction and two columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW);
    gridComponent->SetColumnExtent(190.0);
    gridComponent->SetWidth(899.0);
    gridComponent->SetHeight(950.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 190.0);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 25;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(180.0, 200.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 25);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(899.0, 950.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 25);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(index / 5 * 180.0, (index % 5) * 190.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(180.0, 190.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-1, -1) and offset(1, 1).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-1.0, -1.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(index / 5 * 180.0, (index % 5) * 190.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(1.0, 1.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(index / 5 * 180.0 - 1, (index % 5) * 190.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest018
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column direction and 179 extent.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column direction and 179 extent.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetColumnExtent(179);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(1201.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 179);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 36;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(222.0, 200.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 36);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1024.0, 2048.0));
    renderGrid_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(renderGrid_->NeedLayout());
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(900.0, 1201));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 36);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 6) * 150.0, index / 6 * 200.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(150.0, 200.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(1, 1) and offset(-1, -1).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(1.0, 1.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 6) * 150.0, index / 6 * 200.0));
        index++;
    }

    renderGrid_->UpdateOffset(Offset(-1.0, -1.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 6) * 150.0, index / 6 * 200.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest019
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row reverse direction and two columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row reverse direction and two columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW_REVERSE);
    gridComponent->SetFlexAlign(FlexAlign::FLEX_START);
    gridComponent->SetColumnExtent(233);
    gridComponent->SetWidth(700.0);
    gridComponent->SetHeight(1400.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 233);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 6;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(888.0, 100.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 6);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(700.0, 1400.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 6);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(700.0 - 888 - (index / 7) * 888, (index % 7) * 200.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(888, 100.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-888, -888) and offset(888, 888).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-888.0, -888.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(-188.0 - (index / 7) * 888.0 + 188.0, (index % 7) * 200.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(888.0, 888.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset(-188.0 - (index / 7) * 888.0, (index % 7) * 200.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest020
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column reverse direction and 125 extent.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest020, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column reverse direction and 125 extent.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN_REVERSE);
    gridComponent->SetColumnExtent(125);
    gridComponent->SetWidth(1000.0);
    gridComponent->SetHeight(1200.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 125);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    int32_t count = 32;
    for (int32_t i = 0; i < count; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(150.0, 300.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 32);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(1000.0, 1200.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 32);
    int32_t index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 8) * 125, 1200.0 - 300.0 - (index / 8) * 300.0));
        ASSERT_TRUE(item->GetLayoutSize() == Size(125.0, 300.0));
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-10.0, -10.0) and offset(10.0, 10.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-10.0, -10.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 8) * 125.0, 1200.0 - 300.0 - (index / 8) * 300.0));
        index++;
    }
    renderGrid_->UpdateOffset(Offset(10.0, 10.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 8) * 125.0, 1200.0 - 300.0 - (index / 8) * 300.0));
        index++;
    }
}

/**
 * @tc.name: RenderGridTest021
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row direction and 400.0 extent.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest021, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row direction and 400.0 extent.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW);
    gridComponent->SetFlexAlign(FlexAlign::CENTER);
    gridComponent->SetColumnExtent(400.0);
    gridComponent->SetWidth(400.0);
    gridComponent->SetHeight(400.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 400.0);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    RefPtr<RenderBox> firstBox = GridTestUtils::CreateRenderBox(100.0, 100.0);
    firstBox->Attach(mockContext_);
    renderGrid_->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = GridTestUtils::CreateRenderBox(200.0, 200.0);
    secondBox->Attach(mockContext_);
    renderGrid_->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = GridTestUtils::CreateRenderBox(400.0, 400.0);
    thirdBox->Attach(mockContext_);
    renderGrid_->AddChild(thirdBox);
    RefPtr<RenderBox> fourthBox = GridTestUtils::CreateRenderBox(800.0, 800.0);
    fourthBox->Attach(mockContext_);
    renderGrid_->AddChild(fourthBox);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 4);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(400.0, 400.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 4);
    int32_t index = 0;
    for (const auto& item : items) {
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 150.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(100.0, 100.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(300.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(400.0, 400.0));
        }
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(1100.0, 1100.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(1100.0, 1100.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(100.0 - 1100.0, 100.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(200.0, 200.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(700.0 - 1100.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(800.0, 400.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridTest022
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column direction and 300.0 extent.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest022, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column direction and 300.0 extent.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetColumnExtent(300.0);
    gridComponent->SetWidth(600.0);
    gridComponent->SetHeight(600.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 300.0);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    RefPtr<RenderBox> firstBox = GridTestUtils::CreateRenderBox(400.0, 100.0);
    firstBox->Attach(mockContext_);
    renderGrid_->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = GridTestUtils::CreateRenderBox(500.0, 700.0);
    secondBox->Attach(mockContext_);
    renderGrid_->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = GridTestUtils::CreateRenderBox(600.0, 200.0);
    thirdBox->Attach(mockContext_);
    renderGrid_->AddChild(thirdBox);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 3);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(600.0, 600.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 3);
    int32_t index = 0;
    for (const auto& item : items) {
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(300.0, 100.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(300.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(300.0, 700.0));
        }
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(101, 101).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(101.0, 101.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 100.0 - 100.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(300.0, 200.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridTest023
 * @tc.desc: Verify Grid PerformLayout can calculate six children with row reverse direction and 200.0 extent.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest023, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by row reverse direction and 200.0 extent.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW_REVERSE);
    gridComponent->SetFlexAlign(FlexAlign::FLEX_END);
    gridComponent->SetColumnExtent(200.0);
    gridComponent->SetWidth(400.0);
    gridComponent->SetHeight(400.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 200.0);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    RefPtr<RenderBox> firstBox = GridTestUtils::CreateRenderBox(100.0, 150.0);
    firstBox->Attach(mockContext_);
    renderGrid_->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = GridTestUtils::CreateRenderBox(250.0, 200.0);
    secondBox->Attach(mockContext_);
    renderGrid_->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = GridTestUtils::CreateRenderBox(50.0, 250.0);
    thirdBox->Attach(mockContext_);
    renderGrid_->AddChild(thirdBox);
    RefPtr<RenderBox> fourthBox = GridTestUtils::CreateRenderBox(150.0, 300.0);
    fourthBox->Attach(mockContext_);
    renderGrid_->AddChild(fourthBox);
    RefPtr<RenderBox> fifthBox = GridTestUtils::CreateRenderBox(60.0, 350.0);
    fifthBox->Attach(mockContext_);
    renderGrid_->AddChild(fifthBox);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 5);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(400.0, 400.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 5);
    int32_t index = 0;
    for (const auto& item : items) {
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(300.0, 50.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(100.0, 150.0));
        }
        if (index == 4) {
            ASSERT_TRUE(item->GetPosition() == Offset(190.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(60.0, 200.0));
        }
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-1.0, -1.0) .
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-1.0, -1.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 200.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(150.0, 200.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(150.0, 200.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(250.0, 200.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridTest024
 * @tc.desc: Verify Grid PerformLayout can calculate six children with column reverse direction and 5 columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest024, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with no child by column reverse direction and 5 columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN_REVERSE);
    gridComponent->SetFlexAlign(FlexAlign::STRETCH);
    gridComponent->SetColumnExtent(500.0);
    gridComponent->SetTotalCount(20.0);
    gridComponent->SetOnRequestItem(EventMarker { "1-1" });
    gridComponent->SetCachedCount(5);
    gridComponent->SetWidth(1000.0);
    gridComponent->SetHeight(800.0);
    ASSERT_TRUE(gridComponent->GetColumnExtent() == 500.0);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    RefPtr<RenderBox> firstBox = GridTestUtils::CreateRenderBox(400.0, 500.0);
    firstBox->Attach(mockContext_);
    renderGrid_->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = GridTestUtils::CreateRenderBox(300.0, 600.0);
    secondBox->Attach(mockContext_);
    renderGrid_->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = GridTestUtils::CreateRenderBox(200.0, 700.0);
    thirdBox->Attach(mockContext_);
    renderGrid_->AddChild(thirdBox);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 3);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(1000.0, 800.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 3);
    int32_t index = 0;
    for (const auto& item : items) {
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 300.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(500.0, 500.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(500.0, 200.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(500.0, 600.0));
        }
        index++;
    }

    /**
     * @tc.steps: step4. Trigger a slide event with offset(-400.0, -400.0).
     * @tc.expected: step4. Children's position no change.
     */
    renderGrid_->UpdateOffset(Offset(-400.0, -400.0));
    renderGrid_->PerformLayout();
    index = 0;
    for (const auto& item : items) {
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, -400.0 + 400.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridTest025
 * @tc.desc: Verify Grid PerformLayout can calculate multi children(span 2) by row direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest025, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with multi children(span 2) by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW);
    gridComponent->SetColumnCount(2);
    gridComponent->SetWidth(500.0);
    gridComponent->SetHeight(500.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 2);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    RefPtr<RenderBox> firstItem = GridTestUtils::CreateRenderItem(200.0, 500.0, 2, mockContext_);
    firstItem->Attach(mockContext_);
    renderGrid_->AddChild(firstItem);
    RefPtr<RenderBox> secondItem = GridTestUtils::CreateRenderItem(200.0, 500.0, 1, mockContext_);
    secondItem->Attach(mockContext_);
    renderGrid_->AddChild(secondItem);
    RefPtr<RenderBox> thirdItem = GridTestUtils::CreateRenderItem(200.0, 500.0, 1, mockContext_);
    thirdItem->Attach(mockContext_);
    renderGrid_->AddChild(thirdItem);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 3);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(500.0, 500.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 3);
    int32_t index = 0;
    for (const auto& item : items) {
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(200.0, 500.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(200.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(200.0, 250.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(200.0, 250.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(200.0, 250.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridTest026
 * @tc.desc: Verify Grid PerformLayout can calculate multi children(span 3) by column direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest026, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with multi children(span 3) by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetColumnCount(4);
    gridComponent->SetWidth(600.0);
    gridComponent->SetHeight(600.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 4);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    RefPtr<RenderBox> firstItem = GridTestUtils::CreateRenderItem(600.0, 200.0, 3, mockContext_);
    firstItem->Attach(mockContext_);
    renderGrid_->AddChild(firstItem);
    RefPtr<RenderBox> secondItem = GridTestUtils::CreateRenderItem(600.0, 200.0, 3, mockContext_);
    secondItem->Attach(mockContext_);
    renderGrid_->AddChild(secondItem);
    RefPtr<RenderBox> thirdItem = GridTestUtils::CreateRenderItem(600.0, 200.0, 1, mockContext_);
    thirdItem->Attach(mockContext_);
    renderGrid_->AddChild(thirdItem);
    RefPtr<RenderBox> fourthItem = GridTestUtils::CreateRenderItem(600.0, 200.0, 3, mockContext_);
    fourthItem->Attach(mockContext_);
    renderGrid_->AddChild(fourthItem);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 4);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(600.0, 600.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 4);
    int32_t index = 0;
    for (const auto& item : items) {
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(450.0, 200.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(450.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(150.0, 200.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 200.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(150.0, 200.0));
        }
        if (index == 3) {
            ASSERT_TRUE(item->GetPosition() == Offset(150.0, 200.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(450.0, 200.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridTest027
 * @tc.desc: Verify Grid PerformLayout can calculate multi children(span 4) by row reverse direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest027, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with multi children(span 4) by row reverse direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW_REVERSE);
    gridComponent->SetFlexAlign(FlexAlign::STRETCH);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(900.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 3);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    RefPtr<RenderBox> firstItem = GridTestUtils::CreateRenderItem(500.0, 400.0, 1, mockContext_);
    firstItem->Attach(mockContext_);
    renderGrid_->AddChild(firstItem);
    RefPtr<RenderBox> secondItem = GridTestUtils::CreateRenderItem(500.0, 400.0, 4, mockContext_);
    secondItem->Attach(mockContext_);
    renderGrid_->AddChild(secondItem);
    RefPtr<RenderBox> thirdItem = GridTestUtils::CreateRenderItem(500.0, 400.0, 4, mockContext_);
    thirdItem->Attach(mockContext_);
    renderGrid_->AddChild(thirdItem);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 3);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(900.0, 900.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 3);
    int32_t index = 0;
    for (const auto& item : items) {
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(400.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(500.0, 300.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(400.0, 300.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(500.0, 600.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(-100.0, 0.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(500.0, 900.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridTest028
 * @tc.desc: Verify Grid PerformLayout can calculate multi children(span 3) by column reverse direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest028, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct GridComponent with multi children(span 3) by column reverse direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN_REVERSE);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(750.0);
    gridComponent->SetHeight(750.0);
    ASSERT_TRUE(gridComponent->GetColumnCount() == 3);

    /**
     * @tc.steps: step2. construct RenderGrid with grid component and add six children.
     * @tc.expected: step2. properties and children are set correctly.
     */
    renderGrid_->Update(gridComponent);
    RefPtr<RenderBox> firstItem = GridTestUtils::CreateRenderItem(800.0, 400.0, 3, mockContext_);
    firstItem->Attach(mockContext_);
    renderGrid_->AddChild(firstItem);
    RefPtr<RenderBox> secondItem = GridTestUtils::CreateRenderItem(800.0, 400.0, 1, mockContext_);
    secondItem->Attach(mockContext_);
    renderGrid_->AddChild(secondItem);
    RefPtr<RenderBox> thirdItem = GridTestUtils::CreateRenderItem(800.0, 400.0, 3, mockContext_);
    thirdItem->Attach(mockContext_);
    renderGrid_->AddChild(thirdItem);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 3);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(750.0, 750.0));
    const std::list<RefPtr<RenderNode>>& items = renderGrid_->GetChildren();
    ASSERT_TRUE(items.size() == 3);
    int32_t index = 0;
    for (const auto& item : items) {
        if (index == 0) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, 350.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(750.0, 400.0));
        }
        if (index == 1) {
            ASSERT_TRUE(item->GetPosition() == Offset(0.0, -50.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(250.0, 400.0));
        }
        if (index == 2) {
            ASSERT_TRUE(item->GetPosition() == Offset(250.0, -50.0));
            ASSERT_TRUE(item->GetLayoutSize() == Size(500.0, 400.0));
        }
        index++;
    }
}

/**
 * @tc.name: RenderGridTest029
 * @tc.desc: Verify Grid JumpTo and AnimateTo API when row direction and two columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest029, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create RenderGrid with 10 child by row direction and two columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW);
    gridComponent->SetColumnCount(2);
    gridComponent->SetWidth(800.0);
    gridComponent->SetHeight(800.0);
    gridComponent->GetGridController()->SetInitialIndex(2);
    renderGrid_->Update(gridComponent);
    for (int32_t i = 0; i < 10; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(gridComponent->GetGridController()->GetInitialIndex() == 2);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 10);
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 2);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 400.0));
    int32_t index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset(index / 2 * 400.0 - 400.0, (index % 2) * 400.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(600.0);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 2);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 600.0));
    index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset(index / 2 * 400.0 - 600.0, (index % 2) * 400.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(9);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 6);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 1200.0));
    index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset(index / 2 * 400.0 - 1200.0, (index % 2) * 400.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(0.0);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 0);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 0.0));
}

/**
 * @tc.name: RenderGridTest030
 * @tc.desc: Verify Grid JumpTo and AnimateTo API when column direction and three columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest030, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create RenderGrid with 10 child by row direction and three columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(900.0);
    gridComponent->GetGridController()->SetInitialIndex(3);
    renderGrid_->Update(gridComponent);
    for (int32_t i = 0; i < 10; ++i) {
        RefPtr<RenderBox> box = GridTestUtils::CreateRenderBox(300.0, 450.0);
        box->Attach(mockContext_);
        renderGrid_->AddChild(box);
    }
    ASSERT_TRUE(gridComponent->GetGridController()->GetInitialIndex() == 3);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 10);
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 3);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 450.0));
    int32_t index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 3) * 300.0, index / 3 * 450.0 - 450.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(899.0);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 3);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 899.0));
    index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 3) * 300.0, index / 3 * 450.0 - 899.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(9);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 6);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 900.0));
    index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 3) * 300.0, index / 3 * 450.0 - 900.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(0.0);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 0);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 0.0));
}

/**
 * @tc.name: RenderGridTest031
 * @tc.desc: Verify Grid JumpTo and AnimateTo API when row reverse direction and four columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest031, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create RenderGrid with 10 child by row reverse direction and four columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW_REVERSE);
    gridComponent->SetColumnCount(4);
    gridComponent->SetWidth(1000.0);
    gridComponent->SetHeight(1000.0);
    gridComponent->GetGridController()->SetInitialIndex(4);
    renderGrid_->Update(gridComponent);
    for (int32_t i = 0; i < 10; ++i) {
        renderGrid_->AddChild(GridTestUtils::CreateRenderItem(500.0, 250.0, 3, mockContext_));
    }
    ASSERT_TRUE(gridComponent->GetGridController()->GetInitialIndex() == 4);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 10);
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 4);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), -1000.0));
    int32_t index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset(500.0 - index / 2 * 500.0 + 1000.0, (index % 2) * 750.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(-1499.0);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 4);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), -1499.0));
    index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset(500.0 - index / 2 * 500.0 + 1499.0, (index % 2) * 750.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(9);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 6);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), -1500.0));
    index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset(500.0 - index / 2 * 500.0 + 1500.0, (index % 2) * 750.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(0.0);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 0);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 0.0));
}

/**
 * @tc.name: RenderGridTest032
 * @tc.desc: Verify Grid JumpTo and AnimateTo API when column reverse direction and three columns.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest032, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create RenderGrid with 10 child by column reverse direction and three columns.
     * @tc.expected: step1. properties and children are set correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN_REVERSE);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(900.0);
    gridComponent->GetGridController()->SetInitialIndex(3);
    renderGrid_->Update(gridComponent);
    for (int32_t i = 0; i < 10; ++i) {
        renderGrid_->AddChild(GridTestUtils::CreateRenderItem(300.0, 450.0, 2, mockContext_));
    }
    ASSERT_TRUE(gridComponent->GetGridController()->GetInitialIndex() == 3);

    /**
     * @tc.steps: step2. Verify that the properties are calculated correctly.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 10);
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 2);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), -450.0));
    int32_t index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 2) * 600.0, 450.0 - index / 2 * 450.0 + 450.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(-901.0);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 4);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), -901.0));
    index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 2) * 600.0, 450.0 - index / 2 * 450.0 + 901.0));
        index++;
    }
    gridComponent->GetGridController()->AnimateTo(0, 200.0, Curves::LINEAR);
    gridComponent->GetGridController()->AnimateTo(0.0, 300.0, Curves::SINE);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 4);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), -901.0));
    index = 0;
    for (const auto& item : renderGrid_->GetChildren()) {
        ASSERT_TRUE(item->GetPosition() == Offset((index % 2) * 600.0, 450.0 - index / 2 * 450.0 + 901.0));
        index++;
    }
    gridComponent->GetGridController()->JumpTo(0.0);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(gridComponent->GetGridController()->GetCurrentIndex() == 0);
    ASSERT_TRUE(NearEqual(gridComponent->GetGridController()->GetCurrentPosition(), 0.0));
}

/**
 * @tc.name: RenderGridTest033
 * @tc.desc: Verify Grid Scroll Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest033, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive Scroll event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "grid_scroll_id";
    index_ = 0;
    RefPtr<TestGridEventHander> eventHander = AceType::MakeRefPtr<TestGridEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string scrollResult = std::string("\"scroll\",{\"scrollX\":")
                .append(std::to_string(450.0 * (index_ % 3)))
                .append(",\"scrollY\":")
                .append(std::to_string(450.0 * (index_ % 3)))
                .append(",\"scrollState\":")
                .append(std::to_string(index_ % 3))
                .append("},null");
            EXPECT_EQ(param, scrollResult);
            EXPECT_EQ(eventId, scrollEventId);
            index_++;
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderGrid and add 10 child by row Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(900.0);
    gridComponent->SetOnScroll(EventMarker(scrollEventId));
    renderGrid_->Update(gridComponent);
    for (int32_t i = 0; i < 10; ++i) {
        renderGrid_->AddChild(GridTestUtils::CreateRenderItem(300.0, 450.0, 2, mockContext_));
    }

    /**
     * @tc.steps: step2. Trigger scroll event by call grid API.
     * @tc.expected: step2. scroll event triggered success.
     */
    renderGrid_->PerformLayout();
    renderGrid_->HandleTouchDown();
    renderGrid_->UpdateOffset(Offset(450.0, 450.0));
    renderGrid_->HandleTouchUp();
    renderGrid_->UpdateOffset(Offset(900.0, 900.0));
    renderGrid_->HandleScrollEnd();
}

/**
 * @tc.name: RenderGridTest034
 * @tc.desc: Verify Grid ScrollBottom Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest034, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive ScrollBottom event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "grid_scrollbottom_id";
    index_ = 0;
    RefPtr<TestGridEventHander> eventHander = AceType::MakeRefPtr<TestGridEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string ret = std::string("\"scrollbottom\",null");
            EXPECT_EQ(param, ret);
            EXPECT_EQ(eventId, scrollEventId);
            index_++;
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderGrid and add 10 child by COLUMN Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(900.0);
    gridComponent->SetOnScrollBottom(EventMarker(scrollEventId));
    renderGrid_->Update(gridComponent);
    for (int32_t i = 0; i < 10; ++i) {
        renderGrid_->AddChild(GridTestUtils::CreateRenderItem(300.0, 450.0, 1, mockContext_));
    }

    /**
     * @tc.steps: step2. Trigger scrollbottom event by call grid API.
     * @tc.expected: step2. scrollbottom event triggered success.
     */
    renderGrid_->PerformLayout();
    renderGrid_->UpdateOffset(Offset(500.0, 500.0));
    renderGrid_->UpdateOffset(Offset(1350.0, 1350.0));
    renderGrid_->HandleScrollEnd();
}

/**
 * @tc.name: RenderGridTest035
 * @tc.desc: Verify Grid ScrollTop Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest035, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive ScrollTop event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "grid_scrolltop_id";
    index_ = 0;
    RefPtr<TestGridEventHander> eventHander = AceType::MakeRefPtr<TestGridEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string ret = std::string("\"scrolltop\",null");
            EXPECT_EQ(param, ret);
            EXPECT_EQ(eventId, scrollEventId);
            index_++;
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderGrid and add 10 child by ROW_REVERSE Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::ROW_REVERSE);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(900.0);
    gridComponent->SetOnScrollTop(EventMarker(scrollEventId));
    renderGrid_->Update(gridComponent);
    for (int32_t i = 0; i < 10; ++i) {
        renderGrid_->AddChild(GridTestUtils::CreateRenderItem(300.0, 450.0, 1, mockContext_));
    }

    /**
     * @tc.steps: step2. Trigger scrolltop event by call grid API.
     * @tc.expected: step2. scrolltop event triggered success.
     */
    renderGrid_->PerformLayout();
    renderGrid_->UpdateOffset(Offset(-100.0, -100.0));
    renderGrid_->UpdateOffset(Offset(100.0, 100.0));
    renderGrid_->HandleScrollEnd();
}

/**
 * @tc.name: RenderGridTest036
 * @tc.desc: Verify Grid ScrollEnd Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest036, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive ScrollEnd event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "grid_scrollend_id";
    index_ = 0;
    RefPtr<TestGridEventHander> eventHander = AceType::MakeRefPtr<TestGridEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string ret = std::string("\"scrollend\",null");
            EXPECT_EQ(param, ret);
            EXPECT_EQ(eventId, scrollEventId);
            index_++;
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderGrid and add 10 child by COLUMN_REVERSE Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN_REVERSE);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(900.0);
    gridComponent->SetOnScrollEnd(EventMarker(scrollEventId));
    renderGrid_->Update(gridComponent);
    for (int32_t i = 0; i < 10; ++i) {
        renderGrid_->AddChild(GridTestUtils::CreateRenderItem(300.0, 450.0, 1, mockContext_));
    }

    /**
     * @tc.steps: step2. Trigger scrollend event by call grid API.
     * @tc.expected: step2. scrollend event triggered success.
     */
    renderGrid_->PerformLayout();
    renderGrid_->HandleScrollEnd();
}

/**
 * @tc.name: RenderGridTest037
 * @tc.desc: Verify Grid ScrollTouchUp Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest037, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive ScrollTouchUp event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "grid_scrolltouchup_id";
    index_ = 0;
    RefPtr<TestGridEventHander> eventHander = AceType::MakeRefPtr<TestGridEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string ret = std::string("\"scrolltouchup\",null");
            EXPECT_EQ(param, ret);
            EXPECT_EQ(eventId, scrollEventId);
            index_++;
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderGrid and add 10 child by COLUMN Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(FlexDirection::COLUMN);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(900.0);
    gridComponent->SetHeight(900.0);
    gridComponent->SetOnScrollTouchUp(EventMarker(scrollEventId));
    renderGrid_->Update(gridComponent);
    for (int32_t i = 0; i < 10; ++i) {
        renderGrid_->AddChild(GridTestUtils::CreateRenderItem(300.0, 450.0, 1, mockContext_));
    }

    /**
     * @tc.steps: step2. Trigger scrolltouchup event by call grid API.
     * @tc.expected: step2. scrolltouchup event triggered success.
     */
    renderGrid_->PerformLayout();
    renderGrid_->HandleTouchDown();
    renderGrid_->UpdateOffset(Offset(450.0, 450.0));
    renderGrid_->HandleTouchUp();
    renderGrid_->UpdateOffset(Offset(900.0, 900.0));
}

/**
 * @tc.name: RenderGridTest038
 * @tc.desc: Verify Grid can obtain and lost focus when row direction.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5P
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest038, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderGrid with multi children by row direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    renderGrid_ = GridTestUtils::InitRenderGridDiffSpan(FlexDirection::ROW, mockContext_);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(750.0, 750.0));
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == -1);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 9);

    /**
     * @tc.steps: step2. Trigger Focus Move RIGHT event to Grid.
     * @tc.expected: step2. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(-150.0, 150.0)) == 0); // RIGHT
    renderGrid_->HandleOnFocus(0);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(150.0, 150.0)) == 2); // RIGHT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(450.0, 150.0)) == 5); // RIGHT
    renderGrid_->PerformLayout();
    ASSERT_TRUE(GridTestUtils::GetChildPosition(renderGrid_, 5) == Offset(450.0, 0.0)); // Move Visible
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(675.0, 250.0)) == 7); // RIGHT
    renderGrid_->PerformLayout();
    ASSERT_TRUE(GridTestUtils::GetChildPosition(renderGrid_, 7) == Offset(450.0, 0.0)); // Move Visible
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(675.0, 150.0)) == -1); // RIGHT
    renderGrid_->HandleOnBlur();
    renderGrid_->JumpTo(0);
    renderGrid_->PerformLayout();

    /**
     * @tc.steps: step3. Trigger Focus Move DOWN event to Grid.
     * @tc.expected: step3. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(450.0, -125.0)) == 2); // DOWN
    renderGrid_->HandleOnFocus(2);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(450.0, 125.0)) == 3); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(450.0, 375.0)) == 4); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(450.0, 625.0)) == -1); // DOWN
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step4. Trigger Focus Move LEFT event to Grid.
     * @tc.expected: step4. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(950.0, 375.0)) == 8); // LEFT
    renderGrid_->HandleOnFocus(8);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(900.0, 375.0)) == 5); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(750.0, 300.0)) == 2); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(450.0, 375.0)) == 0); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(150.0, 375.0)) == -1); // LEFT
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step5. Trigger Focus Move UP event to Grid.
     * @tc.expected: step5. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(750.0, 800.0)) == 6); // UP
    renderGrid_->HandleOnFocus(6);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(750.0, 625.0)) == 5); // UP
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(750.0, 250.0)) == -1); // UP
    renderGrid_->HandleOnBlur();
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == 5);
}

/**
 * @tc.name: RenderGridTest039
 * @tc.desc: Verify Grid can obtain and lost focus when column direction.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5P
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest039, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderGrid with multi children by column direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    renderGrid_ = GridTestUtils::InitRenderGridDiffSpan(FlexDirection::COLUMN, mockContext_);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(750.0, 750.0));
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == -1);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 9);

    /**
     * @tc.steps: step2. Trigger Focus Move RIGHT event to Grid.
     * @tc.expected: step2. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(-125.0, 1050.0)) == 5); // RIGHT
    renderGrid_->HandleOnFocus(5);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(250.0, 750.0)) == 6); // RIGHT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(625.0, 750.0)) == -1); // RIGHT
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step3. Trigger Focus Move DOWN event to Grid.
     * @tc.expected: step3. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(375.0, -150.0)) == 1); // DOWN
    renderGrid_->HandleOnFocus(1);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(375.0, 150.0)) == 3); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(375.0, 450.0)) == 5); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(250.0, 750.0)) == 7); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(125.0, 800.0)) == -1); // DOWN
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step4. Trigger Focus Move LEFT event to Grid.
     * @tc.expected: step4. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(900.0, 450.0)) == 4); // LEFT
    renderGrid_->HandleOnFocus(4);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(625.0, 450.0)) == 3); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(375.0, 450.0)) == 2); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(125.0, 450.0)) == -1); // LEFT
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step5. Trigger Focus Move UP event to Grid.
     * @tc.expected: step5. Focus cand obtain and lost success.
     */
    renderGrid_->PerformLayout();
    renderGrid_->JumpTo(0);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(125.0, 1200.0)) == 7); // UP
    renderGrid_->HandleOnFocus(7);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(GridTestUtils::GetChildPosition(renderGrid_, 7) == Offset(0.0, 450.0)); // Move Visible
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(125.0, 1050.0)) == 5); // UP
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(250.0, 750.0)) == 2); // UP
    renderGrid_->PerformLayout();
    ASSERT_TRUE(GridTestUtils::GetChildPosition(renderGrid_, 2) == Offset(0.0, 0.0)); // Move Visible
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(125.0, 450.0)) == 0); // UP
    renderGrid_->PerformLayout();
    ASSERT_TRUE(GridTestUtils::GetChildPosition(renderGrid_, 0) == Offset(0.0, 0.0)); // Move Visible
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(125.0, 150.0)) == -1); // UP
    renderGrid_->HandleOnBlur();
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == 0);
}

/**
 * @tc.name: RenderGridTest040
 * @tc.desc: Verify Grid can obtain and lost focus when row reverse direction.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5P
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest040, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderGrid with multi children by row reverse direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    renderGrid_ = GridTestUtils::InitRenderGridDiffSpan(FlexDirection::ROW_REVERSE, mockContext_);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(750.0, 750.0));
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == -1);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 9);

    /**
     * @tc.steps: step2. Trigger Focus Move RIGHT event to Grid.
     * @tc.expected: step2. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(-500.0, 375.0)) == 8); // RIGHT
    renderGrid_->HandleOnFocus(8);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(-300.0, 375.0)) == 5); // RIGHT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(0.0, 375.0)) == 2); // RIGHT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(300.0, 375.0)) == 0); // RIGHT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(600.0, 375.0)) == -1); // RIGHT
    renderGrid_->HandleOnBlur();
    renderGrid_->PerformLayout();
    renderGrid_->JumpTo(0);
    renderGrid_->PerformLayout();

    /**
     * @tc.steps: step3. Trigger Focus Move DOWN event to Grid.
     * @tc.expected: step3. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(-300.0, -125.0)) == 5); // DOWN
    renderGrid_->PerformLayout();
    renderGrid_->HandleOnFocus(5);
    ASSERT_TRUE(GridTestUtils::GetChildPosition(renderGrid_, 5) == Offset(0.0, 0.0)); // Move Visible
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(0.0, 250.0)) == 6); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(0.0, 625.0)) == -1); // DOWN
    ASSERT_TRUE(renderGrid_->GetFocusState() == true);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == 6);
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step4. Trigger Focus Move LEFT event to Grid.
     * @tc.expected: step4. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(900.0, 500.0)) == 1); // LEFT
    renderGrid_->HandleOnFocus(1);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(600.0, 500.0)) == 3); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(300.0, 375.0)) == 5); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(0.0, 250.0)) == 7); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(-300.0, 125.0)) == -1); // LEFT
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step5. Trigger Focus Move UP event to Grid.
     * @tc.expected: step5. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(750.0, 900.0)) == 1); // UP
    renderGrid_->HandleOnFocus(1);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(600.0, 500.0)) == 0); // UP
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(600.0, 125.0)) == -1); // UP
    renderGrid_->HandleOnBlur();
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == 0);
}

/**
 * @tc.name: RenderGridTest041
 * @tc.desc: Verify Grid can obtain and lost focus when column reverse direction and diff span.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5P
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest041, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderGrid with multi children by column reverse direction and diff span.
     * @tc.expected: step1. properties and children are set correctly.
     */
    renderGrid_ = GridTestUtils::InitRenderGridDiffSpan(FlexDirection::COLUMN_REVERSE, mockContext_);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(750.0, 750.0));
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == -1);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 9);

    /**
     * @tc.steps: step2. Trigger Focus Move RIGHT event to Grid.
     * @tc.expected: step2. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(-125.0, 300.0)) == 2); // RIGHT
    renderGrid_->HandleOnFocus(2);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(125.0, 300.0)) == 3); // RIGHT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(375, 300.0)) == 4); // RIGHT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(625.0, 300.0)) == -1); // RIGHT
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step3. Trigger Focus Move DOWN event to Grid.
     * @tc.expected: step3. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(125.0, -600.0)) == 7); // DOWN
    renderGrid_->PerformLayout();
    renderGrid_->HandleOnFocus(7);
    ASSERT_TRUE(GridTestUtils::GetChildPosition(renderGrid_, 7) == Offset(0.0, 0.0)); // Move Visible
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(125.0, -300.0)) == 5); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(125.0, 0.0)) == 2); // DOWN
    renderGrid_->PerformLayout();
    ASSERT_TRUE(GridTestUtils::GetChildPosition(renderGrid_, 2) == Offset(0.0, 450.0)); // Move Visible
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(125.0, 300.0)) == 0); // DOWN
    renderGrid_->PerformLayout();
    ASSERT_TRUE(GridTestUtils::GetChildPosition(renderGrid_, 0) == Offset(0.0, 450.0)); // Move Visible
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(125.0, 600.0)) == -1); // DOWN
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step4. Trigger Focus Move LEFT event to Grid.
     * @tc.expected: step4. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(800.0, 0.0)) == 6); // LEFT
    renderGrid_->HandleOnFocus(6);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(625.0, 0.0)) == 5); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(250.0, 0.0)) == -1); // LEFT
    renderGrid_->HandleOnBlur();

    /**
     * @tc.steps: step5. Trigger Focus Move UP event to Grid.
     * @tc.expected: step5. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(500.0, 900.0)) == 1); // UP
    renderGrid_->HandleOnFocus(1);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(500.0, 600.0)) == 3); // UP
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(375.0, 300.0)) == 5); // UP
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(250.0, 0.0)) == 7); // UP
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(125.0, -300.0)) == -1); // UP
    ASSERT_TRUE(renderGrid_->GetFocusState() == true);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == 7);
    renderGrid_->HandleOnBlur();
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == 7);
}

/**
 * @tc.name: RenderGridTest042
 * @tc.desc: Verify Grid can obtain and lost focus when column direction and same span.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5P
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderGridTest, RenderGridTest042, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct RenderGrid with multi children by column reverse direction.
     * @tc.expected: step1. properties and children are set correctly.
     */
    renderGrid_ = GridTestUtils::InitRenderGridSameSpan(FlexDirection::COLUMN, mockContext_);
    renderGrid_->PerformLayout();
    ASSERT_TRUE(renderGrid_->GetLayoutSize() == Size(800.0, 800.0));
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == -1);
    ASSERT_TRUE(renderGrid_->GetChildren().size() == 16);

    /**
     * @tc.steps: step2. Trigger Focus Move up/down/left/right event to Grid.
     * @tc.expected: step2. Focus cand obtain and lost success.
     */
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(-100.0, 300.0)) == 4); // RIGHT
    renderGrid_->HandleOnFocus(4);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(0.0, 0.0)) == 5); // RIGHT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(0.0, 0.0)) == 9); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(0.0, 0.0)) == 8); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(0.0, 0.0)) == 4); // UP
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(0.0, 0.0)) == -1); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(0.0, 0.0)) == 8); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(0.0, 0.0)) == -1); // LEFT
    renderGrid_->HandleOnBlur();

    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(500.0, -100.0)) == 2); // DOWN
    renderGrid_->HandleOnFocus(2);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(0.0, 0.0)) == 6); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(0.0, 0.0)) == 10); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(0.0, 0.0)) == 11); // RIGHT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, false, Offset(0.0, 0.0)) == -1); // RIGHT
    renderGrid_->HandleOnBlur();

    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(900.0, 100.0)) == 3); // LEFT
    renderGrid_->HandleOnFocus(3);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(0.0, 0.0)) == 2); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(0.0, 0.0)) == 1); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(0.0, 0.0)) == 0); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(0.0, 0.0)) == -1); // UP
    renderGrid_->HandleOnBlur();

    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(700.0, 900.0)) == 15); // UP
    renderGrid_->HandleOnFocus(15);
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, true, Offset(0.0, 0.0)) == 11); // UP
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(0.0, 0.0)) == 10); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(false, true, Offset(0.0, 0.0)) == 9); // LEFT
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(0.0, 0.0)) == 13); // DOWN
    ASSERT_TRUE(renderGrid_->RequestNextFocus(true, false, Offset(0.0, 0.0)) == -1); // DOWN
    renderGrid_->HandleOnBlur();
    ASSERT_TRUE(renderGrid_->GetFocusState() == false);
    ASSERT_TRUE(renderGrid_->GetFocusIndex() == 13);
}

} // namespace OHOS::Ace
