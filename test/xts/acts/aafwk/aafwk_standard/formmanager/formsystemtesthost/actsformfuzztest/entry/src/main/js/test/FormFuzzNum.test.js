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

import formManager from '@ohos.ability.formManager'
import {describe, it, expect} from 'deccjsunit/index'
import {stringTest, numberTest, booleanTest, nullTest, undefinedTest,
    objectTest, arrayTest, functionTest, REPEAT_COUNT} from './getParam.js'

const TIMEOUT = 8000;
describe('ActsFormFuzzNumTest', function () {
    console.log("ActsFormFuzzNumTest===start");
    for (var i = 0; i < REPEAT_COUNT; i++) {
        
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0200
         * @tc.desc Check whether the deleteForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0200_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0200_callback deleteForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0200_callback deleteForm formId typeof:" + typeof(formId));
    
            try{
            formManager.deleteForm(
                    formId,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0200_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0200_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0200_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0200_callback Exception caught:" + e);
                expect(e.message.indexOf("type of deleteForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0200_callback deleteForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0200_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0200
         * @tc.desc Check whether the deleteForm interface can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_0200_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0200_promise deleteForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0200_promise deleteForm formId typeof:" + typeof(formId));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.deleteForm(formId));
                console.log("FMS_fuzzTestNum_0200_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0200_promise Exception caught:" + e);
                expect(e.message.indexOf("type of deleteForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0200_promise deleteForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0200_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0300_param_01_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_01_callback releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_01_callback releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = arrayTest();
            console.log("FMS_fuzzTestNum_0300_param_01_callback releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            try{
                formManager.releaseForm(
                    formId,
                    isReleaseCache,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0300_param_01_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0300_param_01_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0300_param_01_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_01_callback Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_01_callback releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_01_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_0300_param_01_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_01_promise releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_01_promise releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = arrayTest();
            console.log("FMS_fuzzTestNum_0300_param_01_promise releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.releaseForm(formId, isReleaseCache));
                console.log("FMS_fuzzTestNum_0300_param_01_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_01_promise Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_01_promise releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_01_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0300_param_02_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_02_callback releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_02_callback releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = booleanTest();
            console.log("FMS_fuzzTestNum_0300_param_02_callback releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            try{
                formManager.releaseForm(
                    formId,
                    isReleaseCache,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0300_param_02_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0300_param_02_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0300_param_02_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_02_callback Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_02_callback releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_02_callback==================end');
            }, TIMEOUT)
        })
    
        /**
            * @tc.name fuzzTest
            * @tc.number FMS_fuzzTestNum_0300
            * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by Promise)
            */
        it('FMS_fuzzTestNum_0300_param_02_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_02_promise releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_02_promise releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = booleanTest();
            console.log("FMS_fuzzTestNum_0300_param_02_promise releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.releaseForm(formId, isReleaseCache));
                console.log("FMS_fuzzTestNum_0300_param_02_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_02_promise Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_02_promise releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_02_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0300_param_03_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_03_callback releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_03_callback releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = functionTest();
            console.log("FMS_fuzzTestNum_0300_param_03_callback releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            try{
                formManager.releaseForm(
                    formId,
                    isReleaseCache,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0300_param_03_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0300_param_03_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0300_param_03_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_03_callback Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_03_callback releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_03_callback==================end');
            }, TIMEOUT)
        })
    
        /**
            * @tc.name fuzzTest
            * @tc.number FMS_fuzzTestNum_0300
            * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by Promise)
            */
        it('FMS_fuzzTestNum_0300_param_03_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_03_promise releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_03_promise releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = functionTest();
            console.log("FMS_fuzzTestNum_0300_param_03_promise releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.releaseForm(formId, isReleaseCache));
                console.log("FMS_fuzzTestNum_0300_param_03_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_03_promise Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_03_promise releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_03_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0300_param_04_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_04_callback releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_04_callback releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = nullTest();
            console.log("FMS_fuzzTestNum_0300_param_04_callback releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            try{
                formManager.releaseForm(
                    formId,
                    isReleaseCache,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0300_param_04_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0300_param_04_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0300_param_04_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_04_callback Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_04_callback releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_04_callback==================end');
            }, TIMEOUT)
        })
    
        /**
            * @tc.name fuzzTest
            * @tc.number FMS_fuzzTestNum_0300
            * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by Promise)
            */
        it('FMS_fuzzTestNum_0300_param_04_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_04_promise releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_04_promise releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = nullTest();
            console.log("FMS_fuzzTestNum_0300_param_04_promise releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.releaseForm(formId, isReleaseCache));
                console.log("FMS_fuzzTestNum_0300_param_04_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_04_promise Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_04_promise releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_04_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0300_param_05_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_05_callback releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_05_callback releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_05_callback releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            try{
                formManager.releaseForm(
                    formId,
                    isReleaseCache,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0300_param_05_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0300_param_05_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0300_param_05_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_05_callback Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_05_callback releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_05_callback==================end');
            }, TIMEOUT)
        })
    
        /**
            * @tc.name fuzzTest
            * @tc.number FMS_fuzzTestNum_0300
            * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by Promise)
            */
        it('FMS_fuzzTestNum_0300_param_05_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_05_promise releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_05_promise releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_05_promise releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.releaseForm(formId, isReleaseCache));
                console.log("FMS_fuzzTestNum_0300_param_05_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_05_promise Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_05_promise releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_05_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0300_param_06_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_06_callback releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_06_callback releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = objectTest();
            console.log("FMS_fuzzTestNum_0300_param_06_callback releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            try{
                formManager.releaseForm(
                    formId,
                    isReleaseCache,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0300_param_06_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0300_param_06_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0300_param_06_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_06_callback Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_06_callback releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_06_callback==================end');
            }, TIMEOUT)
        })
    
        /**
            * @tc.name fuzzTest
            * @tc.number FMS_fuzzTestNum_0300
            * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by Promise)
            */
        it('FMS_fuzzTestNum_0300_param_06_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_06_promise releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_06_promise releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = objectTest();
            console.log("FMS_fuzzTestNum_0300_param_06_promise releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.releaseForm(formId, isReleaseCache));
                console.log("FMS_fuzzTestNum_0300_param_06_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_06_promise Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_06_promise releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_06_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0300_param_07_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_07_callback releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_07_callback releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = stringTest();
            console.log("FMS_fuzzTestNum_0300_param_07_callback releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            try{
                formManager.releaseForm(
                    formId,
                    isReleaseCache,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0300_param_07_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0300_param_07_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0300_param_07_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_07_callback Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_07_callback releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_07_callback==================end');
            }, TIMEOUT)
        })
    
        /**
            * @tc.name fuzzTest
            * @tc.number FMS_fuzzTestNum_0300
            * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by Promise)
            */
        it('FMS_fuzzTestNum_0300_param_07_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_07_promise releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_07_promise releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = stringTest();
            console.log("FMS_fuzzTestNum_0300_param_07_promise releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.releaseForm(formId, isReleaseCache));
                console.log("FMS_fuzzTestNum_0300_param_07_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_07_promise Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_07_promise releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_07_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0300_param_08_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_08_callback releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_08_callback releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = undefinedTest();
            console.log("FMS_fuzzTestNum_0300_param_08_callback releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            try{
                formManager.releaseForm(
                    formId,
                    isReleaseCache,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0300_param_08_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0300_param_08_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0300_param_08_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_08_callback Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_08_callback releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_08_callback==================end');
            }, TIMEOUT)
        })
    
        /**
            * @tc.name fuzzTest
            * @tc.number FMS_fuzzTestNum_0300
            * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by Promise)
            */
        it('FMS_fuzzTestNum_0300_param_08_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_08_promise releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_08_promise releaseForm formId typeof:" + typeof(formId));
    
            var isReleaseCache = undefinedTest();
            console.log("FMS_fuzzTestNum_0300_param_08_promise releaseForm isReleaseCache typeof:"
                + typeof(isReleaseCache));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.releaseForm(formId, isReleaseCache));
                console.log("FMS_fuzzTestNum_0300_param_08_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_08_promise Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_08_promise releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_08_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_0300
         * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_0300_param_09_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_09_callback releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_09_callback releaseForm formId typeof:"
                + typeof(formId) + ",formId:" + formId);
    
            try{
                formManager.releaseForm(
                    formId,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_0300_param_09_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_0300_param_09_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_0300_param_09_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
    
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_09_callback Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_09_callback releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_09_callback==================end');
            }, TIMEOUT)
        })
    
        /**
            * @tc.name fuzzTest
            * @tc.number FMS_fuzzTestNum_0300
            * @tc.desc Check whether the releaseForm interface can pass the fuzzy test (by Promise)
            */
        it('FMS_fuzzTestNum_0300_param_09_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_0300_param_09_promise releaseForm begin");
    
            var formId = numberTest();
            console.log("FMS_fuzzTestNum_0300_param_09_promise releaseForm formId typeof:"
                + typeof(formId) + ",formId:" + formId);
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.releaseForm(formId)); 
                console.log("FMS_fuzzTestNum_0300_param_09_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_0300_param_09_promise Exception caught:" + e);
                expect(e.message.indexOf("type of releaseForm is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_0300_param_09_promise releaseForm end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_0300_param_09_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1200
         * @tc.desc Check whether the getAllFormsInfo interface can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1200_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1200_callback getAllFormsInfo begin");
    
            var testParam = numberTest();
            console.log("FMS_fuzzTestNum_1200_callback getAllFormsInfo formIds typeof:" + typeof(testParam));
    
            try{
                formManager.getAllFormsInfo(
                    testParam,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1200_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1200_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1200_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1200_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getAllFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1200_callback getAllFormsInfo end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1200_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1200
         * @tc.desc Check whether the getAllFormsInfo interface can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1200_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1200_promise getAllFormsInfo begin");
    
            var testParam = numberTest();
            console.log("FMS_fuzzTestNum_1200_promise getAllFormsInfo formIds typeof:" + typeof(testParam));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getAllFormsInfo(testParam));
                console.log("FMS_fuzzTestNum_1200_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1200_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getAllFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1200_promise getAllFormsInfo end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1200_promise==================end');
            }, TIMEOUT)
        })
        
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1300
         * @tc.desc Check whether the form configuration information query interface of the specified package 
         * can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1300_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1300_callback getFormsInfoByApp begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1300_callback getFormsInfoByApp bundleName typeof:" + typeof(bundleName));
    
            try{
                formManager.getFormsInfo(
                    bundleName,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1300_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1300_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1300_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1300_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1300_callback getFormsInfoByApp end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1300_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1300
         * @tc.desc Check whether the form configuration information query interface of the specified package 
         * can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1300_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1300_promise getFormsInfoByApp begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1300_promise getFormsInfoByApp bundleName typeof:" + typeof(bundleName));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getFormsInfo(bundleName));
                console.log("FMS_fuzzTestNum_1300_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1300_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1300_promise getFormsInfoByApp end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1300_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1400_param_01_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_01_callback getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_01_callback getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = arrayTest();
            console.log("FMS_fuzzTestNum_1400_param_01_callback getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            try{
                formManager.getFormsInfo(
                    bundleName,
                    moduleName,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1400_param_01_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1400_param_01_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1400_param_01_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_01_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_01_callback getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_01_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1400_param_01_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_01_promise getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_01_promise getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = arrayTest();
            console.log("FMS_fuzzTestNum_1400_param_01_promise getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getFormsInfo(bundleName, moduleName));
                console.log("FMS_fuzzTestNum_1400_param_01_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_01_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_01_promise getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_01_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1400_param_02_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_02_callback getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_02_callback getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = booleanTest();
            console.log("FMS_fuzzTestNum_1400_param_02_callback getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            try{
                formManager.getFormsInfo(
                    bundleName,
                    moduleName,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1400_param_02_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1400_param_02_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1400_param_02_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_02_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_02_callback getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_02_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1400_param_02_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_02_promise getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_02_promise getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = booleanTest();
            console.log("FMS_fuzzTestNum_1400_param_02_promise getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getFormsInfo(bundleName, moduleName));
                console.log("FMS_fuzzTestNum_1400_param_02_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_02_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_02_promise getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_02_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1400_param_03_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_03_callback getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_03_callback getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = functionTest();
            console.log("FMS_fuzzTestNum_1400_param_03_callback getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            try{
                formManager.getFormsInfo(
                    bundleName,
                    moduleName,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1400_param_03_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1400_param_03_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1400_param_03_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_03_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_03_callback getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_03_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1400_param_03_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_03_promise getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_03_promise getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = functionTest();
            console.log("FMS_fuzzTestNum_1400_param_03_promise getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getFormsInfo(bundleName, moduleName));
                console.log("FMS_fuzzTestNum_1400_param_03_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_03_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_03_promise getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_03_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1400_param_04_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_04_callback getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_04_callback getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = nullTest();
            console.log("FMS_fuzzTestNum_1400_param_04_callback getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            try{
                formManager.getFormsInfo(
                    bundleName,
                    moduleName,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1400_param_04_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1400_param_04_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1400_param_04_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_04_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_04_callback getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_04_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1400_param_04_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_04_promise getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_04_promise getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = nullTest();
            console.log("FMS_fuzzTestNum_1400_param_04_promise getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getFormsInfo(bundleName, moduleName));
                console.log("FMS_fuzzTestNum_1400_param_04_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_04_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_04_promise getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_04_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1400_param_05_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_05_callback getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_05_callback getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_05_callback getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            try{
                formManager.getFormsInfo(
                    bundleName,
                    moduleName,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1400_param_05_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1400_param_05_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1400_param_05_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_05_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_05_callback getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_05_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1400_param_05_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_05_promise getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_05_promise getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_05_promise getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getFormsInfo(bundleName, moduleName));
                console.log("FMS_fuzzTestNum_1400_param_05_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_05_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_05_promise getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_05_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1400_param_06_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_06_callback getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_06_callback getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = objectTest();
            console.log("FMS_fuzzTestNum_1400_param_06_callback getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            try{
                formManager.getFormsInfo(
                    bundleName,
                    moduleName,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1400_param_06_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1400_param_06_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1400_param_06_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_06_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_06_callback getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_06_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1400_param_06_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_06_promise getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_06_promise getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = objectTest();
            console.log("FMS_fuzzTestNum_1400_param_06_promise getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getFormsInfo(bundleName, moduleName));
                console.log("FMS_fuzzTestNum_1400_param_06_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_06_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_06_promise getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_06_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1400_param_07_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_07_callback getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_07_callback getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = stringTest();
            console.log("FMS_fuzzTestNum_1400_param_07_callback getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            try{
                formManager.getFormsInfo(
                    bundleName,
                    moduleName,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1400_param_07_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1400_param_07_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1400_param_07_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_07_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_07_callback getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_07_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1400_param_07_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_07_promise getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_07_promise getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = stringTest();
            console.log("FMS_fuzzTestNum_1400_param_07_promise getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getFormsInfo(bundleName, moduleName));
                console.log("FMS_fuzzTestNum_1400_param_07_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_07_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_07_promise getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_07_promise==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by AsyncCallback)
         */
        it('FMS_fuzzTestNum_1400_param_08_callback', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_08_callback getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_08_callback getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = undefinedTest();
            console.log("FMS_fuzzTestNum_1400_param_08_callback getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            try{
                formManager.getFormsInfo(
                    bundleName,
                    moduleName,
                    (err,data) => {
                        console.log("FMS_fuzzTestNum_1400_param_08_callback async::callback return!!!")
                        console.log("FMS_fuzzTestNum_1400_param_08_callback async::success data:" + data);
                        console.log("FMS_fuzzTestNum_1400_param_08_callback async::error err:" + err);
                        fail("should not reach here");
                        done();
                    }
                );
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_08_callback Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_08_callback getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_08_callback==================end');
            }, TIMEOUT)
        })
    
        /**
         * @tc.name fuzzTest
         * @tc.number FMS_fuzzTestNum_1400
         * @tc.desc Check whether the form configuration information query interface of the specified module 
         * can pass the fuzzy test (by Promise)
         */
        it('FMS_fuzzTestNum_1400_param_08_promise', 0, async function (done) {
            console.log("FMS_fuzzTestNum_1400_param_08_promise getFormsInfoByModule begin");
    
            var bundleName = numberTest();
            console.log("FMS_fuzzTestNum_1400_param_08_promise getFormsInfoByModule bundleName typeof:"
                + typeof(bundleName));
    
            var moduleName = undefinedTest();
            console.log("FMS_fuzzTestNum_1400_param_08_promise getFormsInfoByModule moduleName typeof:"
                + typeof(moduleName));
    
            var retResult;
            try{
                retResult = await Promise.all(await formManager.getFormsInfo(bundleName, moduleName));
                console.log("FMS_fuzzTestNum_1400_param_08_promise async::success retResult:" + retResult);
                fail("should not reach here");
                done();
            } catch (e) {
                console.log("FMS_fuzzTestNum_1400_param_08_promise Exception caught:" + e);
                expect(e.message.indexOf("type of getFormsInfo is incorrect") > -1).assertTrue();
            }
    
            console.log("FMS_fuzzTestNum_1400_param_08_promise getFormsInfoByModule end");
            done();
            setTimeout(function () {
                console.info('=====================FMS_fuzzTestNum_1400_param_08_promise==================end');
            }, TIMEOUT)
        })
        
    }
})