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

describe('FieldNodeTest', function() {

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_APPENDCHILD_0100
     * @tc.name [JS-API8]FieldNode.AppendChild().
     * @tc.desc Test Js Api FieldNode.AppendChild() testcase 001
     */
    it('testAppendChild001', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child1 = new ddm.FieldNode("child1");
            let child2 = new ddm.FieldNode("child2");
            let child3 = new ddm.FieldNode("child3");
            node.appendChild(child1);
            node.appendChild(child2);
            node.appendChild(child3);
            child1 = null;
            child2 = null;
            child3 = null;
            node = null;
        } catch (e) {
            console.log("testAppendChild001 " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_APPENDCHILD_0200
     * @tc.name [JS-API8]FieldNode.AppendChild().
     * @tc.desc Test Js Api FieldNode.AppendChild() testcase 002
     */
    it('testAppendChild002', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child = new ddm.FieldNode("child");
            node.appendChild(child);
            child = null;
            node = null;
        } catch (e) {
            console.log("testAppendChild002 " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_APPENDCHILD_0300
     * @tc.name [JS-API8]FieldNode.AppendChild().
     * @tc.desc Test Js Api FieldNode.AppendChild() testcase 003
     */
    it('testAppendChild003', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child = new ddm.FieldNode();
            node.appendChild(child);
            expect(null).assertFail();
        } catch (e) {
            console.log("testAppendChild003 is ok :" + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_TOJSON_0100
     * @tc.name [JS-API8]FieldNode.ToJson().
     * @tc.desc Test Js Api FieldNode.ToJson() testcase 001
     */
    it('testToJson001', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child = new ddm.FieldNode("child");
            node.appendChild(child);
        } catch (e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_TOJSON_0200
     * @tc.name [JS-API8]FieldNode.ToJson().
     * @tc.desc Test Js Api FieldNode.ToJson() testcase 002
     */
    it('testToJson002', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child = new ddm.FieldNode("child");
            node.appendChild(child);
        } catch (e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_TOJSON_0300
     * @tc.name [JS-API8]FieldNode.ToJson().
     * @tc.desc Test Js Api FieldNode.ToJson() testcase 003
     */
    it('testToJson003', 0, async function(done) {
        try {
            let node = new ddm.FieldNode();
            let child = new ddm.FieldNode();
            node.appendChild(child);
            expect(null).assertFail();
        } catch (e) {
            console.log("testToJson003 is ok : " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_DEFAULT_0100
     * @tc.name [JS-API8]FieldNode.default.
     * @tc.desc Test Js Api FieldNode.default testcase 001
     */
     it('testdefault001', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.default = 'first name';
            console.info('defaultValue = ' + node.default);
            expect(node.default === 'first name').assertTrue()
        } catch (e) {
            console.log("testdefault001 fail on exception: " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_NULLABLE_0100
     * @tc.name [JS-API8]FieldNode.nullable.
     * @tc.desc Test Js Api FieldNode.nullable testcase 001
     */
     it('testnullable001', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.nullable = false;
            console.info('nullable = ' + node.nullable);
            expect(node.nullable === false).assertTrue()
        } catch (e) {
            console.log("testnullable001 fail on exception: " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_TYPE_STRING_0100
     * @tc.name [JS-API8]FieldNode.type.STRING
     * @tc.desc Test Js Api FieldNode.type testcase 001
     */
     it('testtype001', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.type.STRING;
            console.info('type = ' + node.type);
            expect(node.type === ddm.type.STRING).assertTrue()
        } catch (e) {
            console.log("testtype001 fail on exception: " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_TYPE_INTEGER_0200
     * @tc.name [JS-API8]FieldNode.type.INTEGER
     * @tc.desc Test Js Api FieldNode.type testcase 002
     */
     it('testtype002', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.type.INTEGER;
            console.info('type = ' + node.type);
            expect(node.type === ddm.type.INTEGER).assertTrue()
        } catch (e) {
            console.log("testtype002 fail on exception: " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_TYPE_FLOAT_0300
     * @tc.name [JS-API8]FieldNode.type.FLOAT
     * @tc.desc Test Js Api FieldNode.type testcase 003
     */
     it('testtype003', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.type.FLOAT;
            console.info('type = ' + node.type);
            expect(node.type === ddm.type.FLOAT).assertTrue()
        } catch (e) {
            console.log("testtype003 fail on exception: " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_TYPE_BYTE_ARRAY_0400
     * @tc.name [JS-API8]FieldNode.type.BYTE_ARRAY
     * @tc.desc Test Js Api FieldNode.type testcase 004
     */
     it('testtype004', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.type.BYTE_ARRAY;
            console.info('type = ' + node.type);
            expect(node.type === ddm.type.BYTE_ARRAY).assertTrue()
        } catch (e) {
            console.log("testtype004 fail on exception: " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_TYPE_BOOLEAN_0500
     * @tc.name [JS-API8]FieldNode.type.BOOLEAN
     * @tc.desc Test Js Api FieldNode.type testcase 005
     */
     it('testtype005', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.type.BOOLEAN;
            console.info('type = ' + node.type);
            expect(node.type === ddm.ValueType.BOOLEAN).assertTrue()
        } catch (e) {
            console.log("testtype005 fail on exception: " + e);
        }
        done();
    })

    /**
     * @tc.number SUB_DISTRIBUTEDDATAMGR_FIELDNODE_TYPE_DOUBLE_0100
     * @tc.name [JS-API8]FieldNode.type.DOUBLE
     * @tc.desc Test Js Api FieldNode.type testcase 006
     */
     it('testtype006', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.type.DOUBLE;
            console.info('type = ' + node.type);
            expect(node.type === ddm.type.DOUBLE).assertTrue()
        } catch (e) {
            console.log("testtype006 fail on exception: " + e);
        }
        done();
    })
})