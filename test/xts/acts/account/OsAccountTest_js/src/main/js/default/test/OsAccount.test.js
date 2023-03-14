/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

// @ts-nocheck
import {describe, beforeEach, afterEach, it, expect} from 'deccjsunit/index'
import account from '@ohos.account.distributedAccount'

describe('AccountTest', function () {
    beforeEach(function () {
    })
    afterEach(function () {
    })

    /**
    * @tc.number     ActsDistributedAccountDeviceId_0100
    * @tc.name       Test query the distribruted id by callback.
    * @tc.desc       Test distributedAccount.queryOsAccountDistributedInfo API functionality by callback.
    */
   it('ActsDistributedAccountDeviceId_0100', 0, async function(done){
        console.log("test query distribtued id start");
        const distributedId = '169C610CEF7F8310D78763C7DD8D28F0668D9FB8B1A3659CEF8F6BC3D9B39E17';
        const accountAbility = account.getDistributedAccountAbility();
        accountAbility.updateOsAccountDistributedInfo(
        {
            name: 'ZhangSan',
            id: '12345',
            event: "Ohos.account.event.LOGIN"
        }, (err)=>{
            console.log("update distributedInfo err:" + JSON.stringify(err));
            accountAbility.queryOsAccountDistributedInfo((err, distributedInfo)=>{
                console.log("query distributedInfo err:" + JSON.stringify(err));
                console.log("query distributedInfo:" + JSON.stringify(distributedInfo));
                expect(distributedInfo.name).assertEqual('ZhangSan');
                expect(distributedInfo.id).assertEqual(distributedId);
                console.log("test query distribtued id end");
                done();
            })
        })
    })

    /**
    * @tc.number     SUB_Account_distributedAccount_JS_API_0100
    * @tc.name       Test distributedAccount.getDistributedAccountAbility.
    * @tc.desc      Test distributedAccount.getDistributedAccountAbility API functionality.
    */
    it('account_getDistributedAccountAbility_test', 0, function () {
        var ret = false;
        const accountAbility = account.getDistributedAccountAbility()
        if(accountAbility !== null){
            ret = true;
        }
        expect(ret).assertTrue()
    })

    /**
    * @tc.number     SUB_Account_distributedAccount_JS_API_0200
    * @tc.name       Test distributedAccount.queryOsAccountDistributedInfo.
    * @tc.desc       Test distributedAccount.queryOsAccountDistributedInfo API functionality.
    */
    it('account_queryOsAccountDistributedInfo_test001', 0, function () {
        const accountAbility = account.getDistributedAccountAbility()
        accountAbility.queryOsAccountDistributedInfo().then(function (data) {
            expect(data.name).assertEqual('anonymous')
        });
    })

    /**
    * @tc.number     SUB_Account_distributedAccount_JS_API_0300
    * @tc.name       Test distributedAccount.queryOsAccountDistributedInfo by callback.
    * @tc.desc       Test distributedAccount.queryOsAccountDistributedInfo API functionality by callback.
    */
    it('account_queryOsAccountDistributedInfo_test002', 0, function () {
        const accountAbility = account.getDistributedAccountAbility()
        accountAbility.queryOsAccountDistributedInfo(function (data) {
            expect(data.name).assertEqual('anonymous')
        });
    })

    /**
    * @tc.number     SUB_Account_distributedAccount_JS_API_0400
    * @tc.name       Test distributedAccount.updateOsAccountDistributedInfo.
    * @tc.desc       Test distributedAccount.updateOsAccountDistributedInfo API functionality.
    */
    it('account_updateOsAccountDistributedInfo_test001', 0, function () {
        const accountAbility = account.getDistributedAccountAbility()
	let data = null
        let obj = {
            id: '12345',
            name: 'ZhangSan',
            event: 'Ohos.account.event.LOGIN',
            scalableData:data
        };
        accountAbility.updateOsAccountDistributedInfo(obj).then(function (result) {
            expect(result).assertTrue()
            accountAbility.queryOsAccountDistributedInfo(function (data) {
                expect(data.name).assertEqual('ZhangSan')
                expect(data.id).assertEqual('12345')
                let obj = {
                    id: '12345',
                    name: 'ZhangSan',
                    event: 'Ohos.account.event.LOGOUT'
                };
                accountAbility.updateOsAccountDistributedInfo(obj).then(function (result) {
                    expect(result).assertTrue()
                });
            });
        });
        accountAbility.queryOsAccountDistributedInfo(function (data) {
            execpt(data).assertNull()
        })
    })

    /**
    * @tc.number     SUB_Account_distributedAccount_JS_API_0500
    * @tc.name       Test distributedAccount.updateOsAccountDistributedInfo by callback.
    * @tc.desc       Test distributedAccount.updateOsAccountDistributedInfo API functionality by callback.
    */
    it('account_updateOsAccountDistributedInfo_test002', 0, function () {
	let data = null
        const accountAbility = account.getDistributedAccountAbility()
        let obj = {
            id: '12345',
            name: 'ZhangSan',
            event: 'Ohos.account.event.LOGIN',
            scalableData:data
        };
        accountAbility.updateOsAccountDistributedInfo(obj, function (result) {
            expect(result).assertTrue()
            accountAbility.queryOsAccountDistributedInfo(function (data) {
                expect(data.name).assertEqual('ZhangSan')
                expect(data.id).assertEqual('12345')
                const accountAbility = account.getDistributedAccountAbility()
                let obj = {
                    id: '12345',
                    name: 'ZhangSan',
                    event: 'Ohos.account.event.LOGOUT'
                    
                };
                accountAbility.updateOsAccountDistributedInfo(obj).then(function (result) {
                    expect(result).assertTrue()
                });
            });
        });
    })
    
    /**
    * @tc.number     SUB_Account_distributedAccount_JS_API_0300
    * @tc.name       Test distributedAccount.updateOsAccountDistributedInfo by callback.
    * @tc.desc       Test distributedAccount.updateOsAccountDistributedInfo API functionality by callback.
    */
    it('account_updateOsAccountDistributedInfo_test003', 0, function () {
        const accountAbility = account.getDistributedAccountAbility()
	let data = null
        let obj = {
            id: '12345',
            name: 'ZhangSan',
            event: 'Ohos.account.event.TOKEN_INVALID',
            scalableData:data
        };
        accountAbility.updateOsAccountDistributedInfo(obj, function (result) {
            expect(result).assertTrue()
            accountAbility.queryOsAccountDistributedInfo(obj).then(function (data) {
                except(data.name).assertNull()
            })
        });
    })
    
    /**
    * @tc.number     SUB_Account_distributedAccount_JS_API_0500
    * @tc.name       Test distributedAccount.updateOsAccountDistributedInfo by callback.
    * @tc.desc       Test distributedAccount.updateOsAccountDistributedInfo API functionality by callback.
    */
    it('account_updateOsAccountDistributedInfo_test004', 0, function () {
        const accountAbility = account.getDistributedAccountAbility()
	let data = null
        let obj = {
            id: '12345',
            name: 'ZhangSan',
            event: 'Ohos.account.event.LOGIN',
            scalableData:data
        };
        accountAbility.updateOsAccountDistributedInfo(obj, function (result) {
            expect(result).assertTrue()
            accountAbility.queryOsAccountDistributedInfo(function (data) {
                expect(data.name).assertEqual('ZhangSan')
                expect(data.id).assertEqual('12345')
                const accountAbility = account.getDistributedAccountAbility()
                let obj = {
                    id: '12345',
                    name: 'ZhangSan',
                    event: 'Ohos.account.event.LOGOFF'
                    
                };
                accountAbility.updateOsAccountDistributedInfo(obj).then(function (result) {
                    expect(result).assertNull()
                });
            });
        });
    })
})
