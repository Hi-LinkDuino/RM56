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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index"
import featureAbility from '@ohos.ability.featureAbility'
import ohosDataAbility from '@ohos.data.dataAbility'

describe('ActsDataAbilityJsTest', function () {
let dataAbilityUri = ("dataability:///com.example.myapplication.DataAbility");
let DAHelper;
let gSetTimeout = 500;
var valueBucket = {
    "name": "ActsDataAbilityHelperTest",
    "age": 24,
    "salary": 2024.20,
}

beforeAll(async (done) => {
    console.debug('= ACTS_AbeforeAll 1425 ====<begin');
    try {
        DAHelper = featureAbility.acquireDataAbilityHelper(dataAbilityUri);
        console.debug('ACTS_AbeforeAll DAHelper ====>: ' + DAHelper + " ,JSON. " + JSON.stringify(DAHelper));
        if(DAHelper == null){
            console.debug('ACTS_AbeforeAll DAHelper ====>: DAHelper == null');
        }
    } catch (err) {
        console.error('=ACTS_AbeforeAll acquireDataAbilityHelper catch(err)====>:' + err);
    }
    console.debug('= ACTS_AbeforeAll ====<end');
    done();
})
beforeEach(async (done) => {
    console.debug('= ACTS_AbeforeEach ====<begin');
    setTimeout(function () {
        console.debug('= ACTS_AbeforeEach ====<end');
        done();
    }, gSetTimeout);
})
afterEach(async (done) => {
    console.debug('= ACTS_AafterEach ====<begin');
    setTimeout(function () {
        console.debug('= ACTS_AafterEach ====<end');
        done();
    }, gSetTimeout);
})
afterAll((done) => {
    console.debug('= ACTS_AafterAll ====<begin');
    setTimeout(function () {
        console.debug('= ACTS_AafterAll ====<end');
        featureAbility.terminateSelf();
        done();
    }, gSetTimeout);
})
    
/*
* @tc.number: ACTS_AfeatureAbilityEnum_0100
* @tc.name: Enum the client resource of the Data ability.
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AfeatureAbilityEnum_0100', 0, async function (done) {
    console.log('ACTS_AfeatureAbilityEnum_0100====<begin');
    try {
        expect(featureAbility.ContinuationState.LOCAL_RUNNING).assertEqual(0);
        expect(featureAbility.ContinuationState.REMOTE_RUNNING).assertEqual(1);
        expect(featureAbility.ContinuationState.REPLICA_RUNNING).assertEqual(2);
        
        expect(abilityManager.ProcessErrCode.NO_ERROR).assertEqual(0);
        expect(abilityManager.ProcessErrCode.CRASHED).assertEqual(1);
        expect(abilityManager.ProcessErrCode.NO_RESPONSE).assertEqual(2);

        console.log('ACTS_AfeatureAbilityEnum_0100====<end');
        done();
    } catch (err) {
        console.log('ACTS_AfeatureAbilityEnum_0100====<end catch(err)=' + err);
        done();
    }
})

/*
* @tc.number: ACTS_AInsert_0800
* @tc.name: Insert : Indicates the path of the data to operate
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AInsert_0800', 0, async function (done) {
    console.log('ACTS_AInsert_0800====<begin');
    console.log('featureAbility getDataAbilityHelper ====>: ' + DAHelper)
    var currentAlertTimeout;
    try {
        function onAsyncCallbackInsert0800(err) {
            clearTimeout(currentAlertTimeout);
            expect(err.code).assertEqual(0);
            console.log('ACTS_AInsert_0800====<onAsyncCallbackInsert0800');
            DAHelper.off("dataChange", dataAbilityUri, onAsyncCallbackInsert0800);
            done();
        }
        DAHelper.on("dataChange", dataAbilityUri, onAsyncCallbackInsert0800);
        await DAHelper.insert(dataAbilityUri, valueBucket,
            (err, data) => {
                console.debug("=ACTS_AInsert_0800 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                expect(data).assertEqual(1);
                var mSetTimeout = 1000
                currentAlertTimeout = setTimeout(mySetTimeout, mSetTimeout);
                function mySetTimeout() {
                    console.debug('ACTS_AInsert_0800====< mySetTimeout');
                    done();
                }
            }
        );
    } catch (err) {
        console.error('=ACTS_AInsert_0800 catch(err)====>:' + err);
        console.log('ACTS_AInsert_0800====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_ABatchInsert_0600
* @tc.name: Inserts multiple data records into the database
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_ABatchInsert_0600', 0, async function (done) {
    console.log('ACTS_ABatchInsert_0600====<begin');
    console.log('featureAbility BatchInsert getDataAbilityHelper ====>: ' + DAHelper)
    var valueBucketArray = new Array({ "name": "ActsDataAbilityHelperTest", "age": 24, "salary": 2024.20, },
        { "name": "ActsDataAbilityHelperTest", "age": 24, "salary": 2024.20, },
        { "name": "ActsDataAbilityHelperTest", "age": 24, "salary": 2024.20, })
    try {
        await DAHelper.batchInsert(
            dataAbilityUri,
            valueBucketArray,
            (err, data) => {
                console.debug("=ACTS_ABatchInsert_0600 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                expect(data).assertEqual(3);
                console.log('ACTS_ABatchInsert_0600====<end');
                done();
            },
        );
    } catch (err) {
        console.error('=ACTS_ABatchInsert_0600 catch(err)====>:' + err);
        console.log('ACTS_ABatchInsert_0600====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_AQuery_0400
* @tc.name: Queries one or more data records in the database
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AQuery_0400', 0, async function (done) {
    console.log('ACTS_AQuery_0400====<begin');
    var columnsArray = new Array("value1", "value2", "value3", "value4")
    try {
        let predicates = new ohosDataAbility.DataAbilityPredicates();
        predicates.equalTo('contact_id', 1);
        predicates.limitAs(10);
        predicates.orderByAsc("order_by_class");
        predicates.offsetAs(1);
        await DAHelper.query(
            dataAbilityUri,
            columnsArray,
            predicates,
            (err, data) => {
                console.debug("=ACTS_AQuery_0400 query err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【")
                    + JSON.stringify(data) + (" 】;") + " , " + data);
                expect(typeof (data)).assertEqual("object");
                console.log('ACTS_AQuery_0400====<end');
                done();
            });
    } catch (err) {
        console.error('=ACTS_AQuery_0400 catch(err)====>:' + err);
        console.log('ACTS_AQuery_0400====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_AUpdate_0400
* @tc.name: Updates one or more data records in the database.
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AUpdate_0400', 0, async function (done) {
    console.log('ACTS_AUpdate_0400====<begin');
    console.log('featureAbility Update getDataAbilityHelper ====>: ' + DAHelper)
    try {
        let predicates = new ohosDataAbility.DataAbilityPredicates();
        predicates.equalTo('contact_id', 1);
        predicates.limitAs(10);
        predicates.orderByAsc("order_by_class");
        predicates.offsetAs(1);
        console.debug("=ACTS_AUpdate_0400 predicates====>"
            + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
        await DAHelper.update(
            dataAbilityUri,
            valueBucket,
            predicates,
            (err, data) => {
                console.debug("=ACTS_AUpdate_0400 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                expect(data).assertEqual(1);
                console.log('ACTS_AUpdate_0400====<end');
                done();
            },
        );
    } catch (err) {
        console.error('=ACTS_AUpdate_0400 catch(err)====>:' + err);
        console.log('ACTS_AUpdate_0400====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_ADelete_0400
* @tc.name: Deletes one or more data records. This method should be implemented by a Data ability.
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_ADelete_0400', 0, async function (done) {
    console.log('ACTS_ADelete_0400====<begin');
    console.log('featureAbility getDataAbilityHelper ====>: ' + DAHelper)
    try {
        let predicates = new ohosDataAbility.DataAbilityPredicates();
        predicates.equalTo('contact_id', 1);
        predicates.limitAs(10);
        predicates.orderByAsc("order_by_class");
        predicates.offsetAs(1);
        console.debug("=ACTS_ADelete_0400 predicates====>"
            + ("json predicates 【") + JSON.stringify(predicates) + (" 】") + " , " + predicates);
        await DAHelper.delete(
            dataAbilityUri,
            predicates,
            (err, data) => {
                console.debug("=ACTS_ADelete_0400 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                expect(data).assertEqual(0);
                console.log('ACTS_ADelete_0400====<end');
                done();
            });
    } catch (err) {
        console.error('=ACTS_ADelete_0400 catch(err)====>:' + err);
        console.log('ACTS_ADelete_0400====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_AGetFileTypes_0200
* @tc.name: Obtains the MIME type of files.
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AGetFileTypes_0200', 0, async function (done) {
    console.log('ACTS_AGetFileTypes_0200====<begin');
    console.log('featureAbility getFileTypes getDataAbilityHelper ====>: ' + DAHelper)
    let mimeTypeFilter = '*/*'
    try {
        await DAHelper.getFileTypes(
            dataAbilityUri,
            mimeTypeFilter,
            (err, data) => {
                console.debug("=ACTS_AGetFileTypes_0200 getFileTypes err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                console.log('=ACTS_AGetFileTypes_0200 data.length ====>: ' + data.length);
                for (var i = 0; i < data.length; i++) {
                    expect(typeof (data[i])).assertEqual("string");
                    console.log('=ACTS_AGetFileTypes_0200 for data ====>: ' + err.code +
                        " data[" + i + "]: " + data[i]);
                    if (i == 0) {
                         expect(data[i]).assertEqual("hap");
                    } else if (i == 1) {
                         expect(data[i]).assertEqual("jpg");
                    } else if (i == 2) {
                         expect(data[i]).assertEqual("image/png");
                    }
                }
                console.log('ACTS_AGetFileTypes_0200====<end');
                done();
            },
        );
    } catch (err) {
        console.error('=ACTS_AGetFileTypes_0200 getFileTypes AsyncCallback catch(err)====>:' + err);
        console.log('ACTS_AGetFileTypes_0200====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_AGetType_0200
* @tc.name: Obtains the MIME type matching the data specified by the URI of the Data ability.
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AGetType_0200', 0, async function (done) {
    console.log('ACTS_AGetType_0200====<begin');
    console.log('featureAbility GetType getDataAbilityHelper ====>: ' + DAHelper)
    try {
        await DAHelper.getType(
            dataAbilityUri,
            (err, data) => {
                console.debug("=ACTS_AGetType_0200 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                expect(data).assertEqual("hap");
                console.log('ACTS_AGetType_0200====<end');
                done();
            },
        );
    } catch (err) {
        console.error('=ACTS_AGetType_0200 getType AsyncCallback catch(err)====>:' + err);
        console.log('ACTS_AGetType_0200====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_AOpenFile_0700
* @tc.name: Opens a file. This method should be implemented by a Data ability.
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AOpenFile_0700', 0, async function (done) {
    console.log('ACTS_AOpenFile_0700====<begin');
    let ret = false;
    console.log('featureAbility ACTS_AOpenFile_0700 OpenFile getDataAbilityHelper ====>: ' + DAHelper)
    var mode = "r";
    try {
        DAHelper.openFile(
            dataAbilityUri,
            mode,
            (err, data) => {
                console.debug("=ACTS_AOpenFile_0700 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                expect(typeof (data)).assertEqual("number");
                console.log('DataAbilityHelper ACTS_AOpenFile_0700 OpenFile asyncCallback errCode ====>: '
                    + err.code + " data: " + data);
                ret = true
                done();
            },
        );
    } catch (err) {
        console.error('=ACTS_AOpenFile_0700 getType AsyncCallback catch(err)====>:' + err);
        ret = false
        done();
    }
    setTimeout(function () {
        console.log('setTimeout function====<');
    }, gSetTimeout);
    console.log('ACTS_AOpenFile_0700====<end');
})

/*
* @tc.number: ACTS_ANormalizeUri_0200
* @tc.name: Converts the given uri that refer to the Data ability into a normalized URI.
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_ANormalizeUri_0200', 0, async function (done) {
    console.log('ACTS_ANormalizeUri_0200====<begin');
    let ret = false;
    console.log('featureAbility normalizeUri getDataAbilityHelper ====>: ' + DAHelper)
    try {
        DAHelper.normalizeUri(
            dataAbilityUri,
            (err, data) => {
                console.debug("=ACTS_ANormalizeUri_0200 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                expect(data).assertEqual(dataAbilityUri);
                console.log('ACTS_ANormalizeUri_0200====<end');
                done();
            },
        );
    } catch (err) {
        console.error('=ACTS_ANormalizeUri_0200 normalizeUri AsyncCallback catch(err)====>:' + err);
        console.log('ACTS_ANormalizeUri_0200====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_ADenormalizeUri_0200
* @tc.name: Converts the given normalized uri generated by normalizeUri into a denormalized one.
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_ADenormalizeUri_0200', 0, async function (done) {
    console.log('ACTS_ADenormalizeUri_0200====<begin');
    let ret = false;
    console.log('featureAbility DenormalizeUri getDataAbilityHelper ====>: ' + DAHelper)
    try {
        DAHelper.denormalizeUri(
            dataAbilityUri,
            (err, data) => {
                console.debug("=ACTS_ADenormalizeUri_0200 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data) + (" 】;"));
                expect(data).assertEqual(dataAbilityUri);
                console.log('ACTS_ADenormalizeUri_0200====<end');
                done();
            },
        );
    } catch (err) {
        console.error('=ACTS_ADenormalizeUri_0200 denormalizeUri AsyncCallback catch(err)====>:' + err);
        console.log('ACTS_ADenormalizeUri_0200====<end catch');
        done();
    }
})


/*
* @tc.number: ACTS_AOnOff_0100
* @tc.name: On/Off : Registers an observer to observe data specified by the given Uri
* @tc.desc: Check the return value of the interface ()
*/
it('ACTS_AOnOff_0100', 0, async function (done) {
    console.log('ACTS_AOnOff_0100====<begin');
    gSetTimeout = 2000;
    try {
        var currentAlertTimeout;
        function onAsyncCallback0100(err) {
            expect(err.code).assertEqual(0);
            DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback0100);
            console.log('ACTS_AOnOff_0100====<end');
            gSetTimeout = 500;
            done();
        }
        DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0100);

        setTimeout(mySetTimeout, gSetTimeout);
        function mySetTimeout() {
            console.debug('ACTS_AOnOff_0100====< mySetTimeout');
            DAHelper.notifyChange(
                dataAbilityUri,
                (err) => {
                    console.debug("=ACTS_AOnOff_0100 err=======>"
                        + ("err【") + JSON.stringify(err) + (" 】") + " , " + err);
                    expect(err.code).assertEqual(0);
                }
            );
        }
    } catch (err) {
        console.error('=ACTS_AOnOff_0100  catch(err)====>:' + err);
        console.log('ACTS_AOnOff_0100====<end catch');
        gSetTimeout = 500;
        done();
    }
})

/*
* @tc.number: ACTS_AOnOff_0200
* @tc.name: On/Off : Registers an observer to observe data specified by the given Uri
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AOnOff_0200', 0, async function (done) {
    console.log('ACTS_AOnOff_0200====<begin');
    gSetTimeout = 2000;
    try {
        var currentAlertTimeout;
        function onAsyncCallback0200(err) {
            console.debug("=ACTS_AOnOff_0200 onAsyncCallback0200=======>");
            expect(false).assertTrue();
            clearTimeout(currentAlertTimeout);
            console.log('ACTS_AOnOff_0200====<end callback_0200');
            done();
        }
        DAHelper.on("dataChange", dataAbilityUri, onAsyncCallback0200);

        DAHelper.off("dataChange", dataAbilityUri, onAsyncCallback0200);

        setTimeout(mySetTimeout, gSetTimeout);
        function mySetTimeout() {
            DAHelper.notifyChange(
                dataAbilityUri,
                (err) => {
                    if (err.code != 0) {
                        console.debug("=ACTS_AOnOff_0200 err=======>"
                            + ("err【") + JSON.stringify(err) + (" 】") + " , " + err);
                        expect(false).assertTrue();
                        console.log('ACTS_AOnOff_0200====<end err.code');
                        done();
                    } else {
                        currentAlertTimeout = setTimeout(() => {
                            expect(err.code).assertEqual(0);
                            console.log('ACTS_AOnOff_0200====<end');
                            done();
                        }, gSetTimeout);
                    }
                }
            );
        }
    } catch (err) {
        console.error('=ACTS_AOnOff_0200  catch(err)====>:' + err);
        console.log('ACTS_AOnOff_0200====<end catch');
        done();
    }
})


/*
* @tc.number: ACTS_AExecuteBatch_Insert_0400
* @tc.name: ExecuteBatch : Preforms batch operations on the database
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AExecuteBatch_Insert_0400', 0, async function (done) {
    console.debug('ACTS_AExecuteBatch_Insert_0400====<begin');
    try {
        DAHelper.executeBatch(dataAbilityUri,
            [
                {
                    uri: dataAbilityUri,
                    type: featureAbility.DataAbilityOperationType.TYPE_INSERT,
                    valuesBucket: valueBucket,
                    predicates: null,
                    expectedCount: 1,
                    PredicatesBackReferences: {},
                    interrupted: true,
                    valueBackReferences: valueBucket,
                }
            ],
            (err, data) => {
                console.debug("=ACTS_AExecuteBatch_Insert_0400 executeBatch err,data====>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                    + (" 】;") + data.length);
                expect(err.code).assertEqual(0);
                expect(data.length).assertEqual(0);
                console.debug('ACTS_AExecuteBatch_Insert_0400====<end');
                done();
            }
        );
    } catch (err) {
        console.error('=ACTS_AExecuteBatch_Insert_0400 catch(err)====>:'
            + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
        console.debug('ACTS_AExecuteBatch_Insert_0400====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_AExecuteBatch_Update_0400
* @tc.name: ExecuteBatch : Preforms batch operations on the database
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AExecuteBatch_Update_0400', 0, async function (done) {
    console.debug('ACTS_AExecuteBatch_Update_0400====<begin');
    try {
        let predicates = new ohosDataAbility.DataAbilityPredicates();
        predicates.equalTo('contact_id', 1);
        predicates.limitAs(10);
        predicates.orderByAsc("order_by_class");
        predicates.offsetAs(1);

        DAHelper.executeBatch(dataAbilityUri,
            [
                {
                    uri: dataAbilityUri,
                    type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                    valuesBucket: valueBucket,
                    predicates: predicates,
                    expectedCount: 1,
                    PredicatesBackReferences: {},
                    interrupted: true,
                    valueBackReferences: valueBucket,
                }
            ],
            (err, DataAbilityResult) => {
                console.debug("=ACTS_AExecuteBatch_Update_0400 executeBatch err,data====>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【")
                    + JSON.stringify(DataAbilityResult) + (" 】;") + DataAbilityResult.length);
                for (var i = 0; i < DataAbilityResult.length; i++) {
                    console.debug('=ACTS_AExecuteBatch_Update_0400 for data[' + i + '].uri ====>: '
                        + DataAbilityResult[i].uri)
                    expect(DataAbilityResult[i].uri).assertEqual(dataAbilityUri);
                    console.debug('=ACTS_AExecuteBatch_Update_0400 for data[' + i + '].count ====>: '
                        + DataAbilityResult[i].count)
                    expect(DataAbilityResult[i].count).assertEqual(1);
                }
                console.debug('ACTS_AExecuteBatch_Update_0400====<end');
                done();
            }
        );
    } catch (err) {
        console.error('=ACTS_AExecuteBatch_Update_0400 catch(err)====>:'
            + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
        console.debug('ACTS_AExecuteBatch_Update_0400====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_AExecuteBatch_Assert_0400
* @tc.name: ExecuteBatch : Preforms batch operations on the database
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AExecuteBatch_Assert_0400', 0, async function (done) {
    console.debug('ACTS_AExecuteBatch_Assert_0400====<begin');
    try {
        let predicates = new ohosDataAbility.DataAbilityPredicates();
        predicates.equalTo('contact_id', 1);
        predicates.limitAs(10);
        predicates.orderByAsc("order_by_class");
        predicates.offsetAs(1);

        DAHelper.executeBatch(dataAbilityUri,
            [
                {
                    uri: dataAbilityUri,
                    type: featureAbility.DataAbilityOperationType.TYPE_ASSERT,
                    valuesBucket: valueBucket,
                    predicates: predicates,
                    expectedCount: 1,
                    PredicatesBackReferences: {},
                    interrupted: true,
                    valueBackReferences: valueBucket,
                }
            ],
            (err, data) => {
                console.debug("=ACTS_AExecuteBatch_Assert_0400 executeBatch err,data====>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(data)
                    + (" 】;") + data.length);
                expect(data.length).assertEqual(0);
                console.debug('ACTS_AExecuteBatch_Assert_0400====<end');
                done();
            }
        );
    } catch (err) {
        console.error('=ACTS_AExecuteBatch_Assert_0400 catch(err)====>:'
            + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
        console.debug('ACTS_AExecuteBatch_Assert_0400====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_AExecuteBatch_Delete_0400
* @tc.name: ExecuteBatch : Preforms batch operations on the database
* @tc.desc: Check the return value of the interface (by AsyncCallback)
*/
it('ACTS_AExecuteBatch_Delete_0400', 0, async function (done) {
    console.debug('ACTS_AExecuteBatch_Delete_0400====<begin');
    try {
        let predicates = new ohosDataAbility.DataAbilityPredicates();
        predicates.equalTo('contact_id', 1);
        predicates.limitAs(10);
        predicates.orderByAsc("order_by_class");
        predicates.offsetAs(1);

        DAHelper.executeBatch(dataAbilityUri,
            [
                {
                    uri: dataAbilityUri,
                    type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                    valuesBucket: valueBucket,
                    predicates: predicates,
                    expectedCount: 1,
                    PredicatesBackReferences: {},
                    interrupted: true,
                    valueBackReferences: valueBucket,
                }
            ],
            (err, DataAbilityResult) => {
                console.debug("=ACTS_AExecuteBatch_Delete_0400 executeBatch err,data====>"
                    + ("json err【") + JSON.stringify(err) + (" 】json data【") + JSON.stringify(DataAbilityResult)
                    + (" 】;") + DataAbilityResult.length);
                for (var i = 0; i < DataAbilityResult.length; i++) {
                    console.debug('=ACTS_AExecuteBatch_Delete_0400 for data[' + i + '].uri ====>: '
                        + DataAbilityResult[i].uri)
                    expect(DataAbilityResult[i].uri).assertEqual(dataAbilityUri);
                    console.debug('=ACTS_AExecuteBatch_Delete_0400 for data[' + i + '].count ====>: '
                        + DataAbilityResult[i].count)
                    expect(DataAbilityResult[i].count).assertEqual(1);
                }
                console.debug('ACTS_AExecuteBatch_Delete_0400====<end');
                done();
            }
        );
    } catch (err) {
        console.error('=ACTS_AExecuteBatch_Delete_0400 catch(err)====>:'
            + ("json err 【") + JSON.stringify(err) + (" 】 ,") + err);
        console.debug('ACTS_AExecuteBatch_Delete_0400====<end catch');
        done();
    }
})

/*
* @tc.number: ACTS_ARelease_0100
* @tc.name: Releases the client resource of the Data ability.
* @tc.desc: Check the return value of the interface (by promise)
*/
it('ACTS_ARelease_0100', 0, async function (done) {
    console.log('ACTS_ARelease_0100====<begin');
    var rDAHelper
    try {
        rDAHelper = featureAbility.acquireDataAbilityHelper(dataAbilityUri);
        console.debug('ACTS_AbeforeAll rDAHelper ====>: ' + rDAHelper + " ,JSON. " + JSON.stringify(rDAHelper));
        expect(typeof (rDAHelper)).assertEqual("object");
        rDAHelper.release().then((data) => {
            console.debug("=ACTS_ARelease_0100 then data====>"
                + ("json data 【") + JSON.stringify(data) + (" 】") + " , " + data);
            expect(data).assertEqual(true);
            console.log('ACTS_ARelease_0100====<end');
            done();
        }).catch(err => {
            console.debug("=ACTS_ARelease_0100 catch err ====>"
                + ("json err 【") + JSON.stringify(err) + (" 】 "));
            expect(false).assertTrue();
            console.log('ACTS_ARelease_0100====<end .catch');
            done();
        });
    } catch (err) {
        console.error('=ACTS_ARelease_0100 release promise catch(err)====>:' + err);
        console.log('ACTS_ARelease_0100====<end catch(err)');
        done();
    }
})

/*
 * @tc.number: ACTS_Call_0100
 * @tc.name: call : Inserts a single data record into the database.
 * @tc.desc: Check the return value of the interface (by promise)
 * @tc.level   0
 */
it("ACTS_Call_0100", 0, async function (done) {
    console.log('ACTS_Call_0100  start......');
    var pacMap = {
        'groupName': 'test1',
        'ringtone_modify_time': 28
    };
    var rDAHelper
    try {
        rDAHelper = featureAbility.acquireDataAbilityHelper(dataAbilityUri);
        console.debug('ACTS_AbeforeAll rDAHelper ====>: ' + rDAHelper + " ,JSON. " + JSON.stringify(rDAHelper));
        expect(typeof (rDAHelper)).assertEqual("object");
        pacMap.groupName = 'test2';
        rDAHelper.call(dataAbilityUri, 'insert', '', pacMap).then((data) => {
            console.info('ACTS_Call_0100 succeeded   data: ' + JSON.stringify(data));
        }).catch((error) => {
            console.error('ACTS_Call_0100 error: ' + JSON.stringify(error));
        });
        done();
    } catch (err) {
        console.error('ACTS_Call_0100 catch(err)====>:' + err);
        console.log('ACTS_Call_0100====<end catch(err)');
        done();
    }
    console.log('ACTS_Call_0100 end......');
})


/*
 * @tc.number: ACTS_Call_0200
 * @tc.name: call : Queries data in the database.
 * @tc.desc: Check the return value of the interface
 * @tc.level  0
 */
it("ACTS_Call_0200", 0, async function (done) {
    console.log('ACTS_Call_0200 start......');
    var arg = "{\"equalTo\": \"groupName\"}";
    var pacMap = {
        "groupName": "test1",
        "columns": "groupName,ringtone_modify_time"
    };
    var rDAHelper
    try {
        rDAHelper = featureAbility.acquireDataAbilityHelper(dataAbilityUri);
        console.debug('ACTS_AbeforeAll rDAHelper ====>: ' + rDAHelper + " ,JSON. " + JSON.stringify(rDAHelper));
        rDAHelper.call(dataAbilityUri, 'query', arg, pacMap, (err, data) => {
            if (err) {
                console.error('ACTS_Call_0200 error: ' + JSON.stringify(err));
            } else {
                console.info('ACTS_Call_0200 succeeded: ' + JSON.stringify(data));
            }
        });
    } catch (err) {
        console.error('ACTS_Call_0200 catch(err)====>:' + err);
        console.log('ACTS_Call_0200====<end catch(err)');
        done();
    }
    console.log('ACTS_Call_0200 end......');
})

/*
 * @tc.number: ACTS_Call_0300
 * @tc.name: call : Updates data records in the database.
 * @tc.desc: Check the return value of the interface
 * @tc.level   0
 */
it("ACTS_Call_0300", 0, async function (done) {
    console.log('ACTS_Call_0300 start......');
    var arg = "{\"equalTo\": \"ringtone_modify_time\"}";
    var pacMap = {
        'ringtone_modify_time': 28,
        'groupName': 'testupdata1'
    };
    var rDAHelper
    try {
        rDAHelper = featureAbility.acquireDataAbilityHelper(dataAbilityUri);
        console.debug('ACTS_AbeforeAll rDAHelper ====>: ' + rDAHelper + " ,JSON. " + JSON.stringify(rDAHelper));
        rDAHelper.call(dataAbilityUri, 'update', arg, pacMap, (err, data) => {
            if (err) {
                console.error('ACTS_Call_0300 error: ' + JSON.stringify(err));
            } else {
                console.info('ACTS_Call_0300 succeeded: ' + JSON.stringify(data));
            }
        });
    } catch (err) {
        console.error('ACTS_Call_0300 catch(err)====>:' + err);
        console.log('ACTS_Call_0300====<end catch(err)');
        done();
    }
    console.log('ACTS_Call_0300 end......');
})

/*
 * @tc.number: ACTS_Call_0400
 * @tc.name: call : Deletes one or more data records from the database.
 * @tc.desc: Check the return value of the interface
 * @tc.level   0
 */
it("ACTS_Call_0400", 0, async function (done) {
    console.log('ACTS_Call_0400 start......');
    var arg = "{\"lessThan\": \"ringtone_modify_time\"}";
    var pacMap = {
        'ringtone_modify_time': 32,
    };
    var rDAHelper
    try {
        rDAHelper = featureAbility.acquireDataAbilityHelper(dataAbilityUri);
        console.debug('ACTS_AbeforeAll rDAHelper ====>: ' + rDAHelper + " ,JSON. " + JSON.stringify(rDAHelper));
        rDAHelper.call(dataAbilityUri, 'delete', arg, pacMap, (err, data) => {
            if (err) {
                console.error('ACTS_Call_0400 error: ' + JSON.stringify(err));
            } else {
                console.info('ACTS_Call_0400 succeeded: ' + JSON.stringify(data));
            }
        });
        done();
    } catch (err) {
        console.error('ACTS_Call_0400 catch(err)====>:' + err);
        console.log('ACTS_Call_0400====<end catch(err)');
        done();
    }
    console.log('ACTS_Call_0400 end......');
})

})