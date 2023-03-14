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

#include "core/components/badge/badge_component.h"

#include "core/components/badge/badge_element.h"
#include "core/components/badge/render_badge.h"

namespace OHOS::Ace {

BadgeComponent::BadgeComponent() : SoleChildComponent()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<BadgeDeclaration>();
        declaration_->Init();
    }
}

RefPtr<RenderNode> BadgeComponent::CreateRenderNode()
{
    return RenderBadge::Create();
}

RefPtr<Element> BadgeComponent::CreateElement()
{
    return AceType::MakeRefPtr<BadgeElement>();
}

const Color& BadgeComponent::GetBadgeColor() const
{
    return declaration_->GetBadgeColor();
}

const Color& BadgeComponent::GetBadgeTextColor() const
{
    return declaration_->GetBadgeTextColor();
}

const Dimension& BadgeComponent::GetBadgeFontSize() const
{
    return declaration_->GetBadgeFontSize();
}

const EventMarker& BadgeComponent::GetClickEvent() const
{
    return declaration_->GetClickEvent();
}

BadgePosition BadgeComponent::GetBadgePosition() const
{
    return declaration_->GetBadgePosition();
}

const Edge& BadgeComponent::GetPadding() const
{
    return declaration_->GetPadding();
}

const Dimension& BadgeComponent::GetBadgeCircleSize() const
{
    return declaration_->GetBadgeCircleSize();
}

const std::string& BadgeComponent::GetBadgeLabel() const
{
    return declaration_->GetBadgeLabel();
}

void BadgeComponent::SetPadding(const Edge& padding)
{
    declaration_->SetPadding(padding);
}

int64_t BadgeComponent::GetMessageCount() const
{
    return declaration_->GetMessageCount();
}

bool BadgeComponent::IsShowMessage() const
{
    return declaration_->IsShowMessage();
}

int64_t BadgeComponent::GetMaxCount() const
{
    return declaration_->GetMaxCount();
}

bool BadgeComponent::IsBadgeCircleSizeDefined() const
{
    return declaration_->IsBadgeCircleSizeDefined();
}

void BadgeComponent::SetBadgeCircleSizeDefined(bool badgeCircleSizeDefined)
{
    declaration_->SetBadgeCircleSizeDefined(badgeCircleSizeDefined);
}

void BadgeComponent::SetMaxCount(int64_t maxCount)
{
    declaration_->SetMaxCount(maxCount);
}

void BadgeComponent::SetShowMessage(bool showMessage)
{
    declaration_->SetShowMessage(showMessage);
}

void BadgeComponent::SetMessageCount(int64_t messageCount)
{
    declaration_->SetMessageCount(messageCount);
}

void BadgeComponent::SetBadgePosition(BadgePosition badgePostion)
{
    declaration_->SetBadgePosition(badgePostion);
}

void BadgeComponent::SetBadgeTextColor(const Color& badgeTextColor)
{
    declaration_->SetBadgeTextColor(badgeTextColor);
}

void BadgeComponent::SetBadgeFontSize(const Dimension& badgeFontSize)
{
    declaration_->SetBadgeFontSize(badgeFontSize);
}

void BadgeComponent::SetBadgeColor(const Color& color)
{
    declaration_->SetBadgeColor(color);
}

void BadgeComponent::SetClickEvent(const EventMarker& event)
{
    declaration_->SetClickEvent(event);
}

void BadgeComponent::SetBadgeCircleSize(const Dimension& badgeCircleSize)
{
    declaration_->SetBadgeCircleSize(badgeCircleSize);
}

void BadgeComponent::SetBadgeLabel(const std::string& badgeLabel)
{
    declaration_->SetBadgeLabel(badgeLabel);
}

void BadgeComponent::SetDeclaration(const RefPtr<BadgeDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
