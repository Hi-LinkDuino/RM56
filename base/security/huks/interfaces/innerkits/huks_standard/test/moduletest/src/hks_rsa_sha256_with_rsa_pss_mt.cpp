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

const SignLocalCaseParams HKS_RSA_MT_40900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_41000_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_41100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_41200_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_41300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_41400_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_41500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_41600_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_41700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_41800_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_41900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_42000_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_42100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_42200_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_42300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_42400_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_42500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_42600_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_42700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_42800_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_42900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_43000_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_43100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_43200_PARAMS = {
    .alias = "This is a test auth id for Sha256AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA256,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaSha256WithRsaPssMt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt40900
 * @tc.name      : HksRsaSha256WithRsaPssMt40900
 * @tc.desc      : Test huks sign (512/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt40900, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_40900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41000
 * @tc.name      : HksRsaSha256WithRsaPssMt41000
 * @tc.desc      : Test huks sign (512/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41000, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_41000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41100
 * @tc.name      : HksRsaSha256WithRsaPssMt41100
 * @tc.desc      : Test huks Verify (512/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41100, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_41100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41200
 * @tc.name      : HksRsaSha256WithRsaPssMt41200
 * @tc.desc      : Test huks Verify (512/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41200, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_41200_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41300
 * @tc.name      : HksRsaSha256WithRsaPssMt41300
 * @tc.desc      : Test huks sign (768/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41300, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_41300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41400
 * @tc.name      : HksRsaSha256WithRsaPssMt41400
 * @tc.desc      : Test huks sign (768/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41400, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_41400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41500
 * @tc.name      : HksRsaSha256WithRsaPssMt41500
 * @tc.desc      : Test huks Verify (768/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41500, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_41500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41600
 * @tc.name      : HksRsaSha256WithRsaPssMt41600
 * @tc.desc      : Test huks Verify (768/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41600, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_41600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41700
 * @tc.name      : HksRsaSha256WithRsaPssMt41700
 * @tc.desc      : Test huks sign (1024/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41700, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_41700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41800
 * @tc.name      : HksRsaSha256WithRsaPssMt41800
 * @tc.desc      : Test huks sign (1024/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41800, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_41800_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt41900
 * @tc.name      : HksRsaSha256WithRsaPssMt41900
 * @tc.desc      : Test huks Verify (1024/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt41900, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_41900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42000
 * @tc.name      : HksRsaSha256WithRsaPssMt42000
 * @tc.desc      : Test huks Verify (1024/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42000, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_42000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42100
 * @tc.name      : HksRsaSha256WithRsaPssMt42100
 * @tc.desc      : Test huks sign (2048/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42100, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_42100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42200
 * @tc.name      : HksRsaSha256WithRsaPssMt42200
 * @tc.desc      : Test huks sign (2048/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42200, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_42200_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42300
 * @tc.name      : HksRsaSha256WithRsaPssMt42300
 * @tc.desc      : Test huks Verify (2048/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42300, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_42300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42400
 * @tc.name      : HksRsaSha256WithRsaPssMt42400
 * @tc.desc      : Test huks Verify (2048/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42400, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_42400_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42500
 * @tc.name      : HksRsaSha256WithRsaPssMt42500
 * @tc.desc      : Test huks sign (3072/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42500, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_42500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42600
 * @tc.name      : HksRsaSha256WithRsaPssMt42600
 * @tc.desc      : Test huks sign (3072/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42600, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_42600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42700
 * @tc.name      : HksRsaSha256WithRsaPssMt42700
 * @tc.desc      : Test huks Verify (3072/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42700, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_42700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42800
 * @tc.name      : HksRsaSha256WithRsaPssMt42800
 * @tc.desc      : Test huks Verify (3072/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42800, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_42800_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt42900
 * @tc.name      : HksRsaSha256WithRsaPssMt42900
 * @tc.desc      : Test huks sign (4096/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt42900, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_42900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt43000
 * @tc.name      : HksRsaSha256WithRsaPssMt43000
 * @tc.desc      : Test huks sign (4096/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt43000, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_43000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt43100
 * @tc.name      : HksRsaSha256WithRsaPssMt43100
 * @tc.desc      : Test huks Verify (4096/SHA256withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt43100, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_43100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha256WithRsaPssMt43200
 * @tc.name      : HksRsaSha256WithRsaPssMt43200
 * @tc.desc      : Test huks Verify (4096/SHA256withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha256WithRsaPssMt, HksRsaSha256WithRsaPssMt43200, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_43200_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS