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
import {backPage,routePage} from "../../../common/js/general";
import {addData} from "../../../common/js/list"
export default {
  data: {
    listData: [
      "Item0",
      "Item1",
      "Item2",
      "Item3"
    ]
  },
  backMain:backPage().backMain,
  prePage:routePage("pages/list/list3/list3").changePage,
  nextPage:routePage("pages/list/list7/list7").changePage,
  back:routePage("pages/list/list5/list5").changePage,
  scrollEnd(endState, endComponentIndex) {
  },
  onInit() {
    console.log("on init called...");
  }
};
