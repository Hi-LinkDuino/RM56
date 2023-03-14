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
import featureAbility from '@ohos.ability.featureability'
import missionManager from '@ohos.application.missionManager'
import appManager from "@ohos.application.appManager"
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

let abilityNameList = [
    "com.ohos.launcher.MainAbility",
    "com.ohos.callui.ServiceAbility",
    "com.example.SimulateFeatureAbilityFir",
    "com.example.VerifyActThirdAbility",
    "com.example.VerifyIoThirdAbility",
    "com.example.SimulateEntryAbility",
    "com.example.actsamscallbackfourthscene.MainAbility"
]

let bundleNameList = [
    "com.ohos.launcher",
    "com.ohos.systemui",
    "com.ohos.callui",
    "com.ohos.contacts",
    "com.ohos.mms",
    "com.ohos.telephonydataability",
    "com.ohos.contactsdataability",
    "com.ix.simulate.feature",
    "com.ix.verify.io",
    "com.ix.simulate.entry",
    "com.ix.verify.act",
    "com.example.actsamscallbackfourthscene"
]

describe('ActsAmsCallBackFourthScene', function () {
    console.info('----ActsAmsCallBackFourthScene----');
    beforeAll(async function (done) {
        let maxnum = 10;
        let data = await missionManager.getMissionInfos("", maxnum);
        console.log('ActsAmsCallBackFourthScene beforeAll getMissionInfos data: ' + JSON.stringify(data));
        for (let i = 0; i < data.length; i++) {
            if (data[i].want.bundleName != 'com.example.actsamscallbackfourthscene') {
                console.log("ActsAmsCallBackFourthScene, missionId: " + data[i].missionId)
                missionManager.clearMission(data[i].missionId,
                    (error, info) => {
                        console.info('ActsAmsCallBackFourthScene beforeAll clearMission error.code \
                        ' + error.code + ', want.bundleName:' + data[i].want.bundleName);
                    }
                );
            }
        }

        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.ix.verify.io",
                    abilityName: "com.example.VerifyIoThirdAbility",
                    action: "action1",
                    parameters:
                        {},
                },
            }, (error, data) => {
                console.info('ActsAmsCallBackFourthScene VerifyIoThirdAbility start, error.code \
                ' + error.code + ', data length [' + data.length + ']');
            }
        );

        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.ix.simulate.feature",
                    abilityName: "com.example.SimulateFeatureAbilityFir",
                    action: "action1",
                    parameters:
                        {},
                },
            }, (error, data) => {
                console.info('ActsAmsCallBackFourthScene SimulateFeatureAbilityFir start, error.code \
                ' + error.code + ', data length [' + data.length + ']');
            }
        );

        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.ix.verify.act",
                    abilityName: "com.example.VerifyActThirdAbility",
                    action: "action1",
                    parameters:
                        {},
                },
            }, (error, data) => {
                console.info('ActsAmsCallBackFourthScene VerifyActThirdAbility start, error.code \
                ' + error.code + ', data length [' + data.length + ']');
            }
        );

        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.ix.simulate.entry",
                    abilityName: "com.example.SimulateEntryAbilityFir",
                    action: "action1",
                    parameters:
                        {},
                },
            }, (error, data) => {
                console.info('ActsAmsCallBackFourthScene SimulateEntryAbilityFir start, error.code \
                ' + error.code + ', data length [' + data.length + ']');
            }
        );
        done();
    });

    function timeout(done) {
        expect().assertFail();
        console.debug('Acts_Ams_test=========timeout========');
        done();
    }

    function sleep(delay) {
        let start = new Date().getTime();
        while (true) {
            if (new Date().getTime() - start > delay) {
                break;
            }
        }
    }

    /*
     * @tc.number    : Acts_Ams_test_5000
     * @tc.name      : getProcessRunningInfos : Get All Running Processes Info
     * @tc.desc      : Get All Running Processes Info(by CallBack)
     */
    it('Acts_Ams_test_5000', 0, async function (done) {
        console.info("sleep begin");
        sleep(5000);
        console.info("sleep end");
        appManager.getProcessRunningInfos(
            (error, info) => {
                console.info('Acts_Ams_test_5000 getProcessRunningInfos error.code \
                ' + error.code + ', data length [' + info.length + ']');
                expect(Array.isArray(info)).assertEqual(true);
                expect(info.length).assertLarger(0);
                for (let i = 0; i < info.length; i++) {
                    console.info('Acts_Ams_test_5000 getProcessRunningInfo[' + i + "]: " + JSON.stringify(info[i]));
                    expect(typeof (info[i].pid)).assertEqual("number");
                    expect(info[i].pid).assertLarger(0);

                    expect(typeof (info[i].processName)).assertEqual("string");
                    expect(info[i].processName.length).assertLarger(0);
                    expect(Array.isArray(info[i].bundleNames)).assertEqual(true);
                    expect(info[i].bundleNames.length).assertEqual(0);

                    expect(typeof (info[i].uid)).assertEqual("number");
                    expect(info[i].uid).assertLarger(0);
                }
                done();
            });
    })

    /*
     * @tc.number    : Acts_Ams_test_5400
     * @tc.name      : getMissionInfos : Query Recent Ability Mission Infos
     * @tc.desc      : Query Recent Ability Mission Infos(by CallBack)
     */
    it('Acts_Ams_test_5400', 0, async function (done) {
        let maxnum = 100;
        missionManager.getMissionInfos("", maxnum,
            (error, data) => {
                console.info('Acts_Ams_test_5400 getMissionInfos error.code : \
                ' + error.code + ',data length [' + data.length + ']');
                expect(Array.isArray(data)).assertEqual(true);
                expect(data.length).assertEqual(4);
                for (let i = 0; i < data.length; i++) {
                    console.info('Acts_Ams_test_5400 getMissionInfos data[' + i + "]: " + JSON.stringify(data[i]));
                    expect(typeof (data[i].missionId)).assertEqual("number");
                    expect(data[i].missionId).assertLarger(0);

                    expect(typeof (data[i].want)).assertEqual("object");
                    expect(typeof (data[i].want.deviceId)).assertEqual("string");
                    expect(typeof (data[i].want.bundleName)).assertEqual("string");
                    expect(data[i].want.bundleName.length).assertLarger(0);
                    expect(bundleNameList.indexOf(data[i].want.bundleName)).assertLarger(-1);
                    expect(typeof (data[i].want.abilityName)).assertEqual("string");
                    expect(data[i].want.abilityName.length).assertLarger(0);
                    expect(abilityNameList.indexOf(data[i].want.abilityName)).assertLarger(-1);

                    expect(typeof (data[i].label)).assertEqual("string");
                    expect(typeof (data[i].iconPath)).assertEqual("string");
                }
                done();
            });
    })

    /*
     * @tc.number    : Acts_Ams_test_5600
     * @tc.name      : clearMission : Remove Mission
     * @tc.desc      : Remove Mission(by CallBack)
     */
    it('Acts_Ams_test_5600', 0, async function (done) {
        let maxnum = 30;
        let result = await missionManager.getMissionInfos("", maxnum);
        let id = 0;
        for (let i = 0; i < result.length; i++) {
            console.info('Acts_Ams_test_5600 getMissionInfos result[' + i + "]: " + JSON.stringify(result[i]));
            if (result[i].want.abilityName != 'com.example.actsamscallbackfourthscene.MainAbility' &&
                result[i].runningState == 0) {
                console.info('5600 id[' + result[i].missionId + "] ");
                id = result[i].missionId;
                break;
            }
        }
        missionManager.clearMission(id,
            (error, info) => {
                console.info('Acts_Ams_test_5600 clearMission error.code \
                ' + error.code + ',data  [' + info + ']');
                expect(error.code).assertEqual(0);
                done();
            });
    })

    /*
     * @tc.number    : Acts_Ams_test_6000
     * @tc.name      : moveMissionToFront : Move Mission To Top
     * @tc.desc      : Move Mission To Top(by CallBack)
     */
    it('Acts_Ams_test_6000', 0, async function (done) {
        let maxnum = 30;
        let result = await missionManager.getMissionInfos("", maxnum);
        for (let i = 0; i < result.length; i++) {
            console.info('Acts_Ams_test_6000 getMissionInfos result[' + i + "]: " + JSON.stringify(result[i]));
        }
        missionManager.moveMissionToFront(result[1].missionId,
            (error, info) => {
                console.info('Acts_Ams_test_6000 moveMissionToFront error.code \
                ' + error.code + ',data  [' + info + ']');
                expect(error.code).assertEqual(0);
                done();
            });
    })

    /*
     * @tc.number    : Acts_Ams_test_6400
     * @tc.name      : killProcessesByBundleName : Kill Processes By BundleName
     * @tc.desc      : Kill Processes By BundleName(by CallBack)
     */
    it('Acts_Ams_test_6400', 0, async function (done) {
        appManager.killProcessesByBundleName('xxxxxxxxx',
            (error) => {
                console.info('Acts_Ams_test_6400 killProcessesByBundleName error.code ' + error.code);
                expect(error.code).assertEqual(2097215);
                done();
            });
    })

    /*
     * @tc.number    : Acts_Ams_test_11900
     * @tc.name      : clearMissions : delete Missions
     * @tc.desc      : delete Missions(by CallBack)
     */
    it('Acts_Ams_test_11900', 0, async function (done) {
        let maxnum = 30;
        let result = await missionManager.getMissionInfos("", maxnum);
        for (let i = 0; i < result.length; i++) {
            console.info('Acts_Ams_test_11900 getMissionInfos result[' + i + "]: " + JSON.stringify(result[i]));
        }
        missionManager.clearAllMissions(
            (error, info) => {
                console.info('Acts_Ams_test_11900 clearAllMissions error.code: \
                ' + error.code + ',data  [' + info + ']');
                expect(error.code).assertEqual(0);
                done();
            });
    })
})