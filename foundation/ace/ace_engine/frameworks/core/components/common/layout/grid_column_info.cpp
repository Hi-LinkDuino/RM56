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

#include "core/components/common/layout/grid_column_info.h"

#include "base/log/log.h"
#include "core/components/common/layout/grid_system_manager.h"

namespace {
constexpr int32_t INVALID_OFFSET = -1;
}

namespace OHOS::Ace {

/* set offset by grid column number */
void GridColumnInfo::Builder::SetOffset(int32_t offset, GridSizeType type)
{
    if (!columnInfo_->hasColumnOffset_) {
        for (uint8_t i = 0; i < sizeof(columnInfo_->offsets_) / sizeof(int32_t); i++) {
            columnInfo_->offsets_[i] = INVALID_OFFSET;
        }
        columnInfo_->hasColumnOffset_ = true;
    }

    const int32_t arraySize = static_cast<const int32_t>(GridSizeType::XL);
    int32_t typeVal = static_cast<int32_t>(type);
    if (typeVal < arraySize) {
        columnInfo_->offsets_[typeVal] = offset;
    }
}

double GridColumnInfo::GetWidth() const
{
    if (!parent_) {
        LOGE("no parent info");
        return 0.0;
    }

    uint32_t columns = 0;
    auto sizeType = parent_->GetSizeType();
    switch (sizeType) {
        case GridSizeType::XS:
            columns = xsSizeColumn_ > 0 ? xsSizeColumn_ : columns_;
            break;
        case GridSizeType::SM:
            columns = smSizeColumn_ > 0 ? smSizeColumn_ : columns_;
            break;
        case GridSizeType::MD:
            columns = mdSizeColumn_ > 0 ? mdSizeColumn_ : columns_;
            break;
        case GridSizeType::LG:
            columns = lgSizeColumn_ > 0 ? lgSizeColumn_ : columns_;
            break;
        case GridSizeType::XL:
            columns = lgSizeColumn_ > 0 ? lgSizeColumn_ : columns_;
            break;
        default:
            break;
    }
    return (columns == 0) ? 0.0 : GetWidth(columns);
}

double GridColumnInfo::GetWidth(uint32_t columns) const
{
    if (!parent_) {
        LOGE("no parent info");
        return 0.0;
    }
    double dipScale = GridSystemManager::GetInstance().GetDipScale();
    return columns == 0 ? 0.0
                        : (columns * parent_->GetColumnWidth()) +
                              ((columns - 1) * parent_->GetGutterWidth().ConvertToPx(dipScale));
}

double GridColumnInfo::GetMaxWidth() const
{
    if (!parent_) {
        LOGE("no parent info");
        return 0.0;
    }

    uint32_t columns = 0;
    auto sizeType = parent_->GetSizeType();
    switch (sizeType) {
        case GridSizeType::XS:
            columns = xsSizeColumn_ > 0 ? xsSizeColumn_ : columns_;
            break;
        case GridSizeType::SM:
            columns = smSizeMaxColumn_ > 0 ? smSizeMaxColumn_ : smSizeColumn_ > 0 ? smSizeColumn_ : columns_;
            break;
        case GridSizeType::MD:
            columns = mdSizeMaxColumn_ > 0 ? mdSizeMaxColumn_ : mdSizeColumn_ > 0 ? mdSizeColumn_ : columns_;
            break;
        case GridSizeType::LG:
            columns = lgSizeMaxColumn_ > 0 ? lgSizeMaxColumn_ : lgSizeColumn_ > 0 ? lgSizeColumn_ : columns_;
            break;
        default:
            break;
    }

    return GetWidth(columns);
}

Dimension GridColumnInfo::GetOffset() const
{
    if (!parent_) {
        LOGE("no parent info");
        return UNDEFINED_DIMENSION;
    }

    /* ace1.0 obsolete logic since 6 */
    if (!hasColumnOffset_) {
        Dimension dim = UNDEFINED_DIMENSION;
        switch (parent_->GetSizeType()) {
            case GridSizeType::XS:
                dim = xsSizeOffset_;
                break;
            case GridSizeType::SM:
                dim = smSizeOffset_;
                break;
            case GridSizeType::MD:
                dim = mdSizeOffset_;
                break;
            case GridSizeType::LG:
                dim = lgSizeOffset_;
                break;
            default:
                break;
        }
        return dim;
    }

    /* ace2.0 */
    int32_t sizeType = static_cast<int32_t>(parent_->GetSizeType());
    int32_t offset = INVALID_OFFSET;
    if (sizeType < static_cast<int32_t>(GridSizeType::XL)) {
        offset = offsets_[sizeType];
    }

    if (offset == INVALID_OFFSET) {
        offset = offsets_[static_cast<int32_t>(GridSizeType::UNDEFINED)]; // use common offset
    }
    if (offset == INVALID_OFFSET) {
        return UNDEFINED_DIMENSION;
    }
    double dipScale = GridSystemManager::GetInstance().GetDipScale();
    double offsetVp = offset * (parent_->GetColumnWidth() + parent_->GetGutterWidth().ConvertToPx(dipScale));
    return Dimension(offsetVp, DimensionUnit::PX);
}

} // namespace OHOS::Ace
