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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_ELEMENT_H

#include "core/components/navigation_bar/navigation_bar_component.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

class NavigationBarElement : public ComposedElement, public FocusGroup {
    DECLARE_ACE_TYPE(NavigationBarElement, ComposedElement, FocusGroup);

public:
    explicit NavigationBarElement(const ComposeId& id) : ComposedElement(id) {}
    ~NavigationBarElement() override = default;

    void PerformBuild() override;

protected:
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override
    {
        return false;
    };

    void OnFocus() override
    {
        auto context = GetContext().Upgrade();
        if (context) {
            context->SetUseRootAnimation(true);
        }
        FocusGroup::OnFocus();
    }

    void OnBlur() override
    {
        auto context = GetContext().Upgrade();
        if (context) {
            context->SetUseRootAnimation(false);
        }
        FocusGroup::OnBlur();
    }

private:
#ifndef WEARABLE_PRODUCT
    EventMarker moreClickMarker_;
    void BindMoreButtonClickEvent(const RefPtr<NavigationBarComponent>& navigationBar);
    void BindClickEventToOptions(const RefPtr<NavigationBarComponent>& navigationBar);
#endif

    std::function<void(const std::string&)> menuSelectedEvent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_ELEMENT_H
