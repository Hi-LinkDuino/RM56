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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_DATE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_DATE_COMPONENT_H

#include "base/i18n/localization.h"
#include "core/components/picker/picker_base_component.h"

namespace OHOS::Ace {

class LunarCalculator {
public:
    static uint32_t GetLunarLeapMonth(uint32_t lunarYear)
    {
        if (lunarYear >= YEAR_START + LUNAR_INFO_SIZE) {
            return 0;
        }
        uint32_t leapMonth = LUNAR_INFO[lunarYear - YEAR_START] & 0xf; // use 0xf to get leap month info
        return leapMonth == 0xf ? 0 : leapMonth;
    }

    static uint32_t GetLunarLeapDays(uint32_t lunarYear)
    {
        if (lunarYear >= YEAR_START - 1 + LUNAR_INFO_SIZE) {
            return 0;
        }
        return GetLunarLeapMonth(lunarYear) > 0 ? ((LUNAR_INFO[lunarYear - YEAR_START + 1] & 0xf) == 0xf ? 30 : 29)
                                                : 0; // big month 30 days other 29
    }

    static uint32_t GetLunarYearDays(uint32_t lunarYear)
    {
        if (lunarYear >= YEAR_START + LUNAR_INFO_SIZE) {
            return 0;
        }
        uint32_t totalDays = 348;                          // lunar year has (12 * 29 =) 348 days at least
        for (uint32_t i = 0x8000; i > 0x8; i >>= 1) { // get month info from bit of LUNAR_INFO
            totalDays += ((LUNAR_INFO[lunarYear - YEAR_START] & i) != 0) ? 1 : 0;
        }

        return totalDays + GetLunarLeapDays(lunarYear);
    }

    static uint32_t GetLunarMonthDays(uint32_t lunarYear, uint32_t lunarMonth)
    {
        if (lunarYear >= YEAR_START + LUNAR_INFO_SIZE) {
            return 0;
        }
        uint32_t month = static_cast<uint32_t>(lunarMonth);
        // big month 30 days other 29
        return ((LUNAR_INFO[lunarYear - YEAR_START] & (0x10000u >> month)) != 0) ? 30 : 29;
    }

private:
    static constexpr uint32_t YEAR_START = 1897; // start year reference with LUNAR_INFO
    static constexpr int32_t LUNAR_INFO_SIZE = 207;
    static const uint16_t LUNAR_INFO[];
};

class PickerStringFormatter {
public:
    static const std::string& GetYear(uint32_t year);

    static const std::string& GetSolarMonth(uint32_t month);

    static const std::string& GetSolarDay(uint32_t day);

    static const std::string& GetLunarMonth(uint32_t month, bool isLeap);

    static const std::string& GetLunarDay(uint32_t day);

    static const std::vector<std::string>& GetTagOrder();

private:
    static void Init();

    static bool inited_;

    static const std::string empty_;

    static std::vector<std::string> years_; // year from 1900 to 2100,count is 201

    static std::vector<std::string> solarMonths_; // solar month from 1 to 12,count is 12
    static std::vector<std::string> solarDays_; // solar day from 1 to 31, count is 31

    static std::vector<std::string> lunarMonths_; // lunar month from 1 to 24, count is 24
    static std::vector<std::string> lunarDays_; // lunar day from 1 to 30, count is 30
    static std::vector<std::string> tagOrder_; // year month day tag order
};

class ACE_EXPORT PickerDateComponent : public PickerBaseComponent {
    DECLARE_ACE_TYPE(PickerDateComponent, PickerBaseComponent);

public:
    PickerDateComponent();

    ~PickerDateComponent() override = default;

    const PickerDate& GetStartDate() const
    {
        return startDateSolar_;
    }
    void SetStartDate(const PickerDate& value)
    {
        startDateSolar_ = value;
        AdjustSolarDate(startDateSolar_, limitStartDate_, limitEndDate_);
        startDateLunar_ = SolarToLunar(startDateSolar_);
    }

    const PickerDate& GetEndDate() const
    {
        return endDateSolar_;
    }
    void SetEndDate(const PickerDate& value)
    {
        endDateSolar_ = value;
        AdjustSolarDate(endDateSolar_, limitStartDate_, limitEndDate_);
        endDateLunar_ = SolarToLunar(endDateSolar_);
    }

    const PickerDate& GetSelectedDate() const
    {
        return selectedDate_;
    }
    void SetSelectedDate(const PickerDate& value)
    {
        selectedDate_ = value;
    }

    void SetOnDateChange(const std::function<void(const PickerDate&)>& value)
    {
        onDateChange_ = value;
    }

    void SetShowLunar(bool value)
    {
        lunar_ = value;
    }

    bool NeedRtlColumnOrder() const override
    {
        return true;
    }

    bool IsShowLunar() const override
    {
        return lunar_;
    }

    void OnTitleBuilding() override;

    void OnColumnsBuilding() override;

    void OnSelectedSaving() override;

    std::string GetSelectedObject(bool isColumnChange,
        const std::string& changeColumnTag, int status = -1) const override;

    void OnDataLinking(
        const std::string& tag, bool isAdd, uint32_t index, std::vector<std::string>& resultTags) override;

    void OnLunarCallback(bool checked, std::vector<std::string>& resultTags) override;

    void OnAnimationPlaying() override;

private:
    PickerDate GetCurrentDate() const;
    LunarDate GetCurrentLunarDate(uint32_t lunarYear) const;

    void HandleYearChange(bool isAdd, uint32_t index, std::vector<std::string>& resultTags);

    void HandleLunarYearChange(bool isAdd, uint32_t index);

    void HandleSolarYearChange(bool isAdd, uint32_t index);

    void HandleMonthChange(bool isAdd, uint32_t index, std::vector<std::string>& resultTags);

    void HandleLunarMonthChange(bool isAdd, uint32_t index);

    void HandleSolarMonthChange(bool isAdd, uint32_t index);

    void HandleDayChange(bool isAdd, uint32_t index, std::vector<std::string>& resultTags);

    void HandleSolarDayChange(bool isAdd, uint32_t index);

    void HandleLunarDayChange(bool isAdd, uint32_t index);

    void HandleAddLunarDayChange(uint32_t index);

    void HandleReduceLunarDayChange(uint32_t index);

    std::string GetYearFormatString(uint32_t year) const;

    std::string GetMonthFormatString(uint32_t month, bool isLunar, bool isLeap) const;

    std::string GetDayFormatString(uint32_t day, bool isLunar) const;

    LunarDate SolarToLunar(const PickerDate& date) const;
    PickerDate LunarToSolar(const LunarDate& date) const;

    bool GetLunarLeapMonth(uint32_t year, uint32_t& outLeapMonth) const;

    uint32_t GetLunarMaxDay(uint32_t year, uint32_t month, bool isLeap) const;

    void SolarColumnsBuilding(const PickerDate& current);

    void LunarColumnsBuilding(const LunarDate& current);

    int SolarDateCompare(const PickerDate& left, const PickerDate& right) const;

    int LunarDateCompare(const LunarDate& left, const LunarDate& right) const;

    void AdjustSolarDate(PickerDate& date) const;
    void AdjustSolarDate(PickerDate& date, const PickerDate& start, const PickerDate& end) const;

    void AdjustLunarDate(LunarDate& date) const;

    bool lunar_ = false;
    PickerDate startDateSolar_ = PickerDate(1970, 1, 1); // default start date is 1970-1-1 from FA document.
    LunarDate startDateLunar_;
    PickerDate endDateSolar_ = PickerDate(2100, 12, 31); // default end date is 2100-12-31 from FA document.
    LunarDate endDateLunar_;
    PickerDate selectedDate_ = PickerDate::Current();
    std::function<void(const PickerDate&)> onDateChange_;

    static const PickerDate limitStartDate_;
    static const PickerDate limitEndDate_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_DATE_COMPONENT_H
