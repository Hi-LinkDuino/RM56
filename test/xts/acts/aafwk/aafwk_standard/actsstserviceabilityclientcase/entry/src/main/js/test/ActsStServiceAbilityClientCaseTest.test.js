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
import featureAbility from '@ohos.ability.featureAbility'
import abilityManager from '@ohos.app.abilityManager'

import commonEvent from '@ohos.commonevent'

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe('ActsStServiceAbilityTest', function () {
    let bundleName = "com.amsst.stserviceabilityserver";
    let abilityName = "com.amsst.stserviceabilityserver.ServiceAbility";

 
    var subscriber0600;
    var CommonEventSubscribeInfo0600 = {
        events: ["ACTS_ASerivceAbilityServer_onConnect_PageConnectService_0600",
            "ACTS_ASerivceAbilityServer_onDisConnect",
        ],
    };

    var subscriber1400;
    var CommonEventSubscribeInfo1400 = {
        events: ["ACTS_ASerivceAbilityServerSecond_onConnect_ServiceConnectService_1400",
            "ACTS_ASerivceAbilityServerSecond_onDisConnect",
        ],
    };

    function unsubscribe(caller, subscriber) {
        commonEvent.unsubscribe(subscriber, (err, data) => {
            console.debug("=ACTS_Aunsubscribe (err,data)=======>"
                + (caller)
                + (" , json err【") + JSON.stringify(err) + (" 】")
                + ("json data【") + JSON.stringify(data) + (" 】")
                + " ,err=" + err + " ,data=" + data);
        });
    }
    let gSetTimeout = 1000
    beforeAll(async (done) => {
        console.debug('= ACTS_AbeforeAll 1541 ====<begin');
        console.debug('= ACTS_AbeforeAll ====<end');
        done();
    })
    beforeEach(async (done) => {
        setTimeout(function () {
            done();
        }, gSetTimeout);
    })
    afterEach(async (done) => {
        setTimeout(function () {
            done();
        }, gSetTimeout);
    })
    afterAll((done) => {
        console.debug('= ACTS_AafterAll ====<begin');
        setTimeout(function () {
            console.debug('= ACTS_AafterAll ====<end');
            featureAbility.terminateSelf();
            done();
        }, gSetTimeout);
    })

    /*
     * @tc.number  ACTS_AAbilityStartSetting_0100
     * @tc.name    The configured URI is started and the page is not configured
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_AAbilityStartSetting_0100",0, async function(done){
        console.info("ACTS_AlogMessage 1 ACTS_AAbilityStartSetting_0100-------------------");
        try{
            let Want = {
                bundleName: "com.example.abilityStartSettingApp",
                abilityName: "com.example.abilityStartSettingApp.MainAbility",
            }

            let abilityStartSetting ={
                [featureAbility.AbilityStartSetting.BOUNDS_KEY] : [100,200,300,400],
                [featureAbility.AbilityStartSetting.WINDOW_MODE_KEY] :
                        featureAbility.AbilityWindowConfiguration.WINDOW_MODE_FULLSCREEN,
                [featureAbility.AbilityStartSetting.DISPLAY_ID_KEY] : 1,
            }

            var StartAbilityParameter = {
                want:Want,
                abilityStartSetting:abilityStartSetting
            }

            featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
                console.log('ACTS_AAbilityStartSetting_0100 asyncCallback errCode : ' + JSON.stringify(err) 
                + " data: " + JSON.stringify(data));
                expect(2097152).assertEqual(err.code);
                done();
            });
        }catch(error){
            console.log("ACTS_AAbilityStartSetting_0100 : error = " + error);
            done();
        }
    })

   /*
     * @tc.number  ACTS_AAbilityStartSettingNew_0200
     * @tc.name    The configured URI is started and the page is not configured
     * @tc.desc    Function test
     * @tc.level   0
     */
   it("ACTS_AAbilityStartSettingNew_0200",0, async function(done){
    console.info("ACTS_logMessage 1 ACTS_AAbilityStartSettingNew_0200-------------------");
    try{
        let Want = {
            bundleName: "com.example.abilityStartSettingApp2",
            abilityName: "com.example.abilityStartSettingApp.MainAbility",
        }

        let abilityStartSetting ={
            [featureAbility.AbilityStartSetting.BOUNDS_KEY] : [100,200,300,400],
            [featureAbility.AbilityStartSetting.WINDOW_MODE_KEY] :
                    featureAbility.AbilityWindowConfiguration.WINDOW_MODE_UNDEFINED,
            [featureAbility.AbilityStartSetting.DISPLAY_ID_KEY] : 1,
        }

        var StartAbilityParameter = {
            want:Want,
            abilityStartSetting:abilityStartSetting
        }

        featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
            console.log('ACTS_AAbilityStartSettingNew_0200 asyncCallback errCode : ' + JSON.stringify(err) 
            + " data: " + JSON.stringify(data));
            expect(2097152).assertEqual(err.code);
            done();
        });
    }catch(error){
        console.log("ACTS_AAbilityStartSettingNew_0200 : error = " + error);
        done();
    }
})

/*
 * @tc.number  ACTS_AAbilityStartSettingNew_0300
 * @tc.name    The configured URI is started and the page is not configured
 * @tc.desc    Function test
 * @tc.level   0
 */
it("ACTS_AAbilityStartSettingNew_0300",0, async function(done){
    console.info("ACTS_logMessage 1 ACTS_AAbilityStartSettingNew_0300-------------------");
    try{
        let Want = {
            bundleName: "com.example.abilityStartSettingApp",
            abilityName: "com.example.abilityStartSettingApp.MainAbility2",
        }

        let abilityStartSetting ={
            [featureAbility.AbilityStartSetting.BOUNDS_KEY] : [100,200,300,400],
            [featureAbility.AbilityStartSetting.WINDOW_MODE_KEY] :
                    featureAbility.AbilityWindowConfiguration.WINDOW_MODE_SPLIT_PRIMARY,
            [featureAbility.AbilityStartSetting.DISPLAY_ID_KEY] : 1,
        }

        var StartAbilityParameter = {
            want:Want,
            abilityStartSetting:abilityStartSetting
        }

        featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
            console.log('ACTS_AAbilityStartSettingNew_0300 asyncCallback errCode : ' + JSON.stringify(err) 
            + " data: " + JSON.stringify(data));
            expect(2097152).assertEqual(err.code);
            done();
        });
    }catch(error){
        console.log("ACTS_AAbilityStartSettingNew_0300 : error = " + error);
        done();
    }
})

/*
 * @tc.number  ACTS_AAbilityStartSettingNew_0400
 * @tc.name    The configured URI is started and the page is not configured
 * @tc.desc    Function test
 * @tc.level   0
 */
it("ACTS_AAbilityStartSettingNew_0400",0, async function(done){
    console.info("ACTS_logMessage 1 ACTS_AAbilityStartSettingNew_0400-------------------");
    try{
        let Want = {
            bundlame: "com.example.abilityStartSettingApp",
            abilityName: "com.example.abilityStartSettingApp.MainAbility",
        }

        let abilityStartSetting ={
            [featureAbility.AbilityStartSetting.BOUNDS_KEY] : [100,200,300,400],
            [featureAbility.AbilityStartSetting.WINDOW_MODE_KEY] :
                    featureAbility.AbilityWindowConfiguration.WINDOW_MODE_SPLIT_SECONDARY,
            [featureAbility.AbilityStartSetting.DISPLAY_ID_KEY] : 1,
        }

        var StartAbilityParameter = {
            want:Want,
            abilityStartSetting:abilityStartSetting
        }

        featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
            console.log('ACTS_AAbilityStartSettingNew_0400 asyncCallback errCode : ' + JSON.stringify(err) 
            + " data: " + JSON.stringify(data));
            expect(2097152).assertEqual(err.code);
            done();
        });
    }catch(error){
        console.log("ACTS_AAbilityStartSettingNew_0400 : error = " + error);
        done();
    }
})

/*
 * @tc.number  ACTS_AAbilityStartSettingNew_0500
 * @tc.name    The configured URI is started and the page is not configured
 * @tc.desc    Function test
 * @tc.level   0
 */
it("ACTS_AAbilityStartSettingNew_0500",0, async function(done){
    console.info("ACTS_--------ACTS_AAbilityStartSettingNew_0500------");
    try{
        let Want = {
            bundleName: "com.example.abilityStartSettingApp",
            abilityName: "com.example.abilityStartSettingApp.MainAbility",
        }

        let abilityStartSetting ={
            [featureAbility.AbilityStartSetting.BOUNDS_KEY] : [100,200,300,400],
            [featureAbility.AbilityStartSetting.WINDOW_MODE_KEY] :
                    featureAbility.AbilityWindowConfiguration.WINDOW_MODE_FLOATING,
            [featureAbility.AbilityStartSetting.DISPLAY_ID_KEY] : 1,
        }

        var StartAbilityParameter = {
            want:Want,
            abilityStartSetting:abilityStartSetting
        }

        featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
            console.log('ACTS_AAbilityStartSettingNew_0500 asyncCallback errCode : ' + JSON.stringify(err) 
            + " data: " + JSON.stringify(data));
            expect(2097152).assertEqual(err.code);
            done();
        });
    }catch(error){
        console.log("ACTS_AAbilityStartSettingNew_0500 : error = " + error);
        done();
    }
})

    /*
    * @tc.number: ACTS_AJsServiceAbility_0600
    * @tc.name: featureAbility.ConnectAbility : Connects an ability to a Service ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_AJsServiceAbility_0600', 0, async function (done) {
        console.log('ACTS_AJsServiceAbility_0600====<begin');
        try {
            var mConnIdJsAsyncCallback;
            commonEvent.createSubscriber(CommonEventSubscribeInfo0600).then(async (data) => {
                console.debug("=ACTS_AJsServiceAbility_0600 createSubscriber .then(data)=======>"
                    + ("json data【") + JSON.stringify(data) + (" 】")
                    + " ,data=" + data);
                subscriber0600 = data;
                await commonEvent.subscribe(subscriber0600, async (err, data) => {
                    console.debug("=ACTS_AJsServiceAbility_0600 subscribe (err,data)=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】")
                        + ("json data【") + JSON.stringify(data) + (" 】")
                        + " ,err=" + err + " ,data=" + data);
                    if (data.event != "ACTS_ASerivceAbilityServer_onDisConnect") {
                        expect("ACTS_ASerivceAbilityServer_onConnect_PageConnectService_0600").assertEqual(
                            data.event);
                        featureAbility.disconnectAbility(mConnIdJsAsyncCallback, (err) => {
                            console.debug("=ACTS_AJsServiceAbility_0600 disconnectAbility err====>"
                                + ("json err=") + JSON.stringify(err));
                        })
                    } else {
                        clearTimeout(currentAlertTimeout);
                        expect("ACTS_ASerivceAbilityServer_onDisConnect").assertEqual(
                            data.event);
                        unsubscribe("ACTS_AJsServiceAbility_0600_unsubscribe", subscriber0600);
                        console.log('ACTS_AJsServiceAbility_0600====<end')
                        done();
                    }
                });
            })
            function onConnectCallback(element, remote) {
                console.debug('ACTS_AJsServiceAbility_0600_onConnectCallback ====> mConnIdJsAsyncCallback='
                    + JSON.stringify(mConnIdJsAsyncCallback) + " , " + mConnIdJsAsyncCallback);
                console.debug('ACTS_AJsServiceAbility_0600_onConnectCallback ====> element='
                    + JSON.stringify(element) + " , " + element);
                console.debug('ACTS_AJsServiceAbility_0600_onConnectCallback ====> remote='
                    + JSON.stringify(remote) + " , " + remote);
                console.debug('ACTS_AJsServiceAbility_0600_onConnectCallback ====> remote is proxy:'
                    + (remote instanceof rpc.RemoteProxy));
            }

            function onDisconnectCallback(element) {
                console.debug('ACTS_AJsServiceAbility_0600_onDisconnectCallback ====> element='
                    + JSON.stringify(element) + " , " + element);
            }

            function onFailedCallback(code) {
                console.debug('ACTS_AJsServiceAbility_0600_onFailedCallback ====> code='
                    + JSON.stringify(code) + " , " + code)
                    expect(code==featureAbility.ErrorCode.ABILITY_NOT_FOUND 
                        || (code!=featureAbility.ErrorCode.NO_ERROR 
                            || code!=featureAbility.ErrorCode.INVALID_PARAMETER
                            || code!=featureAbility.ErrorCode.PERMISSION_DENY
                    )).assertTrue();
            }

            mConnIdJsAsyncCallback = featureAbility.connectAbility(
                {
                    bundleName: bundleName,
                    abilityName: abilityName,
                    action: "PageConnectService_0600",
                },
                {
                    onConnect: onConnectCallback,
                    onDisconnect: onDisconnectCallback,
                    onFailed: onFailedCallback,
                },
            )
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_AJsServiceAbility_0600====<end mySetTimeout')
                done();
            }
        } catch (err) {
            clearTimeout(currentAlertTimeout);
            console.log('ACTS_AJsServiceAbility_0600====<end err=' + err)
            done();
        }
    })

    /*
    * @tc.number: ACTS_AJsServiceAbility_1400
    * @tc.name: particleability.ConnectAbility : Connects an ability to a Service ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_AJsServiceAbility_1400', 0, async function (done) {
        console.log('ACTS_AJsServiceAbility_1400====<begin');
        try {
            var mConnIdJsAsyncCallback;
            var currentAlertTimeout;
            commonEvent.createSubscriber(CommonEventSubscribeInfo1400).then(async (data) => {
                console.debug("=ACTS_AJsServiceAbility_1400 createSubscriber .then(data)=======>"
                    + ("json data【") + JSON.stringify(data) + (" 】")
                    + " ,data=" + data);
                subscriber1400 = data;
                await commonEvent.subscribe(subscriber1400, async (err, data) => {
                    console.debug("=ACTS_AJsServiceAbility_1400 subscribe (err,data)=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】")
                        + ("json data【") + JSON.stringify(data) + (" 】")
                        + " ,err=" + err + " ,data=" + data);
                    if (data.event != "ACTS_ASerivceAbilityServerSecond_onDisConnect") {
                        expect("ACTS_ASerivceAbilityServerSecond_onConnect_ServiceConnectService_1400"
                        ).assertEqual(data.event);
                        featureAbility.disconnectAbility(mConnIdJsAsyncCallback, (err) => {
                            console.debug("=ACTS_AJsServiceAbility_1400 disconnectAbility err====>"
                                + ("json err=") + JSON.stringify(err));
                        })
                    } else {
                        clearTimeout(currentAlertTimeout);
                        expect("ACTS_ASerivceAbilityServerSecond_onDisConnect").assertEqual(
                            data.event);
                        unsubscribe("ACTS_AJsServiceAbility_1400_unsubscribe", subscriber1400);
                        console.log('ACTS_AJsServiceAbility_1400====<end')
                        done();
                    }
                });
            })
            function onConnectCallback(element, remote) {
                console.debug('ACTS_AJsServiceAbility_1400_onConnectCallback ====> mConnIdJsAsyncCallback='
                    + JSON.stringify(mConnIdJsAsyncCallback) + " , " + mConnIdJsAsyncCallback);
                console.debug('ACTS_AJsServiceAbility_1400_onConnectCallback ====> element='
                    + JSON.stringify(element) + " , " + element);
                console.debug('ACTS_AJsServiceAbility_1400_onConnectCallback ====> remote='
                    + JSON.stringify(remote) + " , " + remote);
                console.debug('ACTS_AJsServiceAbility_1400_onConnectCallback ====> remote is proxy:'
                    + (remote instanceof rpc.RemoteProxy));
            }

            function onDisconnectCallback(element) {
                console.debug('ACTS_AJsServiceAbility_1400_onDisconnectCallback ====> element='
                    + JSON.stringify(element) + " , " + element);
            }

            function onFailedCallback(code) {
                console.debug('ACTS_AJsServiceAbility_1400_onFailedCallback ====> code='
                    + JSON.stringify(code) + " , " + code)
            }

            mConnIdJsAsyncCallback = featureAbility.connectAbility(
                {
                    bundleName: bundleName,
                    abilityName: abilityName,
                    action: "ServiceConnectService_1400",
                },
                {
                    onConnect: onConnectCallback,
                    onDisconnect: onDisconnectCallback,
                    onFailed: onFailedCallback,
                },
            )

            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_AJsServiceAbility_1400====<end mySetTimeout')
                done();
            }
        } catch (err) {
            clearTimeout(currentAlertTimeout);
            console.log('ACTS_AJsServiceAbility_1400====<end err' + err)
            done();
        }
    })
})