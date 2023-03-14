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

const TIMEOUT = 5000;
describe('ActsFormsOneAndTwoTest', function () {
    console.log("ActsFormsOneAndTwoTest===start");

    /**
     * @tc.name getAllFormsInfo test
     * @tc.number FMS_getFormsInfo_0600
     * @tc.desc All form configuration information is queried successfully (by AsyncCallback)
     */
    it('FMS_getFormsInfo_0600_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_0600_callback begin");

        var ret = formManager.getAllFormsInfo(
            (result, data) => {
                console.log("FMS_getFormsInfo_0600_callback, the forms number:" + data.length);
                var dataA = data.filter((p) => {
                    return p.bundleName == "com.form.formsystemtestservicea";
                });
                var dataB = data.filter((p) => {
                    return p.bundleName == "com.form.formsystemtestserviceb";
                });
                console.log("FMS_getFormsInfo_0600_callback, the formA number:" + dataA.length);
                console.log("FMS_getFormsInfo_0600_callback async::sucess, dataA json:" + JSON.stringify(dataA));
                console.log("FMS_getFormsInfo_0600_callback async::sucess, dataB json:" + JSON.stringify(dataB));
                console.log("FMS_getFormsInfo_0600_callback getAllFormsInfo async::result, result:" + result);
                expect(result).assertEqual(1);
                function  expe(datainfo) {
                    for(var i = 0; i < datainfo.length; i++) {
                        expect(datainfo[i].description).assertEqual("form_description");
                        expect(datainfo[i].type).assertEqual(1);
                        expect(datainfo[i].colorMode).assertEqual(-1);
                        expect(datainfo[i].jsComponentName).assertEqual("card");
                        expect(datainfo[i].isDefault).assertEqual(1);
                        expect(datainfo[i].updateEnabled).assertEqual(1);
                        expect(datainfo[i].updateDuration).assertEqual(1);
                        expect(datainfo[i].scheduledUpdateTime).assertEqual("10:30");
                        expect(datainfo[i].formVisibleNotify).assertEqual(1);
                        expect(datainfo[i].defaultDimension).assertEqual(1);
                        expect(datainfo[i].supportDimensions[0]).assertEqual(1);
                        expect(datainfo[i].supportDimensions[1]).assertEqual(2);
                        expect(datainfo[i].metaData.customizeData[0].name).assertEqual("originWidgetName");
                        expect(datainfo[i].metaData.customizeData[0].value).assertEqual("myTest");
                        console.log("FMS_getFormsInfo_0600_promise=========expe, formName:" + datainfo[i].name);
                    }
                }
                expe(dataA);
                expe(dataB);
                expect(dataA[0].name).assertEqual("FormA_Js001");
                expect(dataA[1].name).assertEqual("FormA_Js002");
                expect(dataB[0].name).assertEqual("FormB_Js001");
                console.log("FMS_getFormsInfo_0600_callback result end");
                done();
            }
        );
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_0600_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_0600_callback end, ret:" + ret);
    })

    /**
     * @tc.name getAllFormsInfo test
     * @tc.number FMS_getFormsInfo_0600
     * @tc.desc All form configuration information is queried successfully (by Promise)
     */
    it('FMS_getFormsInfo_0600_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_0600_promise begin");

        var promise = formManager.getAllFormsInfo();
        promise.then((data) => {
            console.log("FMS_getFormsInfo_0600_promise, the forms number:" + data.length);
            var dataA = data.filter((p) => {
                return p.bundleName == "com.form.formsystemtestservicea";
            });
            var dataB = data.filter((p) => {
                return p.bundleName == "com.form.formsystemtestserviceb";
            });
            console.log("FMS_getFormsInfo_0600_promise, the formA number:" + dataA.length);
            console.log("FMS_getFormsInfo_0600_promise async::sucess, dataA json:" + JSON.stringify(dataA));
            console.log("FMS_getFormsInfo_0600_promise async::sucess, dataB json:" + JSON.stringify(dataB));

            function  expe(datainfo) {
                for(var i = 0; i < datainfo.length; i++) {
                    expect(datainfo[i].description).assertEqual("form_description");
                    expect(datainfo[i].type).assertEqual(1);
                    expect(datainfo[i].colorMode).assertEqual(-1);
                    expect(datainfo[i].jsComponentName).assertEqual("card");
                    expect(datainfo[i].isDefault).assertEqual(1);
                    expect(datainfo[i].updateEnabled).assertEqual(1);
                    expect(datainfo[i].updateDuration).assertEqual(1);
                    expect(datainfo[i].scheduledUpdateTime).assertEqual("10:30");
                    expect(datainfo[i].formVisibleNotify).assertEqual(1);
                    expect(datainfo[i].defaultDimension).assertEqual(1);
                    expect(datainfo[i].supportDimensions[0]).assertEqual(1);
                    expect(datainfo[i].supportDimensions[1]).assertEqual(2);
                    expect(datainfo[i].metaData.customizeData[0].name).assertEqual("originWidgetName");
                    expect(datainfo[i].metaData.customizeData[0].value).assertEqual("myTest");
                    console.log("FMS_getFormsInfo_0600_promise=========expe, formName:" + datainfo[i].name);
                }
            }
            expe(dataA);
            expe(dataB);
            expect(dataA[0].name).assertEqual("FormA_Js001");
            expect(dataA[1].name).assertEqual("FormA_Js002");
            expect(dataB[0].name).assertEqual("FormB_Js001");
            console.log("FMS_getFormsInfo_0600_promise result end");
            done();
        });
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_0600_promise==================end');
        }, TIMEOUT)
    })

    /**
     * @tc.name bundleName is null test
     * @tc.number FMS_getFormsInfo_0900
     * @tc.desc When the bundleName is null, the form configuration information query fails
     *  (by AsyncCallback)
     */
    it('FMS_getFormsInfo_0900_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_0900_callback begin");

        var ret = formManager.getFormsInfo(
            "",
            (result, data) => {
                console.log("FMS_getFormsInfo_0900_callback getFormsInfo async::sucess, data json:"
                    + JSON.stringify(data));
                console.log("FMS_getFormsInfo_0900_callback async::result, result:" + result);
                expect(result).assertEqual(0);
                console.log("FMS_getFormsInfo_0900_callback result end");
                done();
            }
        );

        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_0900_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_0900_callback end, ret:" + ret);
    })

    /**
     * @tc.name bundleName is null test
     * @tc.number FMS_getFormsInfo_0900
     * @tc.desc When the bundleName is null, the form configuration information query fails (by Promise)
     */
    it('FMS_getFormsInfo_0900_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_0900_promise begin");

        var promise = formManager.getFormsInfo(
            "",
        );
        promise.then((result) => {
            console.log("FMS_getFormsInfo_0900_promise result:" + result);
            expect(result).assertEqual(0);
            console.log("FMS_getFormsInfo_0900_promise result end");
            done();
        }
        );

        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_0900_promise==================end');
        }, TIMEOUT)
    })

    /**
     * @tc.name bundleName isn't exist test
     * @tc.number FMS_getFormsInfo_1000
     * @tc.desc When the bundleName isn't exist, the form configuration information query fails
     *  (by AsyncCallback).
     */
    it('FMS_getFormsInfo_1000_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_1000_callback begin");

        var ret = formManager.getFormsInfo(
            "aaaaaaa",
            (result, data) => {
                console.log("FMS_getFormsInfo_1000_callback getFormsInfo async::sucess, data json:"
                    + JSON.stringify(data));
                console.log("FMS_getFormsInfo_1000_callback async::result, result:" + result);
                expect(result).assertEqual(0);
                console.log("FMS_getFormsInfo_1000_callback result end");
                done();
            }
        );

        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1000_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_1000_callback end, ret:" + ret);
    })

    /**
     * @tc.name bundleName isn't exist test
     * @tc.number FMS_getFormsInfo_1000
     * @tc.desc When the bundleName isn't exist, the form configuration information query fails (by Promise)
     */
    it('FMS_getFormsInfo_1000_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_1000_promise begin");

        var promise = formManager.getFormsInfo(
            "aaaaaaa",
        );
        promise.then((result) => {
            console.log("FMS_getFormsInfo_1000_promise result:" + result);
            expect(result).assertEqual(0);
            console.log("FMS_getFormsInfo_1000_promise result end");
            done();
        }
        );

        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1000_promise==================end');
        }, TIMEOUT)
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1100
     * @tc.desc When the bundleName exist, the form configuration information is queried successfully
     * (by AsyncCallback)
     */
    it('FMS_getFormsInfo_1100_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_1100_callback begin");

        var ret = formManager.getFormsInfo(
            "com.form.formsystemtestservicea",
            (result, data) => {
                console.log("FMS_getFormsInfo_1100_callback getFormsInfo async::sucess, data json:"
                    + JSON.stringify(data));
                console.log("FMS_getFormsInfo_1100_callback async::result, result:" + result);
                for(var i = 0; i < data.length; i++) {
                    expect(typeof (data[i].description)).assertEqual("string");
                    expect(data[i].description).assertEqual("form_description");
                    expect(data[i].type).assertEqual(1);
                    expect(data[i].colorMode).assertEqual(-1);
                    expect(data[i].jsComponentName).assertEqual("card");
                    expect(data[i].isDefault).assertEqual(1);
                    expect(data[i].updateEnabled).assertEqual(1);
                    expect(data[i].updateDuration).assertEqual(1);
                    expect(data[i].scheduledUpdateTime).assertEqual("10:30");
                    expect(data[i].formVisibleNotify).assertEqual(1);
                    expect(data[i].defaultDimension).assertEqual(1);
                    expect(data[i].supportDimensions[0]).assertEqual(1);
                    expect(data[i].supportDimensions[1]).assertEqual(2);
                    expect(data[i].metaData.customizeData[0].name).assertEqual("originWidgetName");
                    expect(data[i].metaData.customizeData[0].value).assertEqual("myTest");
                }
                
                expect(data[0].name).assertEqual("FormA_Js001");
                expect(data[1].name).assertEqual("FormA_Js002");
                console.log("FMS_getFormsInfo_1100_callback result end");
                done();
            }
        );

        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1100_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_1100_callback end, ret:" + ret);
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1100
     * @tc.desc When the bundleName exist, the form configuration information is queried successfully
     *  (by Promise)
     */
    it('FMS_getFormsInfo_1100_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_1100_promise begin");

        var promise = formManager.getFormsInfo(
            "com.form.formsystemtestservicea",
        );
        promise.then((data) => {
            console.log("FMS_getFormsInfo_1100_promise async::sucess, data json:" + JSON.stringify(data));
            for(var i = 0; i < data.length; i++) {
                expect(typeof (data[i].description)).assertEqual("string");
                expect(data[i].description).assertEqual("form_description");
                expect(data[i].type).assertEqual(1);
                expect(data[i].colorMode).assertEqual(-1);
                expect(data[i].jsComponentName).assertEqual("card");
                expect(data[i].isDefault).assertEqual(1);
                expect(data[i].updateEnabled).assertEqual(1);
                expect(data[i].updateDuration).assertEqual(1);
                expect(data[i].scheduledUpdateTime).assertEqual("10:30");
                expect(data[i].formVisibleNotify).assertEqual(1);
                expect(data[i].defaultDimension).assertEqual(1);
                expect(data[i].supportDimensions[0]).assertEqual(1);
                expect(data[i].supportDimensions[1]).assertEqual(2);
                expect(data[i].metaData.customizeData[0].name).assertEqual("originWidgetName");
                expect(data[i].metaData.customizeData[0].value).assertEqual("myTest");
            }
            
            expect(data[0].name).assertEqual("FormA_Js001");
            expect(data[1].name).assertEqual("FormA_Js002");
            console.log("FMS_getFormsInfo_1100_promise result end");
            done();
        });       

        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1100_promise==================end');
        }, TIMEOUT)
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1400
     * @tc.desc When the bundleName is null, moduleName is exist，the form configuration information 
     * query fails (by AsyncCallback).
     */
    it('FMS_getFormsInfo_1400_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_1400_callback begin");

        var ret = formManager.getFormsInfo(
            "",
            "formmodule001",
            (result, data) => {
                console.log("FMS_getFormsInfo_1400_callback getFormsInfo async::sucess, data json:"
                    + JSON.stringify(data));
                console.log("FMS_getFormsInfo_1400_callback async::result, result:" + result);
                expect(result).assertEqual(0);
                console.log("FMS_getFormsInfo_1400_callback result end");
                done();
            }
        );
        
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1400_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_1400_callback end, ret:" + ret);
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1400
     * @tc.desc When the bundleName is null, moduleName is exist，the form configuration information
     *  query fails (by Promise)
     */
    it('FMS_getFormsInfo_1400_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_1400_promise begin");

        var promise = formManager.getFormsInfo(
            "",
            "formmodule001",
        );
        promise.then((result) => {
            console.log("FMS_getFormsInfo_1400_promise result:" + result);
            expect(result).assertEqual(0);
            console.log("FMS_getFormsInfo_1400_promise result end");
            done();
        }
        );
        
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1400_promise==================end');
        }, TIMEOUT)
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1500
     * @tc.desc When the bundleName isn't exist, moduleName is exist, the form configuration information
     *  query fails (by AsyncCallback)
     */
    it('FMS_getFormsInfo_1500_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_1500_callback begin");

        var ret = formManager.getFormsInfo(
            "aaaaaa",
            "formmodule001",
            (result, data) => {
                console.log("FMS_getFormsInfo_1500_callback getFormsInfo async::sucess, data json:"
                    + JSON.stringify(data));
                console.log("FMS_getFormsInfo_1500_callback async::result, result:" + result);
                expect(result).assertEqual(0);
                console.log("FMS_getFormsInfo_1500_callback result end");
                done();
            }
        );
        
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1500_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_1500_callback end, ret:" + ret);
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1500
     * @tc.desc When the bundleName isn't exist, moduleName is exist, the form configuration
     *  information query fails (by Promise)
     */
    it('FMS_getFormsInfo_1500_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_1500_promise begin");

        var promise = formManager.getFormsInfo(
            "aaaaaaa",
            "formmodule001",
        );
        promise.then((result) => {
            console.log("FMS_getFormsInfo_1500_promise result:" + result);
            expect(result).assertEqual(0);
            console.log("FMS_getFormsInfo_1500_promise result end");
            done();
        }
        );
        
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1500_promise==================end');
        }, TIMEOUT)
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1600
     * @tc.desc When the bundleName is exist, moduleName is null, the form configuration information
     * query fails (by AsyncCallback)
     */
    it('FMS_getFormsInfo_1600_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_1600_callback begin");

        var ret = formManager.getFormsInfo(
            "com.form.formsystemtestservicea",
            "",
            (result, data) => {
                console.log("FMS_getFormsInfo_1600_callback getFormsInfo async::sucess, data json:"
                    + JSON.stringify(data));
                console.log("FMS_getFormsInfo_1600_callback async::result, result:" + result);
                expect(result).assertEqual(0);
                console.log("FMS_getFormsInfo_1600_callback result end");
                done();
            }
        );
        
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1600_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_1600_callback end, ret:" + ret);
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1600
     * @tc.desc When the bundleName is exist, moduleName is null, the form configuration information
     *  query fails (by Promise)
     */
    it('FMS_getFormsInfo_1600_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_1600_promise begin");

        var promise = formManager.getFormsInfo(
            "com.form.formsystemtestservicea",
            "",
        );
        promise.then((result) => {
            console.log("FMS_getFormsInfo_1600_promise result:" + result);
            expect(result).assertEqual(0);
            console.log("FMS_getFormsInfo_1600_promise result end");
            done();
        }
        );
        
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1600_promise==================end');
        }, TIMEOUT)
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1700
     * @tc.desc When the bundleName is exist, moduleName is null, the form configuration information 
     * query fails (by AsyncCallback)
     */
    it('FMS_getFormsInfo_1700_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_1700_callback begin");

        var ret = formManager.getFormsInfo(
            "com.form.formsystemtestservicea",
            "aaaaa",
            (result, data) => {
                console.log("FMS_getFormsInfo_1700_callback getFormsInfo async::sucess, data json:"
                    + JSON.stringify(data));
                console.log("FMS_getFormsInfo_1700_callback async::result, result:" + result);
                expect(result).assertEqual(0);
                console.log("FMS_getFormsInfo_1700_callback result end");
                done();
            }
        );

        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1700_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_1700_callback end, ret:" + ret);
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1700
     * @tc.desc When the bundleName is exist, moduleName is null, the form configuration information 
     * query fails (by Promise)
     */
    it('FMS_getFormsInfo_1700_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_1700_promise begin");

        var promise = formManager.getFormsInfo(
            "com.form.formsystemtestservicea",
            "aaaaa",
        );
        promise.then((result) => {
            console.log("FMS_getFormsInfo_1700_promise result:" + result);
            expect(result).assertEqual(0);
            console.log("FMS_getFormsInfo_1700_promise result end");
            done();
        }
        );
        
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1700_promise==================end');
        }, TIMEOUT)
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1800
     * @tc.desc When the bundleName and moduleName is exist, the form configuration information is 
     * queried successfully (by AsyncCallback).
     */
    it('FMS_getFormsInfo_1800_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_1800_callback begin");

        var ret = formManager.getFormsInfo(
            "com.form.formsystemtestservicea",
            "formmodule001",
            (result, data) => {
                console.log("FMS_getFormsInfo_1800_callback getFormsInfo async::sucess, data json:"
                    + JSON.stringify(data));
                console.log("FMS_getFormsInfo_1800_callback async::result, result:" + result);
                for(var i = 0; i < data.length; i++) {
                    expect(typeof (data[i].description)).assertEqual("string");
                    expect(data[i].description).assertEqual("form_description");
                    expect(data[i].type).assertEqual(1);
                    expect(data[i].colorMode).assertEqual(-1);
                    expect(data[i].jsComponentName).assertEqual("card");
                    expect(data[i].isDefault).assertEqual(1);
                    expect(data[i].updateEnabled).assertEqual(1);
                    expect(data[i].updateDuration).assertEqual(1);
                    expect(data[i].scheduledUpdateTime).assertEqual("10:30");
                    expect(data[i].formVisibleNotify).assertEqual(1);
                    expect(data[i].defaultDimension).assertEqual(1);
                    expect(data[i].supportDimensions[0]).assertEqual(1);
                    expect(data[i].supportDimensions[1]).assertEqual(2);
                    expect(data[i].metaData.customizeData[0].name).assertEqual("originWidgetName");
                    expect(data[i].metaData.customizeData[0].value).assertEqual("myTest");
                }
                
                expect(data[0].name).assertEqual("FormA_Js001");
                expect(data[1].name).assertEqual("FormA_Js002");
                console.log("FMS_getFormsInfo_1800_callback result end");
                done();
            }
        );
                
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1800_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_1800_callback end, ret:" + ret);
    })

    /**
     * @tc.name getFormsInfo test
     * @tc.number FMS_getFormsInfo_1800
     * @tc.desc When the bundleName and moduleName is exist, the form configuration information is
     * queried successfully (by Promise)
     */
    it('FMS_getFormsInfo_1800_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_1800_promise begin");

        var promise = formManager.getFormsInfo(
            "com.form.formsystemtestservicea",
            "formmodule001",
        );
        promise.then((data) => {
            console.log("FMS_getFormsInfo_1800_promise async::sucess, data json:" + JSON.stringify(data));
            for(var i = 0; i < data.length; i++) {
                expect(typeof (data[i].description)).assertEqual("string");
                expect(data[i].description).assertEqual("form_description");
                expect(data[i].type).assertEqual(1);
                expect(data[i].colorMode).assertEqual(-1);
                expect(data[i].jsComponentName).assertEqual("card");
                expect(data[i].isDefault).assertEqual(1);
                expect(data[i].updateEnabled).assertEqual(1);
                expect(data[i].updateDuration).assertEqual(1);
                expect(data[i].scheduledUpdateTime).assertEqual("10:30");
                expect(data[i].formVisibleNotify).assertEqual(1);
                expect(data[i].defaultDimension).assertEqual(1);
                expect(data[i].supportDimensions[0]).assertEqual(1);
                expect(data[i].supportDimensions[1]).assertEqual(2);
                expect(data[i].metaData.customizeData[0].name).assertEqual("originWidgetName");
                expect(data[i].metaData.customizeData[0].value).assertEqual("myTest");
            }
            
            expect(data[0].name).assertEqual("FormA_Js001");
            expect(data[1].name).assertEqual("FormA_Js002");
            console.log("FMS_getFormsInfo_1800_promise result end");
            done();
        });
        
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_1800_promise==================end');
        }, TIMEOUT)
    })

}) 
