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
import osaccount from '@ohos.account.osAccount'
import commonevent from '@ohos.commonevent'

const injectRef = Object.getPrototypeOf(global) || global
injectRef.regeneratorRuntime = require('@babel/runtime/regenerator')

export default {
    data: {
    },
    onInit() {
        this.title = "";
    },
    onShow() {
        var createIocalId;
        var commonEventSubscribeInfo = {
            events: ["osaccount_event_delete_permission"]
        }
        var subscriber;
        function subscriberCallback(err, data){
            console.debug("====>subscriberCallback data:" + JSON.stringify(data));
            var osAccountManager = osaccount.getAccountManager();
            console.debug("====>get AccountManager finish====");
            osAccountManager.removeOsAccount(createIocalId);
            commonevent.unsubscribe(subscriber, unSubscriberCallback);
        }
        function unSubscriberCallback(err){
            console.debug("====>unsubscribe err:" + JSON.stringify(err));
        }
        commonevent.createSubscriber(commonEventSubscribeInfo).then(function (data){
            subscriber = data;
            commonevent.subscribe(subscriber, subscriberCallback);
            console.debug("====>subscribe finish====")
        });

        async function sceneCreatOsAccount(){
            console.debug("====>scene start====");
            var osAccountManager = osaccount.getAccountManager();
            console.debug("====>get AccountManager finish====");
            var osAccountInfo = await osAccountManager.createOsAccount("osAccountName", osaccount.OsAccountType.NORMAL);
            createIocalId = osAccountInfo.localId;
            function publishCallback(err){
                console.debug("====>publish call back scene err:" + JSON.stringify(err));
            }

            var commonEventPublishData = {
                code: createIocalId,
                data: "SUCCESS"
            }
            commonevent.publish("osaccount_event_create_permission", commonEventPublishData, publishCallback);
        }
        sceneCreatOsAccount();

    },
    onReady() {
    },
}