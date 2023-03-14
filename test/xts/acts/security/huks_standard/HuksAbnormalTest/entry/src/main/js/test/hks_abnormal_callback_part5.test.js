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
  generateAgreeKeyOption,
  hmacGenerateKeyOption,
  agreeInvalidAlgOption,
  hmacInvalidPurposeOption,
} from './hks_abnormal_common.test.js'

describe('Hks_Abnormal_Callback_Part5', function () {
  async function hmacGenerateKey() {
    var option = hmacGenerateKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39100
   * @tc.name     InvalidAlgorithmAlgRsaForAgreeKeyCallback
   * @tc.desc     Invalid algorithm alg rsa for agree key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39100', 0, async function (done) {
    var option = generateAgreeKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(alias, option);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var agreeOption = agreeInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_RSA);
    agreeOption.inData = exportKeyRet.outData
    hks.agreeKey(alias, agreeOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39200
   * @tc.name     InvalidAlgorithmAlgEccForAgreeKeyCallback
   * @tc.desc     Invalid algorithm alg ecc for agree key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39200', 0, async function (done) {
    var option = generateAgreeKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(alias, option);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var agreeOption = agreeInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ECC);
    agreeOption.inData = exportKeyRet.outData
    hks.agreeKey(alias, agreeOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39300
   * @tc.name     InvalidAlgorithmAlgDsaForAgreeKeyCallback
   * @tc.desc     Invalid algorithm alg dsa for agree key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39300', 0, async function (done) {
    var option = generateAgreeKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(alias, option);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var agreeOption = agreeInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_DSA);
    agreeOption.inData = exportKeyRet.outData
    hks.agreeKey(alias, agreeOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39400
   * @tc.name     InvalidAlgorithmAlgAesForAgreeKeyCallback
   * @tc.desc     Invalid algorithm alg aes for agree key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39400', 0, async function (done) {
    var option = generateAgreeKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(alias, option);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var agreeOption = agreeInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_AES);
    agreeOption.properties[1].value = hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256;
    agreeOption.inData = exportKeyRet.outData
    hks.agreeKey(alias, agreeOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39500
   * @tc.name     InvalidAlgorithmAlgHmacForAgreeKeyCallback
   * @tc.desc     Invalid algorithm alg hmac for agree key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39500', 0, async function (done) {
    var option = generateAgreeKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(alias, option);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var agreeOption = agreeInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HMAC);
    agreeOption.inData = exportKeyRet.outData
    hks.agreeKey(alias, agreeOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39600
   * @tc.name     InvalidAlgorithmAlgHkdfForAgreeKeyCallback
   * @tc.desc     Invalid algorithm alg hkdf for agree key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39600', 0, async function (done) {
    var option = generateAgreeKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(alias, option);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var agreeOption = agreeInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_HKDF);
    agreeOption.inData = exportKeyRet.outData
    hks.agreeKey(alias, agreeOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39700
   * @tc.name     InvalidAlgorithmAlgPbkdf2ForAgreeKeyCallback
   * @tc.desc     Invalid algorithm alg pbkdf2 for agree key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39700', 0, async function (done) {
    var option = generateAgreeKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(alias, option);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var agreeOption = agreeInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_PBKDF2);
    agreeOption.inData = exportKeyRet.outData
    hks.agreeKey(alias, agreeOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39800
   * @tc.name     InvalidAlgorithmAlgEd25519ForAgreeKeyCallback
   * @tc.desc     Invalid algorithm alg ed25519 for agree key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39800', 0, async function (done) {
    var option = generateAgreeKeyOption();
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(alias, option);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var agreeOption = agreeInvalidAlgOption(hks.HuksKeyAlg.HUKS_ALG_ED25519);
    agreeOption.properties[1].value = hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256;
    agreeOption.inData = exportKeyRet.outData
    hks.agreeKey(alias, agreeOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_39900
   * @tc.name     InvalidPurposeEncryptForMacCallback
   * @tc.desc     Invalid purpose encrypt for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_39900', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT, hks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40000
   * @tc.name     InvalidPurposeDecryptForMacCallback
   * @tc.desc     Invalid purpose decrypt for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40000', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      hks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40100
   * @tc.name     InvalidPurposeSignForMacCallback
   * @tc.desc     Invalid purpose sign for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40100', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      hks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40200
   * @tc.name     InvalidPurposeVerifyForMacCallback
   * @tc.desc     Invalid purpose verify for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40200', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40300
   * @tc.name     InvalidPurposeDeriveForMacCallback
   * @tc.desc     Invalid purpose derive for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40300', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      hks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40400
   * @tc.name     InvalidPurposeWrapForMacCallback
   * @tc.desc     Invalid purpose wrap for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40400', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_WRAP,
      hks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40500
   * @tc.name     InvalidPurposeUnwrapForMacCallback
   * @tc.desc     Invalid purpose unwrap for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40500', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_UNWRAP,
      hks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40600
   * @tc.name     InvalidPurposeAgreeForMacCallback
   * @tc.desc     Invalid purpose agree for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40600', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      hks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40700
   * @tc.name     InvalidDigestNoneForMacCallback
   * @tc.desc     Invalid digest none for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40700', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      hks.HuksKeyDigest.HUKS_DIGEST_NONE);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40800
   * @tc.name     InvalidDigestMd5ForMacCallback
   * @tc.desc     Invalid digest md5 for mac callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40800', 0, async function (done) {
    await hmacGenerateKey();
    var macOption = hmacInvalidPurposeOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      hks.HuksKeyDigest.HUKS_DIGEST_MD5);
    hks.mac(alias, macOption, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      done();
    });
  });
});
