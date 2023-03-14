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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_TRANSFORM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_TRANSFORM_COMPONENT_H

#include "base/geometry/dimension_offset.h"
#include "base/geometry/matrix4.h"
#include "base/geometry/transform_util.h"
#include "core/animation/animatable_transform_operation.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/transform/render_transform.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

inline constexpr int32_t INDEX_TWO = 2;
inline constexpr int32_t INDEX_THREE = 3;
inline constexpr float PERSPECTIVE = 0.0005f;
inline constexpr Dimension HALF_PERCENT = 0.5_pct;

enum class TransformStateAttribute {
    ROTATE,
    ROTATEX,
    ROTATEY,
    SCALE,
    SCALEX,
    SCALEY,
    TRANSLATE,
    TRANSLATEX,
    TRANSLATEY,
};

class ACE_EXPORT TransformComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(TransformComponent, SoleChildComponent)

public:
    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;
    // translate
    void Translate(const Dimension& x, const Dimension& y, const AnimationOption& animationOption = AnimationOption());
    void Translate(const Dimension& x, const Dimension& y, const Dimension& z,
        const AnimationOption& animationOption = AnimationOption());
    void TranslateX(const Dimension& x, const AnimationOption& animationOption = AnimationOption());
    void TranslateY(const Dimension& y, const AnimationOption& animationOption = AnimationOption());
    void TranslateZ(const Dimension& z, const AnimationOption& animationOption = AnimationOption());
    // scale
    void Scale(float value, const AnimationOption& animationOption = AnimationOption());
    void Scale(float x, float y, const AnimationOption& animationOption = AnimationOption());
    void Scale(float x, float y, float z, const AnimationOption& animationOption = AnimationOption());
    void ScaleX(float x, const AnimationOption& animationOption = AnimationOption());
    void ScaleY(float y, const AnimationOption& animationOption = AnimationOption());
    void ScaleZ(float z, const AnimationOption& animationOption = AnimationOption());
    // rotate
    void Rotate(float dx, float dy, float dz, float angle, const AnimationOption& animationOption = AnimationOption());
    void RotateX(float angle, const AnimationOption& animationOption = AnimationOption());
    void RotateY(float angle, const AnimationOption& animationOption = AnimationOption());
    void RotateZ(float angle, const AnimationOption& animationOption = AnimationOption());
    // skew
    void Skew(float xAngle, float yAngle, const AnimationOption& animationOption = AnimationOption());
    void SkewX(float angle, const AnimationOption& = AnimationOption());
    void SkewY(float angle, const AnimationOption& = AnimationOption());
    // perspective
    void Perspective(const Dimension& value, const AnimationOption& = AnimationOption());
    // matrix2d
    void Matrix(float a, float b, float c, float d, float dx, float dy, const AnimationOption& = AnimationOption());
    // matrix3d in the column-major order.
    void Matrix3d(float a1, float b1, float c1, float d1, float a2, float b2, float c2, float d2, float a3, float b3,
        float c3, float d3, float a4, float b4, float c4, float d4, const AnimationOption& = AnimationOption());
    void Matrix3d(Matrix4 m, const AnimationOption& = AnimationOption());

    const DimensionOffset& GetOriginDimension() const
    {
        return originDimension_;
    }

    void SetOriginDimension(const DimensionOffset& origin)
    {
        originDimension_ = origin;
    }

    const AnimatableTransformOperations& GetTransformEffects() const
    {
        return transformEffects;
    }

    const Matrix4& GetTransform() const
    {
        return transform_;
    }

    void SetTransform(const Matrix4& transform)
    {
        transform_ = transform;
    }

    void ResetTransform()
    {
        transform_ = Matrix4::CreateIdentity();
        transformEffects.Clear();
    }

    void SetClickSpringEffectType(ClickSpringEffectType type)
    {
        clickSpringEffectType_ = type;
    }

    ClickSpringEffectType GetClickSpringEffectType() const
    {
        return clickSpringEffectType_;
    }

    void SetTransitionEffect(TransitionEffect transitionEffect)
    {
        transitionEffect_ = transitionEffect;
    }

    TransitionEffect GetTransitionEffect() const
    {
        return transitionEffect_;
    }

    void SetShadow(const Shadow& shadow)
    {
        shadow_ = shadow;
    }

    const Shadow& GetShadow() const
    {
        return shadow_;
    }

    const std::vector<TransformOperation>& GetTransformEffectsAppearing() const
    {
        return transformEffectsAppearing_;
    }

    const std::vector<TransformOperation>& GetTransformEffectsDisappearing() const
    {
        return transformEffectsDisappearing_;
    }

    bool HasDisappearTransition() const
    {
        return hasDisappearTransition_;
    }

    bool HasAppearTransition() const
    {
        return hasAppearTransition_;
    }

    void SetScaleTransition(TransitionType type, float x, float y, float z);
    void SetTranslateTransition(TransitionType type, const Dimension& x, const Dimension& y, const Dimension& z);
    void SetRotateTransition(TransitionType type, double x, double y, double z, double angle);

    RefPtr<StateAttributes<TransformStateAttribute>> GetStateAttributes()
    {
        if (stateAttributeList_ == nullptr) {
            stateAttributeList_ = MakeRefPtr<StateAttributes<TransformStateAttribute>>();
        }
        return stateAttributeList_;
    }

    bool HasStateAttributes()
    {
        return stateAttributeList_ != nullptr;
    }


private:
    Matrix4 transform_;
    AnimatableTransformOperations transformEffects;
    DimensionOffset originDimension_ = DimensionOffset(HALF_PERCENT, HALF_PERCENT);
    std::vector<TransformOperation> transformEffectsAppearing_;
    std::vector<TransformOperation> transformEffectsDisappearing_;
    bool hasDisappearTransition_ = false;
    bool hasAppearTransition_ = false;
    ClickSpringEffectType clickSpringEffectType_ = ClickSpringEffectType::NONE;
    TransitionEffect transitionEffect_ = TransitionEffect::NONE;
    Shadow shadow_;
    RefPtr<StateAttributes<TransformStateAttribute>> stateAttributeList_ = nullptr;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSFORM_TRANSFORM_COMPONENT_H
