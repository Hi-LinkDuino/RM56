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
  generateRSAKeyOption,
  generateAesCBCKeyOption,
  generateAesGCMKeyOption,
  encryptOrDecryptGenerateKeyOption,
  encryptOrDecryptOption,
  encryptOrDecryptInvalidAlgOption,
  rsaEncryptInvalidSizeOption,
  aesEncryptOrDecryptInvalidSizeOption,
  aesEncryptOrDecryptInvalidPaddingOption,
  aesEncryptOrDecryptInvalidDigestOption,
  rsaEncryptOrDecryptInvalidIndataOption,
  aesEncryptOrDecryptInvalidIndataOption,
  aesEncryptOrDecryptInvalidPurposeOption,
  rsaDecryptInvalidSizeOption,
} from './hks_abnormal_common.test.js'

describe('Hks_Abnormal_Promise_Part3', function () {

  var cipherText = [];

  /**
 * @tc.number   HUKS_ABNORMAL_PROMISE_25500
 * @tc.name     InvalidAlgorithmAlgEccForEncryptPromise
 * @tc.desc     Invalid algorithm alg ecc for encrypt promise.
 */
  it('HUKS_ABNORMAL_PROMISE_25500', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECC);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25600
   * @tc.name     InvalidAlgorithmAlgDsaForEncryptPromise
   * @tc.desc     Invalid algorithm alg dsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_25600', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DSA);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25700
   * @tc.name     InvalidAlgorithmAlgHmacForEncryptPromise
   * @tc.desc     Invalid algorithm alg hmac for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_25700', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HMAC);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25800
   * @tc.name     InvalidAlgorithmAlgHkdfForEncryptPromise
   * @tc.desc     Invalid algorithm alg hkdf for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_25800', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25900
   * @tc.name     InvalidAlgorithmAlgPbkdf2ForEncryptPromise
   * @tc.desc     Invalid algorithm alg pbkdf2 for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_25900', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_PBKDF2);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26000
   * @tc.name     InvalidAlgorithmAlgEcdhForEncryptPromise
   * @tc.desc     Invalid algorithm alg ecdh for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_26000', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26100
   * @tc.name     InvalidAlgorithmAlgX25519ForEncryptPromise
   * @tc.desc     Invalid algorithm alg x25519 for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_26100', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_X25519);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26200
   * @tc.name     InvalidAlgorithmAlgEd25519ForEncryptPromise
   * @tc.desc     Invalid algorithm alg ed25519 for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_26200', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ED25519);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26300
   * @tc.name     InvalidAlgorithmAlgDhForEncryptPromise
   * @tc.desc     Invalid algorithm alg dh for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_26300', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26400
   * @tc.name     SuccessEcc224AlgRsaForEncryptPromise
   * @tc.desc     Success ecc 224 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_26400', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26500
   * @tc.name     SuccessEcc256AlgRsaForEncryptPromise
   * @tc.desc     Success ecc 256 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_26500', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26600
   * @tc.name     SuccessEcc384AlgRsaForEncryptPromise
   * @tc.desc     Success ecc 384 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_26600', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26700
   * @tc.name     SuccessEcc521AlgRsaForEncryptPromise
   * @tc.desc     Success ecc 521 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_26700', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
 * @tc.number   HUKS_ABNORMAL_PROMISE_26800
 * @tc.name     SuccessAes128AlgRsaForEncryptPromise
 * @tc.desc     Success aes 128 alg rsa for encrypt promise.
 */
  it('HUKS_ABNORMAL_PROMISE_26800', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26900
   * @tc.name     SuccessAes192AlgRsaForEncryptPromise
   * @tc.desc     Success aes 192 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_26900', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_192);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27000
   * @tc.name     SuccessAes256AlgRsaForEncryptPromise
   * @tc.desc     Success aes 256 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27000', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_256);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27100
   * @tc.name     SuccessAes512AlgRsaForEncryptPromise
   * @tc.desc     Success aes 512 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27100', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27200
   * @tc.name     SuccessCurve25519256AlgRsaForEncryptPromise
   * @tc.desc     Success curve25519 256 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27200', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27300
   * @tc.name     SuccessDh2048AlgRsaForEncryptPromise
   * @tc.desc     Success dh 2048 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27300', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27400
   * @tc.name     SuccessDh3072AlgRsaForEncryptPromise
   * @tc.desc     Success dh 3072 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27400', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27500
   * @tc.name     SuccessDh4096AlgRsaForEncryptPromise
   * @tc.desc     Success dh 4096 alg rsa for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27500', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27600
   * @tc.name     SuccessRsa512AlgAesForEncryptPromise
   * @tc.desc     Success rsa 512 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27600', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27700
   * @tc.name     SuccessRsa768AlgAesForEncryptPromise
   * @tc.desc     Success rsa 768 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27700', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_768);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27800
   * @tc.name     SuccessRsa1024AlgAesForEncryptPromise
   * @tc.desc     Success rsa 1024 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27800', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27900
   * @tc.name     SuccessRsa2048AlgAesForEncryptPromise
   * @tc.desc     Success rsa 2048 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_27900', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28000
   * @tc.name     SuccessRsa3072AlgAesForEncryptPromise
   * @tc.desc     Success rsa 3072 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28000', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28100
   * @tc.name     SuccessRsa4096AlgAesForEncryptPromise
   * @tc.desc     Success rsa 4096 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28100', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28200
   * @tc.name     SuccessEcc224AlgAesForEncryptPromise
   * @tc.desc     Success ecc 224 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28200', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28300
   * @tc.name     SuccessEcc256AlgAesForEncryptPromise
   * @tc.desc     Success ecc 256 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28300', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28400
   * @tc.name     SuccessEcc384AlgAesForEncryptPromise
   * @tc.desc     Success ecc 384 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28400', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28500
   * @tc.name     SuccessEcc521AlgAesForEncryptPromise
   * @tc.desc     Success ecc 521 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28500', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28600
   * @tc.name     SuccessCurve25519256AlgAesForEncryptPromise
   * @tc.desc     Success curve25519 256 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28600', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28700
   * @tc.name     SuccessDh2048AlgAesForEncryptPromise
   * @tc.desc     Success dh 2048 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28700', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28800
   * @tc.name     SuccessDh3072AlgAesForEncryptPromise
   * @tc.desc     Success dh 3072 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28800', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28900
   * @tc.name     SuccessDh4096AlgAesForEncryptPromise
   * @tc.desc     Success dh 4096 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_28900', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29000
   * @tc.name     InvalidPaddingPssPurposeEncryptDecryptAlgAesForEncryptPromise
   * @tc.desc     Invalid padding pss purpose encrypt decrypt alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29000', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PSS);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29100
   * @tc.name     InvalidPaddingPkcs5PurposeEncryptDecryptAlgAesForEncryptPromise
   * @tc.desc     Invalid padding pkcs5 purpose encrypt decrypt alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29100', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29200
   * @tc.name     InvalidPaddingPkcs7PurposeEncryptDecryptAlgAesForEncryptPromise
   * @tc.desc     Invalid padding pkcs7 purpose encrypt decrypt alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29200', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29300
   * @tc.name     SuccessDigestMd5AlgAesForEncryptPromise
   * @tc.desc     Success digest md5 alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29300', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidDigestOption(hks.HuksKeyDigest.HUKS_DIGEST_MD5);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29400
   * @tc.name     InvalidArgumentIndataLargerThanKeySizeRsaPaddingNoneForEncryptPromise
   * @tc.desc     Invalid argument indata is larger than key size rsa padding none for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29400', 0, async function (done) {
    var option = generateRSAKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = rsaEncryptOrDecryptInvalidIndataOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29500
   * @tc.name     InvalidArgumentIndataLargerThanKeySizeAesPaddingNoneForEncryptPromise
   * @tc.desc     Invalid argument indata is larger than key size aes padding none for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29500', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidIndataOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29600
   * @tc.name     InvalidPurposeDecryptAlgAesForEncryptPromise
   * @tc.desc     Invalid purpose decrypt alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29600', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29700
   * @tc.name     InvalidPurposeSignAlgAesForEncryptPromise
   * @tc.desc     Invalid purpose sign alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29700', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29800
   * @tc.name     InvalidPurposeVerifyAlgAesForEncryptPromise
   * @tc.desc     Invalid purpose verify alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29800', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29900
   * @tc.name     InvalidPurposeDeriveAlgAesForEncryptPromise
   * @tc.desc     Invalid purpose derive alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_29900', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30000
   * @tc.name     InvalidPurposeWrapAlgAesForEncryptPromise
   * @tc.desc     Invalid purpose wrap alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30000', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30100
   * @tc.name     InvalidPurposeUnwrapAlgAesForEncryptPromise
   * @tc.desc     Invalid purpose unwrap alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30100', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30200
   * @tc.name     InvalidPurposeMacAlgAesForEncryptPromise
   * @tc.desc     Invalid purpose mac alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30200', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30300
   * @tc.name     InvalidPurposeagreeAlgAesForEncryptPromise
   * @tc.desc     Invalid purpose agree alg aes for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30300', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30400
   * @tc.name     InvalidAlgorithmEccForDecryptPromise
   * @tc.desc     Invalid algorithm ecc for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30400', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECC);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30500
   * @tc.name     InvalidAlgorithmDsaForDecryptPromise
   * @tc.desc     Invalid algorithm dsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30500', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DSA);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30600
   * @tc.name     InvalidAlgorithmHmacForDecryptPromise
   * @tc.desc     Invalid algorithm hmac for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30600', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HMAC);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30700
   * @tc.name     InvalidAlgorithmHkdfForDecryptPromise
   * @tc.desc     Invalid algorithm hkdf for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30700', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30800
   * @tc.name     InvalidAlgorithmPbkdf2ForDecryptPromise
   * @tc.desc     Invalid algorithm pbkdf2 for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30800', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_PBKDF2);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30900
   * @tc.name     InvalidAlgorithmEcdhForDecryptPromise
   * @tc.desc     Invalid algorithm ecdh for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_30900', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31000
   * @tc.name     InvalidAlgorithmX25519ForDecryptPromise
   * @tc.desc     Invalid algorithm x25519 for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31000', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_X25519);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31100
   * @tc.name     InvalidAlgorithmEd25519ForDecryptPromise
   * @tc.desc     Invalid algorithm ed25519 for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31100', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ED25519);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31200
   * @tc.name     InvalidAlgorithmDhForDecryptPromise
   * @tc.desc     Invalid algorithm dh for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31200', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31300
   * @tc.name     SuccessEcc224AlgRsaForDecryptPromise
   * @tc.desc     Success ecc 224 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31300', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31400
   * @tc.name     SuccessEcc256AlgRsaForDecryptPromise
   * @tc.desc     Success ecc 256 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31400', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31500
   * @tc.name     SuccessEcc384AlgRsaForDecryptPromise
   * @tc.desc     Success ecc 384 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31500', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31600
   * @tc.name     SuccessEcc521AlgRsaForDecryptPromise
   * @tc.desc     Success ecc 521 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31600', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31700
   * @tc.name     SuccessAes128AlgRsaForDecryptPromise
   * @tc.desc     Success aes 128 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31700', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_128, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31800
   * @tc.name     SuccessAes192AlgRsaForDecryptPromise
   * @tc.desc     Success aes 192 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31800', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_192, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31900
   * @tc.name     SuccessAes256AlgRsaForDecryptPromise
   * @tc.desc     Success aes 256 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_31900', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_256, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32000
   * @tc.name     SuccessAes512AlgRsaForDecryptPromise
   * @tc.desc     Success aes 512 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32000', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32100
   * @tc.name     SuccessCurve25519256AlgRsaForDecryptPromise
   * @tc.desc     Success curve25519 256 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32100', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32200
   * @tc.name     SuccessDh2048AlgRsaForDecryptPromise
   * @tc.desc     Success dh 2048 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32200', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32300
   * @tc.name     SuccessDh3072AlgRsaForDecryptPromise
   * @tc.desc     Success dh 3072 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32300', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32400
   * @tc.name     SuccessDh4096AlgRsaForDecryptPromise
   * @tc.desc     Success dh 4096 alg rsa for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32400', 0, async function (done) {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096, cipherText);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32500
   * @tc.name     SuccessRsa512AlgAesForDecryptPromise
   * @tc.desc     Success rsa 512 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32500', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32600
   * @tc.name     SuccessRsa768AlgAesForDecryptPromise
   * @tc.desc     Success rsa 768 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32600', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_768);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32700
   * @tc.name     SuccessRsa1024AlgAesForDecryptPromise
   * @tc.desc     Success rsa 1024 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32700', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32800
   * @tc.name     SuccessRsa2048AlgAesForDecryptPromise
   * @tc.desc     Success rsa 2048 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32800', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32900
   * @tc.name     SuccessRsa3072AlgAesForDecryptPromise
   * @tc.desc     Success rsa 3072 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_32900', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33000
   * @tc.name     SuccessRsa4096AlgAesForDecryptPromise
   * @tc.desc     Success rsa 4096 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33000', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33100
   * @tc.name     SuccessEcc224AlgAesForDecryptPromise
   * @tc.desc     Success ecc 224 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33100', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33200
   * @tc.name     SuccessEcc256AlgAesForDecryptPromise
   * @tc.desc     Success ecc 256 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33200', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33300
   * @tc.name     SuccessEcc384AlgAesForDecryptPromise
   * @tc.desc     Success ecc 384 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33300', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33400
   * @tc.name     SuccessEcc521AlgAesForDecryptPromise
   * @tc.desc     Success ecc 521 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33400', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33500
   * @tc.name     Successcurve25519256AlgAesForDecryptPromise
   * @tc.desc     Success curve25519 256 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33500', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33600
   * @tc.name     SuccessDh2048AlgAesForDecryptPromise
   * @tc.desc     Success dh 2048 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33600', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33700
   * @tc.name     SuccessDh3072AlgAesForDecryptPromise
   * @tc.desc     Success dh 3072 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33700', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33800
   * @tc.name     SuccessDh4096AlgAesForDecryptPromise
   * @tc.desc     Success dh 4096 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33800', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33900
   * @tc.name     InvalidPaddingPssPurposeEncryptDecryptAlgAesForDecryptPromise
   * @tc.desc     Invalid padding pss purpose encrypt decrypt alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_33900', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PSS);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34000
   * @tc.name     InvalidPaddingPkcs5PurposeEncryptDecryptAlgAesForDecryptPromise
   * @tc.desc     Invalid padding pkcs5 purpose encrypt decrypt alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34000', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34100
   * @tc.name     InvalidPaddingPkcs7PurposeEncryptDecryptAlgAesForDecryptPromise
   * @tc.desc     Invalid padding pkcs7 purpose encrypt decrypt alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34100', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34200
   * @tc.name     SuccessDigestMd5AlgAesForDecryptPromise
   * @tc.desc     Success digest md5 alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34200', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidDigestOption(hks.HuksKeyDigest.HUKS_DIGEST_MD5);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34300
   * @tc.name     InvalidArgumentIndataLargerThanKeySizeRsaPaddingNoneForDecryptPromise
   * @tc.desc     Invalid argument indata is larger than key size rsa padding none for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34300', 0, async function (done) {
    var option = generateRSAKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = rsaEncryptOrDecryptInvalidIndataOption();
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34400
   * @tc.name     InvalidArgumentIndataLargerThanKeySizeAesPaddingNoneForDecryptPromise
   * @tc.desc     Invalid argument indata is larger than key size aes padding none for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34400', 0, async function (done) {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidIndataOption();
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34500
   * @tc.name     InvalidPurposeEncryptAlgAesForDecryptPromise
   * @tc.desc     Invalid purpose encrypt alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34500', 0, async function (done) {
    var option = generateAesGCMKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34600
   * @tc.name     InvalidPurposeSignAlgAesForDecryptPromise
   * @tc.desc     Invalid purpose sign alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34600', 0, async function (done) {
    var option = generateAesGCMKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34700
   * @tc.name     InvalidPurposeVerifyAlgAesForDecryptPromise
   * @tc.desc     Invalid purpose verify alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34700', 0, async function (done) {
    var option = generateAesGCMKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34800
   * @tc.name     InvalidPurposeDeriveAlgAesForDecryptPromise
   * @tc.desc     Invalid purpose derive alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34800', 0, async function (done) {
    var option = generateAesGCMKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34900
   * @tc.name     InvalidPurposeWrapAlgAesForDecryptPromise
   * @tc.desc     Invalid purpose wrap alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_34900', 0, async function (done) {
    var option = generateAesGCMKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35000
   * @tc.name     InvalidPurposeUnwrapAlgAesForDecryptPromise
   * @tc.desc     Invalid purpose unwrap alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_35000', 0, async function (done) {
    var option = generateAesGCMKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35100
   * @tc.name     InvalidPurposeMacAlgAesForDecryptPromise
   * @tc.desc     Invalid purpose mac alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_35100', 0, async function (done) {
    var option = generateAesGCMKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35200
   * @tc.name     InvalidPurposeAgreeAlgAesForDecryptPromise
   * @tc.desc     Invalid purpose agree alg aes for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_35200', 0, async function (done) {
    var option = generateAesGCMKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    var decryptRet = await hks.decrypt(alias, decryptOption);
    expect(decryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });
});

