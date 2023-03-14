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

#include "core/components/declaration/badge/badge_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/badge/badge_theme.h"

namespace OHOS::Ace {

using namespace Framework;

void BadgeDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_BADGE_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_BADGE_STYLE);
    AddSpecializedEvent(DeclarationConstants::DEFAULT_BADGE_EVENT);
}

void BadgeDeclaration::InitializeStyle()
{
    auto badgeTheme = GetTheme<BadgeTheme>();
    if (!badgeTheme) {
        return;
    }
    SetBadgeColor(badgeTheme->GetBadgeColor());
    SetMessageCount(badgeTheme->GetMessageCount());
    SetBadgePosition(badgeTheme->GetBadgePosition());
    SetShowMessage(badgeTheme->GetShowMessage());
    SetBadgeTextColor(badgeTheme->GetBadgeTextColor());
    SetBadgeFontSize(badgeTheme->GetBadgeFontSize());
}

bool BadgeDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(BadgeDeclaration&, const std::string&)> badgeAttrOperators[] = {
        { DOM_BADGE_COUNT, [](BadgeDeclaration& declaration, const std::string& value) {
            declaration.SetMessageCount(static_cast<int64_t>(StringUtils::StringToLongInt(value)));
            } },
        { DOM_BADGE_LABEL,
            [](BadgeDeclaration& declaration, const std::string& value) { declaration.SetBadgeLabel(value); } },
        { DOM_BADGE_MAX_COUNT, [](BadgeDeclaration& declaration, const std::string& value) {
            declaration.SetMaxCount(static_cast<int64_t>(StringUtils::StringToLongInt(value)));
            } },
        { DOM_BADGE_PLACEMENT,
            [](BadgeDeclaration& declaration, const std::string& value) {
                declaration.SetBadgePosition(ConvertStrToBadgePosition(value));
            } },
        { DOM_BADGE_VISIBLE, [](BadgeDeclaration& declaration,
                                 const std::string& value) { declaration.SetShowMessage(StringToBool(value)); } }
    };
    auto operatorIter = BinarySearchFindIndex(badgeAttrOperators, ArraySize(badgeAttrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        badgeAttrOperators[operatorIter].value(*this, attr.second);
        return true;
    }
    return false;
}

bool BadgeDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    const static LinearMapNode<void (*)(BadgeDeclaration&, const std::string&)> badgeOperators[] = {
        { DOM_BADGE_COLOR,
            [](BadgeDeclaration& declaration, const std::string& value) {
                declaration.SetBadgeColor(declaration.ParseColor(value));
            } },
        { DOM_BADGE_CIRCLE_SIZE,
            [](BadgeDeclaration& declaration, const std::string& value) {
                declaration.SetBadgeCircleSize(declaration.ParseDimension(value));
            } },
        { DOM_BADGE_TEXT_COLOR,
            [](BadgeDeclaration& declaration, const std::string& value) {
                declaration.SetBadgeTextColor(declaration.ParseColor(value));
            } },
        { DOM_BADGE_TEXT_FONT_SIZE,
            [](BadgeDeclaration& declaration, const std::string& value) {
                declaration.SetBadgeFontSize(declaration.ParseDimension(value));
            } },
    };
    auto operatorIter = BinarySearchFindIndex(badgeOperators, ArraySize(badgeOperators), style.first.c_str());
    if (operatorIter != -1) {
        badgeOperators[operatorIter].value(*this, style.second);
        return true;
    }
    return false;
}

bool BadgeDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    if (event == DOM_CLICK) {
        EventMarker eventMarker(eventId, event, pageId);
        eventMarker.SetCatchMode(false);
        SetClickEvent(eventMarker);
        return true;
    } else if (event == DOM_CATCH_BUBBLE_CLICK) {
        EventMarker eventMarker(eventId, event, pageId);
        eventMarker.SetCatchMode(true);
        SetClickEvent(eventMarker);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace
