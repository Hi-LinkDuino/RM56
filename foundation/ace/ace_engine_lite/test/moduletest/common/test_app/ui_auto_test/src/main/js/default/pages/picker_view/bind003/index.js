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
import {lifeCycleFunc, getTime, getText} from "../../../common/js/picker_view"
export default {
  data: {
    textValue: "time",
    selectedTime: "0:0"
  },
  getText: getText,
  getTime: getTime,
  ...routePage("pages/picker_view/bind002/index", "pages/picker_view/bind004/index"),
  changeAttrs() {
    this.selectedTime = "11:11";
  },
  ...lifeCycleFunc
};