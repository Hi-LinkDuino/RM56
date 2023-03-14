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
export const lifeCycleFunc = function() {
  return {
    onInit: function() {
      console.log("onInit called...");
    },
    onDestroy: function() {
      console.log("onDestroy called...");
    },
    onReady: function() {
      console.log("onReady called...");
    },
    onShow: function() {
      console.log("onShow called...");
    }
  };
};
export const getText = function(e) {
  console.log("getText called: new selected=" + e.newSelected);
  this.textValue = e.newValue;
};
export const getTime = function(e) {
  console.log("getTime called:hour=" + e.hour + " minute=" + e.minute);
  this.textValue = e.hour + ":" + e.minute;
};
export const data = {
  textValue: "time",
  rangeList: ["Aa", "Bb", "Cc", "Dd", "Ee", "Ff", "Gg", "Hh", "Ii"]
};