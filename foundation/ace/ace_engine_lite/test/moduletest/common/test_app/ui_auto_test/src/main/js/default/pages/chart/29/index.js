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
var xAxis = {
  min: 0,
  max: 20,
  display: true,
  color: "#00ff00"
};
export default {
  data: {
    datasets: datasetsFor272829,
    options1: {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        headPoint: {
          shape: "circle",
          size: "-1px"
        }
      }
    },
    options2: {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        bottomPoint: {
          shape: "aaa",
          size: "3px",
          strokeWidth: "5px"
        }
      }
    },
    options3: {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        topPoint: {
          shape: "circle",
          size: "10px",
          strokeColor: "#0000ff",
          fillColor: "#0xff00",
          strokeWidth: "5px"
        }
      }
    },
    ...routePage("pages/chart/28/index", "pages/chart/30/index")
  }
};