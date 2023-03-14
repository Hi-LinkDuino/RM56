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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_LIST_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_LIST_ELEMENT_H

#include "base/utils/noncopyable.h"
#include "core/components_v2/common/element_proxy.h"
#include "core/components_v2/list/render_list.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace::V2 {

class ListElement : public RenderElement,
                    public FocusGroup,
                    public FlushEvent,
                    private ListItemGenerator,
                    private ElementProxyHost {
    DECLARE_ACE_TYPE(V2::ListElement, RenderElement, FocusGroup, FlushEvent);

public:
    ListElement();
    ~ListElement() override;

    void PerformBuild() override;

    RefPtr<RenderListItem> RequestListItem(size_t index) override;
    RefPtr<Element> GetListItemBySlot(size_t index);
    void RecycleListItem(size_t index) override;
    size_t TotalCount() override;
    size_t FindPreviousStickyListItem(size_t index) override;

    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
    void OnPostFlush() override;

private:
    RefPtr<Element> OnUpdateElement(const RefPtr<Element>& element, const RefPtr<Component>& component) override;
    RefPtr<Component> OnMakeEmptyComponent() override;
    void OnDataSourceUpdated(size_t startIndex) override;

    RefPtr<RenderNode> CreateRenderNode() override;
    void Apply(const RefPtr<Element>& element) override;

    RefPtr<RenderList> renderList_;
    size_t stickyRange_ = 0;

    ACE_DISALLOW_COPY_AND_MOVE(ListElement);
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_LIST_ELEMENT_H
