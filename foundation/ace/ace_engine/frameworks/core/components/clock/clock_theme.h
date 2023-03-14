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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_CLOCK_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_CLOCK_THEME_H

#include "base/resource/internal_resource.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * ClockTheme defines styles of ClockComponent. ClockTheme should be built
 * using ClockTheme::Builder.
 */
class ClockTheme : public virtual Theme {
    DECLARE_ACE_TYPE(ClockTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<ClockTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<ClockTheme> theme = AceType::Claim(new ClockTheme());
            if (!themeConstants) {
                return theme;
            }
            theme = AceType::Claim(new ClockTheme());
            theme->defaultSize_ = themeConstants->GetDimension(THEME_CLOCK_DEFAULT_SIZE);
            return theme;

        }
    };

    ~ClockTheme() override = default;

    const Dimension& GetDefaultSize() const
    {
        return defaultSize_;
    }

protected:
    ClockTheme() = default;

private:
    Dimension defaultSize_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_CLOCK_THEME_H
