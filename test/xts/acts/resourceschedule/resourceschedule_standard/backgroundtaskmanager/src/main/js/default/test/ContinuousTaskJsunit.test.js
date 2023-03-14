/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import notification from '@ohos.notification';
import wantAgent from '@ohos.wantAgent';
import particleAbility from '@ohos.ability.particleAbility'
import backgroundTaskManager from '@ohos.backgroundTaskManager'
import featureAbility from '@ohos.ability.featureAbility'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("ContinuousTaskJsTest", function () {
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
     * @tc.name:ContinuousTaskJsTest001
     * @tc.desc:verify new startBackgroundrunning interface promise mode work properly
     * @tc.type: FUNC
     * @tc.require:
     */
    it("ContinuousTaskJsTest001", 0, async function (done) {
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.continuoustask.test",
                    abilityName: "com.continuoustask.test.MainAbility"
                }
            ],
            operationType: 2,
            requestCode: 0,
            wantAgentFlags: [3]
        };
        wantAgent.getWantAgent(wantAgentInfo).then((data) => {
            backgroundTaskManager.startBackgroundRunning(featureAbility.getContext(),
                backgroundTaskManager.BackgroundMode.DATA_TRANSFER, data).then(() => {
                console.log("ContinuousTaskJsTest001 startBackgroundRunning success");
                expect(true).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }).catch((err) => {
                expect(false).assertTrue();
                console.log("ContinuousTaskJsTest001 startBackgroundRunning failure");
                setTimeout(() => {
                    done();
                }, 500);
            });
        });
    })

    /*
     * @tc.name:ContinuousTaskJsTest002
     * @tc.desc:verify new startBackgroundrunning interface callback mode work properly
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("ContinuousTaskJsTest002", 0, async function (done) {
        function conTaskCallback(err, data) {
            if (err) {
                console.info('ContinuousTaskJsTest002 startBackgroundRunning failed');
                expect(false).assertTrue();
            } else {
                console.info('ContinuousTaskJsTest002 startBackgroundRunning succeed');
                expect(true).assertTrue();
            }
            setTimeout(()=>{
                done();
            }, 500);
        }
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.continuoustask.test",
                    abilityName: "com.continuoustask.test.MainAbility"
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
     * @tc.name:ContinuousTaskJsTest003
     * @tc.desc:verify old startBackgroundrunning interface promise mode work properly
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("ContinuousTaskJsTest003", 0, async function (done) {
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.continuoustask.test",
                    abilityName: "com.continuoustask.test.MainAbility"
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
                console.log("ContinuousTaskJsTest003 startBackgroundRunning success");
                expect(true).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            }).catch((err) => {
                expect(false).assertTrue();
                console.log("ContinuousTaskJsTest003 startBackgroundRunning failure");
                setTimeout(() => {
                    done();
                }, 500);
            });
        });
    })

    /*
     * @tc.name:ContinuousTaskJsTest004
     * @tc.desc:verify old startBackgroundrunning interface callback mode work properly
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("ContinuousTaskJsTest004", 0, async function (done) {
        function conTaskCallback(err, data) {
            if (err) {
                console.info('ContinuousTaskJsTest004 startBackgroundRunning failure');
                expect(false).assertTrue();
            } else {
                console.info('ContinuousTaskJsTest004 startBackgroundRunning success');
                expect(true).assertTrue();
            }
            setTimeout(()=>{
                done();
            }, 500);
        }
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.continuoustask.test",
                    abilityName: "com.continuoustask.test.MainAbility"
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
     * @tc.name:ContinuousTaskJsTest005
     * @tc.desc:verify new api stopBackgroundrunning interface promise mode work properly
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("ContinuousTaskJsTest005", 0, async function (done) {
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.continuoustask.test",
                    abilityName: "com.continuoustask.test.MainAbility"
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
                    console.log("ContinuousTaskJsTest005 cancelBackgroundRunning success");
                    expect(true).assertTrue();
                    setTimeout(() => {
                        done();
                    }, 500);
                }).catch((err) => {
                    expect(false).assertTrue();
                    console.log("ContinuousTaskJsTest005 cancelBackgroundRunning failure");
                    setTimeout(() => {
                        done();
                    }, 500);
                });
            })
        });
    })

    /*
        * @tc.name:ContinuousTaskJsTest006
        * @tc.desc:verify new api stopBackgroundrunning interface callback mode work properly
        * @tc.type: FUNC
        * @tc.require: 
        */
    it("ContinuousTaskJsTest006", 0, async function (done) {
        function conTaskCallback(err, data) {
            if (err) {
                console.info('ContinuousTaskJsTest006 startBackgroundRunning failure');
                expect(false).assertTrue();
            } else {
                console.info('ContinuousTaskJsTest006 startBackgroundRunning success');
                expect(true).assertTrue();
            }
            setTimeout(()=>{
                done();
            }, 500);
        }
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.continuoustask.test",
                    abilityName: "com.continuoustask.test.MainAbility"
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
     * @tc.name:ContinuousTaskJsTest007
     * @tc.desc:verify old api cancelBackgroundrunning interface promise mode work properly
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("ContinuousTaskJsTest007", 0, async function (done) {
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.continuoustask.test",
                    abilityName: "com.continuoustask.test.MainAbility"
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
            console.log("ContinuousTaskJsTest007 cancelBackgroundRunning success");
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }).catch( (err) => {
            expect(false).assertTrue();
            console.log("ContinuousTaskJsTest007 cancelBackgroundRunning failure");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:ContinuousTaskJsTest008
     * @tc.desc:verify old cancelBackgroundrunning interface callback mode work properly
     * @tc.type: FUNC
     * @tc.require: 
     */
    it("ContinuousTaskJsTest008", 0, async function (done) {
        function conTaskCallback(err, data) {
            if (err) {
                console.info('ContinuousTaskJsTest008 startBackgroundRunning failure');
                expect(false).assertTrue();
            } else {
                console.info('ContinuousTaskJsTest008 startBackgroundRunning success');
                expect(true).assertTrue();
            }
            setTimeout(()=>{
                done();
            }, 500);
        }
        let wantAgentInfo = {
            wants: [
                {
                    bundleName: "com.continuoustask.test",
                    abilityName: "com.continuoustask.test.MainAbility"
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
	
	/*
    * @tc.name: ContinuousTaskJsTest009
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("ContinuousTaskJsTest009", 0, function (done) {
        console.info('----------------------ContinuousTaskJsTest009---------------------------');
        let value1 = backgroundTaskManager.BackgroundMode.DATA_TRANSFER;
        expect(value1).assertEqual(1)
        done();
    })
	
	/*
    * @tc.name: ContinuousTaskJsTest010
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("ContinuousTaskJsTest010", 0, function (done) {
        console.info('----------------------ContinuousTaskJsTest010---------------------------');
        let value1 = backgroundTaskManager.BackgroundMode.AUDIO_PLAYBACK;
        expect(value1).assertEqual(2)
        done();
    })
	
	/*
    * @tc.name: ContinuousTaskJsTest011
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("ContinuousTaskJsTest011", 0, function (done) {
        console.info('----------------------ContinuousTaskJsTest011---------------------------');
        let value1 = backgroundTaskManager.BackgroundMode.AUDIO_RECORDING;
        expect(value1).assertEqual(3)
        done();
    })
	
	/*
    * @tc.name: ContinuousTaskJsTest012
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("ContinuousTaskJsTest012", 0, function (done) {
        console.info('----------------------ContinuousTaskJsTest012---------------------------');
        let value1 = backgroundTaskManager.BackgroundMode.LOCATION;
        expect(value1).assertEqual(4)
        done();
    })
	
	/*
    * @tc.name: ContinuousTaskJsTest013
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("ContinuousTaskJsTest013", 0, function (done) {
        console.info('----------------------ContinuousTaskJsTest013---------------------------');
        let value1 = backgroundTaskManager.BackgroundMode.BLUETOOTH_INTERACTION;
        expect(value1).assertEqual(5)
        done();
    })
	
	/*
    * @tc.name: ContinuousTaskJsTest014
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("ContinuousTaskJsTest014", 0, function (done) {
        console.info('----------------------ContinuousTaskJsTest014---------------------------');
        let value1 = backgroundTaskManager.BackgroundMode.MULTI_DEVICE_CONNECTION;
        expect(value1).assertEqual(6)
        done();
    })
	
	/*
    * @tc.name: ContinuousTaskJsTest015
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("ContinuousTaskJsTest015", 0, function (done) {
        console.info('----------------------ContinuousTaskJsTest015---------------------------');
        let value1 = backgroundTaskManager.BackgroundMode.WIFI_INTERACTION;
        expect(value1).assertEqual(7)
        done();
    })
	
	/*
    * @tc.name: ContinuousTaskJsTest016
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("ContinuousTaskJsTest016", 0, function (done) {
        console.info('----------------------ContinuousTaskJsTest016---------------------------');
        let value1 = backgroundTaskManager.BackgroundMode.VOIP;
        expect(value1).assertEqual(8)
        done();
    })
	
	/*
    * @tc.name: ContinuousTaskJsTest017
    * @tc.desc: test work scheduler constant
    * @tc.type: FUNC
    * @tc.require: 
    */
    it("ContinuousTaskJsTest017", 0, function (done) {
        console.info('----------------------ContinuousTaskJsTest017---------------------------');
        let value1 = backgroundTaskManager.BackgroundMode.TASK_KEEPING;
        expect(value1).assertEqual(9)
        done();
    })
})