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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import factory from '@ohos.data.distributedData'

const KEY_TEST_INT_ELEMENT = 'key_test_int_2';
const KEY_TEST_FLOAT_ELEMENT = 'key_test_float_2';
const KEY_TEST_BOOLEAN_ELEMENT = 'key_test_boolean_2';
const KEY_TEST_STRING_ELEMENT = 'key_test_string_2';
const KEY_TEST_SYNC_ELEMENT = 'key_test_sync';

const VALUE_TEST_INT_ELEMENT = 1234;
const VALUE_TEST_FLOAT_ELEMENT = 4321.12;
const VALUE_TEST_BOOLEAN_ELEMENT = true;
const VALUE_TEST_STRING_ELEMENT = 'value-string-002';
const VALUE_TEST_SYNC_ELEMENT = 'value-string-001';

const TEST_BUNDLE_NAME = 'ohos.acts.distributeddatamgr';
const TEST_STORE_ID = 'storeId';
var kvManager = null;
var kvStore = null;
const USED_DEVICE_IDS =  ['A12C1F9261528B21F95778D2FDC0B2E33943E6251AC5487F4473D005758905DB'];
const UNUSED_DEVICE_IDS =  [];  /* add you test device-ids here */
var syncDeviceIds = USED_DEVICE_IDS.concat(UNUSED_DEVICE_IDS);

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function putBatchString(len, prefix) {
    let entries = [];
    for (var i = 0; i < len; i++) {
        var entry = {
            key : prefix + i,
            value : {
                type : factory.ValueType.STRING,
                value : 'batch_test_string_value'
            }
        }
        entries.push(entry);
    }
    return entries;
}

describe('SingleKvStoreCallbackTest', function () {
    const config = {
        bundleName : TEST_BUNDLE_NAME,
        userInfo : {
            userId : '0',
            userType : factory.UserType.SAME_USER_ID
        }
    }

    const options = {
        createIfMissing : true,
        encrypt : false,
        backup : false,
        autoSync : true,
        kvStoreType : factory.KVStoreType.SINGLE_VERSION,
        schema : '',
        securityLevel : factory.SecurityLevel.S2,
    }

    beforeAll(async function (done) {
        console.log('beforeAll config:'+ JSON.stringify(config));
        await factory.createKVManager(config, function (err, manager) {
            kvManager = manager;
            console.log('beforeAll createKVManager success');
            done();
        })
    })

    afterAll(async function (done) {
        console.log('afterAll');
        kvManager = null;
        kvStore = null;
        done();
    })

    beforeEach(async function (done) {
        console.log('beforeEach' + JSON.stringify(options));
        await kvManager.getKVStore(TEST_STORE_ID, options, function (err, store) {
            kvStore = store;
            console.log('beforeEach getKVStore success');
            done();
        });
    })

    afterEach(async function (done) {
        console.log('afterEach');
        await kvManager.closeKVStore(TEST_BUNDLE_NAME, TEST_STORE_ID, kvStore, async function (err, data) {
            console.log('afterEach closeKVStore success');
            await kvManager.deleteKVStore(TEST_BUNDLE_NAME, TEST_STORE_ID, function (err, data) {
                console.log('afterEach deleteKVStore success');
                done();
            });
        });
        kvStore = null;
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTSTRING_1010
     * @tc.name [JS-API8]SingleKvStore.Put(String)
     * @tc.desc Test Js Api SingleKvStore.Put(String) testcase 101
     */
    it('testSingleKvStorePutString101', 0, async function (done) {
        console.log('testSingleKvStorePutString101');
        try {
            await kvStore.put(KEY_TEST_STRING_ELEMENT, VALUE_TEST_STRING_ELEMENT, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStorePutString101 put success');
                } else {
                    console.log('testSingleKvStorePutString101 put fail' + err);
                    expect(null).assertFail();
                }
                done();
            });
        }catch (e) {
            console.log('testSingleKvStorePutString101 put e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTSTRING_1020
     * @tc.name [JS-API8]SingleKvStore.Put(String)
     * @tc.desc Test Js Api SingleKvStore.Put(String) testcase 102
     */
    it('testSingleKvStorePutString102', 0, async function (done) {
        console.log('testSingleKvStorePutString102');
        try {
            var str = '';
            for (var i = 0 ; i < 4095; i++) {
                str += 'x';
            }
            await kvStore.put(KEY_TEST_STRING_ELEMENT+'102', str, async function (err,data) {
                console.log('testSingleKvStorePutString102 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_STRING_ELEMENT+'102', function (err,data) {
                    console.log('testSingleKvStorePutString102 get success');
                    expect(str == data).assertTrue();
                    done();
                });
            });
        }catch (e) {
            console.log('testSingleKvStorePutString102 put e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETSTRING_1010
     * @tc.name [JS-API8]SingleKvStore.GetString()
     * @tc.desc Test Js Api SingleKvStore.GetString() testcase 101
     */
    it('testSingleKvStoreGetString101', 0, async function (done) {
        console.log('testSingleKvStoreGetString101');
        try{
            await kvStore.get(KEY_TEST_STRING_ELEMENT, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreGetString101 get success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreGetString101 get fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreGetString101 get e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETSTRING_1020
     * @tc.name [JS-API8]SingleKvStore.GetString()
     * @tc.desc Test Js Api SingleKvStore.GetString() testcase 102
     */
    it('testSingleKvStoreGetString102', 0, async function (done) {
        console.log('testSingleKvStoreGetString102');
        try{
            await kvStore.put(KEY_TEST_STRING_ELEMENT, VALUE_TEST_STRING_ELEMENT, async function (err,data) {
                console.log('testSingleKvStoreGetString102 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_STRING_ELEMENT, function (err,data) {
                    console.log('testSingleKvStoreGetString102 get success');
                    expect((err == undefined) && (VALUE_TEST_STRING_ELEMENT == data)).assertTrue();
                    done();
                });
            })
        }catch(e) {
            console.log('testSingleKvStoreGetString102 get e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTINT_1010
     * @tc.name [JS-API8]SingleKvStore.Put(Int)
     * @tc.desc Test Js Api SingleKvStore.Put(Int) testcase 101
     */
    it('testSingleKvStorePutInt101', 0, async function (done) {
        console.log('testSingleKvStorePutInt101');
        try {
            await kvStore.put(KEY_TEST_INT_ELEMENT, VALUE_TEST_INT_ELEMENT, async function (err,data) {
                console.log('testSingleKvStorePutInt101 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_INT_ELEMENT, function (err,data) {
                    console.log('testSingleKvStorePutInt101 get success');
                    expect((err == undefined) && (VALUE_TEST_INT_ELEMENT == data)).assertTrue();
                    done();
                })
            });
        }catch(e) {
            console.log('testSingleKvStorePutInt101 put e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTINT_1020
     * @tc.name [JS-API8]SingleKvStore.Put(Int)
     * @tc.desc Test Js Api SingleKvStore.Put(Int) testcase 102
     */
    it('testSingleKvStorePutInt102', 0, async function (done) {
        console.log('testSingleKvStorePutInt102');
        try {
            var intValue = 987654321;
            await kvStore.put(KEY_TEST_INT_ELEMENT, intValue, async function (err,data) {
                console.log('testSingleKvStorePutInt102 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_INT_ELEMENT, function (err,data) {
                    console.log('testSingleKvStorePutInt102 get success');
                    expect((err == undefined) && (intValue == data)).assertTrue();
                    done();
                })
            });
        }catch(e) {
            console.log('testSingleKvStorePutInt102 put e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTINT_1030
     * @tc.name [JS-API8]SingleKvStore.Put(Int)
     * @tc.desc Test Js Api SingleKvStore.Put(Int) testcase 103
     */
    it('testSingleKvStorePutInt103', 0, async function (done) {
        console.log('testSingleKvStorePutInt103');
        try {
            var intValue = Number.MIN_VALUE;
            await kvStore.put(KEY_TEST_INT_ELEMENT, intValue, async function (err,data) {
                console.log('testSingleKvStorePutInt103 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_INT_ELEMENT, function (err,data) {
                    console.log('testSingleKvStorePutInt103 get success');
                    expect((err == undefined) && (intValue == data)).assertTrue();
                    done();
                })
            });
        }catch(e) {
            console.log('testSingleKvStorePutInt103 put e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTINT_1040
     * @tc.name [JS-API8]SingleKvStore.Put(Int)
     * @tc.desc Test Js Api SingleKvStore.Put(Int) testcase 104
     */
    it('testSingleKvStorePutInt104', 0, async function (done) {
        console.log('testSingleKvStorePutInt104');
        try {
            var intValue = Number.MAX_VALUE;
            await kvStore.put(KEY_TEST_INT_ELEMENT, intValue, async function (err,data) {
                console.log('testSingleKvStorePutInt104 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_INT_ELEMENT, function (err,data) {
                    console.log('testSingleKvStorePutInt104 get success');
                    expect((err == undefined) && (intValue == data)).assertTrue();
                    done();
                })
            });
        }catch(e) {
            console.log('testSingleKvStorePutInt104 put e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETINT_1010
     * @tc.name [JS-API8]SingleKvStore.GetInt()
     * @tc.desc Test Js Api SingleKvStore.GetInt() testcase 101
     */
    it('testSingleKvStoreGetInt101', 0, async function (done) {
        console.log('testSingleKvStoreGetInt101');
        try {
            await kvStore.put(KEY_TEST_INT_ELEMENT, VALUE_TEST_INT_ELEMENT, async function (err,data) {
                console.log('testSingleKvStoreGetInt101 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_INT_ELEMENT, function (err,data) {
                    console.log('testSingleKvStoreGetInt101 get success');
                    expect((err == undefined) && (VALUE_TEST_INT_ELEMENT == data)).assertTrue();
                    done();
                })
            });
        }catch(e) {
            console.log('testSingleKvStoreGetInt101 put e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETINT_1020
     * @tc.name [JS-API8]SingleKvStore.GetInt()
     * @tc.desc Test Js Api SingleKvStore.GetInt() testcase 102
     */
    it('testSingleKvStoreGetInt102', 0, async function (done) {
        console.log('testSingleKvStoreGetInt102');
        try {
            await kvStore.get(KEY_TEST_INT_ELEMENT, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreGetInt102 get success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreGetInt102 get fail');
                }
                done();
            })
        }catch(e) {
            console.log('testSingleKvStoreGetInt102 put e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTBOOL_1010
     * @tc.name [JS-API8]SingleKvStore.Put(Bool)
     * @tc.desc Test Js Api SingleKvStore.Put(Bool) testcase 101
     */
    it('testSingleKvStorePutBool101', 0, async function (done) {
        console.log('testSingleKvStorePutBool101');
        try {
            await kvStore.put(KEY_TEST_BOOLEAN_ELEMENT, VALUE_TEST_BOOLEAN_ELEMENT, function (err,data) {
                console.log('testSingleKvStorePutBool101 put success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStorePutBool101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETBOOL_1010
     * @tc.name [JS-API8]SingleKvStore.GetBool()
     * @tc.desc Test Js Api SingleKvStore.GetBool() testcase 101
     */
    it('testSingleKvStoreGetBool101', 0, async function (done) {
        console.log('testSingleKvStoreGetBool101');
        try {
            await kvStore.get(KEY_TEST_BOOLEAN_ELEMENT, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreGetBool101 get success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreGetBool101 get fail' + err);
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreGetBool101 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETBOOL_1020
     * @tc.name [JS-API8]SingleKvStore.GetBool()
     * @tc.desc Test Js Api SingleKvStore.GetBool() testcase 102
     */
    it('testSingleKvStoreGetBool102', 0, async function (done) {
        console.log('testSingleKvStoreGetBool102');
        try {
            await kvStore.put(KEY_TEST_BOOLEAN_ELEMENT, VALUE_TEST_BOOLEAN_ELEMENT, async function (err, data) {
                console.log('testSingleKvStoreGetBool102 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_BOOLEAN_ELEMENT, function (err,data) {
                    console.log('testSingleKvStoreGetBool102 get success');
                    expect((err == undefined) && (VALUE_TEST_BOOLEAN_ELEMENT == data)).assertTrue();
                    done();
                });
            })
        }catch(e) {
            console.log('testSingleKvStoreGetBool102 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTFLOAT_1010
     * @tc.name [JS-API8]SingleKvStore.Put(Float)
     * @tc.desc Test Js Api SingleKvStore.Put(Float) testcase 101
     */
    it('testSingleKvStorePutFloat101', 0, async function (done) {
        console.log('testSingleKvStorePutFloat101');
        try {
            await kvStore.put(KEY_TEST_FLOAT_ELEMENT, VALUE_TEST_FLOAT_ELEMENT, function (err,data) {
                console.log('testSingleKvStorePutFloat101 put success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStorePutFloat101 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTFLOAT_1020
     * @tc.name [JS-API8]SingleKvStore.Put(Float)
     * @tc.desc Test Js Api SingleKvStore.Put(Float) testcase 102
     */
    it('testSingleKvStorePutFloat102', 0, async function (done) {
        console.log('testSingleKvStorePutFloat102');
        try {
            var floatValue = 123456.654321;
            await kvStore.put(KEY_TEST_FLOAT_ELEMENT, floatValue, async function (err,data) {
                console.log('testSingleKvStorePutFloat102 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_FLOAT_ELEMENT, function (err, data) {
                    console.log('testSingleKvStorePutFloat102 get success');
                    expect((err == undefined) && (floatValue == data)).assertTrue();
                    done();
                })
                done();
            });
        }catch(e) {
            console.log('testSingleKvStorePutFloat102 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTFLOAT_1030
     * @tc.name [JS-API8]SingleKvStore.Put(Float)
     * @tc.desc Test Js Api SingleKvStore.Put(Float) testcase 103
     */
    it('testSingleKvStorePutFloat103', 0, async function (done) {
        console.log('testSingleKvStorePutFloat103');
        try {
            var floatValue = 123456.0;
            await kvStore.put(KEY_TEST_FLOAT_ELEMENT, floatValue, async function (err,data) {
                console.log('testSingleKvStorePutFloat103 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_FLOAT_ELEMENT, function (err, data) {
                    console.log('testSingleKvStorePutFloat103 get success');
                    expect((err == undefined) && (floatValue == data)).assertTrue();
                    done();
                })
                done();
            });
        }catch(e) {
            console.log('testSingleKvStorePutFloat103 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTFLOAT_1040
     * @tc.name [JS-API8]SingleKvStore.Put(Float)
     * @tc.desc Test Js Api SingleKvStore.Put(Float) testcase 104
     */
    it('testSingleKvStorePutFloat104', 0, async function (done) {
        console.log('testSingleKvStorePutFloat104');
        try {
            var floatValue = 123456.00;
            await kvStore.put(KEY_TEST_FLOAT_ELEMENT, floatValue, async function (err,data) {
                console.log('testSingleKvStorePutFloat104 put success');
                expect(err == undefined).assertTrue();
                await kvStore.get(KEY_TEST_FLOAT_ELEMENT, function (err, data) {
                    console.log('testSingleKvStorePutFloat104 get success');
                    expect((err == undefined) && (floatValue == data)).assertTrue();
                    done();
                })
                done();
            });
        }catch(e) {
            console.log('testSingleKvStorePutFloat104 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETFLOAT_1010
     * @tc.name [JS-API8]SingleKvStore.GetFloat()
     * @tc.desc Test Js Api SingleKvStore.GetFloat() testcase 101
     */
    it('testSingleKvStoreGetFloat101', 0, async function (done) {
        console.log('testSingleKvStoreGetFloat101');
        try {
            await kvStore.get(KEY_TEST_FLOAT_ELEMENT, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreGetFloat101 get success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreGetFloat101 get fail' + err);
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreGetFloat101 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETESTRING_1010
     * @tc.name [JS-API8]SingleKvStore.DeleteString()
     * @tc.desc Test Js Api SingleKvStore.DeleteString() testcase 101
     */
    it('testSingleKvStoreDeleteString101', 0, async function (done) {
        console.log('testSingleKvStoreDeleteString101');
        try {
            await kvStore.delete(KEY_TEST_STRING_ELEMENT, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreDeleteString101 delete success');
                } else {
                    console.log('testSingleKvStoreDeleteString101 delete fail' + err);
                    expect(null).assertFail();
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreDeleteString101 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETESTRING_1020
     * @tc.name [JS-API8]SingleKvStore.DeleteString()
     * @tc.desc Test Js Api SingleKvStore.DeleteString() testcase 102
     */
    it('testSingleKvStoreDeleteString102', 0, async function (done) {
        console.log('testSingleKvStoreDeleteString102');
        try {
            await kvStore.put(KEY_TEST_STRING_ELEMENT, VALUE_TEST_STRING_ELEMENT, async function (err, data) {
                console.log('testSingleKvStoreDeleteString102 put success');
                expect(err == undefined).assertTrue();
                await kvStore.delete(KEY_TEST_STRING_ELEMENT, function (err,data) {
                    console.log('testSingleKvStoreDeleteString102 delete success');
                    expect(err == undefined).assertTrue();
                    done();
                });
            })
        }catch(e) {
            console.log('testSingleKvStoreDeleteString102 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETEINT_1010
     * @tc.name [JS-API8]SingleKvStore.DeleteInt()
     * @tc.desc Test Js Api SingleKvStore.DeleteInt() testcase 101
     */
    it('testSingleKvStoreDeleteInt101', 0, async function (done) {
        console.log('testSingleKvStoreDeleteInt101');
        try{
            await kvStore.delete(KEY_TEST_INT_ELEMENT, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreDeleteInt101 get success');
                } else {
                    console.log('testSingleKvStoreDeleteInt101 get fail' + err);
                    expect(null).assertFail();
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreDeleteInt101 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETEINT_1020
     * @tc.name [JS-API8]SingleKvStore.DeleteInt()
     * @tc.desc Test Js Api SingleKvStore.DeleteInt() testcase 102
     */
    it('testSingleKvStoreDeleteInt102', 0, async function (done) {
        console.log('testSingleKvStoreDeleteInt102');
        try{
            await kvStore.put(KEY_TEST_INT_ELEMENT, VALUE_TEST_INT_ELEMENT, async function (err,data) {
                console.log('testSingleKvStoreDeleteInt102 put success');
                expect(err == undefined).assertTrue();
                await kvStore.delete(KEY_TEST_INT_ELEMENT, function (err,data) {
                    console.log('testSingleKvStoreDeleteInt102 delete success');
                    expect(err == undefined).assertTrue();
                    done();
                });
            })
        }catch(e) {
            console.log('testSingleKvStoreDeleteInt102 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETEFLOAT_1010
     * @tc.name [JS-API8]SingleKvStore.DeleteFloat()
     * @tc.desc Test Js Api SingleKvStore.DeleteFloat() testcase 101
     */
    it('testSingleKvStoreDeleteFloat101', 0, async function (done) {
        console.log('testSingleKvStoreDeleteFloat101');
        try{
            await kvStore.delete(KEY_TEST_FLOAT_ELEMENT, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreDeleteFloat101 get success');
                } else {
                    console.log('testSingleKvStoreDeleteFloat101 get fail' + err);
                    expect(null).assertFail();
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreDeleteFloat101 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETEFLOAT_1020
     * @tc.name [JS-API8]SingleKvStore.DeleteFloat()
     * @tc.desc Test Js Api SingleKvStore.DeleteFloat() testcase 102
     */
    it('testSingleKvStoreDeleteFloat102', 0, async function (done) {
        console.log('testSingleKvStoreDeleteFloat102');
        try{
            await kvStore.put(KEY_TEST_FLOAT_ELEMENT, VALUE_TEST_FLOAT_ELEMENT, async function (err, data) {
                console.log('testSingleKvStoreDeleteFloat102 put success');
                expect(err == undefined).assertTrue();
                await kvStore.delete(KEY_TEST_FLOAT_ELEMENT, function (err,data) {
                    console.log('testSingleKvStoreDeleteFloat102 delete success');
                    expect(err == undefined).assertTrue();
                    done();
                });
            })
        }catch(e) {
            console.log('testSingleKvStoreDeleteFloat102 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETEBOOL_1010
     * @tc.name [JS-API8]SingleKvStore.DeleteBool()
     * @tc.desc Test Js Api SingleKvStore.DeleteBool() testcase 101
     */
    it('testSingleKvStoreDeleteBool101', 0, async function (done) {
        console.log('testSingleKvStoreDeleteBool101');
        try{
            await kvStore.delete(KEY_TEST_BOOLEAN_ELEMENT, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreDeleteBool101 get success');
                } else {
                    console.log('testSingleKvStoreDeleteBool101 get fail' + err);
                    expect(null).assertFail();
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreDeleteBool101 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETEBOOL_1020
     * @tc.name [JS-API8]SingleKvStore.DeleteBool()
     * @tc.desc Test Js Api SingleKvStore.DeleteBool() testcase 102
     */
    it('testSingleKvStoreDeleteBool102', 0, async function (done) {
        console.log('testSingleKvStoreDeleteBool102');
        try{
            await kvStore.put(KEY_TEST_BOOLEAN_ELEMENT, VALUE_TEST_BOOLEAN_ELEMENT, async function (err, data) {
                console.log('testSingleKvStoreDeleteBool102 put success');
                expect(err == undefined).assertTrue();
                await kvStore.delete(KEY_TEST_BOOLEAN_ELEMENT, function (err,data) {
                    console.log('testSingleKvStoreDeleteBool102 delete success');
                    expect(err == undefined).assertTrue();
                    done();
                });
            })
        }catch(e) {
            console.log('testSingleKvStoreDeleteBool102 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ONCHANGE_1010
     * @tc.name [JS-API8]SingleKvStore.OnChange()
     * @tc.desc Test Js Api SingleKvStore.OnChange() testcase 101
     */
    it('testSingleKvStoreOnChange101', 0, async function (done) {
        console.log('testSingleKvStoreOnChange101');
        try {
            kvStore.on('dataChange', 0, function (data) {
                console.log('testSingleKvStoreOnChange101 dataChange');
                expect(data != null).assertTrue();
            });
            await kvStore.put(KEY_TEST_FLOAT_ELEMENT, VALUE_TEST_FLOAT_ELEMENT, function (err,data) {
                console.log('testSingleKvStoreOnChange101 put success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreOnChange101 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ONCHANGE_1020
     * @tc.name [JS-API8]SingleKvStore.OnChange()
     * @tc.desc Test Js Api SingleKvStore.OnChange() testcase 102
     */
    it('testSingleKvStoreOnChange102', 0, async function (done) {
        console.log('testSingleKvStoreOnChange102');
        try {
            kvStore.on('dataChange', 1, function (data) {
                console.log('testSingleKvStoreOnChange102 dataChange');
                expect(data != null).assertTrue();
            });
            await kvStore.put(KEY_TEST_FLOAT_ELEMENT, VALUE_TEST_FLOAT_ELEMENT, function (err,data) {
                console.log('testSingleKvStoreOnChange102 put success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreOnChange102 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ONCHANGE_1030
     * @tc.name [JS-API8]SingleKvStore.OnChange()
     * @tc.desc Test Js Api SingleKvStore.OnChange() testcase 103
     */
    it('testSingleKvStoreOnChange103', 0, async function (done) {
        console.log('testSingleKvStoreOnChange103');
        try {
            kvStore.on('dataChange', 2, function (data) {
                console.log('testSingleKvStoreOnChange103 dataChange');
                expect(data != null).assertTrue();
            });
            await kvStore.put(KEY_TEST_FLOAT_ELEMENT, VALUE_TEST_FLOAT_ELEMENT, function (err,data) {
                console.log('testSingleKvStoreOnChange103 put success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreOnChange103 e' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ONSYNCCOMPLETE_1010
     * @tc.name [JS-API8]SingleKvStore.OnSyncComplete()
     * @tc.desc Test Js Api SingleKvStore.OnSyncComplete() testcase 101
     */
    it('testSingleKvStoreOnSyncComplete101', 0, async function (done) {
        try {
            kvStore.on('syncComplete', function (data) {
                console.log('testSingleKvStoreOnSyncComplete101 dataChange');
                expect(data != null).assertTrue();
            });
            await kvStore.put(KEY_TEST_SYNC_ELEMENT + 'testSync101', VALUE_TEST_SYNC_ELEMENT, function (err,data) {
                console.log('testSingleKvStoreOnSyncComplete101 put success');
                expect(err == undefined).assertTrue();
            });
            try {
                var mode = factory.SyncMode.PULL_ONLY;
                console.log('kvStore.sync to ' + JSON.stringify(syncDeviceIds));
                kvStore.sync(syncDeviceIds, mode, 10);
            } catch (e) {
                console.log('testSingleKvStoreOnSyncComplete101 sync no peer device :e:' + e);
            }
        }catch(e) {
            console.log('testSingleKvStoreOnSyncComplete101 e' + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ONSYNCCOMPLETE_1020
     * @tc.name [JS-API8]SingleKvStore.OnSyncComplete()
     * @tc.desc Test Js Api SingleKvStore.OnSyncComplete() testcase 102
     */
    it('testSingleKvStoreOnSyncComplete102', 0, async function (done) {
        try {
            kvStore.on('syncComplete', function (data) {
                console.log('testSingleKvStoreOnSyncComplete102 dataChange');
                expect(data != null).assertTrue();
            });
            await kvStore.put(KEY_TEST_SYNC_ELEMENT + 'testSync102', VALUE_TEST_SYNC_ELEMENT, function (err,data) {
                console.log('testSingleKvStoreOnSyncComplete102 put success');
                expect(err == undefined).assertTrue();
            });
            try {
                var mode = factory.SyncMode.PUSH_ONLY;
                console.log('kvStore.sync to ' + JSON.stringify(syncDeviceIds));
                kvStore.sync(syncDeviceIds, mode, 10);
            } catch (e) {
                console.log('testSingleKvStoreOnSyncComplete102 sync no peer device :e:' + e);
            }
        }catch(e) {
            console.log('testSingleKvStoreOnSyncComplete102 e' + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ONSYNCCOMPLETE_1030
     * @tc.name [JS-API8]SingleKvStore.OnSyncComplete()
     * @tc.desc Test Js Api SingleKvStore.OnSyncComplete() testcase 103
     */
    it('testSingleKvStoreOnSyncComplete103', 0, async function (done) {
        try {
            kvStore.on('syncComplete', function (data) {
                console.log('testSingleKvStoreOnSyncComplete103 dataChange');
                expect(data != null).assertTrue();
            });
            await kvStore.put(KEY_TEST_SYNC_ELEMENT + 'testSync103', VALUE_TEST_SYNC_ELEMENT, function (err,data) {
                console.log('testSingleKvStoreOnSyncComplete103 put success');
                expect(err == undefined).assertTrue();
            });
            try {
                var mode = factory.SyncMode.PUSH_PULL;
                console.log('kvStore.sync to ' + JSON.stringify(syncDeviceIds));
                kvStore.sync(syncDeviceIds, mode, 10);
            } catch (e) {
                console.log('testSingleKvStoreOnSyncComplete103 sync no peer device :e:' + e);
            }
        }catch(e) {
            console.log('testSingleKvStoreOnSyncComplete103 e' + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_SETSYNCRANGE_1010
     * @tc.name [JS-API8]SingleKvStore.SetSyncRange()
     * @tc.desc Test Js Api SingleKvStore.SetSyncRange() testcase 101
     */
    it('testSingleKvStoreSetSyncRange101', 0, async function (done) {
        console.log('testSingleKvStoreSetSyncRange101');
        try {
            var localLabels = ['A', 'B'];
            var remoteSupportLabels = ['C', 'D'];
            await kvStore.setSyncRange(localLabels, remoteSupportLabels, function (err,data) {
                console.log('testSingleKvStoreSetSyncRange101 put success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreSetSyncRange101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_SETSYNCRANGE_1020
     * @tc.name [JS-API8]SingleKvStore.SetSyncRange()
     * @tc.desc Test Js Api SingleKvStore.SetSyncRange() testcase 102
     */
    it('testSingleKvStoreSetSyncRange102', 0, async function (done) {
        console.log('testSingleKvStoreSetSyncRange102');
        try {
            var localLabels = ['A', 'B'];
            var remoteSupportLabels = ['B', 'C'];
            await kvStore.setSyncRange(localLabels, remoteSupportLabels, function (err,data) {
                console.log('testSingleKvStoreSetSyncRange102 put success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreSetSyncRange102 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_SETSYNCRANGE_1030
     * @tc.name [JS-API8]SingleKvStore.SetSyncRange()
     * @tc.desc Test Js Api SingleKvStore.SetSyncRange() testcase 103
     */
    it('testSingleKvStoreSetSyncRange103', 0, async function (done) {
        console.log('testSingleKvStoreSetSyncRange103');
        try {
            var localLabels = ['A', 'B'];
            var remoteSupportLabels = ['A', 'B'];
            await kvStore.setSyncRange(localLabels, remoteSupportLabels, function (err,data) {
                console.log('testSingleKvStoreSetSyncRange103 put success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreSetSyncRange103 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTBATCH_1010
     * @tc.name [JS-API8]SingleKvStore.PutBatch()
     * @tc.desc Test Js Api SingleKvStore.PutBatch() testcase 101
     */
    it('testSingleKvStorePutBatch101', 0, async function (done) {
        console.log('testSingleKvStorePutBatch101');
        try {
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_string_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.STRING,
                        value : 'batch_test_string_value'
                    }
                }
                entries.push(entry);
            }
            console.log('testSingleKvStorePutBatch101 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStorePutBatch101 putBatch success');
                expect(err == undefined).assertTrue();
                await kvStore.getEntries('batch_test_string_key', function (err,entrys) {
                    console.log('testSingleKvStorePutBatch101 getEntries success');
                    console.log('testSingleKvStorePutBatch101 entrys.length: ' + entrys.length);
                    console.log('testSingleKvStorePutBatch101 entrys[0]: ' + JSON.stringify(entrys[0]));
                    expect(entrys.length == 10).assertTrue();
                    expect(entrys[0].value.value == 'batch_test_string_value').assertTrue();
                    done();
                });
            });
        }catch(e) {
            console.log('testSingleKvStorePutBatch101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTBATCH_1020
     * @tc.name [JS-API8]SingleKvStore.PutBatch()
     * @tc.desc Test Js Api SingleKvStore.PutBatch() testcase 102
     */
    it('testSingleKvStorePutBatch102', 0, async function (done) {
        console.log('testSingleKvStorePutBatch102');
        try {
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_number_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.INTEGER,
                        value : 222
                    }
                }
                entries.push(entry);
            }
            console.log('testSingleKvStorePutBatch102 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStorePutBatch102 putBatch success');
                expect(err == undefined).assertTrue();
                await kvStore.getEntries('batch_test_number_key', function (err,entrys) {
                    console.log('testSingleKvStorePutBatch102 getEntries success');
                    console.log('testSingleKvStorePutBatch102 entrys.length: ' + entrys.length);
                    console.log('testSingleKvStorePutBatch102 entrys[0]: ' + JSON.stringify(entrys[0]));
                    expect(entrys.length == 10).assertTrue();
                    expect(entrys[0].value.value == 222).assertTrue();
                    done();
                });
            });
        }catch(e) {
            console.log('testSingleKvStorePutBatch102 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTBATCH_1030
     * @tc.name [JS-API8]SingleKvStore.PutBatch()
     * @tc.desc Test Js Api SingleKvStore.PutBatch() testcase 103
     */
    it('testSingleKvStorePutBatch103', 0, async function (done) {
        console.log('testSingleKvStorePutBatch103');
        try {
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_number_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.FLOAT,
                        value : 2.0
                    }
                }
                entries.push(entry);
            }
            console.log('testSingleKvStorePutBatch103 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStorePutBatch103 putBatch success');
                expect(err == undefined).assertTrue();
                await kvStore.getEntries('batch_test_number_key', function (err,entrys) {
                    console.log('testSingleKvStorePutBatch103 getEntries success');
                    console.log('testSingleKvStorePutBatch103 entrys.length: ' + entrys.length);
                    console.log('testSingleKvStorePutBatch103 entrys[0]: ' + JSON.stringify(entrys[0]));
                    expect(entrys.length == 10).assertTrue();
                    expect(entrys[0].value.value == 2.0).assertTrue();
                    done();
                });
            });
        }catch(e) {
            console.log('testSingleKvStorePutBatch103 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTBATCH_1040
     * @tc.name [JS-API8]SingleKvStore.PutBatch()
     * @tc.desc Test Js Api SingleKvStore.PutBatch() testcase 104
     */
    it('testSingleKvStorePutBatch104', 0, async function (done) {
        console.log('testSingleKvStorePutBatch104');
        try {
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_number_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.DOUBLE,
                        value : 2.00
                    }
                }
                entries.push(entry);
            }
            console.log('testSingleKvStorePutBatch104 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStorePutBatch104 putBatch success');
                expect(err == undefined).assertTrue();
                await kvStore.getEntries('batch_test_number_key', function (err,entrys) {
                    console.log('testSingleKvStorePutBatch104 getEntries success');
                    console.log('testSingleKvStorePutBatch104 entrys.length: ' + entrys.length);
                    console.log('testSingleKvStorePutBatch104 entrys[0]: ' + JSON.stringify(entrys[0]));
                    expect(entrys.length == 10).assertTrue();
                    expect(entrys[0].value.value == 2.00).assertTrue();
                    done();
                });
            });
        }catch(e) {
            console.log('testSingleKvStorePutBatch104 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTBATCH_1050
     * @tc.name [JS-API8]SingleKvStore.PutBatch()
     * @tc.desc Test Js Api SingleKvStore.PutBatch() testcase 105
     */
    it('testSingleKvStorePutBatch105', 0, async function (done) {
        console.log('testSingleKvStorePutBatch105');
        try {
            var bo = false;
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_bool_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.BOOLEAN,
                        value : bo
                    }
                }
                entries.push(entry);
            }
            console.log('testSingleKvStorePutBatch105 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStorePutBatch105 putBatch success');
                expect(err == undefined).assertTrue();
                await kvStore.getEntries('batch_test_bool_key', function (err,entrys) {
                    console.log('testSingleKvStorePutBatch105 getEntries success');
                    console.log('testSingleKvStorePutBatch105 entrys.length: ' + entrys.length);
                    console.log('testSingleKvStorePutBatch105 entrys[0]: ' + JSON.stringify(entrys[0]));
                    expect(entrys.length == 10).assertTrue();
                    expect(entrys[0].value.value == bo).assertTrue();
                    done();
                });
            });
        }catch(e) {
            console.log('testSingleKvStorePutBatch105 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_PUTBATCH_1060
     * @tc.name [JS-API8]SingleKvStore.PutBatch()
     * @tc.desc Test Js Api SingleKvStore.PutBatch() testcase 106
     */
    it('testSingleKvStorePutBatch106', 0, async function (done) {
        console.log('testSingleKvStorePutBatch106');
        try {
            var arr = new Uint8Array([21,31]);
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_bool_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.BYTE_ARRAY,
                        value : arr
                    }
                }
                entries.push(entry);
            }
            console.log('testSingleKvStorePutBatch106 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStorePutBatch106 putBatch success');
                expect(err == undefined).assertTrue();
                await kvStore.getEntries('batch_test_bool_key', function (err,entrys) {
                    console.log('testSingleKvStorePutBatch106 getEntries success');
                    console.log('testSingleKvStorePutBatch106 entrys.length: ' + entrys.length);
                    console.log('testSingleKvStorePutBatch106 entrys[0]: ' + JSON.stringify(entrys[0]));
                    expect(entrys.length == 10).assertTrue();
                    expect(entrys[0].value.value.toString() == arr.toString()).assertTrue();
                    done();
                });
            });
        }catch(e) {
            console.log('testSingleKvStorePutBatch106 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETEBATCH_1010
     * @tc.name [JS-API8]SingleKvStore.DeleteBatch()
     * @tc.desc Test Js Api SingleKvStore.DeleteBatch() testcase 101
     */
    it('testSingleKvStoreDeleteBatch101', 0, async function (done) {
        console.log('testSingleKvStoreDeleteBatch101');
        try {
            let entries = [];
            let keys = [];
            for (var i = 0; i < 5; i++) {
                var key = 'batch_test_string_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.STRING,
                        value : 'batch_test_string_value'
                    }
                }
                entries.push(entry);
                keys.push(key + i);
            }
            console.log('testSingleKvStoreDeleteBatch101 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStoreDeleteBatch101 putBatch success');
                expect(err == undefined).assertTrue();
                await kvStore.deleteBatch(keys, async function (err,data) {
                    console.log('testSingleKvStoreDeleteBatch101 deleteBatch success');
                    expect(err == undefined).assertTrue();
                    done();
                });
            });
        }catch(e) {
            console.log('testSingleKvStoreDeleteBatch101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETEBATCH_1020
     * @tc.name [JS-API8]SingleKvStore.DeleteBatch()
     * @tc.desc Test Js Api SingleKvStore.DeleteBatch() testcase 102
     */
    it('testSingleKvStoreDeleteBatch102', 0, async function (done) {
        console.log('testSingleKvStoreDeleteBatch102');
        try {
            let keys = ['batch_test_string_key1', 'batch_test_string_key2'];
            await kvStore.deleteBatch(keys, function (err,data) {
                console.log('testSingleKvStoreDeleteBatch102 deleteBatch success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreDeleteBatch102 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_DELETEBATCH_1030
     * @tc.name [JS-API8]SingleKvStore.DeleteBatch()
     * @tc.desc Test Js Api SingleKvStore.DeleteBatch() testcase 103
     */
    it('testSingleKvStoreDeleteBatch103', 0, async function (done) {
        console.log('testSingleKvStoreDeleteBatch103');
        try {
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_string_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.STRING,
                        value : 'batch_test_string_value'
                    }
                }
                entries.push(entry);
            }
            console.log('testSingleKvStoreDeleteBatch103 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStoreDeleteBatch103 putBatch success');
                expect(err == undefined).assertTrue();
                let keys = ['batch_test_string_key1', 'batch_test_string_keya'];
                await kvStore.deleteBatch(keys, async function (err,data) {
                    console.log('testSingleKvStoreDeleteBatch103 deleteBatch success');
                    expect(err == undefined).assertTrue();
                    done();
                });
            });
        }catch(e) {
            console.log('testSingleKvStoreDeleteBatch103 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_STARTTRANSACTION_1010
     * @tc.name [JS-API8]SingleKvStore.startTransaction()
     * @tc.desc Test Js Api SingleKvStore.startTransaction() testcase 101
     */
    it('testSingleKvStorestartTransaction101', 0, async function (done) {
        console.log('testSingleKvStorestartTransaction101');
        try {
            var count = 0;
            kvStore.on('dataChange', 0, function (data) {
                console.log('testSingleKvStorestartTransaction101 0' + data)
                count++;
            });
            await kvStore.startTransaction(async function (err,data) {
                console.log('testSingleKvStorestartTransaction101 startTransaction success');
                expect(err == undefined).assertTrue();
                let entries = putBatchString(10, 'batch_test_string_key');
                console.log('testSingleKvStorestartTransaction101 entries: ' + JSON.stringify(entries));
                await kvStore.putBatch(entries, async function (err,data) {
                    console.log('testSingleKvStorestartTransaction101 putBatch success');
                    expect(err == undefined).assertTrue();
                    let keys = Object.keys(entries).slice(5); //delete 5 beginnings
                    await kvStore.deleteBatch(keys, async function (err,data) {
                        console.log('testSingleKvStorestartTransaction101 deleteBatch success');
                        expect(err == undefined).assertTrue();
                        await kvStore.commit(async function (err,data) {
                            console.log('testSingleKvStorestartTransaction101 commit success');
                            expect(err == undefined).assertTrue();
                            await sleep(2000);
                            expect(count == 1).assertTrue();
                            done();
                        });
                    });
                });
            });
        }catch(e) {
            console.log('testSingleKvStorestartTransaction101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_STARTTRANSACTION_1020
     * @tc.name [JS-API8]SingleKvStore.startTransaction()
     * @tc.desc Test Js Api SingleKvStore.startTransaction() testcase 102
     */
    it('testSingleKvStorestartTransaction102', 0, async function (done) {
        console.log('testSingleKvStorestartTransaction102');
        try {
            var count = 0;
            kvStore.on('dataChange', 0, function (data) {
                console.log('testSingleKvStorestartTransaction102 0' + data)
                count++;
            });
            await kvStore.startTransaction(async function (err,data) {
                console.log('testSingleKvStorestartTransaction102 startTransaction success');
                expect(err == undefined).assertTrue();
                let entries = putBatchString(10, 'batch_test_string_key');
                console.log('testSingleKvStorestartTransaction102 entries: ' + JSON.stringify(entries));
                await kvStore.putBatch(entries, async function (err,data) {
                    console.log('testSingleKvStorestartTransaction102 putBatch success');
                    expect(err == undefined).assertTrue();
                    let keys = Object.keys(entries).slice(5); //delete 5 beginnings
                    await kvStore.deleteBatch(keys, async function (err,data) {
                        console.log('testSingleKvStorestartTransaction102 deleteBatch success');
                        expect(err == undefined).assertTrue();
                        await kvStore.rollback(async function (err,data) {
                            console.log('testSingleKvStorestartTransaction102 rollback success');
                            expect(err == undefined).assertTrue();
                            await sleep(2000);
                            expect(count == 0).assertTrue();
                            done();
                        });
                    });
                });
            });
        }catch(e) {
            console.log('testSingleKvStorestartTransaction102 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_STARTTRANSACTION_1030
     * @tc.name [JS-API8]SingleKvStore.startTransaction()
     * @tc.desc Test Js Api SingleKvStore.startTransaction() testcase 103
     */
    it('testSingleKvStorestartTransaction103', 0, async function (done) {
        console.log('testSingleKvStorestartTransaction103');
        try {
            await kvStore.startTransaction(1, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStorestartTransaction103 startTransaction success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStorestartTransaction103 startTransaction fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStorestartTransaction103 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_STARTTRANSACTION_1040
     * @tc.name [JS-API8]SingleKvStore.startTransaction()
     * @tc.desc Test Js Api SingleKvStore.startTransaction() testcase 104
     */
    it('testSingleKvStorestartTransaction104', 0, async function (done) {
        console.log('testSingleKvStorestartTransaction104');
        try {
            await kvStore.startTransaction('test_string', function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStorestartTransaction104 startTransaction success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStorestartTransaction104 startTransaction fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStorestartTransaction104 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_STARTTRANSACTION_1050
     * @tc.name [JS-API8]SingleKvStore.startTransaction()
     * @tc.desc Test Js Api SingleKvStore.startTransaction() testcase 105
     */
    it('testSingleKvStorestartTransaction105', 0, async function (done) {
        console.log('testSingleKvStorestartTransaction105');
        try {
            await kvStore.startTransaction(2.000, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStorestartTransaction105 startTransaction success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStorestartTransaction105 startTransaction fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStorestartTransaction105 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_COMMIT_1010
     * @tc.name [JS-API8]SingleKvStore.Commit()
     * @tc.desc Test Js Api SingleKvStore.Commit() testcase 101
     */
    it('testSingleKvStoreCommit101', 0, async function (done) {
        console.log('testSingleKvStoreCommit101');
        try {
            await kvStore.commit(1, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreCommit101 commit success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreCommit101 commit fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreCommit101 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_COMMIT_1020
     * @tc.name [JS-API8]SingleKvStore.Commit()
     * @tc.desc Test Js Api SingleKvStore.Commit() testcase 102
     */
    it('testSingleKvStoreCommit102', 0, async function (done) {
        console.log('testSingleKvStoreCommit102');
        try {
            await kvStore.commit('test_string', function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreCommit102 commit success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreCommit102 commit fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreCommit102 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_COMMIT_1030
     * @tc.name [JS-API8]SingleKvStore.Commit()
     * @tc.desc Test Js Api SingleKvStore.Commit() testcase 103
     */
    it('testSingleKvStoreCommit103', 0, async function (done) {
        console.log('testSingleKvStoreCommit103');
        try {
            await kvStore.commit(2.000, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreCommit103 commit success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreCommit103 commit fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreCommit103 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ROLLBACK_1010
     * @tc.name [JS-API8]SingleKvStore.Rollback()
     * @tc.desc Test Js Api SingleKvStore.Rollback() testcase 101
     */
    it('testSingleKvStoreRollback101', 0, async function (done) {
        console.log('testSingleKvStoreRollback101');
        try {
            await kvStore.rollback(1, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreRollback101 commit success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreRollback101 commit fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreRollback101 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ROLLBACK_1020
     * @tc.name [JS-API8]SingleKvStore.Rollback()
     * @tc.desc Test Js Api SingleKvStore.Rollback() testcase 102
     */
    it('testSingleKvStoreRollback102', 0, async function (done) {
        console.log('testSingleKvStoreRollback102');
        try {
            await kvStore.rollback('test_string', function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreRollback102 commit success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreRollback102 commit fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreRollback102 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ROLLBACK_1030
     * @tc.name [JS-API8]SingleKvStore.Rollback()
     * @tc.desc Test Js Api SingleKvStore.Rollback() testcase 103
     */
    it('testSingleKvStoreRollback103', 0, async function (done) {
        console.log('testSingleKvStoreRollback103');
        try {
            await kvStore.rollback(2.000, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreRollback103 commit success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreRollback103 commit fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreRollback103 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ENABLESYNC_1010
     * @tc.name [JS-API8]SingleKvStore.EnableSync()
     * @tc.desc Test Js Api SingleKvStore.EnableSync() testcase 101
     */
    it('testSingleKvStoreEnableSync101', 0, async function (done) {
        console.log('testSingleKvStoreEnableSync101');
        try {
            await kvStore.enableSync(true, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreEnableSync101 enableSync success');
                    expect(err == undefined).assertTrue();
                } else {
                    console.log('testSingleKvStoreEnableSync101 enableSync fail');
                    expect(null).assertFail();
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreEnableSync101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ENABLESYNC_1020
     * @tc.name [JS-API8]SingleKvStore.EnableSync()
     * @tc.desc Test Js Api SingleKvStore.EnableSync() testcase 102
     */
    it('testSingleKvStoreEnableSync102', 0, async function (done) {
        console.log('testSingleKvStoreEnableSync102');
        try {
            await kvStore.enableSync(false, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreEnableSync102 enableSync success');
                    expect(err == undefined).assertTrue();
                } else {
                    console.log('testSingleKvStoreEnableSync102 enableSync fail');
                    expect(null).assertFail();
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreEnableSync102 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ENABLESYNC_1030
     * @tc.name [JS-API8]SingleKvStore.EnableSync()
     * @tc.desc Test Js Api SingleKvStore.EnableSync() testcase 103
     */
    it('testSingleKvStoreEnableSync103', 0, async function (done) {
        console.log('testSingleKvStoreEnableSync103');
        try {
            await kvStore.enableSync(function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreEnableSync103 enableSync success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreEnableSync103 enableSync fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreEnableSync103 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_ENABLESYNC_1040
     * @tc.name [JS-API8]SingleKvStore.EnableSync()
     * @tc.desc Test Js Api SingleKvStore.EnableSync() testcase 104
     */
    it('testSingleKvStoreEnableSync104', 0, async function (done) {
        console.log('testSingleKvStoreEnableSync104');
        try {
            await kvStore.enableSync(null, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreEnableSync104 enableSync success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreEnableSync104 enableSync fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreEnableSync104 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_REMOVEDEVICEDATA_1010
     * @tc.name [JS-API8]SingleKvStore.RemoveDeviceData()
     * @tc.desc Test Js Api SingleKvStore.RemoveDeviceData() testcase 101
     */
    it('testSingleKvStoreRemoveDeviceData101', 0, async function (done) {
        console.log('testSingleKvStoreRemoveDeviceData101');
        try {
            await kvStore.put(KEY_TEST_STRING_ELEMENT, VALUE_TEST_STRING_ELEMENT, async function (err,data) {
                console.log('testSingleKvStoreRemoveDeviceData101 put success');
                expect(err == undefined).assertTrue();
                var deviceid = 'no_exist_device_id';
                await kvStore.removeDeviceData(deviceid, async function (err,data) {
                    if (err == undefined) {
                        console.log('testSingleKvStoreRemoveDeviceData101 removeDeviceData success');
                        expect(null).assertFail();
                        done();
                    } else {
                        console.log('testSingleKvStoreRemoveDeviceData101 removeDeviceData fail');
                        await kvStore.get(KEY_TEST_STRING_ELEMENT, async function (err,data) {
                            console.log('testSingleKvStoreRemoveDeviceData101 get success');
                            expect(data == VALUE_TEST_STRING_ELEMENT).assertTrue();
                            done();
                        });
                    }
                });
            });
        }catch(e) {
            console.log('testSingleKvStoreRemoveDeviceData101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_REMOVEDEVICEDATA_1020
     * @tc.name [JS-API8]SingleKvStore.RemoveDeviceData()
     * @tc.desc Test Js Api SingleKvStore.RemoveDeviceData() testcase 102
     */
    it('testSingleKvStoreRemoveDeviceData102', 0, async function (done) {
        console.log('testSingleKvStoreRemoveDeviceData102');
        try {
            await kvStore.removeDeviceData(function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreRemoveDeviceData102 removeDeviceData success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreRemoveDeviceData102 removeDeviceData fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreRemoveDeviceData101 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_REMOVEDEVICEDATA_1030
     * @tc.name [JS-API8]SingleKvStore.RemoveDeviceData()
     * @tc.desc Test Js Api SingleKvStore.RemoveDeviceData() testcase 103
     */
    it('testSingleKvStoreRemoveDeviceData103', 0, async function (done) {
        console.log('testSingleKvStoreRemoveDeviceData103');
        try {
            await kvStore.removeDeviceData('', function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreRemoveDeviceData103 removeDeviceData success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreRemoveDeviceData103 removeDeviceData fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreRemoveDeviceData103 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_REMOVEDEVICEDATA_1040
     * @tc.name [JS-API8]SingleKvStore.RemoveDeviceData()
     * @tc.desc Test Js Api SingleKvStore.RemoveDeviceData() testcase 104
     */
    it('testSingleKvStoreRemoveDeviceData104', 0, async function (done) {
        console.log('testSingleKvStoreRemoveDeviceData104');
        try {
            await kvStore.removeDeviceData(null, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreRemoveDeviceData104 removeDeviceData success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreRemoveDeviceData104 removeDeviceData fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreRemoveDeviceData104 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_SETSYNCPARAM_1010
     * @tc.name [JS-API8]SingleKvStore.SetSyncParam()
     * @tc.desc Test Js Api SingleKvStore.SetSyncParam() testcase 101
     */
    it('testSingleKvStoreSetSyncParam101', 0, async function (done) {
        console.log('testSingleKvStoreSetSyncParam101');
        try {
            var defaultAllowedDelayMs = 500;
            await kvStore.setSyncParam(defaultAllowedDelayMs, function (err,data) {
                console.log('testSingleKvStoreSetSyncParam101 put success');
                expect(err == undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreSetSyncParam101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_SETSYNCPARAM_1020
     * @tc.name [JS-API8]SingleKvStore.SetSyncParam()
     * @tc.desc Test Js Api SingleKvStore.SetSyncParam() testcase 102
     */
    it('testSingleKvStoreSetSyncParam102', 0, async function (done) {
        console.log('testSingleKvStoreSetSyncParam102');
        try {
            await kvStore.setSyncParam(function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreSetSyncParam102 put success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreSetSyncParam102 put err' + err);
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreSetSyncParam102 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_SETSYNCPARAM_1030
     * @tc.name [JS-API8]SingleKvStore.SetSyncParam()
     * @tc.desc Test Js Api SingleKvStore.SetSyncParam() testcase 103
     */
    it('testSingleKvStoreSetSyncParam103', 0, async function (done) {
        console.log('testSingleKvStoreSetSyncParam103');
        try {
            await kvStore.setSyncParam('', function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreSetSyncParam103 put success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreSetSyncParam103 put err' + err);
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreSetSyncParam103 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_SETSYNCPARAM_1040
     * @tc.name [JS-API8]SingleKvStore.SetSyncParam()
     * @tc.desc Test Js Api SingleKvStore.SetSyncParam() testcase 104
     */
    it('testSingleKvStoreSetSyncParam104', 0, async function (done) {
        console.log('testSingleKvStoreSetSyncParam104');
        try {
            await kvStore.setSyncParam(null, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreSetSyncParam104 put success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreSetSyncParam104 put err' + err);
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreSetSyncParam104 e ' + e);
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETSECURITYLEVEL_1010
     * @tc.name [JS-API8]SingleKvStore.GetSecurityLevel()
     * @tc.desc Test Js Api SingleKvStore.GetSecurityLevel() testcase 101
     */
    // it('testSingleKvStoreGetSecurityLevel101', 0, async function (done) {
    //     console.log('testSingleKvStoreGetSecurityLevel101');
    //     try {
    //         await kvStore.getSecurityLevel(function (err,data) {
    //             console.log('testSingleKvStoreGetSecurityLevel101 getSecurityLevel success');
    //             expect(data == factory.SecurityLevel.S2).assertTrue();
    //             done();
    //         });
    //     }catch(e) {
    //         console.log('testSingleKvStoreGetSecurityLevel101 e ' + e);
    //         expect(null).assertFail();
    //         done();
    //     }
    // })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETSECURITYLEVEL_1020
     * @tc.name [JS-API8]SingleKvStore.GetSecurityLevel()
     * @tc.desc Test Js Api SingleKvStore.GetSecurityLevel() testcase 102
     */
    it('testSingleKvStoreGetSecurityLevel102', 0, async function (done) {
        console.log('testSingleKvStoreGetSecurityLevel102');
        try {
            await kvStore.getSecurityLevel(1, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreGetSecurityLevel102 getSecurityLevel success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreGetSecurityLevel102 getSecurityLevel fail' + err);
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreGetSecurityLevel102 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETSECURITYLEVEL_1030
     * @tc.name [JS-API8]SingleKvStore.GetSecurityLevel()
     * @tc.desc Test Js Api SingleKvStore.GetSecurityLevel() testcase 103
     */
    it('testSingleKvStoreGetSecurityLevel103', 0, async function (done) {
        console.log('testSingleKvStoreGetSecurityLevel103');
        try {
            await kvStore.getSecurityLevel('test_string', function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreGetSecurityLevel103 getSecurityLevel success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreGetSecurityLevel103 getSecurityLevel fail' + err);
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreGetSecurityLevel103 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETSECURITYLEVEL_1040
     * @tc.name [JS-API8]SingleKvStore.GetSecurityLevel()
     * @tc.desc Test Js Api SingleKvStore.GetSecurityLevel() testcase 104
     */
    it('testSingleKvStoreGetSecurityLevel104', 0, async function (done) {
        console.log('testSingleKvStoreGetSecurityLevel104');
        try {
            await kvStore.getSecurityLevel(2.00, function (err,data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreGetSecurityLevel104 getSecurityLevel success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreGetSecurityLevel104 getSecurityLevel fail' + err);
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreGetSecurityLevel104 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETRESULTSET_1010
     * @tc.name [JS-API8]SingleKvStore.GetResultSet()
     * @tc.desc Test Js Api SingleKvStore.GetResultSet() testcase 101
     */
    it('testSingleKvStoreGetResultSet101', 0, async function (done) {
        console.log('testSingleKvStoreGetResultSet101');
        try {
            let resultSet;
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_string_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.STRING,
                        value : 'batch_test_string_value'
                    }
                }
                entries.push(entry);
            }
            await kvStore.putBatch(entries, async function (err, data) {
                console.log('testSingleKvStoreGetResultSet101 putBatch success');
                expect(err == undefined).assertTrue();
                await kvStore.getResultSet('batch_test_string_key', async function (err, result) {
                    console.log('testSingleKvStoreGetResultSet101 getResultSet success');
                    resultSet = result;
                    expect(resultSet.getCount() == 10).assertTrue();
                    await kvStore.closeResultSet(resultSet, function (err, data) {
                        console.log('testSingleKvStoreGetResultSet101 closeResultSet success');
                        expect(err == undefined).assertTrue();
                        done();
                    })
                });
            });
        }catch(e) {
            console.log('testSingleKvStoreGetResultSet101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETRESULTSET_1020
     * @tc.name [JS-API8]SingleKvStore.GetResultSet()
     * @tc.desc Test Js Api SingleKvStore.GetResultSet() testcase 102
     */
    it('testSingleKvStoreGetResultSet102', 0, async function (done) {
        console.log('testSingleKvStoreGetResultSet102');
        try {
            let resultSet;
            await kvStore.getResultSet('batch_test_string_key', async function (err, result) {
                console.log('testSingleKvStoreGetResultSet102 getResultSet success');
                resultSet = result;
                expect(resultSet.getCount() == 0).assertTrue();
                await kvStore.closeResultSet(resultSet, function (err, data) {
                    console.log('testSingleKvStoreGetResultSet102 closeResultSet success');
                    expect(err == undefined).assertTrue();
                    done();
                })
            });
        }catch(e) {
            console.log('testSingleKvStoreGetResultSet102 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETRESULTSET_1030
     * @tc.name [JS-API8]SingleKvStore.GetResultSet()
     * @tc.desc Test Js Api SingleKvStore.GetResultSet() testcase 103
     */
    it('testSingleKvStoreGetResultSet103', 0, async function (done) {
        console.log('testSingleKvStoreGetResultSet103');
        try {
            let resultSet;
            await kvStore.getResultSet(function (err, result) {
                console.log('testSingleKvStoreGetResultSet103 getResultSet success');
                expect(err != undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreGetResultSet103 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETRESULTSET_1040
     * @tc.name [JS-API8]SingleKvStore.GetResultSet()
     * @tc.desc Test Js Api SingleKvStore.GetResultSet() testcase 104
     */
    it('testSingleKvStoreGetResultSet104', 0, async function (done) {
        console.log('testSingleKvStoreGetResultSet104');
        try {
            let resultSet;
            await kvStore.getResultSet('test_key_string', 123, function (err, result) {
                console.log('testSingleKvStoreGetResultSet104 getResultSet success');
                expect(err != undefined).assertTrue();
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreGetResultSet104 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETRESULTSET_1050
     * @tc.name [JS-API8]SingleKvStore.GetResultSet()
     * @tc.desc Test Js Api SingleKvStore.GetResultSet() testcase 105
     */
    it('testSingleKvStoreGetResultSet105', 0, async function (done) {
        console.log('testSingleKvStoreGetResultSet105');
        try {
            let resultSet;
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_string_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.STRING,
                        value : 'batch_test_string_value'
                    }
                }
                entries.push(entry);
            }
            await kvStore.putBatch(entries, async function (err, data) {
                console.log('testSingleKvStoreGetResultSet105 putBatch success');
                expect(err == undefined).assertTrue();
                var query = new factory.Query();
                query.prefixKey("batch_test");
                await kvStore.getResultSet(query, async function (err, result) {
                    console.log('testSingleKvStoreGetResultSet105 getResultSet success');
                    resultSet = result;
                    expect(resultSet.getCount() == 10).assertTrue();
                    await kvStore.closeResultSet(resultSet, function (err, data) {
                        console.log('testSingleKvStoreGetResultSet105 closeResultSet success');
                        expect(err == undefined).assertTrue();
                        done();
                    })
                });
            });
        } catch(e) {
            console.log('testSingleKvStoreGetResultSet105 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETRESULTSET_1060
     * @tc.name [JS-API8]SingleKvStore.GetResultSet()
     * @tc.desc Test Js Api SingleKvStore.GetResultSet() testcase 106
     */
    it('testSingleKvStoreGetResultSet106', 0, async function (done) {
        console.log('testSingleKvStoreGetResultSet106');
        try {
            let resultSet;
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_string_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.STRING,
                        value : 'batch_test_string_value'
                    }
                }
                entries.push(entry);
            }
            await kvStore.putBatch(entries, async function (err, data) {
                console.log('testSingleKvStoreGetResultSet106 putBatch success');
                expect(err == undefined).assertTrue();
                var query = new factory.Query();
                query.prefixKey("batch_test");
                await kvStore.getResultSet(query, async function (err, result) {
                    console.log('testSingleKvStoreGetResultSet106 getResultSet success');
                    resultSet = result;
                    expect(resultSet.getCount() == 10).assertTrue();
                    await kvStore.closeResultSet(resultSet, function (err, data) {
                        console.log('testSingleKvStoreGetResultSet106 closeResultSet success');
                        expect(err == undefined).assertTrue();
                        done();
                    })
                });
            });
        } catch(e) {
            console.log('testSingleKvStoreGetResultSet106 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_CLOSERESULTSET_1010
     * @tc.name [JS-API8]SingleKvStore.CloseResultSet()
     * @tc.desc Test Js Api SingleKvStore.CloseResultSet() testcase 101
     */
    it('testSingleKvStoreCloseResultSet101', 0, async function (done) {
        console.log('testSingleKvStoreCloseResultSet101');
        try {
            console.log('testSingleKvStoreCloseResultSet101 success');
            let resultSet = null;
            await kvStore.closeResultSet(resultSet, function (err, data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreCloseResultSet101 closeResultSet success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreCloseResultSet101 closeResultSet fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreCloseResultSet101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_CLOSERESULTSET_1020
     * @tc.name [JS-API8]SingleKvStore.CloseResultSet()
     * @tc.desc Test Js Api SingleKvStore.CloseResultSet() testcase 102
     */
    it('testSingleKvStoreCloseResultSet102', 0, async function (done) {
        console.log('testSingleKvStoreCloseResultSet102');
        try {
            let resultSet = null;
            await kvStore.getResultSet('batch_test_string_key', async function(err, result) {
                console.log('testSingleKvStoreCloseResultSet102 getResultSet success');
                resultSet = result;
                await kvStore.closeResultSet(resultSet, function (err, data) {
                    if (err == undefined) {
                        console.log('testSingleKvStoreCloseResultSet102 closeResultSet success');
                        expect(err == undefined).assertTrue();
                    } else {
                        console.log('testSingleKvStoreCloseResultSet102 closeResultSet fail');
                        expect(null).assertFail();
                    }
                    done();
                });
            });
        }catch(e) {
            console.log('testSingleKvStoreCloseResultSet102 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_CLOSERESULTSET_1030
     * @tc.name [JS-API8]SingleKvStore.CloseResultSet()
     * @tc.desc Test Js Api SingleKvStore.CloseResultSet() testcase 103
     */
    it('testSingleKvStoreCloseResultSet103', 0, async function (done) {
        console.log('testSingleKvStoreCloseResultSet103');
        try {
            console.log('testSingleKvStoreCloseResultSet103 success');
            await kvStore.closeResultSet(function (err, data) {
                if (err == undefined) {
                    console.log('testSingleKvStoreCloseResultSet103 closeResultSet success');
                    expect(null).assertFail();
                } else {
                    console.log('testSingleKvStoreCloseResultSet103 closeResultSet fail');
                }
                done();
            });
        }catch(e) {
            console.log('testSingleKvStoreCloseResultSet103 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_CLOSERESULTSET_1040
     * @tc.name [JS-API8]SingleKvStore.CloseResultSet()
     * @tc.desc Test Js Api SingleKvStore.CloseResultSet() testcase 104
     */
    it('testSingleKvStoreCloseResultSet104', 0, async function (done) {
        console.log('testSingleKvStoreCloseResultSet104');
        try {
            console.log('testSingleKvStoreCloseResultSet104 success');
        }catch(e) {
            console.log('testSingleKvStoreCloseResultSet104 e ' + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETRESULTSIZE_1010
     * @tc.name [JS-API8]SingleKvStore.GetResultSize()
     * @tc.desc Test Js Api SingleKvStore.GetResultSize() testcase 101
     */
    it('testSingleKvStoreGetResultSize101', 0, async function (done) {
        console.log('testSingleKvStoreGetResultSize101');
        try {
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_string_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.STRING,
                        value : 'batch_test_string_value'
                    }
                }
                entries.push(entry);
            }
            await kvStore.putBatch(entries, async function (err, data) {
                console.log('testSingleKvStoreGetResultSize101 putBatch success');
                expect(err == undefined).assertTrue();
                var query = new factory.Query();
                query.prefixKey("batch_test");
                await kvStore.getResultSize(query, async function (err, resultSize) {
                    console.log('testSingleKvStoreGetResultSize101 getResultSet success');
                    expect(resultSize == 10).assertTrue();
                    done();
                });
            });
        } catch(e) {
            console.log('testSingleKvStoreGetResultSize101 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETRESULTSIZE_1020
     * @tc.name [JS-API8]SingleKvStore.GetResultSize()
     * @tc.desc Test Js Api SingleKvStore.GetResultSize() testcase 102
     */
    it('testSingleKvStoreGetResultSize102', 0, async function (done) {
        console.log('testSingleKvStoreGetResultSize102');
        try {
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_string_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.STRING,
                        value : 'batch_test_string_value'
                    }
                }
                entries.push(entry);
            }
            await kvStore.putBatch(entries, async function (err, data) {
                console.log('testSingleKvStoreGetResultSize102 putBatch success');
                expect(err == undefined).assertTrue();
                var query = new factory.Query();
                query.prefixKey("batch_test");
                await kvStore.getResultSize(query, async function (err, resultSize) {
                    console.log('testSingleKvStoreGetResultSize102 getResultSet success');
                    expect(resultSize == 10).assertTrue();
                    done();
                });
            });
        } catch(e) {
            console.log('testSingleKvStoreGetResultSize102 e ' + e);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETENTRIES_1010
     * @tc.name [JS-API8]SingleKvStore.GetEntries()
     * @tc.desc Test Js Api SingleKvStore.GetEntries() testcase 101
     */
    it('testSingleKvStoreGetEntries101', 0, async function (done) {
        console.log('testSingleKvStoreGetEntries101');
        try {
            var arr = new Uint8Array([21,31]);
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_bool_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.BYTE_ARRAY,
                        value : arr
                    }
                }
                entries.push(entry);
            }
            console.log('testSingleKvStoreGetEntries101 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStoreGetEntries101 putBatch success');
                expect(err == undefined).assertTrue();
                var query = new factory.Query();
                query.prefixKey("batch_test");
                await kvStore.getEntries(query, function (err,entrys) {
                    console.log('testSingleKvStoreGetEntries101 getEntries success');
                    console.log('testSingleKvStoreGetEntries101 entrys.length: ' + entrys.length);
                    console.log('testSingleKvStoreGetEntries101 entrys[0]: ' + JSON.stringify(entrys[0]));
                    expect(entrys.length == 10).assertTrue();
                    expect(entrys[0].value.value.toString() == arr.toString()).assertTrue();
                    done();
                });
            });
            console.log('testSingleKvStoreGetEntries101 success');
        }catch(e) {
            console.log('testSingleKvStoreGetEntries101 e ' + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_SINGLEKVSTORE_GETENTRIES_1020
     * @tc.name [JS-API8]SingleKvStore.GetEntries()
     * @tc.desc Test Js Api SingleKvStore.GetEntries() testcase 102
     */
    it('testSingleKvStoreGetEntries102', 0, async function (done) {
        console.log('testSingleKvStoreGetEntries102');
        try {
            var arr = new Uint8Array([21,31]);
            let entries = [];
            for (var i = 0; i < 10; i++) {
                var key = 'batch_test_bool_key';
                var entry = {
                    key : key + i,
                    value : {
                        type : factory.ValueType.BYTE_ARRAY,
                        value : arr
                    }
                }
                entries.push(entry);
            }
            console.log('testSingleKvStoreGetEntries102 entries: ' + JSON.stringify(entries));
            await kvStore.putBatch(entries, async function (err,data) {
                console.log('testSingleKvStoreGetEntries102 putBatch success');
                expect(err == undefined).assertTrue();
                var query = new factory.Query();
                query.prefixKey("batch_test");
                await kvStore.getEntries(query, function (err,entrys) {
                    console.log('testSingleKvStoreGetEntries102 getEntries success');
                    console.log('testSingleKvStoreGetEntries102 entrys.length: ' + entrys.length);
                    console.log('testSingleKvStoreGetEntries102 entrys[0]: ' + JSON.stringify(entrys[0]));
                    expect(entrys.length == 10).assertTrue();
                    expect(entrys[0].value.value.toString() == arr.toString()).assertTrue();
                    done();
                });
            });
            console.log('testSingleKvStoreGetEntries101 success');
            console.log('testSingleKvStoreGetEntries102 success');
        }catch(e) {
            console.log('testSingleKvStoreGetEntries102 e ' + e);
            expect(null).assertFail();
        }
        done();
    })
})