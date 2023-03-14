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
import featureAbility from '@ohos.ability.featureability'
import missionManager from '@ohos.application.missionManager'
import appManager from "@ohos.application.appManager"
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

let abilityNameList = [
    "com.ohos.launcher.MainAbility",
    "com.ohos.callui.ServiceAbility",
    "com.example.SimulateFeatureAbilityFir",
    "com.example.VerifyIoThirdAbility",
    "com.example.SimulateFeatureAbilitySed",
    "com.example.actsamscallbackfifthscene.MainAbility"
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
    "com.example.actsamscallbackfifthscene"
]

describe('ActsAmsCallBackFifthScene', function () {
    console.info('----ActsAmsCallBackFifthScene----');
    beforeAll(async function (done) {
        let maxnum = 10;
        let data = await missionManager.getMissionInfos("", maxnum);
        console.log('ActsAmsCallBackFifthScene beforeAll getMissionInfos data: ' + JSON.stringify(data));
        for (let i = 0; i < data.length; i++) {
            if (data[i].want.bundleName != 'com.example.actsamscallbackfifthscene') {
                console.log("ActsAmsCallBackFifthScene, missionId: " + data[i].missionId)
                missionManager.clearMission(data[i].missionId,
                    (error, info) => {
                        console.info('ActsAmsCallBackFifthScene beforeAll clearMission error.code \
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
                    bundleName: "com.ix.simulate.feature",
                    abilityName: "com.example.SimulateFeatureAbilityFir",
                    action: "action1",
                    parameters:
                        {},
                },
            }, (error, data) => {
                console.info('ActsAmsCallBackFifthScene SimulateFeatureAbilityFir start, error.code \
                ' + error.code + ', data length [' + data.length + ']');
            }
        );

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
                console.info('ActsAmsCallBackFifthScene VerifyIoThirdAbility start, error.code \
                ' + error.code + ', data length [' + data.length + ']');
            }
        );

        await featureAbility.startAbility(
            {
                want:
                {
                    deviceId: "",
                    bundleName: "com.ix.simulate.feature",
                    abilityName: "com.example.SimulateFeatureAbilitySed",
                    action: "action1",
                    parameters:
                        {},
                },
            }, (error, data) => {
                console.info('ActsAmsCallBackFifthScene SimulateFeatureAbilitySed start, error.code \
                ' + error.code + ', data length [' + data.length + ']');
            }
        );
        setTimeout(done, 5000);
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
     * @tc.number    : Acts_Ams_test_6600
     * @tc.name      : getProcessRunningInfos : Get All Running Processes Info
     * @tc.desc      : Get All Running Processes Info(by CallBack)
     */
    it('Acts_Ams_test_6600', 0, async function (done) {
        console.info("sleep begin");
        sleep(5000);
        console.info("sleep end");
        appManager.getProcessRunningInfos(
            (error, info) => {
                console.info('Acts_Ams_test_6600 getProcessRunningInfos error.code \
                ' + error.code + ', data length [' + info.length + ']');
                expect(Array.isArray(info)).assertEqual(true);
                expect(info.length).assertLarger(0);
                for (let i = 0; i < info.length; i++) {
                    console.info('Acts_Ams_test_6600 getProcessRunningInfo[' + i + "]: " + JSON.stringify(info[i]));
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
            }
        );
        setTimeout(timeout, 5000);
    })

    /*
     * @tc.number    : Acts_Ams_test_7000
     * @tc.name      : getMissionInfos : Query Recent Ability Mission Infos
     * @tc.desc      : Query Recent Ability Mission Infos(by CallBack)
     */
    it('Acts_Ams_test_7000', 0, async function (done) {
        let maxnum = 10;
        missionManager.getMissionInfos("", maxnum,
            (error, data) => {
                console.info('Acts_Ams_test_7000 getMissionInfos error.code : \
                ' + error.code + ',data length [' + data.length + ']');
                expect(Array.isArray(data)).assertEqual(true);
                expect(data.length).assertEqual(4);
                for (let i = 0; i < data.length; i++) {
                    console.info('Acts_Ams_test_7000 getMissionInfos data[' + i + "]: " + JSON.stringify(data[i]));
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
            }
        );
        setTimeout(timeout, 5000);
    })

    /*
     * @tc.number    : Acts_Ams_test_7600
     * @tc.name      : moveMissionToFront : Move Mission To Top
     * @tc.desc      : Move Mission To Top(by CallBack)
     */
    it('Acts_Ams_test_7600', 0, async function (done) {
        let maxnum = 10;
        let result = await missionManager.getMissionInfos("", maxnum);
        for (let i = 0; i < result.length; i++) {
            console.info('Acts_Ams_test_7600 getMissionInfos result[' + i + "]: " + JSON.stringify(result[i]));
        }
        missionManager.moveMissionToFront(result[0].missionId,
            (error, info) => {
                console.info('Acts_Ams_test_7600 moveMissionToFront error.code \
                ' + error.code + ',data  [' + info + ']');
                expect(error.code).assertEqual(0);
                done();
            }
        );
        setTimeout(timeout, 5000);
    })

    /*
     * @tc.number    : Acts_Ams_test_8000
     * @tc.name      : killProcessesByBundleName : Kill Processes By BundleName
     * @tc.desc      : Kill Processes By BundleName(by CallBack)
     */
    it('Acts_Ams_test_8000', 0, async function (done) {
        appManager.killProcessesByBundleName('com.ix.verify.io',
            (error, info) => {
                console.info('Acts_Ams_test_8000 killProcessesByBundleName error.code: \
                ' + error.code + ',data  [' + info + ']');
                expect(typeof (info)).assertEqual("number");
                expect(info).assertEqual(0);
                done();
            }
        );
        setTimeout(timeout, 5000);
    })
})