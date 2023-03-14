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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DECLARATION_CLOCK_CLOCK_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DECLARATION_CLOCK_CLOCK_DECLARATION_H

#include <float.h>

#include "core/components/clock/clock_theme.h"
#include "core/components/declaration/common/declaration.h"

namespace OHOS::Ace {

struct ClockConfig final {
    // ratio of digit-radius and half of side length of clock-face-image.
    // digit-radius is used to calculate digit offset whose range is (0, 1].
    // e.g., when size of clock-face-image is 200 x 200, digit "3" is [200 / 2 x 0.7 = 70] right of the center.
    double digitRadiusRatio_ = 0.7;
    // ratio of digit-size and side length of clock-face-image, which is used to decide font-size of digit.
    // e.g., when size of clock-face-image is 200 x 200, font-size of digit is 200 x 0.08 = 16
    // its range is (0, 1.0 / 7.0].
    double digitSizeRatio_ = 0.08;

    // image sources and color for day
    std::string clockFaceSrc_;
    std::string hourHandSrc_;
    std::string minuteHandSrc_;
    std::string secondHandSrc_;
    std::string digitColor_;

    // image sources and color for night
    std::string clockFaceNightSrc_;
    std::string hourHandNightSrc_;
    std::string minuteHandNightSrc_;
    std::string secondHandNightSrc_;
    // "#00000000" represents transparent color
    std::string digitColorNight_ = "#00000000";
};

struct ClockAttribute : Attribute {
    double hoursWest = DBL_MAX;
    bool showDigit = true;
};

struct ClockStyle : Style {
    std::vector<std::string> fontFamilies;
};

struct ClockEvent : Event {
    EventMarker hourChangeEvent;
};

class ClockDeclaration : public Declaration {
    DECLARE_ACE_TYPE(ClockDeclaration, Declaration);

public:
    ClockDeclaration() = default;
    ~ClockDeclaration() override = default;

    void InitializeStyle() override;
    void SetClockConfig(const ClockConfig& clockConfig);
    void SetClockFaceSrc(const std::string& clockFaceSrc)
    {
        clockFaceSrc_ = ParseImageSrc(clockFaceSrc);
    }

    const std::string& GetClockFaceSrc() const
    {
        return clockFaceSrc_;
    }

    void SetClockFaceNightSrc(const std::string& clockFaceNightSrc)
    {
        clockFaceNightSrc_ = ParseImageSrc(clockFaceNightSrc);
    }

    const std::string& GetClockFaceNightSrc() const
    {
        return clockFaceNightSrc_;
    }

    void SetHourHandSrc(const std::string& hourHandSrc)
    {
        hourHandSrc_ = ParseImageSrc(hourHandSrc);
    }

    const std::string& GetHourHandSrc() const
    {
        return hourHandSrc_;
    }

    void SetHourHandNightSrc(const std::string& hourHandNightSrc)
    {
        hourHandNightSrc_ = ParseImageSrc(hourHandNightSrc);
    }

    const std::string& GetHourHandNightSrc() const
    {
        return hourHandNightSrc_;
    }

    void SetMinuteHandSrc(const std::string& minuteHandSrc)
    {
        minuteHandSrc_ = ParseImageSrc(minuteHandSrc);
    }

    const std::string& GetMinuteHandSrc() const
    {
        return minuteHandSrc_;
    }

    void SetMinuteHandNightSrc(const std::string& minuteHandNightSrc)
    {
        minuteHandNightSrc_ = ParseImageSrc(minuteHandNightSrc);
    }

    const std::string& GetMinuteHandNightSrc() const
    {
        return minuteHandNightSrc_;
    }

    void SetSecondHandSrc(const std::string& secondHandSrc)
    {
        secondHandSrc_ = ParseImageSrc(secondHandSrc);
    }

    const std::string& GetSecondHandSrc() const
    {
        return secondHandSrc_;
    }

    void SetSecondHandNightSrc(const std::string& secondHandNightSrc)
    {
        secondHandNightSrc_ = ParseImageSrc(secondHandNightSrc);
    }

    const std::string& GetSecondHandNightSrc() const
    {
        return secondHandNightSrc_;
    }

    void SetHoursWest(double hoursWest)
    {
        auto& attribute = MaybeResetAttribute<ClockAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.hoursWest = hoursWest;
    }

    double GetHoursWest() const
    {
        auto& attribute = static_cast<ClockAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.hoursWest;
    }

    void SetDigitColor(const std::string& digitColor)
    {
        digitColor_ = ParseColor(digitColor);
    }

    const Color& GetDigitColor() const
    {
        return digitColor_;
    }

    void SetDigitColorNight(const std::string& digitColorNight)
    {
        digitColorNight_ = ParseColor(digitColorNight);
    }

    const Color& GetDigitColorNight() const
    {
        return digitColorNight_;
    }

    void SetDefaultSize(const Dimension& defaultSize)
    {
        defaultSize_ = defaultSize;
    }

    const Dimension& GetDefaultSize() const
    {
        return defaultSize_;
    }

    void SetDigitSizeRatio(double digitSizeRatio)
    {
        digitSizeRatio_ = digitSizeRatio;
    }

    double GetDigitSizeRatio() const
    {
        return digitSizeRatio_;
    }

    void SetDigitRadiusRatio(double digitRadiusRatio)
    {
        digitRadiusRatio_ = digitRadiusRatio;
    }

    double GetDigitRadiusRatio() const
    {
        return digitRadiusRatio_;
    }

    const std::vector<std::string>& GetFontFamilies() const
    {
        auto& style = static_cast<ClockStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.fontFamilies;
    }

    void SetFontFamilies(const std::vector<std::string>& fontFamilies)
    {
        auto& style = MaybeResetStyle<ClockStyle>(StyleTag::SPECIALIZED_STYLE);
        style.fontFamilies = fontFamilies;
    }

    bool GetShowDigit() const
    {
        auto& attribute = static_cast<ClockAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.showDigit;
    }

    void SetShowDigit(bool showDigit)
    {
        auto& attribute = MaybeResetAttribute<ClockAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.showDigit = showDigit;
    }

    void SetOnHourChangeEvent(const EventMarker& hourChangeEvent)
    {
        auto& clockEvent = MaybeResetEvent<ClockEvent>(EventTag::SPECIALIZED_EVENT);
        clockEvent.hourChangeEvent = hourChangeEvent;
    }

    const EventMarker& GetOnHourChangeEvent() const
    {
        auto& clockEvent = static_cast<ClockEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return clockEvent.hourChangeEvent;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;

private:
    // hours west of Greenwich, for e.g., [hoursWest] is [-8] in  GMT+8.
    // Valid range of [hoursWest] is [-12, 14]. Set default value to DBL_MAX to use current time zone by default.
    Dimension defaultSize_;

    // ratio of digit-radius and half of side length of clock-face-image.
    // digit-radius is used to calculate digit offset whose range is (0, 1].
    // e.g., when size of clock-face-image is 200 x 200, digit "3" is [200 / 2 x 0.7 = 70] right of the center.
    double digitRadiusRatio_ = 0.7;
    // ratio of digit-size and side length of clock-face-image, which is used to decide font-size of digit.
    // e.g., when size of clock-face-image is 200 x 200, font-size of digit is 200 x 0.08 = 16
    // its range is (0, 1.0 / 7.0].
    double digitSizeRatio_ = 0.08;

    // image sources and color for day
    std::string clockFaceSrc_;
    std::string hourHandSrc_;
    std::string minuteHandSrc_;
    std::string secondHandSrc_;
    Color digitColor_;

    // image sources and color for night
    std::string clockFaceNightSrc_;
    std::string hourHandNightSrc_;
    std::string minuteHandNightSrc_;
    std::string secondHandNightSrc_;
    Color digitColorNight_ = Color::TRANSPARENT;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DECLARATION_CLOCK_CLOCK_DECLARATION_H
