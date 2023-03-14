// @ts-nocheck
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
import {describe, expect, it} from 'deccjsunit/index'
import missionManager from '@ohos.application.missionManager'
import featureAbility from '@ohos.ability.featureability'

describe("taskSwitchTest", function() {
    console.log("------------------ taskSwitchTest is starting ------------------");

    var TAG = "";

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_0100
     * @tc.name    Switching the mission in the launcherlist to the foreground is not supported
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_0100', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MoveMissionToFront_0100-------------");
        var missionId = 1;
        try {
            missionManager.moveMissionToFront(missionId, (error, data) => {
                console.log("SUB_AA_OpenHarmony_MoveMissionToFront_0100 error: " +
                JSON.stringify(error) + ", data: " + JSON.stringify(data));
                expect(error.code == 2097181).assertTrue();
                done();
            })
        } catch(error) {
            console.log("SUB_AA_OpenHarmony_MoveMissionToFront_0100: error " + error);
        }
        console.log("------------end SUB_AA_OpenHarmony_MoveMissionToFront_0100-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_2400
     * @tc.name    The input parameter missionid of movemissiontorfront is undefined
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_2400', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MoveMissionToFront_2400-------------");
        var missionId = undefined;
        try {
            missionManager.moveMissionToFront(missionId, (error, data) => {
                console.log("SUB_AA_OpenHarmony_MoveMissionToFront_2400 error: " +
                JSON.stringify(error) + ", data: " + JSON.stringify(data));
                expect(error.code == -1).assertTrue();
                done();
            })
        } catch(error) {
            console.log("SUB_AA_OpenHarmony_MoveMissionToFront_2400: error " + error);
        }
        console.log("------------end SUB_AA_OpenHarmony_MoveMissionToFront_2400-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_2500
     * @tc.name    The input parameter missionid of movemissiontorfront is an ID that does not exist
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_2500', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MoveMissionToFront_2500-------------");
        var missionId = -1;
        try {
            missionManager.moveMissionToFront(missionId, (error, data) => {
                console.log("SUB_AA_OpenHarmony_MoveMissionToFront_2500 error: " +
                JSON.stringify(error) + ", data: " + JSON.stringify(data));
                expect(error.code == 2097181).assertTrue();
                done();
            })
        } catch(error) {
            console.log("SUB_AA_OpenHarmony_MoveMissionToFront_2500: error " + error);
        }
        console.log("------------end SUB_AA_OpenHarmony_MoveMissionToFront_2500-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_2600
     * @tc.name    The input parameter missionid of movemissiontorfront is a number of type string
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_2600', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MoveMissionToFront_2600-------------");
        var missionId = "2";
        try {
            missionManager.moveMissionToFront(missionId, (error, data) => {
                console.log("SUB_AA_OpenHarmony_MoveMissionToFront_2600 error: " +
                JSON.stringify(error) + ", data: " + JSON.stringify(data));
                expect(error.code == -1).assertTrue();
                done();
            })
        } catch(error) {
            console.log("SUB_AA_OpenHarmony_MoveMissionToFront_2600: error " + error);
        }
        console.log("------------end SUB_AA_OpenHarmony_MoveMissionToFront_2600-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_2700
     * @tc.name    The input parameter missionid of movemissiontorfront is a non numeric string
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_2700', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MoveMissionToFront_2700-------------");
        var missionId = "aa";
        try {
            missionManager.moveMissionToFront(missionId, (error, data) => {
                console.log("SUB_AA_OpenHarmony_MoveMissionToFront_2700 error: " +
                JSON.stringify(error) + ", data: " + JSON.stringify(data));
                expect(error.code == -1).assertTrue();
                done();
            })
        } catch(error) {
            console.log("SUB_AA_OpenHarmony_MoveMissionToFront_2700: error " + error);
        }
        console.log("------------end SUB_AA_OpenHarmony_MoveMissionToFront_2700-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_1900
     * @tc.name    Switching the mission in the launcherlist to the foreground is not supported
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_1900', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MoveMissionToFront_1900-------------");
        TAG = "SUB_AA_OpenHarmony_MoveMissionToFront_1900"
        var abilityMissionList = []
        var missionid = "";
        var result = missionManager.registerMissionListener({
            onMissionCreated: (missionid) => {
                console.log(TAG + "onMissionCreated callback")
            },
            onMissionDestroyed: (missionid) => {
                console.log(TAG + "onMissionDestroyed callback")
            }
        })

        setTimeout(function(){
            featureAbility.startAbility({
                want: {
                    bundleName: "com.example.taskswitching",
                    abilityName: "com.example.taskswitching.MainAbility2"
                }
            }, (error, data) => {
                console.log(TAG + " startAbility success, error: "
                + JSON.stringify(error) + ", data: " + JSON.stringify(data));
            });
        }, 2000)

        setTimeout(function(){
            missionManager.getMissionInfos("", 20, (error, data) => {
                console.log(TAG + "Emtry MainAbility getMissionInfos success, error: "
                + JSON.stringify(error) + ", data: " + JSON.stringify(data));
                for(var i = 0; i<data.length; i++){
                    if (data[i].want.abilityName == "com.example.taskswitching.MainAbility2") {
                        abilityMissionList.push(data[i].missionId);
                    }
                }
                console.log(TAG + "abilityMissionList is: " + JSON.stringify(abilityMissionList))
            });
            missionManager.clearMission(parseInt(abilityMissionList[0]), (err, data) => {
                console.log(TAG + "Emtry MainAbility clearAllMissions success, error: "
                + JSON.stringify(error) + ", data: " + JSON.stringify(data));
            })
        },3000)

        setTimeout(function(){
            console.log(TAG + "result is : " + result)
            expect(result != "").assertTrue();
            done()
        }, 4000);
        console.log("------------end SUB_AA_OpenHarmony_MoveMissionToFront_1900-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_2000
     * @tc.name    Switching the mission in the launcherlist to the foreground is not supported
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_2000', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MoveMissionToFront_2000-------------");
        TAG = "SUB_AA_OpenHarmony_MoveMissionToFront_2000"
        var missionid = "";

        var result = missionManager.registerMissionListener({
            onMissionSnapshotChanged: (missionid) => {
                console.log(TAG + "onMissionSnaptShotChanged callback")
            }
        })

        setTimeout(function() {
            featureAbility.startAbility({
                want: {
                    bundleName: "com.example.taskswitching",
                    abilityName: "com.example.taskswitching.MainAbility2"
                }
            }, (error, data) => {
                console.log(TAG + " startAbility success, error: "
                + JSON.stringify(error) + ", data: " + JSON.stringify(data));
            });
        }, 3000)

        setTimeout(function() {
            console.log(TAG + "result is : " + result)
            expect(result != "").assertTrue();
            done()
        }, 4000)
        console.log("------------end SUB_AA_OpenHarmony_MoveMissionToFront_2000-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_2100
     * @tc.name    Switching the mission in the launcherlist to the foreground is not supported
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_2100', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MoveMissionToFront_2100-------------");
        TAG = "SUB_AA_OpenHarmony_MoveMissionToFront_2100"
        var abilityMissionList =[];
        var missionid = "";

        var result = missionManager.registerMissionListener({
            onMissionMovedToFront: (missionid) => {
                console.log(TAG + "onMissionMovedToFront callback")
            }
        })

        setTimeout(function() {
            featureAbility.startAbility({
                want: {
                    bundleName: "com.example.taskswitching",
                    abilityName: "com.example.taskswitching.MainAbility2"
                }
            }, (error, data) => {
                console.log(TAG + " startAbility success, error: "
                + JSON.stringify(error) + ", data: " + JSON.stringify(data));
            });
        }, 2000)


        setTimeout(function(){
            missionManager.getMissionInfos("", 20, (error, data) => {
                console.log(TAG + "Emtry MainAbility getMissionInfos success, error: "
                + JSON.stringify(error) + ", data: " + JSON.stringify(data));
                for(var i = 0; i<data.length; i++){
                    if (data[i].want.abilityName == "com.example.taskswitching.MainAbility2") {
                        abilityMissionList.push(data[i].missionId);
                    }
                }
                console.log(TAG + "abilityMissionList is: " + JSON.stringify(abilityMissionList))
            });
        }, 3000)

        setTimeout(function() {
            console.log(TAG + "MainAbility abilityMissionList data " + parseInt(abilityMissionList[0]));
            missionManager.moveMissionToFront(parseInt(abilityMissionList[0]), (error, data) => {
                console.log(TAG + "MainAbility moveMissionToFront success, error: "
                + JSON.stringify(error) + ", data: " + JSON.stringify(data));
            });
        }, 4000)

        setTimeout(function() {
            console.log(TAG + "result is : " + result)
            expect(result != "").assertTrue();
            done()
        }, 4500)
        console.log("------------end SUB_AA_OpenHarmony_MoveMissionToFront_2100-------------");
    });


    /*
     * @tc.number  SUB_AA_OpenHarmony_MoveMissionToFront_2900
     * @tc.name    Switching the mission in the launcherlist to the foreground is not supported
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MoveMissionToFront_2900', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MoveMissionToFront_2900-------------");
        TAG = "SUB_AA_OpenHarmony_MoveMissionToFront_2900"
        var abilityMissionList = [];
        setTimeout(function(){
            featureAbility.startAbility({
                want: {
                    bundleName : "com.example.taskswitching",
                    abilityName : "com.example.taskswitching.MainAbility2"
                }
            }, (error, data) => {
                console.log(TAG + " startAbility success, error: "
                + JSON.stringify(error) + ", data: " + JSON.stringify(data));
            });
        }, 1000);

        setTimeout(function(){
            missionManager.getMissionInfos("", 20, (error, data) => {
                console.log(TAG + "Emtry MainAbility2 getMissionInfos success, error: "
                + JSON.stringify(error) + ", data: " + JSON.stringify(data));

                for(var i = 0; i<data.length; i++){
                    if (data[i].want.abilityName == "com.example.taskswitching.MainAbility2") {
                        abilityMissionList.push(data[i].missionId);
                    }
                }
                console.log(TAG + "abilityMissionList is: " + JSON.stringify(abilityMissionList))
            });
        }, 2000);

        setTimeout(function(){
            missionManager.getMissionSnapShot("", parseInt(abilityMissionList[0]), (err, data) => {
                console.log(TAG + "getMissionSnapShot success, err: "
                + JSON.stringify(err) + ", data: " + JSON.stringify(data))
                expect(JSON.stringify(data.ability.bundleName) == "com.example.taskswitching")
                done()
            })
        }, 4000)
        console.log("------------end SUB_AA_OpenHarmony_MoveMissionToFront_2900-------------");
    });

    console.log("------------------ taskSwitchTest is end ------------------");
})
