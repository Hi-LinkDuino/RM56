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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_SEMI_MODAL_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_SEMI_MODAL_THEME_H

#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

class SemiModalTheme : public virtual Theme {
    DECLARE_ACE_TYPE(SemiModalTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<SemiModalTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<SemiModalTheme> theme = AceType::Claim(new SemiModalTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->bgColor_ = themeConstants->GetColor(THEME_SEMI_MODAL_BACKGROUND_COLOR);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }
    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<SemiModalTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            auto semiModalPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_SEMI_MODAL, nullptr);
            if (!semiModalPattern) {
                return;
            }
            theme->bgColor_ = semiModalPattern->GetAttr<Color>(SEMI_MODAL_BACKGROUND_COLOR, Color());
        }
    };
    ~SemiModalTheme() override = default;

    const Color& GetBgColor() const
    {
        return bgColor_;
    }

protected:
    SemiModalTheme() = default;

private:
    Color bgColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_SEMI_MODAL_THEME_H
