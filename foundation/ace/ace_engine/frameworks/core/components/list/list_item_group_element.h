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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_GROUP_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_GROUP_ELEMENT_H

#include "core/components/list/list_item_element.h"
#include "core/components/list/render_list_item_group.h"

namespace OHOS::Ace {

class ListItemGroupElement : public ListItemElement {
    DECLARE_ACE_TYPE(ListItemGroupElement, ListItemElement);

public:
    void PerformBuild() override;
    void ApplyRenderChild(const RefPtr<RenderElement>& renderChild) override;

    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
    void HandleOnFocus() override;
    void HandleOnBlur() override;
    RefPtr<Component> GetPrimary();

    bool OnKeyEvent(const KeyEvent& keyEvent) override;
    int32_t GetItemGroupFocusIndex();
    void ItemFocus(int32_t itemIndex);

private:
    void OnFocus() override;
    RefPtr<RenderNode> CreateRenderNode() override;
    void MoveItemToViewPort(const RefPtr<Element>& listItem, bool reverse);
    bool TraverseFocusNode(int32_t currentFocusIndex, bool reverse);
    void AddItemGroupFocusIndex(int32_t groupFocusIndex);
    int32_t GetNextFocusIndex(bool vertical, bool reverse);
    RefPtr<RenderListItemGroup> renderItemGroup_;
    double size_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_GROUP_ELEMENT_H
