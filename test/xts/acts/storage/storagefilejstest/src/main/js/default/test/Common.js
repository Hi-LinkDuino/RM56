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

import fileio from '@ohos.fileio';
import featureAbility from '@ohos.ability.featureAbility'

export const FILE_CONTENT = 'hello world';

import {
  describe, beforeAll, beforeEach, afterEach, afterAll, it, expect
}
from 'deccjsunit/index'

export function prepareFile(fpath, content) {
  try {
    let fd = fileio.openSync(fpath, 0o102, 0o666)
    fileio.ftruncateSync(fd)
    fileio.writeSync(fd, content)
    fileio.fsyncSync(fd)
    fileio.closeSync(fd)
    return true
  }
  catch (e) {
    console.log('Failed to prepareFile for ' + e)
    return false
  }
}
export function prepareEmptyFile(fpath) {
  try {
    let fd = fileio.openSync(fpath, 0o102, 0o777)
    fileio.closeSync(fd)
    return true
  }
  catch (e) {
    console.log('Failed to prepareFile for ' + e)
    return false
  }
}

export async function nextFileName(testName) {
  var context = featureAbility.getContext();
  let data = await context.getFilesDir();
  let BASE_PATH = data.substring(0, data.length - 5) + 'cache/';
  return BASE_PATH + testName
}
export async function fileName(testName) {
  var context = featureAbility.getContext();
  let data = await context.getFilesDir();
  let BASE_PATH = data + '/';
  return BASE_PATH + testName
}
export async function cacheFileName(testName) {
  var context = featureAbility.getContext();
  let data = await context.getFilesDir();
  let BASE_PATH = data + '/cache/';
  return BASE_PATH + testName
}

export function sleep(n) {
  var start = new Date().getTime();
  while (true) {
    if (new Date().getTime() - start > n) {
      break;
    }
  }
}

export function randomString(num) {
  let len= num;
  var $chars = 'aaaabbbbcccc';
  var maxPos = $chars.length;
  var pwd = '';
  for (var i = 0; i < len; i++) {
    pwd += $chars.charAt(Math.floor(Math.random() * maxPos));
  }
  return pwd;
}

export {
  fileio,
  describe,
  beforeAll,
  beforeEach,
  afterEach,
  afterAll,
  it,
  expect
};