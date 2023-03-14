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

#include "core/components/divider/render_divider.h"

#include "base/log/event_report.h"
#include "core/components/divider/divider_component.h"

namespace OHOS::Ace {

void RenderDivider::Update(const RefPtr<Component>& component)
{
    auto divider = AceType::DynamicCast<DividerComponent>(component);
    if (!divider) {
        LOGE("RenderDivider update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    dividerColor_ = divider->GetDividerColor();
    strokeWidth_ = divider->GetStrokeWidth();
    vertical_ = divider->IsVertical();
    lineCap_ = divider->GetLineCap();
    MarkNeedLayout();
}

void RenderDivider::PerformLayout()
{
    auto parent = GetParent().Upgrade();
    if (!parent) {
        LOGE("RenderDivider PerformLayout with nullptr");
        return;
    }

    constrainStrokeWidth_ = GreatNotEqual(NormalizeToPx(strokeWidth_), 0.0) ? NormalizeToPx(strokeWidth_) : 0.0;
    if (!vertical_) {
        dividerLength_ = GetLayoutParam().GetMaxSize().Width();
        auto constrainSize = GetLayoutParam().Constrain(Size(dividerLength_, constrainStrokeWidth_));
        SetLayoutSize(constrainSize);
        dividerLength_ = constrainSize.Width();
        constrainStrokeWidth_ = GreatOrEqual(constrainStrokeWidth_, constrainSize.Height()) ? constrainSize.Height()
                                                                                            : constrainStrokeWidth_;
    } else {
        dividerLength_ = GetLayoutParam().GetMaxSize().Height();
        auto constrainSize = GetLayoutParam().Constrain(Size(constrainStrokeWidth_, dividerLength_));
        SetLayoutSize(constrainSize);
        dividerLength_ = constrainSize.Height();
        constrainStrokeWidth_ =
            GreatOrEqual(constrainStrokeWidth_, constrainSize.Width()) ? constrainSize.Width() : constrainStrokeWidth_;
    }
}

}; // namespace OHOS::Ace
