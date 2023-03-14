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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_COMPONENT_H

#include "core/components/stepper/stepper_item_component.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

using ChangeStatusFunc = std::function<void(const std::string&, const std::string&)>;

class StepperController : public virtual AceType {
    DECLARE_ACE_TYPE(StepperController, AceType);

public:
    void SetRightButtonStatus(const std::string& status, const std::string& label)
    {
        if (setRightButtonStatusImpl_) {
            setRightButtonStatusImpl_(status, label);
        }
    }

    void SetRightButtonStatusImpl(const ChangeStatusFunc& setRightButtonStatusImpl)
    {
        setRightButtonStatusImpl_ = setRightButtonStatusImpl;
    }

private:
    ChangeStatusFunc setRightButtonStatusImpl_;
};

class ACE_EXPORT StepperComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(StepperComponent, ComponentGroup);

public:
    StepperComponent() = default;
    explicit StepperComponent(const std::list<RefPtr<Component>>& children);
    ~StepperComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;
    void SetStepperLabels(const std::vector<StepperLabels>& stepperLabels);
    const std::vector<StepperLabels>& GetStepperLabels() const;
    void InsertChild(int32_t position, const RefPtr<Component>& child) override;
    void AppendChild(const RefPtr<Component>& child) override;
    void RemoveChild(const RefPtr<Component>& child) override;

    RefPtr<StepperController> GetStepperController() const
    {
        return stepperController_;
    }

    int32_t GetIndex() const
    {
        return index_;
    }

    void SetIndex(int32_t index)
    {
        index_ = index;
    }

    void SetFinishEventId(const EventMarker& finishEventId)
    {
        finishEventId_ = finishEventId;
    }

    const EventMarker& GetFinishEventId() const
    {
        return finishEventId_;
    }

    void SetSkipEventId(const EventMarker& skipEventId)
    {
        skipEventId_ = skipEventId;
    }

    const EventMarker& GetSkipEventId() const
    {
        return skipEventId_;
    }

    void SetChangeEventId(const EventMarker& changeEventId)
    {
        changeEventId_ = changeEventId;
    }

    const EventMarker& GetChangeEventId() const
    {
        return changeEventId_;
    }

    void SetNextEventId(const EventMarker& nextEventId)
    {
        nextEventId_ = nextEventId;
    }

    const EventMarker& GetNextEventId() const
    {
        return nextEventId_;
    }

    void SetBackEventId(const EventMarker& backEventId)
    {
        backEventId_ = backEventId;
    }

    const EventMarker& GetBackEventId() const
    {
        return backEventId_;
    }

    void SetDefaultPaddingStart(const Dimension& defaultPaddingStart)
    {
        defaultPaddingStart_ = defaultPaddingStart;
    }

    const Dimension& GetDefaultPaddingStart() const
    {
        return defaultPaddingStart_;
    }

    void SetDefaultPaddingEnd(const Dimension& defaultPaddingEnd)
    {
        defaultPaddingEnd_ = defaultPaddingEnd;
    }

    const Dimension& GetDefaultPaddingEnd() const
    {
        return defaultPaddingEnd_;
    }

    void SetProgressColor(const Color& progressColor)
    {
        progressColor_ = progressColor;
    }

    const Color& GetProgressColor() const
    {
        return progressColor_;
    }

    void SetProgressDiameter(const Dimension& progressDiameter)
    {
        progressDiameter_ = progressDiameter;
    }

    const Dimension& GetProgressDiameter() const
    {
        return progressDiameter_;
    }

    void SetArrowWidth(const Dimension& arrowWidth)
    {
        arrowWidth_ = arrowWidth;
    }

    const Dimension& GetArrowWidth() const
    {
        return arrowWidth_;
    }

    void SetArrowHeight(const Dimension& arrowHeight)
    {
        arrowHeight_ = arrowHeight;
    }

    const Dimension& GetArrowHeight() const
    {
        return arrowHeight_;
    }

    void SetArrowColor(const Color& arrowColor)
    {
        arrowColor_ = arrowColor;
    }

    const Color& GetArrowColor() const
    {
        return arrowColor_;
    }

    void SetDisabledColor(const Color& disabledColor)
    {
        disabledColor_ = disabledColor;
    }

    const Color& GetDisabledColor() const
    {
        return disabledColor_;
    }

    void SetRadius(const Dimension& radius)
    {
        radius_ = radius;
    }

    const Dimension& GetRadius() const
    {
        return radius_;
    }

    void SetButtonPressedColor(const Color& buttonPressedColor)
    {
        buttonPressedColor_ = buttonPressedColor;
    }

    const Color& GetButtonPressedColor() const
    {
        return buttonPressedColor_;
    }

    void SetButtonPressedHeight(const Dimension& buttonPressedHeight)
    {
        buttonPressedHeight_ = buttonPressedHeight;
    }

    const Dimension& GetButtonPressedHeight() const
    {
        return buttonPressedHeight_;
    }

    void SetControlHeight(const Dimension& controlHeight)
    {
        controlHeight_ = controlHeight;
    }

    const Dimension& GetControlHeight() const
    {
        return controlHeight_;
    }

    void SetControlMargin(const Dimension& controlMargin)
    {
        controlMargin_ = controlMargin;
    }

    const Dimension& GetControlMargin() const
    {
        return controlMargin_;
    }

    void SetControlPadding(const Dimension& controlPadding)
    {
        controlPadding_ = controlPadding;
    }

    const Dimension& GetControlPadding() const
    {
        return controlPadding_;
    }

    void SetFocusColor(const Color& focusColor)
    {
        focusColor_ = focusColor;
    }

    const Color& GetFocusColor() const
    {
        return focusColor_;
    }

    void SetFocusBorderWidth(const Dimension& focusBorderWidth)
    {
        focusBorderWidth_ = focusBorderWidth;
    }

    const Dimension& GetFocusBorderWidth() const
    {
        return focusBorderWidth_;
    }

    void SetMouseHoverColor(const Color& mouseHoverColor)
    {
        mouseHoverColor_ = mouseHoverColor;
    }

    const Color& GetMouseHoverColor() const
    {
        return mouseHoverColor_;
    }

    void AppendLabel(const StepperLabels& label)
    {
        stepperLabels_.emplace_back(label);
    }

    void AppendTextStyle(const TextStyle& textStyle)
    {
        labelsTextStyles_.emplace_back(textStyle);
    }

    const std::vector<TextStyle>& GetLabelStyles() const
    {
        return labelsTextStyles_;
    }

    void SetDisabledAlpha(double disabledAlpha)
    {
        disabledAlpha_ = disabledAlpha;
    }

    double GetDisabledAlpha() const
    {
        return disabledAlpha_;
    }

    ACE_DEFINE_COMPONENT_EVENT(OnFinish, void());

    ACE_DEFINE_COMPONENT_EVENT(OnSkip, void());

    ACE_DEFINE_COMPONENT_EVENT(OnChange, void(int32_t, int32_t));

    ACE_DEFINE_COMPONENT_EVENT(OnNext, void(int32_t, int32_t));

    ACE_DEFINE_COMPONENT_EVENT(OnPrevious, void(int32_t, int32_t));
private:
    std::vector<StepperLabels> stepperLabels_;
    std::vector<TextStyle> labelsTextStyles_;
    RefPtr<StepperController> stepperController_;
    int32_t index_ = 0;
    EventMarker finishEventId_;
    EventMarker skipEventId_;
    EventMarker changeEventId_;
    EventMarker nextEventId_;
    EventMarker backEventId_;
    Dimension defaultPaddingStart_;
    Dimension defaultPaddingEnd_;
    Color progressColor_;
    Dimension progressDiameter_;
    Dimension arrowWidth_;
    Dimension arrowHeight_;
    Color arrowColor_;
    Color disabledColor_;
    Dimension radius_;
    Color buttonPressedColor_;
    Dimension buttonPressedHeight_;
    Dimension controlHeight_;
    Dimension controlMargin_;
    Dimension controlPadding_;
    Color focusColor_;
    Dimension focusBorderWidth_;
    Color mouseHoverColor_;
    double disabledAlpha_ = 0.4;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_COMPONENT_H
