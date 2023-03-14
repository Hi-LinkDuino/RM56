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

@Entry
@Component
struct calendar {
  @State curMonth: MonthData = { year: 0, month: 0, data: [] };
  @State preMonth: MonthData = { year: 0, month: 0, data: [] };
  @State nextMonth: MonthData = { year: 0, month: 0, data: [] }
  @State year: number = 0
  @State month: number = 0
  Controller: CalendarController = new CalendarController();
  private lunarMonthDays: string[] = [
    '初一', '初二', '初三', '初四', '初五', '初六', '初七', '初八', '初九', '初十',
    '十一', '十二', '十三', '十四', '十五', '十六', '十七', '十八', '十九', '二十',
    '廿一', '廿二', '廿三', '廿四', '廿五', '廿六', '廿七', '廿八', '廿九', '三十']
  private lunarMonthNames: string[] = ['正月', '二月', '三月', '四月', '五月', '六月', '七月', '八月', '九月', '十月', '冬月', '腊月']

  build() {

    Flex({
      direction: FlexDirection.Column,
      alignItems: ItemAlign.Start
    }) {

      Row() {
        Text(`${this.year} 年 ${this.month + 1} 月`)
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
      }

      Calendar({
        date: { year: 2021, month: 8, day: 35 },
        currentData: this.curMonth,
        preData: this.preMonth,
        nextData: this.nextMonth,
        controller: this.Controller
      })
        .showLunar(true)
        .showHoliday(true)
        .needSlide(true)
        .offDays(Week.Tue | Week.Wed)
        .startOfWeek(Week.Sun)
        .direction(Axis.Horizontal)
        .currentDayStyle({
          dayColor: Color.Black,
          lunarColor: Color.Gray,
          markLunarColor: Color.Black,
          dayFontSize: 15,
          lunarDayFontSize: 10,
          boundaryColOffset: 10,
        })
        .nonCurrentDayStyle({
          nonCurrentMonthDayColor: Color.Black,
          nonCurrentMonthLunarColor: Color.Gray,
          nonCurrentMonthWorkDayMarkColor: Color.Green,
          nonCurrentMonthOffDayMarkColor: Color.Brown,
        })
        .todayStyle({
          focusedDayColor: Color.Red,
          focusedLunarColor: Color.Orange,
          focusedAreaBackgroundColor: Color.Blue,
          focusedAreaRadius: 20
        })
        .weekStyle({
          weekColor: Color.Black,
          weekendDayColor: Color.Orange,
          weekendLunarColor: Color.Blue,
          weekFontSize: 20,
          weekHeight: 5,
          weekWidth: 48,
        })
        .workStateStyle({
          workDayMarkColor: Color.Red,
          offDayMarkColor: Color.Orange,
          workDayMarkSize: 10,
          offDayMarkSize: 2,
          workStateWidth: 12,
          workStateHorizontalMovingDistance: 0,
          workStateVerticalMovingDistance: 12,
        })
        .onRequestData((request) => {
          switch (request.monthState) {
            case 0:
              this.year = request.currentYear;
              this.month = request.currentMonth;
              this.curMonth.year = request.year;
              this.curMonth.month = request.month;
              this.curMonth.data = [];
              for (var i = 0; i < 35; ++i) {
                this.curMonth.data.push({
                  index: i,
                  lunarMonth: this.lunarMonthNames[i%12],
                  lunarDay: this.lunarMonthDays[i % 30],
                  dayMark: "work",
                  dayMarkValue: "",
                  year: request.year,
                  month: request.month,
                  day: i + 1,
                  isFirstOfLunar: false,
                  hasSchedule: false,
                  markLunarDay: false
                });

                if (i % 7 == 0 || i % 7 == 6) {
                  this.curMonth.data[i].dayMarkValue = "休"
                }
              }
              break;
            case 1:
              this.year = request.currentYear;
              this.month = request.currentMonth;
              this.preMonth.year = request.year;
              this.preMonth.month = request.month
              this.preMonth.data = [];
              for (var i = 0; i < 35; ++i) {
                this.preMonth.data.push({
                  index: i,
                  lunarMonth: this.lunarMonthNames[i%12],
                  lunarDay: this.lunarMonthDays[i % 30],
                  dayMark: "work",
                  dayMarkValue: "",
                  year: request.year,
                  month: request.month,
                  day: i + 1,
                  isFirstOfLunar: false,
                  hasSchedule: false,
                  markLunarDay: false
                });
                if (i % 7 == 0 || i % 7 == 6) {
                  this.preMonth.data[i].dayMarkValue = "休"
                }
              }
              break;
            case 2:
              this.year = request.currentYear;
              this.month = request.currentMonth;
              this.nextMonth.year = request.year;
              this.nextMonth.month = request.month;
              this.nextMonth.data = [];
              for (var i = 0; i < 35; ++i) {
                this.nextMonth.data.push({
                  index: i,
                  lunarMonth: this.lunarMonthNames[i%12],
                  lunarDay: this.lunarMonthDays[i % 30],
                  dayMark: "work",
                  dayMarkValue: "",
                  year: request.year,
                  month: request.month,
                  day: i + 1,
                  isFirstOfLunar: false,
                  hasSchedule: false,
                  markLunarDay: false
                });
                if (i % 7 == 0 || i % 7 == 6) {
                  this.nextMonth.data[i].dayMarkValue = "休"
                }
              }
              break;
          }
        })
        .onSelectChange((request) => {
          this.year = request.year;
          this.month = request.month;
          console.info('On Select change: ' + this.year + '/' + this.month)
        })
      Row() {
        Button("back To Today").onClick(() => {
          this.Controller.backToToday();
          console.info("back To Today")
        });
        Button("go To").onClick(() => {
          this.Controller.goTo({ year: 2021, month: 9, day: 10 });
          console.info("go To : 2021/9/10")
        });
      }
    }.margin({ bottom: 10, right: 20, left: 20, top: 0 })
    .height("50%")
  }
}