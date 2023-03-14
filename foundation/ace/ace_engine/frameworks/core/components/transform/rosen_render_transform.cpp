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

#include "core/components/transform/rosen_render_transform.h"

#include "render_service_client/core/animation/rs_transition.h"
#include "render_service_client/core/ui/rs_node.h"

namespace OHOS::Ace {

void RosenRenderTransform::Update(const RefPtr<Component>& component)
{
    RenderTransform::Update(component);
    pendingUpdateTransformLayer_ = true;
    MarkNeedSyncGeometryProperties();
    if (pendingAppearingTransition_ && hasAppearTransition_) {
        // we have a pending appearing transition
        OnRSTransition(TransitionType::APPEARING);
        pendingAppearingTransition_ = false;
    }
}

void RosenRenderTransform::UpdateTransformLayer()
{
    auto rsNode = GetRSNode();
    if (!rsNode) {
        return;
    }

    if (needUpdateTransform_) {
        UpdateTransform(); // Update transform param to Matrix.
    }
    SyncOriginToRsNode(rsNode);

    auto transform = UpdateWithEffectMatrix(transform_);
    if (transform == previousTransformMatrix_) {
        return;
    }
    SyncTransformToRsNode(rsNode, transform);
    previousTransformMatrix_ = transform;
}

void RosenRenderTransform::SyncTransformToRsNode(const std::shared_ptr<RSNode>& rsNode, const Matrix4& transformMatrix)
{
    if (rsNode == nullptr) {
        return;
    }

    DecomposedTransform transform;
    if (!TransformUtil::DecomposeTransform(transform, transformMatrix)) {
        // failback to basic matrix decompose
        rsNode->SetTranslate(transformMatrix.Get(0, 3), transformMatrix.Get(1, 3), transformMatrix.Get(2, 3));
        rsNode->SetScale(0);
        return;
    }

    // translate
    rsNode->SetTranslate(transform.translate[0], transform.translate[1], transform.translate[2]);

    // scale
    rsNode->SetScale(transform.scale[0], transform.scale[1]);

    // rotation
    rsNode->SetRotation({
        transform.quaternion.GetX(),
        transform.quaternion.GetY(),
        transform.quaternion.GetZ(),
        transform.quaternion.GetW(),
    });
}

void RosenRenderTransform::SyncOriginToRsNode(const std::shared_ptr<RSNode>& rsNode)
{
    if (!needUpdateOrigin_ || rsNode == nullptr) {
        return;
    }

    UpdateTransformOrigin();
    needUpdateOrigin_ = false;

    rsNode->SetPivot(ConvertDimensionToScaleBySize(originX_, GetLayoutSize().Width()),
        ConvertDimensionToScaleBySize(originY_, GetLayoutSize().Height()));
}

void RosenRenderTransform::OnAttachContext()
{
    transformAnimation_.SetContextAndCallback(context_, [weak = AceType::WeakClaim(this)]() {
        auto renderNode = weak.Upgrade();
        if (renderNode) {
            renderNode->UpdateTransformLayer();
        }
    });

    transformEffects_.SetContextAndCallback(context_, [weak = AceType::WeakClaim(this)]() {
        auto renderNode = weak.Upgrade();
        if (renderNode) {
            renderNode->needUpdateTransform_ = true;
            renderNode->ResetTransform();
            renderNode->UpdateTransformLayer();
        }
    });
}

void RosenRenderTransform::SyncGeometryProperties()
{
    RenderNode::SyncGeometryProperties();
    if (pendingUpdateTransformLayer_) {
        UpdateTransformLayer();
        pendingUpdateTransformLayer_ = false;
    }
}

Point RosenRenderTransform::GetTransformPoint(const Point& point)
{
    Matrix4 transform = GetEffectiveTransform(GetPosition());
    return Matrix4::Invert(transform) * point;
}

Rect RosenRenderTransform::GetTransformRect(const Rect& rect)
{
    Matrix4 transform = GetEffectiveTransform(GetTransitionPaintRect().GetOffset());
    Point ltPoint = transform * Point(rect.Left(), rect.Top());
    Point rtPoint = transform * Point(rect.Right(), rect.Top());
    Point lbPoint = transform * Point(rect.Left(), rect.Bottom());
    Point rbPoint = transform * Point(rect.Right(), rect.Bottom());
    auto left = std::min(std::min(ltPoint.GetX(), rtPoint.GetX()), std::min(lbPoint.GetX(), rbPoint.GetX()));
    auto right = std::max(std::max(ltPoint.GetX(), rtPoint.GetX()), std::max(lbPoint.GetX(), rbPoint.GetX()));
    auto top = std::min(std::min(ltPoint.GetY(), rtPoint.GetY()), std::min(lbPoint.GetY(), rbPoint.GetY()));
    auto bottom = std::max(std::max(ltPoint.GetY(), rtPoint.GetY()), std::max(lbPoint.GetY(), rbPoint.GetY()));
    return Rect(left, top, right - left, bottom - top).CombineRect(rect);
}

Matrix4 RosenRenderTransform::GetEffectiveTransform(const Offset& offset)
{
    Matrix4 transform = GetTransformByOffset(UpdateWithEffectMatrix(transform_), origin_);
    if (!offset.IsZero()) {
        transform = GetTransformByOffset(transform, offset);
    }
    return transform;
}

bool RosenRenderTransform::HasEffectiveTransform() const
{
    return previousTransformMatrix_.IsIdentityMatrix();
}

bool RosenRenderTransform::CheckNeedPaint() const
{
    double rotateX = 0.0;
    double rotateY = 0.0;
    double sy = sqrt(transform_[0] * transform_[0] + transform_[4] * transform_[4]);
    if (NearZero(sy)) {
        rotateX = atan2(-transform_[6], transform_[5]);
        rotateY = atan2(-transform_[8], sy);
    } else {
        rotateX = atan2(transform_[9], transform_[10]);
        rotateY = atan2(-transform_[8], sy);
    }
    rotateX = std::abs(rotateX * (180.0f / M_PI));
    rotateY = std::abs(rotateY * (180.0f / M_PI));
    if (NearEqual(rotateX, 90.0, 1e-5) || NearEqual(rotateY, 90.0, 1e-5)) {
        return false; // If RotateX or RotateY is 90 deg, not need to paint.
    }
    return true;
}

void RosenRenderTransform::Mirror(const Offset& center, const Offset& global)
{
    float angle = 180.0f;
    if (!center.IsZero()) {
        transform_ = RosenRenderTransform::GetTransformByOffset(transform_, center);
    }
    Matrix4 rotate = Matrix4::CreateRotate(angle, 0.0f, 1.0f, 0.0f);
    transform_ = rotate * transform_;

    auto context = context_.Upgrade();
    if (context) {
        context->MarkForcedRefresh();
    }
}

double RosenRenderTransform::ConvertDimensionToScaleBySize(const Dimension& dimension, double size)
{
    if (dimension.Unit() == DimensionUnit::PERCENT) {
        return dimension.Value();
    }

    auto context = GetContext().Upgrade();
    if (size <= 0. || context == nullptr) {
        return 0.5;
    }

    auto dipScale = context->GetDipScale();
    return dimension.ConvertToPx(dipScale) / size;
}

void RosenRenderTransform::PerformLayout()
{
    RenderTransform::PerformLayout();
    if (auto rsNode = GetRSNode()) {
        SyncOriginToRsNode(rsNode);
    }
}

void RosenRenderTransform::OnRSTransition(TransitionType type)
{
    if (GetRSNode() == nullptr) {
        return;
    }
    std::vector<TransformOperation>* transforms;
    bool appearing;
    if (type == TransitionType::APPEARING && hasAppearTransition_) {
        transforms = &transformEffectsAppearing_;
        appearing = true;
    } else if (type == TransitionType::DISAPPEARING && hasDisappearTransition_) {
        transforms = &transformEffectsDisappearing_;
        appearing = false;
    } else {
        return;
    }

    auto effect = Rosen::RSTransitionEffect::Create();
    for (auto& transform : *transforms) {
        switch (transform.type_) {
            case TransformOperationType::SCALE: {
                auto& scale = transform.scaleOperation_;
                effect->Scale({ scale.scaleX, scale.scaleY, scale.scaleZ });
                break;
            }
            case TransformOperationType::TRANSLATE: {
                auto& translate = transform.translateOperation_;
                effect->Translate({ translate.dx.Value(), translate.dy.Value(), translate.dz.Value() });
                break;
            }
            case TransformOperationType::ROTATE: {
                auto& rotate = transform.rotateOperation_;
                effect->Rotate({ rotate.dx, rotate.dy, rotate.dz, rotate.angle });
                break;
            }
            default: {
                break;
            }
        }
    }
    GetRSNode()->NotifyTransition(effect, appearing);
}

} // namespace OHOS::Ace
