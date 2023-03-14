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

#include "frameworks/bridge/common/dom/dom_badge.h"

#include "base/log/event_report.h"
#include "core/components/declaration/badge/badge_declaration.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMBadge::DOMBadge(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    badgeChild_ = AceType::MakeRefPtr<BadgeComponent>();
}

void DOMBadge::ResetInitializedStyle()
{
    if (declaration_) {
        declaration_->InitializeStyle();
    }
}

void DOMBadge::PrepareSpecializedComponent()
{
    auto declaration = AceType::DynamicCast<BadgeDeclaration>(declaration_);
    badgeChild_->SetDeclaration(AceType::DynamicCast<BadgeDeclaration>(declaration));
    if (!boxComponent_ || !declaration) {
        return;
    }
    badgeChild_->SetTextDirection(declaration->IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    boxComponent_->SetAlignment(Alignment::TOP_LEFT);
    auto edge = boxComponent_->GetPadding();
    if (edge == Edge::NONE) {
        return;
    }
    declaration->SetPadding(edge);
    boxComponent_->SetPadding(Edge());
}

void DOMBadge::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    ACE_DCHECK(child);
    if (badgeChild_->GetChild()) {
        return;
    }
    badgeChild_->SetChild(child->GetRootComponent());
}

void DOMBadge::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    badgeChild_->SetChild(nullptr);
}

void DOMBadge::SetBadgeConfig(const BadgeConfig& badgeConfig)
{
    auto declaration = AceType::DynamicCast<BadgeDeclaration>(declaration_);
    if (!declaration) {
        return;
    }

    if (badgeConfig.badgeColor.second) {
        declaration->SetBadgeColor(badgeConfig.badgeColor.first);
    }
    if (badgeConfig.badgeSize.second) {
        declaration->SetBadgeCircleSize(badgeConfig.badgeSize.first);
    }
    if (badgeConfig.textColor.second) {
        declaration->SetBadgeTextColor(badgeConfig.textColor.first);
    }
    if (badgeConfig.textSize.second) {
        declaration->SetBadgeFontSize(badgeConfig.textSize.first);
    }
}

} // namespace OHOS::Ace::Framework
