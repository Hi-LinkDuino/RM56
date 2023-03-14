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
        max: 5,
        axisTick: 6,
        display: true
      },
      yAxis: {
        min: 1000,
        max: 2500,
        axisTick: 4,
        display: true
      }
    },
    datasets1: [
      {
        backgroundColor: "#ff0000",
        data: [2323, 847, 2000]
      }
    ],
    datasets2: [
      {
        backgroundColor: "#00ff00",
        data: ["kjk", "jke"]
      }
    ],
    datasets3: [
      {
        backgroundColor: "#0000ff",
        data: [-2147483690, 2147483980]
      }
    ],
    ...routePage("pages/chart/0304/index", "pages/chart/1819/index")
  }
};