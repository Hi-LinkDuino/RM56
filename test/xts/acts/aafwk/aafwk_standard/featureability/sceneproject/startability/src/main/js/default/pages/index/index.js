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
    console.debug("====>Publish CallBack ACTS_StartAbility_0100_CommonEvent====>");
}
function PublishCallBackTwo() {
    console.debug("====>Publish CallBack ACTS_StartAbility_0200_CommonEvent====>");
}
function PublishCallBackThree() {
    console.debug("====>Publish CallBack ACTS_StartAbility_0300_CommonEvent====>");
}
function PublishCallBackFour() {
    console.debug("====>Publish CallBack ACTS_StartAbility_0400_CommonEvent====>");
}
function PublishCallBackFive() {
    console.debug("====>Publish CallBack ACTS_StartAbility_0500_CommonEvent====>");
}
function PublishCallBackSix() {
    console.debug("====>Publish CallBack ACTS_StartAbility_0600_CommonEvent====>");
}
function PublishCallBackTen() {
    console.debug("====>Publish CallBack ACTS_StartAbility_1000_CommonEvent====>");
}
function PublishCallBackThirteen() {
    console.debug("====>Publish CallBack ACTS_StartAbility_1300_CommonEvent====>");
}

export default {
    data: {
        title: "StartAbility"
    },
    onInit() {
        this.title = "StartAbility";
    },
    onShow() {
        console.debug('==========onShow Called==========')
        commonEvent.publish("ACTS_StartAbility_0100_CommonEvent", PublishCallBackOne);
        commonEvent.publish("ACTS_StartAbility_0200_CommonEvent", PublishCallBackTwo);
        commonEvent.publish("ACTS_StartAbility_0300_CommonEvent", PublishCallBackThree);
        commonEvent.publish("ACTS_StartAbility_0400_CommonEvent", PublishCallBackFour);
        commonEvent.publish("ACTS_StartAbility_0500_CommonEvent", PublishCallBackFive);
        commonEvent.publish("ACTS_StartAbility_0600_CommonEvent", PublishCallBackSix);
        commonEvent.publish("ACTS_StartAbility_1000_CommonEvent", PublishCallBackTen);
        commonEvent.publish("ACTS_StartAbility_1300_CommonEvent", PublishCallBackThirteen);
        featureAbility.terminateSelf((err, data)=>{
            console.debug('==========Terminate Ability Success==========')
        });
    },
    onReady() {
    },
}
