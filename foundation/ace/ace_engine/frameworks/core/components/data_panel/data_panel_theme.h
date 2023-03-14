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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_DATA_PANEL_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_DATA_PANEL_THEME_H

#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"

namespace OHOS::Ace {

class DataPanelTheme : public virtual Theme {
    DECLARE_ACE_TYPE(DataPanelTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<DataPanelTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<DataPanelTheme> theme = AceType::Claim(new DataPanelTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->backgroundColor_ = themeConstants->GetColor(THEME_DATA_PANEL_BACKGROUND_COLOR);
            theme->thickness_ = themeConstants->GetDimension(THEME_DATA_PANEL_THICKNESS);
            theme->defaultHeight_ = themeConstants->GetDimension(THEME_DATA_PANEL_HEIGHT);
            theme->defaultWidth_ = themeConstants->GetDimension(THEME_DATA_PANEL_WIDTH);
            theme->loadingColors_.first = themeConstants->GetColor(THEME_DATA_PANEL_LOADING_START_COLOR);
            theme->loadingColors_.second = themeConstants->GetColor(THEME_DATA_PANEL_LOADING_END_COLOR);
            theme->progressColors_.first = themeConstants->GetColor(THEME_DATA_PANEL_PROGRESS_START_COLOR);
            theme->progressColors_.second = themeConstants->GetColor(THEME_DATA_PANEL_PROGRESS_END_COLOR);
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_1_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_1_END));
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_1_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_1_END));
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_2_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_2_END));
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_3_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_3_END));
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_4_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_4_END));
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_5_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_5_END));
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_6_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_6_END));
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_7_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_7_END));
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_8_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_8_END));
            theme->percentageColors_.emplace_back(themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_9_START),
                themeConstants->GetColor(THEME_DATA_PANEL_CONFIG_COLOR_9_END));
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<DataPanelTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            auto dataPanelPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_DATA_PANEL, nullptr);
            if (!dataPanelPattern) {
                return;
            }
            theme->backgroundColor_ = dataPanelPattern->GetAttr<Color>(DATA_PANEL_TRACK_COLOR, Color::BLACK);
            theme->loadingColors_.first = dataPanelPattern->GetAttr<Color>(DATA_PANEL_LOADING_COLOR_END, Color::BLACK);
            theme->loadingColors_.second =
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_LOADING_COLOR_START, Color::BLACK);
            theme->progressColors_.first =
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_PROGRESS_COLOR_END, Color::BLACK);
            theme->progressColors_.second =
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_PROGRESS_COLOR_START, Color::BLACK);
            theme->percentageColors_.clear();
            theme->percentageColors_.emplace_back(
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_1_START, Color::BLACK),
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_1_END, Color::BLACK));
            theme->percentageColors_.emplace_back(
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_2_START, Color::BLACK),
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_2_END, Color::BLACK));
            theme->percentageColors_.emplace_back(
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_3_START, Color::BLACK),
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_3_END, Color::BLACK));
            theme->percentageColors_.emplace_back(
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_4_START, Color::BLACK),
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_4_END, Color::BLACK));
            theme->percentageColors_.emplace_back(
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_5_START, Color::BLACK),
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_5_END, Color::BLACK));
            theme->percentageColors_.emplace_back(
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_6_START, Color::BLACK),
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_6_END, Color::BLACK));
            theme->percentageColors_.emplace_back(
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_7_START, Color::BLACK),
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_7_END, Color::BLACK));
            theme->percentageColors_.emplace_back(
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_8_START, Color::BLACK),
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_8_END, Color::BLACK));
            theme->percentageColors_.emplace_back(
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_9_START, Color::BLACK),
                dataPanelPattern->GetAttr<Color>(DATA_PANEL_COLOR_9_END, Color::BLACK));
        }
    };

    const std::pair<Color, Color>& GetLoadingColor() const
    {
        return loadingColors_;
    }

    const std::pair<Color, Color>& GetProgressColor() const
    {
        return progressColors_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const Dimension& GetDefaultHeight() const
    {
        return defaultHeight_;
    }

    const Dimension& GetDefaultWidth() const
    {
        return defaultWidth_;
    }

    const Dimension& GetThickness() const
    {
        return thickness_;
    }

    const std::vector<std::pair<Color, Color>> GetColorsArray() const
    {
        return percentageColors_;
    }

protected:
private:
    std::vector<std::pair<Color, Color>> percentageColors_;
    std::pair<Color, Color> loadingColors_;
    std::pair<Color, Color> progressColors_;
    Color backgroundColor_;
    Dimension defaultHeight_;
    Dimension defaultWidth_;
    Dimension thickness_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_DATA_PANEL_THEME_H
