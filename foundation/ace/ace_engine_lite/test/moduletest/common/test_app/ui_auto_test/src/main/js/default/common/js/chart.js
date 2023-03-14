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
export const datasets = [
  {
    fillColor: "#00ff00",
    data: [18, 55, 64, 80, 45, 29],
    gradient: false
  },
  {
    strokeColor: "#0000FF",
    fillColor: "#00ff00",
    data: [20, 50, 30, 35, 40, 76],
    gradient: false
  }
];
export const xAxis = {
  min: 0,
  max: 100,
  axisTick: 6,
  display: true,
  color: "#00ff00"
};
export const yAxis = {
  min: 0,
  max: 100,
  axisTick: 10,
  display: true,
  color: "#ff0000"
};
export const appendDataFunc = function() {
  return {
    serial: 1,
    data: [50, 70, 90, 65, 50, 10, 50]
  };
};
export const datasetsFor232426 = [
  {
    fillColor: "#00ff00",
    data: [212, 250, 400, 150, 350, 212],
    gradient: false
  },
  {
    strokeColor: "#0000FF",
    fillColor: "#00ff00",
    data: [100, 300, 178, 205, 401, 126],
    gradient: false
  }
];
export const datasetsFor272829 = [
  {
    fillColor: "#00ff00",
    data: [212, 250, 400, 150, 350, 212, 300, 222, 190],
    gradient: false
  },
  {
    strokeColor: "#0000FF",
    fillColor: "#FFE793",
    data: [100, 300, 178, 50, 401, 126, 600, 80],
    gradient: false
  }
];