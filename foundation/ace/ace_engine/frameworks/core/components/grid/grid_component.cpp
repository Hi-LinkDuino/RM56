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

#include "core/components/grid/grid_component.h"

#include "core/components/grid/grid_element.h"
#include "core/components/grid/render_grid.h"

namespace OHOS::Ace {

RefPtr<Element> GridComponent::CreateElement()
{
    return AceType::MakeRefPtr<GridElement>();
}

RefPtr<RenderNode> GridComponent::CreateRenderNode()
{
    return RenderGrid::Create();
}

void GridComponent::SetScrollPage(bool scrollPage)
{
    scrollPage_ = scrollPage;
}

void GridComponent::SetDirection(FlexDirection direction)
{
    if (direction < FlexDirection::ROW || direction > FlexDirection::COLUMN_REVERSE) {
        LOGW("Invalid direction %{public}d", direction);
        return;
    }
    direction_ = direction;
}

void GridComponent::SetFlexAlign(FlexAlign flexAlign)
{
    if (flexAlign < FlexAlign::FLEX_START || flexAlign > FlexAlign::STRETCH) {
        LOGW("Invalid flexAlign %{public}d", flexAlign);
        return;
    }
    flexAlign_ = flexAlign;
}

void GridComponent::SetColumnCount(int32_t count)
{
    if (count <= 0) {
        LOGW("Invalid ColumnCount %{public}d", count);
        return;
    }
    columnCount_ = count;
}

void GridComponent::SetColumnExtent(int32_t extent)
{
    if (extent <= 0) {
        LOGW("Invalid ColumnExtent %{public}d", extent);
        return;
    }
    columnExtent_ = extent;
}

void GridComponent::SetTotalCount(int32_t totalCount)
{
    if (totalCount < 0) {
        LOGW("Invalid TotalCount %{public}d", totalCount);
        return;
    }
    itemCount_ = totalCount;
}

void GridComponent::SetCachedCount(int32_t cachedCount)
{
    if (cachedCount <= 0) {
        LOGW("Invalid CachedCount %{public}d", cachedCount);
        return;
    }
    cachedCount_ = cachedCount;
}

void GridComponent::SetWidth(double width)
{
    if (width <= 0.0) {
        LOGW("Invalid Width %{public}lf", width);
        return;
    }
    width_ = width;
}

void GridComponent::SetHeight(double height)
{
    if (height <= 0.0) {
        LOGW("Invalid Height %{public}lf", height);
        return;
    }
    height_ = height;
}

void GridComponent::SetOnRequestItem(const EventMarker& eventMarker)
{
    onRequestItem_ = eventMarker;
}

void GridComponent::SetOnScroll(const EventMarker& onScroll)
{
    onScroll_ = onScroll;
}

void GridComponent::SetOnScrollBottom(const EventMarker& onScrollBottom)
{
    onScrollBottom_ = onScrollBottom;
}

void GridComponent::SetOnScrollTop(const EventMarker& onScrollTop)
{
    onScrollTop_ = onScrollTop;
}

void GridComponent::SetOnScrollEnd(const EventMarker& onScrollEnd)
{
    onScrollEnd_ = onScrollEnd;
}

void GridComponent::SetOnScrollTouchUp(const EventMarker& onScrollTouchUp)
{
    onScrollTouchUp_ = onScrollTouchUp;
}

} // namespace OHOS::Ace
