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

#include "core/components/menu/menu_component.h"

#include "core/components/menu/menu_element.h"

namespace OHOS::Ace {


RefPtr<Element> MenuComponent::CreateElement()
{
    return AceType::MakeRefPtr<MenuElement>(GetId());
}

const RefPtr<SelectPopupComponent>& MenuComponent::GetPopup() const
{
    return popup_;
}

void MenuComponent::RemoveOption(const RefPtr<OptionComponent>& option)
{
    if (!popup_ || !option) {
        LOGE("popup or option is null.");
        return;
    }
    option->SetSelectable(false);
    popup_->RemoveSelectOption(option);
}

void MenuComponent::AppendOption(const RefPtr<OptionComponent>& option)
{
    if (!popup_ || !option) {
        LOGE("popup or option is null.");
        return;
    }
    option->SetSelectable(false);
    popup_->AppendSelectOption(option);
}

void MenuComponent::InsertOption(const RefPtr<OptionComponent>& option, uint32_t index)
{
    if (!popup_ || !option) {
        return;
    }
    option->SetSelectable(false);
    popup_->InsertSelectOption(option, index);
}

void MenuComponent::ClearOptions()
{
    if (!popup_) {
        LOGE("popup is null.");
        return;
    }
    popup_->ClearAllOptions();
}

std::size_t MenuComponent::GetOptionCount() const
{
    if (!popup_) {
        LOGE("popup is null.");
        return 0;
    }
    return popup_->GetSelectOptionCount();
}

RefPtr<OptionComponent> MenuComponent::GetOption(std::size_t index) const
{
    if (!popup_) {
        LOGE("popup is null.");
        return nullptr;
    }
    return popup_->GetSelectOption(index);
}

std::vector<RefPtr<OptionComponent>> MenuComponent::GetOptions() const
{
    if (!popup_) {
        LOGE("popup is null.");
        return std::vector<RefPtr<OptionComponent>>();
    }
    return popup_->GetSelectOptions();
}

std::string MenuComponent::GetTitle() const
{
    if (!popup_) {
        LOGE("popup is null.");
        return "";
    }
    return popup_->GetTitle();
}

void MenuComponent::SetTitle(const std::string& value)
{
    if (!popup_) {
        LOGE("popup is null.");
        return;
    }
    popup_->SetTitle(value);
}

const TextStyle& MenuComponent::GetTitleStyle() const
{
    if (!popup_) {
        LOGE("popup is null.");
        return textStyle_;
    }
    return popup_->GetTitleStyle();
}

void MenuComponent::SetTitleStyle(const TextStyle& value)
{
    if (!popup_) {
        LOGE("popup is null.");
        return;
    }
    return popup_->SetTitleStyle(value);
}

} // namespace OHOS::Ace
