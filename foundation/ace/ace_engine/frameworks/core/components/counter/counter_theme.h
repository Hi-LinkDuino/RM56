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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COUNTER_COUNTER_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COUNTER_COUNTER_THEME_H

#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/radius.h"
#include "core/components/common/properties/text_style.h"

namespace OHOS::Ace {

class CounterTheme : public virtual Theme {
    DECLARE_ACE_TYPE(CounterTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<CounterTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<CounterTheme> theme = AceType::MakeRefPtr<CounterTheme>();
            if (!themeConstants) {
                return theme;
            }

            // init theme from global data
            theme->contentTextStyle_.SetFontSize(themeConstants->GetDimension(THEME_COUNTER_TITLE_FONTSIZE));
            theme->contentTextStyle_.SetTextColor(themeConstants->GetColor(THEME_COUNTER_FONTCOLOR));
            theme->backgroundColor_ = themeConstants->GetColor(THEME_COUNTER_BACKGROUND_COLOR);
            return theme;
        }
    };

    ~CounterTheme() override = default;

    const TextStyle& GetContentTextStyle() const
    {
        return contentTextStyle_;
    }

    const Color& GetBackGroundColor() const
    {
        return backgroundColor_;
    }

private:
    TextStyle contentTextStyle_;
    Color backgroundColor_ = Color(0xff191919);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COUNTER_COUNTER_THEME_H
