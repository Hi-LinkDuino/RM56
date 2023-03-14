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

#include "core/components/search/search_component.h"

#include "core/components/search/render_search.h"
#include "core/components/search/search_element.h"

namespace OHOS::Ace {

SearchComponent::SearchComponent()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SearchDeclaration>();
        declaration_->Init();
    }
}

RefPtr<RenderNode> SearchComponent::CreateRenderNode()
{
    return RenderSearch::Create();
}

RefPtr<Element> SearchComponent::CreateElement()
{
    return AceType::MakeRefPtr<SearchElement>();
}

void SearchComponent::SetCloseIconSize(const Dimension& closeIconSize)
{
    declaration_->SetCloseIconSize(closeIconSize);
}

const Dimension& SearchComponent::GetCloseIconSize() const
{
    return declaration_->GetCloseIconSize();
}

void SearchComponent::SetCloseIconHotZoneHorizontal(const Dimension& closeIconHotZoneHorizontal)
{
    declaration_->SetCloseIconHotZoneHorizontal(closeIconHotZoneHorizontal);
}

const Dimension& SearchComponent::GetCloseIconHotZoneHorizontal() const
{
    return declaration_->GetCloseIconHotZoneHorizontal();
}

const std::string& SearchComponent::GetSearchText() const
{
    return declaration_->GetSearchText();
}

void SearchComponent::SetSearchText(const std::string& searchText)
{
    declaration_->SetSearchText(searchText);
}

const std::string& SearchComponent::GetCloseIconSrc() const
{
    return declaration_->GetCloseIconSrc();
}

void SearchComponent::SetCloseIconSrc(const std::string& closeIconSrc)
{
    declaration_->SetCloseIconSrc(closeIconSrc);
}

void SearchComponent::SetChangeEventId(const EventMarker& changeEventId)
{
    declaration_->SetChangeEventId(changeEventId);
}

const EventMarker& SearchComponent::GetChangeEventId() const
{
    return declaration_->GetChangeEventId();
}

void SearchComponent::SetSubmitEventId(const EventMarker& submitEventId)
{
    declaration_->SetSubmitEventId(submitEventId);
}

const EventMarker& SearchComponent::GetSubmitEventId() const
{
    return declaration_->GetSubmitEventId();
}

RefPtr<TextEditController> SearchComponent::GetTextEditController() const
{
    return declaration_->GetTextEditController();
}

void SearchComponent::SetTextEditController(const RefPtr<TextEditController>& controller)
{
    declaration_->SetTextEditController(controller);
}

void SearchComponent::SetSubmitEvent(const std::function<void(const std::string&)>& event)
{
    declaration_->SetSubmitEvent(event);
}

const std::function<void(const std::string&)>& SearchComponent::GetSubmitEvent() const
{
    return declaration_->GetSubmitEvent();
}

RefPtr<Decoration> SearchComponent::GetDecoration() const
{
    return declaration_->GetDecoration();
}

void SearchComponent::SetDecoration(const RefPtr<Decoration>& decoration)
{
    declaration_->SetDecoration(decoration);
}

const Color& SearchComponent::GetHoverColor() const
{
    return declaration_->GetHoverColor();
}

void SearchComponent::SetHoverColor(const Color& hoverColor)
{
    declaration_->SetHoverColor(hoverColor);
}

const Color& SearchComponent::GetPressColor() const
{
    return declaration_->GetPressColor();
}

void SearchComponent::SetPressColor(const Color& pressColor)
{
    declaration_->SetPressColor(pressColor);
}

void SearchComponent::SetPlaceHoldStyle(const TextStyle& style)
{
    declaration_->SetPlaceHoldStyle(style);
}

const TextStyle& SearchComponent::GetPlaceHoldStyle() const
{
    return declaration_->GetPlaceHoldStyle();
}

void SearchComponent::SetEditingStyle(const TextStyle& style)
{
    declaration_->SetEditingStyle(style);
}

const TextStyle& SearchComponent::GetEditingStyle() const
{
    return declaration_->GetEditingStyle();
}

void SearchComponent::SetDeclaration(const RefPtr<SearchDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
