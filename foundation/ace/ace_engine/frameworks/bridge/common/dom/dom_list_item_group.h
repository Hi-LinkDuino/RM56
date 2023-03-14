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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_LIST_ITEM_GROUP_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_LIST_ITEM_GROUP_H

#include "core/components/list/list_item_group_component.h"
#include "core/components/theme/card_theme.h"
#include "frameworks/bridge/common/dom/dom_list_item.h"

namespace OHOS::Ace::Framework {

class DOMListItemGroup : public DOMListItem {
    DECLARE_ACE_TYPE(DOMListItemGroup, DOMListItem);

public:
    DOMListItemGroup(NodeId nodeId, const std::string& nodeName, int32_t forIndex);
    ~DOMListItemGroup() override = default;
    RefPtr<Component> GetSpecializedComponent() override;
    int32_t GetItemIndex() const override;

    const std::string& GetGroupId() const
    {
        return groupId_;
    }

    void Update();

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void PrepareSpecializedComponent() override;
    RefPtr<Component> CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components) override;
    void ResetInitializedStyle() override;

private:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void SetCardThemeAttrs();

    EventMarker onClicked_;
    EventMarker onCollapse_;
    EventMarker onExpand_;
    std::string groupId_;
    RefPtr<ListItemGroupComponent> listItemGroupComponent_;
    RefPtr<CardTheme> cardTheme_;
    bool isCardBlur_ = false;
    bool isCard_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_LIST_ITEM_GROUP_H
