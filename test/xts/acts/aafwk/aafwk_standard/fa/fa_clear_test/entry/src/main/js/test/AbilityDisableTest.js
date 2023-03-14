
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
// @ts-nocheck
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index"
import missionManager  from '@ohos.application.missionManager';
import abilityFeatureAbility from '@ohos.ability.featureAbility';
import bundleManager from '@ohos.bundle';

describe('AbilityDisableTests', function () {
    console.log("AbilityDisableTests --- start");

    function fns(){
        setTimeout(()=>{
            abilityFeatureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility2"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 - startMainAbility2: '
                + JSON.stringify(error) + ", " + JSON.stringify(data))
                expect(err.code == 0).assertTrue();
            });
        },500)
        setTimeout(()=>{
            abilityFeatureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility4"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 - startMainAbility4: '
                + JSON.stringify(error) + ", " + JSON.stringify(data))
                expect(err.code == 0).assertTrue();
            });
        },1000)
        setTimeout(()=>{
            abilityFeatureAbility.startAbility({ want: {
                bundleName: "com.example.abilitydisable",
                abilityName: "com.example.abilitydisable.MainAbility3"
            } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 - startMainAbility3:  '
                + JSON.stringify(error) + ", " + JSON.stringify(data))
                expect(err.code == 0).assertTrue();
            });
        },1500)
        setTimeout(()=>{
            missionManager.getMissionInfos("", 20, (err, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 getMissionInfos errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                for (var i = 0;i < data.length; i++) {
                    console.log("bundleName："+data[i].want["abilityName"])
                    if(data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility3"
                    || data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility4"
                    ||data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility"
                    ||data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility2"){
                        console.log("data[i]['lockedState']::::"+data[i]['lockedState']);
                        console.log("data[i].missionId" + data[i].missionId);
                        console.log("SUB_AA_OpenHarmony_Clear_0100, missionId:" + data[i].missionId);
                        missionIds.push(data[i].missionId);
                    }
                }
                console.log('SUB_AA_OpenHarmony_Clear_0100 missionId：'+ missionIds +
                typeof missionIds + ":::::"+missionIds[1] + typeof missionIds[1]);
            })
            console.log("SUB_AA_OpenHarmony_Clear_0100" + JSON.stringify(missionIds));
        },2000)
        setTimeout(()=>{
            missionManager.lockMission(parseInt(missionIds[1]),(err,data)=>{

                console.log('SUB_AA_OpenHarmony_Clear_0100 lockMission1 missionIds[1] : ' +
                JSON.stringify(missionIds[1]), + parseInt(missionIds[1]));

                console.log('SUB_AA_OpenHarmony_Clear_0100 lockMission1 errCode : ' +
                JSON.stringify(err) + " data: " + JSON.stringify(data));
            })
        },2500)
        setTimeout(()=>{
            missionManager.lockMission(parseInt(missionIds[2]),(err,data)=>{
                console.log('SUB_AA_OpenHarmony_Clear_0100 lockMission2 errCode : ' +
                JSON.stringify(err) + " data: " + JSON.stringify(data));
            })
        },2500)
        setTimeout(()=>{
            missionManager.getMissionInfos("", 20, (err, data) => {
                console.log("SUB_AA_OpenHarmony_Clear_0100, twosgetMissionInfos:" +
                JSON.stringify(data));
                for (var i = 0;i < data.length; i++) {
                    console.log("data[i].missionId：missionIds[2] news missionIds[1]" +
                    data[i].missionId + missionIds[2] + missionIds[1])
                    if(data[i].missionId == missionIds[1] || data[i].missionId == missionIds[2]){
                        console.log("data[i]['lockedState']222:::::::"+data[i]['lockedState']);
                        expect(data[i]['lockedState'] == true).assertTrue();
                    }
                }
            })
            console.log("SUB_AA_OpenHarmony_Clear_0100" + JSON.stringify(missionIds));
        },3100)
        setTimeout(()=>{
            missionManager.getMissionInfos("", 20, (err, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 getMissionInfos errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                for (var i = 0;i < data.length; i++) {
                    console.log("bundleName："+data[i].want["abilityName"])
                    if(data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility3"
                    || data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility4"
                    ||data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility"
                    ||data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility2"){
                        console.log("data[i]['lockedState']::::"+data[i]['lockedState']);
                        expect(data[i].lockedState == false).assertTrue();
                    }
                }
                console.log('SUB_AA_OpenHarmony_Clear_0100 missionId：'+ missionIds +
                typeof missionIds + ":::::"+missionIds[1] + typeof missionIds[1]);
            })
            console.log("SUB_AA_OpenHarmony_Clear_0100" + JSON.stringify(missionIds));
        },5000)
        setTimeout(()=>{
            missionManager.moveMissionToFront(parseInt(missionIds[2]),(err,data)=>{
                console.log('SUB_AA_OpenHarmony_Clear_0100 moveMissionToFront errCode : ' +
                JSON.stringify(err) + " data: " + JSON.stringify(data));
            })
        },6000)
        setTimeout(()=>{
            missionManager.clearAllMissions((err,data)=>{
                console.log('SUB_AA_OpenHarmony_Clear_0100 clearAllMissions errCode: ' +
                JSON.stringify(err) + " data: " + JSON.stringify(data));

            })
        },7000)
        setTimeout(()=>{
            missionManager.getMissionInfos("", 20, (err, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 getMissionInfos errCode : ' +
                JSON.stringify(err) + " data: " + JSON.stringify(data));
                for (var i = 0;i < data.length; i++) {
                    console.log("bundleName："+data[i].want["abilityName"])
                    if(data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility3" ||
                    data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility4" ||
                    data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility"||
                    data[i].want["abilityName"] == "com.example.abilitydisable.MainAbility2"){
                        console.log("data[i]['lockedState']end::::"+data[i]['lockedState']);
                        expect(data[i]['lockedState'] == true).assertTrue();
                    }
                }
                console.log('SUB_AA_OpenHarmony_Clear_0100 missionId：'+
                missionIds + typeof missionIds + ":::::"+missionIds[1] + typeof missionIds[1]);
            })
            console.log("SUB_AA_OpenHarmony_Clear_0100" +
            JSON.stringify(missionIds));
        },8000)
    }

        /*
         * @tc.number  SUB_AA_OpenHarmony_Clear_1200
         * @tc.name    missionId for undefined
         * @tc.desc    Function test
         * @tc.level   0
         */
        it("SUB_AA_OpenHarmony_Clear_1200", 0, async function (done) {
            console.info("------------------logMessage SUB_AA_OpenHarmony_Clear_1200-------------------");
            missionManager.clearMission(undefined, (data, err) => {
                console.log('SUB_AA_OpenHarmony_Clear_1200 asyncCallback errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                expect(data["code"]==-1).assertTrue();
            })
            done();
        });
        
        /*
         * @tc.number  SUB_AA_OpenHarmony_Clear_1300
         * @tc.name    MissionId: does not exist
         * @tc.desc    Function test
         * @tc.level   0
         */
        it("SUB_AA_OpenHarmony_Clear_1300", 0, async function (done) {
            console.info("------------------logMessage SUB_AA_OpenHarmony_Clear_1300-------------------");
            missionManager.clearMission("xxxx", (data, err) => {
                console.log('SUB_AA_OpenHarmony_Clear_1300 asyncCallback errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                expect(data["code"]==-1).assertTrue();
            })
            done();
        });

        /*
         * @tc.number  SUB_AA_OpenHarmony_Clear_1400
         * @tc.name    MissionId is a string number
         * @tc.desc    Function test
         * @tc.level   0
         */
        it("SUB_AA_OpenHarmony_Clear_1400", 0, async function (done) {
            console.info("------------------logMessage SUB_AA_OpenHarmony_Clear_1400-------------------");
            missionManager.clearMission("123456", (data, err) => {
                console.log('SUB_AA_OpenHarmony_Clear_1400 asyncCallback errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                expect(data["code"]==-1).assertTrue();
            })
            done();
        });

        /*
         * @tc.number  SUB_AA_OpenHarmony_Clear_1500
         * @tc.name    The missionId is a non-number string
         * @tc.desc    Function test
         * @tc.level   0
         */
        it("SUB_AA_OpenHarmony_Clear_1500", 0, async function (done) {
            console.info("------------------logMessage SUB_AA_OpenHarmony_Clear_1500-------------------");
            missionManager.clearMission("abc@@#$", (data, err) => {
                console.log('SUB_AA_OpenHarmony_Clear_1500 asyncCallback errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                expect(data["code"]==-1).assertTrue();
            })
            done();
        });

        /*
         * @tc.number  SUB_AA_OpenHarmony_Clear_1600
         * @tc.name    MissionId is an existing floating point type
         * @tc.desc    Function test
         * @tc.level   0
         */
        it("SUB_AA_OpenHarmony_Clear_1600", 0, async function (done) {
            console.info("------------------logMessage SUB_AA_OpenHarmony_Clear_1600-------------------");
            missionManager.clearMission(1.1234, (data, err) => {
                console.log('SUB_AA_OpenHarmony_Clear_1600 asyncCallback errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                expect(data["code"]==-1).assertTrue();
            })
            done();
        });

        /*
         * @tc.number  SUB_AA_OpenHarmony_Clear_0100
         * @tc.name    Locking is terminated after mission, switching to the front desk for cleanup
         * @tc.desc    Function test
         * @tc.level   0
         */
        it('SUB_AA_OpenHarmony_Clear_0100', 0, async function (done) {
            var missionIds = [];
            console.log("SUB_AA_OpenHarmony_Clear_0100 --- start")
            fns()
            done()
        });

        
})