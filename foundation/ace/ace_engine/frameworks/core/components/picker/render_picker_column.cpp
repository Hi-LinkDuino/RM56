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

#include "core/components/picker/render_picker_column.h"

#include "base/log/event_report.h"

namespace OHOS::Ace {
namespace {

constexpr double PICKER_ROTATION_SENSITIVITY_NORMAL = 0.6;
const std::string& VIBRATOR_TYPE_WATCH_CROWN_STRENGTH2 = "watchhaptic.crown.strength2";

}

void RenderPickerColumn::Update(const RefPtr<Component>& component)
{
    auto column = AceType::DynamicCast<PickerColumnComponent>(component);
    if (!column) {
        LOGE("input component is incorrect type or null.");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }

    auto theme = column->GetTheme();
    if (!theme) {
        LOGE("theme of component is null.");
        EventReport::SendComponentException(ComponentExcepType::GET_THEME_ERR);
        return;
    }

    auto toss = column->GetToss();
    if (!toss) {
        return;
    }

    data_ = column;
    toss->SetColumn(AceType::WeakClaim(this));
    toss->SetPipelineContext(GetContext());
    jumpInterval_ = theme->GetJumpInterval();
    columnMargin_ = theme->GetColumnIntervalMargin();
    rotateInterval_ = theme->GetRotateInterval();

    needVibrate_ = column->GetNeedVibrate();
    auto context = context_.Upgrade();
    if (needVibrate_ && !vibrator_ && context) {
        vibrator_ = VibratorProxy::GetInstance().GetVibrator(context->GetTaskExecutor());
    }

    MarkNeedLayout();
}

bool RenderPickerColumn::HandleFinished(bool success)
{
    if (!data_) {
        LOGE("data component is null.");
        return false;
    }

    if (!data_->GetInDialog()) {
        return false;
    }

    data_->HandleFinishCallback(success);
    return true;
}

void RenderPickerColumn::HandleFocus(bool focus)
{
    focused_ = focus;
    rotateAngle_ = 0.0;
    for (const auto& option : options_) {
        if (!option->GetSelected()) {
            continue;
        }
        option->UpdateFocus(focus);
        break;
    }
}

std::string RenderPickerColumn::GetColumnTag() const
{
    if (!data_) {
        LOGE("data component is null.");
        return "";
    }

    return data_->GetColumnTag();
}

uint32_t RenderPickerColumn::GetWidthRatio() const
{
    if (!data_) {
        LOGE("data component is null.");
        return 1; // default width ratio is 1:1
    }

    return data_->GetWidthRatio();
}

void RenderPickerColumn::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!rawRecognizer_) {
        rawRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
        auto weak = AceType::WeakClaim(this);
        rawRecognizer_->SetOnTouchDown([weak](const TouchEventInfo& info) {
            auto refPtr = weak.Upgrade();
            if (refPtr) {
                refPtr->HandleDragStart(info);
            }
        });
        rawRecognizer_->SetOnTouchMove([weak](const TouchEventInfo& info) {
            auto refPtr = weak.Upgrade();
            if (refPtr) {
                refPtr->HandleDragMove(info);
            }
        });
        rawRecognizer_->SetOnTouchUp([weak](const TouchEventInfo& info) {
            auto refPtr = weak.Upgrade();
            if (refPtr) {
                refPtr->HandleDragEnd(info);
            }
        });
        rawRecognizer_->SetOnTouchCancel([weak](const TouchEventInfo& info) {
            auto refPtr = weak.Upgrade();
            if (refPtr) {
                refPtr->HandleDragEnd(info);
            }
        });
    }

    if (!dragRecognizer_) {
        dragRecognizer_ = AceType::MakeRefPtr<VerticalDragRecognizer>();
    }

    rawRecognizer_->SetCoordinateOffset(coordinateOffset);
    dragRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(rawRecognizer_);
    result.emplace_back(dragRecognizer_);
}

bool RenderPickerColumn::InnerHandleScroll(bool isDown)
{
    if (!data_ || !data_->GetOptionCount()) {
        LOGE("options is empty.");
        return false;
    }

    if (needVibrate_ && vibrator_) {
        vibrator_->Vibrate(VIBRATOR_TYPE_WATCH_CROWN_STRENGTH2);
    }

    uint32_t totalOptionCount = data_->GetOptionCount();
    uint32_t currentIndex = data_->GetCurrentIndex();
    if (isDown) {
        currentIndex = (totalOptionCount + currentIndex + 1) % totalOptionCount; // index add one
    } else {
        currentIndex = (totalOptionCount + currentIndex - 1) % totalOptionCount; // index reduce one
    }
    data_->SetCurrentIndex(currentIndex);
    FlushCurrentOptions();
    data_->HandleChangeCallback(isDown, true);
    return true;
}

bool RenderPickerColumn::RotationTest(const RotationEvent& event)
{
    if (!focused_) {
        return false;
    }

    rotateAngle_ += event.value * PICKER_ROTATION_SENSITIVITY_NORMAL;
    if (rotateAngle_ > rotateInterval_) {
        HandleScroll(false);
        rotateAngle_ = 0.0;
        return true;
    }

    if (rotateAngle_ < 0 - rotateInterval_) {
        HandleScroll(true);
        rotateAngle_ = 0.0;
        return true;
    }

    return true;
}

void RenderPickerColumn::PerformLayout()
{
    double value = NormalizeToPx(jumpInterval_);
    jumpInterval_ = Dimension(value, DimensionUnit::PX);
    value = NormalizeToPx(columnMargin_);
    columnMargin_ = Dimension(value, DimensionUnit::PX);
    if (!clip_ || !column_) {
        LOGE("can not get render of child column or clip.");
        return;
    }

    double x = columnMargin_.Value() / 2.0; // left and right margin is half of it
    double y = 0.0;
    if (GetLayoutParam().GetMinSize() == GetLayoutParam().GetMaxSize()) {
        auto size = GetLayoutParam().GetMaxSize();
        SetLayoutSize(size);
        column_->Layout(LayoutParam());
        y = (column_->GetLayoutSize().Height() - size.Height()) / 2.0; // center position
        if (LessNotEqual(y, 0)) {
            y = 0.0;
        }
        clip_->SetHeight(size.Height());
        size.SetWidth(size.Width() - columnMargin_.Value());
        size.SetHeight(size.Height() + y);
        clip_->SetOffsetY(y);
        clip_->SetPosition(Offset(x, 0.0 - y));
        LayoutParam layout;
        layout.SetFixedSize(size);
        clip_->Layout(layout);
    } else {
        double fixHeight = 0.0;
        clip_->SetPosition(Offset(x, y));
        if (!NearZero(fixHeight)) {
            auto layout = GetLayoutParam();
            auto max = layout.GetMaxSize();
            auto min = layout.GetMinSize();
            max.SetHeight(fixHeight);
            min.SetHeight(fixHeight);
            layout.SetMaxSize(max);
            layout.SetMinSize(min);
            clip_->Layout(layout);
        } else if (NearZero(adjustHeight_)) {
            clip_->Layout(LayoutParam());
        } else {
            auto layout = GetLayoutParam();
            auto maxSize = layout.GetMaxSize();
            maxSize.SetHeight(maxSize.Height() - adjustHeight_);
            layout.SetMaxSize(maxSize);
            clip_->Layout(layout);
        }
        auto size = clip_->GetLayoutSize();
        size.SetWidth(size.Width() + columnMargin_.Value());
        SetLayoutSize(size);
    }

    LayoutSplitter();
    CreateAnimation();
}

void RenderPickerColumn::LayoutSplitter()
{
    if (!splitter_) {
        return;
    }

    auto size = GetLayoutSize();
    splitter_->Layout(LayoutParam());
    double x = size.Width() - splitter_->GetLayoutSize().Width() / 2.0;     // place center of right.
    double y = (size.Height() - splitter_->GetLayoutSize().Height()) / 2.0; // place center
    splitter_->SetPosition(Offset(x, y));
}

void RenderPickerColumn::UpdateAccessibility()
{
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        LOGE("pipeline is null.");
        return;
    }
    auto manager = pipeline->GetAccessibilityManager();
    if (!manager || data_->GetNodeId() < 0) {
        LOGE("accessibility manager is null or column node id is invalidate.");
        return;
    }
    auto node = manager->GetAccessibilityNodeById(data_->GetNodeId());
    if (!node) {
        LOGE("can not get accessibility node by id");
        return;
    }

    node->SetText(data_->GetPrefix() + data_->GetCurrentText() + data_->GetSuffix());
    node->SetFocusableState(true);
    node->SetFocusedState(focused_);
    node->SetScrollableState(true);
    auto viewScale = pipeline->GetViewScale();
    auto leftTop = GetGlobalOffset();
    node->SetLeft(leftTop.GetX() * viewScale);
    node->SetTop(leftTop.GetY() * viewScale);
    auto size = GetLayoutSize();
    node->SetWidth(size.Width() * viewScale);
    node->SetHeight(size.Height() * viewScale);
    if (!nodeHandlerSetted_) {
        nodeHandlerSetted_ = true;
        node->AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
        node->AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
        node->AddSupportAction(AceAction::ACTION_FOCUS);
        node->SetActionScrollForward([weak = WeakClaim(this)]() {
            auto pickerColumn = weak.Upgrade();
            if (pickerColumn) {
                pickerColumn->HandleScroll(true);
            }
            return true;
        });
        node->SetActionScrollBackward([weak = WeakClaim(this)]() {
            auto pickerColumn = weak.Upgrade();
            if (pickerColumn) {
                pickerColumn->HandleScroll(false);
            }
            return true;
        });
        node->SetActionFocusImpl([weak = WeakClaim(this)]() {
            auto pickerColumn = weak.Upgrade();
            if (pickerColumn) {
                pickerColumn->HandleFocus(true);
            }
        });
    }
}

void RenderPickerColumn::HandleDragStart(const TouchEventInfo& info)
{
    bool isPhone = SystemProperties::GetDeviceType() == DeviceType::PHONE;
    if (data_ && !isPhone) {
        data_->HandleRequestFocus();
    }

    if (info.GetTouches().empty()) {
        LOGE("touches is empty.");
        return;
    }

    if (!data_ || !data_->GetToss()) {
        return;
    }

    auto toss = data_->GetToss();
    yOffset_ = info.GetTouches().back().GetGlobalLocation().GetY();
    toss->SetStart(yOffset_);
    yLast_ = yOffset_;
    pressed_ = true;
}

void RenderPickerColumn::HandleDragMove(const TouchEventInfo& info)
{
    if (info.GetTouches().empty()) {
        LOGE("touches is empty.");
        return;
    }

    if (!pressed_) {
        LOGE("not pressed.");
        return;
    }

    if (!data_ || !data_->GetToss()) {
        return;
    }

    auto toss = data_->GetToss();
    double y = info.GetTouches().back().GetGlobalLocation().GetY();
    if (NearEqual(y, yLast_, 1.0)) { // if changing less than 1.0, no need to handle
        return;
    }
    toss->SetEnd(y);
    UpdatePositionY(y);
}

void RenderPickerColumn::UpdatePositionY(double y)
{
    yLast_ = y;
    double dragDelta = yLast_ - yOffset_;
    if (!CanMove(LessNotEqual(dragDelta, 0))) {
        return;
    }
    // the abs of drag delta is less than jump interval.
    if (LessNotEqual(0.0 - jumpInterval_.Value(), dragDelta) && LessNotEqual(dragDelta, jumpInterval_.Value())) {
        ScrollOption(dragDelta);
        return;
    }

    InnerHandleScroll(LessNotEqual(dragDelta, 0.0));
    double jumpDelta = (LessNotEqual(dragDelta, 0.0) ? jumpInterval_.Value() : 0.0 - jumpInterval_.Value());
    ScrollOption(jumpDelta);
    yOffset_ = y - jumpDelta;
}

void RenderPickerColumn::UpdateToss(double y)
{
    UpdatePositionY(y);
}

void RenderPickerColumn::TossStoped()
{
    yOffset_ = 0.0;
    yLast_ = 0.0;
    ScrollOption(0.0);
}

void RenderPickerColumn::HandleDragEnd(const TouchEventInfo& info)
{
    pressed_ = false;

    if (!data_ || !data_->GetToss()) {
        return;
    }

    auto toss = data_->GetToss();
    if (!NotLoopOptions() && toss->Play()) {
        return;
    }

    yOffset_ = 0.0;
    yLast_ = 0.0;

    if (!animationCreated_) {
        ScrollOption(0.0);
    } else {
        auto curve = CreateAnimation(scrollDelta_, 0.0);
        fromController_->ClearInterpolators();
        fromController_->AddInterpolator(curve);
        fromController_->Play();
    }
}

void RenderPickerColumn::UpdateRenders()
{
    ClearRenders();
    GetRenders();
}

void RenderPickerColumn::GetRenders(const RefPtr<RenderNode>& render)
{
    if (!render) {
        return;
    }

    if (AceType::InstanceOf<RenderText>(render)) {
        auto parent = render->GetParent().Upgrade();
        if (AceType::InstanceOf<RenderPickerColumn>(parent)) {
            splitter_ = AceType::DynamicCast<RenderText>(render);
            return;
        }
    }

    if (AceType::InstanceOf<RenderPickerOption>(render)) {
        options_.emplace_back(AceType::DynamicCast<RenderPickerOption>(render));
        return;
    }

    if (AceType::InstanceOf<RenderFlex>(render)) {
        column_ = AceType::DynamicCast<RenderFlex>(render);
    }

    if (AceType::InstanceOf<RenderClip>(render)) {
        clip_ = AceType::DynamicCast<RenderClip>(render);
    }

    if (AceType::InstanceOf<RenderDisplay>(render)) {
        display_ = AceType::DynamicCast<RenderDisplay>(render);
    }

    for (const auto& child : render->GetChildren()) {
        GetRenders(child);
    }
}

void RenderPickerColumn::GetRenders()
{
    options_.clear();
    GetRenders(AceType::Claim(this));
}

void RenderPickerColumn::ClearRenders()
{
    display_ = nullptr;
    column_ = nullptr;
    clip_ = nullptr;
    splitter_ = nullptr;
    options_.clear();
}

void RenderPickerColumn::FlushCurrentOptions()
{
    if (!data_) {
        LOGE("data is null.");
        return;
    }

    uint32_t totalOptionCount = data_->GetOptionCount();
    uint32_t currentIndex = data_->GetCurrentIndex();
    currentIndex = currentIndex % totalOptionCount;
    uint32_t showOptionCount = options_.size();
    uint32_t selectedIndex = showOptionCount / 2; // the center option is selected.
    for (uint32_t index = 0; index < showOptionCount; index++) {
        uint32_t optionIndex = (totalOptionCount + currentIndex + index - selectedIndex) % totalOptionCount;
        int diffIndex = static_cast<int>(index) - static_cast<int>(selectedIndex);
        int virtualIndex = static_cast<int>(currentIndex) + diffIndex;
        bool virtualIndexValidate = virtualIndex >= 0 && virtualIndex < static_cast<int>(totalOptionCount);
        if (NotLoopOptions() && !virtualIndexValidate) {
            options_[index]->UpdateValue(optionIndex, "");
            continue;
        }
        std::string optionText =
            (index != selectedIndex ? data_->GetOption(optionIndex)
                                    : data_->GetPrefix() + data_->GetOption(optionIndex) + data_->GetSuffix());
        options_[index]->UpdateValue(optionIndex, optionText);
    }
}

bool RenderPickerColumn::NotLoopOptions() const
{
    if (!data_) {
        LOGE("data is null.");
        return false;
    }

    uint32_t totalOptionCount = data_->GetOptionCount();
    uint32_t showOptionCount = options_.size();
    return totalOptionCount <= showOptionCount / 2 + 1; // the critical value of loop condition.
}

bool RenderPickerColumn::CanMove(bool isDown) const
{
    if (!NotLoopOptions()) {
        return true;
    }

    if (!data_) {
        LOGE("data is null.");
        return true;
    }

    int currentIndex = static_cast<int>(data_->GetCurrentIndex());
    int totalOptionCount = static_cast<int>(data_->GetOptionCount());
    int nextVirtualIndex = isDown ? currentIndex + 1 : currentIndex - 1;
    return nextVirtualIndex >= 0 && nextVirtualIndex < totalOptionCount;
}

void RenderPickerColumn::ScrollOption(double delta)
{
    for (const auto& option : options_) {
        option->UpdateScrollDelta(delta);
    }
    scrollDelta_ = delta;
}

void RenderPickerColumn::CreateAnimation()
{
    if (animationCreated_) {
        return;
    }

    toBottomCurve_ = CreateAnimation(0.0, jumpInterval_.Value());
    toTopCurve_ = CreateAnimation(0.0, 0.0 - jumpInterval_.Value());
    toController_ = AceType::MakeRefPtr<Animator>(context_);
    toController_->SetDuration(200); // 200ms for animation that from zero to outer.
    auto weak = AceType::WeakClaim(this);
    toController_->AddStopListener([weak]() {
        auto column = weak.Upgrade();
        if (column) {
            column->HandleCurveStopped();
        } else {
            LOGE("render picker column is null.");
        }
    });
    fromBottomCurve_ = CreateAnimation(jumpInterval_.Value(), 0.0);
    fromTopCurve_ = CreateAnimation(0.0 - jumpInterval_.Value(), 0.0);
    fromController_ = AceType::MakeRefPtr<Animator>(context_);
    fromController_->SetDuration(150); // 150ms for animation that from outer to zero.
    animationCreated_ = true;
}

RefPtr<CurveAnimation<double>> RenderPickerColumn::CreateAnimation(double from, double to)
{
    auto weak = AceType::WeakClaim(this);
    auto curve = AceType::MakeRefPtr<CurveAnimation<double>>(from, to, Curves::FRICTION);
    curve->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto column = weak.Upgrade();
        if (column) {
            column->ScrollOption(value);
        } else {
            LOGE("render picker column is null.");
        }
    }));
    return curve;
}

void RenderPickerColumn::HandleCurveStopped()
{
    if (!animationCreated_) {
        LOGE("animation not created.");
        return;
    }

    if (NearZero(scrollDelta_)) {
        return;
    }

    ScrollOption(0.0 - scrollDelta_);
    InnerHandleScroll(GreatNotEqual(scrollDelta_, 0.0));

    fromController_->ClearInterpolators();
    if (LessNotEqual(scrollDelta_, 0.0)) {
        fromController_->AddInterpolator(fromTopCurve_);
    } else {
        fromController_->AddInterpolator(fromBottomCurve_);
    }
    fromController_->Play();
}

bool RenderPickerColumn::HandleScroll(bool isDown)
{
    if (!CanMove(isDown)) {
        return false;
    }

    if (!animationCreated_) {
        return InnerHandleScroll(isDown);
    }

    toController_->ClearInterpolators();
    if (isDown) {
        toController_->AddInterpolator(toTopCurve_);
    } else {
        toController_->AddInterpolator(toBottomCurve_);
    }
    toController_->Play();
    return true;
}

} // namespace OHOS::Ace
