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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_INDICATOR_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_INDICATOR_THEME_H

#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

class SwiperIndicatorTheme : public virtual Theme {
    DECLARE_ACE_TYPE(SwiperIndicatorTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<SwiperIndicatorTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<SwiperIndicatorTheme> theme = AceType::Claim(new SwiperIndicatorTheme());
            if (!themeConstants) {
                return theme;
            }
            theme->color_ = themeConstants->GetColor(THEME_SWIPER_INDICATOR_NORMAL_COLOR);
            theme->selectedColor_ = themeConstants->GetColor(THEME_SWIPER_INDICATOR_SELECTED_COLOR);
            theme->size_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_SIZE);
            theme->selectedSize_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_SELECTED_SIZE);
            theme->isHasMask_ = themeConstants->GetInt(THEME_SWIPER_INDICATOR_MASK);
            theme->indicatorPointPadding_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_POINT_PADDING);
            theme->digitalIndicatorTextStyle_.SetFontSize(
                themeConstants->GetDimension(THEME_SWIPER_DIGITAL_INDICATOR_FONT_SIZE));
            theme->digitalIndicatorTextStyle_.SetTextColor(
                themeConstants->GetColor(THEME_SWIPER_DIGITAL_INDICATOR_TEXT_COLOR));
            theme->startEndPadding_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_START_END_PADDING);
            theme->pressPadding_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_PRESS_PADDING);
            theme->pressPointPadding_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_PRESS_POINT_PADDING);
            theme->pressSize_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_PRESS_SIZE);
            theme->hoverSize_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_HOVER_SIZE);
            theme->hotZoneSize_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_HOTZONE_SIZE);
            theme->hotZoneColor_ = themeConstants->GetColor(THEME_SWIPER_INDICATOR_HOTZONE_COLOR);
            theme->hotZoneSize_ = themeConstants->GetDimension(THEME_SWIPER_INDICATOR_HOTZONE_SIZE);
            theme->indicatorTextFocusColor_ = themeConstants->GetColor(THEME_SWIPER_DIGITAL_INDICATOR_FOCUS_TEXT_COLOR);
            theme->isIndicatorDisabled_ = themeConstants->GetInt(THEME_SWIPER_INDICATOR_DISABLED);
            theme->animationCurve_ = AnimationCurve(themeConstants->GetInt(THEME_SWIPER_ANIMATION_CURVE));
            theme->animationOpacity_ = themeConstants->GetInt(THEME_SWIPER_ANIMATION_OPACITY);
            auto themeStyle = themeConstants->GetThemeStyle();
            if (!themeStyle) {
                return theme;
            }
            theme->digitalIndicatorTextStyle_.SetFontSize(
                themeStyle->GetAttr<Dimension>(THEME_ATTR_TEXT_SIZE_BODY2, 0.0_vp));
            auto swiperPattern = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_SWIPER, nullptr);
            if (!swiperPattern) {
                LOGE("Pattern of swiper is null, please check!");
                return theme;
            }
            theme->color_ = swiperPattern->GetAttr<Color>(INDICATOR_COLOR_UNSELECTED, Color::RED);
            theme->hotZoneColor_ = swiperPattern->GetAttr<Color>(INDICATOR_MASK_COLOR, Color::RED);
            theme->indicatorTextFocusColor_ = swiperPattern->GetAttr<Color>(INDICATOR_TEXT_COLOR_FOCUS, Color::RED);
            theme->digitalIndicatorTextStyle_.SetTextColor(
                swiperPattern->GetAttr<Color>(INDICATOR_TEXT_COLOR, Color::RED));
            theme->selectedColor_ = swiperPattern->GetAttr<Color>(INDICATOR_COLOR_SELECTED, Color::RED);
            return theme;
        }
    };

    ~SwiperIndicatorTheme() override = default;

    const Color& GetColor() const
    {
        return color_;
    }

    const Color& GetSelectedColor() const
    {
        return selectedColor_;
    }

    const Dimension& GetSize() const
    {
        return size_;
    }

    const Dimension& GetSelectedSize() const
    {
        return selectedSize_;
    }

    bool GetIndicatorMask() const
    {
        return isHasMask_;
    }

    const Dimension& GetIndicatorPointPadding() const
    {
        return indicatorPointPadding_;
    }

    const TextStyle& GetDigitalIndicatorTextStyle() const
    {
        return digitalIndicatorTextStyle_;
    }

    const Dimension& GetStartEndPadding() const
    {
        return startEndPadding_;
    }

    const Dimension& GetPressPadding() const
    {
        return pressPadding_;
    }

    const Dimension& GetPressPointPadding() const
    {
        return pressPointPadding_;
    }

    const Dimension& GetPressSize() const
    {
        return pressSize_;
    }

    const Dimension& GetHoverSize() const
    {
        return hoverSize_;
    }

    const Dimension& GetHotZoneSize() const
    {
        return hotZoneSize_;
    }

    const Color& GetHotZoneColor() const
    {
        return hotZoneColor_;
    }

    const Color& GetIndicatorTextFocusColor() const
    {
        return indicatorTextFocusColor_;
    }

    bool GetIndicatorDisabled() const
    {
        return isIndicatorDisabled_;
    }

    AnimationCurve GetAnimationCurve() const
    {
        return animationCurve_;
    }

    bool IsAnimationOpacity() const
    {
        return animationOpacity_;
    }

protected:
    SwiperIndicatorTheme() = default;

private:
    Color color_;
    Color selectedColor_;
    Dimension size_;
    Dimension selectedSize_;
    Dimension indicatorPointPadding_;
    bool isHasMask_ = false;
    TextStyle digitalIndicatorTextStyle_;
    Dimension startEndPadding_;
    Dimension pressPadding_;
    Dimension pressPointPadding_;
    Dimension pressSize_;
    Dimension hoverSize_;
    Dimension hotZoneSize_;
    Color hotZoneColor_;
    Color indicatorTextFocusColor_;
    bool isIndicatorDisabled_ = false;
    AnimationCurve animationCurve_ = { AnimationCurve::FRICTION };
    bool animationOpacity_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_INDICATOR_THEME_H
