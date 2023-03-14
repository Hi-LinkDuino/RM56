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

#include "core/components/stepper/render_stepper.h"

#include "base/i18n/localization.h"
#include "base/utils/string_utils.h"
#include "core/animation/curve_animation.h"
#include "core/animation/keyframe.h"
#include "core/components/align/render_align.h"
#include "core/components/flex/flex_component.h"
#include "core/components/image/image_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/padding/render_padding.h"
#include "core/components/progress/loading_progress_component.h"
#include "core/components/stepper/render_stepper_item.h"
#include "core/components/stepper/stepper_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

// button params
const char STEPPER_STATUS_NORMAL[] = "normal";
const char STEPPER_STATUS_DISABLED[] = "disabled";
const char STEPPER_STATUS_WAITING[] = "waiting";
const char STEPPER_STATUS_SKIP[] = "skip";
constexpr int32_t LEAST_STEPPER_ITEM_COUNT = 2;

// for focus
constexpr Dimension STEPPER_FOCUS_DEL_OFFSET = 4.0_vp;
constexpr Dimension STEPPER_FOCUS_DEL_SIZE = 8.0_vp;
constexpr Dimension STEPPER_FOCUS_RADIUS_DEL_SIZE = 3.0_vp;

} // namespace

void RenderStepper::Update(const RefPtr<Component>& component)
{
    stepperComponent_ = AceType::DynamicCast<StepperComponent>(component);
    if (!stepperComponent_) {
        LOGW("stepper component is null");
        return;
    }
    childrenArray_.clear();
    needReverse_ = (stepperComponent_->GetTextDirection() == TextDirection::RTL);
    totalItemCount_ = static_cast<int32_t>(stepperComponent_->GetChildren().size());

    // currentIndex_ should be updated only for the first time
    if (currentIndex_ == -1) {
        int32_t index = stepperComponent_->GetIndex();
        if (index >= 0 && index < totalItemCount_) {
            currentIndex_ = index;
        } else {
            currentIndex_ = 0;
        }
    }

    const auto& stepperController = stepperComponent_->GetStepperController();
    if (stepperController) {
        auto weak = AceType::WeakClaim(this);
        stepperController->SetRightButtonStatusImpl([weak](const std::string& status, const std::string& label) {
            auto stepper = weak.Upgrade();
            if (stepper) {
                stepper->SetRightButtonStatus(status, label);
            }
        });
    }
    if (stepperComponent_->GetOnFinish()) {
        onFinish_ = *stepperComponent_->GetOnFinish();
    }
    if (stepperComponent_->GetOnSkip()) {
        onSkip_ = *stepperComponent_->GetOnSkip();
    }
    if (stepperComponent_->GetOnChange()) {
        onChange_ = *stepperComponent_->GetOnChange();
    }
    if (stepperComponent_->GetOnNext()) {
        onNext_ = *stepperComponent_->GetOnNext();
    }
    if (stepperComponent_->GetOnPrevious()) {
        onPrevious_ = *stepperComponent_->GetOnPrevious();
    }
    finishEvent_ = AceAsyncEvent<void(const std::string&)>::Create(stepperComponent_->GetFinishEventId(), context_);
    skipEvent_ = AceAsyncEvent<void(const std::string&)>::Create(stepperComponent_->GetSkipEventId(), context_);
    changeEvent_ = AceAsyncEvent<void(const std::string&)>::Create(stepperComponent_->GetChangeEventId(), context_);
    nextEvent_ = AceSyncEvent<void(const std::string&, std::string&)>::Create(
        stepperComponent_->GetNextEventId(), context_);
    backEvent_ = AceSyncEvent<void(const std::string&, std::string&)>::Create(
        stepperComponent_->GetBackEventId(), context_);
    if (!stepperAnimationController_ && totalItemCount_ >= LEAST_STEPPER_ITEM_COUNT) {
        stepperAnimationController_ = AceType::MakeRefPtr<StepperAnimationController>(GetContext());
        stepperAnimationController_->SetRenderStepper(AceType::WeakClaim(this));
    }
    Initialize();
    MarkNeedLayout();
}

void RenderStepper::Initialize()
{
    InitAttr();
    InitRecognizer();
    UpdateButtonStatus();
    leftButtonData_.isLeft = true;
    InitButton(leftButtonData_);
    rightButtonData_.isLeft = false;
    InitButton(rightButtonData_);
    InitProgress(renderProgress_);
    InitAccessibilityEventListener();
}

void RenderStepper::InitAttr()
{
    stepperLabels_ = stepperComponent_->GetStepperLabels();
    defaultPaddingStart_ = NormalizeToPx(stepperComponent_->GetDefaultPaddingStart());
    defaultPaddingEnd_ = NormalizeToPx(stepperComponent_->GetDefaultPaddingEnd());
    progressColor_ = stepperComponent_->GetProgressColor();
    progressDiameter_ = stepperComponent_->GetProgressDiameter();
    arrowWidth_ = NormalizeToPx(stepperComponent_->GetArrowWidth());
    arrowHeight_ = NormalizeToPx(stepperComponent_->GetArrowHeight());
    arrowColor_ = stepperComponent_->GetArrowColor();
    disabledAlpha_ = stepperComponent_->GetDisabledAlpha();
    disabledColor_ = stepperComponent_->GetDisabledColor().ChangeOpacity(disabledAlpha_);
    rrectRadius_ = NormalizeToPx(stepperComponent_->GetRadius());
    buttonPressedColor_ = stepperComponent_->GetButtonPressedColor();
    buttonPressedHeight_ = NormalizeToPx(stepperComponent_->GetButtonPressedHeight());
    controlPanelHeight_ = NormalizeToPx(stepperComponent_->GetControlHeight());
    controlMargin_ = NormalizeToPx(stepperComponent_->GetControlMargin());
    controlPadding_ = NormalizeToPx(stepperComponent_->GetControlPadding());
    focusColor_ = stepperComponent_->GetFocusColor();
    focusBorderWidth_ = NormalizeToPx(stepperComponent_->GetFocusBorderWidth());
    mouseHoverColor_ = stepperComponent_->GetMouseHoverColor();
    textStyles_ = stepperComponent_->GetLabelStyles();
    textColors_.clear();
    for (auto& textStyle : textStyles_) {
        textColors_.emplace_back(textStyle.GetTextColor());
    }
}

void RenderStepper::InitAccessibilityEventListener()
{
    auto refNode = accessibilityNode_.Upgrade();
    if (!refNode) {
        return;
    }
    refNode->AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
    refNode->AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
    auto weakPtr = AceType::WeakClaim(this);
    refNode->SetActionScrollForward([weakPtr]() {
        auto stepper = weakPtr.Upgrade();
        if (stepper) {
            stepper->StepperNext();
            return true;
        }
        return false;
    });
    refNode->SetActionScrollBackward([weakPtr]() {
        auto stepper = weakPtr.Upgrade();
        if (stepper) {
            stepper->StepperPrev();
            return true;
        }
        return false;
    });
}

void RenderStepper::InitRecognizer()
{
    auto wp = AceType::WeakClaim(this);
    touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    touchRecognizer_->SetOnTouchDown([wp](const TouchEventInfo& info) {
        auto client = wp.Upgrade();
        if (client) {
            client->HandleTouchDown(info);
        }
    });
    touchRecognizer_->SetOnTouchUp([wp](const TouchEventInfo& info) {
        auto client = wp.Upgrade();
        if (client) {
            client->HandleTouchUp(info);
        }
    });
    touchRecognizer_->SetOnTouchMove([wp](const TouchEventInfo& info) {
        auto client = wp.Upgrade();
        if (client) {
            client->HandleTouchMove(info);
        }
    });

    clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer_->SetOnClick([wp](const ClickInfo& info) {
        auto client = wp.Upgrade();
        if (client) {
            client->HandleClick(info);
        }
    });
}

void RenderStepper::InitProgress(RefPtr<RenderLoadingProgress>& renderProgress)
{
    if (!renderProgress) {
        auto progressComponent = AceType::MakeRefPtr<LoadingProgressComponent>();
        progressComponent->SetDiameter(progressDiameter_);
        progressComponent->SetProgressColor(progressColor_);
        renderProgress = AceType::DynamicCast<RenderLoadingProgress>(progressComponent->CreateRenderNode());
        renderProgress->Attach(GetContext());
        renderProgress->Update(progressComponent);
        AddChild(renderProgress);
    }
}

void RenderStepper::InitButton(ControlPanelData& buttonData)
{
    if (!buttonData.displayRender) {
        buttonData.displayComponent = AceType::MakeRefPtr<DisplayComponent>();
        buttonData.displayRender = AceType::DynamicCast<RenderDisplay>(buttonData.displayComponent->CreateRenderNode());
        AddChild(buttonData.displayRender);
        buttonData.displayRender->Attach(GetContext());
    }

    if (!buttonData.hotBoxRender) {
        buttonData.hotBoxComponent = AceType::MakeRefPtr<BoxComponent>();
        buttonData.hotBoxComponent->SetPadding(Edge(controlPadding_));
        LayoutParam constraints;
        constraints.SetMinSize(Size(0, buttonPressedHeight_));
        constraints.SetMaxSize(Size(Size::INFINITE_SIZE, Size::INFINITE_SIZE));
        buttonData.hotBoxComponent->SetConstraints(constraints);
        buttonData.hotBoxRender = AceType::DynamicCast<RenderBox>(buttonData.hotBoxComponent->CreateRenderNode());
        buttonData.displayRender->AddChild(buttonData.hotBoxRender);
        buttonData.hotBoxRender->Attach(GetContext());
    }

    if (!buttonData.flexRender) {
        auto row = AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START,
            FlexAlign::CENTER, std::list<RefPtr<Component>>());
        row->SetMainAxisSize(MainAxisSize::MIN);
        buttonData.flexRender = AceType::DynamicCast<RenderFlex>(row->CreateRenderNode());
        buttonData.hotBoxRender->AddChild(buttonData.flexRender);
        buttonData.flexRender->Attach(GetContext());
        InitHotArea(buttonData);
        buttonData.flexRender->Update(row);
        buttonData.hotBoxRender->Update(buttonData.hotBoxComponent);
        buttonData.displayRender->Update(buttonData.displayComponent);
    }
}

void RenderStepper::InitHotArea(ControlPanelData& buttonData)
{
    if (!buttonData.textBoxRender) {
        auto textBoxComponent = AceType::MakeRefPtr<BoxComponent>();
        buttonData.textBoxRender = AceType::DynamicCast<RenderBox>(textBoxComponent->CreateRenderNode());
        buttonData.textBoxRender->Attach(GetContext());

        buttonData.textComponent = AceType::MakeRefPtr<TextComponent>("");
        buttonData.textComponent->SetData(buttonData.text);

        buttonData.textRender = AceType::DynamicCast<RenderText>(buttonData.textComponent->CreateRenderNode());
        buttonData.textBoxRender->AddChild(buttonData.textRender);
        buttonData.textRender->Attach(GetContext());
        buttonData.textRender->Update(buttonData.textComponent);
        buttonData.textBoxRender->Update(textBoxComponent);
    }

    if (!buttonData.imageBoxRender) {
        auto imageBoxComponent = AceType::MakeRefPtr<BoxComponent>();
        imageBoxComponent->SetPadding(buttonData.isLeft ? Edge(0, 0, controlPadding_, 0, DimensionUnit::PX)
                                                        : Edge(controlPadding_, 0, 0, 0, DimensionUnit::PX));
        buttonData.imageBoxRender = AceType::DynamicCast<RenderBox>(imageBoxComponent->CreateRenderNode());
        buttonData.imageBoxRender->Attach(GetContext());

        auto imageComponent = AceType::MakeRefPtr<ImageComponent>();
        if (buttonData.isLeft) {
            buttonData.imageComponentLeft = imageComponent;
        } else {
            buttonData.imageComponentRight = imageComponent;
        }
        imageComponent->SetResourceId(buttonData.isLeft ? InternalResource::ResourceId::STEPPER_BACK_ARROW
                                                        : InternalResource::ResourceId::STEPPER_NEXT_ARROW);
        imageComponent->SetWidth(stepperComponent_->GetArrowWidth());
        imageComponent->SetHeight(stepperComponent_->GetArrowHeight());
        // this color is only effect svg image path
        imageComponent->SetImageFill(arrowColor_);

        auto renderImage = AceType::DynamicCast<RenderImage>(imageComponent->CreateRenderNode());
        if (buttonData.isLeft) {
            buttonData.imageRenderLeft = renderImage;
        } else {
            buttonData.imageRenderRight = renderImage;
        }
        buttonData.imageBoxRender->AddChild(renderImage);
        renderImage->Attach(GetContext());
        renderImage->Update(imageComponent);
        buttonData.imageBoxRender->Update(imageBoxComponent);
    }
}

void RenderStepper::UpdateButton(ControlPanelData& buttonData)
{
    // update disabled button
    if (!buttonData.isLeft) {
        if (buttonData.buttonStatus == StepperButtonStatus::DISABLED) {
            textStyles_[currentIndex_].SetTextColor(disabledColor_);
            buttonData.imageComponentRight->SetImageFill(arrowColor_.ChangeOpacity(disabledAlpha_));
        } else {
            textStyles_[currentIndex_].SetTextColor(textColors_[currentIndex_]);
            buttonData.imageComponentRight->SetImageFill(arrowColor_);
        }
        if (buttonData.buttonType == StepperButtonType::TEXT_ARROW) {
            buttonData.imageRenderRight->Update(buttonData.imageComponentRight);
        }
    } else {
        textStyles_[currentIndex_].SetTextColor(textColors_[currentIndex_]);
    }

    // update hot area
    auto decoration = AceType::MakeRefPtr<Decoration>();
    if (buttonData.isHovered) {
        decoration->SetBackgroundColor(mouseHoverColor_);
    } else {
        decoration->SetBackgroundColor(Color::TRANSPARENT);
    }
    if (buttonData.isClicked && buttonData.buttonStatus != StepperButtonStatus::DISABLED) {
        decoration->SetBackgroundColor(buttonPressedColor_);
    }
    Border border;
    border.SetBorderRadius(Radius(rrectRadius_));
    decoration->SetBorder(border);
    buttonData.hotBoxRender->SetBackDecoration(decoration);

    // update text
    Size maxSize = GetLayoutParam().GetMaxSize().IsInfinite() ? viewPort_ : GetLayoutParam().GetMaxSize();
    double maxTextWidth = 0.0;
    if (buttonData.buttonType == StepperButtonType::TEXT_ARROW) {
        maxTextWidth = maxSize.Width() / 2 - defaultPaddingStart_ - controlMargin_ - 3 * controlPadding_ - arrowWidth_;
    } else if (buttonData.buttonType == StepperButtonType::TEXT) {
        maxTextWidth = maxSize.Width() / 2 - defaultPaddingEnd_ - controlMargin_ - 2 * controlPadding_;
    }
    LayoutParam constraints;
    constraints.SetMinSize(Size(0, 0));
    constraints.SetMaxSize(Size(maxTextWidth, Size::INFINITE_SIZE));
    buttonData.textBoxRender->SetConstraints(constraints);
    buttonData.textComponent->SetData(buttonData.text);
    textStyles_[currentIndex_].SetTextAlign(buttonData.isLeft ? TextAlign::LEFT : TextAlign::RIGHT);
    buttonData.textComponent->SetTextStyle(textStyles_[currentIndex_]);
    buttonData.textRender->Update(buttonData.textComponent);
    // update flex children
    if (buttonData.isLeft) {
        buttonData.flexRender->AddChild(buttonData.imageBoxRender);
        buttonData.flexRender->AddChild(buttonData.textBoxRender);
    } else {
        if (buttonData.buttonType == StepperButtonType::TEXT_ARROW) {
            buttonData.flexRender->AddChild(buttonData.textBoxRender);
            buttonData.flexRender->AddChild(buttonData.imageBoxRender);
        } else if (buttonData.buttonType == StepperButtonType::TEXT) {
            buttonData.flexRender->AddChild(buttonData.textBoxRender);
            buttonData.flexRender->RemoveChild(buttonData.imageBoxRender);
        }
    }
}

void RenderStepper::UpdateButtonStatus()
{
    if (totalItemCount_ == 0) {
        leftButtonData_.buttonType = StepperButtonType::NONE;
        rightButtonData_.buttonType = StepperButtonType::NONE;
        return;
    }
    LoadDefaultButtonStatus();
    UpdateRightButtonStatus(stepperLabels_[currentIndex_].initialStatus,
        Localization::GetInstance()->GetEntryLetters("stepper.skip"));
}

void RenderStepper::UpdateRightButtonStatus(const std::string& status, const std::string& label)
{
    if (status == STEPPER_STATUS_NORMAL) {
        rightButtonData_.buttonStatus = StepperButtonStatus::NORMAL;
    } else if (status == STEPPER_STATUS_DISABLED) {
        rightButtonData_.buttonStatus = StepperButtonStatus::DISABLED;
    } else if (status == STEPPER_STATUS_WAITING) {
        rightButtonData_.buttonStatus = StepperButtonStatus::WAITING;
    } else if (status == STEPPER_STATUS_SKIP) {
        rightButtonData_.buttonType = StepperButtonType::TEXT;
        rightButtonData_.buttonStatus = StepperButtonStatus::SKIP;
        rightButtonData_.text = label;
    }
}

void RenderStepper::LoadDefaultButtonStatus()
{
    std::string leftLabel = stepperLabels_[currentIndex_].leftLabel;
    std::string rightLabel = stepperLabels_[currentIndex_].rightLabel;
    if (totalItemCount_ == 1) {
        leftButtonData_.buttonType = StepperButtonType::NONE;
        rightButtonData_.buttonType = StepperButtonType::TEXT;
        rightButtonData_.buttonStatus = StepperButtonStatus::NORMAL;
        rightButtonData_.text = rightLabel.empty() ?
            Localization::GetInstance()->GetEntryLetters("stepper.start") : rightLabel;
    } else if (totalItemCount_ > 1) {
        if (currentIndex_ == 0) {
            leftButtonData_.buttonType = StepperButtonType::NONE;
            rightButtonData_.buttonType = StepperButtonType::TEXT_ARROW;
            rightButtonData_.buttonStatus = StepperButtonStatus::NORMAL;
            rightButtonData_.text = rightLabel.empty() ?
                Localization::GetInstance()->GetEntryLetters("stepper.next") : rightLabel;
        } else if (currentIndex_ == totalItemCount_ - 1) {
            leftButtonData_.buttonType = StepperButtonType::TEXT_ARROW;
            leftButtonData_.buttonStatus = StepperButtonStatus::NORMAL;
            leftButtonData_.text = leftLabel.empty() ?
                Localization::GetInstance()->GetEntryLetters("stepper.back") : leftLabel;
            rightButtonData_.buttonType = StepperButtonType::TEXT;
            rightButtonData_.text = rightLabel.empty() ?
                Localization::GetInstance()->GetEntryLetters("stepper.start") : rightLabel;
            rightButtonData_.buttonStatus = StepperButtonStatus::NORMAL;
        } else {
            leftButtonData_.buttonType = StepperButtonType::TEXT_ARROW;
            leftButtonData_.buttonStatus = StepperButtonStatus::NORMAL;
            leftButtonData_.text = leftLabel.empty() ?
                Localization::GetInstance()->GetEntryLetters("stepper.back") : leftLabel;
            rightButtonData_.buttonType = StepperButtonType::TEXT_ARROW;
            rightButtonData_.buttonStatus = StepperButtonStatus::NORMAL;
            rightButtonData_.text = rightLabel.empty() ?
                Localization::GetInstance()->GetEntryLetters("stepper.next") : rightLabel;
        }
    }
}

void RenderStepper::SetRightButtonStatus(const std::string& status, const std::string& label)
{
    UpdateRightButtonStatus(status, label);
    MarkNeedLayout();
}

void RenderStepper::PerformLayout()
{
    // layout stepper item
    InitChildrenArr();
    if (!isAnimation_) {
        LayoutParam innerLayout = GetLayoutParam();
        Size minSize = GetLayoutParam().GetMinSize();
        Size maxSize = GetLayoutParam().GetMaxSize().IsInfinite() ? viewPort_ : GetLayoutParam().GetMaxSize();
        innerLayout.SetMaxSize(Size(maxSize.Width(), maxSize.Height() - controlPanelHeight_));
        double maxWidth = minSize.Width();
        double maxHeight = minSize.Height();
        int32_t childrenSize = static_cast<int32_t>(childrenArray_.size());
        for (int32_t i = 0; i < childrenSize; i++) {
            const auto& childItem = childrenArray_[i];
            childItem->Layout(innerLayout);
            maxWidth = std::max(maxWidth, childItem->GetLayoutSize().Width());
            maxHeight = std::max(maxHeight, childItem->GetLayoutSize().Height());
        }
        SetLayoutSize(maxSize);
        stepperWidth_ = maxWidth;
        prevItemOffset_ = (needReverse_ ? maxWidth : -maxWidth);
        nextItemOffset_ = (needReverse_ ? -maxWidth : maxWidth);
        Offset prevItemPosition = GetMainAxisOffset(prevItemOffset_);
        Offset nextItemPosition = GetMainAxisOffset(nextItemOffset_);

        for (int32_t i = 0; i < childrenSize; i++) {
            const auto& childItem = childrenArray_[i];
            if (i < currentIndex_) {
                childItem->SetPosition(prevItemPosition);
            } else if (i == currentIndex_) {
                childItem->SetPosition(Offset::Zero());
                auto display = AceType::DynamicCast<RenderDisplay>(childItem->GetFirstChild());
                if (!display) {
                    return;
                }
                display->UpdateOpacity(255.0);
            } else {
                childItem->SetPosition(nextItemPosition);
            }
        }
    }
    // layout buttons
    LayoutButton(leftButtonData_);
    LayoutButton(rightButtonData_);
    LayoutProgress();
}

void RenderStepper::LayoutButton(ControlPanelData& buttonData)
{
    if (buttonData.buttonType == StepperButtonType::NONE || buttonData.buttonStatus == StepperButtonStatus::WAITING) {
        buttonData.displayRender->UpdateVisibleType(VisibleType::GONE);
        return;
    } else {
        buttonData.displayRender->UpdateVisibleType(VisibleType::VISIBLE);
    }

    UpdateButton(buttonData);
    auto maxSize = GetLayoutParam().GetMaxSize().IsInfinite() ? viewPort_ : GetLayoutParam().GetMaxSize();
    double maxWidth = 0.0;
    if (buttonData.isLeft) {
        maxWidth = maxSize.Width() / 2 - defaultPaddingStart_ - controlMargin_;
    } else {
        maxWidth = maxSize.Width() / 2 - defaultPaddingEnd_ - controlMargin_;
    }
    LayoutParam layoutParam = GetLayoutParam();
    layoutParam.SetMaxSize(Size(maxWidth, maxSize.Height()));
    buttonData.displayRender->Layout(layoutParam);

    double deltaX = 0.0;
    if (buttonData.isLeft) {
        deltaX = defaultPaddingStart_ + controlMargin_;
    } else {
        deltaX = maxSize.Width() - (defaultPaddingEnd_ + controlMargin_
            + buttonData.displayRender->GetLayoutSize().Width());
    }
    auto deltaY = maxSize.Height() - controlMargin_ - buttonData.displayRender->GetLayoutSize().Height();
    buttonData.displayRender->SetPosition(Offset(deltaX, deltaY));
}

void RenderStepper::LayoutProgress()
{
    if (renderProgress_ && rightButtonData_.buttonStatus == StepperButtonStatus::WAITING) {
        renderProgress_->PerformLayout();
        Size maxSize = GetLayoutParam().GetMaxSize().IsInfinite() ? viewPort_ : GetLayoutParam().GetMaxSize();
        auto deltaX = maxSize.Width() - (
            defaultPaddingEnd_ + controlMargin_ + controlPadding_ + renderProgress_->GetLayoutSize().Width());
        auto deltaY = maxSize.Height() - (
            controlMargin_ + (buttonPressedHeight_ + renderProgress_->GetLayoutSize().Height()) / 2);
        renderProgress_->SetPosition(Offset(deltaX, deltaY));
    }
}

void RenderStepper::RegisterChangeEndListener(int32_t listenerId, const StepperChangeEndListener& listener)
{
    if (listener) {
        changeEndListeners_[listenerId] = listener;
    }
}

void RenderStepper::UnRegisterChangeEndListener(int32_t listenerId)
{
    changeEndListeners_.erase(listenerId);
}

void RenderStepper::OnStatusChanged(RenderStatus renderStatus)
{
    if (renderStatus == RenderStatus::FOCUS) {
        onFocus_ = true;
    } else if (renderStatus == RenderStatus::BLUR) {
        onFocus_ = false;
    }
}

void RenderStepper::FireFinishEvent() const
{
    if (finishEvent_) {
        std::string param = std::string("\"finish\",null");
        finishEvent_(param);
    }
    if (onFinish_) {
        onFinish_();
    }
}

void RenderStepper::FireSkipEvent() const
{
    if (skipEvent_) {
        std::string param = std::string("\"skip\",null");
        skipEvent_(param);
    }
    if (onSkip_) {
        onSkip_();
    }
}

void RenderStepper::FireChangedEvent(int32_t oldIndex, int32_t newIndex) const
{
    if (changeEvent_) {
        std::string param = std::string("\"change\",{\"prevIndex\":")
            .append(std::to_string(oldIndex))
            .append(",\"index\":")
            .append(std::to_string(newIndex))
            .append("},null");
        changeEvent_(param);
    }
    for (const auto& [first, second] : changeEndListeners_) {
        if (second) {
            second(currentIndex_);
        }
    }
    if (onChange_) {
        onChange_(oldIndex, newIndex);
    }
}

void RenderStepper::FireNextEvent(int32_t currentIndex, int32_t& pendingIndex)
{
    if (nextEvent_) {
        std::string result;
        std::string param = std::string("\"next\",{\"index\":")
            .append(std::to_string(currentIndex))
            .append(",\"pendingIndex\":")
            .append(std::to_string(pendingIndex))
            .append("},");
        nextEvent_(param, result);
        if (!result.empty() && isdigit(result[0])) {
            pendingIndex = StringUtils::StringToInt(result);
        }
    }
    if (onNext_) {
        onNext_(currentIndex, pendingIndex);
    }
}

void RenderStepper::FireBackEvent(int32_t currentIndex, int32_t& pendingIndex)
{
    if (backEvent_) {
        std::string result;
        std::string param = std::string("\"back\",{\"index\":")
            .append(std::to_string(currentIndex))
            .append(",\"pendingIndex\":")
            .append(std::to_string(pendingIndex))
            .append("},");
        backEvent_(param, result);
        if (!result.empty() && isdigit(result[0])) {
            pendingIndex = StringUtils::StringToInt(result);
        }
    }
    if (onPrevious_) {
        onPrevious_(currentIndex, pendingIndex);
    }
}

void RenderStepper::FireItemEvent(int32_t index, bool isAppear) const
{
    int32_t childrenCount = static_cast<int32_t>(childrenArray_.size());
    if (index < 0 || index >= childrenCount) {
        LOGW("index is error, index = %{public}d", index);
        return;
    }
    const auto& item = childrenArray_[index];
    auto stepperItem = AceType::DynamicCast<RenderStepperItem>(item);
    if (!stepperItem) {
        LOGW("Get Stepper Item Is Null");
        return;
    }
    if (isAppear) {
        stepperItem->FireAppearEvent();
    } else {
        stepperItem->FireDisappearEvent();
    }
}

int32_t RenderStepper::GetPrevIndex() const
{
    int32_t index = currentIndex_ - 1;
    if (index < 0) {
        index = 0;
    }
    return index;
}

int32_t RenderStepper::GetNextIndex() const
{
    int32_t index = currentIndex_ + 1;
    if (index >= totalItemCount_) {
        index = totalItemCount_ - 1;
    }
    return index;
}

void RenderStepper::StepperPrev()
{
    int32_t toIndex = GetPrevIndex();
    FireBackEvent(currentIndex_, toIndex);
    StepperTo(toIndex, false);
}

void RenderStepper::StepperNext()
{
    int32_t toIndex = GetNextIndex();
    FireNextEvent(currentIndex_, toIndex);
    StepperTo(toIndex, false);
}

void RenderStepper::StepperTo(int32_t index, bool reverse)
{
    int32_t fromIndex = currentIndex_;
    if (index >= totalItemCount_) {
        currentIndex_ = totalItemCount_ - 1;
    } else if (index < 0) {
        currentIndex_ = 0;
    } else {
        currentIndex_ = index;
    }
    if (fromIndex != currentIndex_) {
        outItemIndex_ = fromIndex;
        DoStepperToAnimation(outItemIndex_, currentIndex_, reverse);
    }
}

void RenderStepper::DoStepperToAnimation(int32_t fromIndex, int32_t toIndex, bool reverse)
{
    if (!stepperAnimationController_) {
        return;
    }
    if (onFocus_) {
        auto context = GetContext().Upgrade();
        if (context) {
            context->CancelFocusAnimation();
        }
    }

    auto weak = AceType::WeakClaim(this);
    stepperAnimationController_->SetAnimationStopCallback([weak, fromIndex, toIndex]() {
        auto stepper = weak.Upgrade();
        if (stepper) {
            stepper->FireChangedEvent(stepper->outItemIndex_, stepper->currentIndex_);
            stepper->FireItemEvent(stepper->outItemIndex_, false);
            stepper->FireItemEvent(stepper->currentIndex_, true);
            stepper->isAnimation_ = false;
        }
    });
    isAnimation_ = true;
    stepperAnimationController_->PlayStepperToAnimation(fromIndex, toIndex, reverse);
    UpdateButtonStatus();
    MarkNeedLayout();
}

void RenderStepper::UpdateItemOpacity(uint8_t opacity, int32_t index)
{
    auto child = childrenArray_[index];
    if (!child) {
        return;
    }
    auto display = AceType::DynamicCast<RenderDisplay>(child->GetFirstChild());
    if (!display) {
        return;
    }
    display->UpdateOpacity(opacity);
}

void RenderStepper::InitChildrenArr()
{
    if (!childrenArray_.empty()) {
        return;
    }
    const auto& children = GetChildren();
    for (const auto& child : children) {
        if (AceType::DynamicCast<RenderStepperItem>(child)) {
            childrenArray_.emplace_back(child);
        }
    }
}

void RenderStepper::UpdateItemPosition(double offset, int32_t index)
{
    int32_t childrenCount = static_cast<int32_t>(childrenArray_.size());
    if (index < 0 || index >= childrenCount) {
        LOGE("index is error, index = %{public}d", index);
        return;
    }
    const auto& childItem = childrenArray_[index];
    childItem->SetPosition(GetMainAxisOffset(offset));
    MarkNeedRender();
}

void RenderStepper::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (touchRecognizer_) {
        touchRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(touchRecognizer_);
    }
    if (clickRecognizer_) {
        result.emplace_back(clickRecognizer_);
    }
}

bool RenderStepper::MouseHoverTest(const Point& parentLocalPoint)
{
    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }
    bool isInRegion = InTouchRectList(parentLocalPoint, GetTouchRectList());
    if (isInRegion) {
        context->AddToHoverList(AceType::WeakClaim(this).Upgrade());
    }
    const auto localPoint = parentLocalPoint - GetPosition();
    const auto& children = GetChildren();
    for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
        auto& child = *iter;
        child->MouseHoverTest(localPoint);
    }
    if (leftButtonData_.displayRender->GetPaintRect().IsInRegion(parentLocalPoint)) {
        leftButtonData_.isHovered = true;
        leftOldHover_ = true;
    } else {
        leftButtonData_.isHovered = false;
    }

    if (rightButtonData_.displayRender->GetPaintRect().IsInRegion(parentLocalPoint)) {
        rightButtonData_.isHovered = true;
        rightOldHover_ = true;
    } else {
        rightButtonData_.isHovered = false;
    }

    if (!leftOldHover_ && !rightOldHover_) {
        return isInRegion;
    }
    leftOldHover_ = leftButtonData_.isHovered;
    rightOldHover_ = rightButtonData_.isHovered;

    MarkNeedLayout();
    context->AddToHoverList(AceType::WeakClaim(this).Upgrade());
    return isInRegion;
}

void RenderStepper::HandleClick(const ClickInfo& clickInfo)
{
    Point clickPoint = Point(clickInfo.GetGlobalLocation().GetX(), clickInfo.GetGlobalLocation().GetY());
    if (fingerId_ >= 0 && clickInfo.GetFingerId() != fingerId_) {
        return;
    }

    leftHotRect_ = leftButtonData_.displayRender->GetPaintRect() + GetGlobalOffset();
    if (leftHotRect_.IsInRegion(clickPoint)) {
        StepperPrev();
    }

    rightHotRect_ = rightButtonData_.displayRender->GetPaintRect() + GetGlobalOffset();
    if (rightHotRect_.IsInRegion(clickPoint)) {
        HandleRightButtonClick();
    }
}

void RenderStepper::HandleRightButtonClick()
{
    switch (rightButtonData_.buttonStatus) {
        case StepperButtonStatus::NORMAL:
            if (currentIndex_ == totalItemCount_ - 1) {
                FireFinishEvent();
            } else {
                StepperNext();
            }
            break;
        case StepperButtonStatus::SKIP:
            FireSkipEvent();
            break;
        case StepperButtonStatus::DISABLED:
            break;
        case StepperButtonStatus::WAITING:
            break;
        default:
            break;
    }
}

void RenderStepper::HandleTouchDown(const TouchEventInfo& info)
{
    if (info.GetTouches().empty()) {
        return;
    }
    const auto& locationInfo = info.GetTouches().front();
    Point touchPoint = Point(locationInfo.GetGlobalLocation().GetX(), locationInfo.GetGlobalLocation().GetY());
    if (fingerId_ >= 0 && locationInfo.GetFingerId() != fingerId_) {
        return;
    }

    leftHotRect_ = leftButtonData_.displayRender->GetPaintRect() + GetGlobalOffset();
    if (leftHotRect_.IsInRegion(touchPoint)) {
        fingerId_ = locationInfo.GetFingerId();
        leftButtonData_.isClicked = true;
        MarkNeedLayout();
    }

    rightHotRect_ = rightButtonData_.displayRender->GetPaintRect() + GetGlobalOffset();
    if (rightHotRect_.IsInRegion(touchPoint)) {
        fingerId_ = locationInfo.GetFingerId();
        rightButtonData_.isClicked = true;
        MarkNeedLayout();
    }
}

void RenderStepper::HandleTouchUp(const TouchEventInfo& info)
{
    int32_t fingerId = -1;
    if (!info.GetTouches().empty()) {
        fingerId = info.GetTouches().front().GetFingerId();
    } else if (!info.GetChangedTouches().empty()) {
        fingerId = info.GetChangedTouches().front().GetFingerId();
    }

    if (fingerId_ >= 0 && fingerId != fingerId_) {
        return;
    } else {
        fingerId_ = -1;
        leftButtonData_.isClicked = false;
        rightButtonData_.isClicked = false;
        MarkNeedLayout();
    }
}

void RenderStepper::HandleTouchMove(const TouchEventInfo& info)
{
    if (info.GetTouches().empty()) {
        return;
    }
    const auto& locationInfo = info.GetTouches().front();
    Point touchPoint = Point(locationInfo.GetGlobalLocation().GetX(), locationInfo.GetGlobalLocation().GetY());
    if (fingerId_ >= 0 && locationInfo.GetFingerId() != fingerId_) {
        return;
    }
    if (!leftHotRect_.IsInRegion(touchPoint)) {
        leftButtonData_.isClicked = false;
        MarkNeedLayout();
    }
    if (!rightHotRect_.IsInRegion(touchPoint)) {
        rightButtonData_.isClicked = false;
        MarkNeedLayout();
    }
}

void RenderStepper::UpdateButtonFocus(bool focus, bool isLeft)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("pipeline is null.");
        return;
    }

    if ((isLeft && leftButtonData_.buttonStatus == StepperButtonStatus::DISABLED) ||
        (!isLeft && rightButtonData_.buttonStatus == StepperButtonStatus::DISABLED) ||
        (!isLeft && rightButtonData_.buttonStatus == StepperButtonStatus::WAITING)) {
        context->CancelFocusAnimation();
        return;
    }

    Offset offset;
    Size layoutSize;
    Offset globalOffset;
    if (isLeft) {
        offset = leftButtonData_.displayRender->GetPosition();
        layoutSize = leftButtonData_.displayRender->GetLayoutSize();
        globalOffset = leftButtonData_.displayRender->GetGlobalOffset();
    } else {
        offset = rightButtonData_.displayRender->GetPosition();
        layoutSize = rightButtonData_.displayRender->GetLayoutSize();
        globalOffset = rightButtonData_.displayRender->GetGlobalOffset();
    }
    if (focus) {
        offset += Offset(NormalizeToPx(STEPPER_FOCUS_DEL_OFFSET), NormalizeToPx(STEPPER_FOCUS_DEL_OFFSET));
        layoutSize -= Size(NormalizeToPx(STEPPER_FOCUS_DEL_SIZE), NormalizeToPx(STEPPER_FOCUS_DEL_SIZE));
        globalOffset += Offset(NormalizeToPx(STEPPER_FOCUS_DEL_OFFSET), NormalizeToPx(STEPPER_FOCUS_DEL_OFFSET));
        Radius radius = Radius(rrectRadius_) - Radius(NormalizeToPx(STEPPER_FOCUS_RADIUS_DEL_SIZE));
        context->ShowFocusAnimation(RRect::MakeRRect(Rect(offset, layoutSize), radius),
            focusColor_, globalOffset);
    } else {
        context->CancelFocusAnimation();
    }
}

void RenderStepper::HandleButtonClick(bool isLeft)
{
    if (isLeft) {
        StepperPrev();
    } else {
        HandleRightButtonClick();
    }
}

} // namespace OHOS::Ace
