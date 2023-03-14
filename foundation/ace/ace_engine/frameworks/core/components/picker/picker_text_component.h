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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_TEXT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_TEXT_COMPONENT_H

#include "core/components/picker/picker_base_component.h"

namespace OHOS::Ace {

class ACE_EXPORT PickerTextComponent : public PickerBaseComponent {
    DECLARE_ACE_TYPE(PickerTextComponent, PickerBaseComponent);

public:
    PickerTextComponent();
    ~PickerTextComponent() override = default;

    uint32_t GetSelected() const
    {
        return selectedIndex_;
    }
    void SetSelected(uint32_t value)
    {
        selectedIndex_ = value;
        HandleSelectedChange();
    }

    const std::vector<std::string>& GetRange() const
    {
        return range_;
    }
    void SetRange(const std::vector<std::string>& value)
    {
        if (value.empty()) {
            LOGE("input value of range is empty.");
            return;
        }
        range_ = value;
        HandleRangeChange();
    }

    void SetPrefix(const std::string& value)
    {
        auto column = GetColumn(PICKER_TEXT_COLUMN);
        if (!column) {
            LOGE("can not get column of text");
            return;
        }

        column->SetPrefix(value);
    }

    void SetSuffix(const std::string& value)
    {
        auto column = GetColumn(PICKER_TEXT_COLUMN);
        if (!column) {
            LOGE("can not get column of text");
            return;
        }

        column->SetSuffix(value);
    }

    void SetDefaultPickerItemHeight(const Dimension& value)
    {
        auto column = GetColumn(PICKER_TEXT_COLUMN);
        if (!column) {
            LOGE("can not get column of text");
            return;
        }

        column->SetFixHeight(value);
    }

    void OnTitleBuilding() override;

    void OnColumnsBuilding() override;

    std::string GetSelectedObject(bool isColumnChange,
        const std::string& changeColumnTag, int32_t status = -1) const override;

    void OnSelectedSaving() override;

    void OnDataLinking(const std::string& tag, bool isAdd, uint32_t index,
        std::vector<std::string>& resultTags) override
    {
        resultTags.emplace_back(PICKER_TEXT_COLUMN);
    }

private:
    void HandleSelectedChange();
    void HandleRangeChange();

    uint32_t selectedIndex_ = 0;
    std::string selectedValue_;
    std::vector<std::string> range_ {""};
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_TEXT_COMPONENT_H
