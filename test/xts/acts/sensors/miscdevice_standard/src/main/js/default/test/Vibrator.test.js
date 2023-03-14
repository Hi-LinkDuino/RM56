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
import vibrator from '@ohos.vibrator'

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe("VibratorJsTest", function () {
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
     * @tc.name:VibratorJsTest001
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest001", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest001  vibrator error');
                expect(false).assertTrue();
            } else {
                console.info('VibratorJsTest001  vibrator success');
                expect(true).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate(10, vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest002
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest002", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest002  vibrator success');
                expect(true).assertTrue();
            } else {
                console.info('VibratorJsTest002  vibrator error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate(-1, vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest003
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest003", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest003  vibrator success');
                expect(true).assertTrue();
            } else {
                console.info('VibratorJsTest003  vibrator error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate(1800000 + 1, vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest004
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest004", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest004  vibrator error');
                expect(false).assertTrue();
            } else {
                console.info('VibratorJsTest004  vibrator success');
                expect(true).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate(1800000, vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest005
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest005", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest005  vibrator success');
                expect(true).assertTrue();
            } else {
                console.info('VibratorJsTest005  vibrator error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate("", vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest006
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest006", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest006  vibrator success');
                expect(true).assertTrue();
            } else {
                console.info('VibratorJsTest006  vibrator error');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate("xxx", vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest007
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest007", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest007  vibrator error');
                expect(false).assertTrue();
            } else {
                console.info('VibratorJsTest007  vibrator success');
                expect(true).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.vibrate("haptic.clock.timer", vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest008
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest008", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest008  stop error');
                expect(false).assertTrue();
            } else {
                console.info('VibratorJsTest008  stop success');
                expect(true).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.stop(vibrator.VibratorStopMode.VIBRATOR_STOP_MODE_PRESET, vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest009
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest009", 0, async function (done) {
        console.info('----------------------VibratorJsTest001---------------------------');
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stop("time", (error) => {
                    if (error) {
                        console.info('VibratorJsTest009  stop error');
                        expect(false).assertTrue();
                        setTimeout((err) => {
                            reject(err);
                        }, 500);
                    } else {
                        console.info('VibratorJsTest009  stop success');
                        expect(true).assertTrue();
                        setTimeout(() => {
                            resolve();
                        }, 500);
                    }
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(180000, (error) => {
                if (error) {
                    console.info('VibratorJsTest009  vibrate error');
                    expect(false).assertTrue();
                    setTimeout((err) => {
                        reject(err);
                    }, 500);
                } else {
                    console.info('VibratorJsTest009  vibrate success');
                    expect(true).assertTrue();
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }
            });
        })

        await promise.then(() => {
            return stopPromise();
        }, () => {
            console.info("VibratorJsTest009 reject");
        })
        done();
    })

    /*
     * @tc.name:VibratorJsTest010
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest010", 0, async function (done) {
        function vibrateCallback(error) {
            if (error) {
                console.info('VibratorJsTest010  stop success');
                expect(true).assertTrue();
            } else {
                console.info('VibratorJsTest010  stop off');
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        vibrator.stop("", vibrateCallback);
    })

    /*
     * @tc.name:VibratorJsTest011
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest011", 0, async function (done) {
        vibrator.vibrate(1000).then(() => {
            console.log("VibratorJsTest011  vibrate success");
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(false).assertTrue();
            console.log("VibratorJsTest011  vibrate error");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest012
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest012", 0, async function (done) {
        vibrator.vibrate(-1).then(() => {
            console.log("VibratorJsTest012  vibrate error");
            expect(false).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(true).assertTrue();
            console.log("VibratorJsTest012  vibrate success");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest013
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest013", 0, async function (done) {
        vibrator.vibrate(1800000 + 1).then(() => {
            console.log("VibratorJsTest013  vibrate error");
            expect(false).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(true).assertTrue();
            console.log("VibratorJsTest013  vibrate success");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest014
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest014", 0, async function (done) {
        vibrator.vibrate(vibrator.EffectId.EFFECT_CLOCK_TIMER).then(() => {
            console.log("VibratorJsTest014  vibrate success");
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(false).assertTrue();
            console.log("VibratorJsTest014  vibrate error");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest015
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest015", 0, async function (done) {
        vibrator.vibrate("").then(() => {
            console.log("VibratorJsTest015  vibrate error");
            expect(false).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(true).assertTrue();
            console.log("VibratorJsTest015  vibrate success");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest016
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest016", 0, async function (done) {
        vibrator.stop("").then(() => {
            console.log("VibratorJsTest016  stop error");
            expect(false).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(true).assertTrue();
            console.log("VibratorJsTest016  stop success");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest017
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest017", 0, async function (done) {
        vibrator.stop("preset").then(() => {
            console.log("VibratorJsTest017  off success");
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(false).assertTrue();
            console.log("VibratorJsTest017  off error");
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name:VibratorJsTest018
     * @tc.desc:verify app info is not null
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    it("VibratorJsTest018", 0, async function (done) {
        function stopPromise() {
            return new Promise((resolve, reject) => {
                vibrator.stop(vibrator.VibratorStopMode.VIBRATOR_STOP_MODE_TIME).then(() => {
                    console.log("VibratorJsTest018  stop success");
                    expect(true).assertTrue();
                    setTimeout(() => {
                        resolve();
                    }, 500);
                }, (error) => {
                    expect(false).assertTrue();
                    console.log("VibratorJsTest018  stop error");
                    setTimeout((err) => {
                        reject(err);
                    }, 500);
                });
            })
        }

        let promise = new Promise((resolve, reject) => {
            vibrator.vibrate(180000).then(() => {
                console.log("VibratorJsTest018  vibrate success");
                expect(true).assertTrue();
                setTimeout(() => {
                    resolve();
                }, 500);
            }, (error) => {
                expect(false).assertTrue();
                console.log("VibratorJsTest018  vibrate error");
                setTimeout((err) => {
                    reject(err);
                }, 500);
            });
        })

        await promise.then(() => {
            return stopPromise();
        }, () => {
            console.info("VibratorJsTest018 reject");
        })
        done();
    })
})
