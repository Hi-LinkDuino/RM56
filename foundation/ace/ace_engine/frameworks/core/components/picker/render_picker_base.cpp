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

#include "core/components/picker/render_picker_base.h"
#include "core/components/picker/picker_date_component.h"

#include <numeric>

#include "base/log/event_report.h"
#include "core/components/picker/picker_time_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

bool RenderPickerBase::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (triangle_) {
        Rect rect(triangle_->GetGlobalOffset(), triangle_->GetLayoutSize());
        if (rect.IsInRegion(globalPoint) && GetDisableTouchEvent()) {
            triangle_->OnTouchTestHit(Offset(), touchRestrict, result);
            return true;
        }
    }

    if (lunarText_) {
        Rect rect(lunarText_->GetGlobalOffset(), lunarText_->GetLayoutSize());
        if (rect.IsInRegion(globalPoint) && switch_ && switch_->GetParent().Upgrade()) {
            auto parent = switch_->GetParent().Upgrade();
            Point global;
            Point local;
            auto size = switch_->GetLayoutSize();
            global = global + switch_->GetGlobalOffset() + Offset(size.Width() / 2, size.Height() / 2);
            local = global - parent->GetGlobalOffset();
            return switch_->TouchTest(global, local, touchRestrict, result);
        }
    }

    return RenderNode::TouchTest(globalPoint, parentLocalPoint, touchRestrict, result);
}

void RenderPickerBase::Update(const RefPtr<Component>& component)
{
    auto picker = AceType::DynamicCast<PickerBaseComponent>(component);
    if (!picker) {
        LOGE("input component is incorrect type or null.");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }

    auto textPicker = AceType::DynamicCast<PickerTextComponent>(component);
    if (textPicker) {
        selectedIndex_ = textPicker->GetSelected();
        range_ = textPicker->GetRange();
    }

    auto time = AceType::DynamicCast<PickerTimeComponent>(component);
    if (time) {
        type_ = "DatePickerType.Time";
    } else {
        type_ = "DatePickerType.Date";
    }

    auto datePicker = AceType::DynamicCast<PickerDateComponent>(component);
    if (datePicker) {
        startDateSolar_ = datePicker->GetStartDate();
        endDateSolar_ = datePicker->GetEndDate();
    }

    columnHeight_ = picker->GetColumnHeight();
    data_ = picker;
    SetInterceptTouchEvent(!data_->GetSubsidiary());
    data_->OnColumnsCreating();
    onCancelCallback_ = AceAsyncEvent<void()>::Create(data_->GetOnCancel(), context_);
    onChangeCallback_ = AceSyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::
        Create(data_->GetOnChange(), context_);
    onColumnChangeCallback_ = AceAsyncEvent<void(const std::string&)>::Create(data_->GetOnColumnChange(), context_);
    onDialogAccept_ = AceAsyncEvent<void(const std::string&)>::Create(data_->GetDialogAcceptEvent(), context_);
    onDialogCancel_ = AceAsyncEvent<void()>::Create(data_->GetDialogCancelEvent(), context_);
    onDialogChange_ = AceAsyncEvent<void(const std::string&)>::Create(data_->GetDialogChangeEvent(), context_);

    const auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        if (picker->GetOnTextCancel()) {
            onTextCancel_ = *picker->GetOnTextCancel();
        }
        if (picker->GetOnTextAccept()) {
            onTextAccept_ = *picker->GetOnTextAccept();
        }
        if (picker->GetOnTextChange()) {
            onTextChange_ = *picker->GetOnTextChange();
        }
    }

    data_->SetFinishCallback([weak = WeakClaim(this)](bool success) {
        auto refPtr = weak.Upgrade();
        if (refPtr) {
            refPtr->HandleFinish(success);
        }
    });
    data_->SetChangeCallback([weak = WeakClaim(this)](const std::string& tag, bool add, uint32_t index, bool notify) {
        auto refPtr = weak.Upgrade();
        if (refPtr) {
            refPtr->HandleColumnChange(tag, add, index, notify);
        }
    });
    if (data_->GetIsDialog() || data_->GetIsCreateDialogComponent()) {
        SetButtonHandler();
    }
    SetTextDirection(data_->GetTextDirection());
    MarkNeedLayout();

    const auto& rotationController = picker->GetRotationController();
    if (rotationController) {
        auto weak = AceType::WeakClaim(this);
        rotationController->SetRequestRotationImpl(weak, context_);
    }
}

void RenderPickerBase::SetButtonHandler()
{
    // RenderPicker may created for same component(when showing again),
    // these event id may already bind, so remove first.
    BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(data_->GetOnCancelClickId());
    BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(data_->GetOnOkClickId());

    auto context = context_.Upgrade();
    if (context->GetIsDeclarative()) {
        BackEndEventManager<void(const ClickInfo& info)>::GetInstance().BindBackendEvent(
            data_->GetOnCancelClickId(), [weak = WeakClaim(this)](const ClickInfo& info) {
                const auto& picker = weak.Upgrade();
                if (picker) {
                    picker->HandleFinish(false);
                } else {
                    LOGE("GetOnCancelClickId picker weak.Upgrade is null");
                }
            });
        BackEndEventManager<void(const ClickInfo& info)>::GetInstance().BindBackendEvent(
            data_->GetOnOkClickId(), [weak = WeakClaim(this)](const ClickInfo& info) {
                const auto& picker = weak.Upgrade();
                if (picker) {
                    picker->HandleFinish(true);
                } else {
                    LOGE("GetOnOkClickId picker weak.Upgrade is null");
                }
            });
    } else {
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(
            data_->GetOnCancelClickId(), [weak = WeakClaim(this)]() {
                const auto& picker = weak.Upgrade();
                if (picker) {
                    picker->HandleFinish(false);
                } else {
                    LOGE("GetOnCancelClickId picker weak.Upgrade is null");
                }
            });
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(
            data_->GetOnOkClickId(), [weak = WeakClaim(this)]() {
                const auto& picker = weak.Upgrade();
                if (picker) {
                    picker->HandleFinish(true);
                } else {
                    LOGE("GetOnOkClickId picker weak.Upgrade is null");
                }
            });
    }
    auto cancelNode = data_->GetCancelAccessibility();
    if (cancelNode) {
        cancelNode->SetActionClickImpl([weakPtr = WeakClaim(this)]() {
            const auto& picker = weakPtr.Upgrade();
            if (picker) {
                picker->HandleFinish(false);
            }
        });
    }
    auto okNode = data_->GetOkAccessibility();
    if (okNode) {
        okNode->SetActionClickImpl([weakPtr = WeakClaim(this)]() {
            const auto& picker = weakPtr.Upgrade();
            if (picker) {
                picker->HandleFinish(true);
            }
        });
    }
}

void RenderPickerBase::HandleTriangleClick(bool value)
{
    if (!data_) {
        LOGE("data is null");
        return;
    }

    data_->OnTriangleCallback(value);
    MarkNeedRender();
}

void RenderPickerBase::SetTriangleHandler()
{
    if (!triangle_ || triangle_->GetOnClick()) {
        return;
    }

    triangle_->SetOnClick([weak = WeakClaim(this)](bool value) {
        auto refPtr = weak.Upgrade();
        if (!refPtr) {
            return;
        }
        refPtr->HandleTriangleClick(value);
    });
}

void RenderPickerBase::SetSwitchHandler()
{
    if (switchHandlerSetted_) {
        return;
    }

    if (!switch_) {
        LOGE("can not get switch button.");
        return;
    }

    auto switchNode = data_->GetSwitchAccessibility();
    switch_->SetOnChange([weak = WeakClaim(this)](bool value) {
        auto refPtr = weak.Upgrade();
        if (!refPtr) {
            return;
        }
        refPtr->HandleSwitch(value);
        if (!refPtr->data_) {
            return;
        }
        auto node = refPtr->data_->GetSwitchAccessibility();
        auto pipeline = refPtr->context_.Upgrade();
        if (node && pipeline && node->GetClicked()) {
            node->SetClicked(false);
            AccessibilityEvent clickEvent;
            clickEvent.nodeId = node->GetNodeId();
            clickEvent.eventType = "click";
            pipeline->SendEventToAccessibility(clickEvent);
        }
    });

    if (switchNode && switch_) {
        switchNode->SetActionClickImpl([weak = WeakClaim(this)]() {
            auto refPtr = weak.Upgrade();
            if (refPtr && refPtr->switch_) {
                refPtr->switch_->HandleClick();
            }
        });
    }
    switchHandlerSetted_ = true;
}

void RenderPickerBase::PerformLayout()
{
    SetSwitchHandler();
    SetTriangleHandler();

    if (!data_ || !outBox_) {
        LOGE("data or out box is null.");
        return;
    }

    if (data_->GetIsDialog()) {
        LayoutBoxes();
        LayoutColumns();
        SetLayoutSize(GetLayoutParam().GetMaxSize());
        if (!hasLayout_) {
            hasLayout_ = true;
        }
        return;
    }

    double lunarHeight = 16.0; // use 16.0 not 0.0 for default
    if (data_->GetHasLunar() && data_->GetTheme()) {
        auto theme = data_->GetTheme();
        lunarHeight = NormalizeToPx(theme->GetButtonTopPadding()) + NormalizeToPx(theme->GetLunarHeight());
    }
    for (const auto& column : columns_) {
        column->SetAdjustHeight(lunarHeight);
    }
    outBox_->Layout(GetLayoutParam());
    LayoutColumns();
    SetLayoutSize(outBox_->GetLayoutSize());
}

void RenderPickerBase::LayoutBoxes()
{
    if (!stack_ || !outBox_ || !box_ || !data_ || !data_->GetTheme()) {
        LOGE("inner param invalidate.");
        return;
    }

    auto theme = data_->GetTheme();
    outBox_->Layout(LayoutParam());
    Size innerSize = box_->GetLayoutSize();
    if (innerSize.Width() < NormalizeToPx(theme->GetButtonWidth()) * 2.0) { // ok width + cancel width
        innerSize.SetWidth(NormalizeToPx(theme->GetButtonWidth()) * 2.0);   // ok width + cancel width
    }
    // the min size of pupup screen interval is 12.0 vp + 12.0 vp = 24.0 vp
    if (innerSize.Width() + NormalizeToPx(24.0_vp) > stack_->GetLayoutSize().Width()) {
        innerSize.SetWidth(stack_->GetLayoutSize().Width() - NormalizeToPx(24.0_vp));
    }
    LayoutParam layout;
    layout.SetFixedSize(GetLayoutParam().GetMaxSize());
    outBox_->Layout(layout);
    double x = (stack_->GetLayoutSize().Width() - innerSize.Width()) / 2.0; // place center
    double y = 0.0;
    if (theme->GetShowButtons()) {
        y = (outBox_->GetLayoutSize().Height() - innerSize.Height() -
             NormalizeToPx(theme->GetButtonTopPadding())); // place bottom
    } else {
        y = (outBox_->GetLayoutSize().Height() - innerSize.Height()) / 2.0; // place center
    }
    box_->SetPosition(Offset(x, y));
    layout.SetFixedSize(innerSize);
    box_->Layout(layout);
    if (data_->GetSubsidiary()) {
        auto adjustHeight = NormalizeToPx(theme->GetColumnBottomTotalHeight(data_->GetMasterHasLunar()));
        y -= adjustHeight;
        auto outerSize = innerSize;
        outerSize.AddHeight(adjustHeight);
        layout.SetFixedSize(outerSize);
        outBox_->SetPosition(Offset(x, y));
        outBox_->Layout(layout);
        double boxY = NormalizeToPx(theme->GetButtonHeight() + theme->GetButtonTopPadding());
        box_->SetPosition(Offset(0.0, boxY));
        layout.SetFixedSize(innerSize);
        box_->Layout(layout);
    }
}

void RenderPickerBase::LayoutColumns()
{
    if (!columnParent_ || columns_.empty() || !data_) {
        LOGE("inner param invalidate.");
        return;
    }

    double totalWidth = columnParent_->GetLayoutSize().Width();
    double totalHeight = columnParent_->GetLayoutSize().Height();
    int32_t ratioCount = std::accumulate(columns_.begin(), columns_.end(), 0,
        [](uint32_t sum, const RefPtr<RenderPickerColumn>& column) { return sum += column->GetWidthRatio(); });

    bool divideEvenly = SystemProperties::GetDeviceType() != DeviceType::PHONE || ratioCount == 0;
    double oneWidth = divideEvenly ? totalWidth / columns_.size() : totalWidth / ratioCount;
    double left = 0;
    LayoutParam layout;
    for (uint32_t index = 0; index < columns_.size(); ++index) {
        uint32_t realIndex = 0;
        if (data_->NeedRtlColumnOrder()) {
            if (data_->GetTextDirection() == TextDirection::RTL) {
                realIndex = columns_.size() - 1 - index; // the last index is columns_.size() - 1
            } else {
                realIndex = index;
            }
        } else {
            realIndex = index;
        }
        columns_[realIndex]->SetPosition(Offset(left + columns_[realIndex]->GetXOffset(), 0.0));
        double columnWidth = divideEvenly ? oneWidth : oneWidth * columns_[realIndex]->GetWidthRatio();
        layout.SetFixedSize(Size(columnWidth, totalHeight));
        columns_[realIndex]->Layout(layout);
        left += columnWidth;
    }
}

void RenderPickerBase::UpdateAccessibility()
{
    if (!data_) {
        LOGE("inner params of render picker base has error.");
        return;
    }

    auto titleNode = data_->GetTitleAccessibility();
    if (titleNode && title_) {
        UpdateTextAccessibility(titleNode, title_);
    }

    auto lunarNode = data_->GetLunarAccessibility();
    if (lunarNode && lunarText_) {
        UpdateTextAccessibility(lunarNode, lunarText_);
    }

    auto switchNode = data_->GetSwitchAccessibility();
    if (switchNode && switch_) {
        UpdateSwitchAccessibility(switchNode, switch_);
    }

    auto cancelNode = data_->GetCancelAccessibility();
    if (cancelNode && cancel_) {
        UpdateButtonAccessibility(cancelNode, cancel_);
    }

    auto okNode = data_->GetOkAccessibility();
    if (okNode && ok_) {
        UpdateButtonAccessibility(okNode, ok_);
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    auto pickerDialogNode = data_->GetPickerDialogAccessibility();
    if (pickerDialogNode && box_) {
        UpdatePickerDialogAccessibility(pickerDialogNode, box_);
    }
#endif
}

void RenderPickerBase::UpdateSwitchAccessibility(
    const RefPtr<AccessibilityNode>& node, const RefPtr<RenderCheckbox>& renderSwitch)
{
    if (!node || !renderSwitch) {
        return;
    }
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return;
    }
    auto viewScale = pipeline->GetViewScale();
    auto leftTop = renderSwitch->GetGlobalOffset();
    node->SetLeft(leftTop.GetX() * viewScale);
    node->SetTop(leftTop.GetY() * viewScale);
    auto size = renderSwitch->GetLayoutSize();
    node->SetWidth(size.Width() * viewScale);
    node->SetHeight(size.Height() * viewScale);
    node->SetCheckedState(renderSwitch->GetChecked());
    if (lunarText_) {
        node->SetText(lunarText_->GetTextData());
    }
    node->SetEnabledState(true);
    node->SetCheckableState(true);
    node->SetClickableState(true);
    node->SetFocusableState(true);
    node->AddSupportAction(AceAction::ACTION_CLICK);
}

void RenderPickerBase::UpdatePickerDialogAccessibility(
    const RefPtr<AccessibilityNode>& node, const RefPtr<RenderBox>& box)
{
    if (!node || !box) {
        return;
    }
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return;
    }
    auto viewScale = pipeline->GetViewScale();
    auto leftTop = box->GetGlobalOffset();
    node->SetLeft(leftTop.GetX() * viewScale);
    node->SetTop(leftTop.GetY() * viewScale);
    auto size = box->GetLayoutSize();
    node->SetWidth(size.Width() * viewScale);
    node->SetHeight(size.Height() * viewScale);
}

void RenderPickerBase::UpdateTextAccessibility(const RefPtr<AccessibilityNode>& node, const RefPtr<RenderText>& text)
{
    if (!node || !text) {
        return;
    }
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return;
    }
    auto viewScale = pipeline->GetViewScale();
    auto leftTop = text->GetGlobalOffset();
    node->SetLeft(leftTop.GetX() * viewScale);
    node->SetTop(leftTop.GetY() * viewScale);
    auto size = text->GetLayoutSize();
    node->SetWidth(size.Width() * viewScale);
    node->SetHeight(size.Height() * viewScale);
    node->SetText(text->GetTextData());
}

void RenderPickerBase::UpdateButtonAccessibility(
    const RefPtr<AccessibilityNode>& node, const RefPtr<RenderButton>& button)
{
    if (!node || !button) {
        return;
    }
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return;
    }
    auto viewScale = pipeline->GetViewScale();
    auto leftTop = button->GetGlobalOffset();
    node->SetLeft(leftTop.GetX() * viewScale);
    node->SetTop(leftTop.GetY() * viewScale);
    auto size = button->GetLayoutSize();
    node->SetWidth(size.Width() * viewScale);
    node->SetHeight(size.Height() * viewScale);
    node->SetEnabledState(true);
    node->SetClickableState(true);
    node->SetFocusableState(true);
    node->AddSupportAction(AceAction::ACTION_CLICK);
    node->AddSupportAction(AceAction::ACTION_LONG_CLICK);
}

void RenderPickerBase::HandleFinish(bool success)
{
    if (!data_) {
        LOGE("data is null.");
        return;
    }

    if (data_->GetSubsidiary() && data_->GetAnimationController()) {
        auto controller = data_->GetAnimationController();
        controller->Play(false);
        return;
    }

    if (success) {
        data_->OnSelectedSaving();
    }

    if (success && onChangeCallback_) {
        auto str = data_->GetSelectedObject(false, "");
        onChangeCallback_(std::make_shared<DatePickerChangeEvent>(str));
    }

    if (!success && onCancelCallback_) {
        onCancelCallback_();
    }

    if (onTextCancel_) {
        onTextCancel_();
    }

    if (data_->GetIsCreateDialogComponent()) {
        if (success && onDialogAccept_) {
            onDialogAccept_(std::string("\"change\",") + data_->GetSelectedObject(false, "") + ",null");
        } else if (!success && onDialogCancel_) {
            onDialogCancel_();
        }
    }
    data_->HideDialog();
}

void RenderPickerBase::HandleSwitch(bool checked)
{
    if (!data_) {
        LOGE("data is null.");
        return;
    }

    std::vector<std::string> tags;
    data_->OnLunarCallback(checked, tags);
    for (const auto& tag : tags) {
        auto iter = std::find_if(columns_.begin(), columns_.end(),
            [&tag](const RefPtr<RenderPickerColumn>& column) { return column->GetColumnTag() == tag; });
        if (iter != columns_.end()) {
            (*iter)->FlushCurrentOptions();
        }
    }
}

void RenderPickerBase::HandleColumnChange(const std::string& tag, bool isAdd, uint32_t index, bool needNotify)
{
    if (!data_) {
        LOGE("data is null.");
        return;
    }

    if (onColumnChangeCallback_ && needNotify) {
        onColumnChangeCallback_(std::string("\"columnchange\",") + data_->GetSelectedObject(true, tag) + ",null");
    }

    if (onTextAccept_) {
        onTextAccept_(tag, index);
    }

    std::vector<std::string> tags;
    data_->OnDataLinking(tag, isAdd, index, tags);
    for (const auto& tag : tags) {
        auto iter = std::find_if(columns_.begin(), columns_.end(),
            [&tag](const RefPtr<RenderPickerColumn>& column) { return column->GetColumnTag() == tag; });
        if (iter != columns_.end()) {
            (*iter)->FlushCurrentOptions();
        }
    }

    if (!data_->GetIsDialog() && !data_->GetIsCreateDialogComponent()) {
        HandleFinish(true);
        RefPtr<PickerColumnComponent> pickerColumn = data_->GetColumn(tag);
        if (!pickerColumn) {
            LOGE("pickerColumn Component is null");
            return;
        }
        if (onTextChange_) {
            onTextChange_(pickerColumn->GetCurrentText(), pickerColumn->GetCurrentIndex());
        }
        return;
    }

    if (data_->GetIsCreateDialogComponent() && onDialogChange_) {
        onDialogChange_(std::string("\"columnchange\",") + data_->GetSelectedObject(true, "") + ",null");
    }

    data_->OnTitleBuilding();
    auto titleComponent = data_->GetTitle();
    if (data_->GetHasTitle()) {
        if (title_ && titleComponent) {
            title_->Update(titleComponent);
        } else {
            LOGE("render title or title component is null.");
        }
    }
}

void RenderPickerBase::UpdateRenders()
{
    ClearRenders();
    GetRenders();
    InitPickerAnimation();
    switchHandlerSetted_ = false;
}

void RenderPickerBase::GetRenders()
{
    auto render = GetParent().Upgrade();
    while (render && !AceType::InstanceOf<RenderStack>(render)) {
        render = render->GetParent().Upgrade();
    }
    if (render) {
        stack_ = AceType::DynamicCast<RenderStack>(render);
    }
    GetRenders(AceType::Claim(this));
}

void RenderPickerBase::InitPickerAnimation()
{
    if (!data_) {
        LOGE("data is null for picker animation");
        return;
    }

    auto controller = data_->GetAnimationController();
    if (!controller) {
        LOGE("controller is null for picker animation.");
        return;
    }

    controller->SetPipeline(GetContext());
    if (lunarDisplay_) {
        controller->SetLunar(lunarDisplay_);
    }
    if (buttonsDisplay_) {
        controller->SetButtons(buttonsDisplay_);
    }
    if (triangle_) {
        controller->SetTriangle(triangle_);
    }

    for (const auto& column : columns_) {
        if (!column) {
            continue;
        }

        if (column->GetColumnTag() == PickerBaseComponent::PICKER_YEAR_COLUMN) {
            controller->SetYear(column);
        }
        if (column->GetColumnTag() == PickerBaseComponent::PICKER_MONTH_COLUMN) {
            controller->SetMonth(column);
        }
        if (column->GetColumnTag() == PickerBaseComponent::PICKER_DAY_COLUMN) {
            controller->SetDay(column);
        }
        if (column->GetColumnTag() == PickerBaseComponent::PICKER_MONTHDAY_COLUMN) {
            controller->SetMonthDay(column);
        }
        if (column->GetColumnTag() == PickerBaseComponent::PICKER_AMPM_COLUMN) {
            controller->SetAmPm(column);
        }
        if (column->GetColumnTag() == PickerBaseComponent::PICKER_HOUR_COLUMN) {
            controller->SetHour(column);
        }
        if (column->GetColumnTag() == PickerBaseComponent::PICKER_MINUTE_COLUMN) {
            controller->SetMinute(column);
        }
        if (column->GetColumnTag() == PickerBaseComponent::PICKER_SECOND_COLUMN) {
            controller->SetSecond(column);
        }
    }
}

void RenderPickerBase::DealRenders(const RefPtr<RenderNode>& render)
{
    if (AceType::InstanceOf<RenderDisplay>(render)) {
        if (!buttonsDisplay_) {
            buttonsDisplay_ = AceType::DynamicCast<RenderDisplay>(render);
        } else {
            auto temp = buttonsDisplay_;
            buttonsDisplay_ = AceType::DynamicCast<RenderDisplay>(render);
            lunarDisplay_ = temp;
        }
    }

    if (AceType::InstanceOf<RenderBox>(render)) {
        auto parent = render->GetParent().Upgrade();
        if (parent && AceType::InstanceOf<RenderPickerBase>(parent)) {
            outBox_ = AceType::DynamicCast<RenderBox>(render);
        }
        if (parent && AceType::InstanceOf<RenderBox>(parent)) {
            auto grand = parent->GetParent().Upgrade();
            if (grand && AceType::InstanceOf<RenderPickerBase>(grand)) {
                box_ = AceType::DynamicCast<RenderBox>(render);
            }
        }
    }
}

void RenderPickerBase::GetRenders(const RefPtr<RenderNode>& render)
{
    if (!render) {
        return;
    }

    if (AceType::InstanceOf<RenderTriangle>(render)) {
        triangle_ = AceType::DynamicCast<RenderTriangle>(render);
        return;
    }

    if (AceType::InstanceOf<RenderCheckbox>(render)) {
        switch_ = AceType::DynamicCast<RenderCheckbox>(render);
        return;
    }

    if (AceType::InstanceOf<RenderButton>(render)) {
        auto gesture = AceType::DynamicCast<RenderButton>(render);
        if (!cancel_ && !ok_) {
            cancel_ = gesture;
            return;
        }
        ok_ = gesture;
        return;
    }

    if (AceType::InstanceOf<RenderPickerColumn>(render)) {
        auto column = AceType::DynamicCast<RenderPickerColumn>(render);
        columns_.push_back(column);
        if (!columnParent_ && column->GetParent().Upgrade()) {
            columnParent_ = column->GetParent().Upgrade();
        }
        return;
    }

    if (AceType::InstanceOf<RenderText>(render)) {
        auto parent = render->GetParent().Upgrade();
        if (parent && AceType::InstanceOf<RenderBox>(parent)) {
            title_ = AceType::DynamicCast<RenderText>(render);
            return;
        }
        if (parent && AceType::InstanceOf<RenderFlex>(parent)) {
            lunarText_ = AceType::DynamicCast<RenderText>(render);
            return;
        }
    }

    DealRenders(render);
    for (const auto& child : render->GetChildren()) {
        GetRenders(child);
    }
}

void RenderPickerBase::ClearRenders()
{
    title_ = nullptr;
    triangle_ = nullptr;
    lunarDisplay_ = nullptr;
    buttonsDisplay_ = nullptr;
    stack_ = nullptr;
    box_ = nullptr;
    outBox_ = nullptr;
    cancel_ = nullptr;
    ok_ = nullptr;
    columns_.clear();
    columnParent_ = nullptr;
    switch_ = nullptr;
    lunarText_ = nullptr;
}

} // namespace OHOS::Ace
