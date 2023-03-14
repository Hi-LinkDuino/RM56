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
import {commonFunc, dataCommon} from "../../../common/js/event"

export default {
  data: {
    ...dataCommon
  },
  longPressEvent(e) {
    console.log("press start");
    commonFunc(this, e);
    console.log("press stop");
  },
  ...lifeCycleFunc(),
  backMain:backPage().backMain ,
  changeNext:routePage("pages/event/start/index").changePage,
  changePrev:routePage("pages/event/click/index").changePage
};
