/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
import router from "@system.router"
import app from '@system.app'

export default {
  data: {
    time: ["07:20 上午", "02:00 下午", "12:00 上午", "12:00 下午"],
    repeat: ["一 二 三 四 五", "一 二 三 四 五", "日 一 二 三 四 五 六", "不重复"],
    mode: ["addAlarm", "editAlarm", "deleteAlarm", "timeArrive"],
    switchStatus: [false, false, false, false],
    weeks: ["一", "二", "三", "四", "五", "六", "日"],
    dataWrapper: {
      mode: "",
      time: "00:00",
      repeat: "不重复",
      alarmItemIndex: -1
    },
    originData: [],
    isDebug: false,
    curTime: "",
    curDay: "",
    interValId: ""
  },
  packOriginData() {
    this.originData = [this.time, this.repeat, this.switchStatus];
  },
  unPackOriginData() {
    this.time = this.originData[0];
    this.repeat = this.originData[1];
    var switchStatusIndex = 2;
    this.switchStatus = this.originData[switchStatusIndex];
  },
  onInit() {
    console.log("[mainPage init]");
    this.startTimer();

    if (this.originData.length === 0) { // app first start
      this.packOriginData();
    } else {
      if (this.isDebug) {
        console.log("[mainPage init] mode: " + this.dataWrapper.mode);
        console.log("[mainPage init] mode: " + this.dataWrapper.time);
        console.log("[mainPage init] mode: " + this.dataWrapper.repeat);
        console.log("[mainPage init] mode: " + this.dataWrapper.alarmItemIndex);
      }

      this.unPackOriginData(); // apply origin data to current environment

      var modeDeleteAlarmIndex = 3;
      if (this.dataWrapper.mode === this.mode[0] && typeof (this.cancelAlarm) == "undefined") { // add alarm
        this.time.push(this.transformTimeToTwelveHour(this.dataWrapper.time));
        this.repeat.push(this.dataWrapper.repeat);
        this.switchStatus.push(false);
      } else if (this.dataWrapper.mode === this.mode[1]) { // edit alarm
        if (typeof (this.deleteAlarm) != "undefined") {
          this.time.splice(this.dataWrapper.alarmItemIndex, 1);
          this.repeat.splice(this.dataWrapper.alarmItemIndex, 1);
          this.switchStatus.splice(this.dataWrapper.alarmItemIndex, 1);
        } else {
          this.time[this.dataWrapper.alarmItemIndex] = this.transformTimeToTwelveHour(this.dataWrapper.time);
          this.repeat[this.dataWrapper.alarmItemIndex] = this.dataWrapper.repeat;
          this.switchStatus[this.dataWrapper.alarmItemIndex] = true;
        }
      } else if (this.dataWrapper.mode === this.mode[modeDeleteAlarmIndex]) { // time arrive
        var switchValue = this.switchValue;
        this.switchStatus[this.dataWrapper.alarmItemIndex] = switchValue;
      }

      this.packOriginData(); // wrap current environment data as origin data
    }
  },
  transformTimeToTwelveHour(time) {
    var timeArray = time.split(" ")[0].split(":");
    var hour = timeArray[0];
    var minute = timeArray[1];
    if (minute.length === 1) {
      minute = "0" + minute;
    }
    var onTheHour = 12;
    if (hour == "00") {
      hour = onTheHour;
      return hour + ":" + minute + " 上午";
    }
    if (Number(hour) == onTheHour) {
      return hour + ":" + minute + " 下午";
    }
    if (Number(hour) > onTheHour) {
      hour = Number(hour) - onTheHour;
      return hour + ":" + minute + " 下午";
    } else {
      return hour + ":" + minute + " 上午";
    }
  },
  addAlarm() {
    var date = new Date();
    var strHour = date.getHours();
    var strMin = date.getMinutes();
    this.dataWrapper.mode = this.mode[0];
    this.dataWrapper.time = strHour + ":" + strMin;
    router.replace({
      uri: 'pages/editAlarm/edit_alarm',
      params: {
        dataWrapper: this.dataWrapper,
        originData: this.originData
      }
    });
    clearInterval(this.interValId);
  },
  switchToEditAlarm(index) {
    this.dataWrapper.mode = this.mode[1];
    this.dataWrapper.time = this.time[index];
    this.dataWrapper.repeat = this.repeat[index];
    this.dataWrapper.alarmItemIndex = index;
    router.replace({
      uri: 'pages/editAlarm/edit_alarm',
      params: {
        dataWrapper: this.dataWrapper,
        originData: this.originData
      }
    });
    clearInterval(this.interValId);
  },
  switchChange(index, e) {
    if (e.checked) {
      var modeDeleteAlarmIndex = 3;
      var countDown = 2000;
      this.dataWrapper.time = this.time[index];
      this.dataWrapper.mode = this.mode[modeDeleteAlarmIndex];
      this.dataWrapper.alarmItemIndex = index;
      var dataWrapper = this.dataWrapper;
      var oriData = this.originData;
      setTimeout(function() {
        router.replace({
          uri: 'pages/timeArrive/time_arrive',
          params: {
            dataWrapper: dataWrapper,
            originData: oriData
          }
        });
      }, countDown);
    }
    clearInterval(this.interValId);
  },
  back() {
    app.terminate();
  },
  setCurTime() {
    var date = new Date();
    var strHour = date.getHours() + "";
    var strMin = date.getMinutes() + "";
    var strSecond = date.getSeconds() + "";
    var strDay = date.getDay() + "";
    if (strHour.length == 1) {
      strHour = "0" + strHour;
    }
    if (strMin.length == 1) {
      strMin = "0" + strMin;
    }
    if (strSecond.length == 1) {
      strSecond = "0" + strSecond;
    }
    this.curTime = strHour + " : " + strMin + " : " + strSecond;
    this.curDay = "星期" + this.weeks[Number(strDay) - 1];
  },
  startTimer: function() {
    var intervalTime = 1000;
    this.setCurTime();
    this.interValId = setInterval(() => {
      this.setCurTime();
    }, intervalTime);
  }
}