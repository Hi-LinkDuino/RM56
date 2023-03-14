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

const TIMEOUT = 1000;
const ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR = 4587571;
describe('ActsOsAccountSystemTest', function () {

    /*
     * @tc.number  : ActsOsAccountActivate_0100
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : Verify repeat switch 100 local user
     */
    it('ActsOsAccountActivate_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_0100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.isOsAccountActived(100, (err, isActived)=>{
            console.debug("====>isOsAccountActived err:" + JSON.stringify(err));
            console.debug("====>isOsAccountActived data:" + isActived);
            expect(err.code).assertEqual(0);
            if(isActived){
                osAccountManager.activateOsAccount(100, (err)=>{
                    console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
                    console.debug("====>ActsOsAccountActivate_0100 end====");
                    done();
                })
            }else{
                osAccountManager.activateOsAccount(100, (err)=>{
                    console.debug("====>activateOsAccount first err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    osAccountManager.activateOsAccount(100, (err)=>{
                        console.debug("====>activateOsAccount second err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
                        console.debug("====>ActsOsAccountActivate_0100 end====");
                        done();
                    })
                })
            }
        })
    });

    /*
     * @tc.number  : ActsOsAccountActivate_0200
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : Verify repeat switch 100 local user
     */
    it('ActsOsAccountActivate_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_0200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var isActived = await osAccountManager.isOsAccountActived(100);
        console.debug("====>isOsAccountActived err:" + JSON.stringify(isActived));
        if(isActived){
            try{
                await osAccountManager.activateOsAccount(100);
            }catch(err){
                console.debug("====>catch activateOsAccount 0200 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
                console.debug("====>ActsOsAccountActivate_0200 end====");
                done();
            }
        }else{
            console.debug("====>switch to 100 user====");
            await osAccountManager.activateOsAccount(100);
            try{
                await osAccountManager.activateOsAccount(100);
            }catch(err){
                console.debug("====>catch activateOsAccount 0200 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
                console.debug("====>ActsOsAccountActivate_0200 end====");
                done();
            }
        }
    });

    /*
     * @tc.number  : ActsOsAccountActivate_0300
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : Authenticate switch to newly created local user
     */
    it('ActsOsAccountActivate_0300', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_0300 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var osLocalId;
        osAccountManager.createOsAccount("osAccountNameTestActivate", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            osLocalId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            osAccountManager.isOsAccountActived(osLocalId, (err, data)=>{
                console.debug("====>localId: " + osLocalId + " isOsAccountActived err:" + JSON.stringify(err));
                console.debug("====>localId: " + osLocalId + " isOsAccountActived data:" + data);
                expect(err.code).assertEqual(0);
                expect(data).assertFalse();
                osAccountManager.activateOsAccount(osLocalId, (err)=>{
                    console.debug("====>activateOsAccount localId: " + osLocalId + " err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    osAccountManager.isOsAccountActived(osLocalId, (err, data)=>{
                        console.debug("====>localId: " + osLocalId + " isOsAccountActived err:" + JSON.stringify(err));
                        console.debug("====>localId: " + osLocalId + " isOsAccountActived data:" + data);
                        expect(err.code).assertEqual(0);
                        expect(data).assertTrue();
                        osAccountManager.isOsAccountActived(100, (err, data)=>{
                            console.debug("====>localId: 100 isOsAccountActived err:" + JSON.stringify(err));
                            console.debug("====>localId: 100 isOsAccountActived data:" + data);
                            expect(err.code).assertEqual(0);
                            expect(data).assertFalse();
                            osAccountManager.activateOsAccount(100, (err)=>{
                                console.debug("====>activateOsAccount localId: 100 err:" + JSON.stringify(err));
                                expect(err.code).assertEqual(0);
                                osAccountManager.removeOsAccount(osLocalId, (err)=>{
                                    console.debug("====>remove localId: " + osLocalId + " err:" + JSON.stringify(err));
                                    expect(err.code).assertEqual(0);
                                    console.debug("====>ActsOsAccountActivate_0300 end====");
                                    done();
                                })
                            })
                        })
                    })
                })
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountActivate_0400
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : Authenticate switch to newly created local user
     */
    it('ActsOsAccountActivate_0400', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_0400 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var osLocalId;
        var osAccountInfo = await osAccountManager.createOsAccount("osAccountNameTestActivate", 1);
        console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
        osLocalId = osAccountInfo.localId;
        var isActived = await osAccountManager.isOsAccountActived(osLocalId);    
        console.debug("====>localId: " + osLocalId + " isOsAccountActived:" + isActived);
        expect(isActived).assertFalse();
        await osAccountManager.activateOsAccount(osLocalId);
        var isActivedTwice = await osAccountManager.isOsAccountActived(osLocalId);           
        console.debug("====>after activing localId: " + osLocalId + " isOsAccountActived:" + isActivedTwice);
        expect(isActivedTwice).assertTrue();
        var isActiveHandred = await osAccountManager.isOsAccountActived(100);
        console.debug("====>localId: 100 isOsAccountActived:" + isActiveHandred);
        expect(isActiveHandred).assertFalse();
        await osAccountManager.removeOsAccount(osLocalId);
        console.debug("====>ActsOsAccountActivate_0400 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountActivate_0500
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : Authenticate failed to switch to 0 user
     */
    it('ActsOsAccountActivate_0500', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_0500 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.activateOsAccount(0, (err)=>{
            console.debug("====>activateOsAccount to 0 user err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountActivate_0500 end====");
            done();
        })
    });

    /*
     * @tc.number  : ActsOsAccountActivate_0600
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : Authenticate failed to switch to 0 user
     */
    it('ActsOsAccountActivate_0600', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_0600 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.activateOsAccount(0);
        }catch(err){
            console.debug("====>activateOsAccount to 0 user err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountActivate_0600 end====");
            done();
        }  
    });

    /*
     * @tc.number  : ActsOsAccountActivate_0700
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : Authenticate failed to switch to -1 user
     */
    it('ActsOsAccountActivate_0700', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_0700 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.activateOsAccount(-1, (err)=>{
            console.debug("====>activateOsAccount to -1 user err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountActivate_0700 end====");
            done();
        })
    });

    /*
     * @tc.number  : ActsOsAccountActivate_0800
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : Authenticate failed to switch to -1 user
     */
    it('ActsOsAccountActivate_0800', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_0800 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.activateOsAccount(-1);
        }catch(err){
            console.debug("====>activateOsAccount to -1 user err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountActivate_0800 end====");
            done();
        }  
    });

    /*
     * @tc.number  : ActsOsAccountActivate_0900
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : Authenticate failed to switch to "Abc" user
     */
    it('ActsOsAccountActivate_0900', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_0900 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localIdStr = "100";
        function funcActivateCallback(err){
            console.debug("====>error received callback====");
            console.debug("====>receive activateOsAccount err: " + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        osAccountManager.activateOsAccount(localIdStr, funcActivateCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountActivate_0900 end====");
            done();
        }, TIMEOUT);
    });

    /*
     * @tc.number  : ActsOsAccountActivate_1000
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : Authenticate failed to switch to "Abc" user
     */
    it('ActsOsAccountActivate_1000', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_1000 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localIdStr = "100";
        var activateResult = await osAccountManager.activateOsAccount(localIdStr);
        expect(activateResult).assertEqual(null);
        console.debug("====>ActsOsAccountActivate_1000 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountActivate_1100
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : Authenticate failed to switch to undefined user
     */
    it('ActsOsAccountActivate_1100', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_1100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        function funcActivateCallback(err){
            console.debug("====>error received callback====");
            console.debug("====>receive activateOsAccount err: " + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        osAccountManager.activateOsAccount(undefined, funcActivateCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountActivate_1100 end====");
            done();
        }, TIMEOUT);
    });

     /*
     * @tc.number  : ActsOsAccountActivate_1200
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : Authenticate failed to switch to undefined user
     */
    it('ActsOsAccountActivate_1200', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_1200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var activateResult = await osAccountManager.activateOsAccount(undefined);
        expect(activateResult).assertEqual(null);
        console.debug("====>ActsOsAccountActivate_1200 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountActivate_1300
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : Authenticate failed to switch to non-existent user
     */
    it('ActsOsAccountActivate_1300', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_1300 start====");
        var osAccountManager = osaccount.getAccountManager();
        var nonExistentLocalId = 1000;
        console.debug("====>get AccountManager finish====");
        osAccountManager.activateOsAccount(nonExistentLocalId, (err)=>{
            console.debug("====>activateOsAccount to nonexist user err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountActivate_1300 end====");
            done();
        })
    });

     /*
     * @tc.number  : ActsOsAccountActivate_1400
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : Authenticate failed to switch to non-existent user
     */
    it('ActsOsAccountActivate_1400', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_1400 start====");
        var osAccountManager = osaccount.getAccountManager();
        var nonExistentLocalId = 1000;
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.activateOsAccount(nonExistentLocalId);
        }catch(err){
            console.debug("====>activateOsAccount to nonexist user err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountActivate_1400 end====");
            done();
        }  
    });

    /*
     * @tc.number  : ActsOsAccountActivate_1500
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : Authenticate delete newly created active user
     */
    it('ActsOsAccountActivate_1500', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_1500 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        osAccountManager.createOsAccount("osAccountNameTestDelete", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            osAccountManager.isOsAccountActived(localId, (err, data)=>{
                console.debug("====>localId: " + localId + " isOsAccountActived err:" + JSON.stringify(err));
                console.debug("====>localId: " + localId + " isOsAccountActived data:" + data);
                expect(err.code).assertEqual(0);
                expect(data).assertFalse();
                osAccountManager.activateOsAccount(localId, (err)=>{
                    console.debug("====>activateOsAccount localId: " + localId + " err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    osAccountManager.isOsAccountActived(localId, (err, data)=>{
                        console.debug("====>localId: " + localId + " isOsAccountActived err:" + JSON.stringify(err));
                        console.debug("====>localId: " + localId + " isOsAccountActived data:" + data);
                        expect(err.code).assertEqual(0);
                        expect(data).assertTrue();
                        osAccountManager.removeOsAccount(localId, (err)=>{
                            console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            osAccountManager.isOsAccountActived(100, (err, data)=>{
                                console.debug("====>localId: 100 isOsAccountActived err:" + JSON.stringify(err));
                                console.debug("====>localId: 100 isOsAccountActived data:" + data);
                                expect(err.code).assertEqual(0);
                                expect(data).assertTrue();
                                console.debug("====>ActsOsAccountActivate_1500 end====");
                                done();
                            })
                        })
                    })
                })
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountActivate_1600
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : Authenticate delete newly created active user
     */
    it('ActsOsAccountActivate_1600', 0, async function (done) {
        console.debug("====>ActsOsAccountActivate_1600 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var osAccountInfo = await osAccountManager.createOsAccount("osAccountNameTestActivate", 1);
        console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
        localId = osAccountInfo.localId;
        var isActived = await osAccountManager.isOsAccountActived(localId);
        console.debug("====>localId: " + localId + " isOsAccountActived:" + isActived);
        expect(isActived).assertFalse();
        await osAccountManager.activateOsAccount(localId);
        var isActivedTwice = await osAccountManager.isOsAccountActived(localId);           
        console.debug("====>after activing localId: " + localId + " isOsAccountActived:" + isActivedTwice);
        expect(isActivedTwice).assertTrue();
        await osAccountManager.removeOsAccount(localId);
        var isActiveHandred = await osAccountManager.isOsAccountActived(100);
        console.debug("====>localId: 100 isOsAccountActived:" + isActiveHandred);
        expect(isActiveHandred).assertTrue();
        console.debug("====>ActsOsAccountActivate_1600 end====");
        done();
    });
})