/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
import {backPage,routePage} from "../../common/js/general";

export default {
    data: {
        text1: ""
    },
    clickFunc: function() {
        this.text1 = "点击事件已触发";
    },
    listswipe: function(e) {
        this.text1 = "swipe方向：" + e.direction;
        console.info(this.text1);

        this.text1 = "event Type" + e.type;
        console.info(this.text1);

        this.text1 = "TimeStamp" + String(e.timestamp);
        console.info(this.text1);
    },
    touchstart: function(e) {
        this.text1 = "touchstart is triggered";
        console.info(this.text1);

        console.log(JSON.stringify(e))

    },
    touchmove: function(e) {
        this.text1 = "touchmove is triggered";
        console.info(this.text1);

        console.log(JSON.stringify(e))
    },
    touchend: function(e) {
        this.text1 = "touchend is triggered";
        console.info(this.text1);

        console.log(JSON.stringify(e))
    },
    scrolltop: function(e) {
        this.text1 = "scrolltop is triggered";
        console.info(this.text1);
        this.text1 = "事件type:" + e.type;
        console.info(this.text1);

        this.text1 = "event Type" + e.type;
        console.info(this.text1);

        this.text1 = "TimeStamp" + String(e.timestamp);
        console.info(this.text1);
    },
    scrollbottom: function(e) {
        this.text1 = "scrollbottom is triggered";
        console.info(this.text1);

        this.text1 = "event Type" + e.type;
        console.info(this.text1);

        this.text1 = "TimeStamp" + String(e.timestamp);
        console.info(this.text1);
    }
};
