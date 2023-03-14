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

import router from "@system.router"

export default {
    data : {
        width : 80,
        height : 90
    },
    changeArea : function() {
        if (this.width == 80) {
            this.width = 40;
        } else {
            this.width = 80;
        }
        if (this.height == 90) {
            this.height = 50;
        } else {
            this.height = 90;
        }
    },
    goHome : function() {
        router.replace({uri : "pages/index/index"});
    },
    changeNext : function() {
        router.replace({uri : "pages/switch-radio-width/width_radio_value_test010/index"});
    }
}