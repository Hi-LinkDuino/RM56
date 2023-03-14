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

const GenerateKeyCaseParams HKS_RSA_MT_09100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "01234",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_09200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_09300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_09400_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_09500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_09600_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_09700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "01234",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_09800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "01234",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_09900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_10000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_10100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_10200_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_10300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_10400_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_10500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_10600_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_10700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_10800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_10900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "012345",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_11000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "01234",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_11100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_11200_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_11300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_11400_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_11500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_11600_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_11700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_11800_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_11900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_12000_PARAMS = {
    .alias = "This is a test auth id for OAEPWithSHA-224",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaEcbOaepSha224Mt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt09100
 * @tc.name      : HksRsaEcbOaepSha224Mt09100
 * @tc.desc      : Test huks generate key (512_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt09100, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_09100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt09200
 * @tc.name      : HksRsaEcbOaepSha224Mt09200
 * @tc.desc      : Test huks generate key (768_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt09200, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_09200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt09300
 * @tc.name      : HksRsaEcbOaepSha224Mt09300
 * @tc.desc      : Test huks generate key (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt09300, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_09300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt09400
 * @tc.name      : HksRsaEcbOaepSha224Mt09400
 * @tc.desc      : Test huks generate key (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt09400, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_09400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha224Mt09500
 * @tc.name      : HksRsaEcbOaepSha224Mt09500
 * @tc.desc      : Test huks generate key (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt09500, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_09500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt09600
 * @tc.name      : HksRsaEcbOaepSha224Mt09600
 * @tc.desc      : Test huks generate key (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt09600, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_09600_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt09700
 * @tc.name      : HksRsaEcbOaepSha224Mt09700
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt09700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_09700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt09800
 * @tc.name      : HksRsaEcbOaepSha224Mt09800
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt09800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_09800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt09900
 * @tc.name      : HksRsaEcbOaepSha224Mt09900
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt09900, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_09900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10000
 * @tc.name      : HksRsaEcbOaepSha224Mt10000
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10000, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_10000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10100
 * @tc.name      : HksRsaEcbOaepSha224Mt10100
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10100, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_10100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10200
 * @tc.name      : HksRsaEcbOaepSha224Mt10200
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10200, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_10200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10300
 * @tc.name      : HksRsaEcbOaepSha224Mt10300
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10300, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_10300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10400
 * @tc.name      : HksRsaEcbOaepSha224Mt10400
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10400, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_10400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10500
 * @tc.name      : HksRsaEcbOaepSha224Mt10500
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10500, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_10500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10600
 * @tc.name      : HksRsaEcbOaepSha224Mt10600
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10600, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_10600_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10700
 * @tc.name      : HksRsaEcbOaepSha224Mt10700
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_10700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10800
 * @tc.name      : HksRsaEcbOaepSha224Mt10800
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_10800_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt10900
 * @tc.name      : HksRsaEcbOaepSha224Mt10900
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt10900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_10900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11000
 * @tc.name      : HksRsaEcbOaepSha224Mt11000
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_11000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11100
 * @tc.name      : HksRsaEcbOaepSha224Mt11100
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11100, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_11100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11200
 * @tc.name      : HksRsaEcbOaepSha224Mt11200
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11200, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_11200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11300
 * @tc.name      : HksRsaEcbOaepSha224Mt11300
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11300, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_11300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11400
 * @tc.name      : HksRsaEcbOaepSha224Mt11400
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11400, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_11400_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11500
 * @tc.name      : HksRsaEcbOaepSha224Mt11500
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11500, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_11500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11600
 * @tc.name      : HksRsaEcbOaepSha224Mt11600
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11600, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_11600_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11700
 * @tc.name      : HksRsaEcbOaepSha224Mt11700
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11700, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_11700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11800
 * @tc.name      : HksRsaEcbOaepSha224Mt11800
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11800, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_11800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt11900
 * @tc.name      : HksRsaEcbOaepSha224Mt11900
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt11900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_11900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepSha224Mt12000
 * @tc.name      : HksRsaEcbOaepSha224Mt12000
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/RSA/ECB/OAEPWithSHA-224AndMGF1Padding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepSha224Mt, HksRsaEcbOaepSha224Mt12000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_12000_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS