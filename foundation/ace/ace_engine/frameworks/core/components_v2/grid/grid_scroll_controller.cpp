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

#include "core/components_v2/grid/grid_scroll_controller.h"
#include "core/components_v2/grid/render_grid_scroll.h"

namespace OHOS::Ace::V2 {
void GridScrollController::MarkScrollRender()
{
    auto grid = AceType::DynamicCast<RenderGridScroll>(scroll_.Upgrade());
    if (grid) {
        grid->MarkNeedLayout();
    }
}

bool GridScrollController::CheckScroll()
{
    auto grid = AceType::DynamicCast<RenderGridScroll>(scroll_.Upgrade());
    return grid != nullptr;
}

bool GridScrollController::UpdateScrollPosition(const double offset, int32_t source)
{
    bool ret = true;
    if (!callback_) {
        return true;
    }
    auto grid = AceType::DynamicCast<RenderGridScroll>(scroll_.Upgrade());
    if (!grid || NearZero(grid->GetEstimatedHeight())) {
        return true;
    }
    double height = grid->GetLayoutSize().Height();
    double estimateHeight = grid->GetEstimatedHeight();
    double activeHeight = height * height / estimateHeight;
    if (!NearEqual(height, activeHeight)) {
        double value = offset * estimateHeight / height;
        ret = callback_(value, source);
    }
    return ret;
}

void GridScrollController::ProcessScrollMotion(double position)
{
    if (!NearEqual(currentPos_, position)) {
        UpdateScrollPosition(0.0, SCROLL_FROM_ANIMATION);
    } else {
        if (!UpdateScrollPosition(currentPos_ - position, SCROLL_FROM_BAR)) {
            dragEndAnimator_->Stop();
        }
    }
    currentPos_ = position;
}
} // namespace OHOS::Ace::V2