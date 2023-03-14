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
import systemparameter from '@ohos.systemParameter'

describe('SystemParameterTest', function () {

    console.info('SystemParameterTest start################################start');

    function SetParameter(key, value) {
        let tmp = value;
        if (value === "" || value === undefined) {
            let myDate = new Date();
            tmp = myDate.toLocaleString();
        }

        console.info('SetParameter key ' + key);
        console.info('SetParameter value ' + tmp);
        try {
            systemparameter.setSync(key, tmp);
        } catch (err) {
            expect(ret).assertTrue();
            console.info('SetParameter error: ' + err);
        }
        console.info('SetParameter key' + key + "  end");
    }

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0801
     * @tc.name      testWaitPromise01
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0801', 0, async function (done) {
        console.info('system_parameter_test_0801 start');
        let ret = false;
        try {
            var parameterInfo = systemparameter.wait("test.wait_param.101", "100", 1);
            parameterInfo.then(function (result) { // timeout
                console.info("system_parameter_test_0801 test.wait_param.101 success: " + result);
                expect(ret).assertTrue();
            }).catch(function (err) {
                ret = true;
                console.info("system_parameter_test_0801 test.wait_param.101 error: " + err.code);
                expect(ret).assertTrue();
                done();
            });
        } catch (e) {
            expect(ret).assertTrue();
            console.info("promise get input error: " + e);
        }
        console.info('system_parameter_test_0801 end');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0802
     * @tc.name      testWaitPromise02
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0802', 0, async function (done) {
        console.info('system_parameter_test_0802 start');
        let ret = false;
        try {
            var parameterInfo = systemparameter.wait("test.wait_param.102", "", 1);
            parameterInfo.then(function (result) {  // timeout
                console.info("system_parameter_test_0802 test.wait_param.102 success: ");
                expect(ret).assertTrue();
                done();
            }).catch(function (err) {
                ret = true;
                console.info("system_parameter_test_0802 test.wait_param.102 error: " + err.code);
                expect(ret).assertTrue();
                done();
            });
        } catch (e) {
            expect(ret).assertTrue();
            console.info("promise get input error: " + e);
        }
        console.info('system_parameter_test_0802 end');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0803
     * @tc.name      testWaitPromise03
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0803', 0, async function (done) {
        console.info('system_parameter_test_0803 start');
        let ret = false;
        SetParameter("test.wait_param.103", "103");
        try {
            var parameterInfo = systemparameter.wait("test.wait_param.103", "103", 1);
            parameterInfo.then(function (result) { // ok
                ret = true;
                console.info("system_parameter_test_0803 test.wait_param.103 success: ");
                expect(ret).assertTrue();
                done();
            }).catch(function (err) {
                expect(ret).assertTrue();
                console.info("system_parameter_test_0803 test.wait_param.103 error: " + err.code);
            });
        } catch (e) {
            expect(ret).assertTrue();
            console.info("promise get input error: " + e);
        }
        console.info('system_parameter_test_0803 end');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0804
     * @tc.name      testWaitPromise04
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0804', 0, async function (done) {
        console.info('system_parameter_test_0804 start');
        let ret = false;
        SetParameter("test.wait_param.104", "104");
        try {
            var parameterInfo = systemparameter.wait("test.wait_param.104", "*", 1);
            parameterInfo.then(function (result) { // ok
                ret = true;
                console.info("system_parameter_test_0804 test.wait_param.104 success");
                expect(ret).assertTrue();
                done();
            }).catch(function (err) {
                console.info("system_parameter_test_0804 test.wait_param.104 error: " + err.code);
                expect(ret).assertTrue();
            });
        } catch (e) {
            expect(ret).assertTrue();
            console.info("promise get input error: " + e);
        }
        console.info('system_parameter_test_0804 end');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0805
     * @tc.name      testWaitPromise05
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0805', 0, async function (done) {
        console.info('system_parameter_test_0805 start');
        let ret = false;
        SetParameter("test.wait_param.105", "105");
        try {
            var parameterInfo = systemparameter.wait("test.wait_param.105", "*", -1);
            parameterInfo.then(function (result) {
                ret = true;
                console.info("system_parameter_test_0805 test.wait_param.105 success");
                expect(ret).assertTrue();
                done();
            }).catch(function (err) {
                console.info("system_parameter_test_0805 test.wait_param.105 error: " + err.code);
                expect(ret).assertTrue();
            });
        } catch (e) {
            expect(ret).assertTrue();
            console.info("promise get input error: " + e);
        }
        console.info('system_parameter_test_0805 end');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0806
     * @tc.name      testWait01
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0806', 0, async function (done) {
        console.info('system_parameter_test_0806 start');
        let ret = false;
        try {
            systemparameter.wait("test.wait_param.201", "100", 1, function (err, data) {
                if (err == undefined || err.code === 0) {
                    console.info("system_parameter_test_0806 test.wait_param.201 success")
                } else {
                    ret = true; // wait timeout
                    console.info("system_parameter_test_0806 test.wait_param.201 err:" + err.code);
                    expect(ret).assertTrue();
                }
            });
        } catch (e) {
            expect(ret).assertTrue();
            console.info("system_parameter_test_0806 get input error: " + e);
        }
        setTimeout(function () {
            expect(ret).assertTrue();
            done();
        }, '2000');
        console.info('system_parameter_test_0806 end');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0807
     * @tc.name      testWait02
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0807', 0, async function (done) {
        console.info('system_parameter_test_0807 start');
        let ret = false;
        try {
            ret = true;
            systemparameter.wait("test.wait_param.202", "", 1, function (err, data) {
                if (err == undefined || err.code === 0) { // timeout
                    ret = false;
                    console.info("system_parameter_test_0807 test.wait_param.202 success");
                } else {
                    console.info("system_parameter_test_0807 callback test.wait_param.202 err:" + err.code);
                }
                expect(ret).assertTrue();
                done();
            });
        } catch (e) {
            ret = true;
            console.info("get input error: " + e);
        }
        expect(ret).assertTrue();
        console.info('system_parameter_test_0807 end');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0808
     * @tc.name      testWait03
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0808', 0, async function (done) {
        console.info('system_parameter_test_0808 start');
        let ret = false;
        SetParameter("test.wait_param.203", "103");
        try {
            systemparameter.wait("test.wait_param.203", "103", 1, function (err, data) {
                if (err == undefined || err.code === 0) {
                    ret = true;
                    console.info("system_parameter_test_0808 test.wait_param.203 success")
                } else {
                    console.info("system_parameter_test_0808 test.wait_param.203 err:" + err.code)
                }
                expect(ret).assertTrue();
            });
        } catch (e) {
            expect(ret).assertTrue();
            console.info("get input error: " + e);
        }
        setTimeout(function () {
            expect(ret).assertTrue();
            done();
        }, '1000');
        console.info('system_parameter_test_0808 end');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0809
     * @tc.name      testWait04
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0809', 0, async function (done) {
        console.info('system_parameter_test_0809 start');
        let ret = false;
        SetParameter("test.wait_param.204", "104");
        try {
            systemparameter.wait("test.wait_param.204", "*", 1, function (err, data) {
                if (err == undefined || err.code === 0) {
                    ret = true;
                    console.info("system_parameter_test_0809 test.wait_param.204 success")
                } else {
                    ret = false;
                    console.info("system_parameter_test_0809 callback test.wait_param.204 err:" + err.code)
                }
                expect(ret).assertTrue();
            });
        } catch (e) {
            expect(ret).assertTrue();
            console.info("promise get input error: " + e);
        }
        setTimeout(function () {
            expect(ret).assertTrue();
            done();
        }, '1000');
        console.info('system_parameter_test_0809 end');
    })

    /**
     * @tc.number    SUB_STARTUP_JS_SYSTEM_PARAMETER_0810
     * @tc.name      testWait05
     * @tc.desc      Waits the value of the attribute with the specified key.
     */
    it('system_parameter_test_0810', 0, async function (done) {
        console.info('system_parameter_test_0810 start');
        let ret = false;
        SetParameter("test.wait_param.205", "105");
        try {
            systemparameter.wait("test.wait_param.205", "*", 1, function (err, data) {
                if (err == undefined || err.code === 0) {
                    ret = true;
                    console.info("system_parameter_test_0810 test.wait_param.205 success:" + data)
                } else {
                    console.info("system_parameter_test_0810 test.wait_param.205 err:" + err.code)
                }
                expect(ret).assertTrue();
            });
        } catch (e) {
            ret = false;
            console.info("promise get input error: " + e);
        }
        setTimeout(function () {
            expect(ret).assertTrue();
            done();
        }, '1000');
        console.info('system_parameter_test_0810 end');
    })
})