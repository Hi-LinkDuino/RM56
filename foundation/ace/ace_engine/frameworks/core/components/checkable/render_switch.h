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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_SWITCH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_SWITCH_H

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/checkable/checkable_component.h"
#include "core/components/checkable/render_checkable.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {

class RenderSwitch : public RenderCheckable {
    DECLARE_ACE_TYPE(RenderSwitch, RenderCheckable);

public:
    RenderSwitch() = default;
    ~RenderSwitch() override;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    void HandleClick() override;
    void HandleDragStart(const Offset& updatePoint);
    void HandleDragUpdate(const Offset& updatePoint);
    void HandleDragEnd(const Offset& updatePoint);
    void OnDrag(const Offset& updatePoint);
    void UpdatePointPosition(const Offset& updatePoint);
    void OpenSwitch();
    void CloseSwitch();

    virtual Size CalculateTextSize(const std::string& text, RefPtr<RenderText>& renderText) = 0;
    void PaintText(const Offset& textOffset, RenderContext& context) const;

    void InitCurrentPointPosition();

protected:
    void UpdateRenderText(const RefPtr<SwitchComponent>& component);
    void InitRenderText();
    void HandleDrag();
    void UpdateAnimation();
    void OnAnimationStop();
    void UpdateAccessibilityAttr();
#ifndef WEARABLE_PRODUCT
    void PrepareMultiModalEvent();
    bool SubscribeMultiModal();
    bool UnSubscribeMultiModal();
#endif
    // switch attributes
    bool oldChecked_ = true;
    bool needReverse_ = false;
    Dimension pointPadding_ = 0.0_px;
    double pointRadius_ = 0.0;
    double pointPositionDelta_ = 0.0;
    double dragStartPosition_ = 0.0;
    double currentPointOriginX_ = 0.0;
    Size rawPointSize_;
    Size switchSize_;

    // text attributes
    bool showText_ = false;
    Dimension pointTextPadding_ = 0.0_vp;
    std::string textOn_ = "On";
    std::string textOff_ = "Off";
    TextStyle textStyle_;
    RefPtr<RenderText> renderTextOn_;
    RefPtr<RenderText> renderTextOff_;
    Size textOnSize_;
    Size textOffSize_;
    Color textColorOn_ = Color::BLACK;
    Color textColorOff_ = Color::BLACK;
    RefPtr<TextComponent> textOnComponent_;
    RefPtr<TextComponent> textOffComponent_;
#ifndef WEARABLE_PRODUCT
    bool isSubscribeMultimodal_ = false;
    VoiceEvent switchEvent_;
    WeakPtr<MultiModalScene> multiModalScene_;
    MultimodalEventCallback multimodalSwitchEvent_;
#endif
    // animation control
    RefPtr<Animator> controller_;
    RefPtr<CurveAnimation<double>> translate_;
    bool isSwitchDuringAnimation_ = false;
    RefPtr<SwitchComponent> component_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_SWITCH_H
