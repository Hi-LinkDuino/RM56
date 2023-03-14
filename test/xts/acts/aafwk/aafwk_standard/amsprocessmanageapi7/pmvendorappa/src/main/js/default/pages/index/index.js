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
import abilityManager from '@ohos.application.appManager'
import featureAbility from '@ohos.ability.featureability'

const injectRef = Object.getPrototypeOf(global) || global
injectRef.regeneratorRuntime = require('@babel/runtime/regenerator')

var publishEventName = "event_VendorAppAToSystemAppA";
var publishEventNameVendor = "event_from_ProcessManage_VendorAppA";
var testAction;
var testTarget;

function onReceivedPublishCallBack() {
    console.log("ACTS_ProcessManage VendorAppA onReceivedPublishCallBack");
}

function onReceivedHandleCallback(err, data) {
    console.log("ACTS_ProcessManage VendorAppA onReceivedHandleCallback JSON.stringify data:"
    + JSON.stringify(data));
    console.log("ACTS_ProcessManage VendorAppA onReceivedHandleCallback data:"
    + data);
    console.log("ACTS_ProcessManage VendorAppA onReceivedHandleCallback err:"
    + err);
    commonEvent.publish(publishEventNameVendor,onReceivedPublishCallBack);
}

export default {
    data: {
        title: ""
    },
    onInit() {
        console.info('ACTS_ProcessManage VendorAppA onInit');
        this.title = "ACTS_ProcessManage VendorAppA";
    },
    async onReady() {
        console.info('ACTS_ProcessManage VendorAppA onReady');
        var abilityWant = await featureAbility.getWant();
        console.log("ACTS_ProcessManage VendorAppA onShow abilityWant:"
            + JSON.stringify(abilityWant));
        testAction = abilityWant.action;
        console.log("ACTS_ProcessManage VendorAppA onShow testAction:" + testAction);
        testTarget = abilityWant.parameters.testTarget;
        console.log("ACTS_ProcessManage VendorAppA onShow testTarget:" + testTarget);
        if (testAction == 'thirdpartykill') {
            console.log("ACTS_ProcessManage VendorAppA onForeground testAction:" + testAction);
            appManager.killProcessesByBundleName(testTarget, onReceivedHandleCallback);
        }
        if (testAction == 'thirdpartyclear') {
            console.log("ACTS_ProcessManage VendorAppA onForeground testAction:" + testAction);
            appManager.clearUpApplicationData(testTarget, onReceivedHandleCallback);
        }
        else{
            commonEvent.publish(publishEventName, onReceivedPublishCallBack);
        }
        
    },
    onActive() {
        console.info('ACTS_ProcessManage VendorAppA onActive');
    },
    onShow() {
        console.info('ACTS_ProcessManage VendorAppA onShow');
    },
    onInactive() {
        console.info('ACTS_ProcessManage VendorAppA onInactive');
    },
    onHide() {
        console.info('ACTS_ProcessManage VendorAppA onHide');
    },
    onDestroy() {
        console.info('ACTS_ProcessManage VendorAppA onDestroy');
    },
    onBackPress() {
        console.info('ACTS_ProcessManage VendorAppA onBackPress');
    },
    onNewRequest() {
        console.info('ACTS_ProcessManage VendorAppA onNewRequest');
    },
    onStartContinuation() {
        console.info('ACTS_ProcessManage VendorAppA onStartContinuation');
    },
    onSaveData(value) {
        console.info('ACTS_ProcessManage VendorAppA onSaveData:' + JSON.stringify(value));
    },
    onRestoreData(value) {
        console.info('ACTS_ProcessManage VendorAppA onRestoreData:' + JSON.stringify(value));
    },
    onCompleteContinuation(code) {
        console.info('ACTS_ProcessManage VendorAppA onCompleteContinuation:' + JSON.stringify(code));
    },
    onConfigurationUpdated(configuration) {
        console.info('ACTS_ProcessManage VendorAppA onConfigurationUpdated:' + JSON.stringify(configuration));
    }
}
