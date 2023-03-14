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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_THEME_H

#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * ListTheme defines styles of list. ListTheme should be built
 * using ListTheme::Builder.
 */
class ListTheme : public virtual Theme {
    DECLARE_ACE_TYPE(ListTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<ListTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<ListTheme> theme = AceType::Claim(new ListTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->gradientWidth_ = themeConstants->GetDimension(THEME_LIST_GRADIENT_WIDTH);
            theme->backgroundColor_ = themeConstants->GetColor(THEME_LIST_BACKGROUND_COLOR);
            theme->scrollDistance_ = themeConstants->GetDouble(THEME_LIST_SCROLL_DISTANCE);
            theme->dividerColor_ = themeConstants->GetColor(THEME_LIST_DIVIDER_COLOR);
            return theme;
        }
    };

    ~ListTheme() override = default;

    const Dimension& GetGradientWidth() const
    {
        return gradientWidth_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const Color& GetDividerColor() const
    {
        return dividerColor_;
    }

    double GetScrollDistance() const
    {
        return scrollDistance_;
    }

protected:
    ListTheme() = default;

private:
    Dimension gradientWidth_;
    Color backgroundColor_;
    Color dividerColor_;
    double scrollDistance_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_THEME_H
