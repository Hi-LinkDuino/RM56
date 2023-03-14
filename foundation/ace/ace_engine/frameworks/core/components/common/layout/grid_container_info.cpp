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

#include "core/components/common/layout/grid_container_info.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/grid_layout_info.h"
#include "core/components/common/layout/grid_system_manager.h"

namespace OHOS::Ace {

template<typename T>
const T& GetValue(const T& current, const T& system, const T& defaultValue)
{
    if (current == defaultValue) {
        return system;
    } else {
        return current;
    }
}

void GridContainerInfo::BuildColumnWidth()
{
    BuildColumnWidth(GridSystemManager::GetInstance().GetScreenWidth());
}

void GridContainerInfo::BuildColumnWidth(double width)
{
    SystemGridInfo systemGridInfo;
    if (sizeType_ != GridSizeType::UNDEFINED && currentSizeType_ == GridSizeType::UNDEFINED) {
        systemGridInfo = GridSystemManager::GetInstance().GetSystemGridInfo(sizeType_);
        // using fix size type
        currentSizeType_ = sizeType_;
    } else {
        systemGridInfo = GridSystemManager::GetInstance().GetSystemGridInfo(templateType_, width);
        if (currentSizeType_ != systemGridInfo.sizeType) {
            // system size changed
            currentSizeType_ = systemGridInfo.sizeType;
        } else {
            if (NearEqual(containerWidth_, width)) {
                LOGW("container width not changed.");
                return;
            }
        }
    }
    containerWidth_ = width;
    double dipScale = GridSystemManager::GetInstance().GetDipScale();
    // if not define the prop, use system grid define
    int32_t columns = GetValue(columns_, systemGridInfo.columns, UNDEFINED_INT);
    if (columns == 0) {
        return;
    }
    double gutterWidth = GetValue(gutterWidth_, systemGridInfo.gutter, UNDEFINED_DIMENSION).ConvertToPx(dipScale);
    double marginLeft = GetValue(marginLeft_, systemGridInfo.margin, UNDEFINED_DIMENSION).ConvertToPx(dipScale);
    double marginRight = GetValue(marginRight_, systemGridInfo.margin, UNDEFINED_DIMENSION).ConvertToPx(dipScale);
    double padding = paddingLeft_.ConvertToPx(dipScale) + paddingRight_.ConvertToPx(dipScale);

    columnWidth_ = (width - marginLeft - marginRight - padding - (columns - 1) * gutterWidth) / columns;
    LOGI("width = %{public}f, columns: %{public}d columnWidth: %{public}f gutterWidth: %{public}f marginLeft: "
         "%{public}f marginRight: %{public}f padding: %{public}f",
        width, columns, columnWidth_, gutterWidth, marginLeft, marginRight, padding);
}

GridSizeType GridContainerInfo::GetSizeType() const
{
    // if container don't want to use fix size type, use system current size type default
    return sizeType_ != GridSizeType::UNDEFINED ? sizeType_ : currentSizeType_;
}

int32_t GridContainerInfo::GetColumns() const
{
    if (columns_ == UNDEFINED_INT) {
        return GridSystemManager::GetInstance().GetSystemGridInfo(templateType_, containerWidth_).columns;
    }
    return columns_;
}

Dimension GridContainerInfo::GetGutterWidth() const
{
    if (gutterWidth_ == UNDEFINED_DIMENSION) {
        return GridSystemManager::GetInstance().GetSystemGridInfo(templateType_, containerWidth_).gutter;
    }
    return gutterWidth_;
}

Dimension GridContainerInfo::GetMarginLeft() const
{
    if (marginLeft_ == UNDEFINED_DIMENSION) {
        return GridSystemManager::GetInstance().GetSystemGridInfo(templateType_, containerWidth_).margin;
    }
    return marginLeft_;
}

Dimension GridContainerInfo::GetMarginRight() const
{
    if (marginRight_ == UNDEFINED_DIMENSION) {
        return GridSystemManager::GetInstance().GetSystemGridInfo(templateType_, containerWidth_).margin;
    }
    return marginRight_;
}

} // namespace OHOS::Ace
