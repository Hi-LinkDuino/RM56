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
import { datasetsFor232426, appendDataFunc } from "../../../common/js/chart";
export default {
  data: {
    datasets: datasetsFor232426,
    options: {
      xAxis: {
        min: 0,
        max: 100,
        axisTick: 10,
        display: false,
        color: "##c0c0c0"
      },
      yAxis: {
        min: 0,
        max: 450,
        axisTick: 10,
        display: true,
        color: "#ff0000"
      },
      series: {
        lineStyle: {
          width: "5",
          smooth: false
        },
        headPoint: {},
        topPoint: {},
        bottomPoint: {},
        loop: {
          margin: 5,
          gradient: false
        }
      }
    },
    index: 0,
    sourceData: [62, 60, 58, 57, 56, 56, 56],
    updateData: function updateData() {
      var appendData = appendDataFunc();
      this.$refs.chart.append(appendData);
    },
    ...routePage("pages/chart/25/index", "pages/chart/27/index")
  }
};