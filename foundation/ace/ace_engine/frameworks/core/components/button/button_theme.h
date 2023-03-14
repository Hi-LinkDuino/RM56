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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_BUTTON_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_BUTTON_THEME_H

#include "core/components/common/properties/color.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

/**
 * ButtonTheme defines color and styles of ButtonComponent. ButtonTheme should be built
 * using ButtonTheme::Builder.
 */
class ButtonTheme : public virtual Theme {
    DECLARE_ACE_TYPE(ButtonTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<ButtonTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<ButtonTheme> theme = AceType::Claim(new ButtonTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->radius_ = themeConstants->GetDimension(THEME_BUTTON_RADIUS);
            theme->bgColor_ = themeConstants->GetColor(THEME_BUTTON_BACKGROUND_COLOR);
            theme->bgFocusColor_ = themeConstants->GetColor(THEME_BUTTON_FOCUS_COLOR);
            theme->clickedColor_ = themeConstants->GetColor(THEME_BUTTON_CLICKED_COLOR);
            theme->disabledColor_ = themeConstants->GetColor(THEME_BUTTON_DISABLED_COLOR);
            theme->bgDisabledAlpha_ = themeConstants->GetDouble(THEME_BUTTON_DISABLED_ALPHA);
            theme->hoverColor_ = themeConstants->GetColor(THEME_BUTTON_HOVER_COLOR);
            theme->borderColor_ = themeConstants->GetColor(THEME_BUTTON_BORDER_COLOR);
            theme->borderWidth_ = themeConstants->GetDimension(THEME_BUTTON_BORDER_WIDTH);
            theme->textFocusColor_ = themeConstants->GetColor(THEME_BUTTON_TEXT_FOCUS_COLOR);
            theme->textDisabledColor_ = themeConstants->GetColor(THEME_BUTTON_TEXT_DISABLED_COLOR);
            theme->textWaitingColor_ = themeConstants->GetColor(THEME_BUTTON_TEXT_WAITING_COLOR);
            theme->textStyle_.SetTextColor(themeConstants->GetColor(THEME_BUTTON_TEXT_COLOR));
            theme->textStyle_.SetFontSize(themeConstants->GetDimension(THEME_BUTTON_TEXT_FONTSIZE));
            theme->textStyle_.SetFontWeight(FontWeight(themeConstants->GetInt(THEME_BUTTON_TEXT_FONTWEIGHT)));
            theme->minWidth_ = themeConstants->GetDimension(THEME_BUTTON_MIN_WIDTH);
            theme->height_ = themeConstants->GetDimension(THEME_BUTTON_HEIGHT);
            theme->downloadHeight_ = themeConstants->GetDimension(THEME_BUTTON_DOWNLOAD_HEIGHT);
            theme->padding_ = Edge(themeConstants->GetDimension(THEME_BUTTON_PADDING_HORIZONTAL).Value(),
                themeConstants->GetDimension(THEME_BUTTON_PADDING_VERTICAL).Value(),
                themeConstants->GetDimension(THEME_BUTTON_PADDING_HORIZONTAL).Value(),
                themeConstants->GetDimension(THEME_BUTTON_PADDING_VERTICAL).Value(),
                themeConstants->GetDimension(THEME_BUTTON_PADDING_VERTICAL).Unit());
            theme->minFontSize_ = themeConstants->GetDimension(THEME_BUTTON_TEXT_FONTSIZE_MIN);
            int32_t maxlines = themeConstants->GetInt(THEME_BUTTON_TEXT_MAX_LINES);
            theme->textMaxLines_ = maxlines < 0 ? theme->textMaxLines_ : static_cast<uint32_t>(maxlines);
            theme->minCircleButtonDiameter_ = themeConstants->GetDimension(THEME_BUTTON_CIRCLE_MIN_DIAMETER);
            theme->minCircleButtonIcon_ = themeConstants->GetDimension(THEME_BUTTON_CIRCLE_MIN_ICON_SIZE);
            theme->minCircleButtonPadding_ = Edge(themeConstants->GetDimension(THEME_BUTTON_CIRCLE_MIN_PADDING));
            theme->maxCircleButtonDiameter_ = themeConstants->GetDimension(THEME_BUTTON_CIRCLE_MAX_DIAMETER);
            theme->maxCircleButtonIcon_ = themeConstants->GetDimension(THEME_BUTTON_CIRCLE_MAX_ICON_SIZE);
            theme->maxCircleButtonPadding_ = Edge(themeConstants->GetDimension(THEME_BUTTON_CIRCLE_MAX_PADDING));
            theme->progressFocusColor_ = themeConstants->GetColor(THEME_BUTTON_PROGRESS_FOCUS_COLOR);
            theme->downloadBorderColor_ = themeConstants->GetColor(THEME_BUTTON_DOWNLOAD_BORDER_COLOR);
            theme->normalTextColor_ = themeConstants->GetColor(THEME_BUTTON_NORMAL_TEXT_COLOR);
            theme->downloadBackgroundColor_ = themeConstants->GetColor(THEME_BUTTON_DOWNLOAD_BG_COLOR);
            theme->downloadTextColor_ = themeConstants->GetColor(THEME_BUTTON_DOWNLOAD_TEXT_COLOR);
            theme->downloadFontSize_ = themeConstants->GetDimension(THEME_BUTTON_DOWNLOAD_TEXT_FONTSIZE);
            theme->progressColor_ = themeConstants->GetColor(THEME_BUTTON_PROGRESS_COLOR);
            theme->progressDiameter_ = themeConstants->GetDimension(THEME_BUTTON_PROGRESS_DIAMETER);
            theme->downloadProgressColor_ = themeConstants->GetColor(THEME_BUTTON_DOWNLOAD_PROGRESS_COLOR);
            theme->innerPadding_ = themeConstants->GetDimension(THEME_BUTTON_INNER_PADDING);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<ButtonTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            auto buttonPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_BUTTON, nullptr);
            if (!buttonPattern) {
                return;
            }
            theme->bgColor_ = buttonPattern->GetAttr<Color>(PATTERN_BG_COLOR, Color());
            theme->clickedColor_ = buttonPattern->GetAttr<Color>(BUTTON_CLICK_BLEND_COLOR, Color());
            theme->disabledColor_ = buttonPattern->GetAttr<Color>(PATTERN_BG_COLOR, Color())
                .BlendOpacity(themeStyle->GetAttr<double>(THEME_ATTR_DISABLED_ALPHA, 0.0));
            theme->hoverColor_ = themeStyle->GetAttr<Color>(BUTTON_HOVER_COLOR, Color());
            theme->borderColor_ = buttonPattern->GetAttr<Color>(BUTTON_BORDER_COLOR, Color());
            theme->borderWidth_ = buttonPattern->GetAttr<Dimension>(BUTTON_BORDER_WIDTH, 0.0_vp);
            theme->textStyle_.SetTextColor(buttonPattern->GetAttr<Color>(BUTTON_TEXT_COLOR, Color()));
            theme->textDisabledColor_ = buttonPattern->GetAttr<Color>(BUTTON_TEXT_COLOR, Color())
                .BlendOpacity(themeStyle->GetAttr<double>(THEME_ATTR_DISABLED_ALPHA, 0.0));
            theme->textWaitingColor_ = buttonPattern->GetAttr<Color>(BUTTON_TEXT_COLOR, Color());
            theme->normalTextColor_ = buttonPattern->GetAttr<Color>(BUTTON_NORMAL_TEXT_COLOR, Color());
            theme->downloadBackgroundColor_ = themeStyle->GetAttr<Color>(BUTTON_DOWNLOAD_BG_COLOR, Color())
                .BlendOpacity(themeStyle->GetAttr<double>(THEME_ATTR_FOURTH_CONTENT_ALPHA, 0.0));
            theme->downloadBorderColor_ = buttonPattern->GetAttr<Color>(BUTTON_DOWNLOAD_BORDER_COLOR, Color())
                .BlendOpacity(themeStyle->GetAttr<double>(THEME_ATTR_HIGHLIGHT_BACKGROUND_ALPHA, 0.0));
            theme->downloadProgressColor_ = buttonPattern->GetAttr<Color>(BUTTON_DOWNLOAD_BORDER_COLOR, Color())
                .BlendOpacity(themeStyle->GetAttr<double>(THEME_ATTR_HIGHLIGHT_BACKGROUND_ALPHA, 0.0));
            theme->downloadTextColor_ = buttonPattern->GetAttr<Color>(BUTTON_DOWNLOAD_TEXT_COLOR, Color());
            theme->progressColor_ = buttonPattern->GetAttr<Color>(BUTTON_TEXT_COLOR, Color());
        }
    };

    ~ButtonTheme() override = default;

    const Dimension& GetRadius() const
    {
        return radius_;
    }

    const Color& GetBgColor() const
    {
        return bgColor_;
    }

    const Color& GetBgFocusColor() const
    {
        return bgFocusColor_;
    }

    const Color& GetClickedColor() const
    {
        return clickedColor_;
    }

    const Color& GetDisabledColor() const
    {
        return disabledColor_;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    const Color& GetBorderColor() const
    {
        return borderColor_;
    }

    const Dimension& GetBorderWidth() const
    {
        return borderWidth_;
    }

    double GetBgDisabledAlpha() const
    {
        return bgDisabledAlpha_;
    }

    const Color& GetTextFocusColor() const
    {
        return textFocusColor_;
    }

    const Color& GetTextDisabledColor() const
    {
        return textDisabledColor_;
    }

    const Color& GetNormalTextColor() const
    {
        return normalTextColor_;
    }

    const Color& GetDownloadBackgroundColor() const
    {
        return downloadBackgroundColor_;
    }

    const Color& GetDownloadTextColor() const
    {
        return downloadTextColor_;
    }

    const Color& GetTextWaitingColor() const
    {
        return textWaitingColor_;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    const Dimension& GetMinWidth() const
    {
        return minWidth_;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    const Dimension& GetDownloadHeight() const
    {
        return downloadHeight_;
    }

    const Edge& GetPadding() const
    {
        return padding_;
    }

    const Dimension& GetMinFontSize() const
    {
        return minFontSize_;
    }

    const Dimension& GetDownloadFontSize() const
    {
        return downloadFontSize_;
    }

    const Dimension& GetMaxFontSize() const
    {
        return textStyle_.GetFontSize();
    }

    uint32_t GetTextMaxLines() const
    {
        return textMaxLines_;
    }

    const Dimension& GetMinCircleButtonDiameter() const
    {
        return minCircleButtonDiameter_;
    }

    const Dimension& GetMinCircleButtonIcon() const
    {
        return minCircleButtonIcon_;
    }

    const Edge& GetMinCircleButtonPadding() const
    {
        return minCircleButtonPadding_;
    }

    const Dimension& GetMaxCircleButtonDiameter() const
    {
        return maxCircleButtonDiameter_;
    }

    const Dimension& GetMaxCircleButtonIcon() const
    {
        return maxCircleButtonIcon_;
    }

    const Edge& GetMaxCircleButtonPadding() const
    {
        return maxCircleButtonPadding_;
    }

    const Color& GetProgressFocusColor() const
    {
        return progressFocusColor_;
    }

    const Color& GetDownloadBorderColor() const
    {
        return downloadBorderColor_;
    }

    const Color& GetProgressColor() const
    {
        return progressColor_;
    }

    const Dimension& GetProgressDiameter() const
    {
        return progressDiameter_;
    }

    const Color& GetDownloadProgressColor() const
    {
        return downloadProgressColor_;
    }

    const Dimension& GetInnerPadding() const
    {
        return innerPadding_;
    }

protected:
    ButtonTheme() = default;

private:
    Color bgColor_;
    Color bgFocusColor_;
    Color clickedColor_;
    Color disabledColor_;
    Color hoverColor_;
    Color borderColor_;
    Color textFocusColor_;
    Color textDisabledColor_;
    Color textWaitingColor_;
    Color progressColor_;
    Color progressFocusColor_;
    Color normalTextColor_;
    Color downloadBackgroundColor_;
    Color downloadTextColor_;
    Color downloadBorderColor_;
    Color downloadProgressColor_;
    TextStyle textStyle_;
    Edge padding_;
    Edge minCircleButtonPadding_;
    Edge maxCircleButtonPadding_;

    Dimension radius_;
    Dimension minWidth_;
    Dimension height_;
    Dimension progressDiameter_;
    Dimension innerPadding_;
    Dimension minFontSize_;
    Dimension downloadFontSize_;
    Dimension minCircleButtonDiameter_;
    Dimension minCircleButtonIcon_;
    Dimension maxCircleButtonDiameter_;
    Dimension maxCircleButtonIcon_;
    Dimension borderWidth_;
    Dimension downloadHeight_;

    double bgDisabledAlpha_ = 1.0;
    uint32_t textMaxLines_ = 1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_BUTTON_THEME_H
