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
import featureAbility from '@ohos.ability.featureAbility'
import wantconstant from '@ohos.ability.wantConstant'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import commonEvent from '@ohos.commonEvent'
import notification from '@ohos.notification';
import wantAgent from '@ohos.wantAgent';
import particleAbility from '@ohos.ability.particleAbility'
import backgroundTaskManager from '@ohos.backgroundTaskManager'

const START_ABILITY_TIMEOUT = 4000;
const TERMINATE_ABILITY_TIMEOUT = 1000;
const TIMEOUT = 1000;
var subscriberInfo_ACTS_StartAbility_0100 = {
    events: ["ACTS_StartAbility_0100_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbility_0200 = {
    events: ["ACTS_StartAbility_0200_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbility_0300 = {
    events: ["ACTS_StartAbility_0300_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbility_0400 = {
    events: ["ACTS_StartAbility_0400_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbility_0500 = {
    events: ["ACTS_StartAbility_0500_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbility_0600 = {
    events: ["ACTS_StartAbility_0600_CommonEvent"],
};
var subscriberInfoStartAbilityTen = {
    events: ["ACTS_StartAbility_1000_CommonEvent"],
};
var subscriberInfoStartAbilityThirteen = {
    events: ["ACTS_StartAbility_1300_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbilityForResult_0100 = {
    events: ["ACTS_StartAbilityForResult_0100_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbilityForResult_0200 = {
    events: ["ACTS_StartAbilityForResult_0200_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbilityForResult_0300 = {
    events: ["ACTS_StartAbilityForResult_0300_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbilityForResult_0400 = {
    events: ["ACTS_StartAbilityForResult_0400_CommonEvent"],
};
var subscriberInfo_ACTS_StartAbilityForResult_0500 = {
    events: ["ACTS_StartAbilityForResult_0500_CommonEvent"],
};
var subscriberInfoStartAbilityForResultSix = {
    events: ["ACTS_StartAbilityForResult_0600_CommonEvent"],
};
var subscriberInfo_ACTS_TerminateAbility_0100 = {
    events: ["ACTS_TerminateAbility_0100_CommonEvent",
        "ACTS_TerminateAbility_0100_Return"],
};
var subscriberInfoTerminateAbilityTwo = {
    events: ["ACTS_TerminateAbility_0200_CommonEvent",
        "ACTS_TerminateAbility_0200_Return"],
};
var subscriberInfo_ACTS_FinishWithResult_0100 = {
    events: ["ACTS_FinishWithResult_0100_CommonEvent"],
};
var subscriberInfo_ACTS_GetCallingBundle_0100 = {
    events: ["ACTS_GetCallingBundle_0100_CommonEvent",
        "com.example.actsfeatureabilitytest.promise",
        "com.example.actsfeatureabilitytest.callback"],
};

describe('ActsFeatureAbilityTest', function () {

    beforeAll(function() {

        /*
         * @tc.setup: setup invoked before all testcases
         */
         console.info('beforeAll called')
    })

    afterAll(function() {

        /*
         * @tc.teardown: teardown invoked after all testcases
         */
         console.info('afterAll called')
    })

    beforeEach(function() {

        /*
         * @tc.setup: setup invoked before each testcases
         */
         console.info('beforeEach called')
    })

    afterEach(function() {

        /*
         * @tc.teardown: teardown invoked after each testcases
         */
         console.info('afterEach called')
         particleAbility.cancelBackgroundRunning();
         setTimeout(() => {}, 500);
         backgroundTaskManager.stopBackgroundRunning(featureAbility.getContext());
         setTimeout(() => {}, 500);
    })

    /*
     * @tc.name:Acts_ContinuousTask_0100
     * @tc.desc:verify new startBackgroundrunning interface promise mode work properly
     * @tc.type: FUNC
     * @tc.require: SR000GGT7U AR000GH6ER AR000GH6EM AR000GH6EN AR000GH6EO
     */
    it("Acts_ContinuousTask_0100", 0, async function (done) {
        console.log("Acts_ContinuousTask_0100 start");
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.example.actsfeatureabilitytest",
                    abilityName: "com.example.actsfeatureabilitytest.MainAbility"
                }
            ],
            operationType: 2,
            requestCode: 0,
            wantAgentFlags: [3]
        };
        wantAgent.getWantAgent(wantAgentInfo).then((data) => {
            backgroundTaskManager.startBackgroundRunning(featureAbility.getContext(),
                backgroundTaskManager.BackgroundMode.DATA_TRANSFER, data).then(() => {
                console.log("Acts_ContinuousTask_0100 startBackgroundRunning success");
                expect(true).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }).catch((err) => {
                expect(false).assertTrue();
                console.log("Acts_ContinuousTask_0100 startBackgroundRunning failure");
                setTimeout(() => {
                    done();
                }, 500);
            });
        });
    })

    /*
     * @tc.name:Acts_ContinuousTask_0200
     * @tc.desc:verify new startBackgroundrunning interface callback mode work properly
     * @tc.type: FUNC
     * @tc.require: SR000GGT7T AR000GH6ER AR000GH6EP AR000GJ9PR AR000GH6G8
     */
    it("Acts_ContinuousTask_0200", 0, async function (done) {
        console.log("Acts_ContinuousTask_0200 start");
        function conTaskCallback(err, data) {
            if (err) {
                console.info('Acts_ContinuousTask_0200 startBackgroundRunning failed');
                expect(false).assertTrue();
            } else {
                console.info('Acts_ContinuousTask_0200 startBackgroundRunning succeed');
                expect(true).assertTrue();
            }
            setTimeout(()=>{
                done();
            }, 500);
        }
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.example.actsfeatureabilitytest",
                    abilityName: "com.example.actsfeatureabilitytest.MainAbility"
                }
            ],
            operationType: 2,
            requestCode: 0,
            wantAgentFlags: [3]
        };
        wantAgent.getWantAgent(wantAgentInfo).then((data) => {
            backgroundTaskManager.startBackgroundRunning(featureAbility.getContext(),
                backgroundTaskManager.BackgroundMode.DATA_TRANSFER, data, conTaskCallback);
        });
    })

    /*
     * @tc.name:Acts_ContinuousTask_0300
     * @tc.desc:verify old startBackgroundrunning interface promise mode work properly
     * @tc.type: FUNC
     * @tc.require: SR000GGT7V AR000GH6ER AR000GH6EM AR000GH6G9 AR000GH56K
     */
    it("Acts_ContinuousTask_0300", 0, async function (done) {
        console.log("Acts_ContinuousTask_0300 start");
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.example.actsfeatureabilitytest",
                    abilityName: "com.example.actsfeatureabilitytest.MainAbility"
                }
            ],
            operationType: 2,
            requestCode: 0,
            wantAgentFlags: [3]
        };
        wantAgent.getWantAgent(wantAgentInfo).then((data) => {
            let basicContent = {
                title: "title",
                text: "text"
            };

            let notificationContent = {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: basicContent
            };

            let request = {
                content: notificationContent,
                wantAgent: data
            }

            let id = 1;

            particleAbility.startBackgroundRunning(id, request).then((data) => {
                console.log("Acts_ContinuousTask_0300 startBackgroundRunning success");
                expect(true).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }).catch((err) => {
                expect(false).assertTrue();
                console.log("Acts_ContinuousTask_0300 startBackgroundRunning failure");
                setTimeout(() => {
                    done();
                }, 500);
            });
        });
    })

    /*
     * @tc.name:Acts_ContinuousTask_0400
     * @tc.desc:verify old startBackgroundrunning interface callback mode work properly
     * @tc.type: FUNC
     * @tc.require: SR000GGT81 AR000GH6ER AR000GH6EM AR000GH6G9 AR000GH6ET
     */
    it("Acts_ContinuousTask_0400", 0, async function (done) {
        console.log("Acts_ContinuousTask_0400 start");
        function conTaskCallback(err, data) {
            if (err) {
                console.info('Acts_ContinuousTask_0400 startBackgroundRunning failure');
                expect(false).assertTrue();
            } else {
                console.info('Acts_ContinuousTask_0400 startBackgroundRunning success');
                expect(true).assertTrue();
            }
            setTimeout(()=>{
                done();
            }, 500);
        }
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.example.actsfeatureabilitytest",
                    abilityName: "com.example.actsfeatureabilitytest.MainAbility"
                }
            ],
            operationType: 2,
            requestCode: 0,
            wantAgentFlags: [3]
        };
        wantAgent.getWantAgent(wantAgentInfo).then((data) => {
            let basicContent = {
                title: "title",
                text: "text"
            };

            let notificationContent = {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: basicContent
            };

            let request = {
                content: notificationContent,
                wantAgent: data
            }

            let id = 1;

            particleAbility.startBackgroundRunning(id, request, conTaskCallback);
        });
    })

    /*
     * @tc.name:Acts_ContinuousTask_0500
     * @tc.desc:verify new api stopBackgroundrunning interface promise mode work properly
     * @tc.type: FUNC
     * @tc.require: SR000GGT7U AR000GH6ES AR000GH6EM AR000GH6EN AR000GH6EO
     */
    it("Acts_ContinuousTask_0500", 0, async function (done) {
        console.log("Acts_ContinuousTask_0500 start");
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.example.actsfeatureabilitytest",
                    abilityName: "com.example.actsfeatureabilitytest.MainAbility"
                }
            ],
            operationType: 2,
            requestCode: 0,
            wantAgentFlags: [3]
        };
        wantAgent.getWantAgent(wantAgentInfo).then((data) => {
            backgroundTaskManager.startBackgroundRunning(featureAbility.getContext(),
                backgroundTaskManager.BackgroundMode.DATA_TRANSFER, data).then((data) => {
                backgroundTaskManager.stopBackgroundRunning(featureAbility.getContext()).then((data) => {
                    console.log("Acts_ContinuousTask_0500 cancelBackgroundRunning success");
                    expect(true).assertTrue();
                    setTimeout(() => {
                        done();
                    }, 500);
                }).catch((err) => {
                    expect(false).assertTrue();
                    console.log("Acts_ContinuousTask_0500 cancelBackgroundRunning failure");
                    setTimeout(() => {
                        done();
                    }, 500);
                });
            })
        });
    })

    /*
        * @tc.name:Acts_ContinuousTask_0600
        * @tc.desc:verify new api stopBackgroundrunning interface callback mode work properly
        * @tc.type: FUNC
        * @tc.require: SR000GGT7T AR000GH6ES AR000GH6EP AR000GJ9PR AR000GH6G8
        */
    it("Acts_ContinuousTask_0600", 0, async function (done) {
        console.log("Acts_ContinuousTask_0600 start");
        function conTaskCallback(err, data) {
            if (err) {
                console.info('Acts_ContinuousTask_0600 startBackgroundRunning failure');
                expect(false).assertTrue();
            } else {
                console.info('Acts_ContinuousTask_0600 startBackgroundRunning success');
                expect(true).assertTrue();
            }
            setTimeout(()=>{
                done();
            }, 500);
        }
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.example.actsfeatureabilitytest",
                    abilityName: "com.example.actsfeatureabilitytest.MainAbility"
                }
            ],
            operationType: 2,
            requestCode: 0,
            wantAgentFlags: [3]
        };
        wantAgent.getWantAgent(wantAgentInfo).then((data) => {
            backgroundTaskManager.startBackgroundRunning(featureAbility.getContext(),
                backgroundTaskManager.BackgroundMode.DATA_TRANSFER, data).then((data) => {
                backgroundTaskManager.stopBackgroundRunning(featureAbility.getContext(), conTaskCallback);
            })
        });
    })

    /*
     * @tc.name:Acts_ContinuousTask_0700
     * @tc.desc:verify old api cancelBackgroundrunning interface promise mode work properly
     * @tc.type: FUNC
     * @tc.require: SR000GGT7V AR000GH6ES AR000GH6EM AR000GH6G9 AR000GH56K
     */
    it("Acts_ContinuousTask_0700", 0, async function (done) {
        console.log("Acts_ContinuousTask_0700 start");
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.example.actsfeatureabilitytest",
                    abilityName: "com.example.actsfeatureabilitytest.MainAbility"
                }
            ],
            operationType: 2,
            requestCode: 0,
            wantAgentFlags: [3]
        };
        await wantAgent.getWantAgent(wantAgentInfo).then((data) => {
            particleAbility.startBackgroundRunning(data);
            setTimeout(()=>{
            }, 500);
        });

        particleAbility.cancelBackgroundRunning().then(() => {
            console.log("Acts_ContinuousTask_0700 cancelBackgroundRunning success");
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }).catch( (err) => {
            expect(false).assertTrue();
            console.log("Acts_ContinuousTask_0700 cancelBackgroundRunning failure");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:Acts_ContinuousTask_0800
     * @tc.desc:verify old cancelBackgroundrunning interface callback mode work properly
     * @tc.type: FUNC
     * @tc.require: SR000GGT81 AR000GH6ES AR000GH6EM AR000GH6G9 AR000GH6ET
     */
    it("Acts_ContinuousTask_0800", 0, async function (done) {
        console.log("Acts_ContinuousTask_0800 start");
        function conTaskCallback(err, data) {
            if (err) {
                console.info('Acts_ContinuousTask_0800 startBackgroundRunning failure');
                expect(false).assertTrue();
            } else {
                console.info('Acts_ContinuousTask_0800 startBackgroundRunning success');
                expect(true).assertTrue();
            }
            setTimeout(()=>{
                done();
            }, 500);
        }
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.example.actsfeatureabilitytest",
                    abilityName: "com.example.actsfeatureabilitytest.MainAbility"
                }
            ],
            operationType: 2,
            requestCode: 0,
            wantAgentFlags: [3]
        };
        await wantAgent.getWantAgent(wantAgentInfo).then((data) => {
            particleAbility.startBackgroundRunning(data);
            setTimeout(()=>{
            }, 500);
        });

        particleAbility.cancelBackgroundRunning(conTaskCallback);
    })

    /**
     * @tc.number: ACTS_wantConstant_0100
     * @tc.name: wantConstant : Check specific enum
     * @tc.desc: Check the return type of the interface (by Promise)
     */
    it('ACTS_wantConstant_0100', 0, async function (done) {
        expect(wantconstant.Action.ACTION_HOME).assertEqual("ohos.want.action.home");
        expect(wantconstant.Action.ACTION_DIAL).assertEqual("ohos.want.action.dial");
        expect(wantconstant.Action.ACTION_SEARCH).assertEqual("ohos.want.action.search");
        expect(wantconstant.Action.ACTION_WIRELESS_SETTINGS).assertEqual("ohos.settings.wireless");
        expect(wantconstant.Action.ACTION_MANAGE_APPLICATIONS_SETTINGS).assertEqual("ohos.settings.manage.applications");
        expect(wantconstant.Action.ACTION_APPLICATION_DETAILS_SETTINGS).assertEqual("ohos.settings.application.details");
        expect(wantconstant.Action.ACTION_SET_ALARM).assertEqual("ohos.want.action.setAlarm");
        expect(wantconstant.Action.ACTION_SHOW_ALARMS).assertEqual("ohos.want.action.showAlarms");
        expect(wantconstant.Action.ACTION_SNOOZE_ALARM).assertEqual("ohos.want.action.snoozeAlarm");
        expect(wantconstant.Action.ACTION_DISMISS_ALARM).assertEqual("ohos.want.action.dismissAlarm");
        expect(wantconstant.Action.ACTION_DISMISS_TIMER).assertEqual("ohos.want.action.dismissTimer");
        expect(wantconstant.Action.ACTION_SEND_SMS).assertEqual("ohos.want.action.sendSms");
        expect(wantconstant.Action.ACTION_CHOOSE).assertEqual("ohos.want.action.choose");
        expect(wantconstant.Action.ACTION_SELECT).assertEqual("ohos.want.action.select");
        expect(wantconstant.Action.ACTION_SEND_DATA).assertEqual("ohos.want.action.sendData");
        expect(wantconstant.Action.ACTION_SEND_MULTIPLE_DATA).assertEqual("ohos.want.action.sendMultipleData");
        expect(wantconstant.Action.ACTION_SCAN_MEDIA_FILE).assertEqual("ohos.want.action.scanMediaFile");
        expect(wantconstant.Action.ACTION_VIEW_DATA).assertEqual("ohos.want.action.viewData");
        expect(wantconstant.Action.ACTION_EDIT_DATA).assertEqual("ohos.want.action.editData");
        expect(wantconstant.Action.INTENT_PARAMS_INTENT).assertEqual("ability.want.params.INTENT");
        expect(wantconstant.Action.INTENT_PARAMS_TITLE).assertEqual("ability.want.params.TITLE");
        expect(wantconstant.Action.ACTION_FILE_SELECT).assertEqual("ohos.action.fileSelect");
        expect(wantconstant.Action.PARAMS_STREAM).assertEqual("ability.params.stream");

        expect(wantconstant.Entity.ENTITY_HOME).assertEqual("entity.system.home");
        expect(wantconstant.Entity.ENTITY_DEFAULT).assertEqual("entity.system.default");
        expect(wantconstant.Entity.ENTITY_VOICE).assertEqual("entity.system.voice");
        expect(wantconstant.Entity.ENTITY_BROWSABLE).assertEqual("entity.system.browsable");
        expect(wantconstant.Entity.ENTITY_VIDEO).assertEqual("entity.system.video");
        done();
    })

    /**
     * @tc.number: ACTS_GetContext_0100
     * @tc.name: GetContext : Obtains the Context object
     * @tc.desc: Check the return value of the interface (by Promise)
     */
    it('ACTS_GetContext_0100', 0, async function (done) {
        var promise = await featureAbility.getContext()
        expect(typeof (promise)).assertEqual("object");
        done();
    })

    /**
     * @tc.number: ACTS_HasWindowFocus_0100
     * @tc.name: HasWindowFocus : Checks whether the main window of this ability has window focus
     * @tc.desc: Check the return value of the interface (by Promise)
     */
    it('ACTS_HasWindowFocus_0100', 0, async function (done) {
        var promise = featureAbility.hasWindowFocus();
        expect(typeof (promise)).assertEqual("object");
        var info = await featureAbility.hasWindowFocus();
        expect(info).assertEqual(true);
        done();
        setTimeout(function () {
            console.info('====> ACTS_HasWindowFocus_0100 =====>')
        }, TIMEOUT)
    })

    /**
     * @tc.number: ACTS_HasWindowFocus_0300
     * @tc.name: HasWindowFocus : Checks whether the main window of this ability has window focus
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     */
    it('ACTS_HasWindowFocus_0300', 0, async function (done) {
        var result = featureAbility.hasWindowFocus(
            (err, data) => {
                console.log("hasWindowFocus asyncCallback code: " + err.code + " data: " + data);
                expect(err.code).assertEqual(0);
                expect(data).assertTrue();
                done()
            }
        );
        console.info('AceApplication : hasWindowFocus : ' + result);
        setTimeout(function () {
            console.info('====> ACTS_HasWindowFocus_0300 =====>')
        }, TIMEOUT)
    })

    /*
     * @tc.number  ACTS_StartAbility_1000
     * @tc.name    The configured URI is started and the page is not configured
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_1000",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_1000-------------------");
        try{
            var Subscriber;
            let id;

            function SubscribeCallBack(err, data) {
                clearTimeout(id);
                expect(data.event).assertEqual("ACTS_StartAbility_1000_CommonEvent");
                console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
                commonEvent.unsubscribe(Subscriber, UnSubscribeCallback);
                done();
            }

            commonEvent.createSubscriber(subscriberInfoStartAbilityTen).then(async (data) => {
                console.debug("====>Create Subscriber====>");
                Subscriber = data;
                await commonEvent.subscribe(Subscriber, SubscribeCallBack);
            })

            function UnSubscribeCallback() {
                console.debug("====>UnSubscribe CallBack====>");
                done();
            }

            function timeout() {
                expect().assertFail();
                console.debug('ACTS_StartAbility_1000=====timeout======');
                commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
                done();
            }

            id = setTimeout(timeout, START_ABILITY_TIMEOUT);
            let Want = {
                bundleName: "com.example.startability",
                abilityName: "com.example.startability.MainAbility",
                uri: "xxxxx",
            }
            var StartAbilityParameter = {
                want:Want
            }

            featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
                console.log('ACTS_StartAbility_1000 asyncCallback errCode : ' + JSON.stringify(err) 
                + " data: " + JSON.stringify(data));
                expect(err.code == 0).assertTrue();
                done();
            });
        }catch(error){
            console.log("ACTS_StartAbility_1000 : error = " + error);
        }
    })

    /**
     * @tc.number: ACTS_StartAbility_0100
     * @tc.name: StartAbility : A Page or Service ability uses this method to start a specific ability.
     * @tc.desc: Check the return value of the interface (by Promise)
     */
    it('ACTS_StartAbility_0100', 0, async function (done) {
        var Subscriber;
        let id;

        function SubscribeCallBack(err, data) {
            clearTimeout(id);
            expect(data.event).assertEqual("ACTS_StartAbility_0100_CommonEvent");
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback);
            done();
        }

        commonEvent.createSubscriber(subscriberInfo_ACTS_StartAbility_0100).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_StartAbility_0100=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        var promise = await featureAbility.startAbility(
            {
                want:
                {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    type: "MIMETYPE",
                    options: {
                        // indicates the grant to perform read operations on the URI
                        authReadUriPermission: true,
                        // indicates the grant to perform write operations on the URI
                        authWriteUriPermission: true,
                        // support forward intent result to origin ability
                        abilityForwardResult: true,
                        // used for marking the ability start-up is triggered by continuation
                        abilityContinuation: true,
                        // specifies whether a component does not belong to ohos
                        notOhosComponent: true,
                        // specifies whether an ability is started
                        abilityFormEnabled: true,
                        // indicates the grant for possible persisting on the URI.
                        authPersistableUriPermission: true,
                        // indicates the grant for possible persisting on the URI.
                        authPrefixUriPermission: true,
                        // support distributed scheduling system start up multiple devices
                        abilitySliceMultiDevice: true,
                        // indicates that an ability using the service template is started regardless of whether the
                        // host application has been started.
                        startForegroundAbility: true,
                        // install the specified ability if it's not installed.
                        installOnDemand: true,
                        // return result to origin ability slice
                        abilitySliceForwardResult: true,
                        // install the specified ability with background mode if it's not installed.
                        installWithBackgroundMode: true
                    },
                    deviceId: "",
                    bundleName: "com.example.startability",
                    abilityName: "com.example.startability.MainAbility",
                    uri: ""
                },
            }
        );
        expect(promise).assertEqual(0);
    })

    /**
     * @tc.number: ACTS_StartAbility_0200
     * @tc.name: StartAbility : A Page or Service ability uses this method to start a specific ability.
     * @tc.desc: Pass the parameters, Check the return value of the interface (by Promise)
     */
    it('ACTS_StartAbility_0200', 0, async function (done) {
        var Subscriber;
        let id;

        function SubscribeCallBack(err, data) {
            clearTimeout(id);
            expect(data.event).assertEqual("ACTS_StartAbility_0200_CommonEvent");
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        commonEvent.createSubscriber(subscriberInfo_ACTS_StartAbility_0200).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_StartAbility_0200=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        console.debug('=======wait id======');
        var promise = await featureAbility.startAbility(
            {
                want:
                {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    type: "MIMETYPE",
                    options: {
                        // indicates the grant to perform read operations on the URI
                        authReadUriPermission: true,
                        // indicates the grant to perform write operations on the URI
                        authWriteUriPermission: true,
                        // support forward intent result to origin ability
                        abilityForwardResult: true,
                        // used for marking the ability start-up is triggered by continuation
                        abilityContinuation: true,
                        // specifies whether a component does not belong to ohos
                        notOhosComponent: true,
                        // specifies whether an ability is started
                        abilityFormEnabled: true,
                        // indicates the grant for possible persisting on the URI.
                        authPersistableUriPermission: true,
                        // indicates the grant for possible persisting on the URI.
                        authPrefixUriPermission: true,
                        // support distributed scheduling system start up multiple devices
                        abilitySliceMultiDevice: true,
                        // indicates that an ability using the service template is started regardless of whether the
                        // host application has been started.
                        startForegroundAbility: true,
                        // install the specified ability if it's not installed.
                        installOnDemand: true,
                        // return result to origin ability slice
                        abilitySliceForwardResult: true,
                        // install the specified ability with background mode if it's not installed.
                        installWithBackgroundMode: true
                    },
                    deviceId: "",
                    bundleName: "com.example.startability",
                    abilityName: "com.example.startability.MainAbility",
                    uri: "",
                    parameters:
                    {
                        mykey0: 1111,
                        mykey1: [1, 2, 3],
                        mykey2: "[1, 2, 3]",
                        mykey3: "xxxxxxxxxxxxxxxxxxxxxx",
                        mykey4: [1, 15],
                        mykey5: [false, true, false],
                        mykey6: ["aaaaaa", "bbbbb", "ccccccccccc"],
                        mykey7: true,
                    },
                },
            }
        );
        expect(promise).assertEqual(0);
    })

    /**
     * @tc.number: ACTS_StartAbility_0300
     * @tc.name: StartAbility : A Page or Service ability uses this method to start a specific ability.
     * @tc.desc: Passing null, Check the return value of the interface (by Promise)
     */
    it('ACTS_StartAbility_0300', 0, async function (done) {
        var Subscriber;
        let id;

        function SubscribeCallBack(err, data) {
            clearTimeout(id);
            expect(data.event).assertEqual("ACTS_StartAbility_0300_CommonEvent");
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        commonEvent.createSubscriber(subscriberInfo_ACTS_StartAbility_0300).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_StartAbility_0300=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        var promise = await featureAbility.startAbility(
            {
                want:
                {
                    action: "",
                    entities: [""],
                    type: "",
                    options: {
                        // indicates the grant to perform read operations on the URI
                        authReadUriPermission: true,
                        // indicates the grant to perform write operations on the URI
                        authWriteUriPermission: true,
                        // support forward intent result to origin ability
                        abilityForwardResult: true,
                        // used for marking the ability start-up is triggered by continuation
                        abilityContinuation: true,
                        // specifies whether a component does not belong to ohos
                        notOhosComponent: true,
                        // specifies whether an ability is started
                        abilityFormEnabled: true,
                        // indicates the grant for possible persisting on the URI.
                        authPersistableUriPermission: true,
                        // indicates the grant for possible persisting on the URI.
                        authPrefixUriPermission: true,
                        // support distributed scheduling system start up multiple devices
                        abilitySliceMultiDevice: true,
                        // indicates that an ability using the service template is started regardless of whether the
                        // host application has been started.
                        startForegroundAbility: true,
                        // install the specified ability if it's not installed.
                        installOnDemand: true,
                        // return result to origin ability slice
                        abilitySliceForwardResult: true,
                        // install the specified ability with background mode if it's not installed.
                        installWithBackgroundMode: true
                    },
                    deviceId: "",
                    bundleName: "com.example.startability",
                    abilityName: "com.example.startability.MainAbility",
                    uri: ""
                },
            }
        );
        expect(promise).assertEqual(0);
    })

    /**
     * @tc.number: ACTS_StartAbility_0400
     * @tc.name: StartAbility : A Page or Service ability uses this method to start a specific ability.
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     */
    it('ACTS_StartAbility_0400', 0, async function (done) {
        var Subscriber;
        let id;

        function SubscribeCallBack(err, data) {
            clearTimeout(id);
            expect(data.event).assertEqual("ACTS_StartAbility_0400_CommonEvent");
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        commonEvent.createSubscriber(subscriberInfo_ACTS_StartAbility_0400).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_StartAbility_0400=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        var result = featureAbility.startAbility(
            {
                want:
                {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    type: "MIMETYPE",
                    options: {
                        // indicates the grant to perform read operations on the URI
                        authReadUriPermission: true,
                        // indicates the grant to perform write operations on the URI
                        authWriteUriPermission: true,
                        // support forward intent result to origin ability
                        abilityForwardResult: true,
                        // used for marking the ability start-up is triggered by continuation
                        abilityContinuation: true,
                        // specifies whether a component does not belong to ohos
                        notOhosComponent: true,
                        // specifies whether an ability is started
                        abilityFormEnabled: true,
                        // indicates the grant for possible persisting on the URI.
                        authPersistableUriPermission: true,
                        // indicates the grant for possible persisting on the URI.
                        authPrefixUriPermission: true,
                        // support distributed scheduling system start up multiple devices
                        abilitySliceMultiDevice: true,
                        // indicates that an ability using the service template is started regardless of whether the
                        // host application has been started.
                        startForegroundAbility: true,
                        // install the specified ability if it's not installed.
                        installOnDemand: true,
                        // return result to origin ability slice
                        abilitySliceForwardResult: true,
                        // install the specified ability with background mode if it's not installed.
                        installWithBackgroundMode: true
                    },
                    deviceId: "",
                    bundleName: "com.example.startability",
                    abilityName: "com.example.startability.MainAbility",
                    uri: ""
                },
            },
            (error, data) => {
                console.log('featureAbilityTest startAbility asyncCallback errCode : ' + error + " data: " + data)

            },
        );
    })

    /**
     * @tc.number: ACTS_StartAbility_0500
     * @tc.name: StartAbility : A Page or Service ability uses this method to start a specific ability.
     * @tc.desc: Pass the parameters, Check the return value of the interface (by AsyncCallback)
     */
    it('ACTS_StartAbility_0500', 0, async function (done) {
        var Subscriber;
        let id;

        function SubscribeCallBack(err, data) {
            clearTimeout(id);
            expect(data.event).assertEqual("ACTS_StartAbility_0500_CommonEvent");
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        commonEvent.createSubscriber(subscriberInfo_ACTS_StartAbility_0500).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_StartAbility_0500=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        var result = featureAbility.startAbility(
            {
                want:
                {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    type: "MIMETYPE",
                    options: {
                        // indicates the grant to perform read operations on the URI
                        authReadUriPermission: true,
                        // indicates the grant to perform write operations on the URI
                        authWriteUriPermission: true,
                        // support forward intent result to origin ability
                        abilityForwardResult: true,
                        // used for marking the ability start-up is triggered by continuation
                        abilityContinuation: true,
                        // specifies whether a component does not belong to ohos
                        notOhosComponent: true,
                        // specifies whether an ability is started
                        abilityFormEnabled: true,
                        // indicates the grant for possible persisting on the URI.
                        authPersistableUriPermission: true,
                        // indicates the grant for possible persisting on the URI.
                        authPrefixUriPermission: true,
                        // support distributed scheduling system start up multiple devices
                        abilitySliceMultiDevice: true,
                        // indicates that an ability using the service template is started regardless of whether the
                        // host application has been started.
                        startForegroundAbility: true,
                        // install the specified ability if it's not installed.
                        installOnDemand: true,
                        // return result to origin ability slice
                        abilitySliceForwardResult: true,
                        // install the specified ability with background mode if it's not installed.
                        installWithBackgroundMode: true
                    },
                    deviceId: "",
                    bundleName: "com.example.startability",
                    abilityName: "com.example.startability.MainAbility",
                    uri: ""
                },
                parameters:
                {
                    mykey0: 1111,
                    mykey1: [1, 2, 3],
                    mykey2: "[1, 2, 3]",
                    mykey3: "xxxxxxxxxxxxxxxxxxxxxx",
                    mykey4: [1, 15],
                    mykey5: [false, true, false],
                    mykey6: ["aaaaaa", "bbbbb", "ccccccccccc"],
                    mykey7: true,
                },
            },
            (error, data) => {
                console.log('featureAbilityTest startAbility asyncCallback errCode : ' + error + " data: " + data)

            },
        );
    })

    /**
     * @tc.number: ACTS_StartAbility_0600
     * @tc.name: StartAbility : A Page or Service ability uses this method to start a specific ability.
     * @tc.desc: Passing null, Check the return value of the interface (by AsyncCallback)
     */
    it('ACTS_StartAbility_0600', 0, async function (done) {
        var Subscriber;
        let id;

        function SubscribeCallBack(err, data) {
            clearTimeout(id);
            expect(data.event).assertEqual("ACTS_StartAbility_0600_CommonEvent");
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        commonEvent.createSubscriber(subscriberInfo_ACTS_StartAbility_0600).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_StartAbility_0600=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        var result = featureAbility.startAbility(
            {
                want:
                {
                    action: "",
                    entities: [""],
                    type: "",
                    options: {
                        // indicates the grant to perform read operations on the URI
                        authReadUriPermission: true,
                        // indicates the grant to perform write operations on the URI
                        authWriteUriPermission: true,
                        // support forward intent result to origin ability
                        abilityForwardResult: true,
                        // used for marking the ability start-up is triggered by continuation
                        abilityContinuation: true,
                        // specifies whether a component does not belong to ohos
                        notOhosComponent: true,
                        // specifies whether an ability is started
                        abilityFormEnabled: true,
                        // indicates the grant for possible persisting on the URI.
                        authPersistableUriPermission: true,
                        // indicates the grant for possible persisting on the URI.
                        authPrefixUriPermission: true,
                        // support distributed scheduling system start up multiple devices
                        abilitySliceMultiDevice: true,
                        // indicates that an ability using the service template is started regardless of whether the
                        // host application has been started.
                        startForegroundAbility: true,
                        // install the specified ability if it's not installed.
                        installOnDemand: true,
                        // return result to origin ability slice
                        abilitySliceForwardResult: true,
                        // install the specified ability with background mode if it's not installed.
                        installWithBackgroundMode: true
                    },
                    deviceId: "",
                    bundleName: "com.example.startability",
                    abilityName: "com.example.startability.MainAbility",
                    uri: ""
                },
            },
            (error, data) => {
                console.log('featureAbilityTest startAbility asyncCallback errCode : ' + error + " data: " + data)

            },
        );
    })

    /**
     * @tc.number: ACTS_StartAbilityForResult_0100
     * @tc.name: StartAbilityForResult : Start other ability for result.
     * @tc.desc: Check the return value of the interface (by Promise)
     */
    it('ACTS_StartAbilityForResult_0100', 0, async function (done) {
        var promise = await featureAbility.startAbilityForResult(
            {
                want:
                {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    type: "MIMETYPE",
                    options: {
                        // indicates the grant to perform read operations on the URI
                        authReadUriPermission: true,
                        // indicates the grant to perform write operations on the URI
                        authWriteUriPermission: true,
                        // support forward intent result to origin ability
                        abilityForwardResult: true,
                        // used for marking the ability start-up is triggered by continuation
                        abilityContinuation: true,
                        // specifies whether a component does not belong to ohos
                        notOhosComponent: true,
                        // specifies whether an ability is started
                        abilityFormEnabled: true,
                        // indicates the grant for possible persisting on the URI.
                        authPersistableUriPermission: true,
                        // indicates the grant for possible persisting on the URI.
                        authPrefixUriPermission: true,
                        // support distributed scheduling system start up multiple devices
                        abilitySliceMultiDevice: true,
                        // indicates that an ability using the service template is started regardless of whether the
                        // host application has been started.
                        startForegroundAbility: true,
                        // install the specified ability if it's not installed.
                        installOnDemand: true,
                        // return result to origin ability slice
                        abilitySliceForwardResult: true,
                        // install the specified ability with background mode if it's not installed.
                        installWithBackgroundMode: true
                    },
                    deviceId: "",
                    bundleName: "com.example.startabilityforresult",
                    abilityName: "com.example.startabilityforresult.MainAbility",
                    uri: ""
                }
            }
        );
        console.info('====> ACTS_StartAbilityForResult_0100 start ability=====>' + JSON.stringify(promise))
        checkOnAbilityResult(promise);
        done();
    })

    function checkOnAbilityResult(data) {
        expect(typeof (data)).assertEqual("object");
        expect(typeof (data.resultCode)).assertEqual("number");
        expect(typeof (data.want.action)).assertEqual("string");
        expect(Array.isArray(data.want.entities)).assertEqual(true);
        expect(typeof (data.want.type)).assertEqual("string");
        expect(typeof (data.want.deviceId)).assertEqual("string");
        expect(typeof (data.want.bundleName)).assertEqual("string");
        expect(typeof (data.want.abilityName)).assertEqual("string");
        expect(typeof (data.want.uri)).assertEqual("string");

        console.info('featureAbilityTest onAbilityResult asyncCallback success : *************');
        console.info('resultCode : ' + data.resultCode);
        console.info('want.action : ' + data.want.action);
        console.info('want.entities.length : ' + data.want.entities.length);
        for (var j = 0; j < data.want.entities.length; j++) {
            console.info('want.entities : ' + data.want.entities[j]);
        }
        console.info('want.type : ' + data.want.type);
        console.info('want.bundleName : ' + data.want.bundleName);
        console.info('want.abilityName : ' + data.want.abilityName);
        console.info('want.uri : ' + data.want.uri);
    }

    /**
     * @tc.number: ACTS_HasWindowFocus_0200
     * @tc.name: HasWindowFocus : Checks whether the main window of this ability has window focus
     * @tc.desc: Check the return value of the interface (by Promise)
     */
    it('ACTS_HasWindowFocus_0200', 0, async function (done) {
        console.info('====>ACTS_HasWindowFocus_0200 start=====>')
        featureAbility.startAbility(
            {
                want:
                {
                    bundleName: "com.example.getcallingbundlepromisetest",
                    abilityName: "com.example.getcallingbundlepromisetest.MainAbility",
                },
            }, (err, data)=>{
                console.info('====>ACTS_HasWindowFocus_0200 startAbility err: '+ JSON.stringify(err))
                setTimeout(async function () {
                    console.info('====>ACTS_HasWindowFocus_0200 =====>')
                    var info = await featureAbility.hasWindowFocus();
                    console.info('====>ACTS_HasWindowFocus_0200 info ' + info);
                    expect(info).assertEqual(false);
                    done();
                }, TIMEOUT)
            }
        );
    })

    /**
     * @tc.number: ACTS_HasWindowFocus_0400
     * @tc.name: HasWindowFocus : Checks whether the main window of this ability has window focus
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     */
    it('ACTS_HasWindowFocus_0400', 0, async function (done) {
        console.info('====>ACTS_HasWindowFocus_0400 start=====>')
        featureAbility.startAbility(
            {
                want:
                {
                    bundleName: "com.example.getcallingbundlepromisetest",
                    abilityName: "com.example.getcallingbundlepromisetest.MainAbility",
                },
            }, (err, data)=>{
                console.info('====>ACTS_HasWindowFocus_0400 startAbility err: '+ JSON.stringify(err))
                setTimeout(async function () {
                    console.info('====> ACTS_HasWindowFocus_0400 =====>')
                    featureAbility.hasWindowFocus(
                    (error, data)=>{
                        console.log("ACTS_HasWindowFocus_0400 asyncCallback code: " + error.code + " data: " + data)
                        expect(error.code).assertEqual(0);
                        expect(data).assertEqual(false);
                        done();
                    });
                }, TIMEOUT)
            }
        );
    })

    /**
     * @tc.number: ACTS_TerminateAbility_0100
     * @tc.name: TerminateAbility : Destroys ability
     * @tc.desc: Check the return value of the interface (by Promise)
     */
    it('ACTS_TerminateAbility_0100', 0, async function (done) {
        var Subscriber;
        let id;
        let events = new Map();

        function SubscribeCallBack(err, data) {
            clearTimeout(id);
            events.set(data.event, 0);
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            if (events.size > 1) {
                checkResult();
            } else {
                expect(events.has("ACTS_TerminateAbility_0100_CommonEvent") ||
                    events.has("ACTS_TerminateAbility_0100_Return")).assertTrue();
            }
        }

        commonEvent.createSubscriber(subscriberInfo_ACTS_TerminateAbility_0100).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_TerminateAbility_0100=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        function checkResult() {
            console.debug('==========ACTS_TerminateAbility_0100==========checkResult');
            expect(events.has("ACTS_TerminateAbility_0100_CommonEvent")).assertTrue();
            expect(events.has("ACTS_TerminateAbility_0100_Return")).assertTrue();
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        var promise = featureAbility.startAbility(
            {
                want:
                {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    type: "MIMETYPE",
                    options: {
                        // indicates the grant to perform read operations on the URI
                        authReadUriPermission: true,
                        // indicates the grant to perform write operations on the URI
                        authWriteUriPermission: true,
                        // support forward intent result to origin ability
                        abilityForwardResult: true,
                        // used for marking the ability start-up is triggered by continuation
                        abilityContinuation: true,
                        // specifies whether a component does not belong to ohos
                        notOhosComponent: true,
                        // specifies whether an ability is started
                        abilityFormEnabled: true,
                        // indicates the grant for possible persisting on the URI.
                        authPersistableUriPermission: true,
                        // indicates the grant for possible persisting on the URI.
                        authPrefixUriPermission: true,
                        // support distributed scheduling system start up multiple devices
                        abilitySliceMultiDevice: true,
                        // indicates that an ability using the service template is started regardless of whether the
                        // host application has been started.
                        startForegroundAbility: true,
                        // install the specified ability if it's not installed.
                        installOnDemand: true,
                        // return result to origin ability slice
                        abilitySliceForwardResult: true,
                        // install the specified ability with background mode if it's not installed.
                        installWithBackgroundMode: true
                    },
                    deviceId: "",
                    bundleName: "com.example.terminateabilitytest",
                    abilityName: "com.example.terminateabilitytest.MainAbility",
                    uri: ""
                },
            }
        );
        expect(typeof (promise)).assertEqual("object");
    })

    /**
     * @tc.number: ACTS_TerminateAbility_0200
     * @tc.name: TerminateAbility : Destroys ability
     * @tc.desc: Check the return value of the interface (by promise)
     */
    it('ACTS_TerminateAbility_0200', 0, async function (done) {
        var Subscriber;
        let id;
        let events = new Map();

        function SubscribeCallBack(err, data) {
            clearTimeout(id);
            events.set(data.event, 0);
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            if (events.size > 1) {
                expect(events.has("ACTS_TerminateAbility_0200_CommonEvent") &&
                    events.has("ACTS_TerminateAbility_0200_Return")).assertTrue();
                commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
                done();
            } else {
                expect(events.has("ACTS_TerminateAbility_0200_CommonEvent") ||
                    events.has("ACTS_TerminateAbility_0200_Return")).assertTrue();
            }
        }

        commonEvent.createSubscriber(subscriberInfoTerminateAbilityTwo).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_TerminateAbility_0200=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }
        console.log('=====start ability=====');
        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        var promise = featureAbility.startAbility(
            {
                want:
                {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    type: "MIMETYPE",
                    flags: 2,
                    deviceId: "",
                    bundleName: "com.example.terminateabilitytest",
                    abilityName: "com.example.terminateabilitytest.MainAbility",
                    uri: ""
                },
            }
        );
        expect(typeof (promise)).assertEqual("object");
    })

    // checkAbilityName
    function checkAbilityName(info) {
        console.log("AbilityName name : " + info);
        expect(typeof (info)).assertEqual("string");
        expect(info).assertEqual("com.example.actsfeatureabilitytest.MainAbility");
    }

    /**
     * @tc.number: ACTS_GetAbilityName_0100
     * @tc.name: GetAbilityName : Obtains the class name in this ability name, without the prefixed bundle name.
     * @tc.desc: Check the return type of the interface (by Promise)
     */
    it('ACTS_GetAbilityName_0100', 0, async function (done) {
        var promise = featureAbility.getAbilityName();
        expect(typeof (promise)).assertEqual("object");
        var info = await featureAbility.getAbilityName();
        checkAbilityName(info);
        done();
        setTimeout(function () {
            console.info('====> ACTS_GetAbilityName_0100 =====>')
        }, TIMEOUT)
    })

    /**
     * @tc.number: ACTS_GetAbilityName_0200
     * @tc.name: GetAbilityName : Obtains the class name in this ability name, without the prefixed bundle name.
     * @tc.desc: Check the return type of the interface (by AsyncCallback)
     */
    it('ACTS_GetAbilityName_0200', 0, async function (done) {
        var result = featureAbility.getAbilityName(
            (err, data) => {
                checkAbilityName(data);
                done()
            }
        );
        setTimeout(function () {
            console.info('====> ACTS_GetAbilityName_0200 =====>')
        }, TIMEOUT)
    })

    // checkApplicationInfo
    function checkApplicationInfo(info) {
        console.log("getApplicationInfo : " + info)
        console.log("name : " + info.name);
        console.log("description : " + info.description);
        console.log("descriptionId : " + info.descriptionId);
        console.log("systemApp : " + info.systemApp);
        console.log("enabled : " + info.enabled);
        console.log("label : " + info.label);
        console.log("labelId : " + info.labelId);
        console.log("icon : " + info.icon);
        console.log("iconId : " + info.iconId);
        console.log("process : " + info.process);
        console.log("supportedModes : " + info.supportedModes);

        console.log("moduleSourceDirs length : " + info.moduleSourceDirs.length);
        for (var j = 0; j < info.moduleSourceDirs.length; j++) {
            console.log("info.moduleSourceDirs[" + j + "] : " + info.moduleSourceDirs[j]);
        }
        console.log("permissions length : " + info.permissions.length);
        for (var j = 0; j < info.permissions.length; j++) {
            console.log("info.permissions[" + j + "] : " + info.permissions[j]);
        }
        console.log("moduleInfos length : " + info.moduleInfos.length);
        for (var j = 0; j < info.moduleInfos.length; j++) {
            console.log("info.moduleInfos[" + j + "].moduleName : " + info.moduleInfos[j].moduleName);
            console.log("info.moduleInfos[" + j + "].moduleSourceDir : " + info.moduleInfos[j].moduleSourceDir);
        }
        console.log("entryDir : " + info.entryDir);

        expect(typeof (info)).assertEqual("object");
        expect(typeof (info.name)).assertEqual("string");
        expect(typeof (info.description)).assertEqual("string");
        expect(typeof (info.descriptionId)).assertEqual("number");
        expect(typeof (info.systemApp)).assertEqual("boolean");
        expect(typeof (info.enabled)).assertEqual("boolean");
        expect(typeof (info.label)).assertEqual("string");
        expect(typeof (info.labelId)).assertEqual("string");
        expect(typeof (info.icon)).assertEqual("string");
        expect(typeof (info.iconId)).assertEqual("string");
        expect(typeof (info.process)).assertEqual("string");
        expect(typeof (info.supportedModes)).assertEqual("number");
        expect(Array.isArray(info.moduleSourceDirs)).assertEqual(true);
        expect(Array.isArray(info.permissions)).assertEqual(true);
        expect(Array.isArray(info.moduleInfos)).assertEqual(true);
        expect(typeof (info.entryDir)).assertEqual("string");

        expect(info.name).assertEqual("com.example.actsfeatureabilitytest");
        expect(info.description).assertEqual("$string:mainability_description");
        //            expect(info.descriptionId).assertEqual(0);    //create by DevEco when building HAP.
        expect(info.systemApp).assertEqual(true);
        expect(info.enabled).assertEqual(true);
        expect(info.label).assertEqual("$string:app_name");
        //            expect(info.labelId).assertEqual(0);  //create by DevEco when building HAP.
        expect(info.icon).assertEqual("$media:icon");
        //            expect(info.iconId).assertEqual(0);   //create by DevEco when building HAP.
        expect(info.process).assertEqual("processTest");
        expect(info.supportedModes).assertEqual(0);
        expect(info.moduleSourceDirs[0]).assertEqual("/data/app/el1/bundle/public/" +
            "com.example.actsfeatureabilitytest/com.example.actsfeatureabilitytest");
        expect(info.moduleInfos[0].moduleName).assertEqual("entry");
        expect(info.moduleInfos[0].moduleSourceDir).assertEqual("/data/app/el1/bundle/public/" +
            "com.example.actsfeatureabilitytest/com.example.actsfeatureabilitytest");
        expect(info.entryDir).assertEqual("/data/app/el1/bundle/public/" +
            "com.example.actsfeatureabilitytest/com.example.actsfeatureabilitytest");
    }

    /**
     * @tc.number: ACTS_GetApplicationInfo_0100
     * @tc.name: GetApplicationInfo : Obtains information about the current application.
     * @tc.desc: Check the return type of the interface (by Promise)
     */
    it('ACTS_GetApplicationInfo_0100', 0, async function (done) {
        var context = featureAbility.getContext();
        var promise = context.getApplicationInfo();
        expect(typeof (promise)).assertEqual("object");
        var info = await context.getApplicationInfo();
        checkApplicationInfo(info);
        done();
        setTimeout(function () {
            console.info('====> ACTS_GetApplicationInfo_0100 =====>')
        }, TIMEOUT)
    })

    /**
     * @tc.number: ACTS_GetApplicationInfo_0200
     * @tc.name: GetApplicationInfo : Obtains information about the current application.
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     */
    it('ACTS_GetApplicationInfo_0200', 0, async function (done) {
        var context = featureAbility.getContext()
        var result = context.getApplicationInfo(
            (err, data) => {
                checkApplicationInfo(data);
                done()
            }
        );
        setTimeout(function () {
            console.info('====> ACTS_GetApplicationInfo_0200 =====>')
        }, TIMEOUT)
    })

    // checkProcessInfo
    function checkProcessInfo(info) {
        console.log("checkProcessInfo ProcessInfo: " + info);
        console.log("checkProcessInfo processName : " + info.processName);
        console.log("checkProcessInfo pid : " + info.pid);

        expect(typeof (info)).assertEqual("object");
        expect(typeof (info.processName)).assertEqual("string");
        expect(typeof (info.pid)).assertEqual("number");
        expect(info.processName).assertEqual("processTestAbility");
    }

    /**
     * @tc.number: ACTS_GetProcessInfo_0100
     * @tc.name: GetProcessInfo : Called when getting the ProcessInfo
     * @tc.desc: Check the return type of the interface (by promise)
     */
    it('ACTS_GetProcessInfo_0100', 0, async function (done) {
        var context = featureAbility.getContext()
        var promise = context.getProcessInfo();
        expect(typeof (promise)).assertEqual("object");
        var info = await context.getProcessInfo();
        checkProcessInfo(info);
        done();
        setTimeout(function () {
            console.info('====> ACTS_GetProcessInfo_0100 =====>')
        }, TIMEOUT)
    })

    /**
     * @tc.number: ACTS_GetProcessInfo_0200
     * @tc.name: GetProcessInfo : Called when getting the ProcessInfo
     * @tc.desc: Check the return type of the interface (by AsyncCallback)
     */
    it('ACTS_GetProcessInfo_0200', 0, async function (done) {
        var context = featureAbility.getContext()
        var result = context.getProcessInfo(
            (err, data) => {
                checkProcessInfo(data);
                done()
            }
        );
        setTimeout(function () {
            console.info('====> ACTS_GetProcessInfo_0200 =====>')
        }, TIMEOUT)
    })

    // checkElementName
    function checkElementName(info) {
        console.log("getElementName : " + info);
        console.log("deviceId : " + info.deviceId);
        console.log("bundleName : " + info.bundleName);
        console.log("abilityName : " + info.abilityName);
        console.log("uri : " + info.uri);
        console.log("shortName : " + info.shortName);

        expect(typeof (info)).assertEqual("object");
        expect(typeof (info.deviceId)).assertEqual("string");
        expect(typeof (info.bundleName)).assertEqual("string");
        expect(typeof (info.abilityName)).assertEqual("string");
        expect(typeof (info.uri)).assertEqual("string");
        expect(typeof (info.shortName)).assertEqual("string");

        expect(info.deviceId).assertEqual("");
        expect(info.bundleName).assertEqual("com.example.actsfeatureabilitytest");
        expect(info.abilityName).assertEqual("com.example.actsfeatureabilitytest.MainAbility");
        expect(info.uri).assertEqual("");
        expect(info.shortName).assertEqual("");
    }

    /**
     * @tc.number: ACTS_GetElementName_0100
     * @tc.name: GetElementName : Obtains the ohos.bundle.ElementName object of the current ability.
     * @tc.desc: Check the return value of the interface (by promise)
     */
    it('ACTS_GetElementName_0100', 0, async function (done) {
        var context = featureAbility.getContext()
        var promise = context.getElementName();
        expect(typeof (promise)).assertEqual("object");
        var info = await context.getElementName();
        checkElementName(info);
        done();
        setTimeout(function () {
            console.info('====> ACTS_GetElementName_0100 =====>')
        }, TIMEOUT)
    })

    /**
    * @tc.number: ACTS_GetElementName_0200
    * @tc.name: GetElementName : Obtains the ohos.bundle.ElementName object of the current ability.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_GetElementName_0200', 0, async function (done) {
        var context = featureAbility.getContext()
        var result = context.getElementName(
            (err, data) => {
                checkElementName(data);
                done()
            }
        );
        setTimeout(function () {
            console.info('====> ACTS_GetElementName_0200 =====>')
        }, TIMEOUT)
    })

    // checkAppType
    function checkAppType(info) {
        console.log("AppType : " + info);
        expect(typeof (info)).assertEqual("string");
        expect(info).assertEqual("system");
    }

    /**
     * @tc.number: ACTS_GetAppType_0100
     * @tc.name: GetAppType : Obtains the type of this application.
     * @tc.desc: Check the return value of the interface (by promise)
     */
    it('ACTS_GetAppType_0100', 0, async function (done) {
        var promise = featureAbility.getAppType();
        expect(typeof (promise)).assertEqual("object");
        var info = await featureAbility.getAppType();
        checkAppType(info);
        done();
        setTimeout(function () {
            console.info('====> ACTS_GetAppType_0100 =====>')
        }, TIMEOUT)
    })

    /**
     * @tc.number: ACTS_GetAppType_0200
     * @tc.name: GetAppType : Obtains the type of this application.
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     */
    it('ACTS_GetAppType_0200', 0, async function (done) {
        var result = featureAbility.getAppType(
            (err, data) => {
                checkAppType(data);
                done()
            }
        );
        setTimeout(function () {
            console.info('====> ACTS_GetAppType_0200 =====>')
        }, TIMEOUT)
    })

    // checkAbilityInfo
    function checkAbilityInfo(data) {
        console.log("checkAbilityInfo start  " + data);
        console.log("checkAbilityInfo bundleName : " + data.bundleName);
        console.log("checkAbilityInfo name : " + data.name);
        console.log("checkAbilityInfo label : " + data.label);
        console.log("checkAbilityInfo description : " + data.description);
        console.log("checkAbilityInfo icon : " + data.icon);
        console.log("checkAbilityInfo labelId : " + data.labelId);
        console.log("checkAbilityInfo descriptionId : " + data.descriptionId);
        console.log("checkAbilityInfo iconId : " + data.iconId);
        console.log("checkAbilityInfo moduleName : " + data.moduleName);
        console.log("checkAbilityInfo process : " + data.process);
        console.log("checkAbilityInfo isVisible : " + data.isVisible);
        console.log("checkAbilityInfo type : " + data.type)
        console.log("checkAbilityInfo orientation : " + data.orientation);
        console.log("checkAbilityInfo launchMode : " + data.launchMode);
        console.log("checkAbilityInfo uri : " + data.uri);
        console.log("checkAbilityInfo promise readPermission : " + data.readPermission);
        console.log("checkAbilityInfo writePermission : " + data.writePermission);
        console.log("checkAbilityInfo formEntity : " + data.formEntity);
        console.log("checkAbilityInfo minFormHeight : " + data.minFormHeight);
        console.log("checkAbilityInfo defaultFormHeight : " + data.defaultFormHeight);
        console.log("checkAbilityInfo minFormWidth : " + data.minFormWidth);
        console.log("checkAbilityInfo defaultFormWidth : " + data.defaultFormWidth);
        console.log("checkAbilityInfo targetAbility : " + data.targetAbility);
        console.log("checkAbilityInfo backgroundModes : " + data.backgroundModes);
        console.log("checkAbilityInfo subType : " + data.subType);
        console.log("checkAbilityInfo formEnabled : " + data.formEnabled);

        console.log("checkAbilityInfo permissions length : " + data.permissions.length);
        for (var j = 0; j < data.permissions.length; j++) {
            console.log("getAbilityInfo data.permissions[" + j + "] : " + data.permissions[j]);
        }
        console.log("checkAbilityInfo deviceTypes length : " + data.deviceTypes.length);
        for (var j = 0; j < data.deviceTypes.length; j++) {
            console.log("getAbilityInfo data.deviceTypes[" + j + "] : " + data.deviceTypes[j]);
        }
        console.log("checkAbilityInfo deviceCapabilities length : " + data.deviceCapabilities.length);
        for (var j = 0; j < data.deviceCapabilities.length; j++) {
            console.log("getAbilityInfo data.deviceCapabilities[" + j + "] : " + data.deviceCapabilities[j]);
        }

        expect(typeof (data)).assertEqual("object");
        expect(typeof (data.bundleName)).assertEqual("string");
        expect(typeof (data.name)).assertEqual("string");
        expect(typeof (data.label)).assertEqual("string");
        expect(typeof (data.description)).assertEqual("string");
        expect(typeof (data.icon)).assertEqual("string");
        expect(typeof (data.labelId)).assertEqual("number");
        expect(typeof (data.descriptionId)).assertEqual("number");
        expect(typeof (data.iconId)).assertEqual("number");
        expect(typeof (data.moduleName)).assertEqual("string");
        expect(typeof (data.process)).assertEqual("string");
        expect(typeof (data.targetAbility)).assertEqual("string");
        expect(typeof (data.backgroundModes)).assertEqual("number");
        expect(typeof (data.isVisible)).assertEqual("boolean");
        expect(typeof (data.formEnabled)).assertEqual("boolean");
        expect(typeof (data.type)).assertEqual("number");
        expect(typeof (data.subType)).assertEqual("number");
        expect(typeof (data.orientation)).assertEqual("number");
        expect(typeof (data.launchMode)).assertEqual("number");
        expect(Array.isArray(data.permissions)).assertEqual(true);
        expect(Array.isArray(data.deviceTypes)).assertEqual(true);
        expect(Array.isArray(data.deviceCapabilities)).assertEqual(true);
        expect(typeof (data.readPermission)).assertEqual("string");
        expect(typeof (data.writePermission)).assertEqual("string");
        expect(typeof (data.applicationInfo)).assertEqual("object");
        expect(typeof (data.formEntity)).assertEqual("number");
        expect(typeof (data.minFormHeight)).assertEqual("number");
        expect(typeof (data.defaultFormHeight)).assertEqual("number");
        expect(typeof (data.minFormWidth)).assertEqual("number");
        expect(typeof (data.defaultFormWidth)).assertEqual("number");
        expect(typeof (data.uri)).assertEqual("string");


        expect(data.bundleName).assertEqual("com.example.actsfeatureabilitytest");
        expect(data.name).assertEqual("com.example.actsfeatureabilitytest.MainAbility");
        expect(data.label).assertEqual("$string:app_name");
        expect(data.description).assertEqual("$string:mainability_description");
        expect(data.icon).assertEqual("$media:icon");

        expect(data.moduleName).assertEqual("entry");
        expect(data.process).assertEqual("processTestAbility");
        expect(data.targetAbility).assertEqual("");
        expect(data.backgroundModes).assertEqual(0);
        expect(data.isVisible).assertEqual(true);
        expect(data.formEnabled).assertEqual(false);
        expect(data.type).assertEqual(1);
        expect(data.subType).assertEqual(0);
        expect(data.orientation).assertEqual(0);
        expect(data.launchMode).assertEqual(0);

        expect(data.permissions[0]).assertEqual("ohos.permission.ACCELEROMETER");
        expect(data.deviceTypes[0]).assertEqual("phone");
        expect(data.deviceCapabilities[0]).assertEqual("screen_support");
        expect(data.deviceCapabilities[1]).assertEqual("audio_support");

        expect(data.readPermission).assertEqual("");
        expect(data.writePermission).assertEqual("");
        expect(data.formEntity).assertEqual(0);
        expect(data.minFormHeight).assertEqual(0);
        expect(data.defaultFormHeight).assertEqual(0);
        expect(data.minFormWidth).assertEqual(0);
        expect(data.defaultFormWidth).assertEqual(0);
        expect(data.uri).assertEqual("uriTest");

        console.log("checkAbilityInfo end  " + data);
    }

    /**
     * @tc.number: ACTS_GetAbilityInfo_0100
     * @tc.name: GetAbilityInfo : Obtains the HapModuleInfo object of the application.
     * @tc.desc: Check the return value of the interface (by promise)
     */
    it('ACTS_GetAbilityInfo_0100', 0, async function (done) {
        var promise = featureAbility.getAbilityInfo();
        expect(typeof (promise)).assertEqual("object");

        var info = await featureAbility.getAbilityInfo();
        checkAbilityInfo(info);
        done();
        setTimeout(function () {
            console.info('====> ACTS_GetAbilityInfo_0100 =====>')
        }, TIMEOUT)
    })

    /**
     * @tc.number: ACTS_GetAbilityInfo_0200
     * @tc.name: GetAbilityInfo : Obtains the HapModuleInfo object of the application.
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     */
    it('ACTS_GetAbilityInfo_0200', 0, async function (done) {
        var result = featureAbility.getAbilityInfo(
            (err, data) => {
                checkAbilityInfo(data);
                done()
            }
        );
        setTimeout(function () {
            console.info('====> ACTS_GetAbilityInfo_0200 =====>')
        }, TIMEOUT)
    })

    // checkHapModuleInfo
    function checkHapModuleInfo(data) {
        console.log("checkHapModuleInfo start  " + data);
        console.log("checkHapModuleInfo name : " + data.name);
        console.log("checkHapModuleInfo description : " + data.description);
        console.log("checkHapModuleInfo descriptionId : " + data.descriptionId);
        console.log("checkHapModuleInfo icon : " + data.icon);
        console.log("checkHapModuleInfo label : " + data.label);
        console.log("checkHapModuleInfo labelId : " + data.labelId);
        console.log("checkHapModuleInfo iconId : " + data.iconId);
        console.log("checkHapModuleInfo backgroundImg : " + data.backgroundImg);
        console.log("checkHapModuleInfo supportedModes : " + data.supportedModes);
        console.log("checkHapModuleInfo  reqCapabilities length : " + data.reqCapabilities.length);
        for (var j = 0; j < data.reqCapabilities.length; j++) {
            console.log("getHapModuleInfo data.reqCapabilities[" + j + "] : " + data.reqCapabilities[j]);
        }
        console.log("checkHapModuleInfo  deviceTypes length : " + data.deviceTypes.length);
        for (var j = 0; j < data.deviceTypes.length; j++) {
            console.log("getHapModuleInfo data.deviceTypes[" + j + "] : " + data.deviceTypes[j]);
        }
        console.log("checkHapModuleInfo abilityInfos length : " + data.abilityInfos.length);
        console.log("checkHapModuleInfo moduleName : " + data.moduleName);
        console.log("checkHapModuleInfo mainAbilityName : " + data.mainAbilityName);
        console.log("checkHapModuleInfo installationFree : " + data.installationFree);

        expect(typeof (data)).assertEqual("object");
        expect(typeof (data.name)).assertEqual("string");
        expect(typeof (data.description)).assertEqual("string");
        expect(typeof (data.descriptionId)).assertEqual("number");
        expect(typeof (data.icon)).assertEqual("string");
        expect(typeof (data.label)).assertEqual("string");
        expect(typeof (data.labelId)).assertEqual("number");
        expect(typeof (data.iconId)).assertEqual("number");
        expect(typeof (data.backgroundImg)).assertEqual("string");
        expect(typeof (data.supportedModes)).assertEqual("number");
        expect(Array.isArray(data.reqCapabilities)).assertEqual(true);
        expect(Array.isArray(data.deviceTypes)).assertEqual(true);
        expect(Array.isArray(data.abilityInfos)).assertEqual(true);
        expect(typeof (data.moduleName)).assertEqual("string");
        expect(typeof (data.mainAbilityName)).assertEqual("string");
        expect(typeof (data.installationFree)).assertEqual("boolean");

        expect(data.name).assertEqual("com.example.actsfeatureabilitytest");
        expect(data.description).assertEqual("descriptionTest");
        expect(data.descriptionId).assertEqual(0);
        expect(data.icon).assertEqual("$media:icon");
        expect(data.label).assertEqual("$string:app_name");
        expect(data.labelId).assertEqual(0);
        expect(data.iconId).assertEqual(0);
        expect(data.backgroundImg).assertEqual("");
        expect(data.supportedModes).assertEqual(0);

        expect(data.reqCapabilities[0]).assertEqual("reqCapabilitiesTest1");
        expect(data.reqCapabilities[1]).assertEqual("reqCapabilitiesTest2");
        expect(data.deviceTypes[0]).assertEqual("phone");
        expect(data.moduleName).assertEqual("entry")
        expect(data.mainAbilityName).assertEqual("");
        expect(data.installationFree).assertEqual(false);

        console.log("checkHapModuleInfo end  " + data);
    }

    /**
     * @tc.number: ACTS_GetHapModuleInfo_0100
     * @tc.name: GetHapModuleInfo : Obtains the HapModuleInfo object of the application.
     * @tc.desc: Check the return value of the interface (by promise)
     */
    it('ACTS_GetHapModuleInfo_0100', 0, async function (done) {
        var promise = featureAbility.getHapModuleInfo();
        expect(typeof (promise)).assertEqual("object");
        var info = await featureAbility.getHapModuleInfo();
        checkHapModuleInfo(info);
        done();
        setTimeout(function () {
            console.info('====> ACTS_GetHapModuleInfo_0100 =====>')
        }, TIMEOUT)
    })

    /**
    * @tc.number: ACTS_GetHapModuleInfo_0200
    * @tc.name: GetHapModuleInfo : Obtains the HapModuleInfo object of the application.
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_GetHapModuleInfo_0200', 0, async function (done) {
        var result = featureAbility.getHapModuleInfo(
            (err, data) => {
                checkHapModuleInfo(data);
                done()
            }
        );
        setTimeout(function () {
            console.info('====> ACTS_GetHapModuleInfo_0200 =====>')
        }, TIMEOUT)
    })

    // checkProcessName
    function checkProcessName(info) {
        console.log("checkProcessName processName : " + info);
        expect(typeof (info)).assertEqual("string");
        expect(info).assertEqual("processTestAbility");
    }

    /**
     * @tc.number: ACTS_GetProcessName_0100
     * @tc.name: GetProcessName : Obtains the name of the current process.
     * @tc.desc: Check the return value of the interface (by promise)
     */
    it('ACTS_GetProcessName_0100', 0, async function (done) {
        var context = featureAbility.getContext();
        var promise = context.getProcessName();
        expect(typeof (promise)).assertEqual("object");
        var info = await context.getProcessName();
        checkProcessName(info);
        done();
        setTimeout(function () {
            console.info('====> ACTS_GetProcessName_0100 =====>')
        }, TIMEOUT)
    })

    /**
     * @tc.number: ACTS_GetProcessName_0200
     * @tc.name: GetProcessName : Obtains the name of the current process.
     * @tc.desc: Check the return value of the interface (by AsyncCallback)
     */
    it('ACTS_GetProcessName_0200', 0, async function (done) {
        var context = featureAbility.getContext();
        var result = context.getProcessName(
            (err, data) => {
                checkProcessName(data);
                done()
            }
        );
        setTimeout(function () {
            console.info('====> ACTS_GetProcessName_0200 =====>')
        }, TIMEOUT)
    })

    /**
     * @tc.number: ACTS_GetCallingBundle_0100
     * @tc.name: GetCallingBundle : Obtains the bundle name of the ability that called the current ability.
     * @tc.desc: Check the return type of the interface (by AsyncCallback)
     */
    it('ACTS_GetCallingBundle_0100', 0, async function (done) {
        var Subscriber;
        let id;
        let events = new Map();

        function SubscribeCallBack(err, data) {
            clearTimeout(id);
            events.set(data.event, 0)
            console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
            if (events.size == 1) {
                expect(events.has("ACTS_GetCallingBundle_0100_CommonEvent") ||
                    events.has("com.example.actsfeatureabilitytest.promise") ||
                    events.has("com.example.actsfeatureabilitytest.callback")).assertTrue();
            } else if (events.size == 2) {
                expect(events.has("com.example.actsfeatureabilitytest.promise") ||
                    events.has("com.example.actsfeatureabilitytest.callback")).assertTrue();
            } else if (events.size == 3) {
                expect(events.has("ACTS_GetCallingBundle_0100_CommonEvent") &&
                    events.has("com.example.actsfeatureabilitytest.promise") &&
                    events.has("com.example.actsfeatureabilitytest.callback")).assertTrue();
                commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
                done();
            }
        }

        commonEvent.createSubscriber(subscriberInfo_ACTS_GetCallingBundle_0100).then(async (data) => {
            console.debug("====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('ACTS_GetCallingBundle_0100=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }
        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        var result = await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: "com.example.getcallingbundlepromisetest",
                    abilityName: "com.example.getcallingbundlepromisetest.MainAbility",
                },
            }
        );
    })

    // checkGetOrCreateLocalDir
    function checkGetOrCreateLocalDir(info) {
        console.log("checkGetOrCreateLocalDir root dir : " + info);
        expect(typeof (info)).assertEqual("string");
    }

    // @tc.number: ACTS_GetOrCreateLocalDir_0100
    // @tc.name: GetProcessName : Obtains the name of the current process.
    // @tc.desc: Check the return value of the interface (by promise)
    it('ACTS_GetOrCreateLocalDir_0100', 0, async function (done) {
        var context = featureAbility.getContext();
        var promise = context.getOrCreateLocalDir();
        expect(typeof (promise)).assertEqual("object");
        var info = await context.getOrCreateLocalDir();
        checkGetOrCreateLocalDir(info);
        done();
        setTimeout(function () {
            console.info('====> ACTS_GetOrCreateLocalDir_0100 =====>')
        }, TIMEOUT)
    })

    // @tc.number: ACTS_GetOrCreateLocalDir_0200
    // @tc.name: GetProcessName : Obtains the name of the current process.
    // @tc.desc: Check the return value of the interface (by AsyncCallback)
    it('ACTS_GetOrCreateLocalDir_0200', 0, async function (done) {
        var context = featureAbility.getContext();
        var result = context.getOrCreateLocalDir(
            (err, data) => {
                checkGetOrCreateLocalDir(data);
                done()
            }
        );
        setTimeout(function () {
            console.info('====> ACTS_GetOrCreateLocalDir_0200 =====>')
        }, TIMEOUT)
    })

    /*
     * @tc.number  ACTS_StartAbility_0700
     * @tc.name    Configures the wrong abilityname validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_0700",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_0700-------------------");
        try{
            let Want = {
                deviceId: '',
                bundleName: "com.example.startability",
                abilityName: "com.example.startability.MainAbilityXXXXX",
            }
            var StartAbilityParameter = {
                want:Want
            }

            featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
                console.log('ACTS_StartAbility_0700 asyncCallback errCode : ' + JSON.stringify(err) 
                + " data: " + JSON.stringify(data));
                expect(err.code == 2097152).assertTrue();
                done();
            });
        }catch(error){
            console.log("ACTS_StartAbility_0700 : error = " + error);
        }
    });

    /*
     * @tc.number  ACTS_StartAbility_0800
     * @tc.name    Misconfigured bundlename validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_0800",0, async function(done){
        console.info("------------------logMessage startAbilityTest_0300-------------------");
        try{
            let Want = {
                deviceId: '',
                bundleName: "com.example.startabilityxxxxx",
                abilityName: "com.example.startability.MainAbility",
            }
            var StartAbilityParameter = {
                want:Want
            }

            featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
                console.log('ACTS_StartAbility_0800 asyncCallback errCode : ' + JSON.stringify(err) 
                + " data: " + JSON.stringify(data));
                expect(err.code != 0).assertTrue();
                done();
            });
        }catch(error){
            console.log("ACTS_StartAbility_0800 : error = " + error);
        }
    });

    /*
     * @tc.number  ACTS_StartAbility_0900
     * @tc.name    Misconfigured deviceid validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_0900",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_0900-------------------");
        try{
            let Want = {
                deviceId: "43026cb4ca40ce10101834d72731dc8509be1906d30e8d42e86886d2791e82xx",
                bundleName: "com.example.startability",
                abilityName: "com.example.startability.MainAbility",
            }
            var StartAbilityParameter = {
                want:Want
            }

            featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
                console.log('ACTS_StartAbility_0900 asyncCallback errCode : ' + JSON.stringify(err) 
                + " data: " + JSON.stringify(data) + "err.code: " + err.code);
                expect(err.code != 0).assertTrue();
                done();
            });
        }catch(error){
            console.log("ACTS_StartAbility_0900 : error = " + error);
        }
    });

    /*
     * @tc.number  ACTS_StartAbility_1100
     * @tc.name    Verify the ability of implicitly starting through action without configuring action
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_1100",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_1100-------------------");
        try{
            let Want = {
                action: "action.sysxxxtem.homexxxx"
            }
            var StartAbilityParameter = {
                want:Want
            }

            featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
                console.log('ACTS_StartAbility_1100 asyncCallback errCode : ' + JSON.stringify(err) 
                + " data: " + JSON.stringify(data));
                expect(err.code != 0).assertTrue();
                done();
            });
        }catch(error){
            console.log("ACTS_StartAbility_1100 : error = " + error);
        }
    });

    /*
     * @tc.number  ACTS_StartAbility_1200
     * @tc.name    Verify that the ability without configured entities is started through action and entities
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_1200",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_1200-------------------");
        try{
            let Want = {
                action: "action.system.home",
                entities: ["entity.system.homexxxxxxxxx"]
            }
            var StartAbilityParameter = {
                want:Want
            }

            featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
                console.log('ACTS_StartAbility_1200 asyncCallback errCode : ' + JSON.stringify(err) 
                + " data: " + JSON.stringify(data));
                expect(err.code != 0).assertTrue();
                done();
            });
        }catch(error){
            console.log("ACTS_StartAbility_1200 : error = " + error);
        }
    });

    /*
     * @tc.number  ACTS_StartAbility_1300
     * @tc.name    Start ability through action and entities. Entities is configured as an empty array
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_1300",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_1300-------------------");
        try{
            var Subscriber;
            let id;

            function SubscribeCallBack(err, data) {
                clearTimeout(id);
                expect(data.event).assertEqual("ACTS_StartAbility_1300_CommonEvent");
                console.debug("====>Subscribe CallBack data:====>" + JSON.stringify(data));
                commonEvent.unsubscribe(Subscriber, UnSubscribeCallback);
                done();
            }

            commonEvent.createSubscriber(subscriberInfoStartAbilityThirteen).then(async (data) => {
                console.debug("====>Create Subscriber====>");
                Subscriber = data;
                await commonEvent.subscribe(Subscriber, SubscribeCallBack);
            })

            function UnSubscribeCallback() {
                console.debug("====>UnSubscribe CallBack====>");
                done();
            }

            function timeout() {
                expect().assertFail();
                console.debug('ACTS_StartAbility_1300=====timeout======');
                commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
                done();
            }

            id = setTimeout(timeout, START_ABILITY_TIMEOUT);
            let Want = {
                action: "action.system.home",
                entities: []
            }
            var StartAbilityParameter = {
                want:Want
            }

            featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
                console.log('ACTS_StartAbility_1300 asyncCallback errCode : ' + JSON.stringify(err) 
                + " data: " + JSON.stringify(data));
                expect(err.code == 0).assertTrue();
                done();
            });
        }catch(error){
            console.log("ACTS_StartAbility_1300 : error = " + error);
        }
    });

    /*
     * @tc.number  ACTS_StartAbility_1400
     * @tc.name    Start ability through action and entities, which are configured as strings
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_1400",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_1400-------------------");
        try{
            let Want = {
                action: "action.system.home",
                entities: 'asdfghj'
            }
            var StartAbilityParameter = {
                want:Want
            }

            featureAbility.startAbility(StartAbilityParameter,(err,data)=>{
                console.log('ACTS_StartAbility_1400 asyncCallback errCode : ' + JSON.stringify(err) 
                + " data: " + JSON.stringify(data));
                done();
            });
        }catch(error){
            console.log("ACTS_StartAbility_1400 : error = " + error);
        }
    });

    /*
     * @tc.number  ACTS_StartAbility_1600
     * @tc.name    Startability input parameter want is {}
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_1600",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_1600-------------------");
        let Want = {}
        var StartAbilityParameter = {
            want:Want
        }

        var promise = featureAbility.startAbility(StartAbilityParameter);
        if (promise) {
            promise.then((data) => {
                console.log('ACTS_StartAbility_1600 errCode : ' + " data: " + JSON.stringify(data));
                expect().assertFail();
                done();
            }).catch((err)=>{
                expect(err.code != 0).assertTrue();
                done();
            });
        } else {
            expect(promise == undefined).assertTrue();
            done();
        }
    });

    /*
     * @tc.number  ACTS_StartAbility_1700
     * @tc.name    The startability input parameter is not configured with want
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_1700",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_1700-------------------");
        var StartAbilityParameter = {}
        var promise = featureAbility.startAbility(StartAbilityParameter)
        if (promise) {
            promise.then((data) => {
                console.log('ACTS_StartAbility_1700  data: '  + JSON.stringify(data));
                expect().assertFail();
                done();
            }).catch((err)=>{
                expect(err.code != 0).assertTrue();
                done();
            });
        } else {
            expect(promise == undefined).assertTrue();
            done();
        } 
    });

    /*
     * @tc.number  ACTS_StartAbility_1800
     * @tc.name    The startability input parameter is undefined
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_StartAbility_1800",0, async function(done){
        console.info("------------------logMessage ACTS_StartAbility_1800-------------------");
        var promise = featureAbility.startAbility(undefined);
        if (promise) {
            promise.then((data) => {
                console.log('ACTS_StartAbility_1800 asyncCallback data: ' + JSON.stringify(data));
                expect().assertFail();
                done();
            }).catch((err)=>{
                expect(err.code != 0).assertTrue();
                done();
            });
        } else {
            expect(promise == undefined).assertTrue();
            done();
        }
    });

    /*
    * @tc.number: ACTS_StartAbility_0100
    * @tc.name: featureAbility.getWindow : Get an ability window.
    * @tc.desc: Check the return window of the interface (by AsyncCallback)
    */
    it('ACTS_StartAbility_0100', 0, async function (done) {
        console.log('ACTS_StartAbility_0100====<begin');

        try {
            featureAbility.getWindow((err,data)=>{
                console.log('getWindow call back');
                done();
            });
            done();
        } catch (err) {
            console.log('ACTS_StartAbility_0100====<end err=' + err)
            done();
        }
        console.log('ACTS_StartAbility_0100====<end');
    })

    /*
    * @tc.number: ACTS_StartAbility_0200
    * @tc.name: featureAbility.getWindow : Get an ability window.
    * @tc.desc: Check the return window of the interface
    */
    it('ACTS_StartAbility_0200', 0, async function (done) {
        console.log('ACTS_StartAbility_0200====<begin');
        try {
            var window = featureAbility.getWindow();
            done();
        } catch (err) {
            console.log('ACTS_StartAbility_0200====<end err=' + err)
            done();
        }
        console.log('ACTS_StartAbility_0200====<end');
    })
})
