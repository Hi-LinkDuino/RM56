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
    console.debug("====>Publish CallBack ACTS_TerminateAbility_0100_CommonEvent====>");
}
function PublishCallBackTwo() {
    console.debug("====>Publish CallBack ACTS_TerminateAbility_0200_CommonEvent====>");
}
function PublishCallBackThree() {
    console.debug("====>Publish CallBack ACTS_Test_TerminateAbility_0100_Return====>");
}
function PublishCallBackFour() {
    console.debug("====>Publish CallBack ACTS_Test_TerminateAbility_0200_Return====>");
}
export default {
    data: {
        title: ''
    },
    onInit() {
        this.title = this.$t('strings.world');
    },
    onShow() {
        commonEvent.publish("ACTS_TerminateAbility_0100_CommonEvent", PublishCallBackOne);
        commonEvent.publish("ACTS_TerminateAbility_0200_CommonEvent", PublishCallBackTwo);
        featureAbility.terminateSelf(
            async (err, data)=>{
                console.debug('==========Terminate Ability Success==========');
                await commonEvent.publish("ACTS_TerminateAbility_0100_Return", PublishCallBackThree);
                await commonEvent.publish("ACTS_TerminateAbility_0200_Return", PublishCallBackFour);
            }
        );
    },
    onReady() {
        console.info('onReady');
    },
}
