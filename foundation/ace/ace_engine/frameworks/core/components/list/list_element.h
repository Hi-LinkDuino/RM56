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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ELEMENT_H

#include <map>

#include "core/components/list/render_list.h"
#include "core/pipeline/base/component_group_element.h"

namespace OHOS::Ace {

class ListElement : public ComponentGroupElement, public FocusGroup {
    DECLARE_ACE_TYPE(ListElement, ComponentGroupElement, FocusGroup);

public:
    void PerformBuild() override;
    void Update() override;

    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;

    void MoveItemToViewPort(double position);

    void MoveItemGroupToViewPort(double position, double size);

    void SetGroupState(int32_t expandIndex, bool expand);

    void AddItemGroupFocusIndex(int32_t groupIndex, int32_t groupFocusIndex);

    int32_t GetItemGroupFocusIndex(int32_t groupIndex);

    void NeedMoveFocusItem(bool need)
    {
        needMoveFocusItem_ = need;
    }

private:
    RefPtr<RenderNode> CreateRenderNode() override;
    void ApplyRenderChild(const RefPtr<RenderElement>& renderChild) override;

    bool RecycleItem(int32_t index);
    void RecycleByRange(int32_t& from, int32_t& to);
    void RecycleByItems(const std::vector<int32_t>& items);
    void ReleaseRecycledListItem(int32_t from, int32_t to);

    bool BuildListDataFromChild(int32_t index);
    bool BuildDetachedFocusComponent();
    void PreBuildListItems(int32_t index, const std::list<RefPtr<Component>>& newComponent, int32_t from);
    void RetrieveListData(int32_t beginIndex, int32_t endIndex);
    bool BuildListData(int32_t index);
    bool BuildListComponent(const RefPtr<Component>& component);
    void UpdateListItemElement(const RefPtr<Component>& component);
    RefPtr<RenderNode> BuildStickyItem(int32_t index, bool next);
    int32_t SearchStickyItem(int32_t index);
    bool SupportStickyItem() const;
    void ResetStickyItem();
    void InitStickyFunc();

    RefPtr<Element> GetCachedElement(const std::string& type);
    bool RemoveFromCache(const RefPtr<Element>& item);

    void UpdateListElement();
    void GetRefreshItems(bool& rebuild, int32_t& index);
    void RebuildElements(int32_t tailIndex);
    void PatchElements();
    void OnRefreshed();

    void UpdateCachedComponent();
    int32_t AddToCache(const RefPtr<Component>& item, int32_t index, bool isDynamic = false);
    void RemoveComposedChildFromMap(RefPtr<Element> element);

    RefPtr<RenderList> renderList_;

    using RequestItem = std::function<void(const std::string&, std::string&)>;
    RequestItem requestItem_;
    using RequestItemAsync = std::function<void(const std::string&)>;
    RequestItemAsync requestItemAsync_;

    int32_t maxCount_ = 0;
    int32_t cachedCount_ = 0;
    int32_t beginIndex_ = LIST_PARAM_INVAID;
    int32_t endIndex_ = LIST_PARAM_INVAID;
    int32_t repeatedLength_ = 0;
    // For the case of dynamically loading list-items, length_ = indexOffset_ + repeatedLength_ + tailLength_;
    int32_t length_ = 0;
    int32_t indexOffset_ = 0;
    int32_t tailLength_ = 0;
    bool accessibilityDisabled_ = false;

    int32_t preBuildCount_ = 0;
    std::list<RefPtr<Component>> newListItems_;
    std::map<int32_t, RefPtr<Component>> newListItemsMap_;
    std::vector<RefPtr<Component>> needRefreshItems_;
    std::vector<RefPtr<Component>> itemComponents_;
    std::pair<int32_t, int32_t> itemVectorHit_ {-1, -1};
    RefPtr<Component> listComponent_;
    bool needRefresh_ = false;
    bool isJsCard_ = false;

    size_t bucketSize_ = 6; // default cache size
    bool building_ = false;
    std::map<int32_t, RefPtr<Element>> itemElements_;
    std::map<std::string, std::list<RefPtr<Element>>> cacheBuckets_;

    bool needMoveFocusItem_ = false;
    RefPtr<Element> stickyElement_;
    RefPtr<Element> stickyNextElement_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_ELEMENT_H
