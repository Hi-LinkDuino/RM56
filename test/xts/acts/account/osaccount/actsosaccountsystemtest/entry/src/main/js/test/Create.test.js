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
import osaccount from '@ohos.account.osaccount'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

const ERR_OS_ACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR = 4587523;
const ERR_OSACCOUNT_KIT_REMOVE_OSACCOUNT_ERROR = 4587529;
const ERR_OSACCOUNT_KIT_LOCAL_NAME_EMPTY_ERROR = 4587578;
const ERR_OSACCOUNT_KIT_CANNOT_DELETE_ID_ERROR = 4587581;
describe('ActsOsAccountSystemTest', function () {

    /*
     * @tc.number  : ActsOsAccountCreate_0100
     * @tc.name    : createOsAccount callback
     * @tc.desc    : Authenticate to create a local user of type normal
     */
    it('ActsOsAccountCreate_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_0100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        var createIdStr;
        let dataMap = new Map();
        osAccountManager.createOsAccount("osAccountNameCreateA", osaccount.OsAccountType.NORMAL, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            createIocalId = osAccountInfo.localId;
            createIdStr = createIocalId.toString();
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameCreateA");
            osAccountManager.queryAllCreatedOsAccounts((err, data)=>{
                console.debug("====>queryAllCreatedOsAccounts err:" + JSON.stringify(err));
                console.debug("====>queryAllCreatedOsAccounts data:" + JSON.stringify(data));
                expect(err.code).assertEqual(0);
                for (let i = 0, len = data.length; i < len; i++) {
                    console.debug("====>contain localId:" + data[i].localId);
                    dataMap.set(data[i].localId.toString(), data[i])
                }
                expect(dataMap.has("100")).assertTrue();
                expect(dataMap.has(createIdStr)).assertTrue();
                if (dataMap.has(createIdStr)) {
                    let OsAccountInfo = dataMap.get(createIdStr);
                    console.debug("====>query all local users containing: " + JSON.stringify(OsAccountInfo));
                    expect(OsAccountInfo.localName).assertEqual("osAccountNameCreateA");
                    expect(OsAccountInfo.type.NORMAL).assertEqual(1);
                    expect(OsAccountInfo.isActived).assertFalse();
                    osAccountManager.removeOsAccount(createIocalId, (err)=>{
                        console.debug("====>remove localId: " + createIocalId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountCreate_0100 end====");
                        done();
                    })
                }
            })
        });
    });

    /*
     * @tc.number  : ActsOsAccountCreate_0200
     * @tc.name    : createOsAccount promise
     * @tc.desc    : Authenticate to create a local user of type normal
     */
    it('ActsOsAccountCreate_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_0200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        var createIdStr;
        let dataMap = new Map();
        var osAccount = await osAccountManager.createOsAccount("osAccountNameCreateB", osaccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccount));
        createIocalId = osAccount.localId;
        createIdStr = createIocalId.toString();
        expect(osAccount.localName).assertEqual("osAccountNameCreateB");
        var data = await osAccountManager.queryAllCreatedOsAccounts();
        console.debug("====>queryAllCreatedOsAccounts data:" + JSON.stringify(data));
        for (let i = 0, len = data.length; i < len; i++) {
            console.debug("====>contain localId:" + data[i].localId);
            dataMap.set(data[i].localId.toString(), data[i])
        }
        expect(dataMap.has("100")).assertTrue();
        expect(dataMap.has(createIdStr)).assertTrue();
        if (dataMap.has(createIdStr)) {
            let OsAccountInfo = dataMap.get(createIdStr);
            console.debug("====>query all local users containing: " + JSON.stringify(OsAccountInfo));
            expect(OsAccountInfo.localName).assertEqual("osAccountNameCreateB");
            expect(OsAccountInfo.type.NORMAL).assertEqual(1);
            expect(OsAccountInfo.isActived).assertFalse();
        }
        await osAccountManager.removeOsAccount(createIocalId);
        console.debug("====>ActsOsAccountCreate_0200 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountCreate_0300
     * @tc.name    : createOsAccount callback
     * @tc.desc    : Authenticate to create a local user of type guest
     */
    it('ActsOsAccountCreate_0300', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_0300 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        var createIdStr;
        let dataMap = new Map();
        osAccountManager.createOsAccount("osAccountNameCreateC", osaccount.OsAccountType.GUEST, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            createIocalId = osAccountInfo.localId;
            createIdStr = createIocalId.toString();
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameCreateC");
            osAccountManager.queryAllCreatedOsAccounts((err, data)=>{
                console.debug("====>queryAllCreatedOsAccounts err:" + JSON.stringify(err));
                console.debug("====>queryAllCreatedOsAccounts data:" + JSON.stringify(data));
                expect(err.code).assertEqual(0);
                for (let i = 0, len = data.length; i < len; i++) {
                    console.debug("====>contain localId:" + data[i].localId);
                    dataMap.set(data[i].localId.toString(), data[i])
                }
                expect(dataMap.has("100")).assertTrue();
                expect(dataMap.has(createIdStr)).assertTrue();
                if (dataMap.has(createIdStr)) {
                    let OsAccountInfo = dataMap.get(createIdStr);
                    console.debug("====>query all local users containing: " + JSON.stringify(OsAccountInfo));
                    expect(OsAccountInfo.localName).assertEqual("osAccountNameCreateC");
                    expect(OsAccountInfo.type.GUEST).assertEqual(2);
                    expect(OsAccountInfo.isActived).assertFalse();
                    osAccountManager.removeOsAccount(createIocalId, (err)=>{
                        console.debug("====>remove localId: " + createIocalId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountCreate_0300 end====");
                        done();
                    })
                }
            })
        });
    });

    /*
     * @tc.number  : ActsOsAccountCreate_0400
     * @tc.name    : createOsAccount promise
     * @tc.desc    : Authenticate to create a local user of type guest
     */
    it('ActsOsAccountCreate_0400', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_0400 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        var createIdStr;
        let dataMap = new Map();
        var osAccount = await osAccountManager.createOsAccount("osAccountNameCreateD", osaccount.OsAccountType.GUEST);
        console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccount));
        createIocalId = osAccount.localId;
        createIdStr = createIocalId.toString();
        expect(osAccount.localName).assertEqual("osAccountNameCreateD");
        var data = await osAccountManager.queryAllCreatedOsAccounts();
        console.debug("====>queryAllCreatedOsAccounts data:" + JSON.stringify(data));
        for (let i = 0, len = data.length; i < len; i++) {
            console.debug("====>contain localId:" + data[i].localId);
            dataMap.set(data[i].localId.toString(), data[i])
        }
        expect(dataMap.has("100")).assertTrue();
        expect(dataMap.has(createIdStr)).assertTrue();
        if (dataMap.has(createIdStr)) {
            let OsAccountInfo = dataMap.get(createIdStr);
            console.debug("====>query all local users containing: " + JSON.stringify(OsAccountInfo));
            expect(OsAccountInfo.localName).assertEqual("osAccountNameCreateD");
            expect(OsAccountInfo.type.GUEST).assertEqual(2);
            expect(OsAccountInfo.isActived).assertFalse();
        }
        await osAccountManager.removeOsAccount(createIocalId);
        console.debug("====>ActsOsAccountCreate_0400 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountCreate_0500
     * @tc.name    : createOsAccount callback
     * @tc.desc    : Authenticate to create a local user of type admin
     */
    it('ActsOsAccountCreate_0500', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_0500 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        var createIdStr;
        let dataMap = new Map();
        osAccountManager.createOsAccount("osAccountNameCreateE", osaccount.OsAccountType.ADMIN, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            createIocalId = osAccountInfo.localId;
            createIdStr = createIocalId.toString();
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameCreateE");
            osAccountManager.queryAllCreatedOsAccounts((err, data)=>{
                console.debug("====>queryAllCreatedOsAccounts err:" + JSON.stringify(err));
                console.debug("====>queryAllCreatedOsAccounts data:" + JSON.stringify(data));
                expect(err.code).assertEqual(0);
                for (let i = 0, len = data.length; i < len; i++) {
                    console.debug("====>contain localId:" + data[i].localId);
                    dataMap.set(data[i].localId.toString(), data[i])
                }
                expect(dataMap.has("100")).assertTrue();
                expect(dataMap.has(createIdStr)).assertTrue();
                if (dataMap.has(createIdStr)) {
                    let OsAccountInfo = dataMap.get(createIdStr);
                    console.debug("====>query all local users containing: " + JSON.stringify(OsAccountInfo));
                    expect(OsAccountInfo.localName).assertEqual("osAccountNameCreateE");
                    expect(OsAccountInfo.type.ADMIN).assertEqual(0);
                    expect(OsAccountInfo.isActived).assertFalse();
                    osAccountManager.removeOsAccount(createIocalId, (err)=>{
                        console.debug("====>remove localId: " + createIocalId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountCreate_0500 end====");
                        done();
                    })
                }
            })
        });
    });

    /*
     * @tc.number  : ActsOsAccountCreate_0600
     * @tc.name    : createOsAccount promise
     * @tc.desc    : Authenticate to create a local user of type admin
     */
    it('ActsOsAccountCreate_0600', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_0600 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        var createIdStr;
        let dataMap = new Map();
        var osAccount = await osAccountManager.createOsAccount("osAccountNameCreateF", osaccount.OsAccountType.ADMIN);
        console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccount));
        createIocalId = osAccount.localId;
        createIdStr = createIocalId.toString();
        expect(osAccount.localName).assertEqual("osAccountNameCreateF");
        var data = await osAccountManager.queryAllCreatedOsAccounts();
        console.debug("====>queryAllCreatedOsAccounts data:" + JSON.stringify(data));
        for (let i = 0, len = data.length; i < len; i++) {
            console.debug("====>contain localId:" + data[i].localId);
            dataMap.set(data[i].localId.toString(), data[i])
        }
        expect(dataMap.has("100")).assertTrue();
        expect(dataMap.has(createIdStr)).assertTrue();
        if (dataMap.has(createIdStr)) {
            let OsAccountInfo = dataMap.get(createIdStr);
            console.debug("====>query all local users containing: " + JSON.stringify(OsAccountInfo));
            expect(OsAccountInfo.localName).assertEqual("osAccountNameCreateF");
            expect(OsAccountInfo.type.ADMIN).assertEqual(0);
            expect(OsAccountInfo.isActived).assertFalse();
        }
        await osAccountManager.removeOsAccount(createIocalId);
        console.debug("====>ActsOsAccountCreate_0600 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountCreate_0700
     * @tc.name    : createOsAccount callback
     * @tc.desc    : Verify that the created user name is empty
     */
    it('ActsOsAccountCreate_0700', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_0700 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.createOsAccount("", osaccount.OsAccountType.ADMIN, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_LOCAL_NAME_EMPTY_ERROR);
            console.debug("====>ActsOsAccountCreate_0700 end====");
            done();   
        });
    });

    /*
     * @tc.number  : ActsOsAccountCreate_0800
     * @tc.name    : createOsAccount promise
     * @tc.desc    : Verify that the created user name is empty
     */
    it('ActsOsAccountCreate_0800', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_0800 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.createOsAccount("", osaccount.OsAccountType.ADMIN);
        }catch(err){
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_LOCAL_NAME_EMPTY_ERROR);
            console.debug("====>ActsOsAccountCreate_0800 end====");
            done();
        }
    });

    /*
     * @tc.number  : ActsOsAccountCreate_0900
     * @tc.name    : removeOsAccount callback
     * @tc.desc    : Authenticate to delete the specified local user
     */
    it('ActsOsAccountCreate_0900', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_0900 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        var createIdStr;
        let dataMap = new Map();
        osAccountManager.createOsAccount("osAccountNameCreateG", osaccount.OsAccountType.NORMAL, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            createIocalId = osAccountInfo.localId;
            createIdStr = createIocalId.toString();
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameCreateG");
            osAccountManager.removeOsAccount(createIocalId, (err)=>{
                console.debug("====>remove localId: " + createIocalId + " err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                osAccountManager.queryAllCreatedOsAccounts((err, data)=>{
                    console.debug("====>queryAllCreatedOsAccounts err:" + JSON.stringify(err));
                    console.debug("====>queryAllCreatedOsAccounts data:" + JSON.stringify(data));
                    expect(err.code).assertEqual(0);
                    for (let i = 0, len = data.length; i < len; i++) {
                        console.debug("====>contain localId:" + data[i].localId);
                        dataMap.set(data[i].localId.toString(), data[i])
                    }
                    expect(dataMap.has("100")).assertTrue();
                    expect(dataMap.has(createIdStr)).assertFalse();
                    console.debug("====>ActsOsAccountCreate_0900 end====");
                    done();
                })
            })
        });
    });

    /*
     * @tc.number  : ActsOsAccountCreate_1000
     * @tc.name    : removeOsAccount promise
     * @tc.desc    : Authenticate to delete the specified local user
     */
    it('ActsOsAccountCreate_1000', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1000 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        var createIdStr;
        let dataMap = new Map();
        var osAccount = await osAccountManager.createOsAccount("osAccountNameCreateH", osaccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccount));
        createIocalId = osAccount.localId;
        createIdStr = createIocalId.toString();
        expect(osAccount.localName).assertEqual("osAccountNameCreateH");
        await osAccountManager.removeOsAccount(createIocalId);
        console.debug("====>remove localId: " + createIocalId);
        var data = await osAccountManager.queryAllCreatedOsAccounts();
        console.debug("====>queryAllCreatedOsAccounts data:" + JSON.stringify(data));
        for (let i = 0, len = data.length; i < len; i++) {
            console.debug("====>contain localId:" + data[i].localId);
            dataMap.set(data[i].localId.toString(), data[i])
        }
        expect(dataMap.has("100")).assertTrue();
        expect(dataMap.has(createIdStr)).assertFalse();
        console.debug("====>ActsOsAccountCreate_1000 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountCreate_1100
     * @tc.name    : removeOsAccount callback
     * @tc.desc    : Verify duplicate removal of the same local user
     */
    it('ActsOsAccountCreate_1100', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        osAccountManager.createOsAccount("osAccountNameCreateI", osaccount.OsAccountType.NORMAL, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            createIocalId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameCreateI");
            osAccountManager.removeOsAccount(createIocalId, (err)=>{
                console.debug("====>remove localId: " + createIocalId + " err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                osAccountManager.removeOsAccount(createIocalId, (err)=>{
                    console.debug("====>remove localId: " + createIocalId + " err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_REMOVE_OSACCOUNT_ERROR);
                    console.debug("====>ActsOsAccountCreate_1100 end====");
                    done();
                })
            })
        });
    });

    /*
     * @tc.number  : ActsOsAccountCreate_1200
     * @tc.name    : removeOsAccount promise
     * @tc.desc    : Verify duplicate removal of the same local user
     */
    it('ActsOsAccountCreate_1200', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var createIocalId;
        var osAccount = await osAccountManager.createOsAccount("osAccountNameCreateA", osaccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccount));
        createIocalId = osAccount.localId;
        expect(osAccount.localName).assertEqual("osAccountNameCreateA");
        await osAccountManager.removeOsAccount(createIocalId);
        console.debug("====>remove localId: " + createIocalId);
        try{
            await osAccountManager.removeOsAccount(createIocalId);
        }catch(err){
            console.debug("====>remove localId: " + createIocalId + " err: " + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_REMOVE_OSACCOUNT_ERROR);
            console.debug("====>ActsOsAccountCreate_1200 end====");
            done();
        }  
    });

    /*
     * @tc.number  : ActsOsAccountCreate_1300
     * @tc.name    : removeOsAccount callback
     * @tc.desc    : Validation cannot delete 0 user
     */
    it('ActsOsAccountCreate_1300', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1300 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId = 0;
        osAccountManager.removeOsAccount(localId, (err)=>{
            console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_CANNOT_DELETE_ID_ERROR);
            console.debug("====>ActsOsAccountCreate_1300 end====");
            done();
        })
    });

    /*
     * @tc.number  : ActsOsAccountCreate_1400
     * @tc.name    : removeOsAccount promise
     * @tc.desc    : Validation cannot delete 0 user
     */
    it('ActsOsAccountCreate_1400', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1400 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId = 0;
        try{
            await osAccountManager.removeOsAccount(localId);
        }catch(err){
            console.debug("====>remove localId: " + localId + " err: " + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_CANNOT_DELETE_ID_ERROR);
            console.debug("====>ActsOsAccountCreate_1400 end====");
            done();
        }  
    });
    
    /*
     * @tc.number  : ActsOsAccountCreate_1500
     * @tc.name    : removeOsAccount callback
     * @tc.desc    : Validation cannot delete 100 user
     */
    it('ActsOsAccountCreate_1500', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1500 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId = 100;
        osAccountManager.removeOsAccount(localId, (err)=>{
            console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_CANNOT_DELETE_ID_ERROR);
            console.debug("====>ActsOsAccountCreate_1500 end====");
            done();
        })
    });

    /*
     * @tc.number  : ActsOsAccountCreate_1600
     * @tc.name    : removeOsAccount promise
     * @tc.desc    : Validation cannot delete 100 user
     */
    it('ActsOsAccountCreate_1600', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1600 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId = 100;
        try{
            await osAccountManager.removeOsAccount(localId);
        }catch(err){
            console.debug("====>remove localId: " + localId + " err: " + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_CANNOT_DELETE_ID_ERROR);
            console.debug("====>ActsOsAccountCreate_1600 end====");
            done();
        }  
    });

    /*
     * @tc.number  : ActsOsAccountCreate_1700
     * @tc.name    : createOsAccount callback
     * @tc.desc    : Authentication failed to create user with parameter type not matching
     */
    it('ActsOsAccountCreate_1700', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1700 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.createOsAccount("osAccountNameCreateA", -1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            expect(err.code).assertEqual(ERR_OS_ACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountCreate_1700 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountCreate_1800
     * @tc.name    : createOsAccount promise
     * @tc.desc    : Authentication failed to create user with parameter type not matching
     */
    it('ActsOsAccountCreate_1800', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1800 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.createOsAccount("osAccountNameCreateA", -1);
        }catch(err){
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OS_ACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountCreate_1800 end====");
            done();
        }
    });

    /*
     * @tc.number  : ActsOsAccountCreate_1900
     * @tc.name    : createOsAccount callback
     * @tc.desc    : Authentication failed to create user with parameter type mismatch
     */
    it('ActsOsAccountCreate_1900', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_1900 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.createOsAccount("osAccountNameCreateA", 3, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            expect(err.code).assertEqual(ERR_OS_ACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountCreate_1900 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountCreate_2000
     * @tc.name    : createOsAccount promise
     * @tc.desc    : Authentication failed to create user with parameter type mismatch
     */
    it('ActsOsAccountCreate_2000', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_2000 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.createOsAccount("osAccountNameCreateA", 3);
        }catch(err){
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OS_ACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountCreate_2000 end====");
            done();
        }
    });

    /*
     * @tc.number  : ActsOsAccountCreate_2100
     * @tc.name    : createOsAccount
     * @tc.desc    : Verify that the user localId can be reused and the user serialNumber is not
     */
    it('ActsOsAccountCreate_2100', 0, async function (done) {
        console.debug("====>ActsOsAccountCreate_2100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var OsAccountInfo = await osAccountManager.createOsAccount("nameCreateSerial", osaccount.OsAccountType.NORMAL);
        console.debug("====>first createOsAccount OsAccountInfo:" + JSON.stringify(OsAccountInfo));
        console.debug("====>createTime: " + OsAccountInfo.createTime);
        console.debug("====>lastLoginTime: " + OsAccountInfo.lastLoginTime);
        console.debug("====>serialNumber: " + OsAccountInfo.serialNumber);
        console.debug("====>domainInfo: " + OsAccountInfo.domainInfo);
        var localIdFir = OsAccountInfo.localId;
        var serialNumFir = OsAccountInfo.serialNumber;
        expect(OsAccountInfo.localName).assertEqual("nameCreateSerial");
        console.debug("====>remove os account start====");
        await osAccountManager.removeOsAccount(localIdFir);
        var accountInfoSec = await osAccountManager.createOsAccount("nameCreateSerial", osaccount.OsAccountType.NORMAL);
        console.debug("====>second createOsAccount OsAccountInfo:" + JSON.stringify(accountInfoSec));
        var localIdSec = accountInfoSec.localId;
        var serialNumSec = accountInfoSec.serialNumber;
        await osAccountManager.removeOsAccount(localIdSec);
        console.debug("====>first create localId: " + localIdFir + " second create localId: " + localIdSec);
        console.debug("====>first create serialNumber: " + serialNumFir + " second serialNumber: " + serialNumSec);
        expect(localIdFir).assertEqual(localIdSec);
        var serialNumAdd = serialNumFir + 1;
        expect(serialNumAdd).assertEqual(serialNumSec);
        console.debug("====>ActsOsAccountCreate_2100 end====");
        done();
    });
})