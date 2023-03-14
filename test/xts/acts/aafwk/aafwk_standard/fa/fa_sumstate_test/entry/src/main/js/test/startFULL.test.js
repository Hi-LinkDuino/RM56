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

var assistData
var arr1 = []
var AbilityStateChanged1 = []
var ForegroundApplication1 = []
var testData
var appbefore
var appafter
describe("appManagerTest", function () {
    console.log("---appManagerTest-- starting---");

    /**
     * @tc.number: SUB_AA_OpenHarmony_StateListener_3200
     * @tc.name: Verify the onProcessDied callback
     * @tc.desc: Function test
     * @tc.level   0
     */
    it("SUB_AA_OpenHarmony_StateListener_3200", 0, async function (done) {
        console.log("---start SUB_AA_OpenHarmony_StateListener_3200-----");
        await sleep(1000)
        console.log("---start SUB_AA_OpenHarmony_StateListener_3200-----");
        var rest = appManager.registerApplicationStateObserver({
            onForegroundApplicationChanged: (aapData) => {
                console.log("SUB_onForegroundApplicationChanged" + JSON.stringify(aapData))
                ForegroundApplication1.push(aapData);
                console.log('ForegroundApplication :' + JSON.stringify(ForegroundApplication1))
            },
            onAbilityStateChanged: (StateData) => {
                console.log("==onAbilityStateChanged1==>" + JSON.stringify(StateData))
                AbilityStateChanged1.push(StateData)
                console.log('AbilityStateChanged1:' + JSON.stringify(AbilityStateChanged1))
            },
            onProcessCreated: (CreateData) => {
                console.log("SUB_onProcessCreated" + JSON.stringify(CreateData))
                arr1.push('onProcessCreated', CreateData)
                console.log('arr2:' + JSON.stringify(arr1))
            },
            onProcessDied: (ProcessData) => {
                console.log("SUB_onProcessDied" + JSON.stringify(ProcessData))
                arr1.push('onProcessDied', ProcessData)
                console.log("SUB_onProcessDied" + JSON.stringify(arr1))
                console.log('arr3:' + JSON.stringify(arr1))
            }
        })
        await sleep(400);
        var want = {
            "bundleName": "com.example.fzstatelistener",
            "abilityName": "com.example.fzstatelistener.MainAbility"
        }
        await featureAbility.startAbility({ want }).then().catch();
        await sleep(1000);
        console.log('AbilityStateChanged1===>' + JSON.stringify(AbilityStateChanged1))
        console.log('ForegroundApplication1===>' + JSON.stringify(ForegroundApplication1))
        checkArr(arr1, AbilityStateChanged1, ForegroundApplication1)
        done();
    })

    function checkArr(arr, AbilityStateChanged, ForegroundApplication) {
        console.log('==app1==:' + AbilityStateChanged.length)
        if (arr == [] || AbilityStateChanged == [] || ForegroundApplication == []) {
            expect().assertFail()
        }
        console.log('==check process==>strat' + JSON.stringify(arr))
        let process = getProcessData(arr, 'onProcessCreated')
        console.log(' processlist :' + JSON.stringify(process))
        for (let i = 0;i < process.length; i++) {
            console.log('==check process==> process[i] :' + JSON.stringify(process[i]))
            if (process[i].bundleName == 'com.example.fzstatelistener') {
                assistData = process[i]
                console.log('==assistData==>' + JSON.stringify(assistData))
            }
        }

        checkProcessData(assistData)
        console.log('==app==:' + AbilityStateChanged.length)
        for (let i = 0;i < AbilityStateChanged.length; i++) {
            console.log('==appState==> :' + JSON.stringify(AbilityStateChanged[i]))
            if (AbilityStateChanged[i].bundleName == 'com.example.fzstatelistener') {
                assistData = AbilityStateChanged[i]
                console.log('==check assistData==> :' + JSON.stringify(assistData))
            }
            else if (AbilityStateChanged[i].bundleName == 'com.example.sum_statelistener') {
                testData = AbilityStateChanged[i]
                console.log('==check testData==> :' + JSON.stringify(testData))

            }
        }
        if (testData == undefined || assistData == undefined) {
            console.log('testData||assistData is null')
            expect().assertFail()
        }
        checkAbilityStateData(assistData)
        checkAbilityStateData2(testData)
        console.log('==app==:' + ForegroundApplication.length)
        for (let i = 0;i < ForegroundApplication.length; i++) {
            console.log('==abilityState==>' + JSON.stringify(ForegroundApplication[i]))
            if (ForegroundApplication[i].bundleName == 'com.example.fzstatelistener') {
                assistData = ForegroundApplication[i]
                console.log('==check assistData2==> :' + JSON.stringify(assistData))

            }
            else if (ForegroundApplication[i].bundleName == 'com.example.sum_statelistener') {
                testData = ForegroundApplication[i]
                console.log('==check testData2==> :' + JSON.stringify(testData))
            }
        }
        if (testData == undefined || assistData == undefined) {
            console.log('testData||assistData is null')
            expect().assertFail()
        }
        checkAppStateData(assistData)
        checkAppStateData2(testData)
    }

    function checkAbilityStateData2(list) {
        console.log('==checkAbilityStateData2==>' + JSON.stringify(list))
        expect('com.example.sum_statelistener').assertEqual(list.bundleName)
        expect('com.example.sum_statelistener.MainAbility').assertEqual(list.abilityName)
        expect(list.pid > 0).assertTrue()
        expect(list.uid > 0).assertTrue()
        expect(3).assertEqual(list.state)
    }

    function checkAbilityStateData(list) {
        console.log('==checkAbilityStateData==>' + JSON.stringify(list))
        expect('com.example.fzstatelistener').assertEqual(list.bundleName)
        expect('com.example.fzstatelistener.MainAbility').assertEqual(list.abilityName)
        expect(list.pid > 0).assertTrue()
        expect(list.uid > 0).assertTrue()
        expect(2).assertEqual(list.state)
    }

    function checkAppStateData2(list) {
        console.log('==checkAppStateData2==>' + JSON.stringify(list))
        expect('com.example.sum_statelistener').assertEqual(list.bundleName)
        expect(list.uid > 0).assertTrue()
        expect(3).assertEqual(list.state)
    }

    function checkAppStateData(list) {
        console.log('==checkAppStateData==>' + JSON.stringify(list))
        expect('com.example.fzstatelistener').assertEqual(list.bundleName)
        expect(list.uid > 0).assertTrue()
        expect(2).assertEqual(list.state)
    }

    function checkProcessData(list) {
        console.log('==dhhFdh==>' + JSON.stringify(list))
        expect('com.example.fzstatelistener').assertEqual(list.bundleName)
        console.log('==list.bundleName==>' + JSON.stringify(list.bundleName))
        expect(list.pid > 0).assertTrue()
        console.log('==list.pid==>' + JSON.stringify(list.pid))
        expect(list.uid > 0).assertTrue()
        console.log('==list.uid==>' + JSON.stringify(list.uid))
    }

    function getProcessData(reslist, key) {
        console.log('getProcessData---start' + JSON.stringify(reslist))
        var listofkey = [];
        for (let i = 0;i < reslist.length; i++) {
            console.log('==getProcessData==> reslist[i] : ' + JSON.stringify(reslist[i]))
            var callback = reslist[i]
            console.log('==callback==>  : ' + JSON.stringify(callback))
            if (callback == key) {
                listofkey.push(reslist[i+1])
                console.log('==reslist[i+1]==>  : ' + JSON.stringify(reslist[i+1]))
            }
        }
        console.log('getProcessData' + JSON.stringify(listofkey))
        return listofkey
    }

    function sleep(time) {
        return new Promise((resolve) => setTimeout(resolve, time))
    }

})






