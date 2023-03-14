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
import commonEvent from '@ohos.commonevent'

const injectRef = Object.getPrototypeOf(global) || global
injectRef.regeneratorRuntime = require('@babel/runtime/regenerator')

function onReceivedPublishCallBack(){
    console.log("========Publish CallBack UpdateLanguageFinish========");
}


export default {
    data: {
        title: ""
    },
    onInit() {
        console.info('ACTS_ProcessManage SystemAppB onInit');
        this.title = "ACTS_ProcessManage SystemAppB";
    },
    onReady() {
        console.info('ACTS_ProcessManage SystemAppB onReady');
        commonEvent.publish('UpdateLanguageFinish',onReceivedPublishCallBack);
    },
    onActive() {
        console.info('ACTS_ProcessManage SystemAppB onActive');
    },
    onShow() {
        console.info('ACTS_ProcessManage SystemAppB onShow');
    },

    onDestroy() {
        console.info('ACTS_ProcessManage SystemAppB onDestroy');
    },
    onConfigurationUpdated(config) {
        console.log("Acts_LanguageConfigAppA onConfigurationUpdated called" + JSON.stringify(config));
        console.log("Acts_LanguageConfigAppA  configuration:" +
        JSON.stringify(globalThis.abilityContext.configuration));

    },
    onCompleteContinuation(code) {
        console.info('ACTS_ProcessManage SystemAppB onCompleteContinuation:' + JSON.stringify(code));
    },
    onConfigurationUpdated(configuration) {
        console.info('ACTS_ProcessManage SystemAppB onConfigurationUpdated:' + JSON.stringify(configuration));
    }
}