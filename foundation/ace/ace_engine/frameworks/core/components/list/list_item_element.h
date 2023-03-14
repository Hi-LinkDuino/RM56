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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_ELEMENT_H

#include "core/components/list/list_item_component.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class ListItemElement : public SoleChildElement, public FocusGroup {
    DECLARE_ACE_TYPE(ListItemElement, SoleChildElement, FocusGroup);

public:
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
    virtual void HandleOnFocus();
    virtual void HandleOnBlur();
    virtual void HandleOnClick();

    bool CanUpdate(const RefPtr<Component>& newComponent) override;

    std::string GetItemType() const
    {
        return type_;
    }

    uint32_t GetFlags() const
    {
        return flags_;
    }

    bool TestFlag(uint32_t flag) const
    {
        return (flags_ & flag);
    }

    void RemoveFlag(uint32_t flag)
    {
        flags_ &= ~flag;
    }

    int32_t GetIndex() const
    {
        return index_;
    }

    void SetIndex(int32_t index)
    {
        index_ = index;
        if (listItemComponent_) {
            listItemComponent_->SetIndex(index);
        }
    }

    void AttachRenderNode(const RefPtr<RenderNode>& proxyNode)
    {
        proxyNode_ = proxyNode;
    }

    RefPtr<RenderNode> GetProxyRenderNode() const
    {
        return proxyNode_;
    }

    bool OnKeyEvent(const KeyEvent& keyEvent) override;

    static RefPtr<ListItemElement> GetListItem(const RefPtr<Element>& element)
    {
        RefPtr<Element> listItem = element;
        while (listItem) {
            if (AceType::InstanceOf<ListItemElement>(listItem)) {
                return AceType::DynamicCast<ListItemElement>(listItem);
            }
            listItem = listItem->GetFirstChild();
        }
        return nullptr;
    }

    int32_t GetKey() const
    {
        return key_;
    }

    void SetKey(int32_t key)
    {
        key_ = key;
    }

private:
    RefPtr<RenderNode> CreateRenderNode() override;

    void Update() override;
    void PerformBuild() override;

    uint32_t flags_ = 0;
    std::string type_;
    int32_t index_ = -1;
    Radius topLeftRadius_;
    Radius topRightRadius_;
    Radius bottomLeftRadius_;
    Radius bottomRightRadius_;
    int32_t key_ = -1;

    RefPtr<RenderNode> proxyNode_;
    RefPtr<ListItemComponent> listItemComponent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ITEM_ELEMENT_H
