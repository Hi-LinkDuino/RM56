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

describe('QueryTest', function() {

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_RESET_0100
     * @tc.name [JS-API8]Query.Reset().
     * @tc.desc Test Js Api Query.Reset() testcase 001
     */
    it('testReset001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("test", 3);
            console.log("query is " + query.getSqlLike());
            expect(query.getSqlLike() !== "").assertTrue();
            query.reset();
            expect("").assertEqual(query.getSqlLike());
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("simply calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_RESET_0200
     * @tc.name [JS-API8]Query.Reset().
     * @tc.desc Test Js Api Query.Reset() testcase 002
     */
    it('testReset002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("number", 5);
            query.equalTo("string", 'v');
            query.equalTo("boolean", false);
            console.log("query is " + query.getSqlLike());
            expect(query.getSqlLike() !== "").assertTrue();
            query.reset();
            query.reset();
            query.reset();
            expect("").assertEqual(query.getSqlLike());
            console.log("sql after  reset: " + query.getSqlLike());
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_RESET_0300
     * @tc.name [JS-API8]Query.Reset().
     * @tc.desc Test Js Api Query.Reset() testcase 003
     */
    it('testReset003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("key", "value");
            expect(query.getSqlLike() !== "").assertTrue();
            let sql = query.getSqlLike();
            query.reset().equalTo("key", "value");
            console.log("query is " + query.getSqlLike());
            expect(sql === query.getSqlLike()).assertTrue();
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_RESET_0400
     * @tc.name [JS-API8]Query.Reset().
     * @tc.desc Test Js Api Query.Reset() testcase 004
     */
    it('testReset004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("key", "value");
            expect(query.getSqlLike() !== "").assertTrue();
            query.reset(3);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_EQAULTO_0100
     * @tc.name [JS-API8]Query.EqaulTo().
     * @tc.desc Test Js Api Query.EqaulTo() testcase 001
     */
    it('testEqaulTo001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("key1", 5);
            query.equalTo("key2", 5.0);
            query.equalTo("key3", false);
            query.equalTo("key3", "string");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_EQUALTO_0200
     * @tc.name [JS-API8]Query.EqualTo().
     * @tc.desc Test Js Api Query.EqualTo() testcase 002
     */
    it('testEqualTo002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("key1", 1).equalTo("key2", 2).equalTo("key3", 3);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_EQUALTO_0300
     * @tc.name [JS-API8]Query.EqualTo().
     * @tc.desc Test Js Api Query.EqualTo() testcase 003
     */
    it('testEqualTo003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("key2", NaN);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_EQUALTO_0400
     * @tc.name [JS-API8]Query.EqualTo().
     * @tc.desc Test Js Api Query.EqualTo() testcase 004
     */
    it('testEqualTo004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("key1", "value", "too more");
            console.log("should throw exception on invalid arguments");
            console.log("query is " + query.getSqlLike());
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTEQUALTO_0100
     * @tc.name [JS-API8]Query.NotEqualTo().
     * @tc.desc Test Js Api Query.NotEqualTo() testcase 001
     */
    it('testNotEqualTo001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key1", 5);
            query.notEqualTo("key2", 5.0);
            query.notEqualTo("key3", false);
            query.notEqualTo("key4", "string");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTEQUALTO_0200
     * @tc.name [JS-API8]Query.NotEqualTo().
     * @tc.desc Test Js Api Query.NotEqualTo() testcase 002
     */
    it('testNotEqualTo002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", 5);
            query.reset();
            query.notEqualTo("key0", 5).equalTo("key1", 5).notEqualTo("key2", "str").notEqualTo("key3", false);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTEQUALTO_0300
     * @tc.name [JS-API8]Query.NotEqualTo().
     * @tc.desc Test Js Api Query.NotEqualTo() testcase 003
     */
    it('testNotEqualTo003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key2", NaN);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTEQUALTO_0400
     * @tc.name [JS-API8]Query.NotEqualTo().
     * @tc.desc Test Js Api Query.NotEqualTo() testcase 004
     */
    it('testNotEqualTo004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key1", "value", "too more", 4);
            console.log("should throw exception on invalid arguments");
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GREATERTHAN_0100
     * @tc.name [JS-API8]Query.GreaterThan().
     * @tc.desc Test Js Api Query.GreaterThan() testcase 001
     */
    it('testGreaterThan001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.greaterThan("key1", 5);
            query.greaterThan("key2", 5.0);
            query.greaterThan("key3", true);
            query.greaterThan("key4", "string");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GREATTHAN_0200
     * @tc.name [JS-API8]Query.GreatThan().
     * @tc.desc Test Js Api Query.GreatThan() testcase 002
     */
    it('testGreatThan002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.greaterThan("key", 5);
            query.reset();
            query.greaterThan("key0", 5).greaterThan("key1", "v5").greaterThan("key3", false);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GREATTHAN_0300
     * @tc.name [JS-API8]Query.GreatThan().
     * @tc.desc Test Js Api Query.GreatThan() testcase 003
     */
    it('testGreatThan003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.greaterThan("key2", NaN);
            console.log("should throw exception on invalid arguments");
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GREATTHAN_0400
     * @tc.name [JS-API8]Query.GreatThan().
     * @tc.desc Test Js Api Query.GreatThan() testcase 004
     */
    it('testGreatThan004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.greaterThan("key1", "value", "too more", 4);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LESSTHAN_0100
     * @tc.name [JS-API8]Query.LessThan().
     * @tc.desc Test Js Api Query.LessThan() testcase 001
     */
    it('testLessThan001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.lessThan("key1", 5);
            query.lessThan("key2", 5.0);
            query.lessThan("key3", true);
            query.lessThan("key4", "string");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LESSTHAN_0200
     * @tc.name [JS-API8]Query.LessThan().
     * @tc.desc Test Js Api Query.LessThan() testcase 002
     */
    it('testLessThan002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.lessThan("key", 5);
            query.reset();
            query.lessThan("key0", 5).lessThan("key1", "v5").lessThan("key3", false);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertTrue();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LESSTHAN_0300
     * @tc.name [JS-API8]Query.LessThan().
     * @tc.desc Test Js Api Query.LessThan() testcase 003
     */
    it('testLessThan003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.lessThan("key2", NaN);
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LESSTHAN_0400
     * @tc.name [JS-API8]Query.LessThan().
     * @tc.desc Test Js Api Query.LessThan() testcase 004
     */
    it('testLessThan004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.lessThan("key1", "value", "too more", 4);
            console.log("query is " + query.getSqlLike());
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GREATERTHANOREQUALTO_0100
     * @tc.name [JS-API8]Query.GreaterThanOrEqualTo().
     * @tc.desc Test Js Api Query.GreaterThanOrEqualTo() testcase 001
     */
    it('testGreaterThanOrEqualTo001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.greaterThanOrEqualTo("key1", 5);
            query.greaterThanOrEqualTo("key2", 5.0);
            query.greaterThanOrEqualTo("key3", true);
            query.greaterThanOrEqualTo("key4", "string");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GREATERTHANOREQUALTO_0200
     * @tc.name [JS-API8]Query.GreaterThanOrEqualTo().
     * @tc.desc Test Js Api Query.GreaterThanOrEqualTo() testcase 002
     */
    it('testGreaterThanOrEqualTo002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.greaterThanOrEqualTo("key", 5);
            query.reset();
            query.greaterThanOrEqualTo("key0", 5)
                .greaterThanOrEqualTo("key1", "v5")
                .greaterThanOrEqualTo("key3", false);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GREATERTHANOREQUALTO_0300
     * @tc.name [JS-API8]Query.GreaterThanOrEqualTo().
     * @tc.desc Test Js Api Query.GreaterThanOrEqualTo() testcase 003
     */
    it('testGreaterThanOrEqualTo003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.greaterThanOrEqualTo("key2", NaN);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GREATERTHANOREQUALTO_0400
     * @tc.name [JS-API8]Query.GreaterThanOrEqualTo().
     * @tc.desc Test Js Api Query.GreaterThanOrEqualTo() testcase 004
     */
    it('testGreaterThanOrEqualTo004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.greaterThanOrEqualTo("key1", "value", "too more", 4);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LESSTHANOREQUALTO_0100
     * @tc.name [JS-API8]Query.LessThanOrEqualTo().
     * @tc.desc Test Js Api Query.LessThanOrEqualTo() testcase 001
     */
    it('testLessThanOrEqualTo001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.lessThanOrEqualTo("key1", 5);
            query.lessThanOrEqualTo("key2", 5.0);
            query.lessThanOrEqualTo("key3", true);
            query.lessThanOrEqualTo("key4", "string");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LESSTHANOREQUALTO_0200
     * @tc.name [JS-API8]Query.LessThanOrEqualTo().
     * @tc.desc Test Js Api Query.LessThanOrEqualTo() testcase 002
     */
    it('testLessThanOrEqualTo002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.lessThanOrEqualTo("key", 5);
            query.reset();
            query.lessThanOrEqualTo("key0", 5).lessThanOrEqualTo("key1", "v5").lessThanOrEqualTo("key3", false);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LESSTHANOREQUALTO_0300
     * @tc.name [JS-API8]Query.LessThanOrEqualTo().
     * @tc.desc Test Js Api Query.LessThanOrEqualTo() testcase 003
     */
    it('testLessThanOrEqualTo003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.lessThanOrEqualTo("key2", NaN);
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LESSTHANOREQUALTO_0400
     * @tc.name [JS-API8]Query.LessThanOrEqualTo().
     * @tc.desc Test Js Api Query.LessThanOrEqualTo() testcase 004
     */
    it('testLessThanOrEqualTo004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.lessThanOrEqualTo("key1", "value", "too more", 4);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ISNULL_0100
     * @tc.name [JS-API8]Query.IsNull().
     * @tc.desc Test Js Api Query.IsNull() testcase 001
     */
    it('testIsNull001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.isNull("key");
            query.isNull("key2");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ISNULL_0200
     * @tc.name [JS-API8]Query.IsNull().
     * @tc.desc Test Js Api Query.IsNull() testcase 002
     */
    it('testIsNull002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.isNull("key").notEqualTo("key1", 4).isNull("key2");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ISNULL_0300
     * @tc.name [JS-API8]Query.IsNull().
     * @tc.desc Test Js Api Query.IsNull() testcase 003
     */
    it('testIsNull003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.isNull("key", 0);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ISNULL_0400
     * @tc.name [JS-API8]Query.IsNull().
     * @tc.desc Test Js Api Query.IsNull() testcase 004
     */
    it('testIsNull004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.isNull(0);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /*
    * =======================================================================================
    *           Int8Array             |  INTEGER
    *           Uint8Array            |  INTEGER
    *           Uint8ClampedArray     |  INTEGER
    *           Int16Array            |  INTEGER
    *           Uint16Array           |  INTEGER
    *           Int32Array            |  INTEGER
    *           Uint32Array           |  LONG
    *           Float32Array          |  DOUBLE
    *           Float64Array          |  DOUBLE
    *           BigInt64Array         |  ERROR: cannot convert to bigint
    *           BigUint64Array        |  ERROR: cannot convert to bigint
    * =======================================================================================
	*           Array                 |  DOUBLE    * not-typedArray treated as array of double.
    */

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INNUMBER_0100
     * @tc.name [JS-API8]Query.InNumber().
     * @tc.desc Test Js Api Query.InNumber() testcase 001
     */
    it('testInNumber001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            console.log("testInNumber001 start ");
            var i8 = new Int8Array([-21,31]);
            query.reset().inNumber("key", i8);
            console.log("inNumber(Int8Array([-21,31])  => " + query.getSqlLike());
            var u8 = new Uint8Array([-21,31]);
            query.reset().inNumber("key", u8);
            console.log("inNumber(Uint8Array([-21,31])  => " + query.getSqlLike());
            var c8 = new Uint8ClampedArray([-21,31]);
            query.reset().inNumber("key", c8);
            console.log("inNumber(Uint8Array([-21,31])  => " + query.getSqlLike());
            var i16 = new Int16Array([-21,31]);
            query.reset().inNumber("key", i16);
            console.log("inNumber(Int16Array([-21,31])  => " + query.getSqlLike());
            var u16 = new Uint16Array([-21,31]);
            query.reset().inNumber("key", u16);
            console.log("inNumber(Uint16Array([-21,31])  => " + query.getSqlLike());
            var i32 = new Int32Array([-21, 31]);
            query.reset().inNumber("key", i32);
            console.log("inNumber(Int32Array([-21,31])  => " + query.getSqlLike());
            var u32 = new Uint32Array([-21, 31]);
            query.reset().inNumber("key", u32);
            console.log("inNumber(UInt32Array([-21,31])  => " + query.getSqlLike());
            var f32 = new Float32Array([-21, 31]);
            query.reset().inNumber("key", f32);
            console.log("inNumber(Float32Array([-21,31])  => " + query.getSqlLike());
            var f32e = new Float32Array([21, 31, "a"]); // "a" will be ignored as not a float.
            query.reset().inNumber("key", f32e);
            console.log("inNumber(Float32Array([-21,31, 'a'])  => " + query.getSqlLike());
            var f64 = new Float64Array([-21, 31]);
            query.reset().inNumber("key", f64);
            console.log("inNumber(Float64Array([-21,31])  => " + query.getSqlLike());
            query.reset();
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INNUMBER_0200
     * @tc.name [JS-API8]Query.InNumber().
     * @tc.desc Test Js Api Query.InNumber() testcase 002
     */
    it('testInNumber002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.inNumber("key", [1, 2.3, 987654]).
                inNumber("key2", [0x10abcdef]).
                inNumber("key2", [0xf0123456]).
                inNumber("key2", [0b10101]);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INNUMBER_0300
     * @tc.name [JS-API8]Query.InNumber().
     * @tc.desc Test Js Api Query.InNumber() testcase 003
     */
    it('testInNumber003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.inNumber("key", 0);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INNUMBER_0400
     * @tc.name [JS-API8]Query.InNumber().
     * @tc.desc Test Js Api Query.InNumber() testcase 004
     */
    it('testInNumber004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.inNumber([0, 1]);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INNUMBER_0500
     * @tc.name [JS-API8]Query.InNumber().
     * @tc.desc Test Js Api Query.InNumber() testcase 005
     */
    it('testInNumber005', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            var u64 = new BigUint64Array([21, 31]);
            query.inNumber("key", u64);
            var b64 = new BigInt64Array([21, 31]);
            query.inNumber("key", b64);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INNUMBER_0600
     * @tc.name [JS-API8]Query.InNumber().
     * @tc.desc Test Js Api Query.InNumber() testcase 006
     */
    it('testInNumber006', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            console.log("typeof([1, 2, 97])" + typeof([1, 2, 97]))
            console.log("typeof([1, 2, 97][0])" + typeof([1, 2, 97][0]))
            query.inNumber("key", [1, 2, 97]);
            console.log("inNumber([1, 2, 97])  => " + query.getSqlLike());
            query.reset();
            query.inNumber("key1", [-1, 3, 987654.123, 0xabc123456]);
            console.log("inNumber([1, 2, 0xa1234567890123456])  => " + query.getSqlLike());
            query.reset();
            query.inNumber("key2", [-1, 3, -987654.123, 0xabc123456]);
            console.log("inNumber([1, 2, 0xa1234567890123456])  => " + query.getSqlLike());
            query.reset();
            query.inNumber("key3", [-1, 4, -987654.123, Number.MAX_VALUE]);
            console.log("inNumber([1, 2, Number.MAX_VALUE])  => " + query.getSqlLike());
            query.reset();
            query.inNumber("key4", [1, -2.3, Number.MIN_VALUE, Number.MAX_VALUE]);
            console.log("inNumber([1, -2.3, Number.MAX_VALUE])  => " + query.getSqlLike());
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
            query.reset();
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INSTRING_0100
     * @tc.name [JS-API8]Query.InString().
     * @tc.desc Test Js Api Query.InString() testcase 001
     */
    it('testInString001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.inString("key", ["a2z" , 'z2a']);
            query.inString("key2", ["AAA" ]);
            console.log("query is " + query.getSqlLike());
            expect(query.getSqlLike() !== "").assertTrue();
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INSTRING_0200
     * @tc.name [JS-API8]Query.InString().
     * @tc.desc Test Js Api Query.InString() testcase 002
     */
    it('testInString002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.inString("key", ["a2z" , 'z2a'])
                .inString("key2", ["AAA" ])
                .inString("key2", ["AAA", "AAABBB","CCCAAA" ]);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INSTRING_0300
     * @tc.name [JS-API8]Query.InString().
     * @tc.desc Test Js Api Query.InString() testcase 003
     */
    it('testInString003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.inString("key", 0);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_INSTRING_0400
     * @tc.name [JS-API8]Query.InString().
     * @tc.desc Test Js Api Query.InString() testcase 004
     */
    it('testInString004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.inString("key", [0, 1]);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTINNUMBER_0100
     * @tc.name [JS-API8]Query.NotInNumber().
     * @tc.desc Test Js Api Query.NotInNumber() testcase 001
     */
    it('testNotInNumber001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notInNumber("key", [1, 2]);
            query.notInNumber("key", [1000]);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTINNUMBER_0200
     * @tc.name [JS-API8]Query.NotInNumber().
     * @tc.desc Test Js Api Query.NotInNumber() testcase 002
     */
    it('testNotInNumber002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notInNumber("key", [1, 2, 3]).notInNumber("key", [1, 7, 8]).notEqualTo("kkk", 5);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTINNUMBER_0300
     * @tc.name [JS-API8]Query.NotInNumber().
     * @tc.desc Test Js Api Query.NotInNumber() testcase 003
     */
    it('testNotInNumber003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notInNumber("key", [1], 2);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTINNUMBER_0400
     * @tc.name [JS-API8]Query.NotInNumber().
     * @tc.desc Test Js Api Query.NotInNumber() testcase 004
     */
    it('testNotInNumber004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notInNumber("key", ["string"]);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTINSTRING_0100
     * @tc.name [JS-API8]Query.NotInString().
     * @tc.desc Test Js Api Query.NotInString() testcase 001
     */
    it('testNotInString001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notInString("key", ["v1", "v2"]);
            query.notInString("key", ["v1", "NaN"]);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTINSTRING_0200
     * @tc.name [JS-API8]Query.NotInString().
     * @tc.desc Test Js Api Query.NotInString() testcase 002
     */
    it('testNotInString002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notInString("key", ["v1", "v2", "v3"]).notEqualTo("kkk", "v3");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTINSTRING_0300
     * @tc.name [JS-API8]Query.NotInString().
     * @tc.desc Test Js Api Query.NotInString() testcase 003
     */
    it('testNotInString003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notInString("key", ["", "abccd"], 2);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_NOTINSTRING_0400
     * @tc.name [JS-API8]Query.NotInString().
     * @tc.desc Test Js Api Query.NotInString() testcase 004
     */
    it('testNotInString004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notInString("key", [1, 2]);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LIKE_0100
     * @tc.name [JS-API8]Query.Like().
     * @tc.desc Test Js Api Query.Like() testcase 001
     */
    it('testLike001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.like("key", "v1");
            query.like("key2", "v2");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LIKE_0200
     * @tc.name [JS-API8]Query.Like().
     * @tc.desc Test Js Api Query.Like() testcase 002
     */
    it('testLike002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.like("key", "v1").like("key", "v3").like("key", "v2");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LIKE_0300
     * @tc.name [JS-API8]Query.Like().
     * @tc.desc Test Js Api Query.Like() testcase 003
     */
    it('testLike003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.like("key", 0);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LIKE_0400
     * @tc.name [JS-API8]Query.Like().
     * @tc.desc Test Js Api Query.Like() testcase 004
     */
    it('testLike004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.like("key", "str1", "str2");
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_UNLIKE_0100
     * @tc.name [JS-API8]Query.Unlike().
     * @tc.desc Test Js Api Query.Unlike() testcase 001
     */
    it('testUnlike001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.unlike("key", "v1");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_UNLIKE_0200
     * @tc.name [JS-API8]Query.Unlike().
     * @tc.desc Test Js Api Query.Unlike() testcase 002
     */
    it('testUnlike002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.unlike("key", "v1").unlike("key", "v3").unlike("key", "v2");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_UNLIKE_0300
     * @tc.name [JS-API8]Query.Unlike().
     * @tc.desc Test Js Api Query.Unlike() testcase 003
     */
    it('testUnlike003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.unlike("key", 0);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_UNLIKE_0400
     * @tc.name [JS-API8]Query.Unlike().
     * @tc.desc Test Js Api Query.Unlike() testcase 004
     */
    it('testUnlike004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.unlike("key", "str1", "str2");
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_AND_0100
     * @tc.name [JS-API8]Query.And().
     * @tc.desc Test Js Api Query.And() testcase 001
     */
    it('testAnd001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", 0);
            query.and();
            query.notEqualTo("key", "v1");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_AND_0200
     * @tc.name [JS-API8]Query.And().
     * @tc.desc Test Js Api Query.And() testcase 002
     */
    it('testAnd002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("key1", 0).and().equalTo("key2", "v1");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_AND_0300
     * @tc.name [JS-API8]Query.And().
     * @tc.desc Test Js Api Query.And() testcase 003
     */
    it('testAnd003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", 0).and().notEqualTo("key", 1).and();
            expect(query.getSqlLike() !== "").assertTrue();
            query.reset();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_AND_0400
     * @tc.name [JS-API8]Query.And().
     * @tc.desc Test Js Api Query.And() testcase 004
     */
    it('testAnd004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", 0).and(1).notInNumber("key", [1, 3]);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_OR_0100
     * @tc.name [JS-API8]Query.Or().
     * @tc.desc Test Js Api Query.Or() testcase 001
     */
    it('testOr001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", 0);
            query.or();
            query.notEqualTo("key", "v1");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_OR_0200
     * @tc.name [JS-API8]Query.Or().
     * @tc.desc Test Js Api Query.Or() testcase 002
     */
    it('testOr002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.equalTo("key1", 0).or().equalTo("key2", "v1");
            expect(query.getSqlLike() !== "").assertTrue();
            query.reset();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_OR_0300
     * @tc.name [JS-API8]Query.Or().
     * @tc.desc Test Js Api Query.Or() testcase 003
     */
    it('testOr003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", 0).or();
            console.log("or ... sql:" + query.getSqlLike());
            expect(query.getSqlLike() !== "").assertTrue();
            query.reset();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("throw exception is ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_OR_0400
     * @tc.name [JS-API8]Query.Or().
     * @tc.desc Test Js Api Query.Or() testcase 004
     */
    it('testOr004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", 0).or(1).notInNumber("key", [1, 3]);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ORDERBYASC_0100
     * @tc.name [JS-API8]Query.OrderByAsc().
     * @tc.desc Test Js Api Query.OrderByAsc() testcase 001
     */
    it('testOrderByAsc001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", 0);
            query.orderByAsc("sortbykey");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ORDERBYASC_0200
     * @tc.name [JS-API8]Query.OrderByAsc().
     * @tc.desc Test Js Api Query.OrderByAsc() testcase 002
     */
    it('testOrderByAsc002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", "V0").orderByAsc("sortbykey1").orderByAsc("sortbykey2");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ORDERBYASC_0300
     * @tc.name [JS-API8]Query.OrderByAsc().
     * @tc.desc Test Js Api Query.OrderByAsc() testcase 003
     */
    it('testOrderByAsc003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", false).orderByAsc(1);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ORDERBYASC_0400
     * @tc.name [JS-API8]Query.OrderByAsc().
     * @tc.desc Test Js Api Query.OrderByAsc() testcase 004
     */
    it('testOrderByAsc004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.orderByAsc();
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ORDERBYDESC_0100
     * @tc.name [JS-API8]Query.OrderByDesc().
     * @tc.desc Test Js Api Query.OrderByDesc() testcase 001
     */
    it('testOrderByDesc001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", 0);
            query.orderByDesc("sortbykey");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ORDERBYDESC_0200
     * @tc.name [JS-API8]Query.OrderByDesc().
     * @tc.desc Test Js Api Query.OrderByDesc() testcase 002
     */
    it('testOrderByDesc002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", "V0").orderByDesc("sortbykey1").orderByDesc("sortbykey2");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ORDERBYDESC_0300
     * @tc.name [JS-API8]Query.OrderByDesc().
     * @tc.desc Test Js Api Query.OrderByDesc() testcase 003
     */
    it('testOrderByDesc003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", false).orderByDesc(1);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ORDERBYDESC_0400
     * @tc.name [JS-API8]Query.OrderByDesc().
     * @tc.desc Test Js Api Query.OrderByDesc() testcase 004
     */
    it('testOrderByDesc004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.orderByDesc();
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LIMIT_0100
     * @tc.name [JS-API8]Query.Limit().
     * @tc.desc Test Js Api Query.Limit() testcase 001
     */
    it('testLimit001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", "vx");
            query.limit(10, 2);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LIMIT_0200
     * @tc.name [JS-API8]Query.Limit().
     * @tc.desc Test Js Api Query.Limit() testcase 002
     */
    it('testLimit002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", "vx").limit(10, 2)
                .equalTo("key2", 2).limit(10, 2);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LIMIT_0300
     * @tc.name [JS-API8]Query.Limit().
     * @tc.desc Test Js Api Query.Limit() testcase 003
     */
    it('testLimit003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", false).limit(10, 2, "any");
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LIMIT_0400
     * @tc.name [JS-API8]Query.Limit().
     * @tc.desc Test Js Api Query.Limit() testcase 004
     */
    it('testLimit004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", false).limit(10);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_LIMIT_0500
     * @tc.name [JS-API8]Query.Limit().
     * @tc.desc Test Js Api Query.Limit() testcase 005
     */
    it('testLimit005', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.notEqualTo("key", false).limit("any", 10);
            console.log("should throw exception on invalid arguments");
            console.log("query is " + query.getSqlLike());
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ISNOTNULL_0100
     * @tc.name [JS-API8]Query.IsNotNull().
     * @tc.desc Test Js Api Query.IsNotNull() testcase 001
     */
    it('testIsNotNull001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.isNotNull("key");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ISNOTNULL_0200
     * @tc.name [JS-API8]Query.IsNotNull().
     * @tc.desc Test Js Api Query.IsNotNull() testcase 002
     */
    it('testIsNotNull002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.isNotNull("key1").and().notEqualTo("key1", 123);
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ISNOTNULL_0300
     * @tc.name [JS-API8]Query.IsNotNull().
     * @tc.desc Test Js Api Query.IsNotNull() testcase 003
     */
    it('testIsNotNull003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.isNotNull("key2", "any");
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ISNOTNULL_0400
     * @tc.name [JS-API8]Query.IsNotNull().
     * @tc.desc Test Js Api Query.IsNotNull() testcase 004
     */
    it('testIsNotNull004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.isNotNull(1);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_BEGINGROUP_0100
     * @tc.name [JS-API8]Query.BeginGroup().
     * @tc.desc Test Js Api Query.BeginGroup() testcase 001
     */
    it('testBeginGroup001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.beginGroup();
            query.isNotNull("$.name");
            query.endGroup();
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_BEGINGROUP_0200
     * @tc.name [JS-API8]Query.BeginGroup().
     * @tc.desc Test Js Api Query.BeginGroup() testcase 002
     */
    it('testBeginGroup002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.beginGroup();
            query.beginGroup();
            query.notEqualTo("$.name", 0);
            query.endGroup();
            query.beginGroup();
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_BEGINGROUP_0300
     * @tc.name [JS-API8]Query.BeginGroup().
     * @tc.desc Test Js Api Query.BeginGroup() testcase 003
     */
    it('testBeginGroup003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.beginGroup(1);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_BEGINGROUP_0400
     * @tc.name [JS-API8]Query.BeginGroup().
     * @tc.desc Test Js Api Query.BeginGroup() testcase 004
     */
    it('testBeginGroup004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.beginGroup("any", 1);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ENDGROUP_0100
     * @tc.name [JS-API8]Query.EndGroup().
     * @tc.desc Test Js Api Query.EndGroup() testcase 001
     */
    it('testEndGroup001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.beginGroup();
            query.isNotNull("$.name");
            query.endGroup();
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ENDGROUP_0200
     * @tc.name [JS-API8]Query.EndGroup().
     * @tc.desc Test Js Api Query.EndGroup() testcase 002
     */
    it('testEndGroup002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.endGroup();
            query.beginGroup();
            query.isNotNull("$.name");
            query.endGroup();
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ENDGROUP_0300
     * @tc.name [JS-API8]Query.EndGroup().
     * @tc.desc Test Js Api Query.EndGroup() testcase 003
     */
    it('testEndGroup003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.endGroup(0);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_ENDGROUP_0400
     * @tc.name [JS-API8]Query.EndGroup().
     * @tc.desc Test Js Api Query.EndGroup() testcase 004
     */
    it('testEndGroup004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.endGroup("any");
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_PREFIXKEY_0100
     * @tc.name [JS-API8]Query.PrefixKey().
     * @tc.desc Test Js Api Query.PrefixKey() testcase 001
     */
    it('testPrefixKey001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.prefixKey("$.name");
            query.prefixKey("0");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_PREFIXKEY_0200
     * @tc.name [JS-API8]Query.PrefixKey().
     * @tc.desc Test Js Api Query.PrefixKey() testcase 002
     */
    it('testPrefixKey002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.prefixKey("kx1").or().prefixKey("kx2").or().prefixKey("kx3");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_PREFIXKEY_0300
     * @tc.name [JS-API8]Query.PrefixKey().
     * @tc.desc Test Js Api Query.PrefixKey() testcase 003
     */
    it('testPrefixKey003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.prefixKey("k", "any");
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_PREFIXKEY_0400
     * @tc.name [JS-API8]Query.PrefixKey().
     * @tc.desc Test Js Api Query.PrefixKey() testcase 004
     */
    it('testPrefixKey004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.prefixKey(123);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_SETSUGGESTINDEX_0100
     * @tc.name [JS-API8]Query.SetSuggestIndex().
     * @tc.desc Test Js Api Query.SetSuggestIndex() testcase 001
     */
    it('testSetSuggestIndex001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.setSuggestIndex("$.name");
            query.setSuggestIndex("0");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_SETSUGGESTINDEX_0200
     * @tc.name [JS-API8]Query.SetSuggestIndex().
     * @tc.desc Test Js Api Query.SetSuggestIndex() testcase 002
     */
    it('testSetSuggestIndex002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.setSuggestIndex("kxx").or().equalTo("key2", "v1");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_SETSUGGESTINDEX_0300
     * @tc.name [JS-API8]Query.SetSuggestIndex().
     * @tc.desc Test Js Api Query.SetSuggestIndex() testcase 003
     */
    it('testSetSuggestIndex003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.setSuggestIndex("k", "any");
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_SETSUGGESTINDEX_0400
     * @tc.name [JS-API8]Query.SetSuggestIndex().
     * @tc.desc Test Js Api Query.SetSuggestIndex() testcase 004
     */
    it('testSetSuggestIndex004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.setSuggestIndex(123);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_DEVICEID_0100
     * @tc.name [JS-API8]Query.DeviceId().
     * @tc.desc Test Js Api Query.DeviceId() testcase 001
     */
    it('testDeviceId001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.deviceId("$.name");
            query.deviceId("0");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_DEVICEID_0200
     * @tc.name [JS-API8]Query.DeviceId().
     * @tc.desc Test Js Api Query.DeviceId() testcase 002
     */
    it('testDeviceId002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.deviceId("kxx").equalTo("key2", "v1");
            expect(query.getSqlLike() !== "").assertTrue();
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_DEVICEID_0300
     * @tc.name [JS-API8]Query.DeviceId().
     * @tc.desc Test Js Api Query.DeviceId() testcase 003
     */
    it('testDeviceId003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.deviceId("k", "any");
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_DEVICEID_0400
     * @tc.name [JS-API8]Query.DeviceId().
     * @tc.desc Test Js Api Query.DeviceId() testcase 004
     */
    it('testDeviceId004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.deviceId(123);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GETSQLLIKE_0100
     * @tc.name [JS-API8]Query.GetSqlLike().
     * @tc.desc Test Js Api Query.GetSqlLike() testcase 001
     */
    it('testGetSqlLike001', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            let sql1 = query.getSqlLike();
            console.log("testGetSqlLike001 sql=" + sql1);
            let sql2 = query.getSqlLike();
            expect(sql1).assertEqual(sql2);
            console.log("query is " + query.getSqlLike());
        } catch (e) {
            console.log("dumplicated calls should be ok : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GETSQLLIKE_0200
     * @tc.name [JS-API8]Query.GetSqlLike().
     * @tc.desc Test Js Api Query.GetSqlLike() testcase 002
     */
    it('testGetSqlLike002', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            let sql1 = query.getSqlLike();
            console.log("testGetSqlLike002 sql=" + sql1);
            query.inString("key1", ["AAA", "BBB"])
                .or()
                .notEqualTo("key2", 0);
            let sql2 = query.getSqlLike();
            console.log("testGetSqlLike002 sql=" + sql2);
            console.log("query is " + query.getSqlLike());
            expect(sql1 !== sql2).assertTrue();
        } catch (e) {
            console.log("should be ok on Method Chaining : " + e);
            expect(null).assertFail();
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GETSQLLIKE_0300
     * @tc.name [JS-API8]Query.GetSqlLike().
     * @tc.desc Test Js Api Query.GetSqlLike() testcase 003
     */
    it('testGetSqlLike003', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.inNumber("key");
            query.getSqlLike(0);
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_QUERY_GETSQLLIKE_0400
     * @tc.name [JS-API8]Query.GetSqlLike().
     * @tc.desc Test Js Api Query.GetSqlLike() testcase 004
     */
    it('testGetSqlLike004', 0, async function(done) {
        var query = null;
        try {
            query = new ddm.Query();
            expect("").assertEqual(query.getSqlLike());
            query.inNumber("key");
            query.getSqlLike("any");
            console.log("should throw exception on invalid arguments");
            expect(null).assertFail();
        } catch (e) {
            console.log("throw exception is ok : " + e);
        }
        query = null;
        done();
    })
})