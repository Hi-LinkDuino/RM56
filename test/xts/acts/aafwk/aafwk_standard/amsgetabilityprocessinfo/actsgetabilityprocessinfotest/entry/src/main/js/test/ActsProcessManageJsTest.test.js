/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import abilityManager from '@ohos.application.AbilityManager'
import featureAbility from '@ohos.ability.featureAbility'
import appManager from '@ohos.application.AppManager'

describe('ActsAbilityRunningInfosTest', function () {


    function checkRunningAbility(name, dataInfo) {
        for (let i = 0, len = dataInfo.length; i < len; i++) {
            if (dataInfo[i].ability.abilityName == name) {
                expect(dataInfo[i].ability.deviceId).assertEqual("");
                expect(dataInfo[i].ability.uri).assertEqual(undefined);
                expect(dataInfo[i].ability.ability).assertEqual(undefined);
                expect(dataInfo[i].uid).assertLarger(10000);
                expect(dataInfo[i].pid).assertLarger(500);
                expect(dataInfo[i].startTime).assertLarger(3000);
                expect(dataInfo[i].abilityState).assertLarger(0);
                expect(dataInfo[i].abilityState==abilityManager.AbilityState.FOREGROUND ||
                dataInfo[i].abilityState==abilityManager.AbilityState.BACKGROUNDING
                || (dataInfo[i].abilityState!=abilityManager.AbilityState.INITIAL
                ) || (dataInfo[i].abilityState!=abilityManager.AbilityState.FOREGROUNDING
                ) || (dataInfo[i].abilityState!=abilityManager.AbilityState.BACKGROUNDING
                )).assertTrue();
                return true;
            }
        }
        return false;
    }

    function checkRunningProcess(name, dataInfo) {
        for (let i = 0, len = dataInfo.length; i < len; i++) {
            if (dataInfo[i].processName == name) {
                expect(dataInfo[i].uid).assertLarger(2000);
                expect(dataInfo[i].pid).assertLarger(500);
                return true;
            }
        }
        return false;
    }

    /*
    * @tc.number: ACTS_getAbilityRunningInfos_0100
    * @tc.name: Application running ability information query
    * @tc.desc: Verify applications are started to query getAbilityRunningInfos ability information
    *           （by promise）
    */
    it('ACTS_getAbilityRunningInfos_0100', 0, async function (done) {
        console.log('ACTS_getAbilityRunningInfos_0100====<begin');
        var data = await abilityManager.getAbilityRunningInfos()
        console.info('====> ACTS_getAbilityRunningInfos_0100 ====>' + JSON.stringify(data))
        expect(checkRunningAbility('com.ohos.launcher.MainAbility', data)).assertTrue();
        done();
    })

    /*
    * @tc.number: ACTS_getAbilityRunningInfos_0200
    * @tc.name: Application running ability information query
    * @tc.desc: Verify applications are started to query getAbilityRunningInfos ability information
    *           （by callback）
    */
    it('ACTS_getAbilityRunningInfos_0200', 0, async function (done) {
        console.log('ACTS_getAbilityRunningInfos_0200====<begin');
        abilityManager.getAbilityRunningInfos((err,data)=>{
        console.info('====> ACTS_getAbilityRunningInfos_0200 ====>' + JSON.stringify(data))
        expect(checkRunningAbility('com.ohos.launcher.MainAbility', data)).assertTrue();
        done();
        })
    })

    /*
    * @tc.number: ACTS_getAbilityRunningInfos_0100
    * @tc.name: Application running ability information query
    * @tc.desc: Verify applications are started to query getProcessRunningInfos ability information
    *           （by promise）
    */
    it('ACTS_getProcessRunningInfos_0100', 0, async function (done) {
        console.log('ACTS_getProcessRunningInfos_0100====<begin');
        var data = await appManager.getProcessRunningInfos()
        console.info('====> ACTS_getProcessRunningInfos_0100 ====>' + JSON.stringify(data))
        expect(checkRunningProcess('com.ohos.launcher', data)).assertTrue();
        done();
    })

    /*
    * @tc.number: ACTS_getProcessRunningInfos_0200
    * @tc.name: Application running ability information query
    * @tc.desc: Verify applications are started to query getProcessRunningInfos ability information
    *           （by callback）
    */
    it('ACTS_getProcessRunningInfos_0200', 0, async function (done) {
        console.log('ACTS_getProcessRunningInfos_0200====<begin');
        appManager.getProcessRunningInfos((err, data) => {
            console.info('====> ACTS_getProcessRunningInfos_0200 ====>' + JSON.stringify(data))
            expect(checkRunningProcess('com.ohos.launcher', data)).assertTrue();
            done();
        })
    })
})
