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
import util from '@ohos.util'
import url from '@ohos.url'
import app from '@system.app'
import process from '@ohos.process'
import file from '@system.file';

class Temperature {
    constructor(value) {
        this._temp = value;
    }
    compareTo(value) {
        return this._temp >= value.getTemp();
    }
    getTemp() {
        return this._temp;
    }
    toString() {
        return this._temp.toString();
    }
}
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempLess = new Temperature(20);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var tempMore = new Temperature(45);

var range = new util.Scope(tempLower, tempUpper);
var rangeFir = new util.Scope(tempMiDF, tempMidS);
var rangeSec = new util.Scope(tempLess, tempMore);
var rangeThi = new util.Scope(tempLess, tempMiDF);
var rangeFif = new util.Scope(tempMiDF, tempMore);

describe('TextEncoderTest', function () {

    /**
     * @tc.name: testUtilPrintf001
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf001', 0, function () {
        var format = "%i,%s";
        var value1 = 1.5;
        var value2 = "qwer";
        var value3 = 15;
        var result = util.printf(format, value1, value2, value3);
        expect(result).assertEqual("1,qwer 15");
    })

    /**
     * @tc.name: testUtilPrintf002
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf002', 0, function () {
        var format = "%O";
        var value = { name: 'jack' ,age: 15 };
        var result = util.printf(format, value);
        expect(result).assertEqual("{ name: 'jack',\n  age: 15 }");
    })

    /**
     * @tc.name: testUtilPrintf003
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf003', 0, function () {
        var format = "%o";
        var value = [1, 2, 3];
        var result = util.printf(format, value);
        var res = '[ 1, 2, 3, [length]: 3 ]'
        expect(result).assertEqual('[ 1, 2, 3, [length]: 3 ]');
    })

    /**
     * @tc.name: testUtilPrintf004
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf004', 0, function () {
        var format = "%s,%s,%s";
        var value1 = "1.5";
        var value2 = "qwer";
        var result = util.printf(format, value1, value2);
        expect(result).assertEqual('1.5,qwer,%s');
    })

    /**
     * @tc.name: testUtilPrintf005
     * @tc.desc: Returns the formatted string.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPrintf005', 0, function () {
        var format = "%d,%d";
        var value1 = 6;
        var value2 = 16;
        var result = util.printf(format, value1, value2);
        expect(result).assertEqual('6,16');
    })

    /**
     * @tc.name: testUtilGetErrorString001
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
     it('testUtilGetErrorString001', 0, async function () {
        var errnum = 10;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('Unknown system error 10');
    })

    /**
     * @tc.name: testUtilGetErrorString002
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilGetErrorString002', 0, async function () {
        var errnum = 0;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('Unknown system error 0');
    })

    /**
     * @tc.name: testUtilGetErrorString003
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilGetErrorString003', 0, async function () {
        var errnum = -1;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('operation not permitted');
    })

    /**
     * @tc.name: testUtilGetErrorString004
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilGetErrorString004', 0, async function () {
        var errnum = 9;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('Unknown system error 9');
    })

    /**
     * @tc.name: testUtilGetErrorString005
     * @tc.desc: Get the string name of the system errno.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilGetErrorString005', 0, async function () {
        var errnum = 555;
        var result = util.getErrorString(errnum);
        expect(result).assertEqual('Unknown system error 555');
    })

    /**
     * @tc.name: testUtilPromiseWrapper001
     * @tc.desc: Takes a function following the common error-first callback style,
	   taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper001', 0, async function () {
        function aysnFun(str1, str2, callback) {
            if (typeof str1 === 'string' && typeof str1 === 'string') {
                callback(null, str1 + str2);
            } else {
                callback('type err');
            }
        }
        let newPromiseObj = util.promiseWrapper(aysnFun)("Hello", 'World');
        newPromiseObj.then(res => {
            expect(res).strictEqual('HelloWorld');
        })
    })

    /**
     * @tc.name: testUtilPromiseWrapper002
     * @tc.desc: Takes a function following the common error-first callback style,
	   taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper002', 0, async function () {
        function aysnFun(str1, str2, callback) {
            if (typeof str1 === 'string' && typeof str1 === 'string') {
                callback(null, str1 + str2);
            } else {
                callback('type err');
            }
        }
        let newPromiseObj = util.promiseWrapper(aysnFun)([1, 2], 'World');
        newPromiseObj.catch(err => {
            expect(err).strictEqual('type err');
        })
    })

    /**
     * @tc.name: testUtilPromiseWrapper003
     * @tc.desc: Takes a function following the common error-first callback style,
	   taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper003', 0, async function () {
        const a = util.promiseWrapper(function() {});
        const b = util.promiseWrapper(a);
        expect(a).strictEqual(b);
    })

    /**
     * @tc.name: testUtilPromiseWrapper004
     * @tc.desc: Takes a function following the common error-first callback style,
	   taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper004', 0, async function () {
        let errToThrow;
        const thrower = util.promiseWrapper(function(a, b, c, cb) {
            errToThrow = new Error();
            throw errToThrow;
        });
        thrower(1, 2, 3).catch(err => {
            expect(err).strictEqual(errToThrow);
        })
    })

    /**
     * @tc.name: testUtilPromiseWrapper005
     * @tc.desc: Takes a function following the common error-first callback style,
	   taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper005', 0, async function () {
        const err = new Error();
        const a = util.promiseWrapper((cb) => cb(err))();
        const b = util.promiseWrapper(() => {throw err;})();
        Promise.all([
            a.then(e => {
                expect(e).strictEqual(err);
            }),
            b.then(e => {
                expect(e).strictEqual(err);
            })
        ]);
    })

    /**
     * @tc.name: testUtilPromiseWrapper006
     * @tc.desc: Takes a function following the common error-first callback style,
	   taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper006', 0, async function () {
        const err = new Error('callback with the error.');
        const stack = err.stack;
        const fn = util.promiseWrapper(function(cb) {
            cb(null);
            cb(err);
        });
        (async () => {
            await fn();
            await Promise.resolve();
            return expect(stack).strictEqual(err.stack);
        })();
    })

    /**
     * @tc.name: testUtilPromiseWrapper007
     * @tc.desc: Takes a function following the common error-first callback style,
	   taking an callback as the last argument, and return a version that returns promises.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilPromiseWrapper007', 0, async function () {
        function fn(err, val, callback) {
            callback(err, val);
        }
        (async () => {
            const value = await util.promiseWrapper(fn)(null, 42);
            expect(value).strictEqual(42);
        })();
    })

    /**
     * @tc.name: testUtilCallbackWrapper001
     * @tc.desc: Takes an async function (or a function that returns a Promise) and
	   returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper001', 0, async function () {
        const promiseFn = [1, 2];
        try {
            util.callbackWrapper(promiseFn);
        } catch(e) {
            expect(e.message).strictEqual('original is not function');
        }
    })

    /**
     * @tc.name: testUtilCallbackWrapper002
     * @tc.desc: Takes an async function (or a function that returns a Promise) and
	   returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper002', 0, async function () {
        async function promiseFn() {
            return Promise.resolve('value');
        }
        var cb = util.callbackWrapper(promiseFn);
        cb((err, ret) => {
            expect(err).strictEqual(null);
            expect(ret).strictEqual('value');
        })
    })

    /**
     * @tc.name: testUtilCallbackWrapper003
     * @tc.desc: Takes an async function (or a function that returns a Promise) and
	   returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper003', 0, async function () {
        async function promiseFn() {
            return 42;
        }
        var cb = util.callbackWrapper(promiseFn);
        cb((err, ret) => {
            expect(err).strictEqual(null);
            expect(ret).strictEqual(42);
        })
    })

    /**
     * @tc.name: testUtilCallbackWrapper004
     * @tc.desc: Takes an async function (or a function that returns a Promise) and
	   returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper004', 0, async function () {
        async function promiseFn() {
			var err = Error('value');
			return Promise.reject(err);
			
        }
        var cb = util.callbackWrapper(promiseFn);
        cb((err, ret) => {
            expect(err.message).strictEqual('value');
            expect(ret).strictEqual(undefined);
        })
    })

    /**
     * @tc.name: testUtilCallbackWrapper005
     * @tc.desc: Takes an async function (or a function that returns a Promise) and
	   returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper005', 0, async function () {
        async function promiseFn(a, b) {
            return a + b;
        }
        var cb = util.callbackWrapper(promiseFn);
        cb(1, 2, (err, ret) => {
            expect(err).strictEqual(null);
            expect(ret).strictEqual(3);
        })
    })

    /**
     * @tc.name: testUtilCallbackWrapper006
     * @tc.desc: Takes an async function (or a function that returns a Promise) and
	   returns a function following the error-first callback style.
     * @tc.require: AR000GFB4U
     * @tc.author: shikai
     */
    it('testUtilCallbackWrapper006', 0, async function () {
        async function promiseFn(){
            return null;
        }
        var cb = util.callbackWrapper(promiseFn);
        try {
            cb([1, 2])
        } catch(err) {
            expect(err.message).strictEqual('maybe is not function');
        }
    })

    /**
     * @tc.name: testEncoding001
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding001', 0, function () {
        var  that = new util.TextDecoder('utf-8', { ignoreBOM : true })
        var retStr = that.encoding
        expect(retStr).assertEqual('utf-8')
    })

    /**
     * @tc.name: testEncoding002
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding002', 0, function () {
        var that = new util.TextDecoder('utf-16le')
        var encodingStr = that.encoding
        expect(encodingStr).assertEqual('utf-16le')
    })

    /**
     * @tc.name: testEncoding003
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding003', 0, function () {
        var that = new util.TextDecoder('utf-16be')
        var encodingStr = that.encoding
        expect(encodingStr).assertEqual('utf-16be')
    })

    /**
     * @tc.name: testEncoding004
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding004', 0, function () {
        var that = new util.TextDecoder('utf-16be', { ignoreBOM : true })
        var encodingStr = that.encoding
        expect(encodingStr).assertEqual('utf-16be')
    })

    /**
     * @tc.name: testEncoding005
     * @tc.desc: The source encoding's name, lowercased.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncoding005', 0, function () {
        var that = new util.TextDecoder('utf-16be', { ignoreBOM : false })
        var encodingStr = that.encoding
        expect(encodingStr).assertEqual('utf-16be')
    })


    /**
     * @tc.name: testFatal001
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal001', 0, function () {
        var that = new util.TextDecoder('utf-16be', { fatal : true })
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(true)
    })

    /**
     * @tc.name: testFatal002
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal002', 0, function () {
        var that = new util.TextDecoder('utf-16be', { fatal : false })
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(false)
    })

    /**
     * @tc.name: testFatal003
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal003', 0, function () {
        var that = new util.TextDecoder('utf-16be')
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(false)
    })

    /**
     * @tc.name: testFatal004
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal004', 0, function () {
        var that = new util.TextDecoder('utf-8')
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(false)
    })

    /**
     * @tc.name: testFatal005
     * @tc.desc: Returns `true` if error mode is "fatal", and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testFatal005', 0, function () {
        var that = new util.TextDecoder('utf-16le')
        var fatalStr = that.fatal
        expect(fatalStr).assertEqual(false)
    })

    /**
     * @tc.name: testIgnoreBOM001
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM001', 0, function () {
        var that = new util.TextDecoder('utf-16be', { ignoreBOM : true })
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(true)
    })

    /**
     * @tc.name: testIgnoreBOM002
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM002', 0, function () {
        var that = new util.TextDecoder('utf-16be', { ignoreBOM : false })
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(false)
    })

    /**
     * @tc.name: testIgnoreBOM003
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM003', 0, function () {
        var that = new util.TextDecoder('utf-16be')
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(false)
    })

    /**
     * @tc.name: testIgnoreBOM004
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM004', 0, function () {
        var that = new util.TextDecoder('utf-8')
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(false)
    })

    /**
     * @tc.name: testIgnoreBOM005
     * @tc.desc: Returns `true` if ignore BOM flag is set, and `false` otherwise.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testIgnoreBOM005', 0, function () {
        var that = new util.TextDecoder('utf-16le')
        var ignoreBOMStr = that.ignoreBOM
        expect(ignoreBOMStr).assertEqual(false)
    })


    /**
     * @tc.name: testDecode001
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode001', 0, function () {
        var that = new util.TextDecoder('utf-8');
        var arr = new Uint8Array(3);
        for (var i = 0; i < 3; i++) {
            arr[i] = 0x61 + i;
        }
        var retStr = that.decode(arr);
        var rel = 'abc';
        expect(retStr).assertEqual(rel);
    })

    /**
     * @tc.name: testDecode002
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode002', 0, function () {
        var that = new util.TextDecoder('utf-16le')
        var arr = new Uint8Array(6)
        arr[0] = 0x61;
        arr[1] = 0x00;
        arr[2] = 0x62;
        arr[3] = 0x00;
        arr[4] = 0x63;
        arr[5] = 0x00;
        var retStr = that.decode(arr)
        var rel = 'abc'
        expect(retStr).assertEqual(rel)
    })

    /**
     * @tc.name: testDecode003
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode003', 0, function () {
        var that = new util.TextDecoder('utf-16be');
        var arr = new Uint8Array(6);
        arr[0] = 0x00;
        arr[1] = 0x61;
        arr[2] = 0x00;
        arr[3] = 0x62;
        arr[4] = 0x00;
        arr[5] = 0x63;
        var retStr = that.decode(arr);
        var rel = 'abc'
        expect(retStr).assertEqual(rel);
    })

    /**
     * @tc.name: testDecode004
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode004', 0, function () {
        var that = new  util.TextDecoder('utf-8', { ignoreBOM : true })
        var arr = new Uint8Array(6)
        arr[0] = 0xEF;
        arr[1] = 0xBB;
        arr[2] = 0xBF;
        arr[3] = 0x61;
        arr[4] = 0x62;
        arr[5] = 0x63;
        var retStr = that.decode(arr, {stream:true})
        var BOM = '\uFEFF'
        var rel = 'abc'
        var re = BOM + rel
        expect(retStr).assertEqual(re);
    })

    /**
     * @tc.name: testDecode005
     * @tc.desc: Returns the result of running encoding's decoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testDecode005', 0, function () {
        var that = new util.TextDecoder('utf-16le', { ignoreBOM : false })
        var arr = new Uint8Array(8)
        arr[0] = 0xFF;
        arr[1] = 0xFE;
        arr[2] = 0x61;
        arr[3] = 0x00;
        arr[4] = 0x62;
        arr[5] = 0x00
        arr[6] = 0x63;
        arr[7] = 0x00;
        var retStr = that.decode(arr, { stream : false })
        var BOM = '\uFEFF'
        var rel = 'abc'
        var re = BOM + rel
        expect(retStr).assertEqual(re)
    })

    /**
     * @tc.name: testGetEncoding001
     * @tc.desc: Encoding format.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testGetEncoding001', 0, function () {
        var that = new util.TextEncoder()
        var str = that.encoding
        expect(str).assertEqual('utf-8')
    })

    /**
     * @tc.name: testEncode001
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode001', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('abc')
        expect(result[0]).assertEqual(0x61)
    })

    /**
     * @tc.name: testEncode002
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode002', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('\uD800楼楼')
        expect(result[5]).assertEqual(188)
    })

    /**
     * @tc.name: testEncode003
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode003', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('a\uD800楼楼')
        expect(result[0]).assertEqual(0x61)
    })

    /**
     * @tc.name: testEncode004
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode004', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('abc\uD800楼楼')
        expect(result[1]).assertEqual(0x62)
    })

    /**
     * @tc.name: testEncode005
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode005', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('123\uD800楼楼')
        expect(result[0]).assertEqual(49)
        expect(result[9]).assertEqual(230)
    })

    /**
     * @tc.name: testEncode006
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode006', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('123¥\uD800楼')
        expect(result[10]).assertEqual(0xbc)
    })

    /**
     * @tc.name: testEncode007
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode007', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('¥¥')
        expect(result[0]).assertEqual(0xc2)
    })

    /**
     * @tc.name: testEncode008
     * @tc.desc: Returns the result of encoder.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncode008', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var result = new Uint8Array(buffer)
        result = that.encode('$$')
        expect(result[0]).assertEqual(0x24)
    })

    /**
     * @tc.name: testEncodeInto001
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto001', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var dest = new Uint8Array(buffer, 0, 13)
        var result = new Uint32Array(20)
        result = that.encodeInto('\uD800A\uDF06A楼HiA', dest)
        expect(result.read).assertEqual(7)
        expect(result.written).assertEqual(13)
    })

    /**
     * @tc.name: testEncodeInto002
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto002', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(6)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('abc\u2603d', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(6)
    })

    /**
     * @tc.name: testEncodeInto003
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto003', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(4)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('abcd', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(4)
    })

    /**
     * @tc.name: testEncodeInto004
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto004', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(4)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('12345', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(4)
    })

    /**
     * @tc.name: testEncodeInto005
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto005', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(4)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('123 4*!@#', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(4)
    })

    /**
     * @tc.name: testEncodeInto006
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto006', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(4)
        var dest = new Uint8Array(buffer)
        var result = new Object()
        result = that.encodeInto('', dest)
        expect(result.read).assertEqual(0)
        expect(result.written).assertEqual(0)
    })

    /**
     * @tc.name: testEncodeInto007
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto007', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var dest = new Uint8Array(buffer)
        var result = new Uint32Array(20)
        result = that.encodeInto('12ab', dest)
        expect(result.read).assertEqual(4)
        expect(result.written).assertEqual(4)
    })

    /**
     * @tc.name: testEncodeInto008
     * @tc.desc: encode string, write the result to dest array.
     * @tc.require: AR000GFB4U
     * @tc.author: wangben
     */
    it('testEncodeInto008', 0, function () {
        var that = new util.TextEncoder()
        var buffer = new ArrayBuffer(20)
        var dest = new Uint8Array(buffer, 0, 0)
        var result = new Uint32Array(20)
        result = that.encodeInto('\uD800A\uDF06A楼HiA', dest)
        expect(result.read).assertEqual(0)
        expect(result.written).assertEqual(0)
    })
})

describe('ScopeTest', function () {

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-001
     * @tc.desc: Obtains the lower bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-001', 0, function () {
        var result = range.getLower()
        expect(result.toString()).assertEqual('30')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-002
     * @tc.desc: Obtains the lower bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-002', 0, function () {
        var result = rangeFir.getLower()
        expect(result.toString()).assertEqual('35')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-003
     * @tc.desc: Obtains the lower bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-003', 0, function () {
        var result = rangeSec.getLower()
        expect(result.toString()).assertEqual('20')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-004
     * @tc.desc: Obtains the lower bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-004', 0, function () {
        var result = rangeThi.getLower()
        expect(result.toString()).assertEqual('20')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-005
     * @tc.desc: Obtains the lower bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-005', 0, function () {
        var result = rangeFif.getLower()
        expect(result.toString()).assertEqual('35')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-006
     * @tc.desc: Obtains the upper bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-006', 0, function () {
        var result = range.getUpper()
        expect(result.toString()).assertEqual('40')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-007
     * @tc.desc: Obtains the upper bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-007', 0, function () {
        var result = rangeFir.getUpper()
        expect(result.toString()).assertEqual('39')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-008
     * @tc.desc: Obtains the upper bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-008', 0, function () {
        var result = rangeSec.getUpper()
        expect(result.toString()).assertEqual('45')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-009
     * @tc.desc: Obtains the upper bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-009', 0, function () {
        var result = rangeThi.getUpper()
        expect(result.toString()).assertEqual('35')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-010
     * @tc.desc: Obtains the upper bound of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-010', 0, function () {
        var result = rangeFif.getUpper()
        expect(result.toString()).assertEqual('45')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-011
     * @tc.desc: Clamps a given value to the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-011', 0, function () {
        var result =range.clamp(tempLess)
        expect(result.toString()).assertEqual('30')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-012
     * @tc.desc: Clamps a given value to the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-012', 0, function () {
        var result =range.clamp(tempMiDF)
        expect(result.toString()).assertEqual('35')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-013
     * @tc.desc: Clamps a given value to the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-013', 0, function () {
        var result =range.clamp(tempMore)
        expect(result.toString()).assertEqual('40')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-014
     * @tc.desc: Clamps a given value to the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-014', 0, function () {
        var result =range.clamp(tempLower)
        expect(result.toString()).assertEqual('30')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-015
     * @tc.desc: Clamps a given value to the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-015', 0, function () {
        var result =range.clamp(tempUpper)
        expect(result.toString()).assertEqual('40')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-016
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-016', 0, function () {
        var result = range.contains(tempLess)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-017
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-017', 0, function () {
        var result = range.contains(tempMiDF)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-018
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-018', 0, function () {
        var result = range.contains(tempMore)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-019
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-019', 0, function () {
        var result = range.contains(tempLower)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-020
     * @tc.desc: Checks whether a given value is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-020', 0, function () {
        var result = range.contains(tempUpper)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-021
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-021', 0, function () {
        var result = range.contains(rangeFir)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-022
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-022', 0, function () {
        var result = range.contains(rangeSec)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-023
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-023', 0, function () {
        var result = range.contains(rangeThi)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-024
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-024', 0, function () {
        var result = range.contains(rangeFif)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-025
     * @tc.desc: Checks whether a given range is within the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-025', 0, function () {
        var result = range.contains(range)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-026
     * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-026', 0, function () {
        var result = range.expand(tempMiDF, tempMidS)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-027
     * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-027', 0, function () {
        var result = range.expand(tempLess, tempMore)
        expect(result.toString()).assertEqual('[20, 45]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-028
     * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-028', 0, function () {
        var result = range.expand(tempLess, tempMiDF)
        expect(result.toString()).assertEqual('[20, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-029
     * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-029', 0, function () {
        var result = range.expand(tempMiDF, tempMore)
        expect(result.toString()).assertEqual('[30, 45]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-030
     * @tc.desc: Creates the smallest range that includes the current range and the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-030', 0, function () {
        var result = range.expand(tempLower, tempUpper)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-031
     * @tc.desc: Creates the smallest range that includes the current range and a given range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-031', 0, function () {
        var result = range.expand(rangeFir)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-032
     * @tc.desc: Creates the smallest range that includes the current range and a given range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-032', 0, function () {
        var result = range.expand(rangeSec)
        expect(result.toString()).assertEqual('[20, 45]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-033
     * @tc.desc: Creates the smallest range that includes the current range and a given range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-033', 0, function () {
        var result = range.expand(rangeThi)
        expect(result.toString()).assertEqual('[20, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-034
     * @tc.desc: Creates the smallest range that includes the current range and a given range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-034', 0, function () {
        var result = range.expand(rangeFif)
        expect(result.toString()).assertEqual('[30, 45]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-035
     * @tc.desc: Creates the smallest range that includes the current range and a given range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-035', 0, function () {
        var result = range.expand(range)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-036
     * @tc.desc: Creates the smallest range that includes the current range and a given value.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-036', 0, function () {
        var result = range.expand(tempMiDF)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-037
     * @tc.desc: Creates the smallest range that includes the current range and a given value.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-037', 0, function () {
        var result = range.expand(tempLess)
        expect(result.toString()).assertEqual('[20, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-038
     * @tc.desc: Creates the smallest range that includes the current range and a given value.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-038', 0, function () {
        var result = range.expand(tempMore)
        expect(result.toString()).assertEqual('[30, 45]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-039
     * @tc.desc: Creates the smallest range that includes the current range and a given value.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-039', 0, function () {
        var result = range.expand(tempLower)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-040
     * @tc.desc: Creates the smallest range that includes the current range and a given value.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-040', 0, function () {
        var result = range.expand(tempUpper)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-041
     * @tc.desc: Returns the intersection of a given range and the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-041', 0, function () {
        var result = range.intersect(rangeFir)
        expect(result.toString()).assertEqual('[35, 39]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-042
     * @tc.desc: Returns the intersection of a given range and the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-042', 0, function () {
        var result = range.intersect(rangeSec)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-043
     * @tc.desc: Returns the intersection of a given range and the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-043', 0, function () {
        var result = range.intersect(rangeThi)
        expect(result.toString()).assertEqual('[30, 35]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-044
     * @tc.desc: Returns the intersection of a given range and the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-044', 0, function () {
        var result = range.intersect(rangeFif)
        expect(result.toString()).assertEqual('[35, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-045
     * @tc.desc: Returns the intersection of a given range and the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-045', 0, function () {
        var result = range.intersect(range)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-046
     * @tc.desc: Returns the intersection of the current range and the range specified by
	   the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-046', 0, function () {
        var result = range.intersect(tempMiDF, tempMidS)
        expect(result.toString()).assertEqual('[35, 39]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-047
     * @tc.desc: Returns the intersection of the current range and the range specified by
	   the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-047', 0, function () {
        var result = range.intersect(tempLess, tempMore)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-048
     * @tc.desc: Returns the intersection of the current range and the range specified by
	   the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-048', 0, function () {
        var result = range.intersect(tempLess, tempMiDF)
        expect(result.toString()).assertEqual('[30, 35]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-049
     * @tc.desc: Returns the intersection of the current range and the range specified by
	   the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-049', 0, function () {
        var result = range.intersect(tempMiDF, tempMore)
        expect(result.toString()).assertEqual('[35, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-050
     * @tc.desc: Returns the intersection of the current range and the range specified by
	   the given lower and upper bounds.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-050', 0, function () {
        var result = range.intersect(tempLower, tempUpper)
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-051
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-051', 0, function () {
        var result = range.toString()
        expect(result.toString()).assertEqual('[30, 40]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-052
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-052', 0, function () {
        var result = rangeFir.toString()
        expect(result.toString()).assertEqual('[35, 39]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-053
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-053', 0, function () {
        var result = rangeSec.toString()
        expect(result.toString()).assertEqual('[20, 45]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-054
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-054', 0, function () {
        var result = rangeThi.toString()
        expect(result.toString()).assertEqual('[20, 35]')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR36-055
     * @tc.desc: Obtains a string representation of the current range.
     * @tc.require: AR000GFB4U
     * @tc.author: jiangkai
     */
    it('SUB_Runtime_JSAPI_SR000GGR36-055', 0, function () {
        var result = rangeFif.toString()
        expect(result.toString()).assertEqual('[35, 45]')
    })
})

describe('Base64Test', function () {

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_001
     * @tc.desc: Encodes all bytes from the specified u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_001', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([115,49,51]);
        var rarray = new Uint8Array([99,122,69,122]);
        var result = that.encodeSync(array);
        for (var i = 0; i < 4; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_004
     * @tc.desc: Encodes all bytes from the specified u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_004', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115]);
        var rarray = new Uint8Array([81,109,70,122,90,84,89,48,73,69,53,118,90,71,85,117,97,110,77,61]);
        var result = that.encodeSync(array);
        for (var i = 0; i < 20; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_005
     * @tc.desc: Encodes all bytes from the specified u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_005', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66,97,115,101,54,52,32,69,110,99,111,100,105,110,103,32,105,
		110,32,78,111,100,101,46,106,115]);
        var rarray = new Uint8Array([81,109,70,122,90,84,89,48,73,69,86,117,89,50,57,107,97,87,53,110,
		73,71,108,117,73,69,53,118,90,71,85,117,97,110,77,61]);
        var result = that.encodeSync(array);
        for (var i = 0; i < 36; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_002
     * @tc.desc: Encodes all bytes from the specified u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_002', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([168, 174, 155, 255]);
        var rarray = new Uint8Array([113,75,54,98,47,119,61,61]);
        var result = that.encodeSync(array);
        for (var i = 0; i < 8; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_003
     * @tc.desc: Encodes all bytes from the specified u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_003', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52]);
        var rarray = new Uint8Array([81, 109, 70, 122, 90, 84, 89, 48]);
        var result = that.encodeSync(array);
        for (var i = 0; i <8; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_006
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_006', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([115,49,51]);
        var result = that.encodeToStringSync(array)
        expect(result).assertEqual('czEz')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_009
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_009', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115]);
        var result = that.encodeToStringSync(array);
        expect(result).assertEqual('QmFzZTY0IE5vZGUuanM=')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_010
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_010', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66,97,115,101,54,52,32,69,110,99,111,100,105,110,103,32,105,110,
		32,78,111,100,101,46,106,115]);
        var result = that.encodeToStringSync(array);
        expect(result).assertEqual('QmFzZTY0IEVuY29kaW5nIGluIE5vZGUuanM=')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_007
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_007', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([168, 174, 155, 255]);
        var result = that.encodeToStringSync(array);
        expect(result).assertEqual('qK6b/w==')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_008
     * @tc.desc: Encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_008', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52]);
        var result = that.encodeToStringSync(array);
        expect(result).assertEqual('QmFzZTY0')
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_011
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_011', 0, function () {
        var that = new util.Base64()
        var buff = 'czEz';
        var rarray = new Uint8Array([115,49,51]);
        var result = that.decodeSync(buff);
        for (var i = 0; i < 3; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_013
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_013', 0, function () {
        var that = new util.Base64()
        var buff = 'QmFzZTY0IE5vZGUuanM=';
        var rarray = new Uint8Array([66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115]);
        var result = that.decodeSync(buff);
        for (var i = 0; i < 14; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_014
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_014', 0, function () {
        var that = new util.Base64()
        var buff = 'QmFzZTY0IEVuY29kaW5nIGluIE5vZGUuanM=';
        var rarray = new Uint8Array([66,97,115,101,54,52,32,69,110,99,111,100,105,110,103,32,
		105,110,32,78,111,100,101,46,106,115]);
        var result = that.decodeSync(buff);
        for (var i = 0; i < 26; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_012
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_012', 0, function () {
        var that = new util.Base64()
        var buff = 'qK6b/w==';
        var rarray = new Uint8Array([168, 174, 155, 255]);
        var result = that.decodeSync(buff);
        for (var i = 0; i < 4; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: testDecodeSync005
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testDecodeSync005', 0, function () {
        var that = new util.Base64()
        var buff = 'QmFzZTY0';
        var rarray = new Uint8Array([66, 97, 115, 101, 54, 52]);
        var result = that.decodeSync(buff);
        for (var i = 0; i <6; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_011
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_011', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([99,122,69,122]);
        var rarray = new Uint8Array([115,49,51]);
        var result = that.decodeSync(array);
        for (var i = 0; i < 3; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_013
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_013', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([81,109,70,122,90,84,89,48,73,69,53,118,90,71,85,117,97,110,77,61]);
        var rarray = new Uint8Array([66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115]);
        var result = that.decodeSync(array);
        for (var i = 0; i < 14; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_014
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_014', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([81,109,70,122,90,84,89,48,73,69,86,117,89,50,57,107,97,87,
		53,110,73,71,108,117,73,69,53,118,90,71,85,117,97,110,77,61]);
        var rarray = new Uint8Array([66,97,115,101,54,52,32,69,110,99,111,100,105,110,103,32,105,
		110,32,78,111,100,101,46,106,115]);
        var result = that.decodeSync(array);
        for (var i = 0; i < 26; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_012
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_012', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([113,75,54,98,47,119,61,61]);
        var rarray = new Uint8Array([168, 174, 155, 255]);
        var result = that.decodeSync(array);
        for (var i = 0; i < 4; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    /**
     * @tc.name: testDecodeSync005
     * @tc.desc: Decodes a Base64 encoded String or input u8 array into a newly-allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testDecodeSync005', 0, function () {
        var that = new util.Base64()
        var array = new Uint8Array([81, 109, 70, 122, 90, 84, 89, 48]);
        var rarray = new Uint8Array([66, 97, 115, 101, 54, 52]);
        var result = that.decodeSync(array);
        for (var i = 0; i <6; i++) {
            expect(result[i]).assertEqual(rarray[i]);
        }
    })

    //base64 EncodeAsync test
    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_015
     * @tc.desc: Asynchronously encodes all bytes in the specified u8 array into the newly
	   allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_015', 0, async function () {
        var that = await new util.Base64();
        var array = new Uint8Array([115,49,51]);
        var rarray = new Uint8Array([99,122,69,122]);
        that.encode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_018
     * @tc.desc: Asynchronously encodes all bytes in the specified u8 array into the newly
	   allocated u8 array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_018', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115]);
        var rarray = new Uint8Array([81,109,70,122,90,84,89,48,73,69,53,118,90,71,85,117,97,110,77,61]);
        that.encode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_019
     * @tc.desc: Asynchronously encodes all bytes in the specified u8 array into the newly allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_019', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([66,97,115,101,54,52,32,69,110,99,111,100,105,110,103,32,105,110,32,
		78,111,100,101,46,106,115]);
        var rarray = new Uint8Array([81,109,70,122,90,84,89,48,73,69,86,117,89,50,57,107,97,87,53,110,73,
		71,108,117,73,69,53,118,90,71,85,117,97,110,77,61]);
        that.encode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_016
     * @tc.desc: Asynchronously encodes all bytes in the specified u8 array into the newly allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_016', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([168, 174, 155, 255]);
        var rarray = new Uint8Array([113,75,54,98,47,119,61,61]);
        that.encode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_017
     * @tc.desc: Asynchronously encodes all bytes in the specified u8 array into the newly allocated u8
	   array using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_017', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52]);
        var rarray = new Uint8Array([81, 109, 70, 122, 90, 84, 89, 48]);
        that.encode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_020
     * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_020', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([115,49,51]);
        that.encodeToString(array).then(val=>{
            expect(val).assertEqual('czEz')
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_023
     * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_023', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115]);
        that.encodeToString(array).then(val=>{
            expect(val).assertEqual('QmFzZTY0IE5vZGUuanM=')
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_024
     * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_024', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([81,109,70,122,90,84,89,48,73,69,86,117,89,50,57,107,97,87,53,110,73,71,108,
		117,73,69,53,118,90,71,85,117,97,110,77,61]);
        that.encodeToString(array).then(val=>{
            expect(val).assertEqual('QmFzZTY0IEVuY29kaW5nIGluIE5vZGUuanM=')
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_021
     * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_021', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([168, 174, 155, 255]);
        that.encodeToString(array).then(val=>{
            expect(val).assertEqual('qK6b/w==')
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_022
     * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_022', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([66, 97, 115, 101, 54, 52]);
        that.encodeToString(array).then(val=>{
            expect(val).assertEqual('QmFzZTY0')
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_025
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8
	   array into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_025', 0, async function () {
        var that = new util.Base64()
        var buff = 'czEz';
        var array = new Uint8Array([115,49,51]);
        that.decode(buff).then(val=>{
            for (var i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_027
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8
	   array into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_027', 0, async function () {
        var that = new util.Base64()
        var buff = 'QmFzZTY0IE5vZGUuanM=';
        var array = new Uint8Array([66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115]);
        that.decode(buff).then(val=>{
            for (var i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_028
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8
	   array into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_028', 0, async function () {
        var that = new util.Base64()
        var buff = 'QmFzZTY0IEVuY29kaW5nIGluIE5vZGUuanM=';
        var array = new Uint8Array([66,97,115,101,54,52,32,69,110,99,111,100,105,110,103,32,105,110,32,78,
		111,100,101,46,106,115]);
        that.decode(buff).then(val=>{
            for (var i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_026
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8
	   array into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_026', 0, async function () {
        var that = new util.Base64()
        var buff = 'qK6b/w==';
        var array = new Uint8Array([168, 174, 155, 255]);
        that.decode(buff).then(val=>{
            for (var i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        })
    })

    /**
     * @tc.name: testDecode005
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8
	   array into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testDecode005', 0, async function () {
        var that = new util.Base64()
        var buff = 'QmFzZTY0';
        var rarray = new Uint8Array([66, 97, 115, 101, 54, 52]);
        that.decode(buff).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_025
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8
	   array into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_025', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([99,122,69,122]);
        var rarray = new Uint8Array([115,49,51]);
        that.decode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_027
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8
	   array into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_027', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([81,109,70,122,90,84,89,48,73,69,53,118,90,71,85,117,97,110,77,61]);
        var rarray = new Uint8Array([66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115]);
        that.decode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_028
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8
	   array into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_028', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([81,109,70,122,90,84,89,48,73,69,86,117,89,50,57,107,97,87,53,110,73,71,108,
		117,73,69,53,118,90,71,85,117,97,110,77,61]);
        var rarray = new Uint8Array([66,97,115,101,54,52,32,69,110,99,111,100,105,110,103,32,105,110,32,78,111,
		100,101,46,106,115]);
        that.decode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR37_026
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8 array
	   into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('SUB_Runtime_JSAPI_SR000GGR37_026', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([113,75,54,98,47,119,61,61]);
        var rarray = new Uint8Array([168, 174, 155, 255]);
        that.decode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })

    /**
     * @tc.name: testDecode005
     * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8
	   array into a newly allocated u8 array.
     * @tc.require: AR000GFB2Q
     * @tc.author: bihu
     */
    it('testDecode005', 0, async function () {
        var that = new util.Base64()
        var array = new Uint8Array([81, 109, 70, 122, 90, 84, 89, 48]);
        var rarray = new Uint8Array([66, 97, 115, 101, 54, 52]);
        that.decode(array).then(val=>{
            for (var i = 0; i < rarray.length; i++) {
                expect(val[i]).assertEqual(rarray[i])
            }
        })
    })
})

describe('RationalNumberFunTest', function () {

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_001
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_001', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('-1:2')
        var result1 = res.valueOf()
        expect(result1).assertEqual(-0.5)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_002
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_002', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('+3/4')
        var result1 = res.valueOf()
        expect(result1).assertEqual(0.75)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_003
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_003', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('+3:-4')
        var result1 = res.valueOf()
        expect(result1).assertEqual(-0.75)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_004
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_004', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('+2:4')
        var result1 = res.valueOf()
        expect(result1).assertEqual(0.5)
    })

    /**
     * @tc.name: testRationalNumberCreateRationalFromString005
     * @tc.desc: Creates a RationalNumber object based on a given string.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberCreateRationalFromString005', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.createRationalFromString('+2:-4')
        var result1 = res.valueOf()
        expect(result1).assertEqual(-0.5)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_010
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_010', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(3, 4)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_007
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_007', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(0, 0)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(-1)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_011
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_011', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(8, 3)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(-1)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_005
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_005', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(2, 1)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(0)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_006
     * @tc.desc: Compares the current RationalNumber object with a given object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_006', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var proc = new util.RationalNumber(2, 1)
        var res = pro.compareTo(proc)
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_016
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_016', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(3, 4)
        var res = pro.equals(proc)
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_014
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_014', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var proc = new util.RationalNumber(4, 2)
        var res = pro.equals(proc)
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberEquals003
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberEquals003', 0, function () {
        var pro = new util.RationalNumber(0, 1)
        var proc = new util.RationalNumber(0, 2)
        var res = pro.equals(proc)
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberEquals004
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberEquals004', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var proc = new util.RationalNumber(0, 2)
        var res = pro.equals(proc)
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_015
     * @tc.desc: Checks whether a given object is the same as the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_015', 0, function () {
        var pro = new util.RationalNumber(-2, 0)
        var proc = new util.RationalNumber(2, 0)
        var res = pro.equals(proc)
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberValueOf001
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberValueOf001', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var res = pro.valueOf()
        expect(res).assertEqual(2)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_012
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_012', 0, function () {
        var pro = new util.RationalNumber(2, 10)
        var res = pro.valueOf()
        expect(res).assertEqual(0.2)
    })

    /**
     * @tc.name: testRationalNumberValueOf003
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberValueOf003', 0, function () {
        var pro = new util.RationalNumber(1, 2)
        var res = pro.valueOf()
        expect(res).assertEqual(0.5)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_013
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_013', 0, function () {
        var pro = new util.RationalNumber(4, 2)
        var res = pro.valueOf()
        expect(res).assertEqual(2)
    })

    /**
     * @tc.name: testRationalNumberValueOf005
     * @tc.desc: Obtains the value of the current RationalNumber object as a number.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberValue005', 0, function () {
        var pro = new util.RationalNumber(4, 1)
        var res = pro.valueOf()
        expect(res).assertEqual(4)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_017
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_017', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(4, 8)
        expect(res).assertEqual(4)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_018
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_018', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(10, 15)
        expect(res).assertEqual(5)
    })

    /**
     * @tc.name: testRationalNumberGetCommonDivisor003
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetCommonDivisor003', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(8, 4)
        expect(res).assertEqual(4)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_019
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_019', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(8, 16)
        expect(res).assertEqual(8)
    })

    /**
     * @tc.name: testRationalNumberGetCommonDivisor005
     * @tc.desc: Obtains the greatest common divisor of two specified numbers.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetCommonDivisor005', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.getCommonDivisor(2, 16)
        expect(res).assertEqual(2)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_023
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_023', 0, function () {
        var pro = new util.RationalNumber(2, 1)
        var res = pro.getDenominator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberGetDenominator002
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetDenominator002', 0, function () {
        var pro = new util.RationalNumber(2, 3)
        var res = pro.getDenominator()
        expect(res).assertEqual(3)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_025
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_025', 0, function () {
        var pro = new util.RationalNumber(2, 0)
        var res = pro.getDenominator()
        expect(res).assertEqual(0)
    })

    /**
     * @tc.name: testRationalNumberGetDenominator004
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetDenominator004', 0, function () {
        var pro = new util.RationalNumber(10, 5)
        var res = pro.getDenominator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberGetDenominator005
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetDenominator005', 0, function () {
        var pro = new util.RationalNumber(6, 3)
        var res = pro.getDenominator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_024
     * @tc.desc: Obtains the denominator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_024', 0, function () {
        var pro = new util.RationalNumber(6, -3)
        var res = pro.getDenominator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_021
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_021', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.getNumerator()
        expect(res).assertEqual(-2)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_022
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_022', 0, function () {
        var pro = new util.RationalNumber(0, 3)
        var res = pro.getNumerator()
        expect(res).assertEqual(0)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_020
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_020', 0, function () {
        var pro = new util.RationalNumber(2, 4)
        var res = pro.getNumerator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberGetNumerator004
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetNumerator004', 0, function () {
        var pro = new util.RationalNumber(3, 6)
        var res = pro.getNumerator()
        expect(res).assertEqual(1)
    })

    /**
     * @tc.name: testRationalNumberGetNumerator005
     * @tc.desc: Obtains the numerator of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberGetNumerator005', 0, function () {
        var pro = new util.RationalNumber(10, 5)
        var res = pro.getNumerator()
        expect(res).assertEqual(2)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_028
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_028', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.isFinite()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberIsFinite002
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsFinite002', 0, function () {
        var pro = new util.RationalNumber(0, 3)
        var res = pro.isFinite()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_026
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_026', 0, function () {
        var pro = new util.RationalNumber(2, 0)
        var res = pro.isFinite()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_027
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_027', 0, function () {
        var pro = new util.RationalNumber(1, 3)
        var res = pro.isFinite()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: testRationalNumberIsFinite005
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsFinite005', 0, function () {
        var pro = new util.RationalNumber(10, 5)
        var res = pro.isFinite()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_030
     * @tc.desc: Checks whether the current RationalNumber object represents a finite value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_030', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.isNaN()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_031
     * @tc.desc: Checks whether the current RationalNumber object represents a Not-a-Number (NaN) value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_031', 0, function () {
        var pro = new util.RationalNumber(0, 3)
        var res = pro.isNaN()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_029
     * @tc.desc: Checks whether the current RationalNumber object represents a Not-a-Number (NaN) value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_029', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.isNaN()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_032
     * @tc.desc: Checks whether the current RationalNumber object represents a Not-a-Number (NaN) value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_032', 0, function () {
        var pro = new util.RationalNumber(10, 0)
        var res = pro.isNaN()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsNaN005
     * @tc.desc: Checks whether the current RationalNumber object represents a Not-a-Number (NaN) value.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsNaN005', 0, function () {
        var pro = new util.RationalNumber(10, 1)
        var res = pro.isNaN()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_034
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_034', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.isZero()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_033
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_033', 0, function () {
        var pro = new util.RationalNumber(0, 3)
        var res = pro.isZero()
        expect(res).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_035
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_035', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.isZero()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_036
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_036', 0, function () {
        var pro = new util.RationalNumber(10, 2)
        var res = pro.isZero()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberIsZero005
     * @tc.desc: Checks whether the current RationalNumber object represents the value 0.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberIsZero005', 0, function () {
        var pro = new util.RationalNumber(1, 1)
        var res = pro.isZero()
        expect(res).assertEqual(false)
    })

    /**
     * @tc.name: testRationalNumberToString001
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('testRationalNumberToString001', 0, function () {
        var pro = new util.RationalNumber(-2, 1)
        var res = pro.toString()
        expect(res).assertEqual("-2/1")
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_037
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_037', 0, function () {
        var pro = new util.RationalNumber(0, 0)
        var res = pro.toString()
        expect(res).assertEqual("NaN")
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_038
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_038', 0, function () {
        var pro = new util.RationalNumber(3, 0)
        var res = pro.toString()
        expect(res).assertEqual("Infinity")
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_039
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_039', 0, function () {
        var pro = new util.RationalNumber(-3, 0)
        var res = pro.toString()
        expect(res).assertEqual("-Infinity")
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR38_040
     * @tc.desc: Obtains a string representation of the current RationalNumber object.
     * @tc.require: AR000GFB5A
     * @tc.author: liuqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR38_040', 0, function () {
        var pro = new util.RationalNumber(2, 3)
        var res = pro.toString()
        expect(res).assertEqual('2/3')
    })
})

describe('LruBufferFunTest', function () {

    /**
     * @tc.name: testLruBufferUpdateGetCapacity001
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateGetCapacity001', 0, function () {
        var that = new util.LruBuffer()
        that.updateCapacity(10)
        var result = that.getCapacity()
        expect(result).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferUpdateGetCapacity002
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateGetCapacity002', 0, function () {
        var that = new util.LruBuffer()
        that.updateCapacity(2147483646)
        var result = that.getCapacity()
        expect(result).assertEqual(2147483646)
    })

    /**
     * @tc.name: testLruBufferUpdateGetCapacity003
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateGetCapacity003', 0, function () {
        var that = new util.LruBuffer(100)
        that.updateCapacity(20)
        var result = that.getCapacity()
        expect(result).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferUpdateGetCapacity004
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateGetCapacity004', 0, function () {
        var that = new util.LruBuffer(50)
        that.updateCapacity(2)
        var result = that.getCapacity()
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferUpdateGetCapacity005
     * @tc.desc: Updates the buffer capacity to a specified capacity.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferUpdateGetCapacity005', 0, function () {
        var that = new util.LruBuffer(200)
        that.updateCapacity(100)
        var result = that.getCapacity()
        expect(result).assertEqual(100)
    })

    /**
     * @tc.name: testLruBufferGet001
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet001', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        var result = that.get(1)
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferGet002
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet002', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        var result = that.get(5)
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferGet003
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet003', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        var result1 = that.get(20)
        var result2 = that.get('abcd')
        var result3 = that.get(2)
        expect(result1).assertEqual(undefined)
        expect(result2).assertEqual(15)
        expect(result3).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferGet004
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet004', 0, function () {
        var that = new util.LruBuffer()
        that.put('1111','bcjdshc')
        that.put(20,'cdjcaxb')
        that.put('abcd',15)
        var result1 = that.get('1111')
        var result2 = that.get(20)
        var result3 = that.get('abcd')
        var result4 = that.get(25)
        expect(result1).assertEqual('bcjdshc')
        expect(result2).assertEqual('cdjcaxb')
        expect(result3).assertEqual(15)
        expect(result4).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferGet005
     * @tc.desc: Obtains the value associated with a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGet005', 0, function () {
        var that = new util.LruBuffer()
        that.put('1111','bcjdshc')
        that.put(20,'cdjcaxb')
        that.updateCapacity(2)
        that.put('abcd',15)
        var result1 = that.get('1111')
        var result2 = that.get(20)
        var result3 = that.get('abcd')
        var result4 = that.get(25)
        expect(result1).assertEqual(undefined)
        expect(result2).assertEqual('cdjcaxb')
        expect(result3).assertEqual(15)
        expect(result4).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferPut001
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut001', 0, function () {
        var that = new util.LruBuffer()
        var temp = that.put('1111','bcjdshc')
        expect(temp).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferPut002
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut002', 0, function () {
        var that = new util.LruBuffer()
        var temp1 = that.put('1111','bcjdshc')
        var temp2 = that.put('1111',13)
        expect(temp2).assertEqual('bcjdshc')
    })

    /**
     * @tc.name: testLruBufferPut003
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut003', 0, function () {
        var that = new util.LruBuffer()
        var temp = that.put('1111','bcjdshc')
        var temp1 = that.put(1,12)
        var temp2 = that.put(2,5)
        var temp3 = that.put(2,'adasfdad')
        var temp4 = that.put('abc',10)
        expect(temp1).assertEqual(undefined)
        expect(temp2).assertEqual(undefined)
        expect(temp3).assertEqual(5)
        expect(temp4).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferPut004
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut004', 0, function () {
        var that = new util.LruBuffer()
        var temp = that.put('1111','bcjdshc')
        var temp1 = that.put(1,12)
        var temp2 = that.put(2,5)
        that.updateCapacity(2)
        var temp3 = that.put(2,'adasfdad')
        var temp4 = that.put('1111',10)
        expect(temp).assertEqual(undefined)
        expect(temp1).assertEqual(undefined)
        expect(temp2).assertEqual(undefined)
        expect(temp3).assertEqual(5)
        expect(temp4).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferPut005
     * @tc.desc: Adds a key-value pair to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferPut005', 0, function () {
        var that = new util.LruBuffer()
        var temp = that.put('1111','bcjdshc')
        var temp1 = that.put(1,12)
        var temp2 = that.put(2,5)
        var temp3 = that.put(1,10)
        var temp4 = that.put(2,22)
        var temp5 = that.put(2,30)
        expect(temp).assertEqual(undefined)
        expect(temp1).assertEqual(undefined)
        expect(temp2).assertEqual(undefined)
        expect(temp3).assertEqual(12)
        expect(temp4).assertEqual(5)
        expect(temp5).assertEqual(22)
    })

    /**
     * @tc.name: testLruBufferGetCreateCount001
     * @tc.desc: Obtains the number of times createDefault(Object) returned a value.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCreateCount001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(3)
        that.get(5)
        that.get(10)
        var result = that.getCreateCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetCreateCount002
     * @tc.desc: Obtains the number of times createDefault(Object) returned a value.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCreateCount002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(6)
        that.get(8)
        that.get(20)
        var result = that.getCreateCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetCreateCount003
     * @tc.desc: Obtains the number of times createDefault(Object) returned a value.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCreateCount003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.get('abc')
        that.get('cndk')
        var result = that.getCreateCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetCreateCount004
     * @tc.desc: Obtains the number of times createDefault(Object) returned a value.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCreateCount004', 0, function () {
        var that = new util.LruBuffer()
        that.put(5,2)
        that.put(10,10)
        that.put('abcd','abcd')
        that.get(3)
        that.get(20)
        that.get(12)
        var result = that.getCreateCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetCreateCount005
     * @tc.desc: Obtains the number of times createDefault(Object) returned a value.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCreateCount005', 0, function () {
        var that = new util.LruBuffer()
        that.put('xsjk','bcjds')
        that.put('ajc',10)
        that.put('abcd',15)
        that.get(2)
        that.get(10)
        that.get(30)
        var result = that.getCreateCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetMissCount001
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(3)
        that.get(5)
        that.get(10)
        var result = that.getMissCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMissCount002
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount002', 0, function () {
        var that = new util.LruBuffer()
        that.put(5,2)
        that.put(1,10)
        that.put('abcd','bcjsc')
        that.get(2)
        that.get(10)
        that.get(15)
        var result = that.getMissCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMissCount003
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount003', 0, function () {
        var that = new util.LruBuffer()
        that.put(10,10)
        that.put(3,20)
        that.put(12,16)
        that.get(1)
        that.get(2)
        that.get(6)
        var result = that.getMissCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMissCount004
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount004', 0, function () {
        var that = new util.LruBuffer()
        that.put('acb','asdc')
        that.put('vfb',1)
        that.put('abcd','abcd')
        that.get(3)
        that.get(5)
        that.get(10)
        var result = that.getMissCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMissCount005
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(3)
        that.get(5)
        that.get(10)
        that.contains(2)
        that.contains(6)
        var result = that.getMissCount()
        expect(result).assertEqual(4)
    })

    /**
     * @tc.name: testLruBufferGetMissCount006
     * @tc.desc: Obtains the number of times that the queried values are not matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMissCount006', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(1)
        that.get(2)
        that.get('abcd')
        var result = that.getMissCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount001
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.put(3,20)
        that.put(5,30)
        var result = that.getRemovalCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount002
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount002', 0, function () {
        var that = new util.LruBuffer()
        that.put(10,2)
        that.put(3,2)
        that.put(15,15)
        that.put(1,20)
        var result = that.getRemovalCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount003
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount003', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc',2)
        that.put('abcd','abcd')
        that.put(3,2)
        var result = that.getRemovalCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount004
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.updateCapacity(2)
        that.put(3,20)
        that.put(5,30)
        var result = that.getRemovalCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetRemovalCount005
     * @tc.desc: Obtains the number of times that values are evicted from the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetRemovalCount005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.updateCapacity(3)
        that.put('string',20)
        that.put('abc',30)
        that.put(10,20)
        var result = that.getRemovalCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount001
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(1)
        that.get(2)
        that.get('abcd')
        var result = that.getMatchCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount002
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount002', 0, function () {
        var that = new util.LruBuffer()
        that.put(10,2)
        that.put(2,1)
        that.put('abcd',15)
        that.get(10)
        that.get(2)
        that.get('abcd')
        that.get('string')
        var result = that.getMatchCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount003
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount003', 0, function () {
        var that = new util.LruBuffer()
        that.put(0,1)
        that.put(30,10)
        that.put('abcd',15)
        that.get(0)
        that.get(30)
        that.get('abcd')
        var result = that.getMatchCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount004
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(1)
        that.get(2)
        that.get('abcd')
        that.contains(2)
        that.contains('abcd')
        that.contains(20)
        var result = that.getMatchCount()
        expect(result).assertEqual(5)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount005
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.put('string','string')
        that.get(1)
        that.get(2)
        that.get('abcd')
        that.get('string')
        that.contains(2)
        that.contains('abcd')
        that.contains('string')
        that.contains(10)
        var result = that.getMatchCount()
        expect(result).assertEqual(7)
    })

    /**
     * @tc.name: testLruBufferGetMatchCount006
     * @tc.desc: Obtains the number of times that the queried values are successfully matched.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetMatchCount006', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.get(5)
        var result = that.getMatchCount()
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferGetPutCount001
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.getPutCount()
        expect(result).assertEqual(1)
    })

    /**
     * @tc.name: testLruBufferGetPutCount002
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount002', 0, function () {
        var that = new util.LruBuffer()
        that.put(5,3)
        that.put(2,10)
        var result = that.getPutCount()
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferGetPutCount003
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount003', 0, function () {
        var that = new util.LruBuffer()
        that.put('string',2)
        that.put('abcd',1)
        that.put('ab','a')
        var result = that.getPutCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetPutCount004
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount004', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','string')
        that.put('ac','abc')
        that.put('abcd',15)
        var result = that.getPutCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetPutCount005
     * @tc.desc: Obtains the number of times that values are added to the buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetPutCount005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.updateCapacity(2)
        that.put('abcd','abc')
        var result = that.getPutCount()
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferGetCapacity001
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCapacity001', 0, function () {
        var that = new util.LruBuffer()
        var result = that.getCapacity()
        expect(result).assertEqual(64)
    })

    /**
     * @tc.name: testLruBufferGetCapacity002
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCapacity002', 0, function () {
        var that = new util.LruBuffer(2)
        var result = that.getCapacity()
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferGetCapacity003
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCapacity003', 0, function () {
        var that = new util.LruBuffer(100)
        var result = that.getCapacity()
        expect(result).assertEqual(100)
    })

    /**
     * @tc.name: testLruBufferGetCapacity004
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCapacity004', 0, function () {
        var that = new util.LruBuffer()
        that.updateCapacity(50)
        var result = that.getCapacity()
        expect(result).assertEqual(50)
    })

    /**
     * @tc.name: testLruBufferGetCapacity005
     * @tc.desc: Obtains the capacity of the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferGetCapacity005', 0, function () {
        var that = new util.LruBuffer(100)
        that.updateCapacity(10)
        var result = that.getCapacity()
        expect(result).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferLength001
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferLength001', 0, function () {
        var that = new util.LruBuffer()
        var result = that.length
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferLength002
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferLength002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.length
        expect(result).assertEqual(1)
    })

    /**
     * @tc.name: testLruBufferLength003
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferLength003', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc',2)
        that.put('string','string')
        var result = that.length
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferLength004
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferLength004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.put('abcd',20)
        var result = that.length
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferLength005
     * @tc.desc: Obtains the total number of values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferLength005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put('abcd',15)
        that.put('abcd',20)
        that.updateCapacity(3);
        that.put(5,33);
        that.put(25,35);
        var result = that.length
        expect(result).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferClear001
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result1 = that.length
        that.clear()
        var result2 = that.length
        expect(result1).assertEqual(1)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferClear002
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33);
        that.put(25,35);
        var result1 = that.length
        that.clear()
        var result2 = that.length
        expect(result1).assertEqual(4)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferClear003
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear003', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc',2)
        that.put(2,'abc')
        that.put('ancd','abcd')
        that.put(10,35)
        var result1 = that.length
        that.clear()
        var result2 = that.length
        expect(result1).assertEqual(4)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferClear004
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33);
        that.updateCapacity(3);
        var result1 = that.length
        that.clear()
        var result2 = that.length
        expect(result1).assertEqual(3)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferClear005
     * @tc.desc: Clears key-value pairs from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferClear005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33);
        that.put('abcd','string');
        that.updateCapacity(2);
        var result1 = that.length
        that.clear()
        var result2 = that.length
        expect(result1).assertEqual(2)
        expect(result2).assertEqual(0)
    })

    /**
     * @tc.name: testLruBufferIsEmpty001
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.isEmpty()
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferIsEmpty002
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        var result = that.isEmpty()
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferIsEmpty003
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33)
        var result = that.isEmpty()
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferIsEmpty004
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty004', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc',2)
        that.put('string',10)
        that.put('ab','abc')
        var result = that.isEmpty()
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferIsEmpty005
     * @tc.desc: Checks whether the current buffer is empty.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferIsEmpty005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(2,10)
        that.put(5,33);
        that.clear()
        var result = that.isEmpty()
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testLruBufferContains001
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.contains(1)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testLruBufferContains002
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains002', 0, function () {
        var that = new util.LruBuffer()
        that.put(0,5)
        var result = that.contains(0)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testLruBufferContains003
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains003', 0, function () {
        var that = new util.LruBuffer()
        that.put('abc','abc')
        var result = that.contains(1)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferContains004
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put('abcd',20)
        var result1 = that.contains(1)
        var result2 = that.contains(20)
        expect(result1).assertEqual(true)
        expect(result2).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferContains005
     * @tc.desc: Checks whether the current buffer contains a specified key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferContains005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put('string','string')
        var result1 = that.contains(1)
        var result2 = that.contains('string')
        var result3 = that.contains(0)
        expect(result1).assertEqual(true)
        expect(result2).assertEqual(true)
        expect(result3).assertEqual(false)
    })

    /**
     * @tc.name: testLruBufferRemove001
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove001', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result = that.remove(1)
        expect(result).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferRemove002
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put('abcd',20)
        var result1 = that.remove(1)
        var result2 = that.remove('abcd')
        expect(result1).assertEqual(2)
        expect(result2).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferRemove003
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        var result1 = that.remove(1)
        var result2 = that.remove(5)
        expect(result1).assertEqual(2)
        expect(result2).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferRemove004
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove004', 0, function () {
        var that = new util.LruBuffer()
        that.put(0,'abc')
        that.put('abcd',20)
        var result1 = that.remove(1)
        var result2 = that.remove('abcd')
        expect(result1).assertEqual(undefined)
        expect(result2).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferRemove005
     * @tc.desc: Deletes a specified key and its associated value from the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferRemove005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        var result1 = that.remove(1)
        var result2 = that.remove(3)
        var result3 = that.get(3)
        expect(result1).assertEqual(2)
        expect(result2).assertEqual(10)
        expect(result3).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault001
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault001', 0, function () {
        var that = new util.LruBuffer()
        var result = that.createDefault(1)
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault002
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault002', 0, function () {
        var that = new util.LruBuffer()
        var result = that.createDefault(0)
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault003
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault003', 0, function () {
        var that = new util.LruBuffer()
        var result = that.createDefault('string')
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault004
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault004', 0, function () {
        var that = new util.LruBuffer()
        var result = that.createDefault(10)
        expect(result).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferCreateDefault005
     * @tc.desc: Executes subsequent operations if miss to compute a value for the specific key.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferCreateDefault005', 0, function () {
        var that = new util.LruBuffer()
        var result1 = that.createDefault('abc')
        var result2 = that.createDefault('ab')
        expect(result1).assertEqual(undefined)
        expect(result2).assertEqual(undefined)
    })

    /**
     * @tc.name: testLruBufferKeys001
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys001', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','abc')
        var result = that.keys()
        expect(result[0]).assertEqual('string')
    })

    /**
     * @tc.name: testLruBufferKeys002
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        var result = that.keys()
        expect(result[0]).assertEqual(1)
        expect(result[1]).assertEqual(3)
        expect(result[2]).assertEqual(5)
        expect(result[3]).assertEqual('abc')
    })

    /**
     * @tc.name: testLruBufferKeys003
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var result = that.keys()
        expect(result[0]).assertEqual(1)
        expect(result[1]).assertEqual(5)
        expect(result[2]).assertEqual('abc')
        expect(result[3]).assertEqual(3)
    })

    /**
     * @tc.name: testLruBufferKeys004
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.contains(1)
        var result = that.keys()
        expect(result[0]).assertEqual(5)
        expect(result[1]).assertEqual('abc')
        expect(result[2]).assertEqual(3)
        expect(result[3]).assertEqual(1)
    })

    /**
     * @tc.name: testLruBufferKeys005
     * @tc.desc: Obtains a list of keys for the values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferKeys005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.remove(5)
        that.contains(3)
        that.get(1)
        that.contains('abc')
        var result = that.keys()
        expect(result[0]).assertEqual(3)
        expect(result[1]).assertEqual(1)
        expect(result[2]).assertEqual('abc')
    })

    /**
     * @tc.name: testLruBufferValues001
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues001', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','abc')
        var result = that.values()
        expect(result[0]).assertEqual('abc')
    })

    /**
     * @tc.name: testLruBufferValues002
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        var result = that.values()
        expect(result[0]).assertEqual(2)
        expect(result[1]).assertEqual(10)
        expect(result[2]).assertEqual(15)
        expect(result[3]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferValues003
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var result = that.values()
        expect(result[0]).assertEqual(2)
        expect(result[1]).assertEqual(15)
        expect(result[2]).assertEqual(20)
        expect(result[3]).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferValues004
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.contains(1)
        var result = that.values()
        expect(result[0]).assertEqual(15)
        expect(result[1]).assertEqual(20)
        expect(result[2]).assertEqual(10)
        expect(result[3]).assertEqual(2)
    })

    /**
     * @tc.name: testLruBufferValues005
     * @tc.desc: Obtains a list of all values in the current buffer.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferValues005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.remove(5)
        that.contains(3)
        that.get(1)
        that.contains('abc')
        var result = that.values()
        expect(result[0]).assertEqual(10)
        expect(result[1]).assertEqual(2)
        expect(result[2]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferToString001
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString001', 0, function () {
        var that = new util.LruBuffer()
        that.put('1111','bcjdshc')
        that.put(1,2)
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 64, hits = 0, misses = 0, hitRate = 0% ]')
    })

    /**
     * @tc.name: testLruBufferToString002
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString002', 0, function () {
        var that = new util.LruBuffer()
        that.put('1111','bcjdshc')
        that.put(1,2)
        that.get(1)
        that.get('1111')
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 64, hits = 2, misses = 0, hitRate = 100% ]')
    })

    /**
     * @tc.name: testLruBufferToString003
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString003', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 100, hits = 1, misses = 0, hitRate = 100% ]')
    })

    /**
     * @tc.name: testLruBufferToString004
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString004', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.get(2)
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 100, hits = 1, misses = 1, hitRate = 50% ]')
    })

    /**
     * @tc.name: testLruBufferToString005
     * @tc.desc: Returns a string representation of the object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferToString005', 0, function () {
        var that = new util.LruBuffer(100)
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.get(2)
        that.get(1)
        that.contains(5)
        var result = that.toString()
        expect(result).assertEqual('Lrubuffer[ maxSize = 100, hits = 3, misses = 1, hitRate = 75% ]')
    })

    /**
     * @tc.name: testLruBufferEntries001
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries001', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','abc')
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[1]).assertEqual('abc');
    })

    /**
     * @tc.name: testLruBufferEntries002
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[1]).assertEqual(2);
    })

    /**
     * @tc.name: testLruBufferEntries003
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[0]).assertEqual(1);
    })

    /**
     * @tc.name: testLruBufferEntries004
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.contains(1)
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[1]).assertEqual(15)
        expect(arr[3]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferEntries005
     * @tc.desc: Returns an array of key-value pairs of enumeratable properties of a given object.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferEntries005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.remove(5)
        that.contains(3)
        that.get(1)
        that.contains('abc')
        var i=0;
        var arr={};
        for (let entry of that.entries()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[1]).assertEqual(10)
        expect(arr[3]).assertEqual(2)
        expect(arr[5]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]001
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]001', 0, function () {
        var that = new util.LruBuffer()
        that.put('string','abc')
        that.put('abc',20)
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[1]).assertEqual('abc');
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]002
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]002', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[1]).assertEqual(2);
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]003
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]003', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[0]).assertEqual(1);
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]004
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]004', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.contains(1)
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[1]).assertEqual(15)
        expect(arr[3]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBuffer[Symbol.iterator]005
     * @tc.desc: Returns a two - dimensional array in the form of key - value pairs.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBuffer[Symbol.iterator]005', 0, function () {
        var that = new util.LruBuffer()
        that.put(1,2)
        that.put(3,10)
        that.put(5,15)
        that.put('abc',20)
        that.get(3)
        that.remove(5)
        that.contains(3)
        that.get(1)
        that.contains('abc')
        var i=0;
        var arr={};
        for (let entry of that[Symbol.iterator]()) {
            arr[i]=entry[0];
            i++;
            arr[i]=entry[1];
            i++;
        }
        expect(arr[1]).assertEqual(10)
        expect(arr[3]).assertEqual(2)
        expect(arr[5]).assertEqual(20)
    })

    /**
     * @tc.name: testLruBufferAfterRemoval001
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval001', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === false)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(false,10,30,null)
        expect(arr[0]).assertEqual(10)
    })

    /**
     * @tc.name: testLruBufferAfterRemoval002
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval002', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === false)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(false,'string',10,null)
        expect(arr[0]).assertEqual('string')
    })

    /**
     * @tc.name: testLruBufferAfterRemoval003
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval003', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === false)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(false,10,30,12)
        expect(arr[2]).assertEqual(12)
    })

    /**
     * @tc.name: testLruBufferAfterRemoval004
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval004', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === false)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(false,'abc',30,'string')
        expect(arr[1]).assertEqual(30)
    })

    /**
     * @tc.name: testLruBufferAfterRemoval005
     * @tc.desc: Executes subsequent operations after a value is deleted.
     * @tc.require: AR000GFB59
     * @tc.author: lixingyang
     */
    it('testLruBufferAfterRemoval005', 0, function () {
        var arr = [];
        class ChildLruBuffer extends util.LruBuffer
        {
            constructor()
            {
                super();
            }
            static getInstance()
            {
                if(this.instance ==  null)
                {
                    this.instance = new ChildLruBuffer();
                }
                return this.instance;
            }
            afterRemoval(isEvict, key, value, newValue)
            {
                if (isEvict === true)
                {
                    arr = [key, value, newValue];
                }
            }
        }
        ChildLruBuffer.getInstance().afterRemoval(true,'abc','ab','string')
        expect(arr[2]).assertEqual('string')
    })
})

describe('TypesTest', function() {

    /**
     * @tc.name: testIsAnyArrayBuffer001
     * @tc.desc: Check whether the entered value is of arraybuffer or sharedarraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsAnyArrayBuffer001', 0, function() {
        var proc = new util.Types();
        var result = proc.isAnyArrayBuffer(new ArrayBuffer([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsAnyArrayBuffer002
     * @tc.desc: Check whether the entered value is of arraybuffer or sharedarraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsAnyArrayBuffer002', 0, function() {
        var proc = new util.Types();
        var result = proc.isAnyArrayBuffer(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsAnyArrayBuffer003
     * @tc.desc: Check whether the entered value is of arraybuffer or sharedarraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsAnyArrayBuffer003', 0, function() {
        var proc = new util.Types();
        var result = proc.isAnyArrayBuffer(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsAnyArrayBuffer004
     * @tc.desc: Check whether the entered value is of arraybuffer or sharedarraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsAnyArrayBuffer004', 0, function() {
        var proc = new util.Types();
        var buf = new ArrayBuffer();
        var result = proc.isAnyArrayBuffer(buf);
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsAnyArrayBuffer005
     * @tc.desc: Check whether the entered value is of arraybuffer or sharedarraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsAnyArrayBuffer005', 0, function() {
        var proc = new util.Types();
        var result = proc.isAnyArrayBuffer(new Boolean(false));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsArrayBufferView001
     * @tc.desc: Check whether the entered value is napi_ int8_ array or napi_ uint8_ array
	   or naPi_ uint8_ clamped_ array or naPi_ int16_ array or naPi_ uint16_ array or napi_ int32_ array or napi_
	   uint32_ array or napi_ float32_ array or napi_ float64_ array array or DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBufferView001', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBufferView(new Int8Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsArrayBufferView002
     * @tc.desc: Check whether the entered value is napi_ int8_ array or napi_ uint8_ array
	   or naPi_ uint8_ clamped_ array or naPi_ int16_ array or naPi_ uint16_ array or napi_
	   int32_ array or napi_ uint32_ array or napi_ float32_ array or napi_ float64_ array array or DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBufferView002', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBufferView(new Int32Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsArrayBufferView003
     * @tc.desc: Check whether the entered value is napi_ int8_ array or napi_ uint8_
	   array or naPi_ uint8_ clamped_ array or naPi_ int16_ array or naPi_ uint16_ array or napi_ int32_ array or
	   napi_ uint32_ array or napi_ float32_ array or napi_ float64_ array array or DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBufferView003', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBufferView(new DataView(new ArrayBuffer(16)));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsArrayBufferView004
     * @tc.desc: Check whether the entered value is napi_ int8_ array or napi_ uint8_ array or naPi_ uint8_
	   clamped_ array or naPi_ int16_ array or naPi_ uint16_ array or napi_ int32_ array or napi_ uint32_ array or
	   napi_ float32_ array or napi_ float64_ array array or DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBufferView004', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBufferView(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsArrayBufferView005
     * @tc.desc: Check whether the entered value is napi_ int8_ array or napi_ uint8_ array or naPi_ uint8_
	   clamped_ array or naPi_ int16_ array or naPi_ uint16_ array or napi_ int32_ array or napi_ uint32_ array or
	   napi_ float32_ array or napi_ float64_ array array or DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBufferView005', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBufferView(new Int16Array());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsArgumentsObject001
     * @tc.desc: Check whether the entered value is an arguments object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArgumentsObject001', 0, function() {
        var proc = new util.Types();
        function foo() {
            var result = proc.isArgumentsObject(arguments);
            expect(result).assertEqual(true);
        }
        var f = foo();
    })

    /**
     * @tc.name: testIsArgumentsObject002
     * @tc.desc: Check whether the entered value is an arguments object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArgumentsObject002', 0, function() {
        var proc = new util.Types();
        var result = proc.isArgumentsObject(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsArgumentsObject003
     * @tc.desc: Check whether the entered value is an arguments object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArgumentsObject003', 0, function() {
        var proc = new util.Types();
        var result = proc.isArgumentsObject(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsArgumentsObject004
     * @tc.desc: Check whether the entered value is an arguments object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArgumentsObject004', 0, function() {
        var proc = new util.Types();
        var result = proc.isArgumentsObject(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsArgumentsObject005
     * @tc.desc: Check whether the entered value is an arguments object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArgumentsObject005', 0, function() {
        var proc = new util.Types();
        var result = proc.isArgumentsObject(new Boolean());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsArrayBuffer001
     * @tc.desc: Check whether the entered value is of arraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBuffer001', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBuffer(new ArrayBuffer([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsArrayBuffer002
     * @tc.desc: Check whether the entered value is of arraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBuffer002', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBuffer(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsArrayBuffer003
     * @tc.desc: Check whether the entered value is of arraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBuffer003', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBuffer(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsArrayBuffer004
     * @tc.desc: Check whether the entered value is of arraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBuffer004', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBuffer(new Int8Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsArrayBuffer005
     * @tc.desc: Check whether the entered value is of arraybuffer type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsArrayBuffer005', 0, function() {
        var proc = new util.Types();
        var result = proc.isArrayBuffer(new Int16Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsAsyncFunction001
     * @tc.desc: Check whether the value entered is an asynchronous function type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsAsyncFunction001', 0, function() {
        var proc = new util.Types();
        var result = proc.isAsyncFunction(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsAsyncFunction002
     * @tc.desc: Check whether the value entered is an asynchronous function type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsAsyncFunction002', 0, function() {
        var proc = new util.Types();
        var result = proc.isAsyncFunction(function foo() {});
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsAsyncFunction003
     * @tc.desc: Check whether the value entered is an asynchronous function type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsAsyncFunction003', 0, function() {
        var proc = new util.Types();
        var result = proc.isAsyncFunction(new Int8Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsAsyncFunction004
     * @tc.desc: Check whether the value entered is an asynchronous function type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsAsyncFunction004', 0, function() {
        var proc = new util.Types();
        var result = proc.isAsyncFunction(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsBigInt64Array001
     * @tc.desc: Check whether the entered value is of bigint64array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBigInt64Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isBigInt64Array(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsBigUint64Array001
     * @tc.desc: Check whether the entered value is of biguint64array array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBigUint64Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isBigUint64Array(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsBooleanObject001
     * @tc.desc: Check whether the entered value is a Boolean object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBooleanObject001', 0, function() {
        var proc = new util.Types();
        var result = proc.isBooleanObject(new Boolean(false));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsBooleanObject002
     * @tc.desc: Check whether the entered value is a Boolean object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBooleanObject002', 0, function() {
        var proc = new util.Types();
        var result = proc.isBooleanObject(new Boolean(true));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsBooleanObject003
     * @tc.desc: Check whether the entered value is a Boolean object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBooleanObject003', 0, function() {
        var proc = new util.Types();
        var result = proc.isBooleanObject(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsBooleanObject004
     * @tc.desc: Check whether the entered value is a Boolean object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBooleanObject004', 0, function() {
        var proc = new util.Types();
        var result = proc.isBooleanObject(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsBooleanObject005
     * @tc.desc: Check whether the entered value is a Boolean object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBooleanObject005', 0, function() {
        var proc = new util.Types();
        var result = proc.isBooleanObject(Boolean(true));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsBoxedPrimitive001
     * @tc.desc: Check whether the entered value is a Boolean or number or string or symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBoxedPrimitive001', 0, function() {
        var proc = new util.Types();
        var result = proc.isBoxedPrimitive(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsBoxedPrimitive002
     * @tc.desc: Check whether the entered value is a Boolean or number or string or symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBoxedPrimitive002', 0, function() {
        var proc = new util.Types();
        var result = proc.isBoxedPrimitive(new Boolean(false));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsBoxedPrimitive003
     * @tc.desc: Check whether the entered value is a Boolean or number or string or symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBoxedPrimitive003', 0, function() {
        var proc = new util.Types();
        var result = proc.isBoxedPrimitive(Symbol('foo'));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsBoxedPrimitive004
     * @tc.desc: Check whether the entered value is a Boolean or number or string or symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBoxedPrimitive004', 0, function() {
        var proc = new util.Types();
        var result = proc.isBoxedPrimitive(Object(Symbol('foo')));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsBoxedPrimitive005
     * @tc.desc: Check whether the entered value is a Boolean or number or string or symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsBoxedPrimitive005', 0, function() {
        var proc = new util.Types();
        var result = proc.isBoxedPrimitive(new Boolean(true));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsDataView001
     * @tc.desc: Check whether the entered value is of DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDataView001', 0, function() {
        var proc = new util.Types();
        const ab = new ArrayBuffer(20);
        var result = proc.isDataView(new DataView(ab));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsDataView002
     * @tc.desc: Check whether the entered value is of DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDataView002', 0, function() {
        var proc = new util.Types();
        var result = proc.isDataView(new Int8Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsDataView003
     * @tc.desc: Check whether the entered value is of DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDataView003', 0, function() {
        var proc = new util.Types();
        var result = proc.isDataView(new Float64Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsDataView004
     * @tc.desc: Check whether the entered value is of DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDataView004', 0, function() {
        var proc = new util.Types();
        var result = proc.isDataView(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsDataView005
     * @tc.desc: Check whether the entered value is of DataView type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDataView005', 0, function() {
        var proc = new util.Types();
        var result = proc.isDataView(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsDate001
     * @tc.desc: Check whether the entered value is of type date.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDate001', 0, function() {
        var proc = new util.Types();
        var result = proc.isDate(new Date());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsDate002
     * @tc.desc: Check whether the entered value is of type date.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDate002', 0, function() {
        var proc = new util.Types();
        var result = proc.isDate(new Int8Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsDate003
     * @tc.desc: Check whether the entered value is of type date.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDate003', 0, function() {
        var proc = new util.Types();
        var result = proc.isDate(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsDate004
     * @tc.desc: Check whether the entered value is of type date.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDate004', 0, function() {
        var proc = new util.Types();
        var result = proc.isDate(new Int16Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsDate005
     * @tc.desc: Check whether the entered value is of type date.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsDate005', 0, function() {
        var proc = new util.Types();
        var result = proc.isDate(new Float64Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsExternal001
     * @tc.desc: Check whether the entered value is a native external value type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsExternal001', 0, function() {
        var proc = new util.Types();
        const data = util.createExternalType();
        var result = proc.isExternal(data);
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsExternal002
     * @tc.desc: Check whether the entered value is a native external value type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsExternal002', 0, function() {
        var proc = new util.Types();
        var result = proc.isExternal(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsExternal003
     * @tc.desc: Check whether the entered value is a native external value type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsExternal003', 0, function() {
        var proc = new util.Types();
        var result = proc.isExternal(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsExternal004
     * @tc.desc: Check whether the entered value is a native external value type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsExternal004', 0, function() {
        var proc = new util.Types();
        var result = proc.isExternal(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsExternal005
     * @tc.desc: Check whether the entered value is a native external value type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsExternal005', 0, function() {
        var proc = new util.Types();
        var result = proc.isExternal(new Int16Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsFloat32Array001
     * @tc.desc: Check whether the entered value is of float32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat32Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat32Array(new Float32Array());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsFloat32Array002
     * @tc.desc: Check whether the entered value is of float32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat32Array002', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat32Array(new Int8Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsFloat32Array003
     * @tc.desc: Check whether the entered value is of float32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat32Array003', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat32Array(new Int16Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsFloat32Array004
     * @tc.desc: Check whether the entered value is of float32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat32Array004', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat32Array(new Float64Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsFloat32Array005
     * @tc.desc: Check whether the entered value is of float32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat32Array005', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat32Array(new Uint8Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsFloat64Array001
     * @tc.desc: Check whether the entered value is of float64array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat64Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat64Array(new Float64Array());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsFloat64Array002
     * @tc.desc: Check whether the entered value is of float64array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat64Array002', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat64Array(new Int8Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsFloat64Array003
     * @tc.desc: Check whether the entered value is of float64array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat64Array003', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat64Array(new Float32Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsFloat64Array004
     * @tc.desc: Check whether the entered value is of float64array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat64Array004', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat64Array(new Uint8Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsFloat64Array005
     * @tc.desc: Check whether the entered value is of float64array array type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsFloat64Array005', 0, function() {
        var proc = new util.Types();
        var result = proc.isFloat64Array(new Int16Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsGeneratorFunction001
     * @tc.desc: Check whether the input value is a generator function type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsGeneratorFunction001', 0, function() {
        var proc = new util.Types();
        var result = proc.isGeneratorFunction(new Int8Array());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsGeneratorFunction002
     * @tc.desc: Check whether the input value is a generator function type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsGeneratorFunction002', 0, function() {
        var proc = new util.Types();
        var result = proc.isGeneratorFunction(function foo() {});
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsGeneratorFunction003
     * @tc.desc: Check whether the input value is a generator function type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsGeneratorFunction003', 0, function() {
        var proc = new util.Types();
        var result = proc.isGeneratorFunction(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsGeneratorFunction004
     * @tc.desc: Check whether the input value is a generator function type.
     * @tc.require: AR000GFB59
     * @tc.author: wangjingwu
     */
    it('testIsGeneratorFunction004', 0, function() {
        var proc = new util.Types();
        var result = proc.isGeneratorFunction(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsGeneratorObject001
     * @tc.desc: Check whether the entered value is a generator object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsGeneratorObject001', 0, function() {
        var proc = new util.Types();
        var result = proc.isGeneratorObject(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsGeneratorObject002
     * @tc.desc: Check whether the entered value is a generator object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsGeneratorObject002', 0, function() {
        var proc = new util.Types();
        var result = proc.isGeneratorObject(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsGeneratorObject003
     * @tc.desc: Check whether the entered value is a generator object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsGeneratorObject003', 0, function() {
        var proc = new util.Types();
        var result = proc.isGeneratorObject(new Int16Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsGeneratorObject004
     * @tc.desc: Check whether the entered value is a generator object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsGeneratorObject004', 0, function() {
        var proc = new util.Types();
        var result = proc.isGeneratorObject(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt8Array001
     * @tc.desc: Check whether the entered value is of int8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt8Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt8Array(new Int8Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsInt8Array002
     * @tc.desc: Check whether the entered value is of int8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt8Array002', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt8Array(new ArrayBuffer([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt8Array003
     * @tc.desc: Check whether the entered value is of int8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt8Array003', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt8Array(new Int32Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt8Array004
     * @tc.desc: Check whether the entered value is of int8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt8Array004', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt8Array(new Float64Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt8Array005
     * @tc.desc: Check whether the entered value is of int8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt8Array005', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt8Array(new Uint8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt16Array001
     * @tc.desc: Check whether the entered value is the int16array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt16Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt16Array(new Int16Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsInt16Array002
     * @tc.desc: Check whether the entered value is the int16array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt16Array002', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt16Array(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt16Array003
     * @tc.desc: Check whether the entered value is the int16array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt16Array003', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt16Array(new Int32Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt16Array004
     * @tc.desc: Check whether the entered value is the int16array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt16Array004', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt16Array(new Uint8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt16Array005
     * @tc.desc: Check whether the entered value is the int16array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt16Array005', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt16Array(new Float64Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt32Array001
     * @tc.desc: Check whether the entered value is the int32array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt32Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt32Array(new Int32Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsInt32Array002
     * @tc.desc: Check whether the entered value is the int32array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt32Array002', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt32Array(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt32Array003
     * @tc.desc: Check whether the entered value is the int32array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt32Array003', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt32Array(new Int16Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt32Array004
     * @tc.desc: Check whether the entered value is the int32array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt32Array004', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt32Array(new Float64Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsInt32Array005
     * @tc.desc: Check whether the entered value is the int32array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsInt32Array005', 0, function() {
        var proc = new util.Types();
        var result = proc.isInt32Array(new Uint8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsMap001
     * @tc.desc: Check whether the entered value is of map type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMap001', 0, function() {
        var proc = new util.Types();
        var result = proc.isMap(new Map());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsMap002
     * @tc.desc: Check whether the entered value is of map type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMap002', 0, function() {
        var proc = new util.Types();
        var result = proc.isMap(new Set());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsMap003
     * @tc.desc: Check whether the entered value is of map type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMap003', 0, function() {
        var proc = new util.Types();
        var result = proc.isMap(new String());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsMap004
     * @tc.desc: Check whether the entered value is of map type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMap004', 0, function() {
        var proc = new util.Types();
        var result = proc.isMap(new Object());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsMap005
     * @tc.desc: Check whether the entered value is of map type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMap005', 0, function() {
        var proc = new util.Types();
        var result = proc.isMap(new Boolean());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsMapIterator001
     * @tc.desc: Check whether the entered value is the iterator type of map.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMapIterator001', 0, function() {
        var proc = new util.Types();
        const map = new Map();
        var result = proc.isMapIterator(map.keys());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsMapIterator002
     * @tc.desc: Check whether the entered value is the iterator type of map.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMapIterator002', 0, function() {
        var proc = new util.Types();
        const map = new Map();
        var result = proc.isMapIterator(map.values());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsMapIterator003
     * @tc.desc: Check whether the entered value is the iterator type of map.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMapIterator003', 0, function() {
        var proc = new util.Types();
        const map = new Map();
        var result = proc.isMapIterator(map.entries());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsMapIterator004
     * @tc.desc: Check whether the entered value is the iterator type of map.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMapIterator004', 0, function() {
        var proc = new util.Types();
        const map = new Map();
        var result = proc.isMapIterator(map[Symbol.iterator]());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsMapIterator005
     * @tc.desc: Check whether the entered value is the iterator type of map.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsMapIterator005', 0, function() {
        var proc = new util.Types();
        const map = new Map();
        var result = proc.isMapIterator(map);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsModuleNamespaceObject001
     * @tc.desc: Check whether the entered value is the module namespace object object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsModuleNamespaceObject001', 0, function() {
        var proc = new util.Types();
        var result = proc.isModuleNamespaceObject(util);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsModuleNamespaceObject002
     * @tc.desc: Check whether the entered value is the module namespace object object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsModuleNamespaceObject002', 0, function() {
        var proc = new util.Types();
        var result = proc.isModuleNamespaceObject(url);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsModuleNamespaceObject003
     * @tc.desc: Check whether the entered value is the module namespace object object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsModuleNamespaceObject003', 0, function() {
        var proc = new util.Types();
        var result = proc.isModuleNamespaceObject(file);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsModuleNamespaceObject004
     * @tc.desc: Check whether the entered value is the module namespace object object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsModuleNamespaceObject004', 0, function() {
        var proc = new util.Types();
        var result = proc.isModuleNamespaceObject(process);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsModuleNamespaceObject005
     * @tc.desc: Check whether the entered value is the module namespace object object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsModuleNamespaceObject005', 0, function() {
        var proc = new util.Types();
        var result = proc.isModuleNamespaceObject(app);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsNativeError001
     * @tc.desc: Check whether the value entered is of type error.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNativeError001', 0, function() {
        var proc = new util.Types();
        var result = proc.isNativeError(new TypeError());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsNativeError002
     * @tc.desc: Check whether the value entered is of type error.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNativeError002', 0, function() {
        var proc = new util.Types();
        var result = proc.isNativeError(new Error());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsNativeError003
     * @tc.desc: Check whether the value entered is of type error.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNativeError003', 0, function() {
        var proc = new util.Types();
        var result = proc.isNativeError(new RangeError());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsNativeError004
     * @tc.desc: Check whether the value entered is of type error.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNativeError004', 0, function() {
        var proc = new util.Types();
        var result = proc.isNativeError(true);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsNativeError005
     * @tc.desc: Check whether the value entered is of type error.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNativeError005', 0, function() {
        var proc = new util.Types();
        var result = proc.isNativeError(false);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsNumberObject001
     * @tc.desc: Check whether the entered value is of the number object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNumberObject001', 0, function() {
        var proc = new util.Types();
        var result = proc.isNumberObject(new Number(0));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsNumberObject002
     * @tc.desc: Check whether the entered value is of the number object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNumberObject002', 0, function() {
        var proc = new util.Types();
        var result = proc.isNumberObject(new Number(10));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsNumberObject003
     * @tc.desc: Check whether the entered value is of the number object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNumberObject003', 0, function() {
        var proc = new util.Types();
        var result = proc.isNumberObject(new Number(200));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsNumberObject004
     * @tc.desc: Check whether the entered value is of the number object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNumberObject004', 0, function() {
        var proc = new util.Types();
        var result = proc.isNumberObject(0);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsNumberObject005
     * @tc.desc: Check whether the entered value is of the number object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsNumberObject005', 0, function() {
        var proc = new util.Types();
        var result = proc.isNumberObject(100);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsPromise001
     * @tc.desc: Check whether the entered value is of promise type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsPromise001', 0, function() {
        var proc = new util.Types();
        var result = proc.isPromise(Promise.resolve(42));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsPromise002
     * @tc.desc: Check whether the entered value is of promise type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsPromise002', 0, function() {
        var proc = new util.Types();
        var result = proc.isPromise(Promise.resolve(33));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsPromise003
     * @tc.desc: Check whether the entered value is of promise type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsPromise003', 0, function() {
        var proc = new util.Types();
        var result = proc.isPromise(Promise.resolve(44));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsPromise004
     * @tc.desc: Check whether the entered value is of promise type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsPromise004', 0, function() {
        var proc = new util.Types();
        var err = Error(33);
        var result = proc.isPromise(Promise.reject(err));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsPromise005
     * @tc.desc: Check whether the entered value is of promise type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsPromise005', 0, function() {
        var proc = new util.Types();
		var err = Error(23);
        var result = proc.isPromise(Promise.reject(err));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsProxy001
     * @tc.desc: Check whether the value entered is of proxy type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsProxy001', 0, function() {
        var proc = new util.Types();
        const target = {};
        const proxy = new Proxy(target, {});
        var result = proc.isProxy(proxy);
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsProxy002
     * @tc.desc: Check whether the value entered is of proxy type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsProxy002', 0, function() {
        var proc = new util.Types();
        const target = {};
        const proxy = new Proxy(target, {});
        var result = proc.isProxy(target);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsProxy003
     * @tc.desc: Check whether the value entered is of proxy type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsProxy003', 0, function() {
        var proc = new util.Types();
        const handler = {
            get: function(obj, prop) {
                return prop in obj ? obj[prop] : 37;
            }
        };
        const p = new Proxy({}, handler);
        var result = proc.isProxy(p);
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsProxy004
     * @tc.desc: Check whether the value entered is of proxy type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsProxy004', 0, function() {
        var proc = new util.Types();
        const handler = {
            get: function(obj, prop) {
                return prop in obj ? obj[prop] : 121;
            }
        };
        const p = new Proxy({}, handler);
        var result = proc.isProxy(p);
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsProxy005
     * @tc.desc: Check whether the value entered is of proxy type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsProxy005', 0, function() {
        var proc = new util.Types();
        const handler = {
            get: function(obj, prop) {
                return prop in obj ? obj[prop] : 121;
            }
        };
        const p = new Proxy({}, handler);
        var result = proc.isProxy(handler);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsRegExp001
     * @tc.desc: Check whether the entered value is of type regexp.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsRegExp001', 0, function() {
        var proc = new util.Types();
        var result = proc.isRegExp(new RegExp('abc'));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsRegExp002
     * @tc.desc: Check whether the entered value is of type regexp.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsRegExp002', 0, function() {
        var proc = new util.Types();
        var result = proc.isRegExp(/abc/);
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsRegExp003
     * @tc.desc: Check whether the entered value is of type regexp.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsRegExp003', 0, function() {
        var proc = new util.Types();
        var result = proc.isRegExp(new RegExp('fgd'));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsRegExp004
     * @tc.desc: Check whether the entered value is of type regexp.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsRegExp004', 0, function() {
        var proc = new util.Types();
        var result = proc.isRegExp(new RegExp('123'));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsRegExp005
     * @tc.desc: Check whether the entered value is of type regexp.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsRegExp005', 0, function() {
        var proc = new util.Types();
        var result = proc.isRegExp(new RegExp(/ab+c/, 'i'));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsSet001
     * @tc.desc: Check whether the entered value is of type set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSet001', 0, function() {
        var proc = new util.Types();
        var result = proc.isSet(new Set());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsSet002
     * @tc.desc: Check whether the entered value is of type set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSet002', 0, function() {
        var proc = new util.Types();
        var result = proc.isSet(new Map());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSet003
     * @tc.desc: Check whether the entered value is of type set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSet003', 0, function() {
        var proc = new util.Types();
        var result = proc.isSet(new String());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSet004
     * @tc.desc: Check whether the entered value is of type set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSet004', 0, function() {
        var proc = new util.Types();
        var result = proc.isSet(new Boolean());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSet005
     * @tc.desc: Check whether the entered value is of type set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSet005', 0, function() {
        var proc = new util.Types();
        var result = proc.isSet(new WeakSet());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSetIterator001
     * @tc.desc: Check whether the entered value is the iterator type of set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSetIterator001', 0, function() {
        var proc = new util.Types();
        const set = new Set();
        var result = proc.isSetIterator(set.keys());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsSetIterator002
     * @tc.desc: Check whether the entered value is the iterator type of set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSetIterator002', 0, function() {
        var proc = new util.Types();
        const set = new Set();
        var result = proc.isSetIterator(set.values());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsSetIterator003
     * @tc.desc: Check whether the entered value is the iterator type of set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSetIterator003', 0, function() {
        var proc = new util.Types();
        const set = new Set();
        var result = proc.isSetIterator(set.entries());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsSetIterator004
     * @tc.desc: Check whether the entered value is the iterator type of set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSetIterator004', 0, function() {
        var proc = new util.Types();
        const set = new Set();
        var result = proc.isSetIterator(set[Symbol.iterator]());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsSetIterator005
     * @tc.desc: Check whether the entered value is the iterator type of set.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSetIterator005', 0, function() {
        var proc = new util.Types();
        const set = new Set();
        var result = proc.isSetIterator(set);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSharedArrayBuffer001
     * @tc.desc: Check whether the entered value is of type sharedarraybuffer.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSharedArrayBuffer001', 0, function() {
        var proc = new util.Types();
        var result = proc.isSharedArrayBuffer(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSharedArrayBuffer002
     * @tc.desc: Check whether the entered value is of type sharedarraybuffer.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSharedArrayBuffer002', 0, function() {
        var proc = new util.Types();
        var result = proc.isSharedArrayBuffer(new Int16Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSharedArrayBuffer003
     * @tc.desc: Check whether the entered value is of type sharedarraybuffer.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSharedArrayBuffer003', 0, function() {
        var proc = new util.Types();
        var result = proc.isSharedArrayBuffer(new Int32Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSharedArrayBuffer004
     * @tc.desc: Check whether the entered value is of type sharedarraybuffer.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSharedArrayBuffer004', 0, function() {
        var proc = new util.Types();
        var result = proc.isSharedArrayBuffer(new ArrayBuffer([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSharedArrayBuffer005
     * @tc.desc: Check whether the entered value is of type sharedarraybuffer.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSharedArrayBuffer005', 0, function() {
        var proc = new util.Types();
        var result = proc.isSharedArrayBuffer(new Uint8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsStringObject001
     * @tc.desc: Check whether the entered value is a string object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsStringObject001', 0, function() {
        var proc = new util.Types();
        var result = proc.isStringObject(new String('foo'));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsStringObject002
     * @tc.desc: Check whether the entered value is a string object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsStringObject002', 0, function() {
        var proc = new util.Types();
        var result = proc.isStringObject(new String('twoo'));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsStringObject003
     * @tc.desc: Check whether the entered value is a string object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsStringObject003', 0, function() {
        var proc = new util.Types();
        var result = proc.isStringObject(new String('fdfsfd'));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsStringObject004
     * @tc.desc: Check whether the entered value is a string object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsStringObject004', 0, function() {
        var proc = new util.Types();
        var result = proc.isStringObject('ffds111');
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsStringObject005
     * @tc.desc: Check whether the entered value is a string object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsStringObject005', 0, function() {
        var proc = new util.Types();
        var result = proc.isStringObject('foo');
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSymbolObject001
     * @tc.desc: Check whether the entered value is a symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSymbolObject001', 0, function() {
        var proc = new util.Types();
        const symbols = Symbol('foo');
        var result = proc.isSymbolObject(Object(symbols));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsSymbolObject002
     * @tc.desc: Check whether the entered value is a symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSymbolObject002', 0, function() {
        var proc = new util.Types();
        const symbols = Symbol('s12');
        var result = proc.isSymbolObject(Object(symbols));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsSymbolObject003
     * @tc.desc: Check whether the entered value is a symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSymbolObject003', 0, function() {
        var proc = new util.Types();
        const symbols = Symbol('sads');
        var result = proc.isSymbolObject(Object(symbols));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsSymbolObject004
     * @tc.desc: Check whether the entered value is a symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSymbolObject004', 0, function() {
        var proc = new util.Types();
        const symbols = Symbol('asdd');
        var result = proc.isSymbolObject(symbols);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsSymbolObject005
     * @tc.desc: Check whether the entered value is a symbol object type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsSymbolObject005', 0, function() {
        var proc = new util.Types();
        const symbols = Symbol('foo');
        var result = proc.isSymbolObject(symbols);
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsTypedArray001
     * @tc.desc: Check whether the entered value is a type contained in typedarray.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsTypedArray001', 0, function() {
        var proc = new util.Types();
        var result = proc.isTypedArray(new Float64Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsTypedArray002
     * @tc.desc: Check whether the entered value is a type contained in typedarray.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsTypedArray002', 0, function() {
        var proc = new util.Types();
        var result = proc.isTypedArray(new Uint8Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsTypedArray003
     * @tc.desc: Check whether the entered value is a type contained in typedarray.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsTypedArray003', 0, function() {
        var proc = new util.Types();
        var result = proc.isTypedArray(new Uint16Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsTypedArray004
     * @tc.desc: Check whether the entered value is a type contained in typedarray.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsTypedArray004', 0, function() {
        var proc = new util.Types();
        var result = proc.isTypedArray(new Uint32Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsTypedArray005
     * @tc.desc: Check whether the entered value is a type contained in typedarray.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsTypedArray005', 0, function() {
        var proc = new util.Types();
        var result = proc.isTypedArray(new ArrayBuffer());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint8Array001
     * @tc.desc: Check whether the entered value is the uint8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8Array(new Uint8Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsUint8Array002
     * @tc.desc: Check whether the entered value is the uint8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8Array002', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8Array(new Uint16Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint8Array003
     * @tc.desc: Check whether the entered value is the uint8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8Array003', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8Array(new Uint32Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint8Array004
     * @tc.desc: Check whether the entered value is the uint8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8Array004', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8Array(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint8Array005
     * @tc.desc: Check whether the entered value is the uint8array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8Array005', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8Array(new Int16Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint8ClampedArray001
     * @tc.desc: Check whether the entered value is the uint8clapedarray array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8ClampedArray001', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8ClampedArray(new Uint8ClampedArray([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsUint8ClampedArray002
     * @tc.desc: Check whether the entered value is the uint8clapedarray array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8ClampedArray002', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8ClampedArray(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint8ClampedArray003
     * @tc.desc: Check whether the entered value is the uint8clapedarray array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8ClampedArray003', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8ClampedArray(new Int16Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint8ClampedArray004
     * @tc.desc: Check whether the entered value is the uint8clapedarray array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8ClampedArray004', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8ClampedArray(new Int32Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint8ClampedArray005
     * @tc.desc: Check whether the entered value is the uint8clapedarray array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint8ClampedArray005', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint8ClampedArray(new Uint32Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint16Array001
     * @tc.desc: Check whether the entered value is the uint16array array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint16Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint16Array(new Uint16Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsUint16Array002
     * @tc.desc: Check whether the entered value is the uint16array array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint16Array002', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint16Array(new Uint8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint16Array003
     * @tc.desc: Check whether the entered value is the uint16array array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint16Array003', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint16Array(new Uint32Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint16Array004
     * @tc.desc: Check whether the entered value is the uint16array array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint16Array004', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint16Array(new Uint8ClampedArray([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint16Array005
     * @tc.desc: Check whether the entered value is the uint16array array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint16Array005', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint16Array(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint32Array001
     * @tc.desc: Check whether the entered value is the uint32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint32Array001', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint32Array(new Uint32Array([]));
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsUint32Array002
     * @tc.desc: Check whether the entered value is the uint32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint32Array002', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint32Array(new Uint8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint32Array003
     * @tc.desc: Check whether the entered value is the uint32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint32Array003', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint32Array(new Uint16Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint32Array004
     * @tc.desc: Check whether the entered value is the uint32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint32Array004', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint32Array(new Int8Array([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsUint32Array005
     * @tc.desc: Check whether the entered value is the uint32array array type.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsUint32Array005', 0, function() {
        var proc = new util.Types();
        var result = proc.isUint32Array(new Uint8ClampedArray([]));
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsWeakMap001
     * @tc.desc: Check whether the entered value is of type weakmap.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakMap001', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakMap(new WeakMap());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsWeakMap002
     * @tc.desc: Check whether the entered value is of type weakmap.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakMap002', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakMap(new Map());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsWeakMap003
     * @tc.desc: Check whether the entered value is of type weakmap.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakMap003', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakMap(new Set());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsWeakMap004
     * @tc.desc: Check whether the entered value is of type weakmap.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakMap004', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakMap(new String());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsWeakMap005
     * @tc.desc: Check whether the entered value is of type weakmap.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakMap005', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakMap(new WeakSet());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsWeakSet001
     * @tc.desc: Check whether the entered value is of type weakset.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakSet001', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakSet(new WeakSet());
        expect(result).assertEqual(true);
    })

    /**
     * @tc.name: testIsWeakSet002
     * @tc.desc: Check whether the entered value is of type weakset.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakSet002', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakSet(new WeakMap());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsWeakSet003
     * @tc.desc: Check whether the entered value is of type weakset.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakSet003', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakSet(new String());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsWeakSet004
     * @tc.desc: Check whether the entered value is of type weakset.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakSet004', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakSet(new Set());
        expect(result).assertEqual(false);
    })

    /**
     * @tc.name: testIsWeakSet005
     * @tc.desc: Check whether the entered value is of type weakset.
     * @tc.require: AR000GFB59
     * @tc.author: bihu
     */
    it('testIsWeakSet005', 0, function() {
        var proc = new util.Types();
        var result = proc.isWeakSet(new Map());
        expect(result).assertEqual(false);
    })
})
