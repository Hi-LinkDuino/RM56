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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_PATTERN_LOCK_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_PATTERN_LOCK_THEME_H
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"
namespace OHOS::Ace::V2 {
class PatternLockTheme : public virtual Theme {
    DECLARE_ACE_TYPE(PatternLockTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;
        RefPtr<PatternLockTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<PatternLockTheme> theme = AceType::Claim(new PatternLockTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->regularColor_ = Color::BLACK;
            theme->activeColor_ = Color::BLACK;
            theme->selectedColor_ = Color::BLACK;
            const int16_t redColor = 77;
            const int16_t greenColor = 164;
            const int16_t blueColor = 255;
            theme->pathColor_ = Color::FromRGB(redColor, greenColor, blueColor);
            return theme;
        }
    };
    ~PatternLockTheme() override = default;
    const Color& GetRegularColor() const
    {
        return regularColor_;
    }
    const Color& GetSelectedColor() const
    {
        return selectedColor_;
    }
    const Color& GetActiveColor() const
    {
        return activeColor_;
    }
    const Color& GetPathColor() const
    {
        return pathColor_;
    }

protected:
    PatternLockTheme() = default;

private:
    Color regularColor_;
    Color selectedColor_;
    Color activeColor_;
    Color pathColor_;
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_PATTERN_LOCK_THEME_H
