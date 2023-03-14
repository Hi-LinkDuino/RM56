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
import appManager from '@ohos.application.appManager';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index"

var subscriberInfoMainAbility2 = {
    events: ["SUB_AA_OpenHarmony_StateListener_0300_CommonEvent"],
};


describe("appManagerTest", function () {
    console.log("----appManagerTest is starting------");
    
    /**
     * @tc.number: SUB_AA_OpenHarmony_StateListener_0200
     * @tc.name: Re-call deregister listener interface validation
     * @tc.desc: Function test
     * @tc.level   0
     */
    it("SUB_AA_OpenHarmony_StateListener_0200", 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_StateListener_0200-------------");
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
        console.log('SUB_AA_OpenHarmony_StateListener_0200 registerId  : ' + JSON.stringify(registerId));
        console.log('SUB_AA_OpenHarmony_StateListener_0200 registerId  : ' + typeof registerId);
        expect(typeof(registerId) == "number").assertTrue();

        await appManager.unregisterApplicationStateObserver(registerId, async(err, data) => {
            console.log("SUB_AA_OpenHarmony_StateListener_0200 err1:"
            + JSON.stringify(err) + ",data: " + JSON.stringify(data));
        })
        await appManager.unregisterApplicationStateObserver(registerId, async(err, data) => {
            console.log("SUB_AA_OpenHarmony_StateListener_0200 err2:"
            + JSON.stringify(err) + ",data: " + JSON.stringify(data));
            expect(err.code == 1).assertTrue();
            done();
        })

    })


})




