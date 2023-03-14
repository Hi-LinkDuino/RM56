// @ts-nocheck
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

describe('appInfoTest', function () {
    console.log("*************Thermal API Test Begin*************");
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    test11();
    test12();
    test13();
    console.log("*************Thermal API Test End*************");
})

function test1() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_001
     * @tc.name Thermal_001
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_001', 0, async function (done) {
        console.info("enter");
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                let cold = thermal.getThermalLevel();
                console.info("cold level is: " + cold);
                expect(cold >= thermal.ThermalLevel.COOL && cold <= thermal.ThermalLevel.EMERGENCY).assertTrue();
                resolve();
                done();
            }, MSEC_1000 * 1);
        })
    })
}

function test2() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_002
     * @tc.name Thermal_002
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_002', 0, async function (done) {
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                let warm = thermal.getThermalLevel();
                console.info("warm level is: " + warm);
                expect(warm >= thermal.ThermalLevel.COOL && warm <= thermal.ThermalLevel.EMERGENCY).assertTrue();
                resolve();
                done();
            }, MSEC_1000 * 1);
        })
    })
}

function test3() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_003
     * @tc.name Thermal_003
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_003', 0, async function (done) {
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                let hot = thermal.getThermalLevel();
                console.info("hot level is: " + hot);
                expect(hot >= thermal.ThermalLevel.COOL && hot <= thermal.ThermalLevel.EMERGENCY).assertTrue();
                resolve();
                done();
            }, MSEC_1000 * 1);
        })
    })
}

function test4() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_004
     * @tc.name Thermal_004
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_004', 0, async function (done) {
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                let overheated = thermal.getThermalLevel();
                console.info("overheated level is: " + overheated);
                expect(overheated >= thermal.ThermalLevel.COOL &&
                    overheated <= thermal.ThermalLevel.EMERGENCY).assertTrue();
                resolve();
                done();
            }, MSEC_1000 * 1);
        })
    })
}

function test5() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_005
     * @tc.name Thermal_005
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_005', 0, async function (done) {
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                let warning = thermal.getThermalLevel();
                console.info("warning level is: " + warning);
                expect(warning >= thermal.ThermalLevel.COOL && warning <= thermal.ThermalLevel.EMERGENCY).assertTrue();
                resolve();
                done();
            }, MSEC_1000 * 1);
        })
    })
}

function test6() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_006
     * @tc.name Thermal_006
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_006', 0, async function (done) {
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                let emergency = thermal.getThermalLevel();
                console.info("emergency level is: " + emergency);
                expect(emergency >= thermal.ThermalLevel.COOL &&
                    emergency <= thermal.ThermalLevel.EMERGENCY).assertTrue();
                resolve();
                done();
            }, MSEC_1000 * 1);
        })
    })
}

function test7() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_007
     * @tc.name Thermal_007
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_007', 0, async function (done) {
        thermal.subscribeThermalLevel((cool) => {
            console.info("warm level is: " + cool);
            expect(cool >= thermal.ThermalLevel.COOL && cool <= thermal.ThermalLevel.EMERGENCY).assertTrue();
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

function test8() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_008
     * @tc.name Thermal_008
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_008', 0, async function (done) {
        thermal.subscribeThermalLevel((warm) => {
            console.info("warm level is: " + warm);
            expect(warm >= thermal.ThermalLevel.COOL && warm <= thermal.ThermalLevel.EMERGENCY).assertTrue();
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

function test9() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_009
     * @tc.name Thermal_009
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_009', 0, async function (done) {
        thermal.subscribeThermalLevel((hot) => {
            console.info("hot level is: " + hot);
            expect(hot >= thermal.ThermalLevel.COOL && hot <= thermal.ThermalLevel.EMERGENCY).assertTrue();
            done();
        })
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                thermal.unsubscribeThermalLevel(() => {
                    console.info("unsubscribe successfully!");
                });
                resolve();
            }, MSEC_1000 * 2);
        })
    })

}

function test10() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_010
     * @tc.name Thermal_010
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_010', 0, async function (done) {
        thermal.subscribeThermalLevel((overheated) => {
            console.info("overheated level is: " + overheated);
            expect(overheated >= thermal.ThermalLevel.COOL &&
                overheated <= thermal.ThermalLevel.EMERGENCY).assertTrue();
            done();
        })
        await new Promise((resolve, reject) => {
            setTimeout(() => {
                thermal.unsubscribeThermalLevel(() => {
                    console.info("unsubscribe successfully!");
                });
                resolve();
            }, MSEC_1000 * 2);
        })
    })
}

function test11() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_011
     * @tc.name Thermal_011
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_011', 0, async function (done) {
        thermal.subscribeThermalLevel((warning) => {
            console.info("warning level is: " + warning);
            expect(warning >= thermal.ThermalLevel.COOL && warning <= thermal.ThermalLevel.EMERGENCY).assertTrue();
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

function test12() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_012
     * @tc.name Thermal_012
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_012', 0, async function (done) {
        thermal.subscribeThermalLevel((emergency) => {
            console.info("emergency level is: " + emergency);
            expect(emergency >= thermal.ThermalLevel.COOL && emergency <= thermal.ThermalLevel.EMERGENCY).assertTrue();
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

function test13() {
    const MSEC_1000 = 1000;

    /* @tc.number thermal_manager_js_013
     * @tc.name Thermal_013
     * @tc.desc Thermal acquisition kit
     */
    it('Thermal_013', 0, async function (done) {
        thermal.subscribeThermalLevel((cool) => {
            console.info("cool level is: " + cool);
            expect(cool >= thermal.ThermalLevel.COOL && cool <= thermal.ThermalLevel.EMERGENCY).assertTrue();
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