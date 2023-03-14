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

import prompt from '@system.prompt';
export default {
    data: {
        direction: 'column',
        list: []
    },
    onInit() {
        this.list = []
        this.listAdd = []
        for (var i = 1; i <= 7; i++) {
            var dataItem = {
                value: 'GROUP' + i,
            };
            this.list.push(dataItem);
        }
    },
    collapseOne(e) {
        this.$element('mylist').collapseGroup({
            groupid: 'GROUP1'
        })
    },
    expandOne(e) {
        this.$element('mylist').expandGroup({
            groupid: 'GROUP1'
        })
    },
    collapseAll(e) {
        this.$element('mylist').collapseGroup()
    },
    expandAll(e) {
        this.$element('mylist').expandGroup()
    },
    collapse(e) {
        prompt.showToast({
            message: 'Close ' + e.groupid
        })
    },
    expand(e) {
        prompt.showToast({
            message: 'Open ' + e.groupid
        })
    }
}
