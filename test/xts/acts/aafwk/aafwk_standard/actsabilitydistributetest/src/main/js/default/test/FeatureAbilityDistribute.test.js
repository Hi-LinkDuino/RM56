/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import abilityFeatureAbility from '@ohos.ability.featureAbility'

var timeSleep = 3000;

describe('DistributedTest', function(){
    function sleep(time) {
        return new Promise((resolve) => setTimeout(resolve, time));
    }


    /**
     * @tc.number    SUB_AA_OpenHarmony_ContinueAbility_0300
     * @tc.name      testContinueAbility0300
     * @tc.desc      T
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 3
     */
     it('testContinueAbility0300', 0, async function (done) {
        console.log('testContinueAbility0300 run start');
        abilityFeatureAbility.continueAbility(undefined, (err, data) => {
            console.log('testContinueAbility0300 err is :' + JSON.stringify(err) + ', value is :' 
            + JSON.stringify(data));
        })
        await sleep(timeSleep);
        console.log('testContinueAbility0300 run end');
        done();
     })

    /**
     * @tc.number    SUB_AA_OpenHarmony_ContinueAbility_0400
     * @tc.name      testContinueAbility0200
     * @tc.desc      T
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 3
     */
     it('testContinueAbility0400', 0, async function (done) {
        console.log('testContinueAbility0400 run start');
        abilityFeatureAbility.continueAbility("", (err, data) => {
            console.log('testContinueAbility0400 err is :' + JSON.stringify(err) + ', value is :'
            + JSON.stringify(data));
        })
        await sleep(timeSleep);
        console.log('testContinueAbility0400 run end');
        done();
     })

    /**
     * @tc.number    SUB_AA_OpenHarmony_ContinueAbility_0500
     * @tc.name      testContinueAbility0500
     * @tc.desc      T
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 3
     */
     it('testContinueAbility0500', 0, async function (done) {
        let options = {
            deviceId : undefined,
            reversible : false,
        }
        console.log('testContinueAbility0500 run start');
        abilityFeatureAbility.continueAbility(options, (err, data) => {
            console.log('testContinueAbility0500 err is :' + JSON.stringify(err) + ', value is :' 
            + JSON.stringify(data));
        })
        await sleep(timeSleep);
        console.log('testContinueAbility0500 run end');
        done();
     })

    /**
     * @tc.number    SUB_AA_OpenHarmony_ContinueAbility_0600
     * @tc.name      testContinueAbility0600
     * @tc.desc      T
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 3
     */
     it('testContinueAbility0600', 0, async function (done) {
        let options = {
            deviceId : "",
            reversible : false,
        }
        console.log('testContinueAbility0600 run start');
        abilityFeatureAbility.continueAbility(options, (err, data) => {
            console.log('testContinueAbility0600 err is :' + JSON.stringify(err) + ', value is :' 
            + JSON.stringify(data));
        })
        await sleep(timeSleep);
        console.log('testContinueAbility0600 run end');
        done();
     })

    /**
     * @tc.number    SUB_AA_OpenHarmony_ContinueAbility_0700
     * @tc.name      testContinueAbility0700
     * @tc.desc      T
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 3
     */
     it('testContinueAbility0700', 0, async function (done) {
        let options = {
            reversible : false,
        }
        console.log('testContinueAbility0700 run start');
        abilityFeatureAbility.continueAbility(options, (err, data) => {
            console.log('testContinueAbility0700 err is :' + JSON.stringify(err) + ', value is :' 
            + JSON.stringify(data));
        })
        await sleep(timeSleep);
        console.log('testContinueAbility0700 run end');
        done();
     })

    /**
     * @tc.number    SUB_AA_OpenHarmony_ContinueAbility_0900
     * @tc.name      testContinueAbility0900
     * @tc.desc      T
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 3
     */
     it('testContinueAbility0900', 0, async function (done) {
        let options = {
            deviceId : "2BB6F91131F74C1174904195FE72B6068F8F82280F6CE93BDFE0EFF9B37F9DD26",
            reversible : false,
        }
        console.log('testContinueAbility0900 run start');
        abilityFeatureAbility.continueAbility(options, (err, data) => {
            console.log('testContinueAbility0900 err is :' + JSON.stringify(err) + ', value is :' 
            + JSON.stringify(data));
        })
        await sleep(timeSleep);
        console.log('testContinueAbility0900 run end');
        done();
     })

     /**
     * @tc.number    SUB_AA_OpenHarmony_ContinueAbility_1000
     * @tc.name      testContinueAbility0900
     * @tc.desc      T
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 3
     */
     it('testContinueAbility1000', 0, async function (done) {
        let options = {
            deviceId : "2BB6F91131F74C1174904195FE72B6068F8F82280F6CE93BDFE0EFF9B37F9DD26",
            reversible : false,
        }
        for(var i = 0; i < 10; i++) {
            abilityFeatureAbility.continueAbility(options, (err, data) => {
                console.log('testContinueAbility1000 err is :' + JSON.stringify(err) + ', value is :' 
                + JSON.stringify(data));
            })
        }
        await sleep(timeSleep);
        console.log('testContinueAbility1000 run end');
        done();
     })
})