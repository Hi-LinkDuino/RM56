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

#include "core/components/picker/picker_datetime_component.h"

#include "base/i18n/localization.h"

namespace OHOS::Ace {

PickerDateTimeComponent::PickerDateTimeComponent() {}

void PickerDateTimeComponent::OnTitleBuilding()
{
    auto theme = GetTheme();
    if (!theme) {
        LOGE("theme is null.");
        return;
    }
    SetHasTitle(theme->GetShowButtons());
    SetHasButtons(theme->GetShowButtons());
    SetHasTriangle(true); // triangle only for datetime picker.

    auto titleComponent = GetTitle();
    if (!titleComponent) {
        LOGE("title component is null.");
        return;
    }
    titleComponent->SetData(currentDate_.ToString(false));
}

void PickerDateTimeComponent::OnColumnsBuilding()
{
    PickerTimeComponent::OnColumnsBuilding();
    FillSolarLunarDays(lunar_, selectedDate_);
    InitDatePicker();
}

void PickerDateTimeComponent::OnSelectedSaving()
{
    PickerTimeComponent::OnSelectedSaving();
    selectedDate_ = currentDate_;
}

std::string PickerDateTimeComponent::GetSelectedObject(bool isColumnChange,
    const std::string& changeColumnTag, int32_t status) const
{
    if (isColumnChange) {
        LOGE("datetime picker not support column change event.");
        return "{}";
    }

    auto date = selectedDate_;
    // W3C's month is between 0 to 11, need to reduce one.
    date.SetMonth(date.GetMonth() - 1);
    PickerDateTime dateTime(date, GetSelectedTime());
    return dateTime.ToString(true, status);
}

void PickerDateTimeComponent::OnLunarCallback(bool checked, std::vector<std::string>& resultTags)
{
    FillSolarLunarDays(checked, currentDate_);
    resultTags.emplace_back(PICKER_MONTHDAY_COLUMN);
}

void PickerDateTimeComponent::OnTriangleCallback(bool value)
{
    if (value) {
        HideDatePicker();
    } else {
        ShowDatePicker();
    }
}

void PickerDateTimeComponent::InitDatePicker()
{
    if (datePicker_) {
        return; // already init.
    }
    datePicker_ = AceType::MakeRefPtr<PickerDateComponent>();
    datePicker_->SetSubsidiary(true);
    datePicker_->SetHasLunar(false);
    datePicker_->SetIsDialog(true);
    datePicker_->SetTextDirection(GetTextDirection());
    datePicker_->SetTheme(GetTheme());
    datePicker_->SetColumnHeight(GetColumnHeight());
    datePicker_->SetOnDateChange([weak = WeakClaim(this)] (const PickerDate& date) {
        auto refPtr = weak.Upgrade();
        if (!refPtr) {
            return;
        }
        refPtr->OnSubsidiaryChange(date);
    });
    auto controller = AceType::MakeRefPtr<PickerAnimationController>();
    controller->SetOutStopCallback([weak = AceType::WeakClaim(AceType::RawPtr(datePicker_))] {
        auto refPtr = weak.Upgrade();
        if (!refPtr) {
            return;
        }
        refPtr->HideDialog();
    });
    SetAnimationController(controller);
    datePicker_->SetAnimationController(controller);
}

void PickerDateTimeComponent::OnSubsidiaryChange(const PickerDate& date)
{
    auto days = date.ToDays();
    days--;
    currentDate_.FromDays(days);

    auto monthDay = GetColumn(PICKER_MONTHDAY_COLUMN);
    if (!monthDay) {
        return;
    }

    monthDay->HandleChangeCallback(true, false);
}

void PickerDateTimeComponent::ShowDatePicker()
{
    InitDatePicker();
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    datePicker_->SetPickerBaseId(GetPickerBaseId());
#endif
    datePicker_->SetShowLunar(lunar_);
    datePicker_->SetSelectedDate(currentDate_);
    datePicker_->SetMasterHasLunar(GetHasLunar());
    datePicker_->ShowDialog(GetStack(), false);
}

void PickerDateTimeComponent::HideDatePicker()
{
    InitDatePicker();
    auto controller = GetAnimationController();
    if (!controller) {
        LOGE("controller is null.");
        return;
    }

    controller->Play(false);
}

void PickerDateTimeComponent::OnColumnsCreating()
{
    RemoveColumn(PICKER_MONTHDAY_COLUMN);
    auto monthDay = AceType::MakeRefPtr<PickerColumnComponent>();
    monthDay->SetColumnTag(PICKER_MONTHDAY_COLUMN);
    if (GetHour24()) {
        monthDay->SetWidthRatio(3); // date:hour:minute:second = 3:2:2:2
    } else {
        monthDay->SetWidthRatio(4); // date:amPm:hour:minute:second = 4:2:2:2:2
    }
    AppendColumn(monthDay);

    PickerTimeComponent::OnColumnsCreating();
}

std::string PickerDateTimeComponent::GetMonthDayFormatString(bool lunar, uint32_t days) const
{
    PickerDate outDate;
    outDate.FromDays(days);
    auto nowadays = PickerDate::Current().ToDays();
    if (days == nowadays) {
        return Localization::GetInstance()->GetRelativeDateTime(0.0);
    }
    if (!lunar) {
        DateTime dateTime;
        dateTime.year = outDate.GetYear();
        dateTime.month = outDate.GetMonth() - 1; // W3C's month start from 0 to 11
        dateTime.day = outDate.GetDay();
        return Localization::GetInstance()->FormatDateTime(dateTime, "MMMd");
    }
    Date date;
    date.year = outDate.GetYear();
    date.month = outDate.GetMonth();
    date.day = outDate.GetDay();
    auto lunarDate = Localization::GetInstance()->GetLunarDate(date);
    return Localization::GetInstance()->GetLunarMonth(lunarDate.month, lunarDate.isLeapMonth) +
        Localization::GetInstance()->GetLunarDay(lunarDate.day);
}

void PickerDateTimeComponent::FillSolarLunarDays(bool lunar, const PickerDate& currentDate)
{
    auto monthDay = GetColumn(PICKER_MONTHDAY_COLUMN);
    if (!monthDay) {
        return;
    }
    monthDay->ClearOption();
    monthDay->SetCurrentIndex(3); // total option is fixed 7. center index is 3 which is current.
    uint32_t centerDays = currentDate.ToDays();
    uint32_t startDays = centerDays - 3; // start day 3 days before center day.
    uint32_t endDays = centerDays + 3; // end day 3 days after center day.
    for (uint32_t days = startDays; days <= endDays; ++days) {
        monthDay->AppendOption(GetMonthDayFormatString(lunar, days));
    }
    lunar_ = lunar;
    currentDate_ = currentDate;
}

void PickerDateTimeComponent::OnDataLinking(const std::string& tag, bool isAdd, uint32_t index,
    std::vector<std::string>& resultTags)
{
    PickerTimeComponent::OnDataLinking(tag, isAdd, index, resultTags);
    if (tag == PICKER_MONTHDAY_COLUMN) {
        // linked by month day column itself.
        auto days = currentDate_.ToDays();
        days = (isAdd ? days + 1 : days - 1); // add one day or reduce one day.
        PickerDate date;
        date.FromDays(days);
        FillSolarLunarDays(lunar_, date);
        resultTags.emplace_back(PICKER_MONTHDAY_COLUMN);
        return;
    }
    auto it = std::find(resultTags.begin(), resultTags.end(), PICKER_MONTHDAY_COLUMN);
    if (it != resultTags.end()) {
        // linked by other column
        auto days = currentDate_.ToDays();
        days = (isAdd ? days + 1 : days - 1);
        PickerDate date;
        date.FromDays(days);
        FillSolarLunarDays(lunar_, date);
    }
}

} // namespace OHOS::Ace
