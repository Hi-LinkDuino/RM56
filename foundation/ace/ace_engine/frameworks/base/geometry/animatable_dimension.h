/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_ANIMATABLE_DIMENSION_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_ANIMATABLE_DIMENSION_H

#include "base/geometry/dimension.h"
#include "base/geometry/calc_dimension.h"
#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/common/properties/animation_option.h"

namespace OHOS::Ace {

using RenderNodeAnimationCallback = std::function<void()>;

/*
 * AnimatableDimension is a Dimension with AnimationOption and Animator.
 */
class ACE_EXPORT AnimatableDimension : public CalcDimension {
public:
    AnimatableDimension() = default;
    ~AnimatableDimension() = default;

    explicit AnimatableDimension(
        double value, DimensionUnit unit = DimensionUnit::PX, const AnimationOption& option = AnimationOption())
        : CalcDimension(value, unit), animationOption_(option)
    {}

    explicit AnimatableDimension(
        const std::string& value, DimensionUnit unit = DimensionUnit::CALC,
        const AnimationOption& option = AnimationOption())
        : CalcDimension(value, unit), animationOption_(option)
    {}

    explicit AnimatableDimension(const Dimension& dimension, const AnimationOption& option = AnimationOption())
        : CalcDimension(dimension), animationOption_(option)
    {}

    explicit AnimatableDimension(const CalcDimension& dimension, const AnimationOption& option = AnimationOption())
        : CalcDimension(dimension), animationOption_(option)
    {}

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        context_ = context;
        animationCallback_ = callback;
    }

    void SetContextAndCallbackAfterFirstAssign(
        const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        context_ = context;
        animationCallback_ = callback;
        isFirstAssign_ = false;
    }

    const AnimationOption& GetAnimationOption() const
    {
        return animationOption_;
    }

    void SetAnimationOption(const AnimationOption& option)
    {
        animationOption_ = option;
    }

    void SetAnimationStopCallback(const RenderNodeAnimationCallback& callback)
    {
        stopCallback_ = callback;
    }

    Animator::Status GetAnimationStatus() const
    {
        if (!animationController_) {
            return Animator::Status::IDLE;
        }
        return animationController_->GetStatus();
    }

    void SetEvaluator(const RefPtr<Evaluator<double>>& evaluator)
    {
        evaluator_ = evaluator;
    }

    AnimatableDimension& operator=(const Dimension& newDimension);

    AnimatableDimension& operator=(const CalcDimension& newDimension);

    AnimatableDimension& operator=(const AnimatableDimension& newDimension);

    void MoveTo(double target);

private:
    void AnimateTo(double endValue);
    void ResetController();
    void OnAnimationCallback(double value);
    void ResetAnimatableDimension();

private:
    bool isFirstAssign_ = true;
    AnimationOption animationOption_;
    RefPtr<Animator> animationController_;
    WeakPtr<PipelineContext> context_;
    RenderNodeAnimationCallback animationCallback_;
    RenderNodeAnimationCallback stopCallback_;
    RefPtr<Evaluator<double>> evaluator_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_ANIMATABLE_DIMENSION_H
