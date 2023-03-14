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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_PROPERTIES_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_PROPERTIES_H

#include "base/geometry/dimension.h"
#include "base/memory/ace_type.h"
#include "core/animation/property_animatable_helper.h"
#include "core/animation/property_animation.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/shadow.h"

namespace OHOS::Ace {

class AnimatableProperties : public virtual AceType {
    DECLARE_ACE_TYPE(AnimatableProperties, AceType);

public:
    void UpdatePropAnimation(const PropAnimationMap& propAnimations);

private:
    void SetPropWidth(const Dimension& width);
    Dimension GetPropWidth() const;
    void SetPropHeight(const Dimension& height);
    Dimension GetPropHeight() const;
    void SetPropBgColor(const Color& color);
    Color GetPropBgColor() const;
    void SetPropOpacity(float opacity);
    float GetPropOpacity() const;
    void SetPropShadow(const Shadow& shadow);
    Shadow GetPropShadow() const;
    void SetPropBackgroundPosition(const BackgroundImagePosition& position);
    BackgroundImagePosition GetPropBackgroundImagePosition() const;
    void SetPropBackgroundSize(const BackgroundImageSize& size);
    BackgroundImageSize GetPropBackgroundImageSize() const;
    void SetPadding(const AnimatableDimension& value, const DimensionHelper& helper);
    AnimatableDimension GetPadding(const DimensionHelper& helper);
    void SetMargin(const AnimatableDimension& value, const DimensionHelper& helper);
    AnimatableDimension GetMargin(const DimensionHelper& helper) const;
    void SetBorderWidth(float value, const BorderEdgeHelper& helper);
    float GetBorderWidth(const BorderEdgeHelper& helper) const;
    void SetBorderColor(const Color& color, const BorderEdgeHelper& helper);
    Color GetBorderColor(const BorderEdgeHelper& helper) const;
    void SetBorderStyle(BorderStyle style, const BorderEdgeHelper& helper);
    BorderStyle GetBorderStyle(const BorderEdgeHelper& helper) const;
    void SetBorderRadius(float value, const BorderRadiusHelper& helper);
    float GetBorderRadius(const BorderRadiusHelper& helper) const;
    void SetBlurRadius(const AnimatableDimension& radius);
    AnimatableDimension GetBlurRadius() const;
    void SetBackdropRadius(const AnimatableDimension& radius);
    AnimatableDimension GetBackdropRadius() const;
    void SetWindowBlurProgress(double progress);
    double GetWindowBlurProgress() const;
    void SetPositionLeft(const Dimension& left);
    Dimension GetPositionLeft() const;
    void SetPositionTop(const Dimension& top);
    Dimension GetPositionTop() const;
    void SetPositionRight(const Dimension& right);
    Dimension GetPositionRight() const;
    void SetPositionBottom(const Dimension& bottom);
    Dimension GetPositionBottom() const;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATABLE_PROPERTIES_H
