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
import {datasets,xAxis,yAxis,appendDataFunc} from "../../../common/js/chart";
export default {
  data: {
    datasets: datasets,
    options: {
      xAxis: xAxis,
      yAxis: yAxis
    },
    sourceData: [50, 70, 90, 65, 50, 10, 50],
    updateData: function updateData() {
      var appendData = appendDataFunc();
      this.$refs.chart.append(appendData);
    },
    ...routePage("pages/chart/24/index", "pages/chart/26/index")
  }
};