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
import { datasetsFor272829 } from "../../../common/js/chart";
var yAxis = {
  min: 0,
  max: 450,
  display: true,
  color: "#ff0000"
};
export default {
  data: {
    datasets: datasetsFor272829,
    options1: {
      xAxis: {
        min: -1,
        max: 6,
        display: true,
        color: "#00ff00"
      },
      yAxis: yAxis
    },
    options2: {
      xAxis: {
        min: 5,
        max: 1,
        display: true,
        color: "#00ff00"
      },
      yAxis: yAxis
    },
    options3: {
      xAxis: {
        min: 5,
        max: -1,
        display: true,
        color: "#00ff00"
      },
      yAxis: yAxis
    },
    options4: {
      xAxis: {
        display: true,
        color: "#00ff00"
      },
      yAxis: yAxis
    },
    ...routePage("pages/chart/27/index", "pages/chart/29/index")
  }
};