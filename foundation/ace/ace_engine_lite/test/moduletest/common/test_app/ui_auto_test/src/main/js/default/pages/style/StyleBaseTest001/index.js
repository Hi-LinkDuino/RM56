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

import { routePage, routeSimplePage } from "../../../common/js/general";
export default {
  data: {
    title: "World",
    titleIndex: 0,
    textColor: 16645886,
    title2: "World",
    title2Index: 0,
    text2Color: 16645886
  },
  exitPage: routeSimplePage("pages/index/index"),
  // change second title
  changeTitle2() {
    this.title2Index++;
    if (this.title2Index % 2 > 0) {
      this.title2 = "冲冲";
      this.text2Color = 6749952;
    } else {
      this.title2 = "你好";
      this.text2Color = 16750848;
    }
  },
  // change first title
  changeTitle() {
    this.titleIndex++;
    if (this.titleIndex % 2 > 0) {
      this.title = "JS";
      this.textColor = 13321031;
    } else {
      this.title = "世界";
      this.textColor = 16645886;
    }
  }
};
