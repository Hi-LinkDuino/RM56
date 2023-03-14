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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OPTION_RENDER_OPTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OPTION_RENDER_OPTION_H

#include "base/utils/system_properties.h"
#include "core/components/box/render_box.h"
#include "core/components/common/properties/color.h"
#include "core/components/image/render_image.h"
#include "core/components/select/select_component.h"
#include "core/components/text/render_text.h"
#include "core/components/touch_listener/render_touch_listener.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderOption : public RenderNode {
    DECLARE_ACE_TYPE(RenderOption, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    ~RenderOption() override;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnPaintFinish() override;

    void SetFixedWidth(double width)
    {
        minWidth_ = width;
        maxWidth_ = width;
    }

    void SetMaxWidth(double width)
    {
        maxWidth_ = width;
    }

    bool OnBack();

    void OnClick(bool focus);

    void OnFocus(bool focus);

    void OnTouch(bool down);

    void SetWeakNode(const WeakPtr<FocusNode>& value)
    {
        weakNode_ = value;
    }

    bool GetSelected() const
    {
        if (!data_) {
            return false;
        }
        return data_->GetSelected();
    }

    void UpdateSelfStatus();

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;

    void UpdateAccessibilityInfo(Size size, Offset offset, bool isSelected);

    RefPtr<RenderText> GetRenderText(const RefPtr<RenderNode>& render) const;
    RefPtr<RenderImage> GetRenderImage(const RefPtr<RenderNode>& render) const;

    void InitClickEvent();
    void InitTouchEvent();

    void LayoutText(const RefPtr<RenderText>& text);
    void LayoutTextImage(const RefPtr<RenderText>& text, const RefPtr<RenderImage>& image);

    bool IsRTL() const;

    void RequestFocus();
    void GetAllOptions(std::list<RefPtr<RenderOption>>& result, const RefPtr<RenderNode>& parent) const;
    std::list<RefPtr<RenderOption>> GetAllOptions() const;
    void OnSelect(uint32_t selectIndex);
    void UpdateStatus();
    void UpdateDownStatus();
    void UpdateTvFocusedStatus();
    void UpdateClickedStatus();
    void UpdateHoveredStatus();
    void UpdateSelectedStatus();
    void UpdateOthersStatus();
    RefPtr<RenderOption> GetUpOption() const;
    RefPtr<RenderOption> GetDownOption() const;
    bool IsNormalStatus() const;
    void UpdateNormalText();
    void UpdateSelectedText();
    void UpdateFocusedText();
    void UpdateTextColor(bool selected, bool focused);
    void AdjustScrollPosition();
    void PlayEventEffectAnimation(const Color& endColor, int32_t duration, bool isHoverExists = false);

    RefPtr<ClickRecognizer> click_;
    RefPtr<RawRecognizer> touch_;
    RefPtr<OptionComponent> data_;
    WeakPtr<FocusNode> weakNode_;

    std::function<void()> onClickEvent_;

    double minWidth_ = 0.0;
    double maxWidth_ = Size::INFINITE_SIZE;
    bool focusJumped_ = false;
    bool needLine_ = true;
    Color backColor_ = Color::TRANSPARENT;
    Color clickedColor_ = Color::TRANSPARENT;
    Color hoveredColor_ = Color::TRANSPARENT;
    Color lineColor_;
    bool isTv_ = SystemProperties::GetDeviceType() == DeviceType::TV;
    RefPtr<Animator> eventEffectController_;
    bool hovered_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OPTION_RENDER_OPTION_H
