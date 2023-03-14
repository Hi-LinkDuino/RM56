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
import faultlogger from '@ohos.faultLogger'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("FaultlogJsTest", function () {
    beforeAll(function() {
        /*
         * @tc.setup: setup invoked before all testcases
         */
         console.info('FaultlogJsTest beforeAll called')
    })

    afterAll(function() {
        /*
         * @tc.teardown: teardown invoked after all testcases
         */
         console.info('FaultlogJsTest afterAll called')
    })

    beforeEach(function() {
        /*
         * @tc.setup: setup invoked before each testcases
         */
         console.info('FaultlogJsTest beforeEach called')
    })

    afterEach(function() {
        /*
         * @tc.teardown: teardown invoked after each testcases
         */
         console.info('FaultlogJsTest afterEach called')
    })

    async function msleep(time) {
        let promise = new Promise((resolve, reject) => {
            setTimeout(() => resolve("done!"), time)
        });
        let result = await promise;
    }

    /**
     * test
     *
     * @tc.name: FaultlogJsException_001
     * @tc.desc: 检验函数参数输入错误时程序是否会崩溃
     * @tc.require: AR000GICT2
     * @tc.author:
     */
    it('FaultlogJsException_001', 0, async function (done) {
        console.info("---------------------------FaultlogJsException_001----------------------------------");
        try {
            let ret1 = faultlogger.querySelfFaultLog("faultloggertestsummary01");
            console.info("FaultlogJsException_001 ret1 == " + ret1);
            let a = expect(ret1).assertEqual(undefined);
            console.info('ret1 assertEqual(undefined) ' + a);

            let ret2 = faultlogger.querySelfFaultLog(faultlogger.FaultType.JS_CRASH, "faultloggertestsummary01");
            console.info("FaultlogJsException_001 ret2 == " + ret2);
            expect(ret2).assertEqual(undefined);

            let ret3 = faultlogger.querySelfFaultLog();
            console.info("FaultlogJsException_001 ret3 == " + ret3);
            expect(ret3).assertEqual(undefined);
            done();
            return;
        } catch(err) {
            console.info(err);
        }
        expect(false).assertTrue();
        done();
    })


    /**
     * test
     *
     * @tc.name: FaultlogJsTest_002
     * @tc.desc: 检验promise同步方式获取faultlog日志
     * @tc.require: AR000GICT2
     * @tc.author:
     */
    it('FaultlogJsTest_002', 0, async function (done) {
        console.info("---------------------------FaultlogJsTest_002----------------------------------");
        try {
            let now = Date.now();
            console.info("FaultlogJsTest_002 2 + " + now);
            let module = "com.ohos.hiviewtest.faultlogjs";
            const loopTimes = 10;
            for (let i = 0; i < loopTimes; i++) {
                console.info("--------FaultlogJsTest_002 3 + " + i + "----------");
                faultlogger.addFaultLog(i, faultlogger.FaultType.CPP_CRASH, module, "faultloggertestsummary02 " + i);
                await msleep(300);
            }
            await msleep(1000);

            console.info("--------FaultlogJsTest_002 4" + "----------");
            let ret = await faultlogger.querySelfFaultLog(faultlogger.FaultType.CPP_CRASH);
            console.info("FaultlogJsTest_002 ret == " + ret.length);
            expect(ret.length).assertEqual(loopTimes);
            for (let i = 0; i < loopTimes; i++) {
                console.info("faultloggertestsummary02 " + i + " fullLog.length " + ret[i].fullLog.length);
                console.info(ret[i].fullLog);
                if (ret[i].fullLog.indexOf("faultloggertestsummary02 " + (loopTimes - 1 - i)) != -1) {
                    console.info("FaultlogJsTest_002 " + ret[i].fullLog.length);
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
            }
            done();
            return;
        } catch (err) {
            console.info("catch (err) == " + err);
        }
        console.info("FaultlogJsTest_002 error");
        expect(false).assertTrue();
        done();
    })

    /**
     * test
     *
     * @tc.name: FaultlogJsTest_003
     * @tc.desc: 检验通过回调方式获取faultlog日志
     * @tc.require: AR000GICT2
     * @tc.author:
     */
    it('FaultlogJsTest_003', 0, async function (done) {
        console.info("---------------------------FaultlogJsTest_003----------------------------------");
        try {
            let now = Date.now();
            console.info("FaultlogJsTest_003 start + " + now);
            let module = "com.ohos.hiviewtest.faultlogjs";
            const loopTimes = 10;
            for (let i = 0; i < loopTimes; i++) {
                console.info("--------FaultlogJsTest_003 + " + i + "----------");
                faultlogger.addFaultLog(i + 20,
                    faultlogger.FaultType.CPP_CRASH, module, "faultloggertestsummary03 " + i);
                await msleep(300);
            }
            await msleep(1000);

            console.info("--------FaultlogJsTest_003 4----------");
            function queryFaultLogCallback(error, ret) {
                if (error) {
                    console.info('FaultlogJsTest_003  once error is ' + error);
                } else {
                    console.info("FaultlogJsTest_003 ret == " + ret.length);
                    expect(ret.length).assertEqual(loopTimes);
                    for (let i = 0; i < loopTimes; i++) {
                        console.info("faultloggertestsummary03 " + i + " fullLog.length " + ret[i].fullLog.length);
                        console.info(ret[i].fullLog);
                        if (ret[i].fullLog.indexOf("faultloggertestsummary03 " + (loopTimes - 1 - i)) != -1) {
                            console.info("FaultlogJsTest_003 " + ret[i].fullLog.length);
                            expect(true).assertTrue();
                        } else {
                            expect(false).assertTrue();
                        }
                    }
                }
                done();
            }
            faultlogger.querySelfFaultLog(faultlogger.FaultType.CPP_CRASH, queryFaultLogCallback);
            return;
        } catch (err) {
            console.info(err);
        }
        console.info("FaultlogJsTest_003 error");
        expect(false).assertTrue();
        done();
    })
})