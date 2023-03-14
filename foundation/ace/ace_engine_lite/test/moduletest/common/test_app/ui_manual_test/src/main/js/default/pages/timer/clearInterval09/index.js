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
import {backPage,routePage} from "../../../common/js/general";

export default {
    data:{
        value:"show"
    },
    testMinTime:function() {
        setTimeout(()=>{this.value = "min"},-2);
    },
    testMaxTime:function() {
        setTimeout(()=>{this.value = "max"},4294967297);
    },
    testMinInterval:function() {
        var id = setInterval(()=>{this.value = "min"; clearInterval(id);},-1);
    },
    testMaxInterval:function() {
        var id = setInterval(()=>{this.value = "max", clearInterval(id);},4294967295);
    },
    goPrevious:routePage("pages/timer/clearInterval08/index").changePage,
    goHome:routePage("/").changePage,
    goNext:routePage("/pages/timer/timerLimit10/index").changePage
}