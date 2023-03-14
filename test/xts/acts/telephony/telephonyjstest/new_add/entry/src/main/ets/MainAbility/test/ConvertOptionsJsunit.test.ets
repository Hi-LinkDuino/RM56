// @ts-nocheck
/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
import  ConvertXML from '@ohos.convertxml'
export default function convertOptionsJsunit() {
  describe('XmlTest',function () {
    console.log("************* settings Test start*************");
    it('Telephony_convertxml_ConvertXML_0100', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>  Happy  </title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false,trim :true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_0200', 0,  async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreDeclaration: true});
      var str1 = '{"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_0300', 0,async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreDeclaration: true});
      var str1 = '{"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_0400', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreAttributes: true});
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_0500', 0,async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <!--note-->'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreComment: true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_0600', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <![CDATA[<foo></bar>]]>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreCDATA: true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_0700', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<!DOCTYPE foo>'+
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreDoctype: true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_0800', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreText: true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title"},'+
      '{"_type":"element",'+
      '"_name":"todo"},'+
      '{"_type":"element",'+
      '"_name":"todo"}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_0900', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, declarationKey: "123"})
      var str1 = '{"123":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1000', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <?go there?>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, instructionKey: "123"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"123":"there",'+
      '"_type":"instruction",'+
      '"_name":"go"},'+
      '{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1100', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <?go there?>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, attributesKey: "123"})
      var str1 = '{"_declaration":{"123":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"123":{"importance":"high",'+
      '"logged":"true"},'+
      '"_type":"element",'+
      '"_name":"note",'+
      '"_elements":[{"_type":"instruction",'+
      '"_name":"go",'+
      '"_instruction":"there"},'+
      '{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1200', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, textKey: "123"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"123":"Happy",'+
      '"_type":"text"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"123":"Work",'+
      '"_type":"text"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"123":"Play",'+
      '"_type":"text"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1300', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <![CDATA[1 is < 2]]>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, cdataKey: "123"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"123":"1 is < 2",'+
      '"_type":"cdata"},'+
      '{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1400', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<!DOCTYPE foo>'+
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, doctypeKey: 'doctype'})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"doctype","doctype":"foo"},'+
      '{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1500', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<!--note-->'+
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, commentKey: "123"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"123":"note",'+
      '"_type":"comment"},'+
      '{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1600', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <a><b/></a>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, parentKey: "123"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"a",'+
      '"_elements":[{"_type":"element",'+
      '"_name":"b"}]},'+
      '{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1700', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, typeKey: "123"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"123":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"123":"element",'+
      '"_name":"title",'+
      '"_elements":[{"123":"text",'+
      '"_text":"Happy"}]},'+
      '{"123":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"123":"text",'+
      '"_text":"Work"}]},'+
      '{"123":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"123":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1800', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, nameKey: "123"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"123":"note",'+
      '"_type":"element",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"123":"title",'+
      '"_type":"element",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"123":"todo",'+
      '"_type":"element",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"123":"todo",'+
      '"_type":"element",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_1900', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, elementsKey: "123"})
      var str1 = '{"123":[{"123":[{"123":[{"_type":"text",'+
      '"_text":"Happy"}],'+
      '"_type":"element",'+
      '"_name":"title"},'+
      '{"123":[{"_type":"text",'+
      '"_text":"Work"}],'+
      '"_type":"element",'+
      '"_name":"todo"},'+
      '{"123":[{"_type":"text",'+
      '"_text":"Play"}],'+
      '"_type":"element",'+
      '"_name":"todo"}],'+
      '"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"}}],'+
      '"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}}}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2000', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>  Happy  </title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false,trim :true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2100', 0,  async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreDeclaration: true});
      var str1 = '{"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2200', 0,async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreDeclaration: true});
      var str1 = '{"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2300', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreAttributes: true});
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2400', 0,async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <!--note-->'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreComment: true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2500', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <![CDATA[<foo></bar>]]>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreCDATA: true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2600', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<!DOCTYPE foo>'+
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreDoctype: true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2700', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreText: true})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title"},'+
      '{"_type":"element",'+
      '"_name":"todo"},'+
      '{"_type":"element",'+
      '"_name":"todo"}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2800', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, declarationKey: "456"})
      var str1 = '{"456":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_2900', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <?go there?>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, instructionKey: "456"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"456":"there",'+
      '"_type":"instruction",'+
      '"_name":"go"},'+
      '{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_3000', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <?go there?>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, attributesKey: "456"})
      var str1 = '{"_declaration":{"456":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"456":{"importance":"high",'+
      '"logged":"true"},'+
      '"_type":"element",'+
      '"_name":"note",'+
      '"_elements":[{"_type":"instruction",'+
      '"_name":"go",'+
      '"_instruction":"there"},'+
      '{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_3100', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, textKey: "456"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"456":"Happy",'+
      '"_type":"text"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"456":"Work",'+
      '"_type":"text"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"456":"Play",'+
      '"_type":"text"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_3200', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <![CDATA[1 is < 2]]>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, cdataKey: "456"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"456":"1 is < 2",'+
      '"_type":"cdata"},'+
      '{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_3300', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<!DOCTYPE foo>'+
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, doctypeKey: 'doctype_next'})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"doctype","doctype_next":"foo"},'+
      '{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_3400', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<!--note-->'+
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, commentKey: "456"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"456":"note",'+
      '"_type":"comment"},'+
      '{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_3500', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <a><b/></a>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, parentKey: "456"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"a",'+
      '"_elements":[{"_type":"element",'+
      '"_name":"b"}]},'+
      '{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_3600', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, typeKey: "123"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"123":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"123":"element",'+
      '"_name":"title",'+
      '"_elements":[{"123":"text",'+
      '"_text":"Happy"}]},'+
      '{"123":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"123":"text",'+
      '"_text":"Work"}]},'+
      '{"123":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"123":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_3700', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, nameKey: "123"})
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"123":"note",'+
      '"_type":"element",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":[{"123":"title",'+
      '"_type":"element",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"123":"todo",'+
      '"_type":"element",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"123":"todo",'+
      '"_type":"element",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
      done();
    })

    it('Telephony_convertxml_ConvertXML_3800', 0, async function (done) {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, elementsKey: "123"})
      var str1 = '{"123":[{"123":[{"123":[{"_type":"text",'+
      '"_text":"Happy"}],'+
      '"_type":"element",'+
      '"_name":"title"},'+
      '{"123":[{"_type":"text",'+
      '"_text":"Work"}],'+
      '"_type":"element",'+
      '"_name":"todo"},'+
      '{"123":[{"_type":"text",'+
      '"_text":"Play"}],'+
      '"_type":"element",'+
      '"_name":"todo"}],'+
      '"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"}}],'+
      '"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}}}'
      expect(result1).assertEqual(str1);
      done();
    })

    /**
     * @tc.name: Telephony_convertxml_ConvertXML_3900
     * @tc.desc: To convert XML text to JavaScript object.
     * @tc.require: AR000GFB5B
     */
    it('Telephony_convertxml_ConvertXML_3900', 0, function () {
      var xml =
        '<?xml version="1.0" encoding="utf-8"?>' +
        '<note importance="high" logged="true">' +
        '    <?go there?>'+
        '    <title>Happy</title>' +
        '    <todo>Work</todo>' +
        '    <todo>Play</todo>' +
        '</note>';
      var convertml = new ConvertXML();
      var result1 = convertml.convert(xml, {compact: false, ignoreInstruction: true});
      var str1 = '{"_declaration":{"_attributes":{"version":"1.0",'+
      '"encoding":"utf-8"}},'+
      '"_elements":[{"_type":"element",'+
      '"_name":"note",'+
      '"_attributes":{"importance":"high",'+
      '"logged":"true"},'+
      '"_elements":['+
      '{"_type":"element",'+
      '"_name":"title",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Happy"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Work"}]},'+
      '{"_type":"element",'+
      '"_name":"todo",'+
      '"_elements":[{"_type":"text",'+
      '"_text":"Play"}]}]}]}'
      expect(result1).assertEqual(str1);
    })
  })
}