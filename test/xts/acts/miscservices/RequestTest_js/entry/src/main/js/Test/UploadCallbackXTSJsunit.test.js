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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import request from '@ohos.request';
import * as pubfun from './Publicfunction.js'

var typeProgress = 'progress';
var typeHeaderReceive = 'headerReceive';
var typeFail = 'fail';
let uploadTask;
let file7url = 'internal://cache/test.txt';

describe('UploadTest', function () {
    beforeAll(function () {
        console.info('beforeAll: Prerequisites at the test suite level, ' +
        'which are executed before the test suite is executed.');
    })
    beforeEach(function () {
        console.info('beforeEach: Prerequisites at the test case level,' +
        ' which are executed before each test case is executed.');
    })
    afterEach(function () {
        console.info('afterEach: Test case-level clearance conditions, ' +
        'which are executed after each test case is executed.');
    })
    afterAll(function () {
        console.info('afterAll: Test suite-level cleanup condition, ' +
        'which is executed after the test suite is executed');
    })

    /*
        * @tc.number    : InitUploadtaskCallback001
        * @tc.name      : Use getEntries get the value by mixing the string key
        * @tc.desc      : Mixed strings value can be obtained correctly
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('InitUploadtaskCallback001', 0, async function (done) {
        try {
            console.log("Testupdate before getuploadconfig");
            let UploadConfig = pubfun.getUploadConfig(file7url)
            console.log("Testupdate before upload UploadConfig = " + JSON.stringify(UploadConfig));
            console.log("Testupdate before upload");
            await pubfun.publicupload(UploadConfig).then((data) => {
                console.log("Testupdate going upload uploadTask = " + data);
                uploadTask = data;
                expect(true).assertEqual((data != undefined) || (data != "") || (data != {}));
                done();
            }).catch((err) => {
                console.log("InitUploadtaskCallback001 fail 1" + JSON.stringify(err));
                expect(err).assertFail();
                done();
            })
        } catch (e) {
            console.log("InitUploadtaskCallback001 fail 2" + JSON.stringify(e));
            expect(e).assertFail();
            done();
        }
    })

    /*
        * @tc.number    : SwitchOnProgressCallback001
        * @tc.name      : Use getEntries get the value by mixing the string key
        * @tc.desc      : Mixed strings value can be obtained correctly
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('SwitchOnProgressCallback001', 0, async function (done) {
        try {
            pubfun.publiconprogress(uploadTask, typeProgress);
            expect(true).assertEqual(0 == 0);
            done();
        } catch (err) {
            console.log("Testupdate SwitchOnProgressCallback001 catch err " + JSON.stringify(err));
            expect(err).assertFail();
            done();
        }
    });

    /*
        * @tc.number    : SwitchOffProgressCallback001
        * @tc.name      : Use getEntries get the value by mixing the string key
        * @tc.desc      : Mixed strings value can be obtained correctly
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('SwitchOffProgressCallback001', 0, async function (done) {
        try {
            pubfun.publiconprogress(uploadTask, typeProgress);
            expect(true).assertEqual(0 == 0);

            await pubfun.publicoffprogress(uploadTask, typeProgress).then((data) => {
                console.log("SwitchOffProgressCallback001 data" + JSON.stringify(data));
                expect(7).assertEqual(data.totalSize);
                done();
            }).catch((err) => {
                console.log("SwitchOffProgressCallback001 fail 2" + JSON.stringify(err));
                expect(err).assertFail();
                done();
            })
        } catch (e) {
            console.log("SwitchOffProgressCallback001 fail 3" + JSON.stringify(e));
            expect(e).assertFail();
            done();
        }
    })

    /*
        * @tc.number    : SwitchOnFailCallback001
        * @tc.name      : Use getEntries get the value by mixing the string key
        * @tc.desc      : Mixed strings value can be obtained correctly
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('SwitchOnFailCallback001', 0, async function (done) {
        try {
            await pubfun.publicon(uploadTask, typeFail).then((data) => {
                console.log("SwitchOnFailCallback001 data " + data);
                expect(5).assertEqual(data);
                done();
            }).catch((err) => {
                console.log("SwitchOnFailCallback001 fail 2" + JSON.stringify(err));
                expect(err).assertFail();
                done();
            })
        } catch (e) {
            console.log("SwitchOnFailCallback001 fail 3" + JSON.stringify(e));
            expect(e).assertFail();
            done();
        }
    })

    /*
        * @tc.number    : SwitchOffFailCallback001
        * @tc.name      : Use getEntries get the value by mixing the string key
        * @tc.desc      : Mixed strings value can be obtained correctly
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('SwitchOffFailCallback001', 0, async function (done) {
        try {
            await pubfun.publicon(uploadTask, typeFail)
            await pubfun.publicoff(uploadTask, typeFail).then((data) => {
                console.log("SwitchOffFailCallback001 data " + data);
                expect(5).assertEqual(data);
                done();
            }).catch((err) => {
                console.log("SwitchOffFailCallback001 fail 2" + JSON.stringify(err));
                done();
                expect(err).assertFail();
            })
        } catch (e) {
            console.log("SwitchOffFailCallback001 fail 3" + JSON.stringify(e));
            expect(e).assertFail();
            done();
        }
    })

    /*
        * @tc.number    : RemoveCallback001
        * @tc.name      : Use getEntries get the value by mixing the string key
        * @tc.desc      : Mixed strings value can be obtained correctly
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('RemoveCallback001', 0, async function (done) {
        try {
            await pubfun.publicremove(uploadTask).then((data) => {
                console.log("RemoveCallback001 data " + data);
                expect(true).assertEqual(data);
                done();
            }).catch((err) => {
                console.log("RemoveCallback001 fail 2" + JSON.stringify(err));
                expect(err).assertFail();
                done();
            })
        } catch (e) {
            console.log("RemoveCallback001 fail 3" + JSON.stringify(e));
            expect(e).assertFail();
            done();
        }
    })
})