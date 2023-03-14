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

describe('Hks_Abnormal_Callback_Part3', function () {
  var cipherText = [];

  async function generateKey() {
    var option = encryptOrDecryptGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };

  async function generateRSAKey() {
    var option = generateRSAKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };

  async function generateAesCBCKey() {
    var option = generateAesCBCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };
  async function generateAesGCMKey() {
    var option = generateAesGCMKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25500
   * @tc.name     InvalidAlgorithmAlgEccForEncryptCallback
   * @tc.desc     Invalid algorithm alg ecc for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25500', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECC);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25600
   * @tc.name     InvalidAlgorithmAlgDsaForEncryptCallback
   * @tc.desc     Invalid algorithm alg dsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25600', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DSA);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25700
   * @tc.name     InvalidAlgorithmAlgHmacForEncryptCallback
   * @tc.desc     Invalid algorithm alg hmac for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25700', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HMAC);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25800
   * @tc.name     InvalidAlgorithmAlgHkdfForEncryptCallback
   * @tc.desc     Invalid algorithm alg hkdf for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25800', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_25900
   * @tc.name     InvalidAlgorithmAlgPbkdf2ForEncryptCallback
   * @tc.desc     Invalid algorithm alg pbkdf2 for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_25900', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_PBKDF2);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26000
   * @tc.name     InvalidAlgorithmAlgEcdhForEncryptCallback
   * @tc.desc     Invalid algorithm alg ecdh for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26000', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26100
   * @tc.name     InvalidAlgorithmAlgX25519ForEncryptCallback
   * @tc.desc     Invalid algorithm alg x25519 for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26100', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_X25519);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26200
   * @tc.name     InvalidAlgorithmAlgEd25519ForEncryptCallback
   * @tc.desc     Invalid algorithm alg ed25519 for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26200', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ED25519);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26300
   * @tc.name     InvalidAlgorithmAlgDhForEncryptCallback
   * @tc.desc     Invalid algorithm alg dh for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26300', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26400
   * @tc.name     SuccessEcc224AlgRsaForEncryptCallback
   * @tc.desc     Success ecc 224 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26400', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26500
   * @tc.name     SuccessEcc256AlgRsaForEncryptCallback
   * @tc.desc     Success ecc 256 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26500', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26600
   * @tc.name     SuccessEcc384AlgRsaForEncryptCallback
   * @tc.desc     Success ecc 384 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26600', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26700
   * @tc.name     SuccessEcc521AlgRsaForEncryptCallback
   * @tc.desc     Success ecc 521 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26700', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26800
   * @tc.name     SuccessAes128AlgRsaForEncryptCallback
   * @tc.desc     Success aes 128 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26800', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_26900
   * @tc.name     SuccessAes192AlgRsaForEncryptCallback
   * @tc.desc     Success aes 192 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_26900', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_192);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27000
   * @tc.name     SuccessAes256AlgRsaForEncryptCallback
   * @tc.desc     Success aes 256 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27000', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_256);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27100
   * @tc.name     SuccessAes512AlgRsaForEncryptCallback
   * @tc.desc     Success aes 512 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27100', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27200
   * @tc.name     SuccessCurve25519256AlgRsaForEncryptCallback
   * @tc.desc     Success curve25519 256 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27200', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27300
   * @tc.name     SuccessDh2048AlgRsaForEncryptCallback
   * @tc.desc     Success dh 2048 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27300', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27400
   * @tc.name     SuccessDh3072AlgRsaForEncryptCallback
   * @tc.desc     Success dh 3072 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27400', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27500
   * @tc.name     SuccessDh4096AlgRsaForEncryptCallback
   * @tc.desc     Success dh 4096 alg rsa for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27500', 0, async function (done) {
    await generateKey();
    var encryptOption = rsaEncryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27600
   * @tc.name     SuccessRsa512AlgAesForEncryptCallback
   * @tc.desc     Success rsa 512 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27600', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27700
   * @tc.name     SuccessRsa768AlgAesForEncryptCallback
   * @tc.desc     Success rsa 768 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27700', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_768);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27800
   * @tc.name     SuccessRsa1024AlgAesForEncryptCallback
   * @tc.desc     Success rsa 1024 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27800', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_27900
   * @tc.name     SuccessRsa2048AlgAesForEncryptCallback
   * @tc.desc     Success rsa 2048 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_27900', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28000
   * @tc.name     SuccessRsa3072AlgAesForEncryptCallback
   * @tc.desc     Success rsa 3072 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28000', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28100
   * @tc.name     SuccessRsa4096AlgAesForEncryptCallback
   * @tc.desc     Success rsa 4096 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28100', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28200
   * @tc.name     SuccessEcc224AlgAesForEncryptCallback
   * @tc.desc     Success ecc 224 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28200', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28300
   * @tc.name     SuccessEcc256AlgAesForEncryptCallback
   * @tc.desc     Success ecc 256 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28300', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28400
   * @tc.name     SuccessEcc384AlgAesForEncryptCallback
   * @tc.desc     Success ecc 384 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28400', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28500
   * @tc.name     SuccessEcc521AlgAesForEncryptCallback
   * @tc.desc     Success ecc 521 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28500', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28600
   * @tc.name     SuccessCurve25519256AlgAesForEncryptCallback
   * @tc.desc     Success curve25519 256 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28600', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28700
   * @tc.name     SuccessDh2048AlgAesForEncryptCallback
   * @tc.desc     Success dh 2048 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28700', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28800
   * @tc.name     SuccessDh3072AlgAesForEncryptCallback
   * @tc.desc     Success dh 3072 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28800', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_28900
   * @tc.name     SuccessDh4096AlgAesForEncryptCallback
   * @tc.desc     Success dh 4096 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_28900', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29000
   * @tc.name     InvalidPaddingPssPurposeEncryptDecryptAlgAesForEncryptCallback
   * @tc.desc     Invalid padding pss purpose encrypt decrypt alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29000', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PSS);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29100
   * @tc.name     InvalidPaddingPkcs5PurposeEncryptDecryptAlgAesForEncryptCallback
   * @tc.desc     Invalid padding pkcs5 purpose encrypt decrypt alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29100', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29200
   * @tc.name     InvalidPaddingPkcs7PurposeEncryptDecryptAlgAesForEncryptCallback
   * @tc.desc     Invalid padding pkcs7 purpose encrypt decrypt alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29200', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29300
   * @tc.name     SuccessDigestMd5AlgAesForEncryptCallback
   * @tc.desc     Success digest md5 alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29300', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidDigestOption(hks.HuksKeyDigest.HUKS_DIGEST_MD5);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29400
   * @tc.name     InvalidArgumentIndataLargerThanKeySizeRsaPaddingNoneForEncryptCallback
   * @tc.desc     Invalid argument indata is larger than key size rsa padding none for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29400', 0, async function (done) {
    await generateRSAKey();
    var encryptOption = rsaEncryptOrDecryptInvalidIndataOption();
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29500
   * @tc.name     InvalidArgumentIndataLargerThanKeySizeAesPaddingNoneForEncryptCallback
   * @tc.desc     Invalid argument indata is larger than key size aes padding none for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29500', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidIndataOption();
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29600
   * @tc.name     InvalidPurposeDecryptAlgAesForEncryptCallback
   * @tc.desc     Invalid purpose decrypt alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29600', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29700
   * @tc.name     InvalidPurposeSignAlgAesForEncryptCallback
   * @tc.desc     Invalid purpose sign alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29700', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29800
   * @tc.name     InvalidPurposeVerifyAlgAesForEncryptCallback
   * @tc.desc     Invalid purpose verify alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29800', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_29900
   * @tc.name     InvalidPurposeDeriveAlgAesForEncryptCallback
   * @tc.desc     Invalid purpose derive alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_29900', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30000
   * @tc.name     InvalidPurposeWrapAlgAesForEncryptCallback
   * @tc.desc     Invalid purpose wrap alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30000', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30100
   * @tc.name     InvalidPurposeUnwrapAlgAesForEncryptCallback
   * @tc.desc     Invalid purpose unwrap alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30100', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30200
   * @tc.name     InvalidPurposeMacAlgAesForEncryptCallback
   * @tc.desc     Invalid purpose mac alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30200', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30300
   * @tc.name     InvalidPurposeagreeAlgAesForEncryptCallback
   * @tc.desc     Invalid purpose agree alg aes for encrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30300', 0, async function (done) {
    await generateAesCBCKey();
    var encryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    hks.encrypt(alias, encryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30400
   * @tc.name     InvalidAlgorithmEccForDecryptCallback
   * @tc.desc     Invalid algorithm ecc for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30400', 0, async function (done) {
    await generateKey();
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECC);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30500
   * @tc.name     InvalidAlgorithmDsaForDecryptCallback
   * @tc.desc     Invalid algorithm dsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30500', 0, async function (done) {
    await generateKey();
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DSA);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30600
   * @tc.name     InvalidAlgorithmHmacForDecryptCallback
   * @tc.desc     Invalid algorithm hmac for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30600', 0, async function (done) {
    await generateKey();
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HMAC);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30700
   * @tc.name     InvalidAlgorithmHkdfForDecryptCallback
   * @tc.desc     Invalid algorithm hkdf for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30700', 0, async function (done) {
    await generateKey();
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30800
   * @tc.name     InvalidAlgorithmPbkdf2ForDecryptCallback
   * @tc.desc     Invalid algorithm pbkdf2 for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30800', 0, async function (done) {
    await generateKey();
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_PBKDF2);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_30900
   * @tc.name     InvalidAlgorithmEcdhForDecryptCallback
   * @tc.desc     Invalid algorithm ecdh for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_30900', 0, async function (done) {
    await generateKey();
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECDH);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31000
   * @tc.name     InvalidAlgorithmX25519ForDecryptCallback
   * @tc.desc     Invalid algorithm x25519 for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31000', 0, async function (done) {
    await generateKey();
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_X25519);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31100
   * @tc.name     InvalidAlgorithmEd25519ForDecryptCallback
   * @tc.desc     Invalid algorithm ed25519 for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31100', 0, async function (done) {
    await generateKey();
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ED25519);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31200
   * @tc.name     InvalidAlgorithmDhForDecryptCallback
   * @tc.desc     Invalid algorithm dh for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31200', 0, async function (done) {
    await generateKey();
    var decryptOption = encryptOrDecryptInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DH);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31300
   * @tc.name     SuccessEcc224AlgRsaForDecryptCallback
   * @tc.desc     Success ecc 224 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31300', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31400
   * @tc.name     SuccessEcc256AlgRsaForDecryptCallback
   * @tc.desc     Success ecc 256 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31400', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31500
   * @tc.name     SuccessEcc384AlgRsaForDecryptCallback
   * @tc.desc     Success ecc 384 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31500', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31600
   * @tc.name     SuccessEcc521AlgRsaForDecryptCallback
   * @tc.desc     Success ecc 521 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31600', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31700
   * @tc.name     SuccessAes128AlgRsaForDecryptCallback
   * @tc.desc     Success aes 128 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31700', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_128, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31800
   * @tc.name     SuccessAes192AlgRsaForDecryptCallback
   * @tc.desc     Success aes 192 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31800', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_192, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_31900
   * @tc.name     SuccessAes256AlgRsaForDecryptCallback
   * @tc.desc     Success aes 256 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_31900', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_256, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32000
   * @tc.name     SuccessAes512AlgRsaForDecryptCallback
   * @tc.desc     Success aes 512 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32000', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_AES_KEY_SIZE_512, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32100
   * @tc.name     SuccessCurve25519256AlgRsaForDecryptCallback
   * @tc.desc     Success curve25519 256 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32100', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32200
   * @tc.name     SuccessDh2048AlgRsaForDecryptCallback
   * @tc.desc     Success dh 2048 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32200', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32300
   * @tc.name     SuccessDh3072AlgRsaForDecryptCallback
   * @tc.desc     Success dh 3072 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32300', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32400
   * @tc.name     SuccessDh4096AlgRsaForDecryptCallback
   * @tc.desc     Success dh 4096 alg rsa for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32400', 0, async function (done) {
    await generateKey();
    var encryptOption = encryptOrDecryptOption();
    var encryptRet = await hks.encrypt(alias, encryptOption);
    expect(encryptRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    cipherText = encryptRet.outData;
    var decryptOption = rsaDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096, cipherText);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32500
   * @tc.name     SuccessRsa512AlgAesForDecryptCallback
   * @tc.desc     Success rsa 512 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32500', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32600
   * @tc.name     SuccessRsa768AlgAesForDecryptCallback
   * @tc.desc     Success rsa 768 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32600', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_768);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32700
   * @tc.name     SuccessRsa1024AlgAesForDecryptCallback
   * @tc.desc     Success rsa 1024 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32700', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32800
   * @tc.name     SuccessRsa2048AlgAesForDecryptCallback
   * @tc.desc     Success rsa 2048 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32800', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_32900
   * @tc.name     SuccessRsa3072AlgAesForDecryptCallback
   * @tc.desc     Success rsa 3072 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_32900', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33000
   * @tc.name     SuccessRsa4096AlgAesForDecryptCallback
   * @tc.desc     Success rsa 4096 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33000', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33100
   * @tc.name     SuccessEcc224AlgAesForDecryptCallback
   * @tc.desc     Success ecc 224 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33100', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33200
   * @tc.name     SuccessEcc256AlgAesForDecryptCallback
   * @tc.desc     Success ecc 256 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33200', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33300
   * @tc.name     SuccessEcc384AlgAesForDecryptCallback
   * @tc.desc     Success ecc 384 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33300', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33400
   * @tc.name     SuccessEcc521AlgAesForDecryptCallback
   * @tc.desc     Success ecc 521 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33400', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33500
   * @tc.name     Successcurve25519256AlgAesForDecryptCallback
   * @tc.desc     Success curve25519 256 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33500', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33600
   * @tc.name     SuccessDh2048AlgAesForDecryptCallback
   * @tc.desc     Success dh 2048 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33600', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33700
   * @tc.name     SuccessDh3072AlgAesForDecryptCallback
   * @tc.desc     Success dh 3072 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33700', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_3072);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33800
   * @tc.name     SuccessDh4096AlgAesForDecryptCallback
   * @tc.desc     Success dh 4096 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33800', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidSizeOption(hks.HuksKeySize.HUKS_DH_KEY_SIZE_4096);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_33900
   * @tc.name     InvalidPaddingPssPurposeEncryptDecryptAlgAesForDecryptCallback
   * @tc.desc     Invalid padding pss purpose encrypt decrypt alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_33900', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PSS);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34000
   * @tc.name     InvalidPaddingPkcs5PurposeEncryptDecryptAlgAesForDecryptCallback
   * @tc.desc     Invalid padding pkcs5 purpose encrypt decrypt alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34000', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS5);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34100
   * @tc.name     InvalidPaddingPkcs7PurposeEncryptDecryptAlgAesForDecryptCallback
   * @tc.desc     Invalid padding pkcs7 purpose encrypt decrypt alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34100', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidPaddingOption(hks.HuksKeyPadding.HUKS_PADDING_PKCS7);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34200
   * @tc.name     SuccessDigestMd5AlgAesForDecryptCallback
   * @tc.desc     Success digest md5 alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34200', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidDigestOption(hks.HuksKeyDigest.HUKS_DIGEST_MD5);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34300
   * @tc.name     InvalidArgumentIndataLargerThanKeySizeRsaPaddingNoneForDecryptCallback
   * @tc.desc     Invalid argument indata is larger than key size rsa padding none for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34300', 0, async function (done) {
    await generateRSAKey();
    var decryptOption = rsaEncryptOrDecryptInvalidIndataOption();
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34400
   * @tc.name     InvalidArgumentIndataLargerThanKeySizeAesPaddingNoneForDecryptCallback
   * @tc.desc     Invalid argument indata is larger than key size aes padding none for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34400', 0, async function (done) {
    await generateAesCBCKey();
    var decryptOption = aesEncryptOrDecryptInvalidIndataOption();
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34500
   * @tc.name     InvalidPurposeEncryptAlgAesForDecryptCallback
   * @tc.desc     Invalid purpose encrypt alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34500', 0, async function (done) {
    await generateAesGCMKey();
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34600
   * @tc.name     InvalidPurposeSignAlgAesForDecryptCallback
   * @tc.desc     Invalid purpose sign alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34600', 0, async function (done) {
    await generateAesGCMKey();
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34700
   * @tc.name     InvalidPurposeVerifyAlgAesForDecryptCallback
   * @tc.desc     Invalid purpose verify alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34700', 0, async function (done) {
    await generateAesGCMKey();
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34800
   * @tc.name     InvalidPurposeDeriveAlgAesForDecryptCallback
   * @tc.desc     Invalid purpose derive alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34800', 0, async function (done) {
    await generateAesGCMKey();
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_34900
   * @tc.name     InvalidPurposeWrapAlgAesForDecryptCallback
   * @tc.desc     Invalid purpose wrap alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_34900', 0, async function (done) {
    await generateAesGCMKey();
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35000
   * @tc.name     InvalidPurposeUnwrapAlgAesForDecryptCallback
   * @tc.desc     Invalid purpose unwrap alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35000', 0, async function (done) {
    await generateAesGCMKey();
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35100
   * @tc.name     InvalidPurposeMacAlgAesForDecryptCallback
   * @tc.desc     Invalid purpose mac alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35100', 0, async function (done) {
    await generateAesGCMKey();
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_35200
   * @tc.name     InvalidPurposeAgreeAlgAesForDecryptCallback
   * @tc.desc     Invalid purpose agree alg aes for decrypt callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_35200', 0, async function (done) {
    await generateAesGCMKey();
    var decryptOption = aesEncryptOrDecryptInvalidPurposeOption(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE);
    hks.decrypt(alias, decryptOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });
});