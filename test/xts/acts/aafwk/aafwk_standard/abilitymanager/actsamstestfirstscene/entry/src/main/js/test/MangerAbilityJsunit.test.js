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

var abilityNameList = [
    "com.ohos.launcher.MainAbility",
    "com.ohos.callui.ServiceAbility",
    "com.example.SimulateFeatureAbilityFir",
    "com.example.actsamstestfirstscene.MainAbility"
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
    "com.example.actsamstestfirstscene"
]

describe('ActsAmsTestFirstScene', function () {
    console.info('----ActsAmsTestFirstScene----');
    beforeAll(async function (done) {
        var maxnum = 10;
        var data = await missionManager.getMissionInfos("", maxnum);
        console.log('ActsAmsTestFirstScene beforeAll getMissionInfos data: ' + JSON.stringify(data));
        for (var i = 0; i < data.length; i++) {
            if (data[i].want.bundleName != 'com.example.actsamstestfirstscene') {
                console.log("ActsAmsTestFirstScene, missionId: " + data[i].missionId)
                missionManager.clearMission(data[i].missionId,
                    (error, info) => {
                        console.info('ActsAmsTestFirstScene beforeAll clearMission error.code \
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
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    options:
                        {},
                    parameters:
                        {},
                },
            }, (error, data) => {
                console.info('ActsAmsTestFirstScene SimulateFeatureAbilityFir start, error.code \
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
    * @tc.number    : Acts_Ams_test_0100
    * @tc.name      : getProcessRunningInfos : Get All Running Processes Info
    * @tc.desc      : Get All Running Processes Info(by Promise)
    */
    it('Acts_Ams_test_0100', 0, async function (done) {
        console.info("sleep begin");
        sleep(5000);
        console.info("sleep end");
        var info = await appManager.getProcessRunningInfos();
        expect(Array.isArray(info)).assertEqual(true);
        expect(info.length).assertLarger(0);
        for (var i = 0; i < info.length; i++) {
            console.info('Acts_Ams_test_0100 getProcessRunningInfo[' + i + "]: " + JSON.stringify(info[i]));
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
    })

    
    /*
    * @tc.number    : Acts_Ams_test_1100
    * @tc.name      : moveMissionToFront : Move Mission To Top
    * @tc.desc      : Move Mission To Top(by Promise)
    */
    it('Acts_Ams_test_1100', 0, async function (done) {
        var maxnum = 10;
        var result = await missionManager.getMissionInfos("", maxnum);
        for (var i = 0; i < result.length; i++) {
            console.info('Acts_Ams_test_0100 getMissionInfos result[' + i + "]: " + JSON.stringify(result[i]));
        }
        var info = await missionManager.moveMissionToFront(result[0].missionId).catch(err => {
            console.log('Acts_Ams_test_1100 moveMissionToFront failed: ' + err);
            expect(err).assertEqual(0);
        });
        console.info('Acts_Ams_test_1100 moveMissionToFront data  [' + info + ']');
        done();
    })

    /*
    * @tc.number    : Acts_Ams_test_0300
    * @tc.name      : getMissionInfos : Query Running Ability Mission Infos
    * @tc.desc      : Query Running Ability Mission Infos(by Promise)
    */
    it('Acts_Ams_test_0300', 0, async function (done) {
        var maxnum = 10;
        var data = await missionManager.getMissionInfos("", maxnum);
        expect(Array.isArray(data)).assertEqual(true);
        expect(data.length).assertEqual(2);
        for (var i = 0; i < data.length; i++) {
            console.info('Acts_Ams_test_0300 getMissionInfos data[' + i + "]: " + JSON.stringify(data[i]));
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
    })

    /*
    * @tc.number    : Acts_Ams_test_1500
    * @tc.name      : killProcessesByBundleName : Kill Processes By BundleName
    * @tc.desc      : Kill Processes By BundleName(by Promise)
    */
    it('Acts_Ams_test_1500', 0, async function (done) {
        var info = await appManager.killProcessesByBundleName('com.ix.simulate.feature');
        console.info('Acts_Ams_test_1500 killProcessesByBundleName data  [' + info + ']');
        expect(info).assertEqual(0);
        done();
    })
})