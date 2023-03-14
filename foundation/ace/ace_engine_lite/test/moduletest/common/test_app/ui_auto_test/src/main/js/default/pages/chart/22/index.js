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
import { datasets, xAxis, yAxis,appendDataFunc } from "../../../common/js/chart";
export default {
  data: {
    datasets: datasets,
    options: {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        lineStyle: {
          width: "5",
          smooth: false
        },
        headPoint: {
          shape: "circle",
          size: "3px",
          strokeColor: "#ff0000",
          fillColor: "#090E14"
        },
        topPoint: {
          shape: "circle",
          size: "3px",
          strokeColor: "#00ff00",
          fillColor: "#090E14",
          strokeWidth: "1px"
        },
        bottomPoint: {
          shape: "circle",
          size: "3px",
          strokeColor: "#ffffff",
          fillColor: "#090E14"
        },
        loop: {
          margin: 5,
          gradient: false
        }
      }
    },
    updateData: function updateData() {
      var appendData = appendDataFunc();
      this.$refs.chart.append(appendData);
    },
    ...routePage("pages/chart/21/index", "pages/chart/23/index")
  }
};