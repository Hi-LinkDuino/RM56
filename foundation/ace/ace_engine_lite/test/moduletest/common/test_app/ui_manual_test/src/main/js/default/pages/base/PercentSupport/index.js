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
import router from '@system.router';

export default {
    data: {
        title: 'World',
        listData: [
            "返回首页 1",
            "返回首页 2",
            "返回首页 3",
            "返回首页 4",
            "返回首页 5",
            "返回首页 6",
            "返回首页 7",
            "返回首页 8",
            "返回首页 9",
            "返回首页 10"
        ],
        stackClickCount: 0,
        stackWidth: "50%",
        stackHeight: "30%",
        topValue: "30%",
        leftValue: "0px",
        topLeftClickCount: 0,
        marginValue: "8.5%",
        marginClickCount: 0
    },
    changePercentToPercent() {
        this.topLeftClickCount++;
        if (this.topLeftClickCount % 2 == 0) {
            this.stackWidth = "80%";
            this.stackHeight = "40%";
        } else {
            this.stackWidth = "50%";
            this.stackHeight = "30%";
        }
    },
    changePercentToPX() {
        this.topLeftClickCount++;
        if (this.topLeftClickCount % 2 == 0) {
            this.topValue = "0px";
            this.leftValue = "30%";
        } else {
            this.topValue = "30%";
            this.leftValue = "0px";
        }
    },
    changeMargin() {
        this.marginClickCount++;
        if (this.marginClickCount % 2 == 0) {
            this.marginValue = "8.5%";
        } else {
            this.marginValue = "10px";
        }
    },
    returnIndex() {
        router.replace({ uri:"pages/index/index" });
    }
}
