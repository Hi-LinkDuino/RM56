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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_TABS_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_TABS_COMPOSED_ELEMENT_H

#include "core/components/box/render_box.h"
#include "core/components/flex/render_flex.h"
#include "core/components/tab_bar/render_tab_bar.h"
#include "core/components/tab_bar/render_tab_content.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/components_v2/tabs/tabs_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT TabsComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(TabsComposedElement, InspectorComposedElement)

public:
    explicit TabsComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~TabsComposedElement() override = default;

    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;

    std::string GetBarPosition() const;
    std::string GetIndex() const;
    std::string GetVertical() const;
    bool GetScrollable() const override;
    std::string GetBarMode() const;
    std::string GetBarWidth() const;
    std::string GetBarHeight() const;

    AceType::IdType GetTargetTypeId() const override
    {
        return TabsElement::TypeId();
    }

private:
    RefPtr<RenderFlex> GetRenderFlex() const;
    RefPtr<RenderTabBar> GetRenderTabBar(const RefPtr<RenderNode>& node) const;
    RefPtr<RenderTabContent> GetRenderTabContent(const RefPtr<RenderNode>& node) const;
    RefPtr<RenderBox> GetRenderBox(const RefPtr<RenderNode>& node) const;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_TABS_COMPOSED_ELEMENT_H