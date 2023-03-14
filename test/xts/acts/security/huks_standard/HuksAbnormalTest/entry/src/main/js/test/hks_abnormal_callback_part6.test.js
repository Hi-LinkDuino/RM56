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
  aliasA,
  aliasB,
} from '../../../../../../hks_xts_common.test'
import {
  generateImportKeyOption,
  eccGenerateKeyOption,
  importOptionsInvalidAlgOption,
  importOptionsInvalidSizeOption,
  importOptionsInvalidPurposeOrPaddingOption,
} from './hks_abnormal_common.test.js'

describe('Hks_Abnormal_Callback_Part6', function () {
  var publicKey = [];

  var emptyOption = makeEmptyOptions();

  function makeEmptyOptions() {
    var emptyOptions = {
      properties: []
    };
    return emptyOptions;
  };

  async function generateKeyAndExportKey() {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };
  async function eccGenerateKey() {
    var option = eccGenerateKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_40900
   * @tc.name     InvalidPurposeDeriveAlgRsaForImportKeyCallback
   * @tc.desc     Invalid purpose derive alg rsa for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_40900', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_RSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41000
   * @tc.name     InvalidPurposeDeriveAlgEccForImportKeyCallback
   * @tc.desc     Invalid purpose derive alg ecc for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41000', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECC,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41100
   * @tc.name     InvalidPurposeDeriveAlgDsaForImportKeyCallback
   * @tc.desc     Invalid purpose derive alg dsa for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41100', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41200
   * @tc.name     InvalidPurposeDeriveAlgHkdfForImportKeyCallback
   * @tc.desc     Invalid purpose derive alg hkdf for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41200', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_HKDF,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41300
   * @tc.name     InvalidPurposeDeriveAlgPbkdf2ForImportKeyCallback
   * @tc.desc     Invalid purpose derive alg pbkdf2 for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41300', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_PBKDF2,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41400
   * @tc.name     InvalidPurposeDeriveAlgEcdhForImportKeyCallback
   * @tc.desc     Invalid purpose derive alg ecdh for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41400', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECDH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41500
   * @tc.name     InvalidPurposeDeriveAlgX25519ForImportKeyCallback
   * @tc.desc     Invalid purpose derive alg x25519 for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41500', 0, async function (done) {
    await eccGenerateKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_X25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41600
   * @tc.name     InvalidPurposeDeriveAlgEd25519ForImportKeyCallback
   * @tc.desc     Invalid purpose derive alg ed25519 for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41600', 0, async function (done) {
    await eccGenerateKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ED25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41700
   * @tc.name     InvalidPurposeDeriveAlgDhForImportKeyCallback
   * @tc.desc     Invalid purpose derive alg dh for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41700', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41800
   * @tc.name     InvalidPurposeMacAlgRsaForImportKeyCallback
   * @tc.desc     Invalid purpose mac alg rsa for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41800', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_RSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_41900
   * @tc.name     InvalidPurposeMacAlgEccForImportKeyCallback
   * @tc.desc     Invalid purpose mac alg ecc for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_41900', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECC,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42000
   * @tc.name     InvalidPurposeMacAlgDsaForImportKeyCallback
   * @tc.desc     Invalid purpose mac alg dsa for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_42000', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42100
   * @tc.name     InvalidPurposeMacAlgHkdfForImportKeyCallback
   * @tc.desc     Invalid purpose mac alg hkdf for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_42100', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_HKDF,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42200
   * @tc.name     InvalidPurposeMacAlgPbkdf2ForImportKeyCallback
   * @tc.desc     Invalid purpose mac alg pbkdf2 for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_42200', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_PBKDF2,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42300
   * @tc.name     InvalidPurposeMacAlgEcdhForImportKeyCallback
   * @tc.desc     Invalid purpose mac alg ecdh for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_42300', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECDH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42400
   * @tc.name     InvalidPurposeMacAlgX25519ForImportKeyCallback
   * @tc.desc     Invalid purpose mac alg x25519 for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_42400', 0, async function (done) {
    await eccGenerateKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_X25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42500
   * @tc.name     InvalidPurposeMacAlgEd25519ForImportKeyCallback
   * @tc.desc     Invalid purpose mac alg ed25519 for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_42500', 0, async function (done) {
    await eccGenerateKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ED25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42600
   * @tc.name     InvalidPurposeMacAlgDhForImportKeyCallback
   * @tc.desc     Invalid purpose mac alg dh for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_42600', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42700
   * @tc.name     GenerateAndImportKeySizeDifferentAesForImportKeyCallback
   * @tc.desc     Generate key and import key have different key size aes for import key callback. 
   */
  it('HUKS_ABNORMAL_CALLBACK_42700', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidSizeOption(hks.HuksKeyAlg.HUKS_ALG_AES, publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42800
   * @tc.name     GenerateAndImportKeySizeDifferentRsaForImportKeyCallback
   * @tc.desc     Generate key and import key have different key size rsa for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_42800', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidSizeOption(hks.HuksKeyAlg.HUKS_ALG_RSA, publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_42900
   * @tc.name     GenerateAndImportKeySizeDifferentEccForImportKeyCallback
   * @tc.desc     Generate key and import key have different key size ecc for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_42900', 0, async function (done) {
    var option = eccGenerateKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidSizeOption(hks.HuksKeyAlg.HUKS_ALG_ECC, publicKey);
    importOptions.properties[1].value = hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521;
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_43000
   * @tc.name     InvalidPurposeDecryptForImportKeyCallback
   * @tc.desc     Invalid purpose decrypt for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_43000', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_43100
   * @tc.name     InvalidPurposeSignForImportKeyCallback
   * @tc.desc     Invalid purpose sign for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_43100', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_43200
   * @tc.name     InvalidPaddingNonePurposeEncryptForImportKeyCallback
   * @tc.desc     Invalid padding none purpose encrypt for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_43200', 0, async function (done) {
    var option = generateImportKeyOption();
    var purpose = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT;
    option.properties[2].value = purpose;
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PSS,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_43300
   * @tc.name     InvalidPaddingPkcs5PurposeEncryptForImportKeyCallback
   * @tc.desc     Invalid padding pkcs5 purpose encrypt for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_43300', 0, async function (done) {
    var option = generateImportKeyOption();
    var purpose = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT;
    option.properties[2].value = purpose;
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_43400
   * @tc.name     InvalidPaddingPkcs7PurposeEncryptForImportKeyCallback
   * @tc.desc     Invalid padding pkcs7 purpose encrypt for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_43400', 0, async function (done) {
    var option = generateImportKeyOption();
    var purpose = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT;
    option.properties[2].value = purpose;
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_43500
   * @tc.name     InvalidPaddingNonePurposeVerifyForImportKeyCallback
   * @tc.desc     Invalid padding none purpose verify for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_43500', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_NONE,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_43600
   * @tc.name     InvalidPaddingOaepPurposeVerifyForImportKeyCallback
   * @tc.desc     Invalid padding oaep purpose verify for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_43600', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_43700
   * @tc.name     InvalidPaddingPkcs5PurposeVerifyForImportKeyCallback
   * @tc.desc     Invalid padding pkcs5 purpose verify for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_43700', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ABNORMAL_CALLBACK_43800
   * @tc.name     InvalidPaddingPkcs7PurposeVerifyForImportKeyCallback
   * @tc.desc     Invalid padding pkcs7 purpose verify for import key callback.
   */
  it('HUKS_ABNORMAL_CALLBACK_43800', 0, async function (done) {
    await generateKeyAndExportKey();
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      publicKey);
    hks.importKey(aliasB, importOptions, function (err, data) {
      expect(data.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      done();
    });
  });
});