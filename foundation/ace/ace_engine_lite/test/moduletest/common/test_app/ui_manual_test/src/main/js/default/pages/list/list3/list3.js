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

export default {
  backMain:backPage().backMain,
  prePage:routePage("").changePage,
  nextPage:routePage("pages/list/list5/list5").changePage,
  go:routePage("pages/list/list4/list4").changePage,
  scrollEnd(endState, endComponentIndex) {
    console.log("list onscrollend is triggered, end component index=" + endComponentIndex);
  },
  onInit() {
    console.log("on init called...");
  }
};
