/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/theme/app_theme.h"

#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

RefPtr<AppTheme> AppTheme::Builder::Build(const RefPtr<ThemeConstants>& themeConstants) const
{
    RefPtr<AppTheme> theme = AceType::Claim(new AppTheme());
    if (!themeConstants) {
        LOGI("Build AppTheme error, themeConstants is null!");
        return theme;
    }
    auto themeStyle = themeConstants->GetThemeStyle();
    if (!themeStyle) {
        LOGI("Build AppTheme error, no theme resource, use prebuild color!");
        theme->backgroundColor_ = themeConstants->GetColor(THEME_APP_BACKGROUND);
        return theme;
    }
    if (themeStyle->HasAttr(THEME_ATTR_BG_COLOR) && !themeConstants->HasCustomStyle(THEME_APP_BACKGROUND)) {
        // Get from resource.
        theme->backgroundColor_ = themeStyle->GetAttr<Color>(THEME_ATTR_BG_COLOR, Color::BLACK);
    } else {
        // Get from prebuild or custom color.
        theme->backgroundColor_ = themeConstants->GetColor(THEME_APP_BACKGROUND);
    }
    return theme;
}

} // namespace OHOS::Ace
