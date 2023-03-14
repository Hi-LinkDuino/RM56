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

const EACHTIMEOUT = 2000;
const ERR_OSACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR = 4587523;
const ERR_OSACCOUNT_KIT_REMOVE_OSACCOUNT_ERROR = 4587529;
const ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR = 4587571;
describe('ActsOsAccountPermissionTest', function () {
    function sleep(delay) {
        var start = (new Date()).getTime();
        while((new Date()).getTime() - start < delay) {
            continue;
        }
    }

    beforeEach(async function (done) {
        console.debug("====>beforeEach enter====");
        sleep(EACHTIMEOUT);
        done();
    })

    /*
     * @tc.number  : ActsOsAccountConstraint_0100
     * @tc.name    : createOsAccount callback
     * @tc.desc    : failed to create local user local user after adding constraint
     */
    it('ActsOsAccountConstraint_0100', 0, async function(done){
        console.debug("====>ActsOsAccountConstraint_0100 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        AccountManager.setOsAccountConstraints(100, ["constraint.os.account.create"], true, (err)=>{
            console.debug("====>add Constraints err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL, (err, info)=>{
                console.debug("====>createOsAccount err:" + JSON.stringify(err));
                console.debug("====>createOsAccount info:" + JSON.stringify(info));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR);
                AccountManager.setOsAccountConstraints(100, ["constraint.os.account.create"], false, (err)=>{
                    console.debug("====>delete Constraints err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsOsAccountPermission_0100 end====");
                    done();
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraint_0200
     * @tc.name    : createOsAccount promise
     * @tc.desc    : failed to create local user local user after adding constraint
     */
    it('ActsOsAccountConstraint_0200', 0, async function(done){
        console.debug("====>ActsOsAccountConstraint_0200 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        await AccountManager.setOsAccountConstraints(100, ["constraint.os.account.create"], true);
        try{
            await AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL);
        }
        catch(err){
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_CREATE_OS_ACCOUNT_ERROR);
            await AccountManager.setOsAccountConstraints(100, ["constraint.os.account.create"], false);
            console.debug("====>ActsOsAccountConstraint_0200 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraint_0300
     * @tc.name    : removeOsAccount callback
     * @tc.desc    : failed to delete local user local user after adding constraint
     */
    it('ActsOsAccountConstraint_0300', 0, async function(done){
        console.debug("====>ActsOsAccountConstraint_0300 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        var localId;
        AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL, (err, info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            localId = info.localId;
            AccountManager.setOsAccountConstraints(100, ["constraint.os.account.remove"], true, (err)=>{
                console.debug("====>add Constraints err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                AccountManager.removeOsAccount(localId, (err)=>{
                    console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_REMOVE_OSACCOUNT_ERROR);
                    AccountManager.setOsAccountConstraints(100, ["constraint.os.account.remove"], false, (err)=>{
                        console.debug("====>delete Constraints err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        AccountManager.removeOsAccount(localId, (err)=>{
                            console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsOsAccountConstraint_0300 end====");
                            done();
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraint_0400
     * @tc.name    : removeOsAccount promise
     * @tc.desc    : failed to delete local user local user after adding constraint
     */
    it('ActsOsAccountConstraint_0400', 0, async function(done){
        console.debug("====>ActsOsAccountConstraint_0400 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        var localId;
        var info = await AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("aaa");
        localId = info.localId;
        await AccountManager.setOsAccountConstraints(100, ["constraint.os.account.remove"], true);
        try{
            await AccountManager.removeOsAccount(localId);
        }
        catch(err){
            console.debug("====>removeOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_REMOVE_OSACCOUNT_ERROR);
            await AccountManager.setOsAccountConstraints(100, ["constraint.os.account.remove"], false);
            console.debug("====>delete Constraint finish====");
            await AccountManager.removeOsAccount(localId);
            console.debug("====>ActsOsAccountConstraint_0400 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraint_0500
     * @tc.name    : activateOsAccount callback
     * @tc.desc    : switching local users fails after adding constraints
     */
    it('ActsOsAccountConstraint_0500', 0, async function(done){
        console.debug("====>ActsOsAccountConstraint_0500 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        var localId;
        AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL, (err, info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            localId = info.localId;
            AccountManager.setOsAccountConstraints(100, ["constraint.os.account.start"], true, (err)=>{
                console.debug("====>add Constraints err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                AccountManager.activateOsAccount(localId, (err)=>{
                    console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
                    AccountManager.setOsAccountConstraints(100, ["constraint.os.account.start"], false, (err)=>{
                        console.debug("====>delete Constraints err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        AccountManager.removeOsAccount(localId, (err)=>{
                            console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsOsAccountConstraint_0500 end====");
                            done();
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraint_0600
     * @tc.name    : activateOsAccount promise
     * @tc.desc    : switching local users fails after adding constraints
     */
    it('ActsOsAccountConstraint_0600', 0, async function(done){
        console.debug("====>ActsOsAccountConstraint_0600 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
        var localId;
        var info = await AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("aaa");
        localId = info.localId;
        await AccountManager.setOsAccountConstraints(100, ["constraint.os.account.start"], true);
        try{
            await AccountManager.activateOsAccount(localId);
        }
        catch(err){
            console.debug("====>activateOsAccount err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_ACTIVATE_OS_ACCOUNT_ERROR);
            await AccountManager.setOsAccountConstraints(100, ["constraint.os.account.start"], false);
            console.debug("====>delete Constraint finish====");
            await AccountManager.removeOsAccount(localId);
            console.debug("====>ActsOsAccountConstraint_0600 end====");
            done();
        }
    })

    /*
     * @tc.number  : ActsOsAccountConstraint_0700
     * @tc.name    : setOsAccountProfilePhoto callback
     * @tc.desc    : failed to set profile photo for local user after adding constraint
     */
    it('ActsOsAccountConstraint_0700', 0, async function(done){
        console.debug("====>ActsOsAccountConstraint_0700 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
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
        "3ZvIt9xyUiQnbFGO0aAIMnAGaxlTcqPsnJrrdb7Jfpv1vt1eilap7Syfk/n/nt09NF//9k=";
        AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL, (err, info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            localId = info.localId;
            AccountManager.setOsAccountConstraints(100, ["constraint.os.account.set.icon"], true, (err)=>{
                console.debug("====>add Constraints err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                AccountManager.setOsAccountProfilePhoto(localId, photo, (err)=>{
                    console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
                    expect(err.code !=0 ).assertEqual(true);
                    AccountManager.setOsAccountConstraints(100, ["constraint.os.account.set.icon"], false, (err)=>{
                        console.debug("====>delete Constraints err:" + JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        AccountManager.removeOsAccount(localId, (err)=>{
                            console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                            expect(err.code).assertEqual(0);
                            console.debug("====>ActsOsAccountConstraint_0700 end====");
                            done();
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number  : ActsOsAccountConstraint_0800
     * @tc.name    : setOsAccountProfilePhoto promise
     * @tc.desc    : failed to set profile photo for local user after adding constraint
     */
    it('ActsOsAccountConstraint_0800', 0, async function(done){
        console.debug("====>ActsOsAccountConstraint_0800 start====");
        var AccountManager = osAccount.getAccountManager();
        console.debug("====>get os AccountManager finish====");
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
        "3ZvIt9xyUiQnbFGO0aAIMnAGaxlTcqPsnJrrdb7Jfpv1vt1eilap7Syfk/n/nt09NF//9k=";
        var info = await AccountManager.createOsAccount("aaa", osAccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" + JSON.stringify(info));
        expect(info.localName).assertEqual("aaa");
        localId = info.localId;
        await AccountManager.setOsAccountConstraints(100, ["constraint.os.account.set.icon"], true);
        try{
            await AccountManager.setOsAccountProfilePhoto(localId, photo);
        }
        catch(err){
            console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            await AccountManager.setOsAccountConstraints(100, ["constraint.os.account.set.icon"], false);
            console.debug("====>delete Constraints finish====");
            await AccountManager.removeOsAccount(localId);
            console.debug("====>ActsOsAccountConstraint_0800 end====");
            done();
        }
    })
})