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

#include "picker_animation.h"

namespace OHOS::Ace {

void PickerAnimation::Init()
{
    animation_ = AceType::MakeRefPtr<CurveAnimation<double>>(start_, end_, curve_);
    animation_->AddListener(Animation<double>::ValueCallback(callback_));
    controller_ = AceType::MakeRefPtr<Animator>(pipe_);
    controller_->SetDuration(duration_);
    controller_->SetStartDelay(delay_);
}

void PickerAnimation::Play()
{
    if (!controller_ || !animation_) {
        return;
    }

    controller_->ClearInterpolators();
    controller_->AddInterpolator(animation_);
    controller_->Play();
}

void PickerAnimation::Stop()
{
    if (controller_) {
        controller_->Finish();
    }
}

} // namespace OHOS::Ace

