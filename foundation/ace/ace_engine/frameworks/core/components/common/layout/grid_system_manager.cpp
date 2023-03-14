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

#include "core/components/common/layout/grid_system_manager.h"

#include <map>
#include <vector>

#include "base/json/json_util.h"
#include "base/log/log.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension COLUMN_2_AND_4_BREAKPOINT = 320.0_vp;
constexpr Dimension COLUMN_4_AND_8_BREAKPOINT = 600.0_vp;
constexpr Dimension COLUMN_8_AND_12_BREAKPOINT = 840.0_vp;
constexpr Dimension SMALL_GUTTER = 12.0_vp;
constexpr Dimension SMALL_MARGIN = 12.0_vp;
constexpr Dimension LARGE_GUTTER = 24.0_vp;
constexpr Dimension LARGE_MARGIN = 24.0_vp;
constexpr Dimension CAR_SMALL_GUTTER = 16.0_vp;
constexpr Dimension CAR_SMALL_MARGIN = 32.0_vp;
constexpr uint32_t COLUMN_SM = 0;
constexpr uint32_t COLUMN_MD = 1;
constexpr uint32_t COLUMN_LG = 2;

const SystemGridInfo GRID_COLUMNS_2(GridSizeType::XS, SMALL_GUTTER, SMALL_MARGIN, 2);
const SystemGridInfo GRID_COLUMNS_4(GridSizeType::SM, LARGE_GUTTER, LARGE_MARGIN, 4);
const SystemGridInfo GRID_COLUMNS_8(GridSizeType::MD, LARGE_GUTTER, 32.0_vp, 8);
const SystemGridInfo GRID_COLUMNS_12(GridSizeType::LG, LARGE_GUTTER, 48.0_vp, 12);

const SystemGridInfo GRID_TEMPLATE_COLUMNS_4(GridSizeType::SM, SMALL_GUTTER, LARGE_MARGIN, 4);
const SystemGridInfo GRID_TEMPLATE_COLUMNS_8(GridSizeType::MD, SMALL_GUTTER, 32.0_vp, 8);
const SystemGridInfo GRID_TEMPLATE_COLUMNS_12(GridSizeType::LG, SMALL_GUTTER, 48.0_vp, 12);

const std::map<GridColumnType, std::vector<SystemGridInfo>> SYSTEM_GRID_TYPES = {
    { GridColumnType::BUBBLE_TYPE, { SystemGridInfo(GridSizeType::SM, LARGE_GUTTER, LARGE_MARGIN, 4),
                                       SystemGridInfo(GridSizeType::MD, LARGE_GUTTER, LARGE_MARGIN, 6),
                                       SystemGridInfo(GridSizeType::LG, LARGE_GUTTER, LARGE_MARGIN, 6) } },
    { GridColumnType::BUTTON_TYPE, { SystemGridInfo(GridSizeType::SM, LARGE_GUTTER, LARGE_MARGIN, 4),
                                       SystemGridInfo(GridSizeType::MD, LARGE_GUTTER, LARGE_MARGIN, 6),
                                       SystemGridInfo(GridSizeType::LG, LARGE_GUTTER, LARGE_MARGIN, 8) } },
    { GridColumnType::CARD_TYPE, { SystemGridInfo(GridSizeType::SM, SMALL_GUTTER, SMALL_MARGIN, 4),
                                     SystemGridInfo(GridSizeType::MD, SMALL_GUTTER, SMALL_MARGIN, 6),
                                     SystemGridInfo(GridSizeType::LG, SMALL_GUTTER, SMALL_MARGIN, 8) } },
    { GridColumnType::CONTENT_TYPE, { SystemGridInfo(GridSizeType::SM, LARGE_GUTTER, LARGE_MARGIN, 4),
                                        SystemGridInfo(GridSizeType::MD, LARGE_GUTTER, LARGE_MARGIN, 6),
                                        SystemGridInfo(GridSizeType::LG, LARGE_GUTTER, LARGE_MARGIN, 8) } },
    { GridColumnType::MENU, { SystemGridInfo(GridSizeType::SM, LARGE_GUTTER, LARGE_MARGIN, 2, 4),
                                SystemGridInfo(GridSizeType::MD, LARGE_GUTTER, LARGE_MARGIN, 2, 6),
                                SystemGridInfo(GridSizeType::LG, LARGE_GUTTER, LARGE_MARGIN, 2, 8) } },
    { GridColumnType::TOAST, { SystemGridInfo(GridSizeType::SM, LARGE_GUTTER, LARGE_MARGIN, 2, 4),
                                 SystemGridInfo(GridSizeType::MD, LARGE_GUTTER, LARGE_MARGIN, 2, 6),
                                 SystemGridInfo(GridSizeType::LG, LARGE_GUTTER, LARGE_MARGIN, 2, 6) } },
    { GridColumnType::TOOL_BAR, { SystemGridInfo(GridSizeType::SM, LARGE_GUTTER, LARGE_MARGIN, 4),
                                    SystemGridInfo(GridSizeType::MD, LARGE_GUTTER, LARGE_MARGIN, 8),
                                    SystemGridInfo(GridSizeType::LG, LARGE_GUTTER, LARGE_MARGIN, 12) } },
    { GridColumnType::PANEL, { SystemGridInfo(GridSizeType::SM, SMALL_GUTTER, SMALL_MARGIN, 4),
                                 SystemGridInfo(GridSizeType::MD, SMALL_GUTTER, SMALL_MARGIN, 6),
                                 SystemGridInfo(GridSizeType::LG, SMALL_GUTTER, SMALL_MARGIN, 6) } },
    { GridColumnType::DIALOG, { SystemGridInfo(GridSizeType::SM, SMALL_GUTTER, SMALL_MARGIN, 3),
                                  SystemGridInfo(GridSizeType::MD, SMALL_GUTTER, SMALL_MARGIN, 4),
                                  SystemGridInfo(GridSizeType::LG, SMALL_GUTTER, SMALL_MARGIN, 5) } },
    { GridColumnType::CAR_DIALOG, { SystemGridInfo(GridSizeType::SM, CAR_SMALL_GUTTER, CAR_SMALL_MARGIN, 4),
                                      SystemGridInfo(GridSizeType::MD, CAR_SMALL_GUTTER, CAR_SMALL_MARGIN, 8),
                                      SystemGridInfo(GridSizeType::LG, CAR_SMALL_GUTTER, CAR_SMALL_MARGIN, 12) } },
};

} // namespace

std::string SystemGridInfo::ToString() const
{
    auto json = JsonUtil::Create(true);
    double dipScale = GridSystemManager::GetInstance().GetDipScale();
    json->Put("columns", columns);
    json->Put("gutter", gutter.ConvertToPx(dipScale));
    json->Put("margin", margin.ConvertToPx(dipScale));
    return json->ToString();
}

GridSystemManager* GridSystemManager::instance_ = nullptr;

std::mutex GridSystemManager::mutex_;

GridSystemManager& GridSystemManager::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = new GridSystemManager();
        }
    }
    return *instance_;
}

RefPtr<GridColumnInfo> GridSystemManager::GetInfoByType(const GridColumnType& columnType)
{
    auto iter = SYSTEM_GRID_TYPES.find(columnType);
    if (iter != SYSTEM_GRID_TYPES.end()) {
        GridContainerInfo::Builder containerBuilder;
        containerBuilder.SetGutterWidth(iter->second[COLUMN_SM].gutter);
        containerBuilder.SetMarginLeft(iter->second[COLUMN_SM].margin);
        containerBuilder.SetMarginRight(iter->second[COLUMN_SM].margin);
        containerBuilder.SetColumnType(columnType);

        GridColumnInfo::Builder columnBuilder;
        columnBuilder.SetParent(containerBuilder.Build());
        columnBuilder.SetSmSizeColumn(iter->second[COLUMN_SM].columns);
        columnBuilder.SetMdSizeColumn(iter->second[COLUMN_MD].columns);
        columnBuilder.SetLgSizeColumn(iter->second[COLUMN_LG].columns);
        columnBuilder.SetSmSizeMaxColumn(iter->second[COLUMN_SM].maxColumns);
        columnBuilder.SetMdSizeMaxColumn(iter->second[COLUMN_MD].maxColumns);
        columnBuilder.SetLgSizeMaxColumn(iter->second[COLUMN_LG].maxColumns);
        return columnBuilder.Build();
    }
    LOGE("not find column type: %d", columnType);
    return nullptr;
}

SystemGridInfo GridSystemManager::GetSystemGridInfo(const GridSizeType& sizeType)
{
    if (sizeType == GridSizeType::XS) {
        return GRID_COLUMNS_2;
    } else if (sizeType == GridSizeType::SM) {
        return GRID_COLUMNS_4;
    } else if (sizeType == GridSizeType::MD) {
        return GRID_COLUMNS_8;
    } else if (sizeType == GridSizeType::LG) {
        return GRID_COLUMNS_12;
    }

    return SystemGridInfo();
}

void GridSystemManager::OnSurfaceChanged(double width)
{
    screenWidth_ = width;
    if (width < COLUMN_2_AND_4_BREAKPOINT.Value() * density_) {
        currentSize_ = GridSizeType::XS;
        systemGridInfo_ = GRID_COLUMNS_2;
    } else if (width < COLUMN_4_AND_8_BREAKPOINT.Value() * density_) {
        currentSize_ = GridSizeType::SM;
        systemGridInfo_ = GRID_COLUMNS_4;
    } else if (width < COLUMN_8_AND_12_BREAKPOINT.Value() * density_) {
        currentSize_ = GridSizeType::MD;
        systemGridInfo_ = GRID_COLUMNS_8;
    } else {
        currentSize_ = GridSizeType::LG;
        systemGridInfo_ = GRID_COLUMNS_12;
    }
    currentSize_ = systemGridInfo_.sizeType;

    LOGD("OnSurfaceChanged: %{public}f: sizeType = %{public}d", width, systemGridInfo_.sizeType);
}

SystemGridInfo GridSystemManager::GetSystemGridInfo(const GridTemplateType& templateType, double width)
{
    // Input width is normalized in px.
    if (templateType == GridTemplateType::NORMAL) {
        if (width < COLUMN_2_AND_4_BREAKPOINT.Value() * dipScale_) {
            return GRID_COLUMNS_2;
        } else if (width < COLUMN_4_AND_8_BREAKPOINT.Value() * dipScale_) {
            return GRID_COLUMNS_4;
        } else if (width < COLUMN_8_AND_12_BREAKPOINT.Value() * dipScale_) {
            return GRID_COLUMNS_8;
        } else {
            return GRID_COLUMNS_12;
        }
    }
    if (width < COLUMN_4_AND_8_BREAKPOINT.Value() * dipScale_) {
        return GRID_TEMPLATE_COLUMNS_4;
    } else if (width < COLUMN_8_AND_12_BREAKPOINT.Value() * dipScale_) {
        return GRID_TEMPLATE_COLUMNS_8;
    } else {
        return GRID_TEMPLATE_COLUMNS_12;
    }
}

} // namespace OHOS::Ace
