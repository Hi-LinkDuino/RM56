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
import featureAbility from '@ohos.ability.featureAbility';
import missionManager from "@ohos.application.missionManager";
import commonEvent from '@ohos.commonEvent';
describe('missionManager', function () {
    console.info("------------------logMessage SUB_AA_OpenHarmony_MissionManager_6500-------------------");
    var missionIds = [];
    var timestamps = [];
    var ability1LifeEventsonHide = {
        events : [
        "singletonEntry2Ability1_onHide",
        ]
    }
    var ability1LifeEvents = {
        events : ["singletonEntry2Ability1_onShow",
        "singletonEntry2Ability1_onHide",
        "singletonEntry2Ability1_onInactive",
        "singletonEntry2Ability1_onDestroy"]
    }
    var listKey = [""];
    var Subscriber = "";
    
        /*
         * @tc.number  SUB_AA_OpenHarmony_MissionManager_6500
         * @tc.name    The timestamp of the verification ability will change
         * @tc.desc    Function test
         * @tc.level   0
         */
    it('SUB_AA_OpenHarmony_MissionManager_6500', 0, async function (done) {
        listKey = [""];
        function SubscribeCallBack(error, data) {
            console.log(" Subscribe CallBack data:" + JSON.stringify(data));
            if(data.event == "singletonEntry2Ability1_onHide"){
                listKey[0] = "onHide";
                console.log("print listKey" + listKey);
            }
        }
        commonEvent.createSubscriber(ability1LifeEventsonHide).then(async (data) => {
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        });
        setTimeout(function () {
            featureAbility.startAbility(
                { want: {
                    bundleName: "com.example.missionmanager",
                    abilityName: "com.example.entry2.MainAbility"
                } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_MissionManager_6500 - startAbility1: '
                + JSON.stringify(error) + ", " + JSON.stringify(data))
            });
        }, 1000)
        setTimeout(()=>{
            missionManager.getMissionInfos('', 100, (error, data) => {
                console.log("SUB_AA_OpenHarmony_MissionManager_6500, twosgetMissionInfos:" + JSON.stringify(data));
                for (var i = 0;i < data.length; i++) {
                    console.log('aaaa' + JSON.stringify(data[i].missionId)+"::"+data[i]["want"].abilityName);
                    if (data[i].want.abilityName == "com.example.entry2.MainAbility"){
                        console.log("uuuuu"+ JSON.stringify(data[i].want.abilityName));
                        missionIds.push(data[i].missionId);
                        timestamps.push(data[i].timestamp);
                    } else if ([i].want.abilityName != "com.example.entry2.MainAbility"){
                        console.log("failed222222222222");
                    }
                };
            })
        },1000)
        setTimeout(function () {
            featureAbility.startAbility(
                { want: {
                    bundleName: "com.example.missionmanager",
                    abilityName: "com.example.entry2.MainAbility2"
                } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_MissionManager_6500 - startAbility2: ' +
                JSON.stringify(error) + ", " + JSON.stringify(data))
                console.log("print listKey111" + listKey);
                setTimeout(function () {
                    expect(listKey[0] == "onHide").assertTrue();
                    done();
                }, 3000)
            });
        }, 1300)
        setTimeout(()=>{
            console.log("ttttyyyyy" + missionIds+":::"+timestamps[0]);
            for(var i=0;i<missionIds.length;i++){
                missionManager.getMissionInfo('', parseInt(missionIds[1]), (error, data) => {
                    console.log("SUB_AA_OpenHarmony_MissionManager_6500, infos2:" + JSON.stringify(data) + "");
                    console.log(data.timestamp+"::::"+timestamps[1]);
                    expect(data.timestamp == timestamps[1]).assertTrue();
                })
            }
        },2000)
        console.log("------------end start SUB_AA_OpenHarmony_MissionManager_6500-------------");
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_6400
     * @tc.name    Verify the continuable information in the missionInfo of the ability obtained by getMissionInfos
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_6400', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MissionManager_6400-------------");
        var TAG = "SUB_AA_OpenHarmony_MissionManager_6400";
        var missionInfo = ["","","","","","","","","",""];
        setTimeout(function () {
            featureAbility.startAbility(
                { want: {
                    bundleName: "com.example.myapplication3",
                    abilityName: "com.example.entry2.MainAbility1"
                } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_MissionManager_6400 - startAbility: '
                + JSON.stringify(error) + ", " + JSON.stringify(data))
            });
        }, 1000)
        console.log("------------end start SUB_AA_OpenHarmony_MissionManager_6400-------------");
        setTimeout(()=>{
            missionManager.getMissionInfos('', 10, (err, data) => {
                console.log("SUB_AA_OpenHarmony_MissionManager_6400, twosgetMissionInfos:" + JSON.stringify(data));
                for (var i = 0;i < data.length; i++) {
                    missionInfo[i] = data[i];
                    console.log('qweqweqweqweqwe' + JSON.stringify(missionInfo[i].continuable));
                    if(missionInfo[i].want.abilityName == "com.example.entry2.MainAbility1"){
                        expect(missionInfo[i].continuable == false).assertTrue();
                    };
                }
            })
        },1000)
        done();
    })

    /*
     * @tc.number  SUB_AA_OpenHarmony_MissionManager_6300
     * @tc.name    Verify the want information in the missionInfo of the ability obtained by getMissionInfos
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_MissionManager_6300', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_MissionManager_6300-------------");
        var TAG = "SUB_AA_OpenHarmony_MissionManager_6300";
        var missionInfo = ["","","","","","","","","",""];
        setTimeout(function () {
            featureAbility.startAbility(
                { want: {
                    bundleName: "com.example.myapplication3",
                    abilityName: "com.example.entry2.MainAbility1"
                } }, (error, data) => {
                console.log('SUB_AA_OpenHarmony_MissionManager_6300 - startAbility: '
                + JSON.stringify(error) + ", " + JSON.stringify(data))
            });
        }, 1000)
        console.log("------------end start SUB_AA_OpenHarmony_MissionManager_6300-------------");
        setTimeout(()=>{
            missionManager.getMissionInfos('', 10, (err, data) => {
                console.log("SUB_AA_OpenHarmony_MissionManager_6300, twosgetMissionInfos:" + JSON.stringify(data));
                for (var i = 0;i < data.length; i++) {
                    missionInfo[i] = data[i];
                    console.log('qweqweqweqweqwe' + JSON.stringify(missionInfo[i].want));
                    if(missionInfo[i].want.abilityName == "com.example.entry2.MainAbility1"){
                        if(missionInfo[i].want.bundleName == "com.example.myapplication3"){
                            console.log('weweqqq');
                            expect(missionInfo[i].want.bundleName == "com.example.myapplication3").assertTrue();
                        }
                    };
                }
            })
        },1000)
        done();
    })
})