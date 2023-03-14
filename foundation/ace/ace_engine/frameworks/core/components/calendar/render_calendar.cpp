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

#include "core/components/calendar/render_calendar.h"

#include "base/i18n/localization.h"
#include "base/log/event_report.h"
#include "base/utils/system_properties.h"
#include "core/common/ace_application_info.h"
#include "core/common/font_manager.h"
#include "core/components/calendar/calendar_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t CALENDAR_MIN_WIDTH = 350;
constexpr int32_t CALENDAR_MIN_HEIGHT = 230;
constexpr int32_t DAYS_PER_WEEK = 7;

} // namespace

RenderCalendar::RenderCalendar()
{
    weekNumbers_ = Localization::GetInstance()->GetWeekdays(true);
}

void RenderCalendar::Update(const RefPtr<Component>& component)
{
    auto calendarMonth = AceType::DynamicCast<CalendarMonthComponent>(component);
    if (!calendarMonth) {
        LOGE("calendar month component is null");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }

    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    selectedChangeEvent_ =
        AceAsyncEvent<void(const std::string&)>::Create(calendarMonth->GetSelectedChangeEvent(), context_);
    indexOfContainer_ = calendarMonth->GetIndexOfContainer();
    calendarController_ = calendarMonth->GetCalendarController();
    dataAdapter_ = calendarController_->GetDataAdapter();
    colCount_ = static_cast<int32_t>(weekNumbers_.size());
    dataAdapter_->RegisterDataListener(AceType::Claim(this));
    type_ = calendarMonth->GetCalendarType();
    if (type_ != CalendarType::SIMPLE) {
        clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickDetector_->SetOnClick([weak = WeakClaim(this)](const ClickInfo& info) {
            auto calendar = weak.Upgrade();
            if (calendar) {
                calendar->HandleClick(info.GetLocalLocation());
            }
        });
    }
    OnDataChanged(dataAdapter_->GetDayOfMonthCache()[indexOfContainer_]);

    auto fontManager = context->GetFontManager();
    if (fontManager) {
        fontManager->AddVariationNode(WeakClaim(this));
    }
    MarkNeedLayout();
}

void RenderCalendar::OnPaintFinish()
{
    UpdateAccessibility();
}

void RenderCalendar::UpdateAccessibility()
{
    auto node = GetAccessibilityNode().Upgrade();
    if (!node) {
        return;
    }
    if (!calendarController_) {
        return;
    }
    auto today = calendarController_->GetDataAdapter()->GetToday();
    DateTime dateTime;
    dateTime.year = static_cast<uint32_t>(today.month.year);
    dateTime.month = static_cast<uint32_t>(today.month.month);
    dateTime.day = static_cast<uint32_t>(today.day);
    auto dateText = Localization::GetInstance()->FormatDateTime(dateTime, "yyyyMMdd");
    node->SetText(dateText);
}

void RenderCalendar::PerformLayout()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("calendar perform layout error");
        return;
    }
    Size calendarMinSize(CALENDAR_MIN_WIDTH, CALENDAR_MIN_HEIGHT);
    LayoutParam innerLayout = GetLayoutParam();
    Size maxSize = innerLayout.GetMaxSize();
    Size layoutSize;
    if (maxSize.IsInfinite()) {
        Size minSize = innerLayout.GetMinSize();
        layoutSize = Size(
            std::max(minSize.Width(), calendarMinSize.Width()), std::max(minSize.Height(), calendarMinSize.Height()));
    } else {
        layoutSize = Size(
            std::max(maxSize.Width(), calendarMinSize.Width()), std::max(maxSize.Height(), calendarMinSize.Height()));
    }
    if (SystemProperties::GetDeviceType() == DeviceType::TV && !cardCalendar_) {
        SetLayoutSize(layoutSize);
    } else if (maxSize.IsInfinite()) {
        SetLayoutSize({ context->GetRootWidth(), context->GetRootHeight() });
        maxWidth_ = context->GetRootWidth();
        maxHeight_ = context->GetRootHeight();
    } else {
        SetLayoutSize(maxSize);
        auto constrainSize = innerLayout.Constrain(maxSize);
        maxWidth_ = constrainSize.Width();
        maxHeight_ = constrainSize.Height();
    }
}

void RenderCalendar::OnDataChanged(const CalendarDaysOfMonth& daysOfMonth)
{
    firstDayIndex_ = daysOfMonth.firstDayIndex;
    if (currentMonth_ == daysOfMonth.month) {
        if (SystemProperties::GetDeviceType() == DeviceType::TV && IsValid(focusIndex_) &&
            calendarDays_[focusIndex_].focused) {
            calendarDays_ = daysOfMonth.days;
            calendarDays_[focusIndex_].focused = true;
        } else if (isV2Component_ && IsValid(touchIndex_) && calendarDays_[touchIndex_].touched) {
            calendarDays_ = daysOfMonth.days;
            calendarDays_[touchIndex_].touched = true;
        } else {
            calendarDays_ = daysOfMonth.days;
        }
        // the number of rows will be 5 or 6, and week number height is half of the date number
        rowCount_ = colCount_ ? static_cast<int32_t>(daysOfMonth.days.size()) / colCount_ : 0;
        UpdateBreakInformation();
        isNeedRepaint_ = true;
        MarkNeedLayout();
        return;
    }
    calendarDays_ = daysOfMonth.days;
    currentMonth_ = daysOfMonth.month;
    lastDayIndex_ = daysOfMonth.lastDayIndex;
    if ((!calendarController_->FirstSetToday() || currentMonth_ != calendarController_->GetCurrentMonth()) &&
        IsValid(firstDayIndex_)) {
        calendarDays_[firstDayIndex_].touched = true;
        touchIndex_ = firstDayIndex_;
    }
    UpdateBreakInformation();
    if (GetCalendarController()->GetFirstEnter()) {
        selectedDayNumber_ = daysOfMonth.today - firstDayIndex_ + 1;
        GetCalendarController()->SetFirstEnter(false);
        OnStatusChanged(RenderStatus::FOCUS);
    }
    // the number of rows will be 5 or 6, and week number height is half of the date number
    rowCount_ = colCount_ ? static_cast<int32_t>(daysOfMonth.days.size()) / colCount_ : 0;
    calendarController_->JumpMonth();
    hasRequestFocus_ = false;
    cardCalendar_ ? MarkNeedLayout() : MarkNeedRender();
    isNeedRepaint_ = true;
}

void RenderCalendar::OnSelectedDay(int32_t selected)
{
    if (isV2Component_ && calendarController_->IsCrossMonth() &&
        calendarController_->GetCrossMonthDay().month == currentMonth_ && IsValid(touchIndex_)) {
        calendarDays_[touchIndex_].touched = false;
        if (IsToday(calendarDays_[touchIndex_])) {
            isNeedRepaint_ = true;
        }
        touchIndex_ = selected + firstDayIndex_ - 1;
        calendarDays_[touchIndex_].touched = true;
        if (IsToday(calendarDays_[touchIndex_])) {
            isNeedRepaint_ = true;
        }
        MarkNeedRender();
        return;
    } else if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        selectedDayNumber_ = selected;
        CalendarDay day;
        if (selected < 0) {
            day.index = lastDayIndex_;
        } else {
            day.index = selectedDayNumber_ + firstDayIndex_ - 1;
        }
        if (calendarController_->GetFirstLoad()) {
            calendarController_->SetFirstLoad(false);
        } else {
            OnDateSelected(day);
        }

        if (calendarController_->IsNeedFocus()) {
            calendarController_->RequestFocus();
            calendarController_->SetNeedFocus(false);
        }
        hasRequestFocus_ = true;
        MarkNeedRender();
    }
}

void RenderCalendar::OnStatusChanged(RenderStatus renderStatus)
{
    int32_t calendarDaysSize = static_cast<int32_t>(calendarDays_.size());
    if (renderStatus == RenderStatus::FOCUS) {
        int32_t focusedIndex = selectedDayNumber_ + firstDayIndex_ - 1;
        if (selectedDayNumber_ < 0) {
            focusedIndex = lastDayIndex_;
        }
        focusRow_ = focusedIndex / DAYS_PER_WEEK;
        focusCol_ = focusedIndex % DAYS_PER_WEEK;
        selectedDayNumber_ = focusedIndex - firstDayIndex_ + 1;
        focusIndex_ = focusedIndex;
        if (focusIndex_ >= 0 && focusIndex_ < calendarDaysSize) {
            calendarDays_[focusedIndex].focused = true;
            if (SystemProperties::GetDeviceType() == DeviceType::TV) {
                OnDateSelected(calendarDays_[focusedIndex]);
            }
        }
    } else if (renderStatus == RenderStatus::BLUR) {
        focusRow_ = -1;
        focusCol_ = -1;
        if (focusIndex_ >= 0 && focusIndex_ < calendarDaysSize) {
            calendarDays_[focusIndex_].focused = false;
        }
    }
    MarkNeedRender();
}

int32_t RenderCalendar::GetIndexByGrid(int32_t row, int32_t column)
{
    return (row * colCount_) + column;
}

void RenderCalendar::OnDateSelected(const CalendarDay& date)
{
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        auto calendarCache = calendarController_->GetDataAdapter()->GetCalendarCache()[indexOfContainer_];
        if (selectedChangeEvent_ && date.index >= 0 && date.index < static_cast<int32_t>(calendarCache.size())) {
            if (SystemProperties::GetDeviceType() == DeviceType::WATCH &&
                calendarDays_[date.index].month != calendarController_->GetCurrentMonth()) {
                return;
            }
            std::string result = std::string("\"selectedchange\",").append(calendarCache[date.index]).append(",null");
            selectedChangeEvent_(result);
        }
    } else if (date.index >= 0 && date.index < static_cast<int32_t>(calendarDays_.size())) {
        auto result = JsonUtil::ParseJsonString(calendarDays_[date.index].ToString());
        if (selectedChangeEvent_) {
            selectedChangeEvent_(result->ToString());
        }
    }
}

void RenderCalendar::FocusChanged(int32_t oldIndex, int32_t newIndex)
{
    int32_t calendarDaysSize = static_cast<int32_t>(calendarDays_.size());
    if (oldIndex < 0 || oldIndex >= calendarDaysSize) {
        LOGW("lost focus index is out of calendar days array");
        return;
    }
    calendarDays_[oldIndex].focused = false;

    auto pipelineContext = GetContext().Upgrade();
    if (!pipelineContext) {
        LOGE("pipeline context is null");
        return;
    }
    pipelineContext->CancelFocusAnimation();
    if (newIndex < 0) {
        calendarController_->GoToPrevMonth(-1);
        return;
    }
    if (newIndex >= calendarDaysSize) {
        calendarController_->GoToNextMonth(1);
        return;
    }

    auto& onFocusDay = calendarDays_[newIndex];
    if (onFocusDay.month < currentMonth_) {
        LOGD("focus move to last month");
        calendarController_->GoToPrevMonth(onFocusDay.day);
        return;
    } else if (calendarDays_[newIndex].month > currentMonth_) {
        LOGD("focus move to next month");
        calendarController_->GoToNextMonth(onFocusDay.day);
        return;
    }

    onFocusDay.focused = true;
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        OnDateSelected(onFocusDay);
    }
}

void RenderCalendar::OnFocusChanged(bool focusStatus)
{
    calendarFocusStatus_ = focusStatus;
}

void RenderCalendar::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (type_ == CalendarType::SIMPLE) {
        return;
    }
    clickDetector_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickDetector_);
}

void RenderCalendar::HandleClick(const Offset& offset)
{
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        return;
    }
    auto swiper = calendarController_->GetRenderSwiper();
    if (swiper && swiper->GetMoveStatus()) {
        return;
    }
    auto index = JudgeArea(offset);
    if (index < 0 || index >= static_cast<int32_t>(calendarDays_.size())) {
        return;
    }
    CalendarDay day;
    day.index = index;
    OnDateSelected(day);

    if (!isV2Component_) {
        return;
    }

    if (calendarDays_[index].month > calendarController_->GetCurrentMonth()) {
        calendarController_->SetCrossMonthDay(calendarDays_[index]);
        calendarController_->SetHasMoved(true);
        calendarController_->GoToNextMonth(calendarDays_[index].day);
    } else if (calendarDays_[index].month < calendarController_->GetCurrentMonth()) {
        calendarController_->SetCrossMonthDay(calendarDays_[index]);
        calendarController_->SetHasMoved(true);
        calendarController_->GoToPrevMonth(calendarDays_[index].day);
    } else {
        if (IsValid(touchIndex_)) {
            calendarDays_[touchIndex_].touched = false;
        }
        if (IsToday(calendarDays_[touchIndex_]) || IsToday(calendarDays_[index])) {
            isNeedRepaint_ = true;
        }
        touchIndex_ = index;
        calendarDays_[touchIndex_].touched = true;
        MarkNeedRender();
        return;
    }
}

int32_t RenderCalendar::JudgeArea(const Offset& offset)
{
    const static int32_t rowsOfData = 5;
    auto rowSpace = rowCount_ == rowsOfData ? dailyFiveRowSpace_ : dailySixRowSpace_;
    auto topPadding = NormalizeToPx(calendarTheme_.topPadding);
    auto browHeight = weekHeight_ + topPadding + NormalizeToPx(calendarTheme_.weekAndDayRowSpace);
    auto maxHeight = browHeight + rowCount_ * dayHeight_ + (rowCount_ - 1) * rowSpace +
                     NormalizeToPx(calendarTheme_.weekAndDayRowSpace);
    auto maxWidth = dayWidth_ * DAYS_PER_WEEK + colSpace_ * 6;
    if ((offset.GetX() < 0) || (offset.GetX() > maxWidth) || (offset.GetY() < browHeight) ||
        (offset.GetY() > maxHeight) || LessOrEqual(dayHeight_, 0.0) || LessOrEqual(dayWidth_, 0.0)) {
        return -1;
    }
    auto boundaryColOffset = cardCalendar_ ? 0.0 : NormalizeToPx(calendarTheme_.boundaryColOffset);
    auto height = offset.GetY() - browHeight - boundaryColOffset;
    auto boundaryRowOffset = NormalizeToPx(calendarTheme_.boundaryRowOffset);
    int32_t y =
        height < (dayHeight_ + rowSpace / 2) ? 0 : (height - dayHeight_ - rowSpace / 2) / (dayHeight_ + rowSpace) + 1;
    int32_t x = offset.GetX() < (dayWidth_ + colSpace_ / 2)
                ? 0
                : (offset.GetX() - dayWidth_ - colSpace_ / 2 - boundaryRowOffset) / (dayWidth_ + colSpace_) + 1;
    if (AceApplicationInfo::GetInstance().IsRightToLeft()) {
        x = DAYS_PER_WEEK - x - 1;
    }
    return (y * colCount_ + x);
}

void RenderCalendar::UpdateCardCalendarAttr(CardCalendarAttr&& attr)
{
    auto context = GetContext().Upgrade();
    if (!context) {
        return;
    }
    textDirection_ = attr.textDirection;
    showHoliday_ = attr.showHoliday;
    needSlide_ = attr.needSlide;
    offDays_ = attr.offDays;
    axis_ = attr.axis;
    isV2Component_ = attr.isV2Component;
    if (attr.calendarTheme) {
        calendarTheme_ =
              context->IsJsCard() ? attr.calendarTheme->GetCardCalendarTheme() : attr.calendarTheme->GetCalendarTheme();
    }

    type_ = attr.type;
    UpdateBreakInformation();
    MarkNeedLayout();
}

void RenderCalendar::UpdateBreakInformation()
{
    if ((SystemProperties::GetDeviceType() != DeviceType::WATCH && type_ != CalendarType::SIMPLE) ||
        calendarController_->GetCurrentIndex() != indexOfContainer_ || firstDayIndex_ < 0) {
        return;
    }
    std::vector<int32_t> holidays;
    std::vector<int32_t> workDays;
    auto holidaysValue = dataAdapter_->GetHolidays();
    auto workDayValue = dataAdapter_->GetWorkDays();
    StringUtils::StringSpliter(holidaysValue, ',', holidays);
    StringUtils::StringSpliter(workDayValue, ',', workDays);
    for (auto& day : calendarDays_) {
        day.dayMark = "";
        day.dayMarkValue = "";
    }
    for (auto holiday : holidays) {
        auto index = holiday + firstDayIndex_ - 1;
        if (index >= 0 && index < (int32_t)calendarDays_.size()) {
            calendarDays_[index].dayMark = "off";
            calendarDays_[index].dayMarkValue = "休";
        }
    }

    for (auto workDay : workDays) {
        auto index = workDay + firstDayIndex_ - 1;
        if (index >= 0 && index < (int32_t)calendarDays_.size()) {
            calendarDays_[index].dayMark = "work";
            calendarDays_[index].dayMarkValue = "班";
        }
    }
}

void RenderCalendar::OnSwiperMove()
{
    if (calendarController_->IsCrossMonth() && calendarController_->GetCrossMonthDay().month == currentMonth_) {
        calendarController_->SetCrossMonth(false);
        return;
    }

    if (calendarController_->FirstSetToday() && currentMonth_ == calendarController_->GetCurrentMonth()) {
        if (IsValid(touchIndex_)) {
            calendarDays_[touchIndex_].touched = false;
            isNeedRepaint_ = true;
            MarkNeedRender();
        }
        return;
    }

    if (IsValid(touchIndex_) && touchIndex_ != firstDayIndex_) {
        if (IsToday(calendarDays_[touchIndex_])) {
            isNeedRepaint_ = true;
        }
        calendarDays_[touchIndex_].touched = false;
        touchIndex_ = firstDayIndex_;
        calendarDays_[touchIndex_].touched = true;
        MarkNeedRender();
    }
}

bool RenderCalendar::IsValid(int32_t index) const
{
    return index >= 0 && index < static_cast<int32_t>(calendarDays_.size());
}

bool RenderCalendar::IsToday(const CalendarDay& day) const
{
    auto today = dataAdapter_->GetToday();
    return today.month == day.month && today.day == day.day;
}

} // namespace OHOS::Ace
