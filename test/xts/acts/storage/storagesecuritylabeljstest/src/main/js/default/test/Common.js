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

import securityLabel from '@ohos.securityLabel';
import fileio from '@ohos.fileio';
import featureAbility from '@ohos.ability.featureAbility';

import {
  describe, beforeAll, beforeEach, afterEach, afterAll, it, expect
}
from 'deccjsunit/index'

export async function nextFileName(testName) {
  let context = featureAbility.getContext();
  let data = await context.getFilesDir();
  let BASE_PATH = data.substring(0, data.length - 5) + 'cache/';
  return BASE_PATH + testName;
}

export {
  describe,
  beforeAll,
  beforeEach,
  afterEach,
  afterAll,
  it,
  expect,
  securityLabel,
  fileio
};