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

const NAMELIMIT = 1024;
const TIMEOUT = 1000;
const EACHTIMEOUT = 3000;
const ERR_OSACCOUNT_KIT_LOCAL_NAME_OUTFLOW_ERROR = 4587577;
const ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR = 4587563;
describe('ActsOsAccountSystemTest', function () {
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
     * @tc.number  : ActsOsAccountLimit_0100
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : subscription name exceeds the length limit of 1024 characters
     */
    it('ActsOsAccountLimit_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_0100 start====");
        var nameLimit = "";
        for(var i = 0; i < NAMELIMIT + 1; i++){
            nameLimit += 't';
        }
        var localId;
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====> wrongly received localId:" + localId + "after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        osAccountManager.on("activating", nameLimit, onCallback);
        sleep(TIMEOUT);
        osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL, (err,info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            localId = info.localId;
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                sleep(TIMEOUT);
                setTimeout(()=>{
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>remove localId" + localId + " err:" +JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountLimit_0100 end====");
                        done();
                    })
                }, TIMEOUT)
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLimit_0200
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : subscription name exceeds the length limit of 1024 characters
     */
    it('ActsOsAccountLimit_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_0200 start====");
        var nameLimit = "";
        for(var i = 0; i < NAMELIMIT + 1; i++){
            nameLimit += 't';
        }
        var localId;
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                console.debug("====> wrongly received localId:" + localId + "after unsubscribing");
                expect().assertFail();
                done();
            }
        }
        osAccountManager.on("activate", nameLimit, onCallback);
        sleep(TIMEOUT);
        osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL, (err,info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            localId = info.localId;
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                sleep(TIMEOUT);
                setTimeout(()=>{
                    osAccountManager.removeOsAccount(localId, (err)=>{
                        console.debug("====>removeOsAccount err:" +JSON.stringify(err));
                        expect(err.code).assertEqual(0);
                        console.debug("====>ActsOsAccountLimit_0200 end====");
                        done();
                    })
                }, TIMEOUT)
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLimit_0300
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : subscription name length is 1024 characters
     */
    it('ActsOsAccountLimit_0300', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_0300 start====");
        var osAccountManager = osaccount.getAccountManager();
        var localId;
        var activatingSign = false;
        var nameLimit = "";
        for(var i = 0; i < NAMELIMIT; i++){
            nameLimit += 't';
        }
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                osAccountManager.off("activating", nameLimit, offCallback);
            }
        }
        function onActivateCallback(receiveId){
            console.debug("====>onActivateCallback receive localId:" + receiveId);
            if(receiveId == localId){
                osAccountManager.off("activate", nameLimit, offActivateCallback);
            }
        }
        function removeCallback(err){
            console.debug("====>remove localId: " + localId + " err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>the activating sign is: " + activatingSign);
            expect(activatingSign).assertTrue();
            console.debug("====>ActsOsAccountLimit_0300 end====");
            done();
        }
        function offCallback(){
            console.debug("====>off enter")
            activatingSign = true;
        }
        function offActivateCallback(){
            console.debug("====>offActivateCallback enter")
            sleep(TIMEOUT);
            osAccountManager.removeOsAccount(localId, removeCallback);
        }
        console.debug("====>on activating start====");
        osAccountManager.on("activating", nameLimit, onCallback);
        console.debug("====>on activate start====");
        osAccountManager.on("activate", nameLimit, onActivateCallback);
        sleep(TIMEOUT);
        osAccountManager.createOsAccount("osAccountNameA", 1, (err, osAccountInfo)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount osAccountInfo:" + JSON.stringify(osAccountInfo));
            localId = osAccountInfo.localId;
            expect(err.code).assertEqual(0);
            expect(osAccountInfo.localName).assertEqual("osAccountNameA");
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
            });
        });
    });

    /*
     * @tc.number  : ActsOsAccountLimit_0400
     * @tc.name    : Subscribe and unsubscribing local multi-user
     * @tc.desc    : subscription name length is 1024 characters
     */
    it('ActsOsAccountLimit_0400', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_0400 start====");
        var nameLimit = "";
        for(var i = 0; i < NAMELIMIT; i++){
            nameLimit += 't';
        }
        var localId;
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        function onCallback(receiveLocalId){
            console.debug("====>on receive localId:" + receiveLocalId);
            if(receiveLocalId == localId){
                osAccountManager.off("activate", nameLimit, offCallback);
            }
        }
        function offCallback(receiveLocalId){
            console.debug("====>off receive localId:" +receiveLocalId);
            osAccountManager.removeOsAccount(localId, (err)=>{
                console.debug("====>removeOsAccount err:" +JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>ActsOsAccountLimit_0400 end====");
                done();
            })
        }
        osAccountManager.on("activate", nameLimit, onCallback);
        sleep(TIMEOUT);
        osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL, (err,info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            localId = info.localId;
            osAccountManager.activateOsAccount(localId, (err)=>{
                console.debug("====>activateOsAccount err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLimit_0500
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : create name length is 1024 characters
     */
    it('ActsOsAccountLimit_0500', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_0500 start====");
        var nameLimit = "";
        for(var i = 0; i < NAMELIMIT + 1; i++){
            nameLimit += 't';
        }
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL, (err, info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            var localId = info.localId;
            osAccountManager.setOsAccountName(localId, nameLimit, (err)=>{
                console.debug("====>setOsAccountName err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_LOCAL_NAME_OUTFLOW_ERROR);
                osAccountManager.removeOsAccount(localId, (err)=>{
                    console.debug("====>removeOsAccount " + localId + " err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsOsAccountLimit_0500 end====");
                    done();
                })
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLimit_0600
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : create name length is 1024 characters
     */
    it('ActsOsAccountLimit_0600', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_0600 start====");
        var nameLimit = "";
        for(var i = 0; i < NAMELIMIT + 1; i++){
            nameLimit += 't';
        }
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var info = await osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" +JSON.stringify(info));
        expect(info.localName).assertEqual("aaa");
        var localId = info.localId;
        try{
            await osAccountManager.setOsAccountName(localId, nameLimit);
            console.debug("====>setOsAccountName finish====");
        }
        catch(err){
            console.debug("====>setOsAccountName err:" +JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_LOCAL_NAME_OUTFLOW_ERROR);
            await osAccountManager.removeOsAccount(localId);
            console.debug("====>removeOsAccount " + localId +" finish");
            console.debug("====>ActsOsAccountLimit_0600 end====");
            done();
        }
    });

    /*
     * @tc.number  : ActsOsAccountLimit_0700
     * @tc.name    : setOsAccountName callback
     * @tc.desc    : set the length of the profile photo to exceed the limit of 4096 characters
     */
    it('ActsOsAccountLimit_0700', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_0700 start====");
        var nameLimit = "";
        for(var i = 0; i < NAMELIMIT; i++){
            nameLimit += 't';
        }
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL, (err, info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            var localId = info.localId;
            osAccountManager.setOsAccountName(localId, nameLimit, (err)=>{
                console.debug("====>setOsAccountName err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                osAccountManager.removeOsAccount(localId, (err)=>{
                    console.debug("====>removeOsAccount " + localId + " err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsOsAccountLimit_0700 end====");
                    done();
                })
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLimit_0800
     * @tc.name    : setOsAccountName promise
     * @tc.desc    : set the length of the profile photo to exceed the limit of 4096 characters
     */
    it('ActsOsAccountLimit_0800', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_0800 start====");
        var nameLimit = "";
        for(var i = 0; i < NAMELIMIT; i++){
            nameLimit += 't';
        }
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var info = await osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" +JSON.stringify(info));
        expect(info.localName).assertEqual("aaa");
        var localId = info.localId;
        try{
            await osAccountManager.setOsAccountName(localId, nameLimit);
        }
        catch(err){
            console.debug("====>setOsAccountName err:" +JSON.stringify(err));
            expect().assertFail();
            done();
        }
        await osAccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountLimit_0800 end====");
        done();
    });

    /*
     * @tc.number  : ActsOsAccountLimit_0900
     * @tc.name    : setOsAccountProfilePhoto callback
     * @tc.desc    : set the length of the profile photo to exceed the limit of 4096 characters
     */
    it('ActsOsAccountLimit_0900', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_0900 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var photo = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAA8AAAAIcCAIAAAC2P1AsAAAAAXNSR0IArs4c6QAAAARnQU1BAA"+
        "Cxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAx6SURBVHhe7d0hbFR3HMDxY4rKSkggWTMQLMEUtYppqgZiCZYlrSwJDk3mlrQSBJZkYq"+
        "gVgV1RrZmogVRAAvJk67rr9VoqVug3Gekon08vef/33tV/88/v3ju3vrs+OMbble9v3dte2d2ae33x7pULf08uH1jYWn803F+Ov3n+9urGgy"+
        "vjbx659TnMjv4AAOA0fDM5nsD15c313Y3xZ+v25Nq+qb9+Pz8YDH+8OXj75/Sos2//9BnrGQAATtGxO9Bri7NLj/eXO/dXhy/mj9+Bfj5zY3"+
        "56fLrzdO7aby/Hd4/64f2ztXeXJif/ATvQAACclpONcEwu/Kup/Wi+vrz55OqFG/NTk9r+nFMcAhoAgNNyohGOtcXZG+f2PzNrz2cO1rO/Ph"+
        "9F9szBlvP5pw+nBwvv7lzZPwUAgDPokwE9vTR38c3ouLC1vnq4ozxcORiDfrN5fnwcDDanX7wcrhzuOj8+7OyZtcklAAD44n16B/r6z8PLo8"+
        "MoiOen96/sVfW5mT/Gq8vXhveXx9F87f2To8Meo+A++MXhRydAAADgS3JcQE89nZu9dW/v2Rq/LG3PPdpP4Y2Vheml+cHKpIw3HtwcXFraun"+
        "N18j8AAHDmHRfQ23fWNp4t7+wtX1+8ezD0PH4ux/TSwemNxcM9aQAA+Cqc4EeE3717Mp7EOPLs5537r8ab0B95zsaHGei93xoCAMDZcIKAHt"+
        "l78sbM6+XNlYXRyXDl1fDFlU9l8YcZ6L1JDwAAOBuODei1xfEM9MJwbvyelNurG0+Wtif39vakN799aIQDAICvzsdepPK/5UUqAACclpONcA"+
        "AAAGMCGgAAgnO7u7uTJQAA8Cl2oAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAA"+
        "ACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ"+
        "0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAI"+
        "GABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABg"+
        "CAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQE"+
        "ADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAE"+
        "AgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoA"+
        "EAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIB"+
        "DQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAA"+
        "AQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCG"+
        "gAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAA"+
        "gENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENA"+
        "AABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABA"+
        "IaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAA"+
        "ACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ"+
        "0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAI"+
        "GABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABg"+
        "CAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQE"+
        "ADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAE"+
        "AgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoA"+
        "EAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIB"+
        "DQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAA"+
        "AQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCG"+
        "gAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAA"+
        "gENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENA"+
        "AABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABA"+
        "IaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAA"+
        "ACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ"+
        "0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAI"+
        "GABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABg"+
        "CAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQE"+
        "ADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAAnNhg8A/kgOxh0ol1ZgAAAABJRU"+
        "5ErkJggg=="
        osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL, (err, info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            var localId = info.localId;
            osAccountManager.setOsAccountProfilePhoto(localId, photo, (err)=>{
                console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
                osAccountManager.removeOsAccount(localId, (err)=>{
                    console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsOsAccountLimit_0900 end====");
                    done();
                })
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLimit_1000
     * @tc.name    : setOsAccountProfilePhoto promise
     * @tc.desc    : set the length of the profile photo to exceed the limit of 4096 characters
     */
    it('ActsOsAccountLimit_1000', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_1000 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var photo = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAA8AAAAIcCAIAAAC2P1AsAAAAAXNSR0IArs4c6QAAAARnQU1BAA"+
        "Cxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAx6SURBVHhe7d0hbFR3HMDxY4rKSkggWTMQLMEUtYppqgZiCZYlrSwJDk3mlrQSBJZkYq"+
        "gVgV1RrZmogVRAAvJk67rr9VoqVug3Gekon08vef/33tV/88/v3ju3vrs+OMbble9v3dte2d2ae33x7pULf08uH1jYWn803F+Ov3n+9urGgy"+
        "vjbx659TnMjv4AAOA0fDM5nsD15c313Y3xZ+v25Nq+qb9+Pz8YDH+8OXj75/Sos2//9BnrGQAATtGxO9Bri7NLj/eXO/dXhy/mj9+Bfj5zY3"+
        "56fLrzdO7aby/Hd4/64f2ztXeXJif/ATvQAACclpONcEwu/Kup/Wi+vrz55OqFG/NTk9r+nFMcAhoAgNNyohGOtcXZG+f2PzNrz2cO1rO/Ph"+
        "9F9szBlvP5pw+nBwvv7lzZPwUAgDPokwE9vTR38c3ouLC1vnq4ozxcORiDfrN5fnwcDDanX7wcrhzuOj8+7OyZtcklAAD44n16B/r6z8PLo8"+
        "MoiOen96/sVfW5mT/Gq8vXhveXx9F87f2To8Meo+A++MXhRydAAADgS3JcQE89nZu9dW/v2Rq/LG3PPdpP4Y2Vheml+cHKpIw3HtwcXFraun"+
        "N18j8AAHDmHRfQ23fWNp4t7+wtX1+8ezD0PH4ux/TSwemNxcM9aQAA+Cqc4EeE3717Mp7EOPLs5537r8ab0B95zsaHGei93xoCAMDZcIKAHt"+
        "l78sbM6+XNlYXRyXDl1fDFlU9l8YcZ6L1JDwAAOBuODei1xfEM9MJwbvyelNurG0+Wtif39vakN799aIQDAICvzsdepPK/5UUqAACclpONcA"+
        "AAAGMCGgAAgnO7u7uTJQAA8Cl2oAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAA"+
        "ACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ"+
        "0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAI"+
        "GABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABg"+
        "CAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQE"+
        "ADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAE"+
        "AgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoA"+
        "EAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIB"+
        "DQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAA"+
        "AQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCG"+
        "gAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAA"+
        "gENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENA"+
        "AABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABA"+
        "IaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAA"+
        "ACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ"+
        "0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAI"+
        "GABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABg"+
        "CAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQE"+
        "ADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAE"+
        "AgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoA"+
        "EAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIB"+
        "DQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAA"+
        "AQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCG"+
        "gAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAA"+
        "gENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENA"+
        "AABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABA"+
        "IaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAA"+
        "ACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ"+
        "0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAI"+
        "GABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABg"+
        "CAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQE"+
        "ADAEAgoAEAIBDQAAAQCGgAAAgENAAABAIaAAACAQ0AAIGABgCAQEADAEAgoAEAIBDQAAAQCGgAAAgENAAAnNhg8A/kgOxh0ol1ZgAAAABJRU"+
        "5ErkJggg=="
        var info = await osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" +JSON.stringify(info));
        expect(info.localName).assertEqual("aaa");
        var localId = info.localId;
        try{
            await osAccountManager.setOsAccountProfilePhoto(localId, photo);
        }
        catch(err){
            console.debug("====>setOsAccountProfilePhoto err:" +JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            await osAccountManager.removeOsAccount(localId);
            console.debug("====>ActsOsAccountLimit_1000 end====");
            done();
        }
    });

    /*
     * @tc.number  : ActsOsAccountLimit_1100
     * @tc.name    : setOsAccountProfilePhoto callback
     * @tc.desc    : set the length of the profile photo to exceed the limit of 4096 characters
     */
    it('ActsOsAccountLimit_1100', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_1100 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var photo = "data:image/jpeg;base64,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"
        osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL, (err, info)=>{
            console.debug("====>createOsAccount err:" + JSON.stringify(err));
            console.debug("====>createOsAccount info:" + JSON.stringify(info));
            expect(err.code).assertEqual(0);
            expect(info.localName).assertEqual("aaa");
            var localId = info.localId;
            osAccountManager.setOsAccountProfilePhoto(localId, photo, (err)=>{
                console.debug("====>setOsAccountProfilePhoto err:" + JSON.stringify(err));
                expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
                osAccountManager.removeOsAccount(localId, (err)=>{
                    console.debug("====>removeOsAccount err:" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>ActsOsAccountLimit_1100 end====");
                    done();
                })
            })
        })
    });

    /*
     * @tc.number  : ActsOsAccountLimit_1200
     * @tc.name    : setOsAccountProfilePhoto promise
     * @tc.desc    : set the length of the profile photo to exceed the limit of 4096 characters
     */
    it('ActsOsAccountLimit_1200', 0, async function (done) {
        console.debug("====>ActsOsAccountLimit_1200 start====");
        var osAccountManager = osaccount.getAccountManager();
        console.debug("====>get AccountManager finish====");
        var photo = "data:image/jpeg;base64,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"
        var info = await osAccountManager.createOsAccount("aaa", osaccount.OsAccountType.NORMAL);
        console.debug("====>createOsAccount info:" +JSON.stringify(info));
        expect(info.localName).assertEqual("aaa");
        var localId = info.localId;
        try{
            await osAccountManager.setOsAccountProfilePhoto(localId, photo);
        }
        catch(err){
            console.debug("====>setOsAccountProfilePhoto err:" +JSON.stringify(err));
            expect(err.code).assertEqual(ERR_OSACCOUNT_KIT_SET_OS_ACCOUNT_PROFILE_PHOTO_ERROR);
            await osAccountManager.removeOsAccount(localId);
            console.debug("====>ActsOsAccountLimit_1200 end====");
            done();
        }
    });
})