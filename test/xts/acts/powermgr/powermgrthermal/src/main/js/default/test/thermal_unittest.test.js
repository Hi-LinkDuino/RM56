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

import app from '@system.app'
import thermal from "@ohos.thermal"
import ThermalLevel from "@ohos.thermal"
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
const MSEC_1000 = 1000;

describe('appInfoTest', function () {
    console.log("*************Thermal API Test Begin*************");
    test14();
    test15();
})

function test14() {

    /* @tc.number thermal_manager_js_014
     * @tc.name Thermal_014
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_014', 0, async function (done) {
        console.info("enter");
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                let level = thermal.getThermalLevel();
                console.info("level is: " + level);
                expect(level == -1).assertTrue();
                resolve();
                done();
            }, MSEC_1000 * 4);
        })
    })
}

function test15() {

    /* @tc.number thermal_manager_js_015
     * @tc.name Thermal_015
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_015', 0, async function (done) {
        thermal.subscribeThermalLevel((level) => {
            console.info("level is: " + level);
            expect(level == -1).assertTrue();
            done();
        })
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                thermal.unsubscribeThermalLevel(() => {
                    console.info("unsubscribe successfully!");
                });
                resolve();
            }, MSEC_1000 * 4);
        })
    })
}