/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import ddm from '@ohos.data.distributedData';

const TEST_BUNDLE_NAME = 'ohos.acts.distributeddatamgr';
const TEST_STORE_ID = 'storeId';

var kvManager = null;
var kvStore = null;

function putBatchString(len, prefix) {
    let entries = [];
    for (var i = 0; i < len; i++) {
        entries.push({
            key : prefix + i,
            value : {
                type : ddm.ValueType.STRING,
                value : '{"english":{"first":"leda' + i + '", "second":"yang"}}'
            }
        });
    }
    return entries;
}

async function testPutAndGet(kvManager, options) {
    try {
        await kvManager.getKVStore(TEST_STORE_ID, options).then(async (store) => {
            console.log('testPutAndGet getKVStore success' + JSON.stringify(options));
            kvStore = store;
            expect(store != null).assertTrue();
        }).catch((err) => {
            console.log('testPutAndGet getKVStore fail ' + err);
            expect(null).assertFail();
        });
        var canGet = new Promise((resolve, reject) => {
            kvStore.on('dataChange', 0, function (data) {
                console.log('testPutAndGet resolve on data change: ' + JSON.stringify(data));
                resolve(data.deviceId);
            });
            let entries = putBatchString(10, 'test_key_');
            kvStore.putBatch(entries).then((data) => {
                console.log('testPutAndGet put success');
                expect(data == undefined).assertTrue();
            });
            setTimeout(() => {
                reject(new Error('not resolved in 2 second, reject it.'))
            }, 2000);
        });
        await canGet.then(async function(deviceId) {
            var query = new ddm.Query();
            query.prefixKey('test_key_');
            query.like('$.english.first', 'led%');
            if (options.kvStoreType == ddm.KVStoreType.DEVICE_COLLABORATION) {
                console.log('testPutAndGet deviceId = ' + deviceId);
                query.deviceId(deviceId);
            }
            await kvStore.getEntries(query).then((entries) => {
                console.log('testPutAndGet get success : ' + JSON.stringify(entries));
                expect(entries.length == 10).assertTrue();
            }).catch((err) => {
                console.log('testPutAndGet get fail ' + err);
                expect(null).assertFail();
            });
        }).catch((error) => {
            console.log('testPutAndGet canGet fail: ' + error);
            expect(null).assertFail();
        });
    } catch (e) {
        console.log('testPutAndGet get exception: ' + e);
    }
}

describe('SchemaTest', function() {
    const config = {
        bundleName : TEST_BUNDLE_NAME,
        userInfo : {
            userId : '0',
            userType : ddm.UserType.SAME_USER_ID
        }
    }

    var options = {
        createIfMissing : true,
        encrypt : false,
        backup : false,
        autoSync : true,
        kvStoreType : ddm.KVStoreType.SINGLE_VERSION,
        schema : {},
        securityLevel : ddm.SecurityLevel.NO_LEVEL,
    }

    beforeAll(async function (done) {
        try {
            console.log("beforeAll: createKVManager (single) with " + JSON.stringify(options));
            await ddm.createKVManager(config).then((manager) => {
                kvManager = manager;
                console.log('beforeAll createKVManager success');
            }).catch((err) => {
                console.log('beforeAll createKVManager err ' + err);
            });
        } catch (e) {
            console.log("fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    afterAll(async function (done) {
        console.log('afterAll');
        kvManager = null;
        kvStore = null;
        done();
    })

    beforeEach(async function (done) {
        console.log('beforeEach testcase will update options:' + JSON.stringify(options));
        done();
    })

    afterEach(async function (done) {
        console.log('afterEach');
        await kvManager.closeKVStore(TEST_BUNDLE_NAME, TEST_STORE_ID, kvStore).then(async () => {
            console.log('afterEach closeKVStore success');
            await kvManager.deleteKVStore(TEST_BUNDLE_NAME, TEST_STORE_ID).then(() => {
                console.log('afterEach deleteKVStore success');
            }).catch((err) => {
                console.log('afterEach deleteKVStore err ' + err);
            });
        }).catch((err) => {
            console.log('afterEach closeKVStore err ' + err);
        });
        kvStore = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SCHEMA_TOJSONSTRING_0100
     * @tc.name [JS-API8]Schema.ToJsonString()
     * @tc.desc Test Js Api Schema.ToJsonString() testcase 001
     */
    it('testToJsonString001', 0, async function(done) {
        try {
            let first = new ddm.FieldNode('first');
            first.type = ddm.ValueType.STRING;
            first.nullable = false;
            first.default = 'first name';

            let second = new ddm.FieldNode('second');
            second.type = ddm.ValueType.STRING;
            second.nullable = false;
            second.default = 'second name';

            let english = new ddm.FieldNode('english');
            english.type = ddm.ValueType.STRING;
            english.appendChild(first);
            english.appendChild(second);

            let schema = new ddm.Schema();
            schema.root.appendChild(english);
            schema.indexes = ['$.english.first', '$.english.second'];
        } catch (e) {
            console.log("schema fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SCHEMA_TOJSONSTRING_0200
     * @tc.name [JS-API8]Schema.ToJsonString()
     * @tc.desc Test Js Api Schema.ToJsonString() testcase 002
     */
    it('testToJsonString002', 0, async function(done) {
        try {
            let first = new ddm.FieldNode('first');
            first.type = ddm.ValueType.STRING;
            first.nullable = false;
            first.default = 'first name';

            let second = new ddm.FieldNode('second');
            second.type = ddm.ValueType.STRING;
            second.nullable = false;
            second.default = 'second name';

            let english = new ddm.FieldNode('english');
            english.type = ddm.ValueType.STRING;
            english.appendChild(first);
            english.appendChild(second);

            let schema = new ddm.Schema();
            schema.root.appendChild(english);
            schema.indexes = ['$.english.first', '$.english.second'];
            options.kvStoreType = ddm.KVStoreType.DEVICE_COLLABORATION;
            options.schema = schema;
            await testPutAndGet(kvManager, options);
            console.log("schematestPutAndGet done");
        } catch (e) {
            console.log("schema fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SCHEMA_TOJSONSTRING_0300
     * @tc.name [JS-API8]Schema.ToJsonString()
     * @tc.desc Test Js Api Schema.ToJsonString() testcase 003
     */
    it('testToJsonString003', 0, async function(done) {
        try {
            let name = new ddm.FieldNode('name');
            name.type = ddm.ValueType.INTEGER;
            name.nullable = false;
            name.default = 0;

            let schema = new ddm.Schema();
            schema.root.appendChild(name);
            schema.indexes = ['$.name'];
            schema.mode = 1; // STRICT
            options.kvStoreType = ddm.KVStoreType.SINGLE_VERSION;
            options.schema = schema;
            await kvManager.getKVStore(TEST_STORE_ID, options).then(async (store) => {
                console.log('testToJsonString003 getKVStore success' + JSON.stringify(options));
                kvStore = store;
                expect(store != null).assertTrue();
                await kvStore.put("test_key_1", '{"name":1}');
                await kvStore.put("test_key_2", '{"name":2}');
                await kvStore.put("test_key_3", '{"name":3}');
                console.log('testToJsonString003 Put success');
            });
            console.log('testToJsonString003 start Query ...');
            var query = new ddm.Query();
            query.prefixKey('test_key_');
            query.notEqualTo("$.name", 3);
            await kvStore.getEntries(query).then((entries) => {
                console.log('testToJsonString003 get success : ' + JSON.stringify(entries));
                expect(entries.length == 2).assertTrue();
            }).catch((err) => {
                console.log('testToJsonString003 get fail ' + err);
                expect(null).assertFail();
            });
        } catch (e) {
            console.log("testToJsonString003 fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SCHEMA_TOJSONSTRING_0400
     * @tc.name [JS-API8]Schema.ToJsonString()
     * @tc.desc Test Js Api Schema.ToJsonString() testcase 004
     */
    it('testToJsonString004', 0, async function(done) {
        try {
            let english = new ddm.FieldNode('english');
            english.type = ddm.ValueType.STRING;

            let schema = new ddm.Schema();
            schema.root.appendChild(english);
            schema.indexes = [];    // indexex set to empty array -> invalid indexes.
            expect(null).assertFail();
        } catch (e) {
            console.log("schema exception is ok: " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SCHEMA_ROOT_0100
     * @tc.name [JS-API8]Schema.root
     * @tc.desc Test Js Api Schema.root testcase 001
     */
     it('testroot001', 0, async function(done) {
        try {
            let english = new ddm.FieldNode('english');
            english.type = ddm.ValueType.STRING;

            let schema = new ddm.Schema();
            expect(schema.root instanceof ddm.FieldNode).assertTrue();
        } catch (e) {
            console.log("schema fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SCHEMA_INDEXES_0100
     * @tc.name [JS-API8]Schema.indexes
     * @tc.desc Test Js Api Schema.indexes testcase 001
     */
     it('testindexes001', 0, async function(done) {
        try {

            let schema = new ddm.Schema();
            schema.indexes = ['$.english.first', '$.english.second'];
            expect(schema.indexes[0] === '$.english.first' && schema.indexes[1] === '$.english.second').assertTrue();
        } catch (e) {
            console.log("schema fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SCHEMA_MODE_0100
     * @tc.name [JS-API8]Schema.mode
     * @tc.desc Test Js Api Schema.mode testcase 001
     */
     it('testmode001', 0, async function(done) {
        try {

            let schema = new ddm.Schema();
            schema.mode = 1;
            console.log("schema mode = "+schema.mode)   
            expect(schema.mode === 1).assertTrue();
        } catch (e) {
            console.log("schema fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SCHEMA_MODE_0200
     * @tc.name [JS-API8]Schema.mode
     * @tc.desc Test Js Api Schema.mode testcase 002
     */
     it('testmode002', 0, async function(done) {
        try {

            let schema = new ddm.Schema();
            schema.mode = 0;
            console.log("schema mode = "+schema.mode) 
            expect(schema.mode === 0).assertTrue();
        } catch (e) {
            console.log("schema fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SCHEMA_SKIP_0100
     * @tc.name [JS-API8]Schema.skip
     * @tc.desc Test Js Api Schema.skip testcase 001
     */
     it('testskip001', 0, async function(done) {
        try {

            let schema = new ddm.Schema();
            schema.skip = 0;
            expect(schema.skip === 0).assertTrue();
        } catch (e) {
            console.log("schema fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })
})

