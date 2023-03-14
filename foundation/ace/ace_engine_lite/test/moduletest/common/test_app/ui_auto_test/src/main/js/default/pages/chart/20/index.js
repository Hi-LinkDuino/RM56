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
            flag:true,
            options:{
                xAxis: {
                    min: 0,
                    max: 10,
                    axisTick: 10,
                    display: false
                },
                yAxis: {
                    min: 2000,
                    max: 6000,
                    axisTick: 10,
                    display: true
                }
            },
            datasets:[{
                fillColor:"#ff0000",
                data:[2478, 2600, 3e3, 3200, 3500, 3888, 4023, 4501, 4999, 5267]
            }
            ]
        },
        changeData() {
            if (this.flag === true) {
                this.datasets = [{
                    fillColor:"#00ff00",
                    data:[2000, 3000, 4000],
                    dgs:"jdkllk"
                },{
                    fillColor:"#0000ff",
                    data:[2478, 2600, 3e3, 3200, 3500, 3888, 4023, 4501, 4999, 5267]
                }
                ];
            } else {
                this.datasets = [{
                    fillColor:"#ff0000",
                    data:[2478, 2600, 3e3, 3200, 3500, 3888, 4023, 4501, 4999, 5267]
                }
                ];
            }
            this.flag = this.flag === true ? false : true;
        },
        ...routePage('pages/chart/1819/index', 'pages/chart/21/index')
    }