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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_CHECKABLE_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_CHECKABLE_THEME_H

#include "base/utils/system_properties.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

class CheckableTheme : public virtual Theme {
    DECLARE_ACE_TYPE(CheckableTheme, Theme);

public:
    ~CheckableTheme() override = default;

    const Color& GetPointColor() const
    {
        return pointColor_;
    }
    const Color& GetActiveColor() const
    {
        return activeColor_;
    }
    const Color& GetInactiveColor() const
    {
        return inactiveColor_;
    }
    const Color& GetFocusColor() const
    {
        return focusColor_;
    }
    const Dimension& GetWidth() const
    {
        return width_;
    }
    const Dimension& GetHeight() const
    {
        return height_;
    }
    const Dimension& GetHotZoneHorizontalPadding() const
    {
        return hotZoneHorizontalPadding_;
    }
    const Dimension& GetHotZoneVerticalPadding() const
    {
        return hotZoneVerticalPadding_;
    }
    double GetAspectRatio() const
    {
        return aspectRatio_;
    }
    const Dimension& GetDefaultWidth() const
    {
        return defaultWidth_;
    }
    const Dimension& GetDefaultHeight() const
    {
        return defaultHeight_;
    }
    double GetRadioInnerSizeRatio() const
    {
        return radioInnerSizeRatio_;
    }
    bool GetNeedFocus() const
    {
        return needFocus_;
    }
    bool IsBackgroundSolid() const
    {
        return backgroundSolid_;
    }
    const Dimension& GetBorderWidth() const
    {
        return borderWidth_;
    }

    void SetHoverColor(const Color& hoverColor)
    {
        hoverColor_ = hoverColor;
    }

    const Color& GetHoverColor() const
    {
        return hoverColor_;
    }

    const Color& GetInactivePointColor() const
    {
        return inactivePointColor_;
    }

    const Color& GetShadowColor() const
    {
        return shadowColor_;
    }

    const Dimension& GetShadowWidth() const
    {
        return shadowWidth_;
    }

    const Dimension& GetHoverRadius() const
    {
        return hoverRadius_;
    }

protected:
    CheckableTheme() = default;

    Color pointColor_;
    Color activeColor_;
    Color inactiveColor_;
    Color inactivePointColor_;
    Color focusColor_;
    Color hoverColor_;
    Color shadowColor_;
    Dimension width_;
    Dimension height_;
    Dimension hotZoneHorizontalPadding_;
    Dimension hotZoneVerticalPadding_;
    Dimension defaultWidth_;
    Dimension defaultHeight_;
    Dimension borderWidth_;
    Dimension shadowWidth_;
    Dimension hoverRadius_;
    double aspectRatio_ = 1.0;
    double radioInnerSizeRatio_ = 0.5;
    bool needFocus_ = true;
    bool backgroundSolid_ = true;
};

class CheckboxTheme : public CheckableTheme {
    DECLARE_ACE_TYPE(CheckboxTheme, CheckableTheme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<CheckboxTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<CheckboxTheme> theme = AceType::Claim(new CheckboxTheme());
            if (!themeConstants) {
                LOGI("Build AppTheme error, themeConstants is null!");
                return theme;
            }
            theme->width_ = themeConstants->GetDimension(THEME_CHECKBOX_SIZE);
            theme->height_ = theme->width_;
            theme->hotZoneHorizontalPadding_ = themeConstants->GetDimension(THEME_CHECKBOX_HOTZONE_PADDING);
            theme->hotZoneVerticalPadding_ = theme->hotZoneHorizontalPadding_;
            theme->pointColor_ = themeConstants->GetColor(THEME_CHECKBOX_POINT_COLOR);
            theme->activeColor_ = themeConstants->GetColor(THEME_CHECKBOX_ACTIVE_COLOR);
            theme->inactiveColor_ = themeConstants->GetColor(THEME_CHECKBOX_INACTIVE_COLOR);
            theme->focusColor_ = themeConstants->GetColor(THEME_CHECKBOX_FOCUS_COLOR);
            theme->defaultWidth_ = themeConstants->GetDimension(THEME_CHECKBOX_DEFAULT_SIZE);
            theme->defaultHeight_ = theme->defaultWidth_;
            theme->needFocus_ = themeConstants->GetInt(THEME_CHECKBOX_NEED_FOCUS);
            theme->backgroundSolid_ = themeConstants->GetInt(THEME_CHECKBOX_INACTIVE_BACKGROUND_SOLID);
            theme->borderWidth_ = themeConstants->GetDimension(THEME_CHECKBOX_BORDER_WIDTH);
            theme->borderRadius_ = themeConstants->GetDimension(THEME_CHECKBOX_BORDER_RADIUS);
            theme->checkStroke_ = themeConstants->GetDimension(THEME_CHECKBOX_STROKE_WIDTH);
            theme->hoverColor_ = themeConstants->GetColor(THEME_CHECKBOX_HOVER_COLOR);
            theme->inactivePointColor_ = themeConstants->GetColor(THEME_CHECKBOX_INACTIVE_POINT_COLOR);
            theme->hoverRadius_ = themeConstants->GetDimension(THEME_CHECKBOX_HOVER_RADIUS);
            theme->shadowColor_ = themeConstants->GetColor(THEME_CHECKBOX_SHADOW_COLOR);
            theme->shadowWidth_ = themeConstants->GetDimension(THEME_CHECKBOX_SHADOW_WIDTH);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<CheckboxTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            auto checkboxPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_CHECKBOX, nullptr);
            if (!checkboxPattern) {
                LOGE("Pattern of checkbox is null, please check!");
                return;
            }
            theme->pointColor_ = checkboxPattern->GetAttr<Color>(FG_COLOR_CHECKED, Color::RED);
            theme->activeColor_ = checkboxPattern->GetAttr<Color>(BG_COLOR_CHECKED, Color::RED);
            theme->inactiveColor_ = checkboxPattern->GetAttr<Color>(BG_COLOR_UNCHECKED, Color::RED);
            theme->focusColor_ = checkboxPattern->GetAttr<Color>(BG_COLOR_UNCHECKED, Color::RED);
            theme->borderRadius_ = checkboxPattern->GetAttr<Dimension>(BORDER_RADIUS, 0.0_vp);
            theme->hoverColor_ = checkboxPattern->GetAttr<Color>(HOVER_EFFECT_COLOR, Color::RED);
            theme->inactivePointColor_ = checkboxPattern->GetAttr<Color>(FG_COLOR_UNCHECKED, Color::RED);
            theme->hoverRadius_ = checkboxPattern->GetAttr<Dimension>(HOVER_EFFECT_RADIUS, 0.0_vp);
            if (SystemProperties::GetDeviceType() != DeviceType::CAR) {
                return;
            }
            theme->width_ = checkboxPattern->GetAttr<Dimension>(CHECKBOX_WIDTH, 26.0_vp);
            theme->height_ = theme->width_;
            theme->borderRadius_ = checkboxPattern->GetAttr<Dimension>(CHECKBOX_BORDER_RADIUS, 4.0_vp);
            theme->hotZoneHorizontalPadding_ =
                checkboxPattern->GetAttr<Dimension>(CHECKBOX_PADDING, 11.0_vp);
            theme->hotZoneVerticalPadding_ = theme->hotZoneHorizontalPadding_;
        }
    };

    const Dimension& GetBorderRadius() const
    {
        return borderRadius_;
    }

    const Dimension& GetCheckStroke() const
    {
        return checkStroke_;
    }

private:
    Dimension borderRadius_;
    Dimension checkStroke_;
};

class SwitchTheme : public CheckableTheme {
    DECLARE_ACE_TYPE(SwitchTheme, CheckableTheme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<SwitchTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<SwitchTheme> theme = AceType::Claim(new SwitchTheme());
            if (!themeConstants) {
                LOGE("Build AppTheme error, themeConstants is null!");
                return theme;
            }
            theme->width_ = themeConstants->GetDimension(THEME_SWITCH_WIDTH);
            theme->height_ = themeConstants->GetDimension(THEME_SWITCH_HEIGHT);
            theme->hotZoneHorizontalPadding_ = themeConstants->GetDimension(THEME_SWITCH_HOTZONE_HORIZONTAL_PADDING);
            theme->hotZoneVerticalPadding_ = themeConstants->GetDimension(THEME_SWITCH_HOTZONE_VERTICAL_PADDING);
            theme->aspectRatio_ = themeConstants->GetDouble(THEME_SWITCH_ASPECT_RATIO);
            theme->backgroundSolid_ = themeConstants->GetInt(THEME_SWITCH_INACTIVE_BACKGROUND_SOLID);
            theme->pointColor_ = themeConstants->GetColor(THEME_SWITCH_POINT_COLOR);
            theme->activeColor_ = themeConstants->GetColor(THEME_SWITCH_ACTIVE_COLOR);
            theme->inactiveColor_ = themeConstants->GetColor(THEME_SWITCH_INACTIVE_COLOR);
            theme->focusColor_ = themeConstants->GetColor(THEME_SWITCH_FOCUS_COLOR);
            theme->defaultWidth_ = themeConstants->GetDimension(THEME_SWITCH_DEFAULT_WIDTH);
            theme->defaultHeight_ = themeConstants->GetDimension(THEME_SWITCH_DEFAULT_HEIGHT);
            theme->needFocus_ = themeConstants->GetInt(THEME_SWITCH_NEED_FOCUS);
            theme->borderWidth_ = themeConstants->GetDimension(THEME_SWITCH_BORDER_WIDTH);
            theme->hoverColor_ = themeConstants->GetColor(THEME_SWITCH_HOVER_COLOR);
            theme->hoverRadius_ = themeConstants->GetDimension(THEME_SWITCH_HOVER_RADIUS);
            theme->inactivePointColor_ = themeConstants->GetColor(THEME_SWITCH_INACTIVE_POINT_COLOR);
            theme->shadowColor_ = themeConstants->GetColor(THEME_SWITCH_SHADOW_COLOR);
            theme->shadowWidth_ = themeConstants->GetDimension(THEME_SWITCH_SHADOW_WIDTH);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<SwitchTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            auto switchPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_SWITCH, nullptr);
            if (!switchPattern) {
                LOGE("Pattern of switch is null, please check!");
                return;
            }
            theme->pointColor_ = switchPattern->GetAttr<Color>(FG_COLOR_CHECKED, Color::RED);
            theme->activeColor_ = switchPattern->GetAttr<Color>(BG_COLOR_CHECKED, Color::RED);
            theme->inactiveColor_ = switchPattern->GetAttr<Color>(BG_COLOR_UNCHECKED, Color::RED);
            theme->focusColor_ = switchPattern->GetAttr<Color>(BG_COLOR_UNCHECKED, Color::RED);
            theme->hoverColor_ = switchPattern->GetAttr<Color>(HOVER_EFFECT_COLOR, Color::RED);
            theme->hoverRadius_ = switchPattern->GetAttr<Dimension>(HOVER_EFFECT_RADIUS, 0.0_vp);
            theme->inactivePointColor_ = switchPattern->GetAttr<Color>(FG_COLOR_UNCHECKED, Color::RED);
            if (SystemProperties::GetDeviceType() != DeviceType::CAR) {
                return;
            }
            theme->width_ = switchPattern->GetAttr<Dimension>(SWITCH_WIDTH, 40.0_vp);
            theme->height_ = switchPattern->GetAttr<Dimension>(SWITCH_HEIGHT, 26.0_vp);
            theme->shadowWidth_ = switchPattern->GetAttr<Dimension>(SWITCH_SHADOW_WIDTH, 2.0_vp);
            theme->hotZoneHorizontalPadding_ = switchPattern->GetAttr<Dimension>(SWITCH_HORIZONTAL_PADDING, 4.0_vp);
            theme->hotZoneVerticalPadding_ = switchPattern->GetAttr<Dimension>(SWITCH_VERTICAL_PADDING, 13.0_vp);
        }
    };
};

class RadioTheme : public CheckableTheme {
    DECLARE_ACE_TYPE(RadioTheme, CheckableTheme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<RadioTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<RadioTheme> theme = AceType::Claim(new RadioTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->width_ = themeConstants->GetDimension(THEME_RADIO_SIZE);
            theme->height_ = theme->width_;
            theme->hotZoneHorizontalPadding_ = themeConstants->GetDimension(THEME_RADIO_HOTZONE_PADDING);
            theme->hotZoneVerticalPadding_ = theme->hotZoneHorizontalPadding_;
            theme->pointColor_ = themeConstants->GetColor(THEME_RADIO_POINT_COLOR);
            theme->activeColor_ = themeConstants->GetColor(THEME_RADIO_ACTIVE_COLOR);
            theme->inactiveColor_ = themeConstants->GetColor(THEME_RADIO_INACTIVE_COLOR);
            theme->focusColor_ = themeConstants->GetColor(THEME_RADIO_FOCUS_COLOR);
            theme->defaultWidth_ = themeConstants->GetDimension(THEME_RADIO_DEFAULT_SIZE);
            theme->defaultHeight_ = theme->defaultWidth_;
            theme->radioInnerSizeRatio_ = themeConstants->GetDouble(THEME_RADIO_INNER_SIZE_RATIO);
            theme->needFocus_ = themeConstants->GetInt(THEME_RADIO_NEED_FOCUS);
            theme->backgroundSolid_ = themeConstants->GetInt(THEME_RADIO_INACTIVE_BACKGROUND_SOLID);
            theme->borderWidth_ = themeConstants->GetDimension(THEME_RADIO_BORDER_WIDTH);
            theme->hoverColor_ = themeConstants->GetColor(THEME_RADIO_HOVER_COLOR);
            theme->inactivePointColor_ = themeConstants->GetColor(THEME_RADIO_INACTIVE_POINT_COLOR);
            theme->shadowColor_ = themeConstants->GetColor(THEME_RADIO_SHADOW_COLOR);
            theme->shadowWidth_ = themeConstants->GetDimension(THEME_RADIO_SHADOW_WIDTH);
            ParsePattern(themeConstants->GetThemeStyle(), theme);
            return theme;
        }

    private:
        void ParsePattern(const RefPtr<ThemeStyle>& themeStyle, const RefPtr<RadioTheme>& theme) const
        {
            if (!themeStyle) {
                return;
            }
            auto radioPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_RADIO, nullptr);
            if (!radioPattern) {
                LOGE("Pattern of radio is null, please check!");
                return;
            }
            theme->pointColor_ = radioPattern->GetAttr<Color>(FG_COLOR_CHECKED, Color::RED);
            theme->activeColor_ = radioPattern->GetAttr<Color>(BG_COLOR_CHECKED, Color::RED);
            theme->inactiveColor_ = radioPattern->GetAttr<Color>(BG_COLOR_UNCHECKED, Color::RED);
            theme->inactivePointColor_ = radioPattern->GetAttr<Color>(FG_COLOR_UNCHECKED, Color::RED);
            theme->focusColor_ = radioPattern->GetAttr<Color>(BG_COLOR_UNCHECKED, Color::RED);
            theme->hoverColor_ = radioPattern->GetAttr<Color>(HOVER_EFFECT_COLOR, Color::RED);
            if (SystemProperties::GetDeviceType() != DeviceType::CAR) {
                return;
            }
            theme->width_ = radioPattern->GetAttr<Dimension>(RADIO_WIDTH, 26.0_vp);
            theme->height_ = theme->width_;
            theme->hotZoneHorizontalPadding_ = radioPattern->GetAttr<Dimension>(RADIO_PADDING, 11.0_vp);
            theme->hotZoneVerticalPadding_ = theme->hotZoneHorizontalPadding_;
        }
    };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_CHECKABLE_THEME_H
