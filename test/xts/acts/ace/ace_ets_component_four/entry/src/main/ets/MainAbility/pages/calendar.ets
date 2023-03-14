// @ts-nocheck
/**
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
import events_emitter from '@ohos.emitter';

@Entry
@Component
struct CalendarComponentExample {
  calendarController: CalendarController = new CalendarController();
  @State CalendarDayArray: CalendarDay[] = [{
                                              index: 1,
                                              lunarMonth: "five",
                                              lunarDay: "6",
                                              dayMark: "六",
                                              dayMarkValue: "儿童节",
                                              year: 2020,
                                              month: 11,
                                              day: 10,
                                              isFirstOfLunar: false,
                                              markLunarDay: false
                                            }];
  @State currentMonthday: MonthData = {
    year: 2021,
    month: 3,
    data: this.CalendarDayArray
  };
  @State preMonthday: MonthData = {
    year: 2021,
    month: 2,
    data: this.CalendarDayArray
  };
  @State nextMonthday: MonthData = {
    year: 2021,
    month: 4,
    data: this.CalendarDayArray
  };
  @State colorValue: Color = Color.Blue;
  @State calendarDay1: CalendarDay = {
    index: 6,
    lunarMonth: "111",
    lunarDay: "222",
    dayMark: "333",
    dayMarkValue: "444",
    year: 2022,
    month: 3,
    day: 11,
    isFirstOfLunar: false,
    hasSchedule: false,
    markLunarDay: false
  }
  @State calendarDay2: CalendarDay = {
    index: 6,
    lunarMonth: "111",
    lunarDay: "222",
    dayMark: "333",
    dayMarkValue: "444",
    year: 2022,
    month: 3,
    day: 10,
    isFirstOfLunar: false,
    hasSchedule: false,
    markLunarDay: false
  }
  @State calendarDay3: CalendarDay = {
    index: 6,
    lunarMonth: "111",
    lunarDay: "222",
    dayMark: "333",
    dayMarkValue: "444",
    year: 2022,
    month: 3,
    day: 12,
    isFirstOfLunar: false,
    hasSchedule: false,
    markLunarDay: false
  }
  @State currentData_: MonthData = { year: 2022, month: 3, data: [this.calendarDay1] };
  @State preData_: MonthData = { year: 2022, month: 3, data: [this.calendarDay2] };
  @State nextData_: MonthData = { year: 2022, month: 3, data: [this.calendarDay3] };

  @State colSpace: number = 5;
  @State dailyFiveRowSpace: number = 4;
  @State dailySixRowSpace: number = 5;
  @State lunarHeight: number = 20;
  @State underscoreWidth: number = 10;
  @State underscoreLength: number = 6;
  @State scheduleMarkerRadius: number = 3;
  @State boundaryRowOffset: number = 3;
  @State weekAndDayRowSpace: number = 3;
  @State direction: Axis = Axis.Horizontal;
  @State weekStyle: WeekStyle = {
    weekColor: Color.black,
    weekendDayColor: Color.red,
    weekendLunarColor:  Color.green,
    weekFontSize: 1,
    weekHeight: 2,
    weekWidth: 3,
    weekAndDayRowSpace: this.weekAndDayRowSpace
  };
  private stateChangCallBack = (eventData) => {
    if (eventData != null) {
      console.info("Calendar page state change called:" + JSON.stringify(eventData));
      if (eventData.data.colSpace != null) {
        this.colSpace = eventData.data.colSpace;
        console.log("Calendar page colSpace value" + this.colSpace);
      }
      if (eventData.data.dailyFiveRowSpace != null) {
        this.dailyFiveRowSpace = eventData.data.dailyFiveRowSpace;
        console.log("Calendar page dailyFiveRowSpace value" + this.dailyFiveRowSpace);
      }
      if (eventData.data.dailySixRowSpace != null) {
        this.dailySixRowSpace = eventData.data.dailySixRowSpace;
        console.log("Calendar page dailySixRowSpace value" + this.dailySixRowSpace);
      }
      if (eventData.data.underscoreWidth != null) {
        this.underscoreWidth = eventData.data.underscoreWidth;
        console.log("Calendar page underscoreWidth value" + this.underscoreWidth);
      }
      if (eventData.data.underscoreLength != null) {
        this.underscoreLength = eventData.data.underscoreLength;
        console.log("Calendar page underscoreLength value" + this.underscoreLength);
      }
      if (eventData.data.lunarHeight != null) {
        this.lunarHeight = eventData.data.lunarHeight;
        console.log("Calendar page lunarHeight value" + this.lunarHeight);
      }
      if (eventData.data.scheduleMarkerRadius != null) {
        this.scheduleMarkerRadius = eventData.data.scheduleMarkerRadius;
        console.log("Calendar page scheduleMarkerRadius value" + this.scheduleMarkerRadius);
      }
      if (eventData.data.boundaryRowOffset != null) {
        this.boundaryRowOffset = eventData.data.boundaryRowOffset;
        console.log("Calendar page boundaryRowOffset value" + this.boundaryRowOffset);
      }

      if (eventData.data.weekAndDayRowSpace != null) {
        this.weekAndDayRowSpace = eventData.data.weekAndDayRowSpace;
        console.log("Calendar page weekAndDayRowSpace value" + this.weekAndDayRowSpace);
      }

      if (eventData.data.direction != null) {
        this.direction = eventData.data.direction;
        console.log("Calendar page direction value" + this.direction);
      }
    }
  }

  onPageShow() {
    console.info('Calendar page show called');
    var stateChangeEvent = {
      eventId: 74,
      priority: events_emitter.EventPriority.LOW
    }
    events_emitter.on(stateChangeEvent, this.stateChangCallBack);
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.SpaceBetween }) {
      Calendar({
        year: 2020,
        month: 5,
        day: 12
      }, this.currentMonthday, this.preMonthday, this.nextMonthday, this.calendarController)
        .currentDayStyle({
          dayHeight: 30,
          dayWidth: 30,
          gregorianCalendarHeight: 30,
          dayYAxisOffset: 1,
          lunarDayYAxisOffset: 1,
          underscoreXAxisOffset: 1,
          underscoreYAxisOffset: 1,
          scheduleMarkerXAxisOffset: 0,
          scheduleMarkerYAxisOffset: 0,
          colSpace: this.colSpace,
          dailyFiveRowSpace: this.dailyFiveRowSpace,
          dailySixRowSpace: this.dailySixRowSpace,
          lunarHeight: this.lunarHeight,
          underscoreWidth: this.underscoreWidth,
          underscoreLength: this.underscoreLength,
          scheduleMarkerRadius: this.scheduleMarkerRadius,
          boundaryRowOffset: this.boundaryRowOffset
        })
        .weekStyle(this.weekStyle)
        .direction(this.direction)
        .backgroundColor(0x317aff)
        .width(100)
        .height(50)
        .key('calender')
    }.height(400).padding({ left: 35, right: 35, top: 35 })
  }
}