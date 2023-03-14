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

#include "core/components/test/unittest/grid/grid_test_utils.h"

#include "core/components/box/box_component.h"
#include "core/components/grid/grid_component.h"

namespace OHOS::Ace {

RefPtr<RenderBox> GridTestUtils::CreateRenderBox(double width, double height)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    renderBox->Update(boxComponent);
    return renderBox;
}

RefPtr<RenderBox> GridTestUtils::CreateRenderItem(double width, double height,
    int32_t span, const RefPtr<PipelineContext>& context)
{
    RefPtr<RenderBox> parent = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<RenderGridItem> child = AceType::MakeRefPtr<MockRenderGridItem>();
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    parent->Attach(context);
    child->Attach(context);
    renderBox->Attach(context);
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    renderBox->Update(boxComponent);
    child->SetColumnSpan(span);
    parent->AddChild(child);
    child->AddChild(renderBox);
    return parent;
}

RefPtr<RenderGrid> GridTestUtils::InitRenderGridDiffSpan(FlexDirection direction,
    const RefPtr<PipelineContext>& context)
{
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(direction);
    gridComponent->SetColumnCount(3);
    gridComponent->SetWidth(750.0);
    gridComponent->SetHeight(750.0);
    RefPtr<RenderGrid> renderGrid = AceType::MakeRefPtr<RenderGrid>();
    renderGrid->Update(gridComponent);
    renderGrid->Attach(context);
    int32_t count = 9;
    for (int32_t i = 0; i < count; ++i) {
        int32_t span = 1;
        if (i == 1 || i == 5) {
            span = 2;
        }
        renderGrid->AddChild(GridTestUtils::CreateRenderItem(300.0, 300.0, span, context));
    }
    return renderGrid;
}

RefPtr<RenderGrid> GridTestUtils::InitRenderGridSameSpan(FlexDirection direction,
    const RefPtr<PipelineContext>& context)
{
    std::list<RefPtr<Component>> children;
    RefPtr<GridComponent> gridComponent = AceType::MakeRefPtr<GridComponent>(children);
    gridComponent->SetDirection(direction);
    gridComponent->SetColumnCount(4);
    gridComponent->SetWidth(800.0);
    gridComponent->SetHeight(800.0);
    RefPtr<RenderGrid> renderGrid = AceType::MakeRefPtr<RenderGrid>();
    renderGrid->Update(gridComponent);
    renderGrid->Attach(context);
    int32_t count = 16;
    for (int32_t i = 0; i < count; ++i) {
        renderGrid->AddChild(GridTestUtils::CreateRenderItem(200.0, 200.0, 1, context));
    }
    return renderGrid;
}

Offset GridTestUtils::GetChildPosition(const RefPtr<RenderGrid>& grid, int32_t index)
{
    Offset offset = Offset(-1.0, -1.0);
    if (!grid) {
        return offset;
    }
    int32_t size = grid->GetChildren().size();
    if (index < 0 || index >= size) {
        return offset;
    }
    auto pos = grid->GetChildren().begin();
    std::advance(pos, index);
    offset = (*pos)->GetPosition();
    return offset;
}

} // namespace OHOS::Ace
