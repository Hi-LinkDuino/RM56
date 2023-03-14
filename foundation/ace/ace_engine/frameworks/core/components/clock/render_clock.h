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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_RENDER_CLOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_RENDER_CLOCK_H

#include "base/utils/time_util.h"
#include "core/components/declaration/clock/clock_declaration.h"
#include "core/components/image/render_image.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

using DayToNightCallback = std::function<void()>;
using NightToDayCallback = std::function<void()>;
using OnHourCallback = std::function<void(const std::string& hour)>;
using AccessibilityTimeCallback = std::function<void(double hour, double minute)>;

class RenderClockHand : public RenderNode {
    DECLARE_ACE_TYPE(RenderClockHand, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override {}
    void PerformLayout() override;
    void SetHourHand(const RefPtr<RenderImage>& renderHourHand)
    {
        renderHourHand_ = renderHourHand;
        AddChild(renderHourHand_);
    }
    void SetMinuteHand(const RefPtr<RenderImage>& renderMinuteHand)
    {
        renderMinuteHand_ = renderMinuteHand;
        AddChild(renderMinuteHand_);
    }
    void SetSecondHand(const RefPtr<RenderImage>& renderSecondHand)
    {
        renderSecondHand_ = renderSecondHand;
        AddChild(renderSecondHand_);
    }
    void SetHoursWest(double hoursWest)
    {
        hoursWest_ = hoursWest;
    }
    void SetDayToNightCallback(const DayToNightCallback& dayToNightCallback)
    {
        dayToNightCallback_ = dayToNightCallback;
    }
    void SetNightToDayCallback(const NightToDayCallback& nightToDayCallback)
    {
        nightToDayCallback_ = nightToDayCallback;
    }
    void SetOnHourCallback(const OnHourCallback& hourCallback)
    {
        onHourCallback_ = hourCallback;
    }
    void SetAccessibilityTimeCallback(const AccessibilityTimeCallback& accessibilityTimeCallback)
    {
        accessibilityTimeCallback_ = accessibilityTimeCallback;
    }
    void SetIsDay(bool isLight)
    {
        isDay_ = isLight;
    }
    bool GetIsDay() const
    {
        return isDay_;
    }
    void SetNeedStop(bool needStop)
    {
        needStop_ = needStop;
    }

    virtual void RequestRenderForNextSecond() {}

protected:
    RefPtr<RenderImage> renderHourHand_;
    RefPtr<RenderImage> renderMinuteHand_;
    RefPtr<RenderImage> renderSecondHand_;
    // hours west of Greenwich, for e.g., [hoursWest] is [-8] in  UTC+8.
    // Valid range of [hoursWest] is [-14, 12]. Set default value to DBL_MAX to use current time zone by default.
    double hoursWest_ = DBL_MAX;
    DayToNightCallback dayToNightCallback_;
    NightToDayCallback nightToDayCallback_;
    OnHourCallback onHourCallback_;
    AccessibilityTimeCallback accessibilityTimeCallback_;
    bool isDay_ = true;
    bool needStop_ = false;
};

class RenderClock : public RenderNode {
    DECLARE_ACE_TYPE(RenderClock, RenderNode);

public:
    ~RenderClock() override = default;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void CalculateLayoutSize();
    void UpdateAccessibilityInfo(double hour, double minute);
    void UpdateRenderText(double digitSize, const Color& digitColor);
    void UpdateRenderImage(RefPtr<RenderImage>& renderImage, const std::string& imageSrc);
    static void LayoutClockImage(const RefPtr<RenderImage>& renderImage, const Size& imageComponentSize);
    void UseNightConfig();
    void UseDayConfig();
    void CheckNightConfig();

protected:
    RenderClock();

    bool setScreenCallback_ = false;
    std::string clockFaceNightSrc_;
    std::string hourHandNightSrc_;
    std::string minuteHandNightSrc_;
    std::string secondHandNightSrc_;
    Color digitColorNight_ = Color::TRANSPARENT;

    RefPtr<RenderClockHand> renderClockHand_;
    RefPtr<RenderImage> renderClockFace_;
    RefPtr<RenderImage> renderHourHand_;
    RefPtr<RenderImage> renderMinuteHand_;
    RefPtr<RenderImage> renderSecondHand_;
    std::vector<RefPtr<RenderText>> digitRenderNodes_;
    std::vector<RefPtr<TextComponent>> digitComponentNodes_;
    std::vector<double> radians_;
    Offset paintOffset_;
    Dimension defaultSize_;
    // ratio of digit-radius and half of side length of clock-face-image.
    // digit-radius is used to calculate digit offset whose range is (0, 1].
    // e.g., when size of clock-face-image is 200 x 200, digit "3" is [200 / 2 x 0.7 = 70] right of the center.
    double digitRadiusRatio_ = 0.7;
    // ratio of digit-size and side length of clock-face-image, which is used to decide font-size of digit.
    // e.g., when size of clock-face-image is 200 x 200, font-size of digit is 200 x 0.08 = 16
    // its range is (0, 1.0 / 7.0].
    double digitSizeRatio_ = 0.08;
    Size drawSize_;
    RefPtr<ClockDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CLOCK_RENDER_CLOCK_H
