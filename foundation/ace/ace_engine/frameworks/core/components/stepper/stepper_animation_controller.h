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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_ANIMATION_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_ANIMATION_CONTROLLER_H

#include "base/memory/ace_type.h"
#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/pipeline/base/render_node.h"
#include "core/pipeline/pipeline_context.h"
namespace OHOS::Ace {

using AnimationStopCallback = std::function<void()>;
using AnimationCallback = std::function<void(double)>;

class StepperAnimation : public AceType {
    DECLARE_ACE_TYPE(StepperAnimation, AceType);

public:
    StepperAnimation(const WeakPtr<PipelineContext>& context, double start, double end, int delay,
        int duration, const RefPtr<Curve>& curve, const AnimationCallback& callback);
    ~StepperAnimation() override = default;
    void Play();
    void Stop();

    void AddStopListenerCallback(const AnimationStopCallback& callBack)
    {
        if (controller_) {
            controller_->AddStopListener(callBack);
        }
    }

    void ClearStopListenerCallback()
    {
        if (controller_) {
            controller_->ClearStopListeners();
        }
    }

private:
    double start_ = 0.0;
    double end_ = 0.0;
    int delay_ = 0;
    int duration_ = 0;
    WeakPtr<PipelineContext> context_;
    RefPtr<Curve> curve_;
    AnimationCallback callback_;
    RefPtr<CurveAnimation<double>> animation_;
    RefPtr<Animator> controller_;
};

class StepperAnimationController : public AceType {
    DECLARE_ACE_TYPE(StepperAnimationController, AceType);

public:
    explicit StepperAnimationController(const WeakPtr<PipelineContext>& context) : context_(context) {}
    ~StepperAnimationController() override = default;

    void SetAnimationStopCallback(const AnimationStopCallback& callback)
    {
        animationStopCallback_ = callback;
    }

    void SetRenderStepper(const WeakPtr<RenderNode>& node)
    {
        stepper_ = node;
    }

    const WeakPtr<RenderNode>& GetRenderStepper() const
    {
        return stepper_;
    }

    void PlayStepperToAnimation(int32_t fromIndex, int32_t toIndex, bool reverse);

private:
    void CreateAnimation();
    void CreateOpacityAnimation();
    void CreateTranslateAnimation();
    void PlayNextAnimation();
    void StopNextAnimation();
    void PlayBackAnimation();
    void StopBackAnimation();

    RefPtr<StepperAnimation> fromIndexOpacityAnimation_;
    RefPtr<StepperAnimation> toIndexOpacityAnimation_;
    RefPtr<StepperAnimation> fromIndexTranslateAnimationNext_;
    RefPtr<StepperAnimation> toIndexTranslateAnimationNext_;
    RefPtr<StepperAnimation> fromIndexTranslateAnimationBack_;
    RefPtr<StepperAnimation> toIndexTranslateAnimationBack_;
    WeakPtr<PipelineContext> context_;
    WeakPtr<RenderNode> stepper_;
    bool isAnimationCreated_ = false;
    AnimationStopCallback animationStopCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_ANIMATION_CONTROLLER_H
