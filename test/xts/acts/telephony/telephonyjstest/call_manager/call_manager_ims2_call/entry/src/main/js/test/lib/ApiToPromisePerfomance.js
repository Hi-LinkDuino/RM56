
/**
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

import {TEST_PERF_COUNT, PERF_GOAL_TIME} from './Const.js';
import {toString} from './ApiToPromise.js';
// packaging the function to the perfomace test function
export async function apiToPromisePerfomace (options, args) {
    // recursion plan
    let {func, caseName, target, perfTime, compareFunc} = options;
    let flag = true;
    let count = TEST_PERF_COUNT;
    perfTime = perfTime ? perfTime : PERF_GOAL_TIME;

    console.log(`${caseName} apiToPromisePerfomace func ${func}, caseName ${caseName},args:${toString(args)}`);
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

    let startTime = new Date().getTime();
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
    let endTime = new Date().getTime();
    const cost = endTime - startTime;
    console.log(`${caseName} ${func} result flag:${toString(flag)} count:${TEST_PERF_COUNT}` +
        ` cost:${cost}ms avarage:${parseInt(cost * 1000 / TEST_PERF_COUNT)}`);
    if (flag && cost < perfTime) {
        return flag;
    } else {
        throw new Error(`flag:${flag}`);
    }
}

export async function apiToPerfomace (options, args) {
    // recursion plan
    let {func, caseName, target, perfTime, compareFunc, runedFunc} = options;
    let flag = true;
    let count = TEST_PERF_COUNT;
    perfTime = perfTime ? perfTime : PERF_GOAL_TIME;

    console.log(`${caseName} apiToPerfomace func ${func}, caseName ${caseName},args:${toString(args)}`);
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

    let startTime = new Date().getTime();
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
    let endTime = new Date().getTime();
    const cost = endTime - startTime;
    console.log(`${caseName} ${func} result flag:${toString(flag)} count:${TEST_PERF_COUNT}` +
        ` cost:${cost}ms avarage:${parseInt(cost * 1000 / TEST_PERF_COUNT)}`);
    runedFunc(flag && cost < perfTime);
}
export function validate (error, data, func) {
    return func(error, data);
}