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

#include "core/components/slider/slider_component.h"

#include "core/components/box/box_component.h"
#include "core/components/slider/slider_element.h"

namespace OHOS::Ace {

SliderComponent::SliderComponent(const double value, const double step, const double min, const double max)
    : block_(AceType::MakeRefPtr<CircleBlock>()), track_(AceType::MakeRefPtr<LinearTrack>()),
      rotationController_(AceType::MakeRefPtr<RotationController>())
{
    data_.SetValue(value);
    data_.SetMaxValue(max);
    data_.SetMinValue(min);
    data_.SetStepValue(step);
}

void SliderComponent::SetThemeStyle(const RefPtr<SliderTheme>& theme)
{
    if (!theme) {
        return;
    }
    block_->SetBlockColor(theme->GetBlockColor());
    track_->SetBackgroundColor(theme->GetTrackBgColor());
    track_->SetSelectColor(theme->GetTrackSelectedColor());
    InitStyle(theme);
}

void SliderComponent::InitStyle(const RefPtr<SliderTheme>& theme)
{
    if (!theme) {
        return;
    }
    block_->SetHoverColor(theme->GetBlockHoverColor());
    if (mode_ == SliderMode::INSET) {
        track_->SetTrackThickness(theme->GetInsetTrackThickness());
        block_->SetBlockSize(theme->GetInsetBlockSize());
        block_->SetHotRegionWidth(theme->GetInsetBlockHotSize());
        block_->SetHotRegionHeight(theme->GetInsetBlockHotSize());
    } else if (mode_ == SliderMode::OUTSET) {
        track_->SetTrackThickness(theme->GetOutsetTrackThickness());
        block_->SetBlockSize(theme->GetOutsetBlockSize());
        block_->SetHotRegionWidth(theme->GetOutsetBlockHotSize());
        block_->SetHotRegionHeight(theme->GetOutsetBlockHotSize());
    } else {
        LOGW("slider: invalid mode");
    }
}

RefPtr<Element> SliderComponent::CreateElement()
{
    return AceType::MakeRefPtr<SliderElement>();
}

RefPtr<RenderNode> SliderComponent::CreateRenderNode()
{
    return RenderSlider::Create();
}

} // namespace OHOS::Ace