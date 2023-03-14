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
    options: {
      xAxis: {
        min: 0,
        max: 5,
        axisTick: 6,
        display: true
      },
      yAxis: {
        min: 0,
        max: 5,
        axisTick: 4,
        display: true
      },
      labels: ["00:00", "02:00", "04:00", "06:00"]
    },
    datasets1: [
      {
        backgroundColor: "#000000",
        data: [1, 2, 3, 4]
      }
    ],
    datasets2: [
      {
        backgroundColor: "#ff0000",
        data: [1, 2, 3, 4]
      }
    ],
    datasets3: [
      {
        backgroundColor: "ff0000",
        data: [1, 2, 3, 4]
      }
    ],
    datasets4: [
      {
        backgroundColor: 21474836483,
        data: [1, 2, 3, 4]
      }
    ],
    ...routePage("pages/index/index", "pages/chart/0506/index")
  }
};