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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_ROSEN_RENDER_CALENDAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_ROSEN_RENDER_CALENDAR_H

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/txt/src/txt/text_style.h"

#include "core/components/calendar/render_calendar.h"

namespace OHOS::Rosen {
    class DrawCmdList;
}
namespace OHOS::Ace {

class ScopedCanvas;

class RosenRenderCalendar : public RenderCalendar {
    DECLARE_ACE_TYPE(RosenRenderCalendar, RenderCalendar);

public:
    RosenRenderCalendar() = default;
    ~RosenRenderCalendar() override = default;

    void Update(const RefPtr<Component>& component) override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void PerformLayout() override;

private:
    void DrawWeekAndDates(SkCanvas* canvas, Offset offset);
    void DrawFocusedArea(SkCanvas* canvas, const Offset& offset, const CalendarDay& day, double x, double y) const;
    void DrawWeek(SkCanvas* canvas, const Offset& offset) const;
    void DrawBlurArea(SkCanvas* canvas, const Offset& offset, double x, double y) const;
    void DrawTouchedArea(RenderContext& context, Offset offset) const;
    void PaintDay(SkCanvas* canvas, const Offset& offset, const CalendarDay& day, txt::TextStyle& textStyle) const;
    void PaintLunarDay(
        SkCanvas* canvas, const Offset& offset, const CalendarDay& day, const txt::TextStyle& textStyle) const;
    void SetNonFocusStyle(const CalendarDay& day, txt::TextStyle& dateTextStyle, txt::TextStyle& lunarTextStyle);
    void DrawCardCalendar(
        SkCanvas* canvas, const Offset& offset, const Offset& dayOffset, const CalendarDay& day, int32_t dateNumber);
    void DrawTvCalendar(
        SkCanvas* canvas, const Offset& offset, const Offset& dayOffset, const CalendarDay& day, int32_t dateNumber);
    void InitTextStyle(txt::TextStyle& dateTextStyle, txt::TextStyle& lunarTextStyle);
    void PaintUnderscore(SkCanvas* canvas, const Offset& offset, const CalendarDay& day);
    void PaintScheduleMarker(SkCanvas* canvas, const Offset& offset, const CalendarDay& day);
    void InitWorkStateStyle(
        const CalendarDay& day, const Offset& offset, txt::TextStyle& workStateStyle, Rect& boxRect) const;
    void SetWorkStateStyle(
        const CalendarDay& day, SkColor workColor, SkColor offColor, txt::TextStyle& workStateStyle) const;
    void SetCalendarTheme();
    bool IsOffDay(const CalendarDay& day) const;
    void AddContentLayer(RenderContext& context);

    bool needShrink_ = false;
    double weekFontSize_ = 0.0;
    double dayFontSize_ = 0.0;
    double lunarDayFontSize_ = 0.0;
    double workDayMarkSize_ = 0.0;
    double offDayMarkSize_ = 0.0;
    double focusedAreaRadius_ = 0.0;
    double topPadding_ = 0.0;
    double weekWidth_ = 0.0;
    double weekAndDayRowSpace_ = 0.0;
    double gregorianCalendarHeight_ = 0.0;
    double workStateWidth_ = 0.0;
    double workStateHorizontalMovingDistance_ = 0.0;
    double workStateVerticalMovingDistance_ = 0.0;
    double touchCircleStrokeWidth_ = 0.0;

    SkColor weekColor_;
    SkColor touchColor_;
    SkColor dayColor_;
    SkColor lunarColor_;
    SkColor weekendDayColor_;
    SkColor weekendLunarColor_;
    SkColor todayDayColor_;
    SkColor todayLunarColor_;
    SkColor nonCurrentMonthDayColor_;
    SkColor nonCurrentMonthLunarColor_;
    SkColor workDayMarkColor_;
    SkColor offDayMarkColor_;
    SkColor nonCurrentMonthWorkDayMarkColor_;
    SkColor nonCurrentMonthOffDayMarkColor_;
    SkColor focusedDayColor_;
    SkColor focusedLunarColor_;
    SkColor focusedAreaBackgroundColor_;
    SkColor blurAreaBackgroundColor_;
    SkColor markLunarColor_;

    Size lastLayoutSize_;
    FontWeight dayFontWeight_ = FontWeight::W500;
    FontWeight lunarDayFontWeight_ = FontWeight::W500;
    FontWeight workStateFontWeight_ = FontWeight::W400;
    std::shared_ptr<Rosen::DrawCmdList> drawCmdList_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_ROSEN_RENDER_CALENDAR_H
