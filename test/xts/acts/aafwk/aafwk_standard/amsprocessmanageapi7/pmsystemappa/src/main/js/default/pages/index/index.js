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

var subscribeInfo = {
    events: ["event_from_ProcessManage_SystemTest",
        "event_SystemAppBToSystemAppA",
        "event_VendorAppAToSystemAppA",
        "event_SystemAppAMultiToSystemAppA",
        "event_VendorAppServiceToSystemAppA"
    ],
};
var publishEventName = "event_from_ProcessManage_SystemAppA";
var eventFromSystemAMulti = "event_SystemAppAMultiToSystemAppA";
var testAction;
var testTarget;
var subscriber;
var systemABundleName = "com.acts.pmsystemappaapi7";
var systemBBundleName = "com.acts.pmsystemappbapi7";
var vendorABundleName = "com.acts.pmvendorappaapi7";
var systemBAbilityName = "com.acts.pmsystemappbapi7.MainAbility";
var vendorAAbilityName = "com.acts.pmvendorappaapi7.MainAbility";
var multiAAbilityName = "com.acts.pmsystemappamultiapi7.MainAbility";

function onReceivedPublishCallBack() {
    console.log("ACTS_ProcessManage SystemAppA onReceivedPublishCallBack");
}

function publishResultEvent(testResult) {
    console.log("ACTS_ProcessManage SystemAppA publishResultEvent testAction:" + testAction);
    console.log("ACTS_ProcessManage SystemAppA publishResultEvent testTarget:" + testTarget);
    console.log("ACTS_ProcessManage SystemAppA publishResultEvent testResult:" + testResult);
    let publishData = { data: testResult.toString() };
    console.log("ACTS_ProcessManage SystemAppA publishResultEvent publishData:"
        + JSON.stringify(publishData));
    commonEvent.publish(publishEventName, publishData, onReceivedPublishCallBack);

}
function onReceivedHandleCallback(err, data) {
    publishResultEvent(data);
    console.log("ACTS_ProcessManage SystemAppA onReceivedHandleCallback data:"
        + JSON.stringify(data));
}

async function onShowProcess() {

    var abilityWant = await featureAbility.getWant();
    console.log("ACTS_ProcessManage SystemAppA onShowProcess abilityWant:"
        + JSON.stringify(abilityWant));
    testAction = abilityWant.action;
    console.log("ACTS_ProcessManage SystemAppA onShowProcess testAction:" + testAction);
    testTarget = abilityWant.parameters.testTarget;
    console.log("ACTS_ProcessManage SystemAppA onShowProcess testTarget:" + testTarget);


    if (testTarget == systemABundleName) {

        if (testAction == "promiseKill") {
            console.log("ACTS_ProcessManage SystemAppA onShowProcess testAction:" + testAction);
            var result = await abilityManager.killProcessesByBundleName(testTarget);
            console.log("ACTS_ProcessManage SystemAppA onShowProcess result:" + result);
            publishResultEvent(result);
        } else if (testAction == "asyncKill") {
            console.log("ACTS_ProcessManage SystemAppA onShowProcess testAction:" + testAction);
            abilityManager.killProcessesByBundleName(testTarget, onReceivedHandleCallback);
        } else if (testAction == "promiseCleanup") {
            console.log("ACTS_ProcessManage SystemAppA onShowProcess testAction:" + testAction);
            var result = await abilityManager.clearUpApplicationData(testTarget);
            console.log("ACTS_ProcessManage SystemAppA onShowProcess result:" + result);
            publishResultEvent(result);
        } else if (testAction == "asyncCleanup") {
            console.log("ACTS_ProcessManage SystemAppA onShowProcess testAction:" + testAction);
            abilityManager.clearUpApplicationData(testTarget, onReceivedHandleCallback);
        }

    }
    else {
        try {
            subscriber = await commonEvent.createSubscriber(subscribeInfo)
            console.debug("ACTS_ProcessManage SystemAppA onShowProcess subscriber:"
                + JSON.stringify(subscriber));
            commonEvent.subscribe(subscriber, subscribeCallBack);
        } catch (err) {
            console.debug("ACTS_ProcessManage SystemAppA onShowProcess err:" + err);
        }

        if (testTarget == systemBBundleName) {
            await featureAbility.startAbility(
                {
                    want:
                    {
                        bundleName: systemBBundleName,
                        abilityName: systemBAbilityName,
                    },
                }
            );
        }
        else if (testTarget == vendorABundleName) {
            await featureAbility.startAbility(
                {
                    want:
                    {
                        bundleName: vendorABundleName,
                        abilityName: vendorAAbilityName,
                    },
                }
            );
        }
        else if (testTarget == multiAAbilityName) {
            await featureAbility.startAbility(
                {
                    want:
                    {
                        bundleName: systemABundleName,
                        abilityName: multiAAbilityName,
                    },
                }
            );
        }
    }

    async function subscribeCallBack(err, data) {
        console.debug("ACTS_ProcessManage SystemAppA subscribeCallBack SubscribeCallBack data:====>"
            + JSON.stringify(data));
        console.debug("ACTS_ProcessManage SystemAppA subscribeCallBack SubscribeCallBack data.event:====>"
            + JSON.stringify(data.event));

        if (data.event == eventFromSystemAMulti) {
            testTarget = systemABundleName;
        }

        if (testAction == "promiseKill") {
            console.log("ACTS_ProcessManage SystemAppA subscribeCallBack testAction:" + testAction);
            var result = await abilityManager.killProcessesByBundleName(testTarget);
            console.log("ACTS_ProcessManage SystemAppA subscribeCallBack result:" + result);
            publishResultEvent(result);
        } else if (testAction == "asyncKill") {
            console.log("ACTS_ProcessManage SystemAppA subscribeCallBack testAction:" + testAction);
            await abilityManager.killProcessesByBundleName(testTarget, onReceivedHandleCallback);
        } else if (testAction == "promiseCleanup") {
            console.log("ACTS_ProcessManage SystemAppA subscribeCallBack testAction:" + testAction);
            var result = await abilityManager.clearUpApplicationData(testTarget);
            console.log("ACTS_ProcessManage SystemAppA subscribeCallBack result:" + result);
            publishResultEvent(result);
        } else if (testAction == "asyncCleanup") {
            console.log("ACTS_ProcessManage SystemAppA subscribeCallBack testAction:" + testAction);
            await abilityManager.clearUpApplicationData(testTarget, onReceivedHandleCallback);
        } else if (testAction == "promiseCleanupTentimes") {
            for (var i = 0; i < 10; i++) {
                console.log("ACTS_ProcessManage SystemAppA subscribeCallBack testAction:" + testAction);
                var result = await abilityManager.clearUpApplicationData(testTarget);
                console.log("ACTS_ProcessManage SystemAppA subscribeCallBack result:" + result);
            }
            publishResultEvent(result);
        }
    }

}

export default {
    data: {
        title: ""
    },
    onInit() {
        console.info('ACTS_ProcessManage SystemAppA onInit');
        this.title = "ProcessManageSystemAppA";
    },
    onReady() {
        console.info('ACTS_ProcessManage SystemAppA onReady');
        onShowProcess();
    },
    onActive() {
        console.info('ACTS_ProcessManage SystemAppA onActive');
    },
    onShow() {
        console.info('ACTS_ProcessManage SystemAppA onShow');

    },
    onInactive() {
        console.info('ACTS_ProcessManage SystemAppA onInactive');
    },
    onHide() {
        console.info('ACTS_ProcessManage SystemAppA onHide');
    },
    onDestroy() {
        console.info('ACTS_ProcessManage SystemAppA onDestroy');
    },
    onBackPress() {
        console.info('ACTS_ProcessManage SystemAppA onBackPress');
    },
    onNewRequest() {
        console.info('ACTS_ProcessManage SystemAppA onNewRequest');
    },
    onStartContinuation() {
        console.info('ACTS_ProcessManage SystemAppA onStartContinuation');
    },
    onSaveData(value) {
        console.info('ACTS_ProcessManage SystemAppA onSaveData:' + JSON.stringify(value));
    },
    onRestoreData(value) {
        console.info('ACTS_ProcessManage SystemAppA onRestoreData:' + JSON.stringify(value));
    },
    onCompleteContinuation(code) {
        console.info('ACTS_ProcessManage SystemAppA onCompleteContinuation:' + JSON.stringify(code));
    },
    onConfigurationUpdated(configuration) {
        console.info('ACTS_ProcessManage SystemAppA onConfigurationUpdated:' + JSON.stringify(configuration));
    }
}