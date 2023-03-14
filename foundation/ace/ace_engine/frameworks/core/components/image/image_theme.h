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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_THEME_H

#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

/**
 * ImageTheme defines color and styles of Image. ImageTheme should be built
 * using ImageTheme::Builder.
 */
class ImageTheme : public virtual Theme {
    DECLARE_ACE_TYPE(ImageTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<ImageTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<ImageTheme> theme = AceType::Claim(new ImageTheme());
            if (!themeConstants) {
                return theme;
            }
            auto themeStyle = themeConstants->GetThemeStyle();
            theme->fillColor_ = themeConstants->GetColor(THEME_IMAGE_FILL_COLOR);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<ImageTheme>& theme) const
        {
            if (!themeStyle) {
                LOGI("image theme style is null");
                return;
            }
            theme->fillColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_FOREGROUND, Color());
        }
    };

    ~ImageTheme() override = default;

    const Color& GetFillColor() const
    {
        return fillColor_;
    }

protected:
    ImageTheme() = default;

private:
    Color fillColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_THEME_H
