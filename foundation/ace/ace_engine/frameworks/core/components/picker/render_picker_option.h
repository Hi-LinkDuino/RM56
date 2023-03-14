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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_OPTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_OPTION_H

#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"
#include "core/gestures/press_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderPickerOption : public RenderNode {
    DECLARE_ACE_TYPE(RenderPickerOption, RenderNode);

public:
    RenderPickerOption();
    ~RenderPickerOption() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnPaintFinish() override;

    bool GetSelected() const;

    void UpdateValue(uint32_t newIndex, const std::string& newText);
    void UpdateFocus(bool focus);
    void UpdatePhoneFocus(bool focus);
    void UpdateTextFocus(bool focus);
    void UpdateScrollDelta(double delta);
    void UpdateRenders();

    const Color& GetFocusAnimationColor() const
    {
        return focusColor_;
    }

    Color GetFocusBackColor() const
    {
        if (!focusDecoration_) {
            return Color::WHITE;
        }
        return focusDecoration_->GetBackgroundColor();
    }

    Color GetNormalBackColor() const
    {
        if (!selectedDecoration_) {
            return Color::TRANSPARENT;
        }
        return selectedDecoration_->GetBackgroundColor();
    }

protected:
    void OnTouchTestHit(const Offset&, const TouchRestrict&, TouchTestResult& result) override;
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    void StartHoverAnimation(bool isEnter);
    void StartPressAnimation(bool isDown);
    bool ResetHoverAnimation(bool isEnter);
    bool ResetPressAnimation(bool isDown);
    void ResetMouseController();
    void UpdateBackgroundDecoration(const Color& color);

private:
    double LayoutBox();
    void GetRenders(const RefPtr<RenderNode>& render);
    void GetRenders();
    void ClearRenders();
    void RefreshFocus();

    Size optionSize_;
    DimensionUnit optionSizeUnit_ = DimensionUnit::PX;
    Size realSize_;
    double deltaSize_ = 0.0;
    double optionPadding_ = 0.0;
    double realPadding_ = 0.0;

    TextStyle selectedStyle_;
    TextStyle focusStyle_;
    Color focusColor_;
    Radius rrectRadius_;

    RefPtr<Decoration> selectedDecoration_;
    RefPtr<Decoration> focusDecoration_;
    RefPtr<Decoration> pressDecoration_;
    RefPtr<Decoration> hoverDecoration_;
    RefPtr<TextComponent> textComponent_;
    RefPtr<RenderText> renderText_;
    RefPtr<BoxComponent> boxComponent_;
    RefPtr<RenderBox> renderBox_;
    RefPtr<Animator> mouseAnimationController_;
    RefPtr<PressRecognizer> pressDetect_;

    uint32_t index_ = 0;
    std::string text_;
    bool selected_ = false;
    bool autoLayout_ = false;
    bool hasAnimate_ = false;
    bool alignTop_ = false;
    bool alignBottom_ = false;
    bool hasTextFocus_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_RENDER_PICKER_OPTION_H
