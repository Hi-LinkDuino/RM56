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
import call from '@ohos.telephony.call';
import {toString} from './ApiToPromise.js';
export function scenceTransferCalling (caseName, settingType, type, phoneNumber) {
    return new Promise((resolve, reject) => {
        call.setCallTransfer(0, {
            settingType,
            type,
            phoneNumber,
        })
            .then(data => {
                console.log(`${caseName} scenceTransferCalling success,data:${toString(data)}`);
                resolve(data);
            })
            .catch((error) => {
                console.log(`${caseName} scenceTransferCalling error,error:${toString(error)}`);
                reject(error);
            });
    });
}

export function scenceTransferCancel (caseName) {
    return new Promise((resolve, reject) => {
        call.dial('##002#')
            .then(data => {
                console.log(`${caseName} scenceTransferCancel success,data:${toString(data)}`);
                resolve(data);
            })
            .catch((error) => {
                console.log(`${caseName} scenceTransferCancel error,error${toString(error)}`);
                reject(error);
            });
    });
}

