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
import { datasetsFor272829, appendDataFunc } from "../../../common/js/chart";
export default {
  data: {
    datasets: datasetsFor272829,
    options: {
      xAxis: {
        min: 2,
        max: 6,
        axisTick: 11,
        display: true,
        color: "#00ff00"
      },
      yAxis: {
        min: 126,
        max: 500,
        axisTick: 11,
        display: true,
        color: "#ff0000"
      }
    },
    index: 0,
    sourceData: [62, 60, 58, 57, 56, 56, 56],
    updateData: function updateData() {
      var appendData = appendDataFunc();
      this.$refs.chart.append(appendData);
    },
    ...routePage("pages/chart/26/index", "pages/chart/28/index")
  }
};