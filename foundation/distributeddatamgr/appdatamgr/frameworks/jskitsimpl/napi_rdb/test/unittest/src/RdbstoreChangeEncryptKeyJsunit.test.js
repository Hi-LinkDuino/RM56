// @ts-nocheck
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
    name: "EncryptNoKeyInsertTest.db",
    encryptKey: new Uint8Array([1, 2, 3])
}

const STORE_CONFIG_NO_KEY = {
    name: "EncryptNoKeyInsertTest.db"
}

var i = 1;

var rdbStore = undefined;

describe('rdbStoreInsertTest', function () {
    beforeAll(async function () {
        console.info(TAG + 'beforeAll')
     })

    beforeEach(async function () {
        if ( i== 2){
            rdbStore = await dataRdb.getRdbStore(STORE_CONFIG_NO_KEY, 1);
        }
        rdbStore = await dataRdb.getRdbStore(STORE_CONFIG, 1);

        await rdbStore.executeSql(CREATE_TABLE_TEST);
        console.info(TAG + 'beforeEach')
    })

    afterEach(async function () {
        i ++;
        console.info(TAG + 'afterEach')
        rdbStore = null
        await dataRdb.deleteRdbStore("EncryptNoKeyInsertTest.db");
    })

    afterAll(async function () {
        console.info(TAG + 'afterAll')
    })

    console.log(TAG + "*************Unit Test Begin*************");

    /**
     * @tc.name testRdbStoreChangeEncryptKey0001
     * @tc.require #I4FZ6G
     * @tc.desc encrypt the db at start, then change the encrypt key
     * @tc.author lichenlong1
     */
    it('testRdbStoreChangeEncryptKey0001', 0, async function (done) {
        console.log(TAG + "************* testRdbStoreChangeEncryptKey0001 start *************");

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

        var newKey = new Uint8Array([1, 2])
        await rdbStore.changeEncryptKey(newKey);

        let predicates = new dataRdb.RdbPredicates("test");
        predicates.equalTo("name", "zhangsan")
        let resultSet = await rdbStore.query(predicates)

        try {
            console.log(TAG + "resultSet query done");
            resultSet.goToFirstRow()
            const id = resultSet.getLong(resultSet.getColumnIndex("id"))
            expect(1).assertEqual(id);

        } catch (e) {
            console.log("insert1 error " + e);
        }
        done()
        console.log(TAG + "************* testRdbStoreChangeEncryptKey0001 end *************");
    })

    /**
     * @tc.name testRdbStoreChangeEncryptKey0002
     * @tc.require #I4FZ6G
     * @tc.desc encrypt the db at start, then change the encrypt key
     * @tc.author lichenlong1
     */
    it('testRdbStoreChangeEncryptKey0002', 0, async function (done) {
        console.log(TAG + "************* testRdbStoreChangeEncryptKey0002 start *************");
        var u8 = new Uint8Array([1, 2, 3])
        let result = 1;
        {
            const valueBucket = {
                "name": "zhangsan",
                "age": 18,
                "salary": 100.5,
                "blobType": u8,
            }
            await rdbStore.insert("test", valueBucket)
        }
        try {
            let result = await rdbStore.changeEncryptKey(new Uint8Array([1, 2]));

          } catch (e) {
            expect(1).assertEqual(result);
            console.log(TAG + "can not change encrypt key, since config with no key");
        }
        done()
        console.log(TAG + "************* testRdbStoreChangeEncryptKey0002 end *************");
    })

    /**
     * @tc.name testRdbStoreChangeEncryptKey0003
     * @tc.require #I4FZ6G
     * @tc.desc encrypt the db at start, then clear the encrypt key
     *      now sqlite does not support this feature, switch encrypt to non-encrypt.
     * @tc.author lichenlong1
     */
    it('testRdbStoreChangeEncryptKey0003', 0, async function (done) {
        console.log(TAG + "************* testRdbStoreChangeEncryptKey0003 start *************");
        let result = -1;
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
        try{
            result = await rdbStore.changeEncryptKey(new Uint8Array());
          } catch (e) {
            expect(-1).assertEqual(result);
            console.log(TAG + "can not clear encrypt key 2");
        }

        done()
        console.log(TAG + "************* testRdbStoreChangeEncryptKey0003 end *************");
    })

    /**
     * testRdbStoreChangeEncryptKey0004, actually we cannot realize the scenario,
     * 1. open the connection with encrypt key
     * 2. change the encrypt key, only reopen write connection, no operation to read connection
     * 3. use read connection
     * 4. Expect the failure of readConnection
     *
     * above scenario, can be realized in specific code, when changed the key, only reopen the write connection.
     * the result is as the respect, it fails.
     */

    console.log(TAG + "*************Unit Test End*************");
})