/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import app from '@system.app'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import window from '@ohos.window'
import display from '@ohos.display'

describe('display_test', function () {

    beforeAll(function () {
    })
    beforeEach(function () {
    })
    afterEach(function () {
    })
    afterAll(function () {
    })

    /**
     * @tc.number		SUB_WMS_GETDEFALUTDISPLAY_JSAPI_001
     * @tc.name			Test getDefaultDisplay_Test_001
     * @tc.desc			To test the function of obtaining the default screen
     */
    it('getDefaultDisplay_Test_001', 0, async function (done) {
        console.log('displayTest getDefaultDisplayTest1 begin');
        display.getDefaultDisplay().then(dsp => {
            console.log('displayTest getDefaultDisplayTest1 getDefaultDisplay id :' + JSON.stringify(dsp));
            expect(dsp.id != null).assertTrue();
            expect(dsp.refreshRate != null).assertTrue();
            expect(dsp.width != null).assertTrue();
            expect(dsp.height != null).assertTrue();
            done();
        }, (err) => {
            console.log('displayTest getDefaultDisplayTest1 getDefaultDisplay failed, err :' + JSON.stringify(err));
            expect().assertFail();
            done();
        })
    })

    /**
     * @tc.number    SUB_WMS_GETDEFALUTDISPLAY_JSAPI_001
     * @tc.name      Test getDefaultDisplay_Test_002.
     * @tc.desc      To test the function if obtaining the default screen.
     */
    it('getDefaultDisplay_Test_002', 0, async function (done) {
        console.log('displayTest getDefaultDisplayTest2 begin');
        display.getDefaultDisplay((err, data) => {
            if (err.code != 0) {
                console.log('displayTest getDefaultDisplayTest2 getDefaultDisplay callback fail' + JSON.stringify(err.code));
                expect().assertFail();
                done();
            } else {
                console.log('displayTest getDefaultDisplayTest2 getDefaultDisplay id :' + JSON.stringify(data));
                expect(data.id != null).assertTrue();
                expect(data.refreshRate != null).assertTrue();
                expect(data.width != null).assertTrue();
                expect(data.height != null).assertTrue();
                done();
            }
        })
    })

    /**
     * @tc.number		SUB_WMS_GETALLDISPLAY_JSAPI_001
     * @tc.name			Test getAllDisplay_Test_001.
     * @tc.desc			To verify the function of obtaining all screens.
     */
    it('getAllDisplay_Test_001', 0, async function (done) {
        console.log('displayTest getAllDisplayTest1 begin');
        display.getAllDisplay().then(dsp => {
            console.log('displayTest getAllDisplayTest1 getAllDisplay id :' + JSON.stringify(dsp));
            expect(dsp[0].id != null).assertTrue();
            expect(dsp[0].refreshRate != null).assertTrue();
            expect(dsp[0].width != null).assertTrue();
            expect(dsp[0].height != null).assertTrue();
            done();
        }, (err) => {
            console.log('displayTest getAllDisplayTest1 getAllDisplay failed, err :' + JSON.stringify(err));
            expect().assertFail();
            done();
        })
    })

    /**
     * @tc.number    SUB_WMS_GETALLDISPLAY_JSAPI_002
     * @tc.name      Test getAllDisplay_Test_002
     * @tc.desc      To test the function if obtaining the default screen
     */
    it('getAllDisplay_Test_002', 0, async function (done) {
        console.log('displayTest getAllDisplayTest2 begin');
        display.getAllDisplay((err, data) => {
            if (err.code != 0) {
                console.log('displayTest getAllDisplayTest2 getAllDisplay callback fail' + JSON.stringify(err));
                expect().assertFail();
                done();
            } else {
                console.log('displayTest getAllDisplayTest2 getAllDisplay id :' + JSON.stringify(data));
                expect(data[0].refreshRate != null).assertTrue();
                expect(data[0].width != null).assertTrue();
                expect(data[0].height != null).assertTrue();
                done();
            }
        })
    })

})
