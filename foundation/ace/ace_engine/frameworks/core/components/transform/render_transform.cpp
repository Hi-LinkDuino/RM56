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

#include "core/components/transform/render_transform.h"

#include "base/utils/utils.h"
#include "core/components/box/render_box_base.h"
#include "core/components/transform/transform_component.h"

namespace OHOS::Ace {

// Effect translate to Matrix at the end for percent needs to be calculated after layout.
void RenderTransform::Translate(const Dimension& x, const Dimension& y)
{
    Translate(x, y, Dimension {});
}

void RenderTransform::Translate(const Dimension& x, const Dimension& y, const Dimension& z)
{
    auto dx = CovertDimensionToPxBySize(x, GetLayoutSize().Width());
    auto dy = CovertDimensionToPxBySize(y, GetLayoutSize().Height());
    // not support percent
    auto dz = CovertDimensionToPxBySize(z, 0.0);

    transform_ = transform_ * Matrix4::CreateTranslate(dx, dy, dz);
    UpdateTransformLayer();
    auto context = context_.Upgrade();
    if (context) {
        context->MarkForcedRefresh();
    }
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    UpdateTranslateToAccessibilityNode(x.Value(), y.Value());
#endif
}

void RenderTransform::Scale(float value)
{
    Scale(value, value);
}

void RenderTransform::Scale(float x, float y)
{
    Scale(x, y, 1.0f);
}

void RenderTransform::Scale(float x, float y, float z)
{
    transform_ = transform_ * Matrix4::CreateScale(x, y, z);
    UpdateTransformLayer();
    auto context = context_.Upgrade();
    if (context) {
        context->MarkForcedRefresh();
    }
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    if (!NearEqual(maxScaleXY_, -1.0)) {
        UpdateScaleToAccessibilityNode(maxScaleXY_);
    }
#endif
}

void RenderTransform::Skew(float x, float y)
{
    if (!NearZero(x) || !NearZero(y)) {
        Matrix4 skew = Matrix4::CreateSkew(x, y);
        transform_ = transform_ * skew;
    }
    UpdateTransformLayer();
    auto context = context_.Upgrade();
    if (context) {
        context->MarkForcedRefresh();
    }
}

void RenderTransform::Rotate(float angle, float x, float y, float z)
{
    if (!NearZero(angle) && !NearZero(fmod(angle, 360.0f))) {
        Matrix4 rotate = Matrix4::CreateRotate(angle, x, y, z);
        transform_ = transform_ * rotate;
    }
    UpdateTransformLayer();
    auto context = context_.Upgrade();
    if (context) {
        context->MarkForcedRefresh();
    }
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    if (!NearEqual(angle, 0.0) && !NearEqual(z, 0.0)) {
        UpdateRotateToAccessibilityNode(angle, RotateAxis::AXIS_Z);
    }
#endif
}

void RenderTransform::RotateX(float angle)
{
    Rotate(angle, 1.0f, 0.0f, 0.0f);
}

void RenderTransform::RotateY(float angle)
{
    Rotate(angle, 0.0f, 1.0f, 0.0f);
}

void RenderTransform::RotateZ(float angle)
{
    Rotate(angle, 0.0f, 0.0f, 1.0f);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    if (!NearEqual(angle, 0.0)) {
        UpdateRotateToAccessibilityNode(angle, RotateAxis::AXIS_Z);
    }
#endif
}

void RenderTransform::Matrix3D(Matrix4 m)
{
    if (!m.IsIdentityMatrix()) {
        transform_ = transform_ * m;
        UpdateTransformLayer();
        auto context = context_.Upgrade();
        if (context) {
            context->MarkForcedRefresh();
        }
    }
}

void RenderTransform::Perspective(const Dimension& distance)
{
    if (!NearZero(distance.Value())) {
        auto dx = CovertDimensionToPxBySize(distance, 0);
        transform_ = transform_ * Matrix4::CreatePerspective(static_cast<float>(dx));
        UpdateTransformLayer();
        auto context = context_.Upgrade();
        if (context) {
            context->MarkForcedRefresh();
        }
    }
}

void RenderTransform::ResetTransform()
{
    transform_ = Matrix4::CreateIdentity();
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    ResetTransformToAccessibilityNode();
#endif
}

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
void RenderTransform::ResetTransformToAccessibilityNode()
{
    const auto& context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGE("accessibilityManager is null");
        return;
    }
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(GetNodeId());
    if (!accessibilityNode) {
        LOGE("RenderTransform is null");
        return;
    }
    accessibilityNode->SetScaleToChild(1.0);
    accessibilityNode->SetTranslateOffsetToChild(Offset(0.0, 0.0));
    accessibilityNode->SetRotateToChild(0.0, RotateAxis::AXIS_Z);
    for (const auto& item : GetChildren()) {
        item->NotifyPaintFinish();
    }
}

void RenderTransform::UpdateScaleToAccessibilityNode(float maxScale)
{
    const auto& context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGE("accessibilityManager is null");
        return;
    }
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(GetNodeId());
    if (!accessibilityNode) {
        LOGE("RenderTransform is null");
        return;
    }

    if (!NearEqual(maxScale, 1.0)) {
        Size size = GetLayoutSize();
        Offset globalOffset = GetGlobalOffset();
        Offset scaleCenter =
            Offset(globalOffset.GetX() + size.Width() / 2.0, globalOffset.GetY() + size.Height() / 2.0);
        accessibilityNode->SetScaleToChild(maxScale);
        accessibilityNode->SetScaleCenterToChild(scaleCenter);
        for (const auto& item : GetChildren()) {
            item->NotifyPaintFinish();
        }
    }
}

void RenderTransform::UpdateTranslateToAccessibilityNode(double translateX, double translateY)
{
    const auto& context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGE("accessibilityManager is null");
        return;
    }
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(GetNodeId());
    if (!accessibilityNode) {
        LOGE("RenderTransform is null");
        return;
    }
    if (!NearEqual(translateX, 0.0) || !NearEqual(translateY, 0.0)) {
        Offset translateOffset(translateX, translateY);
        accessibilityNode->SetTranslateOffsetToChild(translateOffset);
        for (const auto& child : GetChildren()) {
            child->NotifyPaintFinish();
        }
    }
}

void RenderTransform::UpdateRotateToAccessibilityNode(float angle, RotateAxis rotateAxis)
{
    const auto& context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGE("accessibilityManager is null");
        return;
    }
    auto accessibilityNode = accessibilityManager->GetAccessibilityNodeById(GetNodeId());
    if (!accessibilityNode) {
        LOGE("RenderTransform is null");
        return;
    }
    if (!NearEqual(angle, 0.0)) {
        accessibilityNode->SetRotateToChild(angle, rotateAxis);
        Size size = GetLayoutSize();
        Offset globalOffset = GetGlobalOffset();
        Offset scaleCenter =
            Offset(globalOffset.GetX() + size.Width() / 2.0, globalOffset.GetY() + size.Height() / 2.0);
        accessibilityNode->SetScaleCenterToChild(scaleCenter);
        for (const auto& item : GetChildren()) {
            item->NotifyPaintFinish();
        }
    }
}
#endif

void RenderTransform::ParseTransformEffects(
    const std::vector<AnimatableTransformOperation>& transformEffects, Matrix4& transform)
{
    for (const auto& effect : transformEffects) {
        transform = transform * ParseTransformEffect(effect);
    }
}

Matrix4 RenderTransform::ParseTransformEffect(const TransformOperation& effect)
{
    switch (effect.type_) {
        case TransformOperationType::TRANSLATE: {
            auto& translate = effect.translateOperation_;
            float dx = CovertDimensionToPxBySize(translate.dx, GetLayoutSize().Width());
            float dy = CovertDimensionToPxBySize(translate.dy, GetLayoutSize().Height());
            // dz not support percent
            float dz = CovertDimensionToPxBySize(translate.dz, 0.0);
            return Matrix4::CreateTranslate(dx, dy, dz);
        }
        case TransformOperationType::SCALE: {
            auto& scale = effect.scaleOperation_;
            return Matrix4::CreateScale(scale.scaleX, scale.scaleY, scale.scaleZ);
        }
        case TransformOperationType::SKEW: {
            auto& skew = effect.skewOperation_;
            return Matrix4::CreateSkew(skew.skewX, skew.skewY);
        }
        case TransformOperationType::ROTATE: {
            auto& rotate = effect.rotateOperation_;
            return Matrix4::CreateRotate(rotate.angle, rotate.dx, rotate.dy, rotate.dz);
        }
        case TransformOperationType::MATRIX: {
            auto& matrix = effect.matrix4_;
            return matrix;
        }
        case TransformOperationType::PERSPECTIVE: {
            auto& perspective = effect.perspectiveOperation_;
            double distance = CovertDimensionToPxBySize(perspective.distance, 0.0);
            return Matrix4::CreatePerspective(distance);
        }
        case TransformOperationType::UNDEFINED:
        default:
            LOGE("unknown transform operation type %{public}d", static_cast<int32_t>(effect.type_));
            return Matrix4::CreateIdentity();
    }
}

void RenderTransform::ParseDimension(TransformOperation& effect)
{
    switch (effect.type_) {
        case TransformOperationType::TRANSLATE: {
            auto& translate = effect.translateOperation_;
            translate.dx = Dimension(CovertDimensionToPxBySize(translate.dx, GetLayoutSize().Width()));
            translate.dy = Dimension(CovertDimensionToPxBySize(translate.dy, GetLayoutSize().Height()));
            // dz not support percent
            translate.dz = Dimension(CovertDimensionToPxBySize(translate.dz, 0.0));
            break;
        }
        case TransformOperationType::PERSPECTIVE: {
            auto& perspective = effect.perspectiveOperation_;
            perspective.distance = Dimension(CovertDimensionToPxBySize(perspective.distance, 0.0));
            break;
        }
        case TransformOperationType::SCALE:
        case TransformOperationType::SKEW:
        case TransformOperationType::ROTATE:
        case TransformOperationType::MATRIX:
        case TransformOperationType::UNDEFINED:
            break;
        default:
            break;
    }
}

void RenderTransform::UpdateTransform()
{
    if (!needUpdateTransform_) {
        return;
    }
    needUpdateTransform_ = false;
    ParseTransformEffects(transformEffects_.GetOperations(), transform_);
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Update transform failed. context is null.");
        return;
    }
    if (pendingAppearing_ && hasAppearTransition_) {
        if (transformAnimation_.GetAnimationStatus() != Animator::Status::RUNNING) {
            for (auto& effect : transformEffectsAppearing_) {
                ParseDimension(effect);
            }
            transformAnimation_.SetTransformOperations(transformEffectsAppearing_);
        }
        transformAnimation_.SetAnimationStopCallback(nullptr);
        // transform from appearing Transform to identity matrix
        transformAnimation_.PlayTransformAnimation(transitionOption_, std::vector<TransformOperation>());
        pendingAppearing_ = false;
    }
}

void RenderTransform::SetTouchable(bool enable)
{
    LOGD("set transform touchable status: %{public}d", enable);
    enableTouchTest_ = enable;
}

void RenderTransform::Update(const RefPtr<Component>& component)
{
    auto transform = AceType::DynamicCast<TransformComponent>(component);
    if (transform == nullptr) {
        LOGE("transform component is nullptr.");
        return;
    }
    ResetTransform();
    needUpdateTransform_ = true;
    transform_ = transform->GetTransform();
    transformEffects_ = transform->GetTransformEffects();
    hasAppearTransition_ = transform->HasAppearTransition();
    if (hasAppearTransition_) {
        transformEffectsAppearing_ = transform->GetTransformEffectsAppearing();
    } else {
        transformEffectsAppearing_.clear();
    }
    hasDisappearTransition_ = transform->HasDisappearTransition();
    if (hasDisappearTransition_) {
        transformEffectsDisappearing_ = transform->GetTransformEffectsDisappearing();
    } else {
        transformEffectsDisappearing_.clear();
    }
    transitionOption_ = context_.Upgrade()->GetExplicitAnimationOption();
    originX_ = transform->GetOriginDimension().GetX();
    originY_ = transform->GetOriginDimension().GetY();
    SetTouchHandle(transform->GetClickSpringEffectType());
    SetShadow(transform->GetShadow());
    MarkNeedLayout();
}

void RenderTransform::PerformLayout()
{
    LOGD("RenderTransform::PerformLayout");
    auto child = GetFirstChild();
    if (child == nullptr) {
        LOGE("child component is nullptr.");
        return;
    }

    Size layoutSize;
    LayoutParam innerLayout;
    Size maxLayoutSize = GetLayoutParam().GetMaxSize();
    if (maxLayoutSize.IsValid()) {
        innerLayout.SetMaxSize(maxLayoutSize);
        child->Layout(innerLayout);
        layoutSize = child->GetLayoutSize();
    }
    SetLayoutSize(layoutSize);
    needUpdateOrigin_ = true;
    MarkNeedSyncGeometryProperties();
}

void RenderTransform::UpdateTransformOrigin()
{
    auto child = GetFirstChild();
    if (child == nullptr) {
        LOGE("child component is nullptr.");
        return;
    }
    Size layoutSize = GetLayoutSize();
    const auto& renderBoxBase = AceType::DynamicCast<RenderBoxBase>(child);
    if (renderBoxBase) {
        auto margin = renderBoxBase->GetMargin();
        double marginTop = margin.TopPx();
        double marginLeft = margin.LeftPx();
        double marginBottom = margin.BottomPx();
        double marginRight = margin.RightPx();
        double paintWidthSize = layoutSize.Width() - marginLeft - marginRight;
        double paintHeightSize = layoutSize.Height() - marginTop - marginBottom;
        origin_.SetX(CovertDimensionToPxBySize(originX_, paintWidthSize) + marginLeft);
        origin_.SetY(CovertDimensionToPxBySize(originY_, paintHeightSize) + marginTop);
    } else {
        origin_.SetX(CovertDimensionToPxBySize(originX_, layoutSize.Width()));
        origin_.SetY(CovertDimensionToPxBySize(originY_, layoutSize.Height()));
    }
}

double RenderTransform::CovertDimensionToPxBySize(const Dimension& dimension, double size)
{
    double result = 0.0;
    if (dimension.Unit() == DimensionUnit::PERCENT) {
        result = dimension.Value() * size;
    } else if (dimension.Unit() == DimensionUnit::VP) {
        result = NormalizeToPx(dimension);
    } else {
        result = dimension.Value();
    }
    return result;
};

void RenderTransform::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (rawRecognizer_) {
        rawRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(rawRecognizer_);
    }
}

void RenderTransform::SetTouchHandle(ClickSpringEffectType type)
{
    if (type == ClickSpringEffectType::NONE) {
        if (rawRecognizer_) {
            rawRecognizer_->SetOnTouchUp(nullptr);
            rawRecognizer_->SetOnTouchDown(nullptr);
            rawRecognizer_->SetOnTouchCancel(nullptr);
        }
    } else {
        if (!rawRecognizer_) {
            rawRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
        }
        if (!clickSpringEffect_) {
            clickSpringEffect_ = AceType::MakeRefPtr<ClickSpringEffect>(GetContext());
            clickSpringEffect_->SetRenderNode(WeakClaim(this));
        }
        auto touchHandle = [weak = AceType::WeakClaim(this)](
                               const TouchEventInfo&, TouchType touchType, ClickSpringEffectType effectType) {
            auto transform = weak.Upgrade();
            if (transform && transform->clickSpringEffect_) {
                transform->clickSpringEffect_->ShowAnimation(touchType, effectType);
            }
        };
        rawRecognizer_->SetOnTouchDown(std::bind(touchHandle, std::placeholders::_1, TouchType::DOWN, type));
        rawRecognizer_->SetOnTouchUp(std::bind(touchHandle, std::placeholders::_1, TouchType::UP, type));
        rawRecognizer_->SetOnTouchCancel(std::bind(touchHandle, std::placeholders::_1, TouchType::CANCEL, type));
    }
}

Matrix4 RenderTransform::UpdateWithEffectMatrix(Matrix4 matrix)
{
    auto animationMatrix = transformAnimation_.ComputerBlendedMatrix4();
    if (!animationMatrix.IsIdentityMatrix()) {
        matrix = matrix * animationMatrix;
    }
    if (clickSpringEffect_ && !disableClickEffect_) {
        double scale = clickSpringEffect_->GetScale();
        if (!NearEqual(scale, 1.0)) {
            return matrix * Matrix4::CreateScale(scale, scale, 1.0);
        }
    }

    return matrix;
}

Offset RenderTransform::GetGlobalOffsetExternal() const
{
    auto renderNode = GetParent().Upgrade();
    auto offset = renderNode ? GetPosition() + renderNode->GetGlobalOffsetExternal() : GetPosition();

    // transformPaint_[12] is translation of x,transformPaint_[13] is translation of y.
    offset += Offset(transformPaint_[12], transformPaint_[13]);
    return offset;
}

bool RenderTransform::HasDisappearingTransition(int32_t nodeId)
{
    return hasDisappearTransition_ || RenderNode::HasDisappearingTransition(nodeId);
}

void RenderTransform::OnTransition(TransitionType type, int32_t id)
{
    LOGD("OnTransition. type: %{public}d, id: %{public}d", type, id);
    if (type == TransitionType::APPEARING) {
        pendingAppearing_ = true;
        needUpdateTransform_ = true;
    } else if (type == TransitionType::DISAPPEARING && hasDisappearTransition_) {
        auto context = context_.Upgrade();
        if (!context) {
            LOGE("OnTransition failed, context_ is null");
            return;
        }
        const auto& option = context->GetExplicitAnimationOption();
        transformAnimation_.SetAnimationStopCallback([weak = AceType::WeakClaim(this)]() {
            auto renderNode = weak.Upgrade();
            if (renderNode) {
                renderNode->OnTransformDisappearingCallback();
            }
        });
        for (auto& effect : transformEffectsDisappearing_) {
            ParseDimension(effect);
        }
        transformAnimation_.PlayTransformAnimation(option, transformEffectsDisappearing_);
    }
}

void RenderTransform::ClearRenderObject()
{
    RenderNode::ClearRenderObject();
    transform_ = Matrix4::CreateIdentity();
    transformAnimation_.SetTransformOperations(std::vector<TransformOperation>());
    transformEffects_.Clear();

    needUpdateTransform_ = false;
    isFirstAnimation_ = true;
    origin_ = Offset();
    originX_ = Dimension();
    originY_ = Dimension();
    needUpdateOrigin_ = false;

    enableTouchTest_ = true;
    transformPaint_ = Matrix4::CreateIdentity();
    transitionOption_ = AnimationOption();
    transformEffectsAppearing_.clear();
    transformEffectsDisappearing_.clear();
    hasDisappearTransition_ = false;
    hasAppearTransition_ = false;
    pendingAppearing_ = false;
}

void RenderTransform::OnTransformDisappearingCallback()
{
    LOGD("OnTransformDisappearingCallback");
    RefPtr<RenderNode> child = AceType::Claim(this);
    while (child && !child->IsDisappearing()) {
        child = child->GetParent().Upgrade();
    }
    if (!child) {
        return;
    }
    auto parent = child->GetParent().Upgrade();
    if (parent) {
        parent->ClearDisappearingNode(child);
    }
}

Matrix4 RenderTransform::GetTransformByOffset(Matrix4 matrix, const Offset& offset)
{
    LOGD("Offset(%{public}lf, %{public}lf)", offset.GetX(), offset.GetY());
    if (offset.IsZero()) {
        return matrix;
    }

    Matrix4 transform =
        Matrix4::CreateTranslate(static_cast<float>(-offset.GetX()), static_cast<float>(-offset.GetY()), 0.0f);
    transform = matrix * transform;
    transform = Matrix4::CreateTranslate(static_cast<float>(offset.GetX()), static_cast<float>(offset.GetY()), 0.0f) *
                transform;
    return transform;
}

} // namespace OHOS::Ace
