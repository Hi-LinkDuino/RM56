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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_RENDER_CALENDAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_RENDER_CALENDAR_H

#include "core/components/calendar/calendar_component.h"
#include "core/components/calendar/focusable_grid.h"
#include "core/gestures/click_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderCalendar : public RenderNode, protected FocusableGrid, public CalendarDataChangeListener {
    DECLARE_ACE_TYPE(RenderCalendar, RenderNode, FocusableGrid, CalendarDataChangeListener);

public:
    RenderCalendar();
    ~RenderCalendar() override = default;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void OnPaintFinish() override;
    void PerformLayout() override;
    void OnStatusChanged(RenderStatus renderStatus) override;
    void OnDataChanged(const CalendarDaysOfMonth& data) override;
    void OnSelectedDay(int32_t selected) override;
    void OnFocusChanged(bool focusStatus) override;
    void HandleClick(const Offset& offset);
    void UpdateCardCalendarAttr(CardCalendarAttr&& attr) override;
    void UpdateBreakInformation();
    void OnSwiperMove() override;

    const RefPtr<CalendarController>& GetCalendarController() const
    {
        return calendarController_;
    }

    const RefPtr<CalendarDataAdapter>& GetCalendarDataAdapter() const
    {
        return dataAdapter_;
    }

    bool GetShowHoliday() const
    {
        return showHoliday_;
    }

    bool GetNeedSlide() const
    {
        return needSlide_;
    }

    Axis GetAxis() const
    {
        return axis_;
    }

    int32_t GetStartDayOfWeek() const
    {
        return startDayOfWeek_;
    }

    const std::string& GetOffdays() const
    {
        return offDays_;
    }

    const CalendarThemeStructure& GetCalendarTheme() const
    {
        return calendarTheme_;
    }

protected:
    int32_t GetIndexByGrid(int32_t row, int32_t column) override;
    void FocusChanged(int32_t oldIndex, int32_t newIndex) override;
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    int32_t JudgeArea(const Offset& offset);
    bool IsValid(int32_t index) const;
    bool IsToday(const CalendarDay& day) const;

    std::vector<std::string> weekNumbers_;
    std::vector<CalendarDay> calendarDays_;
    std::string offDays_;
    CalendarMonth currentMonth_;
    CalendarThemeStructure calendarTheme_;
    TextDirection textDirection_ = TextDirection::LTR;
    int32_t indexOfContainer_ = 0;
    int32_t lastDayIndex_ = 0;
    int32_t touchIndex_ = -1;
    double dayWidth_ = 0.0;
    double dayHeight_ = 0.0;
    double weekHeight_ = 0.0;
    double maxWidth_ = 0.0;
    double maxHeight_ = 0.0;
    double colSpace_ = 0.0;
    double dailyFiveRowSpace_ = 0.0;
    double dailySixRowSpace_ = 0.0;

    // default selected first day of month
    bool cardCalendar_ = false;
    bool calendarFocusStatus_ = false;
    bool hasRequestFocus_ = false;
    bool hasTouched_ = false;
    bool showHoliday_ = true;
    bool isV2Component_ = false;
    bool needSlide_ = false;
    bool isNeedRepaint_ = false;
    Axis axis_ = Axis::HORIZONTAL;
    int32_t startDayOfWeek_ = 6;
    CalendarType type_ { CalendarType::NORMAL };
    RefPtr<CalendarController> calendarController_;
    RefPtr<CalendarDataAdapter> dataAdapter_;

private:
    void OnDateSelected(const CalendarDay& date);
    void UpdateAccessibility();

    std::function<void(const std::string&)> selectedChangeEvent_;
    RefPtr<ClickRecognizer> clickDetector_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_RENDER_CALENDAR_H
