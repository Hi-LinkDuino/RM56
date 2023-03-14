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

#include "core/components/progress/render_progress.h"

namespace OHOS::Ace {

void RenderProgress::Update(const RefPtr<Component>& component)
{
    const RefPtr<ProgressComponent> progress = AceType::DynamicCast<ProgressComponent>(component);
    component_ = progress;
    min_ = progress->GetMinValue();
    max_ = progress->GetMaxValue();
    value_ = progress->GetValue();
    cachedValue_ = progress->GetCachedValue();
    trackThickness_ = NormalizeToPx(progress->GetTrack()->GetTrackThickness());

    type_ = progress->GetType();
    isError_ = false;
    if (min_ >= max_ || value_ > max_ || value_ < min_ || cachedValue_ < min_ || cachedValue_ > max_) {
        isError_ = true;
        LOGE("Progress::RenderProgress update min, max, value, step, cachedValue error");
        return;
    }
    LOGD("Progress::RenderProgress update Min: %{private}lf max: %{private}lf value: %{private}lf, cachedValue: "
         "%{private}lf", min_, max_, value_, cachedValue_);
    totalRatio_ = (value_ - min_) / (max_ - min_);
    cachedRatio_ = (cachedValue_ - min_) / (max_ - min_);
    MarkNeedLayout();
}

void RenderProgress::OnPaintFinish()
{
    UpdateAccessibilityAttr();
}

void RenderProgress::UpdateAccessibilityAttr()
{
    if (!component_) {
        return;
    }
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->SetAccessibilityValue(value_, min_, max_);
    auto context = context_.Upgrade();
    if (context) {
        AccessibilityEvent progressEvent;
        progressEvent.nodeId = accessibilityNode->GetNodeId();
        progressEvent.eventType = "selected";
        progressEvent.componentType = "progress";
        progressEvent.currentItemIndex = value_;
        progressEvent.itemCount = max_ - min_;
        context->SendEventToAccessibility(progressEvent);
    }
}

void RenderProgress::PerformLayout()
{
    Size size = Measure();
    SetLayoutSize(size);
}

Size RenderProgress::Measure()
{
    Size layoutSize;
    for (const auto& item : GetChildren()) {
        item->Layout(GetLayoutParam());
        if (layoutSize.Width() < item->GetLayoutSize().Width()) {
            layoutSize.SetWidth(item->GetLayoutSize().Width());
        }
        if (layoutSize.Height() < item->GetLayoutSize().Height()) {
            layoutSize.SetHeight(item->GetLayoutSize().Height());
        }
    }
    return layoutSize;
}

} // namespace OHOS::Ace
