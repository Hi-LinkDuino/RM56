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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIVIDER_DIVIDER_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIVIDER_DIVIDER_THEME_H

#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

class DividerTheme : public virtual Theme {
    DECLARE_ACE_TYPE(DividerTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<DividerTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<DividerTheme> theme = AceType::Claim(new DividerTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->color_ = themeConstants->GetColor(THEME_DIVIDER_COLOR);
            auto themeStyle = themeConstants->GetThemeStyle();
            if (themeStyle) {
                theme->color_ = themeStyle->GetAttr<Color>(THEME_ATTR_SUBHEADER_DIVIDER, Color::BLACK);
            }
            return theme;
        }
    };

    ~DividerTheme() override = default;

    const Color& GetColor() const
    {
        return color_;
    }

protected:
    DividerTheme() = default;

private:
    Color color_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIVIDER_DIVIDER_THEME_H
