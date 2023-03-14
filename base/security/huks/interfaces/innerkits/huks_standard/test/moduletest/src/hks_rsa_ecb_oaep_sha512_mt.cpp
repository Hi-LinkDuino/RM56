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

const GenerateKeyCaseParams HKS_RSA_MT_18100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_18200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_18300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_18400_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_18500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_18600_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_18700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_18800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_18900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_19000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_19100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_19200_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_19300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_19400_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_19500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_19600_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_19700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_19800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_19900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_20000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_20100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_20200_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_20300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_20400_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_20500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_20600_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_20700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_20800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_20900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_21000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-512",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaEcbOaepSha512Mt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt18100
 * @tc.name      : HksRsaEcbOaepSha512Mt18100
 * @tc.desc      : Test huks generate key (512_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt18100, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_18100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt18200
 * @tc.name      : HksRsaEcbOaepSha512Mt18200
 * @tc.desc      : Test huks generate key (768_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt18200, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_18200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt18300
 * @tc.name      : HksRsaEcbOaepSha512Mt18300
 * @tc.desc      : Test huks generate key (1024_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt18300, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_18300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt18400
 * @tc.name      : HksRsaEcbOaepSha512Mt18400
 * @tc.desc      : Test huks generate key (2048_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt18400, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_18400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha512Mt18500
 * @tc.name      : HksRsaEcbOaepSha512Mt18500
 * @tc.desc      : Test huks generate key (3072_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt18500, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_18500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt18600
 * @tc.name      : HksRsaEcbOaepSha512Mt18600
 * @tc.desc      : Test huks generate key (4096_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt18600, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_18600_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt18700
 * @tc.name      : HksRsaEcbOaepSha512Mt18700
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt18700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_18700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt18800
 * @tc.name      : HksRsaEcbOaepSha512Mt18800
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt18800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_18800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt18900
 * @tc.name      : HksRsaEcbOaepSha512Mt18900
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt18900, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_18900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19000
 * @tc.name      : HksRsaEcbOaepSha512Mt19000
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19000, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_19000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19100
 * @tc.name      : HksRsaEcbOaepSha512Mt19100
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19100, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_19100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19200
 * @tc.name      : HksRsaEcbOaepSha512Mt19200
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19200, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_19200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19300
 * @tc.name      : HksRsaEcbOaepSha512Mt19300
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19300, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_19300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19400
 * @tc.name      : HksRsaEcbOaepSha512Mt19400
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19400, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_19400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19500
 * @tc.name      : HksRsaEcbOaepSha512Mt19500
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19500, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_19500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19600
 * @tc.name      : HksRsaEcbOaepSha512Mt19600
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19600, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_19600_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19700
 * @tc.name      : HksRsaEcbOaepSha512Mt19700
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_19700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19800
 * @tc.name      : HksRsaEcbOaepSha512Mt19800
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_19800_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt19900
 * @tc.name      : HksRsaEcbOaepSha512Mt19900
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt19900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_19900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20000
 * @tc.name      : HksRsaEcbOaepSha512Mt20000
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_20000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20100
 * @tc.name      : HksRsaEcbOaepSha512Mt20100
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20100, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_20100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20200
 * @tc.name      : HksRsaEcbOaepSha512Mt20200
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20200, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_20200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20300
 * @tc.name      : HksRsaEcbOaepSha512Mt20300
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20300, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_20300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20400
 * @tc.name      : HksRsaEcbOaepSha512Mt20400
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20400, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_20400_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20500
 * @tc.name      : HksRsaEcbOaepSha512Mt20500
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20500, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_20500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20600
 * @tc.name      : HksRsaEcbOaepSha512Mt20600
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20600, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_20600_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20700
 * @tc.name      : HksRsaEcbOaepSha512Mt20700
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20700, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_20700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20800
 * @tc.name      : HksRsaEcbOaepSha512Mt20800
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20800, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_20800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt20900
 * @tc.name      : HksRsaEcbOaepSha512Mt20900
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt20900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_20900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha512Mt21000
 * @tc.name      : HksRsaEcbOaepSha512Mt21000
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/OAEPWithSHA-512AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha512Mt, HksRsaEcbOaepSha512Mt21000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_21000_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS