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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DISPLAY_DISPLAY_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DISPLAY_DISPLAY_COMPONENT_H

#include "core/components/common/properties/animatable_double.h"
#include "core/components/common/properties/animation_option.h"
#include "core/components/display/display_element.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

enum class DisplayStateAttribute {
    OPACITY,
};

enum class VisibleType {
    VISIBLE,
    INVISIBLE,
    GONE,
};

class ACE_EXPORT DisplayComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(DisplayComponent, SoleChildComponent);

public:
    DisplayComponent() = default;
    explicit DisplayComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}
    ~DisplayComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<DisplayElement>();
    }

    VisibleType GetVisible() const
    {
        return visible_;
    }

    double GetOpacity() const
    {
        return opacity_.GetValue();
    }

    AnimationOption GetOpacityAnimationOption() const
    {
        return opacity_.GetAnimationOption();
    }

    void SetVisible(VisibleType visible)
    {
        visible_ = visible;
    }

    void SetOpacity(double opacity, const AnimationOption& animationOption = AnimationOption())
    {
        opacity_ = AnimatableDouble(opacity, animationOption);
    }

    void DisableLayer(bool disable)
    {
        disableLayer_ = disable;
    }

    bool IsDisableLayer() const
    {
        return disableLayer_;
    }

    void SetShadow(const Shadow& shadow)
    {
        shadow_ = shadow;
    }

    const Shadow& GetShadow() const
    {
        return shadow_;
    }

    void SetTransition(TransitionType type, double opacity)
    {
        if (type == TransitionType::DISAPPEARING) {
            hasDisappearTransition_ = true;
            disappearingOpacity_ = opacity;
        } else if (type == TransitionType::APPEARING) {
            hasAppearTransition_ = true;
            appearingOpacity_ = opacity;
        } else {
            hasDisappearTransition_ = true;
            hasAppearTransition_ = true;
            disappearingOpacity_ = opacity;
            appearingOpacity_ = opacity;
        }
    }

    bool HasDisappearTransition() const
    {
        return hasDisappearTransition_;
    }

    bool HasAppearTransition() const
    {
        return hasAppearTransition_;
    }

    double GetAppearingOpacity() const
    {
        return appearingOpacity_;
    }

    double GetDisappearingOpacity() const
    {
        return disappearingOpacity_;
    }

    void SetAppearingDuration(int32_t duration)
    {
        duration_ = duration;
    }

    // Duration in millisecond.
    int32_t GetAppearingDuration() const
    {
        return duration_;
    }

    RefPtr<StateAttributes<DisplayStateAttribute>> GetStateAttributes()
    {
        if (stateAttributeList_ == nullptr) {
            stateAttributeList_ = MakeRefPtr<StateAttributes<DisplayStateAttribute>>();
        }
        return stateAttributeList_;
    }

    bool HasStateAttributes()
    {
        return stateAttributeList_ != nullptr;
    }

private:
    VisibleType visible_ = VisibleType::VISIBLE;
    Shadow shadow_;
    AnimatableDouble opacity_ = AnimatableDouble(1.0);
    double appearingOpacity_ = 0.0;
    double disappearingOpacity_ = 0.0;
    bool hasDisappearTransition_ = false;
    bool hasAppearTransition_ = false;
    bool disableLayer_ = false;
    int32_t duration_ = 0;
    RefPtr<StateAttributes<DisplayStateAttribute>> stateAttributeList_ = nullptr;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DISPLAY_DISPLAY_COMPONENT_H
