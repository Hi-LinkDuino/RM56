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
import {routePage} from "../../common/js/general"
var progresses = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100];
export default {
  data: {
    text: 100,
    progress: 100,
    index: 0
  },
  updateProgress() {
    this.progress = progresses[this.index % 10];
    this.text = this.progress;
    this.index++;
  },
  onInit() {
    console.log("init called...");
  },
  longPressTest: function() {
    console.log("long press trigger");
  },
  ...routePage('pages/index/index', 'pages/index/index')
};
