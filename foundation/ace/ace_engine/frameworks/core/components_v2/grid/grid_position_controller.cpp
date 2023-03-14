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

#include "core/components_v2/grid/grid_position_controller.h"

#include "core/components_v2/grid/render_grid_scroll.h"

namespace OHOS::Ace::V2 {

void GridPositionController::JumpTo(int32_t index, int32_t source)
{
    auto grid = AceType::DynamicCast<V2::RenderGridScroll>(scroll_.Upgrade());
    if (!grid) {
        return;
    }
    grid->ScrollToIndex(index, source);
}

bool GridPositionController::AnimateTo(const Dimension& position, float duration, const RefPtr<Curve>& curve)
{
    auto grid = AceType::DynamicCast<V2::RenderGridScroll>(scroll_.Upgrade());
    if (!grid) {
        return false;
    }
    return grid->AnimateTo(position, duration, curve);
}

Offset GridPositionController::GetCurrentOffset() const
{
    auto grid = AceType::DynamicCast<V2::RenderGridScroll>(scroll_.Upgrade());
    if (!grid) {
        return Offset::Zero();
    }
    return grid->CurrentOffset();
}

Axis GridPositionController::GetScrollDirection() const
{
    auto grid = AceType::DynamicCast<V2::RenderGridScroll>(scroll_.Upgrade());
    if (!grid) {
        return Axis::VERTICAL;
    }
    return grid->GetAxis();
}

void GridPositionController::ScrollToEdge(ScrollEdgeType scrollEdgeType, bool smooth)
{
    auto grid = AceType::DynamicCast<V2::RenderGridScroll>(scroll_.Upgrade());
    if (!grid) {
        return;
    }
    grid->ScrollToEdge(scrollEdgeType, smooth);
}

} // namespace OHOS::Ace::V2
