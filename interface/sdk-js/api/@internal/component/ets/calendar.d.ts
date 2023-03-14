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

/**
 * Provides a monthly view component to display information such as date, shift break, and schedule.
 * @since 7
 * @systemapi
 */
interface CalendarDay {
  /**
   * Indicates the sequence number of the 7 x 7 (7 x 6) grid layout on a calendar page by row.
   * The week sequence is one, two, three, four, five, six.
   * @since 7
   * @systemapi
   */
  index: number;

  /**
   * Lunar moon.
   * The week sequence is one, two, three, four, five, six.
   * @since 7
   * @systemapi
   */
  lunarMonth: string;

  /**
   * Lunar day.
   * @since 7
   * @systemapi
   */
  lunarDay: string;

  /**
   * Day.
   * @since 7
   * @systemapi
   */
  dayMark: string;

  /**
   * Indicates the off-duty flag information. The options are work and off.By default, the off-duty flag information is not required.
   * @since 7
   * @systemapi
   */
  dayMarkValue: string;

  /**
   * Gregorian calendar year.
   * @since 7
   * @systemapi
   */
  year: number;

  /**
   * Gregorian calendar month.
   * @since 7
   * @systemapi
   */
  month: number;

  /**
   * Gregorian calendar day.
   * @since 7
   * @systemapi
   */
  day: number;

  /**
   *
   * Indicates whether the default value is Lunar calendar.
   * @since 7
   * @systemapi
   */
  isFirstOfLunar: boolean;

  /**
   * Indicates whether to display has Schedule.
   * The week sequence is one, two, three, four, five, six.
   * @since 7
   * @systemapi
   */
  hasSchedule: boolean;

  /**
   * Display Lunar Date.The week sequence is one, two, three, four, five, six.
   * @since 7
   * @systemapi
   */
  markLunarDay: boolean;
}

/**
 * Date object.
 * @since 7
 * @systemapi
 */
interface MonthData {
  /**
   * Gregorian calendar year.
   * @since 7
   * @systemapi
   */
  year: number;

  /**
   * Gregorian calendar month.
   * @since 7
   * @systemapi
   */
  month: number;

  /**
   * CalendarDay.
   * @since 7
   * @systemapi
   */
  data: CalendarDay[];
}

/**
 * CurrentDayStyle object.
 * @since 7
 * @systemapi
 */
interface CurrentDayStyle {
  /**
   * Text color.
   * @since 7
   * @systemapi
   */
  dayColor?: ResourceColor;

  /**
   * lunar Text color.
   * @since 7
   * @systemapi
   */
  lunarColor?: ResourceColor;

  /**
   * lunar  Work and rest  text color.
   * @since 7
   * @systemapi
   */
  markLunarColor?: ResourceColor;

  /**
   * Text fontSize.
   * @since 7
   * @systemapi
   */
  dayFontSize?: number;

  /**
   * lunar text fontSize.
   * @since 7
   * @systemapi
   */
  lunarDayFontSize?: number;

  /**
   * Single date height.
   * @since 7
   * @systemapi
   */
  dayHeight?: number;

  /**
   * Single date width.
   * @since 7
   * @systemapi
   */
  dayWidth?: number;

  /**
   * Gregorian calendar height.
   * @since 7
   * @systemapi
   */
  gregorianCalendarHeight?: number;

  /**
   * Data y axis Off set.
   * @since 7
   * @systemapi
   */
  dayYAxisOffset?: number;

  /**
   * Lunar data y axis Off set.
   * @since 7
   * @systemapi
   */
  lunarDayYAxisOffset?: number;

  /**
   * Under score X Axis Off set.
   * @since 7
   * @systemapi
   */
  underscoreXAxisOffset?: number;

  /**
   * Under score Y Axis Off set
   * @since 7
   * @systemapi
   */
  underscoreYAxisOffset?: number;

  /**
   * Schedule marker X axis Off set
   * @since 7
   * @systemapi
   */
  scheduleMarkerXAxisOffset?: number;

  /**
   * schedule Marker Y Axis Off set
   * @since 7
   * @systemapi
   */
  scheduleMarkerYAxisOffset?: number;

  /**
   * Number of columns.
   * @since 7
   * @systemapi
   */
  colSpace?: number;

  /**
   * Daily five row space.
   * @since 7
   * @systemapi
   */
  dailyFiveRowSpace?: number;

  /**
   * Daily six row space.
   * @since 7
   * @systemapi
   */
  dailySixRowSpace?: number;

  /**
   * Single lunar height.
   * @since 7
   * @systemapi
   */
  lunarHeight?: number;

  /**
   * Under score width.
   * @since 7
   * @systemapi
   */
  underscoreWidth?: number;

  /**
   * Under score length.
   * @since 7
   * @systemapi
   */
  underscoreLength?: number;

  /**
   * Schedule marker radius.
   * @since 7
   * @systemapi
   */
  scheduleMarkerRadius?: number;

  /**
   * Bound dary row offset.
   * @since 7
   * @systemapi
   */
  boundaryRowOffset?: number;

  /**
   * Boundary col offset.
   * @since 7
   * @systemapi
   */
  boundaryColOffset?: number;
}

/**
 * Non current day style.
 * @since 7
 * @systemapi
 */
interface NonCurrentDayStyle {
  /**
   * Non-current month day color.
   * @since 7
   * @systemapi
   */
  nonCurrentMonthDayColor?: ResourceColor;

  /**
   * Lunar style of non-current month.
   * @since 7
   * @systemapi
   */
  nonCurrentMonthLunarColor?: ResourceColor;

  /**
   * Non-Current Month Workday Marker Color.
   * @since 7
   * @systemapi
   */
  nonCurrentMonthWorkDayMarkColor?: ResourceColor;

  /**
   * Non-Current Month Off Day Marker Color.
   * @since 7
   * @systemapi
   */
  nonCurrentMonthOffDayMarkColor?: ResourceColor;
}

/**
 * Non current day style.
 * @since 7
 * @systemapi
 */
interface TodayStyle {
  /**
   * Style of focus color.
   * @since 7
   * @systemapi
   */
  focusedDayColor?: ResourceColor;

  /**
   * Focus on Lunar Colors.
   * @since 7
   * @systemapi
   */
  focusedLunarColor?: ResourceColor;

  /**
   * Background color of the focus area.
   * @since 7
   * @systemapi
   */
  focusedAreaBackgroundColor?: ResourceColor;

  /**
   * Focus area radius.
   * @since 7
   * @systemapi
   */
  focusedAreaRadius?: number;
}

/**
 * Week Style.
 * @since 7
 * @systemapi
 */
interface WeekStyle {
  /**
   * Style of week color.
   * @since 7
   * @systemapi
   */
  weekColor?: ResourceColor;

  /**
   * Style of week day color.
   * @since 7
   * @systemapi
   */
  weekendDayColor?: ResourceColor;

  /**
   * Style of lunar color.
   * @since 7
   * @systemapi
   */
  weekendLunarColor?: ResourceColor;

  /**
   * Style of week font size.
   * @since 7
   * @systemapi
   */
  weekFontSize?: number;

  /**
   * Style of week height.
   * @since 7
   * @systemapi
   */
  weekHeight?: number;

  /**
   * Style of week width.
   * @since 7
   * @systemapi
   */
  weekWidth?: number;

  /**
   * Style of week space.
   * @since 7
   * @systemapi
   */
  weekAndDayRowSpace?: number;
}

/**
 * Work state style.
 * @since 7
 * @systemapi
 */
interface WorkStateStyle {
  /**
   * Style of day color.
   * @since 7
   * @systemapi
   */
  workDayMarkColor?: ResourceColor;

  /**
   * Style of day color.
   * @since 7
   * @systemapi
   */
  offDayMarkColor?: ResourceColor;

  /**
   * Style of day size.
   * @since 7
   * @systemapi
   */
  workDayMarkSize?: number;

  /**
   * Style of day size.
   * @since 7
   * @systemapi
   */
  offDayMarkSize?: number;

  /**
   * Style of width.
   * @since 7
   * @systemapi
   */
  workStateWidth?: number;

  /**
   * Style of distance.
   * @since 7
   * @systemapi
   */
  workStateHorizontalMovingDistance?: number;

  /**
   * Style of distance.
   * @since 7
   * @systemapi
   */
  workStateVerticalMovingDistance?: number;
}

/**
 * Calendar controller.
 * @since 7
 * @systemapi
 */
declare class CalendarController {
  /**
   * Constructor.
   * @since 7
   * @systemapi
   */
  constructor();

  /**
   * Back to day.
   * @since 7
   * @systemapi
   */
  backToToday();

  /**
   * To the specified element.
   * @since 7
   * @systemapi
   */
  goTo(value: { year: number; month: number; day: number });
}

/**
 * @since 7
 * @systemapi
 */
interface CalendarInterface {
  /**
   * Set value.
   * @since 7
   * @systemapi
   */
  (value: {
    date: { year: number; month: number; day: number };
    currentData: MonthData;
    preData: MonthData;
    nextData: MonthData;
    controller?: CalendarController;
  }): CalendarAttribute;
}

/**
 * @since 7
 * @systemapi
 */
declare class CalendarAttribute {
  /**
   * Specifies whether the component displays the lunar calendar information.
   * @since 7
   * @systemapi
   */
  showLunar(value: boolean): CalendarAttribute;

  /**
   * Setting whether to display holiday information
   * @since 7
   * @systemapi
   */
  showHoliday(value: boolean): CalendarAttribute;

  /**
   * Indicates whether the page can be scrolled.
   * @since 7
   * @systemapi
   */
  needSlide(value: boolean): CalendarAttribute;

  /**
   * Set the start day of the week for the calendar.
   * @since 7
   * @systemapi
   */
  startOfWeek(value: number): CalendarAttribute;

  /**
   * Set weekend. The default value is Sunday and Saturday.
   * @since 7
   * @systemapi
   */
  offDays(value: number): CalendarAttribute;

  /**
   * Sets the sliding direction.
   * @since 7
   * @systemapi
   */
  direction(value: Axis): CalendarAttribute;

  /**
   * Sets the date style in the current month.
   * @since 7
   * @systemapi
   */
  currentDayStyle(value: CurrentDayStyle): CalendarAttribute;

  /**
   * Sets the non-monthly date style.
   * @since 7
   * @systemapi
   */
  nonCurrentDayStyle(value: NonCurrentDayStyle): CalendarAttribute;

  /**
   * Set the date style for today.
   * @since 7
   * @systemapi
   */
  todayStyle(value: TodayStyle): CalendarAttribute;

  /**
   * Sets the date style for the weekend.
   * @since 7
   * @systemapi
   */
  weekStyle(value: WeekStyle): CalendarAttribute;

  /**
   * Sets the style of the working state.
   * @since 7
   * @systemapi
   */
  workStateStyle(value: WorkStateStyle): CalendarAttribute;

  /**
   * Click a date to return the information about the date you clicked.
   * @since 7
   * @systemapi
   */
  onSelectChange(event: (event: { year: number; month: number; day: number }) => void): CalendarAttribute;

  /**
   * When you swipe to switch months, the information about the previous month and the next month is requested.
   * @since 7
   * @systemapi
   */
  onRequestData(
    event: (event: {
      year: number;
      month: number;
      currentYear: number;
      currentMonth: number;
      monthState: number;
    }) => void,
  ): CalendarAttribute;
}

declare const Calendar: CalendarInterface;
declare const CalendarInstance: CalendarAttribute;
