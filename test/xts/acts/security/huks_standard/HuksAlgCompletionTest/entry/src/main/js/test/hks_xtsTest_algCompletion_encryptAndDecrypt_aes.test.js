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
  useLib,
  timer,
  makeGenerateKeyOption,
  makeEncryptAndDecryptOption,
  makeRandomArr
} from '../../../../../../hks_xts_common.test.js'

var plainText = makeRandomArr (64);
var alias = '';

function makeAesGenerateKeyOption (size, padding, mode) {
  var option = makeGenerateKeyOption (
    huks.HuksKeyAlg.HUKS_ALG_AES,
    size,
    huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    padding,
    mode
  );
  return option;
};

function makeAesEncryptOption (padding, mode) {
  var option = makeEncryptAndDecryptOption (
    huks.HuksKeyAlg.HUKS_ALG_AES,
    huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
    padding,
    mode,
    null,
    null,
    plainText
  );
  return option;
};

function makeAesDecryptOption (padding, mode, ciphertext) {
  var option = makeEncryptAndDecryptOption (
    huks.HuksKeyAlg.HUKS_ALG_AES,
    huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    padding,
    mode,
    null,
    null,
    ciphertext
  );
  return option;
};

describe ('Hks_XtsTest_algCompletion_Aes_Encrypt_Decrypt', function () {

  /**
   * @tc.number    HUKS_ALG_COMPLETION_00100
   * @tc.name      AES128_AES_CBC_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES128_AES_CBC_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_00100', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_00100'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_00200
   * @tc.name      AES128_AES_CBC_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion.
   */
  it ('HUKS_ALG_COMPLETION_00200', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_00200'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_00300
   * @tc.name      AES192_AES_CBC_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES192_AES_CBC_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_00300', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_00300'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_00400
   * @tc.name      AES192_AES_CBC_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES192_AES_CBC_NoPadding)..
   */
  it ('HUKS_ALG_COMPLETION_00400', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_00400'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_00500
   * @tc.name      AES256_AES_CBC_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES256_AES_CBC_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_00500', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_00500'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_00600
   * @tc.name      AES256_AES_CBC_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES256_AES_CBC_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_00600', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_00600'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_00700
   * @tc.name      AES128_AES_CBC_PKCS7Padding_Promise
   * @tc.desc      Test for algorithm completion(AES128_AES_CBC_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_00700', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_00700'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_00800
   * @tc.name      AES128_AES_CBC_PKCS7Padding_Callback
   * @tc.desc      Test for algorithm completion(AES128_AES_CBC_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_00800', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_00800'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_00900
   * @tc.name      AES192_AES_CBC_PKCS7Padding_Promise
   * @tc.desc      Test for algorithm completion(AES192_AES_CBC_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_00900', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_00900'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01000
   * @tc.name      AES192_AES_CBC_PKCS7Padding_Callback
   * @tc.desc      Test for algorithm completion(AES192_AES_CBC_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_01000', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_01000'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01100
   * @tc.name      AES256_AES_CBC_PKCS7Padding_Promise
   * @tc.desc      Test for algorithm completion(AES256_AES_CBC_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_01100', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_01100'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01200
   * @tc.name      AES256_AES_CBC_PKCS7Padding_Callback
   * @tc.desc      Test for algorithm completion(AES256_AES_CBC_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_01200', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_CBC,
      done,
      'HUKS_ALG_COMPLETION_01200'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01300
   * @tc.name      AES128_AES_CTR_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES128_AES_CTR_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_01300', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CTR,
      done,
      'HUKS_ALG_COMPLETION_01300'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01400
   * @tc.name      AES128_AES_CTR_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES128_AES_CTR_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_01400', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CTR,
      done,
      'HUKS_ALG_COMPLETION_01400'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01500
   * @tc.name      AES192_AES_CTR_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES192_AES_CTR_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_01500', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CTR,
      done,
      'HUKS_ALG_COMPLETION_01500'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01600
   * @tc.name      AES192_AES_CTR_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES192_AES_CTR_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_01600', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CTR,
      done,
      'HUKS_ALG_COMPLETION_01600'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01700
   * @tc.name      AES256_AES_CTR_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES256_AES_CTR_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_01700', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CTR,
      done,
      'HUKS_ALG_COMPLETION_01700'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01800
   * @tc.name      AES256_AES_CTR_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES128_AES_CTR_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_01800', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CTR,
      done,
      'HUKS_ALG_COMPLETION_01800'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_01900
   * @tc.name      AES128_AES_ECB_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES128_AES_ECB_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_01900', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_01900'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02000
   * @tc.name      AES128_AES_ECB_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES128_AES_ECB_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_02000', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02000'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02100
   * @tc.name      AES192_AES_ECB_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES192_AES_ECB_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_02100', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02100'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02200
   * @tc.name      AES192_AES_ECB_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES192_AES_ECB_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_02200', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02200'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02300
   * @tc.name      AES256_AES_ECB_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES256_AES_ECB_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_02300', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02300'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02400
   * @tc.name      AES256_AES_ECB_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES256_AES_ECB_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_02400', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02400'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02500
   * @tc.name      AES128_AES_ECB_PKCS7Padding_Promise
   * @tc.desc      Test for algorithm completion(AES128_AES_ECB_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_02500', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02500'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02600
   * @tc.name      AES128_AES_ECB_PKCS7Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES128_AES_ECB_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_02600', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02600'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02700
   * @tc.name      Test AES192_AES_ECB_PKCS7Padding_Promise
   * @tc.desc      Test for algorithm completion(AES192_AES_ECB_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_02700', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02700'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02800
   * @tc.name      AES192_AES_ECB_PKCS7Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES192_AES_ECB_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_02800', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02800'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_02900
   * @tc.name      AES256_AES_ECB_PKCS7Padding_Promise
   * @tc.desc      Test for algorithm completion(AES256_AES_ECB_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_02900', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_02900'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_03000
   * @tc.name      AES256_AES_ECB_PKCS7Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES256_AES_ECB_PKCS7Padding).
   */
  it ('HUKS_ALG_COMPLETION_03000', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      done,
      'HUKS_ALG_COMPLETION_03000'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_03100
   * @tc.name      AES128_AES_GCM_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES128_AES_GCM_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_03100', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_GCM,
      done,
      'HUKS_ALG_COMPLETION_03100'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_03200
   * @tc.name      AES28_AES_GCM_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES128_AES_GCM_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_03200', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_GCM,
      done,
      'HUKS_ALG_COMPLETION_03200'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_03300
   * @tc.name      AES192_AES_GCM_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES192_AES_GCM_NoPadding.
   */
  it ('HUKS_ALG_COMPLETION_03300', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_GCM,
      done,
      'HUKS_ALG_COMPLETION_03300'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_03400
   * @tc.name      AES192_AES_GCM_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES192_AES_GCM_NoPadding.
   */
  it ('HUKS_ALG_COMPLETION_03400', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_192,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_GCM,
      done,
      'HUKS_ALG_COMPLETION_03400'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_03500
   * @tc.name      AES256_AES_GCM_NoPadding_Promise
   * @tc.desc      Test for algorithm completion(AES256_AES_GCM_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_03500', 0, async function (done) {
    encryptAndDecryptWithPromise (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_GCM,
      done,
      'HUKS_ALG_COMPLETION_03500'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_03600
   * @tc.name      AES256_AES_GCM_NoPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion(AES256_AES_GCM_NoPadding).
   */
  it ('HUKS_ALG_COMPLETION_03600', 0, async function (done) {
    encryptAndDecryptWithCallback (
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_GCM,
      done,
      'HUKS_ALG_COMPLETION_03600'
    );
  });

  async function encryptAndDecryptWithPromise (size, padding, mode, done, caseId) {
    var generateKeyOption = makeAesGenerateKeyOption (size, padding, mode);
    var generateKeyResult = await huks.generateKey (caseId, generateKeyOption);
    expect (generateKeyResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
    var isKeyExist = await huks.isKeyExist (caseId, emptyOption);
    expect (isKeyExist).assertEqual (true);
    var encryptOption = makeAesEncryptOption (padding, mode);
    var encryptResult = await huks.encrypt (caseId, encryptOption);
    if (useLib == 'openssl') {
      expect (encryptResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
      var decryptOption = makeAesDecryptOption (padding, mode, encryptResult.outData);
      var decryptResult = await huks.decrypt (caseId, decryptOption);
      expect (decryptResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
      expect (JSON.stringify (plainText)).assertEqual (JSON.stringify (decryptResult.outData));
      var deleteKeyResult = await huks.deleteKey (caseId, emptyOption);
      expect (deleteKeyResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
      var isKeyExist = await huks.isKeyExist (caseId, emptyOption);
      expect (isKeyExist).assertEqual (false);
    } else {
      if (mode == huks.HuksCipherMode.HUKS_MODE_CBC
        || mode == huks.HuksCipherMode.HUKS_MODE_CTR
        || mode == huks.HuksCipherMode.HUKS_MODE_GCM) {
          expect (encryptResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
          var decryptOption = makeAesDecryptOption (padding, mode, encryptResult.outData);
          var decryptResult = await huks.decrypt (caseId, decryptOption);
          expect (decryptResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
          expect (JSON.stringify (plainText)).assertEqual (JSON.stringify (decryptResult.outData));
          var deleteKeyResult = await huks.deleteKey (caseId, emptyOption);
          expect (deleteKeyResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
          var isKeyExist = await huks.isKeyExist (caseId, emptyOption);
          expect (isKeyExist).assertEqual (false);
      } else if (huks.HuksCipherMode.HUKS_MODE_ECB) {
        if (padding == huks.HuksKeyPadding.HUKS_PADDING_PKCS7) {
          expect (encryptResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
        } else {
          expect (encryptResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
        }
      } else {
        expect (encryptResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
      }
    }
    console.log (caseId + ' end');
    done ();
    setTimeout (function () {
    }, timer);
  };

  function checkKeyIsExistAfterDeleteKey (done, caseId) {
    huks.isKeyExist (caseId, emptyOption, function (err, data) {
      expect (data).assertEqual (false);
      console.log (caseId + ' end');
      done ();
      setTimeout (function () {
      }, timer);
    });
  };

  function deleteKeyWithCallback (done, caseId) {
    huks.deleteKey (caseId, emptyOption, function (err, data) {
      expect (data.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
      checkKeyIsExistAfterDeleteKey (done, caseId);
    });
  };

  function decryptWithCallback (size, padding, mode, ciphertext, done, caseId) {
    var decryptOption = makeAesDecryptOption (padding, mode, ciphertext);
    huks.decrypt (caseId, decryptOption, function (err, data) {
      expect (data.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
      expect (JSON.stringify (plainText)).assertEqual (JSON.stringify (data.outData));
      deleteKeyWithCallback (done, caseId);
    });
  };

  function encryptWithCallback (size, padding, mode, done, caseId) {
    var encryptOption = makeAesEncryptOption (padding, mode);
    huks.encrypt (caseId, encryptOption, function (err, data) {
      if (useLib == 'openssl') {
        decryptWithCallback (size, padding, mode, data.outData, done, caseId);
      } else {
        switch (mode) {
          case huks.HuksCipherMode.HUKS_MODE_CBC:
          case huks.HuksCipherMode.HUKS_MODE_CTR:
          case huks.HuksCipherMode.HUKS_MODE_GCM:
            expect (data.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
            decryptWithCallback (size, padding, mode, data.outData, done, caseId);
            break;
          case huks.HuksCipherMode.HUKS_MODE_ECB:
            if (padding == huks.HuksKeyPadding.HUKS_PADDING_PKCS7) {
              expect (data.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
              done ();
            } else {
              expect (data.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
              done ();
            }
            break;
          default:
            expect (data.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
            done ();
        }
      }
    });
  };

  function checkKeyIsExistAfterGenerateKey (size, padding, mode, done, caseId) {
    huks.isKeyExist (caseId, emptyOption, function (err, data) {
      expect (data).assertEqual (true);
      encryptWithCallback (size, padding, mode, done, caseId);
    });
  };

  function encryptAndDecryptWithCallback (size, padding, mode, done, caseId) {
    var generateKeyOption = makeAesGenerateKeyOption (size, padding, mode);
    huks.generateKey (caseId, generateKeyOption, function (err, data) {
      expect (data.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
      checkKeyIsExistAfterGenerateKey (size, padding, mode, done, caseId);
    });
  };
});