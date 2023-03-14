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
import account from '@ohos.account.appAccount'
import commonevent from '@ohos.commonevent'
import featureAbility from '@ohos.ability.featureability'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

const TIMEOUT = 5000;
describe('ActsAccountChangeOnOff', async function () {

    beforeAll(async function (done) {
        console.debug("====>startAbility start====");
        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsaccountsceneonoff",
                    abilityName: "com.example.actsaccountsceneonoff.MainAbility",
                    action: "action1",
                    parameters:
                    {},
                },
            },
        );
        setTimeout(done(), TIMEOUT);
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_0100
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Received the account information change to the authorized account of the subscription to change
     *                 the additional information
     */
    it('ActsAccountChangeOnOff_0100', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_0100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_0100 start");
        await appAccountManager.addAccount("changeonoff_extra");
        console.debug("====>enableAppAccess ActsAccountChangeOnOff_0100 start");
        await appAccountManager.enableAppAccess("changeonoff_extra", "com.example.actsaccountsceneonoff");
        function unSubscriberCallback(err){
            console.debug("====>unsubscribe 0100 err:" + JSON.stringify(err));
        }
        function deleteAccountCallback(err){
            console.debug("====>delete account 0100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            done();
        }
        function disCallback(err){
            console.debug("====>delete account 0100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.deleteAccount("changeonoff_extra", deleteAccountCallback);
        }
        function subscriberCallback(err, data){
            console.debug("====>subscriberCallback 0100 data:" + JSON.stringify(data));
            expect(data.event).assertEqual("account_on_change_extra");
            expect(data.data).assertEqual("SUCCESS");
            commonevent.unsubscribe(subscriber, unSubscriberCallback);
            appAccountManager.disableAppAccess("changeonoff_extra", "com.example.actsaccountsceneonoff", disCallback);
        }
        function publishCallback(err){
            console.debug("====>publish call back err:" + JSON.stringify(err));
            setTimeout(async function (){
                console.debug("====>setAccountExtraInfo start====");
                try{
                    await appAccountManager.setAccountExtraInfo("changeonoff_extra", "change_extra");
                }
                catch(err){
                    console.error("====>setAccountExtraInfo fail err:" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
                console.debug("====>setAccountExtraInfo finish====");
            }, 500)
        }
        var commonEventSubscribeInfo = {
            events: ["account_on_change_extra"]
        }
        var subscriber
        commonevent.createSubscriber(commonEventSubscribeInfo).then(function (data){
            subscriber = data;
            commonevent.subscribe(subscriber, subscriberCallback);
            console.debug("====>subscribe ActsAccountChangeOnOff_0100 finish====")
        });

        var commonEventPublishData = {
            code: 1
        }
        setTimeout(function (){
            console.debug("====>publish event account_on_change 0100====");
            commonevent.publish("account_on_change", commonEventPublishData, publishCallback);
        }, 1000);
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_0200
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Received the account information change to the authorized account of the subscription to change
     *                 the associatal data
     */
    it('ActsAccountChangeOnOff_0200', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_0200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_0200 start");
        await appAccountManager.addAccount("onoff_associatedata");
        console.debug("====>enableAppAccess ActsAccountChangeOnOff_0200 start");
        await appAccountManager.enableAppAccess("onoff_associatedata", "com.example.actsaccountsceneonoff");
        function unSubscriberCallback(err){
            console.debug("====>unsubscribe 0200 err:" + JSON.stringify(err));
        }
        function deleteAccountCallback(err){
            console.debug("====>delete account 0200 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            done();
        }
        function subscriberCallback(err, data){
            console.debug("====>subscriberCallback 0200 data:" + JSON.stringify(data));
            expect(data.event).assertEqual("account_on_change_associatedata");
            expect(data.data).assertEqual("SUCCESS");
            commonevent.unsubscribe(subscriber, unSubscriberCallback);
            appAccountManager.deleteAccount("onoff_associatedata", deleteAccountCallback);
        }
        function publishCallback(err){
            console.debug("====>publish call back err:" + JSON.stringify(err));
            setTimeout(async function (){
                console.debug("====>setAssociatedData start====");
                try{
                    await appAccountManager.setAssociatedData("onoff_associatedata", "change_key", "change_value");
                }
                catch(err){
                    console.error("====>setAssociatedData fail err:" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
                console.debug("====>setAssociatedData finish====");
            }, 500)
        }
        var commonEventSubscribeInfo = {
            events: ["account_on_change_associatedata"]
        }
        var subscriber
        commonevent.createSubscriber(commonEventSubscribeInfo).then(function (data){
            subscriber = data;
            commonevent.subscribe(subscriber, subscriberCallback);
            console.debug("====>subscribe ActsAccountChangeOnOff_0200 finish====")
        });

        var commonEventPublishData = {
            code: 2
        }
        setTimeout(function (){
            console.debug("====>publish event account_on_change 0200====");
            commonevent.publish("account_on_change", commonEventPublishData, publishCallback);
        }, 1000);
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_0300
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Received the account information change to the authorized account of the subscription to change
     *                 the credential
     */
    it('ActsAccountChangeOnOff_0300', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_0300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_0300 start");
        await appAccountManager.addAccount("onoff_credential");
        console.debug("====>enableAppAccess ActsAccountChangeOnOff_0300 start");
        await appAccountManager.enableAppAccess("onoff_credential", "com.example.actsaccountsceneonoff");
        function unSubscriberCallback(err){
            console.debug("====>unsubscribe 0300 err:" + JSON.stringify(err));
        }
        function deleteAccountCallback(err){
            console.debug("====>delete account 0300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            done();
        }
        function subscriberCallback(err, data){
            console.debug("====>subscriberCallback 0300 data:" + JSON.stringify(data));
            expect(data.event).assertEqual("account_on_change_credential");
            expect(data.data).assertEqual("SUCCESS");
            commonevent.unsubscribe(subscriber, unSubscriberCallback);
            appAccountManager.deleteAccount("onoff_credential", deleteAccountCallback);
        }
        function publishCallback(err){
            console.debug("====>publish call back err:" + JSON.stringify(err));
            setTimeout(async function (){
                console.debug("====>setAccountCredential start====");
                try{
                    await appAccountManager.setAccountCredential("onoff_credential", "credentialType", "credential");
                }
                catch(err){
                    console.error("====>setAccountCredential fail err:" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
                console.debug("====>setAccountCredential finish====");
            }, 500)
        }
        var commonEventSubscribeInfo = {
            events: ["account_on_change_credential"]
        }
        var subscriber
        commonevent.createSubscriber(commonEventSubscribeInfo).then(function (data){
            subscriber = data;
            commonevent.subscribe(subscriber, subscriberCallback);
            console.debug("====>subscribe ActsAccountChangeOnOff_0300 finish====")
        });

        var commonEventPublishData = {
            code: 3
        }
        setTimeout(function (){
            console.debug("====>publish event account_on_change 0300====");
            commonevent.publish("account_on_change", commonEventPublishData, publishCallback);
        }, 1000);
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_0400
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Received the account information change to the authorized account of the subscription to delete
     *                 authorized account
     */
    it('ActsAccountChangeOnOff_0400', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_0400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add first account ActsAccountChangeOnOff_0400 start");
        await appAccountManager.addAccount("onoff_deleteFir");
        console.debug("====>add second account ActsAccountChangeOnOff_0400 start");
        await appAccountManager.addAccount("onoff_deleteSec");
        console.debug("====>enableAppAccess first ActsAccountChangeOnOff_0400 start");
        await appAccountManager.enableAppAccess("onoff_deleteFir", "com.example.actsaccountsceneonoff");
        console.debug("====>enableAppAccess second ActsAccountChangeOnOff_0400 start");
        await appAccountManager.enableAppAccess("onoff_deleteSec", "com.example.actsaccountsceneonoff");
        function unSubscriberCallback(err){
            console.debug("====>unsubscribe 0400 err:" + JSON.stringify(err));
        }
        function deleteAccountCallback(err){
            console.debug("====>delete account 0400 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            done();
        }
        function subscriberCallback(err, data){
            console.debug("====>subscriberCallback 0400 data:" + JSON.stringify(data));
            expect(data.event).assertEqual("account_on_delete_another");
            expect(data.data).assertEqual("SUCCESS");
            commonevent.unsubscribe(subscriber, unSubscriberCallback);
            appAccountManager.deleteAccount("onoff_deleteFir", deleteAccountCallback);
        }
        function publishCallback(err){
            console.debug("====>publish call back err:" + JSON.stringify(err));
            setTimeout(async function (){
                console.debug("====>deleteAccount start====");
                try{
                    await appAccountManager.deleteAccount("onoff_deleteSec");
                }
                catch(err){
                    console.error("====>deleteAccount fail err:" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
                console.debug("====>deleteAccount finish====");
            }, 500)
        }
        var commonEventSubscribeInfo = {
            events: ["account_on_delete_another"]
        }
        var subscriber
        commonevent.createSubscriber(commonEventSubscribeInfo).then(function (data){
            subscriber = data;
            commonevent.subscribe(subscriber, subscriberCallback);
            console.debug("====>subscribe ActsAccountChangeOnOff_0400 finish====")
        });

        var commonEventPublishData = {
            code: 4
        }
        setTimeout(function (){
            console.debug("====>publish event account_on_change 0400====");
            commonevent.publish("account_on_change", commonEventPublishData, publishCallback);
        }, 1000);
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_0500
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Received the account information change to the authorized account of the subscription to delete
     *                 the only authorized account
     */
    it('ActsAccountChangeOnOff_0500', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_0500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_0500 start");
        await appAccountManager.addAccount("onoff_delete");
        console.debug("====>enableAppAccess ActsAccountChangeOnOff_0500 start");
        await appAccountManager.enableAppAccess("onoff_delete", "com.example.actsaccountsceneonoff");
        function unSubscriberCallback(err){
            console.debug("====>unsubscribe 0500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            done();
        }
        function subscriberCallback(err, data){
            console.debug("====>subscriberCallback 0500 data:" + JSON.stringify(data));
            expect(data.event).assertEqual("account_on_change_delete");
            expect(data.data).assertEqual("SUCCESS");
            commonevent.unsubscribe(subscriber, unSubscriberCallback);
        }
        function publishCallback(err){
            console.debug("====>publish call back err:" + JSON.stringify(err));
            setTimeout(async function (){
                console.debug("====>deleteAccount start====");
                try{
                    await appAccountManager.deleteAccount("onoff_delete");
                }
                catch(err){
                    console.error("====>deleteAccount fail err:" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
                console.debug("====>deleteAccount finish====");
            }, 500)
        }
        var commonEventSubscribeInfo = {
            events: ["account_on_change_delete"]
        }
        var subscriber
        commonevent.createSubscriber(commonEventSubscribeInfo).then(function (data){
            subscriber = data;
            commonevent.subscribe(subscriber, subscriberCallback);
            console.debug("====>subscribe ActsAccountChangeOnOff_0500 finish====")
        });

        var commonEventPublishData = {
            code: 5
        }
        setTimeout(function (){
            console.debug("====>publish event account_on_change 0500====");
            commonevent.publish("account_on_change", commonEventPublishData, publishCallback);
        }, 1000);
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_0600
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Received the account information change to the authorized account of the subscription to cancel
     *                 authorized account   
     */
    it('ActsAccountChangeOnOff_0600', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_0600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add first account ActsAccountChangeOnOff_0600 start");
        await appAccountManager.addAccount("onoff_enableFir");
        console.debug("====>add second account ActsAccountChangeOnOff_0600 start");
        await appAccountManager.addAccount("onoff_enableSec");
        console.debug("====>enableAppAccess first ActsAccountChangeOnOff_0600 start");
        await appAccountManager.enableAppAccess("onoff_enableFir", "com.example.actsaccountsceneonoff");
        console.debug("====>enableAppAccess second ActsAccountChangeOnOff_0600 start");
        await appAccountManager.enableAppAccess("onoff_enableSec", "com.example.actsaccountsceneonoff");
        function unSubscriberCallback(err){
            console.debug("====>unsubscribe 0600 err:" + JSON.stringify(err));
        }
        function deleteAccountCallback(err){
            console.debug("====>delete first account 0600 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.deleteAccount("onoff_enableSec", (err)=>{
                console.debug("====>delete second account 0600 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                done();
            });
        }
        function subscriberCallback(err, data){
            console.debug("====>subscriberCallback 0600 data:" + JSON.stringify(data));
            expect(data.event).assertEqual("account_on_disable_another");
            expect(data.data).assertEqual("SUCCESS");
            commonevent.unsubscribe(subscriber, unSubscriberCallback);
            appAccountManager.deleteAccount("onoff_enableFir", deleteAccountCallback);
        }
        function publishCallback(err){
            console.debug("====>publish call back err:" + JSON.stringify(err));
            setTimeout(async function (){
                console.debug("====>disableAppAccess start====");
                try{
                    await appAccountManager.disableAppAccess("onoff_enableSec", "com.example.actsaccountsceneonoff");
                }
                catch(err){
                    console.error("====>disableAppAccess fail err:" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
                console.debug("====>disableAppAccess finish====");
            }, 500)
        }
        var commonEventSubscribeInfo = {
            events: ["account_on_disable_another"]
        }
        var subscriber
        commonevent.createSubscriber(commonEventSubscribeInfo).then(function (data){
            subscriber = data;
            commonevent.subscribe(subscriber, subscriberCallback);
            console.debug("====>subscribe ActsAccountChangeOnOff_0600 finish====")
        });

        var commonEventPublishData = {
            code: 6
        }
        setTimeout(function (){
            console.debug("====>publish event account_on_change 0600====");
            commonevent.publish("account_on_change", commonEventPublishData, publishCallback);
        }, 1000);
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_0700
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Received the account information change to the authorized account of the subscription to cancel
     *                 the only authorized account
     */
    it('ActsAccountChangeOnOff_0700', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_0700 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_0700 start");
        await appAccountManager.addAccount("onoff_disable");
        console.debug("====>enableAppAccess ActsAccountChangeOnOff_0700 start");
        await appAccountManager.enableAppAccess("onoff_disable", "com.example.actsaccountsceneonoff");
        function unSubscriberCallback(err){
            console.debug("====>unsubscribe 0700 err:" + JSON.stringify(err));
        }
        function deleteAccountCallback(err){
            console.debug("====>delete account 0700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            done();
        }
        function subscriberCallback(err, data){
            console.debug("====>subscriberCallback 0700 data:" + JSON.stringify(data));
            expect(data.event).assertEqual("account_on_change_disable");
            expect(data.data).assertEqual("SUCCESS");
            commonevent.unsubscribe(subscriber, unSubscriberCallback);
            appAccountManager.deleteAccount("onoff_disable", deleteAccountCallback);
        }
        function publishCallback(err){
            console.debug("====>publish call back err:" + JSON.stringify(err));
            setTimeout(async function (){
                console.debug("====>disableAppAccess start====");
                try{
                    await appAccountManager.disableAppAccess("onoff_disable", "com.example.actsaccountsceneonoff");
                }
                catch(err){
                    console.error("====>disableAppAccess fail err:" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
                console.debug("====>disableAppAccess finish====");
            }, 500)
        }
        var commonEventSubscribeInfo = {
            events: ["account_on_change_disable"]
        }
        var subscriber
        commonevent.createSubscriber(commonEventSubscribeInfo).then(function (data){
            subscriber = data;
            commonevent.subscribe(subscriber, subscriberCallback);
            console.debug("====>subscribe ActsAccountChangeOnOff_0700 finish====")
        });

        var commonEventPublishData = {
            code: 7
        }
        setTimeout(function (){
            console.debug("====>publish event account_on_change 0700====");
            commonevent.publish("account_on_change", commonEventPublishData, publishCallback);
        }, 1000);
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_0800
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Receive account information after the app subscribes to itself and changes additional information
     */
    it('ActsAccountChangeOnOff_0800', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_0800 start====");
        let dataMap = new Map();
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_0800 start");
        await appAccountManager.addAccount("onoff_self");
        console.debug("====>on ActsAccountChangeOnOff_0800 start");
        function changeOnCallback(data){
            console.debug("====>receive change 0800 data:" + JSON.stringify(data));
            console.debug("====>data.length:" + data.length);
            for (let i = 0, len = data.length; i < len; i++) {
                dataMap.set(data[i].name, data[i].owner)
            }
            expect(dataMap.has("onoff_self")).assertTrue();
            if (dataMap.has("onoff_self")) {
                let data = dataMap.get("onoff_self");
                console.debug("====>the account owner is: " + data);
                expect(data).assertEqual("com.example.actsaccounttest");
            }
            appAccountManager.off('change', function (){
                console.debug("====>off ActsAccountChangeOnOff_0800 finish====");
                appAccountManager.deleteAccount("onoff_self", (err)=>{
                    console.debug("====>delete account ActsAccountChangeOnOff_0800 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountChangeOnOff_0800 end====");
                    done();
                });
            });
        }
        try{
            appAccountManager.on('change', ["com.example.actsaccounttest"], changeOnCallback);
        }
        catch(err){
            console.error("====>on ActsAccountChangeOnOff_0800 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>setAccountExtraInfo start====");
        try{
            await appAccountManager.setAccountExtraInfo("onoff_self", "change_extra");
        }
        catch(err){
            console.error("====>setAccountExtraInfo fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_0900
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Receiving account information after adding two accounts to the app, subscribing to itself,
     *                 and changing one of the associatal data
     */
    it('ActsAccountChangeOnOff_0900', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_0900 start====");
        let dataMap = new Map();
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add first account ActsAccountChangeOnOff_0900 start");
        await appAccountManager.addAccount("onoff_self_first");
        console.debug("====>add second account ActsAccountChangeOnOff_0900 start");
        await appAccountManager.addAccount("onoff_self_second");
        console.debug("====>on ActsAccountChangeOnOff_0900 start");
        function changeOnCallback(data){
            console.debug("====>receive change 0900 data:" + JSON.stringify(data));
            console.debug("====>data.length:" + data.length);
            for (let i = 0, len = data.length; i < len; i++) {
                dataMap.set(data[i].name, data[i].owner)
            }
            expect(dataMap.has("onoff_self_first")).assertTrue();
            if (dataMap.has("onoff_self_first")) {
                let data = dataMap.get("onoff_self_first");
                console.debug("====>the account owner is: " + data);
                expect(data).assertEqual("com.example.actsaccounttest");
            }
            expect(dataMap.has("onoff_self_second")).assertTrue();
            if (dataMap.has("onoff_self_second")) {
                let data = dataMap.get("onoff_self_second");
                console.debug("====>the account owner is: " + data);
                expect(data).assertEqual("com.example.actsaccounttest");
            }
            appAccountManager.off('change', function (){
                console.debug("====>off ActsAccountChangeOnOff_0900 finish====");
                appAccountManager.deleteAccount("onoff_self_first", (err)=>{
                    console.debug("====>delete first account 0900 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    appAccountManager.deleteAccount("onoff_self_second", (err)=>{
                        console.debug("====>delete second account 0900 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountChangeOnOff_0900 end====");
                        done();
                    });
                });
            });
        }
        try{
            appAccountManager.on('change', ["com.example.actsaccounttest"], changeOnCallback);
        }
        catch(err){
            console.error("====>on ActsAccountChangeOnOff_0900 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>setAssociatedData start====");
        try{
            await appAccountManager.setAssociatedData("onoff_self_second", "change_key", "change_value");
        }
        catch(err){
            console.error("====>setAssociatedData fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_1000
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Repeat subscription account information changes, the subscription behavior is independent
     */
    it('ActsAccountChangeOnOff_1000', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_1000 start====");
        let dataMapFir = new Map();
        let dataMapSec = new Map();
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_1000 start");
        await appAccountManager.addAccount("onoff_twice");
        console.debug("====>on ActsAccountChangeOnOff_1000 start");
        var twiceSign = 0;
        function changeOnFirstCallback(data){
            console.debug("====>first receive change 1000 data:" + JSON.stringify(data));
            console.debug("====>data.length:" + data.length);
            for (let i = 0, len = data.length; i < len; i++) {
                dataMapFir.set(data[i].name, data[i].owner)
            }
            expect(dataMapFir.has("onoff_twice")).assertTrue();
            if (dataMapFir.has("onoff_twice")) {
                let data = dataMapFir.get("onoff_twice");
                console.debug("====>the account owner is: " + data);
                expect(data).assertEqual("com.example.actsaccounttest");
            }
            twiceSign = twiceSign + 1;
        }
        function changeOnSecondCallback(data){
            console.debug("====>second receive change 1000 data:" + JSON.stringify(data));
            console.debug("====>data.length:" + data.length);
            for (let i = 0, len = data.length; i < len; i++) {
                dataMapSec.set(data[i].name, data[i].owner)
            }
            expect(dataMapSec.has("onoff_twice")).assertTrue();
            if (dataMapSec.has("onoff_twice")) {
                let data = dataMapSec.get("onoff_twice");
                console.debug("====>the account owner is: " + data);
                expect(data).assertEqual("com.example.actsaccounttest");
            }
            twiceSign = twiceSign + 1;
            setTimeout(async function (){
                console.debug("====>off start====");
                appAccountManager.off('change', function (){
                    console.debug("====>off ActsAccountChangeOnOff_1000 finish====");
                    appAccountManager.deleteAccount("onoff_twice", (err)=>{
                        console.debug("====>delete account ActsAccountChangeOnOff_1000 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        expect(twiceSign).assertEqual(2);
                        console.debug("====>ActsAccountChangeOnOff_1000 end====");
                        done();
                    });
                });
            }, 1000)
        }
        try{
            appAccountManager.on('change', ["com.example.actsaccounttest"], changeOnFirstCallback);
            appAccountManager.on('change', ["com.example.actsaccounttest"], changeOnSecondCallback);
        }
        catch(err){
            console.error("====>on ActsAccountChangeOnOff_1000 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>setAccountExtraInfo start====");
        try{
            await appAccountManager.setAccountExtraInfo("onoff_twice", "change_extra");
        }
        catch(err){
            console.error("====>setAccountExtraInfo fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_1100
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Subscription account information changes, where the bundleName in the parameter owners is
     *                 duplicated
     */
    it('ActsAccountChangeOnOff_1100', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_1100 start====");
        let dataMap = new Map();
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_1100 start");
        await appAccountManager.addAccount("onoff_same");
        console.debug("====>on ActsAccountChangeOnOff_1100 start");
        var sameSign = 0;
        function onCallBack(data){
            console.debug("====>receive change 1100 data:" + JSON.stringify(data));
            sameSign = sameSign + 1;
            console.debug("====>data.length:" + data.length);
            for (let i = 0, len = data.length; i < len; i++) {
                dataMap.set(data[i].name, data[i].owner)
            }
            expect(dataMap.has("onoff_same")).assertTrue();
            if (dataMap.has("onoff_same")) {
                let data = dataMap.get("onoff_same");
                console.debug("====>the account owner is: " + data);
                expect(data).assertEqual("com.example.actsaccounttest");
            }
        }
        try{
            appAccountManager.on('change', ["com.example.actsaccounttest", "com.example.actsaccounttest"], onCallBack);
        }
        catch(err){
            console.error("====>on ActsAccountChangeOnOff_1100 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>setAccountExtraInfo start====");
        try{
            await appAccountManager.setAccountExtraInfo("onoff_same", "change_extra");
        }
        catch(err){
            console.error("====>setAccountExtraInfo fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        setTimeout(async function (){
            console.debug("====>off 1100 start====");
            expect(sameSign).assertEqual(1);
            appAccountManager.off('change', function (){
                console.debug("====>off ActsAccountChangeOnOff_1100 finish====");
                appAccountManager.deleteAccount("onoff_same", (err)=>{
                    console.debug("====>delete account ActsAccountChangeOnOff_1100 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountChangeOnOff_1100 end====");
                    done();
                });
            });
        }, 1000)
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_1200
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : Repeatedly cancel the subscription after the subscription account information is changed
     */
    it('ActsAccountChangeOnOff_1200', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_1200 start====");
        let dataMap = new Map();
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_1200 start");
        await appAccountManager.addAccount("onoff_repeatoff");
        console.debug("====>on ActsAccountChangeOnOff_1200 start");
        function changeOnCallback(data){
            console.debug("====>receive change 1200 data:" + JSON.stringify(data));
            console.debug("====>data.length:" + data.length);
            for (let i = 0, len = data.length; i < len; i++) {
                dataMap.set(data[i].name, data[i].owner)
            }
            expect(dataMap.has("onoff_repeatoff")).assertTrue();
            if (dataMap.has("onoff_repeatoff")) {
                let data = dataMap.get("onoff_repeatoff");
                console.debug("====>the account owner is: " + data);
                expect(data).assertEqual("com.example.actsaccounttest");
            }
            appAccountManager.off('change', function (){
                console.debug("====>first off ActsAccountChangeOnOff_1200 finish====");
                appAccountManager.off('change', function (){
                    console.debug("====>second off ActsAccountChangeOnOff_1200 finish====");
                    setTimeout(async function (){
                        console.debug("====>delete account 1200 start====");
                        appAccountManager.deleteAccount("onoff_repeatoff", (err)=>{
                            console.debug("====>delete account ActsAccountChangeOnOff_1200 err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsAccountChangeOnOff_1200 end====");
                            done();
                        });
                    }, 1000)
                });
            });
        }
        try{
            appAccountManager.on('change', ["com.example.actsaccounttest"], changeOnCallback);
        }
        catch(err){
            console.error("====>on ActsAccountChangeOnOff_1200 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>setAccountExtraInfo start====");
        try{
            await appAccountManager.setAccountExtraInfo("onoff_repeatoff", "change_extra");
        }
        catch(err){
            console.error("====>setAccountExtraInfo fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_1300
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : There is an unauthorized bundleName in the subscription parameter owners array, and the
     *                 subscription fails
     */
    it('ActsAccountChangeOnOff_1300', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_1300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_1300 start");
        await appAccountManager.addAccount("onoff_same");
        console.debug("====>on ActsAccountChangeOnOff_1300 start");
        function changeOnCallBack(data){
            console.debug("====>receive change 1300 data:" + JSON.stringify(data));
            expect().assertFail();
            done();
        }
        var unauthorizedBundle = "com.example.actsaccountsceneonoff";
        try{
            appAccountManager.on('change', ["com.example.actsaccounttest", unauthorizedBundle], changeOnCallBack);
        }
        catch(err){
            console.error("====>on ActsAccountChangeOnOff_1300 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>setAccountExtraInfo start====");
        try{
            await appAccountManager.setAccountExtraInfo("onoff_same", "change_extra");
        }
        catch(err){
            console.error("====>setAccountExtraInfo fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        setTimeout(async function (){
            console.debug("====>delete account 1300 start====");
            appAccountManager.deleteAccount("onoff_same", (err)=>{
                console.debug("====>delete account ActsAccountChangeOnOff_1300 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>ActsAccountChangeOnOff_1300 end====");
                done();
            });
        }, 1000)
    });

    /*
     * @tc.number    : ActsAccountChangeOnOff_1400
     * @tc.name      : Subscribe/unsubscribe to the change event of application
     * @tc.desc      : There is a bundleName that does not exist in the subscription parameter owners array, and the
     *                 subscription fails
     */
    it('ActsAccountChangeOnOff_1400', 0, async function (done) {
        console.debug("====>ActsAccountChangeOnOff_1400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat appAccountManager finish");
        console.debug("====>add account ActsAccountChangeOnOff_1400 start");
        await appAccountManager.addAccount("onoff_same");
        console.debug("====>on ActsAccountChangeOnOff_1400 start");
        function changeOnCallBack(data){
            console.debug("====>receive change 1300 data:" + JSON.stringify(data));
            expect().assertFail();
            done();
        }
        var nonexistentBundle = "com.example.actsaccountnoneexistent";
        try{
            appAccountManager.on('change', ["com.example.actsaccounttest", nonexistentBundle], changeOnCallBack);
        }
        catch(err){
            console.error("====>on ActsAccountChangeOnOff_1400 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>setAccountExtraInfo start====");
        try{
            await appAccountManager.setAccountExtraInfo("onoff_same", "change_extra");
        }
        catch(err){
            console.error("====>setAccountExtraInfo fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        setTimeout(async function (){
            console.debug("====>delete account 1400 start====");
            appAccountManager.deleteAccount("onoff_same", (err)=>{
                console.debug("====>delete account ActsAccountChangeOnOff_1400 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>ActsAccountChangeOnOff_1400 end====");
                done();
            });
        }, 1000)
    });
})