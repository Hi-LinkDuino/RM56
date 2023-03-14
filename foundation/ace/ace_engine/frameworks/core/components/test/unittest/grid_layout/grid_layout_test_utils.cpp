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

#include "core/components/test/unittest/grid_layout/grid_layout_test_utils.h"

#include "core/components/box/box_component.h"
#include "core/components/grid_layout/grid_layout_component.h"

namespace OHOS::Ace {

void GridLayoutTestUtils::PrintNodeInfo(const RefPtr<RenderNode>& node)
{
    if (!node) {
        printf("ERROR: node is nullptr.");
        return;
    }
    printf("Node Position(%lf %lf) Size(%lf %lf)\n", node->GetPosition().GetX(), node->GetPosition().GetY(),
        node->GetLayoutSize().Width(), node->GetLayoutSize().Height());
}

RefPtr<Component> GridLayoutTestUtils::CreateComponent(FlexDirection direction, std::string rows, std::string cols)
{
    constexpr double DIM_SIZE_VALUE_TEST = 1080.0;
    std::list<RefPtr<Component>> children;
    RefPtr<GridLayoutComponent> component = AceType::MakeRefPtr<GridLayoutComponent>(children);
    component->SetWidth(DIM_SIZE_VALUE_TEST);
    component->SetHeight(DIM_SIZE_VALUE_TEST);
    component->SetDirection(direction);
    component->SetRowsArgs(rows);
    component->SetColumnsArgs(cols);
    return component;
}

RefPtr<RenderNode> GridLayoutTestUtils::CreateRenderItem(int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan)
{
    constexpr double DIM_SIZE_VALUE_TEST = 540.0;
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    boxComponent->SetWidth(DIM_SIZE_VALUE_TEST);
    boxComponent->SetHeight(DIM_SIZE_VALUE_TEST);
    renderBox->Update(boxComponent);
    RefPtr<RenderGridLayoutItem> item = AceType::MakeRefPtr<MockRenderGridLayoutItem>();
    item->SetRowIndex(row);
    item->SetColumnIndex(col);
    item->SetRowSpan(rowSpan);
    item->SetColumnSpan(colSpan);
    item->AddChild(renderBox);
    return item;
}

RefPtr<RenderNode> GridLayoutTestUtils::CreateDragRenderItem()
{
    constexpr double DIM_SIZE_VALUE_TEST = 50.0;
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    boxComponent->SetWidth(DIM_SIZE_VALUE_TEST);
    boxComponent->SetHeight(DIM_SIZE_VALUE_TEST);
    renderBox->Update(boxComponent);
    RefPtr<RenderGridLayoutItem> item = AceType::MakeRefPtr<MockRenderGridLayoutItem>();
    item->AddChild(renderBox);
    return item;
}

RefPtr<Component> GridLayoutTestUtils::CreateDragComponent(std::string rows, std::string cols)
{
    constexpr double DIM_SIZE_VALUE_TEST = 150.0;
    constexpr int32_t MAX_COUNT_VALUE_TEST = 5;
    constexpr int32_t MIN_COUNT_VALUE_TEST = 1;
    constexpr int32_t CELL_LENGTH_VALUE_TEST = 50;

    std::list<RefPtr<Component>> children;
    RefPtr<GridLayoutComponent> component = AceType::MakeRefPtr<GridLayoutComponent>(children);
    component->SetSupportAnimation(false);
    component->SetEditMode(true);
    if (!rows.empty()) {
        component->SetHeight(DIM_SIZE_VALUE_TEST);
    }
    if (!cols.empty()) {
        component->SetWidth(DIM_SIZE_VALUE_TEST);
    }
    component->SetRowsArgs(rows);
    component->SetColumnsArgs(cols);
    component->SetMaxCount(MAX_COUNT_VALUE_TEST);
    component->SetMinCount(MIN_COUNT_VALUE_TEST);
    component->SetCellLength(CELL_LENGTH_VALUE_TEST);
    return component;
}
} // namespace OHOS::Ace
