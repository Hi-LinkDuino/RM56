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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_ITEM_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_ITEM_ELEMENT_H

#include "core/components/box/box_element.h"
#include "core/focus/focus_node.h"

namespace OHOS::Ace {

class TabBarItemElement : public BoxElement, public FocusGroup {
    DECLARE_ACE_TYPE(TabBarItemElement, BoxElement, FocusGroup);

public:
    void OnFocus() override
    {
        if (renderNode_) {
            renderNode_->ChangeStatus(RenderStatus::FOCUS);
        }
        FocusGroup::OnFocus();
    }

    void OnBlur() override
    {
        if (renderNode_) {
            renderNode_->ChangeStatus(RenderStatus::BLUR);
        }
        FocusGroup::OnBlur();
    }

    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override
    {
        return false;
    }

    bool IsFocusable() const override
    {
        return FocusNode::IsFocusable();
    }

    bool AcceptFocusByRectOfLastFocus(const Rect& rect) override
    {
        return FocusNode::AcceptFocusByRectOfLastFocus(rect);
    }

    RefPtr<RenderNode> CreateRenderNode() override
    {
        auto tabBarItemComponent = AceType::DynamicCast<TabBarItemComponent>(component_);
        return tabBarItemComponent ? tabBarItemComponent->CreateRenderNode() : nullptr;
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_ITEM_ELEMENT_H
