/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

/**
 * Defines the cipher response.
 * @since 3
 * @syscap SystemCapability.Security.Cipher
 * @permission N/A
 */
export interface CipherResponse {
  /**
   * response text
   * @since 3
   * @syscap SystemCapability.Security.Cipher
   */
  text: string;
}

/**
 * Defines the rsa cipher options.
 * @since 3
 * @syscap SystemCapability.Security.Cipher
 * @permission N/A
 */
export interface CipherRsaOptions {
  /**
   * Action type.
   * The options are as follows:
   *   encrypt: Encrypts data.
   *   decrypt: Decrypts data.
   * @since 3
   * @syscap SystemCapability.Security.Cipher
   */
  action: string;

  /**
   * Text content to be encrypted or decrypted.
   * The text to be encrypted must be a common text and cannot exceed the length calculated based on the formula (keySize/8 - 66).
   * keySize indicates the key length.
   * For example, if the key length is 1024 bytes, the text cannot exceed 62 bytes (1024/8 - 66 = 62).
   * The text content to be decrypted must be a binary value encoded using Base64.
   * The default format is used for Base64 encoding.
   * @since 3
   * @syscap SystemCapability.Security.Cipher
   */
  text: string;

  /**
   * Keys encrypted using RSA.
   * During encryption, this parameter is a public key.
   * During decryption, it is a private key.
   * @since 3
   * @syscap SystemCapability.Security.Cipher
   */
  key: string;

  /**
   * RSA algorithm padding.
   * The default value is RSA/None/OAEPWithSHA256AndMGF1Padding.
   * @since 3
   * @syscap SystemCapability.Security.Cipher
   */
  transformation?: string;

  /**
   * Called when data is encrypted or decrypted successfully.
   * @since 3
   * @syscap SystemCapability.Security.Cipher
   */
  success: (data: CipherResponse) => void;

  /**
   * Called when data fails to be encrypted or decrypted.
   * @since 3
   * @syscap SystemCapability.Security.Cipher
   */
  fail: (data: string, code: number) => void;

  /**
   * Called when the execution is completed.
   * @since 3
   * @syscap SystemCapability.Security.Cipher
   */
  complete: () => void;
}

/**
 * Defines the aes cipher options.
 * @since 3
 * @syscap SystemCapability.Security.Cipher
 * @permission N/A
 */
export interface CipherAesOptions {
  /**
   * Action type.
   * The options are as follows:
   *   encrypt: Encrypts data.
   *   decrypt: Decrypts data.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  action: string;

  /**
   * Text content to be encrypted or decrypted.
   * The text to be encrypted must be a common text.
   * The text content to be decrypted must be a binary value encoded using Base64.
   * The default format is used for Base64 encoding.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  text: string;

  /**
   * Key used for encryption or decryption, which is a character string encrypted using Base64.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  key: string;

  /**
   * Encryption mode and padding of the AES algorithm.
   * The default value is AES/CBC/PKCS5Padding.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  transformation?: string;

  /**
   * Initial vector for AES-based encryption and decryption.
   * The value is a character string encoded using Base64.
   * The default value is the key value.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  iv?: string;

  /**
   * Offset of the initial vector for AES-based encryption and decryption.
   * The default value is 0.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  ivOffset?: string;

  /**
   * Length of the initial vector for AES-based encryption and decryption.
   * The default value is 16.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  ivLen?: string;

  /**
   * Called when data is encrypted or decrypted successfully.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  success: (data: CipherResponse) => void;

  /**
   * Called when data fails to be encrypted or decrypted.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  fail: (data: string, code: number) => void;

  /**
   * Called when the execution is completed.
   * @syscap SystemCapability.Security.Cipher
   * @since 3
   */
  complete: () => void;
}

/**
 * Defines the cipher functions.
 * @since 3
 * @syscap SystemCapability.Security.Cipher
 * @permission N/A
 */
export default class Cipher {
  /**
   * Encrypts or decrypts data using RSA.
   * @param options RSA options
   * @syscap SystemCapability.Security.Cipher
   */
  static rsa(options: CipherRsaOptions): void;

  /**
   * Encrypts or decrypts data using AES.
   * @param options AES options
   * @syscap SystemCapability.Security.Cipher
   */
  static aes(options: CipherAesOptions): void;
}
