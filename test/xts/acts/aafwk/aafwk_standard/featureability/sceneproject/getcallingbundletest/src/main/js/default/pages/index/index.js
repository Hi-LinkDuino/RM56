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
import commonEvent from '@ohos.commonEvent'
import featureAbility from '@ohos.ability.featureAbility'

const injectRef = Object.getPrototypeOf(global) || global
injectRef.regeneratorRuntime = require('@babel/runtime/regenerator')

function PublishCallBackOne() {
    console.debug("====>Publish CallBack ACTS_GetCallingBundle_0100_CommonEvent====>");
}
function PublishCallBackTwo() {
    console.debug("====>Publish CallBack ACTS_GetCallingBundle_0100_Promise====>");
}
function PublishCallBackThree() {
    console.debug("====>Publish CallBack ACTS_GetCallingBundle_0100_Callback====>");
}
export default {
    data: {
        title: ''
    },
    onInit() {
        this.title = this.$t('strings.world');
    },
    async onShow() {
        commonEvent.publish("ACTS_GetCallingBundle_0100_CommonEvent", PublishCallBackOne);
        let context = featureAbility.getContext();
        let info = await context.getCallingBundle();
        commonEvent.publish(info + ".promise", PublishCallBackTwo);
        context.getCallingBundle(
            (err, data) => {
                console.debug("getCallingBundle : " + data)
                commonEvent.publish(data + ".callback", PublishCallBackThree);
            }
        );
    },
    onReady() {
        console.info('onReady');
    },
}
