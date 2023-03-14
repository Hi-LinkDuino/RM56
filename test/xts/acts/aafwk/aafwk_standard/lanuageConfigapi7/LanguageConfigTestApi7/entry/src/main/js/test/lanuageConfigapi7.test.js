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
import commonEvent from '@ohos.commonevent'
import bundle from '@ohos.bundle'
import missionManager from '@ohos.application.missionManager'
import abilityManager from '@ohos.application.abilityManager'
import featureAbility from '@ohos.ability.featureability'

var subscriberInfoMultiInstance = {
    events: ["UpdateLanguageFinish"]
};
const BUNDLE_PATHS = [['/data/test/ActslanguageconfigAppATest.hap']];
const BUNDLE_NAMES = ['com.example.actslanguageconfigappatest'];
const BUNDLE_COUNT = 1;
const START_ABILITY_TIMEOUT = 5000;
const MAX_MISSION_NUM = 1024;

async function startAbilityProcess(parameters) {
    let bundleName;
    let abilityName;

    let idx = parameters.nextStep;
    switch (parameters.step[idx]) {
        case "StartA1":
            bundleName = "com.example.actslanguageconfigappatest";
            abilityName = "com.example.actslanguageconfigappatest.MainAbility";
            break;

        default:
            break;
    }
    parameters.nextStep = ++idx;
    await featureAbility.startAbility({
        want: {
            bundleName: bundleName,
            abilityName: abilityName,
            parameters: parameters
        }

    })
}


function getMissionId() {
    return new Promise(async (resolve, reject) => {
        var missionId = -1;
        var missionInfos = await missionManager.getMissionInfos('', MAX_MISSION_NUM);
        for (let i = 0; i < missionInfos.length; i++) {
            console.log('getMissionId result: ' + i + '= ' + JSON.stringify(missionInfos[i]))
            if ((missionInfos[i].want.abilityName == "com.example.languageconfigtestapi7.MainAbility") &&
            (missionInfos[i].runningState == 0)) {
                missionId = missionInfos[i].missionId;
                break;
            }
        }
        console.log('======>Acts_getMissionId resolve missionId<=======' + missionId);
        resolve(missionId);
    })
}

describe('ActsAbilityTest', function () {
    beforeEach(async (done) => {
        console.log('======>Acts_beforeEach  ininin<=======');
        let installer = await bundle.getBundleInstaller();
        var count = 0;

        for (let i = 0; i < BUNDLE_COUNT; i++) {
            installer.install(BUNDLE_PATHS[i], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, (err, data) => {
                count++;
                console.log('======>Acts_beforeEach install finish <=======' + err.code);
                console.log('======>Acts_beforeEach install finish <=======' + data.status);
                console.log('======>Acts_beforeEach install finish <=======' + data.statusMessage);
                if (count == BUNDLE_COUNT) {
                    setTimeout(() => {
                        done();
                    }, 1000)
                }
            })
        }
    })
    
    /*
      * @tc.number: Acts_UpdateConfigurationFA_0100
      * @tc.name: Connects a service ability, which is used to start a cloned page ability.
      * @tc.desc: Check the event data of executor page ability publishes
      */
    it('Acts_UpdateConfigurationFA_0100', 0, async function (done) {
        console.log('Acts_UpdateConfigurationFA_0100====<begin');
        var subscriber;
        var id;
        var configDefault = {
            language: "zh-Hans"
        };
        var configChange = {
            language: "en"
        };
        await abilityManager.updateConfiguration(configDefault);



        function subscribeCallBack(err, data) {
            console.debug("====>Acts_Subscribe CallBack data:====>" + JSON.stringify(data));

            expect(data.event).assertEqual("UpdateLanguageFinish");
            clearTimeout(id);
            console.log('Acts_UpdateConfigurationFA_0100 start set lang');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);
        }

        commonEvent.createSubscriber(subscriberInfoMultiInstance).then(async (data) => {
            console.debug("Acts_====>Create Subscriber====>");
            subscriber = data;
            commonEvent.subscribe(subscriber, subscribeCallBack);
        })

        function unSubscribeCallback() {

            setTimeout( function () {
                console.debug("Acts_====>UnSubscribe CallBack====>");
                done();
            }, 2000);

        }

        function timeout() {
            expect().assertFail();
            console.log('Acts_StartLanguageConfigTester timeout');
            commonEvent.unsubscribe(subscriber, unSubscribeCallback);
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);
        console.log('Acts_Start LanguageConfigTester Acts_UpdateConfigurationFA_0100 timer id : ' + id);

        console.log('Acts_LanguageConfigTester Acts_UpdateConfigurationFA_0100 start Ability');
        startAbilityProcess({
            startId: 0,
            stepNum: 1,
            nextStep: 0,
            step: ["StartA1"]
        });
    })


})
