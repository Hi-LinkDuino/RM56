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

#include "core/components/picker/picker_time_component.h"

namespace OHOS::Ace {

PickerTimeComponent::PickerTimeComponent()
{
    vecAmPm_ = Localization::GetInstance()->GetAmPmStrings();
}

void PickerTimeComponent::OnTitleBuilding()
{
    PickerBaseComponent::OnTitleBuilding();

    auto titleComponent = GetTitle();
    if (!titleComponent) {
        LOGE("title component is null.");
        return;
    }

    auto date = PickerDate::Current();
    titleComponent->SetData(date.ToString(false));
}

void PickerTimeComponent::OnColumnsBuilding()
{
    HandleHourColumnBuilding();

    auto minuteColumn = GetColumn(PICKER_MINUTE_COLUMN);
    auto secondColumn = GetColumn(PICKER_SECOND_COLUMN);
    if (!minuteColumn) {
        LOGE("minute column is null.");
        return;
    }

    minuteColumn->ClearOption();
    for (uint32_t minute = 0; minute <= 59; ++minute) { // time's minute from 0 to 59
        if (minute == selectedTime_.GetMinute()) {
            minuteColumn->SetCurrentIndex(minuteColumn->GetOptionCount());
        }
        minuteColumn->AppendOption(GetMinuteFormatString(minute));
    }

    if (hasSecond_ && secondColumn) {
        secondColumn->ClearOption();
        for (uint32_t second = 0; second <= 59; ++second) { // time's second from 0 to 59
            if (second == selectedTime_.GetSecond()) {
                secondColumn->SetCurrentIndex(secondColumn->GetOptionCount());
            }
            secondColumn->AppendOption(GetSecondFormatString(second));
        }
    }
}

void PickerTimeComponent::HandleHourColumnBuilding()
{
    auto hourColumn = GetColumn(PICKER_HOUR_COLUMN);
    auto amPmColumn = GetColumn(PICKER_AMPM_COLUMN);
    if (!hourColumn) {
        LOGE("hour column is null.");
        return;
    }

    hourColumn->ClearOption();
    if (hour24_) {
        for (uint32_t hour = 0; hour <= 23; ++hour) { // time's hour from 0 to 23.
            if (hour == selectedTime_.GetHour()) {
                hourColumn->SetCurrentIndex(hourColumn->GetOptionCount());
            }
            hourColumn->AppendOption(GetHourFormatString(hour));
        }
    } else if (amPmColumn) {
        amPmColumn->ClearOption();
        amPmColumn->AppendOption(GetAmFormatString());
        amPmColumn->AppendOption(GetPmFormatString());
        if (IsAmHour(selectedTime_.GetHour())) {
            amPmColumn->SetCurrentIndex(0); // AM's index
        } else {
            amPmColumn->SetCurrentIndex(1); // PM's index
        }
        auto selectedHour = GetAmPmHour(selectedTime_.GetHour());
        for (uint32_t hour = 1; hour <= 12; ++hour) { // hour start from 1 to 12
            if (hour == selectedHour) {
                hourColumn->SetCurrentIndex(hourColumn->GetOptionCount());
            }
            hourColumn->AppendOption(GetHourFormatString(hour));
        }
    } else {
        LOGE("AM PM column is null.");
    }
}

std::string PickerTimeComponent::GetSelectedObject(bool isColumnChange,
    const std::string& changeColumnTag, int32_t status) const
{
    auto time = selectedTime_;
    if (isColumnChange) {
        time = GetCurrentTime();
    }
    return time.ToString(true, hasSecond_, status);
}

void PickerTimeComponent::OnDataLinking(const std::string& tag, bool isAdd, uint32_t index,
    std::vector<std::string>& resultTags)
{
    if (tag != PICKER_HOUR_COLUMN) {
        return;
    }

    if (hour24_) {
        HandleHour24Change(isAdd, index, resultTags);
    } else {
        HandleHour12Change(isAdd, index, resultTags);
    }
}

void PickerTimeComponent::HandleHour24Change(bool isAdd, uint32_t index, std::vector<std::string>& resultTags)
{
    if (isAdd && index == 0) {
        resultTags.emplace_back(PICKER_MONTHDAY_COLUMN);
        return;
    }
    if (!isAdd && index == 23) { // hour end with 23 which is the last hour
        resultTags.emplace_back(PICKER_MONTHDAY_COLUMN);
        return;
    }
}

void PickerTimeComponent::HandleHour12Change(bool isAdd, uint32_t index, std::vector<std::string>& resultTags)
{
    auto amPm = GetColumn(PICKER_AMPM_COLUMN);
    if (!amPm) {
        LOGE("AM PM column is null.");
        return;
    }

    if (amPm->GetCurrentIndex() == 0 && isAdd && index == 11) { // hour index start from 0 to 11
        amPm->SetCurrentIndex(1); // add to PM's index
        resultTags.emplace_back(PICKER_AMPM_COLUMN);
        return;
    }

    if (amPm->GetCurrentIndex() == 1 && !isAdd && index == 10) { // reduce to 11 hour (index is 10)
        amPm->SetCurrentIndex(0); // change to AM whose index is 0
        resultTags.emplace_back(PICKER_AMPM_COLUMN);
        return;
    }

    if (amPm->GetCurrentIndex() == 1 && isAdd && index == 11) { // is PM (index is 1) and last hour (index is 11)
        amPm->SetCurrentIndex(0); // change to PM (index is 0)
        resultTags.emplace_back(PICKER_AMPM_COLUMN);
        resultTags.emplace_back(PICKER_MONTHDAY_COLUMN);
        return;
    }

    if (amPm->GetCurrentIndex() == 0 && !isAdd && index == 10) { // reduce to 11 hour(index is 10)
        amPm->SetCurrentIndex(1); // change to PM
        resultTags.emplace_back(PICKER_AMPM_COLUMN);
        resultTags.emplace_back(PICKER_MONTHDAY_COLUMN);
        return;
    }
}

void PickerTimeComponent::OnSelectedSaving()
{
    selectedTime_ = GetCurrentTime();
}

PickerTime PickerTimeComponent::GetCurrentTime() const
{
    PickerTime time;
    auto amPmColumn = GetColumn(PICKER_AMPM_COLUMN);
    auto hourColumn = GetColumn(PICKER_HOUR_COLUMN);
    auto minuteColumn = GetColumn(PICKER_MINUTE_COLUMN);
    auto secondColumn = GetColumn(PICKER_SECOND_COLUMN);
    if (!hourColumn || !minuteColumn) {
        LOGE("hour or minute column is null.");
        return time;
    }

    if (hour24_) {
        time.SetHour(hourColumn->GetCurrentIndex()); // hour from 0 to 23, index from 0 to 23
    } else if (amPmColumn) {
        time.SetHour(GetHourFromAmPm(amPmColumn->GetCurrentIndex() == 0, hourColumn->GetCurrentIndex() + 1));
    } else {
        LOGE("AM PM column is null.");
    }

    time.SetMinute(minuteColumn->GetCurrentIndex()); // minute from 0 to 59, index from 0 to 59
    if (hasSecond_ && secondColumn) {
        time.SetSecond(secondColumn->GetCurrentIndex()); // second from 0 to 59, index from 0 to 59
    }
    return time;
}

void PickerTimeComponent::OnColumnsCreating()
{
    RemoveColumn(PICKER_AMPM_COLUMN);
    RemoveColumn(PICKER_HOUR_COLUMN);
    RemoveColumn(PICKER_MINUTE_COLUMN);
    RemoveColumn(PICKER_SECOND_COLUMN);

    if (!hour24_) {
        auto amPmColumn = AceType::MakeRefPtr<PickerColumnComponent>();
        amPmColumn->SetColumnTag(PICKER_AMPM_COLUMN);
        amPmColumn->SetWidthRatio(2); // date:amPm:hour:minute:second = 4:2:2:2:2
        AppendColumn(amPmColumn);
    }

    auto hourColumn = AceType::MakeRefPtr<PickerColumnComponent>();
    hourColumn->SetColumnTag(PICKER_HOUR_COLUMN);
    hourColumn->SetWidthRatio(2); // date:hour:minute:second = 3:2:2:2
    AppendColumn(hourColumn);

    auto minuteColumn = AceType::MakeRefPtr<PickerColumnComponent>();
    minuteColumn->SetColumnTag(PICKER_MINUTE_COLUMN);
    minuteColumn->SetWidthRatio(2); // date:hour:minute:second = 3:2:2:2
    AppendColumn(minuteColumn);

    if (hasSecond_) {
        auto secondColumn = AceType::MakeRefPtr<PickerColumnComponent>();
        secondColumn->SetColumnTag(PICKER_SECOND_COLUMN);
        secondColumn->SetWidthRatio(2); // date:hour:minute:second = 3:2:2:2
        AppendColumn(secondColumn);
        auto theme = GetTheme();
        if (!theme) {
            return;
        }
        hourColumn->SetHasSplitter(theme->HasTimeSplitter());
        minuteColumn->SetHasSplitter(theme->HasTimeSplitter());
    }
}

std::string PickerTimeComponent::GetAmFormatString() const
{
    if (vecAmPm_.empty()) {
        return "AM";
    }
    return vecAmPm_[0]; // first index is AM
}

std::string PickerTimeComponent::GetPmFormatString() const
{
    if (vecAmPm_.size() < 2) { // size need to be 2 for AM and PM
        return "PM";
    }
    return vecAmPm_[1]; // second index is PM
}

bool PickerTimeComponent::IsAmHour(uint32_t hourOf24) const
{
    return (0 <= hourOf24 && hourOf24 <= 11); // 00:00 to 11:00 is AM hour
}

uint32_t PickerTimeComponent::GetAmPmHour(uint32_t hourOf24) const
{
    if (hourOf24 == 0) {
        return 12; // AM 12:00 means 00:00 in 24 hour style
    } else if (1 <= hourOf24 && hourOf24 <= 11) { // 00:00 to 11:00 is the same for any hour style
        return hourOf24;
    } else if (hourOf24 == 12) { // 12:00 means PM start hour
        return 12; // 12 PM
    } else { // hour from 13 to 23
        return hourOf24 - 12; // need reduce 12 to 12 hours style
    }
}

uint32_t PickerTimeComponent::GetHourFromAmPm(bool isAm, uint32_t amPmhour) const
{
    if (isAm) {
        if (amPmhour == 12) { // AM 12:00 means 00:00
            return 0;
        }
        return amPmhour;
    }

    if (amPmhour == 12) { // PM 12 means 12:00
        return 12;
    }

    return amPmhour + 12; // need add 12 hour to 24 hours style
}

std::string PickerTimeComponent::GetHourFormatString(uint32_t hour) const
{
    DateTime time;
    time.minute = hour; // minute range [0, 59], hour range [0, 23]; hour range is in minute range.
    if (Localization::GetInstance()->HasZeroHour()) {
        return AddZeroPrefix(Localization::GetInstance()->FormatDateTime(time, "m"));
    }

    return Localization::GetInstance()->FormatDateTime(time, "m");
}

std::string PickerTimeComponent::GetMinuteFormatString(uint32_t minute) const
{
    DateTime time;
    time.minute = minute;
    return AddZeroPrefix(Localization::GetInstance()->FormatDateTime(time, "m"));
}

std::string PickerTimeComponent::GetSecondFormatString(uint32_t second) const
{
    DateTime time;
    time.second = second;
    return AddZeroPrefix(Localization::GetInstance()->FormatDateTime(time, "s"));
}

std::string PickerTimeComponent::AddZeroPrefix(const std::string& value) const
{
    if (value.size() == 1 && '0' <= value[0] && value[0] <= '9') { // value is number in range [0, 9]
        return std::string("0") + value; // add prefix '0'
    }
    return value;
}

} // namespace OHOS::Ace
