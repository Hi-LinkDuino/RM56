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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_ANIMATION_H

#include "core/animation/animator.h"
#include "core/animation/curve.h"
#include "core/animation/curve_animation.h"
#include "core/animation/curves.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class PickerAnimation : public virtual AceType {
    DECLARE_ACE_TYPE(PickerAnimation, AceType);

public:
    PickerAnimation() = delete;
    ~PickerAnimation() = default;
    PickerAnimation(const WeakPtr<PipelineContext>& pipe, double start, double end, int delay, int duration,
        const RefPtr<Curve>& curve, const std::function<void(double)>& callback)
        : start_(start), end_(end), delay_(delay), duration_(duration), pipe_(pipe), curve_(curve), callback_(callback)
    {
        Init();
    }

    void Play();

    void Stop();

    void AddStopCallback(const std::function<void()>& value)
    {
        if (controller_) {
            controller_->AddStopListener(value);
        }
    }

    void ClearStopCallback()
    {
        if (!controller_) {
            return;
        }
        controller_->ClearStopListeners();
    }

private:
    void Init();

    double start_ = 0.0;
    double end_ = 0.0;
    int delay_ = 0;
    int duration_ = 0;

    WeakPtr<PipelineContext> pipe_;
    RefPtr<Curve> curve_;
    std::function<void(double)> callback_;
    RefPtr<CurveAnimation<double>> animation_;
    RefPtr<Animator> controller_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_ANIMATION_H
