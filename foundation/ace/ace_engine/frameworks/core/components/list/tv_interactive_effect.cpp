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

#include "core/components/list/tv_interactive_effect.h"

namespace OHOS::Ace {

void TVInteractiveEffect::BuildStateAnimation()
{
    if (!theme_) {
        LOGE("theme is invalid, stop build animation");
        return;
    }

    auto scaleFrameStart = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_ZERO_TIME, scaleBegin_);
    auto scaleFrameEnd = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_END_TIME, scaleEnd_);
    scaleFrameEnd->SetCurve(Curves::FRICTION);

    auto scaleAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    scaleAnimation->AddKeyframe(scaleFrameStart);
    scaleAnimation->AddKeyframe(scaleFrameEnd);
    scaleAnimation->AddListener([weakEffect = AceType::WeakClaim(this)](double value) {
        auto effect = weakEffect.Upgrade();
        if (effect) {
            effect->SetScale(value);
            effect->MarkItemRender();
        }
    });

    if (controller_) {
        controller_->ClearInterpolators();
        controller_->AddInterpolator(scaleAnimation);
        controller_->SetDuration(focusDuration_);
        controller_->Play();
    }
}

} // namespace OHOS::Ace
