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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_TOGGLE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_TOGGLE_COMPONENT_H

#include "base/geometry/dimension.h"
#include "core/pipeline/base/sole_child_component.h"
#include "core/components_v2/common/common_def.h"

namespace OHOS::Ace {

class ACE_EXPORT ToggleComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(ToggleComponent, SoleChildComponent);

public:
    ToggleComponent() = default;
    ~ToggleComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    bool GetCheckedState() const
    {
        return isChecked_;
    }

    ACE_DEFINE_COMPONENT_EVENT(OnChange, void(bool));

    const Dimension& GetWidth() const
    {
        return width_;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const Color& GetPressedBlendColor() const
    {
        return pressedBlendColor_;
    }

    const Color& GetCheckedColor() const
    {
        return checkedColor_;
    }

    const EventMarker& GetClickEvent() const
    {
        return clickEvent_;
    }

    const EventMarker& GetChangeEvent() const
    {
        return changeEvent_;
    }

    bool GetFontDefinedState() const
    {
        return fontSizeDefined_;
    }

    void SetCheckedState(bool state)
    {
        isChecked_ = state;
    }

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    void SetBackgroundColor(const Color& color)
    {
        backgroundColor_ = color;
    }

    void SetCheckedColor(const Color& color)
    {
        checkedColor_ = color;
    }

    void SetPressedBlendColor(const Color& color)
    {
        pressedBlendColor_ = color;
    }

    void SetClickEvent(const EventMarker& event)
    {
        clickEvent_ = event;
    }

    void SetChangeEvent(const EventMarker& event)
    {
        changeEvent_ = event;
    }

    void SetFontDefinedState(bool state)
    {
        fontSizeDefined_ = state;
    }

private:
    bool isChecked_ = false;
    bool fontSizeDefined_ = false;
    Color backgroundColor_;
    Color checkedColor_;
    Color pressedBlendColor_;
    EventMarker clickEvent_;
    EventMarker changeEvent_;
    Dimension width_;
    Dimension height_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_TOGGLE_COMPONENT_H
