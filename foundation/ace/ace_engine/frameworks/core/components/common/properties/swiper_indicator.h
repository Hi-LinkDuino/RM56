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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_SWIPER_INDICATOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_SWIPER_INDICATOR_H

#include "core/components/common/properties/color.h"
#include "core/components/swiper/swiper_indicator_theme.h"

namespace OHOS::Ace {

class ACE_EXPORT SwiperIndicator : public AceType {
    DECLARE_ACE_TYPE(SwiperIndicator, AceType);

public:
    void InitStyle(const RefPtr<SwiperIndicatorTheme>& theme)
    {
        if (!theme) {
            LOGD("param theme is nullptr");
            return;
        }
        color_ = theme->GetColor();
        selectedColor_ = theme->GetSelectedColor();
        size_ = theme->GetSize();
        selectedSize_ = theme->GetSelectedSize();
        isHasIndicatorMask_ = theme->GetIndicatorMask();
        indicatorPointPadding_ = theme->GetIndicatorPointPadding();
        digitalIndicatorTextStyle_ = theme->GetDigitalIndicatorTextStyle();
        startEndPadding_ = theme->GetStartEndPadding();
        pressPadding_ = theme->GetPressPadding();
        pressPointPadding_ = theme->GetPressPointPadding();
        pressSize_ = theme->GetPressSize();
        hoverSize_ = theme->GetHoverSize();
        hotZoneSize_ = theme->GetHotZoneSize();
        hotZoneColor_ = theme->GetHotZoneColor();
        indicatorTextFocusColor_ = theme->GetIndicatorTextFocusColor();
        isIndicatorDisabled_ = theme->GetIndicatorDisabled();
    }

    const Color& GetColor() const
    {
        return color_;
    }
    void SetColor(const Color& color)
    {
        color_ = color;
    }

    const Color& GetSelectedColor() const
    {
        return selectedColor_;
    }
    void SetSelectedColor(const Color& selectedColor)
    {
        selectedColor_ = selectedColor;
    }

    const Dimension& GetSize() const
    {
        return size_;
    }
    void SetSize(const Dimension& size)
    {
        size_ = size;
    }

    const Dimension& GetSelectedSize() const
    {
        return selectedSize_;
    }
    void SetSelectedSize(const Dimension& selectedSize)
    {
        selectedSize_ = selectedSize;
    }

    const Dimension& GetTop() const
    {
        return top_;
    }
    void SetTop(const Dimension& top)
    {
        top_ = top;
    }

    const Dimension& GetLeft() const
    {
        return left_;
    }
    void SetLeft(const Dimension& left)
    {
        left_ = left;
    }

    const Dimension& GetBottom() const
    {
        return bottom_;
    }
    void SetBottom(const Dimension& bottom)
    {
        bottom_ = bottom;
    }

    const Dimension& GetRight() const
    {
        return right_;
    }

    void SetRight(const Dimension& right)
    {
        right_ = right;
    }

    bool GetIndicatorMask() const
    {
        return isHasIndicatorMask_;
    }

    void SetIndicatorMask(bool isHasIndicatorMask)
    {
        isHasIndicatorMask_ = isHasIndicatorMask;
    }

    void SetIndicatorPointPadding(const Dimension& indicatorPointPadding)
    {
        indicatorPointPadding_ = indicatorPointPadding;
    }

    const Dimension& GetIndicatorPointPadding() const
    {
        return indicatorPointPadding_;
    }

    static constexpr double DEFAULT_POSITION { -1.0 };

    const TextStyle& GetDigitalIndicatorTextStyle() const
    {
        return digitalIndicatorTextStyle_;
    }

    void SetStartEndPadding(const Dimension& startEndPadding)
    {
        startEndPadding_ = startEndPadding;
    }

    const Dimension& GetStartEndPadding() const
    {
        return startEndPadding_;
    }

    void SetPressPadding(const Dimension& pressPadding)
    {
        pressPadding_ = pressPadding;
    }

    const Dimension& GetPressPadding() const
    {
        return pressPadding_;
    }

    void SetPressPointPadding(const Dimension& pressPointPadding)
    {
        pressPointPadding_ = pressPointPadding;
    }

    const Dimension& GetPressPointPadding() const
    {
        return pressPointPadding_;
    }

    void SetPressSize(const Dimension& pressSize)
    {
        pressSize_ = pressSize;
    }

    const Dimension& GetPressSize() const
    {
        return pressSize_;
    }

    void SetHoverSize(const Dimension& hoverSize)
    {
        hoverSize_ = hoverSize;
    }

    const Dimension& GetHoverSize() const
    {
        return hoverSize_;
    }

    void SetHotZoneSize(const Dimension& hotZoneSize)
    {
        hotZoneSize_ = hotZoneSize;
    }

    const Dimension& GetHotZoneSize() const
    {
        return hotZoneSize_;
    }

    void SetHotZoneColor(const Color& hotZoneColor)
    {
        hotZoneColor_ = hotZoneColor;
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

    void SetIndicatorDisabled(bool isIndicatorDisabled)
    {
        isIndicatorDisabled_ = isIndicatorDisabled;
    }

private:
    Color color_;
    Color selectedColor_;
    Dimension size_;
    Dimension selectedSize_;
    Dimension top_ { DEFAULT_POSITION, DimensionUnit::PX };
    Dimension left_ { DEFAULT_POSITION, DimensionUnit::PX };
    Dimension bottom_ { DEFAULT_POSITION, DimensionUnit::PX };
    Dimension right_ { DEFAULT_POSITION, DimensionUnit::PX };
    Dimension indicatorPointPadding_ { 8.0, DimensionUnit::VP };
    bool isHasIndicatorMask_ { false };
    TextStyle digitalIndicatorTextStyle_;
    Dimension startEndPadding_;
    Dimension pressPadding_;
    Dimension pressPointPadding_;
    Dimension pressSize_;
    Dimension hoverSize_;
    Dimension hotZoneSize_;
    Color hotZoneColor_;
    Color indicatorTextFocusColor_;
    bool isIndicatorDisabled_ { false };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_SWIPER_INDICATOR_H
