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
import osaccount from '@ohos.account.osaccount'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

const TIMEOUT = 1000;
const ERR_OSACCOUNT_KIT_GET_OS_ACCOUNT_PROFILE_PHOTO_ERROR = 4587555;
describe('ActsOsAccountSystemTest', function () {

     /*
     * @tc.number  : ActsOsAccountProfilePhoto_0100
     * @tc.name    : getOsAccountProfilePhoto callback
     * @tc.desc    : Verify that the newly created user photo is an empty string
     */
    it('ActsOsAccountProfilePhoto_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountProfilePhoto_0100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        osAccountManager.createOsAccount("osAccountNameIdSerialC", osaccount.OsAccountType.NORMAL, (err, data)=>{
            console.debug("====>create os account err: " + JSON.stringify(err));
            console.debug("====>create os account OsAccountInfo: " + JSON.stringify(data));
            expect(err.code).assertEqual(0);
            expect(data.localName).assertEqual("osAccountNameIdSerialC");
            localId = data.localId;
            osAccountManager.getOsAccountProfilePhoto(localId, (err, photo)=>{
                console.debug("====>getOsAccountProfilePhoto err:" + JSON.stringify(err));
                console.debug("====>get photo:" + photo + " by localId: " + localId);
                expect(err.code).assertEqual(0);
                expect(photo).assertEqual("");
                osAccountManager.removeOsAccount(localId, (err)=>{
                    console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsOsAccountProfilePhoto_0100 end====");
                    done();
                })
               
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountProfilePhoto_0200
     * @tc.name    : getOsAccountProfilePhoto promise
     * @tc.desc    : Verify that the newly created user photo is an empty string
     */
    it('ActsOsAccountProfilePhoto_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountProfilePhoto_0200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localId;
        var data = await osAccountManager.createOsAccount("osAccountNameIdSerialD", osaccount.OsAccountType.NORMAL);
        console.debug("====>create os account OsAccountInfo: " + JSON.stringify(data));
        expect(data.localName).assertEqual("osAccountNameIdSerialD");
        localId = data.localId;
        var photo = await osAccountManager.getOsAccountProfilePhoto(localId);
        console.debug("====>get photo:" + photo + " by localId: " + localId);
        expect(photo).assertEqual("");
        await osAccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountProfilePhoto_0200 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountProfilePhoto_0300
     * @tc.name    : getOsAccountProfilePhoto callback
     * @tc.desc    : Verify to ger the photo of the user that does not exist
     */
    it('ActsOsAccountProfilePhoto_0300', 0, async function (done) {
        console.debug("====>ActsOsAccountProfilePhoto_0300 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var nonExistentId = 1000;
        osAccountManager.getOsAccountProfilePhoto(nonExistentId, (err)=>{
            console.debug("====>getOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_GET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);  
            console.debug("====>ActsOsAccountProfilePhoto_0300 end====");
            done();
        })
    });

    /*
     * @tc.number  : ActsOsAccountProfilePhoto_0400
     * @tc.name    : getOsAccountProfilePhoto promise
     * @tc.desc    : Verify to get the photo of the user that does not exist
     */
    it('ActsOsAccountProfilePhoto_0400', 0, async function (done) {
        console.debug("====>ActsOsAccountProfilePhoto_0400 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var nonExistentId = 1000;
        try{
            await osAccountManager.getOsAccountProfilePhoto(nonExistentId);
        }catch(err){
            console.debug("====>getOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_GET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);  
            console.debug("====>ActsOsAccountProfilePhoto_0400 end====");
            done();
        }
    });

    /*
     * @tc.number  : ActsOsAccountProfilePhoto_0500
     * @tc.name    : getOsAccountProfilePhoto callback
     * @tc.desc    : Verify to get photo with parameter type mismatch
     */
    it('ActsOsAccountProfilePhoto_0500', 0, async function (done) {
        console.debug("====>ActsOsAccountProfilePhoto_0500 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localIdStr = "100";
        function gerPhotoCallback(err, data){
            console.debug("====>getOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        osAccountManager.getOsAccountProfilePhoto(localIdStr, gerPhotoCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountProfilePhoto_0500 end====");
            done();
        }, TIMEOUT);
    });

    /*
     * @tc.number  : ActsOsAccountProfilePhoto_0600
     * @tc.name    : getOsAccountProfilePhoto promise
     * @tc.desc    : Verify to the get photo with parameter type mismatch
     */
    it('ActsOsAccountProfilePhoto_0600', 0, async function (done) {
        console.debug("====>ActsOsAccountProfilePhoto_0600 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var localIdStr = "100";
        var data = await osAccountManager.getOsAccountProfilePhoto(localIdStr);
        expect(data).assertEqual(null);
        console.debug("====>ActsOsAccountProfilePhoto_0600 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountProfilePhoto_0700
     * @tc.name    : getOsAccountProfilePhoto callback
     * @tc.desc    : Verify to get photo with parameter undefined
     */
    it('ActsOsAccountProfilePhoto_0700', 0, async function (done) {
        console.debug("====>ActsOsAccountProfilePhoto_0700 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var undefinedId = undefined;
        function gerPhotoCallback(err, data){
            console.debug("====>getOsAccountProfilePhoto err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        osAccountManager.getOsAccountProfilePhoto(undefinedId,gerPhotoCallback);
        setTimeout(()=>{
            console.debug("====>ActsOsAccountProfilePhoto_0700 end====");
            done();
        }, TIMEOUT);
    });

    /*
     * @tc.number  : ActsOsAccountProfilePhoto_0800
     * @tc.name    : getOsAccountProfilePhoto promise
     * @tc.desc    : Verify to get photo with parameter undefined
     */
    it('ActsOsAccountProfilePhoto_0800', 0, async function (done) {
        console.debug("====>ActsOsAccountProfilePhoto_0800 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var undefinedId = undefined;
        var data = await osAccountManager.getOsAccountProfilePhoto(undefinedId);
        expect(data).assertEqual(null);
        console.debug("====>ActsOsAccountProfilePhoto_0800 end====");
        done();
    });
})