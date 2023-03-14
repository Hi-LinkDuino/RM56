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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_DATETIME_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_DATETIME_COMPONENT_H

#include "core/components/picker/picker_date_component.h"
#include "core/components/picker/picker_time_component.h"

namespace OHOS::Ace {

class PickerDateTimeComponent : public PickerTimeComponent {
    DECLARE_ACE_TYPE(PickerDateTimeComponent, PickerTimeComponent);

public:
    PickerDateTimeComponent();
    ~PickerDateTimeComponent() override = default;

    void SetSelectedDateTime(const PickerDateTime& value)
    {
        selectedDate_ = value.GetDate();
        SetSelectedTime(value.GetTime());
    }

    void SetShowLunar(bool value)
    {
        lunar_ = value;
    }

    bool SubsidiaryShowed() const override
    {
        if (!datePicker_) {
            return false;
        }
        return datePicker_->IsDialogShowed();
    }

    bool IsShowLunar() const override
    {
        return lunar_;
    }

    void OnTitleBuilding() override;

    void OnColumnsBuilding() override;

    void OnSelectedSaving() override;

    std::string GetSelectedObject(bool isColumnChange,
        const std::string& changeColumnTag, int32_t status = -1) const override;

    void OnLunarCallback(bool checked, std::vector<std::string>& resultTags) override;

    void OnTriangleCallback(bool value) override;

    void OnColumnsCreating() override;

    void OnDataLinking(const std::string& tag, bool isAdd, uint32_t index,
        std::vector<std::string>& resultTags) override;

private:
    std::string GetMonthDayFormatString(bool lunar, uint32_t days) const;

    void FillSolarLunarDays(bool lunar, const PickerDate& currentDate);

    void InitDatePicker();
    void ShowDatePicker();
    void HideDatePicker();
    void OnSubsidiaryChange(const PickerDate& date);

    RefPtr<PickerDateComponent> datePicker_;
    PickerDate selectedDate_ = PickerDate::Current();
    PickerDate currentDate_ = PickerDate::Current();
    bool lunar_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_DATETIME_COMPONENT_H
