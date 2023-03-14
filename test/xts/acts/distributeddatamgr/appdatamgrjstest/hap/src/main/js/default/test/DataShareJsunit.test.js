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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import dataShare from '@ohos.data.dataShare';
import dataAbility from '@ohos.data.dataAbility';

const TAG = "[DATASHARE_JSKITS_TEST]"
const URI = ("datashare:///com.samples.datasharetest.DataShare");
let dseWant = {
    "bundleName": "com.examples.datasharetest.service",
    "abilityName": "DataShareExtAbility",
}

describe('dataShareTest', function () {
    beforeAll(async function () {
        console.info(TAG + 'beforeAll')
    })

    beforeEach(function () {
        console.info(TAG + 'beforeEach')
    })

    afterEach(function () {
        console.info(TAG + 'afterEach')
    })

    afterAll(async function () {
        console.info(TAG + 'afterAll')
    })

    console.log(TAG + "*************Unit Test Begin*************");

    function onChangeNotify() {
        console.info("==========================>onChangeNotify=======================>");
    }

    /**
     * @tc.name data share createDataShareHelper test
     * @tc.number SUB_DDM_DataShare_JS_createDataShareHelper_0000
     * @tc.desc data share createDataShareHelper test
     */
    it('createDataShareHelper_0000', 0, async function (done) {
        console.log(TAG + ":createDataShareHelper_0000 start");
        try {
            let helper = dataShare.createDataShareHelper(this.context, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":createDataShareHelper_0000 end");
    })

    /**
     * @tc.name data share createDataShareHelper test
     * @tc.number SUB_DDM_DataShare_JS_createDataShareHelper_0010
     * @tc.desc data share createDataShareHelper test
     */
    it('createDataShareHelper_0010', 0, async function (done) {
        console.log(TAG + ":createDataShareHelper_0010 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":createDataShareHelper_0010 end");
    })

    /**
     * @tc.name data share createDataShareHelper test
     * @tc.number SUB_DDM_DataShare_JS_createDataShareHelper_0011
     * @tc.desc data share createDataShareHelper test
     */
    it('createDataShareHelper_0011', 0, async function (done) {
        console.log(TAG + ":createDataShareHelper_0011 start");
        try {
            let helper = dataShare.createDataShareHelper(this.context, null, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":createDataShareHelper_0011 end");
    })

    /**
     * @tc.name data share createDataShareHelper test
     * @tc.number SUB_DDM_DataShare_JS_createDataShareHelper_0012
     * @tc.desc data share createDataShareHelper test
     */
    it('createDataShareHelper_0012', 0, async function (done) {
        console.log(TAG + ":createDataShareHelper_0012 start");
        try {
            let helper = dataShare.createDataShareHelper(this.context, dseWant, null);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":createDataShareHelper_0012 end");
    })

    /**
     * @tc.name data share onEvent test
     * @tc.number SUB_DDM_DataShare_JS_on_0020
     * @tc.desc data share onEvent test
     */
    it('on_0020', 0, async function (done) {
        console.log(TAG + ":on_0020 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.on(
                "dataChange",
                URI,
                onChangeNotify
            )
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":on_0020 end");
    })

    /**
     * @tc.name data share onEvent test
     * @tc.number SUB_DDM_DataShare_JS_on_0021
     * @tc.desc data share onEvent test
     */
    it('on_0021', 0, async function (done) {
        console.log(TAG + ":on_0021 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.on(
                "dataChange",
                null,
                onChangeNotify
            )
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":on_0021 end");
    })

    /**
     * @tc.name data share offEvent test
     * @tc.number SUB_DDM_DataShare_JS_off_0030
     * @tc.desc data share offEvent test
     */
    it('off_0030', 0, async function (done) {
        console.log(TAG + ":off_0030 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.off(
                "dataChange",
                URI,
            )
            helper.off(
                "dataChange",
                URI,
                onChangeNotify
            )
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":off_0030 end");
    })

    /**
     * @tc.name data share offEvent test
     * @tc.number SUB_DDM_DataShare_JS_off_0031
     * @tc.desc data share offEvent test
     */
    it('off_0031', 0, async function (done) {
        console.log(TAG + ":off_0031 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.off(
                "dataChange",
                null,
                onChangeNotify
            )
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":off_0031 end");
    })

    /**
     * @tc.name data share getType_callback test
     * @tc.number SUB_DDM_DataShare_getType_0040
     * @tc.desc data share getType_callback test
     */
    it('getType_callback_0040', 0, async function (done) {
        console.log(TAG + ":getType_callback_0040 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.getType(
                URI,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":getType_callback_0040 end");
    })

    /**
     * @tc.name data share getType_promise test
     * @tc.number SUB_DDM_DataShare_JS_getType_promise_0041
     * @tc.desc data share getType_promise test
     */
    it('getType_promise_0041', 0, async function (done) {
        console.log(TAG + ":getType_promise_0041 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.getType(
                URI
                ).then((data) => {
                    console.info("==========================>getTypeCallback=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":getType_promise_0041 end");
    })

    /**
     * @tc.name data share getFileType_callback test
     * @tc.number SUB_DDM_DataShare_JS_getFileType_callback_0050
     * @tc.desc data share getFileType_callback test
     */
    it('getFileType_callback_0050', 0, async function (done) {
        console.log(TAG + ":getFileType_callback_0050 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.getFileTypes(
                URI,
                "image/*",
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":getFileType_callback_0050 end");
    })

    /**
     * @tc.name data share getFileType_promise test
     * @tc.number SUB_DDM_DataShare_JS_getFileType_promise_0051
     * @tc.desc data share getFileType_promise test
     */
    it('getFileType_promise_0051', 0, async function (done) {
        console.log(TAG + ":getFileType_promise_0051 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.getFileTypes(
                URI,
                "image/*"
                ).then((data) => {
                    console.info("==========================>getFileTypesCallback=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":getFileType_promise_0051 end");
    })

    /**
     * @tc.name data share getFileType_callback test
     * @tc.number SUB_DDM_DataShare_JS_getFileType_callback_0052
     * @tc.desc data share getFileType_callback test
     */
    it('getFileType_callback_0052', 0, async function (done) {
        console.log(TAG + ":getFileType_callback_0052 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.getFileTypes(
                null,
                "image/*",
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":getFileType_callback_0052 end");
    })

    /**
     * @tc.name data share normalizeUri_callback test
     * @tc.number SUB_DDM_DataShare_JS_normalizeUri_0060
     * @tc.desc data share normalizeUri_callback test
     */
    it('normalizeUri_callback_0060', 0, async function (done) {
        console.log(TAG + ":normalizeUri_callback_0060 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.normalizeUri(
                URI,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":normalizeUri_callback_0060 end");
    })

    /**
     * @tc.name data share normalizeUri_promise test
     * @tc.number SUB_DDM_DataShare_JS_normalizeUri_promise_0061
     * @tc.desc data share normalizeUri_promise test
     */
    it('normalizeUri_promise_0061', 0, async function (done) {
        console.log(TAG + ":normalizeUri_promise_0061 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.normalizeUri(
                URI,
                ).then((data) => {
                    console.info("==========================>normalizeUriCallback=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":normalizeUri_promise_0061 end");
    })

    /**
     * @tc.name data share normalizeUri_callback test
     * @tc.number SUB_DDM_DataShare_JS_normalizeUri_0062
     * @tc.desc data share normalizeUri_callback test
     */
    it('normalizeUri_callback_0062', 0, async function (done) {
        console.log(TAG + ":normalizeUri_callback_0062 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.normalizeUri(
                null,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":normalizeUri_callback_0062 end");
    })

    /**
     * @tc.name data share denormalizeUri_callback test
     * @tc.number SUB_DDM_DataShare_JS_denormalizeUri_callback_0060
     * @tc.desc data share denormalizeUri_callback test
     */
    it('denormalizeUri_callback_0070', 0, async function (done) {
        console.log(TAG + ":denormalizeUri_callback_0070 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.denormalizeUri(
                URI,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":denormalizeUri_callback_0070 end");
    })

    /**
     * @tc.name data share denormalizeUri_promise test
     * @tc.number SUB_DDM_DataShare_JS_denormalizeUri_promise_0061
     * @tc.desc data share denormalizeUri_promise test
     */
    it('denormalizeUri_promise_0071', 0, async function (done) {
        console.log(TAG + ":denormalizeUri_promise_0071 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.denormalizeUri(
                URI,
                ).then((data) => {
                    console.info("==========================>denormalizeUriCallback=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":denormalizeUri_promise_0071 end");
    })

    /**
     * @tc.name data share denormalizeUri_callback test
     * @tc.number SUB_DDM_DataShare_JS_denormalizeUri_callback_0072
     * @tc.desc data share denormalizeUri_callback test
     */
    it('denormalizeUri_callback_0072', 0, async function (done) {
        console.log(TAG + ":denormalizeUri_callback_0072 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.denormalizeUri(
                null,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":denormalizeUri_callback_0072 end");
    })

    /**
     * @tc.name data share notifyChange_callback test
     * @tc.number SUB_DDM_DataShare_JS_notifyChange_callback_0080
     * @tc.desc data share notifyChange_callback test
     */
    it('notifyChange_callback_0080', 0, async function (done) {
        console.log(TAG + ":notifyChange_callback_0080 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.notifyChange(
                URI,
                (err) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":notifyChange_callback_0080 end");
    })

    /**
     * @tc.name data share notifyChange_promise test
     * @tc.number SUB_DDM_DataShare_JS_notifyChange_promise_0081
     * @tc.desc data share notifyChange_promise test
     */
    it('notifyChange_promise_0081', 0, async function (done) {
        console.log(TAG + ":notifyChange_promise_0081 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.notifyChange(
                URI,
                ).then(() => {
                    console.info("==========================>notifyChangeCallback=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":notifyChange_promise_0081 end");
    })

    /**
     * @tc.name data share notifyChange_callback test
     * @tc.number SUB_DDM_DataShare_JS_notifyChange_callback_0082
     * @tc.desc data share notifyChange_callback test
     */
    it('notifyChange_callback_0082', 0, async function (done) {
        console.log(TAG + ":notifyChange_callback_0082 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            helper.notifyChange(
                null,
                (err) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":notifyChange_callback_0082 end");
    })

    /**
     * @tc.name data share insert_callback test
     * @tc.number SUB_DDM_DataShare_JS_insert_callback_0090
     * @tc.desc data share insert_callback test
     */
    it('insert_callback_0090', 0, async function (done) {
        console.log(TAG + ":insert_callback_0090 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            const valueBucket = {
                "name": "rose",
                "age": 22,
                "salary": 200.5,
                "blobType": u8,
            }
            helper.insert(
                URI,
                valueBucket,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":insert_callback_0090 end");
    })

    /**
     * @tc.name data share insert_promise test
     * @tc.number SUB_DDM_DataShare_JS_insert_promise_0091
     * @tc.desc data share insert_promise test
     */
    it('insert_promise_0091', 0, async function (done) {
        console.log(TAG + ":insert_promise_0081 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            const valueBucket = {
                "name": "rose1",
                "age": 221,
                "salary": 20.5,
                "blobType": u8,
            }
            helper.insert(
                URI,
                valueBucket
                ).then((data) => {
                    console.info("==========================>insertCallback=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":insert_promise_0091 end");
    })

    /**
     * @tc.name data share insert_callback test
     * @tc.number SUB_DDM_DataShare_JS_insert_callback_0092
     * @tc.desc data share insert_callback test
     */
    it('insert_callback_0092', 0, async function (done) {
        console.log(TAG + ":insert_callback_0092 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            const valueBucket = {
                "name": "rose",
                "age": 22,
                "salary": 200.5,
                "blobType": u8,
            }
            helper.insert(
                null,
                valueBucket,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":insert_callback_0092 end");
    })

    /**
     * @tc.name data share batchInsert_callback test
     * @tc.number SUB_DDM_DataShare_JS_batchInsert_callback_0090
     * @tc.desc data share batchInsert_callback test
     */
    it('batchInsert_callback_0100', 0, async function (done) {
        console.log(TAG + ":batchInsert_callback_0100 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            var cars = new Array({"name": "roe11", "age": 21, "salary": 20.5, "blobType": u8,},
                                 {"name": "roe12", "age": 21, "salary": 20.5, "blobType": u8,},
                                 {"name": "roe13", "age": 21, "salary": 20.5, "blobType": u8,})
            helper.batchInsert(
                URI,
                cars,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":batchInsert_callback_0100 end");
    })

    /**
     * @tc.name data share batchInsert_promise test
     * @tc.number SUB_DDM_DataShare_JS_batchInsert_promise_0101
     * @tc.desc data share createDataShareHelper test
     */
    it('batchInsert_promise_0101', 0, async function (done) {
        console.log(TAG + ":batchInsert_promise_0101 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            var cars = new Array({"name": "roe11", "age": 21, "salary": 20.5, "blobType": u8,},
                                 {"name": "roe12", "age": 21, "salary": 20.5, "blobType": u8,},
                                 {"name": "roe13", "age": 21, "salary": 20.5, "blobType": u8,})
            helper.batchInsert(
                URI,
                cars
                ).then((data) => {
                    console.info("==========================>batchInsertCallback=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":batchInsert_promise_0101 end");
    })

    /**
     * @tc.name data share batchInsert_callback test
     * @tc.number SUB_DDM_DataShare_JS_batchInsert_callback_0102
     * @tc.desc data share batchInsert_callback test
     */
    it('batchInsert_callback_0102', 0, async function (done) {
        console.log(TAG + ":batchInsert_callback_0102 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            var cars = new Array({"name": "roe11", "age": 21, "salary": 20.5, "blobType": u8,},
                                 {"name": "roe12", "age": 21, "salary": 20.5, "blobType": u8,},
                                 {"name": "roe13", "age": 21, "salary": 20.5, "blobType": u8,})
            helper.batchInsert(
                null,
                cars,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":batchInsert_callback_0102 end");
    })

    /**
     * @tc.name data share delete_callback test
     * @tc.number SUB_DDM_DataShare_JS_delete_callback_0110
     * @tc.desc data share delete_callback test
     */
    it('delete_callback_0110', 0, async function (done) {
        console.log(TAG + ":delete_callback_0110 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            let da = new dataAbility.DataAbilityPredicates()
            helper.delete(
                URI,
                da,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":delete_callback_0110 end");
    })

    /**
     * @tc.name data share delete_promise test
     * @tc.number SUB_DDM_DataShare_JS_delete_promise_0111
     * @tc.desc data share delete_promise test
     */
    it('delete_promise_0111', 0, async function (done) {
        console.log(TAG + ":delete_promise_0111 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            let da = new dataAbility.DataAbilityPredicates()
            helper.delete(
                URI,
                da
                ).then((data) => {
                    console.info("==========================>deleteCallback=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":delete_promise_0111 end");
    })

    /**
     * @tc.name data share delete_callback test
     * @tc.number SUB_DDM_DataShare_JS_delete_callback_0112
     * @tc.desc data share delete_callback test
     */
    it('delete_callback_0112', 0, async function (done) {
        console.log(TAG + ":delete_callback_0112 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            let da = new dataAbility.DataAbilityPredicates()
            helper.delete(
                null,
                da,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":delete_callback_0112 end");
    })

    /**
     * @tc.name data share delete_callback test
     * @tc.number SUB_DDM_DataShare_JS_delete_callback_0113
     * @tc.desc data share delete_callback test
     */
    it('delete_callback_0113', 0, async function (done) {
        console.log(TAG + ":delete_callback_0113 start");
        try {
            let helper = dataShare.createDataShareHelper(this.context, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            let da = new dataAbility.DataAbilityPredicates()
            helper.delete(
                URI,
                da,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":delete_callback_0113 end");
    })

    /**
     * @tc.name data share update_callback test
     * @tc.number SUB_DDM_DataShare_JS_update_callback_0120
     * @tc.desc data share update_callback test
     */
    it('update_callback_0120', 0, async function (done) {
        console.log(TAG + ":update_callback_0120 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            const va = {
                "name": "roe1",
                "age": 21,
                "salary": 20.5,
                "blobType": u8,
            }
            let da = new dataAbility.DataAbilityPredicates()
            helper.update(
                URI,
                va,
                da,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":update_callback_0120 end");
    })

    /**
     * @tc.name data share update_promise test
     * @tc.number SUB_DDM_DataShare_JS_update_promise_0121
     * @tc.desc data share update_promise test
     */
    it('update_promise_0121', 0, async function (done) {
        console.log(TAG + ":update_promise_0121 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            const va = {
                "name": "roe1",
                "age": 21,
                "salary": 20.5,
                "blobType": u8,
            }
            let da = new dataAbility.DataAbilityPredicates()
            helper.update(
                URI,
                va,
                da
                ).then((data) => {
                    console.info("==========================>updateCallback=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":update_promise_0121 end");
    })

    /**
     * @tc.name data share update_callback test
     * @tc.number SUB_DDM_DataShare_JS_update_callback_0122
     * @tc.desc data share update_callback test
     */
    it('update_callback_0122', 0, async function (done) {
        console.log(TAG + ":update_callback_0122 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            const va = {
                "name": "roe1",
                "age": 21,
                "salary": 20.5,
                "blobType": u8,
            }
            let da = new dataAbility.DataAbilityPredicates()
            helper.update(
                null,
                va,
                da,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(false).assertTrue();
        } catch (err) {
            expect(true).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":update_callback_0122 end");
    })

    /**
     * @tc.name data share update_callback test
     * @tc.number SUB_DDM_DataShare_JS_update_callback_0123
     * @tc.desc data share update_callback test
     */
    it('update_callback_0123', 0, async function (done) {
        console.log(TAG + ":update_callback_0123 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            let da = new dataAbility.DataAbilityPredicates()
            helper.update(
                URI,
                null,
                da,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":update_callback_0123 end");
    })

    /**
     * @tc.name data share query_callback test
     * @tc.number SUB_DDM_DataShare_JS_query_callback_0130
     * @tc.desc data share query_callback test
     */
    it('query_callback_0130', 0, async function (done) {
        console.log(TAG + ":query_callback_0130 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            let da = new dataAbility.DataAbilityPredicates()
            helper.query(
                URI,
                ["*"],
                da,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":query_callback_0130 end");
    })

    /**
     * @tc.name data share query_promise test
     * @tc.number SUB_DDM_DataShare_JS_query_promise_0131
     * @tc.desc data share query_promise test
     */
    it('query_promise_0131', 0, async function (done) {
        console.log(TAG + ":query_promise_0131 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            let da = new dataAbility.DataAbilityPredicates()
            helper.query(
                URI,
                ["*"],
                da
                ).then((data) => {
                    console.info("==========================>queryCallback=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":query_promise_0131 end");
    })

    /**
     * @tc.name data share query_callback test
     * @tc.number SUB_DDM_DataShare_JS_query_callback_0132
     * @tc.desc data share query_callback test
     */
    it('query_callback_0132', 0, async function (done) {
        console.log(TAG + ":query_callback_0132 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            let da = new dataAbility.DataAbilityPredicates()
            helper.query(
                null,
                ["*"],
                da,
                (err, data) => {
                    console.info("==========================>Called=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":query_callback_0130 end");
    })

    /**
     * @tc.name data share query_promise test
     * @tc.number SUB_DDM_DataShare_JS_query_promise_0133
     * @tc.desc data share query_promise test
     */
    it('query_promise_0133', 0, async function (done) {
        console.log(TAG + ":query_promise_0133 start");
        try {
            let helper = dataShare.createDataShareHelper(null, dseWant, URI);
            console.log(TAG + ":DataShareHelper helper:" + helper);
            let da = new dataAbility.DataAbilityPredicates()
            helper.query(
                URI,
                null,
                da
                ).then((data) => {
                    console.info("==========================>queryCallback=======================>");
            });
            expect(true).assertTrue();
        } catch (err) {
            expect(false).assertTrue();
            console.log(TAG + ":DataShareHelper catch err:" + err);
        }
        done();
        console.log(TAG + ":query_promise_0133 end");
    })

    console.log(TAG + "*************Unit Test End*************");
})