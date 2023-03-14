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

#include "core/components/progress/progress_component.h"

#include "core/components/progress/progress_element.h"
#include "core/components/progress/render_progress.h"

namespace OHOS::Ace {

ProgressComponent::ProgressComponent(double min, double value, double cachedValue, double max, ProgressType type)
{
    switch (type) {
        case ProgressType::LINEAR: {
            track_ = AceType::MakeRefPtr<LinearTrack>();
            break;
        }
        case ProgressType::RING: {
            track_ = AceType::MakeRefPtr<CircularTrack>();
            break;
        }
        case ProgressType::SCALE: {
            track_ = AceType::MakeRefPtr<ScaleRingTrack>();
            break;
        }
        case ProgressType::GAUGE:
        case ProgressType::ARC: {
            track_ = AceType::MakeRefPtr<ArcTrack>();
            break;
        }
        case ProgressType::MOON: {
            track_ = AceType::MakeRefPtr<MoonTrack>();
            break;
        }
        case ProgressType::CAPSULE: {
            track_ = AceType::MakeRefPtr<CapsuleTrack>();
            break;
        }
        default: {
            track_ = AceType::MakeRefPtr<LinearTrack>();
            break;
        }
    }
    data_.SetMinValue(min);
    data_.SetValue(value);
    data_.SetMaxValue(max);
    data_.SetCachedValue(cachedValue);
    type_ = type;
}

void ProgressComponent::InitStyle(const RefPtr<ProgressTheme>& theme)
{
    if (!theme) {
        return;
    }
    track_->SetSelectColor(theme->GetTrackSelectedColor());
    track_->SetCachedColor(theme->GetTrackCachedColor());
    track_->SetBackgroundColor(theme->GetTrackBgColor());
    track_->SetTrackThickness(theme->GetTrackThickness());
}

RefPtr<RenderNode> ProgressComponent::CreateRenderNode()
{
    return RenderProgress::Create();
}

RefPtr<Element> ProgressComponent::CreateElement()
{
    return AceType::MakeRefPtr<ProgressElement>();
}

} // namespace OHOS::Ace