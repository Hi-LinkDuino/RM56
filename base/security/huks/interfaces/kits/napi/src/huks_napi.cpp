/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "huks_napi.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "hks_type.h"
#include "huks_napi_agree_key.h"
#include "huks_napi_attest_key.h"
#include "huks_napi_decrypt.h"
#include "huks_napi_delete_key.h"
#include "huks_napi_derive_key.h"
#include "huks_napi_encrypt.h"
#include "huks_napi_export_key.h"
#include "huks_napi_generate_key.h"
#include "huks_napi_get_certificate_chain.h"
#include "huks_napi_get_key_properties.h"
#include "huks_napi_get_sdk_version.h"
#include "huks_napi_import_key.h"
#include "huks_napi_is_key_exist.h"
#include "huks_napi_mac.h"
#include "huks_napi_sign.h"
#include "huks_napi_unwrap_key.h"
#include "huks_napi_verify.h"
#include "huks_napi_wrap_key.h"

#include "huks_napi_abort.h"
#include "huks_napi_finish.h"
#include "huks_napi_init.h"
#include "huks_napi_update.h"

namespace HuksNapi {
inline void AddInt32Property(napi_env env, napi_value object, const char *name, int32_t value)
{
    napi_value property = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, value, &property));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, object, name, property));
}

static void AddHuksTagPart1(napi_env env, napi_value tag)
{
    /* Invalid TAG */
    AddInt32Property(env, tag, "HUKS_TAG_INVALID", HKS_TAG_INVALID);

    /* Base algrithom TAG: 1 - 200 */
    AddInt32Property(env, tag, "HUKS_TAG_ALGORITHM", HKS_TAG_ALGORITHM);
    AddInt32Property(env, tag, "HUKS_TAG_PURPOSE", HKS_TAG_PURPOSE);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_SIZE", HKS_TAG_KEY_SIZE);
    AddInt32Property(env, tag, "HUKS_TAG_DIGEST", HKS_TAG_DIGEST);
    AddInt32Property(env, tag, "HUKS_TAG_PADDING", HKS_TAG_PADDING);
    AddInt32Property(env, tag, "HUKS_TAG_BLOCK_MODE", HKS_TAG_BLOCK_MODE);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_TYPE", HKS_TAG_KEY_TYPE);
    AddInt32Property(env, tag, "HUKS_TAG_ASSOCIATED_DATA", HKS_TAG_ASSOCIATED_DATA);
    AddInt32Property(env, tag, "HUKS_TAG_NONCE", HKS_TAG_NONCE);
    AddInt32Property(env, tag, "HUKS_TAG_IV", HKS_TAG_IV);

    /* Key derivation TAG */
    AddInt32Property(env, tag, "HUKS_TAG_INFO", HKS_TAG_INFO);
    AddInt32Property(env, tag, "HUKS_TAG_SALT", HKS_TAG_SALT);
    AddInt32Property(env, tag, "HUKS_TAG_PWD", HKS_TAG_PWD);
    AddInt32Property(env, tag, "HUKS_TAG_ITERATION", HKS_TAG_ITERATION);

    AddInt32Property(env, tag, "HUKS_TAG_KEY_GENERATE_TYPE", HKS_TAG_KEY_GENERATE_TYPE);
    AddInt32Property(env, tag, "HUKS_TAG_DERIVE_MAIN_KEY", HKS_TAG_DERIVE_MAIN_KEY);
    AddInt32Property(env, tag, "HUKS_TAG_DERIVE_FACTOR", HKS_TAG_DERIVE_FACTOR);
    AddInt32Property(env, tag, "HUKS_TAG_DERIVE_ALG", HKS_TAG_DERIVE_ALG);
    AddInt32Property(env, tag, "HUKS_TAG_AGREE_ALG", HKS_TAG_AGREE_ALG);
    AddInt32Property(env, tag, "HUKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS", HKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS);
    AddInt32Property(env, tag, "HUKS_TAG_AGREE_PRIVATE_KEY_ALIAS", HKS_TAG_AGREE_PRIVATE_KEY_ALIAS);
    AddInt32Property(env, tag, "HUKS_TAG_AGREE_PUBLIC_KEY", HKS_TAG_AGREE_PUBLIC_KEY);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_ALIAS", HKS_TAG_KEY_ALIAS);
    AddInt32Property(env, tag, "HUKS_TAG_DERIVE_KEY_SIZE", HKS_TAG_DERIVE_KEY_SIZE);

    /*
     * Key authentication related TAG: 201 - 300
     *
     * Start of validity
     */
    AddInt32Property(env, tag, "HUKS_TAG_ACTIVE_DATETIME", HKS_TAG_ACTIVE_DATETIME);

    /* Date when new "messages" should not be created. */
    AddInt32Property(env, tag, "HUKS_TAG_ORIGINATION_EXPIRE_DATETIME", HKS_TAG_ORIGINATION_EXPIRE_DATETIME);

    /* Date when existing "messages" should not be used. */
    AddInt32Property(env, tag, "HUKS_TAG_USAGE_EXPIRE_DATETIME", HKS_TAG_USAGE_EXPIRE_DATETIME);

    /* Key creation time */
    AddInt32Property(env, tag, "HUKS_TAG_CREATION_DATETIME", HKS_TAG_CREATION_DATETIME);

    /* Other authentication related TAG: 301 - 500 */
    AddInt32Property(env, tag, "HUKS_TAG_ALL_USERS", HKS_TAG_ALL_USERS);
    AddInt32Property(env, tag, "HUKS_TAG_USER_ID", HKS_TAG_USER_ID);
    AddInt32Property(env, tag, "HUKS_TAG_NO_AUTH_REQUIRED", HKS_TAG_NO_AUTH_REQUIRED);
    AddInt32Property(env, tag, "HUKS_TAG_USER_AUTH_TYPE", HKS_TAG_USER_AUTH_TYPE);
    AddInt32Property(env, tag, "HUKS_TAG_AUTH_TIMEOUT", HKS_TAG_AUTH_TIMEOUT);
    AddInt32Property(env, tag, "HUKS_TAG_AUTH_TOKEN", HKS_TAG_AUTH_TOKEN);

    /* Attestation related TAG: 501 - 600 */
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_CHALLENGE", HKS_TAG_ATTESTATION_CHALLENGE);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_APPLICATION_ID", HKS_TAG_ATTESTATION_APPLICATION_ID);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_BRAND", HKS_TAG_ATTESTATION_ID_BRAND);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_DEVICE", HKS_TAG_ATTESTATION_ID_DEVICE);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_PRODUCT", HKS_TAG_ATTESTATION_ID_PRODUCT);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_SERIAL", HKS_TAG_ATTESTATION_ID_SERIAL);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_IMEI", HKS_TAG_ATTESTATION_ID_IMEI);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_MEID", HKS_TAG_ATTESTATION_ID_MEID);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_MANUFACTURER", HKS_TAG_ATTESTATION_ID_MANUFACTURER);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_MODEL", HKS_TAG_ATTESTATION_ID_MODEL);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_ALIAS", HKS_TAG_ATTESTATION_ID_ALIAS);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_SOCID", HKS_TAG_ATTESTATION_ID_SOCID);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_UDID", HKS_TAG_ATTESTATION_ID_UDID);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO", HKS_TAG_ATTESTATION_ID_SEC_LEVEL_INFO);
    AddInt32Property(env, tag, "HUKS_TAG_ATTESTATION_ID_VERSION_INFO", HKS_TAG_ATTESTATION_ID_VERSION_INFO);
}

static void AddHuksTagPart2(napi_env env, napi_value tag)
{
    /*
     * Other reserved TAG: 601 - 1000
     *
     * Extention TAG: 1001 - 9999
     */
    AddInt32Property(env, tag, "HUKS_TAG_IS_KEY_ALIAS", HKS_TAG_IS_KEY_ALIAS);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_STORAGE_FLAG", HKS_TAG_KEY_STORAGE_FLAG);
    AddInt32Property(env, tag, "HUKS_TAG_IS_ALLOWED_WRAP", HKS_TAG_IS_ALLOWED_WRAP);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_WRAP_TYPE", HKS_TAG_KEY_WRAP_TYPE);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_AUTH_ID", HKS_TAG_KEY_AUTH_ID);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_ROLE", HKS_TAG_KEY_ROLE);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_FLAG", HKS_TAG_KEY_FLAG);
    AddInt32Property(env, tag, "HUKS_TAG_IS_ASYNCHRONIZED", HKS_TAG_IS_ASYNCHRONIZED);
    AddInt32Property(env, tag, "HUKS_TAG_SECURE_KEY_ALIAS", HKS_TAG_SECURE_KEY_ALIAS);
    AddInt32Property(env, tag, "HUKS_TAG_SECURE_KEY_UUID", HKS_TAG_SECURE_KEY_UUID);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_DOMAIN", HKS_TAG_KEY_DOMAIN);

    /* Inner-use TAG: 10001 - 10999 */
    AddInt32Property(env, tag, "HUKS_TAG_PROCESS_NAME", HKS_TAG_PROCESS_NAME);
    AddInt32Property(env, tag, "HUKS_TAG_PACKAGE_NAME", HKS_TAG_PACKAGE_NAME);
    AddInt32Property(env, tag, "HUKS_TAG_ACCESS_TIME", HKS_TAG_ACCESS_TIME);
    AddInt32Property(env, tag, "HUKS_TAG_USES_TIME", HKS_TAG_USES_TIME);
    AddInt32Property(env, tag, "HUKS_TAG_CRYPTO_CTX", HKS_TAG_CRYPTO_CTX);
    AddInt32Property(env, tag, "HUKS_TAG_KEY", HKS_TAG_KEY);
    AddInt32Property(env, tag, "HUKS_TAG_KEY_VERSION", HKS_TAG_KEY_VERSION);
    AddInt32Property(env, tag, "HUKS_TAG_PAYLOAD_LEN", HKS_TAG_PAYLOAD_LEN);
    AddInt32Property(env, tag, "HUKS_TAG_AE_TAG", HKS_TAG_AE_TAG);
    AddInt32Property(env, tag, "HUKS_TAG_IS_KEY_HANDLE", HKS_TAG_IS_KEY_HANDLE);

    /* Os version related TAG */
    AddInt32Property(env, tag, "HUKS_TAG_OS_VERSION", HKS_TAG_OS_VERSION);
    AddInt32Property(env, tag, "HUKS_TAG_OS_PATCHLEVEL", HKS_TAG_OS_PATCHLEVEL);

    /*
     * Reversed TAGs for SOTER: 11000 - 12000
     *
     * Other TAGs: 20001 - N
     * TAGs used for paramSetOut
     */
    AddInt32Property(env, tag, "HUKS_TAG_SYMMETRIC_KEY_DATA", HKS_TAG_SYMMETRIC_KEY_DATA);
    AddInt32Property(env, tag, "HUKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA", HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA);
    AddInt32Property(env, tag, "HUKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA", HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA);
}

static napi_value CreateHuksTag(napi_env env)
{
    napi_value tag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &tag));

    AddHuksTagPart1(env, tag);
    AddHuksTagPart2(env, tag);

    return tag;
}

static void AddHksTagPart1(napi_env env, napi_value tag)
{
    /* Invalid TAG */
    AddInt32Property(env, tag, "HKS_TAG_INVALID", HKS_TAG_INVALID);

    /* Base algrithom TAG: 1 - 200 */
    AddInt32Property(env, tag, "HKS_TAG_ALGORITHM", HKS_TAG_ALGORITHM);
    AddInt32Property(env, tag, "HKS_TAG_PURPOSE", HKS_TAG_PURPOSE);
    AddInt32Property(env, tag, "HKS_TAG_KEY_SIZE", HKS_TAG_KEY_SIZE);
    AddInt32Property(env, tag, "HKS_TAG_DIGEST", HKS_TAG_DIGEST);
    AddInt32Property(env, tag, "HKS_TAG_PADDING", HKS_TAG_PADDING);
    AddInt32Property(env, tag, "HKS_TAG_BLOCK_MODE", HKS_TAG_BLOCK_MODE);
    AddInt32Property(env, tag, "HKS_TAG_KEY_TYPE", HKS_TAG_KEY_TYPE);
    AddInt32Property(env, tag, "HKS_TAG_ASSOCIATED_DATA", HKS_TAG_ASSOCIATED_DATA);
    AddInt32Property(env, tag, "HKS_TAG_NONCE", HKS_TAG_NONCE);
    AddInt32Property(env, tag, "HKS_TAG_IV", HKS_TAG_IV);

    /* Key derivation TAG */
    AddInt32Property(env, tag, "HKS_TAG_INFO", HKS_TAG_INFO);
    AddInt32Property(env, tag, "HKS_TAG_SALT", HKS_TAG_SALT);
    AddInt32Property(env, tag, "HKS_TAG_PWD", HKS_TAG_PWD);
    AddInt32Property(env, tag, "HKS_TAG_ITERATION", HKS_TAG_ITERATION);

    AddInt32Property(env, tag, "HKS_TAG_KEY_GENERATE_TYPE", HKS_TAG_KEY_GENERATE_TYPE);
    AddInt32Property(env, tag, "HKS_TAG_DERIVE_MAIN_KEY", HKS_TAG_DERIVE_MAIN_KEY);
    AddInt32Property(env, tag, "HKS_TAG_DERIVE_FACTOR", HKS_TAG_DERIVE_FACTOR);
    AddInt32Property(env, tag, "HKS_TAG_DERIVE_ALG", HKS_TAG_DERIVE_ALG);
    AddInt32Property(env, tag, "HKS_TAG_AGREE_ALG", HKS_TAG_AGREE_ALG);
    AddInt32Property(env, tag, "HKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS", HKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS);
    AddInt32Property(env, tag, "HKS_TAG_AGREE_PRIVATE_KEY_ALIAS", HKS_TAG_AGREE_PRIVATE_KEY_ALIAS);
    AddInt32Property(env, tag, "HKS_TAG_AGREE_PUBLIC_KEY", HKS_TAG_AGREE_PUBLIC_KEY);
    AddInt32Property(env, tag, "HKS_TAG_KEY_ALIAS", HKS_TAG_KEY_ALIAS);
    AddInt32Property(env, tag, "HKS_TAG_DERIVE_KEY_SIZE", HKS_TAG_DERIVE_KEY_SIZE);

    /*
     * Key authentication related TAG: 201 - 300
     *
     * Start of validity
     */
    AddInt32Property(env, tag, "HKS_TAG_ACTIVE_DATETIME", HKS_TAG_ACTIVE_DATETIME);

    /* Date when new "messages" should not be created. */
    AddInt32Property(env, tag, "HKS_TAG_ORIGINATION_EXPIRE_DATETIME", HKS_TAG_ORIGINATION_EXPIRE_DATETIME);

    /* Date when existing "messages" should not be used. */
    AddInt32Property(env, tag, "HKS_TAG_USAGE_EXPIRE_DATETIME", HKS_TAG_USAGE_EXPIRE_DATETIME);

    /* Key creation time */
    AddInt32Property(env, tag, "HKS_TAG_CREATION_DATETIME", HKS_TAG_CREATION_DATETIME);

    /* Other authentication related TAG: 301 - 500 */
    AddInt32Property(env, tag, "HKS_TAG_ALL_USERS", HKS_TAG_ALL_USERS);
    AddInt32Property(env, tag, "HKS_TAG_USER_ID", HKS_TAG_USER_ID);
    AddInt32Property(env, tag, "HKS_TAG_NO_AUTH_REQUIRED", HKS_TAG_NO_AUTH_REQUIRED);
    AddInt32Property(env, tag, "HKS_TAG_USER_AUTH_TYPE", HKS_TAG_USER_AUTH_TYPE);
    AddInt32Property(env, tag, "HKS_TAG_AUTH_TIMEOUT", HKS_TAG_AUTH_TIMEOUT);
    AddInt32Property(env, tag, "HKS_TAG_AUTH_TOKEN", HKS_TAG_AUTH_TOKEN);

    /* Attestation related TAG: 501 - 600 */
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_CHALLENGE", HKS_TAG_ATTESTATION_CHALLENGE);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_APPLICATION_ID", HKS_TAG_ATTESTATION_APPLICATION_ID);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_ID_BRAND", HKS_TAG_ATTESTATION_ID_BRAND);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_ID_DEVICE", HKS_TAG_ATTESTATION_ID_DEVICE);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_ID_PRODUCT", HKS_TAG_ATTESTATION_ID_PRODUCT);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_ID_SERIAL", HKS_TAG_ATTESTATION_ID_SERIAL);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_ID_IMEI", HKS_TAG_ATTESTATION_ID_IMEI);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_ID_MEID", HKS_TAG_ATTESTATION_ID_MEID);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_ID_MANUFACTURER", HKS_TAG_ATTESTATION_ID_MANUFACTURER);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_ID_MODEL", HKS_TAG_ATTESTATION_ID_MODEL);
    AddInt32Property(env, tag, "HKS_TAG_ATTESTATION_ID_ALIAS", HKS_TAG_ATTESTATION_ID_ALIAS);
}

static void AddHksTagPart2(napi_env env, napi_value tag)
{
    /*
     * Other reserved TAG: 601 - 1000
     *
     * Extention TAG: 1001 - 9999
     */
    AddInt32Property(env, tag, "HKS_TAG_IS_KEY_ALIAS", HKS_TAG_IS_KEY_ALIAS);
    AddInt32Property(env, tag, "HKS_TAG_KEY_STORAGE_FLAG", HKS_TAG_KEY_STORAGE_FLAG);
    AddInt32Property(env, tag, "HKS_TAG_IS_ALLOWED_WRAP", HKS_TAG_IS_ALLOWED_WRAP);
    AddInt32Property(env, tag, "HKS_TAG_KEY_WRAP_TYPE", HKS_TAG_KEY_WRAP_TYPE);
    AddInt32Property(env, tag, "HKS_TAG_KEY_AUTH_ID", HKS_TAG_KEY_AUTH_ID);
    AddInt32Property(env, tag, "HKS_TAG_KEY_ROLE", HKS_TAG_KEY_ROLE);
    AddInt32Property(env, tag, "HKS_TAG_KEY_FLAG", HKS_TAG_KEY_FLAG);
    AddInt32Property(env, tag, "HKS_TAG_IS_ASYNCHRONIZED", HKS_TAG_IS_ASYNCHRONIZED);
    AddInt32Property(env, tag, "HKS_TAG_SECURE_KEY_ALIAS", HKS_TAG_SECURE_KEY_ALIAS);
    AddInt32Property(env, tag, "HKS_TAG_SECURE_KEY_UUID", HKS_TAG_SECURE_KEY_UUID);
    AddInt32Property(env, tag, "HKS_TAG_KEY_DOMAIN", HKS_TAG_KEY_DOMAIN);

    /* Inner-use TAG: 10001 - 10999 */
    AddInt32Property(env, tag, "HKS_TAG_PROCESS_NAME", HKS_TAG_PROCESS_NAME);
    AddInt32Property(env, tag, "HKS_TAG_PACKAGE_NAME", HKS_TAG_PACKAGE_NAME);
    AddInt32Property(env, tag, "HKS_TAG_ACCESS_TIME", HKS_TAG_ACCESS_TIME);
    AddInt32Property(env, tag, "HKS_TAG_USES_TIME", HKS_TAG_USES_TIME);
    AddInt32Property(env, tag, "HKS_TAG_CRYPTO_CTX", HKS_TAG_CRYPTO_CTX);
    AddInt32Property(env, tag, "HKS_TAG_KEY", HKS_TAG_KEY);
    AddInt32Property(env, tag, "HKS_TAG_KEY_VERSION", HKS_TAG_KEY_VERSION);
    AddInt32Property(env, tag, "HKS_TAG_PAYLOAD_LEN", HKS_TAG_PAYLOAD_LEN);
    AddInt32Property(env, tag, "HKS_TAG_AE_TAG", HKS_TAG_AE_TAG);
    AddInt32Property(env, tag, "HKS_TAG_IS_KEY_HANDLE", HKS_TAG_IS_KEY_HANDLE);

    /* Os version related TAG */
    AddInt32Property(env, tag, "HKS_TAG_OS_VERSION", HKS_TAG_OS_VERSION);
    AddInt32Property(env, tag, "HKS_TAG_OS_PATCHLEVEL", HKS_TAG_OS_PATCHLEVEL);

    /*
     * Reversed TAGs for SOTER: 11000 - 12000
     *
     * Other TAGs: 20001 - N
     * TAGs used for paramSetOut
     */
    AddInt32Property(env, tag, "HKS_TAG_SYMMETRIC_KEY_DATA", HKS_TAG_SYMMETRIC_KEY_DATA);
    AddInt32Property(env, tag, "HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA", HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA);
    AddInt32Property(env, tag, "HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA", HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA);
}

static napi_value CreateHksTag(napi_env env)
{
    napi_value tag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &tag));

    AddHksTagPart1(env, tag);
    AddHksTagPart2(env, tag);

    return tag;
}

static napi_value CreateHuksKeySize(napi_env env)
{
    napi_value keySize = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keySize));

    AddInt32Property(env, keySize, "HUKS_RSA_KEY_SIZE_512", HKS_RSA_KEY_SIZE_512);
    AddInt32Property(env, keySize, "HUKS_RSA_KEY_SIZE_768", HKS_RSA_KEY_SIZE_768);
    AddInt32Property(env, keySize, "HUKS_RSA_KEY_SIZE_1024", HKS_RSA_KEY_SIZE_1024);
    AddInt32Property(env, keySize, "HUKS_RSA_KEY_SIZE_2048", HKS_RSA_KEY_SIZE_2048);
    AddInt32Property(env, keySize, "HUKS_RSA_KEY_SIZE_3072", HKS_RSA_KEY_SIZE_3072);
    AddInt32Property(env, keySize, "HUKS_RSA_KEY_SIZE_4096", HKS_RSA_KEY_SIZE_4096);

    AddInt32Property(env, keySize, "HUKS_ECC_KEY_SIZE_224", HKS_ECC_KEY_SIZE_224);
    AddInt32Property(env, keySize, "HUKS_ECC_KEY_SIZE_256", HKS_ECC_KEY_SIZE_256);
    AddInt32Property(env, keySize, "HUKS_ECC_KEY_SIZE_384", HKS_ECC_KEY_SIZE_384);
    AddInt32Property(env, keySize, "HUKS_ECC_KEY_SIZE_521", HKS_ECC_KEY_SIZE_521);

    AddInt32Property(env, keySize, "HUKS_AES_KEY_SIZE_128", HKS_AES_KEY_SIZE_128);
    AddInt32Property(env, keySize, "HUKS_AES_KEY_SIZE_192", HKS_AES_KEY_SIZE_192);
    AddInt32Property(env, keySize, "HUKS_AES_KEY_SIZE_256", HKS_AES_KEY_SIZE_256);
    AddInt32Property(env, keySize, "HUKS_AES_KEY_SIZE_512", HKS_AES_KEY_SIZE_512);

    AddInt32Property(env, keySize, "HUKS_CURVE25519_KEY_SIZE_256", HKS_CURVE25519_KEY_SIZE_256);

    AddInt32Property(env, keySize, "HUKS_DH_KEY_SIZE_2048", HKS_DH_KEY_SIZE_2048);
    AddInt32Property(env, keySize, "HUKS_DH_KEY_SIZE_3072", HKS_DH_KEY_SIZE_3072);
    AddInt32Property(env, keySize, "HUKS_DH_KEY_SIZE_4096", HKS_DH_KEY_SIZE_4096);

    return keySize;
}

static napi_value CreateHksKeySize(napi_env env)
{
    napi_value keySize = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keySize));

    AddInt32Property(env, keySize, "HKS_RSA_KEY_SIZE_512", HKS_RSA_KEY_SIZE_512);
    AddInt32Property(env, keySize, "HKS_RSA_KEY_SIZE_768", HKS_RSA_KEY_SIZE_768);
    AddInt32Property(env, keySize, "HKS_RSA_KEY_SIZE_1024", HKS_RSA_KEY_SIZE_1024);
    AddInt32Property(env, keySize, "HKS_RSA_KEY_SIZE_2048", HKS_RSA_KEY_SIZE_2048);
    AddInt32Property(env, keySize, "HKS_RSA_KEY_SIZE_3072", HKS_RSA_KEY_SIZE_3072);
    AddInt32Property(env, keySize, "HKS_RSA_KEY_SIZE_4096", HKS_RSA_KEY_SIZE_4096);

    AddInt32Property(env, keySize, "HKS_ECC_KEY_SIZE_224", HKS_ECC_KEY_SIZE_224);
    AddInt32Property(env, keySize, "HKS_ECC_KEY_SIZE_256", HKS_ECC_KEY_SIZE_256);
    AddInt32Property(env, keySize, "HKS_ECC_KEY_SIZE_384", HKS_ECC_KEY_SIZE_384);
    AddInt32Property(env, keySize, "HKS_ECC_KEY_SIZE_521", HKS_ECC_KEY_SIZE_521);

    AddInt32Property(env, keySize, "HKS_AES_KEY_SIZE_128", HKS_AES_KEY_SIZE_128);
    AddInt32Property(env, keySize, "HKS_AES_KEY_SIZE_192", HKS_AES_KEY_SIZE_192);
    AddInt32Property(env, keySize, "HKS_AES_KEY_SIZE_256", HKS_AES_KEY_SIZE_256);
    AddInt32Property(env, keySize, "HKS_AES_KEY_SIZE_512", HKS_AES_KEY_SIZE_512);

    AddInt32Property(env, keySize, "HKS_CURVE25519_KEY_SIZE_256", HKS_CURVE25519_KEY_SIZE_256);

    AddInt32Property(env, keySize, "HKS_DH_KEY_SIZE_2048", HKS_DH_KEY_SIZE_2048);
    AddInt32Property(env, keySize, "HKS_DH_KEY_SIZE_3072", HKS_DH_KEY_SIZE_3072);
    AddInt32Property(env, keySize, "HKS_DH_KEY_SIZE_4096", HKS_DH_KEY_SIZE_4096);

    return keySize;
}

static napi_value CreateHuksKeyAlg(napi_env env)
{
    napi_value keyAlg = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyAlg));

    AddInt32Property(env, keyAlg, "HUKS_ALG_RSA", HKS_ALG_RSA);
    AddInt32Property(env, keyAlg, "HUKS_ALG_ECC", HKS_ALG_ECC);
    AddInt32Property(env, keyAlg, "HUKS_ALG_DSA", HKS_ALG_DSA);

    AddInt32Property(env, keyAlg, "HUKS_ALG_AES", HKS_ALG_AES);
    AddInt32Property(env, keyAlg, "HUKS_ALG_HMAC", HKS_ALG_HMAC);
    AddInt32Property(env, keyAlg, "HUKS_ALG_HKDF", HKS_ALG_HKDF);
    AddInt32Property(env, keyAlg, "HUKS_ALG_PBKDF2", HKS_ALG_PBKDF2);

    AddInt32Property(env, keyAlg, "HUKS_ALG_ECDH", HKS_ALG_ECDH);
    AddInt32Property(env, keyAlg, "HUKS_ALG_X25519", HKS_ALG_X25519);
    AddInt32Property(env, keyAlg, "HUKS_ALG_ED25519", HKS_ALG_ED25519);
    AddInt32Property(env, keyAlg, "HUKS_ALG_DH", HKS_ALG_DH);

    return keyAlg;
}

static napi_value CreateHksKeyAlg(napi_env env)
{
    napi_value keyAlg = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyAlg));

    AddInt32Property(env, keyAlg, "HKS_ALG_RSA", HKS_ALG_RSA);
    AddInt32Property(env, keyAlg, "HKS_ALG_ECC", HKS_ALG_ECC);
    AddInt32Property(env, keyAlg, "HKS_ALG_DSA", HKS_ALG_DSA);

    AddInt32Property(env, keyAlg, "HKS_ALG_AES", HKS_ALG_AES);
    AddInt32Property(env, keyAlg, "HKS_ALG_HMAC", HKS_ALG_HMAC);
    AddInt32Property(env, keyAlg, "HKS_ALG_HKDF", HKS_ALG_HKDF);
    AddInt32Property(env, keyAlg, "HKS_ALG_PBKDF2", HKS_ALG_PBKDF2);

    AddInt32Property(env, keyAlg, "HKS_ALG_ECDH", HKS_ALG_ECDH);
    AddInt32Property(env, keyAlg, "HKS_ALG_X25519", HKS_ALG_X25519);
    AddInt32Property(env, keyAlg, "HKS_ALG_ED25519", HKS_ALG_ED25519);
    AddInt32Property(env, keyAlg, "HKS_ALG_DH", HKS_ALG_DH);

    return keyAlg;
}

static napi_value CreateHuksKeyPurpose(napi_env env)
{
    napi_value keyPurpose = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyPurpose));

    AddInt32Property(env, keyPurpose, "HUKS_KEY_PURPOSE_ENCRYPT", HKS_KEY_PURPOSE_ENCRYPT);
    AddInt32Property(env, keyPurpose, "HUKS_KEY_PURPOSE_DECRYPT", HKS_KEY_PURPOSE_DECRYPT);
    AddInt32Property(env, keyPurpose, "HUKS_KEY_PURPOSE_SIGN", HKS_KEY_PURPOSE_SIGN);
    AddInt32Property(env, keyPurpose, "HUKS_KEY_PURPOSE_VERIFY", HKS_KEY_PURPOSE_VERIFY);
    AddInt32Property(env, keyPurpose, "HUKS_KEY_PURPOSE_DERIVE", HKS_KEY_PURPOSE_DERIVE);
    AddInt32Property(env, keyPurpose, "HUKS_KEY_PURPOSE_WRAP", HKS_KEY_PURPOSE_WRAP);
    AddInt32Property(env, keyPurpose, "HUKS_KEY_PURPOSE_UNWRAP", HKS_KEY_PURPOSE_UNWRAP);
    AddInt32Property(env, keyPurpose, "HUKS_KEY_PURPOSE_MAC", HKS_KEY_PURPOSE_MAC);
    AddInt32Property(env, keyPurpose, "HUKS_KEY_PURPOSE_AGREE", HKS_KEY_PURPOSE_AGREE);

    return keyPurpose;
}

static napi_value CreateHksKeyPurpose(napi_env env)
{
    napi_value keyPurpose = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyPurpose));

    AddInt32Property(env, keyPurpose, "HKS_KEY_PURPOSE_ENCRYPT", HKS_KEY_PURPOSE_ENCRYPT);
    AddInt32Property(env, keyPurpose, "HKS_KEY_PURPOSE_DECRYPT", HKS_KEY_PURPOSE_DECRYPT);
    AddInt32Property(env, keyPurpose, "HKS_KEY_PURPOSE_SIGN", HKS_KEY_PURPOSE_SIGN);
    AddInt32Property(env, keyPurpose, "HKS_KEY_PURPOSE_VERIFY", HKS_KEY_PURPOSE_VERIFY);
    AddInt32Property(env, keyPurpose, "HKS_KEY_PURPOSE_DERIVE", HKS_KEY_PURPOSE_DERIVE);
    AddInt32Property(env, keyPurpose, "HKS_KEY_PURPOSE_WRAP", HKS_KEY_PURPOSE_WRAP);
    AddInt32Property(env, keyPurpose, "HKS_KEY_PURPOSE_UNWRAP", HKS_KEY_PURPOSE_UNWRAP);
    AddInt32Property(env, keyPurpose, "HKS_KEY_PURPOSE_MAC", HKS_KEY_PURPOSE_MAC);
    AddInt32Property(env, keyPurpose, "HKS_KEY_PURPOSE_AGREE", HKS_KEY_PURPOSE_AGREE);

    return keyPurpose;
}

static napi_value CreateHuksKeyPadding(napi_env env)
{
    napi_value keyPadding = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyPadding));

    AddInt32Property(env, keyPadding, "HUKS_PADDING_NONE", HKS_PADDING_NONE);
    AddInt32Property(env, keyPadding, "HUKS_PADDING_OAEP", HKS_PADDING_OAEP);
    AddInt32Property(env, keyPadding, "HUKS_PADDING_PSS", HKS_PADDING_PSS);
    AddInt32Property(env, keyPadding, "HUKS_PADDING_PKCS1_V1_5", HKS_PADDING_PKCS1_V1_5);
    AddInt32Property(env, keyPadding, "HUKS_PADDING_PKCS5", HKS_PADDING_PKCS5);
    AddInt32Property(env, keyPadding, "HUKS_PADDING_PKCS7", HKS_PADDING_PKCS7);

    return keyPadding;
}

static napi_value CreateHksKeyPadding(napi_env env)
{
    napi_value keyPadding = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyPadding));

    AddInt32Property(env, keyPadding, "HKS_PADDING_NONE", HKS_PADDING_NONE);
    AddInt32Property(env, keyPadding, "HKS_PADDING_OAEP", HKS_PADDING_OAEP);
    AddInt32Property(env, keyPadding, "HKS_PADDING_PSS", HKS_PADDING_PSS);
    AddInt32Property(env, keyPadding, "HKS_PADDING_PKCS1_V1_5", HKS_PADDING_PKCS1_V1_5);
    AddInt32Property(env, keyPadding, "HKS_PADDING_PKCS5", HKS_PADDING_PKCS5);
    AddInt32Property(env, keyPadding, "HKS_PADDING_PKCS7", HKS_PADDING_PKCS7);

    return keyPadding;
}

static napi_value CreateHuksCipherMode(napi_env env)
{
    napi_value keyCipherMode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyCipherMode));

    AddInt32Property(env, keyCipherMode, "HUKS_MODE_ECB", HKS_MODE_ECB);
    AddInt32Property(env, keyCipherMode, "HUKS_MODE_CBC", HKS_MODE_CBC);
    AddInt32Property(env, keyCipherMode, "HUKS_MODE_CTR", HKS_MODE_CTR);
    AddInt32Property(env, keyCipherMode, "HUKS_MODE_OFB", HKS_MODE_OFB);
    AddInt32Property(env, keyCipherMode, "HUKS_MODE_CCM", HKS_MODE_CCM);
    AddInt32Property(env, keyCipherMode, "HUKS_MODE_GCM", HKS_MODE_GCM);

    return keyCipherMode;
}

static napi_value CreateHksCipherMode(napi_env env)
{
    napi_value keyCipherMode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyCipherMode));

    AddInt32Property(env, keyCipherMode, "HKS_MODE_ECB", HKS_MODE_ECB);
    AddInt32Property(env, keyCipherMode, "HKS_MODE_CBC", HKS_MODE_CBC);
    AddInt32Property(env, keyCipherMode, "HKS_MODE_CTR", HKS_MODE_CTR);
    AddInt32Property(env, keyCipherMode, "HKS_MODE_OFB", HKS_MODE_OFB);
    AddInt32Property(env, keyCipherMode, "HKS_MODE_CCM", HKS_MODE_CCM);
    AddInt32Property(env, keyCipherMode, "HKS_MODE_GCM", HKS_MODE_GCM);

    return keyCipherMode;
}

static napi_value CreateHuksKeyStorageType(napi_env env)
{
    napi_value keyStorageType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyStorageType));

    AddInt32Property(env, keyStorageType, "HUKS_STORAGE_TEMP", HKS_STORAGE_TEMP);
    AddInt32Property(env, keyStorageType, "HUKS_STORAGE_PERSISTENT", HKS_STORAGE_PERSISTENT);

    return keyStorageType;
}

static napi_value CreateHksKeyStorageType(napi_env env)
{
    napi_value keyStorageType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyStorageType));

    AddInt32Property(env, keyStorageType, "HKS_STORAGE_TEMP", HKS_STORAGE_TEMP);
    AddInt32Property(env, keyStorageType, "HKS_STORAGE_PERSISTENT", HKS_STORAGE_PERSISTENT);

    return keyStorageType;
}

static void AddHuksErrorCodePart1(napi_env env, napi_value errorCode)
{
    AddInt32Property(env, errorCode, "HUKS_SUCCESS", HKS_SUCCESS);
    AddInt32Property(env, errorCode, "HUKS_FAILURE", HKS_FAILURE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_BAD_STATE", HKS_ERROR_BAD_STATE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_ARGUMENT", HKS_ERROR_INVALID_ARGUMENT);
    AddInt32Property(env, errorCode, "HUKS_ERROR_NOT_SUPPORTED", HKS_ERROR_NOT_SUPPORTED);
    AddInt32Property(env, errorCode, "HUKS_ERROR_NO_PERMISSION", HKS_ERROR_NO_PERMISSION);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INSUFFICIENT_DATA", HKS_ERROR_INSUFFICIENT_DATA);
    AddInt32Property(env, errorCode, "HUKS_ERROR_BUFFER_TOO_SMALL", HKS_ERROR_BUFFER_TOO_SMALL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INSUFFICIENT_MEMORY", HKS_ERROR_INSUFFICIENT_MEMORY);
    AddInt32Property(env, errorCode, "HUKS_ERROR_COMMUNICATION_FAILURE", HKS_ERROR_COMMUNICATION_FAILURE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_STORAGE_FAILURE", HKS_ERROR_STORAGE_FAILURE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_HARDWARE_FAILURE", HKS_ERROR_HARDWARE_FAILURE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_ALREADY_EXISTS", HKS_ERROR_ALREADY_EXISTS);
    AddInt32Property(env, errorCode, "HUKS_ERROR_NOT_EXIST", HKS_ERROR_NOT_EXIST);
    AddInt32Property(env, errorCode, "HUKS_ERROR_NULL_POINTER", HKS_ERROR_NULL_POINTER);
    AddInt32Property(env, errorCode, "HUKS_ERROR_FILE_SIZE_FAIL", HKS_ERROR_FILE_SIZE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_READ_FILE_FAIL", HKS_ERROR_READ_FILE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_PUBLIC_KEY", HKS_ERROR_INVALID_PUBLIC_KEY);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_PRIVATE_KEY", HKS_ERROR_INVALID_PRIVATE_KEY);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_KEY_INFO", HKS_ERROR_INVALID_KEY_INFO);
    AddInt32Property(env, errorCode, "HUKS_ERROR_HASH_NOT_EQUAL", HKS_ERROR_HASH_NOT_EQUAL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_MALLOC_FAIL", HKS_ERROR_MALLOC_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_WRITE_FILE_FAIL", HKS_ERROR_WRITE_FILE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_REMOVE_FILE_FAIL", HKS_ERROR_REMOVE_FILE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_OPEN_FILE_FAIL", HKS_ERROR_OPEN_FILE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CLOSE_FILE_FAIL", HKS_ERROR_CLOSE_FILE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_MAKE_DIR_FAIL", HKS_ERROR_MAKE_DIR_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_KEY_FILE", HKS_ERROR_INVALID_KEY_FILE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_IPC_MSG_FAIL", HKS_ERROR_IPC_MSG_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_REQUEST_OVERFLOWS", HKS_ERROR_REQUEST_OVERFLOWS);
    AddInt32Property(env, errorCode, "HUKS_ERROR_PARAM_NOT_EXIST", HKS_ERROR_PARAM_NOT_EXIST);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CRYPTO_ENGINE_ERROR", HKS_ERROR_CRYPTO_ENGINE_ERROR);
    AddInt32Property(env, errorCode, "HUKS_ERROR_COMMUNICATION_TIMEOUT", HKS_ERROR_COMMUNICATION_TIMEOUT);
    AddInt32Property(env, errorCode, "HUKS_ERROR_IPC_INIT_FAIL", HKS_ERROR_IPC_INIT_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_IPC_DLOPEN_FAIL", HKS_ERROR_IPC_DLOPEN_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_EFUSE_READ_FAIL", HKS_ERROR_EFUSE_READ_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_NEW_ROOT_KEY_MATERIAL_EXIST", HKS_ERROR_NEW_ROOT_KEY_MATERIAL_EXIST);
    AddInt32Property(
        env, errorCode, "HUKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL", HKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_VERIFICATION_FAILED", HKS_ERROR_VERIFICATION_FAILED);
}

static void AddHuksErrorCodePart2(napi_env env, napi_value errorCode)
{
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_ALG_FAIL", HKS_ERROR_CHECK_GET_ALG_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_KEY_SIZE_FAIL", HKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_PADDING_FAIL", HKS_ERROR_CHECK_GET_PADDING_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_PURPOSE_FAIL", HKS_ERROR_CHECK_GET_PURPOSE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_DIGEST_FAIL", HKS_ERROR_CHECK_GET_DIGEST_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_MODE_FAIL", HKS_ERROR_CHECK_GET_MODE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_NONCE_FAIL", HKS_ERROR_CHECK_GET_NONCE_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_AAD_FAIL", HKS_ERROR_CHECK_GET_AAD_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_IV_FAIL", HKS_ERROR_CHECK_GET_IV_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_AE_TAG_FAIL", HKS_ERROR_CHECK_GET_AE_TAG_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_SALT_FAIL", HKS_ERROR_CHECK_GET_SALT_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_CHECK_GET_ITERATION_FAIL", HKS_ERROR_CHECK_GET_ITERATION_FAIL);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_ALGORITHM", HKS_ERROR_INVALID_ALGORITHM);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_KEY_SIZE", HKS_ERROR_INVALID_KEY_SIZE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_PADDING", HKS_ERROR_INVALID_PADDING);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_PURPOSE", HKS_ERROR_INVALID_PURPOSE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_MODE", HKS_ERROR_INVALID_MODE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_DIGEST", HKS_ERROR_INVALID_DIGEST);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_SIGNATURE_SIZE", HKS_ERROR_INVALID_SIGNATURE_SIZE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_IV", HKS_ERROR_INVALID_IV);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_AAD", HKS_ERROR_INVALID_AAD);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_NONCE", HKS_ERROR_INVALID_NONCE);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_AE_TAG", HKS_ERROR_INVALID_AE_TAG);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_SALT", HKS_ERROR_INVALID_SALT);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_ITERATION", HKS_ERROR_INVALID_ITERATION);
    AddInt32Property(env, errorCode, "HUKS_ERROR_INVALID_OPERATION", HKS_ERROR_INVALID_OPERATION);

    AddInt32Property(env, errorCode, "HUKS_ERROR_INTERNAL_ERROR", HKS_ERROR_INTERNAL_ERROR);
    AddInt32Property(env, errorCode, "HUKS_ERROR_UNKNOWN_ERROR", HKS_ERROR_UNKNOWN_ERROR);
}

static napi_value CreateHuksErrorCode(napi_env env)
{
    napi_value errorCode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &errorCode));

    AddHuksErrorCodePart1(env, errorCode);
    AddHuksErrorCodePart2(env, errorCode);

    return errorCode;
}

static void AddHksErrorCodePart1(napi_env env, napi_value errorCode)
{
    AddInt32Property(env, errorCode, "HKS_SUCCESS", HKS_SUCCESS);
    AddInt32Property(env, errorCode, "HKS_FAILURE", HKS_FAILURE);
    AddInt32Property(env, errorCode, "HKS_ERROR_BAD_STATE", HKS_ERROR_BAD_STATE);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_ARGUMENT", HKS_ERROR_INVALID_ARGUMENT);
    AddInt32Property(env, errorCode, "HKS_ERROR_NOT_SUPPORTED", HKS_ERROR_NOT_SUPPORTED);
    AddInt32Property(env, errorCode, "HKS_ERROR_NO_PERMISSION", HKS_ERROR_NO_PERMISSION);
    AddInt32Property(env, errorCode, "HKS_ERROR_INSUFFICIENT_DATA", HKS_ERROR_INSUFFICIENT_DATA);
    AddInt32Property(env, errorCode, "HKS_ERROR_BUFFER_TOO_SMALL", HKS_ERROR_BUFFER_TOO_SMALL);
    AddInt32Property(env, errorCode, "HKS_ERROR_INSUFFICIENT_MEMORY", HKS_ERROR_INSUFFICIENT_MEMORY);
    AddInt32Property(env, errorCode, "HKS_ERROR_COMMUNICATION_FAILURE", HKS_ERROR_COMMUNICATION_FAILURE);
    AddInt32Property(env, errorCode, "HKS_ERROR_STORAGE_FAILURE", HKS_ERROR_STORAGE_FAILURE);
    AddInt32Property(env, errorCode, "HKS_ERROR_HARDWARE_FAILURE", HKS_ERROR_HARDWARE_FAILURE);
    AddInt32Property(env, errorCode, "HKS_ERROR_ALREADY_EXISTS", HKS_ERROR_ALREADY_EXISTS);
    AddInt32Property(env, errorCode, "HKS_ERROR_NOT_EXIST", HKS_ERROR_NOT_EXIST);
    AddInt32Property(env, errorCode, "HKS_ERROR_NULL_POINTER", HKS_ERROR_NULL_POINTER);
    AddInt32Property(env, errorCode, "HKS_ERROR_FILE_SIZE_FAIL", HKS_ERROR_FILE_SIZE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_READ_FILE_FAIL", HKS_ERROR_READ_FILE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_PUBLIC_KEY", HKS_ERROR_INVALID_PUBLIC_KEY);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_PRIVATE_KEY", HKS_ERROR_INVALID_PRIVATE_KEY);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_KEY_INFO", HKS_ERROR_INVALID_KEY_INFO);
    AddInt32Property(env, errorCode, "HKS_ERROR_HASH_NOT_EQUAL", HKS_ERROR_HASH_NOT_EQUAL);
    AddInt32Property(env, errorCode, "HKS_ERROR_MALLOC_FAIL", HKS_ERROR_MALLOC_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_WRITE_FILE_FAIL", HKS_ERROR_WRITE_FILE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_REMOVE_FILE_FAIL", HKS_ERROR_REMOVE_FILE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_OPEN_FILE_FAIL", HKS_ERROR_OPEN_FILE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CLOSE_FILE_FAIL", HKS_ERROR_CLOSE_FILE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_MAKE_DIR_FAIL", HKS_ERROR_MAKE_DIR_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_KEY_FILE", HKS_ERROR_INVALID_KEY_FILE);
    AddInt32Property(env, errorCode, "HKS_ERROR_IPC_MSG_FAIL", HKS_ERROR_IPC_MSG_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_REQUEST_OVERFLOWS", HKS_ERROR_REQUEST_OVERFLOWS);
    AddInt32Property(env, errorCode, "HKS_ERROR_PARAM_NOT_EXIST", HKS_ERROR_PARAM_NOT_EXIST);
    AddInt32Property(env, errorCode, "HKS_ERROR_CRYPTO_ENGINE_ERROR", HKS_ERROR_CRYPTO_ENGINE_ERROR);
    AddInt32Property(env, errorCode, "HKS_ERROR_COMMUNICATION_TIMEOUT", HKS_ERROR_COMMUNICATION_TIMEOUT);
    AddInt32Property(env, errorCode, "HKS_ERROR_IPC_INIT_FAIL", HKS_ERROR_IPC_INIT_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_IPC_DLOPEN_FAIL", HKS_ERROR_IPC_DLOPEN_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_EFUSE_READ_FAIL", HKS_ERROR_EFUSE_READ_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_NEW_ROOT_KEY_MATERIAL_EXIST", HKS_ERROR_NEW_ROOT_KEY_MATERIAL_EXIST);
    AddInt32Property(
        env, errorCode, "HKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL", HKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL);
}

static void AddHksErrorCodePart2(napi_env env, napi_value errorCode)
{
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_ALG_FAIL", HKS_ERROR_CHECK_GET_ALG_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_KEY_SIZE_FAIL", HKS_ERROR_CHECK_GET_KEY_SIZE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_PADDING_FAIL", HKS_ERROR_CHECK_GET_PADDING_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_PURPOSE_FAIL", HKS_ERROR_CHECK_GET_PURPOSE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_DIGEST_FAIL", HKS_ERROR_CHECK_GET_DIGEST_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_MODE_FAIL", HKS_ERROR_CHECK_GET_MODE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_NONCE_FAIL", HKS_ERROR_CHECK_GET_NONCE_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_AAD_FAIL", HKS_ERROR_CHECK_GET_AAD_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_IV_FAIL", HKS_ERROR_CHECK_GET_IV_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_AE_TAG_FAIL", HKS_ERROR_CHECK_GET_AE_TAG_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_SALT_FAIL", HKS_ERROR_CHECK_GET_SALT_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_CHECK_GET_ITERATION_FAIL", HKS_ERROR_CHECK_GET_ITERATION_FAIL);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_ALGORITHM", HKS_ERROR_INVALID_ALGORITHM);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_KEY_SIZE", HKS_ERROR_INVALID_KEY_SIZE);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_PADDING", HKS_ERROR_INVALID_PADDING);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_PURPOSE", HKS_ERROR_INVALID_PURPOSE);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_MODE", HKS_ERROR_INVALID_MODE);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_DIGEST", HKS_ERROR_INVALID_DIGEST);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_SIGNATURE_SIZE", HKS_ERROR_INVALID_SIGNATURE_SIZE);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_IV", HKS_ERROR_INVALID_IV);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_AAD", HKS_ERROR_INVALID_AAD);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_NONCE", HKS_ERROR_INVALID_NONCE);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_AE_TAG", HKS_ERROR_INVALID_AE_TAG);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_SALT", HKS_ERROR_INVALID_SALT);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_ITERATION", HKS_ERROR_INVALID_ITERATION);
    AddInt32Property(env, errorCode, "HKS_ERROR_INVALID_OPERATION", HKS_ERROR_INVALID_OPERATION);

    AddInt32Property(env, errorCode, "HKS_ERROR_INTERNAL_ERROR", HKS_ERROR_INTERNAL_ERROR);
    AddInt32Property(env, errorCode, "HKS_ERROR_UNKNOWN_ERROR", HKS_ERROR_UNKNOWN_ERROR);
}

static napi_value CreateHksErrorCode(napi_env env)
{
    napi_value errorCode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &errorCode));

    AddHksErrorCodePart1(env, errorCode);
    AddHksErrorCodePart2(env, errorCode);

    return errorCode;
}

static napi_value CreateHksKeyType(napi_env env)
{
    napi_value keyType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyType));

    AddInt32Property(env, keyType, "HKS_KEY_TYPE_RSA_PUBLIC_KEY", HKS_KEY_TYPE_RSA_PUBLIC_KEY);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_RSA_KEYPAIR", HKS_KEY_TYPE_RSA_KEYPAIR);

    AddInt32Property(env, keyType, "HKS_KEY_TYPE_ECC_P256_PUBLIC_KEY", HKS_KEY_TYPE_RSA_KEYPAIR);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_ECC_P256_KEYPAIR", HKS_KEY_TYPE_RSA_KEYPAIR);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_ECC_P384_PUBLIC_KEY", HKS_KEY_TYPE_RSA_KEYPAIR);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_ECC_P384_KEYPAIR", HKS_KEY_TYPE_RSA_KEYPAIR);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_ECC_P521_PUBLIC_KEY", HKS_KEY_TYPE_RSA_KEYPAIR);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_ECC_P521_KEYPAIR", HKS_KEY_TYPE_RSA_KEYPAIR);

    AddInt32Property(env, keyType, "HKS_KEY_TYPE_ED25519_PUBLIC_KEY", HKS_KEY_TYPE_ED25519_PUBLIC_KEY);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_ED25519_KEYPAIR", HKS_KEY_TYPE_ED25519_KEYPAIR);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_X25519_PUBLIC_KEY", HKS_KEY_TYPE_X25519_PUBLIC_KEY);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_X25519_KEYPAIR", HKS_KEY_TYPE_X25519_KEYPAIR);

    AddInt32Property(env, keyType, "HKS_KEY_TYPE_AES", HKS_KEY_TYPE_AES);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_CHACHA20", HKS_KEY_TYPE_CHACHA20);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_CHACHA20_POLY1305", HKS_KEY_TYPE_CHACHA20_POLY1305);

    AddInt32Property(env, keyType, "HKS_KEY_TYPE_HMAC", HKS_KEY_TYPE_HMAC);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_HKDF", HKS_KEY_TYPE_HKDF);
    AddInt32Property(env, keyType, "HKS_KEY_TYPE_PBKDF2", HKS_KEY_TYPE_PBKDF2);

    return keyType;
}

static napi_value CreateHuksKeyDigest(napi_env env)
{
    napi_value keyDisgest = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyDisgest));

    AddInt32Property(env, keyDisgest, "HUKS_DIGEST_NONE", HKS_DIGEST_NONE);
    AddInt32Property(env, keyDisgest, "HUKS_DIGEST_MD5", HKS_DIGEST_MD5);
    AddInt32Property(env, keyDisgest, "HUKS_DIGEST_SHA1", HKS_DIGEST_SHA1);
    AddInt32Property(env, keyDisgest, "HUKS_DIGEST_SHA224", HKS_DIGEST_SHA224);
    AddInt32Property(env, keyDisgest, "HUKS_DIGEST_SHA256", HKS_DIGEST_SHA256);
    AddInt32Property(env, keyDisgest, "HUKS_DIGEST_SHA384", HKS_DIGEST_SHA384);
    AddInt32Property(env, keyDisgest, "HUKS_DIGEST_SHA512", HKS_DIGEST_SHA512);

    return keyDisgest;
}

static napi_value CreateHksKeyDigest(napi_env env)
{
    napi_value keyDisgest = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyDisgest));

    AddInt32Property(env, keyDisgest, "HKS_DIGEST_NONE", HKS_DIGEST_NONE);
    AddInt32Property(env, keyDisgest, "HKS_DIGEST_MD5", HKS_DIGEST_MD5);
    AddInt32Property(env, keyDisgest, "HKS_DIGEST_SHA1", HKS_DIGEST_SHA1);
    AddInt32Property(env, keyDisgest, "HKS_DIGEST_SHA224", HKS_DIGEST_SHA224);
    AddInt32Property(env, keyDisgest, "HKS_DIGEST_SHA256", HKS_DIGEST_SHA256);
    AddInt32Property(env, keyDisgest, "HKS_DIGEST_SHA384", HKS_DIGEST_SHA384);
    AddInt32Property(env, keyDisgest, "HKS_DIGEST_SHA512", HKS_DIGEST_SHA512);

    return keyDisgest;
}

static napi_value CreateHuksKeyGenerateType(napi_env env)
{
    napi_value keyGenerateType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyGenerateType));

    AddInt32Property(env, keyGenerateType, "HUKS_KEY_GENERATE_TYPE_DEFAULT", HKS_KEY_GENERATE_TYPE_DEFAULT);
    AddInt32Property(env, keyGenerateType, "HUKS_KEY_GENERATE_TYPE_DERIVE", HKS_KEY_GENERATE_TYPE_DERIVE);
    AddInt32Property(env, keyGenerateType, "HUKS_KEY_GENERATE_TYPE_AGREE", HKS_KEY_GENERATE_TYPE_AGREE);

    return keyGenerateType;
}

static napi_value CreateHksKeyGenerateType(napi_env env)
{
    napi_value keyGenerateType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyGenerateType));

    AddInt32Property(env, keyGenerateType, "HKS_KEY_GENERATE_TYPE_DEFAULT", HKS_KEY_GENERATE_TYPE_DEFAULT);
    AddInt32Property(env, keyGenerateType, "HKS_KEY_GENERATE_TYPE_DERIVE", HKS_KEY_GENERATE_TYPE_DERIVE);
    AddInt32Property(env, keyGenerateType, "HKS_KEY_GENERATE_TYPE_AGREE", HKS_KEY_GENERATE_TYPE_AGREE);

    return keyGenerateType;
}

static napi_value CreateHuksKeyFlag(napi_env env)
{
    napi_value keyFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyFlag));

    AddInt32Property(env, keyFlag, "HUKS_KEY_FLAG_IMPORT_KEY", HKS_KEY_FLAG_IMPORT_KEY);
    AddInt32Property(env, keyFlag, "HUKS_KEY_FLAG_GENERATE_KEY", HKS_KEY_FLAG_GENERATE_KEY);
    AddInt32Property(env, keyFlag, "HUKS_KEY_FLAG_AGREE_KEY", HKS_KEY_FLAG_AGREE_KEY);
    AddInt32Property(env, keyFlag, "HUKS_KEY_FLAG_DERIVE_KEY", HKS_KEY_FLAG_DERIVE_KEY);

    return keyFlag;
}

static napi_value CreateHksKeyFlag(napi_env env)
{
    napi_value keyFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &keyFlag));

    AddInt32Property(env, keyFlag, "HKS_KEY_FLAG_IMPORT_KEY", HKS_KEY_FLAG_IMPORT_KEY);
    AddInt32Property(env, keyFlag, "HKS_KEY_FLAG_GENERATE_KEY", HKS_KEY_FLAG_GENERATE_KEY);
    AddInt32Property(env, keyFlag, "HKS_KEY_FLAG_AGREE_KEY", HKS_KEY_FLAG_AGREE_KEY);
    AddInt32Property(env, keyFlag, "HKS_KEY_FLAG_DERIVE_KEY", HKS_KEY_FLAG_DERIVE_KEY);

    return keyFlag;
}

static napi_value CreateHuksTagType(napi_env env)
{
    napi_value tagType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &tagType));

    AddInt32Property(env, tagType, "HUKS_TAG_TYPE_INVALID", HKS_TAG_TYPE_INVALID);
    AddInt32Property(env, tagType, "HUKS_TAG_TYPE_INT", HKS_TAG_TYPE_INT);
    AddInt32Property(env, tagType, "HUKS_TAG_TYPE_UINT", HKS_TAG_TYPE_UINT);
    AddInt32Property(env, tagType, "HUKS_TAG_TYPE_ULONG", HKS_TAG_TYPE_ULONG);
    AddInt32Property(env, tagType, "HUKS_TAG_TYPE_BOOL", HKS_TAG_TYPE_BOOL);
    AddInt32Property(env, tagType, "HUKS_TAG_TYPE_BYTES", HKS_TAG_TYPE_BYTES);

    return tagType;
}

static napi_value CreateHksTagType(napi_env env)
{
    napi_value tagType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &tagType));

    AddInt32Property(env, tagType, "HKS_TAG_TYPE_INVALID", HKS_TAG_TYPE_INVALID);
    AddInt32Property(env, tagType, "HKS_TAG_TYPE_INT", HKS_TAG_TYPE_INT);
    AddInt32Property(env, tagType, "HKS_TAG_TYPE_UINT", HKS_TAG_TYPE_UINT);
    AddInt32Property(env, tagType, "HKS_TAG_TYPE_ULONG", HKS_TAG_TYPE_ULONG);
    AddInt32Property(env, tagType, "HKS_TAG_TYPE_BOOL", HKS_TAG_TYPE_BOOL);
    AddInt32Property(env, tagType, "HKS_TAG_TYPE_BYTES", HKS_TAG_TYPE_BYTES);

    return tagType;
}
}  // namespace HuksNapi

using namespace HuksNapi;

extern "C" {
static napi_value HuksNapiRegister(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("HksErrorCode", CreateHksErrorCode(env)),
        DECLARE_NAPI_PROPERTY("HksKeyType", CreateHksKeyType(env)),
        DECLARE_NAPI_PROPERTY("HksKeyPurpose", CreateHksKeyPurpose(env)),
        DECLARE_NAPI_PROPERTY("HksKeyDigest", CreateHksKeyDigest(env)),
        DECLARE_NAPI_PROPERTY("HksKeyPadding", CreateHksKeyPadding(env)),
        DECLARE_NAPI_PROPERTY("HksCipherMode", CreateHksCipherMode(env)),
        DECLARE_NAPI_PROPERTY("HksKeySize", CreateHksKeySize(env)),
        DECLARE_NAPI_PROPERTY("HksKeyAlg", CreateHksKeyAlg(env)),
        DECLARE_NAPI_PROPERTY("HksKeyGenerateType", CreateHksKeyGenerateType(env)),
        DECLARE_NAPI_PROPERTY("HksKeyFlag", CreateHksKeyFlag(env)),
        DECLARE_NAPI_PROPERTY("HksKeyStorageType", CreateHksKeyStorageType(env)),
        DECLARE_NAPI_PROPERTY("HksTagType", CreateHksTagType(env)),
        DECLARE_NAPI_PROPERTY("HksTag", CreateHksTag(env)),

        DECLARE_NAPI_PROPERTY("HuksErrorCode", CreateHuksErrorCode(env)),
        DECLARE_NAPI_PROPERTY("HuksKeyPurpose", CreateHuksKeyPurpose(env)),
        DECLARE_NAPI_PROPERTY("HuksKeyDigest", CreateHuksKeyDigest(env)),
        DECLARE_NAPI_PROPERTY("HuksKeyPadding", CreateHuksKeyPadding(env)),
        DECLARE_NAPI_PROPERTY("HuksCipherMode", CreateHuksCipherMode(env)),
        DECLARE_NAPI_PROPERTY("HuksKeySize", CreateHuksKeySize(env)),
        DECLARE_NAPI_PROPERTY("HuksKeyAlg", CreateHuksKeyAlg(env)),
        DECLARE_NAPI_PROPERTY("HuksKeyGenerateType", CreateHuksKeyGenerateType(env)),
        DECLARE_NAPI_PROPERTY("HuksKeyFlag", CreateHuksKeyFlag(env)),
        DECLARE_NAPI_PROPERTY("HuksKeyStorageType", CreateHuksKeyStorageType(env)),
        DECLARE_NAPI_PROPERTY("HuksTagType", CreateHuksTagType(env)),
        DECLARE_NAPI_PROPERTY("HuksTag", CreateHuksTag(env)),

        DECLARE_NAPI_FUNCTION("generateKey", HuksNapiGenerateKey),
        DECLARE_NAPI_FUNCTION("deleteKey", HuksNapiDeleteKey),
        DECLARE_NAPI_FUNCTION("encrypt", HuksNapiEncrypt),
        DECLARE_NAPI_FUNCTION("decrypt", HuksNapiDecrypt),
        DECLARE_NAPI_FUNCTION("getSdkVersion", HuksNapiGetSdkVersion),
        DECLARE_NAPI_FUNCTION("importKey", HuksNapiImportKey),
        DECLARE_NAPI_FUNCTION("exportKey", HuksNapiExportKey),
        DECLARE_NAPI_FUNCTION("getKeyProperties", HuksNapiGetKeyProperties),
        DECLARE_NAPI_FUNCTION("isKeyExist", HuksNapiIsKeyExist),
        DECLARE_NAPI_FUNCTION("sign", HuksNapiSign),
        DECLARE_NAPI_FUNCTION("verify", HuksNapiVerify),
        DECLARE_NAPI_FUNCTION("agreeKey", HuksNapiAgreeKey),
        DECLARE_NAPI_FUNCTION("deriveKey", HuksNapiDeriveKey),
        DECLARE_NAPI_FUNCTION("mac", HuksNapiMac),
        DECLARE_NAPI_FUNCTION("attestKey", HuksNapiAttestKey),
        DECLARE_NAPI_FUNCTION("getCertificateChain", HuksNapiGetCertificateChain),
        DECLARE_NAPI_FUNCTION("wrapKey", HuksNapiWrapKey),
        DECLARE_NAPI_FUNCTION("unwrapKey", HuksNapiUnwrapKey),
        DECLARE_NAPI_FUNCTION("init", HuksNapiInit),
        DECLARE_NAPI_FUNCTION("update", HuksNapiUpdate),
        DECLARE_NAPI_FUNCTION("finish", HuksNapiFinish),
        DECLARE_NAPI_FUNCTION("abort", HuksNapiAbort),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

static napi_module g_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = HuksNapiRegister,
    .nm_modname = "huks",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

__attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&g_module);
}
}
