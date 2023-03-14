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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_GRID_CONTAINER_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_GRID_CONTAINER_INFO_H

#include "base/geometry/dimension.h"
#include "core/components/common/layout/grid_layout_info.h"

namespace OHOS::Ace {

constexpr int32_t UNDEFINED_INT = -1;
constexpr Dimension UNDEFINED_DIMENSION(-1.0, DimensionUnit::VP);

class GridContainerInfo : public GridLayoutInfo {
    DECLARE_ACE_TYPE(GridContainerInfo, GridLayoutInfo);

public:
    class Builder {
    public:
        Builder()
        {
            containerInfo_ = AceType::Claim(new GridContainerInfo());
        }
        void SetColumns(int32_t columns)
        {
            if (columns > 0) {
                containerInfo_->columns_ = columns;
            }
        }
        void SetGutterWidth(const Dimension& gutterWidth)
        {
            if (GreatOrEqual(gutterWidth.Value(), 0.0)) {
                containerInfo_->gutterWidth_ = gutterWidth;
            }
        }
        void SetMarginLeft(const Dimension& marginLeft)
        {
            if (GreatOrEqual(marginLeft.Value(), 0.0)) {
                containerInfo_->marginLeft_ = marginLeft;
            }
        }
        void SetMarginRight(const Dimension& marginRight)
        {
            if (GreatOrEqual(marginRight.Value(), 0.0)) {
                containerInfo_->marginRight_ = marginRight;
            }
        }
        void SetPaddingLeft(const Dimension& paddingLeft)
        {
            if (GreatOrEqual(paddingLeft.Value(), 0.0)) {
                containerInfo_->paddingLeft_ = paddingLeft;
            }
        }
        void SetPaddingRight(const Dimension& paddingRight)
        {
            if (GreatOrEqual(paddingRight.Value(), 0.0)) {
                containerInfo_->paddingRight_ = paddingRight;
            }
        }
        void SetSizeType(const GridSizeType& sizeType)
        {
            containerInfo_->sizeType_ = sizeType;
        }
        void SetColumnType(const GridColumnType& columnType)
        {
            containerInfo_->columnType_ = columnType;
        }
        void SetGridTemplateType(const GridTemplateType& templateType)
        {
            containerInfo_->templateType_ = templateType;
        }
        const RefPtr<GridContainerInfo>& Build() const
        {
            return containerInfo_;
        }

    private:
        RefPtr<GridContainerInfo> containerInfo_;
    };
    ~GridContainerInfo() override = default;

    double GetColumnWidth() const
    {
        return columnWidth_;
    }

    GridColumnType GetColumnType() const
    {
        return columnType_;
    }
    GridSizeType GetSizeType() const;
    int32_t GetColumns() const;
    Dimension ACE_EXPORT GetGutterWidth() const;
    Dimension GetMarginLeft() const;
    Dimension GetMarginRight() const;

    /*
     * Use system screen width build column width.
     */
    void ACE_EXPORT BuildColumnWidth();
    void BuildColumnWidth(double width);

private:
    GridContainerInfo() = default;
    GridTemplateType templateType_ = GridTemplateType::NORMAL;
    // current used size type
    GridSizeType currentSizeType_ = GridSizeType::UNDEFINED;
    GridSizeType sizeType_ = GridSizeType::UNDEFINED;
    // container total column number
    int32_t columns_ = UNDEFINED_INT;
    Dimension gutterWidth_ = UNDEFINED_DIMENSION;
    Dimension marginLeft_ = UNDEFINED_DIMENSION;
    Dimension marginRight_ = UNDEFINED_DIMENSION;
    Dimension paddingLeft_ = UNDEFINED_DIMENSION;
    Dimension paddingRight_ = UNDEFINED_DIMENSION;

    double containerWidth_ = 0.0;
    double columnWidth_ = 0.0;
    GridColumnType columnType_ = GridColumnType::NONE;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_GRID_CONTAINER_INFO_H
