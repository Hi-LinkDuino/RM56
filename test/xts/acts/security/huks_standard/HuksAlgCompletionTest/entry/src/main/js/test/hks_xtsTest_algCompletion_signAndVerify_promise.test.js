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

import huks from '@ohos.security.huks';
import {describe, it, expect} from 'deccjsunit/index';
import {
  aliasA,
  aliasB,
  useLib,
  makeGenerateKeyOption,
  makeImportOption,
  makeSignAndVerifyOption,
  makeRandomArr,
  timer,
  emptyOption
} from '../../../../../../hks_xts_common.test.js'

var signedText = new Uint8Array( 0);
var publicKey = new Uint8Array( 0);
var plainText = makeRandomArr (16);

function makeSignGenerateKeyOption (alg, size, padding, digest) {
  var option = makeGenerateKeyOption (
    alg,
    size,
    huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    padding,
    null,
    digest
  );
  return option;
};

function makeSignOption (alg, size, padding, digest) {
  var option = makeSignAndVerifyOption (
    alg,
    size,
    huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
    padding,
    digest,
    plainText,
  );
  return option;
};

function makeVerifyOption (alg, size, padding, digest) {
  var option = makeSignAndVerifyOption (
    alg,
    size,
    huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    padding,
    digest,
    plainText,
  );
  return option;
};

function makeImportKeyOption (alg, size, padding, digest) {
  var option = makeImportOption (
    alg,
    size,
    huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    padding,
    null,
    digest,
    publicKey
  );
  return option;
};

describe ('Hks_XtsTest_AlgCompletion_Rsa_Sign_Verify_Promise', function () {

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13300
   * @tc.name     RSA512_MD5withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_13300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_13300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13400
   * @tc.name     RSA768_MD5withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_13400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_13400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13500
   * @tc.name     RSA1024_MD5withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_13500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_13500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13600
   * @tc.name     RSA2048_MD5withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_13600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_13600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13700
   * @tc.name     RSA3072_MD5withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_13700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_13700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13800
   * @tc.name     RSA4096_MD5withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_13800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_13800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13900
   * @tc.name     RSA512_NONEwithRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_13900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_13900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14000
   * @tc.name     RSA768_NONEwithRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_14000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14100
   * @tc.name     RSA1024_NONEwithRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_14100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14200
   * @tc.name     RSA2048_NONEwithRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_14200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14300
   * @tc.name     RSA3072_NONEwithRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_14300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14400
   * @tc.name     RSA4096_NONEwithRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_14400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14500
   * @tc.name     RSA512_SHA1withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_14500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14600
   * @tc.name     RSA768_SHA1withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_14600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14700
   * @tc.name     RSA1024_SHA1withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_14700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14800
   * @tc.name     RSA2048_SHA1withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_14800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_14900
   * @tc.name     RSA3072_SHA1withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_14900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_14900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15000
   * @tc.name     RSA4096_SHA1withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_15000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15100
   * @tc.name     RSA512_SHA224withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_15100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15200
   * @tc.name     RSA768_SHA224withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_15200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15300
   * @tc.name     RSA1024_SHA224withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_15300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15400
   * @tc.name     RSA2048_SHA224withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_15400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15500
   * @tc.name     RSA3072_SHA224withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_15500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15600
   * @tc.name     RSA4096_SHA224withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_15600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15700
   * @tc.name     RSA512_SHA256withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_15700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15800
   * @tc.name     RSA768_SHA256withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_15800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_15900
   * @tc.name     RSA1024_SHA256withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_15900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_15900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16000
   * @tc.name     RSA2048_SHA256withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_16000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16100
   * @tc.name     RSA3072_SHA256withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_16100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16200
   * @tc.name     RSA4096_SHA256withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_16200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16300
   * @tc.name     RSA512_SHA384withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16300', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384
    );
    var generateKeyRet = await huks.generateKey (aliasA, generateKeyOption);
    expect (generateKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);

    var signOption = makeSignAndVerifyOption (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      plainText
    );
    var signRet = await huks.sign (aliasA, signOption);
    expect (signRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
    console.log ('HUKS_ALG_COMPLETION_16300 end');
    done ();
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16400
   * @tc.name     RSA768_SHA384withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_16400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16500
   * @tc.name     RSA1024_SHA384withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_16500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16600
   * @tc.name     RSA2048_SHA384withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_16600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16700
   * @tc.name     RSA3072_SHA384withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_16700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16800
   * @tc.name     RSA4096_SHA384withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_16800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_16900
   * @tc.name     RSA512_SHA512withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_16900', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512
    );
    var generateKeyRet = await huks.generateKey (aliasA, generateKeyOption);
    expect (generateKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);

    var signOption = makeSignAndVerifyOption (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      plainText
    );
    var signRet = await huks.sign (aliasA, signOption);
    expect (signRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
    console.log ('HUKS_ALG_COMPLETION_16900 end');
    done ();
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17000
   * @tc.name     RSA768_SHA512withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_17000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_17000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17100
   * @tc.name     RSA1024_SHA512withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_17100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_17100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17200
   * @tc.name     RSA2048_SHA512withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_17200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_17200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17300
   * @tc.name     RSA3072_SHA512withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_17300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_17300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17400
   * @tc.name     RSA4096_SHA512withRSA_Promise
   * @tc.desc     Test for sign and verify with rsa.
   */
  it ('HUKS_ALG_COMPLETION_17400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_17400'
    );
    setTimeout (function () {
    }, timer);
  });


  /**
   * @tc.number   HUKS_ALG_COMPLETION_21700
   * @tc.name     RSA512_MD5withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_21700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_21700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_21800
   * @tc.name     RSA768_MD5withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_21800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_21800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_21900
   * @tc.name     RSA1024_MD5withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_21900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_21900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22000
   * @tc.name     RSA2048_MD5withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_22000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22100
   * @tc.name     RSA3072_MD5withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_22100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22200
   * @tc.name     RSA4096_MD5withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_22200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22300
   * @tc.name     RSA512_NONEwithRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_22300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22400
   * @tc.name     RSA768_NONEwithRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_22400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22500
   * @tc.name     RSA1024_NONEwithRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_22500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22600
   * @tc.name     RSA2048_NONEwithRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_22600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22700
   * @tc.name     RSA3072_NONEwithRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_22700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22800
   * @tc.name     RSA4096_NONEwithRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_22800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_22900
   * @tc.name     RSA512_SHA1withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_22900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_22900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23000
   * @tc.name     RSA768_SHA1withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_23000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23100
   * @tc.name     RSA1024_SHA1withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_23100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23200
   * @tc.name     RSA2048_SHA1withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_23200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23300
   * @tc.name     RSA3072_SHA1withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_23300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23400
   * @tc.name     RSA4096_SHA1withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_23400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23500
   * @tc.name     RSA512_SHA224withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_23500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23600
   * @tc.name     RSA768_SHA224withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_23600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23700
   * @tc.name     RSA1024_SHA224withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_23700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23800
   * @tc.name     RSA2048_SHA224withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_23800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_23900
   * @tc.name     RSA3072_SHA224withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_23900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_23900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24000
   * @tc.name     RSA4096_SHA224withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_24000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24100
   * @tc.name     RSA512_SHA256withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_24100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24200
   * @tc.name     RSA768_SHA256withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_24200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24300
   * @tc.name     RSA1024_SHA256withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_24300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24400
   * @tc.name     RSA2048_SHA256withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_24400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24500
   * @tc.name     RSA3072_SHA256withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_24500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24600
   * @tc.name     RSA4096_SHA256withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_24600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24700
   * @tc.name     RSA512_SHA384withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_24700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24800
   * @tc.name     RSA768_SHA384withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_24800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_24900
   * @tc.name     RSA1024_SHA384withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_24900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_24900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_25000
   * @tc.name     RSA2048_SHA384withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_25000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_25000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_25100
   * @tc.name     RSA3072_SHA384withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_25100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_25100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_25200
   * @tc.name     RSA4096_SHA384withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_25200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_25200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_25300
   * @tc.name     RSA512_SHA512withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_25300', 0, async function (done) {
    var plainText = makeRandomArr (16);
    var generateKeyOption = makeGenerateKeyOption (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512
    );
    var generateKeyRet = await huks.generateKey (aliasA, generateKeyOption);
    expect (generateKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);

    var signOption = makeSignAndVerifyOption (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      plainText
    );
    var signRet = await huks.sign (aliasA, signOption);
    expect (signRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
    console.log ('HUKS_ALG_COMPLETION_25300 end');
    done ();
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_25400
   * @tc.name     RSA768_SHA512withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_25400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_25400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_25500
   * @tc.name     RSA1024_SHA512withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_25500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_25500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_25600
   * @tc.name     RSA2048_SHA512withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_25600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_25600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_25700
   * @tc.name     RSA3072_SHA512withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_25700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_25700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_25800
   * @tc.name     RSA4096_SHA512withRSA_Pss_Promise
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it ('HUKS_ALG_COMPLETION_25800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_25800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_30100
   * @tc.name     DSA_MD5withDSA_Promise
   * @tc.desc     Test for sign and verify with dsa.
   */
  it ('HUKS_ALG_COMPLETION_30100', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption (
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    var generateKeyRet = await huks.generateKey (aliasA, generateKeyOption);
    expect (generateKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    console.log ('HUKS_ALG_COMPLETION_30100 end');
    done ();
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_30200
   * @tc.name     DSA_NONEwithDSA_Promise
   * @tc.desc     Test for sign and verify with dsa.
   */
  it ('HUKS_ALG_COMPLETION_30200', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption (
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    var generateKeyRet = await huks.generateKey (aliasA, generateKeyOption);
    expect (generateKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    console.log ('HUKS_ALG_COMPLETION_30200 end');
    done ();
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_30300
   * @tc.name     DSA_SHA1withDSA_Promise
   * @tc.desc     Test for sign and verify with dsa.
   */
  it ('HUKS_ALG_COMPLETION_30300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_30300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_30400
   * @tc.name     DSA_SHA224withDSA_Promise
   * @tc.desc     Test for sign and verify with dsa.
   */
  it ('HUKS_ALG_COMPLETION_30400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_30400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_30500
   * @tc.name     DSA_SHA256withDSA_Promise
   * @tc.desc     Test for sign and verify with dsa.
   */
  it ('HUKS_ALG_COMPLETION_30500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_30500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_30600
   * @tc.name     DSA_SHA384withDSA_Promise
   * @tc.desc     Test for sign and verify with dsa.
   */
  it ('HUKS_ALG_COMPLETION_30600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_30600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_30700
   * @tc.name     DSA_SHA512withDSA_Promise
   * @tc.desc     Test for sign and verify with dsa.
   */
  it ('HUKS_ALG_COMPLETION_30700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_30700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31500
   * @tc.name     ECDSA224_MD5withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_31500', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    var genKeyRet = await huks.generateKey (aliasA, generateKeyOption);
    expect (genKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    console.log ('HUKS_ALG_COMPLETION_31500 end');
    done ();
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31600
   * @tc.name     ECDSA224_NONEwithECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_31600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_31600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31700
   * @tc.name     ECDSA224_SHA1withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_31700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_31700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31800
   * @tc.name     ECDSA224_SHA224withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_31800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_31800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31900
   * @tc.name     ECDSA224_SHA256withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_31900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_31900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_32000
   * @tc.name     ECDSA224_SHA384withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_32000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_32000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_32100
   * @tc.name     ECDSA224_SHA512withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_32100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_32100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_32900
   * @tc.name     ECDSA256_MD5withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_32900', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    var genKeyRet = await huks.generateKey (aliasA, generateKeyOption);
    expect (genKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    console.log ('HUKS_ALG_COMPLETION_32900 end');
    done ();
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_33000
   * @tc.name     ECDSA256_NONEwithECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_33000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_33000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_33100
   * @tc.name     ECDSA256_SHA1withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_33100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_33100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_33200
   * @tc.name     ECDSA256_SHA224withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_33200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_33200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_33300
   * @tc.name     ECDSA256_SHA256withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_33300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_33300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_33400
   * @tc.name     ECDSA256_SHA384withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_33400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_33400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_33500
   * @tc.name     ECDSA256_SHA512withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_33500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_33500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34300
   * @tc.name     ECDSA384_MD5withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_34300', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    var genKeyRet = await huks.generateKey (aliasA, generateKeyOption);
    expect (genKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    console.log ('HUKS_ALG_COMPLETION_34300 end');
    done ();
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34400
   * @tc.name     ECDSA384_NONEwithECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_34400', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_34400'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34500
   * @tc.name     ECDSA384_SHA1withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_34500', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_34500'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34600
   * @tc.name     ECDSA384_SHA224withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_34600', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_34600'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34700
   * @tc.name     ECDSA384_SHA256withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_34700', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_34700'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34800
   * @tc.name     ECDSA384_SHA384withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_34800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_34800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34900
   * @tc.name     ECDSA384_SHA512withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_34900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_34900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35700
   * @tc.name     ECDSA521_MD5withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_35700', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    var genKeyRet = await huks.generateKey (aliasA, generateKeyOption);
    expect (genKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    console.log ('HUKS_ALG_COMPLETION_35700 end');
    done ();
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35800
   * @tc.name     ECDSA521_NONEwithECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_35800', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_35800'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35900
   * @tc.name     ECDSA521_SHA1withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_35900', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_35900'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36000
   * @tc.name     ECDSA521_SHA224withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_36000', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_36000'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36100
   * @tc.name     ECDSA521_SHA256withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_36100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_36100'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36200
   * @tc.name     ECDSA521_SHA384withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_36200', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_36200'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36300
   * @tc.name     ECDSA521_SHA512withECDSA_Promise
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it ('HUKS_ALG_COMPLETION_36300', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_36300'
    );
    setTimeout (function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_37100
   * @tc.name     Regression_Testing_ED25519_Promise
   * @tc.desc     Test for ED25519 Regression Testing.
   */
  it ('HUKS_ALG_COMPLETION_37100', 0, async function (done) {
    signAndVerifyPromise (
      huks.HuksKeyAlg.HUKS_ALG_ED25519,
      huks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_37100'
    );
    setTimeout (function () {
    }, timer);
  });

  async function signAndVerifyPromise (alg, size, padding, digest, done, caseId) {
    var generateKeyOption = makeSignGenerateKeyOption (alg, size, padding, digest);
    var generateKeyResult = await huks.generateKey (aliasA, generateKeyOption);
    if (useLib == 'mbedtls' && alg == huks.HuksKeyAlg.HUKS_ALG_DSA) {
      expect (generateKeyResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
    } else {
      expect (generateKeyResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
      var signOption = makeSignOption (alg, size, padding, digest);
      var signResult = await huks.sign (aliasA, signOption);
      if (useLib == 'mbedtls' && (
        caseId == 'HUKS_ALG_COMPLETION_24700' ||
        caseId == 'HUKS_ALG_COMPLETION_25300' ||
        caseId == 'HUKS_ALG_COMPLETION_25400')) {
        expect (signResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
      } else {
        expect (signResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
        signedText = signResult.outData;
        var exportKeyRet = await huks.exportKey (aliasA, emptyOption);
        expect (exportKeyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
        publicKey = exportKeyRet.outData;
        var importOption = makeImportKeyOption (alg, size, padding, digest);
        var importKeyResult = await huks.importKey (aliasB, importOption);
        expect (importKeyResult.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
        var verifyOption = makeVerifyOption (alg, size, padding, digest);
        var verifyRet = await huks.verify (aliasB, verifyOption, signedText);
        expect (verifyRet.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
        var deleteKeyAliasA = await huks.deleteKey (aliasA, emptyOption);
        var deleteKeyAliasB = await huks.deleteKey (aliasB, emptyOption);
        expect (deleteKeyAliasA.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
        expect (deleteKeyAliasB.errorCode).assertEqual (huks.HuksErrorCode.HUKS_SUCCESS);
        var isKeyAExist = await huks.isKeyExist (aliasA, emptyOption);
        expect (isKeyAExist).assertEqual (false);
        var isKeyBExist = await huks.isKeyExist (aliasB, emptyOption);
        expect (isKeyBExist).assertEqual (false);
      }
    }
    console.log (caseId + ' end');
    done ();
  };
});