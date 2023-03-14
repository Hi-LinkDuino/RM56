/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS__TAB_CONTENT_PROXY_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS__TAB_CONTENT_PROXY_ELEMENT_H

#include <list>
#include <unordered_map>

#include "core/components/tab_bar/tab_content_element.h"
#include "core/components/tab_bar/tab_controller.h"
#include "core/components_v2/common/element_proxy.h"
#include "core/focus/focus_node.h"

namespace OHOS::Ace::V2 {

class TabContentProxyElement : public TabContentElement, private ElementProxyHost {
    DECLARE_ACE_TYPE(TabContentElement, TabContentElement);

public:
    explicit TabContentProxyElement(const std::list<RefPtr<Component>>& contents) : TabContentElement(contents) {};
    ~TabContentProxyElement() override = default;

    void PerformBuild() override;
    void Update() override;

    void ChangeByBar(int32_t index, bool isFromController = false) override;
    void PrepareContent(int32_t index) override;

private:
    RefPtr<Element> OnUpdateElement(const RefPtr<Element>& element, const RefPtr<Component>& component) override;
    RefPtr<Component> OnMakeEmptyComponent() override;
    void OnDataSourceUpdated(size_t startIndex) override;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS__TAB_CONTENT_PROXY_ELEMENT_H
