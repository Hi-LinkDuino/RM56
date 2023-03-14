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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_BUTTON_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_BUTTON_COMPONENT_H

#include "base/geometry/dimension.h"
#include "base/utils/label_target.h"
#include "base/utils/macros.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/border_edge.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/state_attributes.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/measurable.h"
#include "frameworks/core/components/declaration/button/button_declaration.h"

namespace OHOS::Ace {

// Layout size extend to max which is specified by parent node when rendering.
constexpr int32_t LAYOUT_FLAG_EXTEND_TO_PARENT = 1;

using ProgressCallback = std::function<void(uint32_t)>;

enum class ButtonStateAttribute { COLOR, RADIUS, HEIGHT, WIDTH };

class ACE_EXPORT ButtonComponent : public ComponentGroup, public LabelTarget, public Measurable {
    DECLARE_ACE_TYPE(ButtonComponent, ComponentGroup, LabelTarget, Measurable);

public:
    explicit ButtonComponent(const std::list<RefPtr<Component>>& children);
    ~ButtonComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    ButtonType GetType() const
    {
        return type_;
    }

    void SetType(ButtonType type)
    {
        type_ = type;
    }

    bool GetDisabledState() const;
    bool GetWaitingState() const;
    bool GetAutoFocusState() const;
    bool GetRadiusState() const;
    bool GetCatchMode() const;
    const Dimension& GetMinWidth() const;
    const Dimension& GetRectRadius() const;
    const Dimension& GetProgressDiameter() const;
    const Color& GetBackgroundColor() const;
    const Color& GetClickedColor() const;
    const Color& GetDisabledColor() const;
    const Color& GetFocusColor() const;
    const Color& GetHoverColor() const;
    const Color& GetProgressColor() const;
    const Color& GetProgressFocusColor() const;
    const Color& GetFocusAnimationColor() const;
    const BorderEdge& GetBorderEdge() const;
    const EventMarker& GetClickedEventId() const;
    const EventMarker& GetKeyEnterEventId() const;
    const EventMarker& GetRemoteMessageEventId() const;
    RefPtr<ButtonProgressController> GetButtonController() const;

    void SetDisabledState(bool state);
    void SetWaitingState(bool state);
    void SetAutoFocusState(bool state);
    void SetRadiusState(bool state);
    void SetMinWidth(const Dimension& width);
    void SetRectRadius(const Dimension& radius);
    void SetCatchMode(bool catchMode);
    void SetProgressDiameter(const Dimension& diameter);
    void SetBackgroundColor(const Color& color);
    void SetClickedColor(const Color& color);
    void SetDisabledColor(const Color& color);
    void SetFocusColor(const Color& color);
    void SetHoverColor(const Color& color);
    void SetProgressColor(const Color& color);
    void SetProgressFocusColor(const Color& color);
    void SetFocusAnimationColor(const Color& color);
    void SetBorderEdge(const BorderEdge& borderEdge);
    void SetClickedEventId(const EventMarker& eventId);
    void SetKeyEnterEventId(const EventMarker& eventId);
    void SetRemoteMessageEventId(const EventMarker& eventId);
    void SetClickFunction(std::function<void()>&& clickCallback);
    void SetDeclaration(const RefPtr<ButtonDeclaration>& declaration);

    bool GetFocusable() const
    {
        return focusable_;
    }

    void SetFocusable(bool focusable)
    {
        focusable_ = focusable;
    }

    uint32_t GetLayoutFlag() const
    {
        return layoutFlag_;
    }

    void SetLayoutFlag(uint32_t flag)
    {
        layoutFlag_ = flag;
    }

    bool IsInnerBorder() const
    {
        return isInnerBorder_;
    }

    void SetIsInnerBorder(bool isInnerBorder)
    {
        isInnerBorder_ = isInnerBorder;
    }

    bool GetStateEffect() const
    {
        return stateEffect_;
    }

    void SetStateEffect(bool effect)
    {
        stateEffect_ = effect;
    }

    bool GetDeclarativeFlag() const
    {
        return isDeclarative_;
    }

    void SetDeclarativeFlag(bool flag)
    {
        isDeclarative_ = flag;
    }

    void SetRectRadii(const std::array<Radius, 4>& radii)
    {
        radii_ = radii;
    }

    const std::array<Radius, 4>& GetRectRadii() const
    {
        return radii_;
    }

    void SetInputButton(bool inputButton)
    {
        isInputButton_ = inputButton;
    }

    bool IsInputButton() const
    {
        return isInputButton_;
    }

    void SetDeclareHeight(bool declareHeight)
    {
        isDeclareHeight_ = declareHeight;
    }

    bool IsDeclareHeight()
    {
        return isDeclareHeight_;
    }

    void FitTextHeight(AnimatableDimension& height);

    uint32_t Compare(const RefPtr<Component>& component) const override;

    RefPtr<StateAttributes<ButtonStateAttribute>> GetStateAttributes()
    {
        if (stateAttributeList_ == nullptr) {
            stateAttributeList_ = MakeRefPtr<StateAttributes<ButtonStateAttribute>>();
        }
        return stateAttributeList_;
    }

    bool HasStateAttributes()
    {
        return stateAttributeList_ != nullptr;
    }

private:
    RefPtr<ButtonDeclaration> declaration_;
    ButtonType type_ { ButtonType::NORMAL };
    bool isInnerBorder_ = false;
    bool focusable_ = true;
    bool stateEffect_ = true;
    bool isDeclarative_ = false;
    bool isInputButton_ = false;
    bool isCatchMode_ = true;
    bool isDeclareHeight_ = false;
    uint32_t layoutFlag_ = 0;
    // for custom button type
    std::array<Radius, 4> radii_ = { Radius(0.0_vp), Radius(0.0_vp), Radius(0.0_vp), Radius(0.0_vp) };
    RefPtr<StateAttributes<ButtonStateAttribute>> stateAttributeList_;
    EventMarker keyEnterId_;
};

class ButtonBuilder {
public:
    static RefPtr<ButtonComponent> Build(const RefPtr<ThemeManager>& themeManager, const std::string& text);
    static RefPtr<ButtonComponent> Build(const RefPtr<ThemeManager>& themeManager, const std::string& text,
        TextStyle& textStyle, const Color& textFocusColor = Color(), bool useTextFocus = false);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_BUTTON_COMPONENT_H
