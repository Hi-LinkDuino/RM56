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
  emptyOption,
  timer
} from '../../../../../../hks_xts_common.test.js'

describe('HuksTest_JSApiTest_GetKeyProperties', function () {
  var dsaKeySize = 1024;
  var defaultAlias = 'defaultAlias';
  var signAndVerify = huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY;
  var encryptAndDecrypt = huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT;
  var aes128Option = makeGenerateAESKeyOptions(huks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
  var rsa512Option = makeGenerateRSAKeyOptions(huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
  var ecc224Option = makeGenerateECCKeyOptions(huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
  var hmacSHA1Option = makeGenerateMACKeyOptions(huks.HuksKeyDigest.HUKS_DIGEST_SHA1, 160);
  var dsaOption = makeGenerateDSAKeyOptions();

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

  function isKeyExistFun(done) {
    huks.isKeyExist(defaultAlias, emptyOption, function (err, data) {
      expect(data).assertEqual(false);
      done();
    });
  };

  function deleteKeyFun(done) {
    huks.deleteKey(defaultAlias, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      isKeyExistFun(done);
    });
  };

  /**
   * @tc.number   HuksTest_JSApiTest_getKeyProperties_00100
   * @tc.name     JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_00100', 0, async function (done) {
    var aes128Result = await huks.generateKey(defaultAlias, aes128Option);
    expect(aes128Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var getKeyPropertiesRet = await huks.getKeyProperties(defaultAlias, emptyOption);
    expect(getKeyPropertiesRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    console.log('getKeyPropertiesRet ' + JSON.stringify(getKeyPropertiesRet));
    expect(getKeyPropertiesRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    expect(getKeyPropertiesRet.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_AES);
    expect(getKeyPropertiesRet.properties[1].value).assertEqual(huks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
    expect(getKeyPropertiesRet.properties[2].value).assertEqual(encryptAndDecrypt);
    expect(getKeyPropertiesRet.properties[3].value).assertEqual(huks.HuksKeyPadding.HUKS_PADDING_NONE);
    expect(getKeyPropertiesRet.properties[4].value).assertEqual(huks.HuksCipherMode.HUKS_MODE_CBC);
    var deleteKeyResult = await huks.deleteKey(defaultAlias, emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist(defaultAlias, emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_getKeyProperties_00200
   * @tc.name     JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_00200', 0, async function (done) {
    var aes128Result = await huks.generateKey(defaultAlias, aes128Option);
    expect(aes128Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    huks.getKeyProperties(defaultAlias, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('getKeyPropertiesRet ' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      expect(data.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_AES);
      expect(data.properties[1].value).assertEqual(huks.HuksKeySize.HUKS_AES_KEY_SIZE_128);
      expect(data.properties[2].value).assertEqual(encryptAndDecrypt);
      expect(data.properties[3].value).assertEqual(huks.HuksKeyPadding.HUKS_PADDING_NONE);
      expect(data.properties[4].value).assertEqual(huks.HuksCipherMode.HUKS_MODE_CBC);
      deleteKeyFun(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_getKeyProperties_00300
   * @tc.name     JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_00300', 0, async function (done) {
    var rsa512Result = await huks.generateKey(defaultAlias, rsa512Option);
    expect(rsa512Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var getKeyPropertiesRet = await huks.getKeyProperties(defaultAlias, emptyOption);
    expect(getKeyPropertiesRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    console.log('getKeyPropertiesRet ' + JSON.stringify(getKeyPropertiesRet));
    expect(getKeyPropertiesRet.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_RSA);
    expect(getKeyPropertiesRet.properties[1].value).assertEqual(huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);
    expect(getKeyPropertiesRet.properties[2].value).assertEqual(encryptAndDecrypt);
    expect(getKeyPropertiesRet.properties[3].value).assertEqual(huks.HuksKeyPadding.HUKS_PADDING_NONE);
    expect(getKeyPropertiesRet.properties[4].value).assertEqual(huks.HuksCipherMode.HUKS_MODE_ECB);
    var deleteKeyResult = await huks.deleteKey(defaultAlias, emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist(defaultAlias, emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_getKeyProperties_00400
   * @tc.name     JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_00400', 0, async function (done) {
    var rsa512Result = await huks.generateKey(defaultAlias, rsa512Option);
    expect(rsa512Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    huks.getKeyProperties(defaultAlias, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      console.log('getKeyPropertiesRet ' + JSON.stringify(data));
      expect(data.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_RSA);
      expect(data.properties[1].value).assertEqual(huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512);

      expect(data.properties[2].value).assertEqual(encryptAndDecrypt);
      expect(data.properties[3].value).assertEqual(huks.HuksKeyPadding.HUKS_PADDING_NONE);
      expect(data.properties[4].value).assertEqual(huks.HuksCipherMode.HUKS_MODE_ECB);
      deleteKeyFun(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_getKeyProperties_00500
   * @tc.name     JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_00500', 0, async function (done) {
    var ecc224Result = await huks.generateKey(defaultAlias, ecc224Option);
    expect(ecc224Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var getKeyPropertiesRet = await huks.getKeyProperties(defaultAlias, emptyOption);
    expect(getKeyPropertiesRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    expect(getKeyPropertiesRet.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_ECC);
    expect(getKeyPropertiesRet.properties[1].value).assertEqual(huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
    expect(getKeyPropertiesRet.properties[2].value).assertEqual(signAndVerify);
    expect(getKeyPropertiesRet.properties[3].value).assertEqual(huks.HuksKeyDigest.HUKS_DIGEST_NONE);
    console.log('getKeyPropertiesRet ' + JSON.stringify(getKeyPropertiesRet));
    var deleteKeyResult = await huks.deleteKey(defaultAlias, emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist(defaultAlias, emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_getKeyProperties_00600
   * @tc.name     JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_00600', 0, async function (done) {
    var ecc224Result = await huks.generateKey(defaultAlias, ecc224Option);
    expect(ecc224Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    huks.getKeyProperties(defaultAlias, emptyOption, function (err, data) {
      console.log('getKeyPropertiesRet ' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      expect(data.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_ECC);
      expect(data.properties[1].value).assertEqual(huks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
      expect(data.properties[2].value).assertEqual(signAndVerify);
      expect(data.properties[3].value).assertEqual(huks.HuksKeyDigest.HUKS_DIGEST_NONE);
      deleteKeyFun(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_getKeyProperties_00700
   * @tc.name     JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_00700', 0, async function (done) {
    var hmacSHA1Result = await huks.generateKey(defaultAlias, hmacSHA1Option);
    expect(hmacSHA1Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var getKeyPropertiesRet = await huks.getKeyProperties(defaultAlias, emptyOption);
    expect(getKeyPropertiesRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    expect(getKeyPropertiesRet.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_HMAC);
    expect(getKeyPropertiesRet.properties[1].value).assertEqual(huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
    expect(getKeyPropertiesRet.properties[2].value).assertEqual(huks.HuksKeyDigest.HUKS_DIGEST_SHA1);
    expect(getKeyPropertiesRet.properties[3].value).assertEqual(160);
    console.log('getKeyPropertiesRet ' + JSON.stringify(getKeyPropertiesRet));
    var deleteKeyResult = await huks.deleteKey(defaultAlias, emptyOption);
    expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
    var isKeyExistResult = await huks.isKeyExist(defaultAlias, emptyOption);
    expect(isKeyExistResult).assertEqual(false);
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_getKeyProperties_00800
   * @tc.name     JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_00800', 0, async function (done) {
    var hmacSHA1Result = await huks.generateKey(defaultAlias, hmacSHA1Option);
    expect(hmacSHA1Result.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    huks.getKeyProperties(defaultAlias, emptyOption, function (err, data) {
      console.log('getKeyPropertiesRet ' + JSON.stringify(data));
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      expect(data.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_HMAC);
      expect(data.properties[1].value).assertEqual(huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC);
      expect(data.properties[2].value).assertEqual(huks.HuksKeyDigest.HUKS_DIGEST_SHA1);
      expect(data.properties[3].value).assertEqual(160);
      deleteKeyFun(done);
    });
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number    HuksTest_JSApiTest_getKeyProperties_00900
   * @tc.name      JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_00900', 0, async function (done) {
    var dsaResult = await huks.generateKey(defaultAlias, dsaOption);
    if (useLib == 'mbedtls') {
      expect(dsaResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
    } else {
      expect(dsaResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      var getKeyPropertiesRet = await huks.getKeyProperties(defaultAlias, emptyOption);
      expect(getKeyPropertiesRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      expect(getKeyPropertiesRet.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_DSA);

      expect(getKeyPropertiesRet.properties[1].value).assertEqual(signAndVerify);
      expect(getKeyPropertiesRet.properties[2].value).assertEqual(dsaKeySize);
      expect(getKeyPropertiesRet.properties[3].value).assertEqual(huks.HuksKeyDigest.HUKS_DIGEST_SHA1);
      console.log('getKeyPropertiesRet ' + JSON.stringify(getKeyPropertiesRet));
      var deleteKeyResult = await huks.deleteKey(defaultAlias, emptyOption);
      expect(huks.HuksErrorCode.HUKS_SUCCESS).assertEqual(deleteKeyResult.errorCode);
      var isKeyExistResult = await huks.isKeyExist(defaultAlias, emptyOption);
      expect(isKeyExistResult).assertEqual(false);
    }
    done();
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HuksTest_JSApiTest_getKeyProperties_01000
   * @tc.name     JSAPI_Test_getKeyProperties
   * @tc.desc     Test for get key properties.
   */
  it('HuksTest_JSApiTest_getKeyProperties_01000', 0, async function (done) {
    var dsaResult = await huks.generateKey(defaultAlias, dsaOption);
    if (useLib == 'mbedtls') {
      expect(dsaResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
      done();
    } else {
      expect(dsaResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      huks.getKeyProperties(defaultAlias, emptyOption, function (err, data) {
        console.log('getKeyPropertiesRet ' + JSON.stringify(data));
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
        expect(data.properties[0].value).assertEqual(huks.HuksKeyAlg.HUKS_ALG_DSA);
        expect(data.properties[1].value).assertEqual(signAndVerify);
        expect(data.properties[2].value).assertEqual(dsaKeySize);
        expect(data.properties[3].value).assertEqual(huks.HuksKeyDigest.HUKS_DIGEST_SHA1);
        deleteKeyFun(done);
      });
    }
    setTimeout(function () {
    }, timer);
  });
});