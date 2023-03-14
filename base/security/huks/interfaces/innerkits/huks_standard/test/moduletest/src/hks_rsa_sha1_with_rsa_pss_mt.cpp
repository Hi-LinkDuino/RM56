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

const SignLocalCaseParams HKS_RSA_MT_31300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_31400_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_31500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_31600_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_31700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_31800_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_31900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_32000_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_32100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_32200_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_32300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_32400_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_32500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_32600_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_32700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_32800_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_32900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_33000_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_33100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_33200_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_33300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_33400_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_33500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_33600_PARAMS = {
    .alias = "This is a test auth id for Sha1AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA1,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaSha1WithRsaPssMt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt31300
 * @tc.name      : HksRsaSha1WithRsaPssMt31300
 * @tc.desc      : Test huks sign (512/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt31300, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_31300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt31400
 * @tc.name      : HksRsaSha1WithRsaPssMt31400
 * @tc.desc      : Test huks sign (512/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt31400, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_31400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt31500
 * @tc.name      : HksRsaSha1WithRsaPssMt31500
 * @tc.desc      : Test huks Verify (512/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt31500, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_31500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt31600
 * @tc.name      : HksRsaSha1WithRsaPssMt31600
 * @tc.desc      : Test huks Verify (512/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt31600, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_31600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt31700
 * @tc.name      : HksRsaSha1WithRsaPssMt31700
 * @tc.desc      : Test huks sign (768/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt31700, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_31700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt31800
 * @tc.name      : HksRsaSha1WithRsaPssMt31800
 * @tc.desc      : Test huks sign (768/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt31800, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_31800_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt31900
 * @tc.name      : HksRsaSha1WithRsaPssMt31900
 * @tc.desc      : Test huks Verify (768/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt31900, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_31900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32000
 * @tc.name      : HksRsaSha1WithRsaPssMt32000
 * @tc.desc      : Test huks Verify (768/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32000, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_32000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32100
 * @tc.name      : HksRsaSha1WithRsaPssMt32100
 * @tc.desc      : Test huks sign (1024/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32100, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_32100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32200
 * @tc.name      : HksRsaSha1WithRsaPssMt32200
 * @tc.desc      : Test huks sign (1024/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32200, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_32200_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32300
 * @tc.name      : HksRsaSha1WithRsaPssMt32300
 * @tc.desc      : Test huks Verify (1024/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32300, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_32300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32400
 * @tc.name      : HksRsaSha1WithRsaPssMt32400
 * @tc.desc      : Test huks Verify (1024/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32400, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_32400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32500
 * @tc.name      : HksRsaSha1WithRsaPssMt32500
 * @tc.desc      : Test huks sign (2048/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32500, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_32500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32600
 * @tc.name      : HksRsaSha1WithRsaPssMt32600
 * @tc.desc      : Test huks sign (2048/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32600, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_32600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32700
 * @tc.name      : HksRsaSha1WithRsaPssMt32700
 * @tc.desc      : Test huks Verify (2048/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32700, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_32700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32800
 * @tc.name      : HksRsaSha1WithRsaPssMt32800
 * @tc.desc      : Test huks Verify (2048/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32800, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_32800_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaSha1WithRsaPssMt32900
 * @tc.name      : HksRsaSha1WithRsaPssMt32900
 * @tc.desc      : Test huks sign (3072/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt32900, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_32900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt33000
 * @tc.name      : HksRsaSha1WithRsaPssMt33000
 * @tc.desc      : Test huks sign (3072/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt33000, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_33000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt33100
 * @tc.name      : HksRsaSha1WithRsaPssMt33100
 * @tc.desc      : Test huks Verify (3072/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt33100, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_33100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt33200
 * @tc.name      : HksRsaSha1WithRsaPssMt33200
 * @tc.desc      : Test huks Verify (3072/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt33200, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_33200_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt33300
 * @tc.name      : HksRsaSha1WithRsaPssMt33300
 * @tc.desc      : Test huks sign (4096/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt33300, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_33300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt33400
 * @tc.name      : HksRsaSha1WithRsaPssMt33400
 * @tc.desc      : Test huks sign (4096/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt33400, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_33400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt33500
 * @tc.name      : HksRsaSha1WithRsaPssMt33500
 * @tc.desc      : Test huks Verify (4096/SHA1withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt33500, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_33500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha1WithRsaPssMt33600
 * @tc.name      : HksRsaSha1WithRsaPssMt33600
 * @tc.desc      : Test huks Verify (4096/SHA1withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha1WithRsaPssMt, HksRsaSha1WithRsaPssMt33600, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_33600_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS