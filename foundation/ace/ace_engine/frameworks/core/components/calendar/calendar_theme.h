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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_THEME_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {

struct CalendarThemeStructure {
    std::string dayFontWeight = "500";
    std::string lunarDayFontWeight = "500";
    std::string workStateFontWeight = "500";
    Color weekColor;
    Color dayColor;
    Color lunarColor;
    Color weekendDayColor;
    Color weekendLunarColor;
    Color todayColor;
    Color todayLunarColor;
    Color nonCurrentMonthDayColor;
    Color nonCurrentMonthLunarColor;
    Color workDayMarkColor;
    Color offDayMarkColor;
    Color nonCurrentMonthWorkDayMarkColor;
    Color nonCurrentMonthOffDayMarkColor;
    Color focusedDayColor;
    Color focusedLunarColor;
    Color focusedAreaBackgroundColor;
    Color blurAreaBackgroundColor;
    Color titleTextColor;
    Color touchColor;
    Color markLunarColor;
    Color clickEffectColor;
    Color simpleWorkTextColor;
    Color simpleOffTextColor;
    Dimension weekFontSize;
    Dimension dayFontSize;
    Dimension lunarDayFontSize;
    Dimension workDayMarkSize;
    Dimension offDayMarkSize;
    Dimension focusedAreaRadius;
    Dimension topPadding;
    Dimension workStateWidth;
    Dimension workStateHorizontalMovingDistance;
    Dimension workStateVerticalMovingDistance;
    Dimension colSpace;
    Dimension weekHeight;
    Dimension dayHeight;
    Dimension weekWidth;
    Dimension dayWidth;
    Dimension weekAndDayRowSpace;
    Dimension dailyFiveRowSpace;
    Dimension dailySixRowSpace;
    Dimension gregorianCalendarHeight;
    Dimension lunarHeight;
    Dimension arrowHeight;
    Dimension arrowWidth;
    Dimension buttonWidth;
    Dimension buttonHeight;
    Dimension titleFontSize;
    Dimension workStateOffset;
    Dimension dayYAxisOffset;
    Dimension lunarDayYAxisOffset;
    Dimension underscoreXAxisOffset;
    Dimension underscoreYAxisOffset;
    Dimension scheduleMarkerXAxisOffset;
    Dimension scheduleMarkerYAxisOffset;
    Dimension underscoreWidth;
    Dimension underscoreLength;
    Dimension scheduleMarkerRadius;
    Dimension touchCircleStrokeWidth;
    Dimension boundaryRowOffset;
    Dimension boundaryColOffset;
};

class CalendarTheme : public virtual Theme {
    DECLARE_ACE_TYPE(CalendarTheme, Theme);

public:
    ~CalendarTheme() override = default;
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<CalendarTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const
        {
            RefPtr<CalendarTheme> theme = AceType::Claim(new CalendarTheme());
            if (!themeConstants) {
                return theme;
            }
            theme = AceType::Claim(new CalendarTheme());
            theme->calendarTheme_.weekFontSize = themeConstants->GetDimension(THEME_CALENDAR_WEEK_FONT_SIZE);
            theme->calendarTheme_.dayFontSize = themeConstants->GetDimension(THEME_CALENDAR_DAY_FONT_SIZE);
            theme->calendarTheme_.lunarDayFontSize = themeConstants->GetDimension(THEME_CALENDAR_LUNAR_FONT_SIZE);
            theme->calendarTheme_.weekColor = themeConstants->GetColor(THEME_CALENDAR_WEEK_COLOR);
            theme->calendarTheme_.dayColor = themeConstants->GetColor(THEME_CALENDAR_DAY_COLOR);
            theme->calendarTheme_.lunarColor = themeConstants->GetColor(THEME_CALENDAR_LUNAR_COLOR);
            theme->calendarTheme_.weekendDayColor = themeConstants->GetColor(THEME_CALENDAR_WEEKEND_DAY_COLOR);
            theme->calendarTheme_.weekendLunarColor = themeConstants->GetColor(THEME_CALENDAR_WEEKEND_LUNAR_COLOR);
            theme->calendarTheme_.todayColor = themeConstants->GetColor(THEME_CALENDAR_TODAY_DAY_COLOR);
            theme->calendarTheme_.todayLunarColor = themeConstants->GetColor(THEME_CALENDAR_TODAY_LUNAR_COLOR);
            theme->calendarTheme_.nonCurrentMonthDayColor =
                themeConstants->GetColor(THEME_CALENDAR_NON_CURRENT_MONTH_DAY_COLOR);
            theme->calendarTheme_.nonCurrentMonthLunarColor =
                themeConstants->GetColor(THEME_CALENDAR_NON_CURRENT_MONTH_LUNAR_COLOR);
            theme->calendarTheme_.workDayMarkSize =
                themeConstants->GetDimension(THEME_CALENDAR_WORK_DAY_MARK_FONT_SIZE);
            theme->calendarTheme_.offDayMarkSize = themeConstants->GetDimension(THEME_CALENDAR_OFF_DAY_MARK_FONT_SIZE);
            theme->calendarTheme_.workDayMarkColor = themeConstants->GetColor(THEME_CALENDAR_WORK_DAY_MARK_COLOR);
            theme->calendarTheme_.offDayMarkColor = themeConstants->GetColor(THEME_CALENDAR_OFF_DAY_MARK_COLOR);
            theme->calendarTheme_.nonCurrentMonthWorkDayMarkColor =
                themeConstants->GetColor(THEME_CALENDAR_NOT_CURRENT_MONTH_WORK_DAY_MARK_COLOR);
            theme->calendarTheme_.nonCurrentMonthOffDayMarkColor =
                themeConstants->GetColor(THEME_CALENDAR_NOT_CURRENT_MONTH_OFF_DAY_MARK_COLOR);
            theme->calendarTheme_.focusedDayColor = themeConstants->GetColor(THEME_CALENDAR_FOCUSED_DAY_COLOR);
            theme->calendarTheme_.focusedLunarColor = themeConstants->GetColor(THEME_CALENDAR_FOCUSED_LUNAR_COLOR);
            theme->calendarTheme_.focusedAreaRadius = themeConstants->GetDimension(THEME_CALENDAR_FOCUSED_AREA_RADIUS);
            theme->calendarTheme_.focusedAreaBackgroundColor =
                themeConstants->GetColor(THEME_CALENDAR_FOCUSED_AREA_BACKGROUND_COLOR);
            theme->calendarTheme_.topPadding = themeConstants->GetDimension(THEME_CALENDAR_TOP_PADDING);
            theme->calendarTheme_.workStateWidth = themeConstants->GetDimension(THEME_CALENDAR_WORK_STATE_WIDTH);
            theme->calendarTheme_.workStateHorizontalMovingDistance =
                themeConstants->GetDimension(THEME_CALENDAR_WORK_STATE_HORIZONTAL_MOVING_DISTANCE);
            theme->calendarTheme_.workStateVerticalMovingDistance =
                themeConstants->GetDimension(THEME_CALENDAR_WORK_STATE_VERTICAL_MOVING_DISTANCE);
            theme->calendarTheme_.colSpace = themeConstants->GetDimension(THEME_CALENDAR_COL_SPACE);
            theme->calendarTheme_.weekHeight = themeConstants->GetDimension(THEME_CALENDAR_WEEK_HEIGHT);
            theme->calendarTheme_.dayHeight = themeConstants->GetDimension(THEME_CALENDAR_DAY_HEIGHT);
            theme->calendarTheme_.weekWidth = themeConstants->GetDimension(THEME_CALENDAR_WEEK_WIDTH);
            theme->calendarTheme_.dayWidth = themeConstants->GetDimension(THEME_CALENDAR_DAY_WIDTH);
            theme->calendarTheme_.weekAndDayRowSpace =
                themeConstants->GetDimension(THEME_CALENDAR_WEEK_AND_DAY_ROW_SPACE);
            theme->calendarTheme_.dailyFiveRowSpace = themeConstants->GetDimension(THEME_CALENDAR_DAILY_FIVE_ROW_SPACE);
            theme->calendarTheme_.dailySixRowSpace = themeConstants->GetDimension(THEME_CALENDAR_DAILY_SIX_ROW_SPACE);
            theme->calendarTheme_.gregorianCalendarHeight =
                themeConstants->GetDimension(THEME_CALENDAR_GREGORIAN_CALENDAR_HEIGHT);
            theme->calendarTheme_.blurAreaBackgroundColor =
                themeConstants->GetColor(THEME_CALENDAR_BLUR_AREA_BACKGROUND_COLOR);
            theme->calendarTheme_.dayFontWeight = themeConstants->GetString(THEME_CALENDAR_DAY_FONT_WIGHT);
            theme->calendarTheme_.lunarDayFontWeight = themeConstants->GetString(THEME_CALENDAR_LUNAR_DAY_FONT_WIGHT);
            theme->calendarTheme_.workStateFontWeight = themeConstants->GetString(THEME_CALENDAR_WORK_STATE_FONT_WIGHT);
            theme->calendarTheme_.workStateOffset =
                themeConstants->GetDimension(THEME_CALENDAR_WORK_STATE_CENTER_ADJUSTMENT);
            theme->calendarTheme_.dayYAxisOffset = themeConstants->GetDimension(THEME_CALENDAR_DAY_YAXIS_OFFSET);
            theme->calendarTheme_.lunarDayYAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_LUNAR_DAY_YAXIS_OFFSET);
            theme->calendarTheme_.underscoreXAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_UNDERSCORE_XAXIS_OFFSET);
            theme->calendarTheme_.underscoreYAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_UNDERSCORE_YAXIS_OFFSET);
            theme->calendarTheme_.scheduleMarkerXAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_SCHEDULE_MARKER_XAXIS_OFFSET);
            theme->calendarTheme_.scheduleMarkerYAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_SCHEDULE_MARKER_YAXIS_OFFSET);
            theme->calendarTheme_.touchCircleStrokeWidth =
                themeConstants->GetDimension(THEME_CALENDAR_TOUCH_CIRCLE_STROKE_WIDTH);
            theme->calendarTheme_.lunarHeight = themeConstants->GetDimension(THEME_CALENDAR_LUNAR_HEIGHT);
            theme->calendarTheme_.underscoreWidth = themeConstants->GetDimension(THEME_CALENDAR_UNDERSCORE_WIDTH);
            theme->calendarTheme_.underscoreLength = themeConstants->GetDimension(THEME_CALENDAR_UNDERSCORE_LENGTH);
            theme->calendarTheme_.scheduleMarkerRadius =
                themeConstants->GetDimension(THEME_CALENDAR_SCHEDULE_MARKER_RADIUS);

            theme->cardCalendarTheme_.titleTextColor = themeConstants->GetColor(THEME_CARD_CALENDAR_TITLE_TEXT_COLOR);
            theme->cardCalendarTheme_.arrowHeight = themeConstants->GetDimension(THEME_CARD_CALENDAR_ARROW_HEIGHT);
            theme->cardCalendarTheme_.arrowWidth = themeConstants->GetDimension(THEME_CARD_CALENDAR_ARROW_WIDTH);
            theme->cardCalendarTheme_.buttonHeight = themeConstants->GetDimension(THEME_CARD_CALENDAR_BUTTON_HEIGHT);
            theme->cardCalendarTheme_.buttonWidth = themeConstants->GetDimension(THEME_CARD_CALENDAR_BUTTON_WIDTH);
            theme->cardCalendarTheme_.titleFontSize = themeConstants->GetDimension(THEME_CARD_CALENDAR_TITLE_FONT_SIZE);
            theme->cardCalendarTheme_.weekColor = themeConstants->GetColor(THEME_CARD_CALENDAR_WEEK_COLOR);
            theme->cardCalendarTheme_.dayColor = themeConstants->GetColor(THEME_CARD_CALENDAR_DAY_COLOR);
            theme->cardCalendarTheme_.weekendDayColor = themeConstants->GetColor(THEME_CARD_CALENDAR_WEEKEND_DAY_COLOR);
            theme->cardCalendarTheme_.nonCurrentMonthDayColor =
                themeConstants->GetColor(THEME_CARD_CALENDAR_NON_CURRENT_MONTH_DAY_COLOR);
            theme->cardCalendarTheme_.focusedAreaBackgroundColor =
                themeConstants->GetColor(THEME_CARD_CALENDAR_FOCUS_AREA_BACKGROUND_COLOR);
            theme->cardCalendarTheme_.focusedDayColor = themeConstants->GetColor(THEME_CARD_CALENDAR_FOCUS_DAY_COLOR);
            theme->cardCalendarTheme_.touchColor = themeConstants->GetColor(THEME_CARD_CALENDAR_TOUCH_COLOR);
            theme->cardCalendarTheme_.topPadding = themeConstants->GetDimension(THEME_CARD_CALENDAR_TOP_PADDING);
            theme->cardCalendarTheme_.weekHeight = themeConstants->GetDimension(THEME_CARD_CALENDAR_WEEK_HEIGHT);
            theme->cardCalendarTheme_.dayHeight = themeConstants->GetDimension(THEME_CARD_CALENDAR_DAY_HEIGHT);
            theme->cardCalendarTheme_.weekWidth = themeConstants->GetDimension(THEME_CARD_CALENDAR_WEEK_WIDTH);
            theme->cardCalendarTheme_.dayWidth = themeConstants->GetDimension(THEME_CARD_CALENDAR_DAY_WIDTH);
            theme->cardCalendarTheme_.focusedAreaRadius =
                themeConstants->GetDimension(THEME_CARD_CALENDAR_FOCUS_AREA_RADIUS);
            theme->cardCalendarTheme_.weekFontSize = themeConstants->GetDimension(THEME_CARD_CALENDAR_WEEK_FONT_SIZE);
            theme->cardCalendarTheme_.dayFontSize = themeConstants->GetDimension(THEME_CARD_CALENDAR_DAY_FONT_SIZE);
            theme->cardCalendarTheme_.lunarColor = themeConstants->GetColor(THEME_CARD_CALENDAR_LUNAR_COLOR);
            theme->cardCalendarTheme_.weekendLunarColor =
                themeConstants->GetColor(THEME_CARD_CALENDAR_WEEKEND_LUNAR_COLOR);
            theme->cardCalendarTheme_.nonCurrentMonthLunarColor =
                themeConstants->GetColor(THEME_CARD_CALENDAR_NON_CURRENT_MONTH_LUNAR_COLOR);
            theme->cardCalendarTheme_.gregorianCalendarHeight =
                themeConstants->GetDimension(THEME_CARD_CALENDAR_GREGORIAN_CALENDAR_HEIGHT);
            theme->cardCalendarTheme_.lunarDayFontSize =
                themeConstants->GetDimension(THEME_CARD_CALENDAR_LUNAR_FONT_SIZE);
            theme->cardCalendarTheme_.workDayMarkColor =
                themeConstants->GetColor(THEME_CARD_CALENDAR_WORK_DAY_MARK_COLOR);
            theme->cardCalendarTheme_.weekAndDayRowSpace =
                themeConstants->GetDimension(THEME_CARD_CALENDAR_WEEK_AND_DAY_ROW_SPACE);
            theme->cardCalendarTheme_.dailyFiveRowSpace =
                themeConstants->GetDimension(THEME_CARD_CALENDAR_DAILY_FIVE_ROW_SPACE);
            theme->cardCalendarTheme_.dayYAxisOffset = themeConstants->GetDimension(THEME_CALENDAR_DAY_YAXIS_OFFSET);
            theme->cardCalendarTheme_.lunarDayYAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_LUNAR_DAY_YAXIS_OFFSET);
            theme->cardCalendarTheme_.underscoreXAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_UNDERSCORE_XAXIS_OFFSET);
            theme->cardCalendarTheme_.underscoreYAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_UNDERSCORE_YAXIS_OFFSET);
            theme->cardCalendarTheme_.scheduleMarkerXAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_SCHEDULE_MARKER_XAXIS_OFFSET);
            theme->cardCalendarTheme_.scheduleMarkerYAxisOffset =
                themeConstants->GetDimension(THEME_CALENDAR_SCHEDULE_MARKER_YAXIS_OFFSET);
            theme->cardCalendarTheme_.lunarHeight = themeConstants->GetDimension(THEME_CALENDAR_LUNAR_HEIGHT);
            theme->cardCalendarTheme_.underscoreWidth = themeConstants->GetDimension(THEME_CALENDAR_UNDERSCORE_WIDTH);
            theme->cardCalendarTheme_.underscoreLength = themeConstants->GetDimension(THEME_CALENDAR_UNDERSCORE_LENGTH);
            theme->cardCalendarTheme_.scheduleMarkerRadius =
                themeConstants->GetDimension(THEME_CALENDAR_SCHEDULE_MARKER_RADIUS);
            theme->cardCalendarTheme_.boundaryColOffset =
                themeConstants->GetDimension(THEME_CARD_CALENDAR_BOUNDARY_COL_OFFSET);

            auto themeStyle = themeConstants->GetThemeStyle();
            if (themeStyle) {
                theme->cardCalendarTheme_.focusedAreaBackgroundColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_ACCENT, Color::BLUE);
                theme->cardCalendarTheme_.dayColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_PRIMARY, Color::BLACK);
                theme->cardCalendarTheme_.weekColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_PRIMARY, Color::BLACK);
                theme->cardCalendarTheme_.nonCurrentMonthDayColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_TERTIARY, Color::BLACK);
                theme->cardCalendarTheme_.weekendDayColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_SECONDARY, Color::BLACK);
                theme->cardCalendarTheme_.weekendLunarColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_SECONDARY, Color::BLACK);
                theme->cardCalendarTheme_.nonCurrentMonthLunarColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_FOURTH, Color::BLACK);
                theme->cardCalendarTheme_.todayColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_PRIMARY_INVERSE, Color::WHITE);
                theme->cardCalendarTheme_.todayLunarColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_PRIMARY_INVERSE, Color::WHITE);
                theme->cardCalendarTheme_.lunarColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_SECONDARY, Color::BLACK);
                theme->cardCalendarTheme_.markLunarColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_COLOR_TEXT_PRIMARY_ACTIVATED, Color::BLUE);
                theme->cardCalendarTheme_.titleTextColor =
                    themeStyle->GetAttr<Color>(THEME_ATTR_TEXT_COLOR_PRIMARY, Color::BLACK);
                theme->cardCalendarTheme_.clickEffectColor =
                    themeStyle->GetAttr<Color>(TEXTFIELD_PRESS_COLOR, Color::TRANSPARENT);
            }
            return theme;
        }
    };

    CalendarThemeStructure& GetCalendarTheme()
    {
        return calendarTheme_;
    }

    CalendarThemeStructure& GetCardCalendarTheme()
    {
        return cardCalendarTheme_;
    }

protected:
    CalendarTheme() = default;

private:
    CalendarThemeStructure calendarTheme_;
    CalendarThemeStructure cardCalendarTheme_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CALENDAR_CALENDAR_THEME_H
