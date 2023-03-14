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
import {
  makeGenerateKeyOption,
  makeImportOption,
  makeEncryptAndDecryptOption,
  makeSignAndVerifyOption
} from '../../../../../../hks_xts_common.test'
export var alias = 'alias';
export var aliasEmpty = '';
export var aliasTooLong = '01234567890123456789012345678901234567890123456789012345678901234';
export var aliasCritical = '0123456789012345678901234567890123456789012345678901234567890123';
export var aliasNotExist = 'aliasNotExist';
export var emptyOption = makeEmptyOption();
export var plainTextSize128 = makeRandomArr(128);
export var plainTextSize64 = makeRandomArr(64);
export var plainTextSize17 = makeRandomArr(17);
export var plainTextSize16 = makeRandomArr(16);
export var plainTextSize8 = makeRandomArr(8);
export var plainTextSize4 = makeRandomArr(4);
export var invalidValue = 99999;
export var inDataTooLong = makeRandomArr(1000);
export var aliasStrange = '+++++++++++';
export var invalidValueUint8Array = new Uint8Array(99999);

export var generateAesCBCKeyOption = makeGenerateKeyOptionForAbnormal(
  hks.HuksKeyAlg.HUKS_ALG_AES,
  hks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
  hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
  hks.HuksKeyPadding.HUKS_PADDING_NONE,
  hks.HuksCipherMode.HUKS_MODE_CBC,
  null
);

export var generateAesGCMKeyOption = makeGenerateKeyOptionForAbnormal(
  hks.HuksKeyAlg.HUKS_ALG_AES,
  hks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
  hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
  hks.HuksKeyPadding.HUKS_PADDING_NONE,
  hks.HuksCipherMode.HUKS_MODE_GCM,
  null
);

export var generateRSAKeyOption = makeGenerateKeyOptionForAbnormal(
  hks.HuksKeyAlg.HUKS_ALG_RSA,
  hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
  hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
  hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
  null,
  hks.HuksKeyDigest.HUKS_DIGEST_SHA1
);

export var generateECCKeyOption = makeGenerateKeyOptionForAbnormal(
  hks.HuksKeyAlg.HUKS_ALG_ECC,
  hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224,
  hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
  null,
  null,
  hks.HuksKeyDigest.HUKS_DIGEST_NONE
);

export var generateMACKeyOption = makeGenerateKeyOptionForAbnormal(
  hks.HuksKeyAlg.HUKS_ALG_HMAC,
  160,
  hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
  null,
  null,
  hks.HuksKeyDigest.HUKS_DIGEST_SHA1
);

export function makeEmptyOption() {
  var emptyOptions = {
    properties: []
  };
  return emptyOptions;
};

export function makeRandomArr(size) {
  var arr = new Uint8Array(size);
  for (var i = 0; i < size; i++) {
    arr[i] = Math.floor(Math.random() * 10);
  }
  return arr;
};

function makeAlgTag(alg) {
  var tag = {
    tag: hks.HuksTag.HUKS_TAG_ALGORITHM,
    value: alg
  };
  return tag;
};

function makeSizeTag(size) {
  var tag = {
    tag: hks.HuksTag.HUKS_TAG_KEY_SIZE,
    value: size
  };
  return tag;
};

function makePurposeTag(purpose) {
  var tag = {
    tag: hks.HuksTag.HUKS_TAG_PURPOSE,
    value: purpose
  };
  return tag;
};

function makePaddingTag(padding) {
  var tag = {
    tag: hks.HuksTag.HUKS_TAG_PADDING,
    value: padding
  };
  return tag;
};

function makeDigestTag(digest) {
  var tag = {
    tag: hks.HuksTag.HUKS_TAG_DIGEST,
    value: digest
  };
  return tag;
};

function makeModeTag(mode) {
  var tag = {
    tag: hks.HuksTag.HUKS_TAG_BLOCK_MODE,
    value: mode
  };
  return tag;
};

export function makeGenerateKeyOptionForAbnormal(alg, size, purpose, padding, mode, digest) {
  var properties = new Array();
  properties[0] = makeAlgTag(alg);
  properties[1] = makeSizeTag(size);
  properties[2] = makePurposeTag(purpose);
  if (purpose == (hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT)) {
    properties[3] = makePaddingTag(padding);
    properties[4] = makeModeTag(mode);
  } else if (purpose == (hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY)) {
    properties[3] = makePaddingTag(padding);
    properties[4] = makeDigestTag(digest);
  } else {
    properties[3] = makeDigestTag(digest);
  }
  var option = {
    properties: properties
  };
  return option;
};

export function makeEncryptAndDecryptOptionForAbnormal(mode) {
  var properties = new Array();
  var purpose = hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT;
  properties[0] = makeAlgTag(hks.HuksKeyAlg.HUKS_ALG_AES);
  properties[1] = makePurposeTag(purpose);
  properties[2] = makeModeTag(mode);
  properties[3] = makePaddingTag(hks.HuksKeyPadding.HUKS_PADDING_NONE);
  if (mode == hks.HuksCipherMode.HUKS_MODE_CBC) {
    properties[4] = {
      tag: hks.HuksTag.HUKS_TAG_IV,
      value: new Uint8Array(16)
    };
  } else {
    properties[4] = {
      tag: hks.HuksTag.HUKS_TAG_NONCE,
      value: new Uint8Array(16)
    };
    properties[5] = {
      tag: hks.HuksTag.HUKS_TAG_ASSOCIATED_DATA,
      value: new Uint8Array(16)
    };
  }
  var option = {
    properties: properties,
    inData: plainTextSize64
  };
  return option;
};

export function makeSignVerifyAndImportOption(text) {
  var properties = new Array();
  properties[0] = makeAlgTag(hks.HuksKeyAlg.HUKS_ALG_RSA);
  properties[1] = makePurposeTag(hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY);
  properties[2] = makePaddingTag(hks.HuksKeyPadding.HUKS_PADDING_NONE);
  properties[3] = makeDigestTag(hks.HuksKeyDigest.HUKS_DIGEST_SHA1);
  var option = {
    properties: properties,
    inData: text
  };
  return option;
};

export function makeAgreeOption() {
  var properties = new Array();
  properties[0] = makeAlgTag(hks.HuksKeyAlg.HUKS_ALG_ECDH);
  properties[1] = makeSizeTag(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
  var options = {
    properties: properties,
    inData: new Uint8Array(76)
  };
  return options;
};

export function makeMacOption() {
  var properties = new Array();
  properties[0] = {
    tag: hks.HuksTag.HUKS_TAG_PURPOSE,
    value: hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC
  };
  properties[1] = {
    tag: hks.HuksTag.HUKS_TAG_DIGEST,
    value: hks.HuksKeyDigest.HUKS_DIGEST_SHA1
  };
  var options = {
    properties: properties,
    inData: plainTextSize16
  };
  return options;
};

export function makeAgreeAlgOptions(alg) {
  var properties = new Array();
  properties[0] = makeAlgTag(alg);
  properties[1] = makeSizeTag(hks.HuksKeySize.HUKS_ECC_KEY_SIZE_224);
  var options = {
    properties: properties,
    inData: new Uint8Array(76)
  };
  return options;
};

export function makeMacOptions(purpose, digest) {
  var properties = new Array();
  properties[0] = {
    tag: hks.HuksTag.HUKS_TAG_PURPOSE,
    value: purpose
  };
  properties[1] = {
    tag: hks.HuksTag.HUKS_TAG_DIGEST,
    value: digest
  };
  var options = {
    properties: properties,
    inData: plainTextSize16
  };
  return options;
};

export function generateAgreeKeyOption() {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_ECC,
    hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
    null,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA224
  );
  return option;
};

export function hmacGenerateKeyOption() {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_HMAC,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
    null,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1
  );
  return option;
};

export function agreeInvalidAlgOption(alg) {
  var option = makeAgreeAlgOptions(
    alg
  );
  return option;
};

export function hmacInvalidPurposeOption(purpose, digest) {
  var option = makeMacOptions(
    purpose,
    digest
  );
  return option;
};

export function encryptOrDecryptGenerateKeyOption() {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1
  );
  return option;
};

export function encryptOrDecryptOption() {
  var option = makeEncryptAndDecryptOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1,
    plainTextSize16
  );
  return option;
};

export function encryptOrDecryptInvalidAlgOption(alg) {
  var option = makeEncryptAndDecryptOption(
    alg,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    plainTextSize16
  );
  return option;
};

export function rsaEncryptInvalidSizeOption(size) {
  var option = makeEncryptAndDecryptOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    size,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1,
    plainTextSize4
  );
  return option;
};

export function aesEncryptOrDecryptInvalidSizeOption(size) {
  var option = makeEncryptAndDecryptOption(
    hks.HuksKeyAlg.HUKS_ALG_AES,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_NONE,
    hks.HuksCipherMode.HUKS_MODE_CBC,
    size,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    plainTextSize16
  );
  return option;
};

export function aesEncryptOrDecryptInvalidPaddingOption(padding) {
  var option = makeEncryptAndDecryptOption(
    hks.HuksKeyAlg.HUKS_ALG_AES,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    padding,
    hks.HuksCipherMode.HUKS_MODE_GCM,
    hks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    plainTextSize64
  );
  return option;
};

export function aesEncryptOrDecryptInvalidDigestOption(digest) {
  var option = makeEncryptAndDecryptOption(
    hks.HuksKeyAlg.HUKS_ALG_AES,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_NONE,
    hks.HuksCipherMode.HUKS_MODE_CBC,
    hks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
    digest,
    plainTextSize16
  );
  return option;
};

export function rsaEncryptOrDecryptInvalidIndataOption() {
  var option = makeEncryptAndDecryptOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_NONE,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    plainTextSize128
  );
  return option;
};

export function aesEncryptOrDecryptInvalidIndataOption() {
  var option = makeEncryptAndDecryptOption(
    hks.HuksKeyAlg.HUKS_ALG_AES,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_NONE,
    hks.HuksCipherMode.HUKS_MODE_GCM,
    hks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    plainTextSize17
  );
  return option;
};

export function aesEncryptOrDecryptInvalidPurposeOption(purpose) {
  var option = makeEncryptAndDecryptOption(
    hks.HuksKeyAlg.HUKS_ALG_AES,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_NONE,
    hks.HuksCipherMode.HUKS_MODE_GCM,
    hks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    plainTextSize64
  );
  return option;
};

export function rsaDecryptInvalidSizeOption(size, cipherText) {
  var option = makeEncryptAndDecryptOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    size,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1,
    cipherText
  );
  return option;
};

export function aesGenerateKeyInvalidSizeOption(size) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_AES,
    size,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_NONE,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE
  );
  return option;
};

export function aesGenerateKeyInvalidModeOption(mode, padding) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_AES,
    hks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    padding,
    mode,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE
  );
  return option;
};

export function aesGenerateKeyInvalidAlgOption(alg, purpose) {
  var option = makeGenerateKeyOption(
    alg,
    hks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_NONE,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE
  );
  return option;
};

export var aesGenerateKeyInvalidPurposeOption = makeGenerateKeyOption(
  hks.HuksKeyAlg.HUKS_ALG_AES,
  hks.HuksKeySize.HUKS_AES_KEY_SIZE_128,
  hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
  hks.HuksKeyPadding.HUKS_PADDING_NONE,
  hks.HuksCipherMode.HUKS_MODE_ECB,
  hks.HuksKeyDigest.HUKS_DIGEST_NONE
);

export function rsaGenerateKeyInvalidSizeOption(size) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    size,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1
  );
  return option;
};

export function rsaGenerateKeyInvalidPaddingOption(purpose, padding) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    purpose,
    padding,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1
  );
  return option;
};

export var rsaGenerateKeyInvalidPurposeOption = makeGenerateKeyOption(
  hks.HuksKeyAlg.HUKS_ALG_RSA,
  hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
  hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_ENCRYPT | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_DECRYPT,
  hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
  hks.HuksCipherMode.HUKS_MODE_ECB,
  hks.HuksKeyDigest.HUKS_DIGEST_SHA1
);

export function eccGenerateKeyInvalidSizeOption(size) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_ECC,
    size,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA224
  );
  return option;
};

export function eccGenerateKeyInvalidDigestOption(digest) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_ECC,
    hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    digest
  );
  return option;
};

export function eccGenerateKeyInvalidPurposeOption(purpose) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_ECC,
    hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA224
  );
  return option;
};

export function ecdhGenerateKeyInvalidAlgOption() {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_ECDH,
    hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_AGREE,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA224
  );
  return option;
};

export function hkdfGenerateKeyInvalidAlgOption() {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_HKDF,
    hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA224
  );
  return option;
};

export function pbkdf2GenerateKeyInvalidAlgOption() {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_PBKDF2,
    hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA224
  );
  return option;
};

export function ed25519GenerateKeyInvalidPurposeOption(purpose) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_ED25519,
    hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE
  );
  return option;
};

export function x25519GenerateKeyInvalidPurposeOption(purpose) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_X25519,
    hks.HuksKeySize.HUKS_CURVE25519_KEY_SIZE_256,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE
  );
  return option;
};

export function hmacGenerateKeyInvalidSizeOption(size) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_HMAC,
    size,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_MAC,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA256
  );
  return option;
};

export function hmacGenerateKeyInvalidPurposeOption(purpose) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_HMAC,
    hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA256
  );
  return option;
};

export function dsaGenerateKeyInvalidSizeOption(size) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_DSA,
    size,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1
  );
  return option;
};

export function dsaGenerateKeyInvalidPurposeOption(purpose) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_DSA,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_1024,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1
  );
  return option;
};

export function dhGenerateKeyInvalidPurposeOption(purpose) {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_DH,
    hks.HuksKeySize.HUKS_DH_KEY_SIZE_2048,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA1
  );
  return option;
};

export function generateImportKeyOption() {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_MD5
  );
  return option;
};

export function eccGenerateKeyOption() {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_ECC,
    hks.HuksKeySize.HUKS_ECC_KEY_SIZE_256,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    null,
    hks.HuksKeyDigest.HUKS_DIGEST_SHA224
  );
  return option;
};

export function importOptionsInvalidAlgOption(alg, purpose, publicKey) {
  var option = makeImportOption(
    alg,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    publicKey
  );
  return option;
};

export function importOptionsInvalidSizeOption(alg, publicKey) {
  var option = makeImportOption(
    alg,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_2048,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    publicKey
  );
  return option;
};

export function importOptionsInvalidPurposeOrPaddingOption(purpose, padding, publicKey) {
  var option = makeImportOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    purpose,
    padding,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    publicKey
  );
  return option;
};

export function signOrVerifyGenerateKeyOption() {
  var option = makeGenerateKeyOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksCipherMode.HUKS_MODE_ECB,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE
  );
  return option;
};

export function signOrVerifyInvalidAlgOption(alg) {
  var option = makeSignAndVerifyOption(
    alg,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    plainTextSize64
  );
  return option;
};

export function signOrVerifyInvalidPurposeOption(purpose) {
  var option = makeSignAndVerifyOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    purpose,
    hks.HuksKeyPadding.HUKS_PADDING_PKCS1_V1_5,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    plainTextSize64
  );
  return option;
};

export function signOrVerifyInvalidPaddingOption(padding) {
  var option = makeSignAndVerifyOption(
    hks.HuksKeyAlg.HUKS_ALG_RSA,
    hks.HuksKeySize.HUKS_RSA_KEY_SIZE_512,
    hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_SIGN | hks.HuksKeyPurpose.HUKS_KEY_PURPOSE_VERIFY,
    padding,
    hks.HuksKeyDigest.HUKS_DIGEST_NONE,
    plainTextSize64
  );
  return option;
};