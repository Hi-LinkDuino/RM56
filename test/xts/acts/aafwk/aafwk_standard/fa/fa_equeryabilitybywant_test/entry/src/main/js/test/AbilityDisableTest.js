
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
import abilityFeatureAbility from '@ohos.ability.featureAbility';
import bundleManager from '@ohos.bundle';
import appManager from "@ohos.application.appManager"

describe('AbilityDisableTests', function () {
    console.log("AbilityDisableTests --- start");

        /*
         * @tc.number  SUB_AA_OpenHarmony_EqueryAbilityByWant_0100
         * @tc.name    Call queryAbilityByWant to get moduleName
         * @tc.desc    Function test
         * @tc.level   0
         */
        it("SUB_AA_OpenHarmony_EqueryAbilityByWant_0100", 0, async function (done) {
            console.info("------------------logMessage SUB_AA_OpenHarmony_EqueryAbilityByWant_0100-------------------");
            var context = abilityFeatureAbility.getContext();
            console.log("+++++++++++++",context);
            let bundleFlags=bundleManager.BundleFlag.GET_ABILITY_INFO_WITH_PERMISSION;
            let userId=100;
            let want={
                bundleName:"com.example.abilitydisable",
                abilityName:"com.example.abilitydisable.MainAbility"
            };
            bundleManager.queryAbilityByWant(want,bundleFlags,userId,(err, data) => {
                console.log('SUB_AA_OpenHarmony_Clear_0100 queryAbilityByWant errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                for(var i=0;i<data.length;i++){
                    console.log("data[i]['moduleName']"+data[i]['moduleName']);
                    expect(data[i]['moduleName'] == "entry").assertTrue();
                }
            })
            done();
        });

        /*
         * @tc.number  SUB_AA_OpenHarmony_EqueryAbilityByWant_0200
         * @tc.name    Call queryAbilityByWant to get the URI
         * @tc.desc    Function test
         * @tc.level   0
         */
        it("SUB_AA_OpenHarmony_EqueryAbilityByWant_0200", 0, async function (done) {
            console.info("------------------logMessage SUB_AA_OpenHarmony_EqueryAbilityByWant_0200-------------------");
            var context = abilityFeatureAbility.getContext();
            let bundleFlags=bundleManager.BundleFlag.GET_ABILITY_INFO_WITH_PERMISSION;
            let userId=100;
            let want={
                bundleName:"com.example.abilitydisable",
                abilityName:"com.example.abilitydisable.MainAbility"
            };
            bundleManager.queryAbilityByWant(want,bundleFlags,userId,(err, data) => {
                console.log('SUB_AA_OpenHarmony_EqueryAbilityByWant_0200 queryAbilityByWant errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                for(var i=0;i<data.length;i++){
                    console.log("data[i]['uri']"+data[i]['uri']);
                    expect(data[i]['uri'] == "").assertTrue();
                }
            })
            done();
        });

        /*
         * @tc.number  SUB_AA_OpenHarmony_EqueryAbilityByWant_0300
         * @tc.name    Call getapplicationInfo to get the name
         * @tc.desc    Function test
         * @tc.level   0
         */
        it("SUB_AA_OpenHarmony_EqueryAbilityByWant_0300", 0, async function (done) {
            console.info("------------------logMessage SUB_AA_OpenHarmony_EqueryAbilityByWant_0300-------------------");
            let bundleName="com.example.abilitydisable";
            let bundleFlags=bundleManager.BundleFlag.GET_ABILITY_INFO_WITH_PERMISSION;
            let userId=100;
            bundleManager.getApplicationInfo(bundleName, bundleFlags,userId,(err, data) => {
                console.log('SUB_AA_OpenHarmony_EqueryAbilityByWant_0300 getapplicationInfo errCode : '
                + JSON.stringify(err) + " data: " + JSON.stringify(data));
                expect(data['name']).assertEqual("com.example.abilitydisable");
            })
            done();
        });
        
        /*
         * @tc.number  SUB_AA_OpenHarmony_EqueryAbilityByWant_0300
         * @tc.name    Call AbilityStateData to get abilityType
         * @tc.desc    Function test
         * @tc.level   0
         */
        it("SUB_AA_OpenHarmony_EqueryAbilityByWant_0400", 0, async function (done) {
            console.log("------------start SUB_AA_OpenHarmony_EqueryAbilityByWant_0400-------------");
            var registerId = appManager.registerApplicationStateObserver({
                onForegroundApplicationChanged: (data) => {
                    console.log("SUB_onForegroundApplicationChanged")
                },
                onAbilityStateChanged: (data) => {
                    console.log("SUB_onAbilityStateChanged")
                },
                onProcessCreated: (data) => {
                    console.log("SUB_onProcessCreated")
                },
                onProcessDied: (data) => {
                    console.log("SUB_onProcessDied")
                }
            })
            console.log('SUB_AA_OpenHarmony_EqueryAbilityByWant_0400 registerId  : ' + JSON.stringify(registerId));
            console.log('SUB_AA_OpenHarmony_EqueryAbilityByWant_0400 registerId  : ' + typeof registerId);
            expect(typeof(registerId) == "number").assertTrue();

            await appManager.unregisterApplicationStateObserver(parseInt(registerId), async(err, data) => {
                console.log("SUB_AA_OpenHarmony_StateListener_0200 err1:"
                + JSON.stringify(err) + ",data: " + JSON.stringify(data));
                expect(err["code"] == "1").assertTrue();
            })
            done();
        })


})