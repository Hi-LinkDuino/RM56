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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_SELECT_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_SELECT_THEME_H

#include "base/geometry/dimension.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

constexpr double SELECT_OPTION_LEFT_LENGTH = 16.0;
constexpr double SELECT_OPTION_TOP_LENGTH = 15.0;
constexpr double SELECT_OPTION_RIGHT_LENGTH = 16.0;
constexpr double SELECT_OPTION_BOTTOM_LENGTH = 15.0;
/**
 * SelectTheme defines color and styles of SelectComponent. SelectTheme should be build
 * using SelectTheme::Builder.
 */
class SelectTheme final : public virtual Theme {
    DECLARE_ACE_TYPE(SelectTheme, Theme);

public:
    class Builder final {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<SelectTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<SelectTheme> theme = AceType::Claim(new SelectTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->disabledColor_ = themeConstants->GetColor(THEME_SELECT_DISABLED_COLOR);
            theme->clickedColor_ = themeConstants->GetColor(THEME_SELECT_CLICKED_COLOR);
            theme->selectedColor_ = themeConstants->GetColor(THEME_SELECT_SELECTED_COLOR);
            theme->fontFamily_ = "sans-serif";
            theme->fontSize_ = themeConstants->GetDimension(THEME_SELECT_FONT_SIZE);
            theme->fontColor_ = themeConstants->GetColor(THEME_SELECT_FONT_COLOR);
            theme->fontWeight_ = FontWeight::NORMAL;
            theme->textDecoration_ = TextDecoration::NONE;
            auto optionSize = themeConstants->GetInt(THEME_SELECT_OPTION_SHOW_COUNT);
            theme->optionSize_ = optionSize < 0 ? theme->optionSize_ : static_cast<size_t>(optionSize);
            theme->rrectSize_ = themeConstants->GetDimension(THEME_SELECT_ITSELF_RRECT_SIZE);
            theme->popupBorderWidth_ = themeConstants->GetDimension(THEME_SELECT_POPUP_BORDER_WIDTH);
            theme->popupShadowWidth_ = themeConstants->GetDimension(THEME_SELECT_POPUP_SHADOW_WIDTH);
            theme->popupRRectSize_ = themeConstants->GetDimension(THEME_SELECT_POPUP_RRECT_SIZE);
            theme->popupMinWidth_ = themeConstants->GetDimension(THEME_SELECT_POPUP_MIN_WIDTH);
            theme->normalPadding_ = themeConstants->GetDimension(THEME_SELECT_NORMAL_PADDING);
            theme->iconSize_ = themeConstants->GetDimension(THEME_SELECT_ITSELF_ICON_SIZE);
            theme->isTV_ = themeConstants->GetInt(THEME_SELECT_IS_TV);
            theme->horizontalSpacing_ = themeConstants->GetDimension(THEME_SELECT_POPUP_SPACING_HORIZONTAL);
            theme->verticalSpacing_ = themeConstants->GetDimension(THEME_SELECT_POPUP_SPACING_VERTICAL);
            theme->contentSpacing_ = themeConstants->GetDimension(THEME_SELECT_POPUP_SPACING_CONTENT);

            theme->selectShowTime_ = 250; // unit is ms.
            theme->selectHideTime_ = 250; // unit is ms.
            theme->menuShowTime_ = 250;   // unit is ms.
            theme->menuHideTime_ = 250;   // unit is ms.
            theme->titleLeftPadding_ = Dimension(16.0, DimensionUnit::VP);
            theme->titleTopPadding_ = Dimension(8.0, DimensionUnit::VP);
            theme->titleRightPadding_ = Dimension(8.0, DimensionUnit::VP);
            theme->titleBottomPadding_ = Dimension(16.0, DimensionUnit::VP);
            theme->titleStyle_.SetFontSize(themeConstants->GetDimension(THEME_OHOS_TEXT_SIZE_HEADLINE7));
            std::vector<std::string> families;
            families.emplace_back("sans-serif");
            theme->titleStyle_.SetFontFamilies(families);
            theme->titleStyle_.SetFontWeight(FontWeight::W500);
            theme->titleStyle_.SetTextColor(themeConstants->GetColor(THEME_OHOS_COLOR_TEXT_PRIMARY));
            theme->titleStyle_.SetTextDecoration(TextDecoration::NONE);
            theme->optionPadding_ = Edge(SELECT_OPTION_LEFT_LENGTH, SELECT_OPTION_TOP_LENGTH,
                SELECT_OPTION_RIGHT_LENGTH, SELECT_OPTION_BOTTOM_LENGTH, DimensionUnit::VP);
            theme->optionInterval_ = theme->isTV_ ? Dimension(6.0, DimensionUnit::VP) : 0.0_vp;
            theme->tvFocusTextColor_ = Color(0xE6000000);
            theme->tvNormalBackColor_ = Color(0x33FFFFFF);
            theme->tvBackColor_ = (theme->isTV_ ? Color(0x99000000) : Color::TRANSPARENT);
            // disabled color
            theme->normalDisableColor_ = themeConstants->GetColor(THEME_SELECT_OPTION_DISABLE_COLOR);
            theme->focusedDisableColor_ = themeConstants->GetColor(THEME_SELECT_OPTION_FOCUSED_DISABLE_COLOR);
            theme->normalTextDisableColor_ = themeConstants->GetColor(THEME_SELECT_OPTION_DISABLE_TEXT_COLOR);
            theme->focusedTextDisableColor_ = themeConstants->GetColor(THEME_SELECT_OPTION_FOCUSED_DISABLE_TEXT_COLOR);
            Parse(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

        void Parse(const RefPtr<ThemeStyle>& style, const RefPtr<SelectTheme>& theme) const
        {
            if (!style || !theme) {
                return;
            }
            theme->fontColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_SPINNER_ICON, theme->fontColor_);
            theme->clickedColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_CLICK_EFFECT, theme->clickedColor_);
            theme->selectedColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_CONTROL_ACTIVE, theme->selectedColor_);
            theme->selectedColorText_ = style->GetAttr<Color>(
                THEME_ATTR_COLOR_TEXT_PRIMARY_ACTIVATED, theme->selectedColorText_);
            theme->hoverColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_HOVER, theme->hoverColor_);
            theme->backgroundColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_DIALOG_BG, theme->backgroundColor_);
            theme->lineColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_LIST_DIVIDER, theme->lineColor_);
        }
    };

    ~SelectTheme() override = default;

    RefPtr<SelectTheme> clone()
    {
        RefPtr<SelectTheme> theme = AceType::Claim(new SelectTheme());
        theme->disabledColor_ = disabledColor_;
        theme->clickedColor_ = clickedColor_;
        theme->selectedColor_ = selectedColor_;
        theme->fontSize_ = fontSize_;
        theme->fontFamily_ = fontFamily_;
        theme->fontColor_ = fontColor_;
        theme->fontWeight_ = fontWeight_;
        theme->textDecoration_ = textDecoration_;
        theme->rrectSize_ = rrectSize_;
        theme->iconSize_ = iconSize_;
        theme->normalPadding_ = normalPadding_;
        theme->optionSize_ = optionSize_;
        theme->popupRRectSize_ = popupRRectSize_;
        theme->popupMinWidth_ = popupMinWidth_;
        theme->popupShadowWidth_ = popupShadowWidth_;
        theme->popupBorderWidth_ = popupBorderWidth_;
        theme->titleLeftPadding_ = titleLeftPadding_;
        theme->titleTopPadding_ = titleTopPadding_;
        theme->titleRightPadding_ = titleRightPadding_;
        theme->titleBottomPadding_ = titleBottomPadding_;
        theme->titleStyle_ = titleStyle_;
        theme->isTV_ = isTV_;
        theme->horizontalSpacing_ = horizontalSpacing_;
        theme->verticalSpacing_ = verticalSpacing_;
        theme->contentSpacing_ = contentSpacing_;
        theme->menuHideTime_ = menuHideTime_;
        theme->menuShowTime_ = menuShowTime_;
        theme->selectShowTime_ = selectShowTime_;
        theme->selectHideTime_ = selectHideTime_;
        theme->optionPadding_ = optionPadding_;
        theme->optionInterval_ = optionInterval_;
        theme->tvFocusTextColor_ = tvFocusTextColor_;
        theme->tvNormalBackColor_ = tvNormalBackColor_;
        theme->tvBackColor_ = tvBackColor_;
        theme->focusedDisableColor_ = focusedDisableColor_;
        theme->normalDisableColor_ = normalDisableColor_;
        theme->focusedTextDisableColor_ = focusedTextDisableColor_;
        theme->normalTextDisableColor_ = normalTextDisableColor_;
        theme->backgroundColor_ = backgroundColor_;
        theme->hoverColor_ = hoverColor_;
        theme->selectedColorText_ = selectedColorText_;
        theme->lineColor_ = lineColor_;
        return theme;
    }

    const Color& GetSelectedColorText() const
    {
        return selectedColorText_;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const Color& GetDisabledColor() const
    {
        return disabledColor_;
    }
    void SetDisabledColor(const Color& value)
    {
        disabledColor_ = value;
    }

    const Color& GetClickedColor() const
    {
        return clickedColor_;
    }
    void SetClickedColor(const Color& value)
    {
        clickedColor_ = value;
    }

    const Color& GetSelectedColor() const
    {
        return selectedColor_;
    }
    void SetSelectedColor(const Color& value)
    {
        selectedColor_ = value;
    }

    const Dimension& GetFontSize() const
    {
        return fontSize_;
    }
    void SetFontSize(const Dimension& value)
    {
        fontSize_ = value;
    }

    const Color& GetFontColor() const
    {
        return fontColor_;
    }
    void SetFontColor(const Color& value)
    {
        fontColor_ = value;
    }

    const std::string& GetFontFamily() const
    {
        return fontFamily_;
    }
    void SetFontFamily(const std::string& value)
    {
        fontFamily_ = value;
    }

    FontWeight GetFontWeight() const
    {
        return fontWeight_;
    }
    void SetFontWeight(FontWeight value)
    {
        fontWeight_ = value;
    }

    TextDecoration GetTextDecoration() const
    {
        return textDecoration_;
    }
    void SetTextDecoration(TextDecoration value)
    {
        textDecoration_ = value;
    }

    std::size_t GetOptionSize() const
    {
        return optionSize_;
    }
    void SetOptionSize(std::size_t value)
    {
        optionSize_ = value;
    }

    const Dimension& GetRRectSize() const
    {
        return rrectSize_;
    }
    void SetRRectSize(const Dimension& value)
    {
        rrectSize_ = value;
    }

    const Dimension& GetPopupRRectSize() const
    {
        return popupRRectSize_;
    }
    void SetPopupRRectSize(const Dimension& value)
    {
        popupRRectSize_ = value;
    }

    const Dimension& GetPopupBorderWidth() const
    {
        return popupBorderWidth_;
    }
    void SetPopupBorderWidth(const Dimension& value)
    {
        popupBorderWidth_ = value;
    }

    const Dimension& GetPopupShadowWidth() const
    {
        return popupShadowWidth_;
    }
    void SetPopupShadowWidth(const Dimension& value)
    {
        popupShadowWidth_ = value;
    }

    const Dimension& GetPopupMinWidth() const
    {
        return popupMinWidth_;
    }
    void SetPopupMinWidth(const Dimension& value)
    {
        popupMinWidth_ = value;
    }

    const Dimension& GetNormalPadding() const
    {
        return normalPadding_;
    }
    void SetNormalPadding(const Dimension& value)
    {
        normalPadding_ = value;
    }

    const Dimension& GetIconSize() const
    {
        return iconSize_;
    }
    void SetIconSize(const Dimension& value)
    {
        iconSize_ = value;
    }

    const Dimension& GetTitleLeftPadding() const
    {
        return titleLeftPadding_;
    }
    void SetTitleLeftPadding(const Dimension& value)
    {
        titleLeftPadding_ = value;
    }

    const Dimension& GetTitleTopPadding() const
    {
        return titleTopPadding_;
    }
    void SetTitleTopPadding(const Dimension& value)
    {
        titleTopPadding_ = value;
    }

    const Dimension& GetTitleRightPadding() const
    {
        return titleRightPadding_;
    }
    void SetTitleRightPadding(const Dimension& value)
    {
        titleRightPadding_ = value;
    }

    const Dimension& GetTitleBottomPadding() const
    {
        return titleBottomPadding_;
    }
    void SetTitleBottomPadding(const Dimension& value)
    {
        titleBottomPadding_ = value;
    }

    const TextStyle& GetTitleStyle()
    {
        return titleStyle_;
    }
    void SetTitleStyle(const TextStyle& value)
    {
        titleStyle_ = value;
    }

    bool IsTV() const
    {
        return isTV_;
    }
    void SetIsTV(bool isTV)
    {
        isTV_ = isTV;
    }

    const Dimension& GetHorizontalSpacing() const
    {
        return horizontalSpacing_;
    }
    void SetHorizontalSpacing(const Dimension& horizontalSpacing)
    {
        horizontalSpacing_ = horizontalSpacing;
    }

    const Dimension& GetVerticalSpacing() const
    {
        return verticalSpacing_;
    }
    void SetVerticalSpacing(const Dimension& verticalSpacing)
    {
        verticalSpacing_ = verticalSpacing;
    }

    const Dimension& GetContentSpacing() const
    {
        return contentSpacing_;
    }
    void SetContentSpacing(const Dimension& contentSpacing)
    {
        contentSpacing_ = contentSpacing;
    }

    const Edge& GetOptionPadding() const
    {
        return optionPadding_;
    }
    void SetOptionPadding(const Edge& value)
    {
        optionPadding_ = value;
    }

    uint32_t GetShowTime(bool isMenu) const
    {
        if (isMenu) {
            return menuShowTime_;
        } else {
            return selectShowTime_;
        }
    }

    uint32_t GetHideTime(bool isMenu) const
    {
        if (isMenu) {
            return menuHideTime_;
        } else {
            return selectHideTime_;
        }
    }

    SelectTheme() = default;

    bool IsAllowScale() const
    {
        return allowScale_;
    }

    void SetAllowScale(bool allowScale)
    {
        allowScale_ = allowScale;
    }

    const Dimension& GetOptionInterval() const
    {
        return optionInterval_;
    }

    const Color& GetTvFocusTextColor() const
    {
        return tvFocusTextColor_;
    }

    const Color& GetTvNormalBackColor() const
    {
        return tvNormalBackColor_;
    }

    const Color& GetTvBackColor() const
    {
        return tvBackColor_;
    }

    const Color& GetFocusedDisableColor() const
    {
        return focusedDisableColor_;
    }

    const Color& GetNormalDisableColor() const
    {
        return normalDisableColor_;
    }

    const Color& GetFocusedTextDisableColor() const
    {
        return focusedTextDisableColor_;
    }

    const Color& GetNormalTextDisableColor() const
    {
        return normalTextDisableColor_;
    }

    const Color& GetLineColor() const
    {
        return lineColor_;
    }

private:
    Color disabledColor_;
    Color clickedColor_;
    Color selectedColor_;

    Color backgroundColor_ = Color::WHITE;
    Color hoverColor_ = Color(0x0c000000);
    Color selectedColorText_ = Color(0xff0a59f7);
    Color lineColor_ = Color(0x33000000);

    bool allowScale_ = true;
    Dimension fontSize_;
    Color fontColor_;
    std::string fontFamily_;
    FontWeight fontWeight_ { FontWeight::NORMAL };
    TextDecoration textDecoration_ { TextDecoration::NONE };

    std::size_t optionSize_ { 0 };
    Dimension rrectSize_;
    Dimension iconSize_;
    Dimension normalPadding_;

    Dimension popupRRectSize_;
    Dimension popupBorderWidth_;
    Dimension popupShadowWidth_;
    Dimension popupMinWidth_;

    Dimension titleLeftPadding_;
    Dimension titleTopPadding_;
    Dimension titleRightPadding_;
    Dimension titleBottomPadding_;
    Dimension horizontalSpacing_;
    Dimension verticalSpacing_;
    Dimension contentSpacing_;
    Dimension optionInterval_;

    Color tvFocusTextColor_;
    Color tvNormalBackColor_;
    Color tvBackColor_;

    Color focusedDisableColor_;
    Color normalDisableColor_;
    Color focusedTextDisableColor_;
    Color normalTextDisableColor_;

    TextStyle titleStyle_;
    bool isTV_ = false;
    uint32_t menuShowTime_ = 0;
    uint32_t selectShowTime_ = 0;
    uint32_t menuHideTime_ = 0;
    uint32_t selectHideTime_ = 0;

    Edge optionPadding_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_SELECT_THEME_H
