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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MENU_MENU_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MENU_MENU_ELEMENT_H

#include "core/components/box/render_box.h"
#include "core/components/menu/menu_component.h"
#include "core/components/select_popup/select_popup_component.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

class MenuElement : public ComposedElement {
    DECLARE_ACE_TYPE(MenuElement, ComposedElement);

public:
    explicit MenuElement(const ComposeId& id) : ComposedElement(id) {}
    ~MenuElement() override = default;

    void PerformBuild() override;

    void SetMenuShow(bool status)
    {
        menuShow_ = status;
    }

    bool GetMenuShow()
    {
        return menuShow_;
    }

    std::vector<RefPtr<OptionComponent>> GetSelectOptions()
    {
        if (data_) {
            return data_->GetOptions();
        }
        return std::vector<RefPtr<OptionComponent>>();
    }

private:
    void OnTargetCallback(const ComposeId& id, const Offset& point);
    void OnTargetContextCallback(const ComposeId& id, const Offset& point);
    void OnOptionCallback(std::size_t index);
    void OnCanceledCallback();
    RefPtr<RenderBox> GetBoxRenderChild(const RefPtr<Element>& element);

    RefPtr<SelectPopupComponent> popup_;
    RefPtr<MenuComponent> data_;
    std::function<void(const std::string&)> jsSuccessCallback_;
    std::function<void(const std::string&)> jsCancelCallback_;
    bool menuShow_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MENU_MENU_ELEMENT_H
