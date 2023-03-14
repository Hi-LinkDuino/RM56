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

#include "core/components/list/watch_interactive_effect.h"

#include "core/components/list/render_list_item.h"

namespace OHOS::Ace {

void WatchInteractiveEffect::BuildClickAnimation()
{
    if (!theme_) {
        LOGE("theme is invalid, stop build animation");
        return;
    }

    if (!NeedClickAnimation()) {
        return;
    }

    RefPtr<KeyframeAnimation<double>> scaleAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    RefPtr<KeyframeAnimation<double>> alphaAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    BuildClickScaleAnimation(scaleAnimation);
    BuildClickAlphaAnimation(alphaAnimation);
    if (controller_) {
        controller_->ClearInterpolators();
        controller_->AddInterpolator(scaleAnimation);
        controller_->AddInterpolator(alphaAnimation);
        controller_->SetDuration(clickDuration_);
        controller_->Play();
    }
}

void WatchInteractiveEffect::BuildClickScaleAnimation(const RefPtr<KeyframeAnimation<double>>& scaleAnimation)
{
    auto scaleFrameStart = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_ZERO_TIME, scaleBegin_);
    auto scaleFrameMid = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_HALF_TIME, scaleEnd_);
    auto scaleFrameEnd = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_END_TIME, scaleBegin_);
    scaleFrameMid->SetCurve(Curves::FRICTION);
    scaleFrameEnd->SetCurve(Curves::FRICTION);

    scaleAnimation->AddKeyframe(scaleFrameStart);
    scaleAnimation->AddKeyframe(scaleFrameMid);
    scaleAnimation->AddKeyframe(scaleFrameEnd);
    scaleAnimation->AddListener([weakEffect = AceType::WeakClaim(this)](double value) {
        auto effect = weakEffect.Upgrade();
        if (effect) {
            effect->SetScale(value);
            effect->MarkItemRender();
        }
    });
}

} // namespace OHOS::Ace
