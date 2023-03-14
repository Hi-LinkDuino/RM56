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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MENU_MENU_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MENU_MENU_COMPONENT_H

#include "base/utils/string_utils.h"
#include "core/components/common/properties/color.h"
#include "core/components/option/option_component.h"
#include "core/components/select_popup/select_popup_component.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

enum MenuStatus {
    OFF = 0,
    ON,
    INIT
};

class ACE_EXPORT MenuComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(MenuComponent, ComposedComponent);

public:
    MenuComponent(const ComposeId& id, const std::string& name, const RefPtr<Component>& child)
        : ComposedComponent(id, name, child)
    {}

    MenuComponent(const ComposeId& id, const std::string& name) : ComposedComponent(id, name)
    {
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        popup_->SetSelectPopupId(StringUtils::StringToInt(id));
#endif
    }

    ~MenuComponent() override = default;

    RefPtr<Element> CreateElement() override;

    void InitTheme(const RefPtr<ThemeManager>& themeManager)
    {
        if (popup_) {
            popup_->InitTheme(themeManager);
        }
    }

    void SetTheme(const RefPtr<SelectTheme>& theme)
    {
        if (popup_) {
            popup_->SetTheme(theme);
        }
    }

    void SetTextDirection(TextDirection direction) override
    {
        ComposedComponent::SetTextDirection(direction);
        if (popup_) {
            popup_->SetTextDirection(direction);
        } else {
            LOGE("popup component of select is null.");
        }
    }

    void ClearOptions();

    void AppendOption(const RefPtr<OptionComponent>& option);

    void RemoveOption(const RefPtr<OptionComponent>& option);

    void InsertOption(const RefPtr<OptionComponent>& option, uint32_t index);

    std::size_t GetOptionCount() const;

    RefPtr<OptionComponent> GetOption(std::size_t index) const;

    std::vector<RefPtr<OptionComponent>> GetOptions() const;

    const std::function<void(const ComposeId&, const Offset&)>& GetTargetCallback() const
    {
        return targetCallback_;
    }

    void SetTargetCallback(const std::function<void(const ComposeId&, const Offset&)>& targetCallback)
    {
        targetCallback_ = targetCallback;
    }

    const EventMarker& GetOnSuccess() const
    {
        return onSuccess_;
    }
    void SetOnSuccess(const EventMarker& value)
    {
        onSuccess_ = value;
    }

    const EventMarker& GetOnCancel() const
    {
        return onCancel_;
    }
    void SetOnCancel(const EventMarker& value)
    {
        onCancel_ = value;
    }

    void SetMenuShow(const MenuStatus& status)
    {
        menuShow_ = status;
    }

    const MenuStatus& GetMenuShow()
    {
        return menuShow_;
    }

    void SetPopupPosition(const Offset& position)
    {
        popupPosition_ = position;
    }

    const Offset& GetPopupPosition()
    {
        return popupPosition_;
    }

    std::string GetTitle() const;
    void SetTitle(const std::string& value);

    const TextStyle& GetTitleStyle() const;
    void SetTitleStyle(const TextStyle& value);
    void SetIsBindTarget(bool isBindTarget) {}
    const RefPtr<SelectPopupComponent>& GetPopup() const;

    void SetIsContextMenu(bool isContextMenu)
    {
        isContextMenu_ = isContextMenu;
        if (popup_) {
            popup_->SetIsContextMenu(isContextMenu);
        }
    }

    bool IsContextMenu()
    {
        return isContextMenu_;
    }

private:
    RefPtr<SelectPopupComponent> popup_ = AceType::MakeRefPtr<SelectPopupComponent>();
    std::function<void(const ComposeId&, const Offset&)> targetCallback_;
    EventMarker onSuccess_;
    EventMarker onCancel_;
    MenuStatus menuShow_ = MenuStatus::INIT;
    Offset popupPosition_ = Offset(0, 0);
    TextStyle textStyle_ = TextStyle();

    // This is mardked by contextMenu
    bool isContextMenu_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MENU_MENU_COMPONENT_H
