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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit'

var abilityNameList = [
    "com.ohos.launcher.MainAbility",
    "com.ohos.callui.ServiceAbility",
    "com.example.SimulateFeatureAbilityFir",
    "com.example.actsamscallbackfirstscene.MainAbility"
]

var bundleNameList = [
    "com.ohos.launcher",
    "com.ohos.systemui",
    "com.ohos.callui",
    "com.ohos.contacts",
    "com.ohos.mms",
    "com.ohos.telephonydataability",
    "com.ohos.contactsdataability",
    "com.ix.simulate.feature",
    "com.example.actsamscallbackfirstscene"
]

describe('ActsAmsCallBackFirstScene', function () {
    console.info('----ActsAmsCallBackFirstScene----');
    beforeAll(async function (done) {
        var maxnum = 10;
        var data = await missionManager.getMissionInfos("", maxnum);
        console.log('ActsAmsCallBackFirstScene beforeAll getMissionInfos data: ' + JSON.stringify(data));
        for (var i = 0; i < data.length; i++) {
            if (data[i].want.bundleName != 'com.example.actsamscallbackfirstscene') {
                console.log("ActsAmsCallBackFirstScene, missionId: " + data[i].missionId)
                missionManager.clearMission(data[i].missionId,
                    (error, info) => {
                        console.info('ActsAmsCallBackFirstScene beforeAll clearMission error.code \
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
                console.info('ActsAmsCallBackFirstScene SimulateFeatureAbilityFir start, error.code \
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
        var start = new Date().getTime();
        while (true) {
            if (new Date().getTime() - start > delay) {
                break;
            }
        }
    }

    /*
     * @tc.number    : Acts_Ams_test_0200
     * @tc.name      : getProcessRunningInfos : Get All Running Processes Info
     * @tc.desc      : Get All Running Processes Info(by CallBack)
     */
    it('Acts_Ams_test_0200', 0, async function (done) {
        console.info("sleep begin");
        sleep(5000);
        console.info("sleep end");
        appManager.getProcessRunningInfos(
            (error, info) => {
                console.info('Acts_Ams_test_0200 getProcessRunningInfos error.code \
                ' + error.code + ', data length [' + info.length + ']');
                expect(Array.isArray(info)).assertEqual(true);
                expect(info.length).assertLarger(0);
                for (var i = 0; i < info.length; i++) {
                    console.info('Acts_Ams_test_0200 getProcessRunningInfo[' + i + "]: " + JSON.stringify(info[i]));
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
    * @tc.number    : Acts_Ams_test_0400
    * @tc.name      : getMissionInfos : Query Running Ability Mission Infos
    * @tc.desc      : Query Running Ability Mission Infos(by CallBack)
    */
    it('Acts_Ams_test_0400', 0, async function (done) {
        var maxnum = 10;
        missionManager.getMissionInfos("", maxnum,
            (error, info) => {
                console.info('Acts_Ams_test_0400 getMissionInfos error.code : \
                ' + error.code + ',data length [' + info.length + ']');
                expect(Array.isArray(info)).assertEqual(true);
                expect(info.length).assertEqual(2);
                for (var i = 0; i < info.length; i++) {
                    console.info('Acts_Ams_test_0400 getMissionInfos data[' + i + "]: " + JSON.stringify(info[i]));
                    expect(typeof (info[i].missionId)).assertEqual("number");
                    expect(info[i].missionId).assertLarger(0);

                    expect(typeof (info[i].want)).assertEqual("object");
                    expect(typeof (info[i].want.deviceId)).assertEqual("string");
                    expect(typeof (info[i].want.bundleName)).assertEqual("string");
                    expect(info[i].want.bundleName.length).assertLarger(0);
                    expect(bundleNameList.indexOf(info[i].want.bundleName)).assertLarger(-1);
                    expect(typeof (info[i].want.abilityName)).assertEqual("string");
                    expect(info[i].want.abilityName.length).assertLarger(0);
                    expect(abilityNameList.indexOf(info[i].want.abilityName)).assertLarger(-1);

                    expect(typeof (info[i].label)).assertEqual("string");
                    expect(typeof (info[i].iconPath)).assertEqual("string");
                }
                done();
            });
    })

    /*
    * @tc.number    : Acts_Ams_test_1600
    * @tc.name      : killProcessesByBundleName : Kill Processes By BundleName
    * @tc.desc      : Kill Processes By BundleName(by CallBack)
    */
    it('Acts_Ams_test_1600', 0, async function (done) {
        appManager.killProcessesByBundleName('com.ix.simulate.feature',
            (error, info) => {
                console.info('Acts_Ams_test_1600 killProcessesByBundleName error.code: \
                    ' + error.code + ',data  [' + info + ']');
                expect(error.code).assertEqual(0);
                done();
            });
    })

    /*
     * @tc.number    : Acts_Ams_test_1200
     * @tc.name      : moveMissionToFront : Move Mission To Top
     * @tc.desc      : Move Mission To Top(by CallBack)
     */
    it('Acts_Ams_test_1200', 0, async function (done) {
        var maxnum = 10;
        var result = await missionManager.getMissionInfos("", maxnum);
        for (var i = 0; i < result.length; i++) {
            console.info('Acts_Ams_test_1200 getMissionInfos result[' + i + "]: " + JSON.stringify(result[i]));
        }
        missionManager.moveMissionToFront(result[0].missionId,
            (error, info) => {
                console.info('Acts_Ams_test_1200 moveMissionToFront error.code \
                ' + error.code + ',data  [' + info + ']');
                expect(error.code).assertEqual(0);
                done();
            });
    })

})