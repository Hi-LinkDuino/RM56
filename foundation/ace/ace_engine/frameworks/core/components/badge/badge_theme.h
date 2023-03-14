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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_BADGE_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_BADGE_THEME_H

#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"

namespace OHOS::Ace {

class BadgeTheme : public virtual Theme {
    DECLARE_ACE_TYPE(BadgeTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<BadgeTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<BadgeTheme> theme = AceType::Claim(new BadgeTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->badgeColor_ = themeConstants->GetColor(THEME_BADGE_COLOR);
            theme->messageCount_ = themeConstants->GetInt(THEME_BADGE_MESSAGECOUNT);
            theme->badgePosition_ = BadgePosition(themeConstants->GetInt(THEME_BADGE_POSITION));
            theme->showMessage_ = themeConstants->GetInt(THEME_BADGE_SHOWMESSAGE);
            theme->badgeTextColor_ = themeConstants->GetColor(THEME_BADGE_TEXT_COLOR);
            theme->badgeFontSize_ = themeConstants->GetDimension(THEME_BADGE_TEXT_FONT_SIZE);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<BadgeTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            theme->badgeColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_BADGE_RED, Color::BLACK);
            theme->badgeFontSize_ = themeStyle->GetAttr<Dimension>(THEME_ATTR_TEXT_SIZE_CAPTION, 0.0_vp);
            theme->badgeTextColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_PRIMARY_INVERSE, Color::BLACK);
        }
    };

    ~BadgeTheme() override = default;

    const Color& GetBadgeColor() const
    {
        return badgeColor_;
    }

    int64_t GetMessageCount() const
    {
        return messageCount_;
    }

    BadgePosition GetBadgePosition() const
    {
        return badgePosition_;
    }

    bool GetShowMessage() const
    {
        return showMessage_;
    }

    const Color& GetBadgeTextColor() const
    {
        return badgeTextColor_;
    }

    const Dimension& GetBadgeFontSize() const
    {
        return badgeFontSize_;
    }

protected:
    BadgeTheme() = default;

private:
    Color badgeColor_;
    Color badgeTextColor_;
    int64_t messageCount_;
    BadgePosition badgePosition_;
    bool showMessage_;
    Dimension badgeFontSize_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BADGE_BADGE_THEME_H
