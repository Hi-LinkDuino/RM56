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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_TIME_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_TIME_COMPONENT_H

#include "base/i18n/localization.h"
#include "core/components/picker/picker_base_component.h"

namespace OHOS::Ace {

class ACE_EXPORT PickerTimeComponent : public PickerBaseComponent {
    DECLARE_ACE_TYPE(PickerTimeComponent, PickerBaseComponent);

public:
    PickerTimeComponent();

    ~PickerTimeComponent() override = default;

    const PickerTime& GetSelectedTime() const
    {
        return selectedTime_;
    }
    void SetSelectedTime(const PickerTime& value)
    {
        selectedTime_ = value;
    }

    bool GetHasSecond() const
    {
        return hasSecond_;
    }
    void SetHasSecond(bool value)
    {
        hasSecond_ = value;
    }

    bool GetHour24() const
    {
        return hour24_;
    }
    void SetHour24(bool value)
    {
        hour24_ = value;
    }

    void OnTitleBuilding() override;

    void OnColumnsBuilding() override;

    std::string GetSelectedObject(bool isColumnChange,
        const std::string& changeColumnTag, int32_t status = -1) const override;

    void OnDataLinking(const std::string& tag, bool isAdd, uint32_t index,
        std::vector<std::string>& resultTags) override;

    void OnSelectedSaving() override;

    void OnColumnsCreating() override;

protected:
    PickerTime GetCurrentTime() const;

    void HandleHour24Change(bool isAdd, uint32_t index, std::vector<std::string>& resultTags);

    void HandleHour12Change(bool isAdd, uint32_t index, std::vector<std::string>& resultTags);

    bool IsAmHour(uint32_t hourOf24) const;

    uint32_t GetAmPmHour(uint32_t hourOf24) const;

    uint32_t GetHourFromAmPm(bool isAm, uint32_t amPmhour) const;

    std::string GetAmFormatString() const;

    std::string GetPmFormatString() const;

    void HandleHourColumnBuilding();

    std::string AddZeroPrefix(const std::string& value) const;

    std::string GetHourFormatString(uint32_t hour) const;

    std::string GetMinuteFormatString(uint32_t minute) const;

    std::string GetSecondFormatString(uint32_t second) const;

private:
    PickerTime selectedTime_ = PickerTime::Current();
    bool hasSecond_ = false;
    // true, use 24 hours style; false, use 12 hours style.
    bool hour24_ = !Localization::GetInstance()->IsAmPmHour();
    std::vector<std::string> vecAmPm_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_TIME_COMPONENT_H
