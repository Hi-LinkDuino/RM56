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

#include "core/components/picker/picker_base_component.h"

#include <chrono>
#include <ctime>

#include "base/i18n/localization.h"
#include "base/utils/string_utils.h"
#include "core/common/container.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/button/button_theme.h"
#include "core/components/dialog/dialog_theme.h"
#include "core/components/display/display_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/focusable/focusable_component.h"
#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/picker/picker_base_element.h"
#include "core/components/picker/picker_theme.h"
#include "core/components/picker/render_picker_base.h"
#include "core/components/triangle/triangle_component.h"

namespace OHOS::Ace {

const char PickerBaseComponent::PICKER_YEAR_COLUMN[] = "year";
const char PickerBaseComponent::PICKER_MONTH_COLUMN[] = "month";
const char PickerBaseComponent::PICKER_DAY_COLUMN[] = "day";
const char PickerBaseComponent::PICKER_HOUR_COLUMN[] = "hour";
const char PickerBaseComponent::PICKER_MINUTE_COLUMN[] = "minute";
const char PickerBaseComponent::PICKER_SECOND_COLUMN[] = "second";
const char PickerBaseComponent::PICKER_TEXT_COLUMN[] = "text";
const char PickerBaseComponent::PICKER_MONTHDAY_COLUMN[] = "monthDay";
const char PickerBaseComponent::PICKER_AMPM_COLUMN[] = "amPm";

PickerDate PickerDate::Current()
{
    PickerDate date;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto local = std::localtime(&now);
    if (local == nullptr) {
        LOGE("get localtime failed.");
        return date;
    }
    date.SetYear(local->tm_year + 1900); // local date start from 1900
    date.SetMonth(local->tm_mon + 1);    // local month start from 0 to 11, need add one.
    date.SetDay(local->tm_mday);
    date.SetWeek(local->tm_wday);
    return date;
}

uint32_t PickerDate::GetMaxDay(uint32_t year, uint32_t month)
{
    if (month == 2) { // days count in february is different between leap year and other.
        bool leapYear = IsLeapYear(year);
        return (leapYear ? 29 : 28); // leap year's february has 29 days, other has 28 days.
    }

    switch (month) {
        case 1:        // january
        case 3:        // march
        case 5:        // may
        case 7:        // july
        case 8:        // august
        case 10:       // october
        case 12:       // december
            return 31; // upper months has 31 days
        default:
            return 30; // other month has 30 days
    }
}

bool PickerDate::IsLeapYear(uint32_t year)
{
    if (year % 100 == 0) {        // special case: year can divided by 100
        return (year % 400 == 0); // leap year equal that can divided by 400.
    }

    return (year % 4 == 0); // other case, leap year equal that can divided by 4.
}

std::string PickerDate::ToString(bool jsonFormat, int32_t status) const
{
    if (!jsonFormat) {
        DateTime date;
        date.year = year_;
        date.month = month_ - 1; // W3C's month start from 0 to 11
        date.day = day_;
        return Localization::GetInstance()->FormatDateTime(date, DateTimeStyle::FULL, DateTimeStyle::NONE);
    }

    return std::string("{\"year\":") + std::to_string(year_) + ",\"month\":" + std::to_string(month_) +
           ",\"day\":" + std::to_string(day_) + ",\"status\":" + std::to_string(status) + "}";
}

uint32_t PickerDate::ToDays() const
{
    uint32_t days = 0;
    days += day_ - 1; // day start from 1
    // month start from 1
    for (uint32_t month = 1; month < month_; ++month) {
        days += PickerDate::GetMaxDay(year_, month);
    }
    // year start from 1900
    for (uint32_t year = 1900; year < year_; ++year) {
        // leap year has 366 days, other year has 365 days.
        days += (PickerDate::IsLeapYear(year) ? 366 : 365);
    }
    return days;
}

void PickerDate::FromDays(uint32_t days)
{
    for (year_ = 1900; year_ <= 2100; ++year_) { // year start from 1900 to 2100.
        // leap year has 366 days, other year has 365 days;
        uint32_t daysInYear = (PickerDate::IsLeapYear(year_) ? 366 : 365);
        if (days < daysInYear) {
            break;
        } else {
            days -= daysInYear;
        }
    }

    for (month_ = 1; month_ <= 12; ++month_) { // month start from 1 to 12
        uint32_t daysInMonth = PickerDate::GetMaxDay(year_, month_);
        if (days < daysInMonth) {
            break;
        } else {
            days -= daysInMonth;
        }
    }

    day_ = days + 1; // days is index start form 0 and day start form 1.
}

PickerTime PickerTime::Current()
{
    PickerTime time;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto local = std::localtime(&now);
    if (local == nullptr) {
        LOGE("get localtime failed.");
        return time;
    }
    time.SetHour(local->tm_hour);
    time.SetMinute(local->tm_min);
    time.SetSecond(local->tm_sec);
    return time;
}

std::string PickerTime::ToString(bool jsonFormat, bool hasSecond, int32_t status) const
{
    if (!jsonFormat) {
        if (!hasSecond) {
            // use char ':' to split.
            return std::to_string(hour_) + ":" + std::to_string(minute_);
        }
        // use char ':' to split.
        return std::to_string(hour_) + ":" + std::to_string(minute_) + ":" + std::to_string(second_);
    }

    if (!hasSecond) {
        // use json format chars
        return std::string("{\"hour\":") + std::to_string(hour_) + ",\"minute\":" + std::to_string(minute_) +
            ",\"status\":" + std::to_string(status) + "}";
    }
    // use json format chars
    return std::string("{\"hour\":") + std::to_string(hour_) + ",\"minute\":" + std::to_string(minute_) +
            ",\"second\":" + std::to_string(second_) + ",\"status\":" + std::to_string(status) + "}";
}

PickerDateTime PickerDateTime::Current()
{
    PickerDateTime dateTime;
    dateTime.SetDate(PickerDate::Current());
    dateTime.SetTime(PickerTime::Current());
    return dateTime;
}

std::string PickerDateTime::ToString(bool jsonFormat, int32_t status) const
{
    if (!jsonFormat) {
        return date_.ToString(jsonFormat);
    }

    return std::string("{\"year\":") + std::to_string(date_.GetYear()) +
           ",\"month\":" + std::to_string(date_.GetMonth()) +
           ",\"day\":" + std::to_string(date_.GetDay()) +
           ",\"hour\":" + std::to_string(time_.GetHour()) +
           ",\"minute\":" + std::to_string(time_.GetMinute()) +
           ",\"status\":" + std::to_string(status) + "}";
}

RefPtr<RenderNode> PickerBaseComponent::CreateRenderNode()
{
    return RenderPickerBase::Create();
}

RefPtr<Element> PickerBaseComponent::CreateElement()
{
    auto element = AceType::MakeRefPtr<PickerBaseElement>();
    element->SetAutoAccessibility(false);
    return element;
}

void PickerBaseComponent::ClearColumns()
{
    auto accessibilityManager = accessibilityManager_.Upgrade();
    if (accessibilityManager) {
        for (const auto& column : columns_) {
            accessibilityManager->RemoveAccessibilityNodeById(column->GetNodeId());
            column->SetNodeId(-1); // reset to invalidate id.
        }
    } else {
        LOGE("accessibility manager is null.");
    }
    columns_.clear();
}

void PickerBaseComponent::AppendColumn(const RefPtr<PickerColumnComponent>& column)
{
    columns_.emplace_back(column);
}

void PickerBaseComponent::SetNeedVibrate(bool needVibrate)
{
    if (needVibrate_ == needVibrate) {
        return;
    }
    needVibrate_ = needVibrate;
    for (auto& column : columns_) {
        column->SetNeedVibrate(needVibrate_);
    }
}

RefPtr<PickerColumnComponent> PickerBaseComponent::GetColumn(const std::string& tag) const
{
    auto iter = std::find_if(columns_.begin(), columns_.end(),
        [&tag](const RefPtr<PickerColumnComponent>& column) { return column->GetColumnTag() == tag; });
    return (iter == columns_.end()) ? nullptr : *iter;
}

void PickerBaseComponent::RemoveColumn(const std::string& tag)
{
    auto column = GetColumn(tag);
    auto accessibilityManager = accessibilityManager_.Upgrade();
    if (accessibilityManager && column) {
        accessibilityManager->RemoveAccessibilityNodeById(column->GetNodeId());
        column->SetNodeId(-1); // reset to invalidate id.
    } else {
        LOGE("accessibility manager is null or can not get the column.");
    }
    columns_.erase(std::remove_if(
        columns_.begin(),
        columns_.end(),
        [tag](const RefPtr<PickerColumnComponent>& col) { return col->GetColumnTag() == tag; }),
        columns_.end());
}

void PickerBaseComponent::SetFinishCallback(const ColumnFinishCallback& value)
{
    for (const auto& column : columns_) {
        column->SetFinishCallback(value);
    }
}

void PickerBaseComponent::SetChangeCallback(const ColumnChangeCallback& value)
{
    for (const auto& column : columns_) {
        column->SetChangeCallback(value);
    }
}

void PickerBaseComponent::ShowDialog(const RefPtr<StackElement>& stack, bool disableTouchEvent)
{
    if (!isDialog_) {
        return;
    }

    if (dialogShowed_) {
        LOGW("dialog is showed already.");
        return;
    }

    if (!stack) {
        LOGE("stack is null.");
        return;
    }

    stack_ = stack;
    if (!stack_->PushDialog(AceType::Claim(this), disableTouchEvent)) {
        LOGE("push dialog failed!");
        return;
    }
    dialogShowed_ = true;
}

bool PickerBaseComponent::HideDialog()
{
    CloseDialog();
    if (!isDialog_) {
        return false;
    }

    if (!dialogShowed_) {
        LOGW("dialog not show.");
        return false;
    }

    if (!stack_) {
        LOGE("stack is null.");
        return false;
    }

    if (!stack_->PopDialog()) {
        LOGE("pop dialog failed!");
        return false;
    }
    dialogShowed_ = false;
    ClearAccessibilityNodes();
    return true;
}

void PickerBaseComponent::OpenDialog(DialogProperties& properties)
{
    if (!isCreateDialogComponent_) {
        return;
    }

    auto container = Container::Current();
    if (!container) {
        return;
    }
    auto context = container->GetPipelineContext();
    if (!context) {
        return;
    }

    auto executor = context->GetTaskExecutor();
    if (!executor) {
        return;
    }

    executor->PostTask(
        [context, dialogProperties = properties, weak = WeakClaim(this)]() mutable {
            const auto& picker = weak.Upgrade();
            if (context && picker) {
                picker->dialogComponent_ = context->ShowDialog(dialogProperties, false, picker->GetDialogName());
            }
        },
        TaskExecutor::TaskType::UI);
}

void PickerBaseComponent::CloseDialog()
{
    if (!isCreateDialogComponent_) {
        return;
    }

    auto container = Container::Current();
    if (!container) {
        return;
    }
    auto context = container->GetPipelineContext();
    if (!context) {
        return;
    }
    const auto& lastStack = context->GetLastStack();
    if (!lastStack) {
        return;
    }
    auto executor = context->GetTaskExecutor();
    if (!executor) {
        return;
    }
    executor->PostTask(
        [lastStack, dialogComponent = dialogComponent_]() {
            if (!lastStack || !dialogComponent) {
                return;
            }
            auto dialogId = dialogComponent->GetDialogId();
            lastStack->PopDialog(dialogId);
        },
        TaskExecutor::TaskType::UI);
}

void PickerBaseComponent::OnTitleBuilding()
{
    SetHasLunar(false);

    auto theme = GetTheme();
    if (!theme) {
        LOGE("theme is null.");
        return;
    }
    SetHasTitle(theme->GetShowButtons());
    SetHasButtons(theme->GetShowButtons());
}

void PickerBaseComponent::InitializeTitle(std::list<RefPtr<Component>>& outChildren)
{
    if (!theme_) {
        LOGE("theme is null.");
        return;
    }
    if (isDialog_ && hasTitle_) {
        auto triangle = AceType::MakeRefPtr<TriangleComponent>();
        triangle->SetPadding(8.0_vp); // all padding
        triangle->SetWidth(25.0_vp); // left padding + it width + right padding = 8dp + 9dp + 8dp
        triangle->SetHeight(22.0_vp); // top padding + it height + bottom padding = 8dp + 6dp + 8dp
        triangle->SetColor(theme_->GetTitleStyle().GetTextColor());
        RefPtr<RowComponent> row = AceType::MakeRefPtr<RowComponent>(FlexAlign::CENTER,
            FlexAlign::CENTER, std::list<RefPtr<Component>>());
        title_->SetTextStyle(theme_->GetTitleStyle());
        RefPtr<BoxComponent> titleBox = AceType::MakeRefPtr<BoxComponent>();
        titleBox->SetChild(title_);
        if (!GetHasTriangle()) {
            row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(0.0, 1.0, 0.0, titleBox));
        } else if (GetTextDirection() != TextDirection::RTL) {
            row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(0.0, 1.0, 0.0, titleBox));
            row->AppendChild(triangle);
        } else {
            row->AppendChild(triangle);
            row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(0.0, 1.0, 0.0, titleBox));
        }
        auto titleComposed = GenerateAccessibilityComposed("picker-title", row, titleAccessibility_);
        outChildren.emplace_back(titleComposed);

        RefPtr<BoxComponent> spaceBox = AceType::MakeRefPtr<BoxComponent>();
        spaceBox->SetWidth(theme_->GetTitleBottomPadding().Value(), theme_->GetTitleBottomPadding().Unit());
        spaceBox->SetHeight(theme_->GetTitleBottomPadding().Value(), theme_->GetTitleBottomPadding().Unit());
        outChildren.emplace_back(spaceBox);
    }
}

void PickerBaseComponent::InitializeColumns(
    std::list<RefPtr<Component>>& outChildren, const RefPtr<AccessibilityManager>& accessibilityManager)
{
    if (!theme_) {
        LOGE("theme is null.");
        return;
    }

    std::list<RefPtr<Component>> columns;
    for (const auto& column : columns_) {
        column->SetTheme(theme_->clone());
        column->SetTextDirection(GetTextDirection());
        column->SetInDialog(isDialog_);
        column->SetFixHeight(GetColumnHeight());
        auto id = column->GetNodeId();
        if (id < 0) {
            id = accessibilityManager->GenerateNextAccessibilityId();
            // picker and picker-view accessibility parentNode different.
            static const char* tag = "picker-column";
            if (isDialog_) {
                accessibilityManager->CreateSpecializedNode(tag, id, rootAccessibilityId_);
            } else {
                accessibilityManager->CreateSpecializedNode(tag, id, GetNodeId());
            }
            column->SetNodeId(id);
        }
        columns.emplace_back(column);
    }
    RefPtr<RowComponent> row = AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, columns);
    outChildren.emplace_back(row);
}

void PickerBaseComponent::InitializeChildAccessibility(const RefPtr<AccessibilityManager>& accessibilityManager)
{
    if (!hasLunar_) {
        if (lunarAccessibility_) {
            accessibilityManager->RemoveAccessibilityNodes(lunarAccessibility_);
            lunarAccessibility_ = nullptr;
        }
        if (switchAccessibility_) {
            accessibilityManager->RemoveAccessibilityNodes(switchAccessibility_);
            switchAccessibility_ = nullptr;
        }
        return;
    }
}

void PickerBaseComponent::ClearAccessibilityNodes()
{
    auto accessibilityManager = accessibilityManager_.Upgrade();
    if (!accessibilityManager) {
        LOGE("accessibilityManager is null");
        return;
    }

    if (rootAccessibility_) {
        accessibilityManager->RemoveAccessibilityNodes(rootAccessibility_);
        rootAccessibility_ = nullptr;
        lunarAccessibility_ = nullptr;
        switchAccessibility_ = nullptr;
        titleAccessibility_ = nullptr;
        cancelAccessibility_ = nullptr;
        okAccessibility_ = nullptr;
    }

    for (const auto& column : columns_) {
        column->SetNodeId(-1); // reset id.
    }
}

RefPtr<Component> PickerBaseComponent::GenerateAccessibilityComposed(
    const std::string& name, const RefPtr<Component>& child, RefPtr<AccessibilityNode>& node)
{
    auto accessibilityManager = accessibilityManager_.Upgrade();
    if (!accessibilityManager) {
        return child;
    }

    if (!node) {
        int32_t parentNodeId = -1;
        int32_t composedId = accessibilityManager->GenerateNextAccessibilityId();
        if (isDialog_) {
            parentNodeId = rootAccessibilityId_;
        } else {
            parentNodeId = GetNodeId();
        }
        node = accessibilityManager->CreateSpecializedNode(name, composedId, parentNodeId);
    }
    return child;
}

void PickerBaseComponent::InitializeLunar(
    std::list<RefPtr<Component>>& outChildren, const RefPtr<ThemeManager>& themeManager)
{
    if (!theme_) {
        LOGE("theme is null");
        return;
    }

    if (hasLunar_) {
        RefPtr<BoxComponent> spaceBox = AceType::MakeRefPtr<BoxComponent>();
        spaceBox->SetWidth(theme_->GetButtonTopPadding().Value(), theme_->GetButtonTopPadding().Unit());
        spaceBox->SetHeight(theme_->GetButtonTopPadding().Value(), theme_->GetButtonTopPadding().Unit());
        outChildren.emplace_back(spaceBox);
        std::wstring lunarString { 0x663E, 0x793A, 0x519C, 0x5386 }; // the unicode encoding of chinese string of lunar.
        RefPtr<TextComponent> lunarText = AceType::MakeRefPtr<TextComponent>(StringUtils::ToString(lunarString));
        auto lunarStyle = theme_->GetOptionStyle(false, false);
        if (GetIsDialog()) {
            lunarStyle.SetLineHeight(theme_->GetLunarHeight());
        }
        lunarText->SetTextStyle(lunarStyle);
        auto checkboxTheme = themeManager->GetTheme<CheckboxTheme>();
        lunar_ = AceType::MakeRefPtr<CheckboxComponent>(checkboxTheme);
        lunar_->SetHorizontalPadding(Dimension(0.0));
        lunar_->SetHotZoneVerticalPadding(Dimension(0.0));
        auto lunarSwitchComposed = GenerateAccessibilityComposed("lunar-switch", lunar_, switchAccessibility_);
        RefPtr<BoxComponent> lunarBox = AceType::MakeRefPtr<BoxComponent>();
        lunarBox->SetChild(lunarSwitchComposed);
        lunarBox->SetWidth(theme_->GetLunarWidth().Value(), theme_->GetLunarWidth().Unit());
        lunarBox->SetHeight(theme_->GetLunarHeight().Value(), theme_->GetLunarHeight().Unit());

        std::list<RefPtr<Component>> lunarChildren;
        FlexAlign mainAlign = FlexAlign::AUTO;
        if (GetTextDirection() == TextDirection::RTL) {
            mainAlign = FlexAlign::FLEX_END;
            lunarChildren.emplace_back(lunarText);
            lunarChildren.emplace_back(lunarBox);
        } else {
            mainAlign = FlexAlign::FLEX_START;
            lunarChildren.emplace_back(lunarBox);
            lunarChildren.emplace_back(lunarText);
        }
        RefPtr<RowComponent> lunarRow =
            AceType::MakeRefPtr<RowComponent>(mainAlign, FlexAlign::CENTER, lunarChildren);
        RefPtr<DisplayComponent> display = AceType::MakeRefPtr<DisplayComponent>();
        display->SetChild(lunarRow);
        outChildren.emplace_back(display);
    }
}

void PickerBaseComponent::InitializeButtons(
    std::list<RefPtr<Component>>& outChildren, const RefPtr<ThemeManager>& themeManager)
{
    auto dialogTheme = themeManager->GetTheme<DialogTheme>();
    if (!theme_ || !dialogTheme) {
        LOGE("theme is null.");
        return;
    }

    auto buttonTheme = AceType::DynamicCast<ButtonTheme>(themeManager->GetTheme(ButtonTheme::TypeId()));
    if (!buttonTheme) {
        return;
    }
    auto buttonTextStyle = buttonTheme->GetTextStyle();
    auto buttonFocusColor = theme_->GetFocusColor();
    buttonTextStyle.SetTextColor(buttonFocusColor);

    if (isDialog_ || isCreateDialogComponent_) {
        RefPtr<BoxComponent> topPaddingBox = AceType::MakeRefPtr<BoxComponent>();
        topPaddingBox->SetWidth(theme_->GetButtonTopPadding().Value(), theme_->GetButtonTopPadding().Unit());
        topPaddingBox->SetHeight(theme_->GetButtonTopPadding().Value(), theme_->GetButtonTopPadding().Unit());
        outChildren.emplace_back(topPaddingBox);

        auto cancelButton = ButtonBuilder::Build(themeManager,
            Localization::GetInstance()->GetEntryLetters("common.cancel"), buttonTextStyle, buttonFocusColor, true);
        cancelButton->SetBackgroundColor(dialogTheme->GetButtonBackgroundColor());
        cancelButton->SetClickedColor(dialogTheme->GetButtonClickedColor());
        cancelButton->SetClickedEventId(onCancelClickId_);
        cancelButton->SetType(ButtonType::CAPSULE);
        cancelButton->SetHeight(theme_->GetButtonHeight());
        auto cancelComposed = GenerateAccessibilityComposed("picker-button", cancelButton, cancelAccessibility_);
        if (cancelAccessibility_) {
            cancelAccessibility_->SetText(Localization::GetInstance()->GetEntryLetters("common.cancel"));
        }

        auto okButton = ButtonBuilder::Build(themeManager, Localization::GetInstance()->GetEntryLetters("common.ok"),
            buttonTextStyle, buttonFocusColor, true);
        okButton->SetBackgroundColor(dialogTheme->GetButtonBackgroundColor());
        okButton->SetClickedColor(dialogTheme->GetButtonClickedColor());
        okButton->SetClickedEventId(onOkClickId_);
        okButton->SetType(ButtonType::CAPSULE);
        okButton->SetHeight(theme_->GetButtonHeight());
        auto okComposed = GenerateAccessibilityComposed("picker-button", okButton, okAccessibility_);
        if (okAccessibility_) {
            okAccessibility_->SetText(Localization::GetInstance()->GetEntryLetters("common.ok"));
        }

        auto dividerBox = AceType::MakeRefPtr<BoxComponent>();
        dividerBox->SetWidth(dialogTheme->GetDividerWidth().Value(), dialogTheme->GetDividerWidth().Unit());
        dividerBox->SetHeight(dialogTheme->GetDividerHeight().Value(), dialogTheme->GetDividerHeight().Unit());
        auto backDecoration = AceType::MakeRefPtr<Decoration>();
        backDecoration->SetBackgroundColor(dialogTheme->GetDividerColor());
        dividerBox->SetBackDecoration(backDecoration);
        dividerBox->SetMargin(dialogTheme->GetDividerPadding());

        std::list<RefPtr<Component>> buttons;
        RefPtr<RowComponent> row =
            AceType::MakeRefPtr<RowComponent>(FlexAlign::SPACE_AROUND, FlexAlign::FLEX_START, buttons);
        row->SetTextDirection(GetTextDirection());
        row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(1.0, 1.0, 0.0, cancelComposed));
        row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(0.0, 0.0, 0.0, dividerBox));
        row->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(1.0, 1.0, 0.0, okComposed));
        row->SetStretchToParent(true);

        auto display = AceType::MakeRefPtr<DisplayComponent>();
        display->SetChild(row);
        outChildren.emplace_back(display);
    }
}

void PickerBaseComponent::InitializeContainer(const std::list<RefPtr<Component>>& outChildren)
{
    if (!theme_) {
        LOGE("theme is null.");
        return;
    }

    RefPtr<ColumnComponent> outColumn =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::CENTER, FlexAlign::FLEX_START, outChildren);
    RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
    box->SetDeliverMinToChild(false);
    box->SetAlignment(Alignment::CENTER);
    if (GetIsDialog()) {
        RefPtr<FocusCollaborationComponent> collaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
        collaboration->InsertChild(0, outColumn);
        box->SetChild(collaboration);
    } else {
        box->SetChild(outColumn);
    }

    RefPtr<BoxComponent> outBox = AceType::MakeRefPtr<BoxComponent>();
    outBox->SetDeliverMinToChild(false);
    outBox->SetAlignment(Alignment::CENTER);
    outBox->SetChild(box);
    if (GetTextDirection() == TextDirection::RTL) {
        outColumn->SetCrossAxisAlign(FlexAlign::FLEX_END);
        box->SetAlignment(Alignment::CENTER_RIGHT);
    }
    // picker need build outer composed component but picker-view don't need.
    if (isDialog_) {
        if (GetSubsidiary()) {
            auto edge = theme_->GetPopupEdge();
            edge.SetTop(0.0_vp);
            edge.SetBottom(0.0_vp);
            auto temp = AceType::MakeRefPtr<Decoration>();
            temp->SetBackgroundColor(Color::TRANSPARENT);
            box->SetBackDecoration(temp);
            box->SetPadding(edge);
            outBox->SetBackDecoration(temp);
        } else {
            box->SetBackDecoration(theme_->GetPopupDecoration(false));
            box->SetPadding(theme_->GetPopupEdge());
            outBox->SetBackDecoration(theme_->GetPopupDecoration(true));
        }
        SetChild(outBox);
    } else {
        SetChild(outBox);
    }
}

void PickerBaseComponent::Initialize(
    const RefPtr<AccessibilityManager>& accessibilityManager, const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        return;
    }

    if (!theme_) {
        theme_ = themeManager->GetTheme<PickerTheme>();
    }
    accessibilityManager_ = accessibilityManager;
    OnColumnsBuilding();
    OnTitleBuilding();

    // picker need build rootAccessibilityNode but picker-view don't need.
    if (!rootAccessibility_ && isDialog_) {
        rootAccessibilityId_ = accessibilityManager->GenerateNextAccessibilityId();
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        rootAccessibility_ = accessibilityManager->CreateSpecializedNode(
            "picker-dialog", rootAccessibilityId_, GetPickerBaseId());
#else
        rootAccessibility_ = accessibilityManager->CreateSpecializedNode("picker-dialog", rootAccessibilityId_, -1);
#endif
    }
    std::list<RefPtr<Component>> outChildren;
    InitializeTitle(outChildren);
    InitializeColumns(outChildren, accessibilityManager);
    InitializeLunar(outChildren, themeManager);
    InitializeButtons(outChildren, themeManager);
    InitializeContainer(outChildren);
    InitializeChildAccessibility(accessibilityManager);
}

} // namespace OHOS::Ace
