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
        ifStatus:true,
        showStatus:true,
        data:["data1","data2"]
    },
    changeIf:function() {
        if(this.ifStatus) {
            this.ifStatus = false;
        } else {
            this.ifStatus = true;
        }
        console.log("ifStatus:" + this.ifStatus);
    },
    changeStatus:function() {
        if(this.showStatus) {
            this.showStatus = false;
        } else {
            this.showStatus = true;
        }
        console.log("showStatus:"+this.showStatus);
    },
    changeFor:function() {
        if(this.data.length == 2) {
            this.data = ["data1"];
        } else  {
            this.data = ["data1", "data2"];
        }
    },
    ...goPage("pages/input/CheckboxE027/index", "pages/input/CheckboxA029/index")
}