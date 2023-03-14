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

describe('Hks_Abnormal_Promise_Part6', function () {
  var publicKey = [];

  var emptyOption = makeEmptyOptions();

  function makeEmptyOptions() {
    var emptyOptions = {
      properties: []
    };
    return emptyOptions;
  };

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_40900
   * @tc.name     InvalidPurposeDeriveAlgRsaForImportKeyPromise
   * @tc.desc     Invalid purpose derive alg rsa for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_40900', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_RSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41000
   * @tc.name     InvalidPurposeDeriveAlgEccForImportKeyPromise
   * @tc.desc     Invalid purpose derive alg ecc for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41000', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECC,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41100
   * @tc.name     InvalidPurposeDeriveAlgDsaForImportKeyPromise
   * @tc.desc     Invalid purpose derive alg dsa for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41100', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41200
   * @tc.name     InvalidPurposeDeriveAlgHkdfForImportKeyPromise
   * @tc.desc     Invalid purpose derive alg hkdf for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41200', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_HKDF,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41300
   * @tc.name     InvalidPurposeDeriveAlgPbkdf2ForImportKeyPromise
   * @tc.desc     Invalid purpose derive alg pbkdf2 for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41300', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_PBKDF2,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41400
   * @tc.name     InvalidPurposeDeriveAlgEcdhForImportKeyPromise
   * @tc.desc     Invalid purpose derive alg ecdh for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41400', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECDH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41500
   * @tc.name     InvalidPurposeDeriveAlgX25519ForImportKeyPromise
   * @tc.desc     Invalid purpose derive alg x25519 for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41500', 0, async function (done) {
    var option = eccGenerateKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_X25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41600
   * @tc.name     InvalidPurposeDeriveAlgEd25519ForImportKeyPromise
   * @tc.desc     Invalid purpose derive alg ed25519 for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41600', 0, async function (done) {
    var option = eccGenerateKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ED25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41700
   * @tc.name     InvalidPurposeDeriveAlgDhForImportKeyPromise
   * @tc.desc     Invalid purpose derive alg dh for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41700', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41800
   * @tc.name     InvalidPurposeMacAlgRsaForImportKeyPromise
   * @tc.desc     Invalid purpose mac alg rsa for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41800', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_RSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_41900
   * @tc.name     InvalidPurposeMacAlgEccForImportKeyPromise
   * @tc.desc     Invalid purpose mac alg ecc for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_41900', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECC,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42000
   * @tc.name     InvalidPurposeMacAlgDsaForImportKeyPromise
   * @tc.desc     Invalid purpose mac alg dsa for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_42000', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DSA,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42100
   * @tc.name     InvalidPurposeMacAlgHkdfForImportKeyPromise
   * @tc.desc     Invalid purpose mac alg hkdf for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_42100', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_HKDF,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42200
   * @tc.name     InvalidPurposeMacAlgPbkdf2ForImportKeyPromise
   * @tc.desc     Invalid purpose mac alg pbkdf2 for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_42200', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_PBKDF2,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42300
   * @tc.name     InvalidPurposeMacAlgEcdhForImportKeyPromise
   * @tc.desc     Invalid purpose mac alg ecdh for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_42300', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ECDH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42400
   * @tc.name     InvalidPurposeMacAlgX25519ForImportKeyPromise
   * @tc.desc     Invalid purpose mac alg x25519 for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_42400', 0, async function (done) {
    var option = eccGenerateKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_X25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42500
   * @tc.name     InvalidPurposeMacAlgEd25519ForImportKeyPromise
   * @tc.desc     Invalid purpose mac alg ed25519 for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_42500', 0, async function (done) {
    var option = eccGenerateKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_ED25519,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42600
   * @tc.name     InvalidPurposeMacAlgDhForImportKeyPromise
   * @tc.desc     Invalid purpose mac alg dh for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_42600', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidAlgOption(
      hks.HuksKeyAlg.HUKS_ALG_DH,
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42700
   * @tc.name     GenerateAndImportKeySizeDifferentAesForImportKeyPromise
   * @tc.desc     Generate key and import key have different key size aes for import key promise. 
   */
  it('HUKS_ABNORMAL_PROMISE_42700', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidSizeOption(hks.HuksKeyAlg.HUKS_ALG_AES, publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42800
   * @tc.name     GenerateAndImportKeySizeDifferentRsaForImportKeyPromise
   * @tc.desc     Generate key and import key have different key size rsa for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_42800', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidSizeOption(hks.HuksKeyAlg.HUKS_ALG_RSA, publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_42900
   * @tc.name     GenerateAndImportKeySizeDifferentEccForImportKeyPromise
   * @tc.desc     Generate key and import key have different key size ecc for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_42900', 0, async function (done) {
    var option = eccGenerateKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidSizeOption(hks.HuksKeyAlg.HUKS_ALG_ECC, publicKey);
    importOptions.properties[1].value = hks.HuksKeySize.HUKS_ECC_KEY_SIZE_521;
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_INFO);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_43000
   * @tc.name     InvalidPurposeDecryptForImportKeyPromise
   * @tc.desc     Invalid purpose decrypt for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_43000', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_43100
   * @tc.name     InvalidPurposeSignForImportKeyPromise
   * @tc.desc     Invalid purpose sign for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_43100', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_43200
   * @tc.name     InvalidPaddingNonePurposeEncryptForImportKeyPromise
   * @tc.desc     Invalid padding none purpose encrypt for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_43200', 0, async function (done) {
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
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_43300
   * @tc.name     InvalidPaddingPkcs5PurposeEncryptForImportKeyPromise
   * @tc.desc     Invalid padding pkcs5 purpose encrypt for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_43300', 0, async function (done) {
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
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_43400
   * @tc.name     InvalidPaddingPkcs7PurposeEncryptForImportKeyPromise
   * @tc.desc     Invalid padding pkcs7 purpose encrypt for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_43400', 0, async function (done) {
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
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_43500
   * @tc.name     InvalidPaddingNonePurposeVerifyForImportKeyPromise
   * @tc.desc     Invalid padding none purpose verify for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_43500', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_NONE,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_43600
   * @tc.name     InvalidPaddingOaepPurposeVerifyForImportKeyPromise
   * @tc.desc     Invalid padding oaep purpose verify for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_43600', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_OAEP,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_43700
   * @tc.name     InvalidPaddingPkcs5PurposeVerifyForImportKeyPromise
   * @tc.desc     Invalid padding pkcs5 purpose verify for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_43700', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS5,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_43800
   * @tc.name     InvalidPaddingPkcs7PurposeVerifyForImportKeyPromise
   * @tc.desc     Invalid padding pkcs7 purpose verify for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_43800', 0, async function (done) {
    var option = generateImportKeyOption();
    var ret = await hks.generateKey(aliasA, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await hks.exportKey(aliasA, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importOptions = importOptionsInvalidPurposeOrPaddingOption(
      hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
      hks.HuksKeyPadding.HUKS_PADDING_PKCS7,
      publicKey);
    var importKeyRet = await hks.importKey(aliasB, importOptions);
    expect(importKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });
});