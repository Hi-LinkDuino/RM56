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
import {routePage} from "../../../common/js/general";
export default {
  data: {
    image: "common/left.png",
    index: 0,
    datasets: [
      {
        backgroundColor: "#FF6364",
        data: [2478, 2600, 3e3, 3200, 3500, 3888, 4023, 4501, 4999, 5267]
      },
      { backgroundColor: "#00FF00", data: [1000, 2, 800, 700, 433] },
      { backgroundColor: "#0000FF", data: [100, 200, 300, 400, 500, 600, 550, 450, 350] }
    ],
    options: {
        xAxis: { min: 0, max: 10, axisTick: 10, display:true },
        yAxis: { min: 100, max: 5267, axisTick: 10, display: true }
    }
  },
  updateData: function updateData() {
    console.log("update data...");
    this.datasets = [
      { backgroundColor: "#FFFF00", data: [400, 200, 800, 400, 200, 800] },
      { backgroundColor: "#FF6364", data: [300, 200, 750] }
    ];
    this.options = {
            xAxis: { min: 0, max: 10, axisTick: 10, display:true },
            yAxis: { min: 400, max: 800, axisTick: 10, display: true }
    }
  },
  onInit() {
    console.log("init called...");
  },
  onDestroy() {
    console.log("destroy called...");
  },
  ...routePage('pages/chart/20/index', 'pages/chart/22/index')
};