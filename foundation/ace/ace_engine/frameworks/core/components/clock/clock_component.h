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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_CLOCK_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_CLOCK_COMPONENT_H

#include "core/components/clock/clock_element.h"
#include "core/components/clock/render_clock.h"
#include "core/components/declaration/clock/clock_declaration.h"

namespace OHOS::Ace {

class ClockComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(ClockComponent, ComponentGroup);

public:
    ClockComponent();
    ~ClockComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderClock::Create();
    }

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<ClockElement>();
    }
    void SetClockFaceSrc(const std::string& clockFaceSrc);
    void SetClockFaceNightSrc(const std::string& clockFaceNightSrc);
    void SetHourHandSrc(const std::string& hourHandSrc);
    void SetHourHandNightSrc(const std::string& hourHandNightSrc);
    void SetMinuteHandSrc(const std::string& minuteHandSrc);
    void SetMinuteHandNightSrc(const std::string& minuteHandNightSrc);
    void SetSecondHandSrc(const std::string& secondHandSrc);
    void SetSecondHandNightSrc(const std::string& secondHandNightSrc);
    void SetHoursWest(double hoursWest);
    void SetDigitColor(const std::string& digitColor);
    void SetDigitColorNight(const std::string& digitColorNight);
    void SetDefaultSize(const Dimension& defaultSize);
    void SetDigitSizeRatio(double digitSizeRatio);
    void SetDigitRadiusRatio(double digitRadiusRatio);
    void SetFontFamilies(const std::vector<std::string>& fontFamilies);
    void SetShowDigit(bool showDigit);
    void SetOnHourChangeEvent(const EventMarker& onHourChangeEvent);
    const std::string& GetClockFaceSrc() const;
    const std::string& GetClockFaceNightSrc() const;
    const std::string& GetHourHandSrc() const;
    const std::string& GetHourHandNightSrc() const;
    const std::string& GetMinuteHandSrc() const;
    const std::string& GetMinuteHandNightSrc() const;
    const std::string& GetSecondHandSrc() const;
    const std::string& GetSecondHandNightSrc() const;
    double GetHoursWest() const;
    const Color& GetDigitColor() const;
    const Color& GetDigitColorNight() const;
    const Dimension& GetDefaultSize() const;
    double GetDigitSizeRatio() const;
    double GetDigitRadiusRatio() const;
    const std::vector<std::string>& GetFontFamilies() const;
    bool GetShowDigit() const;
    const EventMarker& GetOnHourChangeEvent() const;

    void SetDeclaration(const RefPtr<ClockDeclaration>& declaration);
    const RefPtr<ClockDeclaration>& GetDeclaration() const;

private:
    RefPtr<ClockDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_CLOCK_COMPONENT_H
