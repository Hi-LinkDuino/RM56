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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_COMPONENT_H

#include "core/components/button/button_component.h"
#include "core/components/calendar/calendar_data_adapter.h"
#include "core/components/calendar/calendar_theme.h"
#include "core/components/flex/flex_component.h"
#include "core/components/image/image_component.h"
#include "core/components/swiper/render_swiper.h"
#include "core/components/swiper/swiper_component.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

using CalenderRequestFocusImpl = std::function<void()>;

class ACE_EXPORT CalendarController : public AceType {
    DECLARE_ACE_TYPE(CalendarController, AceType);

public:
    CalendarController(
        const CalendarDataAdapterAction& dataAdapterAction, const WeakPtr<PipelineContext>& pipelineContext);
    ~CalendarController() override = default;

    void Initialize();
    void GoToPrevMonth(int32_t selected);
    void GoToNextMonth(int32_t selected);
    void GoTo(int32_t year, int32_t month, int32_t day);

    void SetSwiperController(const RefPtr<SwiperController>& swiperController)
    {
        swiperController_ = swiperController;
    }

    const RefPtr<CalendarDataAdapter>& GetDataAdapter() const
    {
        return dataAdapter_;
    }

    void SetToday(const CalendarDay& today)
    {
        dataAdapter_->SetToday(today);
    }

    const CalendarDay& GetToday()
    {
        return dataAdapter_->GetToday();
    }

    void RequestFocus()
    {
        if (requestFocusImpl_) {
            requestFocusImpl_();
        }
    }

    void SetRequestFocusImpl(const CalenderRequestFocusImpl& requestFocusImpl)
    {
        requestFocusImpl_ = requestFocusImpl;
    }

    void SetFirstEnter(bool firstEnter)
    {
        firstEnter_ = firstEnter;
    }

    bool GetFirstEnter() const
    {
        return firstEnter_;
    }

    void SetNeedFocus(bool needFocus)
    {
        needFocus_ = needFocus;
    }

    bool IsNeedFocus() const
    {
        return needFocus_;
    }

    void NotifyFocusChanged(bool focusStatus) const
    {
        if (dataAdapter_) {
            dataAdapter_->NotifyFocusChanged(focusStatus);
        }
    }

    void SetRenderSwiper(const RefPtr<RenderSwiper>& renderSwiper)
    {
        renderSwiper_ = renderSwiper;
    }

    const RefPtr<RenderSwiper>& GetRenderSwiper() const
    {
        return renderSwiper_;
    }

    int32_t GetCurrentIndex()
    {
        return currentMonthIndex_;
    }

    void JumpMonth()
    {
        while (!swiperReverseCache_.empty()) {
            swiperController_->SwipeTo(swiperReverseCache_.front().first, swiperReverseCache_.front().second);
            swiperReverseCache_.pop();
        }
    }

    void SetFirstLoad(bool firstLoad)
    {
        firstLoad_ = firstLoad;
    }

    bool GetFirstLoad() const
    {
        return firstLoad_;
    }

    void RequestMonthData(int32_t index);
    void CalculateNextIndex(int32_t index);
    void UpdateTheme();
    void UpdateTitle(const CalendarDay& today);

    const CalendarMonth& GetCurrentMonth() const
    {
        return currentCalendarMonth_;
    }

    void SetCardTitle(const RefPtr<RenderText>& renderText)
    {
        renderText_ = renderText;
    }

    void SetRightRowImage(const RefPtr<RenderImage>& rightRowImage)
    {
        rightImage_ = rightRowImage;
    }

    void SetLeftRowImage(const RefPtr<RenderImage>& leftRowImage)
    {
        leftImage_ = leftRowImage;
    }

    void SetRightRowImage(const RefPtr<ImageComponent>& rightRowImage)
    {
        rightImageComponent_ = rightRowImage;
    }

    void SetLeftRowImage(const RefPtr<ImageComponent>& leftRowImage)
    {
        leftImageComponent_ = leftRowImage;
    }

    void SetHasMoved(bool hasMoved)
    {
        dataAdapter_->SetHasMoved(hasMoved);
    }

    bool HasMoved()
    {
        return dataAdapter_->HasMoved();
    }

    void SetFirstSetToday(bool firstSetToday)
    {
        firstSetToday_ = firstSetToday;
    }

    bool FirstSetToday()
    {
        return firstSetToday_;
    }

    void SetCrossMonthDay(const CalendarDay& crossMonthDay)
    {
        dataAdapter_->SetCrossMonth(true);
        crossMonthDay_ = crossMonthDay;
    }

    const CalendarDay& GetCrossMonthDay()
    {
        return crossMonthDay_;
    }

    void SetCrossMonth(bool isCrossMonth)
    {
        dataAdapter_->SetCrossMonth(isCrossMonth);
    }

    bool IsCrossMonth()
    {
        return dataAdapter_->IsCrossMonth();
    }

private:
    enum class SwipeDirection {
        NEXT,
        PREV,
    };

    void GoToMonth(SwipeDirection direction, int32_t selected);
    void JumpToMonth(const CalendarMonth& calendarMonth, int32_t selected, SwipeDirection direction);
    void SetButtonClickColor(const RefPtr<RenderNode>& node, const Color& clickColor) const;

    RefPtr<SwiperController> swiperController_;
    RefPtr<RenderSwiper> renderSwiper_;
    RefPtr<CalendarDataAdapter> dataAdapter_;
    CalendarMonth currentCalendarMonth_;
    CalendarDay crossMonthDay_;
    CalenderRequestFocusImpl requestFocusImpl_;
    int32_t currentMonthIndex_ = 1;
    bool firstEnter_ = true;
    bool needFocus_ = false;
    bool firstLoad_ = true;
    bool firstSetToday_ = true;
    std::queue<std::pair<int32_t, bool>> swiperReverseCache_;

    RefPtr<RenderText> renderText_;
    RefPtr<RenderImage> leftImage_;
    RefPtr<RenderImage> rightImage_;
    RefPtr<ImageComponent> leftImageComponent_;
    RefPtr<ImageComponent> rightImageComponent_;
};

class ACE_EXPORT CalendarComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(CalendarComponent, ComposedComponent);

public:
    CalendarComponent(const ComposeId& id, const std::string& name);
    ~CalendarComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<Component> Build(
        const WeakPtr<PipelineContext>& pipelineContext, const RefPtr<CalendarController>& calendarController);
    void GoTo(int32_t year, int32_t month, int32_t day);

    void SetSelectedChangeEvent(const EventMarker& selectedChangeEvent)
    {
        selectedChangeEvent_ = selectedChangeEvent;
    }

    void SetRequestDataEvent(const EventMarker& requestDataEvent)
    {
        requestDataEvent_ = requestDataEvent;
    }

    void SetDataAdapterAction(const CalendarDataAdapterAction& dataAdapterAction)
    {
        dataAdapterAction_ = dataAdapterAction;
    }

    const CalendarDataAdapterAction& GetDataAdapterAction()
    {
        return dataAdapterAction_;
    }

    void SetCalendarDate(const CalendarDay& date)
    {
        date_ = date;
        isSetToday_ = true;
    }

    const RefPtr<CalendarController>& GetCalendarController() const
    {
        return calendarController_;
    }

    void RequestMonthData(int32_t index);

    void SetAxis(Axis axis)
    {
        axis_ = axis;
    }

    Axis GetAxis() const
    {
        return axis_;
    }

    void SetShowLunar(bool showLunar)
    {
        showLunar_ = showLunar;
    }

    const EventMarker& GetPreClickId() const
    {
        return preClickId_;
    }

    const EventMarker& GetNextClickId() const
    {
        return nextClickId_;
    }

    const EventMarker& GetDateClickId() const
    {
        return dateClickId_;
    }

    void SetCardCalendar(bool cardCalendar)
    {
        cardCalendar_ = cardCalendar;
    }

    bool IsCardCalendar() const
    {
        return cardCalendar_;
    }

    void SetStartDayOfWeek(int32_t indexOfWeek)
    {
        startDayOfWeek_ = indexOfWeek;
    }

    int32_t GetStartDayOfWeek() const
    {
        return startDayOfWeek_;
    }

    bool IsShowLunar() const
    {
        return showLunar_;
    }

    bool IsSetToday() const
    {
        return isSetToday_;
    }

    const CalendarDay& GetDate() const
    {
        return date_;
    }

    const std::string& GetCalendarData() const
    {
        return calendarData_;
    }

    void SetShowHoliday(bool showHoliday)
    {
        showHoliday_ = showHoliday;
    }

    bool GetShowHoliday() const
    {
        return showHoliday_;
    }

    void SetOffDays(const std::string& offDays)
    {
        offDays_ = offDays;
    }

    const std::string& GetOffDays() const
    {
        return offDays_;
    }

    const std::string& GetHolidays() const
    {
        return holidays_;
    }

    void SetHolidays(const std::string& holidays)
    {
        holidays_ = holidays;
    }

    const std::string& GetWorkDays() const
    {
        return workDays_;
    }

    void SetWorkDays(const std::string& workDays)
    {
        workDays_ = workDays;
    }

    void SetNeedSlide(bool needSlide)
    {
        needSlide_ = needSlide;
    }

    bool IsNeedSlide() const
    {
        return needSlide_;
    }

    bool IsV2Componenet() const
    {
        return isV2Component_;
    }

    void SetV2Componenet(bool isV2Component)
    {
        isV2Component_ = isV2Component;
    }

    void SetCalendarTheme(const RefPtr<CalendarTheme>& theme)
    {
        theme_ = theme;
    }

    const RefPtr<CalendarTheme>& GetCalendarTheme()
    {
        return theme_;
    }

    void SetCalendarType(CalendarType type)
    {
        type_ = type;
    }

    CalendarType GetCalendarType() const
    {
        return type_;
    }

    void SetCalendarData(const std::string& value);
    void BuildCardCalendarTitle(RefPtr<ColumnComponent>& col);
    const EventMarker& GetSelectedChangeEvent() const;
    const EventMarker& GetRequestDataEvent() const;
    RefPtr<ButtonComponent> InitCardButton(bool isPreArrow);

protected:
    RefPtr<CalendarTheme> theme_;

private:
    Axis axis_ { Axis::HORIZONTAL };
    CalendarType type_ { CalendarType::NORMAL };
    int32_t startDayOfWeek_ = 6; // the default is Sunday
    bool showHoliday_ = true;
    bool showLunar_ = true;
    bool isSetToday_ = false;
    bool cardCalendar_ = false;
    bool needSlide_ = false;
    bool isV2Component_ = false;
    std::string calendarData_;
    std::string offDays_ = "5, 6"; // The default day of rest is Saturday and Sunday
    std::string holidays_;
    std::string workDays_;
    ColorMode colorMode_ = ColorMode::LIGHT;
    CalendarDay date_;
    EventMarker selectedChangeEvent_;
    EventMarker requestDataEvent_;
    EventMarker preClickId_;
    EventMarker nextClickId_;
    EventMarker dateClickId_;
    CalendarDataAdapterAction dataAdapterAction_;
    RefPtr<CalendarController> calendarController_;
    RefPtr<SwiperComponent> swiperContainer_;
};

class CalendarMonthComponent : public RenderComponent {
    DECLARE_ACE_TYPE(CalendarMonthComponent, RenderComponent);

public:
    CalendarMonthComponent(int32_t indexOfContainer, const RefPtr<CalendarController>& calendarController);
    ~CalendarMonthComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    int32_t GetIndexOfContainer() const
    {
        return indexOfContainer_;
    }

    void SetSelectedChangeEvent(const EventMarker& selectedChangeEvent)
    {
        selectedChangeEvent_ = selectedChangeEvent;
    }

    const EventMarker& GetSelectedChangeEvent() const
    {
        return selectedChangeEvent_;
    }

    const RefPtr<CalendarController>& GetCalendarController() const
    {
        return calendarController_;
    }

    void SetCalendarTheme(const RefPtr<CalendarTheme>& calendarTheme)
    {
        calendarTheme_ = calendarTheme;
    }

    const RefPtr<CalendarTheme>& GetCalendarTheme() const
    {
        return calendarTheme_;
    }

    void SetCardCalendar(bool cardCalendar)
    {
        cardCalendar_ = cardCalendar;
    }

    bool IsCardCalendar() const
    {
        return cardCalendar_;
    }

    void SetCalendarType(CalendarType type)
    {
        type_ = type;
    }

    CalendarType GetCalendarType() const
    {
        return type_;
    }

private:
    bool cardCalendar_ = false;
    int32_t indexOfContainer_ = 0;
    EventMarker selectedChangeEvent_;
    RefPtr<CalendarController> calendarController_;
    RefPtr<CalendarTheme> calendarTheme_;
    CalendarType type_ { CalendarType::NORMAL };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_COMPONENT_H
