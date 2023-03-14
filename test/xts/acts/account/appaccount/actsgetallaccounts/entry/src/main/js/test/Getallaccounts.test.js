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
const LENGTHLIMIT = 1024;
describe('ActsGetAllAccounts', function () {

    /*
     * @tc.number    : ActsGetAllAccounts_0100
     * @tc.name      : getAllAccounts callback
     * @tc.desc      : This application gets its own application information after adding an account
     */
    it('ActsGetAllAccounts_0100', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        appAccountManager.addAccount("Account_this_application_callback", (err)=>{
            console.debug("====>add account 0100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.getAllAccounts(selfBundle, (err, data)=>{
                console.debug("====>getAllAccounts 0100 err:" + JSON.stringify(err));
                console.debug("====>getAllAccounts 0100 data:" + JSON.stringify(data));
                expect(err.code).assertEqual(0);
                try{
                    expect(data[0].name).assertEqual("Account_this_application_callback");
                    expect(data[0].owner).assertEqual("com.example.actsgetallaaccounts");
                }
                catch(err){
                    console.error("====>getAllAccounts 0100 fail err:" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
                appAccountManager.deleteAccount("Account_this_application_callback", (err)=>{
                    console.debug("====>delete account 0100 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsGetAllAccounts_0100 end====");
                    done();
                });
            })
        });
    });

    /*
     * @tc.number    : ActsGetAllAccounts_0200
     * @tc.name      : getAllAccounts promise
     * @tc.desc      : This application gets its own application information after adding an account
     */
    it('ActsGetAllAccounts_0200', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        console.debug("====>add account 0200 start====");
        await appAccountManager.addAccount("Account_this_application_promise");
        console.debug("====>getAllAccounts 0200 start====");
        try{
            var data = await appAccountManager.getAllAccounts(selfBundle);
        }
        catch(err){
            console.error("====>getAllAccounts 0200 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }       
        try{
            expect(data[0].name).assertEqual("Account_this_application_promise");
            expect(data[0].owner).assertEqual("com.example.actsgetallaaccounts");
        }
        catch(err){
            console.error("====>check data 0200 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>delete account 0200 start====");
        await appAccountManager.deleteAccount("Account_this_application_promise");      
        console.debug("====>ActsGetAllAccounts_0200 end====");
        done();
    });

    /*
     * @tc.number    : ActsGetAllAccounts_0300
     * @tc.name      : getAllAccounts callback
     * @tc.desc      : No account has been added to this application and it is not authorized by other applications to
     *                 obtain account information for this application
     */
    it('ActsGetAllAccounts_0300', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        appAccountManager.getAllAccounts(selfBundle, (err, data)=>{
            console.debug("====>getAllAccounts 0300 err:" + JSON.stringify(err));
            console.debug("====>getAllAccounts 0300 data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.length).assertEqual(0);
            console.debug("====>ActsGetAllAccounts_0300 end====");
            done();
        })
    });

    /*
     * @tc.number    : ActsGetAllAccounts_0400
     * @tc.name      : getAllAccounts promise
     * @tc.desc      : No account has been added to this application and it is not authorized by other applications to
     *                 obtain account information for this application
     */
    it('ActsGetAllAccounts_0400', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        var data = await appAccountManager.getAllAccounts(selfBundle);
        console.debug("====>getAllAccounts 0400 data:" + JSON.stringify(data));   
        expect(data.length).assertEqual(0);
        console.debug("====>ActsGetAllAccounts_0400 end====");
        done();
    });

    /*
     * @tc.number    : ActsGetAllAccounts_0500
     * @tc.name      : getAllAccounts callback
     * @tc.desc      : The parameter owner is an empty string
     */
    it('ActsGetAllAccounts_0500', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var emptyBundle = "";
        appAccountManager.getAllAccounts(emptyBundle, (err, data)=>{
            console.debug("====>getAllAccounts 0500 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsGetAllAccounts_0500 end====");
            done();
        })
    });

    /*
     * @tc.number    : ActsGetAllAccounts_0600
     * @tc.name      : getAllAccounts promise
     * @tc.desc      : The parameter owner is an empty string
     */
    it('ActsGetAllAccounts_0600', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var emptyBundle = "";
        try{
            var data = await appAccountManager.getAllAccounts(emptyBundle);
        }
        catch(err){
            console.debug("====>getAllAccounts 0600 err:" + JSON.stringify(err));   
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsGetAllAccounts_0600 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsGetAllAccounts_0700
     * @tc.name      : getAllAccounts callback
     * @tc.desc      : The parameter owner is a string that exceeds the length limit
     */
    it('ActsGetAllAccounts_0700', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0700 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var limitBundle = '';
        for (var i = 0; i < LENGTHLIMIT + 1; i++) {
            limitBundle += 't';
        }
        appAccountManager.getAllAccounts(limitBundle, (err, data)=>{
            console.debug("====>getAllAccounts 0700 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsGetAllAccounts_0700 end====");
            done();
        })
    });

    /*
     * @tc.number    : ActsGetAllAccounts_0800
     * @tc.name      : getAllAccounts promise
     * @tc.desc      : The parameter owner is a string that exceeds the length limit
     */
    it('ActsGetAllAccounts_0800', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0800 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var limitBundle = '';
        for (var i = 0; i < LENGTHLIMIT + 1; i++) {
            limitBundle += 'n';
        }
        try{
            var data = await appAccountManager.getAllAccounts(limitBundle);
        }
        catch(err){
            console.debug("====>getAllAccounts 0800 err:" + JSON.stringify(err));   
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsGetAllAccounts_0800 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsGetAllAccounts_0900
     * @tc.name      : getAllAccounts callback
     * @tc.desc      : This application adds an account, and after other applications authorize an account to this
     *                 application, this application obtains the information of its own application
     */
    it('ActsGetAllAccounts_0900', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0900 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        console.debug("====>add account 0900 start====");
        await appAccountManager.addAccount("Account_application_callback");
        console.debug("====>startAbility 0900 start====");
        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsscenegetallaccounts",
                    abilityName: "com.example.actsscenegetallaccounts.MainAbility",
                    action: "action1",
                    parameters:
                    {},
                },
            },
        );
        function getAllCallback(err, data){
            console.debug("====>getAllAccounts 0900 err:" + JSON.stringify(err));
            console.debug("====>getAllAccounts 0900 data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data[0].name).assertEqual("Account_application_callback");
            expect(data[0].owner).assertEqual("com.example.actsgetallaaccounts");
            appAccountManager.deleteAccount("Account_application_callback", (err)=>{
                console.debug("====>delete account 0900 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>ActsGetAllAccounts_0900 end====");
                done();
            });
        }
        setTimeout(async function(){
            console.debug("====>getAllAccounts 0900 start====");
            appAccountManager.getAllAccounts(selfBundle, getAllCallback); 
        }, TIMEOUT); 
    });

    /*
     * @tc.number    : ActsGetAllAccounts_1000
     * @tc.name      : getAllAccounts promise
     * @tc.desc      : This application adds an account, and after other applications authorize an account to this
     *                 application, this application obtains the information of its own application
     */
    it('ActsGetAllAccounts_1000', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_1000 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        console.debug("====>add account 1000 start====");
        await appAccountManager.addAccount("Account_application_promise");
        console.debug("====>startAbility 1000 start====");
        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsscenegetallaccounts",
                    abilityName: "com.example.actsscenegetallaccounts.MainAbility",
                    action: "action1",
                    parameters:
                    {},
                },
            },
        );
        setTimeout(async function(){
            console.debug("====>getAllAccounts 1000 start====");
            try{
                var data = await appAccountManager.getAllAccounts(selfBundle);
            }
            catch(err){
                console.error("====>getAllAccounts 1000 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>getAllAccounts 1000 data:" + JSON.stringify(data));
            try{
                expect(data[0].name).assertEqual("Account_application_promise");
                expect(data[0].owner).assertEqual("com.example.actsgetallaaccounts");
            }
            catch(err){
                console.error("====>check data 1000 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>delete account 1000 start====");
            await appAccountManager.deleteAccount("Account_application_promise");
            console.debug("====>ActsGetAllAccounts_1000 end====");
            done();
        }, TIMEOUT); 
    });

    /*
     * @tc.number    : ActsGetAllAccounts_1100
     * @tc.name      : getAllAccounts callback
     * @tc.desc      : This application adds an account, and after other applications authorize an account to this
     *                 application, this application obtains the information of its own application
     */
    it('ActsGetAllAccounts_1100', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_0900 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var specifiedBundle = "com.example.actsscenegetallaccounts";
        console.debug("====>startAbility 1100 start====");
        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsscenegetallaccounts",
                    abilityName: "com.example.actsscenegetallaccounts.MainAbility",
                    action: "action1",
                    parameters:
                    {},
                },
            },
        );
        function getAllCallback(err, data){
            console.debug("====>getAllAccounts 1100 err:" + JSON.stringify(err));
            console.debug("====>getAllAccounts 1100 data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data[0].name).assertEqual("account_name_scene_single");
            expect(data[0].owner).assertEqual("com.example.actsscenegetallaccounts");
            console.debug("====>ActsGetAllAccounts_1100 end====");
            done();
        }
        setTimeout(async function(){
            console.debug("====>getAllAccounts 1100 start====");
            appAccountManager.getAllAccounts(specifiedBundle, getAllCallback); 
        }, TIMEOUT); 
    });

    /*
     * @tc.number    : ActsGetAllAccounts_1200
     * @tc.name      : getAllAccounts promise
     * @tc.desc      : This application adds an account, and after other applications authorize an account to this
     *                 application, this application obtains the information of its own application
     */
    it('ActsGetAllAccounts_1200', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_1200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var specifiedBundle = "com.example.actsscenegetallaccounts";
        console.debug("====>startAbility 1200 start====");
        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsscenegetallaccounts",
                    abilityName: "com.example.actsscenegetallaccounts.MainAbility",
                    action: "action1",
                    parameters:
                    {},
                },
            },
        );
        setTimeout(async function(){
            console.debug("====>getAllAccounts 1200 start====");
            try{
                var data = await appAccountManager.getAllAccounts(specifiedBundle);
            }
            catch(err){
                console.error("====>getAllAccounts 1200 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>getAllAccounts 1200 data:" + JSON.stringify(data));
            expect(data[0].name).assertEqual("account_name_scene_single");
            expect(data[0].owner).assertEqual("com.example.actsscenegetallaccounts");
            console.debug("====>ActsGetAllAccounts_1200 end====");
            done();
        }, TIMEOUT);
    });

    /*
     * @tc.number    : ActsGetAllAccounts_1300
     * @tc.name      : getAllAccounts callback
     * @tc.desc      : The parameter owner is a bundlename that does not exist
     */
    it('ActsGetAllAccounts_1300', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_1300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var nonexistentBundle = "com.example.actsgetallaccountsnonexistent";
        appAccountManager.getAllAccounts(nonexistentBundle, (err, data)=>{
            console.debug("====>getAllAccounts 1300 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsGetAllAccounts_1300 end====");
            done();
        })
    });

    /*
     * @tc.number    : ActsGetAllAccounts_1400
     * @tc.name      : getAllAccounts promise
     * @tc.desc      : The parameter owner is a bundlename that does not exist
     */
    it('ActsGetAllAccounts_1400', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_1400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var nonexistentBundle = "com.example.actsgetallaccountsnonexistent";
        try{
            var data = await appAccountManager.getAllAccounts(nonexistentBundle);
        }
        catch(err){
            console.debug("====>getAllAccounts 1400 err:" + JSON.stringify(err));   
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsGetAllAccounts_1400 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsGetAllAccounts_1500
     * @tc.name      : getAllAccounts callback
     * @tc.desc      : Get account information after adding and deleting account
     */
    it('ActsGetAllAccounts_1500', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_1500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        appAccountManager.addAccount("account_callback_delete", (err)=>{
            console.debug("====>add account 1500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.getAllAccounts(selfBundle, (err, data)=>{
                console.debug("====>getAllAccounts 1500 err:" + JSON.stringify(err));
                console.debug("====>getAllAccounts 1500 data:" + JSON.stringify(data));
                expect(err.code).assertEqual(0);
                expect(data[0].name).assertEqual("account_callback_delete");
                expect(data[0].owner).assertEqual("com.example.actsgetallaaccounts");
                appAccountManager.deleteAccount("account_callback_delete", (err)=>{
                    console.debug("====>delete account 1500 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    appAccountManager.getAllAccounts(selfBundle, (err, data)=>{
                        console.debug("====>getAllAccounts 1500 err:" + JSON.stringify(err));
                        console.debug("====>getAllAccounts 1500 data:" + JSON.stringify(data));
                        expect(err.code).assertEqual(0);
                        expect(data.length).assertEqual(0);
                        console.debug("====>ActsGetAllAccounts_1500 end====");
                        done();
                    })
                });
            })
        });
    });

    /*
     * @tc.number    : ActsGetAllAccounts_1600
     * @tc.name      : getAllAccounts promise
     * @tc.desc      : Get account information after adding and deleting account
     */
    it('ActsGetAllAccounts_1600', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_1600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        console.debug("====>add account 1600 start====");
        await appAccountManager.addAccount("account_promise_delete");
        console.debug("====>first getAllAccounts 1600 start====");
        try{
            var data = await appAccountManager.getAllAccounts(selfBundle);
        }
        catch(err){
            console.error("====>getAllAccounts 1600 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }       
        expect(data[0].name).assertEqual("account_promise_delete");
        expect(data[0].owner).assertEqual("com.example.actsgetallaaccounts");
        console.debug("====>delete account 1600 start====");
        await appAccountManager.deleteAccount("account_promise_delete");  
        console.debug("====>second getAllAccounts 1600 start====");
        try{
            var dataDelete = await appAccountManager.getAllAccounts(selfBundle);
        }
        catch(err){
            console.error("====>second getAllAccounts 1600 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }       
        expect(dataDelete.length).assertEqual(0);
        console.debug("====>ActsGetAllAccounts_1600 end====");
        done();
    });

    /*
     * @tc.number    : ActsGetAllAccounts_1700
     * @tc.name      : getAllAccounts callback
     * @tc.desc      : Get account information after adding an account with additional information and deleting an
     *                 account
     */
    it('ActsGetAllAccounts_1700', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_1700 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        appAccountManager.addAccount("account_callback_additional", "account_extrainfo", (err)=>{
            console.debug("====>add account 1700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.getAllAccounts(selfBundle, (err, data)=>{
                console.debug("====>getAllAccounts 1700 err:" + JSON.stringify(err));
                console.debug("====>getAllAccounts 1700 data:" + JSON.stringify(data));
                expect(err.code).assertEqual(0);
                expect(data[0].name).assertEqual("account_callback_additional");
                expect(data[0].owner).assertEqual("com.example.actsgetallaaccounts");
                appAccountManager.deleteAccount("account_callback_additional", (err)=>{
                    console.debug("====>delete account 1700 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    appAccountManager.getAllAccounts(selfBundle, (err, data)=>{
                        console.debug("====>getAllAccounts 1700 err:" + JSON.stringify(err));
                        console.debug("====>getAllAccounts 1700 data:" + JSON.stringify(data));
                        expect(err.code).assertEqual(0);
                        expect(data.length).assertEqual(0);
                        console.debug("====>ActsGetAllAccounts_1700 end====");
                        done();
                    })
                });
            })
        });
    });

    /*
     * @tc.number    : ActsGetAllAccounts_1800
     * @tc.name      : getAllAccounts promise
     * @tc.desc      : Get account information after adding an account with additional information and deleting an
     *                 account
     */
    it('ActsGetAllAccounts_1800', 0, async function (done) {
        console.debug("====>ActsGetAllAccounts_1800 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var selfBundle = "com.example.actsgetallaaccounts";
        console.debug("====>add account 1800 start====");
        await appAccountManager.addAccount("account_promise_additional", "account_extrainfo");
        console.debug("====>first getAllAccounts 1800 start====");
        try{
            var data = await appAccountManager.getAllAccounts(selfBundle);
        }
        catch(err){
            console.error("====>getAllAccounts 1800 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>first getAllAccounts 1800 data:" + JSON.stringify(data)); 
        expect(data[0].name).assertEqual("account_promise_additional");
        expect(data[0].owner).assertEqual("com.example.actsgetallaaccounts");
        console.debug("====>delete account 1800 start====");
        await appAccountManager.deleteAccount("account_promise_additional");  
        console.debug("====>second getAllAccounts 1800 start====");
        try{
            var dataDelete = await appAccountManager.getAllAccounts(selfBundle);
        }
        catch(err){
            console.error("====>second getAllAccounts 1800 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>second getAllAccounts 1800 data:" + JSON.stringify(data));
        expect(dataDelete.length).assertEqual(0);
        console.debug("====>ActsGetAllAccounts_1800 end====");
        done();
    });
})