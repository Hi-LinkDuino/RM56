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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_CONTENT_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_CONTENT_ELEMENT_H

#include <list>
#include <unordered_map>

#include "core/components/tab_bar/tab_controller.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/component_group_element.h"

namespace OHOS::Ace {

class TabContentElement : public ComponentGroupElement, public FocusGroup {
    DECLARE_ACE_TYPE(TabContentElement, ComponentGroupElement, FocusGroup);

public:
    explicit TabContentElement(const std::list<RefPtr<Component>>& contents);
    ~TabContentElement() override = default;

    void PerformBuild() override;
    void Update() override;
    void ChangeByContent(int32_t index);
    virtual void ChangeByBar(int32_t index, bool isFromController = false);
    virtual void PrepareContent(int32_t index);
    void ChangeDispatch(int32_t index);
    RefPtr<Element> GetTabContentChild(int32_t index) const;

    void OnFocus() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
    bool IsFocusable() const override;

protected:
    RefPtr<RenderNode> CreateRenderNode() override;
    void UpdateLastFocusNode();
    RefPtr<FocusNode> GetCurrentFocusNode() const;

    std::list<RefPtr<Component>> contents_;
    std::unordered_map<int32_t, RefPtr<Element>> childMap_;
    std::unordered_set<int32_t> focusIndexMap_;
    RefPtr<TabController> controller_;

    // new content index, requested by tab bar
    int32_t newBarIndex_ = -1;
    // new content index, requested by drag
    int32_t newIndex_ = -1;
    int32_t lastIndex_ = -1;
    bool fromController_ = false;
    bool newComponentBuild_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_CONTENT_ELEMENT_H
