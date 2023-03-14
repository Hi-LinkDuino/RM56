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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_RENDER_STEPPER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_RENDER_STEPPER_H

#include <functional>
#include <map>
#include <vector>

#include "base/memory/ace_type.h"
#include "core/animation/animation.h"
#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/display/render_display.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/render_flex.h"
#include "core/components/image/image_component.h"
#include "core/components/image/render_image.h"
#include "core/components/progress/render_loading_progress.h"
#include "core/components/stepper/stepper_animation_controller.h"
#include "core/components/stepper/stepper_component.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

enum class StepperButtonType {
    TEXT_ARROW,
    TEXT,
    NONE,
};

enum class StepperButtonStatus {
    NORMAL,
    DISABLED,
    WAITING,
    SKIP,
};

struct ControlPanelData {
    bool isLeft = true;
    bool isClicked = false;
    bool isHovered = false;
    std::string text;
    RefPtr<RenderDisplay> displayRender;
    RefPtr<RenderFlex> flexRender;
    RefPtr<RenderBox> hotBoxRender;
    RefPtr<RenderBox> imageBoxRender;
    RefPtr<RenderBox> textBoxRender;
    RefPtr<RenderText> textRender;
    RefPtr<TextComponent> textComponent;
    RefPtr<ImageComponent> imageComponentLeft;
    RefPtr<ImageComponent> imageComponentRight;
    RefPtr<RenderImage> imageRenderLeft;
    RefPtr<RenderImage> imageRenderRight;
    RefPtr<DisplayComponent> displayComponent;
    RefPtr<BoxComponent> hotBoxComponent;
    StepperButtonType buttonType = StepperButtonType::NONE;
    StepperButtonStatus buttonStatus = StepperButtonStatus::NORMAL;
};

using StepperChangeEndListener = std::function<void(const int32_t)>;
using OnEventFunc = std::function<void(const std::string&)>;
using OnReturnEventFunc = std::function<void(const std::string&, std::string&)>;

class RenderStepper : public RenderNode {
    DECLARE_ACE_TYPE(RenderStepper, RenderNode)

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    int32_t GetCurrentIndex() const
    {
        return currentIndex_;
    }
    void RegisterChangeEndListener(int32_t listenerId, const StepperChangeEndListener& listener);
    void UnRegisterChangeEndListener(int32_t listenerId);
    void OnStatusChanged(RenderStatus renderStatus) override;
    void UpdateButtonFocus(bool focus, bool isLeft);
    void HandleButtonClick(bool isLeft);

    // for animation
    void UpdateItemOpacity(uint8_t opacity, int32_t index);
    void UpdateItemPosition(double offset, int32_t index);
    int32_t GetFromIndex() const
    {
        return outItemIndex_;
    }
    int32_t GetToIndex() const
    {
        return currentIndex_;
    }
    double GetStepperWidth() const
    {
        return stepperWidth_;
    }

    RefPtr<StepperComponent> GetStepperComponent() const
    {
        return stepperComponent_;
    }
    
protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    bool MouseHoverTest(const Point& parentLocalPoint) override;
    void HandleClick(const ClickInfo& clickInfo);
    void HandleTouchDown(const TouchEventInfo& info);
    void HandleTouchUp(const TouchEventInfo& info);
    void HandleTouchMove(const TouchEventInfo& info);

    RefPtr<StepperAnimationController> stepperAnimationController_;
    double stepperWidth_ = 0.0;
    bool leftOldHover_ = false;
    bool rightOldHover_ = false;
    RefPtr<StepperComponent> stepperComponent_;
    std::vector<RefPtr<RenderNode>> childrenArray_;
    int32_t totalItemCount_ = 0;
    int32_t currentIndex_ = -1;
    int32_t outItemIndex_ = 0;
    bool needReverse_ = false;
    bool onFocus_ = false;
    std::vector<StepperLabels> stepperLabels_;
    std::vector<TextStyle> textStyles_;
    std::vector<Color> textColors_;
    RefPtr<RenderLoadingProgress> renderProgress_;
    Rect leftHotRect_;
    Rect rightHotRect_;
    ControlPanelData leftButtonData_;
    ControlPanelData rightButtonData_;
    int32_t fingerId_ = -1;

private:
    void Initialize();
    void InitAttr();
    void InitRecognizer();
    void InitProgress(RefPtr<RenderLoadingProgress>& renderProgress);
    void InitButton(ControlPanelData& buttonData);
    void InitHotArea(ControlPanelData& buttonData);
    void UpdateButton(ControlPanelData& buttonData);
    void UpdateButtonStatus();
    void UpdateRightButtonStatus(const std::string& status, const std::string& label);
    void LoadDefaultButtonStatus();
    void LayoutButton(ControlPanelData& buttonData);
    void LayoutProgress();
    void SetRightButtonStatus(const std::string& status, const std::string& label);
    void HandleRightButtonClick();
    void FireFinishEvent() const;
    void FireSkipEvent() const;
    void FireChangedEvent(int32_t oldIndex, int32_t newIndex) const;
    void FireNextEvent(int32_t currentIndex, int32_t& pendingIndex);
    void FireBackEvent(int32_t currentIndex, int32_t& pendingIndex);
    void FireItemEvent(int32_t index, bool isAppear) const;
    int32_t GetPrevIndex() const;
    int32_t GetNextIndex() const;
    void StepperPrev();
    void StepperNext();
    void StepperTo(int32_t index, bool reverse);
    void InitStepperToAnimation(int32_t fromIndex, int32_t toIndex, bool reverse);
    void AddStepperToTranslateListener(int32_t fromIndex, int32_t toIndex);
    void AddStepperToOpacityListener(int32_t fromIndex, int32_t toIndex);
    void AddStepperToStopListener(int32_t fromIndex, int32_t toIndex);
    void DoStepperToAnimation(int32_t fromIndex, int32_t toIndex, bool reverse);
    void InitAccessibilityEventListener();
    void InitChildrenArr();
    Offset GetMainAxisOffset(double offset) const
    {
        return Offset(offset, 0);
    }

    bool isAnimation_ = false;
    double prevItemOffset_ = 0.0;
    double nextItemOffset_ = 0.0;
    std::map<int32_t, std::function<void(const int32_t&)>> changeEndListeners_;
    OnEventFunc finishEvent_;
    OnEventFunc skipEvent_;
    OnEventFunc changeEvent_;
    OnReturnEventFunc nextEvent_;
    OnReturnEventFunc backEvent_;
    RefPtr<RawRecognizer> touchRecognizer_;
    RefPtr<ClickRecognizer> clickRecognizer_;

    std::function<void()> onFinish_;
    std::function<void()> onSkip_;
    std::function<void(int32_t, int32_t)> onChange_;
    std::function<void(int32_t, int32_t)> onNext_;
    std::function<void(int32_t, int32_t)> onPrevious_;
    // theme style
    double defaultPaddingStart_ = 0.0;
    double defaultPaddingEnd_ = 0.0;
    Color progressColor_;
    Dimension progressDiameter_;
    double arrowWidth_ = 0.0;
    double arrowHeight_ = 0.0;
    Color arrowColor_;
    Color disabledColor_;
    double rrectRadius_ = 0.0;
    Color buttonPressedColor_;
    double buttonPressedHeight_ = 0.0;
    double controlPanelHeight_ = 0.0;
    double controlMargin_ = 0.0;
    double controlPadding_ = 0.0;
    Color focusColor_;
    double focusBorderWidth_ = 0.0;
    Color mouseHoverColor_;
    double disabledAlpha_ = 0.4;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_RENDER_STEPPER_H
