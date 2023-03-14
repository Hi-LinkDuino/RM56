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

import formBindingData from "@ohos.application.formBindingData"
import formManager from '@ohos.ability.formManager'
import {describe, it, expect} from 'deccjsunit/index'

const TIMEOUT = 3000;
describe('ActsFormsZeroTest', function () {
    console.log("ActsFormsZeroTest===start");

    /**
     * @tc.name getAllFormsInfo test
     * @tc.number FMS_getFormsInfo_0300
     * @tc.desc All form configuration information is empty (by AsyncCallback)
     */
     it('FMS_getFormsInfo_0300_callback', 0, async function (done) {
        console.log("FMS_getFormsInfo_0300_callback begin");
        var ret = formManager.getAllFormsInfo(
            (result, data) => {
                var dataD = data.filter((p) => {
                    return p.bundleName == "com.form.formsystemtestserviced";
                });
                console.log("FMS_getFormsInfo_0300_callback, dataB json:" + JSON.stringify(data));
                console.log("FMS_getFormsInfo_0300_callback, dataB json:" + JSON.stringify(dataD));
                console.log("FMS_getFormsInfo_0300_callback, the all forms number:" + data.length);
                console.log("FMS_getFormsInfo_0300_callback, the formD number:" + dataD.length);
                console.log("FMS_getFormsInfo_0300_callback async::result, result:" + result);
                expect(dataD.length).assertEqual(0);
                console.log("FMS_getFormsInfo_0300_callback result end");
                done();
            }
        );
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_0300_callback==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_0300_callback end, ret:" + ret);
    })

    /**
     * @tc.name getAllFormsInfo test
     * @tc.number FMS_getFormsInfo_0300
     * @tc.desc All form configuration information is empty (by Promise)
     */
     it('FMS_getFormsInfo_0300_promise', 0, async function (done) {
        console.log("FMS_getFormsInfo_0300_promise begin");
        var ret = formManager.getAllFormsInfo(
            (result, data) => {
                console.log("FMS_getFormsInfo_0300_promise, data json:" + JSON.stringify(data));
                console.log("FMS_getFormsInfo_0300_promise, the all forms number:" + data.length);
                console.log("FMS_getFormsInfo_0300_promise async::result, result:" + result);
                console.log("FMS_getFormsInfo_0300_promise result end");
                if (data.length != 0){
                    var dataD = data.filter((p) => {
                        return p.bundleName == "com.form.formsystemtestserviced";
                    });
                    console.log("FMS_getFormsInfo_0300_promise, dataB json:" + JSON.stringify(dataD));
                    console.log("FMS_getFormsInfo_0300_promise, the formD number:" + dataD.length);
                    expect(dataD.length).assertEqual(0);
                    console.log("FMS_getFormsInfo_0300_promise result end");
                    done();
                } else {
                    var promise = formManager.getAllFormsInfo();
                    promise.then((result) => {
                        console.log("FMS_getFormsInfo_0300_promise result:" + result);
                        expect(result).assertEqual(0);
                        console.log("FMS_getFormsInfo_0300_promise result end");
                        done();
                    });
                }
            }
        );
        setTimeout(function () {
            console.info('=====================FMS_getFormsInfo_0300_promise==================end');
        }, TIMEOUT)
        console.log("FMS_getFormsInfo_0300_promise end, ret:" + ret);
    })

    /**
     * @tc.name CreateFormBindingData test
     * @tc.number FMS_CreateFormBindingData_0100
     * @tc.desc Creates data and returns a data object
     */
     it('FMS_CreateFormBindingData_0100', 0, async function (done) {
        console.log("FMS_CreateFormBindingData_0100 begin");
        let dataObj = {
            temperature:"22c",
            "time":"22:00",
            "test":22,
            "test3":true
        };
        let obj = formBindingData.createFormBindingData(JSON.stringify(dataObj));
        console.log('FMS_CreateFormBindingData_0100, obj.data:' + obj.data);
        expect(JSON.stringify(dataObj)).assertEqual(obj.data);
        done();
        console.log("FMS_CreateFormBindingData_0100 end");
    })

}) 
