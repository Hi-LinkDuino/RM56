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
  data: {
    text: "Hello JS, we are friends.",
    speed: 1,
    flag: true
  },
  changeSpeed() {
    if (this.flag) {
      this.speed = 5;
      this.flag = false;
    } else {
      this.speed = 50;
      this.flag = true;
    }
  },
  backMain:backPage().backMain,
  changeNext:routePage("pages/marquee/marquee2/marquee2").changePage
};