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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_THEME_H

#include "base/geometry/dimension.h"
#include "base/utils/system_properties.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

class PickerTheme final : public virtual Theme {
    DECLARE_ACE_TYPE(PickerTheme, Theme);

public:
    class Builder final {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<PickerTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<PickerTheme> theme = AceType::Claim(new PickerTheme());
            if (!themeConstants) {
                return theme;
            }
            InitializeTextStyles(theme, themeConstants);
            theme->popupDecoration_ = AceType::MakeRefPtr<Decoration>();
            theme->popupDecoration_->SetBackgroundColor(themeConstants->GetColor(THEME_PICKER_BACK_COLOR));
            theme->popupDecoration_->SetBorderRadius(Radius(themeConstants->GetDimension(THEME_PICKER_POPUP_RADIUS)));
            theme->popupEdge_.SetLeft(themeConstants->GetDimension(THEME_PICKER_POPUP_PADDING));
            theme->popupEdge_.SetTop(themeConstants->GetDimension(THEME_PICKER_POPUP_PADDING));
            theme->popupEdge_.SetRight(themeConstants->GetDimension(THEME_PICKER_POPUP_PADDING));
            theme->popupEdge_.SetBottom(themeConstants->GetDimension(THEME_PICKER_POPUP_PADDING_BOTTOM));
            auto showOptionCount = themeConstants->GetInt(THEME_PICKER_SHOW_OPTION_COUNT);
            theme->showOptionCount_ =
                showOptionCount < 0 ? theme->showOptionCount_ : static_cast<uint32_t>(showOptionCount);
            theme->showButtons_ = static_cast<bool>(themeConstants->GetInt(THEME_PICKER_SHOW_BUTTONS));
            theme->focusColor_ = themeConstants->GetColor(THEME_PICKER_FOCUS_COLOR);
            theme->focusRadius_ = Radius(themeConstants->GetDimension(THEME_PICKER_FOCUS_RADIUS));
            theme->selectedOptionSize_ = Size(themeConstants->GetDouble(THEME_PICKER_OPTION_WIDTH),
                themeConstants->GetDouble(THEME_PICKER_SELECT_OPTION_HEIGHT));
            theme->normalOptionSize_ = Size(themeConstants->GetDouble(THEME_PICKER_OPTION_WIDTH),
                themeConstants->GetDouble(THEME_PICKER_NORMAL_OPTION_HEIGHT));
            theme->optionPadding_ = themeConstants->GetDouble(THEME_PICKER_OPTION_PADDING);
            theme->optionSizeUnit_ = DimensionUnit::VP;
            theme->jumpInterval_ = themeConstants->GetDimension(THEME_PICKER_JUMP_INTERVAL);
            theme->columnIntervalMargin_ = themeConstants->GetDimension(THEME_PICKER_COLUMN_MARGIN);
            theme->selectedOptionDecoration_ = AceType::MakeRefPtr<Decoration>();
            theme->selectedOptionDecoration_->SetBackgroundColor(
                themeConstants->GetColor(THEME_PICKER_SELECT_OPTION_BACK_COLOR));
            theme->selectedOptionDecoration_->SetBorderRadius(
                Radius(themeConstants->GetDimension(THEME_PICKER_SELECT_OPTION_RADIUS)));
            theme->focusOptionDecoration_ = AceType::MakeRefPtr<Decoration>();
            theme->focusOptionDecoration_->SetBackgroundColor(
                themeConstants->GetColor(THEME_PICKER_FOCUS_OPTION_BACK_COLOR));
            theme->focusOptionDecoration_->SetBorderRadius(
                Radius(themeConstants->GetDimension(THEME_PICKER_FOCUS_OPTION_RADIUS)));
            theme->lunarWidth_ = Dimension(36.0, DimensionUnit::VP);  // this width do not need setting by outer.
            theme->lunarHeight_ = Dimension(18.0, DimensionUnit::VP); // this height do not need setting by outer.
            theme->buttonWidth_ = themeConstants->GetDimension(THEME_PICKER_BUTTON_WIDTH);
            theme->buttonHeight_ = themeConstants->GetDimension(THEME_PICKER_BUTTON_HEIGHT);
            theme->buttonTopPadding_ = themeConstants->GetDimension(THEME_PICKER_BUTTON_TOP_PADDING);
            theme->titleBottomPadding_ = themeConstants->GetDimension(THEME_PICKER_TITLE_BOTTOM_PADDING);
            theme->popupOutDecoration_ = AceType::MakeRefPtr<Decoration>();
            theme->popupOutDecoration_->SetBackgroundColor(themeConstants->GetColor(THEME_PICKER_DIALOG_MASK_COLOR));
            auto timeSplitter = themeConstants->GetInt(THEME_PICKER_TIME_SPLITTER);
            theme->timeSplitter_ = timeSplitter < 0 ? theme->timeSplitter_ : static_cast<uint32_t>(timeSplitter);
            theme->rotateInterval_ = 15.0; // when rotate 15.0 angle handle scroll of picker column.
            theme->dividerThickness_ = themeConstants->GetDimension(THEME_PICKER_SELECT_DIVIDER_THICKNESS);
            theme->dividerSpacing_ = themeConstants->GetDimension(THEME_PICKER_SELECT_DIVIDER_SPACING);
            theme->dividerColor_ = themeConstants->GetColor(THEME_PICKER_SELECT_DIVIDER_COLOR);
            theme->gradientHeight_ = themeConstants->GetDimension(THEME_PICKER_GRADIENT_HEIGHT);
            theme->columnFixedWidth_ = themeConstants->GetDimension(THEME_PICKER_COLUMN_FIXED_WIDTH);
            theme->pressColor_ = themeConstants->GetColor(THEME_PICKER_PRESS_COLOR);
            Parse(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

        void Parse(const RefPtr<ThemeStyle>& style, const RefPtr<PickerTheme>& theme) const
        {
            if (!style || !theme || !theme->popupDecoration_) {
                return;
            }
            theme->hoverColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_HOVER, theme->hoverColor_);
            theme->pressColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_CLICK_EFFECT, theme->pressColor_);
            if (SystemProperties::GetDeviceType() != DeviceType::PHONE) {
                return; // light, dart color only for phone
            }
            theme->popupDecoration_->SetBackgroundColor(
                style->GetAttr<Color>(THEME_ATTR_COLOR_DIALOG_BG, theme->popupDecoration_->GetBackgroundColor()));
            theme->focusColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_FOCUSED, theme->focusColor_);
            auto id = (style->GetName() == "117440517" ? // the style name of semitransparent
                           THEME_ATTR_TEXT_COLOR_PRIMARY
                                                       : THEME_ATTR_COLOR_TEXT_PRIMARY_ACTIVATED);
            theme->selectedOptionStyle_.SetTextColor(
                style->GetAttr<Color>(id, theme->selectedOptionStyle_.GetTextColor()));
            theme->focusOptionStyle_.SetTextColor(theme->selectedOptionStyle_.GetTextColor());
            theme->normalOptionStyle_.SetTextColor(
                style->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_PRIMARY, theme->normalOptionStyle_.GetTextColor()));
            theme->disappearOptionStyle_.SetTextColor(theme->normalOptionStyle_.GetTextColor());
            theme->titleStyle_.SetTextColor(theme->normalOptionStyle_.GetTextColor());
            theme->dividerColor_ = style->GetAttr<Color>(THEME_ATTR_COLOR_LIST_DIVIDER, theme->dividerColor_);
        }

    private:
        void InitializeTextStyles(const RefPtr<PickerTheme>& theme, const RefPtr<ThemeConstants>& themeConstants) const
        {
            theme->selectedOptionStyle_.SetFontSize(themeConstants->GetDimension(THEME_PICKER_SELECT_OPTION_FONT_SIZE));
            theme->selectedOptionStyle_.SetTextColor(themeConstants->GetColor(THEME_PICKER_SELECT_OPTION_TEXT_COLOR));
            theme->selectedOptionStyle_.SetFontWeight(
                FontWeight(themeConstants->GetInt(THEME_PICKER_SELECT_OPTION_WEIGHT)));
            theme->selectedOptionStyle_.SetAdaptTextSize(theme->selectedOptionStyle_.GetFontSize(),
                themeConstants->GetDimension(THEME_PICKER_SELECT_OPTION_MIN_FONT_SIZE));
            theme->selectedOptionStyle_.SetMaxLines(1);
            theme->selectedOptionStyle_.SetTextOverflow(TextOverflow::ELLIPSIS);
            theme->focusOptionStyle_.SetFontSize(themeConstants->GetDimension(THEME_PICKER_FOCUS_OPTION_FONT_SIZE));
            theme->focusOptionStyle_.SetTextColor(themeConstants->GetColor(THEME_PICKER_FOCUS_OPTION_TEXT_COLOR));
            theme->focusOptionStyle_.SetFontWeight(
                FontWeight(themeConstants->GetInt(THEME_PICKER_FOCUS_OPTION_WEIGHT)));
            theme->focusOptionStyle_.SetAdaptTextSize(theme->focusOptionStyle_.GetFontSize(),
                themeConstants->GetDimension(THEME_PICKER_SELECT_OPTION_MIN_FONT_SIZE));
            theme->focusOptionStyle_.SetMaxLines(1);
            theme->focusOptionStyle_.SetTextOverflow(TextOverflow::ELLIPSIS);
            theme->normalOptionStyle_.SetFontSize(themeConstants->GetDimension(THEME_PICKER_NORMAL_OPTION_FONT_SIZE));
            theme->normalOptionStyle_.SetTextColor(themeConstants->GetColor(THEME_PICKER_NORMAL_OPTION_FONT_COLOR));
            theme->normalOptionStyle_.SetFontWeight(
                FontWeight(themeConstants->GetInt(THEME_PICKER_NORMAL_OPTION_WEIGHT)));
            theme->normalOptionStyle_.SetAdaptTextSize(theme->normalOptionStyle_.GetFontSize(),
                themeConstants->GetDimension(THEME_PICKER_NORMAL_OPTION_MIN_FONT_SIZE));
            theme->normalOptionStyle_.SetMaxLines(1);
            theme->normalOptionStyle_.SetTextOverflow(TextOverflow::ELLIPSIS);
            theme->normalOptionStyle_.SetFontWeight(
                FontWeight(themeConstants->GetInt(THEME_PICKER_NORMAL_OPTION_WEIGHT)));
            theme->disappearOptionStyle_.SetFontSize(
                themeConstants->GetDimension(THEME_PICKER_DISAPPEAR_OPTION_FONT_SIZE));
            theme->disappearOptionStyle_.SetTextColor(themeConstants->GetColor(THEME_PICKER_NORMAL_OPTION_FONT_COLOR));
            theme->normalOptionStyle_.SetFontWeight(
                FontWeight(themeConstants->GetInt(THEME_PICKER_NORMAL_OPTION_WEIGHT)));
            theme->disappearOptionStyle_.SetAdaptTextSize(theme->disappearOptionStyle_.GetFontSize(),
                themeConstants->GetDimension(THEME_PICKER_NORMAL_OPTION_MIN_FONT_SIZE));
            theme->disappearOptionStyle_.SetMaxLines(1);
            theme->disappearOptionStyle_.SetTextOverflow(TextOverflow::ELLIPSIS);
            theme->buttonStyle_.SetFontSize(themeConstants->GetDimension(THEME_PICKER_BUTTON_FONT_SIZE));
            theme->buttonStyle_.SetTextColor(themeConstants->GetColor(THEME_PICKER_BUTTON_TEXT_COLOR));
            theme->titleStyle_.SetFontSize(themeConstants->GetDimension(THEME_PICKER_TITLE_FONT_SIZE));
            theme->titleStyle_.SetTextColor(themeConstants->GetColor(THEME_PICKER_TITLE_TEXT_COLOR));
            theme->titleStyle_.SetFontWeight(FontWeight::W500);
            theme->titleStyle_.SetMaxLines(1);
            theme->titleStyle_.SetTextOverflow(TextOverflow::ELLIPSIS);
            if (SystemProperties::GetDeviceType() == DeviceType::PHONE) {
                theme->focusOptionStyle_ = theme->selectedOptionStyle_; // focus style the same with selected on phone
            }
        }
    };

    ~PickerTheme() override = default;

    RefPtr<PickerTheme> clone() const
    {
        auto theme = AceType::Claim(new PickerTheme());
        theme->selectedOptionSize_ = selectedOptionSize_;
        theme->selectedOptionStyle_ = selectedOptionStyle_;
        theme->normalOptionSize_ = normalOptionSize_;
        theme->normalOptionStyle_ = normalOptionStyle_;
        theme->disappearOptionStyle_ = disappearOptionStyle_;
        theme->showOptionCount_ = showOptionCount_;
        theme->optionSizeUnit_ = optionSizeUnit_;
        theme->popupDecoration_ = popupDecoration_;
        theme->focusColor_ = focusColor_;
        theme->popupEdge_ = popupEdge_;
        theme->focusOptionStyle_ = focusOptionStyle_;
        theme->focusOptionDecoration_ = focusOptionDecoration_;
        theme->selectedOptionDecoration_ = selectedOptionDecoration_;
        theme->buttonStyle_ = buttonStyle_;
        theme->showButtons_ = showButtons_;
        theme->buttonWidth_ = buttonWidth_;
        theme->buttonHeight_ = buttonHeight_;
        theme->buttonTopPadding_ = buttonTopPadding_;
        theme->jumpInterval_ = jumpInterval_;
        theme->columnIntervalMargin_ = columnIntervalMargin_;
        theme->focusRadius_ = focusRadius_;
        theme->optionPadding_ = optionPadding_;
        theme->titleStyle_ = titleStyle_;
        theme->titleBottomPadding_ = titleBottomPadding_;
        theme->popupOutDecoration_ = popupOutDecoration_;
        theme->lunarWidth_ = lunarWidth_;
        theme->lunarHeight_ = lunarHeight_;
        theme->timeSplitter_ = timeSplitter_;
        theme->rotateInterval_ = rotateInterval_;
        theme->dividerThickness_ = dividerThickness_;
        theme->dividerSpacing_ = dividerSpacing_;
        theme->dividerColor_ = dividerColor_;
        theme->gradientHeight_ = gradientHeight_;
        theme->columnFixedWidth_ = columnFixedWidth_;
        theme->disappearOptionStyle_ = disappearOptionStyle_;
        theme->pressColor_ = pressColor_;
        theme->hoverColor_ = hoverColor_;
        return theme;
    }

    const TextStyle& GetOptionStyle(bool selected, bool focus) const
    {
        if (!selected) {
            return normalOptionStyle_;
        }

        if (focus) {
            return focusOptionStyle_;
        }

        return selectedOptionStyle_;
    }
    void SetOptionStyle(bool selected, bool focus, const TextStyle& value)
    {
        if (!selected) {
            normalOptionStyle_ = value;
        } else if (focus) {
            focusOptionStyle_ = value;
        } else {
            selectedOptionStyle_ = value;
        }
    }

    const TextStyle& GetDisappearOptionStyle() const
    {
        return disappearOptionStyle_;
    }
    void SetDisappearOptionStyle(const TextStyle& value)
    {
        disappearOptionStyle_ = value;
    }

    const TextStyle& GetButtonStyle() const
    {
        return buttonStyle_;
    }

    const RefPtr<Decoration>& GetOptionDecoration(bool focus)
    {
        if (focus) {
            return focusOptionDecoration_;
        }

        return selectedOptionDecoration_;
    }
    void SetOptionDecoration(bool focus, const RefPtr<Decoration>& value)
    {
        if (focus) {
            focusOptionDecoration_ = value;
        } else {
            selectedOptionDecoration_ = value;
        }
    }

    const Size& GetOptionSize(bool selected) const
    {
        if (selected) {
            return selectedOptionSize_;
        }

        return normalOptionSize_;
    }

    uint32_t GetShowOptionCount() const
    {
        return showOptionCount_;
    }

    DimensionUnit GetOptionSizeUnit() const
    {
        return optionSizeUnit_;
    }

    const RefPtr<Decoration>& GetPopupDecoration(bool isOutBox) const
    {
        if (!isOutBox) {
            return popupDecoration_;
        }
        return popupOutDecoration_;
    }

    const Color& GetFocusColor() const
    {
        return focusColor_;
    }

    const Edge& GetPopupEdge() const
    {
        return popupEdge_;
    }

    bool GetShowButtons() const
    {
        return showButtons_;
    }

    const Dimension& GetButtonWidth() const
    {
        return buttonWidth_;
    }

    const Dimension& GetButtonHeight() const
    {
        return buttonHeight_;
    }

    const Dimension& GetButtonTopPadding() const
    {
        return buttonTopPadding_;
    }

    const Dimension& GetJumpInterval() const
    {
        return jumpInterval_;
    }

    const Dimension& GetColumnIntervalMargin() const
    {
        return columnIntervalMargin_;
    }

    const Radius& GetFocusRadius() const
    {
        return focusRadius_;
    }

    double GetOptionPadding() const
    {
        return optionPadding_;
    }
    void SetOptionPadding(double value)
    {
        optionPadding_ = value;
    }

    const TextStyle& GetTitleStyle() const
    {
        return titleStyle_;
    }

    const Dimension& GetTitleBottomPadding() const
    {
        return titleBottomPadding_;
    }

    const Dimension& GetLunarWidth() const
    {
        return lunarWidth_;
    }

    const Dimension& GetLunarHeight() const
    {
        return lunarHeight_;
    }

    bool HasTimeSplitter() const
    {
        return (timeSplitter_ > 0);
    }

    double GetRotateInterval() const
    {
        return rotateInterval_;
    }

    const Dimension& GetDividerThickness() const
    {
        return dividerThickness_;
    }

    const Dimension& GetDividerSpacing() const
    {
        return dividerSpacing_;
    }

    const Color& GetDividerColor() const
    {
        return dividerColor_;
    }

    const Dimension& GetGradientHeight() const
    {
        return gradientHeight_;
    }

    const Dimension& GetColumnFixedWidth() const
    {
        return columnFixedWidth_;
    }

    Dimension GetColumnBottomTotalHeight(bool hasLunar) const
    {
        if (hasLunar) {
            return buttonHeight_ + lunarHeight_ + buttonTopPadding_ * 2 + popupEdge_.Bottom();
        } else {
            return buttonHeight_ + buttonTopPadding_ + popupEdge_.Bottom();
        }
    }

    const Color& GetPressColor() const
    {
        return pressColor_;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

private:
    PickerTheme() = default;

    Color focusColor_;
    Color hoverColor_;
    Color pressColor_;
    Radius focusRadius_;
    uint32_t showOptionCount_ = 0;
    bool showButtons_ = false;
    Dimension jumpInterval_;

    // popup style
    RefPtr<Decoration> popupDecoration_;
    RefPtr<Decoration> popupOutDecoration_;
    Edge popupEdge_;

    // column
    Dimension columnIntervalMargin_;

    // text style
    TextStyle focusOptionStyle_;
    TextStyle selectedOptionStyle_;
    TextStyle normalOptionStyle_;
    TextStyle disappearOptionStyle_;
    TextStyle buttonStyle_;
    TextStyle titleStyle_;

    // text around decoration
    RefPtr<Decoration> selectedOptionDecoration_;
    RefPtr<Decoration> focusOptionDecoration_;

    // option size
    Size selectedOptionSize_;
    Size normalOptionSize_;
    double optionPadding_ = 0.0;
    DimensionUnit optionSizeUnit_ = DimensionUnit::PX;

    // buttons size
    Dimension buttonWidth_;
    Dimension buttonHeight_;
    Dimension buttonTopPadding_;
    Dimension titleBottomPadding_;

    // lunar size
    Dimension lunarWidth_;
    Dimension lunarHeight_;

    uint32_t timeSplitter_ = 0;

    double rotateInterval_ = 0.0;
    Dimension dividerThickness_;
    Dimension dividerSpacing_;
    Color dividerColor_;
    Dimension gradientHeight_;
    Dimension columnFixedWidth_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_THEME_H
