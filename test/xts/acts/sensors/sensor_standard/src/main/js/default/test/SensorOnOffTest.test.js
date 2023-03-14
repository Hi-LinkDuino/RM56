/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// @ts-nocheck

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import sensor from '@ohos.sensor'

function sleep(NumberMillis) {
    let now = new Date()
    let exitTime = now.getTime() + NumberMillis
    while (true) {
        now = new Date()
        if (now.getTime > exitTime) {
            return
        }
    }
}

describe('SystemParameterTest', function () {
    beforeAll(function () {
        console.info('beforeAll caled')
    })

    afterAll(function () {
        console.info('afterAll caled')
    })

    beforeEach(function () {
        console.info('beforeEach caled')
    })

    afterEach(function () {
        console.info('afterEach caled')
    })

    let testSensorId = 0;
    let testNullSensorId = -1;

    /**
     * @tc.number    SUB_SENSORS_Sensor_JSTest_0010
     * @tc.name      testRegisterSensortest001
     * @tc.desc      test get sensor data by sensor id.
     */
    it('SUB_SENSORS_Sensor_JSTest_0010', 0, async function (done) {
        console.info('SUB_SENSORS_Sensor_JSTest_0010 start');
        await sensor.on(testSensorId, function (data) {
            if (data) {
                console.info('testRegisterSensortest001  on success');
                expect(data.x).assertInstanceOf('Number');
                expect(data.timestamp).assertInstanceOf('Number');
                done();
            } else {
                console.info('testRegisterSensortest001  on error');
                expect(false).assertTrue();
                done();
            }
        });
        console.info('SUB_SENSORS_Sensor_JSTest_0010 end');
    })

    /**
     * @tc.number    SUB_SENSORS_Sensor_JSTest_0020
     * @tc.name      testRegisterSensortest002
     * @tc.desc      test get sensor data by wrong sensor id.
     */
    it('SUB_SENSORS_Sensor_JSTest_0020', 0, async function (done) {
        console.info('SUB_SENSORS_Sensor_JSTest_0020 start');
        await sensor.on(testNullSensorId, function (data) {
            if (data) {
                console.info('testRegisterSensortest002  on success');
                expect(false).assertTrue();
                done();
            }
        });
        setTimeout(() => {
            expect(true).assertTrue();
            done();
        }, 500)
        console.info('SUB_SENSORS_Sensor_JSTest_0020 end');
    })

})