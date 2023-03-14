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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_GRID_LAYOUT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_GRID_LAYOUT_COMPONENT_H

#include "base/utils/macros.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/components/scroll_bar/scroll_bar_proxy.h"
#include "core/components_v2/foreach/lazy_foreach_component.h"
#include "core/components_v2/grid/grid_position_controller.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

using OnGridDragEnterFunc = std::function<void(const ItemDragInfo&)>;
using OnGridDragMoveFunc = std::function<void(const ItemDragInfo&, int32_t, int32_t)>;
using OnGridDragLeaveFunc = std::function<void(const ItemDragInfo&, int32_t)>;
using OnGridDragStartFunc = std::function<RefPtr<Component>(const ItemDragInfo&, int32_t)>;
using OnGridDropFunc = std::function<void(const ItemDragInfo&, int32_t, int32_t, bool)>;

class ACE_EXPORT GridLayoutComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(GridLayoutComponent, ComponentGroup);

public:
    explicit GridLayoutComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children) {}

    ~GridLayoutComponent() override = default;

    RefPtr<Element> CreateElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    void SetDirection(FlexDirection direction);
    void SetFlexAlign(FlexAlign flexAlign);
    void SetColumnCount(int32_t count);
    void SetRowCount(int32_t count);
    void SetWidth(double width);
    void SetHeight(double height);
    void SetColumnsArgs(const std::string& columnsArgs);
    void SetRowsArgs(const std::string& rowsArgs);
    void SetColumnGap(const Dimension& columnGap);
    void SetRowGap(const Dimension& rowGap);
    void SetRightToLeft(bool rightToLeft);

    // set scroll bar color
    void SetScrollBarColor(const std::string& color);

    // set scroll bar width
    void SetScrollBarWidth(const std::string& width);

    void SetScrollBar(DisplayMode displayMode);

    const std::string& GetColumnsArgs() const
    {
        return columnsArgs_;
    }

    const std::string& GetRowsArgs() const
    {
        return rowsArgs_;
    }

    const Dimension& GetColumnGap() const
    {
        return columnGap_;
    }

    const Dimension& GetRowGap() const
    {
        return rowGap_;
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

    int32_t GetRowCount() const
    {
        return rowCount_;
    }

    double GetWidth() const
    {
        return width_;
    }

    double GetHeight() const
    {
        return height_;
    }

    bool GetRightToLeft() const
    {
        return rightToLeft_;
    }

    void SetUseScroll(bool flag)
    {
        useScroll_ = flag;
    }

    const std::string& GetScrollBarColor() const
    {
        return scrollBarColor_;
    }

    const std::string& GetScrollBarWidth() const
    {
        return scrollBarWidth_;
    }

    DisplayMode GetScrollBar()
    {
        return displayMode_;
    }

    void SetDeclarative()
    {
        isDeclarative_ = true;
    }

    bool IsDeclarative()
    {
        return isDeclarative_;
    }

    void SetNeedShrink(bool flag)
    {
        needShrink_ = flag;
    }

    bool NeedShrink()
    {
        return needShrink_;
    }

    const RefPtr<V2::GridPositionController>& GetController() const
    {
        return controller_;
    }

    void SetController(const RefPtr<V2::GridPositionController>& controller)
    {
        controller_ = controller;
    }

    void SetScrolledEvent(const EventMarker& event)
    {
        scrolledEvent_ = event;
    }

    const EventMarker& GetScrolledEvent() const
    {
        return scrolledEvent_;
    }

    void SetCachedCount(int32_t cacheCount)
    {
        cacheCount_ = cacheCount;
    }

    int32_t GetCacheCount() const
    {
        return cacheCount_;
    }

    void SetScrollBarProxy(const RefPtr<ScrollBarProxy>& scrollBarProxy)
    {
        scrollBarProxy_ = scrollBarProxy;
    }

    const RefPtr<ScrollBarProxy>& GetScrollBarProxy() const
    {
        return scrollBarProxy_;
    }

    void SetEditMode(bool editMode)
    {
        editMode_ = editMode;
    }

    bool GetEditMode() const
    {
        return editMode_;
    }

    void SetMaxCount(int32_t maxCount)
    {
        maxCount_ = maxCount;
    }

    int32_t GetMaxCount(void) const
    {
        return maxCount_;
    }

    void SetMinCount(int32_t minCount)
    {
        minCount_ = minCount;
    }

    int32_t GetMinCount(void) const
    {
        return minCount_;
    }

    void SetCellLength(int32_t cellLength)
    {
        cellLength_ = cellLength;
    }

    int32_t GetCellLength(void) const
    {
        return cellLength_;
    }

    void SetSupportAnimation(bool supportAnimation)
    {
        supportAnimation_ = supportAnimation;
    }

    bool GetSupportAnimation()
    {
        return supportAnimation_;
    }

    void SetDragAnimation(bool value)
    {
        supportDragAnimation_ = value;
    }

    bool GetDragAnimation()
    {
        return supportDragAnimation_;
    }

    void SetEdgeEffection(EdgeEffect value)
    {
        if (value < EdgeEffect::SPRING || value > EdgeEffect::NONE) {
            LOGW("Invalid edgeEffect %{public}d", value);
            return;
        }
        edgeEffect_ = value;
    }

    EdgeEffect GetEdgeEffection()
    {
        return edgeEffect_;
    }

    void SetOnGridDragEnterId(const OnGridDragEnterFunc& onGridDragEnterId);
    void SetOnGridDragMoveId(const OnGridDragMoveFunc& onGridDragMoveId);
    void SetOnGridDragLeaveId(const OnGridDragLeaveFunc& onGridDragLeaveId);
    void SetOnGridDragStartId(const OnGridDragStartFunc& onGridDragStartId);
    void SetOnGridDropId(const OnGridDropFunc& onGridDropId);
    const OnGridDragEnterFunc& GetOnGridDragEnterId() const;
    const OnGridDragMoveFunc& GetOnGridDragMoveId() const;
    const OnGridDragLeaveFunc& GetOnGridDragLeaveId() const;
    const OnGridDragStartFunc& GetOnGridDragStartId() const;
    const OnGridDropFunc& GetOnGridDropId() const;

    void SetMultiSelectable(bool multiSelectable)
    {
        multiSelectable_ = multiSelectable;
    }

    bool GetMultiSelectable() const
    {
        return multiSelectable_;
    }

private:
    FlexDirection direction_ = FlexDirection::COLUMN;
    FlexAlign flexAlign_ = FlexAlign::CENTER;
    EdgeEffect edgeEffect_ = EdgeEffect::NONE;
    double width_ = -1.0;
    double height_ = -1.0;
    int32_t columnCount_ = 1;
    int32_t rowCount_ = 1;
    bool isDeclarative_ = false;
    int32_t cacheCount_ = 1;
    bool supportAnimation_ = false;
    bool supportDragAnimation_ = false;

    std::string columnsArgs_;
    std::string rowsArgs_;
    Dimension columnGap_ = 0.0_px;
    Dimension rowGap_ = 0.0_px;
    bool rightToLeft_ = false;
    bool useScroll_ = true;

    // scroll bar attribute
    std::string scrollBarColor_;
    std::string scrollBarWidth_;
    DisplayMode displayMode_ = DisplayMode::OFF;
    RefPtr<V2::GridPositionController> controller_;
    EventMarker scrolledEvent_;
    RefPtr<ScrollBarProxy> scrollBarProxy_;
    bool needShrink_ = false;

    // drag in grid attribute
    bool editMode_ = false;
    bool multiSelectable_ = false;
    int32_t maxCount_ = 1;
    int32_t minCount_ = 1;
    int32_t cellLength_ = 0;
    OnGridDragEnterFunc onGridDragEnterId_;
    OnGridDragMoveFunc onGridDragMoveId_;
    OnGridDragLeaveFunc onGridDragLeaveId_;
    OnGridDragStartFunc onGridDragStartId_;
    OnGridDropFunc onGridDropId_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_GRID_LAYOUT_COMPONENT_H
