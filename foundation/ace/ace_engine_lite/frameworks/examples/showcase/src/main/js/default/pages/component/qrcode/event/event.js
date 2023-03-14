/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
import { goPage, routePage } from "../../../../common/js/general";
export default {
  data: {
    qr_color: ""
  },
  ...goPage("pages/component/qrcode/index/index"),
  clickEvent() {
    this.qr_color = "#fa8072";
  },
  longpressEvent() {
    this.qr_color = "#87ceeb";
  },
  swipeEvent(e) {
    this.qr_color = '#50bf1f';
  }
};
