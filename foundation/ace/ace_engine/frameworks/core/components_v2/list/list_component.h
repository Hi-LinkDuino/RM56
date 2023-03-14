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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_LIST_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_LIST_COMPONENT_H

#include <list>

#include "base/geometry/axis.h"
#include "base/geometry/dimension.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/components/scroll_bar/scroll_bar_proxy.h"
#include "core/components_v2/common/common_def.h"
#include "core/components_v2/list/list_position_controller.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace::V2 {

struct ItemDivider final {
    Dimension strokeWidth = 0.0_vp;
    Dimension startMargin = 0.0_vp;
    Dimension endMargin = 0.0_vp;
    Color color = Color::TRANSPARENT;
};

enum class ScrollState {
    IDLE = 0,
    SCROLL,
    FLING,
};

using OnItemDragStartFunc = std::function<RefPtr<Component>(const ItemDragInfo&, int32_t)>;
using OnItemDragEnterFunc = std::function<void(const ItemDragInfo&)>;
using OnItemDragMoveFunc = std::function<void(const ItemDragInfo&, int32_t, int32_t)>;
using OnItemDragLeaveFunc = std::function<void(const ItemDragInfo&, int32_t)>;
using OnItemDropFunc = std::function<void(const ItemDragInfo&, int32_t, int32_t, bool)>;

class ACE_EXPORT ListComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(V2::ListComponent, ComponentGroup)

public:
    ListComponent() = default;
    ~ListComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    const std::unique_ptr<ItemDivider>& GetItemDivider() const
    {
        return itemDivider_;
    }
    void SetItemDivider(std::unique_ptr<ItemDivider>&& divider)
    {
        itemDivider_ = std::move(divider);
    }

    ACE_DEFINE_COMPONENT_PROP(Space, Dimension, 0.0_vp);
    ACE_DEFINE_COMPONENT_PROP(Direction, Axis, Axis::VERTICAL);
    ACE_DEFINE_COMPONENT_PROP(EdgeEffect, EdgeEffect, EdgeEffect::SPRING);
    ACE_DEFINE_COMPONENT_PROP(ScrollBar, DisplayMode, DisplayMode::OFF);
    ACE_DEFINE_COMPONENT_PROP(InitialIndex, int32_t, 0);
    ACE_DEFINE_COMPONENT_PROP(CachedCount, int32_t, 1);
    ACE_DEFINE_COMPONENT_PROP(EditMode, bool, false);
    ACE_DEFINE_COMPONENT_PROP(ScrollController, RefPtr<ListPositionController>);
    ACE_DEFINE_COMPONENT_PROP(ScrollBarProxy, RefPtr<ScrollBarProxy>);
    ACE_DEFINE_COMPONENT_PROP(ChainAnimation, bool, false);
    ACE_DEFINE_COMPONENT_PROP(MultiSelectable, bool, false);
    ACE_DEFINE_COMPONENT_PROP(HasHeight, bool, false);
    ACE_DEFINE_COMPONENT_PROP(HasWidth, bool, false);

    ACE_DEFINE_COMPONENT_EVENT(OnScroll, void(Dimension, ScrollState));
    ACE_DEFINE_COMPONENT_EVENT(OnScrollIndex, void(int32_t, int32_t));
    ACE_DEFINE_COMPONENT_EVENT(OnReachStart, void());
    ACE_DEFINE_COMPONENT_EVENT(OnReachEnd, void());
    ACE_DEFINE_COMPONENT_EVENT(OnScrollStop, void());
    ACE_DEFINE_COMPONENT_EVENT(OnItemDelete, bool(int32_t));
    ACE_DEFINE_COMPONENT_EVENT(OnItemMove, bool(int32_t, int32_t));

    uint32_t Compare(const RefPtr<Component>& component) const override;

    OnItemDragStartFunc GetOnItemDragStartId() const
    {
        return onItemDragStartId_;
    }

    void SetOnItemDragStartId(const OnItemDragStartFunc& onItemDragStartId)
    {
        onItemDragStartId_ = onItemDragStartId;
    }

    OnItemDragEnterFunc GetOnItemDragEnterId() const
    {
        return onItemDragEnterId_;
    }

    void SetOnItemDragEnterId(const OnItemDragEnterFunc& onItemDragEnterId)
    {
        onItemDragEnterId_ = onItemDragEnterId;
    }

    OnItemDragMoveFunc GetOnItemDragMoveId() const
    {
        return onItemDragMoveId_;
    }

    void SetOnItemDragMoveId(const OnItemDragMoveFunc& onItemDragMoveId)
    {
        onItemDragMoveId_ = onItemDragMoveId;
    }

    OnItemDragLeaveFunc GetOnItemDragLeaveId() const
    {
        return onItemDragLeaveId_;
    }

    void SetOnItemDragLeaveId(const OnItemDragLeaveFunc& onItemDragLeaveId)
    {
        onItemDragLeaveId_ = onItemDragLeaveId;
    }

    OnItemDropFunc GetOnItemDropId() const
    {
        return onItemDropId_;
    }

    void SetOnItemDropId(const OnItemDropFunc& onItemDropId)
    {
        onItemDropId_ = onItemDropId;
    }

private:
    std::unique_ptr<ItemDivider> itemDivider_;

    OnItemDragStartFunc onItemDragStartId_;
    OnItemDragEnterFunc onItemDragEnterId_;
    OnItemDragMoveFunc onItemDragMoveId_;
    OnItemDragLeaveFunc onItemDragLeaveId_;
    OnItemDropFunc onItemDropId_;

    ACE_DISALLOW_COPY_AND_MOVE(ListComponent);
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_LIST_COMPONENT_H
