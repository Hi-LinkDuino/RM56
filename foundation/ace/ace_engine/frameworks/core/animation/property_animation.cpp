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

#include "core/animation/property_animation.h"

#include "core/animation/animatable_data.h"
#include "core/components/box/render_box.h"

namespace OHOS::Ace {

PropertyAnimation::PropertyAnimation(AnimatableType type)
{
    type_ = type;
}

void PropertyAnimation::SetStart(const RefPtr<Animatable>& animatable)
{
    if (animatable == nullptr) {
        return;
    }
    animatable->SetTimePoint(0.0f);
    animatables_.emplace_front(animatable);
}

void PropertyAnimation::AddAnimatable(const RefPtr<Animatable>& animatable)
{
    if (animatable == nullptr) {
        return;
    }
    animatables_.emplace_back(animatable);
}

void PropertyAnimation::SetCurve(const RefPtr<Curve>& curve)
{
    if (!curve) {
        LOGE("set curve failed. curve is null.");
        return;
    }
    for (auto& animatable : animatables_) {
        animatable->SetCurve(curve);
    }
}

void PropertyAnimation::OnNormalizedTimestampChanged(float normalized, bool reverse)
{
    if (normalized < NORMALIZED_DURATION_MIN || normalized > NORMALIZED_DURATION_MAX) {
        LOGE("normalized time check failed. normalized: %{public}f", normalized);
        return;
    }
    Calculate(normalized);
}

void PropertyAnimation::OnInitNotify(float normalizedTime, bool reverse)
{
    if (init_ && animateTo_) {
        animateTo_(init_);
    } else {
        OnNormalizedTimestampChanged(normalizedTime, reverse);
    }
}

RefPtr<Curve> PropertyAnimation::GetCurve()
{
    if (!animatables_.empty() && animatables_.front()->GetCurve() != nullptr) {
        return animatables_.front()->GetCurve();
    }
    return Curves::EASE_IN_OUT;
}

void PropertyAnimation::Calculate(float keyTime)
{
    if (animatables_.empty()) {
        return;
    }
    auto preAnimatable = animatables_.front();
    for (const auto& animatable : animatables_) {
        if (keyTime < animatable->GetTimePoint()) {
            float preKeyTime = preAnimatable->GetTimePoint();
            float innerKeyTime = (keyTime - preKeyTime) / (animatable->GetTimePoint() - preKeyTime);
            TriggerFrame(preAnimatable, animatable, innerKeyTime);
            return;
        }
        preAnimatable = animatable;
    }
    // in range [1.0, inf]
    const auto& lastAnimatable = animatables_.back();
    TriggerFrame(lastAnimatable, lastAnimatable, 1.0);
}

void PropertyAnimation::TriggerFrame(const RefPtr<Animatable>& start, const RefPtr<Animatable>& end, float time)
{
    if (animateTo_ == nullptr) {
        return;
    }
    switch (type_) {
        case AnimatableType::PROPERTY_WIDTH:
        case AnimatableType::PROPERTY_HEIGHT:
        case AnimatableType::PROPERTY_MARGIN_LEFT:
        case AnimatableType::PROPERTY_MARGIN_TOP:
        case AnimatableType::PROPERTY_MARGIN_RIGHT:
        case AnimatableType::PROPERTY_MARGIN_BOTTOM:
        case AnimatableType::PROPERTY_PADDING_LEFT:
        case AnimatableType::PROPERTY_PADDING_TOP:
        case AnimatableType::PROPERTY_PADDING_RIGHT:
        case AnimatableType::PROPERTY_PADDING_BOTTOM:
        case AnimatableType::PROPERTY_POSITION_LEFT:
        case AnimatableType::PROPERTY_POSITION_TOP:
        case AnimatableType::PROPERTY_POSITION_RIGHT:
        case AnimatableType::PROPERTY_POSITION_BOTTOM: {
            Next<Dimension>(start, end, time);
            break;
        }
        case AnimatableType::PROPERTY_BG_COLOR:
        case AnimatableType::PROPERTY_BORDER_LEFT_COLOR:
        case AnimatableType::PROPERTY_BORDER_TOP_COLOR:
        case AnimatableType::PROPERTY_BORDER_RIGHT_COLOR:
        case AnimatableType::PROPERTY_BORDER_BOTTOM_COLOR: {
            Next<Color>(start, end, time);
            break;
        }
        case AnimatableType::PROPERTY_OPACITY:
        case AnimatableType::PROPERTY_BACKDROP_FILTER_BLUR:
        case AnimatableType::PROPERTY_FILTER_BLUR:
        case AnimatableType::PROPERTY_WINDOW_FILTER_BLUR:
        case AnimatableType::PROPERTY_BORDER_LEFT_WIDTH:
        case AnimatableType::PROPERTY_BORDER_TOP_WIDTH:
        case AnimatableType::PROPERTY_BORDER_RIGHT_WIDTH:
        case AnimatableType::PROPERTY_BORDER_BOTTOM_WIDTH:
        case AnimatableType::PROPERTY_BORDER_TOP_LEFT_RADIUS:
        case AnimatableType::PROPERTY_BORDER_TOP_RIGHT_RADIUS:
        case AnimatableType::PROPERTY_BORDER_BOTTOM_LEFT_RADIUS:
        case AnimatableType::PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS: {
            Next<float>(start, end, time);
            break;
        }
        case AnimatableType::PROPERTY_BACKGROUND_SIZE: {
            Next<BackgroundImageSize>(start, end, time);
            break;
        }
        case AnimatableType::PROPERTY_BACKGROUND_POSITION: {
            Next<BackgroundImagePosition>(start, end, time);
            break;
        }
        case AnimatableType::PROPERTY_BOX_SHADOW: {
            Next<Shadow>(start, end, time);
            break;
        }
        case AnimatableType::PROPERTY_BORDER_LEFT_STYLE:
        case AnimatableType::PROPERTY_BORDER_TOP_STYLE:
        case AnimatableType::PROPERTY_BORDER_RIGHT_STYLE:
        case AnimatableType::PROPERTY_BORDER_BOTTOM_STYLE: {
            Next<BorderStyle>(start, end, time);
            break;
        }
        default:
            break;
    }
}

template<class T>
void PropertyAnimation::Next(const RefPtr<Animatable>& start, const RefPtr<Animatable>& end, float time)
{
    auto from = AceType::DynamicCast<AnimatableData<T>>(start);
    auto to = AceType::DynamicCast<AnimatableData<T>>(end);
    auto value = AceType::MakeRefPtr<AnimatableData<T>>(T {});
    if (from != nullptr && to != nullptr && value != nullptr) {
        float process = to->GetCurve()->Move(time);
        value->SetValue(to->Animate(from->GetValue(), to->GetValue(), process));
        animateTo_(value);
    }
}

} // namespace OHOS::Ace
