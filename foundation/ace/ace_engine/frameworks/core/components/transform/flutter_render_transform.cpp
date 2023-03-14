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

#include "core/components/transform/flutter_render_transform.h"

namespace OHOS::Ace {

using namespace Flutter;

tonic::Float64List ToFloat64List(const Matrix4& matrix4)
{
    tonic::Float64List floatData(matrix4.Count());
    for (int32_t i = 0; i < matrix4.Count(); i++) {
        floatData[i] = matrix4[i];
    }
    return floatData;
}

RenderLayer FlutterRenderTransform::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderTransform::UpdateTransformLayer()
{
    UpdateTransformByGlobalOffset();
}

void FlutterRenderTransform::Paint(RenderContext& context, const Offset& offset)
{
    if (needUpdateOrigin_) {
        UpdateTransformOrigin();
        needUpdateOrigin_ = false;
    }
    UpdateTransform(); // Update transform param to Matrix.

    if (!CheckNeedPaint()) {
        return;
    }

    UpdateTransformByGlobalOffset();
    RenderNode::Paint(context, offset);
}

void FlutterRenderTransform::UpdateTransformByGlobalOffset()
{
    Offset absoluteOffset = GetTransitionGlobalOffset();
    auto currentTransform = GetEffectiveTransform(absoluteOffset);
    if (!(transformPaint_ == currentTransform)) {
        transformPaint_ = currentTransform;
        MarkNeedUpdateTouchRect(true);
        CheckIfNeedUpdateTouchRect();
    }
    if (layer_) {
        layer_->Update(transformPaint_);
    }
}

void FlutterRenderTransform::OnGlobalPositionChanged()
{
    UpdateTransformByGlobalOffset();
    RenderTransform::OnGlobalPositionChanged();
}

Point FlutterRenderTransform::GetTransformPoint(const Point& point)
{
    Matrix4 transform = GetEffectiveTransform(GetPosition());
    return Matrix4::Invert(transform) * point;
}

Rect FlutterRenderTransform::GetTransformRect(const Rect& rect)
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

Matrix4 FlutterRenderTransform::GetEffectiveTransform(const Offset& offset)
{
    Matrix4 transform = GetTransformByOffset(UpdateWithEffectMatrix(transform_), origin_);
    if (!offset.IsZero()) {
        transform = GetTransformByOffset(transform, offset);
    }
    return transform;
}

bool FlutterRenderTransform::HasEffectiveTransform() const
{
    if (!layer_) {
        return false;
    }
    return !layer_->GetMatrix4().IsIdentityMatrix();
}

bool FlutterRenderTransform::CheckNeedPaint() const
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

void FlutterRenderTransform::Mirror(const Offset& center, const Offset& global)
{
    float angle = 180.0f;
    if (!center.IsZero()) {
        transform_ = FlutterRenderTransform::GetTransformByOffset(transform_, center);
    }
    Matrix4 rotate = Matrix4::CreateRotate(angle, 0.0f, 1.0f, 0.0f);
    transform_ = rotate * transform_;

    UpdateTransformLayer();
    auto context = context_.Upgrade();
    if (context) {
        context->MarkForcedRefresh();
    }
}

} // namespace OHOS::Ace
