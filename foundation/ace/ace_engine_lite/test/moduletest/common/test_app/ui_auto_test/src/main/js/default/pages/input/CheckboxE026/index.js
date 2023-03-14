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
        click:"click",
        press:"press",
        change:"change"
    },
    ...goPage("pages/input/CheckboxB025/index", "pages/input/CheckboxE027/index"),
    clickEvent:function() {
        if(this.click == "click") {
            this.click = "clicked";
        } else {
            this.click = "click";
        }
    },
    changeEvent:function(e) {
        if(e.checked) {
            this.change = "true";
        } else {
            this.change = "false";
        }
    },
    pressEvent:function() {
        if(this.press = "press") {
            this.press = "pressed";
        } else {
            this.press = "press";
        }
    }

}