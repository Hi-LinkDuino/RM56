// @ts-nocheck
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

import commonEvent from '@ohos.commonevent'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index";
import appManager from '@ohos.application.appManager';
import abilityManager from '@ohos.application.abilityManager';

const START_ABILITY_TIMEOUT = 5000;
describe('CoverApiTest', function () {
    console.log("CoverApiTest --- start");
    var subscriberInfoMainAbility = {
        events: ["mainAbilityOnConfigurationUpdated"]
    }

    /*
     * @tc.number  SUB_AA_OpenHarmony_CoverApi_0500
     * @tc.name    whether the system is under stability test
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_CoverApi_0500', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_CoverApi_0500-------------");
        appManager.isRunningInStabilityTest()
            .then((data) => {
                console.info('SUB_AA_OpenHarmony_CoverApi_0500 successful. data: ' + JSON.stringify(data));
                expect(data == false).assertTrue();
                done();
            }).catch((error) => {
            console.error('SUB_AA_OpenHarmony_CoverApi_0500 failed. error: ' + JSON.stringify(error));
            expect().assertFalse();
            done();
        })
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_CoverApi_0700
     * @tc.name    Verify that the data of nonexistent apps is cleared
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_CoverApi_0700', 0, async function (done) {
        console.log("SUB_AA_OpenHarmony_CoverApi_0700 --- start");
        appManager.clearUpApplicationData('com.example.coverapi')
            .then((data) => {
                console.info('SUB_AA_OpenHarmony_CoverApi_0700 successful. data: ' + JSON.stringify(data));
                expect(data == 0).assertTrue();
                done();
            }).catch((error) => {
            console.error('SUB_AA_OpenHarmony_CoverApi_0700 failed. error: ' + JSON.stringify(error));
            expect().assertFalse();
            done();
        })
    })

    /**
     * @tc.number: SUB_AA_OpenHarmony_CoverApi_0500
     * @tc.name: wantConstant : Check specific enum
     * @tc.desc: Check the return type of the interface (by Promise)
     */
    it('SUB_AA_OpenHarmony_CoverApi_0800', 0, async function (done) {
        console.log("SUB_AA_OpenHarmony_CoverApi_0800 --- start");

        var Subscriber;
        let id;

        function SubscribeCallBack(err, data) {
            console.debug("SUB_AA_OpenHarmony_CoverApi_0800====>Subscribe CallBack data:====>" + JSON.stringify(data));
            expect(data.event == 'mainAbilityOnConfigurationUpdated').assertTrue();
            if (data.event == 'mainAbility_onConfigurationUpdated') {
                clearTimeout(id);
                commonEvent.unsubscribe(Subscriber, UnSubscribeCallback);
                done();
            }
        }

        commonEvent.createSubscriber(subscriberInfoMainAbility).then(async (data) => {
            console.debug("SUB_AA_OpenHarmony_CoverApi_0800====>Create Subscriber====>");
            Subscriber = data;
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        })


        function UnSubscribeCallback() {
            console.debug("SUB_AA_OpenHarmony_CoverApi_0800====>UnSubscribe CallBack====>");
            done();
        }

        function timeout() {
            expect().assertFail();
            console.debug('SUB_AA_OpenHarmony_CoverApi_0800 - timeout');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback)
            done();
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);

        var Configuration = { language: "en" };
        abilityManager.updateConfiguration(Configuration)
            .then((data) => {
                console.log("SUB_AA_OpenHarmony_CoverApi_0800 --- updateConfiguration " + JSON.stringify(data));
                done();
            }).catch((error) => {
            console.error('SUB_AA_OpenHarmony_CoverApi_0800 failed. error: ' + JSON.stringify(error));
            done();
        })

        console.log("SUB_AA_OpenHarmony_CoverApi_0800 --- end");
    })

})
