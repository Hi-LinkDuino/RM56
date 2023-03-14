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

    let SENSOR_DATA_MATRIX = [
        {
            "rotation": [-0.7980074882507324, 0.5486301183700562, 0.24937734007835388, -0.17277367413043976,
            -0.6047078967094421, 0.7774815559387207, 0.5773502588272095, 0.5773502588272095, 0.5773502588272095],
            "inclination": [1, 0, 0, 0, 0.20444221794605255, 0.9788785576820374, 0, -0.9788785576820374,
                0.20444221794605255]
        },
        {
            "rotation": [-0.8206444382667542, 0.3832680284976959, 0.42384934425354004, 0.021023601293563843,
            -0.7209705710411072, 0.6926466822624207, 0.5710522532463074, 0.57732754945755, 0.5836028456687927],
            "inclination": [1, 0, 0, 0, 0.2584352493286133, 0.9660285115242004, 0, -0.9660285115242004,
                0.2584352493286133]
        },
        {
            "rotation": [0.9583651423454285, 0.08038506656885147, -0.27399733662605286, 0.160231813788414,
                -0.9456362724304199, 0.2830156981945038, -0.23635157942771912, -0.3151354491710663,
                -0.9191450476646423],
            "inclination": [1, 0, 0, 0, 0.34239840507507324, 0.9395548701286316, 0, -0.9395548701286316,
                0.34239840507507324]
        },
        {
            "rotation": [null, null, null, null, null, null, null, null, null],
            "inclination": [1, 0, 0, 0, null, null, 0, null, null]
        },
        {
            "rotation": [null, null, null, null, null, null, 0, 0, 0],
            "inclination": [1, 0, 0, 0, null, 0, 0, 0, null]
        }
    ]

    let GET_DIRECTION = [
        [0.38050639629364014, -0.9783217310905457, -0.6610431671142578],
        [-2.7610862255096436, 1.5018651485443115, 2.987273931503296],
        [0.32175055146217346, -1.006853699684143, -0.6857295036315918],
        [1.3332617282867432, -1.5440233945846558, -0.6627295017242432],
        [NaN, NaN, NaN],
        [0.7853981852531433, -0.6154797077178955, -0.7853981852531433],
        [0.7853981852531433, -0.7853981852531433, -0.7853981852531433],
        [0.785398, -0.615480, -0.785398],
        [0.785398, -0.785398, -0.785398]
    ]

    let rotationMatrix = [
        [1, 2, 3, 4, 5, 6, 7, 8, 9],
        [-1, -2, -3, -4, -5, -6, -7, -78, -45],
        [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16],
        [11111111, 21111111, 31111111, 4111111, 5111111, 61111111, 71111111, 811111111, 91111111],
        [NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN],
        [3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38,
            3.40282e+38],
        [3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39,
            3.40282e+39],
        [3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38,
            3.40282e+38],
        [3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39,
            3.40282e+39]
    ]

    let gravity = [
        [9, 9, 9], [91, 92, 93], [-9, -12, -35], [NaN, NaN, NaN], [3.40282e+38, 3.40282e+38, 3.40282e+38],
        [3.40282e+39, 3.40282e+39, 3.40282e+39]
    ]
    let geomagnetic = [
        [30, 25, 41], [3, 2, 4], [-123, -456, -564], [3.40282e+38, 3.40282e+38, 3.40282e+38], [NaN, NaN, NaN]
    ]

    /**
     * test
     *
     * @tc.name: SensorJsTest_068
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SensorJsTest_068', 0, async function (done) {
        sensor.createRotationMatrix(gravity[0], geomagnetic[0], (error, data) => {
            if (error) {
                console.info('SensorJsTest_068 failed');
                expect(false).assertTrue();
            } else {
                console.info("SensorJsTest_068" + JSON.stringify(data))
                expect(JSON.stringify(data)).assertEqual(JSON.stringify(SENSOR_DATA_MATRIX[0]))
            }
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_069
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */

    it('SensorJsTest_069', 0, async function (done) {
        sensor.createRotationMatrix(gravity[2], geomagnetic[2], (error, data) => {
            if (error) {
                console.info('SensorJsTest_069 failed');
                expect(false).assertTrue();
            } else {
                console.info("SensorJsTest_069" + JSON.stringify(data))
                expect(JSON.stringify(data)).assertEqual(JSON.stringify(SENSOR_DATA_MATRIX[2]))
            }
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_070
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SensorJsTest_070', 0, async function (done) {
        sensor.getDirection(rotationMatrix[0], (error, data) => {
            if (error) {
                console.info('SensorJsTest_070 failed');
                expect(false).assertTrue();
            } else {
                for (let i = 1; i < data.length; i++) {
                    console.info("SensorJsTest_070" + data[i])
                    expect(data[i]).assertEqual(GET_DIRECTION[0][i])
                }
            }
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_071
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SensorJsTest_071', 0, async function (done) {
        sensor.getDirection(rotationMatrix[1], function (error, data) {
            if (error) {
                console.info('SensorJsTest_071 failed');
                expect(false).assertTrue();
            } else {
                for (let i = 1; i < data.length; i++) {
                    console.info("SensorJsTest_071" + data[i])
                    expect(data[i]).assertEqual(GET_DIRECTION[1][i])
                }
            }
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_072
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SensorJsTest_072', 0, async function (done) {
        sensor.createRotationMatrix(gravity[0], geomagnetic[0]).then((data) => {
            console.info("SensorJsTest_072" + JSON.stringify(data))
            expect(JSON.stringify(data)).assertEqual(JSON.stringify(SENSOR_DATA_MATRIX[0]))
            done()
        }, (error) => {
            expect(false).assertTrue();
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_073
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SensorJsTest_073', 0, async function (done) {
        sensor.createRotationMatrix(gravity[1], geomagnetic[1]).then((data) => {
            console.info("SensorJsTest_073" + JSON.stringify(data))
            expect(JSON.stringify(data)).assertEqual(JSON.stringify(SENSOR_DATA_MATRIX[1]))
            done()
        }, (error) => {
            expect(false).assertTrue();
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_074
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SensorJsTest_074', 0, async function (done) {
        sensor.createRotationMatrix(gravity[2], geomagnetic[2]).then((data) => {
            console.info("SensorJsTest_074" + JSON.stringify(data))
            expect(JSON.stringify(data)).assertEqual(JSON.stringify(SENSOR_DATA_MATRIX[2]))
            done()
        }, (error) => {
            expect(false).assertTrue();
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_075
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SensorJsTest_075', 0, async function (done) {
        sensor.getDirection(rotationMatrix[0]).then((data) => {
            for (let i = 0; i < data.length; i++) {
                console.info("SensorJsTest_075" + data[i])
                expect(data[i]).assertEqual(GET_DIRECTION[0][i])
            }
            done()
        }, (error) => {
            expect(false).assertTrue();
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_076
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SensorJsTest_076', 0, async function (done) {
        sensor.getDirection(rotationMatrix[1]).then((data) => {
            for (let i = 0; i < data.length; i++) {
                console.info("SensorJsTest_076" + data[i])
                expect(data[i]).assertEqual(GET_DIRECTION[1][i])
            }
            done()
        }, (error) => {
            expect(false).assertTrue();
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_077
     * @tc.desc: Verification results of the incorrect parameters of the test interface.
     */
    it('SensorJsTest_077', 0, async function (done) {
        sensor.getDirection([1, 2, 3, 1, 2, 3, 1, 2, 3, 0]).then((data) => {
            for (let i = 0; i < data.length; i++) {
                console.info("SensorJsTest_077 failed")
                expect(false).assertTrue();
            }
            done()
        }, (error) => {
            expect(true).assertTrue();
            console.info("SensorJsTest_077 success")
            done()
        })
    })

    let ANGLECHANGE_9_RESULT = [
        [0.7853981852531433, NaN, -0.32175055146217346],  //123123123
        [0.7853981852531433, NaN, -0.7853981852531433],   //FLOAT.MAXVALUE
        [0.0, -0.0, -0.0],                                //FLOAT.MINVALUE
        [0.7853981852531433, NaN, -0.7853981852531433],   //FLOAT.MAXVALUE+1
    ];

    /**
     * test
     *
     * @tc.name: SensorJsTest_078
     * @tc.desc:
     */
    it('SensorJsTest_078', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_078");
        sensor.getAngleModify([1, 2, 3, 1, 2, 3, 1, 2, 3], [2, 2, 2, 2, 2, 2, 2, 2, 2], function (error, data) {
            if (error) {
                console.info('SensorJsTest_078 failed');
                expect(false).assertTrue();
            } else {
                for (let i = 0; i < data.length; i++) {
                    console.info("SensorJsAPI--->SensorJsTest_078 [" + i + "] = " + data[i]);
                    expect(data[0]).assertEqual(ANGLECHANGE_9_RESULT[0][0]) && expect(
                        Number.isNaN(data[1])).assertTrue() &&
                        expect(data[2]).assertEqual(ANGLECHANGE_9_RESULT[0][2]);
                }
            }
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_079
     * @tc.desc:
     */
    it('SensorJsTest_079', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_079");
        sensor.getAngleModify([3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38,
            3.40282e+38, 3.40282e+38, 3.40282e+38],
            [3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38,
                3.40282e+38],
            function (error, data) {
                if (error) {
                    console.info('SensorJsTest_079 failed');
                    expect(false).assertTrue();
                } else {
                    for (let i = 0; i < data.length; i++) {
                        console.info("SensorJsAPI--->SensorJsTest_079 [" + i + "] = " + data[i]);
                        expect(data[0]).assertEqual(ANGLECHANGE_9_RESULT[1][0]) && expect(
                            Number.isNaN(data[1])).assertTrue() &&
                            expect(data[2]).assertEqual(ANGLECHANGE_9_RESULT[1][2]);
                    }
                }
                done()
            })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_080
     * @tc.desc:
     */
    it('SensorJsTest_080', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_080");
        sensor.getAngleModify([1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38,
            1.17549e-38, 1.17549e-38, 1.17549e-38],
            [1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38,
                1.17549e-38],
            function (error, data) {
                if (error) {
                    console.info('SensorJsTest_080 failed');
                    expect(false).assertTrue();
                } else {
                    for (let i = 0; i < data.length; i++) {
                        console.info("SensorJsAPI--->SensorJsTest_080 [" + i + "] = " + data[i]);
                        expect(data[0]).assertEqual(ANGLECHANGE_9_RESULT[2][0])
                            && expect(data[1]).assertEqual(ANGLECHANGE_9_RESULT[2][1])
                            && expect(data[2]).assertEqual(ANGLECHANGE_9_RESULT[2][2]);
                    }
                }
                done()
            });
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_081
     * @tc.desc:
     */
    it('SensorJsTest_081', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_081");
        sensor.getAngleModify([3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1,
        3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1],
            [3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1,
            3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1],
            function (error, data) {
                if (error) {
                    console.info('SensorJsTest_081 failed');
                    expect(false).assertTrue();
                } else {
                    for (let i = 0; i < data.length; i++) {
                        console.info("SensorJsAPI--->SensorJsTest_081 [" + i + "] = " + data[i]);
                        expect(data[0]).assertEqual(ANGLECHANGE_9_RESULT[3][0]) && expect(
                            Number.isNaN(data[1])).assertTrue() && expect(data[2]).assertEqual(
                                ANGLECHANGE_9_RESULT[3][2]);
                    }
                }
                done()
            });
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_082
     * @tc.desc:
     */
    it('SensorJsTest_082', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_082");
        sensor.getAngleModify([0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0,
        0.0 / 0.0],
            [0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0],
            function (error, data) {
                if (error) {
                    console.info('SensorJsTest_082 failed');
                    expect(false).assertTrue();
                } else {
                    for (let i = 0; i < data.length; i++) {
                        console.info("SensorJsAPI--->SensorJsTest_082 [" + i + "] = " + data[i]);
                        expect(Number.isNaN(data[0]) && Number.isNaN(data[1]) && Number.isNaN(data[2])).assertTrue();
                    }
                }
                done()
            });
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_083
     * @tc.desc:
     */
    it('SensorJsTest_083', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_083");
        sensor.getAngleModify([1, 2, 3, 1, 2, 3, 1, 2, 3], [2, 2, 2, 2, 2, 2, 2, 2, 2]).then((data) => {
            console.info("SensorJsAPI--->SensorJsTest_083");
            for (let i = 0; i < data.length; i++) {
                console.info("SensorJsAPI--->SensorJsTest_083 [" + i + "] = " + data[i]);
                expect(data[0]).assertEqual(ANGLECHANGE_9_RESULT[0][0]) && expect(
                    Number.isNaN(data[1])).assertTrue() &&
                    expect(data[2]).assertEqual(ANGLECHANGE_9_RESULT[0][2]);
            }
            done();
        }, (error) => {
            expect(false).assertTrue();
            done()
        })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_084
     * @tc.desc:
     */
    it('SensorJsTest_084', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_084");
        sensor.getAngleModify([3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38,
            3.40282e+38, 3.40282e+38, 3.40282e+38],
            [3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38,
                3.40282e+38, 3.40282e+38]).then((data) => {
                    console.info("SensorJsAPI--->SensorJsTest_084");
                    for (let i = 0; i < data.length; i++) {
                        console.info("SensorJsAPI--->SensorJsTest_084 [" + i + "] = " + data[i]);
                        expect(data[0]).assertEqual(ANGLECHANGE_9_RESULT[1][0]) && expect(
                            Number.isNaN(data[1])).assertTrue() && expect(data[2]).assertEqual(
                                ANGLECHANGE_9_RESULT[1][2]);
                    }
                    done()
                }, (error) => {
                    expect(false).assertTrue();
                    done()
                })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_085
     * @tc.desc:
     */
    it('SensorJsTest_085', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_085");
        sensor.getAngleModify([1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38,
            1.17549e-38, 1.17549e-38, 1.17549e-38],
            [1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38, 1.17549e-38,
                1.17549e-38]).then((data) => {
                    console.info("SensorJsAPI--->SensorJsTest_085");
                    for (let i = 0; i < data.length; i++) {
                        console.info("SensorJsAPI--->SensorJsTest_085 [" + i + "] = " + data[i]);
                        expect(data[0]).assertEqual(ANGLECHANGE_9_RESULT[2][0])
                            && expect(data[1]).assertEqual(ANGLECHANGE_9_RESULT[2][1])
                            && expect(data[2]).assertEqual(ANGLECHANGE_9_RESULT[2][2]);
                    }
                    done()
                }, (error) => {
                    expect(false).assertTrue();
                    done()
                })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_086
     * @tc.desc:
     */
    it('SensorJsTest_086', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_076");
        sensor.getAngleModify([3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38
            + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1],
            [3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38
                + 1, 3.40282e+38 + 1, 3.40282e+38 + 1, 3.40282e+38 + 1])
            .then((data) => {
                console.info("SensorJsAPI--->SensorJsTest_086");
                for (let i = 0; i < data.length; i++) {
                    console.info("SensorJsAPI--->SensorJsTest_086 [" + i + "] = " + data[i]);
                    expect(data[0]).assertEqual(ANGLECHANGE_9_RESULT[3][0]) && expect(
                        Number.isNaN(data[1])).assertTrue() && expect(data[2]).assertEqual(ANGLECHANGE_9_RESULT[3][2]);
                }
                done()
            }, (error) => {
                expect(false).assertTrue();
                done()
            })
    })

    /**
     * test
     *
     * @tc.name: SensorJsTest_087
     * @tc.desc:
     */
    it('SensorJsTest_087', 0, async function (done) {
        console.info("SensorJsAPI--->SensorJsTest_087");
        sensor.getAngleModify([0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0,
        0.0 / 0.0],
            [0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0, 0.0 / 0.0,
            0.0 / 0.0]).then((data) => {
                console.info("SensorJsAPI--->SensorJsTest_087");
                for (let i = 0; i < data.length; i++) {
                    console.info("SensorJsAPI--->SensorJsTest_087 [" + i + "] = " + data[i]);
                    expect(Number.isNaN(data[0]) && Number.isNaN(data[1]) && Number.isNaN(data[2])).assertTrue();
                }
                done()
            }, (error) => {
                expect(false).assertTrue();
                done()
            })
    })

    let result = [
        [0.7441122531890869, 0.5199999809265137, -0.335999995470047, -0.25099998712539673],
        [0, 3.402820018375656e+38, 3.402820018375656e+38, 3.402820018375656e+38],
        [1, 0, 0, 0],
        [0.7183529734611511, -0.32499998807907104, -0.5619999766349792, -0.25],
        [0, 0, 0, 0]
    ]

    /*
    * @tc.name: SensorJsTest_088
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_088', 0, async function (done) {
        console.info('SensorJsTest_088 start')
        sensor.createQuaternion([0.52, -0.336, -0.251], (error, data) => {
            console.info('SensorJsTest_088' + 'length:' + data.length);
            if (error) {
                console.info('SensorJsTest_088 failed');
                expect(false).assertTrue();
            } else {
                for (let i = 0; i < data.length; i++) {
                    console.info("data[" + i + "]: " + data[i])
                    expect(data[i]).assertEqual(result[0][i])
                }
            }
            done()
        })
    })

    /*
    * @tc.name: SensorJsTest_089
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_089', 0, async function (done) {
        console.info('SensorJsTest_089 start')
        sensor.createQuaternion([3.40282e+38, 3.40282e+38, 3.40282e+38], (error, data) => {
            if (error) {
                console.info('SensorJsTest_089 failed');
                expect(false).assertTrue();
            } else {
                for (let i = 0; i < data.length; i++) {
                    console.info("data[" + i + "]: " + data[i])
                    expect(data[i]).assertEqual(result[1][i])
                }
            }
            done()
        })
    })

    /*
    * @tc.name: SensorJsTest_090
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_090', 0, async function (done) {
        console.info('SensorJsTest_090 start')
        sensor.createQuaternion([0, 0, 0], (error, data) => {
            if (error) {
                console.info('SensorJsTest_090 failed');
                expect(false).assertTrue();
            } else {
                for (let i = 0; i < data.length; i++) {
                    console.info("data[" + i + "]: " + data[i])
                    expect(data[i]).assertEqual(result[2][i])
                }
            }
            done()
        })
        console.info("SensorJsTest_090 end")
    })

    /*
    * @tc.name: SensorJsTest_091
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_091', 0, async function (done) {
        console.info('SensorJsTest_091 start')
        sensor.createQuaternion([-0.325, -0.562, -0.25], (error, data) => {
            if (error) {
                console.info('SensorJsTest_091 failed');
                expect(false).assertTrue();
            } else {
                for (let i = 0; i < data.length; i++) {
                    console.info("data[" + i + "]: " + data[i])
                    expect(data[i]).assertEqual(result[3][i])
                }
            }
            done()
        })
        console.info("SensorJsTest_091 end")
    })

    /*
    * @tc.name: SensorJsTest_092
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_092', 0, async function (done) {
        console.info('SensorJsTest_092 start')
        sensor.createQuaternion([0.25, 0.14], (error, data) => {
            if (error) {
                console.info('SensorJsTest_092 failed');
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done()
        })
        console.info("SensorJsTest_092 end")
    })

    /*
    * @tc.name: SensorJsTest_093
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_093', 0, async function (done) {
        console.info('SensorJsTest_093 start')
        sensor.createQuaternion([0.52, -0.336, -0.251]).then((data) => {
            console.info('SensorJsTest_093');
            for (let i = 0; i < data.length; i++) {
                console.info("data[" + i + "]: " + data[i]);
                expect(data[i]).assertEqual(result[0][i])
            }
            done()
        }, (error) => {
            expect(false).assertTrue();
            console.info('promise failed')
            done()
        })
        console.info("SensorJsTest_093 end")
    })

    /*
    * @tc.name: SensorJsTest_094
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_094', 0, async function (done) {
        console.info('SensorJsTest_094 start')
        sensor.createQuaternion([0, 0]).then((data) => {
            console.info('SensorJsTest_094');
            expect(false).assertTrue();
            done()
        }, (error) => {
            expect(true).assertTrue();
            console.info('promise failed')
            done()
        })
        console.info("SensorJsTest_094 end")
    })

    /*
    * @tc.name: SensorJsTest_095
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_095', 0, async function (done) {
        console.info('SensorJsTest_095 start')
        sensor.createQuaternion([0, 0, 0]).then((data) => {
            console.info('SensorJsTest_095');
            for (let i = 0; i < data.length; i++) {
                console.info("data[" + i + "]: " + data[i]);
                expect(data[i]).assertEqual(result[2][i])
            }
            done()
        }, (error) => {
            expect(false).assertTrue();
            console.info('promise failed')
            done()
        })
        console.info("SensorJsTest_095 end")
    })

    /*
    * @tc.name: SensorJsTest_096
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_096', 0, async function (done) {
        console.info('SensorJsTest_096 start')
        sensor.createQuaternion([-0.325, -0.562, -0.25]).then((data) => {
            console.info('SensorJsTest_096');
            for (let i = 0; i < data.length; i++) {
                console.info("data[" + i + "]: " + data[i]);
                expect(data[i]).assertEqual(result[3][i])
            }
            done()
        }, (error) => {
            expect(false).assertTrue();
            console.info('promise failed')
            done()
        })
    })

    /*
    * @tc.name: SensorJsTest_097
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_097', 0, async function (done) {
        console.info('SensorJsTest_097 start')
        sensor.createQuaternion([0.25, 0.14]).then((data) => {
            console.info('SensorJsTest_097');
            expect(false).assertTrue();
            done()
        }, (error) => {
            expect(true).assertTrue();
            console.info('promise failed')
            done()
        })
    })

    let createRotationMatrixResult = [
        [0.6724675297737122, -0.10471208393573761, 0.7326819896697998, 0.06531608104705811, 0.9944750070571899,
            0.08217836916446686, -0.7372390031814575, -0.007406365126371384, 0.6755914688110352],
        [1, 0, 0, 0, 1, 0, 0, 0, 1]
    ]

    /*
    * @tc.name: SensorJsTest_098
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_098', 0, async function (done) {
        console.info("SensorJsTest_098 start");
        sensor.createRotationMatrix([-0.0245, 0.402, 0.0465], (error, data) => {
            console.info("SensorJsTest_098");
            if (error) {
                console.info('SensorJsTest_098 failed');
                expect(false).assertTrue();
            } else {
                for (let i = 0; i < data.length; i++) {
                    console.info("SensorJsTest_098 [" + i + ") = " + data[i]);
                    expect(data[i]).assertEqual(createRotationMatrixResult[0][i])
                }
            }
            done()
        })
        console.info(LABEL + "SensorJsTest_098 end");
    })

    /*
    * tc.name: SensorJsTest_099
    * tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_099', 0, async function (done) {
        console.info('SensorJsTest_099 start')
        sensor.createRotationMatrix([-0.0245, 0.402, 0.0465]).then((data) => {
            for (let i = 0; i < data.length; i++) {
                console.info("SensorJsTest_099 [" + i + "] : " + data[i]);
                expect(data[i]).assertEqual(createRotationMatrixResult[0][i])
            }
            done()
        }, (error) => {
            expect(false).assertTrue();
            console.info('promise failed', error)
            done()
        })
        console.info("SensorJsTest_099 end")
    })

    /*
    * tc.name: SensorJsTest_100
    * tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_100', 0, async function (done) {
        console.info('SensorJsTest_100 start')
        sensor.createRotationMatrix([0, 0, 0]).then((data) => {
            for (let i = 0; i < data.length; i++) {
                console.info("SensorJsTest_100 [" + i + "] : " + data[i]);
                expect(data[i]).assertEqual(createRotationMatrixResult[1][i])
            }
            done()
        }, (error) => {
            expect(false).assertTrue();
            console.info('promise failed', error)
            done()
        })
        console.info("SensorJsTest_100 end")
    })

    let getGeomagneticDipResult = [0.8760581016540527, 0.862170, -Infinity, 44330]

    /*
    * @tc.name: SensorJsTest_101
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_101', 0, async function (done) {
        console.info('SensorJsTest_101 start')
        sensor.getGeomagneticDip([1, 2, 3, 4, 5, 6, 7, 8, 9], (error, data) => {
            if (error) {
                console.info('SensorJsTest_101 failed');
                expect(false).assertTrue();
            } else {
                console.info("SensorJsTest_101" + data)
                expect(data).assertEqual(getGeomagneticDipResult[0])
            }
            done()
            console.info('SensorJsTest_101' + 'length:' + data.length);
        })
        console.info("SensorJsTest_101 end")
    })

    /*
    * @tc.name: SensorJsTest_102
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_102', 0, async function (done) {
        console.info('SensorJsTest_102 start')
        sensor.getGeomagneticDip([1, 2, 3, 4], (error, data) => {
            if (error) {
                console.info('SensorJsTest_102 success');
                expect(true).assertTrue();
            } else {
                console.info("SensorJsTest_102 failed")
                expect(false).assertTrue();
            }
            done()
        })
        console.info("SensorJsTest_102 end")
    })

    /*
    * @tc.name: SensorJsTest_103
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_103', 0, async function (done) {
        console.info('SensorJsTest_103 start')
        sensor.getAltitude(0, 100, (error, data) => {
            if (error) {
                console.info('SensorJsTest_103 failed');
                expect(false).assertTrue();
            } else {
                console.info("SensorJsTest_103" + data)
                expect(data).assertEqual(getGeomagneticDipResult[2])
            }
            done()
            console.info("SensorJsTest_103 end")
        })
    })

    /*
    * @tc.name: SensorJsTest_104
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_104', 0, async function (done) {
        console.info('SensorJsTest_104 start')
        sensor.getAltitude(5, 0, (error, data) => {
            if (error) {
                console.info('SensorJsTest_104 failed');
                expect(false).assertTrue();
            } else {
                console.info("SensorJsTest_104" + data)
                expect(data).assertEqual(getGeomagneticDipResult[3])
            }
            done()
        })
        console.info("SensorJsTest_104 end")
    })

    /*
    * @tc.name: SensorJsTest_105
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_105', 0, async function (done) {
        sensor.getAltitude(0, 100).then((data) => {
            console.info("SensorJsTest_104" + data)
            expect(data).assertEqual(getGeomagneticDipResult[2])
            done()
        }, (error) => {
            console.info('SensorJsTest_104 failed');
            expect(false).assertTrue();
            done()
        });
    })

    /*
    * @tc.name: SensorJsTest_106
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_106', 0, async function (done) {
        sensor.getAltitude(5, 0).then((data) => {
            console.info("SensorJsTest_104" + data)
            expect(data).assertEqual(getGeomagneticDipResult[3])
            done()
        }, (error) => {
            console.info('SensorJsTest_104 failed');
            expect(false).assertTrue();
            done()
        });
    })

    let transformCoordinateSystemResult = [
        [1.500000, 1.500000, 1.500000, 1.500000, 1.500000, 1.500000, 1.500000, 1.500000, 1.500000],
        [340282001837565600000000000000000000000.000000, 340282001837565600000000000000000000000.000000,
            340282001837565600000000000000000000000.000000,
            340282001837565600000000000000000000000.000000, 340282001837565600000000000000000000000.000000,
            340282001837565600000000000000000000000.000000,
            340282001837565600000000000000000000000.000000, 340282001837565600000000000000000000000.000000,
            340282001837565600000000000000000000000.000000],
        [Infinity, -Infinity, Infinity, Infinity, -Infinity, Infinity, Infinity, -Infinity, Infinity]]

    /*
    * @tc.name: SensorJsTest_107
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_107', 0, async function (done) {
        console.info("---------------------------SensorJsTest_107----------------------------------");
        sensor.transformCoordinateSystem([1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5], {
            'axisX': 1,
            'axisY': 2
        }, (error, data) => {
            if (error) {
                console.info('SensorJsTest_107 failed');
                expect(false).assertTrue();
            } else {
                console.info("SensorJsTest_107 " + JSON.stringify(data));
                expect(JSON.stringify(data)).assertEqual(JSON.stringify(transformCoordinateSystemResult[0]))
            }
            done()
        })
    })

    /*
    * @tc.name: SensorJsTest_108
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it('SensorJsTest_108', 0, async function (done) {
        console.info("---------------------------SensorJsTest_108----------------------------------");
        sensor.transformCoordinateSystem([3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38, 3.40282e+38,
            3.40282e+38, 3.40282e+38, 3.40282e+38], { 'axisX': 1, 'axisY': 2 }, (error, data) => {
                if (error) {
                    console.info('SensorJsTest_108 failed');
                    expect(false).assertTrue();
                } else {
                    console.info("SensorJsTest_108 " + JSON.stringify(data));
                    expect(JSON.stringify(data)).assertEqual(JSON.stringify(transformCoordinateSystemResult[1]))
                }
                done()
            })
    })

    /*
    * @tc.name: SensorJsTest_109
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it("SensorJsTest_109", 0, async function (done) {
        console.info("---------------------------SensorJsTest_109----------------------------------");
        sensor.transformCoordinateSystem([1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5], {
            'axisX': 1,
            'axisY': 2
        }).then((data) => {
            for (let i = 0; i < data.length; i++) {
                console.info("SensorJsTest_109 data[ " + i + "] = " + data[i]);
                expect(data[i]).assertEqual(transformCoordinateSystemResult[0][i]);
            }
            done()
        }, (error) => {
            console.info('SensorJsTest_109 failed');
            expect(false).assertTrue();
            done()
        });
    })

    /*
    * @tc.name: SensorJsTest_110
    * @tc.desc: Verfication results of the incorrect parameters of test interface.
    */
    it("SensorJsTest_110", 0, async function (done) {
        console.info("---------------------------SensorJsTest_110----------------------------------");
        sensor.transformCoordinateSystem([3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39,
            3.40282e+39, 3.40282e+39, 3.40282e+39, 3.40282e+39], { 'axisX': 1, 'axisY': 3 }).then((data) => {
                for (let i = 0; i < data.length; i++) {
                    console.info("SensorJsTest_110 data[ " + i + "] = " + data[i]);
                    expect(data[i]).assertEqual(transformCoordinateSystemResult[2][i]);
                }
                done()
            }, (error) => {
                console.info('SensorJsTest_109 failed');
                expect(false).assertTrue();
                done()
            });
    })


})




