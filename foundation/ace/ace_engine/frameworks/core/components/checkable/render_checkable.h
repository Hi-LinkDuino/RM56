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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_CHECKABLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_CHECKABLE_H

#include "base/utils/system_properties.h"
#include "core/components/checkable/checkable_component.h"
#include "core/components/focus_animation/render_focus_animation.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

constexpr Dimension DEFAULT_CHECKABLE_BORDER_WIDTH = 1.0_vp;

enum class UIStatus {
    SELECTED = 0,
    UNSELECTED,
    FOCUS,
    ON_TO_OFF,
    OFF_TO_ON,
    PART,
    PART_TO_OFF,
    OFF_TO_PART,
    PART_TO_ON,
    ON_TO_PART,
};

class RenderCheckable : public RenderNode {
    DECLARE_ACE_TYPE(RenderCheckable, RenderNode);

public:
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    virtual void HandleClick();
    void ApplyAspectRatio(Size& drawSize) const;
    void InitClickRecognizer(bool catchMode);
    void AddAccessibilityAction();
    bool GetChecked() const
    {
        return checked_;
    }

    void SetNeedFocus(bool needFocus)
    {
        needFocus_ = needFocus;
    }

    void SetOnChange(const std::function<void(bool)>& value)
    {
        onChange_ = value;
    }

    bool Isdisable()
    {
        return disabled_;
    }

    bool IsPhone() const
    {
        return SystemProperties::GetDeviceType() == DeviceType::PHONE;
    }
    bool IsOnHover() const
    {
        return mouseState_ == MouseState::HOVER;
    }
    Color GetActiveColor() const
    {
        return activeColorInspector_;
    }
    Color GetPointColor()
    {
        return pointColorInspector_;
    }
    virtual void RequestFocusBorder(const Offset& focusOffset, const Size& focusSize, double borderRadius);

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void OnStatusChanged(RenderStatus renderStatus) override;
    void InitSize();
    void CalculateSize();
    virtual void UpdateUIStatus()
    {
        uiStatus_ = checked_ ? UIStatus::SELECTED : ((onFocus_ && needFocus_) ? UIStatus::FOCUS : UIStatus::UNSELECTED);
    }
    void SetDragPosition(double dragPosition)
    {
        dragPosition_ = dragPosition;
    }
    double GetDragPosition() const
    {
        return dragPosition_;
    }
    std::string UpdateChangedResult();
    virtual void OnHandleChangedResult(const std::string& result);

    double width_ = -1.0;
    double height_ = -1.0;
    Dimension hotZoneHorizontalPadding_ = 0.0_px;
    Dimension hotZoneVerticalPadding_ = 0.0_px;
    Dimension defaultWidth_ = 1.0_px;
    Dimension defaultHeight_ = 1.0_px;
    Dimension shadowWidth_ = 1.0_vp;
    double aspectRatio_ = 1.0;
    double dragPosition_ = 0.0;
    Dimension borderWidth_ = 0.0_px;
    Dimension borderRadius_ = 0.0_px;
    Dimension checkStroke_ = 0.0_px;
    Offset paintPosition_;
    uint32_t pointColor_ = 0;
    uint32_t activeColor_ = 0;
    Color pointColorInspector_;
    Color activeColorInspector_;
    uint32_t inactiveColor_ = 0;
    uint32_t focusColor_ = 0;
    uint32_t inactivePointColor_ = 0;
    uint32_t shadowColor_ = 0x0C000000;
    bool disabled_ = false;
    bool checked_ = false;
    bool onFocus_ = false;
    bool needFocus_ = true;
    bool backgroundSolid_ = true;
    UIStatus uiStatus_ = UIStatus::UNSELECTED;
    RefPtr<DragRecognizer> dragRecognizer_;
    RefPtr<ClickRecognizer> clickRecognizer_;
    std::function<void(const std::string&)> changeEvent_;
    std::function<void(const std::string&)> valueChangeEvent_;
    std::function<void()> clickEvent_;
    std::function<void(const std::string&)> domChangeEvent_;
    std::function<void(bool)> onChange_;
    std::function<void()> onClick_;
    Size drawSize_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_CHECKABLE_H
