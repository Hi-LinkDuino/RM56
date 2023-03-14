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
import dataUriUtils from '@ohos.ability.dataUriUtils'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('ActsDataUriUtilsTest', function () {

/*
* @tc.number: ACTS_GetIdSync_0100
* @tc.name: GetIdSync : Obtains the ID attached to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_GetIdSync_0100', 0, function (done) {
        console.log('dataUriUtils getIdSync start1');
        var result = dataUriUtils.getIdSync("com.ix.dataUriUtils/1221");
        console.log('dataUriUtils getIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("number");
        expect(result).assertEqual(1221);
        console.log('dataUriUtils getIdSync_test start1 successful');
        done();
    })

/*
* @tc.number: ACTS_GetIdSync_0200
* @tc.name: GetIdSync : Obtains the ID attached to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_GetIdSync_0200', 0, function (done) {
        console.log('dataUriUtils getIdSync start2 ');
        var result = dataUriUtils.getIdSync("com.ix.dataUriUtils1221");
        console.log('dataUriUtils getIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("number");
        expect(result).assertEqual(-1);
        console.log('dataUriUtils getIdSync_test start2 successful ');

        done();
    })

/*
* @tc.number: ACTS_GetIdSync_0300
* @tc.name: GetIdSync : Obtains the ID attached to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_GetIdSync_0300', 0, function (done) {
        console.log('dataUriUtils getIdSync start3 ');
        var result = dataUriUtils.getIdSync("com.ix.dataUriUtils/11234567890");
        console.log('dataUriUtils getIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("number");
        expect(result).assertEqual(11234567890);
        console.log('dataUriUtils getIdSync_test start3 successful ');

        done();
    })

/*
* @tc.number: ACTS_GetIdSync_0400
* @tc.name: GetIdSync : Obtains the ID attached to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_GetIdSync_0400', 0, function (done) {
        console.log('dataUriUtils getIdSync start4 ');
        var result = dataUriUtils.getIdSync("com.ix.dataUriUtils");
        console.log('dataUriUtils getIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("number");
        expect(result).assertEqual(-1);
        console.log('dataUriUtils getIdSync_test start4 successful ');

        done();
    })

/*
* @tc.number: ACTS_GetIdSync_0500
* @tc.name: GetIdSync : Obtains the ID attached to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_GetIdSync_0500', 0, function (done) {
        console.log('dataUriUtils getIdSync start5 ');
        var result = dataUriUtils.getIdSync("com.ix.dataUriUtils/-777");
        console.log('dataUriUtils getIdSync URI : ' + result);//-777

        expect(typeof(result)).assertEqual("number");
        expect(result).assertEqual(-777);
        console.log('dataUriUtils getIdSync_test start5 successful ');

        done();
    })

/*
* @tc.number: ACTS_AttachIdSync_0100
* @tc.name: Attaches the given ID to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_AttachIdSync_0100',0, function (done) {
        console.log('dataUriUtils attachIdSync start1 ');
        var idint = 1122;
        var result = dataUriUtils.attachIdSync("com.ix.dataUriUtils",idint);
        console.log('dataUriUtils attachIdSync URI :' + result)

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/1122");
        console.log('dataUriUtils attachIdSync_test start1 successful ');

        done();
    })

/*
* @tc.number: ACTS_AttachIdSync_0200
* @tc.name: Attaches the given ID to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_AttachIdSync_0200',0, function (done) {
        console.log('dataUriUtils attachIdSync start2 ');
        var idint = 0;
        var result = dataUriUtils.attachIdSync("com.ix.dataUriUtils/777",idint);
        console.log('dataUriUtils attachIdSync URI :' + result)

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/777/0");
        console.log('dataUriUtils attachIdSync_test start2 successful ');

        done();
    })

/*
* @tc.number: ACTS_AttachIdSync_0300
* @tc.name: Attaches the given ID to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_AttachIdSync_0300',0, function (done) {
        console.log('dataUriUtils attachIdSync start3 ');
        var idint = 456789;
        var result = dataUriUtils.attachIdSync("com.ix.dataUriUtils/123",idint);
        console.log('dataUriUtils attachIdSync URI :' + result)

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/123/456789");
        console.log('dataUriUtils attachIdSync_test start3 successful ');

        done();
    })

/*
* @tc.number: ACTS_AttachIdSync_0400
* @tc.name: Attaches the given ID to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_AttachIdSync_0400',0, function (done) {
        console.log('dataUriUtils attachIdSync start4 ');
        var idint = 123456789011;
        var result = dataUriUtils.attachIdSync("com.ix.dataUriUtils",idint);
        console.log('dataUriUtils attachIdSync URI :' + result)

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/123456789011");
        console.log('dataUriUtils attachIdSync_test start4 successful ');

        done();
    })

/*
* @tc.number: ACTS_AttachIdSync_0500
* @tc.name: Attaches the given ID to the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_AttachIdSync_0500',0, function (done) {
        console.log('dataUriUtils attachIdSync start5 ');
        var idint = -1;
        var result = dataUriUtils.attachIdSync("com.ix.dataUriUtils",idint);
        console.log('dataUriUtils attachIdSync URI :' + result)

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/-1");
        console.log('dataUriUtils attachIdSync_test start5 successful ');

        done();
    })

/*
* @tc.number: ACTS_DeleteIdSync_0100
* @tc.name: Deletes the ID from the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_DeleteIdSync_0100',0, function (done) {
        console.log('dataUriUtils deleteIdSync start1 ');
        var result = dataUriUtils.deleteIdSync("com.ix.dataUriUtils");
        console.log('dataUriUtils deleteIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils");
        console.log('dataUriUtils deleteIdSync_test start1 successful ');

        done();
    })

/*
* @tc.number: ACTS_DeleteIdSync_0200
* @tc.name: Deletes the ID from the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_DeleteIdSync_0200',0, function (done) {
        console.log('dataUriUtils deleteIdSync start2 ');
        var result = dataUriUtils.deleteIdSync("com.ix.dataUriUtils/1122");
        console.log('dataUriUtils deleteIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils");
        console.log('dataUriUtils deleteIdSync_test start2 successful ');

        done();
    })

/*
* @tc.number: ACTS_DeleteIdSync_0300
* @tc.name: Deletes the ID from the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_DeleteIdSync_0300',0, function (done) {
        console.log('dataUriUtils deleteIdSync start3 ');
        var result = dataUriUtils.deleteIdSync("com.ix.dataUriUtils1122");
        console.log('dataUriUtils deleteIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils1122");
        console.log('dataUriUtils deleteIdSync_test start3 successful ');

        done();
    })

/*
* @tc.number: ACTS_DeleteIdSync_0400
* @tc.name: Deletes the ID from the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_DeleteIdSync_0400',0, function (done) {
        console.log('dataUriUtils deleteIdSync start4 ');
        var result = dataUriUtils.deleteIdSync("com.ix.dataUriUtils/-1");
        console.log('dataUriUtils deleteIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils");
        console.log('dataUriUtils deleteIdSync_test start4 successful ');

        done();
    })

/*
* @tc.number: ACTS_DeleteIdSync_0500
* @tc.name: Deletes the ID from the end of the path component of the given URI.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_DeleteIdSync_0500',0, function (done) {
        console.log('dataUriUtils deleteIdSync start5 ');
        var result = dataUriUtils.deleteIdSync("com.ix.dataUriUtils/1122/2211");
        console.log('dataUriUtils deleteIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/1122");
        console.log('dataUriUtils deleteIdSync_test start5 successful ');

        done();
    })

/*
* @tc.number: ACTS_UpdateIdSync_0100
* @tc.name: Updates the ID in the specified dataUri.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_UpdateIdSync_0100',0, function (done) {
        console.log('dataUriUtils updateIdSync start1 ');
        var idint = 1122;
        var result = dataUriUtils.updateIdSync("com.ix.dataUriUtils",idint);
        console.log('updateIdSync updateIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils");
        console.log('dataUriUtils updateIdSync_test start1 successful ');

        done();
    })

/*
* @tc.number: ACTS_UpdateIdSync_0200
* @tc.name: Updates the ID in the specified dataUri.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_UpdateIdSync_0200',0, function (done) {
        console.log('dataUriUtils updateIdSync start2 ');
        var idint = 1122;
        var result = dataUriUtils.updateIdSync("com.ix.dataUriUtils/",idint)
        console.log('updateIdSync updateIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/");
        console.log('dataUriUtils updateIdSync_test start2 successful ');

        done();
    })

/*
* @tc.number: ACTS_UpdateIdSync_0300
* @tc.name: Updates the ID in the specified dataUri.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_UpdateIdSync_0300',0, function (done) {
        console.log('dataUriUtils updateIdSync start3 ');
        var idint = 3344;
        var result = dataUriUtils.updateIdSync("com.ix.dataUriUtils/-1",idint)
        console.log('updateIdSync updateIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/3344");
        console.log('dataUriUtils updateIdSync_test start3 successful ');

        done();
    })

/*
* @tc.number: ACTS_UpdateIdSync_0400
* @tc.name: Updates the ID in the specified dataUri.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_UpdateIdSync_0400',0, function (done) {
        console.log('dataUriUtils updateIdSync start4 ');
        var idint = 5566;
        var result = dataUriUtils.updateIdSync("com.ix.dataUriUtils/123456789011",idint)
        console.log('updateIdSync updateIdSync URI : ' + result);

        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/5566");
        console.log('dataUriUtils updateIdSync_test start4 successful ');

        done();
    })

/*
* @tc.number: ACTS_UpdateIdSync_0500
* @tc.name: Updates the ID in the specified dataUri.
* @tc.desc: Check the return value of the interface
*/
    it('ACTS_UpdateIdSync_0500',0, function (done) {
        console.log('dataUriUtils updateIdSync start5 ');

        var idint = 0;
        expect(typeof(idint)).assertEqual("number");
        console.log('dataUriUtils updateIdSync start666 ');

        var result = dataUriUtils.updateIdSync("com.ix.dataUriUtils/1122",idint)
        console.log('updateIdSync updateIdSync URI : ' + result);
        expect(typeof(result)).assertEqual("string");
        expect(result).assertEqual("com.ix.dataUriUtils/0");
        done();
    })
})
