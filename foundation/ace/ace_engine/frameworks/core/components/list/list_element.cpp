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

#include "core/components/list/list_element.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/common/frontend.h"
#include "core/components/list/list_component.h"
#include "core/components/list/list_item_component.h"
#include "core/components/list/list_item_element.h"
#include "core/components/list/render_list.h"
#include "core/components/list/render_list_item.h"
#include "core/components/proxy/render_item_proxy.h"
#include "core/components/scroll/render_multi_child_scroll.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {
namespace {

std::string BuildEventParam(int32_t beginIndex, int32_t endIndex)
{
    return std::string("{\"begin\":")
        .append(std::to_string(beginIndex))
        .append(",\"end\":")
        .append(std::to_string(endIndex))
        .append("}");
}

} // namespace

RefPtr<RenderNode> ListElement::CreateRenderNode()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is nullptr!");
        return nullptr;
    }
    isJsCard_ = context->IsJsCard();
    RefPtr<RenderNode> node = ComponentGroupElement::CreateRenderNode();
    renderList_ = AceType::DynamicCast<RenderList>(node);
    auto multiChildScroll = AceType::DynamicCast<RenderMultiChildScroll>(RenderMultiChildScroll::Create());
    multiChildScroll->AddChild(renderList_);
    renderList_->RegisterRequestItemsCallback(
        [weakListElement = AceType::WeakClaim(this)](int32_t index, int32_t count) {
            auto listElement = weakListElement.Upgrade();
            if (listElement) {
                listElement->RetrieveListData(index, count);
            }
        });

    renderList_->RegisterRecycleByRangeCallback(
        [weakListElement = AceType::WeakClaim(this)](int32_t& from, int32_t& to) -> bool {
            auto listElement = weakListElement.Upgrade();
            if (!listElement) {
                LOGE("list element is nullptr");
                return false;
            }
            listElement->RecycleByRange(from, to);
            return true;
        });

    renderList_->RegisterRecycleByItemsCallback(
        [weakListElement = AceType::WeakClaim(this)](const std::vector<int32_t>& items) -> bool {
            auto listElement = weakListElement.Upgrade();
            if (listElement) {
                listElement->RecycleByItems(items);
                return true;
            }
            return false;
        });

    renderList_->RegisterBuildItemCallback(
        [weakListElement = AceType::WeakClaim(this)](int32_t index) -> bool {
            auto listElement = weakListElement.Upgrade();
            if (listElement) {
                return listElement->BuildListData(index);
            }
            return false;
        });

    renderList_->RegisterOnRefreshedCallback(
        [weakListElement = AceType::WeakClaim(this)]() {
            auto listElement = weakListElement.Upgrade();
            if (listElement) {
                listElement->OnRefreshed();
            }
        });

    InitStickyFunc();

    return multiChildScroll;
}

void ListElement::RetrieveListData(int32_t beginIndex, int32_t endIndex)
{
    if (requestItemAsync_ && !building_) {
        std::string command("\"");
        command.append(LIST_EVENT_REQUEST_ITEM);
        command.append("\",");
        command.append(BuildEventParam(beginIndex, endIndex));
        requestItemAsync_(command);
        building_ = true;
    }

    if (endIndex > 0) {
        std::string param = BuildEventParam(beginIndex, endIndex);
        LOGD("RetrieveListData [begin = %{public}d, end = %{public}d)", beginIndex, endIndex);
        std::string result;
        if (requestItem_) {
            requestItem_(param, result);
        }
    }
}

bool ListElement::BuildListDataFromChild(int32_t index)
{
    if (beginIndex_ != LIST_PARAM_INVAID && endIndex_ != LIST_PARAM_INVAID) {
        if (index > itemVectorHit_.first) {
            for (int32_t position = itemVectorHit_.second + 1; position < (int32_t)itemComponents_.size(); position++) {
                auto itemComponent = ListItemComponent::GetListItem(itemComponents_[position]);
                if (!itemComponent) {
                    LOGE("itemComponent exist but is null");
                    return false;
                }
                if (index == itemComponent->GetIndex()) {
                    itemVectorHit_.first = index;
                    itemVectorHit_.second = position;
                    return BuildListComponent(itemComponents_[position]);
                }
            }
        } else {
            for (int position = itemVectorHit_.second; position >= 0; position--) {
                auto itemComponent = ListItemComponent::GetListItem(itemComponents_[position]);
                if (!itemComponent) {
                    LOGE("itemComponent exist but is null");
                    return false;
                }
                if (index == itemComponent->GetIndex()) {
                    itemVectorHit_.first = index;
                    itemVectorHit_.second = position;
                    return BuildListComponent(itemComponents_[position]);
                }
            }
        }
        LOGW("list-item component (index=%{public}d) not in cache!", index);
        return false;
    }
    // now just for prebuild more items.
    preBuildCount_ = static_cast<int32_t>(itemComponents_.size());
    if (index >= preBuildCount_ - cachedCount_ && requestItemAsync_) {
        RetrieveListData(0, preBuildCount_ + cachedCount_);
    }

    if (index < 0 || index >= preBuildCount_) {
        LOGE("invalid index: %{public}d, size: %{public}d", index, preBuildCount_);
        return false;
    }

    auto item = itemComponents_[index];
    return BuildListComponent(item);
}

bool ListElement::BuildListData(int32_t index)
{
    if (BuildListDataFromChild(index)) {
        return true;
    }

    bool result = false;
    auto iter = newListItemsMap_.find(index);
    if (iter != newListItemsMap_.end()) {
        auto component = iter->second;
        newListItemsMap_.erase(iter);
        result = BuildListComponent(component);
    }

    return result;
}

void ListElement::InitStickyFunc()
{
    renderList_->RegisterStickyItemBuilderCallback(
        [weakListElement = AceType::WeakClaim(this)](int32_t index, bool next) -> RefPtr<RenderNode> {
            auto listElement = weakListElement.Upgrade();
            if (listElement) {
                return listElement->BuildStickyItem(index, next);
            } else {
                return nullptr;
            }
        });

    renderList_->RegisterStickyItemSearcherCallback(
        [weakListElement = AceType::WeakClaim(this)](int32_t index) -> int32_t {
            auto listElement = weakListElement.Upgrade();
            if (listElement && listElement->SupportStickyItem()) {
                return listElement->SearchStickyItem(index);
            } else {
                return -1;
            }
        });
}

void ListElement::ResetStickyItem()
{
    if (!stickyElement_) {
        return;
    }
    auto sticky = stickyElement_->GetRenderNode()->GetParent().Upgrade();
    auto listItem = RenderListItem::GetRenderListItem(sticky);
    if (listItem) {
        listItem->SetIndex(-1);
    }
}

RefPtr<RenderNode> ListElement::BuildStickyItem(int32_t index, bool next)
{
    RefPtr<Component> component;
    for (auto& item : itemComponents_) {
        auto itemComponent = ListItemComponent::GetListItem(item);
        if (itemComponent && itemComponent->GetIndex() == index) {
            component = item;
            break;
        }
    }
    if (!component) {
        LOGE("Build sticky item for index:%{public}d next:%{public}d failed", index, next);
        return nullptr;
    }

    RefPtr<Element> sticky;
    if (next) {
        stickyNextElement_ = UpdateChild(stickyNextElement_, component);
        if (!stickyNextElement_) {
            LOGE("get second sticky element failed.");
            return nullptr;
        }
        sticky = stickyNextElement_;
    } else {
        stickyElement_ = UpdateChild(stickyElement_, component);
        if (!stickyElement_) {
            LOGE("get first sticky element failed.");
            return nullptr;
        }
        sticky = stickyElement_;
    }
    RefPtr<RenderListItem> renderItem = AceType::DynamicCast<RenderListItem>(sticky->GetRenderNode());
    if (renderItem && renderList_) {
        renderItem->SetScrollController(renderList_->GetController());
        renderItem->SetClonedBySticky(true);
        renderItem->SetNeedUpdateAccessibility(false);
    }
    return sticky->GetRenderNode()->GetParent().Upgrade();
}

int32_t ListElement::SearchStickyItem(int32_t index)
{
    for (auto iter = itemComponents_.rbegin(); iter != itemComponents_.rend(); ++iter) {
        auto item = ListItemComponent::GetListItem(*iter);
        if (item && item->GetIndex() <= index && item->GetSticky()) {
            return item->GetIndex();
        }
    }
    return INVALID_INDEX;
}

bool ListElement::SupportStickyItem() const
{
    if (!renderList_) {
        LOGE("Render is null, do not support sticky item.");
        return false;
    }
    return renderList_->SupportStickyItem();
}

RefPtr<Element> ListElement::GetCachedElement(const std::string& type)
{
    // Find same type element.
    auto bucket = cacheBuckets_.find(type);

    RefPtr<Element> element;
    if (bucket != cacheBuckets_.end() && !bucket->second.empty()) {
        element = bucket->second.front();
        bucket->second.pop_front();
        LOGD("[recycle] pop element from bucket. now size: %{public}zu", bucket->second.size());
    }

    return element;
}

void ListElement::RemoveComposedChildFromMap(RefPtr<Element> element)
{
    // Remove all composed element children in list-item from pipeline context composed element map.
    // Make sure updating list-item can only be done by using list-item composed id.
    if (element) {
        const auto& children = element->GetChildren();
        for (const auto& child : children) {
            const auto& composedChild = AceType::DynamicCast<ComposedElement>(child);
            if (composedChild) {
                composedChild->Detached();
            }
            RemoveComposedChildFromMap(child);
        }
    }
}

void ListElement::UpdateListItemElement(const RefPtr<Component>& component)
{
    auto itemComponent = ListItemComponent::GetListItem(component);
    if (!itemComponent) {
        LOGE("itemComponent exist but is null");
        return;
    }

    // Update New Component to Element.
    int32_t index = itemComponent->GetIndex();
    RefPtr<Element> element;
    auto item = itemElements_.find(index);
    if (item != itemElements_.end() && item->second) {
        element = item->second;
    }
    if (element) {
        auto itemElement = ListItemElement::GetListItem(element);
        if (itemElement->GetKey() == -1 || itemElement->GetKey() != itemComponent->GetKey()) {
            UpdateChild(element, component);
            if (accessibilityDisabled_) {
                auto renderNode = element->GetRenderNode();
                if (renderNode) {
                    renderNode->SetNeedUpdateAccessibility(false);
                }
            }
        }
    }
}

bool ListElement::BuildListComponent(const RefPtr<Component>& component)
{
    auto itemComponent = ListItemComponent::GetListItem(component);
    if (!itemComponent) {
        LOGE("itemComponent exist but is null");
        return false;
    }

    RefPtr<Element> element;
    int32_t index = itemComponent->GetIndex();

    if (itemComponent->TestFlag(LIST_ITEM_FLAG_FROM_CHILD)) {
        auto item = itemElements_.find(index);
        if (item != itemElements_.end()) {
            element = item->second;
        }
    }

    if (!element) {
        element = GetCachedElement(itemComponent->GetType());
    }

    if (element) {
        Element::AddChild(element);
        if (!element->GetRenderNode()) {
            LOGW("no render node in this recycled element");
            element = nullptr;
        }
    }

    LOGD("build item index:%{public}d type:%{public}s", index, itemComponent->GetType().c_str());
    element = UpdateChild(element, component);
    if (itemComponent->TestFlag(LIST_ITEM_FLAG_DYNAMIC)) {
        RemoveComposedChildFromMap(element);
    }

    if (!element) {
        LOGE("no element found!");
        return false;
    }

    // Get element proxy.
    auto parent = element->GetRenderNode()->GetParent();
    auto itemProxy = parent.Upgrade();
    if (!itemProxy) {
        LOGE("itemProxy is null");
        return false;
    }

    // Add list item element to focus tree.
    auto itemElement = ListItemElement::GetListItem(element);
    if (itemElement) {
        itemElement->AddToFocus();
    }

    itemElements_[index] = element;
    renderList_->AddListItem(index, itemProxy);
    itemProxy->SetHidden(false);
    if (accessibilityDisabled_) {
        auto renderNode = element->GetRenderNode();
        if (renderNode) {
            renderNode->SetNeedUpdateAccessibility(false);
        }
    }
    // recover visible state.
    if (itemProxy->GetVisible() != GetRenderNode()->GetVisible()) {
        itemProxy->SetVisible(GetRenderNode()->GetVisible());
    }

    // refresh focus
    auto pipelineContext = context_.Upgrade();
    if (pipelineContext) {
        pipelineContext->RefreshStageFocus();
    }

    return true;
}

void ListElement::PreBuildListItems(int32_t index, const std::list<RefPtr<Component>>& newComponent, int32_t from)
{
    for (const auto& child : newComponent) {
        if (index >= preBuildCount_) {
            auto itemComponent = ListItemComponent::GetListItem(child);
            if (!itemComponent) {
                LOGE("cast to ListItemComponent failed");
                continue;
            }
            itemComponent->SetIndex(index);
            itemComponent->SetFlags(from);
            newListItemsMap_.emplace(std::make_pair(index, itemComponent));
        }
        index++;
    }
    LOGD("PreBuildListItems: index:%{public}d, count:%{public}d", index, preBuildCount_);
    preBuildCount_ = index;
}

void ListElement::ReleaseRecycledListItem(int32_t from, int32_t to)
{
    for (int32_t i = from; i <= to; ++i) {
        auto item = itemElements_.find(i);
        if (item == itemElements_.end()) {
            continue;
        }

        auto itemElement = ListItemElement::GetListItem(item->second);
        if (!itemElement) {
            LOGW("not item element(%{public}s)", AceType::TypeName(item->second));
            continue;
        }

        RefPtr<RenderNode> proxyNode;
        auto renderNode = item->second->GetRenderNode();
        if (renderNode) {
            proxyNode = renderNode->GetParent().Upgrade();
        }

        if (!proxyNode) {
            LOGW("Proxy node is null.");
            continue;
        }

        // RemoveChild will reset render node in this element.
        // Here we save and restore its render node for recycling.
        Element::RemoveChild(item->second);
        if (!itemElement->IsCurrentFocus()) {
            // Remove list item element from focus tree.
            auto focusNode = AceType::DynamicCast<FocusNode>(itemElement);
            if (focusNode) {
                focusNode->RemoveSelf();
            }
        }
        proxyNode->Unmount();
        itemElement->AttachRenderNode(proxyNode);

        // Hidden the release node for stop it's layout and paint.
        proxyNode->SetHidden(true);
        itemElements_.erase(item);
    }
}

void ListElement::RecycleByItems(const std::vector<int32_t>& items)
{
    if (!itemElements_.empty()) {
        for (auto item : items) {
            if (RecycleItem(item)) {
                ReleaseRecycledListItem(item, item);
            }
        }
    }

    std::map<int32_t, RefPtr<Element>> newItems;
    auto iter = itemElements_.begin();
    while (iter != itemElements_.end()) {
        auto item = iter->second;
        auto proxyNode = item->GetRenderNode();
        auto listItemNode = RenderListItem::GetRenderListItem(proxyNode);
        if (listItemNode) {
            int32_t index = listItemNode->GetIndex();
            auto listItemElement = ListItemElement::GetListItem(item);
            if (listItemElement) {
                listItemElement->SetIndex(index);
            }
            newItems.emplace(std::make_pair(index, item));
        }

        ++iter;
    }

    itemElements_.clear();
    itemElements_.swap(newItems);
}

bool ListElement::RemoveFromCache(const RefPtr<Element>& item)
{
    auto itemElement = ListItemElement::GetListItem(item);
    if (!itemElement) {
        return false;
    }

    auto bucket = cacheBuckets_.find(itemElement->GetItemType());
    if (bucket == cacheBuckets_.end()) {
        return false;
    }

    auto& elements = bucket->second;
    elements.remove(item);
    return true;
}

bool ListElement::RecycleItem(int32_t index)
{
    auto element = itemElements_.find(index);
    if (element == itemElements_.end()) {
        return false;
    }
    auto itemElement = ListItemElement::GetListItem(element->second);
    if (!itemElement) {
        return false;
    }
    itemElement->SetKey(-1);
    auto bucket = cacheBuckets_.find(itemElement->GetItemType());
    if (bucket != cacheBuckets_.end()) {
        auto& elements = bucket->second;
        size_t size = elements.size();
        if (size < bucketSize_) {
            LOGD("[recycle] bucket size is: %{public}zu, add it", size);
            elements.emplace_back(element->second);
        } else {
            LOGD("[recycle] recycle element: bucket size is too large: %{public}zu, drop it", size);
        }
    } else {
        std::list<RefPtr<Element>> newBucket { element->second };
        cacheBuckets_.emplace(std::make_pair(itemElement->GetItemType(), newBucket));
        LOGD("[recycle] recycle element: new bucket, type: %{public}s", itemElement->GetItemType().c_str());
    }
    return true;
}

void ListElement::RecycleByRange(int32_t& from, int32_t& to)
{
    int32_t firstRecycled = -1;
    int32_t lastRecycled = to;
    LOGD("[recycle] recycle list items from: %{public}d, to: %{public}d", from, to);

    if (!itemElements_.empty()) {
        int32_t start = std::max(itemElements_.begin()->first, from);
        int32_t end = std::min(itemElements_.rbegin()->first, to);
        for (int32_t i = start; i <= end; ++i) {
            if (!RecycleItem(i)) {
                continue;
            }
            if (firstRecycled < 0) {
                firstRecycled = i;
            }
            lastRecycled = i;
        }
    }

    // Release all recycled items.
    from = firstRecycled;
    to = lastRecycled;
    if (from >= 0) {
        ReleaseRecycledListItem(from, to);
    }
}

void ListElement::OnRefreshed()
{
    needRefresh_ = false;
    RefPtr<ComponentGroup> group = AceType::DynamicCast<ComponentGroup>(listComponent_);
    if (!group || group->GetChildren().empty()) {
        listComponent_ = nullptr;
        return;
    }

    int32_t index = 0;
    auto children = group->GetChildren();
    auto iter = children.begin();
    while (iter != children.end()) {
        auto listItemComponent = ListItemComponent::GetListItem(*iter);
        if (listItemComponent) {
            if (listItemComponent->GetOperation() == LIST_ITEM_OP_REMOVE) {
                group->ComponentGroup::RemoveChild(*iter);
            } else {
                listItemComponent->SetOperation(LIST_ITEM_OP_NONE);
                listItemComponent->RemoveFlag(LIST_ITEM_FLAG_IN_RANGE);
                listItemComponent->SetIndex(index++);
            }
        }
        ++iter;
    }

    listComponent_ = nullptr;

    for (auto& item : itemElements_) {
        auto listItemElement = ListItemElement::GetListItem(item.second);
        if (listItemElement) {
            listItemElement->RemoveFlag(LIST_ITEM_FLAG_IN_RANGE);
        }
    }
}

inline int32_t ListElement::AddToCache(const RefPtr<Component>& item, int32_t index, bool isDynamic)
{
    auto listItemComponent = ListItemComponent::GetListItem(item);
    if (listItemComponent && listItemComponent->GetOperation() != LIST_ITEM_OP_REMOVE) {
        listItemComponent->SetOperation(LIST_ITEM_OP_NONE);
        listItemComponent->RemoveFlag(LIST_ITEM_FLAG_IN_RANGE);
        listItemComponent->SetIndex(index);
        if (isDynamic) {
            listItemComponent->AddFlag(LIST_ITEM_FLAG_DYNAMIC);
        }
        itemComponents_.emplace_back(item);
        UpdateListItemElement(item);
        return index + 1;
    }
    return index;
}

void ListElement::UpdateCachedComponent()
{
    // Normally, components are not saved in element.
    // However, list-element may need caching list-item-components to improve the speed of sliding.
    RefPtr<ComponentGroup> group = AceType::DynamicCast<ComponentGroup>(component_);
    if (!group) {
        LOGW("invalid input component!");
        return;
    }
    if (beginIndex_ == LIST_PARAM_INVAID || endIndex_ == LIST_PARAM_INVAID || repeatedLength_ == LIST_PARAM_INVAID) {
        LOGW("invalid list parameter");
        return;
    }

    itemComponents_.clear();
    itemVectorHit_ = std::make_pair(-1, -1);
    auto children = group->GetChildren();

    children.sort([](const RefPtr<Component>& node1, const RefPtr<Component>& node2) {
        auto itemNode1 = ListItemComponent::GetListItem(node1);
        auto itemNode2 = ListItemComponent::GetListItem(node2);
        if (itemNode1 && itemNode2) {
            return itemNode1->GetKey() < itemNode2->GetKey();
        }
        return false;
    });

    auto child = children.begin();
    int32_t index = 0;
    for (index = 0; index < indexOffset_ && child != children.end(); ++child) {
        index = AddToCache(*child, index);
    }
    for (index = beginIndex_ + indexOffset_; index < endIndex_ + indexOffset_ && child != children.end(); ++child) {
        index = AddToCache(*child, index, true);
    }
    renderList_->SyncIndex(beginIndex_ + indexOffset_, endIndex_ + indexOffset_);
    maxCount_ = index;
    tailLength_ = 0;
    if (repeatedLength_ != LIST_LENGTH_INFINITE) {
        for (index = repeatedLength_ + indexOffset_; child != children.end(); ++child) {
            index = AddToCache(*child, index);
            ++tailLength_;
        }
        if (endIndex_ == repeatedLength_) {
            maxCount_ += tailLength_;
        }
        length_ = indexOffset_ + repeatedLength_ + tailLength_;
    } else {
        length_ = LIST_LENGTH_INFINITE;
    }
    renderList_->SetLength(length_);
}

void ListElement::UpdateListElement()
{
    bool rebuild = false;
    int32_t tailIndex = -1;
    if (needRefresh_) {
        renderList_->MarkNeedRefresh();
    }

    GetRefreshItems(rebuild, tailIndex);
    if (rebuild) {
        LOGD("rebuild elements");
        ResetStickyItem();
        RebuildElements(tailIndex);
        renderList_->MarkNeedRefresh();
        needRefresh_ = true;
    } else {
        LOGD("patch elements");
        PatchElements();
    }
}

void ListElement::GetRefreshItems(bool& rebuild, int32_t& tailIndex)
{
    needRefreshItems_.clear();
    RefPtr<ComponentGroup> group = AceType::DynamicCast<ComponentGroup>(component_);
    if (!group) {
        return;
    }

    int32_t currentMin = renderList_->GetCurrentMinIndex();
    int32_t currentMax = renderList_->GetCurrentMaxIndex() + 1;
    const auto& components = group->GetChildren();

    LOGD("currentMin: %{public}d, currentMax: %{public}d, components.size(): %{public}d", currentMin, currentMax,
        static_cast<int>(components.size()));
    int32_t head = 0;
    bool needRefresh = true;
    bool inRange = false;
    itemComponents_.clear();
    for (const auto& component : components) {
        auto item = ListItemComponent::GetListItem(component);
        if (!item) {
            LOGW("item is not list item component");
            continue;
        }

        int32_t op = item->GetOperation();
        if (op != LIST_ITEM_OP_REMOVE) {
            itemComponents_.emplace_back(component);
        }

        if (!needRefresh) {
            continue;
        }

        if (op != LIST_ITEM_OP_ADD) {
            if (item->GetIndex() == currentMax) {
                needRefresh = false;
            } else if (item->GetIndex() == currentMin) {
                inRange = true;
                tailIndex = head + currentMin - 1;
            }
        }

        if (!inRange) {
            if (op == LIST_ITEM_OP_ADD) {
                head++;
            } else if (op == LIST_ITEM_OP_REMOVE) {
                head--;
            }
            if (op != LIST_ITEM_OP_NONE) {
                needRefreshItems_.emplace_back(component);
                rebuild = true;
            }
        } else {
            tailIndex++;
            item->AddFlag(LIST_ITEM_FLAG_IN_RANGE);
            needRefreshItems_.emplace_back(component);
            if (op != LIST_ITEM_OP_NONE) {
                rebuild = true;
            }
        }
    }
    if (isJsCard_) {
        rebuild = true;
    }
    maxCount_ = static_cast<int32_t>(itemComponents_.size());
}

void ListElement::RebuildElements(int32_t tailIndex)
{
    LOGD("tailIndex: %{public}d", tailIndex);
    listComponent_ = component_;
    auto items = itemElements_;
    renderList_->RecycleAllChild();
    int32_t index = tailIndex;
    auto riter = needRefreshItems_.rbegin();
    while (index >= 0 && riter != needRefreshItems_.rend()) {
        RefPtr<Element> updatedChild;
        auto item = ListItemComponent::GetListItem(*riter);
        if (item) {
            if (item->TestFlag(LIST_ITEM_FLAG_IN_RANGE) && item->GetOperation() != LIST_ITEM_OP_ADD) {
                RefPtr<Element> oldElement;
                auto oldElementIter = items.find(item->GetIndex());
                if (oldElementIter != items.end()) {
                    oldElement = oldElementIter->second;
                    RemoveFromCache(oldElement);
                }

                item->SetIndex(index);
                if (oldElement) {
                    Element::AddChild(oldElement);
                }
                updatedChild = UpdateChild(oldElement, *riter);
            } else {
                item->SetIndex(index);
                updatedChild = UpdateChild(nullptr, *riter);
            }

            // Get item proxy.
            auto parent = updatedChild->GetRenderNode()->GetParent();
            auto itemProxy = parent.Upgrade();
            if (itemProxy) {
                // Add list item element to focus tree.
                auto itemElement = ListItemElement::GetListItem(updatedChild);
                if (itemElement) {
                    itemElement->AddToFocus();
                }

                itemElements_[index] = updatedChild;
                renderList_->AddListItem(index, itemProxy);
                itemProxy->SetHidden(false);
                // recover visible state.
                if (itemProxy->GetVisible() != GetRenderNode()->GetVisible()) {
                    itemProxy->SetVisible(GetRenderNode()->GetVisible());
                }
            } else {
                LOGE("itemProxy is null");
            }
        }
        --index;
        ++riter;
    }
}

void ListElement::PatchElements()
{
    if (needRefreshItems_.empty()) {
        LOGE("need refresh is empty");
        return;
    }

    RefPtr<ComponentGroup> group = AceType::DynamicCast<ComponentGroup>(component_);
    if (!group || group->GetChildren().empty()) {
        return;
    }

    auto startItem = ListItemComponent::GetListItem(needRefreshItems_.front());
    if (!startItem) {
        LOGE("start item is not list item");
        return;
    }

    int32_t start = startItem->GetIndex();

    auto children = group->GetChildren();
    auto iter = children.begin();
    std::advance(iter, start);
    while (iter != children.end()) {
        auto listItemComponent = ListItemComponent::GetListItem(*iter);
        if (listItemComponent) {
            if (listItemComponent->GetOperation() == LIST_ITEM_OP_REMOVE) {
                group->RemoveChild(*iter);
            } else {
                listItemComponent->SetOperation(LIST_ITEM_OP_NONE);
                listItemComponent->RemoveFlag(LIST_ITEM_FLAG_IN_RANGE);
                listItemComponent->SetIndex(start++);
            }
        }
        ++iter;
    }
}

void ListElement::PerformBuild()
{
    building_ = false;
    LOGD("pre max: %{public}d, cur max: %{public}d", renderList_->GetMaxCount(), maxCount_);
    renderList_->SetMaxCount(maxCount_);
    if (beginIndex_ != LIST_PARAM_INVAID && endIndex_ != LIST_PARAM_INVAID) {
        if (endIndex_ == 0 && repeatedLength_ != LIST_PARAM_INVAID && repeatedLength_ != 0) {
            LOGD("request initial children, cacheCount: %{public}d", cachedCount_);
            RetrieveListData(0, cachedCount_);
        }
    } else {
        if (maxCount_ == 0) {
            LOGD("request initial children, cacheCount: %{public}d", cachedCount_);
            RetrieveListData(0, cachedCount_);
        }
    }
}

void ListElement::Update()
{
    RefPtr<ListComponent> list = AceType::DynamicCast<ListComponent>(component_);
    if (!list) {
        return;
    }

    auto context = context_.Upgrade();
    if (context && context->GetFrontend() && context->GetFrontendType() == FrontendType::JS) {
        requestItemAsync_ = AceAsyncEvent<void(const std::string&)>::Create(list->GetOnRequestItem(), context_);
    } else {
        requestItem_ = AceSyncEvent<void(const std::string&, std::string&)>::Create(list->GetOnRequestItem(), context_);
    }
    cachedCount_ = list->GetCachedCount();
    beginIndex_ = list->GetBeginIndex();
    endIndex_ = list->GetEndIndex();
    repeatedLength_ = list->GetRepeatedLength();
    indexOffset_ = list->GetIndexOffset();
    accessibilityDisabled_ = list->IsAccessibilityDisabled();

    if (beginIndex_ != LIST_PARAM_INVAID && endIndex_ != LIST_PARAM_INVAID) {
        UpdateCachedComponent();
        ComponentGroupElement::Update();
    } else {
        ComponentGroupElement::Update();
        if (list->NeedUpdateElement() || isJsCard_) {
            LOGD("update list element");
            UpdateListElement();
            list->MarkNeedUpdateElement(false);
        } else if (list->NeedPreBuild() && newListItemsMap_.empty()) {
            LOGD("update from json");
            RefPtr<ComponentGroup> group = AceType::DynamicCast<ComponentGroup>(component_);
            if (group) {
                const auto& children = group->GetChildren();
                PreBuildListItems(0, children, LIST_ITEM_FLAG_FROM_CHILD);
                maxCount_ = list->GetTotalCount();
            }
        }
    }
}

void ListElement::ApplyRenderChild(const RefPtr<RenderElement>& renderChild)
{
    if (!renderChild) {
        LOGE("Element child is null");
        return;
    }

    if (!renderList_) {
        LOGE("ListElement don't have a render list");
        return;
    }

    RefPtr<RenderNode> proxy;
    auto listItemElement = ListItemElement::GetListItem(renderChild);
    if (listItemElement) {
        proxy = listItemElement->GetProxyRenderNode();
    }
    if (!proxy) {
        proxy = RenderItemProxy::Create();
    }

    proxy->AddChild(renderChild->GetRenderNode());
    proxy->Attach(context_);
    renderList_->AddChild(proxy);
}

bool ListElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    LOGD("[ListFocus]RequestNextFocus Vertical:%{public}d, Reverse:%{public}d.", vertical, reverse);
    bool ret = false;
    while (!ret) {
        int32_t focusIndex = renderList_->RequestNextFocus(vertical, reverse);
        if (focusIndex < 0) {
            LOGW("No item can focus.");
            return false;
        }
        for (auto focusNode : GetChildrenList()) {
            auto listItem = AceType::DynamicCast<ListItemElement>(focusNode);
            if (listItem && listItem->GetIndex() == focusIndex) {
                // If current Node can not obtain focus, move to next.
                if (!focusNode->IsFocusable()) {
                    continue;
                }
                renderList_->CalculateFocusIndexPosition();
                ret = focusNode->RequestFocusImmediately();
                break;
            }
        }
    }
    return ret;
}

void ListElement::MoveItemToViewPort(double position)
{
    if (!needMoveFocusItem_) {
        return;
    }
    LOGD("[ListFocus]MoveItemToViewPort position:%{public}.1lf.", position);
    renderList_->MoveItemToViewPort(position);
    needMoveFocusItem_ = false;
}

void ListElement::MoveItemGroupToViewPort(double position, double size)
{
    if (!needMoveFocusItem_) {
        return;
    }
    LOGD("[ListFocus]MoveItemGroupToViewPort position:%{public}.1lf.", position);
    renderList_->MoveItemGroupToViewPort(position, size);
    needMoveFocusItem_ = false;
}

void ListElement::SetGroupState(int32_t expandIndex, bool expand)
{
    renderList_->SetGroupState(expandIndex, expand);
}

void ListElement::AddItemGroupFocusIndex(int32_t groupIndex, int32_t groupFocusIndex)
{
    renderList_->GetLayoutManager()->AddItemGroupFocusIndex(groupIndex, groupFocusIndex);
}

int32_t ListElement::GetItemGroupFocusIndex(int32_t groupIndex)
{
    return renderList_->GetLayoutManager()->GetItemGroupFocusIndex(groupIndex);
}

} // namespace OHOS::Ace
