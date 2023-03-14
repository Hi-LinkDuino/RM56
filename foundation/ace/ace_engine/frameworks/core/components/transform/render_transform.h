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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_RENDER_TRANSFORM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_RENDER_TRANSFORM_H

#include "base/geometry/animatable_matrix4.h"
#include "base/geometry/axis.h"
#include "base/geometry/matrix4.h"
#include "base/geometry/transform_util.h"
#include "core/animation/animatable_transform_operation.h"
#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/common/properties/motion_path_evaluator.h"
#include "core/components/common/properties/tween_option.h"
#include "core/components/transform/click_spring_effect.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

enum class TransformType {
    PERSPECTIVE,
};

class RenderTransform : public RenderNode {
    DECLARE_ACE_TYPE(RenderTransform, RenderNode);

public:
    static Matrix4 GetTransformByOffset(Matrix4 matrix, const Offset& offset);
    static RefPtr<RenderNode> Create();
    void Translate(const Dimension& x, const Dimension& y, const Dimension& z);
    void Translate(const Dimension& x, const Dimension& y);
    void Scale(float value);
    void Scale(float x, float y);
    void Scale(float x, float y, float z);
    void Skew(float x, float y);
    void Rotate(float angle, float x, float y, float z);
    void RotateX(float angle);
    void RotateY(float angle);
    void RotateZ(float angle);
    void Matrix3D(Matrix4 m);
    void Perspective(const Dimension& distance);
    void ResetTransform();
    void UpdateTransform();
    void SetTouchable(bool enable);
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void UpdateTransformOrigin();
    virtual void Mirror(const Offset& center, const Offset& global) {}

    void SetMaxScaleXY(double maxScaleXY)
    {
        maxScaleXY_ = maxScaleXY;
    }

    void MarkNeedUpdateOrigin()
    {
        needUpdateOrigin_ = true;
    }

    void SetTransformOrigin(const Dimension& x, const Dimension& y)
    {
        originX_ = x;
        originY_ = y;
        MarkNeedRender();
    }
    DimensionOffset GetTransformOrigin() const
    {
        return DimensionOffset(originX_, originY_);
    }
    const Dimension& GetOriginX() const
    {
        return originX_;
    }
    const Dimension& GetOriginY() const
    {
        return originY_;
    }

    void SetDisableClickEffect(bool isDisable)
    {
        disableClickEffect_ = isDisable;
    }

    const AnimatableTransformOperations& GetTransformEffects() const
    {
        return transformEffects_;
    }

    virtual void UpdateTransformLayer() {}

    Offset GetGlobalOffsetExternal() const override;

    void OnTransition(TransitionType type, int32_t id) override;

    bool HasDisappearingTransition(int32_t nodeId) override;

    void OnAttachContext() override
    {
        transformAnimation_.SetContextAndCallback(context_, [weak = AceType::WeakClaim(this)]() {
            auto renderNode = weak.Upgrade();
            if (renderNode) {
                renderNode->MarkNeedLayout();
                renderNode->MarkNeedRender();
            }
        });

        transformEffects_.SetContextAndCallback(context_, [weak = AceType::WeakClaim(this)]() {
            auto renderNode = weak.Upgrade();
            if (renderNode) {
                renderNode->needUpdateTransform_ = true;
                renderNode->ResetTransform();
                renderNode->MarkNeedRender();
            }
        });
    }

    void SetMotionPathEvaluator(const RefPtr<MotionPathEvaluator>& evaluator)
    {
        auto context = context_.Upgrade();
        if (!context) {
            LOGE("SetMotionPathEvaluator failed, context is null");
            return;
        }
        const auto& option = context->GetExplicitAnimationOption();
        if (option.IsValid()) {
            transformAnimation_.SetEvaluator(evaluator->CreateTransformOperationsEvaluator());
            transformAnimation_.SetAnimationStopCallback([weak = AceType::WeakClaim(this)]() {
                auto renderNode = weak.Upgrade();
                if (renderNode) {
                    renderNode->transformAnimation_.SetEvaluator(nullptr);
                }
            });
            transformAnimation_.PlayTransformAnimation(option, std::vector<TransformOperation>(), true);
        }
    }

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    Matrix4 UpdateWithEffectMatrix(Matrix4 matrix);
    void OnTransformDisappearingCallback();
    void ClearRenderObject() override;
    void ParseTransformEffects(const std::vector<AnimatableTransformOperation>& transformEffects, Matrix4& transform);
    Matrix4 ParseTransformEffect(const TransformOperation& effect);
    void ParseDimension(TransformOperation& effect);

    Matrix4 transform_;
    bool needUpdateTransform_ = false;
    bool isFirstAnimation_ = true;
    AnimatableTransformOperations transformEffects_;
    Offset origin_;
    Dimension originX_;
    Dimension originY_;
    bool needUpdateOrigin_ = false;
    double maxScaleXY_ = -1.0;
    RefPtr<RawRecognizer> rawRecognizer_;
    RefPtr<ClickSpringEffect> clickSpringEffect_;
    bool disableClickEffect_ = false;
    bool enableTouchTest_ = true;
    Matrix4 transformPaint_;
    TransformAnimation transformAnimation_;
    AnimationOption transitionOption_;
    std::vector<TransformOperation> transformEffectsAppearing_;
    std::vector<TransformOperation> transformEffectsDisappearing_;
    bool hasDisappearTransition_ = false;
    bool hasAppearTransition_ = false;
    bool pendingAppearing_ = false;

private:
    double CovertDimensionToPxBySize(const Dimension& dimension, double size);
    void SetTouchHandle(ClickSpringEffectType type);

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void ResetTransformToAccessibilityNode();
    void UpdateScaleToAccessibilityNode(float maxScale);
    void UpdateTranslateToAccessibilityNode(double translateX, double translateY);
    void UpdateRotateToAccessibilityNode(float angle, RotateAxis rotateAxis);
#endif
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_RENDER_TRANSFORM_H
