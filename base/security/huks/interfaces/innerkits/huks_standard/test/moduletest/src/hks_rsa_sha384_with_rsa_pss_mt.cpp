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

const SignLocalCaseParams HKS_RSA_MT_45700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#else
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const SignServiceCaseParams HKS_RSA_MT_45800_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#else
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const VerifyLocalCaseParams HKS_RSA_MT_45900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_46000_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_512 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_46100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_46200_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_46300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_46400_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_768 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_768,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_46500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_46600_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_46700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_46800_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_1024,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_46900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_47000_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_47100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_47200_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_2048 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_2048,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_47300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_47400_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_47500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_47600_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_3072 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = KEY_SIZE_3072,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignLocalCaseParams HKS_RSA_MT_47700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const SignServiceCaseParams HKS_RSA_MT_47800_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyLocalCaseParams HKS_RSA_MT_47900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const VerifyServiceCaseParams HKS_RSA_MT_48000_PARAMS = {
    .alias = "This is a test auth id for Sha384AndPss",
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_RSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_4096 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PSS },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .padding = RSA_PKCS1_PSS_PADDING,
    .keyDigest = HKS_DIGEST_SHA384,
    .keySize = SET_SIZE_4096,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};
}  // namespace

class HksRsaSha384WithRsaPssMt : public HksRsaCommonMt, public testing::Test {};

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt45700
 * @tc.name      : HksRsaSha384WithRsaPssMt45700
 * @tc.desc      : Test huks sign (512/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt45700, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_45700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt45800
 * @tc.name      : HksRsaSha384WithRsaPssMt45800
 * @tc.desc      : Test huks sign (512/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt45800, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_45800_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt45900
 * @tc.name      : HksRsaSha384WithRsaPssMt45900
 * @tc.desc      : Test huks Verify (512/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt45900, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_45900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46000
 * @tc.name      : HksRsaSha384WithRsaPssMt46000
 * @tc.desc      : Test huks Verify (512/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46000, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_46000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46100
 * @tc.name      : HksRsaSha384WithRsaPssMt46100
 * @tc.desc      : Test huks sign (768/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46100, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_46100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46200
 * @tc.name      : HksRsaSha384WithRsaPssMt46200
 * @tc.desc      : Test huks sign (768/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46200, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_46200_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46300
 * @tc.name      : HksRsaSha384WithRsaPssMt46300
 * @tc.desc      : Test huks Verify (768/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46300, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_46300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46400
 * @tc.name      : HksRsaSha384WithRsaPssMt46400
 * @tc.desc      : Test huks Verify (768/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46400, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_46400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46500
 * @tc.name      : HksRsaSha384WithRsaPssMt46500
 * @tc.desc      : Test huks sign (1024/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46500, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_46500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46600
 * @tc.name      : HksRsaSha384WithRsaPssMt46600
 * @tc.desc      : Test huks sign (1024/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46600, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_46600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46700
 * @tc.name      : HksRsaSha384WithRsaPssMt46700
 * @tc.desc      : Test huks Verify (1024/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46700, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_46700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46800
 * @tc.name      : HksRsaSha384WithRsaPssMt46800
 * @tc.desc      : Test huks Verify (1024/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46800, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_46800_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt46900
 * @tc.name      : HksRsaSha384WithRsaPssMt46900
 * @tc.desc      : Test huks sign (2048/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt46900, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_46900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47000
 * @tc.name      : HksRsaSha384WithRsaPssMt47000
 * @tc.desc      : Test huks sign (2048/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47000, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_47000_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47100
 * @tc.name      : HksRsaSha384WithRsaPssMt47100
 * @tc.desc      : Test huks Verify (2048/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47100, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_47100_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47200
 * @tc.name      : HksRsaSha384WithRsaPssMt47200
 * @tc.desc      : Test huks Verify (2048/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47200, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_47200_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47300
 * @tc.name      : HksRsaSha384WithRsaPssMt47300
 * @tc.desc      : Test huks sign (3072/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47300, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_47300_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47400
 * @tc.name      : HksRsaSha384WithRsaPssMt47400
 * @tc.desc      : Test huks sign (3072/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47400, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_47400_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47500
 * @tc.name      : HksRsaSha384WithRsaPssMt47500
 * @tc.desc      : Test huks Verify (3072/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47500, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_47500_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47600
 * @tc.name      : HksRsaSha384WithRsaPssMt47600
 * @tc.desc      : Test huks Verify (3072/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47600, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_47600_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47700
 * @tc.name      : HksRsaSha384WithRsaPssMt47700
 * @tc.desc      : Test huks sign (4096/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47700, TestSize.Level1)
{
    SignLocalTestCase(HKS_RSA_MT_47700_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47800
 * @tc.name      : HksRsaSha384WithRsaPssMt47800
 * @tc.desc      : Test huks sign (4096/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47800, TestSize.Level1)
{
    SignServiceTestCase(HKS_RSA_MT_47800_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt47900
 * @tc.name      : HksRsaSha384WithRsaPssMt47900
 * @tc.desc      : Test huks Verify (4096/SHA384withRSA/PSS/TEMP)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt47900, TestSize.Level1)
{
    VerifyLocalTestCase(HKS_RSA_MT_47900_PARAMS);
}

/**
 * @tc.number    : HksRsaSha384WithRsaPssMt48000
 * @tc.name      : HksRsaSha384WithRsaPssMt48000
 * @tc.desc      : Test huks Verify (4096/SHA384withRSA/PSS/PERSISTENT)
 */
HWTEST_F(HksRsaSha384WithRsaPssMt, HksRsaSha384WithRsaPssMt48000, TestSize.Level1)
{
    VerifyServiceTestCase(HKS_RSA_MT_48000_PARAMS);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS