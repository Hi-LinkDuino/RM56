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

#include "core/components/list/list_component.h"

#include "frameworks/core/components/foreach/for_each_component.h"
#include "frameworks/core/components/ifelse/if_else_component.h"

namespace OHOS::Ace {

ListComponent::ListComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    needPreBuild_ = true;
}

RefPtr<Element> ListComponent::CreateElement()
{
    // This func will be called repeatedly when list in dialog.
    needUpdateElement_ = true;
    auto listElement = AceType::MakeRefPtr<ListElement>();
    listElement_ = AceType::WeakClaim(AceType::RawPtr(listElement));
    return listElement;
}

RefPtr<RenderNode> ListComponent::CreateRenderNode()
{
    return RenderList::Create();
}

void ListComponent::InitStyle(const RefPtr<ListTheme>& theme)
{
    if (theme) {
        gradientWidth_ = theme->GetGradientWidth();
        backgroundColor_ = theme->GetBackgroundColor();
    }
}

void ListComponent::SetFlexAlign(FlexAlign flexAlign)
{
    if (flexAlign < FlexAlign::FLEX_START || flexAlign > FlexAlign::STRETCH) {
        LOGW("Invalid flexAlign %{public}d", flexAlign);
        return;
    }
    LOGD("SetFlexAlign to %{public}d.", flexAlign);
    flexAlign_ = flexAlign;
}

void ListComponent::SetColumnCount(int32_t count)
{
    if (count <= 0) {
        LOGW("Invalid ColumnCount %{public}d", count);
        return;
    }
    LOGD("SetColumnCount to %{public}d.", count);
    columnCount_ = count;
}

void ListComponent::SetColumnExtent(int32_t extent)
{
    if (extent <= 0) {
        return;
    }
    LOGD("SetColumnExtent to %{public}d.", extent);
    columnExtent_ = extent;
}

void ListComponent::SetItemExtent(const Dimension& itemExtent)
{
    if (!itemExtent.IsValid()) {
        return;
    }
    LOGD("SetItemExtent to [%{public}d:%{public}lf].", itemExtent.Unit(), itemExtent.Value());
    itemExtent_ = itemExtent;
}

void ListComponent::SetWidth(double width)
{
    if (width <= 0.0) {
        LOGW("Invalid Width %{public}lf", width);
        return;
    }
    width_ = width;
}

void ListComponent::SetHeight(double height)
{
    if (height <= 0.0) {
        LOGW("Invalid Height %{public}lf", height);
        return;
    }
    height_ = height;
}

void ListComponent::InsertChild(int32_t position, const RefPtr<Component>& child)
{
    if (position < 0) {
        LOGE("InsertChild: the position is negative");
        return;
    }
    auto pos = static_cast<uint32_t>(position);
    auto item = ListItemComponent::GetListItem(child);
    if (!item) {
        LOGE("InsertChild: no list item in child");
        return;
    }

    const auto& children = GetChildren();
    if (!child || pos > children.size()) {
        return;
    }

    if (pos == children.size()) {
        AppendChild(child);
        return;
    }

    int32_t current = 0;
    auto it = children.begin();
    while (it != children.end()) {
        auto listItemComponent = ListItemComponent::GetListItem(*it);
        if (listItemComponent && listItemComponent->GetOperation() == LIST_ITEM_OP_REMOVE) {
            --current;
        }

        if (current == static_cast<int32_t>(pos)) {
            break;
        }

        ++current;
        ++it;
    }

    LOGD("InsertChild: position: %{public}d, index: %{public}d, type: %{public}s", pos, item->GetIndex(),
        item->GetType().c_str());
    item->SetOperation(LIST_ITEM_OP_ADD);
    needUpdateElement_ = true;
    ComponentGroup::InsertChild(std::distance(children.begin(), it), child);
}

void ListComponent::AppendChild(const RefPtr<Component>& child)
{
    auto item = ListItemComponent::GetListItem(child);
    if (!item) {
        auto multiComposed = AceType::DynamicCast<MultiComposedComponent>(child);
        if (!multiComposed) {
            return;
        }

        for (const auto& childComponent : multiComposed->GetChildren()) {
            AppendChild(childComponent);
        }
        return;
    }
    if (needDivider_) {
        item->MarkNeedDivider(needDivider_);
        item->SetDividerColor(dividerColor_);
        item->SetDividerHeight(dividerHeight_);
        item->SetDividerLength(dividerLength_);
        item->SetDividerOrigin(dividerOrigin_);
    }

    item->SetIndex(GetChildren().size());
    LOGD("AppendChild: index: %{public}d, type: %{public}s", item->GetIndex(), item->GetType().c_str());
    item->SetOperation(LIST_ITEM_OP_ADD);

    needUpdateElement_ = true;
    // Version 1.0 will wrap ComposedComponent for ListItem by DomNode.
    if (AceType::InstanceOf<ComposedComponent>(child)) {
        ComponentGroup::AppendChild(child);
    } else {
        ComponentGroup::AppendChild(
            AceType::MakeRefPtr<ComposedComponent>((const char*)&item, "ListItemWrapper", child));
    }
}

void ListComponent::RemoveChild(const RefPtr<Component>& child)
{
    auto item = ListItemComponent::GetListItem(child);
    if (!item) {
        LOGE("RemoveChild: no list item in child");
        return;
    }

    LOGD("RemoveChild: index: %{public}d, type: %{public}s", item->GetIndex(), item->GetType().c_str());
    if (beginIndex_ != LIST_PARAM_INVAID && endIndex_ != LIST_PARAM_INVAID) {
        ComponentGroup::RemoveChild(child);
        needUpdateElement_ = true;
    } else {
        if (item->GetOperation() == LIST_ITEM_OP_ADD) {
            LOGD("RemoveChild: this child is added just before");
            ComponentGroup::RemoveChild(child);
            return;
        }
        item->SetOperation(LIST_ITEM_OP_REMOVE);
        needUpdateElement_ = true;
    }
}

void ListComponent::SetGroupState(int32_t expandIndex, bool expand)
{
    auto listElement = listElement_.Upgrade();
    if (!listElement) {
        LOGE("SetGroupState failed");
        return;
    }
    listElement->SetGroupState(expandIndex, expand);
}

} // namespace OHOS::Ace
