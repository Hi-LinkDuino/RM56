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
  useLib,
  timer,
  makeGenerateKeyOption,
} from '../../../../../../hks_xts_common.test.js'

var defaultAliasA = 'defaultAliasA';
var defaultAliasB = 'defaultAliasB';
var publicKeyA = [];
var publicKeyB = [];
var keyA = [];
var keyB = [];

function makeAgreeOptions(alg, size, publicKey) {
  var properties = new Array();
  properties[0] = {
    tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
    value: alg
  };
  properties[1] = {
    tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
    value: size
  };
  var options = {
    properties: properties,
    inData: publicKey
  };
  return options;
};

describe('Hks_XtsTest_AlgCompletion_Agree', function () {

  /**
   * @tc.number    HUKS_ALG_COMPLETION_37300
   * @tc.name      DH2048_DH_Promise
   * @tc.desc      Test for algorithm completion(agree with DH).
   */
  it('HUKS_ALG_COMPLETION_37300', 0, async function (done) {
    agreePromise(
      huks.HuksKeyAlg.HUKS_ALG_DH,
      huks.HuksKeySize.HUKS_DH_KEY_SIZE_2048,
      done,
      'HUKS_ALG_COMPLETION_37300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_37400
   * @tc.name      DH3072_DH_Promise
   * @tc.desc      Test for algorithm completion(agree with DH).
   */
  it('HUKS_ALG_COMPLETION_37400', 0, async function (done) {
    agreePromise(
      huks.HuksKeyAlg.HUKS_ALG_DH,
      huks.HuksKeySize.HUKS_DH_KEY_SIZE_3072,
      done,
      'HUKS_ALG_COMPLETION_37400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_37500
   * @tc.name      DH4096_DH_Promise
   * @tc.desc      Test for algorithm completion(agree with DH).
   */
  it('HUKS_ALG_COMPLETION_37500', 0, async function (done) {
    agreePromise(
      huks.HuksKeyAlg.HUKS_ALG_DH,
      huks.HuksKeySize.HUKS_DH_KEY_SIZE_4096,
      done,
      'HUKS_ALG_COMPLETION_37500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_37600
   * @tc.name      DH2048_DH_AsyncCallback
   * @tc.desc      Test for algorithm completion(agree with DH).
   */
  it('HUKS_ALG_COMPLETION_37600', 0, async function (done) {
    agreeCallback(
      huks.HuksKeyAlg.HUKS_ALG_DH,
      huks.HuksKeySize.HUKS_DH_KEY_SIZE_2048,
      done,
      'HUKS_ALG_COMPLETION_37600'
    );
    setTimeout(function () {
    }, timer);
  });
  
  /**
   * @tc.number    HUKS_ALG_COMPLETION_37700
   * @tc.name      DH3072_DH_AsyncCallback
   * @tc.desc      Test for algorithm completion(agree with DH).
   */
  it('HUKS_ALG_COMPLETION_37700', 0, async function (done) {
    agreeCallback(
      huks.HuksKeyAlg.HUKS_ALG_DH,
      huks.HuksKeySize.HUKS_DH_KEY_SIZE_3072,
      done,
      'HUKS_ALG_COMPLETION_37700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_37800
   * @tc.name      DH4096_DH_AsyncCallback
   * @tc.desc      Test for algorithm completion(agree with DH).
   */
  it('HUKS_ALG_COMPLETION_37800', 0, async function (done) {
    agreeCallback(
      huks.HuksKeyAlg.HUKS_ALG_DH,
      huks.HuksKeySize.HUKS_DH_KEY_SIZE_4096,
      done,
      'HUKS_ALG_COMPLETION_37800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_37900
   * @tc.name      ECDH224_Promise
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_37900', 0, async function (done) {
    agreePromise(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      done,
      'HUKS_ALG_COMPLETION_37900'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38000
   * @tc.name      ECDH256_Promise
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_38000', 0, async function (done) {
    agreePromise(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      done,
      'HUKS_ALG_COMPLETION_38000'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38100
   * @tc.name      ECDH384_Promise
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_38100', 0, async function (done) {
    agreePromise(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      done,
      'HUKS_ALG_COMPLETION_38100'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38200
   * @tc.name      ECDH521_Promise
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_38200', 0, async function (done) {
    agreePromise(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      done,
      'HUKS_ALG_COMPLETION_38200'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38300
   * @tc.name      ECDH224_AsyncCallback
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_38300', 0, async function (done) {
    agreeCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      done,
      'HUKS_ALG_COMPLETION_38300'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38400
   * @tc.name      ECDH256_AsyncCallback
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_38400', 0, async function (done) {
    agreeCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
      done,
      'HUKS_ALG_COMPLETION_38400'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38500
   * @tc.name      ECDH384_AsyncCallback
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_38500', 0, async function (done) {
    agreeCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384,
      done,
      'HUKS_ALG_COMPLETION_38500'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38600
   * @tc.name      ECDH521_AsyncCallback
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_38600', 0, async function (done) {
    agreeCallback(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521,
      done,
      'HUKS_ALG_COMPLETION_38600'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38700
   * @tc.name      X25519_Promise
   * @tc.desc      Test for X25519 Regression Testing.
   */
  it('HUKS_ALG_COMPLETION_38700', 0, async function (done) {
    agreePromise(
      huks.HuksKeyAlg.HUKS_ALG_X25519,
      huks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256,
      done,
      'HUKS_ALG_COMPLETION_38700'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38800
   * @tc.name      X25519_Promise
   * @tc.desc      Test for X25519 Regression Testing.
   */
  it('HUKS_ALG_COMPLETION_38800', 0, async function (done) {
    agreeCallback(
      huks.HuksKeyAlg.HUKS_ALG_X25519,
      huks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256,
      done,
      'HUKS_ALG_COMPLETION_38800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_38900
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_38900', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      -1,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    var generateKeyRet = await huks.generateKey(defaultAliasA, generateKeyOptions);
    expect(generateKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_39000
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_39000', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      -1,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    huks.generateKey(defaultAliasA, generateKeyOptions, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_39100
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_39100', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      -1,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    var generateKeyRet = await huks.generateKey(defaultAliasA, generateKeyOptions);
    expect(generateKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_39200
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_39200', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      -1,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    huks.generateKey(defaultAliasA, generateKeyOptions, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_39300
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_39300', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      -1,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    var generateKeyRet = await huks.generateKey(defaultAliasA, generateKeyOptions);
    expect(generateKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_39400
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_39400', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      -1,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    huks.generateKey(defaultAliasA, generateKeyOptions, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_39500
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_39500', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      -1
    );
    var generateKeyRet = await huks.generateKey(defaultAliasA, generateKeyOptions);
    expect(generateKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_39600
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_39600', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      -1
    );
    huks.generateKey(defaultAliasA, generateKeyOptions, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_DIGEST);
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_39700
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_39700', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    var generateKeyARet = await huks.generateKey(defaultAliasA, generateKeyOptions);
    expect(generateKeyARet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var generateKeyBRet = await huks.generateKey(defaultAliasB, generateKeyOptions);
    expect(generateKeyBRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var exportKeyARet = await huks.exportKey(defaultAliasA, emptyOption);
    publicKeyA = exportKeyARet.outData;

    var exportKeyBRet = await huks.exportKey(defaultAliasB, emptyOption);
    publicKeyB = exportKeyBRet.outData;

    var agreeKeyAOption = makeAgreeOptions(
      -1,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      publicKeyB
    );
    var agreeKeyARet = await huks.agreeKey(defaultAliasB, agreeKeyAOption);
    expect(agreeKeyARet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);

    huks.agreeKey(defaultAliasA, agreeKeyAOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      done();
    });

    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_39800
   * @tc.name      AgreeWithError
   * @tc.desc      Test for algorithm completion(agree with ECDH).
   */
  it('HUKS_ALG_COMPLETION_39800', 0, async function (done) {
    var generateKeyOptions = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_ECC,
      huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    var generateKeyARet = await huks.generateKey(defaultAliasA, generateKeyOptions);
    expect(generateKeyARet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var generateKeyBRet = await huks.generateKey(defaultAliasB, generateKeyOptions);
    expect(generateKeyBRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyARet = await huks.exportKey(defaultAliasA, emptyOption);
    publicKeyA = exportKeyARet.outData;
    var exportKeyBRet = await huks.exportKey(defaultAliasB, emptyOption);
    publicKeyB = exportKeyBRet.outData;
    var agreeKeyAOption = makeAgreeOptions(
      huks.HuksKeyAlg.HUKS_ALG_ECDH,
      -1,
      publicKeyB
    );
    var agreeKeyARet = await huks.agreeKey(defaultAliasB, agreeKeyAOption);
    expect(agreeKeyARet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
    huks.agreeKey(defaultAliasA, agreeKeyAOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  function isKeyBExist(caseId, done) {
    huks.isKeyExist(defaultAliasB, emptyOption, function (err, data) {
      expect(data).assertEqual(false);
      console.info(caseId + ' end');
      done();
    });
  };

  function isKeyAExist(caseId, done) {
    huks.isKeyExist(defaultAliasA, emptyOption, function (err, data) {
      expect(data).assertEqual(false);
      isKeyBExist(caseId, done);
    });
  };

  function deleteKeyB(caseId, done) {
    huks.deleteKey(defaultAliasB, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      isKeyAExist(caseId, done);
    });
  };

  function deleteKeyA(caseId, done) {
    huks.deleteKey(defaultAliasA, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      deleteKeyB(caseId, done);
    });
  };

  function agreeKeyB(alg, size, done, caseId) {
    var agreeKeyAOption = makeAgreeOptions(alg, size, publicKeyB);
    huks.agreeKey(defaultAliasA, agreeKeyAOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      keyB = data.outData;
      expect(JSON.stringify(keyB)).assertEqual(JSON.stringify(keyA));
      deleteKeyA(caseId, done);
    });
  };

  function agreeKeyA(alg, size, done, caseId) {
    if (alg == huks.HuksKeyAlg.HUKS_ALG_ECC) {
      alg = huks.HuksKeyAlg.HUKS_ALG_ECDH
    }
    var agreeKeyAOption = makeAgreeOptions(alg, size, publicKeyB);
    huks.agreeKey(defaultAliasA, agreeKeyAOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      keyA = data.outData;
      agreeKeyB(alg, size, done, caseId);
    });
  };

  function exportKeyB(alg, size, done, caseId) {
    huks.exportKey(defaultAliasB, emptyOption, function (err, data) {
      if (useLib == 'openssl' && alg == huks.HuksKeyAlg.HUKS_ALG_X25519) {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
        done();
      } else {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
        publicKeyB = data.outData;
        agreeKeyA(alg, size, done, caseId);
      }
    });
  };

  function exportKeyA(alg, size, done, caseId) {
    huks.exportKey(defaultAliasA, emptyOption, function (err, data) {
      if (useLib == 'openssl' && alg == huks.HuksKeyAlg.HUKS_ALG_X25519) {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      } else {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
        publicKeyA = data.outData;
      }
      exportKeyB(alg, size, done, caseId);
    });
  };

  function generateKeyB(alg, size, done, caseId) {
    var generateKeyBOption = makeGenerateKeyOption(
      alg,
      size,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    huks.generateKey(defaultAliasB, generateKeyBOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      exportKeyA(alg, size, done, caseId);
    });
  };

  function generateKeyA(alg, size, done, caseId) {
    var generateKeyAOption = makeGenerateKeyOption(
      alg,
      size,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
      null,
      null,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    huks.generateKey(defaultAliasA, generateKeyAOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      generateKeyB(alg, size, done, caseId);
    });
  };

  function agreeCallback(alg, size, done, caseId) {
    generateKeyA(alg, size, done, caseId);
  };

  async function agreePromise(alg, size, done, caseId){
    var generateKeyAOption = makeGenerateKeyOption(
      alg, size, huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE, null, null, huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    var generateKeyARet = await huks.generateKey(defaultAliasA, generateKeyAOption);
    expect(generateKeyARet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var generateKeyBOption = makeGenerateKeyOption(
      alg, size, huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE, null, null, huks.HuksKeyDigest.HUKS_DIGEST_NONE
    );
    var generateKeyBRet = await huks.generateKey(defaultAliasB, generateKeyBOption);
    expect(generateKeyBRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var exportKeyARet = await huks.exportKey(defaultAliasA, emptyOption);
    var exportKeyBRet = await huks.exportKey(defaultAliasB, emptyOption);
    if (useLib == 'openssl' && alg == huks.HuksKeyAlg.HUKS_ALG_X25519) {
      expect(exportKeyARet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      expect(exportKeyBRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      done();
    } else {
      expect(exportKeyARet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      expect(exportKeyBRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      publicKeyA = exportKeyARet.outData;
      publicKeyB = exportKeyBRet.outData;
      if (alg == huks.HuksKeyAlg.HUKS_ALG_ECC) {
        alg = huks.HuksKeyAlg.HUKS_ALG_ECDH
      }
      var agreeKeyAOption = makeAgreeOptions(alg, size, publicKeyB);
      var agreeKeyARet = await huks.agreeKey(defaultAliasA, agreeKeyAOption);
      expect(agreeKeyARet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      var agreeKeyBOption = makeAgreeOptions(alg, size, publicKeyA);
      var agreeKeyBRet = await huks.agreeKey(defaultAliasB, agreeKeyBOption);
      expect(agreeKeyBRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      expect(JSON.stringify(agreeKeyARet.outData)).assertEqual(JSON.stringify(agreeKeyBRet.outData));
      var deleteKeyARet = await huks.deleteKey(defaultAliasA, emptyOption);
      expect(deleteKeyARet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      var deleteKeyBRet = await huks.deleteKey(defaultAliasB, emptyOption);
      expect(deleteKeyBRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      var isKeyAExist = await huks.isKeyExist(defaultAliasA, emptyOption);
      expect(isKeyAExist).assertEqual(false);
      var isKeyBExist = await huks.isKeyExist(defaultAliasA, emptyOption);
      expect(isKeyBExist).assertEqual(false);
      console.info(caseId + ' end');
      done();
    }
  };
});