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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_GROUP_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_GROUP_COMPONENT_H

#include "core/components/list/list_item_component.h"

namespace OHOS::Ace {

class ListItemGroupComponent : public ListItemComponent {
    DECLARE_ACE_TYPE(ListItemGroupComponent, ListItemComponent);

public:
    explicit ListItemGroupComponent(const std::string& type);
    ~ListItemGroupComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void SetOnClicked(const EventMarker& onClicked)
    {
        onClicked_ = onClicked;
    }

    const EventMarker& GetOnClicked() const
    {
        return onClicked_;
    }

    void SetOnCollapse(const EventMarker& onCollapse)
    {
        onCollapse_ = onCollapse;
    }

    const EventMarker& GetOnCollapse() const
    {
        return onCollapse_;
    }

    void SetOnExpand(const EventMarker& onExpand)
    {
        onExpand_ = onExpand;
    }

    const EventMarker& GetOnExpand() const
    {
        return onExpand_;
    }

    void SetGroupId(const std::string& groupId)
    {
        groupId_ = groupId;
    }

    const std::string& GetGroupId() const
    {
        return groupId_;
    }

    const std::list<RefPtr<Component>>& GetChildren() const
    {
        return children_;
    }

    void AppendChild(const RefPtr<Component>& child)
    {
        if (child) {
            children_.emplace_back(child);
        }
    }

    void RemoveChild(const RefPtr<Component>& child)
    {
        if (child) {
            children_.remove(child);
        }
    }

    void SetDirection(FlexDirection direction)
    {
        direction_ = direction;
    }

    FlexDirection GetDirection() const
    {
        return direction_;
    }

    void SetExpand(bool expand)
    {
        expand_ = expand;
    }

    bool GetExpand() const
    {
        return expand_;
    }

private:
    EventMarker onClicked_;
    EventMarker onCollapse_;
    EventMarker onExpand_;
    std::string groupId_;
    bool expand_ = false;
    std::list<RefPtr<Component>> children_;
    FlexDirection direction_ = FlexDirection::COLUMN;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_GROUP_COMPONENT_H
