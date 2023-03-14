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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

const TIMEOUT = 5000;
const STRCOUNT = 1025;
const EACHTIMEOUT = 500;
describe('ActsAccountAppAccess', function () {
    function sleep(delay) {
        var start = (new Date()).getTime();
        while((new Date()).getTime() - start < delay) {
            continue;
        }
    }

    beforeAll(async function (done) {
        console.debug("====>beforeAll start====");
        sleep(TIMEOUT);
        console.debug("====>beforeAll end====");
        done();
    })

    beforeEach(async function (done) {
        console.debug("====>beforeEach enter====");
        sleep(EACHTIMEOUT);
        done();
    })

    /*
     * @tc.number    : ActsAccountAppAccess_0100
     * @tc.name      : enableAppAccess callback
     * @tc.desc      : Enable the app to access the app itself
     */
    it('ActsAccountAppAccess_0100', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_0100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("AppAccess_callback_itself", "extrainfo_callback_itself", (err)=>{
            console.debug("====>add account ActsAccountAppAccess_0100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.enableAppAccess("AppAccess_callback_itself", "com.example.actsaccountappaccess", (err)=>{
                console.debug("====>enableAppAccess err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("AppAccess_callback_itself", (err)=>{
                    console.debug("====>delete Account ActsAccountAppAccess_0100 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAppAccess_0100 end====");
                    done();
                });
            })
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_0200
     * @tc.name      : enableAppAccess promise
     * @tc.desc      : Enable the app to access the app itself
     */
    it('ActsAccountAppAccess_0200', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_0200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountAppAccess_0200 start====");
        await appAccountManager.addAccount("AppAccess_promise_itself", "extrainfo_promise_itself");
        console.debug("====>enableAppAccess ActsAccountAppAccess_0200 start====");
        try{
            await appAccountManager.enableAppAccess("AppAccess_promise_itself", "com.example.actsaccountappaccess");
        }
        catch(err){
            console.debug("====>enableAppAccess 0200 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAppAccess_0200 start====");
            await appAccountManager.deleteAccount("AppAccess_promise_itself");
            console.debug("====>ActsAccountAppAccess_0200 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAppAccess_0300
     * @tc.name      : enableAppAccess callback
     * @tc.desc      : The enabled bundleName does not exist
     */
    it('ActsAccountAppAccess_0300', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_0300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var nonExistBundle = "com.example.NonExistentBundleCallback";
        appAccountManager.addAccount("AppAccess_callback_NotExistBundle", "extrainfo_callback_NotExistBundle", (err)=>{
            console.debug("====>add account ActsAccountAppAccess_0300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.enableAppAccess("AppAccess_callback_NotExistBundle", nonExistBundle, (err)=>{
                console.debug("====>enableAppAccess 0300 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("AppAccess_callback_NotExistBundle", (err)=>{
                    console.debug("====>delete Account ActsAccountAppAccess_0300 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAppAccess_0300 end====");
                    done();
                });
            })
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_0400
     * @tc.name      : enableAppAccess promise
     * @tc.desc      : The enabled bundleName does not exist
     */
    it('ActsAccountAppAccess_0400', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_0400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var nonExistBundle = "com.example.NonExistentBundlePromise";
        console.debug("====>add account ActsAccountAppAccess_0400 start====");
        await appAccountManager.addAccount("AppAccess_promise_NotExistBundle", "extrainfo_promise_NotExistBundle");
        console.debug("====>enableAppAccess ActsAccountAppAccess_0400 start====");
        try{
            await appAccountManager.enableAppAccess("AppAccess_promise_NotExistBundle", nonExistBundle);
        }
        catch(err){
            console.error("====>enableAppAccess ActsAccountAppAccess_0400 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAppAccess_0400 start====");
            await appAccountManager.deleteAccount("AppAccess_promise_NotExistBundle");
            console.debug("====>ActsAccountAppAccess_0400 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAppAccess_0500
     * @tc.name      : enableAppAccess callback
     * @tc.desc      : The enabled account name does not exist
     */
    it('ActsAccountAppAccess_0500', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_0500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var enableBundle = "com.example.actsaccountsceneappaccess";
        var nonExistAccount = "NonExistentAccountCallback";
        appAccountManager.enableAppAccess(nonExistAccount, enableBundle, (err)=>{
            console.debug("====>enableAppAccess 0500 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsAccountAppAccess_0500 end====");
            done();
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_0600
     * @tc.name      : enableAppAccess promise
     * @tc.desc      : The enabled account name does not exist
     */
    it('ActsAccountAppAccess_0600', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_0600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var enableBundle = "com.example.actsaccountsceneappaccess";
        var nonExistAccount = "NonExistentAccountPromise";
        console.debug("====>enableAppAccess ActsAccountAppAccess_0600 start====");
        try{
            await appAccountManager.enableAppAccess("NonExistentAccountPromise", enableBundle);
        }
        catch(err){
            console.debug("====>enableAppAccess 0600 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsAccountAppAccess_0600 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAppAccess_0700
     * @tc.name      : enableAppAccess disableAppAccess callback
     * @tc.desc      : Cancel the authorization after authorizing the account to another app
     */
    it('ActsAccountAppAccess_0700', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_0700 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var enableBundle = "com.example.actsaccountsceneappaccess";
        appAccountManager.addAccount("AppAccess_callback_account", (err)=>{
            console.debug("====>add account ActsAccountAppAccess_0700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.enableAppAccess("AppAccess_callback_account", enableBundle, (err)=>{
                console.debug("====>enableAppAccess 0700 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.disableAppAccess("AppAccess_callback_account", enableBundle, (err)=>{
                    expect(err.code).assertEqual(0)
                    appAccountManager.deleteAccount("AppAccess_callback_account", (err)=>{
                        console.debug("====>delete Account ActsAccountAppAccess_0700 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountAppAccess_0700 end====");
                        done();
                    });
                })
            })
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_0800
     * @tc.name      : enableAppAccess disableAppAccess promise
     * @tc.desc      : Cancel the authorization after authorizing the account to another account
     */
    it('ActsAccountAppAccess_0800', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_0800 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var enableBundle = "com.example.actsaccountsceneappaccess";
        console.debug("====>add account ActsAccountAppAccess_0800 start====");
        await appAccountManager.addAccount("AppAccess_promise_account");
        console.debug("====>enableAppAccess ActsAccountAppAccess_0800 start====");
        try{
            await appAccountManager.enableAppAccess("AppAccess_promise_account", enableBundle);
        }
        catch(err){
            console.error("====>enableAppAccess ActsAccountAppAccess_0800 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>disableAppAccess ActsAccountAppAccess_0800 start====");
        await appAccountManager.disableAppAccess("AppAccess_promise_account", enableBundle);
        console.debug("====>delete account ActsAccountAppAccess_0800 start====");
        await appAccountManager.deleteAccount("AppAccess_promise_account");
        console.debug("====>ActsAccountAppAccess_0800 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountAppAccess_0900
     * @tc.name      : enableAppAccess callback
     * @tc.desc      : The authorized account name is a long string
     */
    it('ActsAccountAppAccess_0900', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_0900 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var bigAccountName = '';
        for (var i = 0; i < STRCOUNT; i++) {
            bigAccountName += 't';
        }
        var enableBundle = "com.example.actsaccountsceneappaccess";
        appAccountManager.enableAppAccess(bigAccountName, enableBundle, (err)=>{
            console.debug("====>enableAppAccess 0900 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsAccountAppAccess_0900 end====");
            done();
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1000
     * @tc.name      : enableAppAccess promise
     * @tc.desc      : The authorized account name is a long string
     */
    it('ActsAccountAppAccess_1000', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1000 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var bigAccountName = '';
        for (var i = 0; i < STRCOUNT; i++) {
            bigAccountName += 't';
        }
        var enableBundle = "com.example.actsaccountsceneappaccess";
        console.debug("====>enableAppAccess ActsAccountAppAccess_1000 start====");
        try{
            await appAccountManager.enableAppAccess(bigAccountName, enableBundle);
        }
        catch(err){
            console.debug("====>enableAppAccess 1000 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsAccountAppAccess_1000 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1100
     * @tc.name      : enableAppAccess callback
     * @tc.desc      : The authorized bundle name is a long string
     */
    it('ActsAccountAppAccess_1100', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var bigBundleName = '';
        for (var i = 0; i < STRCOUNT; i++) {
            bigBundleName += 't';
        }
        appAccountManager.addAccount("AppAccess_callback_bigBundleName", (err)=>{
            console.debug("====>add account ActsAccountAppAccess_1100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.enableAppAccess("AppAccess_callback_bigBundleName", bigBundleName, (err)=>{
                console.debug("====>enableAppAccess 1100 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("AppAccess_callback_bigBundleName", (err)=>{
                    console.debug("====>delete Account ActsAccountAppAccess_1100 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAppAccess_1100 end====");
                    done();
                });
            })
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1200
     * @tc.name      : enableAppAccess promise
     * @tc.desc      : The authorized bundle name is a long string
     */
    it('ActsAccountAppAccess_1200', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var bigBundleName = '';
        for (var i = 0; i < STRCOUNT; i++) {
            bigBundleName += 't';
        }
        console.debug("====>add account ActsAccountAppAccess_1200 start====");
        await appAccountManager.addAccount("AppAccess_promise_bigBundleName");
        console.debug("====>enableAppAccess ActsAccountAppAccess_1200 start====");
        try{
            await appAccountManager.enableAppAccess("AppAccess_promise_bigBundleName", bigBundleName);
        }
        catch(err){
            console.debug("====>enableAppAccess 1200 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAppAccess_1200 start====");
            await appAccountManager.deleteAccount("AppAccess_promise_bigBundleName");
            console.debug("====>ActsAccountAppAccess_1200 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1300
     * @tc.name      : enableAppAccess callback
     * @tc.desc      : The authorized account name is an empty string
     */
    it('ActsAccountAppAccess_1300', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var emptyBundleName = '';
        appAccountManager.addAccount("AppAccess_callback_emptyBundleName", (err)=>{
            console.debug("====>add account ActsAccountAppAccess_1300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.enableAppAccess("AppAccess_callback_emptyBundleName", emptyBundleName, (err)=>{
                console.debug("====>enableAppAccess 1300 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("AppAccess_callback_emptyBundleName", (err)=>{
                    console.debug("====>delete Account ActsAccountAppAccess_1300 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAppAccess_1300 end====");
                    done();
                });
            })
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1400
     * @tc.name      : enableAppAccess promise
     * @tc.desc      : The authorized account name is an empty string
     */
    it('ActsAccountAppAccess_1400', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var emptyBundleName = '';
        console.debug("====>add account ActsAccountAppAccess_1400 start====");
        await appAccountManager.addAccount("AppAccess_promise_emptyBundleName");
        console.debug("====>enableAppAccess ActsAccountAppAccess_1400 start====");
        try{
            await appAccountManager.enableAppAccess("AppAccess_promise_emptyBundleName", emptyBundleName);
        }
        catch(err){
            console.debug("====>enableAppAccess 1400 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAppAccess_1400 start====");
            await appAccountManager.deleteAccount("AppAccess_promise_emptyBundleName");
            console.debug("====>ActsAccountAppAccess_1400 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1500
     * @tc.name      : disableAppAccess callback
     * @tc.desc      : Cancel authorization Accounts that have not authorized other applications
     */
    it('ActsAccountAppAccess_1500', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var enableBundle = "com.example.actsaccountsceneappaccess";
        appAccountManager.addAccount("AppAccess_callback_account", (err)=>{
            console.debug("====>add account ActsAccountAppAccess_1500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.disableAppAccess("AppAccess_callback_account", enableBundle, (err)=>{
                expect(err.code != 0).assertEqual(true)
                appAccountManager.deleteAccount("AppAccess_callback_account", (err)=>{
                    console.debug("====>delete Account ActsAccountAppAccess_1500 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAppAccess_1500 end====");
                    done();
                });
            })
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1600
     * @tc.name      : disableAppAccess promise
     * @tc.desc      : Cancel authorization Accounts that have not authorized other applications
     */
    it('ActsAccountAppAccess_1600', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var enableBundle = "com.example.actsaccountsceneappaccess";
        console.debug("====>add account ActsAccountAppAccess_1600 start====");
        await appAccountManager.addAccount("AppAccess_promise_account");
        console.debug("====>disableAppAccess ActsAccountAppAccess_1600 start====");
        try{
            await appAccountManager.disableAppAccess("AppAccess_promise_account", enableBundle);
        }
        catch(err){
            console.debug("====>disableAppAccess ActsAccountAppAccess_1600 err:" + JSON.stringify(err));
            console.debug("====>delete account ActsAccountAppAccess_1600 start====");
            await appAccountManager.deleteAccount("AppAccess_promise_account");
            console.debug("====>ActsAccountAppAccess_1600 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1700
     * @tc.name      : disableAppAccess callback
     * @tc.desc      : The disableAppAccess parameter bundleName does not exist
     */
    it('ActsAccountAppAccess_1700', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1700 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var notExistBundle = "com.example.actsaccountnotexistbundle";
        appAccountManager.addAccount("AppAccess_callback_notExistBundle", (err)=>{
            console.debug("====>add account ActsAccountAppAccess_1700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.disableAppAccess("AppAccess_callback_notExistBundle", notExistBundle, (err)=>{
                expect(err.code != 0).assertEqual(true)
                appAccountManager.deleteAccount("AppAccess_callback_notExistBundle", (err)=>{
                    console.debug("====>delete Account ActsAccountAppAccess_1700 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAppAccess_1700 end====");
                    done();
                });
            })
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1800
     * @tc.name      : disableAppAccess promise
     * @tc.desc      : The disableAppAccess parameter bundleName does not exist
     */
    it('ActsAccountAppAccess_1800', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1800 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var notExistBundle = "com.example.actsaccountnotexistbundle";
        console.debug("====>add account ActsAccountAppAccess_1800 start====");
        await appAccountManager.addAccount("AppAccess_promise_notExistBundle");
        console.debug("====>disableAppAccess ActsAccountAppAccess_1800 start====");
        try{
            await appAccountManager.disableAppAccess("AppAccess_promise_notExistBundle", notExistBundle);
        }
        catch(err){
            console.debug("====>disableAppAccess ActsAccountAppAccess_1800 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAppAccess_1800 start====");
            await appAccountManager.deleteAccount("AppAccess_promise_notExistBundle");
            console.debug("====>ActsAccountAppAccess_1600 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAppAccess_1900
     * @tc.name      : disableAppAccess callback
     * @tc.desc      : The disableAppAccess parameter accountName does not exist
     */
    it('ActsAccountAppAccess_1900', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_1900 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var notExistAccount = "AppAccess_callback_notExistAccount";
        var enableBundle = "com.example.actsaccountsceneappaccess";
        console.debug("====>disableAppAccess ActsAccountAppAccess_1900====");
        appAccountManager.disableAppAccess(notExistAccount, enableBundle, (err)=>{
            console.debug("====>disableAppAccess ActsAccountAppAccess_1900 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsAccountAppAccess_1900 end====");
            done();
        })
    });

    /*
     * @tc.number    : ActsAccountAppAccess_2000
     * @tc.name      : disableAppAccess promise
     * @tc.desc      : The disableAppAccess parameter accountName does not exist
     */
    it('ActsAccountAppAccess_2000', 0, async function (done) {
        console.debug("====>ActsAccountAppAccess_2000 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        var notExistAccount = "AppAccess_promise_notExistAccount";
        var enableBundle = "com.example.actsaccountsceneappaccess";
        console.debug("====>disableAppAccess ActsAccountAppAccess_2000====");
        try{
            await appAccountManager.disableAppAccess(notExistAccount, enableBundle);
        }
        catch(err){
            console.debug("====>disableAppAccess ActsAccountAppAccess_2000 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>ActsAccountAppAccess_2000 end====");
            done();
        }
    });
})