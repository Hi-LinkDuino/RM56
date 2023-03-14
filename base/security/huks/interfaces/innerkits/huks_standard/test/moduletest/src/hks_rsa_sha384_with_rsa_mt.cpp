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

const SignLocalCaseParams HKS_RSA_MT_43300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
#if (defined(_USE_OPENSSL_) || defined(_USE_MBEDTLS_))
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#else
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const SignServiceCaseParams HKS_RSA_MT_43400_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
#if (defined(_USE_OPENSSL_) || defined(_USE_MBEDTLS_))
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#else
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const VerifyLocalCaseParams HKS_RSA_MT_43500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_FAILURE,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_43600_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_FAILURE,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_43700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_43800_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_43900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_44000_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_44100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_44200_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_44300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_44400_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_44500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_44600_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_44700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_44800_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_44900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_45000_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_45100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_45200_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_45300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_45400_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_45500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_45600_PARAMS = {
    .alias = "This is a test auth id for SHA384",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaSha384WithRsaMt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaSha384WithRsaMt43300
 * @tc.name      : HksRsaSha384WithRsaMt43300
 * @tc.desc      : Test huks sign (512/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt43300, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_43300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt43400
 * @tc.name      : HksRsaSha384WithRsaMt43400
 * @tc.desc      : Test huks sign (512/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt43400, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_43400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt43500
 * @tc.name      : HksRsaSha384WithRsaMt43500
 * @tc.desc      : Test huks Verify (512/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt43500, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_43500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt43600
 * @tc.name      : HksRsaSha384WithRsaMt43600
 * @tc.desc      : Test huks Verify (512/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt43600, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_43600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt43700
 * @tc.name      : HksRsaSha384WithRsaMt43700
 * @tc.desc      : Test huks sign (768/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt43700, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_43700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt43800
 * @tc.name      : HksRsaSha384WithRsaMt43800
 * @tc.desc      : Test huks sign (768/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt43800, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_43800_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt43900
 * @tc.name      : HksRsaSha384WithRsaMt43900
 * @tc.desc      : Test huks Verify (768/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt43900, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_43900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt44000
 * @tc.name      : HksRsaSha384WithRsaMt44000
 * @tc.desc      : Test huks Verify (768/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44000, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_44000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt44100
 * @tc.name      : HksRsaSha384WithRsaMt44100
 * @tc.desc      : Test huks sign (1024/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44100, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_44100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt44200
 * @tc.name      : HksRsaSha384WithRsaMt44200
 * @tc.desc      : Test huks sign (1024/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44200, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_44200_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt44300
 * @tc.name      : HksRsaSha384WithRsaMt44300
 * @tc.desc      : Test huks Verify (1024/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44300, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_44300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt44400
 * @tc.name      : HksRsaSha384WithRsaMt44400
 * @tc.desc      : Test huks Verify (1024/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44400, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_44400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt44500
 * @tc.name      : HksRsaSha384WithRsaMt44500
 * @tc.desc      : Test huks sign (2048/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44500, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_44500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt44600
 * @tc.name      : HksRsaSha384WithRsaMt44600
 * @tc.desc      : Test huks sign (2048/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44600, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_44600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt44700
 * @tc.name      : HksRsaSha384WithRsaMt44700
 * @tc.desc      : Test huks Verify (2048/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44700, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_44700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt44800
 * @tc.name      : HksRsaSha384WithRsaMt44800
 * @tc.desc      : Test huks Verify (2048/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44800, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_44800_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaSha384WithRsaMt44900
 * @tc.name      : HksRsaSha384WithRsaMt44900
 * @tc.desc      : Test huks sign (3072/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt44900, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_44900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt45000
 * @tc.name      : HksRsaSha384WithRsaMt45000
 * @tc.desc      : Test huks sign (3072/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt45000, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_45000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt45100
 * @tc.name      : HksRsaSha384WithRsaMt45100
 * @tc.desc      : Test huks Verify (3072/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt45100, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_45100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt45200
 * @tc.name      : HksRsaSha384WithRsaMt45200
 * @tc.desc      : Test huks Verify (3072/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt45200, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_45200_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt45300
 * @tc.name      : HksRsaSha384WithRsaMt45300
 * @tc.desc      : Test huks sign (4096/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt45300, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_45300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt45400
 * @tc.name      : HksRsaSha384WithRsaMt45400
 * @tc.desc      : Test huks sign (4096/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt45400, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_45400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt45500
 * @tc.name      : HksRsaSha384WithRsaMt45500
 * @tc.desc      : Test huks Verify (4096/SHA384withRSA/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt45500, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_45500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaMt45600
 * @tc.name      : HksRsaSha384WithRsaMt45600
 * @tc.desc      : Test huks Verify (4096/SHA384withRSA/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaMt, HksRsaSha384WithRsaMt45600, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_45600_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS