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
import { backPage, routePage, lifeCycleFunc } from "../../../common/js/general";

export default {
  data: {
    msg: ""
  },
  makeScreenshot() {
    if (dfx.screenshotToFile()) {
      this.msg = "success";
      console.log("screen success");
    } else {
      this.msg = "fail";
      console.log("screen fail");
    }
  },
  ...lifeCycleFunc(),
  backMain: backPage().backMain,
  changePrev: routePage("pages/dfx/dfx6/index").changePage,
  changeNext: routePage("pages/dfx/assist1/index").changePage
};
