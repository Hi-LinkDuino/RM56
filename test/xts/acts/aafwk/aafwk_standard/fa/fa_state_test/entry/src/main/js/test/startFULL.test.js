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
import appManager from '@ohos.application.appManager';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index"
import featureAbility from '@ohos.ability.featureAbility';
import abilityfeatureAbility from '@ohos.ability.featureAbility';
var appbefore
var appafter
describe("appManagerTest", function () {
    console.log("---appManagerTest-- starting---");
    function sleep(time) {
        return new Promise((resolve) => setTimeout(resolve, time))
    }

    /**
     * @tc.number: SUB_AA_OpenHarmony_StateListener_0300
     * @tc.name: Find all foreground application validations
     * @tc.desc: Function test
     * @tc.level   0
     */
    it("SUB_AA_OpenHarmony_StateListener_0300", 0, async function (done) {
      await  appManager.getForegroundApplications().then((data) => {
          console.log("---appM- starting---");
            for (var i = 0; i < data.length; i++) {
                if (data[i].bundleName == "com.example.statelistener") {
                    appbefore = data[i]
                }
            }
            checkAppBefore(appbefore)
        }).catch(()=>{
            expect().assertFail()
        })
        var want = {
            "bundleName": "com.example.fzgc",
            "abilityName": "com.example.fzgc.MainAbility"
        }
        await featureAbility.startAbility({ want }).then().catch();
        await sleep(1000)
        await  appManager.getForegroundApplications().then((data) => {
            for (var i = 0; i < data.length; i++) {
                if (data[i].bundleName == "com.example.fzgc") {
                    appafter = data[i]
                }
            }
            checkAppAfter(appafter)
        }).catch(()=>{
            expect().assertFail()
        })
        await sleep(2000)
        done();
    })

    function checkAppAfter(app) {
        expect(app.state == 2).assertTrue();
        expect(app.uid > 0).assertTrue();
        expect(app.bundleName).assertEqual('com.example.fzgc');
    }
    function checkAppBefore(app) {
        expect(app.state == 2).assertTrue();
        expect(app.uid > 0).assertTrue();
        expect(app.bundleName).assertEqual('com.example.statelistener');
    }
})






