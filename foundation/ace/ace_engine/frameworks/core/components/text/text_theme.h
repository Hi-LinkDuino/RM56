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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TEXT_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TEXT_THEME_H

#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * TextTheme defines color and styles of ThemeComponent. TextTheme should be built
 * using TextTheme::Builder.
 */
class TextTheme : public virtual Theme {
    DECLARE_ACE_TYPE(TextTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<TextTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<TextTheme> theme = AceType::Claim(new TextTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->textStyle_.SetTextColor(themeConstants->GetColor(THEME_OHOS_COLOR_TEXT_PRIMARY));
            theme->textStyle_.SetFontSize(themeConstants->GetDimension(THEME_TEXT_SIZE));
            // Styles below do not need to get from ThemeConstants, directly set at here.
            theme->textStyle_.SetFontStyle(FontStyle::NORMAL);
            theme->textStyle_.SetFontWeight(FontWeight::NORMAL);
            theme->textStyle_.SetTextDecoration(TextDecoration::NONE);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }
    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<TextTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            theme->textStyle_.SetTextColor(themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_PRIMARY, Color::BLACK));
        }
    };

    ~TextTheme() override = default;

    TextStyle GetTextStyle() const
    {
        return textStyle_;
    }

protected:
    TextTheme() = default;

private:
    TextStyle textStyle_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_TEXT_THEME_H
