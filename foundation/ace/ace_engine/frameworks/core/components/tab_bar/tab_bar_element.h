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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_ELEMENT_H

#include "core/components/box/box_component.h"
#include "core/components/box/box_element.h"
#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/tab_bar/tab_controller.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/component_group_element.h"

namespace OHOS::Ace {

class TabBarElement : public ComponentGroupElement, public FocusGroup {
    DECLARE_ACE_TYPE(TabBarElement, ComponentGroupElement, FocusGroup);

public:
    void UpdateIndex(int32_t index);
    void PerformBuild() override;
    void Update() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;

protected:
    void OnFocus() override;
    void OnBlur() override;

private:
    RefPtr<RenderNode> CreateRenderNode() override;
    void UpdateElement(int32_t index);

    bool vertical_ { false }; // the tab is vertical or not, default value is false
    std::list<RefPtr<TabBarItemComponent>> tabs_;
    RefPtr<TabController> controller_;
    RefPtr<BoxComponent> indicatorStyle_;
    RefPtr<BoxComponent> focusIndicatorStyle_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_ELEMENT_H
