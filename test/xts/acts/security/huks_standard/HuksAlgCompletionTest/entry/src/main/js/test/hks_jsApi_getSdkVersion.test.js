/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

import huks from '@ohos.security.huks'
import {describe, it, expect} from 'deccjsunit/index'
import {
  emptyOption,
  timer
} from '../../../../../../hks_xts_common.test.js'

describe('HuksTest_JSApiTest_getSdkVersion', function () {

  /**
   * @tc.number   HuksTest_JSApiTest_getSdkVersion_00100
   * @tc.name     JSAPI_Test_getSdkVersion
   * @tc.desc     Test for get sdk version.
   */
  it('HuksTest_JSApiTest_getSdkVersion_00100', 0, async function (done) {
    var sdkVersion = await huks.getSdkVersion(emptyOption);
    console.log('sdk version ' + JSON.stringify(sdkVersion));
    var result = false;
    if (sdkVersion.length > 0) {
      result = true;
    }
    expect(result).assertEqual(true);
    done();
    setTimeout(function () {
    }, timer);
  });
});