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
  plainTextSize64,
  signOrVerifyGenerateKeyOption,
  signOrVerifyInvalidAlgOption,
  signOrVerifyInvalidPurposeOption,
  signOrVerifyInvalidPaddingOption,
} from './hks_abnormal_common.test.js'

describe('Hks_Abnormal_Promise_Part4', function () {

  /**
 * @tc.number   HUKS_ABNORMAL_PROMISE_35300
 * @tc.name     InvalidAlgorithmAlgAesForSignPromise
 * @tc.desc     Invalid algorithm alg aes for sign promise.
 */
  it('HUKS_ABNORMAL_PROMISE_35300', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_AES);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_35400
   * @tc.name     InvalidAlgorithmAlghmacForSignPromise
   * @tc.desc     Invalid algorithm alg hmac for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_35400', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HMAC);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_35500
   * @tc.name     InvalidAlgorithmAlgHkdfForSignPromise
   * @tc.desc     Invalid algorithm alg hkdf for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_35500', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_35600
   * @tc.name     InvalidAlgorithmAlgPbkdf2ForSignPromise
   * @tc.desc     Invalid algorithm alg pbkdf2 for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_35600', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_PBKDF2);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_35700
   * @tc.name     InvalidAlgorithmAlgEcdhForSignPromise
   * @tc.desc     Invalid algorithm alg ecdh for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_35700', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_35800
   * @tc.name     InvalidAlgorithmAlgX25519ForSignPromise
   * @tc.desc     Invalid algorithm alg x25519 for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_35800', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_X25519);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_35900
   * @tc.name     InvalidAlgorithmAlgDhForSignPromise
   * @tc.desc     Invalid algorithm alg dh for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_35900', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36000
   * @tc.name     InvalidPurposeEncryptForSignPromise
   * @tc.desc     Invalid purpose encrypt for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36000', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36100
   * @tc.name     InvalidPurposeDecryptForSignPromise
   * @tc.desc     Invalid purpose decrypt for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36100', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36200
   * @tc.name     InvalidPurposeVerifyForSignPromise
   * @tc.desc     Invalid purpose verify for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36200', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36300
   * @tc.name     InvalidPurposeDeriveForSignPromise
   * @tc.desc     Invalid purpose derive for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36300', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36400
   * @tc.name     InvalidPurposeWrapForSignPromise
   * @tc.desc     Invalid purpose wrap for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36400', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36500
   * @tc.name     InvalidPurposeUnwrapForSignPromise
   * @tc.desc     Invalid purpose unwrap for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36500', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36600
   * @tc.name     InvalidPurposeMacForSignPromise
   * @tc.desc     Invalid purpose mac for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36600', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36700
   * @tc.name     InvalidPurposeAgreeForSignPromise
   * @tc.desc     Invalid purpose agree for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36700', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36800
   * @tc.name     InvalidPaddingNoneAlgRsaForSignPromise
   * @tc.desc     Invalid padding none alg rsa for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36800', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_NONE);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_36900
   * @tc.name     InvalidPaddingOaepAlgRsaForSignPromise
   * @tc.desc     Invalid padding oaep alg rsa for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_36900', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_37000
   * @tc.name     InvalidPaddingPkcs5AlgRsaForSignPromise
   * @tc.desc     Invalid padding pkcs5 alg rsa for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_37000', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_37100
   * @tc.name     InvalidPaddingPkcs7AlgRsaForSignPromise
   * @tc.desc     Invalid padding pkcs7 alg rsa for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_37100', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var signOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    var signRet = await hks.sign(alias, signOption);
    expect(signRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
 * @tc.number   HUKS_ABNORMAL_PROMISE_37200
 * @tc.name     InvalidAlgorithmAlgAesForVerifyPromise
 * @tc.desc     Invalid algorithm alg aes for verify promise.
 */
  it('HUKS_ABNORMAL_PROMISE_37200', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_AES);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_37300
   * @tc.name     InvalidAlgorithmAlgHmacForVerifyPromise
   * @tc.desc     Invalid algorithm alg hmac for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_37300', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HMAC);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_37400
   * @tc.name     InvalidAlgorithmAlgHkdfForVerifyPromise
   * @tc.desc     Invalid algorithm alg hkdf for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_37400', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_37500
   * @tc.name     InvalidAlgorithmAlgPbkdf2ForVerifyPromise
   * @tc.desc     Invalid algorithm alg pbkdf2 for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_37500', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_PBKDF2);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_37600
   * @tc.name     InvalidAlgorithmAlgEcdhForVerifyPromise
   * @tc.desc     Invalid algorithm alg ecdh for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_37600', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_37700
   * @tc.name     InvalidAlgorithmAlgX25519ForVerifyPromise
   * @tc.desc     Invalid algorithm alg x25519 for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_37700', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_X25519);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_37800
   * @tc.name     InvalidAlgorithmAlgDhForVerifyPromise
   * @tc.desc     Invalid algorithm alg dh for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_37800', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_37900
   * @tc.name     InvalidAlgorithmEncryptForVerifyPromise
   * @tc.desc     Invalid purpose encrypt for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_37900', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38000
   * @tc.name     InvalidAlgorithmDecryptForVerifyPromise
   * @tc.desc     Invalid purpose decrypt for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38000', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38100
   * @tc.name     InvalidAlgorithmSignForVerifyPromise
   * @tc.desc     Invalid purpose sign for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38100', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38200
   * @tc.name     InvalidAlgorithmDeriveForVerifyPromise
   * @tc.desc     Invalid purpose derive for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38200', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38300
   * @tc.name     InvalidAlgorithmWrapForVerifyPromise
   * @tc.desc     Invalid purpose wrap for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38300', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38400
   * @tc.name     InvalidAlgorithmUnwrapForVerifyPromise
   * @tc.desc     Invalid purpose unwrap for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38400', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38500
   * @tc.name     InvalidAlgorithmMacForVerifyPromise
   * @tc.desc     Invalid purpose mac for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38500', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38600
   * @tc.name     InvalidAlgorithmAgreeForVerifyPromise
   * @tc.desc     Invalid purpose agree for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38600', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38700
   * @tc.name     InvalidPaddingNoneAlgRsaForVerifyPromise
   * @tc.desc     Invalid padding none alg rsa for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38700', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_NONE);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38800
   * @tc.name     InvalidPaddingOaepAlgRsaForVerifyPromise
   * @tc.desc     Invalid padding oaep alg rsa for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38800', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_38900
   * @tc.name     InvalidPaddingPksc5AlgRsaForVerifyPromise
   * @tc.desc     Invalid padding pksc5 alg rsa for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_38900', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_39000
   * @tc.name     InvalidPaddingPkcs7AlgRsaForVerifyPromise
   * @tc.desc     Invalid padding pkcs7 alg rsa for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_39000', 0, async function (done) {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var verifyOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    var verifyRet = await hks.verify(alias, verifyOption, plainTextSize64);
    expect(verifyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });
});