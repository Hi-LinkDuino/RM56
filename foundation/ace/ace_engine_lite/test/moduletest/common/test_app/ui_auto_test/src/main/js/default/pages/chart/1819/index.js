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
import { routePage } from "../../../common/js/general";
export default {
  data: {
    labels: ["00:00", "02:00", "04:00", "06:00"],
    options: {
      xAxis: {
        min: 0,
        max: 10,
        axisTick: 6,
        display: true
      },
      yAxis: {
        min: 0,
        max: 6000,
        axisTick: 10,
        display: false
      }
    },
    datasets1: [
      {
        strokeColor: "#ff0000",
        data: [2478, 2600, 3e3, 3200, 3500, 3888, 4023, 4501, 4999, 5267]
      }
    ],
    datasets2: [
      {
        backgroundColor: "#00ff00",
        data: [1, 2, 3],
        dgs: "jdkllk"
      }
    ],
    datasets3: [
      {
        backgroundColor: "#ff0000"
      }
    ],
    datasets4: "not array",
    ...routePage("pages/chart/0506/index", "pages/chart/20/index")
  }
};