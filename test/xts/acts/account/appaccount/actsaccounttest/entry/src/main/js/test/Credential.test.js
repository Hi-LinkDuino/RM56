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
const EACHTIMEOUT = 500;
describe('ActsAccountCredential', function () {
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
     * @tc.number    : ActsAccountCredential_0100
     * @tc.name      : The correct calls setAssociatedData and getAccountCredential get the credential
     * @tc.desc      : The setAccountCredential setting credential is called when the forwarding parameters
     *                 are correct, and then getAccountCredential is called for the credential(callback)
     */
    it('ActsAccountCredential_0100', 0, async function (done) {
        console.debug("====>ActsAccountCredential_0100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0100", (err)=>{
            console.debug("====>add account ActsAccountCredential_0100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAccountCredential("account_name_0100", "credentialType1", "credential1",(err)=>{
                console.debug("====>ActsAccountCredential_0100 setAccountCredential:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.getAccountCredential("account_name_0100", "credentialType1", (err,data)=>{
                    console.debug("====>getAccountCredential 0100 err:" + JSON.stringify(err));
                    console.debug("====>getAccountCredential 0100 data:" + JSON.stringify(data));
                    expect(err.code).assertEqual(0);
                    expect(data).assertEqual("credential1");
                    appAccountManager.deleteAccount("account_name_0100", (err)=>{
                        console.debug("====>delete Account 0100 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountCredential_0100 end====");
                        done();
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_0200
     * @tc.name      : The correct calls setAccountCredential and getAccountCredential get the credential
     * @tc.desc      : The setAssociatedData setting credential is called when the forwarding parameters
     *                 are correct, and then getAccountCredential is called for the credential(promise)
     */
    it('ActsAccountCredential_0200', 0, async function (done) {
        console.debug("====>ActsAccountCredential_0200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add accountActsAccountCredential_0200 start====");
        await appAccountManager.addAccount("account_name_0200");
        console.debug("====>setAccountCredential ActsAccountCredential_0200 start====");
        try{
            await appAccountManager.setAccountCredential("account_name_0200", "credentialType2", "credential2");
        }
        catch(err){
            console.error("setAccountCredential ActsAccountCredential_0200 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAccountCredential ActsAccountCredential_0200 start====");
        try{
            var data = await appAccountManager.getAccountCredential("account_name_0200", "credentialType2");
        }
        catch(err){
            console.error("getAccountCredential ActsAccountCredential_0200 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAccountCredential ActsAccountCredential_0200 data:" + JSON.stringify(data));
        expect(data).assertEqual("credential2");
        console.debug("====>delete account ActsAccountCredential_0200 start====");
        appAccountManager.deleteAccount("account_name_0200")
        console.debug("====>ActsAccountCredential_0200 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountCredential_0300
     * @tc.name      : Whether getAssociatedData can get the correct value when calling setAccountCredential two times
     * @tc.desc      : When the first setAccountCredential is called and the second setAccountCredential
     *                 is called,the setting of the credential is different if the call getAccountCredential
     *                 can get the second credential(callback)
     */
    it('ActsAccountCredential_0300', 0, async function (done) {
        console.debug("====>ActsAccountCredential_0300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0300",(err)=>{
            console.debug("====>add account ActsAccountCredential_0300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAccountCredential("account_name_0300", "credentialType3", "credential3",(err)=>{
                console.debug("====>setAccountCredential first time 0300 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.setAccountCredential("account_name_0300", "credentialType3", "newcredential3",(err)=>{
                    console.debug("====>setAccountCredential second time 0300 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    appAccountManager.getAccountCredential("account_name_0300", "credentialType3", (err, data)=>{
                        console.debug("====>getAccountCredential 0300 err:" + JSON.stringify(err));
                        console.debug("====>getAccountCredential 0300 data:" + JSON.stringify(data));
                        expect(err.code).assertEqual(0);
                        expect(data).assertEqual("newcredential3");
                        appAccountManager.deleteAccount("account_name_0300", (err)=>{
                            console.debug("====>delete Account 0300 err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsAccountCredential_0300 end====");
                            done();
                        });
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_0400
     * @tc.name      : Whether getAssociatedData can get the correct value when calling setAccountCredential two times
     * @tc.desc      : When the first setAccountCredential is called and the second setAccountCredential
     *                 is called,the setting of the credential is different if the call getAccountCredential
     *                 can get the second credential(promise)
     */
    it('ActsAccountCredential_0400', 0, async function (done) {
        console.debug("====>ActsAccountCredential_0400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountCredential_0400 start====");
        await appAccountManager.addAccount("account_name_0400");
        try{
            await appAccountManager.setAccountCredential("account_name_0400", "credentialType4", "credential4");
            await appAccountManager.setAccountCredential("account_name_0400", "credentialType4", "newcredential4")
        }
        catch(err){
            console.error("setAccountCredential ActsAccountCredential_0400 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            var data = await appAccountManager.getAccountCredential("account_name_0400", "credentialType4");
        }
        catch(err){
            console.error("getAccountCredential ActsAccountCredential_0400 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>ActsAccountCredential_0400 getAccountCredential data:" + JSON.stringify(data));
        expect(data).assertEqual("newcredential4");
        console.debug("====>delete account ActsAccountCredential_0400 start====");
        appAccountManager.deleteAccount("account_name_0400")
        console.debug("====>ActsAccountCredential_0400 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountCredential_0500
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is null
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming credential is null(callback)
     */
    it('ActsAccountCredential_0500', 0, async function (done) {
        console.debug("====>ActsAccountCredential_0500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0500",(err)=>{
            console.debug("====>add account ActsAccountCredential_0500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAccountCredential("account_name_0500", "credentialType5", "", (err)=>{
                console.debug("====>setAccountCredential ActsAccountCredential_0500 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.getAccountCredential("account_name_0500", "credentialType5", (err,data)=>{
                    console.debug("====>getAccountCredential ActsAccountCredential_0500 err:" + JSON.stringify(err));
                    console.debug("====>getAccountCredential ActsAccountCredential_0500 data:" + JSON.stringify(data));
                    expect(err.code).assertEqual(0);
                    expect(data).assertEqual("");
                    appAccountManager.deleteAccount("account_name_0500", (err)=>{
                        console.debug("====>delete Account ActsAccountCredential_0500 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountCredential_0500 end====");
                        done();
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_0600
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is null
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming credential is null(promise)
     */
    it('ActsAccountCredential_0600', 0, async function (done) {
        console.debug("====>ActsAccountCredential_0600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountCredential_0600 start====");
        await appAccountManager.addAccount("account_name_0600");
        console.debug("====>setAccountCredential ActsAccountCredential_0600 start====");
        await appAccountManager.setAccountCredential("account_name_0600", "credentialType6", "");
        console.debug("====>getAccountCredential ActsAccountCredential_0600 start====");
        var data = await appAccountManager.getAccountCredential("account_name_0600", "credentialType6");
        console.debug("====>getAccountCredential ActsAccountCredential_0600 data" + JSON.stringify(data));
        expect(data).assertEqual("");
        console.debug("====>delete account ActsAccountCredential_0600 start====");
        await appAccountManager.deleteAccount("account_name_0600");
        console.debug("====>ActsAccountCredential_0600 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountCredential_0700
     * @tc.name      : setAssociatedData setting value is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting credential when the incoming parameter type is null(callback)
     */
    it('ActsAccountCredential_0700', 0, async function (done) {
        console.debug("====>ActsAccountCredential_0700 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0700", (err)=>{
            console.debug("====>add account ActsAccountCredential_0700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAccountCredential("account_name_0700", "", "credential7", (err)=>{
                console.debug("====>setAccountCredential ActsAccountCredential_0700 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_0700", (err)=>{
                    console.debug("====>delete Account ActsAccountCredential_0700 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountCredential_0700 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_0800
     * @tc.name      : setAssociatedData setting value is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting credential when the incoming parameter type is null(promise)
     */
    it('ActsAccountCredential_0800', 0, async function (done) {
        console.debug("====>ActsAccountCredential_0800 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ccountActsAccountCredential_0800 start====");
        await appAccountManager.addAccount("account_name_0800");
        try{
            await appAccountManager.setAccountCredential("account_name_0800", "", "credential8");
        }
        catch(err){
            console.debug("====>setAccountCredential ActsAccountCredential_0800 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountCredential_0800 start====");
            appAccountManager.deleteAccount("account_name_0800");
            console.debug("====>ActsAccountCredential_0800 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountCredential_0900
     * @tc.name      : getAccountCredential getting value is called when the argument is wrong
     * @tc.desc      : Call getAccountCredential getting credential when the incoming parameter type is wrong(callback)
     */
    it('ActsAccountCredential_0900', 0, async function (done) {
        console.debug("====>ActsAccountCredential_0900 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0900", (err)=>{
            console.debug("====>add account ActsAccountCredential_0900 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.getAccountCredential("account_name_0900", "", (err)=>{
                console.debug("====>getAccountCredential ActsAccountCredential_0900 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_0900", (err)=>{
                    console.debug("====>delete Account ActsAccountCredential_0900 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountCredential_0900 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_1000
     * @tc.name      : getAccountCredential getting value is called when the argument is wrong
     * @tc.desc      : Call getAccountCredential getting credential when the incoming parameter type is wrong(promise)
     */
    it('ActsAccountCredential_1000', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1000 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountCredential_1000 start====");
        await appAccountManager.addAccount("account_name_1000");
        try{
            await appAccountManager.getAccountCredential("account_name_1000","");
        }
        catch(err){
            console.debug("====>getAccountCredential ActsAccountCredential_1000 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountCredential_1000 start====");
            await appAccountManager.deleteAccount("account_name_1000");
            console.debug("====>ActsAccountCredential_1000 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountCredential_1100
     * @tc.name      : setAssociatedData setting credential is called when the argument is sapce
     * @tc.desc      : Call setAssociatedData setting credential when the incoming parameter type is space
     *                 then use getAccountCredential getting the credential(callback)
     */
    it('ActsAccountCredential_1100', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1100",(err)=>{
            console.debug("====>add account ActsAccountCredential_1100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAccountCredential("account_name_1100", " ", "credential11",(err)=>{
                console.debug("====>setAccountCredential ActsAccountCredential_1100 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.getAccountCredential("account_name_1100", " ", (err, data)=>{
                    console.debug("====>getAccountCredential 1100 err:" + JSON.stringify(err));
                    console.debug("====>getAccountCredential 1100 data:" + JSON.stringify(data));
                    expect(err.code).assertEqual(0);
                    expect(data).assertEqual("credential11");
                    appAccountManager.deleteAccount("account_name_1100", (err)=>{
                        console.debug("====>delete Account 1100 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountCredential_1100 end====");
                        done();
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_1200
     * @tc.name      : setAssociatedData setting credential is called when the argument is sapce
     * @tc.desc      : Call setAssociatedData setting credential when the incoming parameter type is space
     *                 then use getAccountCredential getting the credential(promise)
     */
    it('ActsAccountCredential_1200', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountCredential_1200 start====");
        await appAccountManager.addAccount("account_name_1200");
        console.debug("====>setAccountCredential ActsAccountCredential_1200 start====");
        await appAccountManager.setAccountCredential("account_name_1200", " ", "credential12");
        console.debug("====>getAccountCredential ActsAccountCredential_1200 start====");
        var data = await appAccountManager.getAccountCredential("account_name_1200", " ");
        console.debug("====>getAccountCredential ActsAccountCredential_1200 data" + JSON.stringify(data));
        expect(data).assertEqual("credential12");
        console.debug("====>delete account ActsAccountCredential_1200 start====");
        await appAccountManager.deleteAccount("account_name_1200");
        console.debug("====>ActsAccountCredential_1200 end====");
        done(); 
    });

    /*
     * @tc.number    : ActsAccountCredential_1300
     * @tc.name      : setAssociatedData setting credential is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting credential when the incoming parameter name is null(callback)
     */
    it('ActsAccountCredential_1300', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1300",(err)=>{
            console.debug("====>add account ActsAccountCredential_1300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAccountCredential("", "credentialType13", "credential13", (err)=>{
                console.debug("====>setAccountCredential ActsAccountCredential_1300 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_1300", (err)=>{
                    console.debug("====>delete Account ActsAccountCredential_1300 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountCredential_1300 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_1400
     * @tc.name      : setAssociatedData setting credential is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting credential when the incoming parameter name is null(promise)
     */
    it('ActsAccountCredential_1400', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountCredential_1400 start====");
        await appAccountManager.addAccount("account_name_1400");
        try{
            await appAccountManager.setAccountCredential("", "credentialType14", "credential14");
        }
        catch(err){
            console.debug("====>setAccountCredential ActsAccountCredential_1400 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountCredential_1400 start====");
            await appAccountManager.deleteAccount("account_name_1400");
            console.debug("====>ActsAccountCredential_1400 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountCredential_1500
     * @tc.name      : Get it directly without setting credential callback
     * @tc.desc      : Call getAccountCredential directly to get credential without calling setAccountCredential
     */
    it('ActsAccountCredential_1500', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1500", (err)=>{
            console.debug("====>add account ActsAccountCredential_1500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.getAccountCredential("account_name_1500", "credentialType15", (err)=>{
                console.debug("====>getAccountCredential ActsAccountCredential_1500 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_1500", (err)=>{
                    console.debug("====>delete Account ActsAccountCredential_1500 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountCredential_1500 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_1600
     * @tc.name      : Get it directly without setting credential promise
     * @tc.desc      : Call getAccountCredential directly to get credential without calling setAccountCredential
     */
    it('ActsAccountCredential_1600', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountCredential_1600 start====");
        await appAccountManager.addAccount("account_name_1600");
        try{
            await appAccountManager.getAccountCredential("account_name_1600", "credentialType16");
        }
        catch(err){
            console.debug("====>getAccountCredential ActsAccountCredential_1600 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountCredential_1600 start====");
            await appAccountManager.deleteAccount("account_name_1600");
            console.debug("====>ActsAccountCredential_1600 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountCredential_1700
     * @tc.name      : setAccountCredential setting credential is called when the argument is wrong callback
     * @tc.desc      : Call setAccountCredential setting credential when the incoming parameter credential is wrong
     */
    it('ActsAccountCredential_1700', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1700 start====");
        var CREDENTIALOVERSIZE = "K"
        for(var i = 0;i < 256;i++)
        CREDENTIALOVERSIZE = CREDENTIALOVERSIZE + "K!@#";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1700", (err)=>{
            console.debug("====>add account ActsAccountCredential_1700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAccountCredential("account_name_1700", "credentialType17", CREDENTIALOVERSIZE, (err)=>{
                console.debug("====>CREDENTIALOVERSIZE.length:" + JSON.stringify(CREDENTIALOVERSIZE.length));
                expect(CREDENTIALOVERSIZE.length).assertEqual(1025);
                console.debug("====>setAccountCredential ActsAccountCredential_1700 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_1700", (err)=>{
                    console.debug("====>delete Account ActsAccountCredential_1700 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountCredential_1700 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_1800
     * @tc.name      : setAccountCredential setting credential is called when the argument is wrong promise
     * @tc.desc      : Call setAccountCredential setting credential when the incoming parameter credential is wrong
     */
    it('ActsAccountCredential_1800', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1800 start====");
        var bigStr = "K"
        for(var i = 0;i < 256;i++)
        bigStr = bigStr + "K!@#";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountCredential_1800 start====");
        await appAccountManager.addAccount("account_name_1800");
        try{
            await appAccountManager.setAccountCredential("account_name_1800", "credentialType18", bigStr);
        }
        catch(err){
            console.debug("====>bigStr.length:" + JSON.stringify(bigStr.length));
            expect(bigStr.length).assertEqual(1025);
            console.debug("====>setAccountCredential ActsAccountCredential_1800 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountCredential_1800 start====");
            await appAccountManager.deleteAccount("account_name_1800");
            console.debug("====>ActsAccountCredential_1800 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountCredential_1900
     * @tc.name      : setAccountCredential setting credential is called when the argument is wrong
     * @tc.desc      : Call setAccountCredential setting type when the incoming parameter credential is wrong(callback)
     */
    it('ActsAccountCredential_1900', 0, async function (done) {
        console.debug("====>ActsAccountCredential_1900 start====");
        var CREDENTIALTYPEOVERSIZE = "K"
        for(var i = 0;i < 256;i++)
        CREDENTIALTYPEOVERSIZE = CREDENTIALTYPEOVERSIZE + "K!@#";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1900", (err)=>{
            console.debug("====>add account ActsAccountCredential_1900 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAccountCredential("account_name_1900", CREDENTIALTYPEOVERSIZE, "credential19", (err)=>{
                console.debug("====>CREDENTIALTYPEOVERSIZE.length:" + JSON.stringify(CREDENTIALTYPEOVERSIZE.length));
                expect(CREDENTIALTYPEOVERSIZE.length).assertEqual(1025);
                console.debug("====>setAccountCredential ActsAccountCredential_1900 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_1900", (err)=>{
                    console.debug("====>delete Account ActsAccountCredential_1900 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountCredential_1900 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_2000
     * @tc.name      : setAccountCredential setting credential is called when the argument is wrong
     * @tc.desc      : Call setAccountCredential setting type when the incoming parameter credential is wrong(promise)
     */
    it('ActsAccountCredential_2000', 0, async function (done) {
        console.debug("====>ActsAccountCredential_2000 start====");
        var CREDENTIALTYPEOVERSIZE = "K"
        for(var i = 0;i < 256;i++)
        CREDENTIALTYPEOVERSIZE=CREDENTIALTYPEOVERSIZE+"K!@#";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountCredential_2000 start====");
        await appAccountManager.addAccount("account_name_2000");
        try{
            await appAccountManager.setAccountCredential("account_name_2000", CREDENTIALTYPEOVERSIZE , "credential20");
        }
        catch(err){
            console.debug("====>CREDENTIALTYPEOVERSIZE.length:" + JSON.stringify(CREDENTIALTYPEOVERSIZE.length));
            expect(CREDENTIALTYPEOVERSIZE.length).assertEqual(1025);
            console.debug("====>setAccountCredential ActsAccountCredential_2000 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountCredential_2000 start====");
            await appAccountManager.deleteAccount("account_name_2000");
            console.debug("====>ActsAccountCredential_2000 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountCredential_2100
     * @tc.name      : setAccountCredential setting credential is called when the argument is wrong
     * @tc.desc      : Call setAccountCredential setting name when the incoming parameter name is wrong(callback)
     */
    it('ActsAccountCredential_2100', 0, async function (done) {
        console.debug("====>ActsAccountCredential_2100 start====");
        var NAMEOVERSIZE = "n"
        for(var i = 0;i < 256;i++)
        NAMEOVERSIZE = NAMEOVERSIZE + "name";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_2100", (err)=>{
            console.debug("====>add account ActsAccountCredential_2100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAccountCredential(NAMEOVERSIZE, "credentialType21", "credential21", (err)=>{
                console.debug("====>setAccountCredential ActsAccountCredential_2100 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_2100", (err)=>{
                    console.debug("====>delete Account ActsAccountCredential_2100 err:" + JSON.stringify(err));
                    console.debug("====>ActsAccountCredential_2100 end====");
                    expect(err.code).assertEqual(0);
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountCredential_2200
     * @tc.name      : setAccountCredential setting credential is called when the argument is wrong
     * @tc.desc      : Call setAccountCredential setting name when the incoming parameter name is wrong(promise)
     */
    it('ActsAccountCredential_2200', 0, async function (done) {
        console.debug("====>ActsAccountCredential_2200 start====");
        var NAMEOVERSIZE ="n"
        for(var i=0;i<256;i++)
        NAMEOVERSIZE=NAMEOVERSIZE+"name";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountCredential_2200 start====");
        try{
            await appAccountManager.addAccount("account_name_2200");
        }
        catch(err){
            console.debug("====>add account ActsAccountCredential_2200 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>setAccountCredential ActsAccountCredential_2200 start====");
        try{
            await appAccountManager.setAccountCredential(NAMEOVERSIZE, "credentialType22", "credential22");
        }
        catch(err){
            console.debug("====>setAccountCredential ActsAccountCredential_2200 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountCredential_2200 start====");
            await appAccountManager.deleteAccount("account_name_2200");
            console.debug("====>ActsAccountCredential_2200 end====");
            done();
        }
    });
})