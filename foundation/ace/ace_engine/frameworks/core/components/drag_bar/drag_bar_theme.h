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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_DRAG_BAR_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_DRAG_BAR_THEME_H

#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

class DragBarTheme : public virtual Theme {
    DECLARE_ACE_TYPE(DragBarTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<DragBarTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<DragBarTheme> theme = AceType::Claim(new DragBarTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->barColor_ = themeConstants->GetColor(THEME_DRAG_BAR_COLOR);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }
    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<DragBarTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            auto dragBarPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_DRAG_BAR, nullptr);
            if (!dragBarPattern) {
                return;
            }
            theme->barColor_ = dragBarPattern->GetAttr<Color>(DRAG_BAR_COLOR, Color());
        }
    };
    ~DragBarTheme() override = default;

    const Color& GetBarColor() const
    {
        return barColor_;
    }

protected:
    DragBarTheme() = default;

private:
    Color barColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_DRAG_BAR_THEME_H
