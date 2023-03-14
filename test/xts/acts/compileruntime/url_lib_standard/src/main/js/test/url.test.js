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
import  Url from '@ohos.url'
describe('UrlFunTest', function () {

    /**
     * @tc.name: testUrlAppend001
     * @tc.desc: Appends a specified key/value pair as a new search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlAppend001', 0, function () {
        var that = new Url.URL('http://username:password@host:8080/directory/file?foo=1&bar=2');
        var params = new Url.URLSearchParams(that.search);
        params.append('ma','jk')
        var result = params.toString()
        expect(result).assertEqual('foo=1&bar=2&ma=jk')
    })

    /**
     * @tc.name: testUrlAppend002
     * @tc.desc: Appends a specified key/value pair as a new search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlAppend002', 0, function () {
        let that = new Url.URL('https://example.com?foo=1&bar=2')
        let params = new Url.URLSearchParams(that.search)
        params.append("ma 大","jk￥")
        var result = params.toString()
        expect(result).assertEqual("foo=1&bar=2&ma+%E5%A4%A7=jk%EF%BF%A5")
    })

    /**
     * @tc.name: testUrlAppend003
     * @tc.desc: Appends a specified key/value pair as a new search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlAppend003', 0, function () {
        let that = new Url.URL('https://example.com?foo=1&bar=2')
        let params = new Url.URLSearchParams(that.search)
        params.append("foo~!@#$%^&*()_+-=","jk")
        var result = params.toString()
        expect(result).assertEqual("foo=1&bar=2&foo%7E%21%40%23%24%25%5E%26*%28%29_%2B-%3D=jk")
    })

    /**
     * @tc.name: testUrlAppend004
     * @tc.desc: Appends a specified key/value pair as a new search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlAppend004', 0, function () {
        let that = new Url.URL('https://example.com?foo=1&bar=2')
        let params = new Url.URLSearchParams(that.search)
        params.append("app","par")
        var result = params.toString()
        expect(result).assertEqual("foo=1&bar=2&app=par")
    })

    /**
     * @tc.name: testUrlAppend005
     * @tc.desc: Appends a specified key/value pair as a new search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlAppend005', 0, function () {
        let that = new Url.URL('https://example.com?foo=1&bar=2')
        let params = new Url.URLSearchParams(that.search)
        params.append("123","456")
        var result = params.toString()
        expect(result).assertEqual("foo=1&bar=2&123=456")
    })

    /**
     * @tc.name: testUrlDelete001
     * @tc.desc: Deletes the given search parameter and its associated value,from the list of all search parameters.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlDelete001', 0, function () {
        let that = new Url.URL('https://example.com?foo=1&bar=2')
        let params = new Url.URLSearchParams(that.search)
        params.delete("foo")
        var result = params.toString()
        expect(result).assertEqual("bar=2")
    })

    /**
     * @tc.name: testUrlDelete002
     * @tc.desc: Deletes the given search parameter and its associated value,from the list of all search parameters.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlDelete002', 0, function () {
        let that = new Url.URL('https://example.com?foo大=1&bar=2');
        let params = new Url.URLSearchParams(that.search);
        params.delete('foo');
        var result = params.toString();
        expect(result).assertEqual("foo%E5%A4%A7=1&bar=2");
    })

    /**
     * @tc.name: testUrlDelete003
     * @tc.desc: Deletes the given search parameter and its associated value,from the list of all search parameters.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlDelete003', 0, function () {
        let that = new Url.URL('https://example.com?foo大=1&bar=2');
        let params = new Url.URLSearchParams(that.search);
        params.delete("foo大");
        var result = params.toString();
        expect(result).assertEqual("bar=2")
    })

    /**
     * @tc.name: testUrlDelete004
     * @tc.desc: Deletes the given search parameter and its associated value,from the list of all search parameters.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlDelete004', 0, function () {
        let that = new Url.URL('https://example.com?foo=1&bar=2');
        let params = new Url.URLSearchParams(that.search);
        params.delete('bar');
        var result = params.toString();
        expect(result).assertEqual("foo=1");
    })

    /**
     * @tc.name: testUrlDelete005
     * @tc.desc: Deletes the given search parameter and its associated value,from the list of all search parameters.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlDelete005', 0, function () {
        let that = new Url.URL('https://example.com?foo=1&bar=2');
        let params = new Url.URLSearchParams(that.search);
        params.delete("faa");
        var result = params.toString();
        expect(result).assertEqual("foo=1&bar=2")
    })

    /**
     * @tc.name: testUrlEntries001
     * @tc.desc: Returns an ES6 iterator. Each item of the iterator is a JavaScript Array.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlEntries001', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2");

        var i=0;
        var arr={};
        for(var pair of params.entries()) {
            arr[i]=pair[0];
            i++;
            arr[i]=pair[1];
            i++;
        }
        expect(arr[1]).assertEqual("value1");
    })

    /**
     * @tc.name: testUrlEntries002
     * @tc.desc: Returns an ES6 iterator. Each item of the iterator is a JavaScript Array.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlEntries002', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        var i=0;
        var arr={};
        for(var pair of params.entries()) {
            arr[i]=pair[0];
            i++;
            arr[i]=pair[1];
            i++;
        }
        expect(arr[2]).assertEqual("key2")
    })

    /**
     * @tc.name: testUrlEntries003
     * @tc.desc: Returns an ES6 iterator. Each item of the iterator is a JavaScript Array.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlEntries003', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        params.append("foo","jk")
        var i=0;
        var arr={};
        for(var pair of params.entries()) {
            arr[i]=pair[0];
            i++;
            arr[i]=pair[1];
            i++;
        }
        expect(arr[5]).assertEqual("jk")
    })

    /**
     * @tc.name: testUrlEntries004
     * @tc.desc: Returns an ES6 iterator. Each item of the iterator is a JavaScript Array.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlEntries004', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        var i=0;
        var arr={};
        for(var pair of params.entries()) {
            arr[i]=pair[0];
            i++;
            arr[i]=pair[1];
            i++;
        }
        expect(arr[3]).assertEqual("value2")
    })

    /**
     * @tc.name: testUrlEntries005
     * @tc.desc: Returns an ES6 iterator. Each item of the iterator is a JavaScript Array.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlEntries005', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        params.append("jss","txt")
        var i=0;
        var arr={};
        for(var pair of params.entries()) {
            arr[i]=pair[0];
            i++;
            arr[i]=pair[1];
            i++;
        }
        expect(arr[4]).assertEqual("jss")
    })

   /**
    * @tc.name: testUrlForEach001
    * @tc.desc: Callback functions are used to traverse key-value pairs on the URLSearchParams instance object.
    * @tc.require: AR000GFB2S
    * @tc.author: jiangkai
    */
    it('testUrlForEach001', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        var arr={};
        var i = 0;
        function func(value, key, SearchParams)
        {
            arr[i] = value + " " + key + " " + (params === SearchParams)
            i++
        }
        params.forEach(func);
        expect(arr[0]).assertEqual("value1 key1 true");
    })

   /**
    * @tc.name: testUrlForEach002
    * @tc.desc: Callback functions are used to traverse key-value pairs on the URLSearchParams instance object.
    * @tc.require: AR000GFB2S
    * @tc.author: jiangkai
    */
    it('testUrlForEach002', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2");
        var arr={};
        var i = 0;
        function func(value, key, SearchParams)
        {
            arr[i] = value + " " + key + " " + (params === SearchParams)
            i++
        }
        params.forEach(func);
        expect(arr[1]).assertEqual("value2 key2 true");
    })

    /**
     * @tc.name: testUrlForEach003
     * @tc.desc: Callback functions are used to traverse key-value pairs on the URLSearchParams instance object.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlForEach003', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2");
        params.append("foo","jk");
        var arr={};
        var i = 0;
        function func(value, key, SearchParams)
        {
            arr[i] = value + " " + key + " " + (params === SearchParams)
            i++
        }
        params.forEach(func);
        expect(arr[2]).assertEqual("jk foo true");
    })

    /**
     * @tc.name: testUrlForEach004
     * @tc.desc: Callback functions are used to traverse key-value pairs on the URLSearchParams instance object.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlForEach004', 0, function () {
        let params = new Url.URLSearchParams("foo=bar&jss=txt");
        var arr={};
        var i = 0;
        function func(value, key, SearchParams)
        {
            arr[i] = value + " " + key + " " + (params === SearchParams)
            i++
        }
        params.forEach(func);
        expect(arr[1]).assertEqual("txt jss true");
    })

    /**
     * @tc.name: testUrlForEach005
     * @tc.desc: Callback functions are used to traverse key-value pairs on the URLSearchParams instance object.
     * @tc.require: AR000GFB2S
     * @tc.author: jiangkai
     */
    it('testUrlForEach005', 0, function () {
        let params = new Url.URLSearchParams("foo=bar&jss=txt");
        params.append("foo","jk");
        var arr={};
        var i = 0;
        function func(value, key, SearchParams)
        {
            arr[i] = value + " " + key + " " + (params === SearchParams)
            i++
        }
        params.forEach(func);
        expect(arr[0]).assertEqual("bar foo true");
    })

    /**
     * @tc.name: testUrlGet001
     * @tc.desc: Returns the first value associated to the given search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlGet001', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        var result =  params.get("1")
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testUrlForEach002
     * @tc.desc: Returns the first value associated to the given search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlForEach002', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        var result =  params.get("key2")
        expect(result).assertEqual("value2")
    })

    /**
     * @tc.name: testUrlForEach003
     * @tc.desc: Returns the first value associated to the given search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlForEach003', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        params.append("5","JKL")
        var result =  params.get("5")
        expect(result).assertEqual("JKL")
    })

    /**
     * @tc.name: testUrlForEach004
     * @tc.desc: Returns the first value associated to the given search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlForEach004', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        var result =  params.get("key1")
        expect(result).assertEqual("value1")
    })

    /**
     * @tc.name: testUrlForEach005
     * @tc.desc: Returns the first value associated to the given search parameter.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlForEach005', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        params.append("jss","JL")
        var result =  params.get("jss")
        expect(result).assertEqual("JL")
    })

    /**
     * @tc.name: testUrlGetAll001
     * @tc.desc: Returns all key-value pairs associated with a given search parameter as an array.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlGetAll001', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2")
        params.append("key1","AAA")
        var result =  params.getAll("key1")
        expect(result.toString()).assertEqual("value1,AAA")
    })

    /**
     * @tc.name: testUrlGetAll002
     * @tc.desc: Returns all key-value pairs associated with a given search parameter as an array.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlGetAll002', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&8=DEF")
        params.append("8","A8A")
        var result =  params.getAll("8")
        expect(result.toString()).assertEqual("DEF,A8A")
    })

    /**
     * @tc.name: testUrlGetAll003
     * @tc.desc: Returns all key-value pairs associated with a given search parameter as an array.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlGetAll003', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2&key3=da")
        params.append("key3","A3A")
        var result =  params.getAll("key3")
        expect(result.toString()).assertEqual("da,A3A")
    })

    /**
     * @tc.name: testUrlGetAll004
     * @tc.desc: Returns all key-value pairs associated with a given search parameter as an array.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlGetAll004', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2&key3=大")
        params.append("key3","A3A")
        var result =  params.getAll("key4")
        expect(result.toString()).assertEqual('')
    })

    /**
     * @tc.name: testUrlGetAll005
     * @tc.desc: Returns all key-value pairs associated with a given search parameter as an array.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlGetAll005', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2&key3=大")
        params.append("key3","A3A")
        var result =  params.getAll("key2")
        expect(result.toString()).assertEqual("value2")
    })

    /**
     * @tc.name: testUrlHas001
     * @tc.desc: Returns a Boolean that indicates whether a parameter with the specified name exists.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlHas001', 0, function () {
        let params = new Url.URLSearchParams("key1=value1&key2=value2&key3=大")
        var result =  params.has("2")
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testUrlHas002
     * @tc.desc: Returns a Boolean that indicates whether a parameter with the specified name exists.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlHas002', 0, function () {
        let params = new Url.URLSearchParams("小=value1&key2=value2&key3=大")
        var result =  params.has("小")
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testUrlHas003
     * @tc.desc: Returns a Boolean that indicates whether a parameter with the specified name exists.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlHas003', 0, function () {
        let params = new Url.URLSearchParams("小=value1&￥=value2&key3=大")
        params.append("￥","ACA")
        var result =  params.has("￥")
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testUrlHas004
     * @tc.desc: Returns a Boolean that indicates whether a parameter with the specified name exists.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlHas004', 0, function () {
        let params = new Url.URLSearchParams("小=value1&key2=value2&key3=大")
        var result =  params.has("无")
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testUrlHas005
     * @tc.desc: Returns a Boolean that indicates whether a parameter with the specified name exists.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlHas005', 0, function () {
        let params = new Url.URLSearchParams("小=value1&￥=value2&key3=大")
        params.append("￥","ACA")
        var result =  params.has("￥11")
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testUrlKeys001
     * @tc.desc: Returns an iterator allowing to go through all keys contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlKeys001', 0, function () {
        let params = new Url.URLSearchParams("小=value1&￥=value2&key3=大");
        var arr={};
        var i = 0;
        for(var key of params.keys()) {
            arr[i] = key
            i++
        }
        expect(arr[0]).assertEqual("小");
    })

    /**
     * @tc.name: testUrlKeys002
     * @tc.desc: Returns an iterator allowing to go through all keys contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlKeys002', 0, function () {
        let params = new Url.URLSearchParams("小=value1&￥=value2&key3=大");
        var arr={};
        var i = 0;
        for(var key of params.keys()) {
            arr[i] = key
            i++
        }
        expect(arr[1]).assertEqual("￥");
    })

    /**
     * @tc.name: testUrlKeys003
     * @tc.desc: Returns an iterator allowing to go through all keys contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlKeys003', 0, function () {
        let params = new Url.URLSearchParams("小=value1&￥=value2&key3=大");
        var arr={};
        var i = 0;
        for(var key of params.keys()) {
            arr[i] = key
            i++
        }
        expect(arr[2]).assertEqual("key3");
    })

    /**
     * @tc.name: testUrlKeys004
     * @tc.desc: Returns an iterator allowing to go through all keys contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlKeys004', 0, function () {
        let params = new Url.URLSearchParams("小=value1&￥=value2&key3=大&key4=六");
        var arr={};
        var i = 0;
        for(var key of params.keys()) {
            arr[i] = key
            i++
        }
        expect(arr[3]).assertEqual("key4");
    })

    /**
     * @tc.name: testUrlKeys005
     * @tc.desc: Returns an iterator allowing to go through all keys contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlKeys005', 0, function () {
        let params = new Url.URLSearchParams("小=value1&￥=value2&key3=大&key4=六&key5=发");
        var arr={};
        var i = 0;
        for(var key of params.keys()) {
            arr[i] = key
            i++
        }
        expect(arr[4]).assertEqual("key5");
    })

    /**
     * @tc.name: testUrlSet001
     * @tc.desc: Sets the value associated with a given search parameter to the given value.
     * If there were several matching values, this method deletes the others.
     * If the search parameter doesn't exist, this method creates it.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSet001', 0, function () {
        let params = new Url.URLSearchParams("1=value1&2=value2&key3=3");
        // params.append("11","ACA");
        params.set("11","CCC");
        var res = params.toString();
        expect(res).assertEqual("1=value1&2=value2&key3=3&11=CCC");
    })

    /**
     * @tc.name: testUrlSet002
     * @tc.desc: Sets the value associated with a given search parameter to the given value.
     * If there were several matching values, this method deletes the others.
     * If the search parameter doesn't exist, this method creates it.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSet002', 0, function () {
        let params = new Url.URLSearchParams("1=value1&2=value2&key3=3");
        params.set('10','BBB');
        var res = params.toString();
        expect(res).assertEqual("1=value1&2=value2&key3=3&10=BBB");
    })

    /**
     * @tc.name: testUrlSet003
     * @tc.desc: Sets the value associated with a given search parameter to the given value.
     * If there were several matching values, this method deletes the others.
     * If the search parameter doesn't exist, this method creates it.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSet003', 0, function () {
        let params = new Url.URLSearchParams("1=value1&2=value2&key3=3");
        params.set("ma 大" ,"10￥");
        var res = params.toString();
        expect(res).assertEqual("1=value1&2=value2&key3=3&ma+%E5%A4%A7=10%EF%BF%A5");
    })

    /**
     * @tc.name: testUrlSet004
     * @tc.desc: Sets the value associated with a given search parameter to the given value.
     * If there were several matching values, this method deletes the others.
     * If the search parameter doesn't exist, this method creates it.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSet004', 0, function () {
        let params = new Url.URLSearchParams("1=value1&2=value2&key3=3");
        // params.append("1","ACA");
        params.set("1","CCC");
        var res = params.toString();
        expect(res).assertEqual("1=CCC&2=value2&key3=3");
    })

    /**
     * @tc.name: testUrlSet005
     * @tc.desc: Sets the value associated with a given search parameter to the given value.
     * If there were several matching values, this method deletes the others.
     * If the search parameter doesn't exist, this method creates it.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSet005', 0, function () {
        let params = new Url.URLSearchParams("1=value1&2=value2&key3=3");
        params.set('12','BBB');
        var res = params.toString();
        expect(res).assertEqual("1=value1&2=value2&key3=3&12=BBB");
    })

    /**
     * @tc.name: testUrlSort001
     * @tc.desc: Sort all key/value pairs contained in this object in place and return undefined.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSort001', 0, function () {
        let params = new Url.URLSearchParams("1=value1&3=value3&2=key2");
        params.sort();
        var res = params.toString();
        expect(res).assertEqual("1=value1&2=key2&3=value3");
    })

    /**
     * @tc.name: testUrlSort002
     * @tc.desc: Sort all key/value pairs contained in this object in place and return undefined.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSort002', 0, function () {
        let params = new Url.URLSearchParams("a=value1&c=value2&b=key2");
        params.sort();
        var res = params.toString();
        expect(res).assertEqual("a=value1&b=key2&c=value2");
    })

    /**
     * @tc.name: testUrlSort003
     * @tc.desc: Sort all key/value pairs contained in this object in place and return undefined.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSort003', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b=33");
        params.append("a","ACA");
        params.sort();
        var res = params.toString();
        expect(res).assertEqual("a=ACA&b=33&c=value2&d=value1");
    })

    /**
     * @tc.name: testUrlSort004
     * @tc.desc: Sort all key/value pairs contained in this object in place and return undefined.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSort004', 0, function () {
        let params = new Url.URLSearchParams("1=value1&3=value3&2=key2&4=key4");
        params.sort();
        var res = params.toString();
        expect(res).assertEqual("1=value1&2=key2&3=value3&4=key4");
    })

    /**
     * @tc.name: testUrlSort005
     * @tc.desc: Sort all key/value pairs contained in this object in place and return undefined.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlSort005', 0, function () {
        let params = new Url.URLSearchParams("a=value1&c=value2&4=key4&b=key2");
        params.sort();
        var res = params.toString();
        expect(res).assertEqual("4=key4&a=value1&b=key2&c=value2");
    })

    /**
     * @tc.name: testUrlValues001
     * @tc.desc: Returns an iterator allowing to go through all values contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlValues001', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b=大")
        var arr={}
        var i = 0
        for(var value  of params.values()) {
            arr[i] = value
            i++
        }
        expect(arr[0]).assertEqual("value1")
    })

    /**
     * @tc.name: testUrlValues002
     * @tc.desc: Returns an iterator allowing to go through all values contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlValues002', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b=大")
        var arr={}
        var i = 0
        for(var value  of params.values()) {
            arr[i] = value
            i++
        }
        expect(arr[1]).assertEqual("value2")
    })

    /**
     * @tc.name: testUrlValues003
     * @tc.desc: Returns an iterator allowing to go through all values contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlValues003', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b=大")
        params.append("a","ACA")
        var arr={}
        var i = 0
        for(var value  of params.values()) {
            arr[i] = value
            i++
        }
        expect(arr[3]).assertEqual("ACA")
    })

    /**
     * @tc.name: testUrlValues004
     * @tc.desc: Returns an iterator allowing to go through all values contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlValues004', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b=大&4=key4")
        var arr={}
        var i = 0
        for(var value  of params.values()) {
            arr[i] = value
            i++
        }
        expect(arr[3]).assertEqual("key4")
    })

    /**
     * @tc.name: testUrlValues005
     * @tc.desc: Returns an iterator allowing to go through all values contained in this object.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlValues005', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b=大&4=key4&5=key5")
        var arr={}
        var i = 0
        for(var value  of params.values()) {
            arr[i] = value
            i++
        }
        expect(arr[4]).assertEqual("key5")
    })

    /**
     * @tc.name: testUrlToString001
     * @tc.desc: Returns a query string suitable for use in a URL.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlToString001', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b=大")
        var result= params.toString()
        expect(result).assertEqual("d=value1&c=value2&b=%E5%A4%A7")
    })

    /**
     * @tc.name: testUrlToString002
     * @tc.desc: Returns a query string suitable for use in a URL.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlToString002', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b= 大")
        params.append("1 12","QQQ")
        var result= params.toString()
        expect(result).assertEqual("d=value1&c=value2&b=+%E5%A4%A7&1+12=QQQ")
    })

    /**
     * @tc.name: testUrlToString003
     * @tc.desc: Returns a query string suitable for use in a URL.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlToString003', 0, function () {
        let params = new Url.URLSearchParams("￥=)")
        params.delete("5")
        var result= params.toString()
        expect(result).assertEqual("%EF%BF%A5=%29")
    })

    /**
     * @tc.name: testUrlToString004
     * @tc.desc: Returns a query string suitable for use in a URL.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlToString004', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b=大&4=key4")
        var result= params.toString()
        expect(result).assertEqual("d=value1&c=value2&b=%E5%A4%A7&4=key4")
    })

    /**
     * @tc.name: testUrlToString005
     * @tc.desc: Returns a query string suitable for use in a URL.
     * @tc.require: AR000GFB2S
     * @tc.author: maxiaodong
     */
    it('testUrlToString005', 0, function () {
        let params = new Url.URLSearchParams("d=value1&c=value2&b= 大&4=key4&5=key5")
        params.append("1 12","QQQ")
        var result= params.toString()
        expect(result).assertEqual("d=value1&c=value2&b=+%E5%A4%A7&4=key4&5=key5&1+12=QQQ")
    })

    /**
     * @tc.name: testUrlSearchParamsConstruction001
     * @tc.desc: A parameterized constructor used to create an URLSearchParams instance.
     * @tc.require: AR000GFB2S
     * @tc.author: zhangyouyou
     */
    it('testUrlSearchParamsConstruction001', 0, function () {
        let params = new Url.URLSearchParams('?user=abc&query=xyz')
        var result= params.toString()
        expect(result).assertEqual("user=abc&query=xyz")
    })

    /**
     * @tc.name: testUrlSearchParamsConstruction002
     * @tc.desc: A parameterized constructor used to create an URLSearchParams instance.
     * @tc.require: AR000GFB2S
     * @tc.author: zhangyouyou
     */
    it('testUrlSearchParamsConstruction002', 0, function () {
        let params = new Url.URLSearchParams({
            user: 'abc',
            query: ['first', 'second']
        });
        var result= params.toString()
        expect(result).assertEqual("user=abc&query=first%2Csecond")
    })

    /**
     * @tc.name: testUrlSearchParamsConstruction003
     * @tc.desc: A parameterized constructor used to create an URLSearchParams instance.
     * @tc.require: AR000GFB2S
     * @tc.author: zhangyouyou
     */
    it('testUrlSearchParamsConstruction003', 0, function () {
        let params = new Url.URLSearchParams([
                ['user', 'abc'],
                ['query', 'first'],
                ['query', 'second'],
        ]);
        var result= params.toString()
        expect(result).assertEqual("user=abc&query=first&query=second")
    })

    /**
     * @tc.name: testUrlSearchParamsConstruction004
     * @tc.desc: A parameterized constructor used to create an URLSearchParams instance.
     * @tc.require: AR000GFB2S
     * @tc.author: zhangyouyou
     */
    it('testUrlSearchParamsConstruction004', 0, function () {
        const map = new Map();
        map.set('user', 'abc');
        map.set('query', 'xyz');
        let params = new Url.URLSearchParams(map)
        var result= params.toString()
        expect(result).assertEqual("user=abc&query=xyz")
    })

    /**
     * @tc.name: testUrlSearchParamsConstruction005
     * @tc.desc: A parameterized constructor used to create an URLSearchParams instance.
     * @tc.require: AR000GFB2S
     * @tc.author: zhangyouyou
     */
    it('testUrlSearchParamsConstruction005', 0, function () {
        function* getQueryPairs() {
            yield ['user', 'abc'];
            yield ['query', 'first'];
            yield ['query', 'second'];
        }
        let params = new Url.URLSearchParams(getQueryPairs());
        var result= params.toString()
        expect(result).assertEqual("user=abc&query=first&query=second")
    })

    /**
     * @tc.name: testUrlSearchParamsConstruction006
     * @tc.desc: A parameterized constructor used to create an URLSearchParams instance.
     * @tc.require: AR000GFB2S
     * @tc.author: zhangyouyou
     */
    it('testUrlSearchParamsConstruction006', 0, function () {
        let params = new Url.URLSearchParams()
        params.append('abcde','fghki')
        var paramsResult = params.toString()
        expect(paramsResult).assertEqual('abcde=fghki')
    })

    /**
     * @tc.name: testUrlSearchParamsConstruction007
     * @tc.desc: A parameterized constructor used to create an URLSearchParams instance.
     * @tc.require: AR000GFB2S
     * @tc.author: zhangyouyou
     */
    it('testUrlSearchParamsConstruction007', 0, function () {
        let param
        let params = new Url.URLSearchParams(param)
        params.append('abcde','fghki')
        var paramsResult = params.toString()
        expect(paramsResult).assertEqual('abcde=fghki')
    })

    /**
     * @tc.name: testUrlToString001
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToString001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        var result= params.toString()
        expect(result).assertEqual("http://username:password@host:8080/directory/file?query#fragment")
    })

    /**
     * @tc.name: testUrlToString002
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToString002', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file')
        var result= params.toString()
        expect(result).assertEqual("http://username:password@host:8080/directory/file")
    })

    /**
     * @tc.name: testUrlToString003
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToString003', 0, function () {
        let params = new Url.URL('http://username:password@host:8080#fragment')
        var result= params.toString()
        expect(result).assertEqual("http://username:password@host:8080/#fragment")
    })

    /**
     * @tc.name: testUrlToString004
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToString004', 0, function () {
        let params = new Url.URL('http1://host/directory/file?query#fragment')
        var result= params.toString()
        expect(result).assertEqual("http1://host/directory/file?query#fragment")
    })

    /**
     * @tc.name: testUrlToString005
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToString005', 0, function () {
        let params = new Url.URL('http:host:8080/directory/file?query#fragment')
        var result= params.toString()
        expect(result).assertEqual("http://host:8080/directory/file?query#fragment")
    })

    /**
     * @tc.name: testUrlHref001
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHref001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        var result= params.href
        expect(result).assertEqual("http://username:password@host:8080/directory/file?query#fragment")
    })

    /**
     * @tc.name: testUrlHref002
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHref002', 0, function () {
        let params = new Url.URL('http://host:8080/directory/file?query#fragment')
        var result= params.href
        expect(result).assertEqual("http://host:8080/directory/file?query#fragment")
    })

    /**
     * @tc.name: testUrlHref003
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHref003', 0, function () {
        let params = new Url.URL('http://username:password@host:8080')
        var result= params.href
        expect(result).assertEqual("http://username:password@host:8080/")
    })

    /**
     * @tc.name: testUrlHref004
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHref004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.href = 'http1://myhost/path?sss=1#ff';
        var result= params.href
        expect(result).assertEqual("http1://myhost/path?sss=1#ff")
    })

    /**
     * @tc.name: testUrlHref005
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHref005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.href = 'http://myhost:99/path#ff';
        var result= params.href
        expect(result).assertEqual("http://myhost:99/path#ff")
    })

    /**
     * @tc.name: testUrlOrigin001
     * @tc.desc: Gets the read-only serialization of the URL's origin.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlOrigin001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        var result= params.origin
        expect(result).assertEqual('http://host:8080')
    })

    /**
     * @tc.name: testUrlOrigin002
     * @tc.desc: Gets the read-only serialization of the URL's origin.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlOrigin002', 0, function () {
        let params = new Url.URL('http://username:password@host:11/directory/file?query#fragment')
        var result= params.origin
        expect(result).assertEqual('http://host:11')
    })

    /**
     * @tc.name: testUrlOrigin003
     * @tc.desc: Gets the read-only serialization of the URL's origin.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlOrigin003', 0, function () {
        let params = new Url.URL('http://username:password@host/directory/file?query#fragment')
        var result= params.origin
        expect(result).assertEqual('http://host')
    })

    /**
     * @tc.name: testUrlOrigin004
     * @tc.desc: Gets the read-only serialization of the URL's origin.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlOrigin004', 0, function () {
        let params = new Url.URL('http://username:password@aaaasshost:212/directory/file?query#fragment')
        var result= params.origin
        expect(result).assertEqual('http://aaaasshost:212')
    })

    /**
     * @tc.name: testUrlOrigin005
     * @tc.desc: Gets the read-only serialization of the URL's origin.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlOrigin005', 0, function () {
        let params = new Url.URL('http://username:password@host22:100#fragment')
        var result= params.origin
        expect(result).assertEqual('http://host22:100')
    })

    /**
     * @tc.name: testUrlProtocol001
     * @tc.desc: Gets and sets the protocol portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlProtocol001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        var result= params.protocol
        expect(result).assertEqual('http:')
    })

    /**
     * @tc.name: testUrlProtocol002
     * @tc.desc: Gets and sets the protocol portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlProtocol002', 0, function () {
        let params = new Url.URL('http1://username:password@host:8080/directory/file?query#fragment')
        var result= params.protocol
        expect(result).assertEqual('http1:')
    })

    /**
     * @tc.name: testUrlProtocol003
     * @tc.desc: Gets and sets the protocol portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlProtocol003', 0, function () {
        let params = new Url.URL('https://username:password@host:8080/directory/file?query#fragment')
        var result= params.protocol
        expect(result).assertEqual('https:')
    })

    /**
     * @tc.name: testUrlProtocol004
     * @tc.desc: Gets and sets the protocol portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlProtocol004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.protocol = 'https:';
        var result= params.protocol
        expect(result).assertEqual('https:')
    })

    /**
     * @tc.name: testUrlProtocol005
     * @tc.desc: Gets and sets the protocol portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlProtocol005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.protocol = 'ftp:';
        var result= params.protocol
        expect(result).assertEqual('ftp:')
    })

    /**
     * @tc.name: testUrlUsername001
     * @tc.desc: Gets and sets the username portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlUsername001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        var result= params.username
        expect(result).assertEqual('username')
    })

    /**
     * @tc.name: testUrlUsername002
     * @tc.desc: Gets and sets the username portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlUsername002', 0, function () {
        let params = new Url.URL('http://zhao:password@host:8080/directory/file?query#fragment')
        var result= params.username
        expect(result).assertEqual('zhao')
    })

    /**
     * @tc.name: testUrlUsername003
     * @tc.desc: Gets and sets the username portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlUsername003', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.username = 'skk'
        var result= params.username
        expect(result).assertEqual('skk')
    })

    /**
     * @tc.name: testUrlUsername004
     * @tc.desc: Gets and sets the username portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlUsername004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.username = 'skp111'
        var result= params.username
        expect(result).assertEqual('skp111')
    })

    /**
     * @tc.name: testUrlUsername005
     * @tc.desc: Gets and sets the username portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlUsername005', 0, function () {
        let params = new Url.URL('http://usme@host:8080/directory/file?query#fragment')
        var result= params.username
        expect(result).assertEqual('usme')
    })

    /**
     * @tc.name: testUrlPassword001
     * @tc.desc: Gets and sets the password portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPassword001', 0, function () {
        let params = new Url.URL('http://username:11@host:8080/directory/file?query#fragment')
        var result= params.password
        expect(result).assertEqual('11')
    })

    /**
     * @tc.name: testUrlPassword002
     * @tc.desc: Gets and sets the password portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPassword002', 0, function () {
        let params = new Url.URL('http://username:23aa@host:8080/directory/file?query#fragment')
        var result= params.password
        expect(result).assertEqual('23aa')
    })

    /**
     * @tc.name: testUrlPassword003
     * @tc.desc: Gets and sets the password portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPassword003', 0, function () {
        let params = new Url.URL('http://username@host:8080/directory/file?query#fragment')
        var result= params.password
        expect(result).assertEqual('')
    })

    /**
     * @tc.name: testUrlPassword004
     * @tc.desc: Gets and sets the password portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPassword004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.password = '1234'
        var result= params.password
        expect(result).assertEqual('1234')
    })

    /**
     * @tc.name: testUrlPassword005
     * @tc.desc: Gets and sets the password portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPassword005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.password = '';
        var result= params.password
        expect(result).assertEqual('')
    })

    /**
     * @tc.name: testUrlHost001
     * @tc.desc: Gets and sets the host portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHost001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        var result= params.host
        expect(result).assertEqual('host:8080')
    })

    /**
     * @tc.name: testUrlHost002
     * @tc.desc: Gets and sets the host portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHost002', 0, function () {
        let params = new Url.URL('http://username:password@hosthost/directory/file?query#fragment')
        var result= params.host
        expect(result).assertEqual('hosthost')
    })

    /**
     * @tc.name: testUrlHost003
     * @tc.desc: Gets and sets the host portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHost003', 0, function () {
        let params = new Url.URL('http://username:password@host:199/directory/file?query#fragment')
        var result= params.host
        expect(result).assertEqual('host:199')
    })

    /**
     * @tc.name: testUrlHost004
     * @tc.desc: Gets and sets the host portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHost004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.host = 'jkjk'
        var result= params.host
        expect(result).assertEqual('jkjk:8080')
    })

    /**
     * @tc.name: testUrlHost005
     * @tc.desc: Gets and sets the host portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHost005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.host = 'jkjk:100'
        var result= params.host
        expect(result).assertEqual('jkjk:100')
    })

    /**
     * @tc.name: testUrlHostname001
     * @tc.desc: Gets and sets the host name portion of the URL，not include the port.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHostname001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        var result= params.hostname
        expect(result).assertEqual('host')
    })

    /**
     * @tc.name: testUrlHostname002
     * @tc.desc: Gets and sets the host name portion of the URL，not include the port.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHostname002', 0, function () {
        let params = new Url.URL('http://username:password@host123:8080/directory/file?query#fragment')
        var result= params.hostname
        expect(result).assertEqual('host123')
    })

    /**
     * @tc.name: testUrlHostname003
     * @tc.desc: Gets and sets the host name portion of the URL，not include the port.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHostname003', 0, function () {
        let params = new Url.URL('http://username:password@885ssa:8080/directory/file?query#fragment')
        var result= params.hostname
        expect(result).assertEqual('885ssa')
    })

    /**
     * @tc.name: testUrlHostname004
     * @tc.desc: Gets and sets the host name portion of the URL，not include the port.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHostname004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.hostname = 'sksk'
        var result= params.hostname
        expect(result).assertEqual('sksk')
    })

    /**
     * @tc.name: testUrlHostname005
     * @tc.desc: Gets and sets the host name portion of the URL，not include the port.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHostname005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.hostname = 'hosthost'
        var result= params.hostname
        expect(result).assertEqual('hosthost')
    })

    /**
     * @tc.name: testUrlPort001
     * @tc.desc: Gets and sets the port portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPort001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        var result= params.port
        expect(result).assertEqual('8080')
    })

    /**
     * @tc.name: testUrlPort002
     * @tc.desc: Gets and sets the port portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPort002', 0, function () {
        let params = new Url.URL('http://username:password@host:100/directory/file?query#fragment')
        var result= params.port
        expect(result).assertEqual('100')
    })

    /**
     * @tc.name: testUrlPort003
     * @tc.desc: Gets and sets the port portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPort003', 0, function () {
        let params = new Url.URL('http://username:password@host/directory/file?query#fragment')
        var result= params.port
        expect(result).assertEqual('')
    })

    /**
     * @tc.name: testUrlPort004
     * @tc.desc: Gets and sets the port portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPort004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.port = '99'
        var result= params.port
        expect(result).assertEqual('99')
    })

    /**
     * @tc.name: testUrlPort005
     * @tc.desc: Gets and sets the port portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPort005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.port = '123'
        var result= params.port
        expect(result).assertEqual('123')
    })

    /**
     * @tc.name: testUrlPathname001
     * @tc.desc: Gets and sets the path portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPathname001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        var result= params.pathname
        expect(result).assertEqual('/directory/file')
    })

    /**
     * @tc.name: testUrlPathname002
     * @tc.desc: Gets and sets the path portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPathname002', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory?query#fragment')
        var result= params.pathname
        expect(result).assertEqual('/directory')
    })

    /**
     * @tc.name: testUrlPathname003
     * @tc.desc: Gets and sets the path portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPathname003', 0, function () {
        let params = new Url.URL('http://username:password@host:8080?query#fragment')
        var result= params.pathname
        expect(result).assertEqual('/')
    })

    /**
     * @tc.name: testUrlPathname004
     * @tc.desc: Gets and sets the path portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPathname004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.pathname = 'path'
        var result= params.pathname
        expect(result).assertEqual('/path')
    })

    /**
     * @tc.name: testUrlPathname005
     * @tc.desc: Gets and sets the path portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlPathname005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query#fragment')
        params.pathname = '/directory/file11'
        var result= params.pathname
        expect(result).assertEqual('/directory/file11')
    })

    /**
     * @tc.name: testUrlSearch001
     * @tc.desc: Gets and sets the serialized query portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlSearch001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=qqqq#fragment')
        var result= params.search
        expect(result).assertEqual('?query=qqqq')
    })

    /**
     * @tc.name: testUrlSearch002
     * @tc.desc: Gets and sets the serialized query portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlSearch002', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=qqqq&ll=pp#fragment')
        var result= params.search
        expect(result).assertEqual('?query=qqqq&ll=pp')
    })

    /**
     * @tc.name: testUrlSearch003
     * @tc.desc: Gets and sets the serialized query portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlSearch003', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?ppp9875=77#fragment')
        var result= params.search
        expect(result).assertEqual('?ppp9875=77')
    })

    /**
     * @tc.name: testUrlSearch004
     * @tc.desc: Gets and sets the serialized query portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlSearch004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=qqqq#fragment')
        params.search = 'kk=99'
        var result= params.search
        expect(result).assertEqual('?kk=99')
    })

    /**
     * @tc.name: testUrlSearch005
     * @tc.desc: Gets and sets the serialized query portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlSearch005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=qqqq#fragment')
        params.search = 'pppk=99'
        var result= params.search
        expect(result).assertEqual('?pppk=99')
    })

    /**
     * @tc.name: testUrlHash001
     * @tc.desc: Gets and sets the fragment portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHash001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=pppppp#fragment')
        var result= params.hash
        expect(result).assertEqual('#fragment')
    })

    /**
     * @tc.name: testUrlHash002
     * @tc.desc: Gets and sets the fragment portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHash002', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=pppppp#fragment')
        params.hash = '123456'
        var result= params.hash
        expect(result).assertEqual('#123456')
    })

    /**
     * @tc.name: testUrlHash003
     * @tc.desc: Gets and sets the fragment portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHash003', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=pppppp#poiu')
        var result= params.hash
        expect(result).assertEqual('#poiu')
    })

    /**
     * @tc.name: testUrlHash004
     * @tc.desc: Gets and sets the fragment portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHash004', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=pppppp')
        var result= params.hash
        expect(result).assertEqual('')
    })

    /**
     * @tc.name: testUrlHash005
     * @tc.desc: Gets and sets the fragment portion of the URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlHash005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=pppppp#fragment')
        params.hash = 'qwer'
        var result= params.hash
        expect(result).assertEqual('#qwer')
    })

    /**
     * @tc.name: testUrlToJson001
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToJson001', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory/file?query=pppppp#qwer=da')
        var result= params.toJSON()
        expect(result).assertEqual('http://username:password@host:8080/directory/file?query=pppppp#qwer=da')
    })

    /**
     * @tc.name: testUrlToJson002
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToJson002', 0, function () {
        let params = new Url.URL('http://host:8080/directory/file?query=pppppp#qwer=da')
        var result= params.toJSON()
        expect(result).assertEqual('http://host:8080/directory/file?query=pppppp#qwer=da')
    })

    /**
     * @tc.name: testUrlToJson003
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToJson003', 0, function () {
        let params = new Url.URL('http://username:password@host:8080')
        var result= params.toJSON()
        expect(result).assertEqual('http://username:password@host:8080/')
    })

    /**
     * @tc.name: testUrlToJson004
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToJson004', 0, function () {
        let params = new Url.URL('http11://username:password@host:8080?query=pppppp#qwer=da')
        var result= params.toJSON()
        expect(result).assertEqual('http11://username:password@host:8080?query=pppppp#qwer=da')
    })

    /**
     * @tc.name: testUrlToJson005
     * @tc.desc: Returns the serialized URL as a string.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlToJson005', 0, function () {
        let params = new Url.URL('http://username:password@host:8080/directory')
        var result= params.toJSON()
        expect(result).assertEqual('http://username:password@host:8080/directory')
    })

    /**
     * @tc.name: testUrlIPv6001
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlIPv6001', 0, function () {
        let params = new Url.URL('http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html')
        var result= params.href
        expect(result).assertEqual('http://[fedc:ba98:7654:3210:fedc:ba98:7654:3210]/index.html')
    })

    /**
     * @tc.name: testUrlIPv6002
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlIPv6002', 0, function () {
        let params = new Url.URL('http://[1080:0:0:0:8:800:200C:417A]/index.html')
        var result= params.href
        expect(result).assertEqual('http://[1080::8:800:200c:417a]/index.html')
    })

    /**
     * @tc.name: testUrlIPv6003
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlIPv6003', 0, function () {
        let params = new Url.URL('http://[::FFFF:129.144.52.38]:80/index.html')
        var result= params.href
        expect(result).assertEqual('http://[::ffff:8190:3426]/index.html')
    })

    /**
     * @tc.name: testUrlIPv4001
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlIPv4001', 0, function () {
        let params = new Url.URL('http://0377.0xff.255.1:80/index.html')
        var result= params.href
        expect(result).assertEqual('http://255.255.255.1/index.html')
    })

    /**
     * @tc.name: testUrlIPv4002
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlIPv4002', 0, function () {
        let params = new Url.URL('http://0377.0xff.255.g/index.html')
        var result= params.href
        expect(result).assertEqual('http://0377.0xff.255.g/index.html')
    })

    /**
     * @tc.name: testUrlIPv4003
     * @tc.desc: Gets and sets the serialized URL.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlIPv4003', 0, function () {
        let params = new Url.URL('http://190.254.245.9:80/index.html')
        var result= params.href
        expect(result).assertEqual('http://190.254.245.9/index.html')
    })

    /**
     * @tc.name: testUrlConstruction001
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction001', 0, function () {
        let params = new Url.URL('https://developer.mozilla.org',)
        var result= params.href
        expect(result).assertEqual('https://developer.mozilla.org/')
    })

    /**
     * @tc.name: testUrlConstruction002
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction002', 0, function () {
        let params = new Url.URL('https://developer.mozilla.org','flie:/developer.mozilla.org')
        var result= params.href
        expect(result).assertEqual('https://developer.mozilla.org/')
    })

    /**
     * @tc.name: testUrlConstruction003
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction003', 0, function () {
        let params = new Url.URL('https://developer.mozilla.org','ftp://www.example.com')
        var result= params.href
        expect(result).assertEqual('https://developer.mozilla.org/')
    })

    /**
     * @tc.name: testUrlConstruction004
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction004', 0, function () {
        let params = new Url.URL(' ', 'http://www.example.com')
        var result= params.href
        expect(result).assertEqual('http://www.example.com/')
    })

    /**
     * @tc.name: testUrlConstruction005
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction005', 0, function () {
        let params = new Url.URL('.', 'http://www.example.com')
        var result= params.href
        expect(result).assertEqual('http://www.example.com/')
    })

    /**
     * @tc.name: testUrlConstruction006
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction006', 0, function () {
        let params = new Url.URL('../h:', 'http://www.example.com')
        var result= params.href
        expect(result).assertEqual('http://www.example.com/h:')
    })

    /**
     * @tc.name: testUrlConstruction007
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction007', 0, function () {
        let params = new Url.URL('/sca/./path/path/../scasa/jjjjj', 'http://www.example.com')
        var result= params.href
        expect(result).assertEqual('http://www.example.com/sca/path/scasa/jjjjj')
    })

    /**
     * @tc.name: testUrlConstruction008
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction008', 0, function () {
        let params = new Url.URL('/../sca/./path/path/../scasa/jjjjj', 'http://www.example.com')
        var result= params.href
        expect(result).assertEqual('http://www.example.com/sca/path/scasa/jjjjj')
    })

    /**
     * @tc.name: testUrlConstruction009
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction009', 0, function () {
        let params = new Url.URL( '/../sca/./path/path/../scasa/jjjjj', 'file://www.example.com')
        var result= params.href
        expect(result).assertEqual('file://www.example.com/sca/path/scasa/jjjjj')
    })

    /**
     * @tc.name: testUrlConstruction010
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction010', 0, function () {
        let params = new Url.URL('/../sca/./path/path/../scasa/jjjjj', 'file1://www.example.com')
        var result= params.href
        expect(result).assertEqual('file1://www.example.com/sca/path/scasa/jjjjj')
    })

    /**
     * @tc.name: testUrlConstruction011
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction011', 0, function () {
        let params = new Url.URL('htt1p://www.0902zy.cn/path?abc=123&def=456#yyyy')
        let params1 = new Url.URL('www.baidu.com/ssaa',params)
        var result = params1.href
        expect(result).assertEqual('htt1p://www.0902zy.cn/www.baidu.com/ssaa')
    })

    /**
     * @tc.name: testUrlConstruction012
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction012', 0, function () {
        let params = new Url.URL('htt1p://www.0902zy.cn/path?abc=123&def=456#yyyy')
        let params1 = new Url.URL('//www.baidu.com/ssaa',params)
        var result = params1.href
        expect(result).assertEqual('htt1p://www.baidu.com/ssaa')
    })

    /**
     * @tc.name: testUrlConstruction013
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction013', 0, function () {
        let params = new Url.URL('htt1p://www.0902zy.cn/path?abc=123&def=456#yyyy')
        let params1 = new Url.URL('',params)
        var result = params1.href
        expect(result).assertEqual('htt1p://www.0902zy.cn/path?abc=123&def=456#yyyy')
    })

    /**
     * @tc.name: testUrlConstruction014
     * @tc.desc: URL constructor, which is used to instantiate a URL object.
     * @tc.require: AR000GFB2S
     * @tc.author: zhaoduwei
     */
    it('testUrlConstruction014', 0, function () {
        let params = new Url.URL('htt1p://www.0902zy.cn/path?abc=123&def=456#yyyy')
        var result = params.searchParams.toString();
        expect(result).assertEqual('abc=123&def=456')
    })
})