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

const GenerateKeyCaseParams HKS_RSA_MT_03100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_03200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_03300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_03400_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_03500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const GenerateKeyCaseParams HKS_RSA_MT_03600_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_03700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_03800_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_03900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_04000_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_04100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_04200_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_04300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_04400_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptLocalCaseParams HKS_RSA_MT_04500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_04600_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
const EncryptLocalCaseParams HKS_RSA_MT_04700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const EncryptServiceCaseParams HKS_RSA_MT_04800_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_04900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_05000_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_05100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_05200_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_05300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_05400_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_05500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_05600_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_05700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_05800_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptLocalCaseParams HKS_RSA_MT_05900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const DecryptServiceCaseParams HKS_RSA_MT_06000_PARAMS = {
    .alias = "This is a test auth id for OAEPPadding",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_OAEP },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .hexData = "00112233445566778899aabbccddeeff",
    .padding = RSA_PKCS1_OAEP_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaEcbOaepPaddingMt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt03100
 * @tc.name      : HksRsaEcbOaepPaddingMt03100
 * @tc.desc      : Test huks generate key (512_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt03100, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_03100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt03200
 * @tc.name      : HksRsaEcbOaepPaddingMt03200
 * @tc.desc      : Test huks generate key (768_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt03200, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_03200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt03300
 * @tc.name      : HksRsaEcbOaepPaddingMt03300
 * @tc.desc      : Test huks generate key (1024_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt03300, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_03300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt03400
 * @tc.name      : HksRsaEcbOaepPaddingMt03400
 * @tc.desc      : Test huks generate key (2048_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt03400, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_03400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepPaddingMt03500
 * @tc.name      : HksRsaEcbOaepPaddingMt03500
 * @tc.desc      : Test huks generate key (3072_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt03500, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_03500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt03600
 * @tc.name      : HksRsaEcbOaepPaddingMt03600
 * @tc.desc      : Test huks generate key (4096_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt03600, TestSize.Level1)
{
    GenerateKeyTestCase(HKS_RSA_MT_03600_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt03700
 * @tc.name      : HksRsaEcbOaepPaddingMt03700
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt03700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_03700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt03800
 * @tc.name      : HksRsaEcbOaepPaddingMt03800
 * @tc.desc      : Test huks Encrypt (512_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt03800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_03800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt03900
 * @tc.name      : HksRsaEcbOaepPaddingMt03900
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt03900, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_03900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04000
 * @tc.name      : HksRsaEcbOaepPaddingMt04000
 * @tc.desc      : Test huks Encrypt (768_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04000, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_04000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04100
 * @tc.name      : HksRsaEcbOaepPaddingMt04100
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04100, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_04100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04200
 * @tc.name      : HksRsaEcbOaepPaddingMt04200
 * @tc.desc      : Test huks Encrypt (1024_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04200, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_04200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04300
 * @tc.name      : HksRsaEcbOaepPaddingMt04300
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04300, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_04300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04400
 * @tc.name      : HksRsaEcbOaepPaddingMt04400
 * @tc.desc      : Test huks Encrypt (2048_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04400, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_04400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04500
 * @tc.name      : HksRsaEcbOaepPaddingMt04500
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04500, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_04500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04600
 * @tc.name      : HksRsaEcbOaepPaddingMt04600
 * @tc.desc      : Test huks Encrypt (3072_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04600, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_04600_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04700
 * @tc.name      : HksRsaEcbOaepPaddingMt04700
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04700, TestSize.Level1)
{
    EncryptLocalTestCase(HKS_RSA_MT_04700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04800
 * @tc.name      : HksRsaEcbOaepPaddingMt04800
 * @tc.desc      : Test huks Encrypt (4096_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04800, TestSize.Level1)
{
    EncryptServiceTestCase(HKS_RSA_MT_04800_PARAMS);
}
#endif

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt04900
 * @tc.name      : HksRsaEcbOaepPaddingMt04900
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt04900, TestSize.Level1)

{
    DecryptLocalTestCase(HKS_RSA_MT_04900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05000
 * @tc.name      : HksRsaEcbOaepPaddingMt05000
 * @tc.desc      : Test huks Decrypt (512_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_05000_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05100
 * @tc.name      : HksRsaEcbOaepPaddingMt05100
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05100, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_05100_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05200
 * @tc.name      : HksRsaEcbOaepPaddingMt05200
 * @tc.desc      : Test huks Decrypt (768_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05200, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_05200_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05300
 * @tc.name      : HksRsaEcbOaepPaddingMt05300
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05300, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_05300_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05400
 * @tc.name      : HksRsaEcbOaepPaddingMt05400
 * @tc.desc      : Test huks Decrypt (1024_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05400, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_05400_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05500
 * @tc.name      : HksRsaEcbOaepPaddingMt05500
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05500, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_05500_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05600
 * @tc.name      : HksRsaEcbOaepPaddingMt05600
 * @tc.desc      : Test huks Decrypt (2048_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05600, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_05600_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05700
 * @tc.name      : HksRsaEcbOaepPaddingMt05700
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05700, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_05700_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05800
 * @tc.name      : HksRsaEcbOaepPaddingMt05800
 * @tc.desc      : Test huks Decrypt (3072_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05800, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_05800_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt05900
 * @tc.name      : HksRsaEcbOaepPaddingMt05900
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/RSA/ECB/OAEPPadding/TEMP)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt05900, TestSize.Level1)
{
    DecryptLocalTestCase(HKS_RSA_MT_05900_PARAMS);
}

/**
 * @tc.number    : HksRsaEcbOaepPaddingMt06000
 * @tc.name      : HksRsaEcbOaepPaddingMt06000
 * @tc.desc      : Test huks Decrypt (4096_RSA/ECB/RSA/ECB/OAEPPadding/PERSISTENT)
 */
HWTEST_F(HksRsaEcbOaepPaddingMt, HksRsaEcbOaepPaddingMt06000, TestSize.Level1)
{
    DecryptServiceTestCase(HKS_RSA_MT_06000_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS