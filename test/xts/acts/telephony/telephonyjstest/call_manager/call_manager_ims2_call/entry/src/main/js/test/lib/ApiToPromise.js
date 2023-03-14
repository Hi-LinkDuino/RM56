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

export function toString (data) {
    if (typeof data === 'object') {
        return JSON.stringify(data);
    } else {
        return data;
    }
}

export function promisify (fn) {
    return function () {
        let args = Array.prototype.slice.call(arguments);
        return new Promise(function (resolve, reject) {
            console.log(`Telephony_CallManager promisify args:${toString(arguments)},fn:${fn}`);
            args.push(function (error, data) {
                console.log(`Telephony_CallManager promisify error:${toString(error)},data:${toString(data)}`);
                if (error) {
                    reject(error);
                } else {
                    resolve(data);
                }
            });
            fn.apply(null, args);
        });
    };
}