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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_FOCUS_ANIMATION_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_FOCUS_ANIMATION_THEME_H

#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * FocusAnimationTheme defines color of FocusAnimationTheme. FocusAnimationTheme should be built
 * using FocusAnimationTheme::Builder.
 */
class FocusAnimationTheme : public virtual Theme {
    DECLARE_ACE_TYPE(FocusAnimationTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<FocusAnimationTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<FocusAnimationTheme> theme = AceType::Claim(new FocusAnimationTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->color_ = themeConstants->GetColor(THEME_FOCUSANIMATION_COLOR);
            auto themeStyle = themeConstants->GetThemeStyle();
            if (themeStyle) {
                theme->color_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_FOCUSED, Color::BLUE);
            }
            return theme;
        }
    };

    ~FocusAnimationTheme() override = default;

    const Color& GetColor() const
    {
        return color_;
    }

protected:
    FocusAnimationTheme() = default;

private:
    Color color_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_FOCUS_ANIMATION_THEME_H
