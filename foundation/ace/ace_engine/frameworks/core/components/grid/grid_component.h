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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_COMPONENT_H

#include "core/components/common/layout/constants.h"
#include "core/components/grid/grid_controller.h"
#include "core/components/grid/grid_element.h"
#include "core/components/grid/render_grid.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

class GridComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(GridComponent, ComponentGroup);

public:
    explicit GridComponent(const std::list<RefPtr<Component>>& children)
        : ComponentGroup(children)
    {
        gridController_ = AceType::MakeRefPtr<GridController>();
    }

    ~GridComponent() override = default;

    RefPtr<Element> CreateElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    void SetScrollPage(bool scrollPage);
    void SetDirection(FlexDirection direction);
    void SetFlexAlign(FlexAlign flexAlign);
    void SetColumnCount(int32_t count);
    void SetColumnExtent(int32_t extent);
    void SetTotalCount(int32_t totalCount);
    void SetCachedCount(int32_t cachedCount);
    void SetWidth(double width);
    void SetHeight(double height);
    void SetOnRequestItem(const EventMarker& eventMarker);
    void SetOnScroll(const EventMarker& onScroll);
    void SetOnScrollBottom(const EventMarker& onScrollBottom);
    void SetOnScrollTop(const EventMarker& onScrollTop);
    void SetOnScrollEnd(const EventMarker& onScrollEnd);
    void SetOnScrollTouchUp(const EventMarker& onScrollTouchUp);

    bool GetScrollPage() const
    {
        return scrollPage_;
    }

    FlexDirection GetDirection() const
    {
        return direction_;
    }

    FlexAlign GetFlexAlign() const
    {
        return flexAlign_;
    }

    int32_t GetColumnCount() const
    {
        return columnCount_;
    }

    int32_t GetColumnExtent() const
    {
        return columnExtent_;
    }

    int32_t GetTotalCount() const
    {
        return itemCount_;
    }

    int32_t GetCachedCount() const
    {
        return cachedCount_;
    }

    double GetWidth() const
    {
        return width_;
    }

    double GetHeight() const
    {
        return height_;
    }

    RefPtr<GridController> GetGridController() const
    {
        return gridController_;
    }

    const EventMarker& GetOnRequestItem() const
    {
        return onRequestItem_;
    }

    const EventMarker& GetOnScroll() const
    {
        return onScroll_;
    }

    const EventMarker& GetOnScrollBottom() const
    {
        return onScrollBottom_;
    }

    const EventMarker& GetOnScrollTop() const
    {
        return onScrollTop_;
    }

    const EventMarker& GetOnScrollEnd() const
    {
        return onScrollEnd_;
    }

    const EventMarker& GetOnScrollTouchUp() const
    {
        return onScrollTouchUp_;
    }

private:
    EventMarker onRequestItem_;
    EventMarker onScroll_;
    EventMarker onScrollBottom_;
    EventMarker onScrollTop_;
    EventMarker onScrollEnd_;
    EventMarker onScrollTouchUp_;
    FlexDirection direction_ { FlexDirection::COLUMN };
    FlexAlign flexAlign_ { FlexAlign::STRETCH };
    bool scrollPage_ { false };
    double width_ { DEFAULT_GRID_WIDTH };
    double height_ { DEFAULT_GRID_HEIGHT };
    int32_t columnCount_ { DEFAULT_COLUMN_COUNT };
    int32_t columnExtent_ { DEFAULT_COLUMN_EXTENT };
    int32_t itemCount_ { DEFAULT_ITEM_COUNT };
    int32_t cachedCount_ { DEFAULT_CACHED_COUNT };
    RefPtr<GridController> gridController_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_COMPONENT_H
