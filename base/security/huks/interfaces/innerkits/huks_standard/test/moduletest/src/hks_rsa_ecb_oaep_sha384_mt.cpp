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

const GenerateKeyCaseParams HKS_RSA_MT_15100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_15200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_15300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_15400_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_15500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_15600_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_15700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_15800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_15900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_16000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_ERROR_INVALID_KEY_FILE,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_16100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_16200_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_16300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_16400_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_16500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_16600_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_16700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_16800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_16900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_17000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_17100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_17200_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_FAILURE,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_17300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_17400_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_17500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_17600_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_17700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_17800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_17900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_18000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaEcbOaepSha384Mt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt15100
 * @tc.name      : HksRsaEcbOaepSha384Mt15100
 * @tc.desc      : Test huks generate key (512_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt15100, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_15100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt15200
 * @tc.name      : HksRsaEcbOaepSha384Mt15200
 * @tc.desc      : Test huks generate key (768_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt15200, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_15200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt15300
 * @tc.name      : HksRsaEcbOaepSha384Mt15300
 * @tc.desc      : Test huks generate key (1024_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt15300, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_15300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt15400
 * @tc.name      : HksRsaEcbOaepSha384Mt15400
 * @tc.desc      : Test huks generate key (2048_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt15400, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_15400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha384Mt15500
 * @tc.name      : HksRsaEcbOaepSha384Mt15500
 * @tc.desc      : Test huks generate key (3072_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt15500, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_15500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt15600
 * @tc.name      : HksRsaEcbOaepSha384Mt15600
 * @tc.desc      : Test huks generate key (4096_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt15600, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_15600_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt15700
 * @tc.name      : HksRsaEcbOaepSha384Mt15700
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt15700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_15700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt15800
 * @tc.name      : HksRsaEcbOaepSha384Mt15800
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt15800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_15800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt15900
 * @tc.name      : HksRsaEcbOaepSha384Mt15900
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt15900, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_15900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16000
 * @tc.name      : HksRsaEcbOaepSha384Mt16000
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16000, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_16000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16100
 * @tc.name      : HksRsaEcbOaepSha384Mt16100
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16100, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_16100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16200
 * @tc.name      : HksRsaEcbOaepSha384Mt16200
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16200, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_16200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16300
 * @tc.name      : HksRsaEcbOaepSha384Mt16300
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16300, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_16300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16400
 * @tc.name      : HksRsaEcbOaepSha384Mt16400
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16400, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_16400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16500
 * @tc.name      : HksRsaEcbOaepSha384Mt16500
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16500, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_16500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16600
 * @tc.name      : HksRsaEcbOaepSha384Mt16600
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16600, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_16600_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16700
 * @tc.name      : HksRsaEcbOaepSha384Mt16700
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_16700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16800
 * @tc.name      : HksRsaEcbOaepSha384Mt16800
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_16800_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt16900
 * @tc.name      : HksRsaEcbOaepSha384Mt16900
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt16900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_16900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17000
 * @tc.name      : HksRsaEcbOaepSha384Mt17000
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_17000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17100
 * @tc.name      : HksRsaEcbOaepSha384Mt17100
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17100, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_17100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17200
 * @tc.name      : HksRsaEcbOaepSha384Mt17200
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17200, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_17200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17300
 * @tc.name      : HksRsaEcbOaepSha384Mt17300
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17300, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_17300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17400
 * @tc.name      : HksRsaEcbOaepSha384Mt17400
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17400, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_17400_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17500
 * @tc.name      : HksRsaEcbOaepSha384Mt17500
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17500, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_17500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17600
 * @tc.name      : HksRsaEcbOaepSha384Mt17600
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17600, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_17600_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17700
 * @tc.name      : HksRsaEcbOaepSha384Mt17700
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17700, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_17700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17800
 * @tc.name      : HksRsaEcbOaepSha384Mt17800
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17800, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_17800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt17900
 * @tc.name      : HksRsaEcbOaepSha384Mt17900
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt17900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_17900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha384Mt18000
 * @tc.name      : HksRsaEcbOaepSha384Mt18000
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/OAEPWithSHA-384AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha384Mt, HksRsaEcbOaepSha384Mt18000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_18000_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS