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
     * @tc.name clear callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0010
     * @tc.desc clear callback interface test
     */
    it('testClear0012', 0, async function (done) {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        mPref.flushSync();
        await mPref.clear(function (err, ret) {
            expect("defaultvalue").assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
            done();
        });
    })

    /**
     * @tc.name has string callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0020
     * @tc.desc has string callback interface test
     */
    it('testHasKey0032', 0, async function (done) {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        await mPref.has(KEY_TEST_STRING_ELEMENT, function (err, ret) {
            expect(true).assertEqual(ret);
            done();
        })
    })

    /**
     * @tc.name has int callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0130
     * @tc.desc has int callback interface test
     */
    it('testHasKey0033', 0, async function (done) {
        mPref.putSync(KEY_TEST_INT_ELEMENT, 1);
        await mPref.has(KEY_TEST_INT_ELEMENT, function (err, ret) {
            expect(true).assertEqual(ret);
            done();
        })
    })

    /**
     * @tc.name has float callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0140
     * @tc.desc has float callback interface test
     */
    it('testHasKey0034', 0, async function (done) {
        mPref.putSync(KEY_TEST_FLOAT_ELEMENT, 1.1);
        await mPref.has(KEY_TEST_FLOAT_ELEMENT, function (err, ret) {
            expect(true).assertEqual(ret);
            done();
        })
    })

    /**
     * @tc.name has long callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0150
     * @tc.desc has long callback interface test
     */
    it('testHasKey0035', 0, async function (done) {
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 0);
        await mPref.has(KEY_TEST_LONG_ELEMENT, function (err, ret) {
            expect(true).assertEqual(ret);
            done();
        })
    })

    /**
     * @tc.name has boolean callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0160
     * @tc.desc has boolean callback interface test
     */
    it('testHasKey0036', 0, async function (done) {
        mPref.putSync(KEY_TEST_BOOLEAN_ELEMENT, false);
        await mPref.has(KEY_TEST_BOOLEAN_ELEMENT, function (err, ret) {
            expect(true).assertEqual(ret);
            done();
        })
    })

    /**
     * @tc.name get defaultValue callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0030
     * @tc.desc get defaultValue callback interface test
     */
    it('testGetDefValue0062', 0, async function (done) {
        mPref.clearSync();
        await mPref.get(KEY_TEST_STRING_ELEMENT, "defaultValue", function (err, ret) {
            expect('defaultValue').assertEqual(ret);
            done();
        })
    })

    /**
     * @tc.name get float callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0040
     * @tc.desc get float callback interface test
     */
    it('testGetFloat0072', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_FLOAT_ELEMENT, 3.0);
        await mPref.get(KEY_TEST_FLOAT_ELEMENT, 0.0, function (err, ret) {
            expect(3.0).assertEqual(ret);
            done();
        })
    })

    /**
     * @tc.name get int callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0050
     * @tc.desc get int callback interface test
     */
    it('testGetInt0082', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_INT_ELEMENT, 3);
        await mPref.get(KEY_TEST_INT_ELEMENT, 0.0, function (err, ret) {
            expect(3).assertEqual(ret);
            done();
        })
    })

    /**
     * @tc.name get long callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0060
     * @tc.desc get long callback interface test
     */
    it('testGetLong0092', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 3);
        expect(3).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
        await mPref.get(KEY_TEST_LONG_ELEMENT, 0, function (err, ret) {
            expect(3).assertEqual(ret);
            done();
        });
    })

    /**
     * @tc.name get String callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0070
     * @tc.desc get String callback interface test
     */
    it('testGetString102', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        mPref.flushSync();
        await mPref.get(KEY_TEST_STRING_ELEMENT, "defaultvalue", function (err, ret) {
            expect('test').assertEqual(ret);
            done();
        });
    })

    /**
     * @tc.name put boolean callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0080
     * @tc.desc put boolean callback interface test
     */
    it('testPutBoolean0122', 0, async function (done) {
        mPref.clearSync();
        await mPref.put(KEY_TEST_BOOLEAN_ELEMENT, true, function (err, ret) {
            expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, false));
            mPref.flushSync();
            expect(true).assertEqual(mPref.getSync(KEY_TEST_BOOLEAN_ELEMENT, false));
            done();
        });
    })

    /**
     * @tc.name put float callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0090
     * @tc.desc put float callback interface test
     */
    it('testPutFloat0132', 0, async function (done) {
        mPref.clearSync();
        await mPref.put(KEY_TEST_FLOAT_ELEMENT, 4.0, function (err, ret) {
            expect(4.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0));
            mPref.flushSync();
            expect(4.0).assertEqual(mPref.getSync(KEY_TEST_FLOAT_ELEMENT, 0.0));
            done();
        });
    })

    /**
     * @tc.name put int callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0100
     * @tc.desc put int callback interface test
     */
    it('testPutInt0142', 0, async function (done) {
        mPref.clearSync();
        await mPref.put(KEY_TEST_INT_ELEMENT, 4, function (err, ret) {
            expect(4).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
            mPref.flushSync();
            expect(4).assertEqual(mPref.getSync(KEY_TEST_INT_ELEMENT, 0));
            done();
        });
    })

    /**
     * @tc.name put long callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0110
     * @tc.desc put long callback interface test
     */
    it('testPutLong0152', 0, async function (done) {
        mPref.clearSync();
        mPref.putSync(KEY_TEST_LONG_ELEMENT, 4);
        await mPref.put(KEY_TEST_LONG_ELEMENT, 4, function (err, ret) {
            expect(4).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
            mPref.flushSync();
            expect(4).assertEqual(mPref.getSync(KEY_TEST_LONG_ELEMENT, 0));
            done();
        });
    })

    /**
     * @tc.name put String callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0120
     * @tc.desc put String callback interface test
     */
    it('testPutString0162', 0, async function (done) {
        mPref.clearSync();
        await mPref.put(KEY_TEST_STRING_ELEMENT, '', function (err, ret) {
            expect('').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
            mPref.flushSync();
            expect('').assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "defaultvalue"));
            done();
        });
    })

        /**
     * @tc.name flush callback interface test
     * @tc.number SUB_DDM_AppDataFWK_JSPreferences_CallBack_0010
     * @tc.desc flush callback interface test
     */
    it('testFluesh00172', 0, async function (done) {
        mPref.putSync(KEY_TEST_STRING_ELEMENT, "test");
        await mPref.flush(function (err, ret) {
            expect("test").assertEqual(mPref.getSync(KEY_TEST_STRING_ELEMENT, "default"));
            done();
        });
    })
})