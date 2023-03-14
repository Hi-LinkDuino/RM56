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

import hks from '@ohos.security.huks'
import { describe, it, expect } from 'deccjsunit/index'
import {
  alias,
} from '../../../../../../hks_xts_common.test'
import {
  aesGenerateKeyInvalidSizeOption,
  aesGenerateKeyInvalidModeOption,
  aesGenerateKeyInvalidAlgOption,
  aesGenerateKeyInvalidPurposeOption,
  rsaGenerateKeyInvalidSizeOption,
  rsaGenerateKeyInvalidPaddingOption,
  rsaGenerateKeyInvalidPurposeOption,
  eccGenerateKeyInvalidSizeOption,
  eccGenerateKeyInvalidDigestOption,
  eccGenerateKeyInvalidPurposeOption,
  ecdhGenerateKeyInvalidAlgOption,
  hkdfGenerateKeyInvalidAlgOption,
  pbkdf2GenerateKeyInvalidAlgOption,
  ed25519GenerateKeyInvalidPurposeOption,
  x25519GenerateKeyInvalidPurposeOption,
  hmacGenerateKeyInvalidSizeOption,
  hmacGenerateKeyInvalidPurposeOption,
  dsaGenerateKeyInvalidSizeOption,
  dsaGenerateKeyInvalidPurposeOption,
  dhGenerateKeyInvalidPurposeOption,
} from './hks_abnormal_common.test.js'

describe('Hks_Abnormal_Callback_Part2', function () {
  var errHmacKeySizeA = 0;
  var errHmacKeySizeB = 9;
  var errDsaKeySizeA = 0;
  var errDsaKeySizeB = 9;

  /**
    * @tc.number   HUKS_ABNORMAL_CALLBACK_11000
    * @tc.name     InvalidAesKeySize512AlgAesForGenerateGKeyCallback
    * @tc.desc     Invalid aes key size 512 alg aes for generate key callback.
    */
  it('HUKS_ABNORMAL_CALLBACK_11000', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
  * @tc.number   HUKS_ABNORMAL_CALLBACK_11100
  * @tc.name     InvalidRsaKeySize512AlgAesForGenerateGKeyCallback
  * @tc.desc     Invalid rsa key size 512 alg aes for generate key callback.
  */
  it('HUKS_ABNORMAL_CALLBACK_11100', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_11200
   * @tc.name     InvalidRsaKeySize768AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 768 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_11200', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_768);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_11300
   * @tc.name     InvalidRsaKeySize1024AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 1024 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_11300', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_11400
   * @tc.name     InvalidRsaKeySize2048AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 2048 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_11400', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_11500
   * @tc.name     InvalidRsaKeySize3072AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 3072 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_11500', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_11600
   * @tc.name     InvalidRsaKeySize4096AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 4096 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_11600', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_11700
   * @tc.name     InvalidEccKeySize224AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid ecc key size 224 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_11700', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_11800
   * @tc.name     InvalidEccKeySize256AlgAesForGenerateGKeyCallback
   * @tc.desc     Ecc key size 256 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_11800', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_11900
   * @tc.name     InvalidEccKeySize384AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid ecc key size 384 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_11900', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12000
   * @tc.name     InvalidEccKeySize521AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid ecc key size 521 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12000', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12100
   * @tc.name     InvalidCurve25519KeySize256AlgAesForGenerateGKeyCallback
   * @tc.desc     Curve25519 key size 256 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12100', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12200
   * @tc.name     InvaliddhKeySize2048AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid dh key size 2048 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12200', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12300
   * @tc.name     InvaliddhKeySize3072AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid dh key size 2048 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12300', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12400
   * @tc.name     InvaliddhKeySize4096AlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid dh key size 4096 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12400', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12500
   * @tc.name     InvalidPaddingOaepModeCbcAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding oaep mode cbc alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12500', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CBC,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12600
   * @tc.name     InvalidPaddingPssModeCbcAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pss mode cbc alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12600', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_CBC, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12700
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeCbcAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs1_v1_5 mode cbc alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12700', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CBC,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12800
   * @tc.name     InvalidPaddingPkcs5ModeCbcAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs5 mode cbc alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12800', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CBC,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_12900
   * @tc.name     InvalidPaddingOaepModeCtrAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding oaep mode ctr alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_12900', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CTR,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13000
   * @tc.name     InvalidPaddingPssModeCtrAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pss mode ctr alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13000', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_CTR, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13100
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeCtrAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs1_v1_5 mode ctr alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13100', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CTR,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13200
   * @tc.name     InvalidPaddingPkcs5ModeCtrAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs5 mode ctr alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13200', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CTR,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13300
   * @tc.name     InvalidPaddingPkcs7ModeCtrAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs7 mode ctr alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13300', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CTR,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13400
   * @tc.name     InvalidPaddingOaepModeEcbAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding oaep mode ecb alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13400', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_ECB,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13500
   * @tc.name     InvalidPaddingPssModeEcbAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pss mode ecb alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13500', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_ECB, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13600
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeEcbAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs1_v1_5 mode ecb alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13600', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_ECB,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13700
   * @tc.name     InvalidPaddingPkcs5ModeEcbAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs5 mode ecb alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13700', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_ECB,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13800
   * @tc.name     InvalidPaddingOaepModeGcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding oaep mode gcm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13800', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_GCM,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13900
   * @tc.name     InvalidPaddingpssModeGcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pss mode gcm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_13900', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_GCM, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14000
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeGcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs1_v1_5 mode gcm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14000', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_GCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14100
   * @tc.name     InvalidPaddingPkcs5ModeGcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs5 mode gcm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14100', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_GCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14200
   * @tc.name     InvalidPaddingPkcs7ModeGcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs7 mode gcm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14200', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_GCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14300
   * @tc.name     InvalidPaddingOaepModeCcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding oaep mode ccm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14300', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CCM,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14400
   * @tc.name     InvalidPaddingPssModeCcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pss mode ccm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14400', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_CCM, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14500
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeCcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs1_v1_5 mode ccm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14500', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14600
   * @tc.name     InvalidPaddingPkcs5ModeCcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs5 mode ccm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14600', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14700
   * @tc.name     InvalidPaddingPkcs7ModeCcmAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid padding pkcs7 mode ccm alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14700', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14800
   * @tc.name     InvalidModeOfbAlgAesForGenerateGKeyCallback
   * @tc.desc     Invalid mode ofb alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14800', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_OFB,
      hks.HuksKeyPadding.HUKS_PADDING_NONE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_MODE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14900
   * @tc.name     InvalidPurposeDeriveAlgRsaAlgAesForGenerateKeyCallback.
   * @tc.desc     Invalid purpose derive alg rsa alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_14900', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_RSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15000
   * @tc.name     InvalidPurposeDeriveAlgEccAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg ecc alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15000', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECC,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15100
   * @tc.name     InvalidPurposeDeriveAlgDsaAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg dsa alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15100', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15200
   * @tc.name     InvalidPurposeDeriveAlgHkdfAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg hkdf alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15200', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_HKDF,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15300
   * @tc.name     InvalidPurposeDeriveAlgPbkdf2AlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg pbkdf2 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15300', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_PBKDF2,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15400
   * @tc.name     InvalidPurposeDeriveAlgEcdhAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg ecdh alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15400', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECDH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15500
   * @tc.name     InvalidPurposeDeriveAlgX25519AlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg x25519 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15500', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_X25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15600
   * @tc.name     InvalidPurposeDeriveAlgEd25519AlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg ed25519 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15600', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ED25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15700
   * @tc.name     InvalidPurposeDeriveAlgDhAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg dh alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15700', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15800
   * @tc.name     InvalidPurposeMacAlgRsaAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg rsa alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15800', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_RSA, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15900
   * @tc.name     InvalidPurposeMacAlgEccAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg ecc alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_15900', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECC, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16000
   * @tc.name     InvalidPurposeMacAlgDsaAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg dsa alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16000', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DSA, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16100
   * @tc.name     InvalidPurposeMacAlgHkdfAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg hkdf alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16100', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16200
   * @tc.name     InvalidPurposeMacAlgPbkdf2AlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg pbkdf2 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16200', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_PBKDF2,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16300
   * @tc.name     InvalidPurposeMacAlgEcdhAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg ecdh alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16300', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16400
   * @tc.name     InvalidPurposeMacAlgX25519AlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg x25519 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16400', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_X25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16500
   * @tc.name     InvalidPurposeMacAlgEd25519AlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg ed25519 alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16500', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ED25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16600
   * @tc.name     InvalidPurposeMacAlgDhAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg dh alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16600', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16700
   * @tc.name     InvalidPurposeSignAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose sign alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16700', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(aesGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN;
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16800
   * @tc.name     InvalidPurposeVerifyAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose verify alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16800', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(aesGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY;
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16900
   * @tc.name     InvalidPurposeAgreeAlgAesForGenerateKeyCallback
   * @tc.desc     Invalid purpose agree alg aes for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_16900', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(aesGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE;
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17000
   * @tc.name     InvalidEccKeySize224AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid ecc key size 224 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17000', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17100
   * @tc.name     InvalidEccKeySize256AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid ecc key size 256 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17100', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17200
   * @tc.name     InvalidEccKeySize384AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid ecc key size 384 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17200', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17300
   * @tc.name     InvalidEccKeySize521AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid ecc key size 521 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17300', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17400
   * @tc.name     InvalidAesKeySize128AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid aes key size 128 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17400', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17500
   * @tc.name     InvalidAesKeySize192AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid aes key size 192 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17500', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_192);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17600
   * @tc.name     InvalidAesKeySize256AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid aes key size 256 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17600', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_256);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17700
   * @tc.name     InvalidAesKeySize512AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid aes key size 512 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17700', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17800
   * @tc.name     InvalidCurve25519KeySize256AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid curve25519 key size 256 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17800', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17900
   * @tc.name     InvalidDhKeySize2048AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid dh key size 2048 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_17900', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18000
   * @tc.name     InvalidDhKeySize3072AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid dh key size 3072 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18000', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18100
   * @tc.name     InvalidDhKeySize4096AlgRsaForGenerateGKeyCallback
   * @tc.desc     Invalid dh key size 4096 alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18100', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18200
   * @tc.name     InvalidPaddingPssPurposeEncryptDecryptAlgRsaForGenerateKeyCallback.
   * @tc.desc     Invalid padding pss purpose encrypt decrypt alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18200', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PSS);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18300
   * @tc.name     InvalidPaddingPkcs5PurposeEncryptDecryptAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid padding pkcs5 purpose encrypt decrypt alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18300', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18400
   * @tc.name     InvalidPaddingPkcs7PurposeEncryptDecryptAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid padding pkcs7 purpose encrypt decrypt alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18400', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18500
   * @tc.name     InvalidPaddingNonePurposeSignVerifyAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid padding none purpose sign verify alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18500', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_NONE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18600
   * @tc.name     InvalidPaddingOaepPurposeSignVerifyAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid padding oaep purpose sign verify alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18600', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18700
   * @tc.name     InvalidPaddingPkcs5PurposeSignVerifyAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid padding pkcs5 purpose sign verify alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18700', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18800
   * @tc.name     InvalidPaddingPkcs7PurposeSignVerifyAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid padding pkcs7 purpose sign verify alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18800', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18900
   * @tc.name     InvalidPurposeDeriveAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_18900', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE;
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19000
   * @tc.name     InvalidPurposeMacAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19000', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC;
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19100
   * @tc.name     InvalidPurposeWrapAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose wrap alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19100', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP;
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19200
   * @tc.name     InvalidPurposeUnwrapAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose unwrap alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19200', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP;
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19300
   * @tc.name     InvalidPurposeagreeAlgRsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose agree alg rsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19300', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE;
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19400
   * @tc.name     InvalidRsaKeySize512AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 512 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19400', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19500
   * @tc.name     InvalidRsaKeySize768AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 768 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19500', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_768);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19600
   * @tc.name     InvalidRsaKeySize1024AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 1024 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19600', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19700
   * @tc.name     InvalidRsaKeySize2048AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 2048 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19700', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19800
   * @tc.name     InvalidRsaKeySize3072AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 3072 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19800', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19900
   * @tc.name     InvalidRsaKeySize4096AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid rsa key size 4096 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_19900', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20000
   * @tc.name     InvalidAesKeySize128AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid aes key size 128 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20000', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20100
   * @tc.name     InvalidAesKeySize192AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid aes key size 192 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20100', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_192);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20200
   * @tc.name     InvalidAesKeySize256AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid aes key size 256 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20200', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_256);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20300
   * @tc.name     InvalidAesKeySize512AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid aes key size 512 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20300', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20400
   * @tc.name     InvalidCurve25519KeySize256AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid curve25519 key size 256 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20400', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20500
   * @tc.name     InvalidDhKeySize2048AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid dh key size 2048 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20500', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20600
   * @tc.name     InvalidDhKeySize3072AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid dh key size 3072 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20600', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20700
   * @tc.name     InvalidDhKeySize4096AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid dh key size 4096 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20700', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20800
   * @tc.name     InvalidDigestMd5AlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid digest md5 alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20800', 0, async function (done) {
    var option = eccGenerateKeyInvalidDigestOption(hks.HuksKeyDigest.HUKS_DIGEST_MD5);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20900
   * @tc.name     InvalidPurposeDeriveAlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid purpose derive alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_20900', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21000
   * @tc.name     InvalidPurposeMacAlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid purpose mac alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21000', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21100
   * @tc.name     InvalidPurposeWrapAlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid purpose wrap alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21100', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });

  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21200
   * @tc.name     InvalidPurposeUnwrapAlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid purpose unwrap alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21200', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21300
   * @tc.name     InvalidPurposeencryptAlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid purpose encrypt alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21300', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21400
   * @tc.name     InvalidPurposeDecryptAlgEccForGenerateGKeyCallback
   * @tc.desc     Invalid purpose decrypt alg ecc for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21400', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21500
   * @tc.name     InvalidArgumentAlgEcdhForGenerateGKeyCallback
   * @tc.desc     Invalid argument alg ecdh for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21500', 0, async function (done) {
    var option = ecdhGenerateKeyInvalidAlgOption();
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21600
   * @tc.name     InvalidAlgorithmAlgHkdfForGenerateGKeyCallback
   * @tc.desc     Invalid algorithm alg hkdf for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21600', 0, async function (done) {
    var option = hkdfGenerateKeyInvalidAlgOption();
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21700
   * @tc.name     InvalidAlgorithmAlgPbkdf2ForGenerateGKeyCallback
   * @tc.desc     Invalid algorithm alg pbkdf2 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21700', 0, async function (done) {
    var option = pbkdf2GenerateKeyInvalidAlgOption();
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21800
   * @tc.name     InvalidPurposeDeriveAlgEd25519ForGenerateKeyCallback.
   * @tc.desc     Invalid purpose derive alg ed25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21800', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21900
   * @tc.name     InvalidPurposeMacAlgEd25519ForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg ed25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_21900', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22000
   * @tc.name     InvalidPurposeWrapAlgEd25519ForGenerateKeyCallback
   * @tc.desc     Invalid purpose wrap alg ed25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22000', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22100
   * @tc.name     InvalidPurposeUnwrapAlgEd25519ForGenerateKeyCallback
   * @tc.desc     Invalid purpose unwrap alg ed25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22100', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22200
   * @tc.name     InvalidPurposeEncryptAlgEd25519ForGenerateKeyCallback
   * @tc.desc     Invalid purpose encrypt alg ed25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22200', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22300
   * @tc.name     InvalidPurposeDecryptAlgEd25519ForGenerateKeyCallback
   * @tc.desc     Invalid purpose decrypt alg ed25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22300', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22400
   * @tc.name     InvalidPurposeDeriveAlgX25519ForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg x25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22400', 0, async function (done) {
    var option = x25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22500
   * @tc.name     InvalidPurposeMacAlgX25519ForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg x25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22500', 0, async function (done) {
    var option = x25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22600
   * @tc.name     InvalidPurposeEncryptAlgX25519ForGenerateKeyCallback
   * @tc.desc     Invalid purpose encrypt alg x25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22600', 0, async function (done) {
    var option = x25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22700
   * @tc.name     InvalidPurposeDecryptAlgX25519ForGenerateKeyCallback
   * @tc.desc     Invalid purpose decrypt alg x25519 for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22700', 0, async function (done) {
    var option = x25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22800
   * @tc.name     InvalidArgumentSizeaAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid argument sizea alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22800', 0, async function (done) {
    var option = hmacGenerateKeyInvalidSizeOption(errHmacKeySizeA);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22900
   * @tc.name     InvalidArgumentSizebAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid argument sizeb alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_22900', 0, async function (done) {
    var option = hmacGenerateKeyInvalidSizeOption(errHmacKeySizeB);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23000
   * @tc.name     InvalidPurposeDeriveAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23000', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23100
   * @tc.name     InvalidPurposeSignAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid purpose sign alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23100', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23200
   * @tc.name     InvalidPurposeVerifyAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid purpose verify alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23200', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23300
   * @tc.name     InvalidPurposeWrapAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid purpose wrap alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23300', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23400
   * @tc.name     InvalidPurposeUnwrapAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid purpose unwrap alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23400', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23500
   * @tc.name     InvalidPurposeEncryptAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid purpose encrypt alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23500', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23600
   * @tc.name     InvalidPurposeDecryptAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid purpose decrypt alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23600', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23700
   * @tc.name     InvalidPurposeAgreeAlgHmacForGenerateKeyCallback
   * @tc.desc     Invalid purpose agree alg hmac for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23700', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23800
   * @tc.name     InvalidArgumentSizeaAlgDsaForGenerateKeyCallback
   * @tc.desc     Invalid argument sizea alg dsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23800', 0, async function (done) {
    var option = dsaGenerateKeyInvalidSizeOption(errDsaKeySizeA);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23900
   * @tc.name     InvalidArgumentSizebAlgDsaForGenerateKeyCallback
   * @tc.desc     Invalid argument sizeb alg dsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_23900', 0, async function (done) {
    var option = dsaGenerateKeyInvalidSizeOption(errDsaKeySizeB);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24000
   * @tc.name     InvalidPurposeDeriveAlgDsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg dsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24000', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24100
   * @tc.name     InvalidPurposeMacAlgDsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg dsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24100', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24200
   * @tc.name     InvalidPurposeWrapAlgDsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose wrap alg dsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24200', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24300
   * @tc.name     InvalidPurposeUnwrapAlgDsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose unwrap alg dsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24300', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24400
   * @tc.name     InvalidPurposeEncryptAlgDsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose encrypt alg dsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24400', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24500
   * @tc.name     InvalidPurposeDecryptAlgDsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose decrypt alg dsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24500', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24600
   * @tc.name     InvalidPurposeAgreeAlgDsaForGenerateKeyCallback
   * @tc.desc     Invalid purpose agree alg dsa for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24600', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24700
   * @tc.name     InvalidPurposeDeriveAlgDhForGenerateKeyCallback
   * @tc.desc     Invalid purpose derive alg dh for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24700', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24800
   * @tc.name     InvalidPurposeMacAlgDhForGenerateKeyCallback
   * @tc.desc     Invalid purpose mac alg dh for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24800', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24900
   * @tc.name     InvalidPurposeWrapAlgDhForGenerateKeyCallback
   * @tc.desc     Invalid purpose wrap alg dh for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_24900', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25000
   * @tc.name     InvalidPurposeUnwrapAlgDhForGenerateKeyCallback
   * @tc.desc     Invalid purpose unwrap alg dh for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25000', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25100
   * @tc.name     InvalidPurposeEncryptAlgDhForGenerateKeyCallback
   * @tc.desc     Invalid purpose encrypt alg dh for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25100', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25200
   * @tc.name     InvalidPurposeDecryptAlgDhForGenerateKeyCallback
   * @tc.desc     Invalid purpose decrypt alg dh for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25200', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25300
   * @tc.name     InvalidPurposeSignAlgDhForGenerateKeyCallback
   * @tc.desc     Invalid purpose sign alg dh for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25300', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25400
   * @tc.name     InvalidPurposeVerifyAlgDhForGenerateKeyCallback
   * @tc.desc     Invalid purpose verify alg dh for generate key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25400', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    hks.generateKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });
});

