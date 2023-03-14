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

#include "core/components/indexer/indexer_list_component.h"

#include "core/components/indexer/indexer_list_element.h"
#include "core/components/indexer/render_indexer_list.h"

namespace OHOS::Ace {

RefPtr<Element> Ace::IndexerListComponent::CreateElement()
{
    return AceType::MakeRefPtr<IndexerListElement>();
}

RefPtr<RenderNode> IndexerListComponent::CreateRenderNode()
{
    return AceType::MakeRefPtr<RenderIndexerList>();
}

void IndexerListComponent::InsertChild(uint32_t position, const RefPtr<Component>& child)
{
    // the list component will sort all of the items automatically
    AppendChild(child);
}

void IndexerListComponent::AppendChild(const RefPtr<Component>& child)
{
    if (!list_ || !indexer_) {
        LOGE("[indexer] listData or indexer is NULL.");
        return;
    }
    RefPtr<ComposedComponent> composedItem = AceType::DynamicCast<ComposedComponent>(child);
    if (composedItem && !composedItem->IsInspector()) {
        RefPtr<ListItemComponent> listItem = AceType::DynamicCast<ListItemComponent>(composedItem->GetChild());
        if (listItem) {
            int32_t itemIndex = indexer_->AddItemIndexKey(listItem->GetIndexKey(), listItem->GetType());
            LOGD("[indexer] AppendChild key:%{public}s, index:%{public}d", listItem->GetIndexKey().c_str(), itemIndex);
            list_->InsertChild(itemIndex, child);
        }
    }
}

void IndexerListComponent::RemoveChild(const RefPtr<Component>& child)
{
    if (!list_ || !indexer_) {
        LOGE("[indexer] listData or indexer is NULL.");
        return;
    }
    RefPtr<ComposedComponent> composedItem = AceType::DynamicCast<ComposedComponent>(child);
    if (composedItem && !composedItem->IsInspector()) {
        RefPtr<ListItemComponent> listItem = AceType::DynamicCast<ListItemComponent>(composedItem->GetChild());
        if (listItem) {
            indexer_->RemoveItemIndexKey(listItem->GetIndexKey());
            list_->RemoveChild(child);
        }
    }
}

void IndexerListComponent::BuildChildren()
{
    std::list<RefPtr<Component>> children;
    children.emplace_back(list_);
    children.emplace_back(indexer_);

    LOGD("[indexer] SetRightToLeft: %d", isRightToLeft_);
    if (isRightToLeft_) {
        stack_ = AceType::MakeRefPtr<StackComponent>(Alignment::CENTER_LEFT, StackFit::KEEP, Overflow::CLIP, children);
    } else {
        stack_ = AceType::MakeRefPtr<StackComponent>(Alignment::CENTER_RIGHT, StackFit::KEEP, Overflow::CLIP, children);
    }
    SetChild(stack_);
}

} // namespace OHOS::Ace
