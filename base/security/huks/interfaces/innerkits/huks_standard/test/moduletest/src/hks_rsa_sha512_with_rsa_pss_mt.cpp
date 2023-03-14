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

const SignLocalCaseParams HKS_RSA_MT_50500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
#if (defined(_USE_OPENSSL_) || defined(_USE_MBEDTLS_))
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#else
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const SignServiceCaseParams HKS_RSA_MT_50600_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
#if (defined(_USE_OPENSSL_) || defined(_USE_MBEDTLS_))
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#else
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const VerifyLocalCaseParams HKS_RSA_MT_50700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_FAILURE,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_50800_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_FAILURE,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_50900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#else
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const SignServiceCaseParams HKS_RSA_MT_51000_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#else
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const VerifyLocalCaseParams HKS_RSA_MT_51100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_51200_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_51300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_51400_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_51500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_51600_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_51700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_51800_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_51900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_52000_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_52100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_52200_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_52300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_52400_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_52500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_52600_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_52700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_52800_PARAMS = {
    .alias = "This is a test auth id for Sha512AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA512,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaSha512WithRsaPssMt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt50500
 * @tc.name      : HksRsaSha512WithRsaPssMt50500
 * @tc.desc      : Test huks sign (512/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt50500, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_50500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt50600
 * @tc.name      : HksRsaSha512WithRsaPssMt50600
 * @tc.desc      : Test huks sign (512/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt50600, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_50600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt50700
 * @tc.name      : HksRsaSha512WithRsaPssMt50700
 * @tc.desc      : Test huks Verify (512/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt50700, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_50700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt50800
 * @tc.name      : HksRsaSha512WithRsaPssMt50800
 * @tc.desc      : Test huks Verify (512/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt50800, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_50800_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt50900
 * @tc.name      : HksRsaSha512WithRsaPssMt50900
 * @tc.desc      : Test huks sign (768/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt50900, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_50900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51000
 * @tc.name      : HksRsaSha512WithRsaPssMt51000
 * @tc.desc      : Test huks sign (768/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51000, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_51000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51100
 * @tc.name      : HksRsaSha512WithRsaPssMt51100
 * @tc.desc      : Test huks Verify (768/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51100, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_51100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51200
 * @tc.name      : HksRsaSha512WithRsaPssMt51200
 * @tc.desc      : Test huks Verify (768/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51200, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_51200_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51300
 * @tc.name      : HksRsaSha512WithRsaPssMt51300
 * @tc.desc      : Test huks sign (1024/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51300, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_51300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51400
 * @tc.name      : HksRsaSha512WithRsaPssMt51400
 * @tc.desc      : Test huks sign (1024/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51400, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_51400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51500
 * @tc.name      : HksRsaSha512WithRsaPssMt51500
 * @tc.desc      : Test huks Verify (1024/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51500, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_51500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51600
 * @tc.name      : HksRsaSha512WithRsaPssMt51600
 * @tc.desc      : Test huks Verify (1024/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51600, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_51600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51700
 * @tc.name      : HksRsaSha512WithRsaPssMt51700
 * @tc.desc      : Test huks sign (2048/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51700, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_51700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51800
 * @tc.name      : HksRsaSha512WithRsaPssMt51800
 * @tc.desc      : Test huks sign (2048/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51800, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_51800_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt51900
 * @tc.name      : HksRsaSha512WithRsaPssMt51900
 * @tc.desc      : Test huks Verify (2048/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt51900, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_51900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt52000
 * @tc.name      : HksRsaSha512WithRsaPssMt52000
 * @tc.desc      : Test huks Verify (2048/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt52000, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_52000_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaSha512WithRsaPssMt52100
 * @tc.name      : HksRsaSha512WithRsaPssMt52100
 * @tc.desc      : Test huks sign (3072/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt52100, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_52100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt52200
 * @tc.name      : HksRsaSha512WithRsaPssMt52200
 * @tc.desc      : Test huks sign (3072/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt52200, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_52200_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt52300
 * @tc.name      : HksRsaSha512WithRsaPssMt52300
 * @tc.desc      : Test huks Verify (3072/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt52300, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_52300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt52400
 * @tc.name      : HksRsaSha512WithRsaPssMt52400
 * @tc.desc      : Test huks Verify (3072/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt52400, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_52400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt52500
 * @tc.name      : HksRsaSha512WithRsaPssMt52500
 * @tc.desc      : Test huks sign (4096/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt52500, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_52500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt52600
 * @tc.name      : HksRsaSha512WithRsaPssMt52600
 * @tc.desc      : Test huks sign (4096/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt52600, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_52600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt52700
 * @tc.name      : HksRsaSha512WithRsaPssMt52700
 * @tc.desc      : Test huks Verify (4096/SHA512withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt52700, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_52700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha512WithRsaPssMt52800
 * @tc.name      : HksRsaSha512WithRsaPssMt52800
 * @tc.desc      : Test huks Verify (4096/SHA512withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha512WithRsaPssMt, HksRsaSha512WithRsaPssMt52800, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_52800_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS