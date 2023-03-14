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

#include "core/components/picker/picker_multitext_component.h"

namespace OHOS::Ace {

void PickerMultiTextComponent::OnTitleBuilding()
{
    PickerBaseComponent::OnTitleBuilding();
    SetHasTitle(false);
}

void PickerMultiTextComponent::OnColumnsBuilding()
{
    for (uint32_t index = 0; index < columnCount_; ++index) {
        if (index >= range_.size()) {
            LOGW("range size is smaller than column count");
            auto emptyColumn = GetColumn(std::to_string(index));
            if (emptyColumn) {
                emptyColumn->ClearOption();
                emptyColumn->AppendOption("");
                emptyColumn->SetCurrentIndex(0);
            } else {
                LOGE("the column of index[%{private}u] not exists.", index);
            }
            continue;
        }
        const auto& textRange = range_[index];
        auto column = GetColumn(std::to_string(index));
        if (!column) {
            LOGE("the column of index[%{private}u] not exists.", index);
            continue;
        }
        column->ClearOption();
        for (uint32_t subIndex = 0; subIndex < textRange.size(); ++subIndex) {
            column->AppendOption(textRange[subIndex]);
        }
        if (index < selectedIndexes_.size()) {
            selectedIndexes_[index] = textRange.empty() ? 0 : selectedIndexes_[index] % textRange.size();
            column->SetCurrentIndex(selectedIndexes_[index]);
        } else {
            LOGE("array size of selected is not equal range.");
        }
    }
}

std::string PickerMultiTextComponent::GetSelectedObject(bool isColumnChange,
    const std::string& changeColumnTag, int32_t status) const
{
    auto container = Container::Current();
    if (!container) {
        return "";
    }
    auto context = container->GetPipelineContext();
    if (!context) {
        return "";
    }

    if (isColumnChange) {
        auto column = GetColumn(changeColumnTag);
        if (!column) {
            LOGE("can not get the changed column.");
            return "";
        }
        if (context->GetIsDeclarative()) {
            return std::string("{\"column\":") + changeColumnTag + ",\"value\":" + "\"" +
                column->GetCurrentText() + "\"" + ",\"index\":" + std::to_string(column->GetCurrentIndex()) + "}";
        } else {
            return std::string("{\"column\":") + changeColumnTag + ",\"newValue\":" + "\"" +
                column->GetCurrentText() + "\"" + ",\"newSelected\":" + std::to_string(column->GetCurrentIndex()) + "}";
        }
    }

    std::string result = "{\"newValue\":[";
    if (context->GetIsDeclarative()) {
        result = "{\"value\":[";
    }
    for (uint32_t index = 0; index < selectedValues_.size(); ++index) {
        if (index == 0) {
            result.append("\"");
        } else {
            result.append(",\"");
        }
        result.append(selectedValues_[index]);
        result.append("\"");
    }
    if (context->GetIsDeclarative()) {
        result.append("],\"index\":[");
    } else {
        result.append("],\"newSelected\":[");
    }
    for (uint32_t index = 0; index < selectedIndexes_.size(); ++index) {
        if (index != 0) {
            result.append(",");
        }
        result.append(std::to_string(selectedIndexes_[index]));
    }
    result.append("],\"status\":").append(std::to_string(status)).append("}");
    return result;
}

void PickerMultiTextComponent::OnSelectedSaving()
{
    selectedIndexes_.clear();
    selectedValues_.clear();
    for (uint32_t index = 0; index < columnCount_; ++index) {
        selectedIndexes_.emplace_back(0);
        selectedValues_.emplace_back("");
        auto column = GetColumn(std::to_string(index));
        if (!column) {
            LOGE("can not get column by index[%{private}u].", index);
            continue;
        }
        selectedIndexes_[index] = column->GetCurrentIndex();
        selectedValues_[index] = column->GetCurrentText();
    }
}

void PickerMultiTextComponent::OnDataLinking(const std::string& tag, bool isAdd, uint32_t index,
    std::vector<std::string>& resultTags)
{
    for (uint32_t val = 0; val < columnCount_; ++val) {
        resultTags.emplace_back(std::to_string(val));
    }
}

void PickerMultiTextComponent::OnColumnsCreating()
{
    if (hasCreateColumn_) {
        return;
    }

    if (!columnCount_) {
        if (range_.empty()) {
            LOGE("did not set attribute of column count and initialize range is empty.");
        } else {
            columnCount_ = range_.size();
        }
    }

    ClearColumns();
    for (uint32_t index = 0; index < columnCount_; ++index) {
        auto column = AceType::MakeRefPtr<PickerColumnComponent>();
        column->SetColumnTag(std::to_string(index));
        column->AppendOption("");
        AppendColumn(column);
    }
    hasCreateColumn_ = true;
}

void PickerMultiTextComponent::HandleRangeChange()
{
    if (!GetIsDialog() || !IsDialogShowed()) {
        return;
    }

    auto column = GetColumn("0");
    if (!column) {
        LOGE("has no column now.");
        return;
    }

    auto backupIndexes = selectedIndexes_;
    auto backupValues = selectedValues_;
    OnSelectedSaving();
    OnColumnsBuilding();
    column->HandleChangeCallback(true, false);
    selectedIndexes_ = backupIndexes;
    selectedValues_ = backupValues;
}

void PickerMultiTextComponent::HandleSelectedChange()
{
    if (!GetIsDialog() || !IsDialogShowed()) {
        return;
    }

    auto column = GetColumn("0");
    if (!column) {
        LOGE("has no column now.");
        return;
    }

    OnColumnsBuilding();
    column->HandleChangeCallback(true, false);
}

bool PickerMultiTextComponent::IsEqualSelected(const std::vector<uint32_t>& value) const
{
    if (value.size() != selectedIndexes_.size()) {
        return false;
    }

    for (uint32_t index = 0; index < value.size(); ++index) {
        if (value[index] != selectedIndexes_[index]) {
            return false;
        }
    }

    return true;
}

bool PickerMultiTextComponent::IsEqualRange(const std::vector<std::vector<std::string>>& value) const
{
    if (value.size() != range_.size()) {
        return false;
    }

    for (uint32_t index = 0; index < value.size(); ++index) {
        if (!IsEqualStrings(value[index], range_[index])) {
            return false;
        }
    }

    return true;
}

bool PickerMultiTextComponent::IsEqualStrings(const std::vector<std::string>& first,
    const std::vector<std::string>& second) const
{
    if (first.size() != second.size()) {
        return false;
    }

    for (uint32_t index = 0; index < first.size(); ++index) {
        if (first[index] != second[index]) {
            return false;
        }
    }

    return true;
}

} // namespace OHOS::Ace
