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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('ActsOsAccountPermissionTest', function () {

    /*
     * @tc.number  : ActsOsAccountPermissionManage_0100
     * @tc.name    : isOsAccountActived callback
     * @tc.desc    : only satisfies the MANAGE permission
     */
    it('ActsOsAccountPermissionManage_0100', 0, async function(done){
        console.debug("====>ActsOsAccountPermissionManage_0100 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.isOsAccountActived(100, (err, isActived)=>{
            console.debug("====>isOsAccountActived err:" + JSON.stringify(err));
            console.debug("====>isOsAccountActived isActived:" + isActived);
            expect(err.code).assertEqual(0);
            expect(isActived).assertEqual(true);
            console.debug("====>ActsOsAccountPermissionManage_0100 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermissionManage_0200
     * @tc.name    : isOsAccountActived promise
     * @tc.desc    : only satisfies the MANAGE permission
     */
    it('ActsOsAccountPermissionManage_0200', 0, async function(done){
        console.debug("====>ActsOsAccountPermissionManage_0200 start====");
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
        console.debug("====>ActsOsAccountPermissionManage_0200 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountPermissionManage_0300
     * @tc.name    : isOsAccountVerified callback
     * @tc.desc    : only satisfies the MANAGE permission
     */
    it('ActsOsAccountPermissionManage_0300', 0, async function(done){
        console.debug("====>ActsOsAccountPermissionManage_0300 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.isOsAccountVerified(100, (err, isVerified)=>{
            console.debug("====>isOsAccountVerified err:" + JSON.stringify(err));
            console.debug("====>isOsAccountVerified isVerified:" + isVerified);
            expect(err.code).assertEqual(0);
            expect(isVerified).assertEqual(false);
            console.debug("====>ActsOsAccountPermissionManage_0300 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermissionManage_0400
     * @tc.name    : isOsAccountVerified promise
     * @tc.desc    : only satisfies the MANAGE permission
     */
    it('ActsOsAccountPermissionManage_0400', 0, async function(done){
        console.debug("====>ActsOsAccountPermissionManage_0400 start====");
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
        console.debug("====>ActsOsAccountPermissionManage_0400 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountPermissionManage_0500
     * @tc.name    : queryOsAccountById callback
     * @tc.desc    : only satisfies the MANAGE permission
     */
    it('ActsOsAccountPermissionManage_0500', 0, async function(done){
        console.debug("====>ActsOsAccountPermissionManage_0500 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.queryOsAccountById(100, (err, data)=>{
            console.debug("====>queryOsAccountById err:" + JSON.stringify(err));
            console.debug("====>queryOsAccountById data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localId).assertEqual(100);
            expect(data.type.ADMIN).assertEqual(0);
            var serialNumberStr = data.serialNumber.toString();
            var serialIntercept = serialNumberStr.substring(8);
            console.debug("====>truncate the last eight characters: " + serialIntercept);
            expect(serialIntercept).assertEqual("00000001");
            expect(data.isCreateCompleted).assertTrue();
            console.debug("====>ActsOsAccountPermissionManage_0500 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountPermissionManage_0600
     * @tc.name    : queryOsAccountById promise
     * @tc.desc    : only satisfies the MANAGE permission
     */
    it('ActsOsAccountPermissionManage_0600', 0, async function(done){
        console.debug("====>ActsOsAccountPermissionManage_0600 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        var osAccountInfo = await AccountManager.queryOsAccountById(100);
        console.debug("====>queryOsAccountById osAccountInfo:" + JSON.stringify(osAccountInfo));
        expect(osAccountInfo.localId).assertEqual(100);
        expect(osAccountInfo.type.ADMIN).assertEqual(0);
        var serialNumberStr = osAccountInfo.serialNumber.toString();
        var serialIntercept = serialNumberStr.substring(8);
        console.debug("====>truncate the last eight characters: " + serialIntercept);
        expect(serialIntercept).assertEqual("00000001");
        expect(osAccountInfo.isCreateCompleted).assertTrue();
        console.debug("====>ActsOsAccountPermissionManage_0600 end====");
        done();
    })
})