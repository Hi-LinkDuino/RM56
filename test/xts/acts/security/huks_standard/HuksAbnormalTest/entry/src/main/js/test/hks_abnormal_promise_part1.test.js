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
  aliasEmpty,
  aliasTooLong,
  aliasCritical,
  aliasNotExist,
  emptyOption,
  invalidValue,
  generateAesCBCKeyOption,
  generateAesGCMKeyOption,
  generateRSAKeyOption,
  generateECCKeyOption,
  generateMACKeyOption,
  plainTextSize8,
  plainTextSize16,
  plainTextSize64,
  makeAgreeOption,
  makeEncryptAndDecryptOptionForAbnormal,
  makeMacOption,
  makeSignVerifyAndImportOption,
  inDataTooLong,
  aliasStrange,
  invalidValueUint8Array
} from './hks_abnormal_common.test.js'

describe('Hks_Abnormal_Promise_Part1', function () {

  async function generateAesKey(mode) {
    var option = {};
    if (mode == 'CBC') {
      option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    } else {
      option = JSON.parse(JSON.stringify(generateAesGCMKeyOption));
    }
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };

  async function deleteKeyPromise(alias, done) {
    var ret = await hks.deleteKey(alias, emptyOption);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  };

  async function generateRsaKey() {
    var option = generateRSAKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };

  async function generateEccKey() {
    var option = generateECCKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };

  async function generateMacKey() {
    var option = generateMACKeyOption;
    var ret = await hks.generateKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
  };


  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_00100
   * @tc.name     GenerateKeyPromiseEmptyAlias
   * @tc.desc     Generate key promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_00100', 0, async function (done) {
    var genKeyRet = await hks.generateKey(aliasEmpty, generateAesCBCKeyOption);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_00200
   * @tc.name     GenerateKeyPromiseCriticalAlias
   * @tc.desc     Generate key promise with critical length alias.
   */
  it('HUKS_ABNORMAL_PROMISE_00200', 0, async function (done) {
    var genKeyRet = await hks.generateKey(aliasCritical, generateAesCBCKeyOption);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var deleteKeyRet = await hks.deleteKey(aliasCritical, emptyOption);
    expect(deleteKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_00300
   * @tc.name     GenerateKeyPromiseLongAlias
   * @tc.desc     Generate key promise with too long alias.
   */
  it('HUKS_ABNORMAL_PROMISE_00300', 0, async function (done) {
    var genKeyRet = await hks.generateKey(aliasTooLong, generateAesCBCKeyOption);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_00400
   * @tc.name     GenerateKeyPromiseLackAlgTag
   * @tc.desc     Generate key promise lack of alg tag.
   */
  it('HUKS_ABNORMAL_PROMISE_00400', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties.splice(0, 1);
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_ALG_FAIL);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_00500
   * @tc.name     GenerateKeyPromiseLackSizeTag
   * @tc.desc     Generate key promise lack of size tag.
   */
  it('HUKS_ABNORMAL_PROMISE_00500', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties.splice(1, 1);
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_00600
   * @tc.name     GenerateKeyPromiseLackPurposeTag
   * @tc.desc     Generate key promise lack of purpose tag.
   */
  it('HUKS_ABNORMAL_PROMISE_00600', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties.splice(2, 1);
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_PURPOSE_FAIL);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_00700
   * @tc.name     GenerateKeyPromiseLackPaddingTag
   * @tc.desc     Generate key promise lack of padding tag.
   */
  it('HUKS_ABNORMAL_PROMISE_00700', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties.splice(3, 1);
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_PADDING_FAIL);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_00800
   * @tc.name     GenerateKeyPromiseLackModeTag
   * @tc.desc     Generate key promise lack of mode tag.
   */
  it('HUKS_ABNORMAL_PROMISE_00800', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties.splice(4, 1);
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_MODE_FAIL);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_00900
   * @tc.name     InvalidAlgTagForGeneratingKeyPromise
   * @tc.desc     Invalid value of alg tag for generating key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_00900', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties[0].value = invalidValue;
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01000
   * @tc.name     InvalidSizeTagForGeneratingKeyPromise
   * @tc.desc     Invalid value of size tag for generating key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_01000', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties[1].value = invalidValue;
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01100
   * @tc.name     InvalidPurposeTagForGeneratingKeyPromise
   * @tc.desc     Invalid value of purpose tag for generating key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_01100', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties[2].value = invalidValue;
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01200
   * @tc.name     InvalidPaddingTagForGeneratingKeyPromise
   * @tc.desc     Invalid value of padding tag for generating key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_01200', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties[3].value = invalidValue;
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01300
   * @tc.name     InvalidModeTagForGeneratingKeyPromise
   * @tc.desc     Invalid value of mode tag for generating key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_01300', 0, async function (done) {
    var option = JSON.parse(JSON.stringify(generateAesCBCKeyOption));
    option.properties[4].value = invalidValue;
    var genKeyRet = await hks.generateKey(alias, option);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_MODE);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01400
   * @tc.name     DeletekeyPromiseNotExistAlias
   * @tc.desc     Delete key promise with not exist alias.
   */
  it('HUKS_ABNORMAL_PROMISE_01400', 0, async function (done) {
    var ret = await hks.deleteKey(aliasNotExist, emptyOption);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01500
   * @tc.name     DeletekeyPromiseEmptyAlias
   * @tc.desc     Delete key promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_01500', 0, async function (done) {
    var ret = await hks.deleteKey(aliasEmpty, emptyOption);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01600
   * @tc.name     DeletekeyPromiseLongAlias
   * @tc.desc     Delete key promise with too long alias.
   */
  it('HUKS_ABNORMAL_PROMISE_01600', 0, async function (done) {
    var ret = await hks.deleteKey(aliasTooLong, emptyOption);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01700
   * @tc.name     EncryptPromiseEmptyAlias
   * @tc.desc     Encrypt promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_01700', 0, async function (done) {
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    var ret = await hks.encrypt(aliasEmpty, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01800
   * @tc.name     EncryptPromiseNotExistAlias
   * @tc.desc     Encrypt promise with not exist alias.
   */
  it('HUKS_ABNORMAL_PROMISE_01800', 0, async function (done) {
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    var ret = await hks.encrypt(aliasNotExist, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_01900
   * @tc.name     EncryptPromiseCriticalAlias
   * @tc.desc     Encrypt promise with critical length alias.
   */
  it('HUKS_ABNORMAL_PROMISE_01900', 0, async function (done) {
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    var ret = await hks.encrypt(aliasCritical, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02000
   * @tc.name     EncryptPromiseLongAlias
   * @tc.desc     Encrypt promise with too long alias.
   */
  it('HUKS_ABNORMAL_PROMISE_02000', 0, async function (done) {
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    var ret = await hks.encrypt(aliasTooLong, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02100
   * @tc.name     EncryptPromiseLackAlgTag
   * @tc.desc     Encrypt promise lack of alg tag.
   */
  it('HUKS_ABNORMAL_PROMISE_02100', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(0, 1);
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_ALG_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02200
   * @tc.name     EncryptPromiseLackPurposeTag
   * @tc.desc     Encrypt promise lack of purpose tag.
   */
  it('HUKS_ABNORMAL_PROMISE_02200', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(1, 1);
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_PURPOSE_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02300
   * @tc.name     EncryptPromiseLackModeTag
   * @tc.desc     Encrypt promise lack of mode tag.
   */
  it('HUKS_ABNORMAL_PROMISE_02300', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(2, 1);
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_MODE_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02400
   * @tc.name     EncryptPromiseLackPaddingTag
   * @tc.desc     Encrypt promise lack of padding tag.
   */
  it('HUKS_ABNORMAL_PROMISE_02400', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(3, 1);
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_PADDING_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02500
   * @tc.name     EncryptPromiseLackIvTag
   * @tc.desc     Encrypt promise lack of iv tag.
   */
  it('HUKS_ABNORMAL_PROMISE_02500', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(4, 1);
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_IV_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02600
   * @tc.name     EncryptPromiseLackNonceTag
   * @tc.desc     Encrypt promise lack of nonce tag.
   */
  it('HUKS_ABNORMAL_PROMISE_02600', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.properties.splice(4, 1);
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_NONCE_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02700
   * @tc.name     EncryptPromiseLackAssociatedDataTag
   * @tc.desc     Encrypt promise lack of associated data tag.
   */
  it('HUKS_ABNORMAL_PROMISE_02700', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.properties.splice(5, 1);
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_AAD_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02800
   * @tc.name     InvalidAlgTagForEncryptPromise
   * @tc.desc     Invalid value of alg tag for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_02800', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[0].value = invalidValue;
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_02900
   * @tc.name     InvalidPurposeTagForEncryptPromise
   * @tc.desc     Invalid value of purpose tag for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_02900', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[1].value = invalidValue;
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03000
   * @tc.name     InvalidModeTagForEncryptPromise
   * @tc.desc     Invalid value of mode tag for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_03000', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[2].value = invalidValue;
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_MODE);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03100
   * @tc.name     InvalidPaddingTagForEncryptPromise
   * @tc.desc     Invalid value of padding tag for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_03100', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[3].value = invalidValue;
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03200
   * @tc.name     InvalidIndataOfOptionForEncryptPromise
   * @tc.desc     Invalid indata of option for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_03200', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.inData = new Uint8Array(0);
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03300
   * @tc.name     DecryptPromiseEmptyAlias
   * @tc.desc     Decrypt promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_03300', 0, async function (done) {
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    var ret = await hks.decrypt(aliasEmpty, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03400
   * @tc.name     DecryptPromiseNotExistAlias
   * @tc.desc     Decrypt promise with not exist alias.
   */
  it('HUKS_ABNORMAL_PROMISE_03400', 0, async function (done) {
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    var ret = await hks.decrypt(aliasNotExist, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03500
   * @tc.name     DecryptPromiseCriticalAlias
   * @tc.desc     Decrypt promise with critical length alias.
   */
  it('HUKS_ABNORMAL_PROMISE_03500', 0, async function (done) {
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    var ret = await hks.decrypt(aliasCritical, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03600
   * @tc.name     DecryptPromiseLongAlias
   * @tc.desc     Decrypt promise with too long alias.
   */
  it('HUKS_ABNORMAL_PROMISE_03600', 0, async function (done) {
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    var ret = await hks.decrypt(aliasTooLong, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03700
   * @tc.name     DecryptPromiseLackAlgTag
   * @tc.desc     Decrypt promise lack of alg tag.
   */
  it('HUKS_ABNORMAL_PROMISE_03700', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(0, 1);
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_ALG_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03800
   * @tc.name     DecryptPromiseLackPurposeTag
   * @tc.desc     Decrypt promise lack of purpose tag.
   */
  it('HUKS_ABNORMAL_PROMISE_03800', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(1, 1);
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_PURPOSE_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_03900
   * @tc.name     DecryptPromiseLackModeTag
   * @tc.desc     Decrypt promise lack of mode tag.
   */
  it('HUKS_ABNORMAL_PROMISE_03900', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(2, 1);
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_MODE_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04000
   * @tc.name     DecryptPromiseLackPaddingTag
   * @tc.desc     Decrypt promise lack of padding tag.
   */
  it('HUKS_ABNORMAL_PROMISE_04000', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(3, 1);
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_PADDING_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04100
   * @tc.name     DecryptPromiseLackIvTag
   * @tc.desc     Decrypt promise lack of iv tag.
   */
  it('HUKS_ABNORMAL_PROMISE_04100', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties.splice(4, 1);
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_IV_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04200
   * @tc.name     DecryptPromiseLackNonceTag
   * @tc.desc     Decrypt promise lack of nonce tag.
   */
  it('HUKS_ABNORMAL_PROMISE_04200', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.properties.splice(4, 1);
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_NONCE_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04300
   * @tc.name     DecryptPromiseLackAssociatedDataTag
   * @tc.desc     Decrypt promise lack of associated data tag.
   */
  it('HUKS_ABNORMAL_PROMISE_04300', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.properties.splice(5, 1);
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_AAD_FAIL);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04400
   * @tc.name     InvalidAlgTagForDecryptPromise
   * @tc.desc     Invalid value of alg tag for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_04400', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[0].value = invalidValue;
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04500
   * @tc.name     InvalidPurposeTagForDecryptPromise
   * @tc.desc     Invalid value of purpose tag for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_04500', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[1].value = invalidValue;
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04600
   * @tc.name     InvalidModeTagForDecryptPromise
   * @tc.desc     Invalid value of mode tag for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_04600', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[2].value = invalidValue;
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_MODE);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04700
   * @tc.name     InvalidPaddingTagForDecryptPromise
   * @tc.desc     Invalid value of padding tag for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_04700', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[3].value = invalidValue;
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04800
   * @tc.name     InvalidIndataOfOptionForDecryptPromise
   * @tc.desc     Invalid indata of option for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_04800', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.inData = new Uint8Array(0);
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_04900
   * @tc.name     ImportKeyPromiseEmptyAlias
   * @tc.desc     Import key promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_04900', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    var ret = await hks.importKey(aliasEmpty, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05000
   * @tc.name     ImportKeyPromiseCriticalAlias
   * @tc.desc     Import key promise with critical length alias.
   */
  it('HUKS_ABNORMAL_PROMISE_05000', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    var ret = await hks.importKey(aliasCritical, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05100
   * @tc.name     ImportKeyPromiseLongAlias
   * @tc.desc     Import key promise with too long alias.
   */
  it('HUKS_ABNORMAL_PROMISE_05100', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    var ret = await hks.importKey(aliasTooLong, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05200
   * @tc.name     ImportKeyPromiseLackAlgTag
   * @tc.desc     Import key promise lack of alg tag.
   */
  it('HUKS_ABNORMAL_PROMISE_05200', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties.splice(0, 1);
    var ret = await hks.importKey(aliasNotExist, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_ALG_FAIL);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05300
   * @tc.name     ImportKeyPromiseLackPurposeTag
   * @tc.desc     Import key promise lack of purpose tag.
   */
  it('HUKS_ABNORMAL_PROMISE_05300', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties.splice(1, 1);
    var ret = await hks.importKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05400
   * @tc.name     ImportKeyPromiseLackPaddingTag
   * @tc.desc     Import key promise lack of padding tag.
   */
  it('HUKS_ABNORMAL_PROMISE_05400', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties.splice(2, 1);
    var ret = await hks.importKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05500
   * @tc.name     ImportKeyPromiseLackDigestTag
   * @tc.desc     Import key promise lack of digest tag.
   */
  it('HUKS_ABNORMAL_PROMISE_05500', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties.splice(3, 1);
    var ret = await hks.importKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05600
   * @tc.name     InvalidAlgTagForImportKeyPromise
   * @tc.desc     Invalid value of alg tag for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_05600', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties[0].value = invalidValue;
    var ret = await hks.importKey(aliasNotExist, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05700
   * @tc.name     InvalidPurposeTagForImportKeyPromise
   * @tc.desc     Invalid value of alg tag for purpose key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_05700', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties[1].value = invalidValue;
    var ret = await hks.importKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05800
   * @tc.name     InvalidPaddingTagForImportKeyPromise
   * @tc.desc     Invalid value of alg tag for padding key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_05800', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties[2].value = invalidValue;
    var ret = await hks.importKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_05900
   * @tc.name     InvalidDigestTagForImportKeyPromise
   * @tc.desc     Invalid value of alg tag for digest key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_05900', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties[3].value = invalidValue;
    var ret = await hks.importKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06000
   * @tc.name     InvalidIndataOfOptionForImportKeyPromise
   * @tc.desc     Invalid indata of option for import key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_06000', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.inData = new Uint8Array(0);
    var ret = await hks.importKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done()
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06100
   * @tc.name     ExportKeyPromiseNotExistAlias
   * @tc.desc     Export key promise with not exist alias.
   */
  it('HUKS_ABNORMAL_PROMISE_06100', 0, async function (done) {
    var ret = await hks.exportKey(aliasNotExist, emptyOption);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06200
   * @tc.name     ExportKeyPromiseLongAlias
   * @tc.desc     Export key promise with too long alias.
   */
  it('HUKS_ABNORMAL_PROMISE_06200', 0, async function (done) {
    var ret = await hks.exportKey(aliasTooLong, emptyOption);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06300
   * @tc.name     ExportKeyPromiseEmptyAlias
   * @tc.desc     Export key promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_06300', 0, async function (done) {
    var ret = await hks.exportKey(aliasEmpty, emptyOption);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06400
   * @tc.name     SignPromiseNotExistAlias
   * @tc.desc     Sign promise with not exist alias.
   */
  it('HUKS_ABNORMAL_PROMISE_06400', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    var ret = await hks.sign(aliasNotExist, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06500
   * @tc.name     SignPromiseLongAlias
   * @tc.desc     Sign promise with too long alias.
   */
  it('HUKS_ABNORMAL_PROMISE_06500', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    var ret = await hks.sign(aliasTooLong, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06600
   * @tc.name     SignPromiseEmptyAlias
   * @tc.desc     Sign promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_06600', 0, async function (done) {
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    var ret = await hks.sign(aliasEmpty, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06700
   * @tc.name     InvalidAlgTagForSignPromise
   * @tc.desc     Invalid value of alg tag for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_06700', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties[0].value = invalidValue;
    var ret = await hks.sign(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06800
   * @tc.name     InvalidPurposeTagForSignPromise
   * @tc.desc     Invalid value of purpose tag for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_06800', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties[1].value = invalidValue;
    var ret = await hks.sign(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_06900
   * @tc.name     InvalidPaddingTagForSignPromise
   * @tc.desc     Invalid value of padding tag for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_06900', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize8);
    option.properties[2].value = invalidValue;
    var ret = await hks.sign(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07000
   * @tc.name     InvalidDigestTagForSignPromise
   * @tc.desc     Invalid value of digest tag for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_07000', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize8);
    option.properties[3].value = invalidValue;
    var ret = await hks.sign(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07100
   * @tc.name     InvalidIndataOfOptionForSignPromise
   * @tc.desc     Invalid indata of option for sign promise.
   */
  it('HUKS_ABNORMAL_PROMISE_07100', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.inData = new Uint8Array(0);
    var ret = await hks.sign(aliasEmpty, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07200
   * @tc.name     SignPromiseLackAlgTag
   * @tc.desc     Sign promise lack of alg tag.
   */
  it('HUKS_ABNORMAL_PROMISE_07200', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties.splice(0, 1);
    var ret = await hks.sign(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_ALG_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07300
   * @tc.name     SignPromiseLackPurposeTag
   * @tc.desc     Sign promise lack of purpose tag.
   */
  it('HUKS_ABNORMAL_PROMISE_07300', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties.splice(1, 1);
    var ret = await hks.sign(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07400
   * @tc.name     SignPromiseLackPaddingTag
   * @tc.desc     Sign promise lack of padding tag.
   */
  it('HUKS_ABNORMAL_PROMISE_07400', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize8);
    option.properties.splice(2, 1);
    var ret = await hks.sign(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07500
   * @tc.name     SignPromiseLackDigestTag
   * @tc.desc     Sign promise lack of digest tag.
   */
  it('HUKS_ABNORMAL_PROMISE_07500', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize8);
    option.properties.splice(3, 1);
    var ret = await hks.sign(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07600
   * @tc.name     VerifyPromiseNotExistAlias
   * @tc.desc     Verify promise with not exist alias.
   */
  it('HUKS_ABNORMAL_PROMISE_07600', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    var ret = await hks.verify(aliasNotExist, option, plainTextSize64);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07700
   * @tc.name     VerifyPromiseLongAlias
   * @tc.desc     Verify promise with too long alias.
   */
  it('HUKS_ABNORMAL_PROMISE_07700', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    var ret = await hks.verify(aliasTooLong, option, plainTextSize64);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07800
   * @tc.name     VerifyPromiseEmptyAlias
   * @tc.desc     Verify promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_07800', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    var ret = await hks.verify(aliasEmpty, option, plainTextSize64);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_07900
   * @tc.name     InvalidAlgTagForVerifyPromise
   * @tc.desc     Invalid value of alg tag for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_07900', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties[0].value = invalidValue;
    var ret = await hks.verify(alias, option, plainTextSize64);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08000
   * @tc.name     InvalidPurposeTagForVerifyPromise
   * @tc.desc     Invalid value of purpose tag for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_08000', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize8);
    option.properties[1].value = invalidValue;
    var ret = await hks.verify(alias, option, plainTextSize8);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08100
   * @tc.name     InvalidPaddingTagForVerifyPromise
   * @tc.desc     Invalid value of padding tag for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_08100', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize8);
    option.properties[2].value = invalidValue;
    var ret = await hks.verify(alias, option, plainTextSize8);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08200
   * @tc.name     InvalidDigestTagForVerifyPromise
   * @tc.desc     Invalid value of digest tag for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_08200', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize8);
    option.properties[3].value = invalidValue;
    var ret = await hks.verify(alias, option, plainTextSize8);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08300
   * @tc.name     InvalidIndataOfOptionForVerifyPromise
   * @tc.desc     Invalid indata of option for verify promise.
   */
  it('HUKS_ABNORMAL_PROMISE_08300', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize16);
    option.inData = new Uint8Array(0);
    var ret = await hks.verify(alias, option, plainTextSize64);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08400
   * @tc.name     VerifyPromiseLackAlgTag
   * @tc.desc     Verify promise lack of alg tag.
   */
  it('HUKS_ABNORMAL_PROMISE_08400', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties.splice(0, 1);
    var ret = await hks.verify(alias, option, plainTextSize64);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_ALG_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08500
   * @tc.name     VerifyPromiseLackPurposeTag
   * @tc.desc     Verify promise lack of purpose tag.
   */
  it('HUKS_ABNORMAL_PROMISE_08500', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties.splice(1, 1);
    var ret = await hks.verify(aliasNotExist, option, plainTextSize64);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08600
   * @tc.name     VerifyPromiseLackPaddingTag
   * @tc.desc     Verify promise lack of padding tag.
   */
  it('HUKS_ABNORMAL_PROMISE_08600', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize8);
    option.properties.splice(2, 1);
    var ret = await hks.verify(alias, option, plainTextSize8);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08700
   * @tc.name     VerifyPromiseLackDigestTag
   * @tc.desc     Verify promise lack of digest tag.
   */
  it('HUKS_ABNORMAL_PROMISE_08700', 0, async function (done) {
    await generateRsaKey();
    var option = makeSignVerifyAndImportOption(plainTextSize64);
    option.properties.splice(3, 1);
    var ret = await hks.verify(alias, option, plainTextSize64);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08800
   * @tc.name     InvalidIvTagForEncryptPromise
   * @tc.desc     Invalid value of iv tag for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_08800', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[4].value = invalidValueUint8Array;
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_08900
   * @tc.name     AgreeKeyPromiseEmptyAlias
   * @tc.desc     Agree key promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_08900', 0, async function (done) {
    var option = makeAgreeOption();
    var ret = await hks.agreeKey(aliasEmpty, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09000
   * @tc.name     InvalidAlgTagForAgreeKeyPromise
   * @tc.desc     Invalid value of alg tag for agree key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_09000', 0, async function (done) {
    await generateEccKey();
    var option = makeAgreeOption();
    option.properties[0].value = invalidValue;
    var ret = await hks.agreeKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09100
   * @tc.name     InvalidSizeTagForAgreeKeyPromise
   * @tc.desc     Invalid value of size tag for agree key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_09100', 0, async function (done) {
    await generateEccKey();
    var option = makeAgreeOption();
    option.properties[1].value = invalidValue;
    var ret = await hks.agreeKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09200
   * @tc.name     InvalidIndataOfOptionForAgreeKeyPromise
   * @tc.desc     Invalid indata of option for agree key promise.
   */
  it('HUKS_ABNORMAL_PROMISE_09200', 0, async function (done) {
    await generateEccKey();
    var option = makeAgreeOption();
    option.inData = new Uint8Array(0);
    var ret = await hks.agreeKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09300
   * @tc.name     AgreeKeyPromiseLackAlgTag
   * @tc.desc     Agree key promise lack of alg tag.
   */
  it('HUKS_ABNORMAL_PROMISE_09300', 0, async function (done) {
    await generateEccKey();
    var option = makeAgreeOption();
    option.properties.splice(0, 1);
    var ret = await hks.agreeKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_ALG_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09400
   * @tc.name     AgreeKeyPromiseLackSizeTag
   * @tc.desc     Agree key promise lack of size tag.
   */
  it('HUKS_ABNORMAL_PROMISE_09400', 0, async function (done) {
    await generateEccKey();
    var option = makeAgreeOption();
    option.properties[1].value = invalidValue;
    var ret = await hks.agreeKey(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09500
   * @tc.name     MacPromiseEmptyAlias
   * @tc.desc     Mac promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_09500', 0, async function (done) {
    await generateMacKey();
    var option = makeMacOption();
    var ret = await hks.mac(aliasNotExist, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_NOT_EXIST);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09600
   * @tc.name     MacPromiseTooLongAlias
   * @tc.desc     Mac promise with too long alias.
   */
  it('HUKS_ABNORMAL_PROMISE_09600', 0, async function (done) {
    await generateMacKey();
    var option = makeMacOption();
    var ret = await hks.mac(aliasTooLong, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09700
   * @tc.name     MacPromiseEmptyAlias
   * @tc.desc     Mac promise with empty alias.
   */
  it('HUKS_ABNORMAL_PROMISE_09700', 0, async function (done) {
    await generateMacKey();
    var option = makeMacOption();
    var ret = await hks.mac(aliasEmpty, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09800
   * @tc.name     InvalidPurposeTagForMacPromise
   * @tc.desc     Invalid value of purpose tag for mac promise.
   */
  it('HUKS_ABNORMAL_PROMISE_09800', 0, async function (done) {
    await generateMacKey();
    var option = makeMacOption();
    option.properties[0].value = invalidValue;
    var ret = await hks.mac(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_09900
   * @tc.name     InvalidDigestTagForMacPromise
   * @tc.desc     Invalid value of digest tag for mac promise.
   */
  it('HUKS_ABNORMAL_PROMISE_09900', 0, async function (done) {
    await generateMacKey();
    var option = makeMacOption();
    option.properties[1].value = invalidValue;
    var ret = await hks.mac(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10000
   * @tc.name     InvalidIndataOfOptionForMacPromise
   * @tc.desc     Invalid indata of option for mac promise.
   */
  it('HUKS_ABNORMAL_PROMISE_10000', 0, async function (done) {
    await generateMacKey();
    var option = makeMacOption();
    option.inData = new Uint8Array(0);
    var ret = await hks.mac(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10100
   * @tc.name     MacPromiseLackPurposeTag
   * @tc.desc     Mac promise lack of purpose tag.
   */
  it('HUKS_ABNORMAL_PROMISE_10100', 0, async function (done) {
    await generateMacKey();
    var option = makeMacOption();
    option.properties.splice(0, 1);
    var ret = await hks.mac(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_PURPOSE_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10200
   * @tc.name     MacPromiseLackDigestTag
   * @tc.desc     Mac promise lack of digest tag.
   */
  it('HUKS_ABNORMAL_PROMISE_10200', 0, async function (done) {
    await generateMacKey();
    var option = makeMacOption();
    option.properties.splice(1, 1);
    var ret = await hks.mac(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_CHECK_GET_DIGEST_FAIL);
    deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10300
   * @tc.name     LongIndataOfOptionForEncryptPromise
  * @tc.desc     Too long indata of option for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_10300', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.inData = inDataTooLong;
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10400
   * @tc.name     GenerateKeyPromiseStrangeAlias
   * @tc.desc     Generate key promise with strange alias.
   */
  it('HUKS_ABNORMAL_PROMISE_10400', 0, async function (done) {
    var genKeyRet = await hks.generateKey(aliasStrange, generateAesCBCKeyOption);
    expect(genKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    var deleteKeyRet = await hks.deleteKey(aliasStrange, emptyOption);
    expect(deleteKeyRet.errorCode).assertEqual(hks.HuksErrorCode.HUKS_SUCCESS);
    done();
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10500
   * @tc.name     InvalidNonceTagForEncryptPromise
    * @tc.desc     Invalid value of nonce tag for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_10500', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.properties[4].value = invalidValueUint8Array;
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10600
   * @tc.name     InvalidASSOCIATEDDATATagForEncryptPromise
   * @tc.desc     Invalid value of associated data tag for encrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_10600', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.properties[5].value = invalidValueUint8Array;
    var ret = await hks.encrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10700
   * @tc.name     InvalidIvTagForDecryptPromise
   * @tc.desc     Invalid value of iv tag for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_10700', 0, async function (done) {
    await generateAesKey('CBC');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_CBC);
    option.properties[4].value = invalidValueUint8Array;
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10800
   * @tc.name     InvalidNonceTagForDecryptPromise
   * @tc.desc     Invalid value of nonce tag for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_10800', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.properties[4].value = invalidValueUint8Array;
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });

  /**
   * @tc.number   HUKS_ABNORMAL_PROMISE_10900
   * @tc.name     InvalidAssociatedDataTagForDecryptPromise
   * @tc.desc     Invalid value of associated data tag for decrypt promise.
   */
  it('HUKS_ABNORMAL_PROMISE_10900', 0, async function (done) {
    await generateAesKey('GCM');
    var option = makeEncryptAndDecryptOptionForAbnormal(hks.HuksCipherMode.HUKS_MODE_GCM);
    option.properties[5].value = invalidValueUint8Array;
    var ret = await hks.decrypt(alias, option);
    expect(ret.errorCode).assertEqual(hks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    await deleteKeyPromise(alias, done);
  });
});