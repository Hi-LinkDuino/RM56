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
import {routePage} from "../../../common/js/general";
export default {
  data: {
    list: ["item1", "item2"]
  },
  ...routePage("pages/directive/01/index", "pages/directive/03/index"),
  handleIncrease() {
    this.list.push("item" + (this.list.length + 1));
  },
  handleDecrease() {
    this.list.splice(this.list.length - 1, 1);
  }
};