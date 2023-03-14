/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <openssl/rsa.h>

#include "hks_rsa_common_mt.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace MT {
namespace {
const int SET_SIZE_4096 = 4096;
const int KEY_SIZE_512 = 512;
const int KEY_SIZE_768 = 768;
const int KEY_SIZE_1024 = 1024;
const int KEY_SIZE_2048 = 2048;
const int KEY_SIZE_3072 = 3072;

const GenerateKeyCaseParams HKS_RSA_MT_00100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_00200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_00300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_00400_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "67890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_00500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "901234567890123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_00600_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "9012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901"
               "2345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_00700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptServiceCaseParams HKS_RSA_MT_00800_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptLocalCaseParams HKS_RSA_MT_00900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptServiceCaseParams HKS_RSA_MT_01000_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptLocalCaseParams HKS_RSA_MT_01100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptServiceCaseParams HKS_RSA_MT_01200_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptLocalCaseParams HKS_RSA_MT_01300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "67890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptServiceCaseParams HKS_RSA_MT_01400_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "67890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptLocalCaseParams HKS_RSA_MT_01500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "901234567890123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptServiceCaseParams HKS_RSA_MT_01600_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "901234567890123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptLocalCaseParams HKS_RSA_MT_01700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "9012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901"
               "2345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const EncryptServiceCaseParams HKS_RSA_MT_01800_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "9012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901"
               "2345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptLocalCaseParams HKS_RSA_MT_01900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptServiceCaseParams HKS_RSA_MT_02000_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptLocalCaseParams HKS_RSA_MT_02100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptServiceCaseParams HKS_RSA_MT_02200_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptLocalCaseParams HKS_RSA_MT_02300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptServiceCaseParams HKS_RSA_MT_02400_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptLocalCaseParams HKS_RSA_MT_02500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "67890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptServiceCaseParams HKS_RSA_MT_02600_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "67890123456789012345678901234567890123456789012345",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptLocalCaseParams HKS_RSA_MT_02700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "901234567890123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptServiceCaseParams HKS_RSA_MT_02800_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "901234567890123456789012345678901234567890123456789012345678901234567890123",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptLocalCaseParams HKS_RSA_MT_02900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "9012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901"
               "2345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const DecryptServiceCaseParams HKS_RSA_MT_03000_PARAMS = {
    .alias = "This is a test auth id for NoPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012"
               "3456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
               "6789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
               "9012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901"
               "2345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901",
    .padding = RSA_NO_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};
}  // namespace

class HksRsaEcbNopaddingMt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaEcbNopaddingMt00100
 * @tc.name      : HksRsaEcbNopaddingMt00100
 * @tc.desc      : Test huks generate key (512_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt00100, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_00100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt00200
 * @tc.name      : HksRsaEcbNopaddingMt00200
 * @tc.desc      : Test huks generate key (768_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt00200, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_00200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt00300
 * @tc.name      : HksRsaEcbNopaddingMt00300
 * @tc.desc      : Test huks generate key (1024_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt00300, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_00300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt00400
 * @tc.name      : HksRsaEcbNopaddingMt00400
 * @tc.desc      : Test huks generate key (2048_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt00400, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_00400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbNopaddingMt00500
 * @tc.name      : HksRsaEcbNopaddingMt00500
 * @tc.desc      : Test huks generate key (3072_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt00500, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_00500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt00600
 * @tc.name      : HksRsaEcbNopaddingMt00600
 * @tc.desc      : Test huks generate key (4096_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt00600, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_00600_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbNopaddingMt00700
 * @tc.name      : HksRsaEcbNopaddingMt00700
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt00700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_00700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt00800
 * @tc.name      : HksRsaEcbNopaddingMt00800
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt00800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_00800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt00900
 * @tc.name      : HksRsaEcbNopaddingMt00900
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt00900, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_00900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt01000
 * @tc.name      : HksRsaEcbNopaddingMt01000
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01000, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_01000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt01100
 * @tc.name      : HksRsaEcbNopaddingMt01100
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01100, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_01100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt01200
 * @tc.name      : HksRsaEcbNopaddingMt01200
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01200, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_01200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt01300
 * @tc.name      : HksRsaEcbNopaddingMt01300
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01300, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_01300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt01400
 * @tc.name      : HksRsaEcbNopaddingMt01400
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01400, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_01400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbNopaddingMt01500
 * @tc.name      : HksRsaEcbNopaddingMt01500
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01500, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_01500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt01600
 * @tc.name      : HksRsaEcbNopaddingMt01600
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01600, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_01600_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt01700
 * @tc.name      : HksRsaEcbNopaddingMt01700
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_01700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt01800
 * @tc.name      : HksRsaEcbNopaddingMt01800
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_01800_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbNopaddingMt01900
 * @tc.name      : HksRsaEcbNopaddingMt01900
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt01900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_01900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt02000
 * @tc.name      : HksRsaEcbNopaddingMt02000
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_02000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt02100
 * @tc.name      : HksRsaEcbNopaddingMt02100
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02100, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_02100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt02200
 * @tc.name      : HksRsaEcbNopaddingMt02200
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02200, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_02200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt02300
 * @tc.name      : HksRsaEcbNopaddingMt02300
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02300, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_02300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt02400
 * @tc.name      : HksRsaEcbNopaddingMt02400
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02400, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_02400_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt02500
 * @tc.name      : HksRsaEcbNopaddingMt02500
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02500, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_02500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt02600
 * @tc.name      : HksRsaEcbNopaddingMt02600
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02600, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_02600_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbNopaddingMt02700
 * @tc.name      : HksRsaEcbNopaddingMt02700
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02700, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_02700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt02800
 * @tc.name      : HksRsaEcbNopaddingMt02800
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02800, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_02800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt02900
 * @tc.name      : HksRsaEcbNopaddingMt02900
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/RSA/ECB/NoPadding/TEMP)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt02900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_02900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbNopaddingMt03000
 * @tc.name      : HksRsaEcbNopaddingMt03000
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/RSA/ECB/NoPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbNopaddingMt, HksRsaEcbNopaddingMt03000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_03000_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS