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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_BADGE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_BADGE_COMPONENT_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/edge.h"
#include "core/pipeline/base/sole_child_component.h"
#include "core/components/declaration/badge/badge_declaration.h"

namespace OHOS::Ace {

class ACE_EXPORT BadgeComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(BadgeComponent, SoleChildComponent);

public:
    BadgeComponent();
    ~BadgeComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    const Color& GetBadgeColor() const;
    const Color& GetBadgeTextColor() const;
    const Dimension& GetBadgeFontSize() const;
    const EventMarker& GetClickEvent() const;
    BadgePosition GetBadgePosition() const;
    const Edge& GetPadding() const;
    const Dimension& GetBadgeCircleSize() const;
    const std::string& GetBadgeLabel() const;
    void SetPadding(const Edge& padding);
    int64_t GetMessageCount() const;
    bool IsShowMessage() const;
    int64_t GetMaxCount() const;
    bool IsBadgeCircleSizeDefined() const;
    void SetBadgeCircleSizeDefined(bool badgeCircleSizeDefined);
    void SetMaxCount(int64_t maxCount);
    void SetShowMessage(bool showMessage);
    void SetMessageCount(int64_t messageCount);
    void SetBadgePosition(BadgePosition badgePostion);
    void SetBadgeTextColor(const Color& badgeTextColor);
    void SetBadgeFontSize(const Dimension& badgeFontSize);
    void SetBadgeColor(const Color& color);
    void SetClickEvent(const EventMarker& event);
    void SetBadgeCircleSize(const Dimension& badgeCircleSize);
    void SetBadgeLabel(const std::string& badgeLabel);

    void SetDeclaration(const RefPtr<BadgeDeclaration>& declaration);

private:
    RefPtr<BadgeDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_BADGE_COMPONENT_H
