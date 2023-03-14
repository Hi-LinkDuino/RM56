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

const GenerateKeyCaseParams HKS_RSA_MT_12100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_12200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_12300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_12400_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_12500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_12600_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_12700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_12800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_12900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_13000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_13100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_13200_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_13300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_13400_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_13500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_13600_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_13700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_13800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_13900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_14000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_14100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_14200_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_14300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_14400_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_14500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_14600_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_14700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_14800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_14900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_15000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-256",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaEcbOaepSha256Mt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt12100
 * @tc.name      : HksRsaEcbOaepSha256Mt12100
 * @tc.desc      : Test huks generate key (512_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt12100, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_12100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt12200
 * @tc.name      : HksRsaEcbOaepSha256Mt12200
 * @tc.desc      : Test huks generate key (768_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt12200, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_12200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt12300
 * @tc.name      : HksRsaEcbOaepSha256Mt12300
 * @tc.desc      : Test huks generate key (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt12300, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_12300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt12400
 * @tc.name      : HksRsaEcbOaepSha256Mt12400
 * @tc.desc      : Test huks generate key (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt12400, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_12400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha256Mt12500
 * @tc.name      : HksRsaEcbOaepSha256Mt12500
 * @tc.desc      : Test huks generate key (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt12500, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_12500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt12600
 * @tc.name      : HksRsaEcbOaepSha256Mt12600
 * @tc.desc      : Test huks generate key (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt12600, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_12600_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt12700
 * @tc.name      : HksRsaEcbOaepSha256Mt12700
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt12700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_12700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt12800
 * @tc.name      : HksRsaEcbOaepSha256Mt12800
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt12800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_12800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt12900
 * @tc.name      : HksRsaEcbOaepSha256Mt12900
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt12900, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_12900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt13000
 * @tc.name      : HksRsaEcbOaepSha256Mt13000
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13000, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_13000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt13100
 * @tc.name      : HksRsaEcbOaepSha256Mt13100
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13100, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_13100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt13200
 * @tc.name      : HksRsaEcbOaepSha256Mt13200
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13200, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_13200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt13300
 * @tc.name      : HksRsaEcbOaepSha256Mt13300
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13300, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_13300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt13400
 * @tc.name      : HksRsaEcbOaepSha256Mt13400
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13400, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_13400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha256Mt00100
 * @tc.name      : HksRsaEcbOaepSha256Mt00100
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13500, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_13500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt13600
 * @tc.name      : HksRsaEcbOaepSha256Mt13600
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13600, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_13600_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt13700
 * @tc.name      : HksRsaEcbOaepSha256Mt13700
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_13700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt13800
 * @tc.name      : HksRsaEcbOaepSha256Mt13800
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_13800_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt13900
 * @tc.name      : HksRsaEcbOaepSha256Mt13900
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt13900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_13900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14000
 * @tc.name      : HksRsaEcbOaepSha256Mt14000
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_14000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14100
 * @tc.name      : HksRsaEcbOaepSha256Mt14100
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14100, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_14100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14200
 * @tc.name      : HksRsaEcbOaepSha256Mt14200
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14200, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_14200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14300
 * @tc.name      : HksRsaEcbOaepSha256Mt14300
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14300, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_14300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14400
 * @tc.name      : HksRsaEcbOaepSha256Mt14400
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14400, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_14400_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14500
 * @tc.name      : HksRsaEcbOaepSha256Mt14500
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14500, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_14500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14600
 * @tc.name      : HksRsaEcbOaepSha256Mt14600
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14600, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_14600_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14700
 * @tc.name      : HksRsaEcbOaepSha256Mt14700
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14700, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_14700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14800
 * @tc.name      : HksRsaEcbOaepSha256Mt14800
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14800, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_14800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt14900
 * @tc.name      : HksRsaEcbOaepSha256Mt14900
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt14900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_14900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha256Mt15000
 * @tc.name      : HksRsaEcbOaepSha256Mt15000
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-256AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha256Mt, HksRsaEcbOaepSha256Mt15000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_15000_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS