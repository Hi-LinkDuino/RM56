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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_TRANSFORM_OPERATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_TRANSFORM_OPERATION_H

#include <list>
#include <vector>

#include "base/geometry/transform_util.h"
#include "core/animation/animatable_base.h"

namespace OHOS::Ace {

class ACE_EXPORT AnimatableTransformOperation final : public AnimatableBase<TransformOperation> {
public:
    explicit AnimatableTransformOperation(const AnimationOption& animationOption = AnimationOption())
    {
        animationOption_ = animationOption;
    }
    explicit AnimatableTransformOperation(
        const TransformOperation& operation, const AnimationOption& animationOption = AnimationOption())
    {
        animationOption_ = animationOption;
        SetCurrentValue(operation);
    }
    ~AnimatableTransformOperation() override = default;

    void MoveTo(const TransformOperation& value) override
    {
        SetCurrentValue(value);
    }

private:
    void OnAnimationCallback(const TransformOperation& value) override
    {
        SetCurrentValue(value);
        AnimatableBase<TransformOperation>::OnAnimationCallback(value);
    }

    void SetCurrentValue(const TransformOperation& other)
    {
        auto pipelineContext = context_.Upgrade();
        if (pipelineContext && pipelineContext->GetIsDeclarative() && type_ != TransformOperationType::UNDEFINED &&
            type_ != other.type_) {
            LOGE("SetCurrentValue failed, not same type");
            return;
        }
        TransformOperation& self = *this;
        self = other;
    }
};

class AnimatableTransformOperations final {
public:
    AnimatableTransformOperations() = default;
    AnimatableTransformOperations(const AnimatableTransformOperations& other) = default;
    ~AnimatableTransformOperations() = default;

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        context_ = context;
        animationCallback_ = callback;
    }

    const std::vector<AnimatableTransformOperation>& GetOperations() const
    {
        return operations_;
    }

    void AddTransformOperation(
        const TransformOperation& operation, const AnimationOption& animationOption = AnimationOption())
    {
        operations_.emplace_back(operation, animationOption);
    }

    void Clear()
    {
        operations_.clear();
    }

    AnimatableTransformOperations& operator=(const AnimatableTransformOperations& other)
    {
        if (isFirstAssign_) {
            operations_ = other.operations_;
            isFirstAssign_ = false;
        } else {
            auto pipelineContext = context_.Upgrade();
            if (!pipelineContext) {
                LOGE("AnimatableTransformOperations pipelineContext is null!");
                operations_ = other.operations_;
                return *this;
            }
            const auto& explicitOption = pipelineContext->GetExplicitAnimationOption();
            if (other.operations_.size() == operations_.size()) {
                for (std::size_t i = 0; i < operations_.size(); i++) {
                    if (explicitOption.IsValid()) {
                        operations_[i].SetAnimationOption(explicitOption);
                    } else {
                        operations_[i].SetAnimationOption(other.operations_[i].GetAnimationOption());
                    }
                    operations_[i].SetContextAndCallback(context_, animationCallback_);
                    operations_[i].AnimateTo(operations_[i], other.operations_[i]);
                }
            } else {
                LOGE("AddTransformOperation %{public}d != %{public}d", static_cast<int32_t>(operations_.size()),
                    static_cast<int32_t>(other.operations_.size()));
                operations_ = other.operations_;
            }
        }
        return *this;
    }

private:
    std::vector<AnimatableTransformOperation> operations_;
    RenderNodeAnimationCallback animationCallback_;
    WeakPtr<PipelineContext> context_;
    bool isFirstAssign_ = true;
};

class TransformAnimation final : public AnimatableBase<TransformOperations> {
public:
    Matrix4 ComputerBlendedMatrix4()
    {
        TransformOperations::ParseOperationsToMatrix(blended.GetOperations());
        return blended.ComputerRemaining(0);
    }

    void SetTransformOperations(const std::vector<TransformOperation>& other)
    {
        operations_ = other;
        blended = TransformOperations(operations_);
        endValue_ = blended;
    }

    void PlayTransformAnimation(
        const AnimationOption& option, const std::vector<TransformOperation>& other, bool alwaysRotate = false)
    {
        SetAnimationOption(option);
        auto target = TransformOperations(other);
        TransformOperations::ParseOperationsToMatrix(operations_);
        TransformOperations::ParseOperationsToMatrix(target.GetOperations());
        target.SetAlwaysRotate(alwaysRotate);
        AnimateTo(blended, target);
        // just move to target, renderNode should use blended to computer matrix
        MoveTo(target);
    }

    void MoveTo(const TransformOperations& value) override
    {
        operations_ = value.GetOperations();
    }

private:
    void OnAnimationCallback(const TransformOperations& value) override
    {
        blended = value;
        AnimatableBase<TransformOperations>::OnAnimationCallback(value);
    }

private:
    TransformOperations blended;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_TRANSFORM_OPERATION_H
