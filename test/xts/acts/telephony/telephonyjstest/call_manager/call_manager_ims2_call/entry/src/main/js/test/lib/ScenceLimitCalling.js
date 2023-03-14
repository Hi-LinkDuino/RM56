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

import {RIGHT_PASSWORD, DEFAULT_SLOT_ID, RESTRICTION_MODE_DEACTIVATION} from './Const.js';
import {toString} from './ApiToPromise.js';
import call from '@ohos.telephony.call';

// limit call scence
export function scenceLimitCalling (caseName, type, mode, slotId) {
    return new Promise((resolve, reject) => {
        console.log(`${caseName} scenceLimitCalling type:${type},mode:${mode}`);
        call.setCallRestriction(slotId ? slotId : DEFAULT_SLOT_ID, {
            type,
            mode,
            password:RIGHT_PASSWORD,
        })
            .then(data => {
                console.log(`${caseName} setCallRestriction success, data:${toString(data)}`);
                resolve(data);
            })
            .catch((error) => {
                console.log(`${caseName} setCallRestriction error,error:${toString(error)}`);
                reject(error);
            });
    });
}

export function scenceLimitCancel (caseName, type, slotId) {
    return new Promise((resolve, reject) => {
        console.log(`${caseName} scenceLimitCancel type:${type}`);
        call.setCallRestriction(slotId ? slotId : DEFAULT_SLOT_ID, {
            type,
            mode:RESTRICTION_MODE_DEACTIVATION,
            password:RIGHT_PASSWORD,
        })
            .then(data => {
                console.log(`${caseName} scenceLimitCancel success, data:${toString(data)}`);
                resolve(data);
            })
            .catch((error) => {
                console.log(`${caseName} scenceLimitCancel error,error:${toString(error)}`);
                reject(error);
            });
    });
}
