// @ts-nocheck
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index"
import missionManager  from '@ohos.application.missionManager';
//import ability_featureAbility from '@ohos.ability.featureAbility';

describe('ShiAbilityDisableTest', function () {
    var result
    console.log("ShiAbilityDisableTest --- start");

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4000
     * @tc.name    The interface parameter local id and is 1
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_4000',0, async function (done){
         missionManager.getMissionInfos("", 1, (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_4000 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code == 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4100
     * @tc.name    Interface parameter does not exist and 1
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_4100', 0, async function (done) {
         missionManager.getMissionInfos("aaaaaa", 1, (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_4100 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4200
     * @tc.name    Interface parameter null and 1
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_4200', 0, async function (done) {
        missionManager.getMissionInfos('', 1, (err, data) => {
            console.log('SUB_AA_OpenHarmony_MissionManager_4200 AsyncCallback errCode : '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4300
     * @tc.name    Interface parameter null and 10
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_4300', 0, async function (done) {
        missionManager.getMissionInfos('', 10, (err, data) => {
            console.log('SUB_AA_OpenHarmony_MissionManager_4300 AsyncCallback errCode : '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4400
     * @tc.name    Interface parameter null and 1000
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_4400', 0, async function (done){
        missionManager.getMissionInfos('', 1000, (err, data) => {
            console.log('SUB_AA_OpenHarmony_MissionManager_4400 AsyncCallback errCode : '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4500
     * @tc.name    Interface parameter null and null
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_4500', 0, async function (done){
         missionManager.getMissionInfos("", "", (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_4500 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4600
     * @tc.name    Interface parameter null and string 1
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_4600', 0, async function (done){
         missionManager.getMissionInfos("", "1", (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_4600 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4700
     * @tc.name    Interface parameter null and floating point number
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_4700', 0, async function (done){
        missionManager.getMissionInfos('', 2.0, (err, data) => {
            console.log('SUB_AA_OpenHarmony_MissionManager_4700 AsyncCallback errCode : '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            done();
        })
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4800
     * @tc.name    Interface parameter null and null
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_4800', 0, async function (done){
         missionManager.getMissionInfos("", "", (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_4800 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_4900
     * @tc.name    Interface parameter does not exist and null
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_4900', 0, async function (done){
         missionManager.getMissionInfos("bbbb", "", (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_4900 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5000
     * @tc.name    Interface parameter does not exist and 1
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_5000', 0, async function (done){
         missionManager.getMissionInfos("", 1, (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_5000 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5100
     * @tc.name    Interface parameter null and null
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_5100', 0, async function (done){
         missionManager.getMissionInfos("", "", (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_5100 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5200
     * @tc.name    Interface parameter null and id
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_5200', 0, async function (done){
         var id = 65024;
         missionManager.getMissionInfos("", id, (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_5200 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5300
     * @tc.name    Interface parameter null and null
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_5300', 0, async function (done) {
         missionManager.getMissionInfo('', '', (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_5300 AsyncCallback errCode ##: '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5400
     * @tc.name    Interface parameter null and string
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_5400', 0, async function (done) {
         missionManager.getMissionInfo('', "aa", (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_5400 AsyncCallback errCode ##: '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5500
     * @tc.name    Interface parameter null and float
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_5500', 0, async function (done){
         missionManager.getMissionInfos("", 2.0, (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_5500 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5600
     * @tc.name    Interface parameter undefined
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_5600', 0, async function (done) {
        let object = undefined
        var result = missionManager.registerMissionListener(object);
        console.log('SUB_AA_OpenHarmony_MissionManager_5600 result ###' + JSON.stringify(result));
        expect(result != undefined).assertTrue();
        done();
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5700
     * @tc.name    Interface parameter Empty object
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_5700', 0, async function (done) {
        let object = {}
        var result = missionManager.registerMissionListener(object);
        console.log('SUB_AA_OpenHarmony_MissionManager_5700 result ### ' + JSON.stringify(object));
        console.log('SUB_AA_OpenHarmony_MissionManager_5700 result ### ' + JSON.stringify(result));
        expect(result != undefined).assertTrue();
        done();
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5800
     * @tc.name    Multiple registration listening
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_5800', 0, async function (done){
        for(let i = 0; i< 5; i++){
            result = missionManager.registerMissionListener({
                onMissionCreated:function onMissionCreated(number){
                    console.log("SUB_onForegroundApplicationChanged")
                },
                onMissionDestroyed:function onMissionDestroyed(number){
                    console.log("SUB_onAbilityStateChanged")
                },
                onMissionSnapshotChanged:function onMissionSnapshotChanged(number){
                    console.log("SUB_onProcessCreated")
                },
                onMissionMovedToFront:function onMissionMovedToFront(number){
                    console.log("SUB_onProcessDied")
                },
            });
            console.log('SUB_AA_OpenHarmony_MissionManager_5800 result ### ' + JSON.stringify(result));
            expect(result != undefined).assertTrue();
            done();
        }
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_5900
     * @tc.name    Interface parameter non-existent
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_5900', 0, async function (done) {
        console.log("---------start SUB_AA_OpenHarmony_MissionManager_5900------------")
        var id = 44642;
        missionManager.unregisterMissionListener(id, (err, data) => {
            console.log('SUB_AA_OpenHarmony_MissionManager_5900 AsyncCallback errCode : '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            done();
        })
        console.log("---------start SUB_AA_OpenHarmony_MissionManager_5900------------")
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_6000
     * @tc.name    Interface parameter Registered ID
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_6000', 0, async function (done){
         missionManager.unregisterMissionListener(result, (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_6000 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code == 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_6100
     * @tc.name    Interface parameter Unregistered ID
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_6100', 0, async function (done){
         missionManager.unregisterMissionListener(result, (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_6100 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code == 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_6200
     * @tc.name    Interface parameter undefined
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_6200', 0, async function (done){
         let listenerId = undefined
         missionManager.unregisterMissionListener(listenerId, (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_6200 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_6300
     * @tc.name    Interface parameter null
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_6300', 0, async function (done) {
         missionManager.unregisterMissionListener(null, (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_6300 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_6400
     * @tc.name    Interface parameter string
     * @tc.desc    Function test
     * @tc.level   0
     */
     it('SUB_AA_OpenHarmony_MissionManager_6400', 0, async function (done){
         missionManager.unregisterMissionListener('cc', (err, data) => {
             console.log('SUB_AA_OpenHarmony_MissionManager_6400 AsyncCallback errCode : '
             + JSON.stringify(err) + " data: " + JSON.stringify(data));
             expect(err.code != 0).assertTrue();
             done();
         })
     })
     
    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_6500
     * @tc.name    Interface parameter float
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_6500', 0, async function (done){
        missionManager.unregisterMissionListener(2.0, (err, data) => {
            console.log('SUB_AA_OpenHarmony_MissionManager_6500 AsyncCallback errCode123123 : '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            done();
        })
    })
})