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
        console.info('FaultlogJsTest beforeAll called')
    })

    afterAll(function() {
        console.info('FaultlogJsTest afterAll called')
    })

    beforeEach(function() {
        console.info('FaultlogJsTest beforeEach called')
    })

    afterEach(function() {
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
            expect(ret1).assertEqual(undefined);

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
     * @tc.desc: 检验promise同步方式获取faultlog CPP_CRASH日志
     * @tc.require: AR000GICT2
     * @tc.author:
     */
    it('FaultlogJsTest_002', 0, async function (done) {
        console.info("---------------------------FaultlogJsTest_002----------------------------------");
        try {
            let now = Date.now();
            console.info("FaultlogJsTest_002 2 + " + now);
            let module = "ohos.faultloggerjs.test";
            const loopTimes = 10;
            for (let i = 0; i < loopTimes; i++) {
                console.info("--------FaultlogJsTest_002 3 + " + i + "----------");
                faultlogger.addFaultLog(i + 100, 
                    faultlogger.FaultType.CPP_CRASH, module, "faultloggertestsummary02 " + i);
                await msleep(300);
            }
            await msleep(1000);

            console.info("--------FaultlogJsTest_002 4" + "----------");
            let ret = await faultlogger.querySelfFaultLog(faultlogger.FaultType.CPP_CRASH);
            console.info("FaultlogJsTest_002 ret == " + ret.length);
            expect(ret.length).assertEqual(loopTimes);
            for (let i = 0; i < loopTimes; i++) {
                console.info("faultloggertestsummary02 " + i + " fullLog.length " + ret[i].fullLog.length);
                if (ret[i].fullLog.indexOf("faultloggertestsummary02 " + (loopTimes - 1 - i)) != -1) {
                    console.info("FaultlogJsTest_002 " + ret[i].fullLog.length);
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
            }

            console.info("--------FaultlogJsTest_002 5" + "----------");
            ret = await faultlogger.querySelfFaultLog(faultlogger.FaultType.NO_SPECIFIC);
            console.info("FaultlogJsTest_002 ret == " + ret.length);
            expect(ret.length).assertEqual(loopTimes);
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
     * @tc.desc: 检验promise同步方式获取faultlog JS_CRASH日志
     * @tc.require: AR000GICT2
     * @tc.author:
     */
     it('FaultlogJsTest_003', 0, async function (done) {
        console.info("---------------------------FaultlogJsTest_003----------------------------------");
        try {
            let now = Date.now();
            console.info("FaultlogJsTest_003 2 + " + now);
            const loopTimes = 2;
            let i = 0;
            let pro = new Promise(
                (r, e) => {
                    setTimeout(function run() {
                        if (i < loopTimes) {
                            setTimeout(run, 1001);
                        } else {
                            r("done!")
                            return
                        }
                        console.info("--------FaultlogJsTest_003 3 + " + i + "----------");
                        ++i;
                        let dataStr = ["1", "2"]
                        console.info(dataStr[2].test);
                    }, 1001);
                }
            );
            await pro;
            await msleep(1000);

            console.info("--------FaultlogJsTest_003 4" + "----------");
            let ret = await faultlogger.querySelfFaultLog(faultlogger.FaultType.JS_CRASH);
            console.info("FaultlogJsTest_003 ret == " + ret.length);
            if (ret.length > 0) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            for (let i = 0; i < ret.length; i++) {
                console.info("faultloggertestsummary03 " + i + " fullLog.length " + ret[i].fullLog.length);
            }
            done();
            return;
        } catch (err) {
            console.info("catch (err) == " + err);
        }
        console.info("FaultlogJsTest_003 error");
        expect(false).assertTrue();
        done();
    })

    /**
     * test
     *
     * @tc.name: FaultlogJsTest_004
     * @tc.desc: 检验promise同步方式获取faultlog APP_FREEZE日志
     * @tc.require: AR000GICT2
     * @tc.author:
     */
     it('FaultlogJsTest_004', 0, async function (done) {
        console.info("---------------------------FaultlogJsTest_004----------------------------------");
        try {
            let now = Date.now();
            console.info("FaultlogJsTest_004 2 + " + now);
            let module = "ohos.faultloggerjs.test";
            const loopTimes = 10;
            for (let i = 0; i < loopTimes; i++) {
                console.info("--------FaultlogJsTest_004 3 + " + i + "----------");
                faultlogger.addFaultLog(i + 300, 
                    faultlogger.FaultType.APP_FREEZE, module, "faultloggertestsummary04 " + i);
                await msleep(300);
            }
            await msleep(1000);

            console.info("--------FaultlogJsTest_004 4" + "----------");
            let ret = await faultlogger.querySelfFaultLog(faultlogger.FaultType.APP_FREEZE);
            console.info("FaultlogJsTest_004 ret == " + ret.length);
            expect(ret.length).assertEqual(loopTimes);
            for (let i = 0; i < loopTimes; i++) {
                console.info("faultloggertestsummary04 " + i + " fullLog.length " + ret[i].fullLog.length);
                if (ret[i].fullLog.indexOf("faultloggertestsummary04 " + (loopTimes - 1 - i)) != -1) {
                    console.info("FaultlogJsTest_004 " + ret[i].fullLog.length);
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
        console.info("FaultlogJsTest_004 error");
        expect(false).assertTrue();
        done();
    })

    /**
     * test
     *
     * @tc.name: FaultlogJsTest_005
     * @tc.desc: 检验通过回调方式获取faultlog日志
     * @tc.require: AR000GICT2
     * @tc.author:
     */
    it('FaultlogJsTest_005', 0, async function (done) {
        console.info("---------------------------FaultlogJsTest_005----------------------------------");
        try {
            let now = Date.now();
            console.info("FaultlogJsTest_005 start + " + now);
            let module = "ohos.faultloggerjs.test";
            const loopTimes = 10;
            for (let i = 0; i < loopTimes; i++) {
                console.info("--------FaultlogJsTest_005 + " + i + "----------");
                faultlogger.addFaultLog(i + 400,
                    faultlogger.FaultType.CPP_CRASH, module, "faultloggertestsummary05 " + i);
                await msleep(300);
            }
            await msleep(1000);

            console.info("--------FaultlogJsTest_005 4----------");
            function queryFaultLogCallback(error, ret) {
                if (error) {
                    console.info('FaultlogJsTest_005  once error is ' + error);
                } else {
                    console.info("FaultlogJsTest_005 ret == " + ret.length);
                    expect(ret.length).assertEqual(loopTimes);
                    for (let i = 0; i < loopTimes; i++) {
                        console.info("faultloggertestsummary05 " + i + " fullLog.length " + ret[i].fullLog.length);
                        if (ret[i].fullLog.indexOf("faultloggertestsummary05 " + (loopTimes - 1 - i)) != -1) {
                            console.info("FaultlogJsTest_005 " + ret[i].fullLog.length);
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
        console.info("FaultlogJsTest_005 error");
        expect(false).assertTrue();
        done();
    })
})