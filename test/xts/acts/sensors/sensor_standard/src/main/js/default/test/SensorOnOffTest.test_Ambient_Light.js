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
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0010
     * @tc.name: SensorAmbientLightJSTest001
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0010", 0, async function (done) {
        console.info('----------------------SUB_SensorsSystem_Ambient_Light_JSTest_0010---------------------------');
        function offPromise() {
            return new Promise((resolve, reject) => {
                sensor.off(sensor.SensorType.SENSOR_TYPE_ID_AMBIENT_LIGHT, (error) => {
                    if (error) {
                        console.info('SensorAmbientLightJSTest001  off error');
                        expect(false).assertTrue();
                        console.info('setTimeout ..start')
                        setTimeout((err) => {
                            reject(err);
                        }, 500);
                    } else {
                        console.info('SensorAmbientLightJSTest001  off success');
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }
                }, 1000)
            })
        }

        let promise = new Promise((resolve, reject) => {
            sensor.on(sensor.SensorType.SENSOR_TYPE_ID_AMBIENT_LIGHT, function (error, data) {
                if (error) {
                    console.info('SensorAmbientLightJSTest001  on error');
                    expect(false).assertTrue();
                    setTimeout((err) => {
                        reject(err);
                    }, 500);
                } else {
                    console.info('SensorAmbientLightJSTest001  on success, success data: ' + data.intensity);
                    expect(typeof (data.intensity)).assertEqual("number");
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }
            });
        })

        await promise.then(() => {
            return offPromise();
        }, () => {
            console.info("SensorAmbientLightJSTest001 reject");
        })
        done();
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0020
     * @tc.name: SensorAmbientLightJSTest002
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0020", 0, async function (done) {
        console.info('----------------------SUB_SensorsSystem_Ambient_Light_JSTest_0020---------------------------');
        function onSensorCallback(error, data) {
            if (error) {
                console.info('SensorAmbientLightJSTest002  on success');
                expect(true).assertTrue();
            } else {
                console.info('SensorAmbientLightJSTest002  on error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        sensor.on(-1, onSensorCallback);
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0030
     * @tc.name: SensorAmbientLightJSTest003
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0030", 0, async function (done) {
        console.info('----------------------SUB_SensorsSystem_Ambient_Light_JSTest_0030---------------------------');
        function offPromise() {
            return new Promise((resolve, reject) => {
                sensor.off(5, (error) => {
                    if (error) {
                        console.info('SensorAmbientLightJSTest003  off error');
                        expect(false).assertTrue();
                        setTimeout((err) => {
                            done();
                            reject(err);
                        }, 500);
                    } else {
                        console.info('SensorAmbientLightJSTest003  off success');
                        expect(true).assertTrue();
                        setTimeout(() => {
                            done();
                            resolve();
                        }, 500);
                    }
                }, 1000)
            })
        }

        let promise = new Promise((resolve, reject) => {
            sensor.on(5, function (error, data) {
                if (error) {
                    console.info('SensorAmbientLightJSTest003  on error');
                    expect(false).assertTrue();
                    setTimeout((err) => {
                        reject(err);
                    }, 500);
                } else {
                    console.info('SensorAmbientLightJSTest003  on success data: ' + data.intensity);
                    expect(typeof (data.intensity)).assertEqual("number");
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }
            }, { 'interval': 200000000 });
        })

        await promise.then(() => {
            return offPromise();
        }, () => {
            console.info("SensorAmbientLightJSTest003 reject");
        })
        done();
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0040
     * @tc.name: SensorAmbientLightJSTest004
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0040", 0, function () {
        console.info('----------------------SensorAmbientLightJSTest004---------------------------');
        sensor.on(5, function () { }, { 'interval': 100000000 }, 5);
        expect(true).assertTrue();
        console.info('----------------------SensorAmbientLightJSTest004--------------------------- end');
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0050
     * @tc.name: SensorAmbientLightJSTest005
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0050", 0, async function (done) {
        function onceSensorCallback(error, data) {
            if (error) {
                console.info('SensorAmbientLightJSTest005  once error');
                expect(false).assertTrue();
            } else {
                console.info('SensorAmbientLightJSTest005  once success data: ' + data.intensity);
                expect(typeof (data.intensity)).assertEqual("number");
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        sensor.once(sensor.SensorType.SENSOR_TYPE_ID_AMBIENT_LIGHT, onceSensorCallback);
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0060
     * @tc.name: SensorAmbientLightJSTest006
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0060", 0, async function (done) {
        function onceSensorCallback(error, data) {
            if (error) {
                console.info('SensorAmbientLightJSTest006  on success');
                expect(true).assertTrue();
            } else {
                console.info('SensorAmbientLightJSTest006  on error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        sensor.once(-1, onceSensorCallback);
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0070
     * @tc.name: SensorAmbientLightJSTest007
     * @tc.desc:verify app info is not null
     */
    it("SensorJsTest007", 0, function () {
        sensor.once(5, function () { }, 5);
        expect(true).assertTrue();
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0080
     * @tc.name: SensorAmbientLightJSTest008
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0080", 0, async function (done) {
        function offCallback(error) {
            if (error) {
                console.info('SensorAmbientLightJSTest008  off success');
                expect(true).assertTrue();
            } else {
                console.info('SensorAmbientLightJSTest008  off error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        sensor.off(-1, offCallback);
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0090
     * @tc.name: SensorAmbientLightJSTest009
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0090", 0, async function (done) {
        function offCallback(error) {
            if (error) {
                console.info('SensorAmbientLightJSTest009  off success');
                expect(true).assertTrue();
            } else {
                console.info('SensorAmbientLightJSTest009  off error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        sensor.off(5, offCallback);
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0100
     * @tc.name: SensorAmbientLightJSTest010
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0100", 0, async function (done) {
        function offCallback(error) {
            if (error) {
                console.info('SensorAmbientLightJSTest010  off success');
                expect(true).assertTrue();
            } else {
                console.info('SensorAmbientLightJSTest010  off error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        sensor.off(2000000, offCallback);
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0110
     * @tc.name: SensorAmbientLightJSTest011
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0110", 0, async function (done) {
        function onceSensorCallback(error, data) {
            if (error) {
                console.info('SensorAmbientLightJSTest011  once success');
                expect(true).assertTrue();
            } else {
                console.info('SensorAmbientLightJSTest011  once error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        sensor.once(2000000, onceSensorCallback);
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0120
     * @tc.name: SensorAmbientLightJSTest012
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0120", 0, async function (done) {
        function onCallback(error) {
            if (error) {
                console.info('SensorAmbientLightJSTest012  on success');
                expect(true).assertTrue();
            } else {
                console.info('SensorAmbientLightJSTest012  on error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        sensor.on(2000000, onCallback);
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0130
     * @tc.name: SensorAmbientLightJSTest013
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0130", 0, function () {
        sensor.off(5, 5);
        expect(true).assertTrue();
    })


    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0140
     * @tc.name: SensorAmbientLightJSTest014
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0140", 0, async function (done) {
        console.info('----------------------SUB_SensorsSystem_Ambient_Light_JSTest_0140---------------------------');
        function offPromise2() {
            return new Promise((resolve, reject) => {
                sensor.off(5, (error) => {
                    if (error) {
                        console.info('SensorAmbientLightJSTest014  off2 success');
                        expect(true).assertTrue();
                        setTimeout((err) => {
                            done();
                            reject(err)
                        }, 500);
                    } else {
                        console.info('SensorAmbientLightJSTest014  off2 error');
                        expect(false).assertTrue();
                        setTimeout(() => {
                            done();
                            resolve()
                        }, 500);
                    }
                });
            })
        }

        function offPromise1() {
            return new Promise((resolve, reject) => {
                sensor.off(5, (error) => {
                    if (error) {
                        console.info('SensorAmbientLightJSTest014  off1  error');
                        expect(false).assertTrue();
                        setTimeout((err) => {
                            reject(err);
                        }, 500);
                    } else {
                        console.info('SensorAmbientLightJSTest014  off1  success');
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            sensor.on(5, function (error, data) {
                if (error) {
                    console.info('SensorAmbientLightJSTest014  on error');
                    expect(false).assertTrue();
                    setTimeout((err) => {
                        reject(err);
                    }, 500);
                } else {
                    console.info('SensorAmbientLightJSTest014  on success data: ' + data.intensity);
                    expect(typeof (data.intensity)).assertEqual("number");
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }
            });
        })

        await promise.then(() => {
            return offPromise1();
        }).then(() => {
            return offPromise2();
        });
        done();
    })

    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0150
     * @tc.name: SensorAmbientLightJSTest015
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0150", 0, async function (done) {
        console.info('----------------------SUB_SensorsSystem_Ambient_Light_JSTest_0150---------------------------');
        function offPromise() {
            return new Promise((resolve, reject) => {
                sensor.off(5, (error) => {
                    if (error) {
                        console.info('SensorAmbientLightJSTest015  off error');
                        expect(false).assertTrue();
                        setTimeout((err) => {
                            done();
                            reject(err);
                        }, 500);
                    } else {
                        console.info('SensorAmbientLightJSTest015  off success');
                        expect(true).assertTrue();
                        setTimeout(() => {
                            done();
                            resolve();
                        }, 500);
                    }
                });
            })
        }
        function onPromise2() {
            return new Promise((resolve, reject) => {
                sensor.on(5, function (error, data) {
                    if (error) {
                        console.info('SensorAmbientLightJSTest015  on2 error');
                        expect(false).assertTrue();
                        setTimeout((err) => {
                            reject(err);
                        }, 500);
                    } else {
                        console.info('SensorAmbientLightJSTest015  on2 success data: ' + data.intensity);
                        expect(typeof (data.intensity)).assertEqual("number");
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }
                });
            })
        }

        let onPromise1 = new Promise((resolve, reject) => {
            sensor.on(5, function (error, data) {
                if (error) {
                    console.info('SensorAmbientLightJSTest015  on1 error');
                    expect(false).assertTrue();
                    setTimeout((err) => {
                        reject(err);
                    }, 500);
                } else {
                    console.info('SensorAmbientLightJSTest015  on1 success data: ' + data.intensity);
                    expect(typeof (data.intensity)).assertEqual("number");
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }
            });
        })

        await onPromise1.then(() => {
            return onPromise2();
        }).then(() => {
            return offPromise();
        });
        done();
    })


    /*
     * @tc.number: SUB_SensorsSystem_Ambient_Light_JSTest_0160
     * @tc.name: SensorAmbientLightJSTest016
     * @tc.desc:verify app info is not null
     */
    it("SUB_SensorsSystem_Ambient_Light_JSTest_0160", 0, async function (done) {
        console.info('----------------------SUB_SensorsSystem_Ambient_Light_JSTest_0160---------------------------');
        function offPromise() {
            return new Promise((resolve, reject) => {
                sensor.off(5, (error) => {
                    if (error) {
                        console.info('SensorAmbientLightJSTest016  off error');
                        expect(false).assertTrue();
                        setTimeout((err) => {
                            done();
                            reject(err);
                        }, 500);
                    } else {
                        console.info('SensorAmbientLightJSTest016  off success');
                        expect(true).assertTrue();
                        setTimeout(() => {
                            done();
                            resolve();
                        }, 500);
                    }
                });
            })
        }
        function oncePromise() {
            return new Promise((resolve, reject) => {
                sensor.once(5, function (error, data) {
                    if (error) {
                        console.info('SensorAmbientLightJSTest016  once error');
                        expect(false).assertTrue();
                        setTimeout((err) => {
                            reject(err);
                        }, 500);
                    } else {
                        console.info('SensorAmbientLightJSTest016  once success data: ' + data.intensity);
                        expect(typeof (data.intensity)).assertEqual("number");
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }
                });
            })
        }

        let onPromise1 = new Promise((resolve, reject) => {
            sensor.on(5, function (error, data) {
                if (error) {
                    console.info('SensorAmbientLightJSTest016  on1 error');
                    expect(false).assertTrue();
                    setTimeout((err) => {
                        reject(err);
                    }, 500);
                } else {
                    console.info('SensorAmbientLightJSTest016  on1 success data: ' + data.intensity);
                    expect(typeof (data.intensity)).assertEqual("number");
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }
            });
        })

        await onPromise1.then(() => {
            return oncePromise();
        }).then(() => {
            return offPromise();
        });
        done();
    })
})
