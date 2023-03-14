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

import thermal from "@ohos.thermal"
import { describe, it, expect } from 'deccjsunit/index';

describe('appInfoTest', function () {
    console.log("*************Thermal Performance Test Begin*************");
    performanceTest1();
    performanceTest2();
    performanceTest3();
    console.log("*************Thermal Performance Test End*************");
})

function performanceTest1() {
    let MAXNUM = 1000;

    /**
     * @tc.number ThermalPerformance_001
     * @tc.name test get thermal level
     * @tc.desc Thermal acquisition kit
     */
    it('ThermalPerformance_001', 0, function () {
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            let level = thermal.getThermalLevel();
        }
        let waitTime = new Date().getTime() - startTime;
        let avgTime = waitTime; //us
        console.info(`ThermalPerformance_001: Promise: getlevel Wait Time :  ${waitTime}`);
    })
}

function performanceTest2() {
    let MAXNUM = 1000;

    /**
     * @tc.number ThermalPerformance_002
     * @tc.name subscribe thermal level
     * @tc.desc Thermal acquisition kit
     */
    it('ThermalPerformance_002', 0, function () {
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            thermal.subscribeThermalLevel((level) => {
                console.info("hot level is: " + level);
            })
        }
        let waitTime = new Date().getTime() - startTime;
        let avgTime = waitTime; //us
        console.info(`ThermalPerformance_002: Promise: subscribeThermalLevel Wait Time :  ${waitTime}`);
    })
}

function performanceTest3() {
    let MAXNUM = 1000;

    /**
     * @tc.number ThermalPerformance_002
     * @tc.name subscribe thermal level
     * @tc.desc Thermal acquisition kit
     */
    it('ThermalPerformance_003', 0, function () {
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            thermal.unsubscribeThermalLevel(() => {
                console.info("unsubscribe success.");
            });
        }
        let waitTime = new Date().getTime() - startTime;
        let avgTime = waitTime; //us
        console.info(`ThermalPerformance_003: Promise: unsubscribeThermalLevel Wait Time :  ${waitTime}`);
    })
}
