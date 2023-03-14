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

#include "core/components/transform/transform_component.h"

#include "core/components/transform/transform_element.h"

namespace OHOS::Ace {

RefPtr<Element> TransformComponent::CreateElement()
{
    return AceType::MakeRefPtr<TransformElement>();
}

RefPtr<RenderNode> TransformComponent::CreateRenderNode()
{
    return RenderTransform::Create();
}

// translate
void TransformComponent::Translate(const Dimension& x, const Dimension& y, const AnimationOption& animationOption)
{
    Translate(x, y, Dimension(), animationOption);
}

void TransformComponent::Translate(
    const Dimension& x, const Dimension& y, const Dimension& z, const AnimationOption& animationOption)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::TRANSLATE;
    operation.translateOperation_ = TranslateOperation(x, y, z);
    transformEffects.AddTransformOperation(operation, animationOption);
}

void TransformComponent::TranslateX(const Dimension& x, const AnimationOption& animationOption)
{
    Translate(x, Dimension {}, Dimension {}, animationOption);
}

void TransformComponent::TranslateY(const Dimension& y, const AnimationOption& animationOption)
{
    Translate(Dimension {}, y, Dimension {}, animationOption);
}

void TransformComponent::TranslateZ(const Dimension& z, const AnimationOption& animationOption)
{
    Translate(Dimension {}, Dimension {}, z);
}

// scale
void TransformComponent::Scale(float value, const AnimationOption& animationOption)
{
    Scale(value, value, 1.0f, animationOption);
}

void TransformComponent::Scale(float x, float y, const AnimationOption& animationOption)
{
    Scale(x, y, 1.0f, animationOption);
}

void TransformComponent::Scale(float x, float y, float z, const AnimationOption& animationOption)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::SCALE;
    operation.scaleOperation_ = ScaleOperation(x, y, z);
    transformEffects.AddTransformOperation(operation, animationOption);
}

void TransformComponent::ScaleX(float x, const AnimationOption& animationOption)
{
    Scale(x, 1.0f, 1.0f, animationOption);
}

void TransformComponent::ScaleY(float y, const AnimationOption& animationOption)
{
    Scale(1.0f, y, 1.0f, animationOption);
}

void TransformComponent::ScaleZ(float z, const AnimationOption& animationOption)
{
    Scale(1.0f, 1.0f, z, animationOption);
}

// rotate
void TransformComponent::Rotate(float dx, float dy, float dz, float angle, const AnimationOption& animationOption)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::ROTATE;
    operation.rotateOperation_ = RotateOperation(dx, dy, dz, angle);
    transformEffects.AddTransformOperation(operation, animationOption);
}

void TransformComponent::RotateX(float angle, const AnimationOption& animationOption)
{
    Rotate(1.0f, 0.0f, 0.0f, angle, animationOption);
}

void TransformComponent::RotateY(float angle, const AnimationOption& animationOption)
{
    Rotate(0.0f, 1.0f, 0.0f, angle, animationOption);
}

void TransformComponent::RotateZ(float angle, const AnimationOption& animationOption)
{
    Rotate(0.0f, 0.0f, 1.0f, angle, animationOption);
}

// skew
void TransformComponent::Skew(float xAngle, float yAngle, const AnimationOption& animationOption)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::SKEW;
    operation.skewOperation_ = SkewOperation(xAngle, yAngle);
    transformEffects.AddTransformOperation(operation, animationOption);
}

void TransformComponent::SkewX(float angle, const AnimationOption& animationOption)
{
    Skew(angle, 0.0f, animationOption);
}

void TransformComponent::SkewY(float angle, const AnimationOption& animationOption)
{
    Skew(0.0f, angle, animationOption);
}

void TransformComponent::Perspective(const Dimension& value, const AnimationOption& animationOption)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::PERSPECTIVE;
    operation.perspectiveOperation_ = PerspectiveOperation(value);
    transformEffects.AddTransformOperation(operation, animationOption);
}

void TransformComponent::Matrix(
    float a, float b, float c, float d, float dx, float dy, const AnimationOption& animationOption)
{
    return Matrix3d(a, b, 0, 0, c, d, 0, 0, 0, 0, 1, 0, dx, dy, 0, 1);
}

void TransformComponent::Matrix3d(float a1, float b1, float c1, float d1, float a2, float b2, float c2, float d2,
    float a3, float b3, float c3, float d3, float a4, float b4, float c4, float d4,
    const AnimationOption& animationOption)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::MATRIX;
    operation.matrix4_ = Matrix4(a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4);
    transformEffects.AddTransformOperation(operation, animationOption);
}

void TransformComponent::Matrix3d(Matrix4 m, const AnimationOption& animationOption)
{
    TransformOperation operation;
    operation.type_ = TransformOperationType::MATRIX;
    operation.matrix4_ = m;
    transformEffects.AddTransformOperation(operation, animationOption);
}

void TransformComponent::SetRotateTransition(TransitionType type, double x, double y, double z, double angle)
{
    if (type == TransitionType::ALL) {
        SetRotateTransition(TransitionType::APPEARING, x, y, z, angle);
        SetRotateTransition(TransitionType::DISAPPEARING, x, y, z, angle);
        hasDisappearTransition_ = true;
        hasAppearTransition_ = true;
        return;
    }

    TransformOperation operation;
    operation.type_ = TransformOperationType::ROTATE;
    operation.rotateOperation_ = RotateOperation(x, y, z, angle);
    if (type == TransitionType::APPEARING) {
        transformEffectsAppearing_.emplace_back(operation);
        hasAppearTransition_ = true;
    } else if (type == TransitionType::DISAPPEARING) {
        transformEffectsDisappearing_.emplace_back(operation);
        hasDisappearTransition_ = true;
    }
}

void TransformComponent::SetTranslateTransition(
    TransitionType type, const Dimension& x, const Dimension& y, const Dimension& z)
{
    if (type == TransitionType::ALL) {
        SetTranslateTransition(TransitionType::APPEARING, x, y, z);
        SetTranslateTransition(TransitionType::DISAPPEARING, x, y, z);
        hasDisappearTransition_ = true;
        hasAppearTransition_ = true;
        return;
    }

    TransformOperation operation;
    operation.type_ = TransformOperationType::TRANSLATE;
    operation.translateOperation_ = TranslateOperation(x, y, z);
    if (type == TransitionType::APPEARING) {
        transformEffectsAppearing_.emplace_back(operation);
        hasAppearTransition_ = true;
    } else if (type == TransitionType::DISAPPEARING) {
        transformEffectsDisappearing_.emplace_back(operation);
        hasDisappearTransition_ = true;
    }
}

void TransformComponent::SetScaleTransition(TransitionType type, float x, float y, float z)
{
    if (type == TransitionType::ALL) {
        SetScaleTransition(TransitionType::APPEARING, x, y, z);
        SetScaleTransition(TransitionType::DISAPPEARING, x, y, z);
        hasDisappearTransition_ = true;
        hasAppearTransition_ = true;
        return;
    }
    TransformOperation operation;
    operation.type_ = TransformOperationType::SCALE;
    operation.scaleOperation_ = ScaleOperation(x, y, z);
    if (type == TransitionType::APPEARING) {
        transformEffectsAppearing_.emplace_back(operation);
        hasAppearTransition_ = true;
    } else if (type == TransitionType::DISAPPEARING) {
        transformEffectsDisappearing_.emplace_back(operation);
        hasDisappearTransition_ = true;
    }
}

} // namespace OHOS::Ace
