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

import huks from '@ohos.security.huks'
import {describe, it, expect} from 'deccjsunit/index'
import {
  emptyOption,
  timer,
} from '../../../../../../hks_xts_common.test.js'

describe('HuksTest_JSApiTest_DeriveKey', function () {

  function makeGenerateKeyOption(digest) {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: huks.HuksKeyAlg.HUKS_ALG_AES
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
      value: huks.HuksKeySize.HUKS_AES_KEY_SIZE_128
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE
    };
    properties[3] = {
      tag: huks.HuksTag.HUKS_TAG_BLOCK_MODE,
      value: huks.HuksCipherMode.HUKS_MODE_CBC
    };
    properties[4] = {
      tag: huks.HuksTag.HUKS_TAG_DIGEST,
      value: digest
    };
    var options = {
      properties: properties
    };
    return options;
  };

  function makeHKDFDeriveOption(digest) {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: huks.HuksKeyAlg.HUKS_ALG_HKDF
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_DIGEST,
      value: digest
    };
    var options = {
      properties: properties
    };
    return options;
  };

  function makePBKDF2DeriveOption(digest, iteration, salt) {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: huks.HuksKeyAlg.HUKS_ALG_PBKDF2
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DERIVE
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_DIGEST,
      value: digest
    };
    properties[3] = {
      tag: huks.HuksTag.HUKS_TAG_ITERATION,
      value: iteration
    };
    properties[4] = {
      tag: huks.HuksTag.HUKS_TAG_SALT,
      value: salt
    };
    var options = {
      properties: properties
    };
    return options;
  };

  function deleteKey(done) {
    huks.deleteKey('alias1', emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      isKeyExist(done);
    });
  };

  function isKeyExist(done) {
    huks.isKeyExist('alias1', emptyOption, function (err, data) {
      expect(data).assertEqual(false);
      done();
    });
  };

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_00100
   * @tc.name     PromiseGenerateSha256HkdfDeriveNone
   * @tc.desc     Promise generate key digest sha256 hkdf derive key digest none. 
   */
  it('HuksTest_JSApiTest_DeriveKey_00100', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_00100 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_NONE);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(deriveKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    var deleteKeyResult = await huks.deleteKey('alias1', emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_00100 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_00200
   * @tc.name     PromiseGenerateSha256HkdfDeriveMd5
   * @tc.desc     Promise generate key digest sha256 hkdf derive key digest md5.
   */
  it('HuksTest_JSApiTest_DeriveKey_00200', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_00200 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_MD5);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(deriveKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    var deleteKeyResult = await huks.deleteKey('alias1', emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_00200 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_00300
   * @tc.name     PromiseGenerateSha256HkdfDeriveSha1
   * @tc.desc     Promise generate key digest sha256 hkdf derive key digest sha1.
   */
  it('HuksTest_JSApiTest_DeriveKey_00300', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_00300 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT).assertEqual(deriveKeyResult.errorCode);
    var deleteKeyResult = await huks.deleteKey('alias1', emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_00300 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_00400
   * @tc.name     PromiseGenerateSha256HkdfDeriveSha224
   * @tc.desc     Promise generate key digest sha256 hkdf derive key digest sha224.
   */
  it('HuksTest_JSApiTest_DeriveKey_00400', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_00400 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA224);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT).assertEqual(deriveKeyResult.errorCode);
    var deleteKeyResult = await huks.deleteKey('alias1', emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_00400 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_00500
   * @tc.name     PromiseGenerateSha256HkdfDeriveSha256
   * @tc.desc     Promise generate key digest sha256 hkdf derive key digest sha256.
   */
  it('HuksTest_JSApiTest_DeriveKey_00500', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_00500 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(deriveKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deleteResult = await huks.deleteKey('alias1', emptyOption);
    expect(deleteResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_00500 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_00600
   * @tc.name     PromiseGenerateSha384HkdfDeriveSha384
   * @tc.desc     Promise generate key digest sha384 hkdf derive key digest sha384.
   */
  it('HuksTest_JSApiTest_DeriveKey_00600', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_00500 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA384);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA384);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(deriveKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deleteResult = await huks.deleteKey('alias1', emptyOption);
    expect(deleteResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_00500 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_00700
   * @tc.name     PromiseGenerateSha512HkdfDeriveSha512
   * @tc.desc     Promise generate key digest sha512 hkdf derive key digest sha512.
   */
  it('HuksTest_JSApiTest_DeriveKey_00700', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_00700 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA512);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA512);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(deriveKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deleteResult = await huks.deleteKey('alias1', emptyOption);
    expect(deleteResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_00700 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_00800
   * @tc.name     CallbackGenerateSha256HkdfDeriveNone
   * @tc.desc     Callback generate key digest sha256 hkdf derive key digest none.
   */
  it('HuksTest_JSApiTest_DeriveKey_00800', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_00800 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_NONE);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('data' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      console.log('HuksTest_JSApiTest_DeriveKey_00800 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_00900
   * @tc.name     CallbackGenerateSha256HkdfDeriveMd5
   * @tc.desc     Callback generate key digest sha256 hkdf derive key digest md5.
   */
  it('HuksTest_JSApiTest_DeriveKey_00900', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_00900 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_MD5);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('data' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      console.log('HuksTest_JSApiTest_DeriveKey_00900 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01000
   * @tc.name     CallbackGenerateSha256HkdfDeriveSha1
   * @tc.desc     Callback generate key digest sha256 hkdf derive key digest sha1.
   */
  it('HuksTest_JSApiTest_DeriveKey_01000', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01000 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('data' + JSON.stringify(data));
      expect(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT).assertEqual(data.errorCode);
      console.log('HuksTest_JSApiTest_DeriveKey_01000 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01100
   * @tc.name     CallbackGenerateSha256HkdfDeriveSha224
   * @tc.desc     Callback generate key digest sha256 hkdf derive key digest sha224.
   */
  it('HuksTest_JSApiTest_DeriveKey_01100', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01100 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA224);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('data' + JSON.stringify(data));
      expect(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT).assertEqual(data.errorCode);
      console.log('HuksTest_JSApiTest_DeriveKey_01100 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01200
   * @tc.name     CallbackGenerateSha256HkdfDeriveSha256
   * @tc.desc     Callback generate key digest sha256 hkdf derive key digest sha256.
   */
  it('HuksTest_JSApiTest_DeriveKey_01200', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01200 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('data' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HuksTest_JSApiTest_DeriveKey_01200 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01300
   * @tc.name     CallbackGenerateSha384HkdfDeriveSha384
   * @tc.desc     Callback generate key digest sha384 hkdf derive key digest sha384.
   */
  it('HuksTest_JSApiTest_DeriveKey_01300', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01300 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA384);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA384);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('data' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HuksTest_JSApiTest_DeriveKey_01300 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01400
   * @tc.name     CallbackGenerateSha512HkdfDeriveSha512
   * @tc.desc     Callback generate key digest sha512 hkdf derive key digest sha512.
   */
  it('HuksTest_JSApiTest_DeriveKey_01400', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01400 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA512);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makeHKDFDeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA512);
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('data' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HuksTest_JSApiTest_DeriveKey_01400 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01500
   * @tc.name     PromiseGenerateSha256Pbkdf2DeriveSha256
   * @tc.desc     Promise generate key digest sha256 pbkdf2 derive key digest sha256.
   */
  it('HuksTest_JSApiTest_DeriveKey_01500', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01500 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 1000, new Uint8Array(16));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(deriveKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deleteKeyResult = await huks.deleteKey('alias1', emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_01500 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01600
   * @tc.name     PromiseGenerateSha256Pbkdf2DeriveSha384
   * @tc.desc     Promise generate key digest sha256 pbkdf2 derive key digest sha384.
   */
  it('HuksTest_JSApiTest_DeriveKey_01600', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01600 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 1000, new Uint8Array(16));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT).assertEqual(deriveKeyResult.errorCode);
    var deleteKeyResult = await huks.deleteKey('alias1', emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_01600 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01700
   * @tc.name     PromiseGenerateSha256Pbkdf2DeriveSha512
   * @tc.desc     Promise generate key digest sha256 pbkdf2 derive key digest sha512.
   */
  it('HuksTest_JSApiTest_DeriveKey_01700', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01700 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 1000, new Uint8Array(16));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(deriveKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    var deleteKeyResult = await huks.deleteKey('alias1', emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_01700 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01800
   * @tc.name     PromisePbkdf2DeriveIterationAbnormal
   * @tc.desc     Promise pbkdf2 derive key iteration abnormal.
   */
  it('HuksTest_JSApiTest_DeriveKey_01800', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01800 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 999, new Uint8Array(16));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(deriveKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ITERATION);
    var deleteKeyResult = await huks.deleteKey('alias1', emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_01800 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_01900
   * @tc.name     PromisePbkdf2DeriveSaltAbnormal
   * @tc.desc     Promise pbkdf2 derive key salt abnormal.
   */
  it('HuksTest_JSApiTest_DeriveKey_01900', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_01900 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 1000, new Uint8Array(15));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    var deriveKeyResult = await huks.deriveKey('alias1', deriveKeyOption);
    console.log('deriveKeyResult' + JSON.stringify(deriveKeyResult));
    expect(deriveKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_SALT);
    var deleteKeyResult = await huks.deleteKey('alias1', emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist('alias1', emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    console.log('HuksTest_JSApiTest_DeriveKey_01900 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_02000
   * @tc.name     CallbackGenerateSha256Pbkdf2DeriveSha256
   * @tc.desc     Callback generate key digest sha256 pbkdf2 derive key digest sha256.
   */
  it('HuksTest_JSApiTest_DeriveKey_02000', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_02000 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 1000, new Uint8Array(16));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('deriveKeyResult' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HuksTest_JSApiTest_DeriveKey_02000 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_02100
   * @tc.name     CallbackGenerateSha256Pbkdf2DeriveSha384
   * @tc.desc     Callback generate key digest sha256 pbkdf2 derive key digest sha384.
   */
  it('HuksTest_JSApiTest_DeriveKey_02100', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_02100 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 1000, new Uint8Array(16));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('deriveKeyResult' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HuksTest_JSApiTest_DeriveKey_02000 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_02200
   * @tc.name     CallbackGenerateSha256Pbkdf2DeriveSha512
   * @tc.desc     Callback generate key digest sha256 pbkdf2 derive key digest sha512.
   */
  it('HuksTest_JSApiTest_DeriveKey_02200', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_02200 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 1000, new Uint8Array(16));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('deriveKeyResult' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HuksTest_JSApiTest_DeriveKey_02000 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_02300
   * @tc.name     CallbackPbkdf2DeriveIterationAbnormal
   * @tc.desc     Callback pbkdf2 derive key iteration abnormal.
   */
  it('HuksTest_JSApiTest_DeriveKey_02300', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_02300 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 999, new Uint8Array(16));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('deriveKeyResult' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ITERATION);
      console.log('HuksTest_JSApiTest_DeriveKey_02300 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_DeriveKey_02400
   * @tc.name     CallbackPbkdf2DeriveSaltAbnormal
   * @tc.desc     Callback pbkdf2 derive key salt abnormal.
   */
  it('HuksTest_JSApiTest_DeriveKey_02400', 0, async function (done) {
    console.log('HuksTest_JSApiTest_DeriveKey_02400 start');
    var generateKeyOption = makeGenerateKeyOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256);
    console.log('generateKeyOption' + JSON.stringify(generateKeyOption));
    var generateKeyResult = await huks.generateKey('alias1', generateKeyOption);
    console.log('generateKeyResult' + JSON.stringify(generateKeyResult));
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deriveKeyOption = makePBKDF2DeriveOption(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 1000, new Uint8Array(15));
    console.log('deriveKeyOption' + JSON.stringify(deriveKeyOption));
    huks.deriveKey('alias1', deriveKeyOption, function (err, data) {
      console.log('deriveKeyResult' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_SALT);
      console.log('HuksTest_JSApiTest_DeriveKey_02000 end');
      deleteKey(done);
    });
    setTimeout(function () {
    }, timer);
  });
});