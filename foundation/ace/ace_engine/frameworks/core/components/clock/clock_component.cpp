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

#include "core/components/clock/clock_component.h"

namespace OHOS::Ace {

ClockComponent::ClockComponent()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<ClockDeclaration>();
        declaration_->Init();
    }
}

void ClockComponent::SetClockFaceSrc(const std::string& clockFaceSrc)
{
    declaration_->SetClockFaceSrc(clockFaceSrc);
}

const std::string& ClockComponent::GetClockFaceSrc() const
{
    return declaration_->GetClockFaceSrc();
}

void ClockComponent::SetClockFaceNightSrc(const std::string& clockFaceNightSrc)
{
    declaration_->SetClockFaceNightSrc(clockFaceNightSrc);
}

const std::string& ClockComponent::GetClockFaceNightSrc() const
{
    return declaration_->GetClockFaceNightSrc();
}

void ClockComponent::SetHourHandSrc(const std::string& hourHandSrc)
{
    declaration_->SetHourHandSrc(hourHandSrc);
}

const std::string& ClockComponent::GetHourHandSrc() const
{
    return declaration_->GetHourHandSrc();
}

void ClockComponent::SetHourHandNightSrc(const std::string& hourHandNightSrc)
{
    declaration_->SetHourHandNightSrc(hourHandNightSrc);
}

const std::string& ClockComponent::GetHourHandNightSrc() const
{
    return declaration_->GetHourHandNightSrc();
}

void ClockComponent::SetMinuteHandSrc(const std::string& minuteHandSrc)
{
    declaration_->SetMinuteHandSrc(minuteHandSrc);
}

const std::string& ClockComponent::GetMinuteHandSrc() const
{
    return declaration_->GetMinuteHandSrc();
}

void ClockComponent::SetMinuteHandNightSrc(const std::string& minuteHandNightSrc)
{
    declaration_->SetMinuteHandNightSrc(minuteHandNightSrc);
}

const std::string& ClockComponent::GetMinuteHandNightSrc() const
{
    return declaration_->GetMinuteHandNightSrc();
}

void ClockComponent::SetSecondHandSrc(const std::string& secondHandSrc)
{
    declaration_->SetSecondHandSrc(secondHandSrc);
}

const std::string& ClockComponent::GetSecondHandSrc() const
{
    return declaration_->GetSecondHandSrc();
}

void ClockComponent::SetSecondHandNightSrc(const std::string& secondHandNightSrc)
{
    declaration_->SetSecondHandNightSrc(secondHandNightSrc);
}

const std::string& ClockComponent::GetSecondHandNightSrc() const
{
    return declaration_->GetSecondHandNightSrc();
}

void ClockComponent::SetHoursWest(double hoursWest)
{
    declaration_->SetHoursWest(hoursWest);
}

double ClockComponent::GetHoursWest() const
{
    return declaration_->GetHoursWest();
}

void ClockComponent::SetDigitColor(const std::string& digitColor)
{
    declaration_->SetDigitColor(digitColor);
}

const Color& ClockComponent::GetDigitColor() const
{
    return declaration_->GetDigitColor();
}

void ClockComponent::SetDigitColorNight(const std::string& digitColorNight)
{
    declaration_->SetDigitColorNight(digitColorNight);
}

const Color& ClockComponent::GetDigitColorNight() const
{
    return declaration_->GetDigitColorNight();
}

void ClockComponent::SetDefaultSize(const Dimension& defaultSize)
{
    declaration_->SetDefaultSize(defaultSize);
}

const Dimension& ClockComponent::GetDefaultSize() const
{
    return declaration_->GetDefaultSize();
}

void ClockComponent::SetDigitSizeRatio(double digitSizeRatio)
{
    declaration_->SetDigitSizeRatio(digitSizeRatio);
}

double ClockComponent::GetDigitSizeRatio() const
{
    return declaration_->GetDigitSizeRatio();
}

void ClockComponent::SetDigitRadiusRatio(double digitRadiusRatio)
{
    declaration_->SetDigitRadiusRatio(digitRadiusRatio);
}

double ClockComponent::GetDigitRadiusRatio() const
{
    return declaration_->GetDigitRadiusRatio();
}

const std::vector<std::string>& ClockComponent::GetFontFamilies() const
{
    return declaration_->GetFontFamilies();
}

void ClockComponent::SetFontFamilies(const std::vector<std::string>& fontFamilies)
{
    declaration_->SetFontFamilies(fontFamilies);
}

bool ClockComponent::GetShowDigit() const
{
    return declaration_->GetShowDigit();
}

void ClockComponent::SetShowDigit(bool showDigit)
{
    declaration_->SetShowDigit(showDigit);
}

void ClockComponent::SetOnHourChangeEvent(const EventMarker& onHourChangeEvent)
{
    declaration_->SetOnHourChangeEvent(onHourChangeEvent);
}

const EventMarker& ClockComponent::GetOnHourChangeEvent() const
{
    return declaration_->GetOnHourChangeEvent();
}

const RefPtr<ClockDeclaration>& ClockComponent::GetDeclaration() const
{
    return declaration_;
}

void ClockComponent::SetDeclaration(const RefPtr<ClockDeclaration>& declaration)
{
    declaration_ = declaration;
}

} // namespace OHOS::Ace