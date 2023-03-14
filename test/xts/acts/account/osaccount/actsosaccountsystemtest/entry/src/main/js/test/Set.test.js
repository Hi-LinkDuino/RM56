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

const TIMEOUT = 1000;
const ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_NAME_ERROR = 4587561;
const ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR = 4587563;
describe('ActsOsAccountSystemTest', function () {

    /*
     * @tc.number  : ActsOsAccountSet_0100
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : Set the local name of the os account based on the localId
     */
    it('ActsOsAccountSet_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_0100 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        osAccountManager.createOsAccount("osAccountNameSetA", osAccount.OsAccountType.NORMAL, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameSetA");
            osAccountManager.setOsAccountName(localId, "Abc", (err)=>{
                console.debug("====>setOsAccountName err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                osAccountManager.queryOsAccountById(localId, (err, info)=>{
                    console.debug("====>queryOsAccountById err:" + JSON.stringify(err));
                    console.debug("====>queryOsAccountById info:" + JSON.stringify(info));
                    expect(err.code).assertEqual(0);
                    expect(info.localName).assertEqual("Abc");
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountSet_0100 end====");
                        done();
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_0200
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : Set the local name of the os account based on the localId
     */
    it('ActsOsAccountSet_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_0200 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var osAccountInfo = await osAccountManager.createOsAccount("osAccountNameSetB", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
        localId = osAccountInfo.localId;
        expect(osAccountInfo.localName).assertEqual("osAccountNameSetB");
        await osAccountManager.setOsAccountName(localId, "Abc");
        var getInfo = await osAccountManager.queryOsAccountById(localId);
        console.debug("====>queryOsAccountById info:" + JSON.stringify(getInfo));
        expect(getInfo.localName).assertEqual("Abc");
        await osAccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountSet_0200 end====");
        done();
    })


    /*
     * @tc.number  : ActsOsAccountSet_0300
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : Authentication failed to set local name for 0 user
     */
    it('ActsOsAccountSet_0300', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_0300 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.setOsAccountName(0, "Abc", (err)=>{
            console.debug("====>setOsAccountName err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_NAME_ERROR);
            console.debug("====>ActsOsAccountSet_0300 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_0400
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : Authentication failed to set local name for 0 user
     */
    it('ActsOsAccountSet_0400', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_0400 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.setOsAccountName(0, "Abc");
        }
        catch(err){
            console.debug("====>catch setOsAccountName err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_NAME_ERROR);
            console.debug("====>ActsOsAccountSet_0400 end====");
            done();
        }
     })

    /*
     * @tc.number  : ActsOsAccountSet_0500
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : Authentication failed to set local name for -1 user
     */
    it('ActsOsAccountSet_0500', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_0500 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.setOsAccountName(-1, "Abc", (err)=>{
            console.debug("====>setOsAccountName err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_NAME_ERROR);
            console.debug("====>ActsOsAccountSet_0500 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_0600
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : Authentication failed to set local name for -1 user
     */
    it('ActsOsAccountSet_0600', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_0600 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.setOsAccountName(-1, "Abc");
        }
        catch(err){
            console.debug("====>catch setOsAccountName err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_NAME_ERROR);
            console.debug("====>ActsOsAccountSet_0600 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountSet_0700
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : Authentication failed to set local name for "100" user
     */
    it('ActsOsAccountSet_0700', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_0700 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        function funcSetNameCallback(err){
            console.debug("====>setOsAccountName err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        osAccountManager.setOsAccountName("100", "Abc", funcSetNameCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountSet_0700 end====");
            done();
        }, TIMEOUT);
    })

    /*
     * @tc.number  : ActsOsAccountSet_0800
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : Authentication failed to set local name for "100" user
     */
    it('ActsOsAccountSet_0800', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_0800 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var result = await osAccountManager.setOsAccountName("100", "Abc");
        console.debug("====>catch setOsAccountName err:" + JSON.stringify(result));
        expect(result).assertEqual(null);
        console.debug("====>ActsOsAccountSet_0800 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountSet_0900
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : Authentication failed to set local name for undefined user
     */
    it('ActsOsAccountSet_0900', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_0900 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        function funcSetNameCallback(err){
            console.debug("====>setOsAccountName err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        osAccountManager.setOsAccountName(undefined, "Abc", funcSetNameCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountSet_0900 end====");
            done();
        }, TIMEOUT);
    })

    /*
     * @tc.number  : ActsOsAccountSet_1000
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : Authentication failed to set local name for undefined user
     */
    it('ActsOsAccountSet_1000', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_1000 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var result = await osAccountManager.setOsAccountName(undefined, "Abc");
        console.debug("====>catch setOsAccountName err:" + JSON.stringify(result));
        expect(result).assertEqual(null);
        console.debug("====>ActsOsAccountSet_1000 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountSet_1100
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : Verify that the same username is set repeatedly
     */
    it('ActsOsAccountSet_1100', 0, async function(done){
        console.debug("====>ActsOsAccountSet_1100 start====");
        var osAccountManager = osAccount.getAccountManager();
        var localId;
        osAccountManager.createOsAccount('localName_callback', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("localName_callback");
            localId = data.localId;
            osAccountManager.setOsAccountName(localId, "localName_callback", (err)=>{
                console.debug("====>setOsAccountName err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                osAccountManager.setOsAccountName(localId, "localName_callback", (err)=>{
                    console.debug("====>twice setOsAccountName err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    osAccountManager.queryOsAccountById(localId, (err,info)=>{
                        console.debug("====>queryOsAccountById err:" + JSON.stringify(err));
                        console.debug("====>queryOsAccountById data:" +JSON.stringify(info));
                        expect(err.code).assertEqual(0);
                        expect(info.localName).assertEqual("localName_callback");
                        osAccountManager.removeOsAccount(localId, (err)=>{
                            console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsOsAccountSet_1100 end====");
                            done();
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_1200
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : Verify that the same username is set repeatedly
     */
    it('ActsOsAccountSet_1200', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_1200 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        try{
            var info = await osAccountManager.createOsAccount("localName_promise", osAccount.OsAccountType.NORMAL);
        }
        catch(err){
            console.debug("====>catch createOsAccount err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("localName_promise");
        localId = info.localId
        try{
            await osAccountManager.setOsAccountName(localId, "localName_promise");
        }
        catch(err){
            console.debug("====>catch setOsAccountName err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await osAccountManager.setOsAccountName(localId, "localName_promise");
        }
        catch(err){
            console.debug("====>catch twice setOsAccountName err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            var info = await osAccountManager.queryOsAccountById(localId);
        }
        catch(err){
            console.debug("====>catch queryOsAccountById err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>queryOsAccountById info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("localName_promise");
        await osAccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountSet_1200 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountSet_1300
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : Verify repeated settings with different username
     */
    it('ActsOsAccountSet_1300', 0, async function(done){
        console.debug("====>ActsOsAccountSet_1300 start====");
        var osAccountManager = osAccount.getAccountManager();
        var localId;
        osAccountManager.createOsAccount('localName_callback', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("localName_callback");
            localId = data.localId;
            osAccountManager.setOsAccountName(localId, "localName_callback", (err)=>{
                console.debug("====>setOsAccountName err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                osAccountManager.setOsAccountName(localId, "localName_callbackTwice", (err)=>{
                    console.debug("====>twice setOsAccountName err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    osAccountManager.queryOsAccountById(localId, (err,info)=>{
                        console.debug("====>queryOsAccountById err:" + JSON.stringify(err));
                        console.debug("====>queryOsAccountById data:" +JSON.stringify(info));
                        expect(err.code).assertEqual(0);
                        expect(info.localName).assertEqual("localName_callbackTwice");
                        osAccountManager.removeOsAccount(localId, (err)=>{
                            console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsOsAccountSet_1300 end====");
                            done();
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_1400
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : Verify repeated settings with different username
     */
    it('ActsOsAccountSet_1400', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_1400 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        try{
            var info = await osAccountManager.createOsAccount("localName_promise", osAccount.OsAccountType.NORMAL);
        }
        catch(err){
            console.debug("====>catch createOsAccount err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("localName_promise");
        localId = info.localId
        try{
            await osAccountManager.setOsAccountName(localId, "localName_promise");
        }
        catch(err){
            console.debug("====>catch setOsAccountName err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            await osAccountManager.setOsAccountName(localId, "localName_promiseTwice");
        }
        catch(err){
            console.debug("====>catch setOsAccountName err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            var info = await osAccountManager.queryOsAccountById(localId);
        }
        catch(err){
            console.debug("====>catch twice queryOsAccountById err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>queryOsAccountById info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("localName_promiseTwice");
        await osAccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountSet_1400 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountSet_1500
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : create and set profile photo the base64 image converted from png image
     */
    it('ActsOsAccountSet_1500', 0, async function(done){
        console.debug("====>ActsOsAccountSet_1500 start====");
        var osAccountManager = osAccount.getAccountManager();
        var localId;
        var photo = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA0AAAAPCAYAAAA/I0V3AAAAAXNSR0IArs4c6QAAAARnQU1BAA"+
        "Cxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAACwSURBVDhPvZLBDYMwDEV/ugsXRjAT0EHCOuFIBwkbdIRewi6unbiAyoGgSn1SFH85+Y"+
        "q/4ljARW62X+LHS8uIzjm4dXUYF+utzBikB52Jo5e5iEPKqpACk7R9NM2RvWm5tIkD2czLCUFNKLD6IjdMHFHDzws285MgGrT0xCtp3WOKHo"+
        "+7q0mP0DZW9pNmoEFUzrQjp5cCnaen2kSJXLFD8ghbXyZCMQf/8e8Ns1XVAG/XAgqKzVnJFAAAAABJRU5ErkJggg=="
        osAccountManager.createOsAccount('localNamePhotoPng_callback', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("localNamePhotoPng_callback");
            localId = data.localId;
            osAccountManager.setOsAccountProfilePhoto(localId, photo, (err)=>{
                console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                osAccountManager.queryOsAccountById(localId, (err,info)=>{
                    console.debug("====>queryOsAccountById err:" + JSON.stringify(err));
                    console.debug("====>queryOsAccountById data:" +JSON.stringify(info));
                    expect(err.code).assertEqual(0);
                    expect(info.photo).assertEqual(photo);
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountSet_1500 end====");
                        done();
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_1600
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : create and set profile photo the base64 image converted from png image
     */
    it('ActsOsAccountSet_1600', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_1600 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var photo = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA0AAAAPCAYAAAA/I0V3AAAAAXNSR0IArs4c6QAAAARnQU1BAA"+
        "Cxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAACwSURBVDhPvZLBDYMwDEV/ugsXRjAT0EHCOuFIBwkbdIRewi6unbiAyoGgSn1SFH85+Y"+
        "q/4ljARW62X+LHS8uIzjm4dXUYF+utzBikB52Jo5e5iEPKqpACk7R9NM2RvWm5tIkD2czLCUFNKLD6IjdMHFHDzws285MgGrT0xCtp3WOKHo"+
        "+7q0mP0DZW9pNmoEFUzrQjp5cCnaen2kSJXLFD8ghbXyZCMQf/8e8Ns1XVAG/XAgqKzVnJFAAAAABJRU5ErkJggg=="
        try{
            var info = await osAccountManager.createOsAccount("localPhotoPng_promise", osAccount.OsAccountType.NORMAL);
        }
        catch(err){
            console.debug("====>catch createOsAccount err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("localPhotoPng_promise");
        localId = info.localId
        try{
            await osAccountManager.setOsAccountProfilePhoto(localId, photo);
        }
        catch(err){
            console.debug("====>catch setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            var info = await osAccountManager.queryOsAccountById(localId);
        }
        catch(err){
            console.debug("====>catch queryOsAccountById err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>queryOsAccountById info:" + JSON.stringify(info));
        expect(info.photo).assertEqual(photo);
        await osAccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountSet_1600 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountSet_1700
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : create and set profile photo the base64 image converted from jpg image
     */
    it('ActsOsAccountSet_1700', 0, async function(done){
        console.debug("====>ActsOsAccountSet_1700 start====");
        var osAccountManager = osAccount.getAccountManager();
        var localId;
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
        "3ZvIt9xyUiQnbFGO0aAIMnAGaxlTcqPsnJrrdb7Jfpv1vt1eilap7Syfk/n/nt09NF//9k="
        osAccountManager.createOsAccount('localNamePhotoJpg_callback', osAccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount data:" + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("localNamePhotoJpg_callback");
            localId = data.localId;
            osAccountManager.setOsAccountProfilePhoto(localId, photo, (err)=>{
                console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                osAccountManager.queryOsAccountById(localId, (err,info)=>{
                    console.debug("====>queryOsAccountById err:" + JSON.stringify(err));
                    console.debug("====>queryOsAccountById data:" +JSON.stringify(info));
                    expect(err.code).assertEqual(0);
                    expect(info.photo).assertEqual(photo);
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountSet_1700 end====");
                        done();
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_1800
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : create and set profile photo the base64 image converted from jpg image
     */
    it('ActsOsAccountSet_1800', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_1800 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
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
        "3ZvIt9xyUiQnbFGO0aAIMnAGaxlTcqPsnJrrdb7Jfpv1vt1eilap7Syfk/n/nt09NF//9k="
        try{
            var info = await osAccountManager.createOsAccount("localPhotoJpg_promise", osAccount.OsAccountType.NORMAL);
        }
        catch(err){
            console.debug("====>catch createOsAccount err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("localPhotoJpg_promise");
        localId = info.localId
        try{
            await osAccountManager.setOsAccountProfilePhoto(localId, photo);
        }
        catch(err){
            console.debug("====>catch setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        try{
            var info = await osAccountManager.queryOsAccountById(localId);
        }
        catch(err){
            console.debug("====>catch queryOsAccountById err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>queryOsAccountById info:" + JSON.stringify(info));
        expect(info.photo).assertEqual(photo);
        await osAccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountSet_1800 end====");
        done();
    })

    /*
     * @tc.number  : ActsOsAccountSet_1900
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : set 0 local user profile photo the base64 image converted from png image
     */
    it('ActsOsAccountSet_1900', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_1900 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var photo = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA0AAAAPCAYAAAA/I0V3AAAAAXNSR0IArs4c6QAAAARnQU1BAA"+
        "Cxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAACwSURBVDhPvZLBDYMwDEV/ugsXRjAT0EHCOuFIBwkbdIRewi6unbiAyoGgSn1SFH85+Y"+
        "q/4ljARW62X+LHS8uIzjm4dXUYF+utzBikB52Jo5e5iEPKqpACk7R9NM2RvWm5tIkD2czLCUFNKLD6IjdMHFHDzws285MgGrT0xCtp3WOKHo"+
        "+7q0mP0DZW9pNmoEFUzrQjp5cCnaen2kSJXLFD8ghbXyZCMQf/8e8Ns1XVAG/XAgqKzVnJFAAAAABJRU5ErkJggg=="
        osAccountManager.setOsAccountProfilePhoto(0, photo, (err)=>{
            console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountSet_1900 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_2000
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : set 0 local user profile photo the base64 image converted from png image
     */
    it('ActsOsAccountSet_2000', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_2000 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var photo = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA0AAAAPCAYAAAA/I0V3AAAAAXNSR0IArs4c6QAAAARnQU1BAA"+
        "Cxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAACwSURBVDhPvZLBDYMwDEV/ugsXRjAT0EHCOuFIBwkbdIRewi6unbiAyoGgSn1SFH85+Y"+
        "q/4ljARW62X+LHS8uIzjm4dXUYF+utzBikB52Jo5e5iEPKqpACk7R9NM2RvWm5tIkD2czLCUFNKLD6IjdMHFHDzws285MgGrT0xCtp3WOKHo"+
        "+7q0mP0DZW9pNmoEFUzrQjp5cCnaen2kSJXLFD8ghbXyZCMQf/8e8Ns1XVAG/XAgqKzVnJFAAAAABJRU5ErkJggg=="
        try{
            await osAccountManager.setOsAccountProfilePhoto(0, photo);
        }
        catch(err){
            console.debug("====>catch setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountSet_2000 end====");
            done();
        }
     })

    /*
     * @tc.number  : ActsOsAccountSet_2100
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : set 0 local user profile photo the base64 image converted from jpg image
     */
    it('ActsOsAccountSet_2100', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_2100 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
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
        "3ZvIt9xyUiQnbFGO0aAIMnAGaxlTcqPsnJrrdb7Jfpv1vt1eilap7Syfk/n/nt09NF//9k="
        osAccountManager.setOsAccountProfilePhoto(0, photo, (err)=>{
            console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountSet_2100 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_2200
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : set 0 local user profile photo the base64 image converted from jpg image
     */
    it('ActsOsAccountSet_2200', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_2200 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
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
        "3ZvIt9xyUiQnbFGO0aAIMnAGaxlTcqPsnJrrdb7Jfpv1vt1eilap7Syfk/n/nt09NF//9k="
        try{
            await osAccountManager.setOsAccountProfilePhoto(0, photo);
        }
        catch(err){
            console.debug("====>catch setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountSet_2200 end====");
            done();
        }
     })

    /*
     * @tc.number  : ActsOsAccountSet_2300
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : set the specified local profile photo to "bbb"
     */
    it('ActsOsAccountSet_2300', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_2300 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.setOsAccountProfilePhoto(100, "bbb", (err)=>{
            console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountSet_2300 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_2400
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : set the specified local profile photo to "bbb"
     */
    it('ActsOsAccountSet_2400', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_2400 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.setOsAccountProfilePhoto(100, "bbb");
        }
        catch(err){
            console.debug("====>catch setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountSet_2400 end====");
            done();
        }
     })

     /*
     * @tc.number  : ActsOsAccountSet_2500
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : set the specified local profile photo to ""
     */
    it('ActsOsAccountSet_2500', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_2500 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.setOsAccountProfilePhoto(100, "", (err)=>{
            console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountSet_2500 end====");
            done();
        })
    })

    /*
     * @tc.number  : ActsOsAccountSet_2600
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : set the specified local profile photo to ""
     */
    it('ActsOsAccountSet_2600', 0, async function (done) {
        console.debug("====>ActsOsAccountSet_2600 start====");
        var osAccountManager = osAccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        try{
            await osAccountManager.setOsAccountProfilePhoto(100, "");
        }
        catch(err){
            console.debug("====>catch setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            console.debug("====>ActsOsAccountSet_2600 end====");
            done();
        }
     })

})