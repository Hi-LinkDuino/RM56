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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

const TIMEOUTHALF = 500;
const TIMEOUT = 1000;
describe('ActsOsAccountSystemTest', function () {
    function sleep(delay) {
        var start = (new Date()).getTime();
        while((new Date()).getTime() - start < delay) {
            continue;
        }
    }

    /*
     * @tc.number  : ActsOsAccountOnOff_0100
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verify that the activating type subscription can receive user switching
     */
    it('ActsOsAccountOnOff_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_0100 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        var activatingSign = false;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                osAccountManager.off("activating", "osAccountOnOffNameA", offCallback);
            }
        }
        function onActivateCallback(receiveId){
            console.debug("====>onActivateCallback receive localId:" + receiveId);
            if(receiveId == localId){
                osAccountManager.off("activate", "osAccountactivateNameA", offActivateCallback);
            }
        }
        function removeCallback(err){
            console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>the activating sign is: " + activatingSign);
            expect(activatingSign).assertTrue();
            console.debug("====>ActsOsAccountOnOff_0100 end====");
            done();
        }
        function offCallback(){
            console.debug("====>off enter")
            activatingSign = true;
        }
        function offActivateCallback(){
            console.debug("====>offActivateCallback enter")
            sleep(TIMEOUT);
            osAccountManager.removeOsAccount(localId, removeCallback);
        }
        console.debug("====>on activating start====");
        osAccountManager.on("activating", "osAccountOnOffNameA", onCallback);
        console.debug("====>on activate start====");
        osAccountManager.on("activate", "osAccountactivateNameA", onActivateCallback);
        sleep(TIMEOUT);
        osAccountManager.createOsAccount("osAccountNameA", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameA");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_0200
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verify that the activate type subscription can receive user switching
     */
    it('ActsOsAccountOnOff_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_0200 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                osAccountManager.off("activate", "osAccountOnOffNameB", offCallback);
            }
        }
        function offCallback(){
            console.debug("====>off enter")
            osAccountManager.removeOsAccount(localId, (err)=>{
                console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>ActsOsAccountOnOff_0200 end====");
                done();
            })
        }
        osAccountManager.on("activate", "osAccountOnOffNameB", onCallback);
        sleep(TIMEOUT);
        osAccountManager.createOsAccount("osAccountNameB", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameB");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
            });
        });
    })

    /*
     * @tc.number  : ActsOsAccountOnOff_0300
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verify that the activating type unsubscribes successfully
     */
    it('ActsOsAccountOnOff_0300', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_0300 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====>wrongly received localId: " + localId + " after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        function offCallback(){
            console.debug("====>off enter")
        }
        osAccountManager.on("activating", "osAccountOnOffNameC", onCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.off("activating", "osAccountOnOffNameC", offCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.createOsAccount("osAccountNameC", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameC");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                setTimeout(()=>{
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountOnOff_0300 end====");
                        done();
                    })
                }, TIMEOUT);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_0400
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verify that the activate type unsubscribes successfully
     */
    it('ActsOsAccountOnOff_0400', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_0400 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====>wrongly received localId: " + localId + " after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        function offCallback(){
            console.debug("====>off enter")
        }
        osAccountManager.on("activate", "osAccountOnOffNameD", onCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.off("activate", "osAccountOnOffNameD", offCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.createOsAccount("osAccountNameD", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameD");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                setTimeout(()=>{
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountOnOff_0400 end====");
                        done();
                    })
                }, TIMEOUT);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_0500
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verification cannot cancel activating type not subscribed
     */
    it('ActsOsAccountOnOff_0500', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_0500 start====");
        var osAccountManager = osaccount.getAccountManager();
        var notOnName = "nameNotSubscribeActivatingTest";
        console.debug("====>get AccountManager finish====");
        function offCallback(){
            console.debug("====>wrongly enter off callback====");
            expect().assertFail();
            done();
        }
        osAccountManager.off("activating", notOnName, offCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountOnOff_0500 end====");
            done();
        }, TIMEOUT);
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_0600
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verification cannot cancel activate type not subscribed
     */
    it('ActsOsAccountOnOff_0600', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_0600 start====");
        var osAccountManager = osaccount.getAccountManager();
        var notOnName = "nameNotSubscribeActivateTest";
        console.debug("====>get AccountManager finish====");
        function offCallback(){
            console.debug("====>wrongly enter off callback====");
            expect().assertFail();
            done();
        }
        osAccountManager.off("activate", notOnName, offCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountOnOff_0600 end====");
            done();
        }, TIMEOUT);
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_0700
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Validate subscription callback with parameter name as empty string
     */
    it('ActsOsAccountOnOff_0700', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_0700 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====>wrongly received localId: " + localId + " after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        osAccountManager.on("activating", "", onCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.createOsAccount("osAccountNameE", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameE");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount errcode:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                sleep(TIMEOUT);
                setTimeout(()=>{
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountOnOff_0700 end====");
                        done();
                    })
                }, TIMEOUT);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_0800
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Validate subscription promise with parameter name as empty string
     */
    it('ActsOsAccountOnOff_0800', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_0800 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====>wrongly received localId: " + localId + " after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        osAccountManager.on("activate", "", onCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.createOsAccount("osAccountNameF", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameF");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                setTimeout(()=>{
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountOnOff_0800 end====");
                        done();
                    })
                }, TIMEOUT);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_0900
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verification does not trigger callbacks for activating type subscriptions when switching fails
     */
    it('ActsOsAccountOnOff_0900', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_0900 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====>wrongly received localId: " + localId + " after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        function offCallback(){
            console.debug("====>off enter");
            console.debug("====>ActsOsAccountOnOff_0900 end====");
            done();
        }
        osAccountManager.on("activating", "osAccountOnOffNameG", onCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.queryAllCreatedOsAccounts((err, data)=>{
            console.debug("====>queryAllCreatedOsAccounts err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            for (var i = 0;i< data.length;i++){
                if(data[i].isActived == true){
                    localId = data[i].localId;
                }
            }
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code !=0).assertEqual(true);
                setTimeout(()=>{
                    osAccountManager.off("activating", "osAccountOnOffNameG", offCallback);
                }, TIMEOUT);
            });
        })
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_1000
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verification does not trigger callbacks for activate type subscriptions when switching fails
     */
    it('ActsOsAccountOnOff_1000', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1000 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====>wrongly received localId: " + localId + " after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        function offCallback(){
            console.debug("====>off enter");
            console.debug("====>ActsOsAccountOnOff_1000 end====");
            done();
        }
        osAccountManager.on("activate", "osAccountOnOffNameH", onCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.queryAllCreatedOsAccounts((err, data)=>{
            console.debug("====>queryAllCreatedOsAccounts err:" + JSON.stringify(err));
            for (var i = 0;i< data.length;i++){
                if(data[i].isActived == true){
                    localId = data[i].localId;
                }
            }
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                setTimeout(()=>{
                    expect(err.code !=0).assertEqual(true);
                    osAccountManager.off("activate", "osAccountOnOffNameH", offCallback);
                }, 2000);
            });
        })
    })

    /*
     * @tc.number  : ActsOsAccountOnOff_1100
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verify that an instance repeatedly subscribes to local users
     */
    it('ActsOsAccountOnOff_1100', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1100 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        var countNum = 0;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                countNum++;
            }
        }
        function onCallbackTwice(receiveLocalId){
            console.debug("====>receive twice localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                countNum++;
            }
        }
        function offCallback(){
            console.debug("====>off enter====");
            expect(countNum).assertEqual(2);
            osAccountManager.removeOsAccount(localId, (err)=>{
                console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>ActsOsAccountOnOff_1100 end====");
                done();
            })
        }
        osAccountManager.on("activate", "osAccountOnOffNameI", onCallback);
        osAccountManager.on("activate", "osAccountOnOffNameI", onCallbackTwice);
        sleep(TIMEOUT);
        osAccountManager.createOsAccount("osAccountNameI", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameI");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                setTimeout(()=>{
                    osAccountManager.off("activate", "osAccountOnOffNameI", offCallback);
                }, 2000);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_1200
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verify that an instance unsubscribes local users afer resubscribing
     */
    it('ActsOsAccountOnOff_1200', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1200 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        var countNum = 0;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                countNum++;
            }
        }
        function onCallbackTwice(receiveLocalId){
            console.debug("====>receive twice localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                countNum++;
            }
        }
        function offCallback(){
            console.debug("====>off enter====");
        }
        osAccountManager.on("activate", "osAccountOnOffNameJ", onCallback);
        osAccountManager.on("activate", "osAccountOnOffNameJ", onCallbackTwice);
        sleep(TIMEOUTHALF);
        osAccountManager.off("activate", "osAccountOnOffNameJ", offCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.createOsAccount("osAccountNameJ", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameJ");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                setTimeout(()=>{
                    expect(countNum).assertEqual(0);
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountOnOff_1200 end====");
                        done();
                     })
                }, TIMEOUT);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_1300
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Verify that an instance unsubscribes local users afer resubscribing
     */
    it('ActsOsAccountOnOff_1300', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1300 start====");
        var osAccountManager = osaccount.getAccountManager();
        var osAccountManagerTwice = osaccount.getAccountManager();
        var localId;
        var countFirstNum = 0;
        var countSecondNum = 0;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                countFirstNum++;
            }
        }
        function onCallbackTwice(receiveLocalId){
            console.debug("====>receive twice localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                countSecondNum++;
            }
        }
        function offCallback(){
            console.debug("====>off enter====");
        }
        function offCallbackTwice(){
            console.debug("====>off twice enter====");
            osAccountManager.removeOsAccount(localId, (err)=>{
                console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>ActsOsAccountOnOff_1300 end====");
                done();
             })
        }
        osAccountManager.on("activate", "osAccountOnOffNameJ", onCallback);
        osAccountManagerTwice.on("activate", "osAccountOnOffNameJ", onCallbackTwice);
        sleep(TIMEOUTHALF);
        osAccountManager.off("activate", "osAccountOnOffNameJ", offCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.createOsAccount("osAccountNameK", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameK");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                setTimeout(()=>{
                    expect(countFirstNum).assertEqual(0);
                    expect(countSecondNum).assertEqual(1);
                    osAccountManagerTwice.off("activate", "osAccountOnOffNameJ", offCallbackTwice);
                }, TIMEOUT);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_1400
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Validate subscription with parameter type as empty string
     */
    it('ActsOsAccountOnOff_1400', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1400 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====>wrongly received localId: " + localId + " after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        osAccountManager.on("", "osAccountOnOffNameL", onCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.createOsAccount("osAccountNameL", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameL");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                setTimeout(()=>{
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountOnOff_1400 end====");
                        done();
                    })
                }, TIMEOUT);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_1500
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Validate subscription with parameter type as other string
     */
    it('ActsOsAccountOnOff_1500', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1500 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====>wrongly received localId: " + localId + " after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        osAccountManager.on("Abc", "osAccountOnOffNameM", onCallback);
        sleep(TIMEOUTHALF);
        osAccountManager.createOsAccount("osAccountNameM", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameM");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                setTimeout(()=>{
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountOnOff_1500 end====");
                        done();
                    })
                }, TIMEOUT);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_1600
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Validate subscription with parameter type as undefined
     */
    it('ActsOsAccountOnOff_1600', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1600 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====>wrongly received localId: " + localId + " after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        osAccountManager.on(undefined, "osAccountOnOffNameM", onCallback);
        osAccountManager.createOsAccount("osAccountNameM", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameM");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                setTimeout(()=>{
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountOnOff_1600 end====");
                        done();
                    })
                }, TIMEOUT);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_1700
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Validate unsubscribe with parameter type as empty string
     */
    it('ActsOsAccountOnOff_1700', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1700 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        function offCallback(){
            console.debug("====>wrongly enter off callback====");
            expect().assertFail();
            done();
        }
        osAccountManager.off("", "osAccountOnOffNameN", offCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountOnOff_1700 end====");
            done();
        }, TIMEOUT);
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_1800
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Validate unsubscribe with parameter type as other string
     */
    it('ActsOsAccountOnOff_1800', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1800 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        function offCallback(){
            console.debug("====>wrongly enter off callback====");
            expect().assertFail();
            done();
        }
        osAccountManager.off("Abc", "osAccountOnOffNameO", offCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountOnOff_1800 end====");
            done();
        }, TIMEOUT);
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_1900
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Validate unsubscribe with parameter type as undefined
     */
    it('ActsOsAccountOnOff_1900', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_1900 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        function offCallback(){
            console.debug("====>wrongly enter off callback====");
            expect().assertFail();
            done();
        }
        osAccountManager.off(undefined, "osAccountOnOffNameP", offCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountOnOff_1900 end====");
            done();
        }, TIMEOUT);
    });

    /*
     * @tc.number  : ActsOsAccountOnOff_2000
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : Authenticate switch back after switching user subscription receives two callbacks
     */
    it('ActsOsAccountOnOff_2000', 0, async function (done) {
        console.debug("====>ActsOsAccountOnOff_2000 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        var enterSign = 0;
        var localHundredId = 100;
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                enterSign = enterSign + 1;
            }
            if(receiveLocalId == localHundredId){
                enterSign = enterSign + 1;
                osAccountManager.off("activate", "osAccountOnOffNameQ", offCallback);
            }
        }
        function offCallback(){
            console.debug("====>off enter")
            osAccountManager.removeOsAccount(localId, (err)=>{
                console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>the enterSign is: " + enterSign);
                expect(enterSign).assertEqual(2);
                console.debug("====>ActsOsAccountOnOff_2000 end====");
                done();
            })
        }
        osAccountManager.on("activate", "osAccountOnOffNameQ", onCallback);
        sleep(TIMEOUT);
        osAccountManager.createOsAccount("osAccountNameQ", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameQ");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                osAccountManager.activateOsAccount(localHundredId, (err)=>{
                    console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                });
            });
        });
    })

})