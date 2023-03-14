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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_GRID_SYSTEM_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_GRID_SYSTEM_MANAGER_H

#include <mutex>

#include "base/geometry/dimension.h"
#include "base/memory/ace_type.h"
#include "core/components/common/layout/grid_column_info.h"

namespace OHOS::Ace {

struct SystemGridInfo {
    SystemGridInfo() = default;
    SystemGridInfo(
        GridSizeType sizeType, const Dimension& gutter, const Dimension& margin, int32_t columns, int32_t maxColumns)
        : sizeType(sizeType), gutter(gutter), margin(margin), columns(columns), maxColumns(maxColumns)
    {}
    SystemGridInfo(GridSizeType sizeType, const Dimension& gutter, const Dimension& margin, int32_t columns)
        : sizeType(sizeType), gutter(gutter), margin(margin), columns(columns), maxColumns(columns)
    {}
    ACE_FORCE_EXPORT_WITH_PREVIEW std::string ToString() const;

    GridSizeType sizeType = GridSizeType::UNDEFINED;
    Dimension gutter;
    Dimension margin;
    int32_t columns = 0;
    int32_t maxColumns = 0;
};

class ACE_FORCE_EXPORT_WITH_PREVIEW GridSystemManager final {
public:
    ~GridSystemManager() = default;
    ACE_EXPORT static GridSystemManager& GetInstance();

    const SystemGridInfo& GetCurrentGridInfo()
    {
        return systemGridInfo_;
    }

    static SystemGridInfo GetSystemGridInfo(const GridSizeType& sizeType);
    static RefPtr<GridColumnInfo> GetInfoByType(const GridColumnType& type);
    // width is use px unit.
    SystemGridInfo GetSystemGridInfo(const GridTemplateType& templateType, double width);
    void OnSurfaceChanged(double width);
    void SetWindowInfo(double screenWidth, double density, double dipScale)
    {
        screenWidth_ = screenWidth;
        density_ = density;
        dipScale_ = dipScale;
        viewScale_ = density / dipScale;
    }

    double GetScreenWidth() const
    {
        return screenWidth_;
    }

    double GetDipScale() const
    {
        return dipScale_;
    }

    GridSizeType GetCurrentSize() const
    {
        return currentSize_;
    }

private:
    GridSystemManager() = default;

    static GridSystemManager* instance_;
    static std::mutex mutex_;

    double screenWidth_ = 0.0;
    double density_ = 1.0;
    double dipScale_ = 1.0;
    double viewScale_ = 1.0;
    GridSizeType currentSize_ = GridSizeType::UNDEFINED;
    SystemGridInfo systemGridInfo_;

    ACE_DISALLOW_COPY_AND_MOVE(GridSystemManager);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_GRID_SYSTEM_MANAGER_H
