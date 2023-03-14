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

#include "core/components/picker/picker_date_component.h"

namespace OHOS::Ace {

/*
 * Lunar information in 200 years from 1900.
 * <p>
 * | 0 - 11(bit) | 12 - 15(bit) |
 * month      leap month
 * If last 4bit is 1111 or 0000 means no leap month.
 * If the last 4bit in next data is 1111, the days of leap month is 30 days,
 * otherwise, the days of leap month is 29days.
 */
const uint16_t LunarCalculator::LUNAR_INFO[] = {
    0x6aa0, 0xbaa3, 0xab50,
    0x4bd8, 0x4ae0, 0xa570, 0x54d5, 0xd260, 0xd950, 0x5554, 0x56af,
    0x9ad0, 0x55d2, 0x4ae0, 0xa5b6, 0xa4d0, 0xd250, 0xd295, 0xb54f,
    0xd6a0, 0xada2, 0x95b0, 0x4977, 0x497f, 0xa4b0, 0xb4b5, 0x6a50,
    0x6d40, 0xab54, 0x2b6f, 0x9570, 0x52f2, 0x4970, 0x6566, 0xd4a0,
    0xea50, 0x6a95, 0x5adf, 0x2b60, 0x86e3, 0x92ef, 0xc8d7, 0xc95f,
    0xd4a0, 0xd8a6, 0xb55f, 0x56a0, 0xa5b4, 0x25df, 0x92d0, 0xd2b2,
    0xa950, 0xb557, 0x6ca0, 0xb550, 0x5355, 0x4daf, 0xa5b0, 0x4573,
    0x52bf, 0xa9a8, 0xe950, 0x6aa0, 0xaea6, 0xab50, 0x4b60, 0xaae4,
    0xa570, 0x5260, 0xf263, 0xd950, 0x5b57, 0x56a0, 0x96d0, 0x4dd5,
    0x4ad0, 0xa4d0, 0xd4d4, 0xd250, 0xd558, 0xb540, 0xb6a0, 0x95a6,
    0x95bf, 0x49b0, 0xa974, 0xa4b0, 0xb27a, 0x6a50, 0x6d40, 0xaf46,
    0xab60, 0x9570, 0x4af5, 0x4970, 0x64b0, 0x74a3, 0xea50, 0x6b58,
    0x5ac0, 0xab60, 0x96d5, 0x92e0, 0xc960, 0xd954, 0xd4a0, 0xda50,
    0x7552, 0x56a0, 0xabb7, 0x25d0, 0x92d0, 0xcab5, 0xa950, 0xb4a0,
    0xbaa4, 0xad50, 0x55d9, 0x4ba0, 0xa5b0, 0x5176, 0x52bf, 0xa930,
    0x7954, 0x6aa0, 0xad50, 0x5b52, 0x4b60, 0xa6e6, 0xa4e0, 0xd260,
    0xea65, 0xd530, 0x5aa0, 0x76a3, 0x96d0, 0x4afb, 0x4ad0, 0xa4d0,
    0xd0b6, 0xd25f, 0xd520, 0xdd45, 0xb5a0, 0x56d0, 0x55b2, 0x49b0,
    0xa577, 0xa4b0, 0xaa50, 0xb255, 0x6d2f, 0xada0, 0x4b63, 0x937f,
    0x49f8, 0x4970, 0x64b0, 0x68a6, 0xea5f, 0x6b20, 0xa6c4, 0xaaef,
    0x92e0, 0xd2e3, 0xc960, 0xd557, 0xd4a0, 0xda50, 0x5d55, 0x56a0,
    0xa6d0, 0x55d4, 0x52d0, 0xa9b8, 0xa950, 0xb4a0, 0xb6a6, 0xad50,
    0x55a0, 0xaba4, 0xa5b0, 0x52b0, 0xb273, 0x6930, 0x7337, 0x6aa0,
    0xad50, 0x4b55, 0x4b6f, 0xa570, 0x54e4, 0xd260, 0xe968, 0xd520,
    0xdaa0, 0x6aa6, 0x56df, 0x4ae0, 0xa9d4, 0xa4d0, 0xd150, 0xf252,
    0xd520, 0xdd45, 0xb5a0, 0x56d0
};

bool PickerStringFormatter::inited_ = false;
const std::string PickerStringFormatter::empty_;
std::vector<std::string> PickerStringFormatter::years_; // year from 1900 to 2100,count is 201
std::vector<std::string> PickerStringFormatter::solarMonths_; // solar month from 1 to 12,count is 12
std::vector<std::string> PickerStringFormatter::solarDays_; // solar day from 1 to 31, count is 31
std::vector<std::string> PickerStringFormatter::lunarMonths_; // lunar month from 1 to 24, count is 24
std::vector<std::string> PickerStringFormatter::lunarDays_; // lunar day from 1 to 30, count is 30
std::vector<std::string> PickerStringFormatter::tagOrder_; // order of year month day

void PickerStringFormatter::Init()
{
    if (inited_) {
        return;
    }
    years_.resize(201); // year from 1900 to 2100,count is 201
    solarMonths_.resize(12); // solar month from 1 to 12,count is 12
    solarDays_.resize(31); // solar day from 1 to 31, count is 31
    lunarMonths_.resize(24); // lunar month from 1 to 24, count is 24
    lunarDays_.resize(30); // lunar day from 1 to 30, count is 30
    // init year from 1900 to 2100
    for (uint32_t year = 1900; year <= 2100; ++year) {
        DateTime date;
        date.year = year;
        years_[year - 1900] = Localization::GetInstance()->FormatDateTime(date, "y"); // index start from 0
    }
    // init solar month from 1 to 12
    auto months = Localization::GetInstance()->GetMonths(true);
    for (uint32_t month = 1; month <= 12; ++month) {
        if (month - 1 < months.size()) {
            solarMonths_[month - 1] = months[month - 1];
            continue;
        }
        DateTime date;
        date.month = month - 1; // W3C's month start from 0 to 11
        solarMonths_[month - 1] = Localization::GetInstance()->FormatDateTime(date, "M"); // index start from 0
    }
    // init solar day from 1 to 31
    for (uint32_t day = 1; day <= 31; ++day) {
        DateTime date;
        date.day = day;
        solarDays_[day - 1] = Localization::GetInstance()->FormatDateTime(date, "d"); // index start from 0
    }
    // init lunar month from 1 to 24 which is 1th, 2th, ... leap 1th, leap 2th ...
    for (uint32_t index = 1; index <= 24; ++index) {
        uint32_t month = (index > 12 ? index - 12 : index);
        bool isLeap = (index > 12);
        lunarMonths_[index - 1] = Localization::GetInstance()->GetLunarMonth(month, isLeap); // index start from 0
    }
    // init lunar day from 1 to 30
    for (uint32_t day = 1; day <= 30; ++day) {
        lunarDays_[day - 1] = Localization::GetInstance()->GetLunarDay(day); // index start from 0
    }
    inited_ = true;
    Localization::GetInstance()->SetOnChange([]() { PickerStringFormatter::inited_ = false; });
}

const std::string& PickerStringFormatter::GetYear(uint32_t year)
{
    Init();
    if (!(1900 <= year && year <= 2100)) { // year in [1900,2100]
        return empty_;
    }
    return years_[year - 1900]; // index in [0, 200]
}

const std::string& PickerStringFormatter::GetSolarMonth(uint32_t month)
{
    Init();
    if (!(1 <= month && month <= 12)) { // solar month in [1,12]
        return empty_;
    }
    return solarMonths_[month - 1]; // index in [0,11]
}

const std::string& PickerStringFormatter::GetSolarDay(uint32_t day)
{
    Init();
    if (!(1 <= day && day <= 31)) { // solar day in [1,31]
        return empty_;
    }
    return solarDays_[day - 1]; // index in [0,30]
}

const std::string& PickerStringFormatter::GetLunarMonth(uint32_t month, bool isLeap)
{
    Init();
    uint32_t index = (isLeap ? month + 12 : month); // leap month is behind 12 index
    if (!(1 <= index && index <= 24)) { // lunar month need in [1,24]
        return empty_;
    }
    return lunarMonths_[index - 1]; // index in [0,23]
}

const std::string& PickerStringFormatter::GetLunarDay(uint32_t day)
{
    Init();
    if (!(1 <= day && day <= 30)) { // lunar day need in [1,30]
        return empty_;
    }
    return lunarDays_[day - 1]; // index in [0,29]
}

const std::vector<std::string>& PickerStringFormatter::GetTagOrder()
{
    tagOrder_.clear();
    Localization::GetInstance()->GetDateColumnFormatOrder(tagOrder_);
    return tagOrder_;
}

// all date should in  solar range [1900.1.31, 2100.12.31] and lunar range [1900.1.1, 2100.12.1]
const PickerDate PickerDateComponent::limitStartDate_(1900, 1, 31);
const PickerDate PickerDateComponent::limitEndDate_(2100, 12, 31);

PickerDateComponent::PickerDateComponent()
{
    auto yearColumn = AceType::MakeRefPtr<PickerColumnComponent>();
    yearColumn->SetColumnTag(PICKER_YEAR_COLUMN);
    yearColumn->SetWidthRatio(3); // year:month:day = 3:2:2
    auto monthColumn = AceType::MakeRefPtr<PickerColumnComponent>();
    monthColumn->SetColumnTag(PICKER_MONTH_COLUMN);
    monthColumn->SetWidthRatio(2); // year:month:day = 3:2:2
    auto dayColumn = AceType::MakeRefPtr<PickerColumnComponent>();
    dayColumn->SetColumnTag(PICKER_DAY_COLUMN);
    dayColumn->SetWidthRatio(2); // year:month:day = 3:2:2
    auto order = PickerStringFormatter::GetTagOrder();
    if (order.size() != 3) { // has 3 columns: year month day
        AppendColumn(yearColumn);
        AppendColumn(monthColumn);
        AppendColumn(dayColumn);
    } else {
        std::map<std::string, RefPtr<PickerColumnComponent>> tagColumns;
        tagColumns[PICKER_YEAR_COLUMN] = yearColumn;
        tagColumns[PICKER_MONTH_COLUMN] = monthColumn;
        tagColumns[PICKER_DAY_COLUMN] = dayColumn;
        for (uint32_t i = 0; i < order.size(); ++i) {
            AppendColumn(tagColumns[order[i]]);
        }
    }
    startDateLunar_ = SolarToLunar(startDateSolar_);
    endDateLunar_ = SolarToLunar(endDateSolar_);
}

void PickerDateComponent::OnTitleBuilding()
{
    auto theme = GetTheme();
    if (!theme) {
        LOGE("theme is null.");
        return;
    }
    SetHasTitle(theme->GetShowButtons());
    SetHasButtons(theme->GetShowButtons());

    auto date = GetCurrentDate();
    if (onDateChange_) {
        onDateChange_(date);
    }

    auto titleComponent = GetTitle();
    if (!titleComponent) {
        LOGE("title component is null.");
        return;
    }
    titleComponent->SetData(date.ToString(false));
}

void PickerDateComponent::OnColumnsBuilding()
{
    AdjustSolarDate(selectedDate_);
    if (lunar_) {
        auto lunarDate = SolarToLunar(selectedDate_);
        LunarColumnsBuilding(lunarDate);
    } else {
        SolarColumnsBuilding(selectedDate_);
    }
}

void PickerDateComponent::OnSelectedSaving()
{
    selectedDate_ = GetCurrentDate();
}

std::string PickerDateComponent::GetSelectedObject(bool isColumnChange,
    const std::string& changeColumnTag, int status) const
{
    auto date = selectedDate_;
    if (isColumnChange) {
        date = GetCurrentDate();
    }
    // W3C's month is between 0 to 11, need to reduce one.
    date.SetMonth(date.GetMonth() - 1);
    return date.ToString(true, status);
}

void PickerDateComponent::OnDataLinking(const std::string& tag, bool isAdd, uint32_t index,
    std::vector<std::string>& resultTags)
{
    if (tag == PICKER_YEAR_COLUMN) {
        HandleYearChange(isAdd, index, resultTags);
        return;
    }

    if (tag == PICKER_MONTH_COLUMN) {
        HandleMonthChange(isAdd, index, resultTags);
        return;
    }

    if (tag == PICKER_DAY_COLUMN) {
        HandleDayChange(isAdd, index, resultTags);
        return;
    }

    LOGE("unknown tag[%{private}s] of column.", tag.c_str());
}


void PickerDateComponent::OnAnimationPlaying()
{
    auto controller = GetAnimationController();
    if (!controller) {
        LOGE("controller is null.");
        return;
    }

    controller->Play(true);
}

PickerDate PickerDateComponent::GetCurrentDate() const
{
    PickerDate result;
    auto yearColumn = GetColumn(PICKER_YEAR_COLUMN);
    auto monthColumn = GetColumn(PICKER_MONTH_COLUMN);
    auto dayColumn = GetColumn(PICKER_DAY_COLUMN);
    if (!yearColumn || !monthColumn || !dayColumn) {
        LOGE("year or month or day column is null.");
        return result;
    }

    if (!lunar_) {
        result.SetYear(startDateSolar_.GetYear() + yearColumn->GetCurrentIndex());
        result.SetMonth(monthColumn->GetCurrentIndex() + 1); // month from 1 to 12, index from 0 to 11.
        result.SetDay(dayColumn->GetCurrentIndex() + 1); // day from 1 to 31, index from 0 to 30.
        return result;
    }

    uint32_t lunarYear = startDateLunar_.year + yearColumn->GetCurrentIndex();
    return LunarToSolar(GetCurrentLunarDate(lunarYear));
}

LunarDate PickerDateComponent::GetCurrentLunarDate(uint32_t lunarYear) const
{
    LunarDate lunarResult;
    auto yearColumn = GetColumn(PICKER_YEAR_COLUMN);
    auto monthColumn = GetColumn(PICKER_MONTH_COLUMN);
    auto dayColumn = GetColumn(PICKER_DAY_COLUMN);
    if (!yearColumn || !monthColumn || !dayColumn) {
        LOGE("year or month or day column is null.");
        return lunarResult;
    }

    uint32_t lunarLeapMonth = 0;
    bool hasLeapMonth = GetLunarLeapMonth(lunarYear, lunarLeapMonth);
    lunarResult.isLeapMonth = false;
    if (!hasLeapMonth) {
        lunarResult.month = monthColumn->GetCurrentIndex() + 1; // month from 1 to 12, index from 0 to 11
    } else {
        if (monthColumn->GetCurrentIndex() == lunarLeapMonth) {
            lunarResult.isLeapMonth = true;
            lunarResult.month = lunarLeapMonth;
        } else if (monthColumn->GetCurrentIndex() < lunarLeapMonth) {
            lunarResult.month = monthColumn->GetCurrentIndex() + 1; // month start from 1, index start from 0
        } else {
            lunarResult.month = monthColumn->GetCurrentIndex();
        }
    }
    lunarResult.year = startDateLunar_.year + yearColumn->GetCurrentIndex();
    lunarResult.day = dayColumn->GetCurrentIndex() + 1; // day start form 1, index start from 0
    return lunarResult;
}

void PickerDateComponent::HandleYearChange(bool isAdd, uint32_t index, std::vector<std::string>& resultTags)
{
    if (lunar_) {
        HandleLunarYearChange(isAdd, index);
    } else {
        HandleSolarYearChange(isAdd, index);
    }
    resultTags.emplace_back(PICKER_YEAR_COLUMN);
    resultTags.emplace_back(PICKER_MONTH_COLUMN);
    resultTags.emplace_back(PICKER_DAY_COLUMN);
}

void PickerDateComponent::HandleLunarYearChange(bool isAdd, uint32_t index)
{
    auto yearColumn = GetColumn(PICKER_YEAR_COLUMN);
    if (!yearColumn) {
        LOGE("year column is null.");
        return;
    }

    uint32_t lastYearIndex = index;
    auto optionCount = yearColumn->GetOptionCount();
    if (isAdd) { // need reduce one index
        lastYearIndex = optionCount != 0 ? (yearColumn->GetOptionCount() + lastYearIndex - 1) % optionCount : 0;
    } else { // need add one index
        lastYearIndex = optionCount != 0 ? (yearColumn->GetOptionCount() + lastYearIndex + 1) % optionCount : 0;
    }
    uint32_t lastLunarYear = startDateLunar_.year + lastYearIndex;
    auto lunarDate = GetCurrentLunarDate(lastLunarYear);
    uint32_t nowLeapMonth = 0;
    bool hasLeapMonth = GetLunarLeapMonth(lunarDate.year, nowLeapMonth);
    if (!hasLeapMonth && lunarDate.isLeapMonth) {
        lunarDate.isLeapMonth = false;
    }
    uint32_t nowMaxDay = GetLunarMaxDay(lunarDate.year, lunarDate.month, lunarDate.isLeapMonth);
    if (lunarDate.day > nowMaxDay) {
        lunarDate.day = nowMaxDay;
    }

    AdjustLunarDate(lunarDate);
    LunarColumnsBuilding(lunarDate);
}

void PickerDateComponent::HandleSolarYearChange(bool isAdd, uint32_t index)
{
    auto date = GetCurrentDate();

    bool leapYear = PickerDate::IsLeapYear(date.GetYear());
    if (date.GetMonth() == 2 && !leapYear && date.GetDay() > 28) { // invalidate of 2th month
        date.SetDay(28); // the max day of the 2th month of none leap year is 28
    }

    AdjustSolarDate(date);
    SolarColumnsBuilding(date);
}

void PickerDateComponent::HandleMonthChange(bool isAdd, uint32_t index, std::vector<std::string>& resultTags)
{
    if (lunar_) {
        HandleLunarMonthChange(isAdd, index);
    } else {
        HandleSolarMonthChange(isAdd, index);
    }
    resultTags.emplace_back(PICKER_YEAR_COLUMN);
    resultTags.emplace_back(PICKER_MONTH_COLUMN);
    resultTags.emplace_back(PICKER_DAY_COLUMN);
}

void PickerDateComponent::HandleLunarMonthChange(bool isAdd, uint32_t index)
{
    auto yearColumn = GetColumn(PICKER_YEAR_COLUMN);
    auto monthColumn = GetColumn(PICKER_MONTH_COLUMN);
    if (!yearColumn || !monthColumn) {
        LOGE("year or month column is null.");
        return;
    }

    uint32_t nowLunarYear = startDateLunar_.year + yearColumn->GetCurrentIndex();
    auto lunarDate = GetCurrentLunarDate(nowLunarYear);
    if (isAdd && index == 0) {
        lunarDate.year = lunarDate.year + 1; // add to next year
        if (lunarDate.year > endDateLunar_.year) {
            lunarDate.year = startDateLunar_.year;
        }
    }
    if (!isAdd && index == monthColumn->GetOptionCount() - 1) {
        lunarDate.year = lunarDate.year - 1; // reduce to previous year
        if (lunarDate.year < startDateLunar_.year) {
            lunarDate.year = endDateLunar_.year;
        }
    }
    uint32_t lunarLeapMonth = 0;
    bool hasLeapMonth = GetLunarLeapMonth(lunarDate.year, lunarLeapMonth);
    if (!hasLeapMonth && lunarDate.isLeapMonth) {
        lunarDate.isLeapMonth = false;
    }
    uint32_t maxDay = GetLunarMaxDay(lunarDate.year, lunarDate.month, lunarDate.isLeapMonth);
    if (lunarDate.day > maxDay) {
        lunarDate.day = maxDay;
    }

    AdjustLunarDate(lunarDate);
    LunarColumnsBuilding(lunarDate);
}

void PickerDateComponent::HandleSolarMonthChange(bool isAdd, uint32_t index)
{
    auto date = GetCurrentDate();
    if (isAdd && date.GetMonth() == 1) { // first month is 1
        date.SetYear(date.GetYear() + 1); // add 1 year, the next year
        if (date.GetYear() > endDateSolar_.GetYear()) {
            date.SetYear(startDateSolar_.GetYear());
        }
    }
    if (!isAdd && date.GetMonth() == 12) { // the last month is 12
        date.SetYear(date.GetYear() - 1); // reduce 1 year, the previous year
        if (date.GetYear() < startDateSolar_.GetYear()) {
            date.SetYear(endDateSolar_.GetYear());
        }
    }
    uint32_t maxDay = PickerDate::GetMaxDay(date.GetYear(), date.GetMonth());
    if (date.GetDay() > maxDay) {
        date.SetDay(maxDay);
    }
    AdjustSolarDate(date);
    SolarColumnsBuilding(date);
}

void PickerDateComponent::HandleDayChange(bool isAdd, uint32_t index, std::vector<std::string>& resultTags)
{
    if (lunar_) {
        HandleLunarDayChange(isAdd, index);
    } else {
        HandleSolarDayChange(isAdd, index);
    }
    resultTags.emplace_back(PICKER_YEAR_COLUMN);
    resultTags.emplace_back(PICKER_MONTH_COLUMN);
    resultTags.emplace_back(PICKER_DAY_COLUMN);
}

void PickerDateComponent::HandleSolarDayChange(bool isAdd, uint32_t index)
{
    auto yearColumn = GetColumn(PICKER_YEAR_COLUMN);
    auto monthColumn = GetColumn(PICKER_MONTH_COLUMN);
    auto dayColumn = GetColumn(PICKER_DAY_COLUMN);
    if (!yearColumn || !monthColumn || !dayColumn) {
        LOGE("year or month or day column is null.");
        return;
    }

    auto date = GetCurrentDate();
    if (isAdd && index == 0) {
        date.SetMonth(date.GetMonth() + 1); // add to next month
        if (date.GetMonth() > 12) { // invalidate month, max month is 12
            date.SetMonth(1); // first month is 1
            date.SetYear(date.GetYear() + 1); // add to next year
            if (date.GetYear() > endDateSolar_.GetYear()) {
                date.SetYear(startDateSolar_.GetYear());
            }
        }
    }
    if (!isAdd && dayColumn->GetCurrentIndex() == dayColumn->GetOptionCount() - 1) { // last index is count - 1
        date.SetMonth(date.GetMonth() - 1); // reduce to previous month
        if (date.GetMonth() == 0) { // min month is 1, invalidate
            date.SetMonth(12); // set to be the last month
            date.SetYear(date.GetYear() - 1); // reduce to previous year
            if (date.GetYear() < startDateSolar_.GetYear()) {
                date.SetYear(endDateSolar_.GetYear());
            }
        }
        date.SetDay(PickerDate::GetMaxDay(date.GetYear(), date.GetMonth())); // reduce to previous month's last day
    }
    uint32_t maxDay = PickerDate::GetMaxDay(date.GetYear(), date.GetMonth());
    if (date.GetDay() > maxDay) {
        date.SetDay(maxDay);
    }
    AdjustSolarDate(date);
    SolarColumnsBuilding(date);
}

void PickerDateComponent::HandleLunarDayChange(bool isAdd, uint32_t index)
{
    if (isAdd) {
        HandleAddLunarDayChange(index);
    } else {
        HandleReduceLunarDayChange(index);
    }
}

void PickerDateComponent::HandleAddLunarDayChange(uint32_t index)
{
    auto yearColumn = GetColumn(PICKER_YEAR_COLUMN);
    auto monthColumn = GetColumn(PICKER_MONTH_COLUMN);
    auto dayColumn = GetColumn(PICKER_DAY_COLUMN);
    if (!yearColumn || !monthColumn || !dayColumn) {
        LOGE("year or month or day column is null.");
        return;
    }

    uint32_t nowLunarYear = startDateLunar_.year + yearColumn->GetCurrentIndex();
    auto lunarDate = GetCurrentLunarDate(nowLunarYear);
    uint32_t lunarLeapMonth = 0;
    bool hasLeapMonth = GetLunarLeapMonth(lunarDate.year, lunarLeapMonth);
    if (index == 0) {
        if (monthColumn->GetCurrentIndex() == monthColumn->GetOptionCount() - 1) { // max index is count - 1
            lunarDate.year = lunarDate.year + 1; // add to next year
            if (lunarDate.year > endDateLunar_.year) {
                lunarDate.year = startDateLunar_.year;
            }
            lunarDate.month = 1; // first month
            lunarDate.isLeapMonth = false;
        } else {
            if (lunarDate.isLeapMonth) {
                lunarDate.month = lunarDate.month + 1; // add to next month
                lunarDate.isLeapMonth = false;
            } else if (!hasLeapMonth) {
                lunarDate.month = lunarDate.month + 1; // add to next month
            } else if (lunarLeapMonth == lunarDate.month) {
                lunarDate.isLeapMonth = true;
            } else {
                lunarDate.month = lunarDate.month + 1; // add to next month
            }
        }
    }

    AdjustLunarDate(lunarDate);
    LunarColumnsBuilding(lunarDate);
}

void PickerDateComponent::HandleReduceLunarDayChange(uint32_t index)
{
    auto yearColumn = GetColumn(PICKER_YEAR_COLUMN);
    auto monthColumn = GetColumn(PICKER_MONTH_COLUMN);
    auto dayColumn = GetColumn(PICKER_DAY_COLUMN);
    if (!yearColumn || !monthColumn || !dayColumn) {
        LOGE("year or month or day column is null.");
        return;
    }

    uint32_t nowLunarYear = startDateLunar_.year + yearColumn->GetCurrentIndex();
    auto lunarDate = GetCurrentLunarDate(nowLunarYear);
    uint32_t lunarLeapMonth = 0;
    bool hasLeapMonth = GetLunarLeapMonth(lunarDate.year, lunarLeapMonth);
    if (dayColumn->GetCurrentIndex() == dayColumn->GetOptionCount() - 1) { // max index is count - 1
        if (monthColumn->GetCurrentIndex() == 0) {
            lunarDate.year = lunarDate.year - 1; // reduce to previous year
            if (lunarDate.year < startDateLunar_.year) {
                lunarDate.year = endDateLunar_.year;
            }
            lunarDate.month = 12; // set to be previous year's max month
            lunarDate.isLeapMonth = false;
            if (LunarCalculator::GetLunarLeapMonth(lunarDate.year) == 12) { // leap 12th month
                lunarDate.isLeapMonth = true;
            }
            lunarDate.day = GetLunarMaxDay(lunarDate.year, lunarDate.month, lunarDate.isLeapMonth);
        } else {
            if (lunarDate.isLeapMonth) {
                lunarDate.isLeapMonth = false;
            } else if (!hasLeapMonth) {
                lunarDate.month = lunarDate.month - 1; // reduce to previous month
            } else if (lunarLeapMonth == lunarDate.month - 1) { // leap month is previous month
                lunarDate.isLeapMonth = true;
                lunarDate.month = lunarLeapMonth;
            } else {
                lunarDate.month = lunarDate.month - 1; // reduce to previous month
            }
            lunarDate.day = GetLunarMaxDay(lunarDate.year, lunarDate.month, lunarDate.isLeapMonth);
        }
    }

    AdjustLunarDate(lunarDate);
    LunarColumnsBuilding(lunarDate);
}

std::string PickerDateComponent::GetYearFormatString(uint32_t year) const
{
    return PickerStringFormatter::GetYear(year);
}

std::string PickerDateComponent::GetMonthFormatString(uint32_t month, bool isLunar, bool isLeap) const
{
    if (isLunar) {
        return PickerStringFormatter::GetLunarMonth(month, isLeap);
    }

    return PickerStringFormatter::GetSolarMonth(month);
}

std::string PickerDateComponent::GetDayFormatString(uint32_t day, bool isLunar) const
{
    if (isLunar) {
        return PickerStringFormatter::GetLunarDay(day);
    }

    return PickerStringFormatter::GetSolarDay(day);
}

void PickerDateComponent::OnLunarCallback(bool checked, std::vector<std::string>& resultTags)
{
    if (checked) {
        auto solarDate = GetCurrentDate();
        auto lunarDate = SolarToLunar(solarDate);
        LunarColumnsBuilding(lunarDate);
    } else {
        auto solarDate = GetCurrentDate();
        SolarColumnsBuilding(solarDate);
    }

    resultTags.emplace_back(PICKER_YEAR_COLUMN);
    resultTags.emplace_back(PICKER_MONTH_COLUMN);
    resultTags.emplace_back(PICKER_DAY_COLUMN);
}

LunarDate PickerDateComponent::SolarToLunar(const PickerDate& date) const
{
    Date result;
    result.year = date.GetYear();
    result.month = date.GetMonth();
    result.day = date.GetDay();
    return Localization::GetInstance()->GetLunarDate(result);
}

PickerDate PickerDateComponent::LunarToSolar(const LunarDate& date) const
{
    uint32_t days = date.day - 1; // calculate days from 1900.1.1 to this date
    if (date.isLeapMonth) {
        days += LunarCalculator::GetLunarMonthDays(date.year, date.month);
    } else {
        uint32_t leapMonth = LunarCalculator::GetLunarLeapMonth(date.year);
        if (leapMonth < date.month) {
            days += LunarCalculator::GetLunarLeapDays(date.year);
        }
    }
    for (uint32_t month = 1; month < date.month; ++month) { // month start from 1
        days += LunarCalculator::GetLunarMonthDays(date.year, month);
    }
    for (uint32_t year = 1900; year < date.year; ++year) { // year start from 1900
        days += LunarCalculator::GetLunarYearDays(year);
    }
    days += 30; // days from solar's 1900.1.1 to lunar's 1900.1.1 is 30
    PickerDate result;
    result.FromDays(days);
    return result;
}

bool PickerDateComponent::GetLunarLeapMonth(uint32_t year, uint32_t& outLeapMonth) const
{
    auto leapMonth = LunarCalculator::GetLunarLeapMonth(year);
    if (leapMonth <= 0) {
        return false;
    }

    outLeapMonth = static_cast<uint32_t>(leapMonth);
    return true;
}

uint32_t PickerDateComponent::GetLunarMaxDay(uint32_t year, uint32_t month, bool isLeap) const
{
    if (isLeap) {
        return static_cast<uint32_t>(LunarCalculator::GetLunarLeapDays(year));
    } else {
        return static_cast<uint32_t>(LunarCalculator::GetLunarMonthDays(year, month));
    }
}

void PickerDateComponent::SolarColumnsBuilding(const PickerDate& current)
{
    auto yearColumn = GetColumn(PICKER_YEAR_COLUMN);
    auto monthColumn = GetColumn(PICKER_MONTH_COLUMN);
    auto dayColumn = GetColumn(PICKER_DAY_COLUMN);
    if (!yearColumn || !monthColumn || !dayColumn) {
        LOGE("year or month or day column is null.");
        return;
    }

    auto startYear = startDateSolar_.GetYear();
    auto endYear = endDateSolar_.GetYear();
    auto startMonth = startDateSolar_.GetMonth();
    auto endMonth = endDateSolar_.GetMonth();
    auto startDay = startDateSolar_.GetDay();
    auto endDay = endDateSolar_.GetDay();
    if (startYear > endYear) {
        return;
    }
    if (startYear == endYear && startMonth > endMonth) {
        return;
    }
    if (startYear == endYear && startMonth == endMonth && startDay > endDay) {
        return;
    }
    uint32_t maxDay = PickerDate::GetMaxDay(current.GetYear(), current.GetMonth());
    if (startYear < endYear) {
        startMonth = 1;
        endMonth = 12;
        startDay = 1;
        endDay = maxDay;
    }
    if (startYear == endYear && startMonth < endMonth) {
        startDay = 1;
        endDay = maxDay;
    }

    yearColumn->ClearOption();
    for (uint32_t year = startYear; year <= endYear; ++year) {
        if (year == current.GetYear()) {
            yearColumn->SetCurrentIndex(yearColumn->GetOptionCount());
        }
        yearColumn->AppendOption(GetYearFormatString(year));
    }

    monthColumn->ClearOption();
    // solar's month start form 1 to 12
    for (uint32_t month = startMonth; month <= endMonth; month++) {
        if (month == current.GetMonth()) {
            monthColumn->SetCurrentIndex(monthColumn->GetOptionCount());
        }
        monthColumn->AppendOption(GetMonthFormatString(month, false, false));
    }

    dayColumn->ClearOption();

    // solar's day start from 1
    for (uint32_t day = startDay; day <= endDay; day++) {
        if (day == current.GetDay()) {
            dayColumn->SetCurrentIndex(dayColumn->GetOptionCount());
        }
        dayColumn->AppendOption(GetDayFormatString(day, false));
    }

    lunar_ = false;
}

void PickerDateComponent::LunarColumnsBuilding(const LunarDate& current)
{
    auto yearColumn = GetColumn(PICKER_YEAR_COLUMN);
    auto monthColumn = GetColumn(PICKER_MONTH_COLUMN);
    auto dayColumn = GetColumn(PICKER_DAY_COLUMN);
    if (!yearColumn || !monthColumn || !dayColumn) {
        LOGE("year or month or day column is null.");
        return;
    }

    auto startYear = startDateLunar_.year;
    auto endYear = endDateLunar_.year;
    auto startMonth = startDateLunar_.month;
    auto endMonth = endDateLunar_.month;
    auto startDay = startDateLunar_.day;
    auto endDay = endDateLunar_.day;
    if (startYear > endYear) {
        return;
    }
    if (startYear == endYear && startMonth > endMonth) {
        return;
    }
    if (startYear == endYear && startMonth == endMonth && startDay > endDay) {
        return;
    }
    uint32_t maxDay = GetLunarMaxDay(current.year, current.month, current.isLeapMonth);
    if (startYear < endYear) {
        startMonth = 1;
        endMonth = 12;
        startDay = 1;
        endDay = maxDay;
    }
    if (startYear == endYear && startMonth < endMonth) {
        startDay = 1;
        endDay = maxDay;
    }

    yearColumn->ClearOption();
    for (uint32_t index = startYear; index <= endYear; ++index) {
        if (current.year == index) {
            yearColumn->SetCurrentIndex(yearColumn->GetOptionCount());
        }
        yearColumn->AppendOption(GetYearFormatString(index));
    }

    uint32_t lunarLeapMonth = 0;
    bool hasLeapMonth = GetLunarLeapMonth(current.year, lunarLeapMonth);
    monthColumn->ClearOption();
    // lunar's month start form startMonth to endMonth
    for (uint32_t index = startMonth; index <= endMonth; ++index) {
        if (!current.isLeapMonth && current.month == index) {
            monthColumn->SetCurrentIndex(monthColumn->GetOptionCount());
        }
        monthColumn->AppendOption(GetMonthFormatString(index, true, false));
        if (hasLeapMonth && lunarLeapMonth == index) {
            if (current.isLeapMonth && current.month == index) {
                monthColumn->SetCurrentIndex(monthColumn->GetOptionCount());
            }
            monthColumn->AppendOption(GetMonthFormatString(index, true, true));
        }
    }

    
    dayColumn->ClearOption();
    // lunar's day start from startDay
    for (uint32_t index = startDay; index <= endDay; ++index) {
        if (current.day == index) {
            dayColumn->SetCurrentIndex(dayColumn->GetOptionCount());
        }
        dayColumn->AppendOption(GetDayFormatString(index, true));
    }

    lunar_ = true;
}

void PickerDateComponent::AdjustSolarDate(PickerDate& date) const
{
    AdjustSolarDate(date, startDateSolar_, endDateSolar_);
}

void PickerDateComponent::AdjustSolarDate(PickerDate& date, const PickerDate& start, const PickerDate& end) const
{
    if (SolarDateCompare(date, start) < 0) {
        date = start;
        return;
    }
    if (SolarDateCompare(date, end) > 0) {
        date = end;
    }
}

void PickerDateComponent::AdjustLunarDate(LunarDate& date) const
{
    if (LunarDateCompare(date, startDateLunar_) < 0) {
        date = startDateLunar_;
        return;
    }
    if (LunarDateCompare(date, endDateLunar_) > 0) {
        date = endDateLunar_;
    }
}

int PickerDateComponent::SolarDateCompare(const PickerDate& left, const PickerDate& right) const
{
    static const int leftEqualRight = 0; // means left = right
    static const int leftGreatRight = 1; // means left > right
    static const int leftLessRight = -1; // means left < right
    if (left.GetYear() > right.GetYear()) {
        return leftGreatRight;
    }
    if (left.GetYear() < right.GetYear()) {
        return leftLessRight;
    }
    if (left.GetMonth() > right.GetMonth()) {
        return leftGreatRight;
    }
    if (left.GetMonth() < right.GetMonth()) {
        return leftLessRight;
    }
    if (left.GetDay() > right.GetDay()) {
        return leftGreatRight;
    }
    if (left.GetDay() < right.GetDay()) {
        return leftLessRight;
    }
    return leftEqualRight;
}

int PickerDateComponent::LunarDateCompare(const LunarDate& left, const LunarDate& right) const
{
    static const int leftEqualRight = 0; // means left = right
    static const int leftGreatRight = 1; // means left > right
    static const int leftLessRight = -1; // means left < right
    static const double addingValue = 0.5; // adding value for leap month.
    if (left.year > right.year) {
        return leftGreatRight;
    }
    if (left.year < right.year) {
        return leftLessRight;
    }
    double leftMonth = (left.isLeapMonth ? left.month + addingValue : left.month);
    double rightMonth = (right.isLeapMonth ? right.month + addingValue : right.month);
    if (GreatNotEqual(leftMonth, rightMonth)) {
        return leftGreatRight;
    }
    if (LessNotEqual(leftMonth, rightMonth)) {
        return leftLessRight;
    }
    if (left.day > right.day) {
        return leftGreatRight;
    }
    if (left.day < right.day) {
        return leftLessRight;
    }
    return leftEqualRight;
}

} // namespace OHOS::Ace
