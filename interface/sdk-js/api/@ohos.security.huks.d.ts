/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
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

import {AsyncCallback} from './basic';

/**
 * OpenHarmony Universal KeyStore
 * @since 8
 * @syscap SystemCapability.Security.Huks
 * @permission N/A
 */
declare namespace huks {
    /**
     * Generate Key.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param keyAlias Indicates the key's name.
     * @param options Indicates the properties of the key.
     */
    function generateKey(keyAlias: string, options: HuksOptions, callback: AsyncCallback<HuksResult>) : void;
    function generateKey(keyAlias: string, options: HuksOptions) : Promise<HuksResult>;

    /**
     * Delete Key.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param keyAlias Indicates the key's name.
     * @param options Indicates the properties of the key.
     */
    function deleteKey(keyAlias: string, options: HuksOptions, callback: AsyncCallback<HuksResult>) : void;
    function deleteKey(keyAlias: string, options: HuksOptions) : Promise<HuksResult>;

    /**
     * Import Key.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param keyAlias Indicates the key's name.
     * @param options Indicates the properties of the key.
     */
    function importKey(keyAlias: string, options: HuksOptions, callback: AsyncCallback<HuksResult>) : void;
    function importKey(keyAlias: string, options: HuksOptions) : Promise<HuksResult>;

    /**
     * Export Key.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param keyAlias Indicates the key's name.
     * @param options Indicates the properties of the key.
     */
    function exportKey(keyAlias: string, options: HuksOptions, callback: AsyncCallback<HuksResult>) : void;
    function exportKey(keyAlias: string, options: HuksOptions) : Promise<HuksResult>;

    /**
     * Get properties of the key.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param keyAlias Indicates the key's name.
     * @param options Indicates the properties of the key.
     */
    function getKeyProperties(keyAlias: string, options: HuksOptions, callback: AsyncCallback<HuksResult>) : void;
    function getKeyProperties(keyAlias: string, options: HuksOptions) : Promise<HuksResult>;

    /**
     * Check whether the key exists.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param keyAlias Indicates the key's name.
     * @param options Indicates the properties of the key.
     */
    function isKeyExist(keyAlias: string, options: HuksOptions, callback: AsyncCallback<boolean>) : void;
    function isKeyExist(keyAlias: string, options: HuksOptions) : Promise<boolean>;

    /**
     * Init Operation.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param keyAlias Indicates the key's name.
     * @param options Indicates the properties of the key.
     * @return The handle of the init Operation.
     */
    function init(keyAlias: string, options: HuksOptions, callback: AsyncCallback<HuksHandle>) : void;
    function init(keyAlias: string, options: HuksOptions) : Promise<HuksHandle>;

    /**
     * Update Operation.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param handle Indicates the handle of the init operation.
     * @param token Indicates the value of token.
     * @param options Indicates the properties of the update operation.
     */
    function update(handle: number, token?: Uint8Array, options: HuksOptions, callback: AsyncCallback<HuksResult>) : void;
    function update(handle: number, token?: Uint8Array, options: HuksOptions) : Promise<HuksResult>;

    /**
     * Finish Operation.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param handle Indicates the handle of the init operation.
     * @param options Indicates the properties of the finish operation.
     */
    function finish(handle: number, options: HuksOptions, callback: AsyncCallback<HuksResult>) : void;
    function finish(handle: number, options: HuksOptions) : Promise<HuksResult>;

    /**
     * Abort Operation.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param handle Indicates the handle of the init operation.
     * @param options Indicates the properties of the abort operation.
     */
    function abort(handle: number, options: HuksOptions, callback: AsyncCallback<HuksResult>) : void;
    function abort(handle: number, options: HuksOptions) : Promise<HuksResult>;

    /**
     * Get the sdk version.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @param options Indicates the properties of the key.
     * @return Return the character string of the sdk version.
     */
    function getSdkVersion(options: HuksOptions) : string;

    /**
     * Interface of huks param.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export interface HuksParam {
        tag: HuksTag;
        value: boolean | number | bigint | Uint8Array;
    }

    /**
     * Interface of huks handle.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export interface HuksHandle {
        errorCode: number;
        handle: number;
        token?: Uint8Array;
    }

    /**
     * Interface of huks option.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export interface HuksOptions {
        properties?: Array<HuksParam>;
        inData?: Uint8Array;
    }

    /**
     * Interface of huks result.
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export interface HuksResult {
        errorCode: number;
        outData?: Uint8Array;
        properties?: Array<HuksParam>;
        certChains?: Array<string>;
    }

    /**
     * @name HuksErrorCode
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksErrorCode {
        HUKS_SUCCESS = 0,
        HUKS_FAILURE = -1,
        HUKS_ERROR_BAD_STATE = -2,
        HUKS_ERROR_INVALID_ARGUMENT = -3,
        HUKS_ERROR_NOT_SUPPORTED = -4,
        HUKS_ERROR_NO_PERMISSION = -5,
        HUKS_ERROR_INSUFFICIENT_DATA = -6,
        HUKS_ERROR_BUFFER_TOO_SMALL = -7,
        HUKS_ERROR_INSUFFICIENT_MEMORY = -8,
        HUKS_ERROR_COMMUNICATION_FAILURE = -9,
        HUKS_ERROR_STORAGE_FAILURE = -10,
        HUKS_ERROR_HARDWARE_FAILURE = -11,
        HUKS_ERROR_ALREADY_EXISTS = -12,
        HUKS_ERROR_NOT_EXIST = -13,
        HUKS_ERROR_NULL_POINTER = -14,
        HUKS_ERROR_FILE_SIZE_FAIL = -15,
        HUKS_ERROR_READ_FILE_FAIL = -16,
        HUKS_ERROR_INVALID_PUBLIC_KEY = -17,
        HUKS_ERROR_INVALID_PRIVATE_KEY = -18,
        HUKS_ERROR_INVALID_KEY_INFO = -19,
        HUKS_ERROR_HASH_NOT_EQUAL = -20,
        HUKS_ERROR_MALLOC_FAIL = -21,
        HUKS_ERROR_WRITE_FILE_FAIL = -22,
        HUKS_ERROR_REMOVE_FILE_FAIL = -23,
        HUKS_ERROR_OPEN_FILE_FAIL = -24,
        HUKS_ERROR_CLOSE_FILE_FAIL = -25,
        HUKS_ERROR_MAKE_DIR_FAIL = -26,
        HUKS_ERROR_INVALID_KEY_FILE = -27,
        HUKS_ERROR_IPC_MSG_FAIL = -28,
        HUKS_ERROR_REQUEST_OVERFLOWS = -29,
        HUKS_ERROR_PARAM_NOT_EXIST = -30,
        HUKS_ERROR_CRYPTO_ENGINE_ERROR = -31,
        HUKS_ERROR_COMMUNICATION_TIMEOUT = -32,
        HUKS_ERROR_IPC_INIT_FAIL = -33,
        HUKS_ERROR_IPC_DLOPEN_FAIL = -34,
        HUKS_ERROR_EFUSE_READ_FAIL = -35,
        HUKS_ERROR_NEW_ROOT_KEY_MATERIAL_EXIST = -36,
        HUKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL = -37,
        HUKS_ERROR_VERIFICATION_FAILED = -38,

        HUKS_ERROR_CHECK_GET_ALG_FAIL = -100,
        HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL = -101,
        HUKS_ERROR_CHECK_GET_PADDING_FAIL = -102,
        HUKS_ERROR_CHECK_GET_PURPOSE_FAIL = -103,
        HUKS_ERROR_CHECK_GET_DIGEST_FAIL =  -104,
        HUKS_ERROR_CHECK_GET_MODE_FAIL = -105,
        HUKS_ERROR_CHECK_GET_NONCE_FAIL = -106,
        HUKS_ERROR_CHECK_GET_AAD_FAIL = -107,
        HUKS_ERROR_CHECK_GET_IV_FAIL = -108,
        HUKS_ERROR_CHECK_GET_AE_TAG_FAIL = -109,
        HUKS_ERROR_CHECK_GET_SALT_FAIL = -110,
        HUKS_ERROR_CHECK_GET_ITERATION_FAIL = -111,
        HUKS_ERROR_INVALID_ALGORITHM = -112,
        HUKS_ERROR_INVALID_KEY_SIZE = -113,
        HUKS_ERROR_INVALID_PADDING = -114,
        HUKS_ERROR_INVALID_PURPOSE = -115,
        HUKS_ERROR_INVALID_MODE = -116,
        HUKS_ERROR_INVALID_DIGEST =  -117,
        HUKS_ERROR_INVALID_SIGNATURE_SIZE = -118,
        HUKS_ERROR_INVALID_IV = -119,
        HUKS_ERROR_INVALID_AAD = -120,
        HUKS_ERROR_INVALID_NONCE = -121,
        HUKS_ERROR_INVALID_AE_TAG = -122,
        HUKS_ERROR_INVALID_SALT = -123,
        HUKS_ERROR_INVALID_ITERATION = -124,
        HUKS_ERROR_INVALID_OPERATION = -125,

        HUKS_ERROR_INTERNAL_ERROR = -999,
        HUKS_ERROR_UNKNOWN_ERROR = -1000,
    }

    /**
     * @name HuksKeyPurpose
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksKeyPurpose {
        HUKS_KEY_PURPOSE_ENCRYPT = 1,                   /* Usable with RSA, EC and AES keys. */
        HUKS_KEY_PURPOSE_DECRYPT = 2,                   /* Usable with RSA, EC and AES keys. */
        HUKS_KEY_PURPOSE_SIGN = 4,                      /* Usable with RSA, EC keys. */
        HUKS_KEY_PURPOSE_VERIFY = 8,                    /* Usable with RSA, EC keys. */
        HUKS_KEY_PURPOSE_DERIVE = 16,                   /* Usable with EC keys. */
        HUKS_KEY_PURPOSE_WRAP = 32,                     /* Usable with wrap key. */
        HUKS_KEY_PURPOSE_UNWRAP = 64,                   /* Usable with unwrap key. */
        HUKS_KEY_PURPOSE_MAC = 128,                     /* Usable with mac. */
        HUKS_KEY_PURPOSE_AGREE = 256,                   /* Usable with agree. */
    }

    /**
     * @name HuksKeyDigest
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksKeyDigest {
        HUKS_DIGEST_NONE = 0,
        HUKS_DIGEST_MD5 = 1,
        HUKS_DIGEST_SHA1 = 10,
        HUKS_DIGEST_SHA224 = 11,
        HUKS_DIGEST_SHA256 = 12,
        HUKS_DIGEST_SHA384 = 13,
        HUKS_DIGEST_SHA512 = 14,
    }

    /**
     * @name HuksKeyPadding
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksKeyPadding {
        HUKS_PADDING_NONE = 0,
        HUKS_PADDING_OAEP = 1,
        HUKS_PADDING_PSS = 2,
        HUKS_PADDING_PKCS1_V1_5 = 3,
        HUKS_PADDING_PKCS5 = 4,
        HUKS_PADDING_PKCS7 = 5,
    }

    /**
     * @name HuksCipherMode
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksCipherMode {
        HUKS_MODE_ECB = 1,
        HUKS_MODE_CBC = 2,
        HUKS_MODE_CTR = 3,
        HUKS_MODE_OFB = 4,
        HUKS_MODE_CCM = 31,
        HUKS_MODE_GCM = 32,
    }

    /**
     * @name HuksKeySize
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksKeySize {
        HUKS_RSA_KEY_SIZE_512 = 512,
        HUKS_RSA_KEY_SIZE_768 = 768,
        HUKS_RSA_KEY_SIZE_1024 = 1024,
        HUKS_RSA_KEY_SIZE_2048 = 2048,
        HUKS_RSA_KEY_SIZE_3072 = 3072,
        HUKS_RSA_KEY_SIZE_4096 = 4096,

        HUKS_ECC_KEY_SIZE_224 = 224,
        HUKS_ECC_KEY_SIZE_256 = 256,
        HUKS_ECC_KEY_SIZE_384 = 384,
        HUKS_ECC_KEY_SIZE_521 = 521,

        HUKS_AES_KEY_SIZE_128 = 128,
        HUKS_AES_KEY_SIZE_192 = 192,
        HUKS_AES_KEY_SIZE_256 = 256,
        HUKS_AES_KEY_SIZE_512 = 512,

        HUKS_CURVE25519_KEY_SIZE_256 = 256,

        HUKS_DH_KEY_SIZE_2048 = 2048,
        HUKS_DH_KEY_SIZE_3072 = 3072,
        HUKS_DH_KEY_SIZE_4096 = 4096,
    }

    /**
     * @name HuksKeyAlg
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksKeyAlg {
        HUKS_ALG_RSA = 1,
        HUKS_ALG_ECC = 2,
        HUKS_ALG_DSA = 3,

        HUKS_ALG_AES = 20,
        HUKS_ALG_HMAC = 50,
        HUKS_ALG_HKDF = 51,
        HUKS_ALG_PBKDF2 = 52,

        HUKS_ALG_ECDH = 100,
        HUKS_ALG_X25519 = 101,
        HUKS_ALG_ED25519 = 102,
        HUKS_ALG_DH = 103,
    }

    /**
     * @name HuksKeyGenerateType
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksKeyGenerateType {
        HUKS_KEY_GENERATE_TYPE_DEFAULT = 0,
        HUKS_KEY_GENERATE_TYPE_DERIVE = 1,
        HUKS_KEY_GENERATE_TYPE_AGREE = 2,
    }

    /**
     * @name HuksKeyFlag
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksKeyFlag {
        HUKS_KEY_FLAG_IMPORT_KEY = 1,
        HUKS_KEY_FLAG_GENERATE_KEY = 2,
        HUKS_KEY_FLAG_AGREE_KEY = 3,
        HUKS_KEY_FLAG_DERIVE_KEY = 4,
    }

    /**
     * @name HuksKeyStorageType
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksKeyStorageType {
        HUKS_STORAGE_TEMP = 0,
        HUKS_STORAGE_PERSISTENT = 1,
    }

    /**
     * @name HuksSendType
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksSendType {
        HUKS_SEND_TYPE_ASYNC = 0,
        HUKS_SEND_TYPE_SYNC = 1,
    }

    /**
     * @name HuksTagType
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    declare enum HuksTagType {
        HUKS_TAG_TYPE_INVALID = 0 << 28,
        HUKS_TAG_TYPE_INT = 1 << 28,
        HUKS_TAG_TYPE_UINT = 2 << 28,
        HUKS_TAG_TYPE_ULONG = 3 << 28,
        HUKS_TAG_TYPE_BOOL = 4 << 28,
        HUKS_TAG_TYPE_BYTES = 5 << 28,
    }

    /**
     * @name HuksTag
     * @since 8
     * @syscap SystemCapability.Security.Huks
     * @permission N/A
     */
    export enum HuksTag {
        /* Invalid TAG */
        HUKS_TAG_INVALID = HuksTagType.HUKS_TAG_TYPE_INVALID | 0,

        /* Base algrithom TAG: 1 - 200 */
        HUKS_TAG_ALGORITHM = HuksTagType.HUKS_TAG_TYPE_UINT | 1,
        HUKS_TAG_PURPOSE = HuksTagType.HUKS_TAG_TYPE_UINT | 2,
        HUKS_TAG_KEY_SIZE = HuksTagType.HUKS_TAG_TYPE_UINT | 3,
        HUKS_TAG_DIGEST = HuksTagType.HUKS_TAG_TYPE_UINT | 4,
        HUKS_TAG_PADDING = HuksTagType.HUKS_TAG_TYPE_UINT | 5,
        HUKS_TAG_BLOCK_MODE = HuksTagType.HUKS_TAG_TYPE_UINT | 6,
        HUKS_TAG_KEY_TYPE = HuksTagType.HUKS_TAG_TYPE_UINT | 7,
        HUKS_TAG_ASSOCIATED_DATA = HuksTagType.HUKS_TAG_TYPE_BYTES | 8,
        HUKS_TAG_NONCE = HuksTagType.HUKS_TAG_TYPE_BYTES | 9,
        HUKS_TAG_IV = HuksTagType.HUKS_TAG_TYPE_BYTES | 10,

        /* Key derivation TAG */
        HUKS_TAG_INFO = HuksTagType.HUKS_TAG_TYPE_BYTES | 11,
        HUKS_TAG_SALT = HuksTagType.HUKS_TAG_TYPE_BYTES | 12,
        HUKS_TAG_PWD = HuksTagType.HUKS_TAG_TYPE_BYTES | 13,
        HUKS_TAG_ITERATION = HuksTagType.HUKS_TAG_TYPE_UINT | 14,

        HUKS_TAG_KEY_GENERATE_TYPE = HuksTagType.HUKS_TAG_TYPE_UINT | 15, /* choose from enum HuksKeyGenerateType */
        HUKS_TAG_DERIVE_MAIN_KEY = HuksTagType.HUKS_TAG_TYPE_BYTES | 16,
        HUKS_TAG_DERIVE_FACTOR = HuksTagType.HUKS_TAG_TYPE_BYTES | 17,
        HUKS_TAG_DERIVE_ALG = HuksTagType.HUKS_TAG_TYPE_UINT | 18,
        HUKS_TAG_AGREE_ALG = HuksTagType.HUKS_TAG_TYPE_UINT | 19,
        HUKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS = HuksTagType.HUKS_TAG_TYPE_BOOL | 20,
        HUKS_TAG_AGREE_PRIVATE_KEY_ALIAS = HuksTagType.HUKS_TAG_TYPE_BYTES | 21,
        HUKS_TAG_AGREE_PUBLIC_KEY = HuksTagType.HUKS_TAG_TYPE_BYTES | 22,
        HUKS_TAG_KEY_ALIAS = HuksTagType.HUKS_TAG_TYPE_BYTES | 23,
        HUKS_TAG_DERIVE_KEY_SIZE = HuksTagType.HUKS_TAG_TYPE_UINT | 24,

        /*
         * Key authentication related TAG: 201 - 300
         *
         * Start of validity
         */
        HUKS_TAG_ACTIVE_DATETIME = HuksTagType.HUKS_TAG_TYPE_ULONG | 201,

        /* Date when new "messages" should not be created. */
        HUKS_TAG_ORIGINATION_EXPIRE_DATETIME = HuksTagType.HUKS_TAG_TYPE_ULONG | 202,

        /* Date when existing "messages" should not be used. */
        HUKS_TAG_USAGE_EXPIRE_DATETIME = HuksTagType.HUKS_TAG_TYPE_ULONG | 203,

        /* Key creation time */
        HUKS_TAG_CREATION_DATETIME = HuksTagType.HUKS_TAG_TYPE_ULONG | 204,

        /* Other authentication related TAG: 301 - 500 */
        HUKS_TAG_ALL_USERS = HuksTagType.HUKS_TAG_TYPE_BOOL | 301,
        HUKS_TAG_USER_ID = HuksTagType.HUKS_TAG_TYPE_UINT | 302,
        HUKS_TAG_NO_AUTH_REQUIRED = HuksTagType.HUKS_TAG_TYPE_BOOL | 303,
        HUKS_TAG_USER_AUTH_TYPE = HuksTagType.HUKS_TAG_TYPE_UINT | 304,
        HUKS_TAG_AUTH_TIMEOUT = HuksTagType.HUKS_TAG_TYPE_UINT | 305,
        HUKS_TAG_AUTH_TOKEN = HuksTagType.HUKS_TAG_TYPE_BYTES | 306,

        /* Attestation related TAG: 501 - 600 */
        HUKS_TAG_ATTESTATION_CHALLENGE = HuksTagType.HUKS_TAG_TYPE_BYTES | 501,
        HUKS_TAG_ATTESTATION_APPLICATION_ID = HuksTagType.HUKS_TAG_TYPE_BYTES | 502,
        HUKS_TAG_ATTESTATION_ID_BRAND = HuksTagType.HUKS_TAG_TYPE_BYTES | 503,
        HUKS_TAG_ATTESTATION_ID_DEVICE = HuksTagType.HUKS_TAG_TYPE_BYTES | 504,
        HUKS_TAG_ATTESTATION_ID_PRODUCT = HuksTagType.HUKS_TAG_TYPE_BYTES | 505,
        HUKS_TAG_ATTESTATION_ID_SERIAL = HuksTagType.HUKS_TAG_TYPE_BYTES | 506,
        HUKS_TAG_ATTESTATION_ID_IMEI = HuksTagType.HUKS_TAG_TYPE_BYTES | 507,
        HUKS_TAG_ATTESTATION_ID_MEID = HuksTagType.HUKS_TAG_TYPE_BYTES | 508,
        HUKS_TAG_ATTESTATION_ID_MANUFACTURER = HuksTagType.HUKS_TAG_TYPE_BYTES | 509,
        HUKS_TAG_ATTESTATION_ID_MODEL = HuksTagType.HUKS_TAG_TYPE_BYTES | 510,
        HUKS_TAG_ATTESTATION_ID_ALIAS = HuksTagType.HUKS_TAG_TYPE_BYTES | 511,
        HUKS_TAG_ATTESTATION_ID_SOCID = HuksTagType.HUKS_TAG_TYPE_BYTES | 512,
        HUKS_TAG_ATTESTATION_ID_UDID = HuksTagType.HUKS_TAG_TYPE_BYTES | 513,
        HUKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO = HuksTagType.HUKS_TAG_TYPE_BYTES | 514,
        HUKS_TAG_ATTESTATION_ID_VERSION_INFO = HuksTagType.HUKS_TAG_TYPE_BYTES | 515,

        /*
         * Other reserved TAG: 601 - 1000
         *
         * Extention TAG: 1001 - 9999
         */
        HUKS_TAG_IS_KEY_ALIAS = HuksTagType.HUKS_TAG_TYPE_BOOL | 1001,
        HUKS_TAG_KEY_STORAGE_FLAG = HuksTagType.HUKS_TAG_TYPE_UINT | 1002, /* choose from enum HuksKeyStorageType */
        HUKS_TAG_IS_ALLOWED_WRAP = HuksTagType.HUKS_TAG_TYPE_BOOL | 1003,
        HUKS_TAG_KEY_WRAP_TYPE = HuksTagType.HUKS_TAG_TYPE_UINT | 1004,
        HUKS_TAG_KEY_AUTH_ID = HuksTagType.HUKS_TAG_TYPE_BYTES | 1005,
        HUKS_TAG_KEY_ROLE = HuksTagType.HUKS_TAG_TYPE_UINT | 1006,
        HUKS_TAG_KEY_FLAG = HuksTagType.HUKS_TAG_TYPE_UINT | 1007, /* choose from enum HuksKeyFlag */
        HUKS_TAG_IS_ASYNCHRONIZED = HuksTagType.HUKS_TAG_TYPE_UINT | 1008,
        HUKS_TAG_SECURE_KEY_ALIAS = HuksTagType.HUKS_TAG_TYPE_BOOL | 1009,
        HUKS_TAG_SECURE_KEY_UUID = HuksTagType.HUKS_TAG_TYPE_BYTES | 1010,
        HUKS_TAG_KEY_DOMAIN = HuksTagType.HUKS_TAG_TYPE_UINT | 1011,

        /* Inner-use TAG: 10001 - 10999 */
        HUKS_TAG_PROCESS_NAME = HuksTagType.HUKS_TAG_TYPE_BYTES | 10001,
        HUKS_TAG_PACKAGE_NAME = HuksTagType.HUKS_TAG_TYPE_BYTES | 10002,
        HUKS_TAG_ACCESS_TIME = HuksTagType.HUKS_TAG_TYPE_UINT | 10003,
        HUKS_TAG_USES_TIME = HuksTagType.HUKS_TAG_TYPE_UINT | 10004,
        HUKS_TAG_CRYPTO_CTX = HuksTagType.HUKS_TAG_TYPE_ULONG | 10005,
        HUKS_TAG_KEY = HuksTagType.HUKS_TAG_TYPE_BYTES | 10006,
        HUKS_TAG_KEY_VERSION = HuksTagType.HUKS_TAG_TYPE_UINT | 10007,
        HUKS_TAG_PAYLOAD_LEN = HuksTagType.HUKS_TAG_TYPE_UINT | 10008,
        HUKS_TAG_AE_TAG = HuksTagType.HUKS_TAG_TYPE_BYTES | 10009,
        HUKS_TAG_IS_KEY_HANDLE = HuksTagType.HUKS_TAG_TYPE_ULONG | 10010,

        /* Os version related TAG */
        HUKS_TAG_OS_VERSION = HuksTagType.HUKS_TAG_TYPE_UINT | 10101,
        HUKS_TAG_OS_PATCHLEVEL = HuksTagType.HUKS_TAG_TYPE_UINT | 10102,

        /*
         * Reversed TAGs for SOTER: 11000 - 12000
         *
         * Other TAGs: 20001 - N
         * TAGs used for paramSetOut
         */
        HUKS_TAG_SYMMETRIC_KEY_DATA = HuksTagType.HUKS_TAG_TYPE_BYTES | 20001,
        HUKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA = HuksTagType.HUKS_TAG_TYPE_BYTES | 20002,
        HUKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA = HuksTagType.HUKS_TAG_TYPE_BYTES | 20003,
    }
}

export default huks;
