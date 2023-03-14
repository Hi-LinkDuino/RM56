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

#include "core/components/calendar/calendar_component.h"

#include "base/i18n/localization.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/button/render_button.h"
#include "core/components/calendar/calendar_element.h"
#include "core/components/calendar/render_calendar.h"
#include "core/components/display/display_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/image/image_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/text/text_component.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"

namespace OHOS::Ace {
namespace {

constexpr double MAX_OPACITY = 255.0;
constexpr int32_t MAX_MONTH_CACHE_NUM = 3;
constexpr int32_t DISTANCE_FORM_LAST = MAX_MONTH_CACHE_NUM - 1;
constexpr int32_t TODAY_MONTH_INDEX_OF_CONTAINER = 1;
constexpr int32_t NEXT_TO_TODAY_MONTH_INDEX_OF_CONTAINER = 2;
constexpr int32_t LAST_TO_TODAY_MONTH_INDEX_OF_CONTAINER = 0;
constexpr Dimension CALENDAR_BOX_TOP_AND_BOTTOM_MARGIN = 4.0_vp;
constexpr Dimension CALENDAR_BOX_LEFT_AND_RIGHT_MARGIN = 24.0_vp;
constexpr Dimension CALENDAR_BUTTON_RADIUS = 4.0_vp;

} // namespace

CalendarController::CalendarController(
    const CalendarDataAdapterAction& dataAdapterAction, const WeakPtr<PipelineContext>& pipelineContext)
    : dataAdapter_(AceType::MakeRefPtr<CalendarDataAdapter>(dataAdapterAction, pipelineContext)) {}

void CalendarController::Initialize()
{
    currentCalendarMonth_ = dataAdapter_->GetToday().month;
    dataAdapter_->SetCurrentMonth(currentCalendarMonth_);
    dataAdapter_->SetSelectedChanged(dataAdapter_->GetToday().day, TODAY_MONTH_INDEX_OF_CONTAINER);
    CalendarDataRequest requestMonth(currentCalendarMonth_, TODAY_MONTH_INDEX_OF_CONTAINER);
    requestMonth.state = MonthState::CUR_MONTH;
    dataAdapter_->RequestData(requestMonth);
    requestMonth.month = CalendarMonth::GetNextMonth(currentCalendarMonth_);
    requestMonth.indexOfContainer = NEXT_TO_TODAY_MONTH_INDEX_OF_CONTAINER;
    requestMonth.state = MonthState::NEXT_MONTH;
    dataAdapter_->AddPendingRequest(requestMonth);
    requestMonth.month = CalendarMonth::GetLastMonth(currentCalendarMonth_);
    requestMonth.indexOfContainer = LAST_TO_TODAY_MONTH_INDEX_OF_CONTAINER;
    requestMonth.state = MonthState::PRE_MONTH;
    dataAdapter_->AddPendingRequest(requestMonth);
}

void CalendarController::GoToPrevMonth(int32_t selected)
{
    GoToMonth(SwipeDirection::PREV, selected);
}

void CalendarController::GoToNextMonth(int32_t selected)
{
    GoToMonth(SwipeDirection::NEXT, selected);
}

void CalendarController::GoToMonth(SwipeDirection direction, int32_t selected)
{
    int32_t index;
    CalendarMonth calendarMonth;
    CalendarMonth cacheMonth;
    bool reverse = false;
    if (direction == SwipeDirection::PREV) {
        index = (currentMonthIndex_ + 1) % MAX_MONTH_CACHE_NUM;
        if (--currentMonthIndex_ < 0) {
            reverse = true;
            currentMonthIndex_ = MAX_MONTH_CACHE_NUM - 1;
        }
        calendarMonth = CalendarMonth::GetLastMonth(currentCalendarMonth_);
        cacheMonth = CalendarMonth::GetLastMonth(calendarMonth);
    } else if (direction == SwipeDirection::NEXT) {
        index = (currentMonthIndex_ + DISTANCE_FORM_LAST) % MAX_MONTH_CACHE_NUM;
        if (++currentMonthIndex_ >= MAX_MONTH_CACHE_NUM) {
            reverse = true;
            currentMonthIndex_ = 0;
        }
        calendarMonth = CalendarMonth::GetNextMonth(currentCalendarMonth_);
        cacheMonth = CalendarMonth::GetNextMonth(calendarMonth);
    } else {
        return;
    }

    swiperReverseCache_.push({ currentMonthIndex_, reverse });
    dataAdapter_->SetSelectedChanged(selected, currentMonthIndex_);
    dataAdapter_->NotifySelectedChanged();
    currentCalendarMonth_ = calendarMonth;
    dataAdapter_->SetCurrentMonth(currentCalendarMonth_);
    dataAdapter_->RequestData({ cacheMonth, index });
}

void CalendarController::JumpToMonth(const CalendarMonth& calendarMonth, int32_t selected, SwipeDirection direction)
{
    if (direction == SwipeDirection::PREV) {
        // target -- last(replaced by target) -- current -- next
        int32_t destIndex = (currentMonthIndex_ + DISTANCE_FORM_LAST) % MAX_MONTH_CACHE_NUM;
        dataAdapter_->RequestData({ calendarMonth, destIndex });
        CalculateNextIndex(currentMonthIndex_ - 1);
        dataAdapter_->SetSelectedChanged(selected, currentMonthIndex_);

        int32_t lastIndex = (destIndex + DISTANCE_FORM_LAST) % MAX_MONTH_CACHE_NUM;
        dataAdapter_->AddPendingRequest(CalendarMonth::GetLastMonth(calendarMonth), lastIndex);

        // cache the next month date after the animation ends, otherwise the content of the animation page will be
        // changed
        int32_t nextIndex = (destIndex + 1) % MAX_MONTH_CACHE_NUM;
        dataAdapter_->AddPendingRequest(CalendarMonth::GetNextMonth(calendarMonth), nextIndex);
    } else if (direction == SwipeDirection::NEXT) {
        // last -- current -- next(replaced by target) -- target
        int32_t destIndex = (currentMonthIndex_ + 1) % MAX_MONTH_CACHE_NUM;
        dataAdapter_->RequestData({ calendarMonth, destIndex });
        CalculateNextIndex(currentMonthIndex_ + 1);
        dataAdapter_->SetSelectedChanged(selected, currentMonthIndex_);

        int32_t nextIndex = (destIndex + 1) % MAX_MONTH_CACHE_NUM;
        dataAdapter_->AddPendingRequest(CalendarMonth::GetNextMonth(calendarMonth), nextIndex);

        // cache the previous month date after the animation ends, otherwise the content of the animation page will be
        // changed
        int32_t lastIndex = (destIndex + DISTANCE_FORM_LAST) % MAX_MONTH_CACHE_NUM;
        dataAdapter_->AddPendingRequest(CalendarMonth::GetLastMonth(calendarMonth), lastIndex);
    }
}

void CalendarController::GoTo(int32_t year, int32_t month, int32_t day)
{
    if (!dataAdapter_) {
        LOGE("calendar data adapter is nullptr");
        return;
    }
    if (!swiperController_) {
        LOGE("swiper controller is nullptr");
        return;
    }

    LOGD("go to: year=%{private}d, month=%{private}d, day=%{private}d", year, month, day);
    CalendarMonth calendarMonth { year, month };
    auto nextMonth = CalendarMonth::GetNextMonth(currentCalendarMonth_);
    auto lastMonth = CalendarMonth::GetLastMonth(currentCalendarMonth_);
    if (calendarMonth == lastMonth) {
        GoToPrevMonth(day);
    } else if (calendarMonth == nextMonth) {
        GoToNextMonth(day);
    } else if (calendarMonth < lastMonth) {
        currentCalendarMonth_ = calendarMonth;
        dataAdapter_->SetCurrentMonth(currentCalendarMonth_);
        JumpToMonth(calendarMonth, day, SwipeDirection::PREV);
    } else if (calendarMonth > nextMonth) {
        currentCalendarMonth_ = calendarMonth;
        dataAdapter_->SetCurrentMonth(currentCalendarMonth_);
        JumpToMonth(calendarMonth, day, SwipeDirection::NEXT);
    } else {
        dataAdapter_->SetSelectedChanged(day, currentMonthIndex_);
        dataAdapter_->NotifySelectedChanged();
    }
}

void CalendarController::RequestMonthData(int32_t index)
{
    auto tmpPreIndex = (currentMonthIndex_ + 1) % MAX_MONTH_CACHE_NUM;
    auto tmpNextIndex = (currentMonthIndex_ + DISTANCE_FORM_LAST) % MAX_MONTH_CACHE_NUM;
    auto nextIndex = currentMonthIndex_;
    auto preIndex = currentMonthIndex_;
    CalendarMonth calendarMonth;
    CalendarMonth cacheMonth;
    static const int32_t selectedDay = 1;
    if (++nextIndex >= MAX_MONTH_CACHE_NUM) {
        nextIndex = 0;
    }
    if (--preIndex < 0) {
        preIndex = MAX_MONTH_CACHE_NUM - 1;
    }
    if (nextIndex == index) {
        currentMonthIndex_ = nextIndex;
        calendarMonth = CalendarMonth::GetNextMonth(currentCalendarMonth_);
        cacheMonth = CalendarMonth::GetNextMonth(calendarMonth);

        dataAdapter_->SetSelectedChanged(selectedDay, currentMonthIndex_);
        CalendarDataRequest requestMonth(cacheMonth, tmpNextIndex);
        requestMonth.state = MonthState::NEXT_MONTH;
        currentCalendarMonth_ = calendarMonth;
        dataAdapter_->SetCurrentMonth(currentCalendarMonth_);
        dataAdapter_->RequestData(requestMonth);
    } else if (preIndex == index) {
        currentMonthIndex_ = preIndex;
        calendarMonth = CalendarMonth::GetLastMonth(currentCalendarMonth_);
        cacheMonth = CalendarMonth::GetLastMonth(calendarMonth);

        dataAdapter_->SetSelectedChanged(selectedDay, currentMonthIndex_);
        CalendarDataRequest requestMonth(cacheMonth, tmpPreIndex);
        requestMonth.state = MonthState::PRE_MONTH;
        currentCalendarMonth_ = calendarMonth;
        dataAdapter_->SetCurrentMonth(currentCalendarMonth_);
        dataAdapter_->RequestData(requestMonth);
    } else {
        return;
    }
}

void CalendarController::CalculateNextIndex(int32_t index)
{
    bool reverse = false;
    if (index >= MAX_MONTH_CACHE_NUM) {
        reverse = true;
        index = 0;
    } else if (index < 0) {
        reverse = true;
        index = MAX_MONTH_CACHE_NUM - 1;
    }
    currentMonthIndex_ = index;
    swiperReverseCache_.push({ currentMonthIndex_, reverse });
}

void CalendarController::UpdateTheme()
{
    if (!renderText_) {
        return;
    }
    auto theme = renderText_->GetTheme<CalendarTheme>();
    if (!leftImage_ || !leftImageComponent_ || !rightImageComponent_ || !rightImage_ || !theme) {
        return;
    }
    TextStyle style;
    auto cardTheme = theme->GetCardCalendarTheme();
    style.SetFontSize(cardTheme.titleFontSize);
    style.SetTextColor(cardTheme.titleTextColor);
    style.SetFontWeight(FontWeight::W500);
    style.SetAllowScale(false);
    renderText_->SetTextStyle(style);
    renderText_->MarkNeedMeasure();
    renderText_->MarkNeedLayout();
    leftImageComponent_->SetImageFill(cardTheme.titleTextColor);
    leftImage_->Update(leftImageComponent_);
    leftImage_->MarkNeedLayout();
    rightImageComponent_->SetImageFill(cardTheme.titleTextColor);
    rightImage_->Update(rightImageComponent_);
    rightImage_->MarkNeedLayout();
    SetButtonClickColor(renderText_, cardTheme.clickEffectColor);
    SetButtonClickColor(leftImage_, cardTheme.clickEffectColor);
    SetButtonClickColor(rightImage_, cardTheme.clickEffectColor);
}

void CalendarController::SetButtonClickColor(const RefPtr<RenderNode>& node, const Color& clickColor) const
{
    auto button = AceType::DynamicCast<RenderButton>(node->GetParent().Upgrade());
    if (!button) {
        LOGE("this node is not button");
        return;
    }
    button->SetClickedColor(clickColor);
    button->MarkNeedLayout();
}

void CalendarController::UpdateTitle(const CalendarDay& today)
{
    if (!renderText_) {
        return;
    }
    DateTime dateTime;
    dateTime.year = today.month.year;
    dateTime.month = today.month.month;
    auto date = Localization::GetInstance()->FormatDateTime(dateTime, "yyyyMMM");
    renderText_->SetTextData(date);
    renderText_->MarkNeedMeasure();
    renderText_->MarkNeedLayout();
}

CalendarComponent::CalendarComponent(const ComposeId& id, const std::string& name) : ComposedComponent(id, name) {}

RefPtr<Component> CalendarComponent::Build(
    const WeakPtr<PipelineContext>& pipelineContext, const RefPtr<CalendarController>& calendarController)
{
    auto context = pipelineContext.Upgrade();
    if (!context || !calendarController) {
        return nullptr;
    }
    calendarController_ = calendarController;

    auto direction = GetTextDirection();

    if (!theme_) {
        auto themeManager = context->GetThemeManager();
        if (!themeManager) {
            return nullptr;
        }
        theme_ = themeManager->GetTheme<CalendarTheme>();
    }

    std::list<RefPtr<Component>> monthChildren;
    for (int32_t index = 0; index < MAX_MONTH_CACHE_NUM; index++) {
        auto calendarMonth = AceType::MakeRefPtr<CalendarMonthComponent>(index, calendarController_);
        auto display = AceType::MakeRefPtr<DisplayComponent>(calendarMonth);
        calendarMonth->SetSelectedChangeEvent(selectedChangeEvent_);
        calendarMonth->SetCalendarTheme(theme_);
        calendarMonth->SetCardCalendar(cardCalendar_);
        calendarMonth->SetTextDirection(direction);
        calendarMonth->SetCalendarType(type_);
        display->SetOpacity(MAX_OPACITY);
        monthChildren.emplace_back(display);
    }
    if (!swiperContainer_) {
        swiperContainer_ = AceType::MakeRefPtr<SwiperComponent>(monthChildren, false);
    }
    swiperContainer_->SetAxis(axis_);
    swiperContainer_->SetIndex(TODAY_MONTH_INDEX_OF_CONTAINER);
    swiperContainer_->SetTextDirection(direction);
    swiperContainer_->SetSlideContinue(true);
    calendarController_->SetSwiperController(swiperContainer_->GetSwiperController());
    swiperContainer_->SetMoveCallback([controller = WeakPtr<CalendarController>(calendarController_)](int32_t index) {
        auto calendarController = controller.Upgrade();
        if (calendarController) {
            calendarController->SetHasMoved(true);
            calendarController->RequestMonthData(index);
        }
    });
    if (type_ == CalendarType::SIMPLE) {
        swiperContainer_->SetDisabledStatus(true);
    }
    if (!needSlide_) {
        swiperContainer_->SetLoop(false);
        swiperContainer_->DisableSwipe(true);
        swiperContainer_->SetDisableRotation(true);
    }
    if (!cardCalendar_) {
        return swiperContainer_;
    } else {
        RefPtr<ColumnComponent> colComponent;
        BuildCardCalendarTitle(colComponent);
        return colComponent;
    }
}

RefPtr<Element> CalendarComponent::CreateElement()
{
    return AceType::MakeRefPtr<CalendarElement>(GetId());
}

void CalendarComponent::GoTo(int32_t year, int32_t month, int32_t day)
{
    if (day < 0) {
        calendarController_->GoTo(year, month, 1);
    } else {
        calendarController_->SetNeedFocus(true);
        calendarController_->GoTo(year, month, day);
    }
}

void CalendarComponent::BuildCardCalendarTitle(RefPtr<ColumnComponent>& col)
{
    auto preButton = InitCardButton(true);
    auto nextButton = InitCardButton(false);
    DateTime dateTime;
    dateTime.year = calendarController_->GetCurrentMonth().year;
    dateTime.month = calendarController_->GetCurrentMonth().month;
    auto date = Localization::GetInstance()->FormatDateTime(dateTime, "yyyyMMM");
    auto text = AceType::MakeRefPtr<TextComponent>(date);
    TextStyle style;
    auto calendarTheme = theme_->GetCardCalendarTheme();
    style.SetFontSize(calendarTheme.titleFontSize);
    style.SetTextColor(calendarTheme.titleTextColor);
    style.SetFontWeight(FontWeight::W500);
    style.SetAllowScale(false);
    text->SetTextStyle(style);

    auto box = AceType::MakeRefPtr<BoxComponent>();
    Edge edge(CALENDAR_BOX_LEFT_AND_RIGHT_MARGIN, CALENDAR_BOX_TOP_AND_BOTTOM_MARGIN,
        CALENDAR_BOX_LEFT_AND_RIGHT_MARGIN, CALENDAR_BOX_TOP_AND_BOTTOM_MARGIN);

    std::list<RefPtr<Component>> children;
    auto padding = AceType::MakeRefPtr<PaddingComponent>();
    padding->SetChild(text);
    padding->SetPadding(edge);
    children.emplace_back(padding);
    auto button = AceType::MakeRefPtr<ButtonComponent>(children);
    button->SetHeight(calendarTheme.buttonHeight);
    button->SetType(ButtonType::TEXT);
    button->SetBackgroundColor(Color::TRANSPARENT);
    button->SetClickedColor(calendarTheme.clickEffectColor);
    button->SetRectRadius(CALENDAR_BUTTON_RADIUS);
    dateClickId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    button->SetClickedEventId(dateClickId_);
    box->SetChild(button);
    auto flexItem = AceType::MakeRefPtr<FlexItemComponent>(0.0, 0.0, 0.0, box);

    std::list<RefPtr<Component>> rowChildren;
    rowChildren.emplace_back(preButton);
    rowChildren.emplace_back(flexItem);
    rowChildren.emplace_back(nextButton);
    auto rowComponent = AceType::MakeRefPtr<RowComponent>(FlexAlign::CENTER, FlexAlign::CENTER, rowChildren);
    rowComponent->SetTextDirection(TextDirection::LTR);
    std::list<RefPtr<Component>> colChildren;
    colChildren.emplace_back(rowComponent);
    colChildren.emplace_back(swiperContainer_);
    col = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, colChildren);
}

const EventMarker& CalendarComponent::GetSelectedChangeEvent() const
{
    return selectedChangeEvent_;
}

const EventMarker& CalendarComponent::GetRequestDataEvent() const
{
    return requestDataEvent_;
}

CalendarMonthComponent::CalendarMonthComponent(
    int32_t indexOfContainer, const RefPtr<CalendarController>& calendarController)
    : indexOfContainer_(indexOfContainer), calendarController_(calendarController) {}

RefPtr<RenderNode> CalendarMonthComponent::CreateRenderNode()
{
    return RenderCalendar::Create();
}

RefPtr<Element> CalendarMonthComponent::CreateElement()
{
    return AceType::MakeRefPtr<CalendarMonthElement>();
}

void CalendarComponent::SetCalendarData(const std::string& value)
{
    if (calendarController_) {
        calendarData_ = value;
        auto dataAdapter = calendarController_->GetDataAdapter();
        if (dataAdapter) {
            dataAdapter->SetOffDays(offDays_);
            dataAdapter->ParseCardCalendarData(calendarData_);
        }
        calendarController_->UpdateTheme();
    } else {
        calendarData_ = value;
    }
}

RefPtr<ButtonComponent> CalendarComponent::InitCardButton(bool isPreArrow)
{
    auto Arrow = isPreArrow ? AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::LEFT_ARROW_SVG)
                            : AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::RIGHT_ARROW_SVG);
    isPreArrow ? calendarController_->SetLeftRowImage(Arrow) : calendarController_->SetRightRowImage(Arrow);
    auto calendarTheme = theme_->GetCardCalendarTheme();
    Arrow->SetWidth(calendarTheme.arrowWidth);
    Arrow->SetHeight(calendarTheme.arrowHeight);
    std::list<RefPtr<Component>> children;
    children.emplace_back(Arrow);
    auto button = AceType::MakeRefPtr<ButtonComponent>(children);
    button->SetBackgroundColor(Color::TRANSPARENT);
    button->SetClickedColor(calendarTheme.clickEffectColor);
    button->SetWidth(calendarTheme.buttonWidth);
    button->SetHeight(calendarTheme.buttonHeight);
    button->SetRectRadius(CALENDAR_BUTTON_RADIUS);
    isPreArrow ? preClickId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker()
               : nextClickId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    isPreArrow ? button->SetClickedEventId(preClickId_) : button->SetClickedEventId(nextClickId_);

    return button;
}

} // namespace OHOS::Ace
