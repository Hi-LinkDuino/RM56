/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License')
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
import  URI from '@ohos.uri'
describe('UriTest', function () {

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_012
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_012', 0, function () {
        try {
            let that = new URI('#http://username:password@host:8080/directory/file?foo=1&bar=2');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: #It can't be the first");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_010
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_010', 0, function () {
        try {
            let that = new URI({name: 'gaogao'});
        } catch (err) {
            expect(err.toString()).assertEqual("Error: input type err");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_032
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_032', 0, function () {
        try {
            let that = new URI('ht/tp://username:pas sword@host:8080/directory/file?foo=1&bar=2');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: SpecialPath does not conform to the rule");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_030
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_030', 0, function () {
        try {
            let that = new URI('http://username:password@[::]:8080/directory/file?Query#gaogao faofao');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: Fragment does not conform to the rule");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_028
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_028', 0, function () {
        try {
            let that = new URI('http://username:password@host:8080/directory/file?foo^=1&bar=2#gaogaofaofao');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: Query does not conform to the rule");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_013
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_013', 0, function () {
        try {
            let that = new URI('1http://username:password@host:8080/directory/file?foo=1&bar=2#gaogaofaofao');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: Scheme the first character must be a letter");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_014
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_014', 0, function () {
        try {
            let that = new URI('ht@tp://username:password@host:8080/directory/file?foo=1&bar=2#gaogaofaofao');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: scheme does not conform to the rule");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_026
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_026', 0, function () {
        try {
            let that = new URI('http://username:password@[::]:80r80/directory/file?foo=1&bar=2#gaogaofaofao');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: Prot does not conform to the rule");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_021
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_021', 0, function () {
        try {
            let that = new URI('http://username:password@[::12:55:8080/directory/file?foo=1&bar=2#gaogaofaofao');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: IPv6 is missing a closing bracket");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_018
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_018', 0, function () {
        try {
            let that = new URI('http://username:pa^ssword@[::12:55]:8080/directory/file?foo=1&bar=2#gaogaofaofao');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: userInfo does not conform to the rule");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_020
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_020', 0, function () {
        try {
            let that = new URI('http://username:password@[::1你2:55]:8080/directory/file?foo=1&bar=2#gaogaofaofao');
        } catch (err) {
            expect(err.toString()).assertEqual("Error: ipv6 does not conform to the rule");
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_001
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_001', 0, function () {
        let gaogao = new URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@www.baidu.com:99");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@www.baidu.com:99/path/path?query");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("www.baidu.com");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/path");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: testConstructor013
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor013', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1080::8:800:200C:417A]:99/path/66path1?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@[1080::8:800:200C:417A]:99");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@[1080::8:800:200C:417A]:99/path/66path1?query");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("[1080::8:800:200C:417A]");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/66path1");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_002
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_002', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[::]:88/path/path66?foooo#gaogao');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@[::]:88");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@[::]:88/path/path66?foooo");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("[::]");
        expect(gaogao.port).assertEqual("88");
        expect(gaogao.path).assertEqual("/path/path66");
        expect(gaogao.query).assertEqual("foooo");
        expect(gaogao.fragment).assertEqual("gaogao");
    })

    /**
     * @tc.name: testConstructor015
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor015', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path/66path1?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@[1:0:0:1:2:1:2:1]:99");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@[1:0:0:1:2:1:2:1]:99/path/66path1?query");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("[1:0:0:1:2:1:2:1]");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/66path1");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_003
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_003', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[::FFFF:129.144.52.38]:99/path/path?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@[::FFFF:129.144.52.38]:99");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@[::FFFF:129.144.52.38]:99/path/path?query");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("[::FFFF:129.144.52.38]");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/path");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: testConstructor017
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor017', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[::192.9.5.5]:99/path/path?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@[::192.9.5.5]:99");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@[::192.9.5.5]:99/path/path?query");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("[::192.9.5.5]");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/path");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: testConstructor018
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor018', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[22::22:2:2%ss]:99/path/path?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@[22::22:2:2%ss]:99");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@[22::22:2:2%ss]:99/path/path?query");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("[22::22:2:2%ss]");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/path");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: testConstructor019
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor019', 0, function () {
        let gaogao =
		new URI('http://gg:gaogao@[fe80:0000:0001:0000:0440:44ff:1233:5678]:99/path/path?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@[fe80:0000:0001:0000:0440:44ff:1233:5678]:99");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@[fe80:0000:0001:0000:0440:44ff:1233:5678]:99/path/path?query");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("[fe80:0000:0001:0000:0440:44ff:1233:5678]");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/path");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: testConstructor020
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor020', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[fe80::0001:0000]:99/path/path?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@[fe80::0001:0000]:99");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@[fe80::0001:0000]:99/path/path?query");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("[fe80::0001:0000]");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/path");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_004
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_004', 0, function () {
        let gaogao = new URI('http://gg:gaogao@199.98.55.44:99/path/path?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("gg:gaogao@199.98.55.44:99");
        expect(gaogao.ssp).assertEqual("//gg:gaogao@199.98.55.44:99/path/path?query");
        expect(gaogao.userinfo).assertEqual("gg:gaogao");
        expect(gaogao.host).assertEqual("199.98.55.44");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/path");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_005
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_005', 0, function () {
        let gaogao = new URI('http://16.9.5.4:99/path/path?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("16.9.5.4:99");
        expect(gaogao.ssp).assertEqual("//16.9.5.4:99/path/path?query");
        expect(gaogao.userinfo).assertEqual("null");
        expect(gaogao.host).assertEqual("16.9.5.4");
        expect(gaogao.port).assertEqual("99");
        expect(gaogao.path).assertEqual("/path/path");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: testConstructor023
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor023', 0, function () {
        let gaogao = new URI('http://49.99.54.12:50/path/path23?query#fagment');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("49.99.54.12:50");
        expect(gaogao.ssp).assertEqual("//49.99.54.12:50/path/path23?query");
        expect(gaogao.userinfo).assertEqual("null");
        expect(gaogao.host).assertEqual("49.99.54.12");
        expect(gaogao.port).assertEqual("50");
        expect(gaogao.path).assertEqual("/path/path23");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("fagment");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_006
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_006', 0, function () {
        let gaogao = new URI('http://user@49.10pe8.54.12:80/path/path23?query#qwer');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("user@49.10pe8.54.12:80");
        expect(gaogao.ssp).assertEqual("//user@49.10pe8.54.12:80/path/path23?query");
        expect(gaogao.userinfo).assertEqual("null");
        expect(gaogao.host).assertEqual("null");
        expect(gaogao.port).assertEqual("-1");
        expect(gaogao.path).assertEqual("/path/path23");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("qwer");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_007
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_007', 0, function () {
        let gaogao = new URI('http://user@www.baidu.com/path/path23?query#qwer');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("user@www.baidu.com");
        expect(gaogao.ssp).assertEqual("//user@www.baidu.com/path/path23?query");
        expect(gaogao.userinfo).assertEqual("user");
        expect(gaogao.host).assertEqual("www.baidu.com");
        expect(gaogao.port).assertEqual("-1");
        expect(gaogao.path).assertEqual("/path/path23");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("qwer");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_008
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_008', 0, function () {
        let gaogao = new URI('http://user@www.hw.com:77/path/path23?query#qwer');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("user@www.hw.com:77");
        expect(gaogao.ssp).assertEqual("//user@www.hw.com:77/path/path23?query");
        expect(gaogao.userinfo).assertEqual("user");
        expect(gaogao.host).assertEqual("www.hw.com");
        expect(gaogao.port).assertEqual("77");
        expect(gaogao.path).assertEqual("/path/path23");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("qwer");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_009
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_009', 0, function () {
        let gaogao = new URI('ht2tp://user@www.h12343w.com:77/path/path23?query#qwer');
        expect(gaogao.scheme).assertEqual("ht2tp");
        expect(gaogao.authority).assertEqual("user@www.h12343w.com:77");
        expect(gaogao.ssp).assertEqual("//user@www.h12343w.com:77/path/path23?query");
        expect(gaogao.userinfo).assertEqual("user");
        expect(gaogao.host).assertEqual("www.h12343w.com");
        expect(gaogao.port).assertEqual("77");
        expect(gaogao.path).assertEqual("/path/path23");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("qwer");
    })

    /**
     * @tc.name: testConstructor028
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor028', 0, function () {
        let gaogao = new URI('ht2tp://user@www.1hw.1com:77/path/path23?query#qwer');
        expect(gaogao.scheme).assertEqual("ht2tp");
        expect(gaogao.authority).assertEqual("user@www.1hw.1com:77");
        expect(gaogao.ssp).assertEqual("//user@www.1hw.1com:77/path/path23?query");
        expect(gaogao.userinfo).assertEqual("null");
        expect(gaogao.host).assertEqual("null");
        expect(gaogao.port).assertEqual("-1");
        expect(gaogao.path).assertEqual("/path/path23");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("qwer");
    })

    /**
     * @tc.name: testConstructor029
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor029', 0, function () {
        let gaogao = new URI('http://user@hosthost/path/path23?query#qwer');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("user@hosthost");
        expect(gaogao.ssp).assertEqual("//user@hosthost/path/path23?query");
        expect(gaogao.userinfo).assertEqual("user");
        expect(gaogao.host).assertEqual("hosthost");
        expect(gaogao.port).assertEqual("-1");
        expect(gaogao.path).assertEqual("/path/path23");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("qwer");
    })

    /**
     * @tc.name: testConstructor030
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor030', 0, function () {
        let gaogao = new URI('http://user@[::]/path/path23?query#qwer');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("user@[::]");
        expect(gaogao.ssp).assertEqual("//user@[::]/path/path23?query");
        expect(gaogao.userinfo).assertEqual("user");
        expect(gaogao.host).assertEqual("[::]");
        expect(gaogao.port).assertEqual("-1");
        expect(gaogao.path).assertEqual("/path/path23");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("qwer");
    })

    /**
     * @tc.name: testConstructor031
     * @tc.desc: Constructs a URI by parsing the given string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testConstructor031', 0, function () {
        let gaogao = new URI('http://[::192:0:5]/path/path23?query#qwer');
        expect(gaogao.scheme).assertEqual("http");
        expect(gaogao.authority).assertEqual("[::192:0:5]");
        expect(gaogao.ssp).assertEqual("//[::192:0:5]/path/path23?query");
        expect(gaogao.userinfo).assertEqual("null");
        expect(gaogao.host).assertEqual("[::192:0:5]");
        expect(gaogao.port).assertEqual("-1");
        expect(gaogao.path).assertEqual("/path/path23");
        expect(gaogao.query).assertEqual("query");
        expect(gaogao.fragment).assertEqual("qwer");
    })

    /**
     * @tc.name: testEquals001
     * @tc.desc: Tests this URI for equality with another object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testEquals001', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment');
        let gaogao1 = gaogao;
        let res = gaogao.equals(gaogao1);
        expect(res).assertEqual(true);
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_034
     * @tc.desc: Tests this URI for equality with another object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_034', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment');
        let gaogao1 = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment');
        let res = gaogao.equals(gaogao1);
        expect(res).assertEqual(true);
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_035
     * @tc.desc: Tests this URI for equality with another object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_035', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment');
        let gaogao1 = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment123');
        let res = gaogao.equals(gaogao1);
        expect(res).assertEqual(false);
    })

    /**
     * @tc.name: testEquals004
     * @tc.desc: Tests this URI for equality with another object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testEquals004', 0, function () {
        let gaogao = new URI('http://gg:gaogao@wwwaaa:99/path1?query#fagment');
        let gaogao1 = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment123');
        let res = gaogao.equals(gaogao1);
        expect(res).assertEqual(false);
    })

    /**
     * @tc.name: testEquals005
     * @tc.desc: Tests this URI for equality with another object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testEquals005', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment');
        let gaogao1 = new URI('http://[1:0:0:1:2:1:2:1]/path1?query#fagment123');
        let res = gaogao.equals(gaogao1);
        expect(res).assertEqual(false);
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_036
     * @tc.desc: Normalizes this URI's path.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_036', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path/66./../././mm/.././path1?query#fagment');
        let res = gaogao.normalize();
        expect(res.path).assertEqual("/path/path1");
        expect(res.toString()).assertEqual("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path/path1?query#fagment");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_037
     * @tc.desc: Normalizes this URI's path.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_037', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path?query#fagment');
        let res = gaogao.normalize();
        expect(res.path).assertEqual("/../../path");
        expect(res.toString()).assertEqual("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path?query#fagment");
    })

    /**
     * @tc.name: testNormalize003
     * @tc.desc: Normalizes this URI's path.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testNormalize003', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path/.././../aa/bb/cc?query#fagment');
        let res = gaogao.normalize();
        expect(res.path).assertEqual("/../../../aa/bb/cc");
        expect(res.toString()).assertEqual("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../../aa/bb/cc?query#fagment");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_038
     * @tc.desc: Normalizes this URI's path.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_038', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../path/.././../aa/bb/cc?query');
        let res = gaogao.normalize();
        expect(res.path).assertEqual("/../../aa/bb/cc");
        expect(res.toString()).assertEqual("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../aa/bb/cc?query");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_039
     * @tc.desc: Normalizes this URI's path.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_039', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/./path/./aa/bb/cc?query#fagment');
        let res = gaogao.normalize();
        expect(res.path).assertEqual("/path/aa/bb/cc");
        expect(res.toString()).assertEqual("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path/aa/bb/cc?query#fagment");
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_042
     * @tc.desc: Returns the content of this URI as a US-ASCII string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_042', 0, function () {
        let gaogao = new URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path/.././../aa/bb/cc?query#fagment');
        let res = gaogao.toString();
        expect(res).assertEqual('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path/.././../aa/bb/cc?query#fagment');
    })

    /**
     * @tc.name: testToString002
     * @tc.desc: Returns the content of this URI as a US-ASCII string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testToString002', 0, function () {
        let gaogao = new URI('htt1p://gg:gaogao@[::192.9.5.5]:99/path/66path1?query#fagment');
        let res = gaogao.toString();
        expect(res).assertEqual('htt1p://gg:gaogao@[::192.9.5.5]:99/path/66path1?query#fagment');
    })

    /**
     * @tc.name: testToString003
     * @tc.desc: Returns the content of this URI as a US-ASCII string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testToString003', 0, function () {
        let gaogao = new URI('ftp://username:password@www.baidu.com:88/path?query#fagment');
        let res = gaogao.toString();
        expect(res).assertEqual('ftp://username:password@www.baidu.com:88/path?query#fagment');
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_043
     * @tc.desc: Returns the content of this URI as a US-ASCII string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_043', 0, function () {
        let gaogao = new URI('http://gg:gao你好gao@199.98.55.44:99/path/p你好ath?qu你好ery#fag你好ment');
        let res = gaogao.toString();
        expect(res).assertEqual('http://gg:gao%E4%BD%A0%E5%A5%BDgao@199.98.55.44:99/path/' +
        'p%E4%BD%A0%E5%A5%BDath?qu%E4%BD%A0%E5%A5%BDery#fag%E4%BD%A0%E5%A5%BDment');
    })

    /**
     * @tc.name: testToString005
     * @tc.desc: Returns the content of this URI as a US-ASCII string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testToString005', 0, function () {
        let gaogao = new URI('http://gg:gaogao@199.98.55.44:99/path/path?query#fagment');
        let res = gaogao.toString();
        expect(res).assertEqual('http://gg:gaogao@199.98.55.44:99/path/path?query#fagment');
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_041
     * @tc.desc: Tells whether or not this URI is absolute.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_041', 0, function () {
        let gaogao = new URI('f/tp://username:password@www.baidu.com:88/path?query#fagment');
        let res = gaogao.checkIsAbsolute();
        expect(res).assertEqual(false);
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR34_040
     * @tc.desc: Tells whether or not this URI is absolute.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('SUB_Runtime_JSAPI_SR000GGR34_040', 0, function () {
        let gaogao = new URI('ftp://username:password@www.baidu.com:88/path?query#fagment');
        let res = gaogao.checkIsAbsolute();
        expect(res).assertEqual(true);
    })

    /**
     * @tc.name: testCheckIsAbsolute003
     * @tc.desc: Tells whether or not this URI is absolute.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testCheckIsAbsolute003', 0, function () {
        let gaogao = new URI('htt/p://username:password@www.baidu.com:88/path?query#fagment');
        let res = gaogao.checkIsAbsolute();
        expect(res).assertEqual(false);
    })

    /**
     * @tc.name: testCheckIsAbsolute004
     * @tc.desc: Tells whether or not this URI is absolute.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testCheckIsAbsolute004', 0, function () {
        let gaogao = new URI('https://username:password@www.baidu.com:88/path?query#fagment');
        let res = gaogao.checkIsAbsolute();
        expect(res).assertEqual(true);
    })

    /**
     * @tc.name: testCheckIsAbsolute005
     * @tc.desc: Tells whether or not this URI is absolute.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testCheckIsAbsolute005', 0, function () {
        let gaogao = new URI('http1://username:password@www.baidu.com:88/path?query#fagment');
        let res = gaogao.checkIsAbsolute();
        expect(res).assertEqual(true);
    })
})