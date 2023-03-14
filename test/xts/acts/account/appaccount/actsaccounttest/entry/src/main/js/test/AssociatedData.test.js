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
describe('ActsAccountAssociatedData', function () {
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
     * @tc.number    : ActsAccountAssociatedData_0100
     * @tc.name      : The correct calls setAssociatedData and getAssociatedData get the value
     * @tc.desc      : The setAssociatedData setting valueis called when the forwarding parameters
     *                 are correct, and then getAssociatedData is called for the value(callback)
     */
    it('ActsAccountAssociatedData_0100', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_0100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0100",(err)=>{
            console.debug("====>add accountActsAccountAssociatedData_0100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("account_name_0100", "key1", "value1", (err)=>{
                console.debug("====>setAssociatedData ActsAccountAssociatedData_0100 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.getAssociatedData("account_name_0100", "key1", (err, data)=>{
                    console.debug("====>getAssociatedData 0100 err:" + JSON.stringify(err));
                    console.debug("====>getAssociatedData 0100 data:" + JSON.stringify(data));
                    expect(err.code).assertEqual(0);
                    expect(data).assertEqual("value1");
                    appAccountManager.deleteAccount("account_name_0100", (err)=>{
                        console.debug("====>delete Account 0100 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountAssociatedData_0100 end====");
                        done();
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_0200
     * @tc.name      : The correct calls setAssociatedData and getAssociatedData get the value
     * @tc.desc      : The setAssociatedData setting value is called when the forwarding parameters
     *                 are correct, and then getAssociatedData is called for the value(promise)
     */
    it('ActsAccountAssociatedData_0200', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_0200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountAssociatedData_0200 start====");
        try{
            await appAccountManager.addAccount("account_name_0200");
        }
        catch(err){
            console.error("====>add Account ActsAccountAssociatedData_0200 err:" + JSON.stringify(err));
            expect().asserFail();
            done();
        }
        console.debug("====>setAssociatedData ActsAccountAssociatedData_0200 start====");
        try{
            await appAccountManager.setAssociatedData("account_name_0200", "key2", "value2");
        }
        catch(err){
            console.error("====>setAssociatedData ActsAccountAssociatedData_0200 err:" + JSON.stringify(err));
            expect().asserFail();
            done();
        }
        console.debug("====>getAssociatedData ActsAccountAssociatedData_0200 start====");
        try{
            var data = await appAccountManager.getAssociatedData("account_name_0200", "key2");
        }
        catch(err){
            console.error("====>getAssociatedData ActsAccountAssociatedData_0200 err:" + JSON.stringify(err));
            expect().asserFail();
            done();
        }
        console.debug("====>getAssociatedData ActsAccountAssociatedData_0200 data:" + JSON.stringify(data));
        expect(data).assertEqual("value2");
        console.debug("====>delete account ActsAccountAssociatedData_0200 start====");
        try{
            await appAccountManager.deleteAccount("account_name_0200");
        }
        catch(err){
            console.error("====>delete account 0200 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>ActsAccountAssociatedData_0200 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_0300
     * @tc.name      : Get it directly without setting value
     * @tc.desc      : Call getAssociatedData directly to get value without calling setAssociatedData(callback)
     */
    it('ActsAccountAssociatedData_0300', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_0300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0300", (err)=>{
            console.debug("====>add account ActsAccountAssociatedData_0300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.getAssociatedData("account_name_0300", "key3", (err, data)=>{
                console.debug("====>getAssociatedData 0300 err:" + JSON.stringify(err));
                console.debug("====>getAssociatedData 0300 data:" + JSON.stringify(data));
                expect(err.code != 0).assertEqual(true);
                expect(data).assertEqual("");
                appAccountManager.deleteAccount("account_name_0300", (err)=>{
                    console.debug("====>delete Account 0300 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAssociatedData_0300 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_0400
     * @tc.name      : Get it directly without setting value
     * @tc.desc      : Call getAssociatedData directly to get value without calling setAssociatedData(promise)
     */
    it('ActsAccountAssociatedData_0400', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_0400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountAssociatedData_0400 start====");
        try{
            await appAccountManager.addAccount("account_name_0400");
        }
        catch(err){
            console.error("====>add Account ActsAccountAssociatedData_0400 err:" + JSON.stringify(err));
            expect().asserFail();
            done();
        }
        try{
            await appAccountManager.getAssociatedData("account_name_0400", "key4");
            console.error("====>getAssociatedData fail ActsAccountAssociatedData_0400====");
            expect().assertFail();
            done();
        }
        catch(err){
            console.debug("====>getAssociatedData ActsAccountAssociatedData_0400 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAssociatedData_0400 start====");
            try{
                await appAccountManager.deleteAccount("account_name_0400");
            }
            catch(err){
                console.error("====>delete account 0400 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>ActsAccountAssociatedData_0400 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_0500
     * @tc.name      : Call getAssociatedData to get value when passing in the error parameter
     * @tc.desc      : After calling setAssociatedData setting value correctly,
     *                 call the getAssociatedData of the pass error to check if you get the value(callback)
     */
    it('ActsAccountAssociatedData_0500', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_0500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0500",(err)=>{
            console.debug("====>add account ActsAccountAssociatedData_0500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("account_name_0500", "key5", "value5", (err)=>{
                console.debug("====>setAssociatedData ActsAccountAssociatedData_0500 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.getAssociatedData("account_name_0500", "keyerr", (err, data)=>{
                    console.debug("====>getAssociatedData 0500 err:" + JSON.stringify(err));
                    console.debug("====>getAssociatedData 0500 data:" + JSON.stringify(data));
                    expect(err.code != 0).assertEqual(true);
                    appAccountManager.deleteAccount("account_name_0500", (err)=>{
                        console.debug("====>delete Account 0500 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountAssociatedData_0500 end====");
                        done();
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_0600
     * @tc.name      : Call getAssociatedData to get value when passing in the error parameter
     * @tc.desc      : After calling setAssociatedData setting value correctly,
     *                 call the getAssociatedData of the pass error to check if you get the value(promise)
     */
    it('ActsAccountAssociatedData_0600', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_0600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountAssociatedData_0600 start====");
        try{
            await appAccountManager.addAccount("account_name_0600");
        }
        catch(err){
            console.error("====>add Account ActsAccountAssociatedData_0600 fail err:" + JSON.stringify(err));
            expect().asserFail();
            done();
        }
        await appAccountManager.setAssociatedData("account_name_0600", "key6", "value6");
        try{
            var data = await appAccountManager.getAssociatedData("account_name_0600", "keyerr");
        }
        catch(err){
            console.debug("====>getAssociatedData 0600 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAssociatedData_0600 start====");
            try{
                await appAccountManager.deleteAccount("account_name_0600");
            }
            catch(err){
                console.error("====>delete account 0600 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>ActsAccountAssociatedData_0600 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_0700
     * @tc.name      : Whether getAssociatedData can get the correct value when calling setAssociatedData multiple times
     * @tc.desc      : When the first setAssociatedData is called and the second setAssociatedData
     *                 is called,the setting of the value is different if the call getAssociatedData
     *                 can get the second value(callback)
     */
    it('ActsAccountAssociatedData_0700', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_0700 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0700",(err)=>{
            console.debug("====>add account ActsAccountAssociatedData_0700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("account_name_0700", "key7", "value7", (err)=>{
                console.debug("====>setAssociatedDatafir first time 0700 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.setAssociatedData("account_name_0700", "key7", "newvalue7", (err)=>{
                    console.debug("====>setAssociatedDatafir second time 0700 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    appAccountManager.getAssociatedData("account_name_0700", "key7", (err, data)=>{
                        console.debug("====>getAssociatedData 0700 err:" + JSON.stringify(err));
                        console.debug("====>getAssociatedData 0700 data:" + JSON.stringify(data));
                        expect(err.code).assertEqual(0);
                        expect(data).assertEqual("newvalue7");
                        appAccountManager.deleteAccount("account_name_0700", (err)=>{
                            console.debug("====>delete Account 0700 err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsAccountAssociatedData_0700 end====");
                            done();
                        });
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_0800
     * @tc.name      : Whether getAssociatedData can get the correct value when calling setAssociatedData multiple times
     * @tc.desc      : When the first setAssociatedData is called and the second setAssociatedData
     *                 is called,the setting of the value is different if the call getAssociatedData
     *                 can get the second value(promise)
     */
    it('ActsAccountAssociatedData_0800', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_0800 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        try{
            await appAccountManager.addAccount("account_name_0800");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_0800 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await appAccountManager.setAssociatedData("account_name_0800","key8","value8");
            await appAccountManager.setAssociatedData("account_name_0800","key8","newvalue8");
        }catch(err){
            console.error("====>setAssociatedData ActsAccountAssociatedData_0800 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            var data = await appAccountManager.getAssociatedData("account_name_0800", "key8");
        }
        catch(err){
            console.error("====>getAssociatedData ActsAccountAssociatedData_0800 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAssociatedData ActsAccountAssociatedData_0800 data:" + JSON.stringify(data));
        expect(data).assertEqual("newvalue8");
        console.debug("====>delete account ActsAccountAssociatedData_0800 start====");
        try{
            await appAccountManager.deleteAccount("account_name_0800");
        }
        catch(err){
            console.error("====>delete account 0800 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>ActsAccountAssociatedData_0800 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_0900
     * @tc.name      : Whether getAssociatedData can get the correct value when calling setAssociatedData multiple times
     * @tc.desc      : When the first setAssociatedData is called and the second setAssociatedData
     *                 is called,the setting of the value is same if the call getAssociatedData
     *                 can get the second value(callback)
     */
    it('ActsAccountAssociatedData_0900', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_0900 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_0900",(err)=>{
            console.debug("====>add account ActsAccountAssociatedData_0900 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("account_name_0900", "key9", "value9", (err)=>{
                console.debug("====>setAssociatedData first time 0900 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.setAssociatedData("account_name_0900", "key9", "value9", (err)=>{
                    console.debug("====>setAssociatedData second time 0900 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    appAccountManager.getAssociatedData("account_name_0900", "key9", (err, data)=>{
                        console.debug("====>getAssociatedData 0900 err:" + JSON.stringify(err));
                        console.debug("====>getAssociatedData 0900 data:" + JSON.stringify(data));
                        expect(err.code).assertEqual(0);
                        expect(data).assertEqual("value9");
                        appAccountManager.deleteAccount("account_name_0900", (err)=>{
                            console.debug("====>delete Account 0900 err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsAccountAssociatedData_0900 end====");
                            done();
                        });
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1000
     * @tc.name      : Whether getAssociatedData can get the correct value when calling setAssociatedData multiple times
     * @tc.desc      : When the first setAssociatedData is called and the second setAssociatedData
     *                 is called,the setting of the value is same if the call getAssociatedData
     *                 can get the second value(promise)
     */
    it('ActsAccountAssociatedData_1000', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1000 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>addAccount ActsAccountAssociatedData_1000 start====");
        try{
            await appAccountManager.addAccount("account_name_1000");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_1000 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await appAccountManager.setAssociatedData("account_name_1000", "key10", "value10");
            await appAccountManager.setAssociatedData("account_name_1000", "key10", "value10");
        }
        catch(err){
            console.error("====>setAssociatedData ActsAccountAssociatedData_1000 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            var data = await appAccountManager.getAssociatedData("account_name_1000", "key10");
        }
        catch(err){
            console.error("====>getAssociatedData ActsAccountAssociatedData_1000 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAssociatedData 1000 data:" + JSON.stringify(data));
        expect(data).assertEqual("value10");
        console.debug("====>delete account ActsAccountAssociatedData_1000 start====");
        try{
            await appAccountManager.deleteAccount("account_name_1000");
        }
        catch(err){
            console.error("====>delete account 1000 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>ActsAccountAssociatedData_1000 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1100
     * @tc.name      : Call multiple setAssociatedData calls multiple times getAssociatedData gets the value separately
     * @tc.desc      : Call setAssociatedData setting value again after calling setAssociatedData setting
     *                 different value, and then call getAssociatedData twice to get the set value(callback)
     */
    it('ActsAccountAssociatedData_1100', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1100 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1100",(err)=>{
            console.debug("====>add account ActsAccountAssociatedData_1100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("account_name_1100", "key11_first", "value11_first", (err)=>{
                console.debug("====>setAssociatedData first time 1100 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.setAssociatedData("account_name_1100", "key11_second", "value11_second", (err)=>{
                    console.debug("====>setAssociatedData second time 1100 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    appAccountManager.getAssociatedData("account_name_1100", "key11_first", (err,data)=>{
                        console.debug("====>getAssociatedData key11_first 1100 err:" + JSON.stringify(err));
                        console.debug("====>getAssociatedData key11_first 1100 data:" + JSON.stringify(data));
                        expect(err.code).assertEqual(0);
                        expect(data).assertEqual("value11_first");
                        appAccountManager.getAssociatedData("account_name_1100", "key11_second", (err,data)=>{
                            console.debug("====>getAssociatedData key11_second 1100 err:" + JSON.stringify(err));
                            console.debug("====>getAssociatedData key11_second 1100 data:" + JSON.stringify(data));
                            expect(err.code).assertEqual(0);
                            expect(data).assertEqual("value11_second");
                            appAccountManager.deleteAccount("account_name_1100", (err)=>{
                                console.debug("====>delete Account 1100 err:" + JSON.stringify(err));
                            });
                            console.debug("====>ActsAccountAssociatedData_1100 end====");
                            done();
                        });
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1200
     * @tc.name      : Call multiple setAssociatedData calls multiple times getAssociatedData gets the value separately
     * @tc.desc      : Call setAssociatedData setting value again after calling setAssociatedData setting
     *                 different value, and then call getAssociatedData twice to get the set value(promise)
     */
    it('ActsAccountAssociatedData_1200', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1200 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>addAccount ActsAccountAssociatedData_1200 start====");
        try{
            await appAccountManager.addAccount("account_name_1200");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_1200 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await appAccountManager.setAssociatedData("account_name_1200", "key12_first", "value12_first");
            await appAccountManager.setAssociatedData("account_name_1200", "key12_second", "value12_second");
        }
        catch(err){
            console.error("====>setAssociatedData ActsAccountAssociatedData_1200 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            var dataFir = await appAccountManager.getAssociatedData("account_name_1200", "key12_first");
            var dataSec = await appAccountManager.getAssociatedData("account_name_1200", "key12_second");
        }
        catch(err){
            console.error("====>getAssociatedData ActsAccountAssociatedData_1200 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>ActsAccountAssociatedData_1200 getAssociatedData dataFir:" + JSON.stringify(dataFir));
        expect(dataFir).assertEqual("value12_first");
        console.debug("====>ActsAccountAssociatedData_1200 getAssociatedData dataSec:" + JSON.stringify(dataSec));
        expect(dataSec).assertEqual("value12_second");
        console.debug("====>delete account ActsAccountAssociatedData_1200 start====");
        try{
            await appAccountManager.deleteAccount("account_name_1200");
        }
        catch(err){
            console.error("====>delete account 1200 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>ActsAccountAssociatedData_1200 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1300
     * @tc.name      : setAssociatedData setting value is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting value when the incoming parameter KEY is wrong(callback)
     */
    it('ActsAccountAssociatedData_1300', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1300", (err)=>{
            console.debug("====>add account ActsAccountAssociatedData_1300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("account_name_1300", "", "value13", (err)=>{
                console.debug("====>setAssociatedData ActsAccountAssociatedData_1300 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_1300", (err)=>{
                    console.debug("====>delete account ActsAccountAssociatedData_1300 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAssociatedData_1300 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1400
     * @tc.name      : setAssociatedData setting value is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting value when the incoming parameter KEY is wrong(promise)
     */
    it('ActsAccountAssociatedData_1400', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountAssociatedData_1400 start====");
        try{
            await appAccountManager.addAccount("account_name_1400");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_1400 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await appAccountManager.setAssociatedData("account_name_1400", "", "value14");
        }
        catch(err){
            console.debug("====>setAssociatedData ActsAccountAssociatedData_1400 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAssociatedData_1400 start====");
            try{
                await appAccountManager.deleteAccount("account_name_1400");
            }
            catch(err){
                console.error("====>delete account 1400 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>ActsAccountAssociatedData_1400 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1500
     * @tc.name      : setAssociatedData setting key is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting key when the incoming parameter KEY is wrong(callback)
     */
    it('ActsAccountAssociatedData_1500', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1500 start====");
        var keyOverSize = "K"
        for(var i = 0;i < 256;i++)
        keyOverSize = keyOverSize + "K!@#";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1500", (err)=>{
            console.debug("====>add account ActsAccountAssociatedData_1500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>1500 keyOverSize.length:" + JSON.stringify(keyOverSize.length));
            expect(keyOverSize.length).assertEqual(1025);
            appAccountManager.setAssociatedData("account_name_1500", keyOverSize, "value15", (err)=>{
                console.debug("====>setAssociatedData ActsAccountAssociatedData_1500 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_1500", (err)=>{
                    console.debug("====>delete account ActsAccountAssociatedData_1500 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAssociatedData_1500 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1600
     * @tc.name      : setAssociatedData setting key is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting key when the incoming parameter KEY is wrong(promise)
     */
    it('ActsAccountAssociatedData_1600', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1600 start====");
        var keyOverSize = "K"
        for(var i=0;i<256;i++)
        keyOverSize = keyOverSize + "K!@#";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        expect(keyOverSize.length).assertEqual(1025);
        console.debug("====>add account ActsAccountAssociatedData_1600 start====");
        try{
            await appAccountManager.addAccount("account_name_1600");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_1600 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await appAccountManager.setAssociatedData("account_name_1600", keyOverSize, "value16");
        }
        catch(err){
            console.debug("====>setAssociatedData ActsAccountAssociatedData_1600 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAssociatedData_1600 start====");
            try{
                await appAccountManager.deleteAccount("account_name_1600");
            }
            catch(err){
                console.error("====>delete account 1600 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>ActsAccountAssociatedData_1600 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1700
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming key is a space(callback)
     */
    it('ActsAccountAssociatedData_1700', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1700 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1700", (err)=>{
            console.debug("====>add account ActsAccountAssociatedData_1700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("account_name_1700", " ", "value17", (err)=>{
                console.debug("====>ActsAccountAssociatedData_1700 setAssociatedData:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.getAssociatedData("account_name_1700", " ", (err, data)=>{
                    console.debug("====>getAssociatedData 1700 err:" + JSON.stringify(err));
                    console.debug("====>getAssociatedData 1700 data:" + JSON.stringify(data));
                    expect(err.code).assertEqual(0);
                    expect(data).assertEqual("value17");
                    appAccountManager.deleteAccount("account_name_1700", (err)=>{
                        console.debug("====>delete Account 1700 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountAssociatedData_1700 end====");
                        done();
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1800
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming key is a space(promise)
     */
    it('ActsAccountAssociatedData_1800', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1800 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountAssociatedData_1800 start====");
        try{
            await appAccountManager.addAccount("account_name_1800");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_1800 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        await appAccountManager.setAssociatedData("account_name_1800", " ", "value18");
        try{
            var data = await appAccountManager.getAssociatedData("account_name_1800", " ");
        }
        catch(err){
            console.error("====>setAssociatedData ActsAccountAssociatedData_1800 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAssociatedData ActsAccountAssociatedData_1800 data:" + JSON.stringify(data));
        expect(data).assertEqual("value18");
        console.debug("====>delete account ActsAccountAssociatedData_1800 start====");
        try{
            await appAccountManager.deleteAccount("account_name_1800");
        }
        catch(err){
            console.error("====>delete account 1800 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>ActsAccountAssociatedData_1800 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_1900
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming value is null(callback)
     */
    it('ActsAccountAssociatedData_1900', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_1900 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_1900", (err)=>{
            console.debug("====>add account ActsAccountAssociatedData_1900 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("account_name_1900", "key19", "", (err)=>{
                console.debug("====>setAssociatedData ActsAccountAssociatedData_1900 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.getAssociatedData("account_name_1900", "key19", (err, data)=>{
                    console.debug("====>getAssociatedData 1900 err:" + JSON.stringify(err));
                    console.debug("====>getAssociatedData 1900 data:" + JSON.stringify(data));
                    expect(err.code).assertEqual(0);
                    expect(data).assertEqual("");
                    appAccountManager.deleteAccount("account_name_1900", (err)=>{
                        console.debug("====>delete Account 1900 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountAssociatedData_1900 end====");
                        done();
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2000
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming value is null(promise)
     */
    it('ActsAccountAssociatedData_2000', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2000 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountAssociatedData_2000 start====");
        try{
            await appAccountManager.addAccount("account_name_2000");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_2000 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await appAccountManager.setAssociatedData("account_name_2000", "key20", "");
        }
        catch(err){
            console.error("====>setAssociatedData ActsAccountAssociatedData_2000 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            var data = await appAccountManager.getAssociatedData("account_name_2000", "key20");
        }
        catch(err){
            console.error("====>getAssociatedData 2000 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAssociatedData ActsAccountAssociatedData_2000 data:" + JSON.stringify(data));
        expect(data).assertEqual("");
        console.debug("====>delete account ActsAccountAssociatedData_2000 start====");
        try{
            await appAccountManager.deleteAccount("account_name_2000");
        }
        catch(err){
            console.error("====>delete account 2000 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>ActsAccountAssociatedData_2000 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2100
     * @tc.name      : setAssociatedData setting value is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting value when the incoming parameter value is wrong(callback)
     */
    it('ActsAccountAssociatedData_2100', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2100 start====");
        var valueOverSize = "K"
        for(var i = 0;i < 256;i++)
        valueOverSize = valueOverSize + "K!@#";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_2100",(err)=>{
            console.debug("====>add account ActsAccountAssociatedData_2100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            expect(valueOverSize.length).assertEqual(1025);
            appAccountManager.setAssociatedData("account_name_2100", "key21", valueOverSize, (err)=>{
                console.debug("====>setAssociatedData ActsAccountAssociatedData_2100 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_2100", (err)=>{
                    console.debug("====>delete Account ActsAccountAssociatedData_2100 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAssociatedData_2100 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2200
     * @tc.name      : setAssociatedData setting value is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting value when the incoming parameter value is wrong(promise)
     */
    it('ActsAccountAssociatedData_2200', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2200 start====");
        var valueOverSize = "K"
        for(var i = 0;i < 256;i++)
        valueOverSize = valueOverSize + "K!@#";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        expect(valueOverSize.length).assertEqual(1025);
        try{
            await appAccountManager.addAccount("account_name_2200");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_2200 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await appAccountManager.setAssociatedData("account_name_2200", "key22", valueOverSize);
        }
        catch(err){
            console.debug("====>setAssociatedData ActsAccountAssociatedData_2200 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAssociatedData_2200 start====");
            try{
                await appAccountManager.deleteAccount("account_name_2200");
            }
            catch(err){
                console.error("====>delete account 2200 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>ActsAccountAssociatedData_2200 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2300
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming value is a space(callback)
     */
    it('ActsAccountAssociatedData_2300', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2300 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_2300", (err)=>{
            console.debug("====>add account ActsAccountAssociatedData_2300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("account_name_2300", "key23", " ", (err)=>{
                console.debug("====>setAssociatedData ActsAccountAssociatedData_2300 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                appAccountManager.getAssociatedData("account_name_2300", "key23", (err, data)=>{
                    console.debug("====>getAssociatedData 2300 err:" + JSON.stringify(err));
                    console.debug("====>getAssociatedData 2300 data:" + JSON.stringify(data));
                    expect(err.code).assertEqual(0);
                    expect(data).assertEqual(" ");
                    appAccountManager.deleteAccount("account_name_2300", (err)=>{
                        console.debug("====>delete Account 2300 err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsAccountAssociatedData_2300 end====");
                        done();
                    });
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2400
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming value is a space(promise)
     */
    it('ActsAccountAssociatedData_2400', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2400 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>add account ActsAccountAssociatedData_2400 start====");
        try{
            await appAccountManager.addAccount("account_name_2400");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_2400 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        await appAccountManager.setAssociatedData("account_name_2400", "key24", " ");
        try{
            var data = await appAccountManager.getAssociatedData("account_name_2400", "key24");
        }
        catch(err){
            console.error("====>getAssociatedData 2400 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>getAssociatedData 2400 err:" + JSON.stringify(data));
        expect(data).assertEqual(" ");
        console.debug("====>delete account ActsAccountAssociatedData_2400 start====");
        try{
            await appAccountManager.deleteAccount("account_name_2400");
        }
        catch(err){
            console.error("====>delete account 2400 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>ActsAccountAssociatedData_2400 end====");
        done();
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2500
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming name is null(callback)
     */
    it('ActsAccountAssociatedData_2500', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2500 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_2500", (err)=>{
            console.debug("====>add account ActsAccountAssociatedData_2500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            appAccountManager.setAssociatedData("", "key25", "value25", (err)=>{
                console.debug("====>setAssociatedData ActsAccountAssociatedData_2500 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_2500", (err)=>{
                    console.debug("====>delete Account ActsAccountAssociatedData_2500 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAssociatedData_2500 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2600
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming name is null(promise)
     */
    it('ActsAccountAssociatedData_2600', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2600 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        console.debug("====>ActsAccountAssociatedData_2600 addAccount start====");
        try{
            await appAccountManager.addAccount("account_name_2600");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_2600 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await appAccountManager.setAssociatedData("", "key26", "value26");
        }
        catch(err){
            console.debug("====>setAssociatedData ActsAccountAssociatedData_2600 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAssociatedData_2600 start====");
            try{
                await appAccountManager.deleteAccount("account_name_2600");
            }
            catch(err){
                console.error("====>delete account 2600 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>ActsAccountAssociatedData_2600 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2700
     * @tc.name      : setAssociatedData setting name is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting name when the incoming parameter name is wrong(callback)
     */
    it('ActsAccountAssociatedData_2700', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2700 start====");
        var nameOverSize = "n"
        for(var i = 0;i < 256;i++)
        nameOverSize = nameOverSize+"name";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount("account_name_2700",(err)=>{
            console.debug("====>add account ActsAccountAssociatedData_2700 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            expect(nameOverSize.length).assertEqual(1025);
            appAccountManager.setAssociatedData(nameOverSize, "key27", "value27", (err)=>{
                console.debug("====>ActsAccountAssociatedData_2700 setAssociatedData:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.deleteAccount("account_name_2700", (err)=>{
                    console.debug("====>delete Account ActsAccountAssociatedData_2700 err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsAccountAssociatedData_2700 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2800
     * @tc.name      : setAssociatedData setting name is called when the argument is wrong
     * @tc.desc      : Call setAssociatedData setting name when the incoming parameter name is wrong(promise)
     */
    it('ActsAccountAssociatedData_2800', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2800 start====");
        var nameOverSize = "n"
        for(var i = 0;i < 256;i++)
        nameOverSize = nameOverSize + "name";
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        expect(nameOverSize.length).assertEqual(1025);
        console.debug("====>add account ActsAccountAssociatedData_2800 start====");
        try{
            await appAccountManager.addAccount("account_name_2800");
        }
        catch(err){
            console.error("====>add account ActsAccountAssociatedData_2800 fail err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await appAccountManager.setAssociatedData(nameOverSize, "key28", "value28");
        }
        catch(err){
            console.debug("====>setAssociatedData ActsAccountAssociatedData_2800 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>delete account ActsAccountAssociatedData_2800 start====");
            try{
                await appAccountManager.deleteAccount("account_name_2800");
            }
            catch(err){
                console.error("====>delete account 2800 fail err:" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
            console.debug("====>ActsAccountAssociatedData_2800 end====");
            done();
        }
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_2900
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming name is a space(callback)
     */
    it('ActsAccountAssociatedData_2900', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_2900 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        appAccountManager.addAccount(" ", (err)=>{
            console.debug("====>add account ActsAccountAssociatedData_2900 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            appAccountManager.setAssociatedData(" ", "key29", "value29", (err)=>{
                console.debug("====>setAssociatedData ActsAccountAssociatedData_2900 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                appAccountManager.getAssociatedData(" ", "key29", (err, data)=>{
                    console.debug("====>getAssociatedData 2900 err:" + JSON.stringify(err));
                    console.debug("====>getAssociatedData 2900 data:" + JSON.stringify(data));
                    expect(err.code != 0).assertEqual(true);
                    expect(data).assertEqual("");
                    console.debug("====>ActsAccountAssociatedData_2900 end====");
                    done();
                });
            });
        });
    });

    /*
     * @tc.number    : ActsAccountAssociatedData_3000
     * @tc.name      : setAssociatedData and getAssociatedData are called when the argument is spaced
     * @tc.desc      : Call setAssociatedData and then getAssociatedData when the incoming name is a space(promise)
     */
    it('ActsAccountAssociatedData_3000', 0, async function (done) {
        console.debug("====>ActsAccountAssociatedData_3000 start====");
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat finish====");
        try{
            await appAccountManager.addAccount(" ");
        }
        catch(err){
            console.debug("====>add account ActsAccountAssociatedData_3000 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            try{
                await appAccountManager.setAssociatedData(" ", "key30", "value30");
            }
            catch(err){
                console.error("====>setAssociatedData ActsAccountAssociatedData_3000 err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                try{
                    await appAccountManager.getAssociatedData(" ", "key30");
                }
                catch(err){
                    console.error("====>getAssociatedData ActsAccountAssociatedData_3000 err:" + JSON.stringify(err));
                    expect(err.code != 0).assertEqual(true);
                    console.debug("====>ActsAccountAssociatedData_3000 end====");
                    done();
                }
            }
        }
    })
})