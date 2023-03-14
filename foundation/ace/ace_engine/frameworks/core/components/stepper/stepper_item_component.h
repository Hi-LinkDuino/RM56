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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_ITEM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_ITEM_COMPONENT_H

#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_node.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

struct StepperLabels {
    std::string leftLabel;
    std::string rightLabel;
    std::string initialStatus;
};

class ACE_EXPORT StepperItemComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(StepperItemComponent, SoleChildComponent);

public:
    explicit StepperItemComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}
    ~StepperItemComponent() override = default;
    static RefPtr<StepperItemComponent> GetStepperItem(const RefPtr<Component>& component);

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void SetIndex(int32_t index)
    {
        index_ = index;
    }

    int32_t GetIndex() const
    {
        return index_;
    }

    void SetFocusAnimationColor(const Color& color)
    {
        focusAnimationColor_ = color;
    }

    const Color& GetFocusAnimationColor() const
    {
        return focusAnimationColor_;
    }

    void SetAppearEventId(const EventMarker& appearEventId)
    {
        appearEventId_ = appearEventId;
    }

    const EventMarker& GetAppearEventId() const
    {
        return appearEventId_;
    }

    void SetDisappearEventId(const EventMarker& disappearEventId)
    {
        disappearEventId_ = disappearEventId;
    }

    const EventMarker& GetDisappearEventId() const
    {
        return disappearEventId_;
    }

    void SetTextStyle(const TextStyle& textStyle)
    {
        textStyle_ = textStyle;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    void SetLabel(const StepperLabels& label)
    {
        label_ = label;
    }

    const StepperLabels& GetLabel() const
    {
        return label_;
    }

private:
    int32_t index_ = -1;
    TextStyle textStyle_;
    StepperLabels label_;
    Color focusAnimationColor_ = Color::WHITE;
    EventMarker appearEventId_;
    EventMarker disappearEventId_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_ITEM_COMPONENT_H
