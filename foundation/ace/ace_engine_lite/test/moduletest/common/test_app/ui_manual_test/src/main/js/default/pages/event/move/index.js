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

import {backPage, lifeCycleFunc, routePage} from "../../../common/js/general";
import {commonFunc, dataCommon} from "../../../common/js/event";

export default {
  data: {
    ...dataCommon,
    startX: 0,
    startY: 0
  },
  touchstart(e) {
    console.log("touch  start");
    commonFunc(this, e);
    console.log("touch  stop");
  },
  touchmove(e) {
    console.log("touch move start");
    commonFunc(this, e);
    this.startX = e.touches.startX;
    this.startY = e.touches.startY;
    console.log("touch move end");
  },
  ...lifeCycleFunc(),
  backMain:backPage().backMain,
  changeNext:routePage("pages/event/cancel/index").changePage,
  changePrev:routePage("pages/event/start/index").changePage
};
