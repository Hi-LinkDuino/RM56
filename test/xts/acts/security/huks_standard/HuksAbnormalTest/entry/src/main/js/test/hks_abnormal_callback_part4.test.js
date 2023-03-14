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

describe('Hks_Abnormal_Callback_Part4', function () {

  async function generateKey() {
    var option = signOrVerifyGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35300
   * @tc.name     InvalidAlgorithmAlgAesForSignCallback
   * @tc.desc     Invalid algorithm alg aes for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35300', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_AES);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35400
   * @tc.name     InvalidAlgorithmAlghmacForSignCallback
   * @tc.desc     Invalid algorithm alg hmac for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35400', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HMAC);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35500
   * @tc.name     InvalidAlgorithmAlgHkdfForSignCallback
   * @tc.desc     Invalid algorithm alg hkdf for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35500', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35600
   * @tc.name     InvalidAlgorithmAlgPbkdf2ForSignCallback
   * @tc.desc     Invalid algorithm alg pbkdf2 for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35600', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_PBKDF2);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35700
   * @tc.name     InvalidAlgorithmAlgEcdhForSignCallback
   * @tc.desc     Invalid algorithm alg ecdh for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35700', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35800
   * @tc.name     InvalidAlgorithmAlgX25519ForSignCallback
   * @tc.desc     Invalid algorithm alg x25519 for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35800', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_X25519);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35900
   * @tc.name     InvalidAlgorithmAlgDhForSignCallback
   * @tc.desc     Invalid algorithm alg dh for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35900', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36000
   * @tc.name     InvalidPurposeEncryptForSignCallback
   * @tc.desc     Invalid purpose encrypt for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36000', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36100
   * @tc.name     InvalidPurposeDecryptForSignCallback
   * @tc.desc     Invalid purpose decrypt for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36100', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36200
   * @tc.name     InvalidPurposeVerifyForSignCallback
   * @tc.desc     Invalid purpose verify for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36200', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36300
   * @tc.name     InvalidPurposeDeriveForSignCallback
   * @tc.desc     Invalid purpose derive for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36300', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36400
   * @tc.name     InvalidPurposeWrapForSignCallback
   * @tc.desc     Invalid purpose wrap for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36400', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36500
   * @tc.name     InvalidPurposeUnwrapForSignCallback
   * @tc.desc     Invalid purpose unwrap for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36500', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36600
   * @tc.name     InvalidPurposeMacForSignCallback
   * @tc.desc     Invalid purpose mac for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36600', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36700
   * @tc.name     InvalidPurposeAgreeForSignCallback
   * @tc.desc     Invalid purpose agree for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36700', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36800
   * @tc.name     InvalidPaddingNoneAlgRsaForSignCallback
   * @tc.desc     Invalid padding none alg rsa for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36800', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_NONE);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_36900
   * @tc.name     InvalidPaddingOaepAlgRsaForSignCallback
   * @tc.desc     Invalid padding oaep alg rsa for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_36900', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37000
   * @tc.name     InvalidPaddingPkcs5AlgRsaForSignCallback
   * @tc.desc     Invalid padding pkcs5 alg rsa for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37000', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37100
   * @tc.name     InvalidPaddingPkcs7AlgRsaForSignCallback
   * @tc.desc     Invalid padding pkcs7 alg rsa for sign callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37100', 0, async function (done) {
    await generateKey();
    var signOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    hks.sign(alias, signOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37200
   * @tc.name     InvalidAlgorithmAlgAesForVerifyCallback
   * @tc.desc     Invalid algorithm alg aes for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37200', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_AES);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37300
   * @tc.name     InvalidAlgorithmAlgHmacForVerifyCallback
   * @tc.desc     Invalid algorithm alg hmac for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37300', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HMAC);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37400
   * @tc.name     InvalidAlgorithmAlgHkdfForVerifyCallback
   * @tc.desc     Invalid algorithm alg hkdf for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37400', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37500
   * @tc.name     InvalidAlgorithmAlgPbkdf2ForVerifyCallback
   * @tc.desc     Invalid algorithm alg pbkdf2 for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37500', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_PBKDF2);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37600
   * @tc.name     InvalidAlgorithmAlgEcdhForVerifyCallback
   * @tc.desc     Invalid algorithm alg ecdh for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37600', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37700
   * @tc.name     InvalidAlgorithmAlgX25519ForVerifyCallback
   * @tc.desc     Invalid algorithm alg x25519 for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37700', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_X25519);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37800
   * @tc.name     InvalidAlgorithmAlgDhForVerifyCallback
   * @tc.desc     Invalid algorithm alg dh for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37800', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_37900
   * @tc.name     InvalidAlgorithmEncryptForVerifyCallback
   * @tc.desc     Invalid purpose encrypt for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_37900', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38000
   * @tc.name     InvalidAlgorithmDecryptForVerifyCallback
   * @tc.desc     Invalid purpose decrypt for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38000', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38100
   * @tc.name     InvalidAlgorithmSignForVerifyCallback
   * @tc.desc     Invalid purpose sign for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38100', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38200
   * @tc.name     InvalidAlgorithmDeriveForVerifyCallback
   * @tc.desc     Invalid purpose derive for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38200', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38300
   * @tc.name     InvalidAlgorithmWrapForVerifyCallback
   * @tc.desc     Invalid purpose wrap for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38300', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38400
   * @tc.name     InvalidAlgorithmUnwrapForVerifyCallback
   * @tc.desc     Invalid purpose unwrap for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38400', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38500
   * @tc.name     InvalidAlgorithmMacForVerifyCallback
   * @tc.desc     Invalid purpose mac for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38500', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38600
   * @tc.name     InvalidAlgorithmAgreeForVerifyCallback
   * @tc.desc     Invalid purpose agree for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38600', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38700
   * @tc.name     InvalidPaddingNoneAlgRsaForVerifyCallback
   * @tc.desc     Invalid padding none alg rsa for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38700', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_NONE);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38800
   * @tc.name     InvalidPaddingOaepAlgRsaForVerifyCallback
   * @tc.desc     Invalid padding oaep alg rsa for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38800', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_OAEP);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_38900
   * @tc.name     InvalidPaddingPksc5AlgRsaForVerifyCallback
   * @tc.desc     Invalid padding pksc5 alg rsa for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_38900', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39000
   * @tc.name     InvalidPaddingPkcs7AlgRsaForVerifyCallback
   * @tc.desc     Invalid padding pkcs7 alg rsa for verify callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39000', 0, async function (done) {
    await generateKey();
    var verifyOption = signOrVerifyInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    hks.verify(alias, verifyOption, plainTextSize64, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });
});