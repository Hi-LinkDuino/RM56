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

    let GEOMAGNETIC_COMPONENT_YEAR_RESULT = [
        [6570.3935546875, -146.3289337158203, 54606.0078125, -1.2758207321166992, 83.13726043701172, 6572.02294921875,
            55000.0703125],
        [6554.17041015625, -87.19947052001953, 54649.078125, -0.7622424364089966, 83.16046905517578, 6554.75048828125,
            55040.7734375],
        [6537.99169921875, -28.231582641601562, 54692.02734375, -0.24740631878376007, 83.18303680419922,
            6538.052734375, 55081.4296875],
        [6521.81201171875, 30.73670768737793, 54734.97265625, 0.2700277864933014, 83.20502471923828, 6521.88427734375,
            55122.15625],
        [6505.6328125, 89.70511627197266, 54777.90625, 0.7899921536445618, 83.22642517089844, 6506.2509765625,
            55162.9453125]]

    let GEOMAGNETIC_COMPONENT_COORDINATES_RESULT = [
        [6570.3935546875, -146.3289337158203, 54606.0078125, -1.2758207321166992, 83.13726043701172, 6572.02294921875,
            55000.0703125],
        [39624.28125, 109.8766098022461, -10932.4638671875, 0.15887857973575592, -15.424291610717773, 39624.43359375,
            41104.921875],
        [37636.72265625, 104.90892791748047, -10474.810546875, 0.15970633924007416, -15.552550315856934, 37636.8671875,
            39067.3203125],
        [5940.583984375, 15772.0927734375, -52480.7578125, 69.36103820800781, -72.19599914550781, 16853.765625,
            55120.58984375],
        [5744.87255859375, 14799.48046875, -49969.40234375, 68.78474426269531, -72.37483215332031, 15875.3955078125,
            52430.61328125]]

    let GEOMAGNETIC_COORDINATES = [[80, 0, 0],
    [0, 120, 0],
    [0, 120, 100000],
    [-80, 240, 0],
    [-80, 240, 100000]]

    let timeMillis = [1580486400000, 1612108800000, 1643644800000, 1675180800000, 1706716800000]

    /**
    * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0010
    * @tc.name: SensorGeomagenticAlgorithmJSTest001
    * @tc.desc: Verification results of the incorrect parameters of the test interface.
    */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0010', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0010-------------------------");
        let promiseArray = []
        for (let i = 0; i < timeMillis.length; i++) {
            promiseArray.push(new Promise((resolve, reject) => {
                let j = i
                sensor.getGeomagneticField({ 'latitude': 80, 'longitude': 0, 'altitude': 0 }, timeMillis[j],
                    (error, data) => {
                        if (error) {
                            console.info('SensorGeomagenticAlgorithmJSTest001 failed');
                            expect(false).assertTrue();
                            setTimeout((err) => {
                                reject(err)
                            }, 500)
                        } else {
                            console.info('SensorGeomagenticAlgorithmJSTest001 success x: ' + data.x + ',y: '
                                + data.y + ',z: ' + data.z + ',geomagneticDip: ' + data.geomagneticDip
                                + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: '
                                + data.levelIntensity + ',totalIntensity: ' + data.totalIntensity)
                            expect(data.x).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[j][0])
                            expect(data.y).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[j][1])
                            expect(data.z).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[j][2])
                            expect(data.deflectionAngle).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[j][3])
                            expect(data.geomagneticDip).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[j][4])
                            expect(data.levelIntensity).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[j][5])
                            expect(data.totalIntensity).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[j][6])
                            setTimeout(() => {
                                resolve()
                            }, 500)
                        }
                    })
            }))
        }
        Promise.all(promiseArray).then(done)
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0020
     * @tc.name: SensorGeomagenticAlgorithmJSTest002
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0020', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0020-------------------------");
        let promiseArray = []
        for (let i = 0; i < GEOMAGNETIC_COORDINATES.length; i++) {
            promiseArray.push(new Promise((resolve, reject) => {
                let j = i
                sensor.getGeomagneticField({
                    'latitude': GEOMAGNETIC_COORDINATES[j][0],
                    'longitude': GEOMAGNETIC_COORDINATES[j][1], 'altitude': GEOMAGNETIC_COORDINATES[j][2]
                }, timeMillis[0], (error, data) => {
                    if (error) {
                        console.info('SensorGeomagenticAlgorithmJSTest002 failed');
                        expect(false).assertTrue();
                        setTimeout((err) => {
                            reject(err)
                        }, 500)
                    } else {
                        console.info('SensorGeomagenticAlgorithmJSTest002 success x: ' + data.x + ',y: '
                            + data.y + ',z: '
                            + data.z + ',geomagneticDip: ' + data.geomagneticDip
                            + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                            + ',totalIntensity: ' + data.totalIntensity)
                        expect(data.x).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[j][0])
                        expect(data.y).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[j][1])
                        expect(data.z).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[j][2])
                        expect(data.deflectionAngle).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[j][3])
                        expect(data.geomagneticDip).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[j][4])
                        expect(data.levelIntensity).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[j][5])
                        expect(data.totalIntensity).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[j][6])
                        setTimeout(() => {
                            resolve()
                        }, 500)
                    }
                })
            }))
        }
        Promise.all(promiseArray).then(done)
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0030
     * @tc.name: SensorGeomagenticAlgorithmJSTest003
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0030', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0030-------------------------");
        let geomagneticComponent = [27779.234375, -6214.9794921875, -14924.6611328125, -27.667943954467773,
            -12.610970497131348, 28465.9765625, 32141.2109375]
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': 0 }, Number.MAX_VALUE,
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest003 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest003 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(data.x).assertEqual(geomagneticComponent[0])
                    expect(data.y).assertEqual(geomagneticComponent[1])
                    expect(data.z).assertEqual(geomagneticComponent[2])
                    expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                    expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                    expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                    expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0040
     * @tc.name: SensorGeomagenticAlgorithmJSTest004
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0040', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0040-------------------------");
        let geomagneticComponent = [27779.234375, -6214.9794921875, -14924.6611328125, -27.667943954467773,
            -12.610970497131348, 28465.9765625, 32141.2109375]
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': 0 }, Number.MIN_VALUE,
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest004 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest004 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(data.x).assertEqual(geomagneticComponent[0])
                    expect(data.y).assertEqual(geomagneticComponent[1])
                    expect(data.z).assertEqual(geomagneticComponent[2])
                    expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                    expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                    expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                    expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0050
     * @tc.name: SensorGeomagenticAlgorithmJSTest005
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0050', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0050-------------------------");
        let geomagneticComponent = [1824.141845703125, 116.58167266845703, 56727.7734375, 88.15447235107422,
            3.6568238735198975, 1827.8634033203125, 56757.21484375]
        sensor.getGeomagneticField({ 'latitude': Number.MAX_VALUE, 'longitude': 0, 'altitude': 0 },
            timeMillis[0], (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest005 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest005 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(data.x).assertEqual(geomagneticComponent[0])
                    expect(data.y).assertEqual(geomagneticComponent[1])
                    expect(data.z).assertEqual(geomagneticComponent[2])
                    expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                    expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                    expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                    expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0060
     * @tc.name: SensorGeomagenticAlgorithmJSTest006
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0060', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0060-------------------------");
        let geomagneticComponent = [1824.141845703125, 116.58167266845703, 56727.7734375, 88.15447235107422,
            3.6568238735198975, 1827.8634033203125, 56757.21484375]
        sensor.getGeomagneticField({ 'latitude': Number.NaN, 'longitude': 0, 'altitude': 0 }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest006 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest006 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(data.x).assertEqual(geomagneticComponent[0])
                    expect(data.y).assertEqual(geomagneticComponent[1])
                    expect(data.z).assertEqual(geomagneticComponent[2])
                    expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                    expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                    expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                    expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0070
     * @tc.name: SensorGeomagenticAlgorithmJSTest007
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0070', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0070-------------------------");
        let geomagneticComponent = [14425.57421875, -17156.767578125, -52023.21484375, -66.69005584716797,
            -49.94255447387695, 22415.4375, 56646.859375]
        sensor.getGeomagneticField({ 'latitude': Number.NEGATIVE_INFINITY, 'longitude': 0, 'altitude': 0 },
            timeMillis[0], (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest007 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest007 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(data.x).assertEqual(geomagneticComponent[0])
                    expect(data.y).assertEqual(geomagneticComponent[1])
                    expect(data.z).assertEqual(geomagneticComponent[2])
                    expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                    expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                    expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                    expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0080
     * @tc.name: SensorGeomagenticAlgorithmJSTest008
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0080', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0080-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.MAX_VALUE, 'altitude': 0 }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest008 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest008 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue();
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0090
     * @tc.name: SensorGeomagenticAlgorithmJSTest009
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0090', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0090-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NaN, 'altitude': 0 }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest009 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest009 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0100
     * @tc.name: SensorGeomagenticAlgorithmJSTest010
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0100', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0100-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NEGATIVE_INFINITY, 'altitude': 0 },
            timeMillis[0], (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest010 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest010 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0110
     * @tc.name: SensorGeomagenticAlgorithmJSTest011
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0110', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0110-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.MAX_VALUE }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest011 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest011 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0120
     * @tc.name: SensorGeomagenticAlgorithmJSTest012
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0120', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0120-------------------------");
        let geomagneticComponent = [27536.40234375, -2248.586669921875, -16022.4306640625, -30.110872268676758,
            -4.66834020614624, 27628.05859375, 31937.875]
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.MIN_VALUE }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest012 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest012 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(data.x).assertEqual(geomagneticComponent[0])
                    expect(data.y).assertEqual(geomagneticComponent[1])
                    expect(data.z).assertEqual(geomagneticComponent[2])
                    expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                    expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                    expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                    expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0130
     * @tc.name: SensorGeomagenticAlgorithmJSTest013
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0130', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0130-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NaN }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest013 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest013 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0140
     * @tc.name: SensorGeomagenticAlgorithmJSTest014
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0140', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0140-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NEGATIVE_INFINITY },
            timeMillis[0], (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest014 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest014 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0150
     * @tc.name: SensorGeomagenticAlgorithmJSTest015
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0150', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0150-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NaN, 'altitude': 0 }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest015 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest015 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0160
     * @tc.name: SensorGeomagenticAlgorithmJSTest016
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0160', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0160-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NEGATIVE_INFINITY, 'altitude': 0 },
            timeMillis[0], (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest016 once success');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest016 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0170
     * @tc.name: SensorGeomagenticAlgorithmJSTest017
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0170', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0170-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.MAX_VALUE }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest017 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest017 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0180
     * @tc.name: SensorGeomagenticAlgorithmJSTest018
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0180', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0180-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NaN }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest018 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest018 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0190
     * @tc.name: SensorGeomagenticAlgorithmJSTest019
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0190', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0190-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NEGATIVE_INFINITY },
            timeMillis[0], (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest019 failed');
                    expect(false).assertfalse();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest019 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0200
     * @tc.name: SensorGeomagenticAlgorithmJSTest020
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0200', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0200-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.MAX_VALUE, 'altitude': 0 }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest020 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest020 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue();
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0210
     * @tc.name: SensorGeomagenticAlgorithmJSTest021
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0210', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0210-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NaN, 'altitude': 0 }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest021 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest021 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0220
     * @tc.name: SensorGeomagenticAlgorithmJSTest022
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0220', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0220-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NEGATIVE_INFINITY, 'altitude': 0 },
            timeMillis[0], (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest022 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest022 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0230
     * @tc.name: SensorGeomagenticAlgorithmJSTest023
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0230', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0230-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.MAX_VALUE }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest023 failed');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest023 success x: ' + data.x + ',y: ' + data.y
                        + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0240
     * @tc.name: SensorGeomagenticAlgorithmJSTest024
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0240', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0240-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NaN }, timeMillis[0],
            (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest024 once success');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest024 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0250
     * @tc.name: SensorGeomagenticAlgorithmJSTest025
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0250', 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0250-------------------------");
        sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NEGATIVE_INFINITY },
            timeMillis[0], (error, data) => {
                if (error) {
                    console.info('SensorGeomagenticAlgorithmJSTest025 once success');
                    expect(false).assertTrue();
                } else {
                    console.info('SensorGeomagenticAlgorithmJSTest025 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue()
                }
                setTimeout(() => {
                    done()
                }, 500)
            })
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0260
     * @tc.name: SensorGeomagenticAlgorithmJSTest026
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0260", 0, async function (done) {
        console.info("------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0260-------------------------");
        for (let i = 0; i < timeMillis.length; i++) {
            await sensor.getGeomagneticField({ 'latitude': 80, 'longitude': 0, 'altitude': 0 },
                timeMillis[i]).then((data) => {
                    console.info('SensorGeomagenticAlgorithmJSTest026 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity);
                    expect(data.x).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][0])
                    expect(data.y).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][1])
                    expect(data.z).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][2])
                    expect(data.deflectionAngle).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][3])
                    expect(data.geomagneticDip).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][4])
                    expect(data.levelIntensity).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][5])
                    expect(data.totalIntensity).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][6])
                }).catch((error) => {
                    console.info("promise::catch", error);
                })
        }
        done()
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0270
     * @tc.name: SensorGeomagenticAlgorithmJSTest027
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0270", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0270------------------');
        let geomagneticComponent = [27779.234375, -6214.9794921875, -14924.6611328125, -27.667943954467773,
            -12.610970497131348, 28465.9765625, 32141.2109375]
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': 0 },
            Number.MAX_VALUE).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest027 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(data.x).assertEqual(geomagneticComponent[0])
                expect(data.y).assertEqual(geomagneticComponent[1])
                expect(data.z).assertEqual(geomagneticComponent[2])
                expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0280
     * @tc.name: SensorGeomagenticAlgorithmJSTest028
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0280", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0280------------------');
        let geomagneticComponent = [27779.234375, -6214.9794921875, -14924.6611328125, -27.667943954467773,
            -12.610970497131348, 28465.9765625, 32141.2109375]
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': 0 },
            Number.MIN_VALUE).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest028 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(data.x).assertEqual(geomagneticComponent[0])
                expect(data.y).assertEqual(geomagneticComponent[1])
                expect(data.z).assertEqual(geomagneticComponent[2])
                expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
            }).catch((error) => {
                console.info("promise::catch", error);
            });
        done()
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0290
     * @tc.name: SensorGeomagenticAlgorithmJSTest029
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0290", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0290------------------');
        let geomagneticComponent = [1824.141845703125, 116.58167266845703, 56727.7734375, 88.15447235107422,
            3.6568238735198975, 1827.8634033203125, 56757.21484375]
        await sensor.getGeomagneticField({ 'latitude': Number.MAX_VALUE, 'longitude': 0, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest029 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(data.x).assertEqual(geomagneticComponent[0])
                expect(data.y).assertEqual(geomagneticComponent[1])
                expect(data.z).assertEqual(geomagneticComponent[2])
                expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0300
     * @tc.name: SensorGeomagenticAlgorithmJSTest030
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0300", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0300------------------');
        let geomagneticComponent = [1824.141845703125, 116.58167266845703, 56727.7734375, 88.15447235107422,
            3.6568238735198975, 1827.8634033203125, 56757.21484375]
        await sensor.getGeomagneticField({ 'latitude': Number.NaN, 'longitude': 0, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest030 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(data.x).assertEqual(geomagneticComponent[0])
                expect(data.y).assertEqual(geomagneticComponent[1])
                expect(data.z).assertEqual(geomagneticComponent[2])
                expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /**
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0310
     * @tc.name: SensorGeomagenticAlgorithmJSTest031
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0310", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0310------------------');
        let geomagneticComponent = [14425.57421875, -17156.767578125, -52023.21484375, -66.69005584716797,
            -49.94255447387695, 22415.4375, 56646.859375]
        await sensor.getGeomagneticField({ 'latitude': Number.NEGATIVE_INFINITY, 'longitude': 0, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest031 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(data.x).assertEqual(geomagneticComponent[0])
                expect(data.y).assertEqual(geomagneticComponent[1])
                expect(data.z).assertEqual(geomagneticComponent[2])
                expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0320
     * @tc.name: SensorGeomagenticAlgorithmJSTest032
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0320", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0320------------------');
        let geomagneticComponent = [NaN, NaN, NaN]
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.MAX_VALUE, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest032 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0330
     * @tc.name: SensorGeomagenticAlgorithmJSTest033
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0330", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0330------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NaN, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest033 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0340
     * @tc.name: SensorGeomagenticAlgorithmJSTest034
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0340", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0340------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NEGATIVE_INFINITY, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest034 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0350
     * @tc.name: SensorGeomagenticAlgorithmJSTest035
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0350", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0350------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.MAX_VALUE },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest035 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0360
     * @tc.name: SensorGeomagenticAlgorithmJSTest036
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0360", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0360------------------');
        let geomagneticComponent = [27536.40234375, -2248.586669921875, -16022.4306640625, -30.110872268676758,
            -4.66834020614624, 27628.05859375, 31937.875]
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.MIN_VALUE },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest036 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(data.x).assertEqual(geomagneticComponent[0])
                expect(data.y).assertEqual(geomagneticComponent[1])
                expect(data.z).assertEqual(geomagneticComponent[2])
                expect(data.geomagneticDip).assertEqual(geomagneticComponent[3])
                expect(data.deflectionAngle).assertEqual(geomagneticComponent[4])
                expect(data.levelIntensity).assertEqual(geomagneticComponent[5])
                expect(data.totalIntensity).assertEqual(geomagneticComponent[6])
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0370
     * @tc.name: SensorGeomagenticAlgorithmJSTest037
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0370", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0370------------------start');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NaN },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest037 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0380
     * @tc.name: SensorGeomagenticAlgorithmJSTest038
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0380", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0380------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NEGATIVE_INFINITY },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest038 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.x) && Number.isNaN(data.y) && Number.isNaN(data.z)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0390
     * @tc.name: SensorGeomagenticAlgorithmJSTest039
     * @tc.desc:verify app info is not null
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0390', 0, async function (done) {
        for (let i = 0; i < timeMillis.length; i++) {
            console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0390------------------');
            await sensor.getGeomagneticField({ 'latitude': 80, 'longitude': 0, 'altitude': 0 },
                timeMillis[i]).then((data) => {
                    console.info('SensorGeomagenticAlgorithmJSTest039 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity);
                    expect(data.x).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][0])
                    expect(data.y).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][1])
                    expect(data.z).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][2])
                    expect(data.deflectionAngle).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][3])
                    expect(data.geomagneticDip).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][4])
                    expect(data.levelIntensity).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][5])
                    expect(data.totalIntensity).assertEqual(GEOMAGNETIC_COMPONENT_YEAR_RESULT[i][6])
                }).catch((error) => {
                    console.info("promise::catch", error)
                });
        }
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0400
     * @tc.name: SensorGeomagenticAlgorithmJSTest040
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0400", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0400------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NaN, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest040 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0410
     * @tc.name: SensorGeomagenticAlgorithmJSTest041
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0410", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0410------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NEGATIVE_INFINITY, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest041 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0420
     * @tc.name: SensorGeomagenticAlgorithmJSTest042
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0420", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0420 max ------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.MAX_VALUE },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest042 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0430
     * @tc.name: SensorGeomagenticAlgorithmJSTest043
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0430", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0430------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NaN },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest043 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0440
     * @tc.name: SensorGeomagenticAlgorithmJSTest044
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0440", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0440------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NEGATIVE_INFINITY },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest044 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error)
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0450
     * @tc.name: SensorGeomagenticAlgorithmJSTest045
     * @tc.desc:verify app info is not null
     */
    it('SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0450', 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0450------------------');
        for (let i = 0; i < GEOMAGNETIC_COORDINATES.length; i++) {
            await sensor.getGeomagneticField({
                'latitude': GEOMAGNETIC_COORDINATES[i][0],
                'longitude': GEOMAGNETIC_COORDINATES[i][1], 'altitude': GEOMAGNETIC_COORDINATES[i][2]
            },
                timeMillis[0]).then((data) => {
                    console.info('SensorGeomagenticAlgorithmJSTest045 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                        + ',geomagneticDip: ' + data.geomagneticDip
                        + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                        + ',totalIntensity: ' + data.totalIntensity)
                    expect(data.x).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[i][0])
                    expect(data.y).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[i][1])
                    expect(data.z).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[i][2])
                    expect(data.deflectionAngle).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[i][3])
                    expect(data.geomagneticDip).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[i][4])
                    expect(data.levelIntensity).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[i][5])
                    expect(data.totalIntensity).assertEqual(GEOMAGNETIC_COMPONENT_COORDINATES_RESULT[i][6])
                }).catch((error) => {
                    console.info("promise::catch", error);
                });
        }
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0460
     * @tc.name: SensorGeomagenticAlgorithmJSTest046
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0460", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0460------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.MAX_VALUE, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest046 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue();
            }).catch((error) => {
                console.info("promise::catch", error);
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0470
     * @tc.name: SensorGeomagenticAlgorithmJSTest047
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0470", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0470------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NaN, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest047 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error);
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0480
     * @tc.name: SensorGeomagenticAlgorithmJSTest048
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0480", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0480------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': Number.NEGATIVE_INFINITY, 'altitude': 0 },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest048 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.deflectionAngle) && Number.isNaN(data.geomagneticDip)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error);
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0490
     * @tc.name: SensorGeomagenticAlgorithmJSTest049
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0490", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0490------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.MAX_VALUE },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest049 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error);
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0500
     * @tc.name: SensorGeomagenticAlgorithmJSTest050
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0500", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0500------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NaN },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest050 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error);
            });
        done()
    })

    /*
     * @tc.number: SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0510
     * @tc.name: SensorGeomagenticAlgorithmJSTest051
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0510", 0, async function (done) {
        console.info('------------------SUB_SensorsSystem_GeomagneticAlgorithm_JSTest_0510------------------');
        await sensor.getGeomagneticField({ 'latitude': 0, 'longitude': 0, 'altitude': Number.NEGATIVE_INFINITY },
            timeMillis[0]).then((data) => {
                console.info('SensorGeomagenticAlgorithmJSTest051 x: ' + data.x + ',y: ' + data.y + ',z: ' + data.z
                    + ',geomagneticDip: ' + data.geomagneticDip
                    + ',deflectionAngle: ' + data.deflectionAngle + ',levelIntensity: ' + data.levelIntensity
                    + ',totalIntensity: ' + data.totalIntensity)
                expect(Number.isNaN(data.levelIntensity) && Number.isNaN(data.totalIntensity)).assertTrue()
            }).catch((error) => {
                console.info("promise::catch", error);
            });
        done()
    })
})
