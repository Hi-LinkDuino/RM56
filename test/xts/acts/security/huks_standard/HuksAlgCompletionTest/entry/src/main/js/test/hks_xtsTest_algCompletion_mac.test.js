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
  makeGenerateKeyOption,
  makeRandomArr,
  timer
} from '../../../../../../hks_xts_common.test.js'

var defaultAlias = 'defaultAlias';
var plaintText = makeRandomArr(64);

describe('Hks_XtsTest_AlgCompletion_Mac', function () {

  /**
   * @tc.number   HUKS_ALG_COMPLETION_39900
   * @tc.name     HmacSHA1_Promise
   * @tc.desc     Test for algorithm completion(HmacSHA1).
   */
  it('HUKS_ALG_COMPLETION_39900', 0, async function (done) {
    macPromise(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      160,
      done,
      'HUKS_ALG_COMPLETION_39900'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_40000
   * @tc.name     HmacSHA224_Promise
   * @tc.desc     Test for algorithm completion(HmacSHA224).
   */
  it('HUKS_ALG_COMPLETION_40000', 0, async function (done) {
    macPromise(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      224,
      done,
      'HUKS_ALG_COMPLETION_40000'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_40100
   * @tc.name     HmacSHA256_Promise
   * @tc.desc     Test for algorithm completion(HmacSHA256).
   */
  it('HUKS_ALG_COMPLETION_40100', 0, async function (done) {
    macPromise(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      256,
      done,
      'HUKS_ALG_COMPLETION_40100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_40200
   * @tc.name     HmacSHA384_Promise
   * @tc.desc     Test for algorithm completion(HmacSHA384).
   */
  it('HUKS_ALG_COMPLETION_40200', 0, async function (done) {
    macPromise(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      384,
      done,
      'HUKS_ALG_COMPLETION_40200'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_40300
   * @tc.name     HmacSHA512_Promise
   * @tc.desc     Test for algorithm completion(HmacSHA512).
   */
  it('HUKS_ALG_COMPLETION_40300', 0, async function (done) {
    macPromise(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      512,
      done,
      'HUKS_ALG_COMPLETION_40300'
    );
  });

  function isKeyExist(done,caseId){
    huks.isKeyExist(defaultAlias, emptyOption,function(err,data){
      expect(data).assertEqual(false);
      console.log(caseId + ' end');
      done();
    });
  };

  function deleteKey(done, caseId){
    huks.deleteKey(defaultAlias, emptyOption, function(err,data){
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      isKeyExist(done, caseId);
    });
  };

  function doMac(digest, size, done, caseId){
    var macOption = makeMacOption(size, digest);
    huks.mac(defaultAlias, macOption,function(err,data){
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      deleteKey(done,caseId);
    });
  };

  function macCallback(digest, size, done, caseId) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_HMAC, size, huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC, null, null, digest
    );
    huks.generateKey(defaultAlias, generateKeyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      doMac(digest, size, done, caseId);
    });
  };

  /**
   * @tc.number   HUKS_ALG_COMPLETION_40400
   * @tc.name     HmacSHA1_AsyncCallback
   * @tc.desc     Test for algorithm completion(HmacSHA512).
   */
  it('HUKS_ALG_COMPLETION_40400', 0, async function (done) {
    macCallback(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      160,
      done,
      'HUKS_ALG_COMPLETION_40400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_40500
   * @tc.name     HmacSHA224_AsyncCallback
   * @tc.desc     Test for algorithm completion(HmacSHA512).
   */
  it('HUKS_ALG_COMPLETION_40500', 0, async function (done) {
    macCallback(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      224,
      done,
      'HUKS_ALG_COMPLETION_40500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_40600
   * @tc.name     HmacSHA256_AsyncCallback
   * @tc.desc     Test for algorithm completion(HmacSHA512).
   */
  it('HUKS_ALG_COMPLETION_40600', 0, async function (done) {
    macCallback(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      256,
      done,
      'HUKS_ALG_COMPLETION_40600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_40700
   * @tc.name     HmacSHA384_AsyncCallback
   * @tc.desc     Test for algorithm completion(HmacSHA512).
   */
  it('HUKS_ALG_COMPLETION_40700', 0, async function (done) {
    macCallback(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      384,
      done,
      'HUKS_ALG_COMPLETION_40700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_40800
   * @tc.name     HmacSHA512_AsyncCallback
   * @tc.desc     Test for algorithm completion(HmacSHA512).
   */
  it('HUKS_ALG_COMPLETION_40800', 0, async function (done) {
    macCallback(
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      512,
      done,
      'HUKS_ALG_COMPLETION_40800'
    );
    setTimeout(function () {
    }, timer);
  });

  function makeMacOption(size, digest) {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: huks.HuksKeyAlg.HUKS_ALG_HMAC
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_DIGEST,
      value: digest
    };
    var options = {
      properties: properties,
      inData: plaintText
    };
    return options;
  };

  async function macPromise(digest, size, done, caseId) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_HMAC, size, huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC, null, null, digest
    );
    var generateKeyRet = await huks.generateKey(defaultAlias, generateKeyOption);
    expect(generateKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var macOption = makeMacOption(size, digest);
    var macRet = await huks.mac(defaultAlias, macOption);
    expect(macRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var deleteKeyRet = await huks.deleteKey(defaultAlias, emptyOption);
    expect(deleteKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var isKeyExist = await huks.isKeyExist(defaultAlias, emptyOption);
    expect(isKeyExist).assertEqual(false);
    console.log(caseId + ' end');
    done();
    setTimeout(function () {
    }, timer);
  }
});