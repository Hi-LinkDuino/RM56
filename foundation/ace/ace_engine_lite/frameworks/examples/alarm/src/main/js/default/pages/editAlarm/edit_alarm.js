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

export default {
  data: {
    selectedTime: "00:00",
    targetHour: "00",
    targetMinute: "00",
    isSelectTimeChanged: false,
    weekImgSrcOn: "common/ic_clock_btn_on.png",
    weekImgSrcOff: "common/ic_clock_btn_off.png",
    weekOnList: [false, false, false, false, false, false, false],
    weekImgSrcList: ["common/ic_clock_btn_off.png", "common/ic_clock_btn_off.png",
      "common/ic_clock_btn_off.png", "common/ic_clock_btn_off.png",
      "common/ic_clock_btn_off.png", "common/ic_clock_btn_off.png", "common/ic_clock_btn_off.png"],
    weekList: ["日", "一", "二", "三", "四", "五", "六"],
    dataWrapper: {
      mode: "",
      time: "00:00",
      repeat: "不重复",
      alarmItemIndex: -1
    },
    originData: [],
    isDebug: false,
    buttonInfo: "删除闹钟",
    repeatBeforeModify: ""
  },
  onInit() {
    if (this.isDebug) {
      console.log("[editAlarm init] mode: " + this.dataWrapper.mode);
      console.log("[editAlarm init] mode: " + this.dataWrapper.time);
      console.log("[editAlarm init] mode: " + this.dataWrapper.repeat);
      console.log("[editAlarm init] mode: " + this.dataWrapper.alarmItemIndex);
      console.log("[weekSelector init] mode: " + this.dataWrapper.mode);
      console.log("[weekSelector init] time: " + this.dataWrapper.time);
      console.log("[weekSelector init] repeat: " + this.dataWrapper.repeat);
      console.log("[weekSelector init] alarmItemIndex: " + this.dataWrapper.alarmItemIndex);
    }
    if (this.dataWrapper.mode === "addAlarm") {
      this.buttonInfo = "取消";
    }

    this.transforTimeToTwentyFourHour(this.dataWrapper.time);

    if (this.dataWrapper.mode === "addAlarm") {
      for (var i = 0; i < this.weekList.length; i++) {
        this.weekOnList[i] = false;
        this.weekImgSrcList[i] = this.weekImgSrcOff;
      }
    } else {
      for (var i = 0; i < this.weekList.length; i++) {
        if (this.dataWrapper.repeat.indexOf(this.weekList[i]) !== -1) {
          this.weekOnList[i] = true;
          this.weekImgSrcList[i] = this.weekImgSrcOn;
        }
      }
    }

    this.repeatBeforeModify = this.dataWrapper.repeat;
    this.dataWrapper.repeat = "";

    if (this.originData.toString() !== "undefined") {
      this.originData = this.originData;
    }
  },
  transforTimeToTwentyFourHour(time) {
    var timeArray = time.split(" ");
    var timeDigital = timeArray[0];
    var timeAMPM = timeArray[1];

    var hourTimeArray = timeDigital.split(":");
    var hour = hourTimeArray[0];
    var minute = hourTimeArray[1];

    var onTheHour = 12;
    if (timeAMPM === "下午") {
      if (Number(hour) != onTheHour) {
        hour = Number(hour) + onTheHour;
      }
      this.dataWrapper.time = hour + ":" + minute;
    } else {
      if (Number(hour) == onTheHour) {
        hour = "00";
      }
      this.dataWrapper.time = hour + ":" + minute;
    }
  },
  changeWeekSelected(index) {
    this.weekOnList[index] = this.weekOnList[index] == false ? true : false;
    this.weekImgSrcList[index] = this.weekOnList[index] == false ? this.weekImgSrcOff : this.weekImgSrcOn;
  },
  getSelectedTime(e) {
    this.targetHour = e.hour;
    this.targetMinute = e.minute;
    this.isSelectTimeChanged = true;
  },
  submit() {
    if (this.isSelectTimeChanged) {
      this.targetHour = this.targetHour.toString().length === 1 ? "0" + this.targetHour : this.targetHour;
      this.targetMinute = this.targetMinute.toString().length === 1 ? "0" + this.targetMinute : this.targetMinute;
      this.dataWrapper.time = this.targetHour + ":" + this.targetMinute + " 上午";
    }

    if (this.weekOnList[0] == true) {
      this.dataWrapper.repeat = this.weekList[0];
    }
    for (var index = 1; index < this.weekOnList.length; index++) {
      if (this.weekOnList[index] == true) {
        this.dataWrapper.repeat = this.dataWrapper.repeat + " " + this.weekList[index];
      }
    }
    if (this.dataWrapper.repeat === "") {
      this.dataWrapper.repeat = "不重复";
    }
    router.replace({
      uri: 'pages/index/index',
      params: {
        dataWrapper: this.dataWrapper,
        originData: this.originData
      }
    });
  },
  remove() {
    if (this.dataWrapper.mode === "editAlarm") {
      router.replace({
        uri: 'pages/index/index',
        params: {
          dataWrapper: this.dataWrapper,
          deleteAlarm: true,
          originData: this.originData
        }
      });
    } else if (this.dataWrapper.mode === "addAlarm") {
      router.replace({
        uri: 'pages/index/index',
        params: {
          dataWrapper: this.dataWrapper,
          cancelAlarm: true,
          originData: this.originData
        }
      });
    }
  },
  back() {
    this.dataWrapper.repeat = this.repeatBeforeModify;
    if (this.dataWrapper.mode === "addAlarm") {
      router.replace({
        uri: 'pages/index/index',
        params: {
          dataWrapper: this.dataWrapper,
          cancelAlarm: true,
          originData: this.originData
        }
      });
    } else {
      router.replace({
        uri: 'pages/index/index',
        params: {
          dataWrapper: this.dataWrapper,
          originData: this.originData
        }
      });
    }
  }
}