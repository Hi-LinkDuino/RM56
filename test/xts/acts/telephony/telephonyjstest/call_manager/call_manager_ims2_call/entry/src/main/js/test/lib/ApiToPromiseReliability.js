/**
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
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

import {toString} from './ApiToPromise.js';
import {TEST_RELY_NUMBER} from './Const.js';
// packaging the function to the reliability test function
export async function apiToPromiseReliability (options, args) {
    // recursion plan
    let {func, caseName, target, compareFunc} = options;
    let flag = true;
    let count = TEST_RELY_NUMBER;

    console.log(`${caseName} apiToPromiseReliability func ${func}, caseName ${caseName},args:${toString(args)}`);
    function excuteCallbackPromise () {
        return new Promise((resolve, reject) => {
            function excuteCallback () {
                target[func](...args);
            }
            args[args.length - 1] = function (error, data) {
                if (count === 0) {
                    if (flag) {
                        resolve(flag);
                    } else {
                        reject(new Error(`flag:${flag}`));
                    }
                } else {
                    flag = flag && !!compareFunc(error, data);
                    count--;
                    excuteCallback();
                }
            };
            excuteCallback();
        });
    }

    if (typeof args[args.length - 1] === 'function') {
        try {
            flag = await excuteCallbackPromise();
        } catch (error) {
            flag = false;
        }
    } else {
        for (;count > 0;count--) {
            let error = null;
            let data = null;
            try {
                data = await target[func](...args);
                flag = flag && !!compareFunc(error, data);
            } catch (err) {
                error = err;
                flag = flag && !!compareFunc(error, data);
            }
        }
    }
    console.log(`${caseName} result ${func} flag:${toString(flag)} count:${TEST_RELY_NUMBER}`);
    if (flag) {
        return flag;
    } else {
        throw new Error(`flag:${flag}`);
    }
}

export async function apiToReliability (options, args) {
    // recursion plan
    let {func, caseName, target, compareFunc, runedFunc} = options;
    let flag = true;
    let count = TEST_RELY_NUMBER;

    console.log(`${caseName} apiToPromiseReliability func ${func}, caseName ${caseName},args:${toString(args)}`);
    function excuteCallbackPromise () {
        return new Promise((resolve, reject) => {
            function excuteCallback () {
                target[func](...args);
            }
            args[args.length - 1] = function (error, data) {
                console.log(`${caseName} error ${toString(error)} data ${toString(data)}`);
                if (count === 0) {
                    if (flag) {
                        resolve(flag);
                    } else {
                        reject(new Error(`flag:${flag}`));
                    }
                } else {
                    flag = flag && !!compareFunc(error, data);
                    count--;
                    console.log(`${caseName} error:${toString(error)},data:${toString(data)},` +
                        `flag:${flag},count:${count}`);
                    excuteCallback();
                }
            };
            excuteCallback();
        });
    }

    if (typeof args[args.length - 1] === 'function') {
        try {
            flag = await excuteCallbackPromise();
        } catch (error) {
            flag = false;
        }
    } else {
        for (;count > 0;count--) {
            let error = null;
            let data = null;
            try {
                data = await target[func](...args);
                flag = flag && !!compareFunc(error, data);
            } catch (err) {
                error = err;
                flag = flag && !!compareFunc(error, data);
            }
            console.log(`${caseName} error:${toString(error)},data:${toString(data)},flag:${flag},count:${count}`);
        }
    }
    console.log(`${caseName} result ${func} flag:${toString(flag)} count:${TEST_RELY_NUMBER}`);
    runedFunc(flag);
}


