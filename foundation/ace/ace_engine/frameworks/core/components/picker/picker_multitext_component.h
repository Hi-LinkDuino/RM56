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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_MULTITEXT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_MULTITEXT_COMPONENT_H

#include "core/components/picker/picker_base_component.h"

namespace OHOS::Ace {

class PickerMultiTextComponent : public PickerBaseComponent {
    DECLARE_ACE_TYPE(PickerMultiTextComponent, PickerBaseComponent);

public:
    PickerMultiTextComponent() = default;
    ~PickerMultiTextComponent() override = default;

    const std::vector<uint32_t>& GetSelected() const
    {
        return selectedIndexes_;
    }
    void SetSelected(const std::vector<uint32_t>& value)
    {
        if (IsEqualSelected(value)) {
            return;
        }
        selectedIndexes_ = value;
        HandleSelectedChange();
    }

    const std::vector<std::vector<std::string>>& GetRange() const
    {
        return range_;
    }
    void SetRange(const std::vector<std::vector<std::string>>& value)
    {
        if (IsEqualRange(value)) {
            return;
        }
        range_ = value;
        HandleRangeChange();
    }

    uint32_t GetColumnCount() const override
    {
        return columnCount_;
    }
    void SetColumnCount(uint32_t value)
    {
        if (hasCreateColumn_) {
            return;
        }
        columnCount_ = value;
    }

    void OnTitleBuilding() override;

    void OnColumnsBuilding() override;

    std::string GetSelectedObject(bool isColumnChange,
        const std::string& changeColumnTag, int32_t status = -1) const override;

    void OnSelectedSaving() override;

    void OnDataLinking(const std::string& tag, bool isAdd, uint32_t index,
        std::vector<std::string>& resultTags) override;

    void OnColumnsCreating() override;

private:
    void HandleRangeChange();

    void HandleSelectedChange();

    bool IsEqualSelected(const std::vector<uint32_t>& value) const;

    bool IsEqualRange(const std::vector<std::vector<std::string>>& value) const;

    bool IsEqualStrings(const std::vector<std::string>& first, const std::vector<std::string>& second) const;

    std::vector<uint32_t> selectedIndexes_;
    std::vector<std::string> selectedValues_;
    std::vector<std::vector<std::string>> range_;

    uint32_t columnCount_ = 0;
    bool hasCreateColumn_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_MULTITEXT_COMPONENT_H
