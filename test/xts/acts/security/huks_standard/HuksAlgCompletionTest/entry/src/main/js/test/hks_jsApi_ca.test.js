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
  alias,
  emptyOption,
  inDataOption,
  timer,
  targetAlias
} from '../../../../../../hks_xts_common.test'

describe('HuksTest_JSApiTest_CA', function () {

  /**
   * @tc.number   HuksTest_JSApiTest_CA_00100
   * @tc.name     JSAPI_Test_CA
   * @tc.desc     Test for attestKey.
   */
  it('HuksTest_JSApiTest_CA_00100', 0, async function (done) {
    var attestKeyRet = await huks.attestKey(alias, emptyOption);
    expect(attestKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);

    var getCertificateChainRet = await huks.getCertificateChain(alias, emptyOption);
    expect(getCertificateChainRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);

    var wrapKeyRet = await huks.wrapKey(alias, targetAlias, emptyOption);
    expect(wrapKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);

    var unwrapKeyRet = await huks.unwrapKey(alias, targetAlias, inDataOption);
    expect(unwrapKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);

    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_CA_00200
   * @tc.name     JSAPI_Test_attestKey
   * @tc.desc     Test for attestKey.
   */
  it('HuksTest_JSApiTest_CA_00200', 0, async function (done) {
    huks.attestKey(alias, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
      done();
    });
  });

  /**
   * @tc.number   HuksTest_JSApiTest_CA_00300
   * @tc.name     JSAPI_Test_getCertificateChain
   * @tc.desc     Test for getCertificateChain.
   */
  it('HuksTest_JSApiTest_CA_00300', 0, async function (done) {
    huks.getCertificateChain(alias, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
      done();
    });
  });

  /**
   * @tc.number   HuksTest_JSApiTest_CA_00400
   * @tc.name     JSAPI_Test_wrapKey
   * @tc.desc     Test for wrapKey.
   */
  it('HuksTest_JSApiTest_CA_00400', 0, async function (done) {
    huks.wrapKey(alias, targetAlias, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
      done();
    });
  });

  /**
   * @tc.number   HuksTest_JSApiTest_CA_00500
   * @tc.name     JSAPI_Test_unwrapKey
   * @tc.desc     Test for unwrapKey.
   */
  it('HuksTest_JSApiTest_CA_00500', 0, async function (done) {
    huks.unwrapKey(alias, targetAlias, inDataOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
      done();
    });
  });
});