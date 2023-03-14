/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import missionManager from "@ohos.application.missionManager";

describe('missionManager', function () {
    console.info("----------------logMessage SUB_AA_OpenHarmony_Lock-------------------");

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_1500
     * @tc.name    Locking function abnormal parameter_-1
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_1500', 0, async function (done) {
        console.info("-------------logMessage SUB_AA_OpenHarmony_Lock_1500---------")
        missionManager.lockMission(-1, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_1500 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code != 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_1800
     * @tc.name    Lock function abnormal parameter _number decimal 0.2
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_1800', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_Lock_1800-------------");
        var missionId = 0.2;
        setTimeout(() => {
            missionManager.lockMission(missionId).then((data) => {
                console.log("SUB_AA_OpenHarmony_Lock_1800 lockMission data: " +
                JSON.stringify(data));
            }).catch((error) => {
                console.log("SUB_AA_OpenHarmony_Lock_1800 lockMission error: " +
                JSON.stringify(error));
                expect(error.code != 0).assertTrue();
                done();
            })
       }, 1000)
        console.log("------------end SUB_AA_OpenHarmony_Lock_1800-------------");
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_1900
     * @tc.name    Lock function abnormal parameter _number decimal 1.2
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_1900', 0, async function (done) {
        console.info("------------------logMessage SUB_AA_OpenHarmony_Lock_1900-------------------")
        missionManager.lockMission(1.2, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_1900 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(data == undefined).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_1700
     * @tc.name    Lock function exception parameter _string type is not a number
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_1700', 0, async function (done) {
        console.info("------------logMessage SUB_AA_OpenHarmony_Lock_1700-----------")
        missionManager.lockMission("A", (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_1700 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code != 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_1600
     * @tc.name    Lock function exception parameter _string type number
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_1600', 0, async function (done) {
        console.info("---------------logMessage SUB_AA_OpenHarmony_Lock_1600------------")
        missionManager.lockMission("12", (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_1600 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code != 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_1400
     * @tc.name    Lock function exception parameter _undefined
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_1400', 0, async function (done) {
        console.info("---------------logMessage SUB_AA_OpenHarmony_Lock_1400-------------")
        missionManager.lockMission(undefined, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_1400 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code != 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_2200
     * @tc.name    Unlock function abnormal parameter_-1
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_2200', 0, async function (done) {
        console.info("------------------logMessage SUB_AA_OpenHarmony_Lock_2200-------------")
        missionManager.unlockMission(-1, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_2200 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code != 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_2500
     * @tc.name    Unlock function abnormal parameter _number decimal 0.2
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_2500', 0, async function (done) {
        console.info("------------------logMessage SUB_AA_OpenHarmony_Lock_2500--------------")
        missionManager.unlockMission(0.2, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_2500 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code != 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_2400
     * @tc.name    Unlock function exception parameter _string type non-numeric
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_2400', 0, async function (done) {
        console.info("------------------logMessage SUB_AA_OpenHarmony_Lock_2400-----------")
        missionManager.unlockMission("a", (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_2400 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code != 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_2300
     * @tc.name    Unlock function exception parameter _string type number
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_2300', 0, async function (done) {
        console.info("------------------logMessage SUB_AA_OpenHarmony_Lock_2300--------------")
        missionManager.unlockMission("123", (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_2300 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code != 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_2100
     * @tc.name    Unlock function exception parameter _undefined
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_2100', 0, async function (done) {
        console.info("------------------logMessage SUB_AA_OpenHarmony_Lock_2100--------------")
        missionManager.unlockMission(undefined, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_2100 AsyncCallback errCode : ' +
            JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code != 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_2600
     * @tc.name    Unlock function abnormal parameter _number decimal 1.2
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_2600', 0, async function (done) {
        console.log("------------SUB_AA_OpenHarmony_Lock_2600-------------");
        var missionId = 1.2;
        setTimeout(() => {
            missionManager.unlockMission(missionId).then((data) => {
                console.log("SUB_AA_OpenHarmony_Lock_2600 unlockMission data: " +
                JSON.stringify(data));

            }).catch((error) => {
                console.log("SUB_AA_OpenHarmony_Lock_2600 unlockMission error: " +
                JSON.stringify(error));
                expect(error.code != 0).assertTrue();
                done();
            })
        }, 1000)
        console.log("------------end SUB_AA_OpenHarmony_Lock_2600-------------");
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_2000
     * @tc.name    Repeat lock
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_2000',0,async function (done){
        console.log("------------start SUB_AA_OpenHarmony_Lock_2000-------------");
        for(var i=0;i<2;i++){
            missionManager.lockMission(1.2, (error, data) => {
                console.log("SUB_AA_OpenHarmony_Lock_0900, twosgetMissionInfos:"  +
                JSON.stringify(error) + ", " + JSON.stringify(data));
                expect(data == undefined).assertTrue();
            })
        }
        done();
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_Lock_2700
     * @tc.name    Repeat unlock
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_Lock_2700',0,async function (done){
        var missionId = 1.2
        console.log("------------start SUB_AA_OpenHarmony_Lock_2700-------------");
        missionManager.lockMission(missionId, (error, data) => {
            console.log("lockMission info" + JSON.stringify(data));
        })
        missionManager.unlockMission(missionId,(error, data) => {
            for(var i=0;i<2;i++){
                missionManager.unlockMission(missionId,(error, data1) => {
                    console.log("unlockMission info" + JSON.stringify(data1));
                    expect(data1 == undefined);
                })
            }
        })
        done();
    })
})
