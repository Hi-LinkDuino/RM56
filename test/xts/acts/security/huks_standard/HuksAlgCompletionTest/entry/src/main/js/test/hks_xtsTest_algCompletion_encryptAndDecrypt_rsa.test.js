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
  makeGenerateKeyOption,
  makeEncryptAndDecryptOption,
  makeRandomArr,
  emptyOption,
  makeImportOption,
  useLib,
  timer
} from '../../../../../../hks_xts_common.test.js'

describe('Hks_XtsTest_ALGCOMPLETION_RSA_ENCRYPT_DECRYPT', function () {
  var aliasForRSAEncrypt = 'aliasForRSAEncrypt';
  var aliasForRSADecrypt = 'aliasForRSADecrypt';
  var publicKey = [];
  var ciphertext = [];

  function makePlainTextSize(size,padding,digest){
    var plainTextSize = 0;
    if (padding == huks.HuksKeyPadding.HUKS_PADDING_OAEP) {
      if (digest == huks.HuksKeyDigest.HUKS_DIGEST_SHA224 && size == huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512) {
        plainTextSize = 4;
      } else {
        plainTextSize = 16;
      }
    } else if (padding == huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5) {
      plainTextSize = 8;
    } else {
      switch (size) {
        case huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512:
          plainTextSize = 64;
          break;
        case huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768:
          plainTextSize = 96;
          break;
        case huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024:
          plainTextSize = 128;
          break;
        case huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048:
          plainTextSize = 256;
          break;
        case huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072:
          plainTextSize = 384;
          break;
        case huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096:
          plainTextSize = 512;
          break;
        default:
          plainTextSize = 512;
      }
    }
    return plainTextSize;
  };

  async function encryptAndDecryptWithPromise (size, padding, mode, digest, done, caseId){
    var plainTextSize = makePlainTextSize(size, padding,digest);
    var plainText = makeRandomArr(plainTextSize);
    var genKeyOpt = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,size,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,padding,mode,digest
    );
    var genKeyRet = await huks.generateKey(aliasForRSADecrypt, genKeyOpt);
    expect(genKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var exportKeyRet = await huks.exportKey(aliasForRSADecrypt, emptyOption);
    expect(exportKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    publicKey = exportKeyRet.outData;
    var importKeyOpt = makeImportOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,size,huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,padding,mode,digest,publicKey
    );
    var importRet = await huks.importKey(aliasForRSAEncrypt, importKeyOpt);
    expect(importRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var isEncryptKeyExist = await huks.isKeyExist(aliasForRSAEncrypt, emptyOption);
    expect(isEncryptKeyExist).assertEqual(true);
    var isDecryptKeyExist = await huks.isKeyExist(aliasForRSADecrypt, emptyOption);
    expect(isDecryptKeyExist).assertEqual(true);
    var encryptOpt = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,padding,mode,size,digest,plainText
    );
    var encryptRet = await huks.encrypt(aliasForRSAEncrypt, encryptOpt);
    if (useLib == 'mbedtls'
      && mode == huks.HuksCipherMode.HUKS_MODE_ECB
      && padding == huks.HuksKeyPadding.HUKS_PADDING_NONE){
      expect(encryptRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
    } else {
      expect(encryptRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      ciphertext = encryptRet.outData;
      var decryptOpt = makeEncryptAndDecryptOption(
        huks.HuksKeyAlg.HUKS_ALG_RSA,huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,padding,mode,size,digest,ciphertext
      );
      var decryptRet = await huks.decrypt(aliasForRSADecrypt, decryptOpt);
      expect(decryptRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      expect(JSON.stringify(plainText)).assertEqual(JSON.stringify(decryptRet.outData));
      var deleteEncryptKeyRet = await huks.deleteKey(aliasForRSAEncrypt, emptyOption);
      expect(deleteEncryptKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      var deleteDecryptKeyRet = await huks.deleteKey(aliasForRSADecrypt, emptyOption);
      expect(deleteDecryptKeyRet.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      isEncryptKeyExist = await huks.isKeyExist(aliasForRSAEncrypt, emptyOption);
      expect(isEncryptKeyExist).assertEqual(false);
      isDecryptKeyExist = await huks.isKeyExist(aliasForRSADecrypt, emptyOption);
      expect(isDecryptKeyExist).assertEqual(false);
    }
    console.log(caseId + ' end');
    done();
  };

  function checkDecryptKeyDeletedWithCallback(done, caseId) {
    huks.isKeyExist(aliasForRSADecrypt, emptyOption, function (err, data) {
      expect(data).assertEqual(false);
      console.log(caseId + ' end');
      done();
    });
  };

  function checkEncryptKeyDeletedWithCallback(done, caseId) {
    huks.isKeyExist(aliasForRSAEncrypt, emptyOption, function (err, data) {
      expect(data).assertEqual(false);
      checkDecryptKeyDeletedWithCallback(done, caseId);
    });
  };

  function deleteDecryptKeyWithCallback(done, caseId) {
    huks.deleteKey(aliasForRSADecrypt, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      checkEncryptKeyDeletedWithCallback(done, caseId);
    });
  };

  function deleteEncryptKeyWithCallback(done, caseId) {
    huks.deleteKey(aliasForRSAEncrypt, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      deleteDecryptKeyWithCallback(done, caseId);
    });
  };

  function decryptWithCallback(size, padding, mode, digest, plainText, ciphertext, done, caseId) {
    var decryptOpt = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      padding,
      mode,
      size,
      digest,
      ciphertext
    );
    huks.decrypt(aliasForRSADecrypt, decryptOpt, function (err, data) {
      expect(JSON.stringify(plainText)).assertEqual(JSON.stringify(data.outData));
      deleteEncryptKeyWithCallback(done, caseId);
    });
  };

  function encryptWithCallback(size, padding, mode, digest, done, caseId) {
    var plainTextSize = makePlainTextSize(size, padding,digest);
    var plainText = makeRandomArr(plainTextSize);
    var encryptOpt = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      padding,
      mode,
      size,
      digest,
      plainText
    );
    huks.encrypt(aliasForRSAEncrypt, encryptOpt, function (err, data) {
      if (useLib == 'mbedtls'
        && mode == huks.HuksCipherMode.HUKS_MODE_ECB
        && padding == huks.HuksKeyPadding.HUKS_PADDING_NONE){
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_NOT_SUPPORTED);
        done();
      } else {
        expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
        decryptWithCallback(size, padding, mode, digest, plainText, data.outData, done, caseId);
      }
    });
  };

  function checkDecryptKeyExistWithCallback(size, padding, mode, digest, done, caseId) {
    huks.isKeyExist(aliasForRSADecrypt, emptyOption, function (err, data) {
      expect(data).assertEqual(true);
      encryptWithCallback(size, padding, mode, digest, done, caseId);
    });
  };

  function checkEncryptKeyExistWithCallback(size, padding, mode, digest, done, caseId) {
    huks.isKeyExist(aliasForRSAEncrypt, emptyOption, function (err, data) {
      expect(data).assertEqual(true);
      checkDecryptKeyExistWithCallback(size, padding, mode, digest, done, caseId);
    });
  };

  function importWithCallback(size, padding, mode, digest, done, caseId) {
    var importKeyOpt = makeImportOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      size,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      padding,
      mode,
      digest,
      publicKey
    );
    huks.importKey(aliasForRSAEncrypt, importKeyOpt, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      checkEncryptKeyExistWithCallback(size, padding, mode, digest, done, caseId);
    });
  };

  function exportWithCallback(size, padding, mode, digest, done, caseId) {
    huks.exportKey(aliasForRSADecrypt, emptyOption, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      publicKey = data.outData;
      importWithCallback(size, padding, mode, digest, done, caseId);
    });
  };

  function encryptAndDecryptWithCallback(size, padding, mode, digest, done, caseId) {
    var genKeyOpt = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      size,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      padding,
      mode,
      digest
    );
    huks.generateKey(aliasForRSADecrypt, genKeyOpt, function (err, data) {
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
      exportWithCallback(size, padding, mode, digest, done, caseId);
    });
  };

  /**
   * @tc.number   HUKS_ALG_COMPLETION_03700
   * @tc.name     RSA512_RSA_ECB_NoPadding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_03700', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_03700'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_03800
   * @tc.name     RSA512_RSA_ECB_NoPadding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_03800', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_03800'
    );
    setTimeout(function () {
    }, timer);
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_03900
   * @tc.name     RSA768_RSA_ECB_NoPadding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_03900', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_03900'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04000
   * @tc.name     RSA768_RSA_ECB_NoPadding_Callback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04000', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04000'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04100
   * @tc.name     RSA1024_RSA_ECB_NoPadding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04100', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04200
   * @tc.name     RSA1024_RSA_ECB_NoPadding_Callback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04200', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04200'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04300
   * @tc.name     RSA2048_RSA_ECB_NoPadding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04300', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04300'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04400
   * @tc.name     RSA2048_RSA_ECB_NoPadding_Callback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04400', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04400'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04500
   * @tc.name     RSA3072_RSA_ECB_NoPadding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04500', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04500'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04600
   * @tc.name     RSA3072_RSA_ECB_NoPadding_Callback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04600', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04600'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04700
   * @tc.name     RSA4096_RSA_ECB_NoPadding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04700', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04700'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04800
   * @tc.name     RSA4096_RSA_ECB_NoPadding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04800', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_NONE,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04800'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_04900
   * @tc.name     RSA512_RSA_ECB_PKCS1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_04900', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_04900'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05000
   * @tc.name     RSA512_RSA_ECB_PKCS1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05000', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05000'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05100
   * @tc.name     RSA768_RSA_ECB_PKCS1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05100', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05200
   * @tc.name     RSA768_RSA_ECB_PKCS1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05200', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05200'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05300
   * @tc.name     RSA1024_RSA_ECB_PKCS1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05300', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05300'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05400
   * @tc.name     RSA1024_RSA_ECB_PKCS1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05400', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05400'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05500
   * @tc.name     RSA2048_RSA_ECB_PKCS1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05500', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05500'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05600
   * @tc.name     RSA2048_RSA_ECB_PKCS1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05600', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05600'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05700
   * @tc.name     RSA3072_RSA_ECB_PKCS1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05700', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05700'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05800
   * @tc.name     RSA3072_RSA_ECB_PKCS1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05800', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05800'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_05900
   * @tc.name     RSA4096_RSA_ECB_PKCS1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_05900', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_05900'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_06000
   * @tc.name     RSA4096_RSA_ECB_PKCS1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06000', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06000'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_06100
   * @tc.name     RSA512_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_Promise
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06100', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_06200
   * @tc.name     RSA512_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06200', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06200'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_06300
   * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_Promise
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06300', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06300'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_06400
   * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06400', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06400'
    );
  });

  /**
   * @tc.number    HUKS_ALG_COMPLETION_06500
   * @tc.name      RSA1024_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_Promise
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06500', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06500'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_06600
   * @tc.name     RSA1024_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06600', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06600'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_06700
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06700', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06700'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_06800
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06800', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06800'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_06900
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_Promise
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_06900', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_06900'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07000
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07000', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_07000'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07100
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_Promise
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07100', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_07100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07200
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-1AndMGF1Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07200', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_NONE,
      done,
      'HUKS_ALG_COMPLETION_07200'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07300
   * @tc.name     RSA512_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07300', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_07300'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07400
   * @tc.name     RSA512_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07400', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_07400'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07500
   * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07500', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_07500'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07600
   * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07600', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_07600'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07700
   * @tc.name     RSA1024_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07700', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_07700'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07800
   * @tc.name     RSA1024_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07800', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_07800'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_07900
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_07900', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_07900'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_08000
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_08000', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_08000'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_08100
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_08100', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_08100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_08200
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_08200', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_08200'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_08300
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_Promise
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_08300', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_08300'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_08400
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-224AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_08400', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA224,
      done,
      'HUKS_ALG_COMPLETION_08400'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_08700
   * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_08700', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_08700'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_08800
   * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_08800', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_08800'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_08900
   * @tc.name     RSA1024_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_08900', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_08900'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_09000
   * @tc.name     RSA1024_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_09000', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_09000'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_09100
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_09100', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_09100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_09200
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_09200', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_09200'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_09300
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_09300', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_09300'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_09400
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_09400', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_09400'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_09500
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_09500', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_09500'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_09600
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-256AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_09600', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA256,
      done,
      'HUKS_ALG_COMPLETION_09600'
    );
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_09700
  * @tc.name     RSA512_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_Promise
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_09700', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      text
    );
    var encryptResult = await huks.encrypt('alias',encryptOption);
    expect(encryptResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
    done();
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_09800
  * @tc.name     RSA512_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_AsyncCallback
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_09800', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      text
    );
    huks.encrypt('alias',encryptOption,function(err,data){
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
      done();
    });
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_09900
  * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_Promise
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_09900', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      text
    );
    var encryptResult = await huks.encrypt('alias',encryptOption);
    expect(encryptResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
    done();
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_10000
  * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_AsyncCallback
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_10000', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      text
    );
    huks.encrypt('alias',encryptOption,function(err,data){
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_10100
   * @tc.name     RSA1024_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_10100', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_10100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_10200
   * @tc.name     RSA1024_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_10200', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_10200'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_10300
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_10300', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_10300'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_10400
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_10400', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_10400'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_10500
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_Promise
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_10500', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_10500'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_10600
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_10600', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_10600'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_10700
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_Promise
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_10700', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_10700'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_10800
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_10800', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      done,
      'HUKS_ALG_COMPLETION_10800'
    );
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_10900
  * @tc.name     RSA512_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_Promise
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_10900', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      text
    );
    var encryptResult = await huks.encrypt('alias',encryptOption);
    expect(encryptResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
    done();
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_11000
  * @tc.name     RSA512_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_AsyncCallback
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_11000', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      text
    );
    huks.encrypt('alias',encryptOption,function(err,data){
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
      done();
    });
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_11100
  * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_Promise
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_11100', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      text
    );
    var encryptResult = await huks.encrypt('alias',encryptOption);
    expect(encryptResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
    done();
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_11200
  * @tc.name     RSA768_RSA_ECB_OAEPWithSHA-384AndMGF1Padding_AsyncCallback
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_11200', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA384,
      text
    );
    huks.encrypt('alias',encryptOption,function(err,data){
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
      done();
    });
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_11300
  * @tc.name     RSA1024_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_Promise
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_11300', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      text
    );
    var encryptResult = await huks.encrypt('alias',encryptOption);
    expect(encryptResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
    done();
  });

 /**
  * @tc.number   HUKS_ALG_COMPLETION_11400
  * @tc.name     RSA1024_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_AsyncCallback
  * @tc.desc     Test for algorithm completion.
  */
  it('HUKS_ALG_COMPLETION_11400', 0, async function (done) {
    var generateKeyOption = makeGenerateKeyOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
    );
    var generateKeyResult = await huks.generateKey('alias',generateKeyOption);
    expect(generateKeyResult.errorCode).assertEqual(huks.HuksErrorCode.HUKS_SUCCESS);
    var text = makeRandomArr(256);
    var encryptOption = makeEncryptAndDecryptOption(
      huks.HuksKeyAlg.HUKS_ALG_RSA,
      huks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      text
    );
    huks.encrypt('alias',encryptOption,function(err,data){
      expect(data.errorCode).assertEqual(huks.HuksErrorCode.HUKS_ERROR_INVALID_KEY_FILE);
      done();
    });
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_11500
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_11500', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_11500'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_11600
   * @tc.name     RSA2048_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_11600', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_11600'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_11700
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_11700', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_11700'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_11800
   * @tc.name     RSA3072_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_11800', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_11800'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_11900
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_11900', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_11900'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12000
   * @tc.name     RSA4096_RSA_ECB_OAEPWithSHA-512AndMGF1Padding_AsyncCallback
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_12000', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA512,
      done,
      'HUKS_ALG_COMPLETION_12000'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12100
   * @tc.name     RSA512_RSA_ECB_OAEPPadding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_12100', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_12100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12200
   * @tc.name     RSA512_RSA_ECB_OAEPPadding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_12200', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_12200'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12300
   * @tc.name     RSA768_RSA_ECB_OAEPPadding_Promise
   * @tc.desc      Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_12300', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_12300'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12400
   * @tc.name     RSA768_RSA_ECB_OAEPPadding_AsyncCallback
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_12400', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_768,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_12400'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12500
   * @tc.name     RSA1024_RSA_ECB_OAEPPadding_Promise
   * @tc.desc     Test for algorithm completion.
   */
  it('HUKS_ALG_COMPLETION_12500', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_12500'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12600
   * @tc.name     RSA1024_RSA_ECB_OAEPPadding_AsyncCallback
   * @tc.desc     Test for algorithm completion
   */
  it('HUKS_ALG_COMPLETION_12600', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_12600'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12700
   * @tc.name     RSA2048_RSA_ECB_OAEPPadding_Promise
   * @tc.desc      Test for algorithm completion
   */
  it('HUKS_ALG_COMPLETION_12700', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_12700'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12800
   * @tc.name     RSA2048_RSA_ECB_OAEPPadding_AsyncCallback
   * @tc.desc     Test for algorithm completion
   */
  it('HUKS_ALG_COMPLETION_12800', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_12800'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_12900
   * @tc.name     RSA3072_RSA_ECB_OAEPPadding_Promise
   * @tc.desc     Test for algorithm completion
   */
  it('HUKS_ALG_COMPLETION_12900', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_12900'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13000
   * @tc.name     RSA3072_RSA_ECB_OAEPPadding_AsyncCallback
   * @tc.desc     Test for algorithm completion
   */
  it('HUKS_ALG_COMPLETION_13000', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_3072,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_13000'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13100
   * @tc.name     RSA4096_RSA_ECB_OAEPPadding_Promise
   * @tc.desc      Test for algorithm completion
   */
  it('HUKS_ALG_COMPLETION_13100', 0, async function (done) {
    encryptAndDecryptWithPromise(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_13100'
    );
  });

  /**
   * @tc.number   HUKS_ALG_COMPLETION_13200
   * @tc.name     RSA4096_RSA_ECB_OAEPPadding_AsyncCallback
   * @tc.desc      Test for algorithm completion
   */
  it('HUKS_ALG_COMPLETION_13200', 0, async function (done) {
    encryptAndDecryptWithCallback(
      huks.HuksKeySize.HUKS_RSA_KEY_SIZE_4096,
      huks.HuksKeyPadding.HUKS_PADDING_OAEP,
      huks.HuksCipherMode.HUKS_MODE_ECB,
      huks.HuksKeyDigest.HUKS_DIGEST_SHA1,
      done,
      'HUKS_ALG_COMPLETION_13200'
    );
  });
});