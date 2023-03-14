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
  useLib,
  timer
} from '../../../../../../hks_xts_common.test.js'

describe('HuksTest_JSApiTest_GenerateKey', function () {

  var dsaKeySize = 1024;
  var defaultAlias = 'defaultAlias';

  var aes128Option = makeGenerateAESKeyOptions(huks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
  var aes192Option = makeGenerateAESKeyOptions(huks.HuksKeySize.HUKS_AES_KEY_SIZE_192);
  var aes256Option = makeGenerateAESKeyOptions(huks.HuksKeySize.HUKS_AES_KEY_SIZE_256);
  var rsa512Option = makeGenerateRSAKeyOptions(huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
  var rsa768Option = makeGenerateRSAKeyOptions(huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768);
  var rsa1024Option = makeGenerateRSAKeyOptions(huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024);
  var rsa2048Option = makeGenerateRSAKeyOptions(huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048);
  var rsa3072Option = makeGenerateRSAKeyOptions(huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072);
  var rsa4096Option = makeGenerateRSAKeyOptions(huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096);
  var ecc224Option = makeGenerateECCKeyOptions(huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
  var ecc256Option = makeGenerateECCKeyOptions(huks.HuksKeySize.HUKS_ECC_KEY_SIZE_256);
  var ecc384Option = makeGenerateECCKeyOptions(huks.HuksKeySize.HUKS_ECC_KEY_SIZE_384);
  var ecc521Option = makeGenerateECCKeyOptions(huks.HuksKeySize.HUKS_ECC_KEY_SIZE_521);
  var dsaOption = makeGenerateDSAKeyOptions();

  function makeWrongOption(alg, size, purpose, padding, mode) {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: alg
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
      value: size
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: purpose
    };
    properties[3] = {
      tag: huks.HuksTag.HUKS_TAG_PADDING,
      value: padding
    };
    properties[4] = {
      tag: huks.HuksTag.HUKS_TAG_BLOCK_MODE,
      value: mode
    };
    var options = {
      properties: properties
    };
    return options;
  }

  function makeGenerateAESKeyOptions(size) {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: huks.HuksKeyAlg.HUKS_ALG_AES
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
      value: size
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT
    };
    properties[3] = {
      tag: huks.HuksTag.HUKS_TAG_PADDING,
      value: huks.HuksKeyPadding.HUKS_PADDING_NONE
    };
    properties[4] = {
      tag: huks.HuksTag.HUKS_TAG_BLOCK_MODE,
      value: huks.HuksCipherMode.HUKS_MODE_CBC
    };
    var options = {
      properties: properties
    };
    return options;
  };

  function makeGenerateRSAKeyOptions(size) {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: huks.HuksKeyAlg.HUKS_ALG_RSA
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
      value: size
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT
    };
    properties[3] = {
      tag: huks.HuksTag.HUKS_TAG_PADDING,
      value: huks.HuksKeyPadding.HUKS_PADDING_NONE
    };
    properties[4] = {
      tag: huks.HuksTag.HUKS_TAG_BLOCK_MODE,
      value: huks.HuksCipherMode.HUKS_MODE_ECB
    };
    properties[5] = {
      tag: huks.HuksTag.HUKS_TAG_DIGEST,
      value: huks.HuksKeyDigest.HUKS_DIGEST_NONE
    };
    properties[6] = {
      tag: huks.HuksTag.HUKS_TAG_KEY_GENERATE_TYPE,
      value: huks.HuksKeyGenerateType.HUKS_KEY_GENERATE_TYPE_DEFAULT
    };
    var options = {
      properties: properties
    };
    return options;
  };

  function makeGenerateECCKeyOptions(size) {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: huks.HuksKeyAlg.HUKS_ALG_ECC
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
      value: size
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY
    };
    properties[3] = {
      tag: huks.HuksTag.HUKS_TAG_DIGEST,
      value: huks.HuksKeyDigest.HUKS_DIGEST_NONE
    };
    var options = {
      properties: properties
    };
    return options;
  };

  function makeGenerateMACKeyOptions(digest, size) {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: huks.HuksKeyAlg.HUKS_ALG_HMAC
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_DIGEST,
      value: digest
    };
    properties[3] = {
      tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
      value: size
    };
    var options = {
      properties: properties
    };
    return options;
  };

  function makeGenerateDSAKeyOptions() {
    var properties = new Array();
    properties[0] = {
      tag: huks.HuksTag.HUKS_TAG_ALGORITHM,
      value: huks.HuksKeyAlg.HUKS_ALG_DSA
    };
    properties[1] = {
      tag: huks.HuksTag.HUKS_TAG_PURPOSE,
      value: huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY
    };
    properties[2] = {
      tag: huks.HuksTag.HUKS_TAG_KEY_SIZE,
      value: dsaKeySize
    };
    properties[3] = {
      tag: huks.HuksTag.HUKS_TAG_DIGEST,
      value: huks.HuksKeyDigest.HUKS_DIGEST_SHA1
    };
    var options = {
      properties: properties
    };
    return options;
  };

  function makeEmptyOptions() {
    var emptyOptions = {
      properties: []
    };
    return emptyOptions;
  }

  function deleteKeyFun(alias, done) {
    var option = makeEmptyOptions();
    huks.deleteKey(alias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      done();
    });
  };

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_00100
   * @tc.name     JSAPI_GenerateAESKey_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_00100', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_00100 start');
    var aes128Result = await huks.generateKey(defaultAlias, aes128Option);
    expect(aes128Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var aes192Result = await huks.generateKey(defaultAlias, aes192Option);
    expect(aes192Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var aes256Result = await huks.generateKey(defaultAlias, aes256Option);
    expect(aes256Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var deleteKeyOption = makeEmptyOptions();
    var deleteKeyResult = await huks.deleteKey(defaultAlias, deleteKeyOption);
    expect(deleteKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    console.log('HUKS_JSAPI_GenerateKey_Test_00100 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_00200
   * @tc.name     JSAPI_GenerateAES128Key_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_00200', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_00200 start')
    huks.generateKey(defaultAlias, aes128Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_00300
   * @tc.name     JSAPI_GenerateAES192Key_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_00300', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_00300 start');
    huks.generateKey(defaultAlias, aes192Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_00300 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_00400
   * @tc.name     JSAPI_GenerateAES256Key_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_00400', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_00400 start');
    huks.generateKey(defaultAlias, aes256Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_00400 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_00500
   * @tc.name     JSAPI_GenerateRSAKey_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_00500', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_00500 start');
    var rsa512Result = await huks.generateKey(defaultAlias, rsa512Option);
    expect(rsa512Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var rsa768Result = await huks.generateKey(defaultAlias, rsa768Option);
    expect(rsa768Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var rsa1024Result = await huks.generateKey(defaultAlias, rsa1024Option);
    expect(rsa1024Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var rsa2048Result = await huks.generateKey(defaultAlias, rsa2048Option);
    expect(rsa2048Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var rsa3072Result = await huks.generateKey(defaultAlias, rsa3072Option);
    expect(rsa3072Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var rsa4096Result = await huks.generateKey(defaultAlias, rsa4096Option);
    expect(rsa4096Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    console.log('HUKS_JSAPI_GenerateKey_Test_00500 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_00600
   * @tc.name     JSAPI_GenerateRSA512Key_callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_00600', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_00600 start');
    huks.generateKey(defaultAlias, rsa512Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_00600 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_00700
   * @tc.name     JSAPI_GenerateRSA768Key_callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_00700', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_00700 start');
    huks.generateKey(defaultAlias, rsa768Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_00700 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_00800
   * @tc.name     JSAPI_GenerateRSA1024Key_callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_00800', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_00800 start');
    huks.generateKey(defaultAlias, rsa1024Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_00800 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_00900
   * @tc.name     JSAPI_GenerateRSA2048Key_callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_00900', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_00900 start');
    huks.generateKey(defaultAlias, rsa2048Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_00900 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01000
   * @tc.name     JSAPI_GenerateRSA3072Key_callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01000', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01000 start');
    huks.generateKey(defaultAlias, rsa3072Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_01000 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01100
   * @tc.name     JSAPI_GenerateRSA4096Key_callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01100', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01100 start');
    huks.generateKey(defaultAlias, rsa4096Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_01100 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01200
   * @tc.name     JSAPI_GenerateECCKey_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01200', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01200 start');
    var ecc224Result = await huks.generateKey(defaultAlias, ecc224Option);
    expect(ecc224Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var ecc256Result = await huks.generateKey(defaultAlias, ecc256Option);
    expect(ecc256Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var ecc384Result = await huks.generateKey(defaultAlias, ecc384Option);
    expect(ecc384Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var ecc521Result = await huks.generateKey(defaultAlias, ecc521Option);
    expect(ecc521Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    console.log('HUKS_JSAPI_GenerateKey_Test_01200 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01300
   * @tc.name     JSAPI_GenerateECC224Key_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01300', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01300 start');
    huks.generateKey(defaultAlias, ecc224Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_01300 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01400
   * @tc.name     JSAPI_GenerateECC256Key_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01400', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01400 start');
    huks.generateKey(defaultAlias, ecc256Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_01400 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01500
   * @tc.name     JSAPI_GenerateECC384Key_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01500', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01500 start');
    huks.generateKey(defaultAlias, ecc384Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_01500 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01600
   * @tc.name     JSAPI_GenerateECC521Key_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01600', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01600 start');
    huks.generateKey(defaultAlias, ecc521Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_01600 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01700
   * @tc.name     JSAPI_GenerateHmacKey_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01700', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01700 start');
    var hmacSHA1Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 160);
    var hmacSHA224Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 224);
    var hmacSHA256Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 256);
    var hmacSHA384Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 384);
    var hmacSHA512Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 512);
    var hmacSHA1Result = await huks.generateKey(defaultAlias, hmacSHA1Option);
    expect(hmacSHA1Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var hmacSHA224Result = await huks.generateKey(defaultAlias, hmacSHA224Option);
    expect(hmacSHA224Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var hmacSHA256Result = await huks.generateKey(defaultAlias, hmacSHA256Option);
    expect(hmacSHA256Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var hmacSHA384Result = await huks.generateKey(defaultAlias, hmacSHA384Option);
    expect(hmacSHA384Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var hmacSHA512Result = await huks.generateKey(defaultAlias, hmacSHA512Option);
    expect(hmacSHA512Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    console.log('HUKS_JSAPI_GenerateKey_Test_01700 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01800
   * @tc.name     JSAPI_GenerateHmacSHA1Key
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01800', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01800 start');
    var hmacSHA1Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 160);
    huks.generateKey(defaultAlias, hmacSHA1Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_01800 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_01900
   * @tc.name     JSAPI_GenerateHmacSHA224Key
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_01900', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_01900 start');
    var hmacSHA224Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 224);
    huks.generateKey(defaultAlias, hmacSHA224Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_01900 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02000
   * @tc.name     JSAPI_GenerateHmacSHA256Key
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02000', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02000 start');
    var hmacSHA256Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 256);
    huks.generateKey(defaultAlias, hmacSHA256Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_02000 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02100
   * @tc.name     JSAPI_GenerateHmacSHA384Key
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02100', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02100 start');
    var hmacSHA384Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 384);
    huks.generateKey(defaultAlias, hmacSHA384Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_02100 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02200
   * @tc.name     JSAPI_GenerateHmacSHA512Key
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02200', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02200 start');
    var hmacSHA512Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 512);
    huks.generateKey(defaultAlias, hmacSHA512Option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_02200 end');
      deleteKeyFun(defaultAlias, done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02300
   * @tc.name     JSAPI_GenerateDSAKey_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02300', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02300 start');
    var dsaResult = await huks.generateKey(defaultAlias, dsaOption);
    if (useLib == 'mbedtls') {
      expect(dsaResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
      done();
    } else {
      expect(dsaResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_02300 end');
      deleteKeyFun(defaultAlias, done);
    }
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02400
   * @tc.name     JSAPI_GenerateDSAKey_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02400', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02400 start');
    huks.generateKey(defaultAlias, dsaOption, function (err, data) {
      if (useLib == 'mbedtls') {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
        done();
      } else {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
        console.log('HUKS_JSAPI_GenerateKey_Test_02400 end');
        deleteKeyFun(defaultAlias, done);
      }
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02500
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongAlg_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02500', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02500 start');
    var notExistAlg = -1;
    console.log('HUKS_JSAPI_GenerateKey_Test_02500_01 start');
    var option = makeWrongOption(
      notExistAlg,
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC
    );

    var result = await huks.generateKey(defaultAlias, option);
    expect(result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);

    console.log('HUKS_JSAPI_GenerateKey_Test_02500 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02600
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongSize_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02600', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02600 start');
    var notExistSize = -1;
    var option = makeWrongOption(
      huks.HuksKeyAlg.HUKS_ALG_AES,
      notExistSize,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC
    );

    var result = await huks.generateKey(defaultAlias, option);
    expect(result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);

    console.log('HUKS_JSAPI_GenerateKey_Test_02600 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02700
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongPurpose_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02700', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02700 start');
    var notExistPurpose = -1;
    var option = makeWrongOption(
      huks.HuksKeyAlg.HUKS_ALG_AES,
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      notExistPurpose,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC
    );

    var result = await huks.generateKey(defaultAlias, option);
    expect(result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);

    console.log('HUKS_JSAPI_GenerateKey_Test_02700 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02800
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongPadding_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02800', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02800 start');
    var notExistPadding = -1;
    var option = makeWrongOption(
      huks.HuksKeyAlg.HUKS_ALG_AES,
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      notExistPadding,
      huks.HuksCipherMode.HUKS_MODE_CBC
    );

    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      console.log('HUKS_JSAPI_GenerateKey_Test_02800 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_02900
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongAlg_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_02900', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_02900 start');
    var notExistAlg = -1;
    var option = makeWrongOption(
      notExistAlg,
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC
    );

    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ALGORITHM);
      console.log('HUKS_JSAPI_GenerateKey_Test_02900 end');
      done();
    });

    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03000
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongSize_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03000', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03000 start');
    var notExistSize = -1;
    var option = makeWrongOption(
      huks.HuksKeyAlg.HUKS_ALG_AES,
      notExistSize,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC
    );

    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_SIZE);
      console.log('HUKS_JSAPI_GenerateKey_Test_03000 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03100
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongPurpose_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03100', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03100 start');
    var notExistPurpose = -1;
    var option = makeWrongOption(
      huks.HuksKeyAlg.HUKS_ALG_AES,
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      notExistPurpose,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_CBC
    );

    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_PURPOSE);
      console.log('HUKS_JSAPI_GenerateKey_Test_03100 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**C
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03200
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongPadding_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03200', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03200 start');
    var notExistPadding = -1;
    var option = makeWrongOption(
      huks.HuksKeyAlg.HUKS_ALG_AES,
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      notExistPadding,
      huks.HuksCipherMode.HUKS_MODE_CBC
    );

    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_PADDING);
      console.log('HUKS_JSAPI_GenerateKey_Test_03200 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03300
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03300', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03300 start');
    var hmacSHA1OptionSize8 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 8);
    var hmacSHA1ResultSize8 = await huks.generateKey(defaultAlias, hmacSHA1OptionSize8);
    expect(hmacSHA1ResultSize8.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var hmacSHA1OptionSize1024 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 1024);
    var hmacSHA1ResultSize1024 = await huks.generateKey(defaultAlias, hmacSHA1OptionSize1024);
    expect(hmacSHA1ResultSize1024.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var hmacSHA224OptionSize8 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 8);
    var hmacSHA224ResultSize8 = await huks.generateKey(defaultAlias, hmacSHA224OptionSize8);
    expect(hmacSHA224ResultSize8.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var hmacSHA224OptionSize1024 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 1024);
    var hmacSHA224ResultSize1024 = await huks.generateKey(defaultAlias, hmacSHA224OptionSize1024);
    expect(hmacSHA224ResultSize1024.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var hmacSHA256OptionSize8 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 8);
    var hmacSHA256ResultSize8 = await huks.generateKey(defaultAlias, hmacSHA256OptionSize8);
    expect(hmacSHA256ResultSize8.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var hmacSHA256OptionSize1024 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 1024);
    var hmacSHA256ResultSize1024 = await huks.generateKey(defaultAlias, hmacSHA256OptionSize1024);
    expect(hmacSHA256ResultSize1024.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var hmacSHA384OptionSize8 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 8);
    var hmacSHA384ResultSize8 = await huks.generateKey(defaultAlias, hmacSHA384OptionSize8);
    expect(hmacSHA384ResultSize8.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var hmacSHA384OptionSize1024 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 1024);
    var hmacSHA384ResultSize1024 = await huks.generateKey(defaultAlias, hmacSHA384OptionSize1024);
    expect(hmacSHA384ResultSize1024.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var hmacSHA512OptionSize8 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 8);
    var hmacSHA512ResultSize8 = await huks.generateKey(defaultAlias, hmacSHA512OptionSize8);
    expect(hmacSHA512ResultSize8.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    var hmacSHA512OptionSize1024 = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 1024);
    var hmacSHA512ResultSize1024 = await huks.generateKey(defaultAlias, hmacSHA512OptionSize1024);
    expect(hmacSHA512ResultSize1024.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);

    console.log('HUKS_JSAPI_GenerateKey_Test_03300 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03400
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03400', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03400 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 8);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_03400 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03500
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03500', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03500 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 1024);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_03500 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03600
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03600', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03600 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 8);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_03600 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03700
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03700', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03700 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 1024);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_03700 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03800
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03800', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03800 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 8);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_03800 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_03900
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_03900', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_03900 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 1024);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_03600 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04000
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04000', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04000 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 8);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_04000 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04100
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04100', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04100 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 1024);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_04100 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04200
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04200', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04200 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 8);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_04200 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04300
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04300', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04300 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 1024);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('HUKS_JSAPI_GenerateKey_Test_04300 end');
      deleteKeyFun(defaultAlias, done);
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04400
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_WrongSize_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04400', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04400 start');
    var hmacSHA1Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 7);
    var hmacSHA1Result = await huks.generateKey(defaultAlias, hmacSHA1Option);
    expect(hmacSHA1Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    hmacSHA1Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 1025);
    hmacSHA1Result = await huks.generateKey(defaultAlias, hmacSHA1Option);
    expect(hmacSHA1Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    var hmacSHA224Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 7);
    var hmacSHA224Result = await huks.generateKey(defaultAlias, hmacSHA224Option);
    expect(hmacSHA224Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    hmacSHA224Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 1025);
    hmacSHA224Result = await huks.generateKey(defaultAlias, hmacSHA224Option);
    expect(hmacSHA224Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    var hmacSHA256Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 7);
    var hmacSHA256Result = await huks.generateKey(defaultAlias, hmacSHA256Option);
    expect(hmacSHA256Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    hmacSHA256Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 1025);
    hmacSHA256Result = await huks.generateKey(defaultAlias, hmacSHA256Option);
    expect(hmacSHA256Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    var hmacSHA384Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 7);
    var hmacSHA384Result = await huks.generateKey(defaultAlias, hmacSHA384Option);
    expect(hmacSHA384Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    hmacSHA384Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 1025);
    hmacSHA384Result = await huks.generateKey(defaultAlias, hmacSHA384Option);
    expect(hmacSHA384Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    var hmacSHA512Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 7);
    var hmacSHA512Result = await huks.generateKey(defaultAlias, hmacSHA512Option);
    expect(hmacSHA512Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    hmacSHA512Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 1025);
    hmacSHA512Result = await huks.generateKey(defaultAlias, hmacSHA512Option);
    expect(hmacSHA512Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);

    console.log('HUKS_JSAPI_GenerateKey_Test_04400 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04500
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04500', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04500 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 7);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_04500 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04600
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04600', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04600 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 1025);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_04600 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04700
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04700', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04700 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 7);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_04700 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04800
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04800', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04800 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA224, 1025);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_04800 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_04900
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_04900', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_04900 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 7);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_04900 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_05000
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_05000', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_05000 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA256, 1025);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_05000 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_05100
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_05100', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_05100 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 7);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_05100 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_05200
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_05200', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_05200 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA384, 1025);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_05200 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_05300
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_05300', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_05300 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 7);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_05300 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_05400
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_Hmac_Margin_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_05400', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_05400 start');
    var option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA512, 1025);
    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_ARGUMENT);
      console.log('HUKS_JSAPI_GenerateKey_Test_05400 end');
      done();
    });
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_05500
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongMode_Promise
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_05500', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_05500 start');
    var notExistMode = -1;
    var option = makeWrongOption(
      huks.HuksKeyAlg.HUKS_ALG_AES,
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      notExistMode
    );

    var result = await huks.generateKey(defaultAlias, option);
    expect(result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_MODE);

    console.log('HUKS_JSAPI_GenerateKey_Test_05500 end');
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_JSAPI_GenerateKey_Test_05600
   * @tc.name     JSAPI_GenerateKey_Test_GenKey_WrongAlg_Callback
   * @tc.desc     Test for APP JSAPI_GenerateKey.
   */
  it('HUKS_JSAPI_GenerateKey_Test_05600', 0, async function (done) {
    console.log('HUKS_JSAPI_GenerateKey_Test_05600 start');
    var notExistMode = -1;
    var option = makeWrongOption(
      huks.HuksKeyAlg.HUKS_ALG_AES,
      huks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      notExistMode
    );

    huks.generateKey(defaultAlias, option, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_MODE);
      console.log('HUKS_JSAPI_GenerateKey_Test_05600 end');
      done();
    });
    setTimeout(function () {
    }, timer);
  });
});