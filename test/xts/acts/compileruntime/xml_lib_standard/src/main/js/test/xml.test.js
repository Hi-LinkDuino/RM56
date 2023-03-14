/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import xml from '@ohos.xml'
describe('XmlSerializerXmlPullParserTest', function () {

    /**
     * @tc.name: testSetDeclaration001
     * @tc.desc: Writes xml declaration with encoding. For example: <?xml version="1.0" encoding="utf-8"?>.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDeclaration001', 0, function () {
        var arrayBuffer = new ArrayBuffer(2048);
        var thatSer = new xml.XmlSerializer(arrayBuffer,"utf-8");
        thatSer.setDeclaration();
        var result = '<?xml version="1.0" encoding="utf-8"?>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetDeclaration002
     * @tc.desc: Writes xml declaration with encoding. For example: <?xml version="1.0" encoding="utf-8"?>.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDeclaration002', 0, function () {
        var arrayBuffer = new ArrayBuffer(2048);
        var thatSer = new xml.XmlSerializer(arrayBuffer,"utf-8");
        thatSer.setDeclaration();
        var result = '<?xml version="1.0" encoding="utf-8"?>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetDeclaration003
     * @tc.desc: Writes xml declaration with encoding. For example: <?xml version="1.0" encoding="utf-8"?>.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDeclaration003', 0, function () {
        var arrayBuffer = new ArrayBuffer(2048);
        var thatSer = new xml.XmlSerializer(arrayBuffer,"utf-8");
        thatSer.setDeclaration();
        var result = '<?xml version="1.0" encoding="utf-8"?>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetDeclaration004
     * @tc.desc: Writes xml declaration with encoding. For example: <?xml version="1.0" encoding="utf-8"?>.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDeclaration004', 0, function () {
        var arrayBuffer = new ArrayBuffer(2048);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDeclaration();
        var result = '<?xml version="1.0" encoding="utf-8"?>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetDeclaration005
     * @tc.desc: Writes xml declaration with encoding. For example: <?xml version="1.0" encoding="utf-8"?>.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDeclaration005', 0, function () {
        var arrayBuffer = new ArrayBuffer(2048);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDeclaration();
        var result = '<?xml version="1.0" encoding="utf-8"?>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testStartElement001
     * @tc.desc: Writes an elemnet start tag with the given name.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testStartElement001', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note1");
        thatSer.endElement();
        var result = '<note1/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testStartElement002
     * @tc.desc: Writes an elemnet start tag with the given name.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testStartElement002', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note1");
        thatSer.endElement();
        thatSer.startElement("note2");
        thatSer.endElement();
        var result = '<note1/>\r\n<note2/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testStartElement003
     * @tc.desc: Writes an elemnet start tag with the given name.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testStartElement003', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note1");
        thatSer.startElement("note2");
        thatSer.endElement();
        thatSer.endElement();
        var result = '<note1>\r\n  <note2/>\r\n</note1>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testStartElement004
     * @tc.desc: Writes an elemnet start tag with the given name.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testStartElement004', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note1");
        thatSer.startElement("note2");
        thatSer.startElement("note3");
        thatSer.endElement();
        thatSer.endElement();
        thatSer.endElement();
        var result = '<note1>\r\n  <note2>\r\n    <note3/>\r\n  </note2>\r\n</note1>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testStartElement005
     * @tc.desc: Writes an elemnet start tag with the given name.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testStartElement005', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note1");
        thatSer.startElement("note2");
        thatSer.endElement();
        thatSer.startElement("note3");
        thatSer.endElement();
        thatSer.endElement();
        var result = '<note1>\r\n  <note2/>\r\n  <note3/>\r\n</note1>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetAttributes001
     * @tc.desc: Write an attribute.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetAttributes001', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance1", "high1");
        thatSer.endElement();
        var result = '<note importance1="high1"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetAttributes002
     * @tc.desc: Write an attribute.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetAttributes002', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance1", "high1");
        thatSer.setAttributes("importance2", "high2");
        thatSer.endElement();
        var result = '<note importance1="high1" importance2="high2"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetAttributes003
     * @tc.desc: Write an attribute.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetAttributes003', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance1", "high1");
        thatSer.setAttributes("importance2", "high2");
        thatSer.setAttributes("importance3", "high3");
        thatSer.endElement();
        var result = '<note importance1="high1" importance2="high2" importance3="high3"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetAttributes004
     * @tc.desc: Write an attribute.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetAttributes004', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance1", "high1");
        thatSer.setAttributes("importance2", "high2");
        thatSer.setAttributes("importance3", "high3");
        thatSer.setAttributes("importance4", "high4");
        thatSer.endElement();
        var result = '<note importance1="high1" importance2="high2" importance3="high3" importance4="high4"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetAttributes005
     * @tc.desc: Write an attribute.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetAttributes005', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance1", "high1");
        thatSer.setAttributes("importance2", "high2");
        thatSer.setAttributes("importance3", "high3");
        thatSer.setAttributes("importance5", "high5");
        thatSer.endElement();
        var result = '<note importance1="high1" importance2="high2" importance3="high3" importance5="high5"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testEndElement001
     * @tc.desc: Writes end tag of the element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testEndElement001', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.endElement();
        var result = '<note/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testEndElement002
     * @tc.desc: Writes end tag of the element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testEndElement002', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance", "high");
        thatSer.endElement();
        var result = '<note importance="high"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testEndElement003
     * @tc.desc: Writes end tag of the element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testEndElement003', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("impo", "hi");
        thatSer.endElement();
        var result = '<note impo="hi"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testEndElement004
     * @tc.desc: Writes end tag of the element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testEndElement004', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note1");
        thatSer.startElement("note2");
        thatSer.endElement();
        thatSer.endElement();
        var result = '<note1>\r\n  <note2/>\r\n</note1>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testEndElement005
     * @tc.desc: Writes end tag of the element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testEndElement005', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note2");
        thatSer.setAttributes("importance", "high");
        thatSer.endElement();
        var result = '<note2 importance="high"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetText001
     * @tc.desc: Writes the text.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetText001', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance", "high");
        thatSer.setText("Happy1");
        thatSer.endElement();
        var result = '<note importance="high">Happy1</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetText002
     * @tc.desc: Writes the text.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetText002', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance", "high");
        thatSer.setText("Happy2");
        thatSer.endElement();
        var result = '<note importance="high">Happy2</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetText003
     * @tc.desc: Writes the text.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetText003', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance", "high");
        thatSer.setText("Happy3");
        thatSer.endElement();
        var result = '<note importance="high">Happy3</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetText004
     * @tc.desc: Writes the text.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetText004', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance", "high");
        thatSer.setText("Happy4");
        thatSer.endElement();
        var result = '<note importance="high">Happy4</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetText005
     * @tc.desc: Writes the text.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetText005', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setAttributes("importance", "high");
        thatSer.setText("Happy5");
        thatSer.endElement();
        var result = '<note importance="high">Happy5</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testAddEmptyElement001
     * @tc.desc: Add an empty element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testAddEmptyElement001', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.addEmptyElement("a");
        thatSer.endElement();
        var result = '<note>\r\n  <a/>\r\n</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testAddEmptyElement002
     * @tc.desc: Add an empty element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testAddEmptyElement002', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.endElement();
        thatSer.addEmptyElement("b");
        var result = '<note/>\r\n<b/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testAddEmptyElement003
     * @tc.desc: Add an empty element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testAddEmptyElement003', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.addEmptyElement("c");
        thatSer.startElement("note");
        thatSer.endElement();
        var result = '<c/>\r\n<note/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testAddEmptyElement004
     * @tc.desc: Add an empty element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testAddEmptyElement004', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.addEmptyElement("d");
        var result = '<d/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testAddEmptyElement005
     * @tc.desc: Add an empty element.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testAddEmptyElement005', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.addEmptyElement("c");
        thatSer.addEmptyElement("d");
        thatSer.endElement();
        var result = '<note>\r\n  <c/>\r\n  <d/>\r\n</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetNamespace001
     * @tc.desc: Writes the namespace of the current element tag.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetNamespace001', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDeclaration();
        thatSer.setNamespace("h", "http://www.w3.org/TR/html4/");
        thatSer.startElement("note");
        thatSer.endElement();
        var result = '<?xml version="1.0" encoding="utf-8"?>\r\n<h:note xmlns:h="http://www.w3.org/TR/html4/"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetNamespace002
     * @tc.desc: Writes the namespace of the current element tag.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetNamespace002', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDeclaration();
        thatSer.setNamespace("b", "http://www.w3.org/TR/html4/");
        thatSer.startElement("note");
        thatSer.endElement();
        var result = '<?xml version="1.0" encoding="utf-8"?>\r\n<b:note xmlns:b="http://www.w3.org/TR/html4/"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetNamespace003
     * @tc.desc: Writes the namespace of the current element tag.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetNamespace003', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDeclaration();
        thatSer.setNamespace("h", "http://www.111/");
        thatSer.startElement("note");
        thatSer.endElement();
        var result = '<?xml version="1.0" encoding="utf-8"?>\r\n<h:note xmlns:h="http://www.111/"/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetNamespace004
     * @tc.desc: Writes the namespace of the current element tag.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetNamespace004', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDeclaration();
        thatSer.setNamespace("h", "http://www.w3.org/TR/html4/");
        thatSer.startElement("note1");
        thatSer.startElement("note2");
        thatSer.endElement();
        thatSer.endElement();
        var result = '<?xml version="1.0" encoding="utf-8"?>\r\n<h:note1 xmlns:h="http://www.w3.org/TR/html4/">' +
        '\r\n  <h:note2/>\r\n</h:note1>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetNamespace005
     * @tc.desc: Writes the namespace of the current element tag.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetNamespace005', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDeclaration();
        thatSer.setNamespace("h", "http://www.w3.org/TR/html4/");
        thatSer.startElement("note1");
        thatSer.startElement("note2");
        thatSer.endElement();
        thatSer.endElement();
        var result = '<?xml version="1.0" encoding="utf-8"?>\r\n<h:note1 xmlns:h="http://www.w3.org/TR/html4/">' +
        '\r\n  <h:note2/>\r\n</h:note1>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetComment001
     * @tc.desc: Writes the comment.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetComment001', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setComment("Hi!");
        thatSer.endElement();
        var result = '<note>\r\n  <!--Hi!-->\r\n</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetComment002
     * @tc.desc: Writes the comment.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetComment002', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setComment("Hello, World!");
        thatSer.endElement();
        var result = '<note>\r\n  <!--Hello, World!-->\r\n</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetComment003
     * @tc.desc: Writes the comment.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetComment003', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setComment("Hello, World!");
        var result = '<!--Hello, World!-->';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetComment004
     * @tc.desc: Writes the comment.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetComment004', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setComment("Hello, World!");
        var result = '<!--Hello, World!-->';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetComment005
     * @tc.desc: Writes the comment.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetComment005', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setComment("Hello, World!");
        thatSer.startElement("note");
        thatSer.endElement();
        var result = '<!--Hello, World!-->\r\n<note/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetDocType001
     * @tc.desc: Writes the DOCTYPE.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDocType001', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDocType('root SYSTEM');
        var result = '<!DOCTYPE root SYSTEM>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetDocType002
     * @tc.desc: Writes the DOCTYPE.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDocType002', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDocType('root SYSTEM "http://www.test.org/test.dtd"');
        var result = '<!DOCTYPE root SYSTEM "http://www.test.org/test.dtd">';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetDocType003
     * @tc.desc: Writes the DOCTYPE.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDocType003', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.endElement();
        thatSer.setDocType('root SYSTEM "http://www.test.org/test.dtd"');
        var result = '<note/>\r\n<!DOCTYPE root SYSTEM "http://www.test.org/test.dtd">';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetDocType004
     * @tc.desc: Writes the DOCTYPE.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDocType004', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setDocType('root SYSTEM "http://www.test.org/test.dtd"');
        thatSer.startElement("note");
        thatSer.endElement();
        var result = '<!DOCTYPE root SYSTEM "http://www.test.org/test.dtd">\r\n<note/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetDocType005
     * @tc.desc: Writes the DOCTYPE.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetDocType005', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setDocType('root SYSTEM "http://www.test.org/test.dtd"');
        thatSer.endElement();
        var result = '<note>\r\n  <!DOCTYPE root SYSTEM "http://www.test.org/test.dtd">\r\n</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetCDATA001
     * @tc.desc: Writes the CDATA.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetCDATA001', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setCDATA('root SYSTEM')
        var result = '<![CDATA[root SYSTEM]]>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetCDATA002
     * @tc.desc: Writes the CDATA.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetCDATA002', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setCDATA('root SYSTEM "http://www.test.org/test.dtd"')
        var result = '<![CDATA[root SYSTEM "http://www.test.org/test.dtd"]]>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetCDATA003
     * @tc.desc: Writes the CDATA.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetCDATA003', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.endElement();
        thatSer.setCDATA('root SYSTEM "http://www.test.org/test.dtd"')
        var result = '<note/>\r\n<![CDATA[root SYSTEM "http://www.test.org/test.dtd"]]>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetCDATA004
     * @tc.desc: Writes the CDATA.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetCDATA004', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.setCDATA('root SYSTEM "http://www.test.org/test.dtd"')
        thatSer.startElement("note");
        thatSer.endElement();
        var result = '<![CDATA[root SYSTEM "http://www.test.org/test.dtd"]]>\r\n<note/>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testSetCDATA005
     * @tc.desc: Writes the CDATA.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testSetCDATA005', 0, function () {
        const myMAX = 2048;
        var arrayBuffer = new ArrayBuffer(myMAX);
        var thatSer = new xml.XmlSerializer(arrayBuffer);
        thatSer.startElement("note");
        thatSer.setCDATA('root SYSTEM "http://www.test.org/test.dtd"')
        thatSer.endElement();
        var result = '<note>\r\n  <![CDATA[root SYSTEM "http://www.test.org/test.dtd"]]>\r\n</note>';
        var view = new Uint8Array(arrayBuffer);
        var view1 = "";
        for (var i = 0; i < result.length; ++i) {
            view1 = view1 + String.fromCodePoint(view[i]);
        }
        expect(view1).assertEqual(result);
    })

    /**
     * @tc.name: testParse001
     * @tc.desc: Starts parsing the XML file.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testParse001', 0, function () {
        var strXml =
            '<?xml version="1.0" encoding="utf-8"?>' +
            '<!DOCTYPE note [\n<!ENTITY foo "baa">]>' +
            '<note importance="high" logged="true">' +
            '    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>' +
            '    <!--Hello, World!-->' +
            '    <company>John &amp; Hans</company>' +
            '    <title>Happy</title>' +
            '    <title>Happy</title>' +
            '    <lens>Work</lens>' +
            '    <lens>Play</lens>' +
            '    <?go there?>' +
            '    <a><b/></a>' +
            '    <h:table xmlns:h="http://www.w3.org/TR/html4/">' +
            '        <h:tr>' +
            '            <h:td>Apples</h:td>' +
            '            <h:td>Bananas</h:td>' +
            '        </h:tr>' +
            '    </h:table>' +
            '</note>';
        var arrayBuffer = new ArrayBuffer(strXml.length*2);
        var bufView = new Uint8Array(arrayBuffer);
        var strLen = strXml.length;
        for (var i = 0; i < strLen; ++i) {
            bufView[i] = strXml.charCodeAt(i);
        }
        var that = new xml.XmlPullParser(arrayBuffer, 'UTF-8');
        var arr = {};
        var i = 0;
        function func3(key, info){
            arr[i] = 'key:' + JSON.stringify(key) + info.getDepth() + info.getColumnNumber() + info.getLineNumber() +
            info.getAttributeCount() + info.getName() + info.getNamespace() + info.getPrefix() + info.getText() +
            info.isEmptyElementTag()+ info.isWhitespace();
            i++;
            return true;
        }
        var options = {supportDoctype:true, ignoreNameSpace:false, tokenValueCallbackFunction:func3}
        that.parse(options);
        var str = '';
        for(var j = 0; j < i; ++j) {
            str = str + arr[j];
        }
        var str1 = 'key:00110falsetruekey:702220 note [\n<!ENTITY foo "baa">]falsefalsekey:216022notefalsefalsekey:' +
        '1016420     falsetruekey:51470\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\nfalsetruekey:101870     ' +
        'falsetruekey:612870Hello, World!falsetruekey:1013270     falsetruekey:224170companyfalsetruekey:424670 John ' +
        'falsefalsekey:925170amp;amp;falsetruekey:425670  Hansfalsefalsekey:326670companyfalsetruekey:1017070     ' +
        'falsetruekey:227770titlefalsetruekey:428270 Happyfalsefalsekey:329070titlefalsetruekey:1019470     ' +
        'falsetruekey:2210170titlefalsetruekey:4210670 Happyfalsefalsekey:3211470titlefalsetruekey:10111870     ' +
        'falsetruekey:2212470lensfalsetruekey:4212870 Workfalsefalsekey:3213570lensfalsetruekey:10113970     ' +
        'falsetruekey:2214570lensfalsetruekey:4214970 Playfalsefalsekey:3215670lensfalsetruekey:10116070     ' +
        'falsetruekey:8117270go therefalsetruekey:10117670     falsetruekey:2217970afalsetruekey:' +
        '2318370btruetruekey:3318370bfalsetruekey:3218770afalsetruekey:10119170     falsetruekey:2223870table ' +
        'http://www.w3.org/TR/html4/hfalsefalsekey:10224670         ' +
        'falsetruekey:2325270tr http://www.w3.org/TR/html4/hfalsetruekey:10326470             ' +
        'falsetruekey:2427070td http://www.w3.org/TR/html4/hfalsetruekey:4427670 Applesfalsefalsekey:3428370td ' +
        'http://www.w3.org/TR/html4/hfalsetruekey:10329570             ' +
        'falsetruekey:2430170td http://www.w3.org/TR/html4/hfalsetruekey:4430870 Bananasfalsefalsekey:3431570td ' +
        'http://www.w3.org/TR/html4/hfalsetruekey:10332370         falsetruekey:3333070tr ' +
        'http://www.w3.org/TR/html4/hfalsetruekey:10233470     falsetruekey:3234470table ' +
        'http://www.w3.org/TR/html4/hfalsetruekey:3135170notefalsetruekey:10351130falsetrue';
        expect(str).assertEqual(str1);
    })

    /**
     * @tc.name: testParse002
     * @tc.desc: Starts parsing the XML file.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testParse002', 0, function () {
        var strXml =
            '<?xml version="1.0" encoding="utf-8"?>' +
            '<!DOCTYPE note [\n<!ENTITY foo "baa">]>' +
            '<note importance="high" logged="true">' +
            '    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>' +
            '    <!--Hello, World!-->' +
            '    <company>John &amp; Hans</company>' +
            '    <title>Happy</title>' +
            '    <title>Happy</title>' +
            '    <lens>Work</lens>' +
            '    <lens>Play</lens>' +
            '    <?go there?>' +
            '    <a><b/></a>' +
            '    <h:table xmlns:h="http://www.w3.org/TR/html4/">' +
            '        <h:tr>' +
            '            <h:td>Apples</h:td>' +
            '            <h:td>Bananas</h:td>' +
            '        </h:tr>' +
            '    </h:table>' +
            '</note>';
        var arrayBuffer = new ArrayBuffer(strXml.length*2);
        var bufView = new Uint8Array(arrayBuffer);
        var strLen = strXml.length;
        for (var i = 0; i < strLen; ++i) {
            bufView[i] = strXml.charCodeAt(i);
        }
        var that = new xml.XmlPullParser(arrayBuffer);
        var arr = {};
        var i = 0;
        function func3(key, info){
            arr[i] = 'key:' + JSON.stringify(key) + info.getDepth() + info.getColumnNumber() + info.getLineNumber() +
            info.getAttributeCount() + info.getName() + info.getNamespace() + info.getPrefix() + info.getText() +
            info.isEmptyElementTag()+ info.isWhitespace();
            i++;
            return true;
        }
        var options = {supportDoctype:true, ignoreNameSpace:true, tokenValueCallbackFunction:func3}
        that.parse(options);
        var str = '';
        for(var j = 0; j < i; ++j) {
            str = str + arr[j];
        }
        var str1 = 'key:00110falsetruekey:702220 note [\n<!ENTITY foo "baa">]falsefalsekey:216022notefalsefalsekey:' +
        '1016420     falsetruekey:51470\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\nfalsetruekey:101870     ' +
        'falsetruekey:612870Hello, World!falsetruekey:1013270     falsetruekey:224170companyfalsetruekey:424670 ' +
        'John falsefalsekey:925170amp;amp;falsetruekey:425670  Hansfalsefalsekey:326670companyfalsetruekey:1017070' +
        '     falsetruekey:227770titlefalsetruekey:428270 Happyfalsefalsekey:329070titlefalsetruekey:1019470     ' +
        'falsetruekey:2210170titlefalsetruekey:4210670 Happyfalsefalsekey:3211470titlefalsetruekey:10111870     ' +
        'falsetruekey:2212470lensfalsetruekey:4212870 Workfalsefalsekey:3213570lensfalsetruekey:10113970     ' +
        'falsetruekey:2214570lensfalsetruekey:4214970 Playfalsefalsekey:3215670lensfalsetruekey:10116070     ' +
        'falsetruekey:8117270go therefalsetruekey:10117670     falsetruekey:2217970afalsetruekey:' +
        '2318370btruetruekey:3318370bfalsetruekey:3218770afalsetruekey:10119170     falsetruekey:' +
        '2223871h:tablefalsefalsekey:10224670         falsetruekey:2325270h:trfalsetruekey:10326470             ' +
        'falsetruekey:2427070h:tdfalsetruekey:4427670 Applesfalsefalsekey:3428370h:tdfalsetruekey:' +
        '10329570             falsetruekey:2430170h:tdfalsetruekey:4430870 Bananasfalsefalsekey:3431570h:' +
        'tdfalsetruekey:10332370         falsetruekey:3333070h:trfalsetruekey:10233470     falsetruekey:3234470h:' +
        'tablefalsetruekey:3135170notefalsetruekey:10351130falsetrue'
        expect(str).assertEqual(str1);
    })

    /**
     * @tc.name: testParse003
     * @tc.desc: Starts parsing the XML file.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testParse003', 0, function () {
        var strXml =
            '<?xml version="1.0" encoding="utf-8"?>' +
            '<!DOCTYPE note [\n<!ENTITY foo "baa">]>' +
            '<note importance="high" logged="true">' +
            '    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>' +
            '    <!--Hello, World!-->' +
            '    <company>John &amp; Hans</company>' +
            '    <title>Happy</title>' +
            '    <title>Happy</title>' +
            '    <lens>Work</lens>' +
            '    <lens>Play</lens>' +
            '    <?go there?>' +
            '    <a><b/></a>' +
            '    <h:table xmlns:h="http://www.w3.org/TR/html4/">' +
            '        <h:tr>' +
            '            <h:td>Apples</h:td>' +
            '            <h:td>Bananas</h:td>' +
            '        </h:tr>' +
            '    </h:table>' +
            '</note>';
        var arrayBuffer = new ArrayBuffer(strXml.length*2);
        var bufView = new Uint8Array(arrayBuffer);
        var strLen = strXml.length;
        for (var i = 0; i < strLen; ++i) {
            bufView[i] = strXml.charCodeAt(i);
        }
        var that = new xml.XmlPullParser(arrayBuffer, 'UTF-8');
        var arr = {};
        var i = 0;
        function func3(key, info){
            arr[i] = 'key:' + JSON.stringify(key) + info.getDepth() + info.getColumnNumber() + info.getLineNumber() +
            info.getAttributeCount() + info.getName() + info.getNamespace() + info.getPrefix() + info.getText() +
            info.isEmptyElementTag()+ info.isWhitespace();
            i++;
            return true;
        }
        var options = {supportDoctype:false, ignoreNameSpace:true, tokenValueCallbackFunction:func3}
        that.parse(options);
        var str = '';
        for(var j = 0; j < i; ++j) {
            str = str + arr[j];
        }
        var str1 = 'key:00110falsetruekey:216022notefalsefalsekey:1016420     falsetruekey:51470\r\nfuncrion ' +
        'matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\nfalsetruekey:101870     falsetruekey:612870Hello, World!falsetruekey' +
        ':1013270     falsetruekey:224170companyfalsetruekey:424670 John falsefalsekey:925170amp;amp;falsetruekey' +
        ':425670  Hansfalsefalsekey:326670companyfalsetruekey:1017070     falsetruekey:227770titlefalsetruekey:428' +
        '270 Happyfalsefalsekey:329070titlefalsetruekey:1019470     falsetruekey:2210170titlefalsetruekey:4210670 ' +
        'Happyfalsefalsekey:3211470titlefalsetruekey:10111870     falsetruekey:2212470lensfalsetruekey:4212870 ' +
        'Workfalsefalsekey:3213570lensfalsetruekey:10113970     falsetruekey:2214570lensfalsetruekey:4214970 ' +
        'Playfalsefalsekey:3215670lensfalsetruekey:10116070     falsetruekey:8117270go therefalsetruekey' +
        ':10117670     falsetruekey:2217970afalsetruekey:2318370btruetruekey:3318370bfalsetruekey' +
        ':3218770afalsetruekey:10119170     falsetruekey:2223871h:tablefalsefalsekey:10224670         ' +
        'falsetruekey:2325270h:trfalsetruekey:10326470             falsetruekey:' +
        '2427070h:tdfalsetruekey:4427670 Applesfalsefalsekey:3428370h:tdfalsetruekey:10329570             ' +
        'falsetruekey:2430170h:tdfalsetruekey:4430870 Bananasfalsefalsekey:3431570h:tdfalsetruekey:10332370        ' +
        ' falsetruekey:3333070h:trfalsetruekey:10233470     falsetruekey:3234470h:tablefalsetruekey:' +
        '3135170notefalsetruekey:10351130falsetrue'
        expect(str).assertEqual(str1);

    })

    /**
     * @tc.name: testParse004
     * @tc.desc: Starts parsing the XML file.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testParse004', 0, function () {
        var strXml =
            '<?xml version="1.0" encoding="utf-8"?>' +
            '<!DOCTYPE note [\n<!ENTITY foo "baa">]>' +
            '<note importance="high" logged="true">' +
            '    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>' +
            '    <!--Hello, World!-->' +
            '    <company>John &amp; Hans</company>' +
            '    <title>Happy</title>' +
            '    <title>Happy</title>' +
            '    <lens>Work</lens>' +
            '    <lens>Play</lens>' +
            '    <?go there?>' +
            '    <a><b/></a>' +
            '    <h:table xmlns:h="http://www.w3.org/TR/html4/">' +
            '        <h:tr>' +
            '            <h:td>Apples</h:td>' +
            '            <h:td>Bananas</h:td>' +
            '        </h:tr>' +
            '    </h:table>' +
            '</note>';
        var arrayBuffer = new ArrayBuffer(strXml.length*2);
        var bufView = new Uint8Array(arrayBuffer);
        var strLen = strXml.length;
        for (var i = 0; i < strLen; ++i) {
            bufView[i] = strXml.charCodeAt(i);
        }
        var that = new xml.XmlPullParser(arrayBuffer, 'UTF-8');
        var str = '';
        function func2(name, value){
            str += name+':'+value;
            return true;
        }
        var options = {supportDoctype:true, ignoreNameSpace:true, attributeValueCallbackFunction:func2}
        that.parse(options);
        var str1= 'importance: highxmlns:h: http://www.w3.org/TR/html4/'
        expect(str).assertEqual(str1);
    })

    /**
     * @tc.name: testParse005
     * @tc.desc: Starts parsing the XML file.
     * @tc.require: AR000GGRB8
     * @tc.author: lihucheng
     */
    it('testParse005', 0, function () {
        var strXml =
            '<?xml version="1.0" encoding="utf-8"?>' +
            '<!DOCTYPE note [\n<!ENTITY foo "baa">]>' +
            '<note importance="high" logged="true">' +
            '    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>' +
            '    <!--Hello, World!-->' +
            '    <company>John &amp; Hans</company>' +
            '    <title>Happy</title>' +
            '    <title>Happy</title>' +
            '    <lens>Work</lens>' +
            '    <lens>Play</lens>' +
            '    <?go there?>' +
            '    <a><b/></a>' +
            '    <h:table xmlns:h="http://www.w3.org/TR/html4/">' +
            '        <h:tr>' +
            '            <h:td>Apples</h:td>' +
            '            <h:td>Bananas</h:td>' +
            '        </h:tr>' +
            '    </h:table>' +
            '</note>';
        var arrayBuffer = new ArrayBuffer(strXml.length*2);
        var bufView = new Uint8Array(arrayBuffer);
        var strLen = strXml.length;
        for (var i = 0; i < strLen; ++i) {
            bufView[i] = strXml.charCodeAt(i);
        }
        var that = new xml.XmlPullParser(arrayBuffer, 'UTF-8');
        var str1 = '';
        function func1(name, value){
            str1 += name+':'+value;
            return true;
        }
        var options = {supportDoctype:true, ignoreNameSpace:true, tagValueCallbackFunction:func1}
        that.parse(options);
        var str3 = 'note:company:title:title:lens:lens:a:b:h:table:h:tr:h:td:h:td:'
        expect(str1).assertEqual(str3)
    })
})