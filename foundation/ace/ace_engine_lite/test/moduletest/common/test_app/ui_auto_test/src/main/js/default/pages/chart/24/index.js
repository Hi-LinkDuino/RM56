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
import { appendDataFunc, datasetsFor232426 } from "../../../common/js/chart"
var xAxis = {
  min: 0,
  max: 50,
  display: true,
  color: "#00ff00"
};
var yAxis = {
  min: 0,
  max: 450,
  display: true,
  color: "#ff0000"
};
export default {
  data: {
    datasets: datasetsFor232426,
    options1: {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        loop: {
          margin: 5
        }
      }
    },
    options2: {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        loop: {
          margin: 0
        }
      }
    },
    options3: {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        loop: {
          margin: -1
        }
      }
    },
    options4: {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        loop: {}
      }
    },
    index: 0,
    sourceData: [62, 60, 58, 57, 56, 56, 56],
    updateData: function updateData(count) {
      var appendData = {
        serial: 0,
        data: appendDataFunc().data
      };
      if (count == 1) {
        this.$refs.chart1.append(appendData);
      } else if (count == 2) {
        this.$refs.chart2.append(appendData);
      } else if (count == 3) {
        this.$refs.chart3.append(appendData);
      } else if (count == 4) {
        this.$refs.chart4.append(appendData);
      }
    },
    ...routePage("pages/chart/23/index", "pages/chart/25/index")
  }
};