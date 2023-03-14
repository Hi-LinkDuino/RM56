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
import sensor from '@ohos.sensor'

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe("SensorJsTest", function () {
    beforeAll(function () {

        /*
         * @tc.setup: setup invoked before all testcases
         */
        console.info('beforeAll caled')
    })

    afterAll(function () {

        /*
         * @tc.teardown: teardown invoked after all testcases
         */
        console.info('afterAll caled')
    })

    beforeEach(function () {

        /*
         * @tc.setup: setup invoked before each testcases
         */
        console.info('beforeEach caled')
    })

    afterEach(function () {

        /*
         * @tc.teardown: teardown invoked after each testcases
         */
        console.info('afterEach caled')
    })

    /*
     * @tc.number: SUB_SensorsSystem_Gyroscope_Uncalibrated_JSTest_0010
     * @tc.name: SensorGyroscopeUncalibratedJSTest001
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Gyroscope_Uncalibrated_JSTest_0010", 0, async function (done) {
        console.info('-----------------SUB_SensorsSystem_Gyroscope_Uncalibrated_JSTest_0010----------------------');
        function offPromise() {
            return new Promise((resolve, reject) => {
                sensor.off(sensor.SensorType.SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED, (error) => {
                    if (error) {
                        console.info('SensorGyroscopeUncalibratedJSTest001  off error');
                        expect(false).assertTrue();
                        console.info('setTimeout ..start')
                        setTimeout((err) => {
                            reject(err);
                        }, 500);
                    } else {
                        console.info('SensorGyroscopeUncalibratedJSTest001  off success');
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }
                }, 1000)
            })
        }

        let promise = new Promise((resolve, reject) => {
            sensor.on(sensor.SensorType.SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED, function (error, data) {
                if (error) {
                    console.info('SensorGyroscopeUncalibratedJSTest001  on error');
                    expect(false).assertTrue();
                    setTimeout((err) => {
                        reject(err);
                    }, 500);
                } else {
                    console.info('SensorGyroscopeUncalibratedJSTest001  on  success, x: ' + data.x + "y: " 
                        + data.y + "z: " 
                        + data.z + "biasX:" + data.biasX + "biasY: " + data.biasY + "biasZ: " + data.biasZ);
                    expect(typeof (data.x)).assertEqual("number");
                    expect(typeof (data.y)).assertEqual("number");
                    expect(typeof (data.z)).assertEqual("number");
                    expect(typeof (data.biasX)).assertEqual("number");
                    expect(typeof (data.biasY)).assertEqual("number");
                    expect(typeof (data.biasZ)).assertEqual("number");
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }
            });
        })

        await promise.then(() => {
            return offPromise();
        }, () => {
            console.info("SensorGyroscopeUncalibratedJSTest001 reject");
        })
        done();
    })

    /*
     * @tc.number: SUB_SensorsSystem_Gyroscope_Uncalibrated_JSTest_0050
     * @tc.name: SensorGyroscopeUncalibratedJSTest005
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Gyroscope_Uncalibrated_JSTest_0050", 0, async function (done) {
        function onceSensorCallback(error, data) {
            if (error) {
                console.info('SensorGyroscopeUncalibratedJSTest005  once error');
                expect(false).assertTrue();
            } else {
                console.info('SensorGyroscopeUncalibratedJSTest005  on  success, x: ' + data.x + "y: " + data.y + "z: " 
                    + data.z + "biasX:" + data.biasX + "biasY: " + data.biasY + "biasZ: " + data.biasZ);
                expect(typeof (data.x)).assertEqual("number");
                expect(typeof (data.y)).assertEqual("number");
                expect(typeof (data.z)).assertEqual("number");
                expect(typeof (data.biasX)).assertEqual("number");
                expect(typeof (data.biasY)).assertEqual("number");
                expect(typeof (data.biasZ)).assertEqual("number");
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        sensor.once(sensor.SensorType.SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED, onceSensorCallback);
    })
})
