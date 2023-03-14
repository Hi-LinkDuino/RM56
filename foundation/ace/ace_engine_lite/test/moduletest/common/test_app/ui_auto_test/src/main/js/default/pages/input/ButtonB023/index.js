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
import {goPage} from "../../../common/js/general";
export default {
    data:{
        value:"中文",
        fontSize:30,
        fontFamily:"HYQiHei-65S",
        color:	220
    },
    changeValue:function() {
        if(this.value == "chinese") {
            this.value = "中文";
        } else {
            this.value = "chinese";
        }
    },
    changeFontSize:function() {
        if(this.fontSize == 30) {
            this.fontSize = 38;
        } else {
            this.fontSize = 30;
        }
    },
    changeFontFamily:function () {
        if(this.fontFamily == "HYQiHei-65S") {
            this.fontFamily = "";
        } else {
            this.fontFamily = "HYQiHei-65S";
        }
    },
    changeColor:function() {
        if(this.color == 220) {
            this.color = 80;
        } else {
            this.color = 220;
        }
    },
    ...goPage("pages/index/index", "pages/input/InputEventTest024/index")
}