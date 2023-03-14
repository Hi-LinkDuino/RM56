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
import osAccount from '@ohos.account.osAccount'
import commonevent from '@ohos.commonevent'
import featureAbility from '@ohos.ability.featureability'
import bundle from '@ohos.bundle'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

var commonEventSubscribeInfo = {
    events: ["osaccount_event_create_permission"]
}
var subscriber;
var receiveLocalId = -1;
const TIMEOUT = 2000;
const TIMEOUTSTART = 20000;
const OSACCOUNTMAXNUMBER = 999;
const ERR_OSACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR = 4587523;
const ERR_OSACCOUNT_KIT_IS_OS_ACCOUNT_ACTIVED_ERROR = 4587542;
const ERR_OSACCOUNT_KIT_IS_OS_ACCOUNT_CONSTRAINT_ENABLE_ERROR = 4587543;
const ERR_OSACCOUNT_KIT_IS_OS_ACCOUNT_VERIFIED_ERROR = 4587545;
const ERR_OSACCOUNT_KIT_GET_CREATED_OS_ACCOUNT_COUNT_ERROR = 4587546;
const ERR_OSACCOUNT_KIT_GET_OS_ACCOUNT_ALL_CONSTRAINTS_ERROR = 4587550;
const ERR_OSACCOUNT_KIT_QUERY_CURRENT_OS_ACCOUNT_ERROR = 4587552;
const ERR_OSACCOUNT_KIT_QUERY_OS_ACCOUNT_BY_ID_ERROR = 4587553;
const ERR_OSACCOUNT_KIT_GET_OS_ACCOUNT_PROFILE_PHOTO_ERROR = 4587555;
const ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR = 4587563;
const ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_CONSTRAINTS_ERROR = 4587562;
const ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR = 4587571;
const ERR_OSACCOUNT_KIT_REMOVE_OSACCOUNT_ERROR = 4587529;
describe('ActsOsAccountPermissionTest', function () {
    function sleep(delay) {
        var start = (new Date()).getTime();
        while((new Date()).getTime() - start < delay) {
            continue;
        }
    }

    beforeAll(async function (done) {
        console.debug("====>subscriber start====");
        function subscriberCallback(err, data){
            console.debug("====>subscriberCallback data:" + JSON.stringify(data));
            expect(data.event).assertEqual("osaccount_event_create_permission");
            receiveLocalId = data.code;
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
        sleep(TIMEOUT);
        console.debug("====>startAbility start====");
        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsosaccountscenepermission",
                    abilityName: "com.example.actsosaccountscenepermission.MainAbility",
                    action: "action1",
                    parameters:
                    {},
                },
            },
        );
        sleep(TIMEOUTSTART);
        done();
    });

    afterAll(async function (done) {
        console.debug("====>afterAll enter====");
        function publishCallback(err){
            console.debug("====>publish call back scene err:" + JSON.stringify(err));
        }
        commonevent.publish("osaccount_event_delete_permission", publishCallback);
        sleep(TIMEOUT);
        console.debug("====>afterAll finish====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountPermission_0100
     * @tc.name    : setOsAccountConstraints callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_0100', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_0100 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.setOsAccountConstraints(100, ["constraint.cell.broadcasts.set"], true, (err)=>{
            console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_CONSTRAINTS_ERROR);
            console.debug("====>ActsOsAccountPermission_0100 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_0200
     * @tc.name    : setOsAccountConstraints promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_0200', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_0200 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            await AccountManager.setOsAccountConstraints(100, ["constraint.cell.broadcasts.set"], true);
        }
        catch(err){
            console.debug("====>setOsAccountConstraints err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_CONSTRAINTS_ERROR);
            console.debug("====>ActsOsAccountPermission_0200 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_0300
     * @tc.name    : createOsAccount callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_0300', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_0300 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL, (err,info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountPermission_0300 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_0400
     * @tc.name    : createOsAccount promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_0400', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_0400 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            await AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL);
        }
        catch(err){
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountPermission_0400 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_0500
     * @tc.name    : queryOsAccountById callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_0500', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_0500 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.queryOsAccountById(100, (err)=>{
            console.debug("====>queryOsAccountById err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_QUERY_OS_ACCOUNT_BY_ID_ERROR);
            console.debug("====>ActsOsAccountPermission_0500 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_0600
     * @tc.name    : queryOsAccountById promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_0600', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_0600 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            await AccountManager.queryOsAccountById(100);
        }
        catch(err){
            console.debug("====>queryOsAccountById err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_QUERY_OS_ACCOUNT_BY_ID_ERROR);
            console.debug("====>ActsOsAccountPermission_0600 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_0700
     * @tc.name    : getOsAccountProfilePhoto callback
     * @tc.desc    : get the photo of the specified user does not meet the permission
     */
    it('ActsOsAccountPermission_0700', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_0700 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != -1){
            AccountManager.getOsAccountProfilePhoto(receiveLocalId, (err)=>{
                console.debug("====>getOsAccountProfilePhoto err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_GET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
                console.debug("====>ActsOsAccountPermission_0700 end====");
                done();
            })
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_0800
     * @tc.name    : getOsAccountProfilePhoto promise
     * @tc.desc    : get the photo of the specified user does not meet the permission
     */
    it('ActsOsAccountPermission_0800', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_0800 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != -1){
            try{
                await AccountManager.getOsAccountProfilePhoto(receiveLocalId);
            }
            catch(err){
                console.debug("====>getOsAccountProfilePhoto err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_GET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
                console.debug("====>ActsOsAccountPermission_0800 end====");
                done();
            }
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_0900
     * @tc.name    : setOsAccountProfilePhoto callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_0900', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_0900 start====");
        var AccountManager = osAccount.getAccountManager();
        var photo = "data:image/jpeg;base64,/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAMCAgMCAgMDAwMEAwMEBQgFBQQEBQoHBwYIDAoMD"+
        "AsKCwsNDhIQDQ4RDgsLEBYQERMUFRUVDA8XGBYUGBIUFRT/2wBDAQMEBAUEBQkFBQkUDQsNFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUF"+
        "BQUFBQUFBQUFBQUFBQUFBQUFBQUFBT/wAARCAALAAwDASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAA"+
        "gEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTV"+
        "FVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5"+
        "ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBU"+
        "QdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEh"+
        "YaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAP"+
        "wD2v4p+OPEXwz+LN3rvivwn4nh0bV9dsP7Oe51nRLOC7EF0kpEkL6psLRLDGsVwFhZkm8ucFljc/R1z8SPiZqBjl0n4SXljb7cPD4g1iwiuA"+
        "+TyBbTzoVxtwd+c5yBwT1/hv4W+F/CupalqVjpSy6rqQaO81PUJpL28niLFvJaednkMKljti3bEzhVArQ8H+CtE+H+iro/h7T49J0mORpIbC"+
        "3ZvIt9xyUiQnbFGO0aAIMnAGaxlTcqPsnJrrdb7Jfpv1vt1eilap7Syfk/n/nt09NF//9k=";
        console.debug("====>get os AccountManager finish====");
        AccountManager.setOsAccountProfilePhoto(100, photo, (err)=>{
            console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountPermission_0900 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1000
     * @tc.name    : setOsAccountProfilePhoto promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1000', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1000 start====");
        var AccountManager = osAccount.getAccountManager();
        var photo = "data:image/jpeg;base64,/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAMCAgMCAgMDAwMEAwMEBQgFBQQEBQoHBwYIDAoMD"+
        "AsKCwsNDhIQDQ4RDgsLEBYQERMUFRUVDA8XGBYUGBIUFRT/2wBDAQMEBAUEBQkFBQkUDQsNFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUF"+
        "BQUFBQUFBQUFBQUFBQUFBQUFBQUFBT/wAARCAALAAwDASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAA"+
        "gEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTV"+
        "FVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5"+
        "ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBU"+
        "QdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEh"+
        "YaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAP"+
        "wD2v4p+OPEXwz+LN3rvivwn4nh0bV9dsP7Oe51nRLOC7EF0kpEkL6psLRLDGsVwFhZkm8ucFljc/R1z8SPiZqBjl0n4SXljb7cPD4g1iwiuA"+
        "+TyBbTzoVxtwd+c5yBwT1/hv4W+F/CupalqVjpSy6rqQaO81PUJpL28niLFvJaednkMKljti3bEzhVArQ8H+CtE+H+iro/h7T49J0mORpIbC"+
        "3ZvIt9xyUiQnbFGO0aAIMnAGaxlTcqPsnJrrdb7Jfpv1vt1eilap7Syfk/n/nt09NF//9k=";
        console.debug("====>get os AccountManager finish====");
        try{
            await AccountManager.setOsAccountProfilePhoto(100, photo);
        }
        catch(err){
            console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountPermission_1000 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1100
     * @tc.name    : isOsAccountActived callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1100', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1100 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != -1){
            AccountManager.isOsAccountActived(receiveLocalId, (err)=>{
                console.debug("====>isOsAccountActived err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_IS_OS_ACCOUNT_ACTIVED_ERROR);
                console.debug("====>ActsOsAccountPermission_1100 end====");
                done();
            })
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1200
     * @tc.name    : isOsAccountActived promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1200', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1200 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != -1){
            try{
                await AccountManager.isOsAccountActived(receiveLocalId);
            }
            catch(err){
                console.debug("====>isOsAccountActived err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_IS_OS_ACCOUNT_ACTIVED_ERROR);
                console.debug("====>ActsOsAccountPermission_1200 end====");
                done();
            }
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1300
     * @tc.name    : isOsAccountConstraintEnable callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1300', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1300 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.isOsAccountConstraintEnable(100, "constraint.bluetooth", (err)=>{
            console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_IS_OS_ACCOUNT_CONSTRAINT_ENABLE_ERROR);
            console.debug("====>ActsOsAccountPermission_1300 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1400
     * @tc.name    : isOsAccountConstraintEnable promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1400', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1400 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            await AccountManager.isOsAccountConstraintEnable(100, "constraint.bluetooth");
        }
        catch(err){
            console.debug("====>isOsAccountConstraintEnable err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_IS_OS_ACCOUNT_CONSTRAINT_ENABLE_ERROR);
            console.debug("====>ActsOsAccountPermission_1400 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1500
     * @tc.name    : isOsAccountVerified callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1500', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1500 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != 0){
            AccountManager.isOsAccountVerified(receiveLocalId, (err)=>{
                console.debug("====>isOsAccountVerified err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_IS_OS_ACCOUNT_VERIFIED_ERROR);
                console.debug("====>ActsOsAccountPermission_1500 end====");
                done();
            })
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1600
     * @tc.name    : isOsAccountVerified promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1600', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1600 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != -1){
            try{
                await AccountManager.isOsAccountVerified(receiveLocalId);
            }
            catch(err){
                console.debug("====>isOsAccountVerified err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_IS_OS_ACCOUNT_VERIFIED_ERROR);
                console.debug("====>ActsOsAccountPermission_1600 end====");
                done();
            }
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1700
     * @tc.name    : getCreatedOsAccountsCount callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1700', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1700 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.getCreatedOsAccountsCount((err)=>{
            console.debug("====>getCreatedOsAccountsCount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_GET_CREATED_OS_ACCOUNT_COUNT_ERROR);
            console.debug("====>ActsOsAccountPermission_1700 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1800
     * @tc.name    : getCreatedOsAccountsCount promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1800', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1800 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            await AccountManager.getCreatedOsAccountsCount();
        }
        catch(err){
            console.debug("====>getCreatedOsAccountsCount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_GET_CREATED_OS_ACCOUNT_COUNT_ERROR);
            console.debug("====>ActsOsAccountPermission_1800 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_1900
     * @tc.name    : getOsAccountAllConstraints callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_1900', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_1900 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.getOsAccountAllConstraints(100, (err)=>{
            console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_GET_OS_ACCOUNT_ALL_CONSTRAINTS_ERROR);
            console.debug("====>ActsOsAccountPermission_1900 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2000
     * @tc.name    : getOsAccountAllConstraints promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_2000', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2000 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            await AccountManager.getOsAccountAllConstraints(100);
        }
        catch(err){
            console.debug("====>getOsAccountAllConstraints err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_GET_OS_ACCOUNT_ALL_CONSTRAINTS_ERROR);
            console.debug("====>ActsOsAccountPermission_2000 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2100
     * @tc.name    : queryCurrentOsAccount callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_2100', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2100 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.queryCurrentOsAccount((err)=>{
            console.debug("====>queryCurrentOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_QUERY_CURRENT_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountPermission_2100 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2200
     * @tc.name    : queryCurrentOsAccount promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_2200', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2200 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            await AccountManager.queryCurrentOsAccount();
        }
        catch(err){
            console.debug("====>queryCurrentOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_QUERY_CURRENT_OS_ACCOUNT_ERROR);
            console.debug("====>ActsOsAccountPermission_2200 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2300
     * @tc.name    : getOsAccountProfilePhoto callback
     * @tc.desc    : get current user photo does not meet the permission
     */
    it('ActsOsAccountPermission_2300', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2300 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.getOsAccountProfilePhoto(100, (err, photo)=>{
            console.debug("====>getOsAccountProfilePhoto err:" + JSON.stringify(err));
            console.debug("====>getOsAccountProfilePhoto photo:" + photo);
            expect(err.code).assertEqual(0);
            expect(photo).assertEqual("");
            console.debug("====>ActsOsAccountPermission_2300 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2400
     * @tc.name    : getOsAccountProfilePhoto promise
     * @tc.desc    : get current user photo does not meet the permission
     */
    it('ActsOsAccountPermission_2400', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2400 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            var photo = await AccountManager.getOsAccountProfilePhoto(100);
        }
        catch(err){
            console.debug("====>getOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getOsAccountProfilePhoto photo:" + photo);
        expect(photo).assertEqual("");
        console.debug("====>ActsOsAccountPermission_2400 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2500
     * @tc.name    : isOsAccountActived callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_2500', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2500 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.isOsAccountActived(100, (err, isActived)=>{
            console.debug("====>isOsAccountActived err:" + JSON.stringify(err));
            console.debug("====>isOsAccountActived isActived:" + isActived);
            expect(err.code).assertEqual(0);
            expect(isActived).assertEqual(true);
            console.debug("====>ActsOsAccountPermission_2500 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2600
     * @tc.name    : isOsAccountActived promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_2600', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2600 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            var isActived = await AccountManager.isOsAccountActived(100);
        }
        catch(err){
            console.debug("====>isOsAccountActived err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>isOsAccountActived isActived:" + isActived);
        expect(isActived).assertEqual(true);
        console.debug("====>ActsOsAccountPermission_2600 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2700
     * @tc.name    : isOsAccountVerified callback
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_2700', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2700 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.isOsAccountVerified(100, (err, isVerified)=>{
            console.debug("====>isOsAccountVerified err:" + JSON.stringify(err));
            console.debug("====>isOsAccountVerified isVerified:" + isVerified);
            expect(err.code).assertEqual(0);
            expect(isVerified).assertEqual(false);
            console.debug("====>ActsOsAccountPermission_2700 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2800
     * @tc.name    : isOsAccountVerified promise
     * @tc.desc    : the application call interface does not meet the permissions
     */
    it('ActsOsAccountPermission_2800', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2800 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            var isVerified = await AccountManager.isOsAccountVerified(100);
        }
        catch(err){
            console.debug("====>isOsAccountVerified err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>isOsAccountVerified isVerified:" + isVerified);
        expect(isVerified).assertEqual(false);
        console.debug("====>ActsOsAccountPermission_2800 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountPermission_2900
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : activate specified user does not meet the permission
     */
    it('ActsOsAccountPermission_2900', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_2900 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != -1){
            AccountManager.activateOsAccount(receiveLocalId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
                console.debug("====>ActsOsAccountPermission_2900 end====");
                done();
            })
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_3000
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : activate specified user does not meet the permission
     */
    it('ActsOsAccountPermission_3000', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_3000 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != -1){
            try{
                await AccountManager.activateOsAccount(receiveLocalId);
            }
            catch(err){
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
                console.debug("====>ActsOsAccountPermission_3000 end====");
                done();
            }
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_3100
     * @tc.name    : removeOsAccount callback
     * @tc.desc    : remove specified user does not meet the permission
     */
    it('ActsOsAccountPermission_3100', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_3100 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != -1){
            AccountManager.removeOsAccount(receiveLocalId, (err)=>{
                console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_REMOVE_OSACCOUNT_ERROR);
                console.debug("====>ActsOsAccountPermission_3100 end====");
                done();
            })
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_3200
     * @tc.name    : removeOsAccount promise
     * @tc.desc    : remove specified user does not meet the permission
     */
    it('ActsOsAccountPermission_3200', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_3200 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        if(receiveLocalId != -1){
            try{
                await AccountManager.removeOsAccount(receiveLocalId);
            }
            catch(err){
                console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_REMOVE_OSACCOUNT_ERROR);
                console.debug("====>ActsOsAccountPermission_3200 end====");
                done();
            }
        }
        else{
            console.debug("====>user creation failed====");
            expect().assertFail();
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountPermission_3300
     * @tc.name    : isMultiOsAccountEnable callback
     * @tc.desc    : whether to support multiple user does not meet the permission
     */
    it('ActsOsAccountPermission_3300', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_3300 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.isMultiOsAccountEnable((err, data)=>{
            console.debug("====>isMultiOsAccountEnable err:" + JSON.stringify(err));
            console.debug("====>isMultiOsAccountEnable data:" + data);
            expect(err.code).assertEqual(0);
            expect(data).assertEqual(true);
            console.debug("====>ActsOsAccountPermission_3300 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_3400
     * @tc.name    : isMultiOsAccountEnable promise
     * @tc.desc    : whether to support multiple user does not meet the permission
     */
    it('ActsOsAccountPermission_3400', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_3400 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        try{
            var data = await AccountManager.isMultiOsAccountEnable();
        }
        catch(err){
            console.debug("====>catch isMultiOsAccountEnable err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>isMultiOsAccountEnable data:" + JSON.stringify(data));
        expect(data).assertEqual(true);
        console.debug("====>ActsOsAccountPermission_3400 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountPermission_3500
     * @tc.name    : queryMaxOsAccountNumber callback
     * @tc.desc    : Query the maximum number of users does not meet the permission
     */
    it('ActsOsAccountPermission_3500', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_3500 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.queryMaxOsAccountNumber((err, data)=>{
            console.debug("====>queryMaxOsAccountNumber err:" + JSON.stringify(err));
            console.debug("====>queryMaxOsAccountNumber data:" + data);
            expect(err.code).assertEqual(0);
            expect(data).assertEqual(OSACCOUNTMAXNUMBER);
            console.debug("====>ActsOsAccountPermission_3500 end====");
            done();
        })
    });

    /*
     * @tc.number  : ActsOsAccountPermission_3600
     * @tc.name    : queryMaxOsAccountNumber promise
     * @tc.desc    : Query the maximum number of users does not meet the permission
     */
    it('ActsOsAccountPermission_3600', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_3600 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var maxOsAccount = await osAccountManager.queryMaxOsAccountNumber();
        console.debug("====>queryMaxOsAccountNumber:" + maxOsAccount);
        expect(maxOsAccount).assertEqual(OSACCOUNTMAXNUMBER);
        console.debug("====>ActsOsAccountPermission_3600 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountPermission_3700
     * @tc.name    : getOsAccountLocalIdFromProcess callback
     * @tc.desc    : Verify that the user localId obtained from the current process uid does not meet the permission
     */
    it('ActsOsAccountPermission_3700', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_3700 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.getOsAccountLocalIdFromProcess((err, localId)=>{
            console.debug("====>get localId err: " + JSON.stringify(err));
            console.debug("====>localId obtained by process:" + localId);
            expect(err.code).assertEqual(0);
            expect(localId).assertEqual(100);
            console.debug("====>ActsOsAccountPermission_3700 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountPermission_3800
     * @tc.name    : getOsAccountLocalIdFromProcess promise
     * @tc.desc    : Verify that the user localId obtained from the current process uid does not meet the permission
     */
    it('ActsOsAccountPermission_3800', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_3800 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId = await osAccountManager.getOsAccountLocalIdFromProcess();
        console.debug("====>localId obtained by process:" + localId);
        expect(localId).assertEqual(100);
        console.debug("====>ActsOsAccountPermission_3800 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountPermission_3900
     * @tc.name    : getOsAccountLocalIdFromUid callback
     * @tc.desc    : Verify that the user localId is obtained by uid does not meet the permission
     */
    it('ActsOsAccountPermission_3900', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_3900 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var bundleName = "com.example.actsosaccountpermissionnotest";
        var bundleInfo = await bundle.getBundleInfo(bundleName, bundle.BundleFlag.GET_BUNDLE_WITH_ABILITIES);
        var uid = bundleInfo.uid;
        console.debug("====>obtained uid:" + uid);
        osAccountManager.getOsAccountLocalIdFromUid(uid, (err, localId)=>{
            console.debug("====>get localId err: " + JSON.stringify(err));
            console.debug("====>localId obtained by uid:" + localId);
            expect(err.code).assertEqual(0);
            expect(localId).assertEqual(100);
            console.debug("====>ActsOsAccountPermission_3900 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountPermission_4000
     * @tc.name    : getOsAccountLocalIdFromUid promise
     * @tc.desc    : Verify that the user localId is obtained by uid does not meet the permission
     */
    it('ActsOsAccountPermission_4000', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_4000 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var bundleName = "com.example.actsosaccountpermissionnotest";
        var bundleInfo = await bundle.getBundleInfo(bundleName, bundle.BundleFlag.GET_BUNDLE_WITH_ABILITIES);
        var uid = bundleInfo.uid;
        console.debug("====>obtained uid:" + uid);
        var localId = await osAccountManager.getOsAccountLocalIdFromUid(uid);
        console.debug("====>localId obtained by uid:" + localId);
        expect(localId).assertEqual(100);
        console.debug("====>ActsOsAccountPermission_4000 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountPermission_4100
     * @tc.name    : getOsAccountTypeFromProcess callback
     * @tc.desc    : Verify that the user type obtained from the current process uid does not meet the permission
     */
    it('ActsOsAccountPermission_4100', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_4100 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.getOsAccountTypeFromProcess((err, accountType)=>{
            console.debug("====>get type err: " + JSON.stringify(err));
            console.debug("====>type obtained by process:" + JSON.stringify(accountType));
            expect(err.code).assertEqual(0);
            expect(accountType.ADMIN).assertEqual(0);
            console.debug("====>ActsOsAccountPermission_4100 end====");
            done();
        });
    });

    /*
     * @tc.number  : ActsOsAccountPermission_4200
     * @tc.name    : getOsAccountTypeFromProcess promise
     * @tc.desc    : Verify that the user type obtained from the current process uid does not meet the permission
     */
    it('ActsOsAccountPermission_4200', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_4200 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var accountType = await osAccountManager.getOsAccountTypeFromProcess();
        console.debug("====>type obtained by process:" + JSON.stringify(accountType));
        expect(accountType.ADMIN).assertEqual(0);
        console.debug("====>ActsOsAccountPermission_4200 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountPermission_4300
     * @tc.name    : getSerialNumberByOsAccountLocalId callback
     * @tc.desc    : Verify query serialNumber by 100 user and query 100 user by serialNumber does not meet the
     *               permission
     */
    it('ActsOsAccountPermission_4300', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_4300 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.getSerialNumberByOsAccountLocalId(100, (err, serialNumber)=>{
            console.debug("====>ger serialNumber err:" + JSON.stringify(err));
            console.debug("====>get serialNumber:" + serialNumber + " by localId: 100" );
            expect(err.code).assertEqual(0);
            var serialNumberStr = serialNumber.toString();
            var serialIntercept = serialNumberStr.substring(8);
            console.debug("====>truncate the last eight characters: " + serialIntercept);
            expect(serialIntercept).assertEqual("00000001");
            osAccountManager.getOsAccountLocalIdBySerialNumber(serialNumber, (err, localId)=>{
                console.debug("====>ger localId err:" + JSON.stringify(err));
                console.debug("====>get localId:" + localId + " by serialNumber: " + serialNumber);
                expect(err.code).assertEqual(0);
                expect(localId).assertEqual(100);
                console.debug("====>ActsOsAccountPermission_4300 end====");
                done();
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountPermission_4400
     * @tc.name    : getSerialNumberByOsAccountLocalId promise
     * @tc.desc    : Verify query serialNumber by 100 user and query 100 user by serialNumber does not meet the
     *               permission
     */
    it('ActsOsAccountPermission_4400', 0, async function (done) {
        console.debug("====>ActsOsAccountPermission_4400 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var serialNumber = await osAccountManager.getSerialNumberByOsAccountLocalId(100);
        console.debug("====>get serialNumber:" + serialNumber + " by localId: 100" );
        var serialNumberStr = serialNumber.toString();
        var serialIntercept = serialNumberStr.substring(8);
        console.debug("====>truncate the last eight characters: " + serialIntercept);
        expect(serialIntercept).assertEqual("00000001");
        var localId = await osAccountManager.getOsAccountLocalIdBySerialNumber(serialNumber);
        console.debug("====>get localId:" + localId + " by serialNumber: " + serialNumber);
        expect(localId).assertEqual(100);
        console.debug("====>ActsOsAccountPermission_4400 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountPermission_4500
     * @tc.name    : isTestOsAccount callback
     * @tc.desc    : check whether this OS account is a test OS account does not meet the permission
     */
    it('ActsOsAccountPermission_4500', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_4500 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.isTestOsAccount((err, data)=>{
            console.debug("====>isTestOsAccount err:" + JSON.stringify(err));
            console.debug("====>isTestOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data).assertEqual(false);
            console.debug("====>ActsOsAccountPermission_4500 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermission_4600
     * @tc.name    : isTestOsAccount promise
     * @tc.desc    : check whether this OS account is a test OS account does not meet the permission
     */
    it('ActsOsAccountPermission_4600', 0, async function(done){
        console.debug("====>ActsOsAccountPermission_4600 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        var isTest = await AccountManager.isTestOsAccount();
        expect(isTest).assertFalse();
        console.debug("====>ActsOsAccountPermission_4600 end====");
        done();
    })
})