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

describe('Hks_Abnormal_Promise_Part2', function () {
  var errHmacKeySizeA = 0;
  var errHmacKeySizeB = 9;
  var errDsaKeySizeA = 0;
  var errDsaKeySizeB = 9;

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11000
   * @tc.name     InvalidAesKeySize512AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid aes key size 512 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11000', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11100
   * @tc.name     InvalidRsaKeySize512AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 512 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11100', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11200
   * @tc.name     InvalidRsaKeySize768AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 768 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11200', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_768);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11300
   * @tc.name     InvalidRsaKeySize1024AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 1024 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11300', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11400
   * @tc.name     InvalidRsaKeySize2048AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 2048 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11400', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11500
   * @tc.name     InvalidRsaKeySize3072AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 3072 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11500', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11600
   * @tc.name     InvalidRsaKeySize4096AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 4096 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11600', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11700
   * @tc.name     InvalidEccKeySize224AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid ecc key size 224 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11700', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11800
   * @tc.name     InvalidEccKeySize256AlgAesForGenerateGKeyPromise
   * @tc.desc     Ecc key size 256 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11800', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_11900
   * @tc.name     InvalidEccKeySize384AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid ecc key size 384 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_11900', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12000
   * @tc.name     InvalidEccKeySize521AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid ecc key size 521 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12000', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12100
   * @tc.name     InvalidCurve25519KeySize256AlgAesForGenerateGKeyPromise
   * @tc.desc     Curve25519 key size 256 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12100', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12200
   * @tc.name     InvaliddhKeySize2048AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid dh key size 2048 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12200', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12300
   * @tc.name     InvaliddhKeySize3072AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid dh key size 2048 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12300', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12400
   * @tc.name     InvaliddhKeySize4096AlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid dh key size 4096 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12400', 0, async function (done) {
    var option = aesGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12500
   * @tc.name     InvalidPaddingOaepModeCbcAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding oaep mode cbc alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12500', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CBC,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12600
   * @tc.name     InvalidPaddingPssModeCbcAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pss mode cbc alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12600', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_CBC, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12700
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeCbcAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs1_v1_5 mode cbc alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12700', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CBC,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12800
   * @tc.name     InvalidPaddingPkcs5ModeCbcAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs5 mode cbc alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12800', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CBC,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_12900
   * @tc.name     InvalidPaddingOaepModeCtrAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding oaep mode ctr alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_12900', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CTR,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_13000
   * @tc.name     InvalidPaddingPssModeCtrAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pss mode ctr alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13000', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_CTR, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_13100
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeCtrAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs1_v1_5 mode ctr alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13100', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CTR,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_13200
   * @tc.name     InvalidPaddingPkcs5ModeCtrAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs5 mode ctr alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13200', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CTR,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13300
   * @tc.name     InvalidPaddingPkcs7ModeCtrAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs7 mode ctr alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13300', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CTR,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13400
   * @tc.name     InvalidPaddingOaepModeEcbAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding oaep mode ecb alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13400', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_ECB,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13500
   * @tc.name     InvalidPaddingPssModeEcbAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pss mode ecb alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13500', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_ECB, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13600
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeEcbAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs1_v1_5 mode ecb alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13600', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_ECB,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13700
   * @tc.name     InvalidPaddingPkcs5ModeEcbAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs5 mode ecb alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13700', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_ECB,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13800
   * @tc.name     InvalidPaddingOaepModeGcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding oaep mode gcm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13800', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_GCM,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_13900
   * @tc.name     InvalidPaddingpssModeGcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pss mode gcm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_13900', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_GCM, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14000
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeGcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs1_v1_5 mode gcm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14000', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_GCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14100
   * @tc.name     InvalidPaddingPkcs5ModeGcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs5 mode gcm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14100', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_GCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14200
   * @tc.name     InvalidPaddingPkcs7ModeGcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs7 mode gcm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14200', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_GCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14300
   * @tc.name     InvalidPaddingOaepModeCcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding oaep mode ccm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14300', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CCM,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14400
   * @tc.name     InvalidPaddingPssModeCcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pss mode ccm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14400', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(hks.HuksCipherMode.HUKS_MODE_CCM, hks.HuksKeyPadding.HUKS_PADDING_PSS);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14500
   * @tc.name     InvalidPaddingPkcs1_v1_5ModeCcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs1_v1_5 mode ccm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14500', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14600
   * @tc.name     InvalidPaddingPkcs5ModeCcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs5 mode ccm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14600', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14700
   * @tc.name     InvalidPaddingPkcs7ModeCcmAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid padding pkcs7 mode ccm alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14700', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_CCM,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14800
   * @tc.name     InvalidModeOfbAlgAesForGenerateGKeyPromise
   * @tc.desc     Invalid mode ofb alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14800', 0, async function (done) {
    var option = aesGenerateKeyInvalidModeOption(
      hks.HuksCipherMode.HUKS_MODE_OFB,
      hks.HuksKeyPadding.HUKS_PADDING_NONE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_MODE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_14900
   * @tc.name     InvalidPurposeDeriveAlgRsaAlgAesForGenerateKeyPromise.
   * @tc.desc     Invalid purpose derive alg rsa alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_14900', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_RSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15000
   * @tc.name     InvalidPurposeDeriveAlgEccAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg ecc alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15000', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECC,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15100
   * @tc.name     InvalidPurposeDeriveAlgDsaAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg dsa alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15100', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15200
   * @tc.name     InvalidPurposeDeriveAlgHkdfAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg hkdf alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15200', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_HKDF,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15300
   * @tc.name     InvalidPurposeDeriveAlgPbkdf2AlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg pbkdf2 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15300', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_PBKDF2,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15400
   * @tc.name     InvalidPurposeDeriveAlgEcdhAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg ecdh alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15400', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECDH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15500
   * @tc.name     InvalidPurposeDeriveAlgX25519AlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg x25519 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15500', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_X25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15600
   * @tc.name     InvalidPurposeDeriveAlgEd25519AlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg ed25519 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15600', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ED25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15700
   * @tc.name     InvalidPurposeDeriveAlgDhAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg dh alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15700', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15800
   * @tc.name     InvalidPurposeMacAlgRsaAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg rsa alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15800', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_RSA, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_15900
   * @tc.name     InvalidPurposeMacAlgEccAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg ecc alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_15900', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECC, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16000
   * @tc.name     InvalidPurposeMacAlgDsaAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg dsa alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16000', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DSA, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16100
   * @tc.name     InvalidPurposeMacAlgHkdfAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg hkdf alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16100', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16200
   * @tc.name     InvalidPurposeMacAlgPbkdf2AlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg pbkdf2 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16200', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_PBKDF2,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16300
   * @tc.name     InvalidPurposeMacAlgEcdhAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg ecdh alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16300', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16400
   * @tc.name     InvalidPurposeMacAlgX25519AlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg x25519 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16400', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_X25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16500
   * @tc.name     InvalidPurposeMacAlgEd25519AlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg ed25519 alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16500', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ED25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16600
   * @tc.name     InvalidPurposeMacAlgDhAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg dh alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16600', 0, async function (done) {
    var option = aesGenerateKeyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH, hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16700
   * @tc.name     InvalidPurposeSignAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose sign alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16700', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(aesGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN;
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16800
   * @tc.name     InvalidPurposeVerifyAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose verify alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16800', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(aesGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY;
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_16900
   * @tc.name     InvalidPurposeAgreeAlgAesForGenerateKeyPromise
   * @tc.desc     Invalid purpose agree alg aes for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_16900', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(aesGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE;
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17000
   * @tc.name     InvalidEccKeySize224AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid ecc key size 224 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17000', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17100
   * @tc.name     InvalidEccKeySize256AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid ecc key size 256 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17100', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17200
   * @tc.name     InvalidEccKeySize384AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid ecc key size 384 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17200', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17300
   * @tc.name     InvalidEccKeySize521AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid ecc key size 521 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17300', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17400
   * @tc.name     InvalidAesKeySize128AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid aes key size 128 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17400', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17500
   * @tc.name     InvalidAesKeySize192AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid aes key size 192 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17500', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_192);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17600
   * @tc.name     InvalidAesKeySize256AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid aes key size 256 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17600', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_256);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17700
   * @tc.name     InvalidAesKeySize512AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid aes key size 512 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17700', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17800
   * @tc.name     InvalidCurve25519KeySize256AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid curve25519 key size 256 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17800', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_17900
   * @tc.name     InvalidDhKeySize2048AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid dh key size 2048 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_17900', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18000
   * @tc.name     InvalidDhKeySize3072AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid dh key size 3072 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18000', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18100
   * @tc.name     InvalidDhKeySize4096AlgRsaForGenerateGKeyPromise
   * @tc.desc     Invalid dh key size 4096 alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18100', 0, async function (done) {
    var option = rsaGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18200
   * @tc.name     InvalidPaddingPssPurposeEncryptDecryptAlgRsaForGenerateKeyPromise.
   * @tc.desc     Invalid padding pss purpose encrypt decrypt alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18200', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PSS);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18300
   * @tc.name     InvalidPaddingPkcs5PurposeEncryptDecryptAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid padding pkcs5 purpose encrypt decrypt alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18300', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18400
   * @tc.name     InvalidPaddingPkcs7PurposeEncryptDecryptAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid padding pkcs7 purpose encrypt decrypt alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18400', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18500
   * @tc.name     InvalidPaddingNonePurposeSignVerifyAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid padding none purpose sign verify alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18500', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_NONE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18600
   * @tc.name     InvalidPaddingOaepPurposeSignVerifyAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid padding oaep purpose sign verify alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18600', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18700
   * @tc.name     InvalidPaddingPkcs5PurposeSignVerifyAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid padding pkcs5 purpose sign verify alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18700', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18800
   * @tc.name     InvalidPaddingPkcs7PurposeSignVerifyAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid padding pkcs7 purpose sign verify alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18800', 0, async function (done) {
    var option = rsaGenerateKeyInvalidPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_18900
   * @tc.name     InvalidPurposeDeriveAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_18900', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE;
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19000
   * @tc.name     InvalidPurposeMacAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19000', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC;
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19100
   * @tc.name     InvalidPurposeWrapAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose wrap alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19100', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP;
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19200
   * @tc.name     InvalidPurposeUnwrapAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose unwrap alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19200', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP;
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19300
   * @tc.name     InvalidPurposeagreeAlgRsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose agree alg rsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19300', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(rsaGenerateKeyInvalidPurposeOption));
    option.properties[2].value = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE;
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19400
   * @tc.name     InvalidRsaKeySize512AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 512 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19400', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19500
   * @tc.name     InvalidRsaKeySize768AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 768 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19500', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_768);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19600
   * @tc.name     InvalidRsaKeySize1024AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 1024 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19600', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19700
   * @tc.name     InvalidRsaKeySize2048AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 2048 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19700', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19800
   * @tc.name     InvalidRsaKeySize3072AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 3072 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19800', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_19900
   * @tc.name     InvalidRsaKeySize4096AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid rsa key size 4096 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_19900', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20000
   * @tc.name     InvalidAesKeySize128AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid aes key size 128 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20000', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20100
   * @tc.name     InvalidAesKeySize192AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid aes key size 192 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20100', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_192);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20200
   * @tc.name     InvalidAesKeySize256AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid aes key size 256 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20200', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_256);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20300
   * @tc.name     InvalidAesKeySize512AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid aes key size 512 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20300', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20400
   * @tc.name     InvalidCurve25519KeySize256AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid curve25519 key size 256 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20400', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20500
   * @tc.name     InvalidDhKeySize2048AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid dh key size 2048 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20500', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20600
   * @tc.name     InvalidDhKeySize3072AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid dh key size 3072 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20600', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20700
   * @tc.name     InvalidDhKeySize4096AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid dh key size 4096 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20700', 0, async function (done) {
    var option = eccGenerateKeyInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20800
   * @tc.name     InvalidDigestMd5AlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid digest md5 alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20800', 0, async function (done) {
    var option = eccGenerateKeyInvalidDigestOption(hks.HuksKeyDigest.HUKS_DIGEST_MD5);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_20900
   * @tc.name     InvalidPurposeDeriveAlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid purpose derive alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_20900', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21000
   * @tc.name     InvalidPurposeMacAlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid purpose mac alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21000', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21100
   * @tc.name     InvalidPurposeWrapAlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid purpose wrap alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21100', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21200
   * @tc.name     InvalidPurposeUnwrapAlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid purpose unwrap alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21200', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21300
   * @tc.name     InvalidPurposeencryptAlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid purpose encrypt alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21300', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21400
   * @tc.name     InvalidPurposeDecryptAlgEccForGenerateGKeyPromise
   * @tc.desc     Invalid purpose decrypt alg ecc for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21400', 0, async function (done) {
    var option = eccGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21500
   * @tc.name     InvalidArgumentAlgEcdhForGenerateGKeyPromise
   * @tc.desc     Invalid argument alg ecdh for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21500', 0, async function (done) {
    var option = ecdhGenerateKeyInvalidAlgOption();
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21600
   * @tc.name     InvalidAlgorithmAlgHkdfForGenerateGKeyPromise
   * @tc.desc     Invalid algorithm alg hkdf for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21600', 0, async function (done) {
    var option = hkdfGenerateKeyInvalidAlgOption();
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21700
   * @tc.name     InvalidAlgorithmAlgPbkdf2ForGenerateGKeyPromise
   * @tc.desc     Invalid algorithm alg pbkdf2 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21700', 0, async function (done) {
    var option = pbkdf2GenerateKeyInvalidAlgOption();
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21800
   * @tc.name     InvalidPurposeDeriveAlgEd25519ForGenerateKeyPromise.
   * @tc.desc     Invalid purpose derive alg ed25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21800', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_21900
   * @tc.name     InvalidPurposeMacAlgEd25519ForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg ed25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_21900', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22000
   * @tc.name     InvalidPurposeWrapAlgEd25519ForGenerateKeyPromise
   * @tc.desc     Invalid purpose wrap alg ed25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22000', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22100
   * @tc.name     InvalidPurposeUnwrapAlgEd25519ForGenerateKeyPromise
   * @tc.desc     Invalid purpose unwrap alg ed25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22100', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22200
   * @tc.name     InvalidPurposeEncryptAlgEd25519ForGenerateKeyPromise
   * @tc.desc     Invalid purpose encrypt alg ed25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22200', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22300
   * @tc.name     InvalidPurposeDecryptAlgEd25519ForGenerateKeyPromise
   * @tc.desc     Invalid purpose decrypt alg ed25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22300', 0, async function (done) {
    var option = ed25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22400
   * @tc.name     InvalidPurposeDeriveAlgX25519ForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg x25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22400', 0, async function (done) {
    var option = x25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22500
   * @tc.name     InvalidPurposeMacAlgX25519ForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg x25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22500', 0, async function (done) {
    var option = x25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22600
   * @tc.name     InvalidPurposeEncryptAlgX25519ForGenerateKeyPromise
   * @tc.desc     Invalid purpose encrypt alg x25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22600', 0, async function (done) {
    var option = x25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22700
   * @tc.name     InvalidPurposeDecryptAlgX25519ForGenerateKeyPromise
   * @tc.desc     Invalid purpose decrypt alg x25519 for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22700', 0, async function (done) {
    var option = x25519GenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22800
   * @tc.name     InvalidArgumentSizeaAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid argument sizea alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22800', 0, async function (done) {
    var option = hmacGenerateKeyInvalidSizeOption(errHmacKeySizeA);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_22900
   * @tc.name     InvalidArgumentSizebAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid argument sizeb alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_22900', 0, async function (done) {
    var option = hmacGenerateKeyInvalidSizeOption(errHmacKeySizeB);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23000
   * @tc.name     InvalidPurposeDeriveAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23000', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23100
   * @tc.name     InvalidPurposeSignAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid purpose sign alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23100', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23200
   * @tc.name     InvalidPurposeVerifyAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid purpose verify alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23200', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23300
   * @tc.name     InvalidPurposeWrapAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid purpose wrap alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23300', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23400
   * @tc.name     InvalidPurposeUnwrapAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid purpose unwrap alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23400', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23500
   * @tc.name     InvalidPurposeEncryptAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid purpose encrypt alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23500', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23600
   * @tc.name     InvalidPurposeDecryptAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid purpose decrypt alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23600', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23700
   * @tc.name     InvalidPurposeAgreeAlgHmacForGenerateKeyPromise
   * @tc.desc     Invalid purpose agree alg hmac for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23700', 0, async function (done) {
    var option = hmacGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23800
   * @tc.name     InvalidArgumentSizeaAlgDsaForGenerateKeyPromise
   * @tc.desc     Invalid argument sizea alg dsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23800', 0, async function (done) {
    var option = dsaGenerateKeyInvalidSizeOption(errDsaKeySizeA);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_23900
   * @tc.name     InvalidArgumentSizebAlgDsaForGenerateKeyPromise
   * @tc.desc     Invalid argument sizeb alg dsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_23900', 0, async function (done) {
    var option = dsaGenerateKeyInvalidSizeOption(errDsaKeySizeB);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24000
   * @tc.name     InvalidPurposeDeriveAlgDsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg dsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24000', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24100
   * @tc.name     InvalidPurposeMacAlgDsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg dsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24100', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24200
   * @tc.name     InvalidPurposeWrapAlgDsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose wrap alg dsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24200', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24300
   * @tc.name     InvalidPurposeUnwrapAlgDsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose unwrap alg dsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24300', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24400
   * @tc.name     InvalidPurposeEncryptAlgDsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose encrypt alg dsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24400', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24500
   * @tc.name     InvalidPurposeDecryptAlgDsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose decrypt alg dsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24500', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24600
   * @tc.name     InvalidPurposeAgreeAlgDsaForGenerateKeyPromise
   * @tc.desc     Invalid purpose agree alg dsa for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24600', 0, async function (done) {
    var option = dsaGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24700
   * @tc.name     InvalidPurposeDeriveAlgDhForGenerateKeyPromise
   * @tc.desc     Invalid purpose derive alg dh for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24700', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24800
   * @tc.name     InvalidPurposeMacAlgDhForGenerateKeyPromise
   * @tc.desc     Invalid purpose mac alg dh for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24800', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_24900
   * @tc.name     InvalidPurposeWrapAlgDhForGenerateKeyPromise
   * @tc.desc     Invalid purpose wrap alg dh for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_24900', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25000
   * @tc.name     InvalidPurposeUnwrapAlgDhForGenerateKeyPromise
   * @tc.desc     Invalid purpose unwrap alg dh for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_25000', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25100
   * @tc.name     InvalidPurposeEncryptAlgDhForGenerateKeyPromise
   * @tc.desc     Invalid purpose encrypt alg dh for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_25100', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25200
   * @tc.name     InvalidPurposeDecryptAlgDhForGenerateKeyPromise
   * @tc.desc     Invalid purpose decrypt alg dh for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_25200', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25300
   * @tc.name     InvalidPurposeSignAlgDhForGenerateKeyPromise
   * @tc.desc     Invalid purpose sign alg dh for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_25300', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25400
   * @tc.name     InvalidPurposeVerifyAlgDhForGenerateKeyPromise
   * @tc.desc     Invalid purpose verify alg dh for generate key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_25400', 0, async function (done) {
    var option = dhGenerateKeyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    var result = await hks.generateKey(alias, option);
    expect(result.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });
});