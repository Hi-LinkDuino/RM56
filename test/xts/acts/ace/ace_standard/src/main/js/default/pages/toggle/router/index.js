/**
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

export default {
    data: {
        toggleList: [
            { "id":"1001", "name":"Living room", "checked":true },
            { "id":"1002", "name":"Bedroom", "checked":false },
            { "id":"1003", "name":"Second bedroom", "checked":false },
            { "id":"1004", "name":"Study", "checked":false },
        ],
        toggles: ["Living room","Bedroom","Study"],
        idx: ""
    },
    allclick(arg) {
        this.idx = arg
    },
    allchange(e) {
        if (e.checked === true) {
            for (var i = 0; i < this.toggle_list.length; i++) {
                if (this.toggleList[i].id === this.idx) {
                    this.toggleList[i].checked = true
                } else {
                    this.toggleList[i].checked = false
                }
            }
        }
    }
}