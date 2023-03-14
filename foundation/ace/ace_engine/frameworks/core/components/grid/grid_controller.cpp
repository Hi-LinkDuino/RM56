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

#include "core/components/grid/grid_controller.h"

#include "core/components/grid/render_grid.h"

namespace OHOS::Ace {

void GridController::JumpTo(double position)
{
    auto grid = AceType::DynamicCast<RenderGrid>(grid_.Upgrade());
    if (!grid) {
        LOGW("RenderGrid is null. JumpTo %{public}lf failed.", position);
        return;
    }
    grid->JumpTo(position);
}

void GridController::AnimateTo(double position, float duration, const RefPtr<Curve>& curve)
{
    auto grid = AceType::DynamicCast<RenderGrid>(grid_.Upgrade());
    if (!grid) {
        LOGW("RenderGrid is null. AnimateTo %{public}lf failed.", position);
        return;
    }
    grid->AnimateTo(position, duration, curve);
}

void GridController::JumpTo(int32_t index)
{
    auto grid = AceType::DynamicCast<RenderGrid>(grid_.Upgrade());
    if (!grid) {
        LOGW("RenderGrid is null. JumpTo %{public}d failed.", index);
        return;
    }
    grid->JumpTo(index);
}

void GridController::AnimateTo(int32_t index, float duration, const RefPtr<Curve>& curve)
{
    auto grid = AceType::DynamicCast<RenderGrid>(grid_.Upgrade());
    if (!grid) {
        LOGW("RenderGrid is null. AnimateTo %{public}d failed.", index);
        return;
    }
    grid->AnimateTo(index, duration, curve);
}

void GridController::ScrollBy(int32_t pixelX, int32_t pixelY, bool smooth)
{
    auto grid = AceType::DynamicCast<RenderGrid>(grid_.Upgrade());
    if (!grid) {
        LOGW("RenderGrid is null. ScrollBy failed.");
        return;
    }
    grid->ScrollBy(pixelX, pixelY, smooth);
}

int32_t GridController::GetCurrentIndex() const
{
    auto grid = AceType::DynamicCast<RenderGrid>(grid_.Upgrade());
    if (!grid) {
        LOGW("RenderGrid is null. GetCurrentIndex failed.");
        return 0;
    }
    return grid->GetCurrentIndex();
}

double GridController::GetCurrentPosition() const
{
    auto grid = AceType::DynamicCast<RenderGrid>(grid_.Upgrade());
    if (!grid) {
        LOGW("RenderGrid is null. GetCurrentPosition failed.");
        return 0.0;
    }
    return grid->GetCurrentPosition();
}

} // namespace OHOS::Ace
