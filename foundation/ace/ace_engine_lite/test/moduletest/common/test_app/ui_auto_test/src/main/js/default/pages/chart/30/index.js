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
    datasets1: [
      {
        fillColor: "#00ff00",
        data: [300, 250, 400, 150, 350, 212, 300, 222, 190],
        gradient: false
      },
      {
        strokeColor: "#FBD140",
        fillColor: "#0000ff",
        data: [300, 300, 178, 50, 401, 126, 600, 80],
        gradient: true
      }
    ],
    options1: {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        headPoint: {
          shape: "circle",
          size: "5px",
          strokeColor: "#ff0000",
          fillColor: "#ff0000"
        },
        bottomPoint: {
          shape: "circle",
          size: "3px",
          strokeColor: "#FEFEFE",
          fillColor: "#A1928F"
        },
        topPoint: {
          size: "10px",
          strokeWidth: "3px",
          strokeColor: "#FEFEFE",
          fillColor: "#007DFF"
        }
      }
    },
    ...routePage("pages/chart/28/index", "")
  },
  changeParam() {
    this.datasets1 = [
      {
        fillColor: "#00ff00",
        data: [280, 230, 380, 130, 330, 192, 280, 202, 170],
        gradient: false
      },
      {
        strokeColor: "#FBD140",
        fillColor: "#0000ff",
        data: [280, 280, 158, 30, 381, 106, 580, 60],
        gradient: false
      }
    ],
    this.options1 = {
      xAxis: xAxis,
      yAxis: yAxis,
      series: {
        headPoint: {
          display: false,
          shape: "circle",
          size: "5px",
          strokeColor: "#ff0000",
          fillColor: "#ffffff"
        },
        loop: {

        }
      }
    }
  },
  ...routePage("pages/chart/29/index", "")
};