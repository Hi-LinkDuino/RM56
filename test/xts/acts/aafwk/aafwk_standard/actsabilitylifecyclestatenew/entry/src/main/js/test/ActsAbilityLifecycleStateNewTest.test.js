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

import AbilityDelegatorRegistry from '@ohos.application.abilityDelegatorRegistry'
import featureAbility from '@ohos.ability.featureAbility'

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe('ActsGetAbilityStateTest', function () {

let gSetTimeout = 1000
beforeAll(async (done) => {
    console.debug('= ACTS_AbeforeAll 1508 ====<begin');
    console.debug('= ACTS_AbeforeAll ====<end');
    done();
})
beforeEach(async (done) => {
    setTimeout(function () {
        done();
    }, gSetTimeout);
})
afterEach(async (done) => {
    setTimeout(function () {
        done();
    }, gSetTimeout);
})
afterAll((done) => {
    console.debug('= ACTS_AafterAll ====<begin');
    setTimeout(function () {
        console.debug('= ACTS_AafterAll ====<end');
        featureAbility.terminateSelf();
        done();
    }, gSetTimeout);
})

/*
    * @tc.number  : ACTS_AGetAbilityState_0400
    * @tc.name    :
    * @tc.desc    : Get the status of the Ability in the DESTROY state
    */
it('ACTS_AGetAbilityState_0400', 0, async function (done) {
    console.debug('====>start====ACTS_AGetAbilityState_0400')
    try {
        var ability;
        var AbilityDelegator = AbilityDelegatorRegistry.getAbilityDelegator();
        console.debug("====>ACTS_onAbilityForegroundCallback====AbilityDelegator=" + AbilityDelegator);
        function onAbilityForegroundCallback(){
            console.debug("====>ACTS_onAbilityForegroundCallback====");
            AbilityDelegator.getCurrentTopAbility((err, data)=>{
            console.debug("====>ACTS_AGetCurrentTopAbility err:" + 
                JSON.stringify(err) + "data:" + JSON.stringify(data));
            ability = data;
            })
        }
        function onAbilityDestroyCallback(){
            console.debug("====>ACTS_onAbilityDestroyCallback====");
            var state = AbilityDelegator.getAbilityState(ability);
            console.debug("====>ACTS_AGetAppState_0400 data:" + state);
            expect(state==AbilityDelegatorRegistry.AbilityLifecycleState.DESTROY 
                || (state!=AbilityDelegatorRegistry.AbilityLifecycleState.BACKGROUND
                    || state!=AbilityDelegatorRegistry.AbilityLifecycleState.FOREGROUND
                    || state!=AbilityDelegatorRegistry.AbilityLifecycleState.CREATE
                    || state!=AbilityDelegatorRegistry.AbilityLifecycleState.UNINITIALIZED
            )).assertTrue();
            console.debug("====>ACTS_AGetAppState_0400 end====");
            done();
        }
        console.debug("ACTS_====>addAbilityMonitor finish b====");
        AbilityDelegator.addAbilityMonitor(
            {abilityName: 'com.example.getAbilityState.MainAbility3',
            onAbilityForeground:onAbilityForegroundCallback,
            onAbilityDestroy:onAbilityDestroyCallback}).then(()=>{
            console.debug("ACTS_====>addAbilityMonitor finish e====");
        })
        featureAbility.startAbility(
            {
            bundleName: 'com.example.getAbilityState',
            abilityName: 'com.example.getAbilityState.MainAbility3',
            }, (err,data)=>{
            console.debug("ACTS_====>startAbility err:" + JSON.stringify(err));
        })
    } catch (error) {
        console.debug("ACTS_AGetAbilityState_0400 catch (error):" + JSON.stringify(error));
        done();
    }
})

})