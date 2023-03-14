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

const GenerateKeyCaseParams HKS_RSA_MT_21100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_21200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_21300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_21400_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_21500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_21600_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_21700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_21800_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_21900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_22000_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_22100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_22200_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_22300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_22400_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_22500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_22600_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },

        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_22700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_22800_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_22900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_23000_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_23100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_23200_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_23300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_23400_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_23500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_23600_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_23700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_23800_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_23900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_24000_PARAMS = {
    .alias = "This is a test auth id for PKCS1Padding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_NONE,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaEcbPkcs1PaddingMt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt21100
 * @tc.name      : HksRsaEcbPkcs1PaddingMt21100
 * @tc.desc      : Test huks generate key (512_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt21100, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_21100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt21200
 * @tc.name      : HksRsaEcbPkcs1PaddingMt21200
 * @tc.desc      : Test huks generate key (768_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt21200, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_21200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt21300
 * @tc.name      : HksRsaEcbPkcs1PaddingMt21300
 * @tc.desc      : Test huks generate key (1024_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt21300, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_21300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt21400
 * @tc.name      : HksRsaEcbPkcs1PaddingMt21400
 * @tc.desc      : Test huks generate key (2048_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt21400, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_21400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt21500
 * @tc.name      : HksRsaEcbPkcs1PaddingMt21500
 * @tc.desc      : Test huks generate key (3072_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt21500, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_21500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt21600
 * @tc.name      : HksRsaEcbPkcs1PaddingMt21600
 * @tc.desc      : Test huks generate key (4096_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt21600, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_21600_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt21700
 * @tc.name      : HksRsaEcbPkcs1PaddingMt21700
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt21700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_21700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt21800
 * @tc.name      : HksRsaEcbPkcs1PaddingMt21800
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt21800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_21800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt21900
 * @tc.name      : HksRsaEcbPkcs1PaddingMt21900
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/PKCS1Padding/TEMP)
 */
HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt21900, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_21900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22000
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22000
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22000, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_22000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22100
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22100
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22100, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_22100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22200
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22200
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22200, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_22200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22300
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22300
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22300, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_22300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22400
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22400
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22400, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_22400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22500
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22500
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22500, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_22500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22600
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22600
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22600, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_22600_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22700
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22700
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_22700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22800
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22800
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_22800_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt22900
 * @tc.name      : HksRsaEcbPkcs1PaddingMt22900
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt22900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_22900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23000
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23000
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_23000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23100
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23100
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23100, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_23100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23200
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23200
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23200, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_23200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23300
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23300
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23300, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_23300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23400
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23400
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23400, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_23400_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23500
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23500
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23500, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_23500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23600
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23600
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23600, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_23600_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23700
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23700
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23700, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_23700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23800
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23800
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23800, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_23800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt23900
 * @tc.name      : HksRsaEcbPkcs1PaddingMt23900
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/PKCS1Padding/TEMP)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt23900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_23900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbPkcs1PaddingMt24000
 * @tc.name      : HksRsaEcbPkcs1PaddingMt24000
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/PKCS1Padding/PERSISTENT)
 */

HWTEST_F(HksRsaEcbPkcs1PaddingMt, HksRsaEcbPkcs1PaddingMt24000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_24000_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS