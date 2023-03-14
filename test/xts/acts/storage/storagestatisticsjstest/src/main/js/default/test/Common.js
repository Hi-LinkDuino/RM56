/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

import storageStatistics from "@ohos.storagestatistics";
import volumeManager from "@ohos.volumemanager";
import featureAbility from '@ohos.ability.featureAbility';
import {
    describe,
    beforeAll,
    beforeEach,
    afterEach,
    afterAll,
    it,
    expect,
} from "deccjsunit/index";

export function randomString(num) {
    let len = num;
    var $chars = "helloWord";
    var maxPos = $chars.length;
    var pwd = "";
    for (var i = 0; i < len; i++) {
        pwd += $chars.charAt(Math.floor(Math.random() * maxPos));
    }
    return pwd;
}

function isIntNum(val) {
    return typeof val === "number" && val % 1 === 0;
}

function isNegativeNum(val) {
    return val < 0;
}

function isString(str) {
    return typeof str == "string" && str.constructor == String;
}

function isBoolean(val) {
    return typeof val == "boolean";
}

function isInclude(error, message) {
    return error.toString().indexOf(message) != -1;
}

function isArray(object) {
    return object && typeof object === 'object' && Array == object.constructor;
}

async function getPackageName() {
    console.log("featureAbility:" + JSON.stringify(featureAbility));
    let want = await featureAbility.getWant();
    return want.bundleName;
}

export {
    isIntNum,
    isNegativeNum,
    isString,
    isBoolean,
    isInclude,
    isArray,
    getPackageName,
    describe,
    beforeAll,
    beforeEach,
    afterEach,
    afterAll,
    it,
    expect,
    storageStatistics,
    volumeManager,
};
