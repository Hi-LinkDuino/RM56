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
import featureAbility from '@ohos.ability.featureability'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

const TIMEOUT = 2000;
describe('ActsGetAllAccessibleAccounts', function () {

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_0100
     * @tc.name      : getAllAccessibleAccounts callback
     * @tc.desc      : This application has not added an account and has not been authorized by other applications to
     *                 obtain authorization
     */
    it('ActsGetAllAccessibleAccounts_0100', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_0100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.getAllAccessibleAccounts((err, data)=>{
            console.debug("====>getAllAccessibleAccounts 0100 err:" + JSON.stringify(err));
            console.debug("====>getAllAccessibleAccounts 0100 data:" + JSON.stringify(data));
            console.debug("====>getAllAccessibleAccounts 0100 data.length:" + data.length);
            expect(err.code).assertEqual(0);
            expect(data.length).assertEqual(0);
            console.debug("====>ActsGetAllAccessibleAccounts_0100 end====");
            done();
        });
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_0200
     * @tc.name      : getAllAccessibleAccounts promise
     * @tc.desc      : This application has not added an account and has not been authorized by other applications to
     *                 obtain authorization
     */
    it('ActsGetAllAccessibleAccounts_0200', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_0200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>getAllAccessibleAccounts 0300 start====");
        try{
            var data = await appAccountManager.getAllAccessibleAccounts();
        }
        catch(err){
            console.error("====>getAllAccessibleAccounts 0200 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAllAccessibleAccounts 0200 data:" + JSON.stringify(data));
        console.debug("====>getAllAccessibleAccounts 0200 data.length:" + data.length);
        expect(data.length).assertEqual(0);
        console.debug("====>ActsGetAllAccessibleAccounts_0200 end====");
        done();
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_0300
     * @tc.name      : getAllAccessibleAccounts callback
     * @tc.desc      : This application gets authorization after adding a single account
     */
    it('ActsGetAllAccessibleAccounts_0300', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_0300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("accessibleAccount_callback_single", (err)=>{
            console.debug("====> add account ActsGetAllAccessibleAccounts_0300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.getAllAccessibleAccounts((err, data)=>{
                console.debug("====>getAllAccessibleAccounts 0300 err:" + JSON.stringify(err));
                console.debug("====>getAllAccessibleAccounts 0300 data:" + JSON.stringify(data));
                console.debug("====>getAllAccessibleAccounts 0300 data.length:" + data.length);
                expect(err.code).assertEqual(0);
                expect(data.length).assertEqual(1);
                try{
                    expect(data[0].name).assertEqual("accessibleAccount_callback_single");
                    expect(data[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                }
                catch(err){
                    console.error("====>getAllAccessibleAccounts check data 0300 fail err:" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
                appAccountManager.deleteAccount("accessibleAccount_callback_single", (err)=>{
                    console.debug("====>delete Account ActsGetAllAccessibleAccounts_0300 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsGetAllAccessibleAccounts_0300 end====");
                    done();
                })
            });
        });
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_0400
     * @tc.name      : getAllAccessibleAccounts promise
     * @tc.desc      : This application gets authorization after adding a single account
     */
    it('ActsGetAllAccessibleAccounts_0400', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_0400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsGetAllAccessibleAccounts_0400 start====");
        await appAccountManager.addAccount("accessibleAccount_promise_single");
        console.debug("====>getAllAccessibleAccounts 0400 start====");
        try{
            var data = await appAccountManager.getAllAccessibleAccounts();
        }
        catch(err){
            console.error("====>getAllAccessibleAccounts 0400 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAllAccessibleAccounts 0400 data:" + JSON.stringify(data));
        console.debug("====>getAllAccessibleAccounts 0400 data.length:" + data.length);
        try{
            expect(data[0].name).assertEqual("accessibleAccount_promise_single");
            expect(data[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
        }
        catch(err){
            console.error("====>getAllAccessibleAccounts check data 0400 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        await appAccountManager.deleteAccount("accessibleAccount_promise_single");
        console.debug("====>ActsGetAllAccessibleAccounts_0400 end====");
        done();
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_0500
     * @tc.name      : getAllAccessibleAccounts callback
     * @tc.desc      : This application is added to obtain authorization after adding multiple application accounts
     */
    it('ActsGetAllAccessibleAccounts_0500', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_0500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("accessibleAccount_callback_first", (err)=>{
            console.debug("====>add account first time 0500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.addAccount("accessibleAccount_callback_second", (err)=>{
                console.debug("====>add account second time 0500 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.addAccount("accessibleAccount_callback_third", (err)=>{
                    console.debug("====>add account third time 0500 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    appAccountManager.getAllAccessibleAccounts((err, data)=>{
                        console.debug("====>getAllAccessibleAccounts 0500 err:" + JSON.stringify(err));
                        console.debug("====>getAllAccessibleAccounts 0500 data:" + JSON.stringify(data));
                        console.debug("====>getAllAccessibleAccounts 0500 data.length:" + data.length);
                        expect(err.code).assertEqual(0);
                        expect(data.length).assertEqual(3);
                        try{
                            expect(data[0].name).assertEqual("accessibleAccount_callback_first");
                            expect(data[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                            expect(data[1].name).assertEqual("accessibleAccount_callback_second");
                            expect(data[1].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                            expect(data[2].name).assertEqual("accessibleAccount_callback_third");
                            expect(data[2].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                        }
                        catch(err){
                            console.error("====>check data 0500 fail err:" + JSON.stringify(err));
                            expect().assertFail();
                            done();
                        }
                        appAccountManager.deleteAccount("accessibleAccount_callback_first", (err)=>{
                            console.debug("====>delete Account first 0500 err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            appAccountManager.deleteAccount("accessibleAccount_callback_second", (err)=>{
                                console.debug("====>delete Account second 0500 err:" + JSON.stringify(err));
                                expect(err.code).assertEqual(0);
                                appAccountManager.deleteAccount("accessibleAccount_callback_third", (err)=>{
                                    console.debug("====>delete Account third 0500 err:" + JSON.stringify(err));
                                    expect(err.code).assertEqual(0);
                                    console.debug("====>ActsGetAllAccessibleAccounts_0500 end====");
                                    done();
                                })
                            })
                        })
                    });
                })
            })
        });
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_0600
     * @tc.name      : getAllAccessibleAccounts promise
     * @tc.desc      : This application is added to obtain authorization after adding multiple application accounts
     */
    it('ActsGetAllAccessibleAccounts_0600', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_0600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add first account 0600 start====");
        await appAccountManager.addAccount("accessibleAccount_promise_first");
        console.debug("====>add second account 0600 start====");
        await appAccountManager.addAccount("accessibleAccount_promise_second");
        console.debug("====>add third account 0600 start====");
        await appAccountManager.addAccount("accessibleAccount_promise_third");
        try{
            var data = await appAccountManager.getAllAccessibleAccounts();
        }
        catch(err){
            console.error("====>getAllAccessibleAccounts 0600 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAllAccessibleAccounts 0600 data:" + JSON.stringify(data));
        expect(data.length).assertEqual(3);
        try{
            expect(data[0].name).assertEqual("accessibleAccount_promise_first");
            expect(data[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
            expect(data[1].name).assertEqual("accessibleAccount_promise_second");
            expect(data[1].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
            expect(data[2].name).assertEqual("accessibleAccount_promise_third");
            expect(data[2].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
        }
        catch(err){
            console.error("====>check data 0600 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>delete first account 0600 start====");
        await appAccountManager.deleteAccount("accessibleAccount_promise_first");
        console.debug("====>delete second account 0600 start====");
        await appAccountManager.deleteAccount("accessibleAccount_promise_second");
        console.debug("====>delete third account 0600 start====");
        await appAccountManager.deleteAccount("accessibleAccount_promise_third");
        console.debug("====>ActsGetAllAccessibleAccounts_0600 end====");
        done();
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_0700
     * @tc.name      : getAllAccessibleAccounts callback
     * @tc.desc      : This application adds different application accounts three times in a row, and obtains
     *                 authorization after deleting the account added for the second time
     */
    it('ActsGetAllAccessibleAccounts_0700', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_0700 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("accessibleAccount_callback_delete_first", (err)=>{
            console.debug("====>add first account 0700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.addAccount("accessibleAccount_callback_delete_second", (err)=>{
                console.debug("====>add second account 0700 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.addAccount("accessibleAccount_callback_delete_third", (err)=>{
                    console.debug("====>add third account 0700 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    appAccountManager.deleteAccount("accessibleAccount_callback_delete_second", (err)=>{
                        console.debug("====>delete second account 0700 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        appAccountManager.getAllAccessibleAccounts((err, data)=>{
                            console.debug("====>getAllAccessibleAccounts 0700 err:" + JSON.stringify(err));
                            console.debug("====>getAllAccessibleAccounts 0700 data:" + JSON.stringify(data));
                            console.debug("====>getAllAccessibleAccounts 0700 data.length:" + data.length);
                            expect(err.code).assertEqual(0);
                            expect(data.length).assertEqual(2);
                            try{
                                expect(data[0].name).assertEqual("accessibleAccount_callback_delete_first");
                                expect(data[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                                expect(data[1].name).assertEqual("accessibleAccount_callback_delete_third");
                                expect(data[1].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                            }
                            catch(err){
                                console.error("====>check data 0700 fail err:" + JSON.stringify(err));
                                expect().assertFail();
                                done();
                            }
                            appAccountManager.deleteAccount("accessibleAccount_callback_delete_first", (err)=>{
                                console.debug("====>delete first account 0700 err:" + JSON.stringify(err));
                                expect(err.code).assertEqual(0);
                                appAccountManager.deleteAccount("accessibleAccount_callback_delete_third", (err)=>{
                                    console.debug("====>delete third account 0700 err:" + JSON.stringify(err));
                                    expect(err.code).assertEqual(0);
                                    console.debug("====>ActsGetAllAccessibleAccounts_0700 end====");
                                    done();
                                })
                            })
                        });
                    })
                })
            })
        });
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_0800
     * @tc.name      : getAllAccessibleAccounts promise
     * @tc.desc      : This application adds different application accounts three times in a row, and obtains
     *                 authorization after deleting the account added for the second time
     */
    it('ActsGetAllAccessibleAccounts_0800', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_0800 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add first account 0800 start====");
        await appAccountManager.addAccount("accessibleAccount_promise_first");
        console.debug("====>add second account 0800 start====");
        await appAccountManager.addAccount("accessibleAccount_promise_second");
        console.debug("====>add third account 0800 start====");
        await appAccountManager.addAccount("accessibleAccount_promise_third");
        console.debug("====>delete second account 0800 start====");
        await appAccountManager.deleteAccount("accessibleAccount_promise_second");
        try{
            var data = await appAccountManager.getAllAccessibleAccounts();
        }
        catch(err){
            console.error("====>getAllAccessibleAccounts 0800 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAllAccessibleAccounts 0800 data:" + JSON.stringify(data));
        expect(data.length).assertEqual(2);
        try{
            expect(data[0].name).assertEqual("accessibleAccount_promise_first");
            expect(data[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
            expect(data[1].name).assertEqual("accessibleAccount_promise_third");
            expect(data[1].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
        }
        catch(err){
            console.error("====>check data 0800 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>delete first account 0800 start====");
        await appAccountManager.deleteAccount("accessibleAccount_promise_first");
        console.debug("====>delete third account 0800 start====");
        await appAccountManager.deleteAccount("accessibleAccount_promise_third");
        console.debug("====>ActsGetAllAccessibleAccounts_0800 end====");
        done();
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_0900
     * @tc.name      : getAllAccessibleAccounts callback
     * @tc.desc      : This application adds different application accounts three times in a row, and then obtains
     *                 authorization multiple times
     */
    it('ActsGetAllAccessibleAccounts_0900', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_0900 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("accessibleAccount_callback_multiple_first", (err)=>{
            console.debug("====>add first account 0900 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.addAccount("accessibleAccount_callback_multiple_second", (err)=>{
                console.debug("====>add third account 0900 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.getAllAccessibleAccounts((err, data)=>{
                    console.debug("====>getAllAccessibleAccounts first 0900 err:" + JSON.stringify(err));
                    console.debug("====>getAllAccessibleAccounts first 0900 data:" + JSON.stringify(data));
                    console.debug("====>getAllAccessibleAccounts first 0900 data.length:" + data.length);
                    expect(err.code).assertEqual(0);
                    expect(data.length).assertEqual(2);
                    try{
                        expect(data[0].name).assertEqual("accessibleAccount_callback_multiple_first");
                        expect(data[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                        expect(data[1].name).assertEqual("accessibleAccount_callback_multiple_second");
                        expect(data[1].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                    }
                    catch(err){
                        console.error("====>check data first 0900 fail err:" + JSON.stringify(err));
                        expect().assertFail();
                        done();
                    }
                    appAccountManager.getAllAccessibleAccounts((err, data)=>{
                        console.debug("====>getAllAccessibleAccounts second 0900 err:" + JSON.stringify(err));
                        console.debug("====>getAllAccessibleAccounts second 0900 data:" + JSON.stringify(data));
                        console.debug("====>getAllAccessibleAccounts second 0900 data.length:" + data.length);
                        expect(err.code).assertEqual(0);
                        expect(data.length).assertEqual(2);
                        try{
                            expect(data[0].name).assertEqual("accessibleAccount_callback_multiple_first");
                            expect(data[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                            expect(data[1].name).assertEqual("accessibleAccount_callback_multiple_second");
                            expect(data[1].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
                        }
                        catch(err){
                            console.error("====>check data second 0900 fail err:" + JSON.stringify(err));
                            expect().assertFail();
                            done();
                        }
                        appAccountManager.deleteAccount("accessibleAccount_callback_multiple_first", (err)=>{
                            console.debug("====>delete first account 0900 err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            appAccountManager.deleteAccount("accessibleAccount_callback_multiple_second", (err)=>{
                                console.debug("====>delete second account 0900 err:" + JSON.stringify(err));
                                expect(err.code).assertEqual(0);
                                console.debug("====>ActsGetAllAccessibleAccounts_0900 end====");
                                done();
                            })
                        })
                    })
                })
            })
        });
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_1000
     * @tc.name      : getAllAccessibleAccounts promise
     * @tc.desc      : This application adds different application accounts three times in a row, and then obtains
     *                 authorization multiple times
     */
    it('ActsGetAllAccessibleAccounts_1000', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_1000 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add first account 1000 start====");
        await appAccountManager.addAccount("accessibleAccount_promise_multiple_first");
        console.debug("====>add second account 1000 start====");
        await appAccountManager.addAccount("accessibleAccount_promise_multiple_second");
        console.debug("====>getAllAccessibleAccounts first 1000 start====");
        try{
            var data = await appAccountManager.getAllAccessibleAccounts();
        }
        catch(err){
            console.error("====>getAllAccessibleAccounts first 1000 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAllAccessibleAccounts 1000 data:" + JSON.stringify(data));
        expect(data.length).assertEqual(2);
        try{
            expect(data[0].name).assertEqual("accessibleAccount_promise_multiple_first");
            expect(data[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
            expect(data[1].name).assertEqual("accessibleAccount_promise_multiple_second");
            expect(data[1].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
        }
        catch(err){
            console.error("====>check data 1000 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAllAccessibleAccounts second 1000 start====");
        try{
            var dataTwo = await appAccountManager.getAllAccessibleAccounts();
        }
        catch(err){
            console.error("====>getAllAccessibleAccounts second 1000 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAllAccessibleAccounts 1000 dataTwo:" + JSON.stringify(dataTwo));
        expect(data.length).assertEqual(2);
        try{
            expect(dataTwo[0].name).assertEqual("accessibleAccount_promise_multiple_first");
            expect(dataTwo[0].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
            expect(dataTwo[1].name).assertEqual("accessibleAccount_promise_multiple_second");
            expect(dataTwo[1].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
        }
        catch(err){
            console.error("====>check dataTwo 1000 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>delete first account 1000 start====");
        await appAccountManager.deleteAccount("accessibleAccount_promise_multiple_first");
        console.debug("====>delete second account 1000 start====");
        await appAccountManager.deleteAccount("accessibleAccount_promise_multiple_second");
        console.debug("====>ActsGetAllAccessibleAccounts_1000 end====");
        done();
    });

    /*
     * @tc.number    : ActsGetAllAccessibleAccounts_1100
     * @tc.name      : getAllAccessibleAccounts
     * @tc.desc      : This application adds an account, other application authorizes an account to this application
     *                 and this application obtains authorization
     */
    it('ActsGetAllAccessibleAccounts_1100', 0, async function (done) {
        console.debug("====>ActsGetAllAccessibleAccounts_1100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account 1100 start====");
        await appAccountManager.addAccount("accessibleAccount_this_application");
        console.debug("====>startAbility 1100 start====");
        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsaccountsceneaccessible",
                    abilityName: "com.example.actsaccountsceneaccessible.MainAbility",
                    action: "action1",
                    parameters:
                    {},
                },
            },
        );
        setTimeout(async function(){
            console.debug("====>getAllAccessibleAccounts 1100 start====");
            try{
                var data = await appAccountManager.getAllAccessibleAccounts();
            }
            catch(err){
                console.error("====>getAllAccessibleAccounts 1100 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>getAllAccessibleAccounts 1100 data:" + JSON.stringify(data));
            expect(data.length).assertEqual(2);
            try{
                expect(data[0].name).assertEqual("account_name_scene_single");
                expect(data[0].owner).assertEqual("com.example.actsaccountsceneaccessible");
                expect(data[1].name).assertEqual("accessibleAccount_this_application");
                expect(data[1].owner).assertEqual("com.example.actsgetallaccessibleaccounts");
            }
            catch(err){
                console.error("====>check data 1100 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>delete account 1100 start====");
            await appAccountManager.deleteAccount("accessibleAccount_this_application");
            console.debug("====>ActsGetAllAccessibleAccounts_1100 end====");
            done();
        }, TIMEOUT);
    });
})