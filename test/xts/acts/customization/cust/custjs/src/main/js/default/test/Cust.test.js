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

import configPolicy from '@ohos.configPolicy'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('CustTest', function () {
    console.log('*************start CustTest*************');

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETLIST_JS_001
    * @tc.name test getCfgDirList method in callback mode
    * @tc.desc get getCfgDirList in callback mode
    */
    it('getCfgDirList_test_001', 0, async function (done) {
        configPolicy.getCfgDirList((error, value) => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getCfgDirList_test_001:' + value.length);
            console.log('CustTest getCfgDirList_test_001:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETLIST_JS_002
    * @tc.name test getCfgDirList method in promise mode
    * @tc.desc get getCfgDirList in promise mode
    */
    it('getCfgDirList_test_002', 0, async function (done) {
        configPolicy.getCfgDirList().then(value => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getCfgDirList_test_002:' + value.length);
            console.log('CustTest getCfgDirList_test_002:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETFILES_JS_001
    * @tc.name test getCfgFiles method in callback mode with not exist param
    * @tc.desc get getCfgFiles in callback mode
    */
    it('getCfgFiles_test_001', 0, async function (done) {
        configPolicy.getCfgFiles('custxmltest/none.xml', (error, value) => {
            expect(value.length == 0).assertTrue();
            console.log('CustTest getCfgFiles_test_001, none.xml:' + value.length);
            console.log('CustTest getCfgFiles_test_001, none.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETFILES_JS_002
    * @tc.name test getCfgFiles method in callback mode with low priority param
    * @tc.desc get getCfgFiles in callback mode
    */
    it('getCfgFiles_test_002', 0, async function (done) {
        configPolicy.getCfgFiles('custxmltest/system.xml', (error, value) => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getCfgFiles_test_002, system.xml:' + value.length);
            console.log('CustTest getCfgFiles_test_002, system.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETFILES_JS_003
    * @tc.name test getCfgFiles method in callback mode both exist param
    * @tc.desc get getCfgFiles in callback mode
    */
    it('getCfgFiles_test_003', 0, async function (done) {
        configPolicy.getCfgFiles('custxmltest/both.xml', (error, value) => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getCfgFiles_test_003, both.xml:' + value.length);
            console.log('CustTest getCfgFiles_test_003, both.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETFILES_JS_004
    * @tc.name test getCfgFiles method in callback mode with high priority param
    * @tc.desc get getCfgFiles in callback mode
    */
    it('getCfgFiles_test_004', 0, async function (done) {
        configPolicy.getCfgFiles('custxmltest/user.xml', (error, value) => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getCfgFiles_test_004, user.xml:' + value.length);
            console.log('CustTest getCfgFiles_test_004, user.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETFILES_JS_005
    * @tc.name test getCfgFiles method in promise mode with not exist param
    * @tc.desc get getCfgFiles in promise mode
    */
    it('getCfgFiles_test_005', 0, async function (done) {
        configPolicy.getCfgFiles('custxmltest/none.xml').then(value => {
            expect(value.length == 0).assertTrue();
            console.log('CustTest getCfgFiles_test_005, none.xml:' + value.length);
            console.log('CustTest getCfgFiles_test_005, none.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETFILES_JS_006
    * @tc.name test getCfgFiles method in promise mode with low priority param
    * @tc.desc get getCfgFiles in promise mode
    */
    it('getCfgFiles_test_006', 0, async function (done) {
        configPolicy.getCfgFiles('custxmltest/system.xml').then(value => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getCfgFiles_test_006, system.xml:' + value.length);
            console.log('CustTest getCfgFiles_test_006, system.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETFILES_JS_007
    * @tc.name test getCfgFiles method in promise mode with both exist param
    * @tc.desc get getCfgFiles in promise mode
    */
    it('getCfgFiles_test_007', 0, async function (done) {
        configPolicy.getCfgFiles('custxmltest/both.xml').then(value => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getCfgFiles_test_007, both.xml:' + value.length);
            console.log('CustTest getCfgFiles_test_007, both.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETFILES_JS_008
    * @tc.name test getCfgFiles method in promise mode with high priority param
    * @tc.desc get getCfgFiles in promise mode
    */
    it('getCfgFiles_test_008', 0, async function (done) {
        configPolicy.getCfgFiles('custxmltest/user.xml').then(value => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getCfgFiles_test_008, user.xml:' + value.length);
            console.log('CustTest getCfgFiles_test_008, user.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETONEFILE_JS_001
    * @tc.name test getOneCfgFile method in callback mode with not exist param
    * @tc.desc get getOneCfgFile in callback mode
    */
    it('getOneCfgFile_test_001', 0, async function (done) {
        configPolicy.getOneCfgFile('custxmltest/none.xml', (error, value) => {
            expect(value.length == 0).assertTrue();
            console.log('CustTest getOneCfgFile_test_001, none.xml:' + value.length);
            console.log('CustTest getOneCfgFile_test_001, none.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETONEFILE_JS_002
    * @tc.name test getOneCfgFile method in callback mode with low priority param
    * @tc.desc get getOneCfgFile in callback mode
    */
    it('getOneCfgFile_test_002', 0, async function (done) {
        configPolicy.getOneCfgFile('custxmltest/system.xml', (error, value) => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getOneCfgFile_test_002, system.xml:' + value.length);
            console.log('CustTest getOneCfgFile_test_002, system.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETONEFILE_JS_003
    * @tc.name test getOneCfgFile method in callback mode with both exist param
    * @tc.desc get getOneCfgFile in callback mode
    */
    it('getOneCfgFile_test_003', 0, async function (done) {
        configPolicy.getOneCfgFile('custxmltest/both.xml', (error, value) => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getOneCfgFile_test_003, both.xml:' + value.length);
            console.log('CustTest getOneCfgFile_test_003, both.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETONEFILE_JS_004
    * @tc.name test getOneCfgFile method in callback mode with high priority param
    * @tc.desc get getOneCfgFile in callback mode
    */
    it('getOneCfgFile_test_004', 0, async function (done) {
        configPolicy.getOneCfgFile('custxmltest/user.xml', (error, value) => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getOneCfgFile_test_004, user.xml:' + value.length);
            console.log('CustTest getOneCfgFile_test_004, user.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETONEFILE_JS_005
    * @tc.name test getOneCfgFile method in promise mode with not exist param
    * @tc.desc get getOneCfgFile in promise mode
    */
    it('getOneCfgFile_test_005', 0, async function (done) {
        configPolicy.getOneCfgFile('custxmltest/none.xml').then(value => {
            expect(value.length == 0).assertTrue();
            console.log('CustTest getOneCfgFile_test_005, none.xml:' + value.length);
            console.log('CustTest getOneCfgFile_test_005, none.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETONEFILE_JS_006
    * @tc.name test getOneCfgFile method in promise mode with low priority param
    * @tc.desc get getOneCfgFile in promise mode
    */
    it('getOneCfgFile_test_006', 0, async function (done) {

        configPolicy.getOneCfgFile('custxmltest/system.xml').then(value => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getOneCfgFile_test_006, system.xml:' + value.length);
            console.log('CustTest getOneCfgFile_test_006, system.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETONEFILE_JS_007
    * @tc.name test getOneCfgFile method in promise mode with both exist param
    * @tc.desc get getOneCfgFile in promise mode and return the high priority file
    */
    it('getOneCfgFile_test_007', 0, async function (done) {

        configPolicy.getOneCfgFile('custxmltest/both.xml').then(value => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getOneCfgFile_test_007, both.xml:' + value.length);
            console.log('CustTest getOneCfgFile_test_007, both.xml:' + value);
        });
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_CUST_GETONEFILE_JS_008
    * @tc.name test getOneCfgFile method in promise mode with high priority param
    * @tc.desc get getOneCfgFile in promise mode
    */
    it('getOneCfgFile_test_008', 0, async function (done) {
        configPolicy.getOneCfgFile('custxmltest/user.xml').then(value => {
            expect(value.length != 0).assertTrue();
            console.log('CustTest getOneCfgFile_test_008, user.xml:' + value.length);
            console.log('CustTest getOneCfgFile_test_008, user.xml:' + value);
        });
        done();
    })

    console.log('*************end CustTest*************');
})
