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

var signedText = [];
var publicKey = [];

describe('Hks_XtsTest_AlgCompletion_Sign_Verify', function () {

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17500
   * @tc.name     RSA512_MD5withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_17500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_17500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17600
   * @tc.name     RSA768_MD5withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_17600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_17600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17700
   * @tc.name     RSA1024_MD5withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_17700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_17700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17800
   * @tc.name     RSA2048_MD5withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_17800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_17800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_17900
   * @tc.name     RSA3072_MD5withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_17900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_17900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18000
   * @tc.name     RSA4096_MD5withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_18000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18100
   * @tc.name     RSA512_NONEwithRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_18100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18200
   * @tc.name     RSA768_NONEwithRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_18200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18300
   * @tc.name     RSA1024_NONEwithRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_18300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18400
   * @tc.name     RSA2048_NONEwithRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_18400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18500
   * @tc.name     RSA3072_NONEwithRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_18500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18600
   * @tc.name     RSA4096_NONEwithRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_18600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18700
   * @tc.name     RSA512_SHA1withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_18700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18800
   * @tc.name     RSA768_SHA1withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_18800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_18900
   * @tc.name     RSA1024_SHA1withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_18900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_18900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19000
   * @tc.name     RSA2048_SHA1withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_19000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19100
   * @tc.name     RSA3072_SHA1withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_19100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19200
   * @tc.name     RSA4096_SHA1withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_19200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19300
   * @tc.name     RSA512_SHA224withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_19300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19400
   * @tc.name     RSA768_SHA224withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_19400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19500
   * @tc.name     RSA1024_SHA224withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_19500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19600
   * @tc.name     RSA2048_SHA224withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_19600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19700
   * @tc.name     RSA3072_SHA224withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_19700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19800
   * @tc.name     RSA4096_SHA224withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_19800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_19900
   * @tc.name     RSA512_SHA256withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_19900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_19900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20000
   * @tc.name     RSA768_SHA256withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_20000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20100
   * @tc.name     RSA1024_SHA256withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_20100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20200
   * @tc.name     RSA2048_SHA256withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_20200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20300
   * @tc.name     RSA3072_SHA256withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_20300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20400
   * @tc.name     RSA4096_SHA256withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_20400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20500
   * @tc.name     RSA512_SHA384withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20500', 0, async function (done) {
    var plainText = makeRandomArr(16);
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384
    );
    var generateKeyRet = await huks.generateKey(aliasA, generateKeyOption);
    expect(generateKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var signOption = makeSignAndVerifyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      plainText
    );
    huks.sign(aliasA, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
      console.log('HUKS_ALG_COMPLETION_20500 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20600
   * @tc.name     RSA768_SHA384withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_20600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20700
   * @tc.name     RSA1024_SHA384withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_20700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20800
   * @tc.name     RSA2048_SHA384withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_20800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_20900
   * @tc.name     RSA3072_SHA384withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_20900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_20900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_21000
   * @tc.name     RSA4096_SHA384withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_21000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_21000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_21100
   * @tc.name     RSA512_SHA512withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_21100', 0, async function (done) {
    var plainText = makeRandomArr(16);
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512
    );
    var generateKeyRet = await huks.generateKey(aliasA, generateKeyOption);
    expect(generateKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var signOption = makeSignAndVerifyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      plainText
    );
    huks.sign(aliasA, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
      console.log('HUKS_ALG_COMPLETION_21100 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_21200
   * @tc.name     RSA768_SHA512withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_21200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_21200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_21300
   * @tc.name     RSA1024_SHA512withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_21300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_21300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   zHUKS_ALG_COMPLETION_21400
   * @tc.name     RSA2048_SHA512withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_21400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_21400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_21500
   * @tc.name     RSA3072_SHA512withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_21500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_21500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_21600
   * @tc.name     RSA4096_SHA512withRSA_Callback
   * @tc.desc     Test for sign and verify with rsa.
   */
  it('HUKS_ALG_COMPLETION_21600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_21600'
    );
    setTimeout(function () {
    }, timer);
  });



  /**
   * @tc.number   HUKS_ALG_COMPLETION_25900
   * @tc.name     RSA512_MD5withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_25900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_25900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26000
   * @tc.name     RSA768_MD5withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_26000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26100
   * @tc.name     RSA1024_MD5withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_26100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26200
   * @tc.name     RSA2048_MD5withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_26200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26300
   * @tc.name     RSA3072_MD5withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_26300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26400
   * @tc.name     RSA4096_MD5withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5,
      done,
      'HUKS_ALG_COMPLETION_26400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26500
   * @tc.name     RSA512_NONEwithRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_26500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26600
   * @tc.name     RSA768_NONEwithRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_26600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26700
   * @tc.name     RSA1024_NONEwithRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_26700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26800
   * @tc.name     RSA2048_NONEwithRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_26800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_26900
   * @tc.name     RSA3072_NONEwithRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_26900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_26900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27000
   * @tc.name     RSA4096_NONEwithRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_27000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27100
   * @tc.name     RSA512_SHA1withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_27100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27200
   * @tc.name     RSA768_SHA1withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_27200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27300
   * @tc.name     RSA1024_SHA1withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_27300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27400
   * @tc.name     RSA2048_SHA1withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_27400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27500
   * @tc.name     RSA3072_SHA1withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_27500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27600
   * @tc.name     RSA4096_SHA1withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_27600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27700
   * @tc.name     RSA512_SHA224withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_27700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27800
   * @tc.name     RSA768_SHA224withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_27800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_27900
   * @tc.name     RSA1024_SHA224withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_27900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_27900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28000
   * @tc.name     RSA2048_SHA224withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_28000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28100
   * @tc.name     RSA3072_SHA224withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_28100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28200
   * @tc.name     RSA4096_SHA224withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_28200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28300
   * @tc.name     RSA512_SHA256withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_28300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28400
   * @tc.name     RSA768_SHA256withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_28400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28500
   * @tc.name     RSA1024_SHA256withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_28500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28600
   * @tc.name     RSA2048_SHA256withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_28600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28700
   * @tc.name     RSA3072_SHA256withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_28700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28800
   * @tc.name     RSA4096_SHA256withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_28800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_28900
   * @tc.name     RSA512_SHA384withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_28900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_28900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29000
   * @tc.name     RSA768_SHA384withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_29000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29100
   * @tc.name     RSA1024_SHA384withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_29100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29200
   * @tc.name     RSA2048_SHA384withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_29200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29300
   * @tc.name     RSA3072_SHA384withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_29300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29400
   * @tc.name     RSA4096_SHA384withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_29400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29500
   * @tc.name     RSA512_SHA512withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29500', 0, async function (done) {
    var plainText = makeRandomArr(16);
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512
    );
    var generateKeyRet = await huks.generateKey(aliasA, generateKeyOption);
    expect(generateKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var signOption = makeSignAndVerifyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      plainText
    );
    huks.sign(aliasA, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
      console.log('HUKS_ALG_COMPLETION_29500 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29600
   * @tc.name     RSA768_SHA512withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_29600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29700
   * @tc.name     RSA1024_SHA512withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_29700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29800
   * @tc.name     RSA2048_SHA512withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_29800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_29900
   * @tc.name     RSA3072_SHA512withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_29900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_29900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_30000
   * @tc.name     RSA4096_SHA512withRSA_Pss_AsyncCallback
   * @tc.desc     Test for sign and verify with rsa/pss.
   */
  it('HUKS_ALG_COMPLETION_30000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PSS,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_30000'
    );
    setTimeout(function () {
    }, timer);
  });



  /**
   * @tc.number   HUKS_ALG_COMPLETION_30800
   * @tc.name     DSA_MD5withDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with dsa.
   */
  it('HUKS_ALG_COMPLETION_30800', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    huks.generateKey(aliasA, generateKeyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      console.log('HUKS_ALG_COMPLETION_30800 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_30900
   * @tc.name     DSA_NONEwithDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with dsa.
   */
  it('HUKS_ALG_COMPLETION_30900', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    huks.generateKey(aliasA, generateKeyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      console.log('HUKS_ALG_COMPLETION_30900 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31000
   * @tc.name     DSA_SHA1withDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with dsa.
   */
  it('HUKS_ALG_COMPLETION_31000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_31000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31100
   * @tc.name     DSA_SHA224withDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with dsa.
   */
  it('HUKS_ALG_COMPLETION_31100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_31100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31200
   * @tc.name     DSA_SHA256withDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with dsa.
   */
  it('HUKS_ALG_COMPLETION_31200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_31200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31300
   * @tc.name     DSA_SHA384withDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with dsa.
   */
  it('HUKS_ALG_COMPLETION_31300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_31300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_31400
   * @tc.name     DSA_SHA512withDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with dsa.
   */
  it('HUKS_ALG_COMPLETION_31400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_DSA,
      1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_31400'
    );
    setTimeout(function () {
    }, timer);
  });



  /**
   * @tc.number   HUKS_ALG_COMPLETION_32200
   * @tc.name     ECDSA224_MD5withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_32200', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    huks.generateKey(aliasA, generateKeyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      console.log('HUKS_ALG_COMPLETION_32200 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_32300
   * @tc.name     ECDSA224_NONEwithECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_32300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_32300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_32400
   * @tc.name     ECDSA224_SHA1withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_32400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_32400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_32500
   * @tc.name     ECDSA224_SHA224withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_32500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_32500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_32600
   * @tc.name     ECDSA224_SHA256withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_32600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_32600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_32700
   * @tc.name     ECDSA224_SHA384withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_32700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_32700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_32800
   * @tc.name     ECDSA224_SHA512withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_32800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_32800'
    );
    setTimeout(function () {
    }, timer);
  });



  /**
   * @tc.number   HUKS_ALG_COMPLETION_33600
   * @tc.name     ECDSA256_MD5withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_33600', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    huks.generateKey(aliasA, generateKeyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      console.log('HUKS_ALG_COMPLETION_33600 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_33700
   * @tc.name     ECDSA256_NONEwithECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_33700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_33700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_33800
   * @tc.name     ECDSA256_SHA1withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_33800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_33800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_33900
   * @tc.name     ECDSA256_SHA224withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_33900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_33900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34000
   * @tc.name     ECDSA256_SHA256withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_34000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_34000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34100
   * @tc.name     ECDSA256_SHA384withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_34100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_34100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_34200
   * @tc.name     ECDSA256_SHA512withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_34200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_34200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35000
   * @tc.name     ECDSA384_MD5withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_35000', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    huks.generateKey(aliasA, generateKeyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      console.log('HUKS_ALG_COMPLETION_35000 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35100
   * @tc.name     ECDSA384_NONEwithECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_35100', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_35100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35200
   * @tc.name     ECDSA384_SHA1withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_35200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_35200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35300
   * @tc.name     ECDSA384_SHA224withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_35300', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_35300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35400
   * @tc.name     ECDSA384_SHA256withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_35400', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_35400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35500
   * @tc.name     ECDSA384_SHA384withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_35500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_35500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_35600
   * @tc.name     ECDSA384_SHA512withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_35600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_35600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36400
   * @tc.name     ECDSA521_MD5withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_36400', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_MD5
    );
    huks.generateKey(aliasA, generateKeyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      console.log('HUKS_ALG_COMPLETION_36400 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36500
   * @tc.name     ECDSA521_NONEwithECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_36500', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_36500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36600
   * @tc.name     ECDSA521_SHA1withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_36600', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_36600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36700
   * @tc.name     ECDSA521_SHA224withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_36700', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_36700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36800
   * @tc.name     ECDSA521_SHA256withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_36800', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_36800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_36900
   * @tc.name     ECDSA521_SHA384withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_36900', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_36900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_37000
   * @tc.name     ECDSA521_SHA512withECDSA_AsyncCallback
   * @tc.desc     Test for sign and verify with ECDSA.
   */
  it('HUKS_ALG_COMPLETION_37000', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_37000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_37200
   * @tc.name     Regression_Testing_ED25519_CallBack
   * @tc.desc     Test for ED25519 Regression Testing.
   */
  it('HUKS_ALG_COMPLETION_37200', 0, async function (done) {
    signAndVerifyCallback(
      huks.HuksKeyAlg.HUKS_ALG_ED25519,
      huks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_37200'
    );
    setTimeout(function () {
    }, timer);
  });

  function isAliasBExist(done, caseId) {
    huks.isKeyExist(aliasB, emptyOption, function (err, data) {
      expect(data).assertEqual(false);
      console.log(caseId + ' end');
      done();
    })
  };

  function isAliasAExist(done, caseId) {
    huks.isKeyExist(aliasA, emptyOption, function (err, data) {
      expect(data).assertEqual(false);
      isAliasBExist(done, caseId);
    })
  };

  function deleteAliasB(done, caseId) {
    huks.deleteKey(aliasB, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      isAliasAExist(done, caseId);
    })
  };

  function deleteAliasA(done, caseId) {
    huks.deleteKey(aliasA, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      deleteAliasB(done, caseId);
    })
  };

  function verifyWithCallback(alg, size, padding, digest, plainText, done, caseId) {
    var verifyOptions = makeSignAndVerifyOption(
      alg,
      size,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      padding,
      digest,
      plainText
    );
    huks.verify(aliasB, verifyOptions, signedText, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      deleteAliasA(done, caseId);
    });
  };

  function importWithCallback(alg, size, padding, digest, plainText, done, caseId) {
    var importOption = makeImportOption(
      alg,
      size,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      padding,
      null,
      digest,
      publicKey
    );
    huks.importKey(aliasB, importOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      verifyWithCallback(alg, size, padding, digest, plainText, done, caseId);
    });
  };

  function exportWithCallback(alg, size, padding, digest, plainText, done, caseId) {
    huks.exportKey(aliasA, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      publicKey = data.outData;
      importWithCallback(alg, size, padding, digest, plainText, done, caseId);
    });
  };

  function signWithCallback(alg, size, padding, digest, done, caseId) {
    var plainText = makeRandomArr(16);
    var signOption = makeSignAndVerifyOption(
      alg,
      size,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      padding,
      digest,
      plainText
    );
    huks.sign(aliasA, signOption, function (err, data) {
      if (useLib == 'mbedtls' && (
        caseId == 'HUKS_ALG_COMPLETION_28900' ||
        caseId == 'HUKS_ALG_COMPLETION_29500' ||
        caseId == 'HUKS_ALG_COMPLETION_29600')) {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_CRYPTO_ENGINE_ERROR);
        done();
      } else {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
        signedText = data.outData;
        console.log('signedText ' + JSON.stringify(signedText));
        exportWithCallback(alg, size, padding, digest, plainText, done, caseId);
      }
    });
  };

  function signAndVerifyCallback(alg, size, padding, digest, done, caseId) {
    var generateKeyOption = makeGenerateKeyOption(
      alg,
      size,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      padding,
      null,
      digest
    );
    huks.generateKey(aliasA, generateKeyOption, function (err, data) {
      if (useLib == 'mbedtls' && alg == huks.HuksKeyAlg.HUKS_ALG_DSA) {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
        console.log(caseId + ' end');
        done();
      } else {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
        signWithCallback(alg, size, padding, digest, done, caseId);
      }
    });
  };
});