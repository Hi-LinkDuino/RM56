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

import {backPage, routePage, lifeCycleFunc} from "../../../common/js/general";

export default {
  data: {
    color: "#5265ef",
    flag: false,
    msg: "key event"
  },
  keyEvent(e) {
    console.log("key start");
    if (this.flag) {
      this.color = 16766720;
      this.flag = false;
      this.msg = "color is 16766720";
    } else {
      this.color = 32768;
      this.flag = true;
      this.msg = "color is 32768";
    }
    console.log("key stop");
  },
  ...lifeCycleFunc(),
  backMain:backPage().backMain,
  changePrev:routePage("pages/event/end/index").changePage
};
