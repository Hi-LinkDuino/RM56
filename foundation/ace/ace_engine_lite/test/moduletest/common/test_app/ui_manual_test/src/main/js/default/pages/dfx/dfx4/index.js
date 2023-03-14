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

var dfx = require("@system.dfx");
import {backPage, routePage, lifeCycleFunc} from "../../../common/js/general";

export default {
  data: {
    color: "#5265ef"
  },
  changeGreen() {
    this.color = 32768;
  },
  changeRed() {
    this.color = 16711680;
  },
  backMain:backPage().backMain,
  changeNext:routePage("pages/dfx/dfx5/index").changePage,
  changePrev:routePage("pages/dfx/dfx3/index").changePage,
  onInit: lifeCycleFunc().onInit,
  onReady: lifeCycleFunc().onReady,
  onShow() {
    console.log("onShow called...");

    console.log("click...");
    var points = [
      [200, 250, 1],
      [200, 250, 1],
      [200, 250, 1],
      [200, 250, 1],
      [200, 250, 0]
    ];

    dfx.injectEvent(points, "point");
    console.log("click...");
  },
  onDestroy: lifeCycleFunc().onDestroy
};