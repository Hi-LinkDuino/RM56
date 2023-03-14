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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import dataRdb from '@ohos.data.rdb';

const TAG = "[RDB_JSKITS_TEST]"
const CREATE_TABLE_TEST = "CREATE TABLE IF NOT EXISTS test (" + "id INTEGER PRIMARY KEY AUTOINCREMENT, " + "name TEXT NOT NULL, " + "age INTEGER, " + "salary REAL, " + "blobType BLOB)";

const STORE_CONFIG = {
    name: "Delete.db",
}
var rdbStore = undefined;

describe('rdbStoreDeleteTest', function () {
    beforeAll(function () {
        console.info(TAG + 'beforeAll')
    })

    beforeEach(async function () {
        console.info(TAG + 'beforeEach')
        rdbStore = await dataRdb.getRdbStore(STORE_CONFIG, 1);
        await rdbStore.executeSql(CREATE_TABLE_TEST, null);
    })

    afterEach(async function () {
        console.info(TAG + 'afterEach')
        await rdbStore.executeSql("DELETE FROM test");
        rdbStore = null
        await dataRdb.deleteRdbStore("Delete.db");
    })

    afterAll(async function () {
        console.info(TAG + 'afterAll')
    })

    console.log(TAG + "*************Unit Test Begin*************");
    /**
     * @tc.name rdb delete test
     * @tc.number SUB_DDM_AppDataFWK_JSRDB_Delete_0010
     * @tc.desc rdb delete test
     */
    it('testRdbStoreDelete0001', 0, async function (done) {
        console.log(TAG + "************* testRdbStoreDelete0001 start *************");
        var u8 = new Uint8Array([1, 2, 3])
        {
            const valueBucket = {
                "name": "zhangsan",
                "age": 18,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 28,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 38,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        //删除
        {
            let predicates = await new dataRdb.RdbPredicates("test")
            let deletePromise = rdbStore.delete(predicates)
            deletePromise.then(async (ret) => {
                expect(3).assertEqual(ret)
                console.log(TAG + "Delete done: " + ret)
            }).catch((err) => {
                expect(null).assertFail()
            })
            await deletePromise
        }
        done()
        console.log(TAG + "************* testRdbStoreDelete0001 end *************");
    })

    /**
     * @tc.name rdb delete test
     * @tc.number SUB_DDM_AppDataFWK_JSRDB_Delete_0020
     * @tc.desc rdb delete test
     */
    it('testRdbStoreDelete0002', 0, async function (done) {
        console.log(TAG + "************* testRdbStoreDelete0002 start *************");
        var u8 = new Uint8Array([1, 2, 3])
        {
            const valueBucket = {
                "name": "zhangsan",
                "age": 18,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 28,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 38,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        //删除
        {
            let predicates = await new dataRdb.RdbPredicates("test")
            predicates.equalTo("name", "zhangsan")
            let deletePromise = rdbStore.delete(predicates)
            deletePromise.then(async (ret) => {
                await expect(1).assertEqual(ret)
                await console.log(TAG + "Delete done: " + ret)
            }).catch((err) => {
                expect(null).assertFail()
            })
            await deletePromise
        }
        done()
        console.log(TAG + "************* testRdbStoreDelete0002 end *************");
    })

    /**
     * @tc.name rdb delete test
     * @tc.number SUB_DDM_AppDataFWK_JSRDB_Delete_0030
     * @tc.desc rdb delete test
     */
    it('testRdbStoreDelete0003', 0, async function (done) {
        console.log(TAG + "************* testRdbStoreDelete0003 start *************");
        var u8 = new Uint8Array([1, 2, 3])
        {
            const valueBucket = {
                "name": "zhangsan",
                "age": 18,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 28,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        //删除前查询
        {
            let predicates = await new dataRdb.RdbPredicates("test")
            predicates.equalTo("age", 28)
            let resultSet = await rdbStore.query(predicates)
            expect(1).assertEqual(resultSet.rowCount)
        }
        //删除
        {
            let predicates = await new dataRdb.RdbPredicates("test")
            predicates.equalTo("age", 28)
            let deletePromise = rdbStore.delete(predicates)
            deletePromise.then(async (ret) => {
                expect(1).assertEqual(ret)
                console.log(TAG + "Delete done: " + ret)
            }).catch((err) => {
                expect(null).assertFail()
            })
            await deletePromise
        }
        //删除后查询
        {
            let predicates = await new dataRdb.RdbPredicates("test")
            predicates.equalTo("age", 28)
            let resultSet = await rdbStore.query(predicates)
            expect(0).assertEqual(resultSet.rowCount)
        }
        done()
        console.log(TAG + "************* testRdbStoreDelete0003 end *************");
    })

    /**
     * @tc.name rdb delete test
     * @tc.number SUB_DDM_AppDataFWK_JSRDB_Delete_0040
     * @tc.desc rdb delete test
     */
    it('testRdbStoreDelete0004', 0, async function (done) {
        console.log(TAG + "************* testRdbStoreDelete0004 start *************");
        var u8 = new Uint8Array([1, 2, 3])
        {
            const valueBucket = {
                "name": "zhangsan",
                "age": 18,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 28,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 38,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        //删除
        {
            let predicates = await new dataRdb.RdbPredicates("test")
            predicates.equalTo("aaa id", 1)
            let deletePromise = rdbStore.delete(predicates)
            deletePromise.then(async (ret) => {
                console.log(TAG + "delete done: " + ret)
                expect(null).assertFail()
            }).catch((err) => {
                console.log(TAG + "delete with wrong conditions")
            })
        }
        done()
        console.log(TAG + "************* testRdbStoreDelete0004 end *************");
    })

    /**
     * @tc.name rdb delete test
     * @tc.number SUB_DDM_AppDataFWK_JSRDB_Delete_0050
     * @tc.desc rdb delete test
     */
    it('testRdbStoreDelete0005', 0, async function (done) {
        console.log(TAG + "************* testRdbStoreDelete0005 start *************");
        var u8 = new Uint8Array([1, 2, 3])
        {
            const valueBucket = {
                "name": "zhangsan",
                "age": 18,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 28,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 38,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        //删除
        {
            let predicates = await new dataRdb.RdbPredicates("test")
            predicates.equalTo("name", "lisi")
            let deletePromise = rdbStore.delete(predicates)
            deletePromise.then(async (ret) => {
                expect(2).assertEqual(ret)
                console.log(TAG + "Delete done: " + ret)
            }).catch((err) => {
                expect(null).assertFail()
            })
            await deletePromise
        }
        done()
        console.log(TAG + "************* testRdbStoreDelete0005 end *************");
    })

    /**
     * @tc.name rdb delete test
     * @tc.number SUB_DDM_AppDataFWK_JSRDB_Delete_0060
     * @tc.desc rdb delete test
     */
    it('testRdbStoreDelete0006', 0, async function (done) {
        console.log(TAG + "************* testRdbStoreDelete0006 start *************");
        var u8 = new Uint8Array([1, 2, 3])
        {
            const valueBucket = {
                "name": "zhangsan",
                "age": 18,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 28,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        {
            const valueBucket = {
                "name": "lisi",
                "age": 38,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        //删除
        {
            let predicates = await new dataRdb.RdbPredicates("")
            let deletePromise = rdbStore.delete(predicates)
            deletePromise.then(async (ret) => {
                console.log(TAG + "delete done: " + ret)
                expect(null).assertFail()
            }).catch((err) => {
                console.log(TAG + "delete with null")
            })
        }
        done()
        console.log(TAG + "************* testRdbStoreDelete0006 end *************");
    })

    console.log(TAG + "*************Unit Test End*************");
})