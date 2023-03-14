/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import storage from '@ohos.data.storage'

const PATH = '/data/test_storage';
const KEY_TEST_INT_ELEMENT = 'key_test_int';
const KEY_TEST_LONG_ELEMENT = 'key_test_long';
const KEY_TEST_FLOAT_ELEMENT = 'key_test_float';
const KEY_TEST_BOOLEAN_ELEMENT = 'key_test_boolean';
const KEY_TEST_STRING_ELEMENT = 'key_test_string';
var mPref;

describe('storageTest', function () {
    beforeAll(function () {
        console.info('beforeAll')
        mPref = storage.getStorageSync(PATH);
    })

    afterAll(function () {
        console.info('afterAll')
        storage.deleteStorageSync(PATH);
    })

    /**
     * @tc.name clear promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Promise_0010
     * @tc.desc clear promise interface test
     */
    it('testClear0011', 0, async function (done) {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        mPref.flushSync();
        const promise = mPref.clear();
        promise.then((ret) => {
            expect("defaultvalue").assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name has string interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0020
     * @tc.desc has string interface test
     */
    it('testHasKey0031', 0, async function (done) {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        const promise = mPref.has(KEY_TEST_STRING_ELEMENT);
        promise.then((ret) => {
            expect(true).assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name has int interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0140
     * @tc.desc has int interface test
     */
    it('testHasKey0032', 0, async function (done) {
        mPref.putSync(KEY_TEST_INT_ELEMENT, 1);
        const promise = mPref.has(KEY_TEST_INT_ELEMENT);
        promise.then((ret) => {
            expect(true).assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name has float interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0150
     * @tc.desc has float interface test
     */
    it('testHasKey0033', 0, async function (done) {
        mPref.putSync(KEY_TEST_FLOAT_ELEMENT, 2.0);
        const promise = mPref.has(KEY_TEST_FLOAT_ELEMENT);
        promise.then((ret) => {
            expect(true).assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name has boolean interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0160
     * @tc.desc has boolean interface test
     */
    it('testHasKey0034', 0, async function (done) {
        mPref.putSync(KEY_TEST_BOOLEAN_ELEMENT, false);
        const promise = mPref.has(KEY_TEST_BOOLEAN_ELEMENT);
        promise.then((ret) => {
            expect(true).assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name has long interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0170
     * @tc.desc has long interface test
     */
    it('testHasKey0035', 0, async function (done) {
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 0);
        const promise = mPref.has(KEY_TEST_LONG_ELEMENT);
        promise.then((ret) => {
            expect(true).assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name get string promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0030
     * @tc.desc get string promise interface test
     */
    it('testGetDefValue0061', 0, async function (done) {
        mPref.clearSync();
        const promise = mPref.get(KEY_TEST_STRING_ELEMENT, "defaultValue");
        promise.then((ret) => {
            expect('defaultValue').assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name get float promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0040
     * @tc.desc get float promise interface test
     */
    it('testGetFloat0071', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_FLOAT_ELEMENT, 3.0);
        const promise = mPref.get(KEY_TEST_FLOAT_ELEMENT, 0.0);
        promise.then((ret) => {
            expect(3.0).assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name get int promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0050
     * @tc.desc get int promise interface test
     */
    it('testGetInt0081', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_INT_ELEMENT, 3);
        const promise = mPref.get(KEY_TEST_INT_ELEMENT, 0.0);
        promise.then((ret) => {
            expect(3).assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name get long promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0060
     * @tc.desc get long promise interface test
     */
    it('testGetLong0091', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 3);
        const promise = mPref.get(KEY_TEST_LONG_ELEMENT, 0);
        promise.then((ret) => {
            expect(3).assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name get String promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0070
     * @tc.desc get String promise interface test
     */
    it('testGetString101', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        mPref.flushSync();
        const promise = mPref.get(KEY_TEST_STRING_ELEMENT, "defaultvalue");
        promise.then((ret) => {
            expect('test').assertEqual(ret);
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name put boolean promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0090
     * @tc.desc put boolean promise interface test
     */
    it('testPutBoolean0121', 0, async function (done) {
        mPref.clearSync();
        const promise = mPref.put(KEY_TEST_BOOLEAN_ELEMENT, true);
        promise.then((ret) => {
            expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, false));
            mPref.flushSync();
            expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, false));
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name put float promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0100
     * @tc.desc put float promise interface test
     */
    it('testPutFloat0131', 0, async function (done) {
        mPref.clearSync();
        const promise = mPref.put(KEY_TEST_FLOAT_ELEMENT, 4.0);
        promise.then((ret) => {
            expect(4.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0));
            mPref.flushSync();
            expect(4.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0));
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name put int promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0110
     * @tc.desc put int promise interface test
     */
    it('testPutInt0141', 0, async function (done) {
        mPref.clearSync();
        const promise = mPref.put(KEY_TEST_INT_ELEMENT, 4);
        promise.then((ret) => {
            expect(4).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
            mPref.flushSync();
            expect(4).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name put long promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0120
     * @tc.desc put long promise interface test
     */
    it('testPutLong0151', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 4);
        const promise = mPref.put(KEY_TEST_LONG_ELEMENT, 4);
        promise.then((ret) => {
            expect(4).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
            mPref.flushSync();
            expect(4).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })

    /**
     * @tc.name put String promise interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_Storage_0130
     * @tc.desc put String promise interface test
     */
    it('testPutString0161', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "abc");
        const promise = mPref.put(KEY_TEST_STRING_ELEMENT, '');
        promise.then((ret) => {
            expect('').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
            mPref.flushSync();
            expect('').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
        }).catch((err) => {
            expect(null).assertFail();
        });
        await promise;
        done();
    })
})