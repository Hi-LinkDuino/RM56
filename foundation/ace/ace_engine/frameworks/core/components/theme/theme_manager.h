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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_THEME_THEME_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_THEME_THEME_MANAGER_H

#include <mutex>

#include "base/memory/ace_type.h"
#include "base/resource/asset_manager.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"

namespace OHOS::Ace {

class ACE_EXPORT ThemeManager : public AceType {
    DECLARE_ACE_TYPE(ThemeManager, AceType);

public:
    ThemeManager();
    ~ThemeManager() = default;

    void InitResource(const ResourceInfo& resourceInfo)
    {
        themeConstants_->InitResource(resourceInfo);
    }

    void UpdateConfig(const ResourceConfiguration& config)
    {
        themeConstants_->UpdateConfig(config);
    }

    void LoadSystemTheme(int32_t themeId)
    {
        currentThemeId_ = themeId;
        themeConstants_->LoadTheme(themeId);
    }

    void ParseSystemTheme()
    {
        themeConstants_->ParseTheme();
    }

    void LoadCustomTheme(const RefPtr<AssetManager>& assetManager)
    {
        themeConstants_->LoadCustomStyle(assetManager);
    }

    /*
     * Color scheme of the whole window, app bg color will be change in transparent scheme.
     */
    void SetColorScheme(ColorScheme colorScheme)
    {
        themeConstants_->SetColorScheme(colorScheme);
    }

    /*
     * Get color value from AppTheme (if exists) or system theme style.
     * Prebuild background color will be returned if AppTheme and system theme style both not exists.
     * @return App background color.
     */
    Color GetBackgroundColor() const;

    RefPtr<ThemeConstants> GetThemeConstants() const
    {
        return themeConstants_;
    }

    /*
     * Get target theme, this function will cause construction of the theme if it not exists.
     * @return Target component theme.
     */
    RefPtr<Theme> GetTheme(ThemeType type);

    template<typename T>
    RefPtr<T> GetTheme()
    {
        return AceType::DynamicCast<T>(GetTheme(T::TypeId()));
    }

    void ReloadThemes();

private:
    std::unordered_map<ThemeType, RefPtr<Theme>> themes_;
    RefPtr<ThemeConstants> themeConstants_;
    int32_t currentThemeId_ = -1;

    ACE_DISALLOW_COPY_AND_MOVE(ThemeManager);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_THEME_THEME_MANAGER_H
