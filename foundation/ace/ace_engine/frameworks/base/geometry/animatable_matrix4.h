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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_ANIMATABLE_MATRIX4_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_ANIMATABLE_MATRIX4_H

#include "base/geometry/matrix4.h"
#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/common/properties/animation_option.h"

namespace OHOS::Ace {

using RenderNodeAnimationCallback = std::function<void()>;

class ACE_EXPORT AnimatableMatrix4 : public Matrix4 {
public:
    AnimatableMatrix4() = default;
    ~AnimatableMatrix4() = default;

    explicit AnimatableMatrix4(Matrix4 matrix4, const AnimationOption& option = AnimationOption())
        : Matrix4(matrix4), animationOption_(option)
    {}

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        context_ = context;
        animationCallback_ = callback;
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

    AnimatableMatrix4& operator=(const Matrix4& matrix4);

    AnimatableMatrix4& operator=(const AnimatableMatrix4& newMatrix4);

    void MoveTo(const Matrix4& target);

    void SetEvaluator(const RefPtr<Evaluator<TransformOperation>>& evaluator)
    {
        evaluator_ = evaluator;
    }

private:
    void AnimateTo(const Matrix4& endValue);
    void ResetController();
    void OnAnimationCallback(const TransformOperation& value);
    void ResetAnimatableMatrix();

private:
    bool isFirstAssign_ = true;
    AnimationOption animationOption_;
    RefPtr<Animator> animationController_;
    WeakPtr<PipelineContext> context_;
    RenderNodeAnimationCallback animationCallback_;
    RenderNodeAnimationCallback stopCallback_;

    RefPtr<Evaluator<TransformOperation>> evaluator_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_GEOMETRY_ANIMATABLE_MATRIX4_H
