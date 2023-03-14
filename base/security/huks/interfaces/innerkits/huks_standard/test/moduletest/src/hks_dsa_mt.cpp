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

#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <openssl/evp.h>
#include <securec.h>

#include "hks_api.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "openssl_dsa_helper.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace MT {
namespace {
const char TEST_KEY_AUTH_ID[] = "This is a test auth id for DSA";
const int SET_SIZE_4096 = 4096;
const int KEY_SIZE_1024 = 1024;

struct TestCaseParams {
    std::vector<HksParam> params;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode signResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode verifyResult = HksErrorCode::HKS_SUCCESS;
};

const TestCaseParams HKS_DSA_MT_00100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA1,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_00200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA1,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_00300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA1,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_00400_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA1,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_00500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA1,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .verifyResult = HKS_ERROR_INVALID_ARGUMENT,
#else
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_00600_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA224,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_00700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA224,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_00800_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA224,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_00900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA224,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01000_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA224,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .verifyResult = HKS_ERROR_INVALID_ARGUMENT,
#else
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA256,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA256,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA256,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01400_PARAMS = {
    .params = { { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB } },
    .keyDigest = HKS_DIGEST_SHA256,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA256,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .verifyResult = HKS_ERROR_INVALID_ARGUMENT,
#else
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01600_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA384,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01700_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA384,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01800_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA384,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_01900_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA384,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_02000_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA384,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .verifyResult = HKS_ERROR_INVALID_ARGUMENT,
#else
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_02100_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA512,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_02200_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA512,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_02300_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY | HKS_KEY_PURPOSE_SIGN },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA512,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_02400_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA512,
#if defined(_USE_MBEDTLS_)
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#else
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_DSA_MT_02500_PARAMS = {
    .params =
        {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DSA },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_RSA_KEY_SIZE_1024 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
            { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS1_V1_5 },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
            { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        },
    .keyDigest = HKS_DIGEST_SHA512,
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .verifyResult = HKS_ERROR_INVALID_ARGUMENT,
#else
    .verifyResult = HKS_SUCCESS,
#endif
};
}  // namespace

class HksDsaMt : public testing::Test {
protected:
    void GenerateKeyTestCase(const TestCaseParams &testCaseParams)
    {
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        ASSERT_NE(paramInSet, nullptr);

        struct HksParamSet *paramSetOut = nullptr;
        HksInitParamSet(&paramSetOut);
        struct HksParam localKey = {
            .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) }
        };
        ASSERT_NE(localKey.blob.data, nullptr);
        HksAddParams(paramSetOut, &localKey, 1);

        HksBuildParamSet(&paramSetOut);

        EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

        EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

        EXPECT_EQ(HksGenerateKey(NULL, paramInSet, paramSetOut), testCaseParams.generateKeyResult);
        if (testCaseParams.generateKeyResult == HKS_SUCCESS) {
            HksParam *pubKeyExport = nullptr;
            EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubKeyExport), HKS_SUCCESS);
            HksBlob publicKey = {
                .size = pubKeyExport->blob.size,
                .data = (uint8_t *)HksMalloc(pubKeyExport->blob.size)
            };
            ASSERT_NE(publicKey.data, nullptr);
            (void)memcpy_s(publicKey.data, pubKeyExport->blob.size, pubKeyExport->blob.data, pubKeyExport->blob.size);

            HksParam *priKeyExport = nullptr;
            EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priKeyExport), HKS_SUCCESS);
            HksBlob privateKey = {
                .size = priKeyExport->blob.size,
                .data = (uint8_t *)HksMalloc(priKeyExport->blob.size)
            };
            ASSERT_NE(privateKey.data, nullptr);
            (void)memcpy_s(privateKey.data, priKeyExport->blob.size, priKeyExport->blob.data, priKeyExport->blob.size);

            const char *hexData = "00112233445566778899aabbccddeeff";

            HksBlob plainText = { .size = (uint32_t)strlen(hexData), .data = (uint8_t *)hexData };

            HksBlob signData = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
            ASSERT_NE(signData.data, nullptr);

            EXPECT_EQ(OpensslSignDsa(&plainText, &signData, &privateKey, testCaseParams.keyDigest),
                testCaseParams.signResult);

            EXPECT_EQ(OpensslVerifyDsa(&plainText, &signData, &publicKey, testCaseParams.keyDigest),
                testCaseParams.verifyResult);

            HksFree(publicKey.data);
            HksFree(privateKey.data);
            HksFree(signData.data);
        }
        HksFree(localKey.blob.data);
        HksFreeParamSet(&paramSetOut);
        HksFreeParamSet(&paramInSet);
    }

    void SignLocalTestCase(const TestCaseParams &testCaseParams)
    {
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        ASSERT_NE(paramInSet, nullptr);

        struct HksParamSet *paramSetOut = nullptr;
        HksInitParamSet(&paramSetOut);
        struct HksParam localKey = {
            .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) }
        };
        ASSERT_NE(localKey.blob.data, nullptr);
        HksAddParams(paramSetOut, &localKey, 1);

        HksBuildParamSet(&paramSetOut);

        EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

        EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

        EXPECT_EQ(HksGenerateKey(NULL, paramInSet, paramSetOut), testCaseParams.generateKeyResult);
        if (testCaseParams.generateKeyResult == HKS_SUCCESS) {
            HksParam *pubKeyExport = nullptr;
            EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubKeyExport), HKS_SUCCESS);
            HksBlob publicKey = {
                .size = pubKeyExport->blob.size,
                .data = (uint8_t *)HksMalloc(pubKeyExport->blob.size)
            };
            ASSERT_NE(publicKey.data, nullptr);
            (void)memcpy_s(publicKey.data, pubKeyExport->blob.size, pubKeyExport->blob.data, pubKeyExport->blob.size);

            HksParam *priKeyExport = nullptr;
            EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priKeyExport), HKS_SUCCESS);
            HksBlob privateKey = {
                .size = priKeyExport->blob.size,
                .data = (uint8_t *)HksMalloc(priKeyExport->blob.size)
            };
            ASSERT_NE(privateKey.data, nullptr);
            (void)memcpy_s(privateKey.data, priKeyExport->blob.size, priKeyExport->blob.data, priKeyExport->blob.size);

            const char *hexData = "00112233445566778899aabbccddeeff";

            HksBlob plainText = { .size = (uint32_t)strlen(hexData), .data = (uint8_t *)hexData };

            HksBlob signData = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
            ASSERT_NE(signData.data, nullptr);

            EXPECT_EQ(HksSign(&privateKey, paramInSet, &plainText, &signData), testCaseParams.signResult);

            EXPECT_EQ(OpensslVerifyDsa(&plainText, &signData, &publicKey, testCaseParams.keyDigest),
                testCaseParams.verifyResult);

            HksFree(publicKey.data);
            HksFree(privateKey.data);
            HksFree(signData.data);
        }
        HksFree(localKey.blob.data);
        HksFreeParamSet(&paramSetOut);
        HksFreeParamSet(&paramInSet);
    }

    void SignServiceTestCase(const TestCaseParams &testCaseParams)
    {
        struct HksBlob authId = { (uint32_t)strlen(TEST_KEY_AUTH_ID), (uint8_t *)TEST_KEY_AUTH_ID };
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        ASSERT_NE(paramInSet, nullptr);

        struct HksParamSet *paramSetOut = nullptr;
        HksInitParamSet(&paramSetOut);
        struct HksParam localKey = {
            .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) }
        };
        ASSERT_NE(localKey.blob.data, nullptr);
        HksAddParams(paramSetOut, &localKey, 1);

        HksBuildParamSet(&paramSetOut);

        EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

        EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

        EXPECT_EQ(HksGenerateKey(&authId, paramInSet, paramSetOut), testCaseParams.generateKeyResult);
        if (testCaseParams.generateKeyResult == HKS_SUCCESS) {
            uint8_t opensslDsaKey[SET_SIZE_4096] = {0};
            uint32_t opensslDsaKeyLen = SET_SIZE_4096;
            struct HksBlob opensslDsaKeyInfo = { opensslDsaKeyLen, opensslDsaKey };
            EXPECT_EQ(HksExportPublicKey(&authId, paramInSet, &opensslDsaKeyInfo), HKS_SUCCESS);

            uint8_t dsaPublicKey[SET_SIZE_4096] = {0};
            uint32_t dsaPublicKeyLen = SET_SIZE_4096;
            struct HksBlob dsaPublicKeyInfo = { dsaPublicKeyLen, dsaPublicKey };
            EXPECT_EQ(X509ToDsaPublicKey(&opensslDsaKeyInfo, &dsaPublicKeyInfo), 0);

            HksBlob publicKey = { .size = dsaPublicKeyInfo.size, .data = (uint8_t *)HksMalloc(dsaPublicKeyInfo.size) };
            ASSERT_NE(publicKey.data, nullptr);
            ASSERT_EQ(memcpy_s(publicKey.data, publicKey.size, dsaPublicKeyInfo.data, dsaPublicKeyInfo.size), 0);

            const char *hexData = "00112233445566778899aabbccddeeff";

            HksBlob plainText = { .size = (uint32_t)strlen(hexData), .data = (uint8_t *)hexData };

            HksBlob signData = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
            ASSERT_NE(signData.data, nullptr);

            EXPECT_EQ(HksSign(&authId, paramInSet, &plainText, &signData), testCaseParams.signResult);

            EXPECT_EQ(OpensslVerifyDsa(&plainText, &signData, &publicKey, testCaseParams.keyDigest),
                testCaseParams.verifyResult);

            HksFree(publicKey.data);
            HksFree(signData.data);
        }

        HksFree(localKey.blob.data);
        HksFreeParamSet(&paramSetOut);
        HksFreeParamSet(&paramInSet);
    }

    void VerifyLocalTestCase(const TestCaseParams &testCaseParams)
    {
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        ASSERT_NE(paramInSet, nullptr);

        struct HksParamSet *paramSetOut = nullptr;
        HksInitParamSet(&paramSetOut);
        struct HksParam localKey = {
            .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) }
        };
        ASSERT_NE(localKey.blob.data, nullptr);
        HksAddParams(paramSetOut, &localKey, 1);

        HksBuildParamSet(&paramSetOut);

        EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

        EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

        EXPECT_EQ(HksGenerateKey(NULL, paramInSet, paramSetOut), testCaseParams.generateKeyResult);
        if (testCaseParams.generateKeyResult == HKS_SUCCESS) {
            HksParam *pubKeyExport = nullptr;
            EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubKeyExport), HKS_SUCCESS);

            HksBlob publicKey = {
                .size = pubKeyExport->blob.size,
                .data = (uint8_t *)HksMalloc(pubKeyExport->blob.size)
            };
            ASSERT_NE(publicKey.data, nullptr);
            (void)memcpy_s(publicKey.data, pubKeyExport->blob.size, pubKeyExport->blob.data, pubKeyExport->blob.size);

            HksParam *priKeyExport = nullptr;
            EXPECT_EQ(HksGetParam(paramSetOut, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priKeyExport), HKS_SUCCESS);

            HksBlob privateKey = {
                .size = priKeyExport->blob.size,
                .data = (uint8_t *)HksMalloc(priKeyExport->blob.size)
            };
            ASSERT_NE(privateKey.data, nullptr);
            (void)memcpy_s(privateKey.data, priKeyExport->blob.size, priKeyExport->blob.data, priKeyExport->blob.size);

            const char *hexData = "00112233445566778899aabbccddeeff"; // as plainText data input

            HksBlob plainText = { .size = (uint32_t)strlen(hexData), .data = (uint8_t *)hexData };

            HksBlob signData = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
            ASSERT_NE(signData.data, nullptr);

            EXPECT_EQ(OpensslSignDsa(&plainText, &signData, &privateKey, testCaseParams.keyDigest),
                testCaseParams.signResult);

            EXPECT_EQ(HksVerify(&publicKey, paramInSet, &plainText, &signData), testCaseParams.verifyResult);

            HksFree(publicKey.data);
            HksFree(privateKey.data);
            HksFree(signData.data);
        }
        HksFree(localKey.blob.data);
        HksFreeParamSet(&paramSetOut);
        HksFreeParamSet(&paramInSet);
    }

    void VerifyServiceTestCase(const TestCaseParams &testCaseParams)
    {
        struct HksBlob authId = { (uint32_t)strlen(TEST_KEY_AUTH_ID), (uint8_t *)TEST_KEY_AUTH_ID };

        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        ASSERT_NE(paramInSet, nullptr);

        EXPECT_EQ(HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size()), HKS_SUCCESS);

        EXPECT_EQ(HksBuildParamSet(&paramInSet), HKS_SUCCESS);

        const char *hexData = "00112233445566778899aabbccddeeff";

        HksBlob plainText = { .size = (uint32_t)strlen(hexData), .data = (uint8_t *)hexData };

        HksBlob signData = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
        ASSERT_NE(signData.data, nullptr);

        struct HksBlob opensslDsaKeyInfo = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
        ASSERT_NE(opensslDsaKeyInfo.data, nullptr);

        struct HksBlob x509Key = { .size = SET_SIZE_4096, .data = (uint8_t *)HksMalloc(SET_SIZE_4096) };
        ASSERT_NE(x509Key.data, nullptr);

        EVP_PKEY *pkey = GenerateDsaKey(KEY_SIZE_1024);
        ASSERT_NE(pkey, nullptr);

        DsaGetx509PubKey(pkey, &x509Key);

        EXPECT_EQ(HksImportKey(&authId, paramInSet, &x509Key), HKS_SUCCESS);

        SaveDsaKeyToHksBlob(pkey, KEY_SIZE_1024, &opensslDsaKeyInfo);

        EXPECT_EQ(OpensslSignDsa(&plainText, &signData, &opensslDsaKeyInfo, testCaseParams.keyDigest),
            testCaseParams.signResult);

        EXPECT_EQ(HksVerify(&authId, paramInSet, &plainText, &signData), testCaseParams.verifyResult);

        EVP_PKEY_free(pkey);
        HksFree(signData.data);
        HksFree(x509Key.data);
        HksFree(opensslDsaKeyInfo.data);
        HksFreeParamSet(&paramInSet);
    }
};

/**
 * @tc.number    : HksDsaMT00100
 * @tc.name      : HksDsaMT00100
 * @tc.desc      : Test huks generate key (1024/SHA1withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT00100, TestSize.Level0)
{
    GenerateKeyTestCase(HKS_DSA_MT_00100_PARAMS);
}

/**
 * @tc.number    : HksDsaMT00200
 * @tc.name      : HksDsaMT00200
 * @tc.desc      : Test huks sign (1024/SHA1withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT00200, TestSize.Level0)
{
    SignLocalTestCase(HKS_DSA_MT_00200_PARAMS);
}

/**
 * @tc.number    : HksDsaMT00300
 * @tc.name      : HksDsaMT00300
 * @tc.desc      : Test huks sign (1024/SHA1withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT00300, TestSize.Level0)
{
    SignServiceTestCase(HKS_DSA_MT_00300_PARAMS);
}

/**
 * @tc.number    : HksDsaMT00400
 * @tc.name      : HksDsaMT00400
 * @tc.desc      : Test huks Verify (1024/SHA1withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT00400, TestSize.Level0)
{
    VerifyLocalTestCase(HKS_DSA_MT_00400_PARAMS);
}

/**
 * @tc.number    : HksDsaMT00500
 * @tc.name      : HksDsaMT00500
 * @tc.desc      : Test huks Verify (1024/SHA1withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT00500, TestSize.Level0)
{
    VerifyServiceTestCase(HKS_DSA_MT_00500_PARAMS);
}

/**
 * @tc.number    : HksDsaMT00600
 * @tc.name      : HksDsaMT00600
 * @tc.desc      : Test huks generate key (1024/SHA224withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT00600, TestSize.Level0)
{
    GenerateKeyTestCase(HKS_DSA_MT_00600_PARAMS);
}

/**
 * @tc.number    : HksDsaMT00700
 * @tc.name      : HksDsaMT00700
 * @tc.desc      : Test huks sign (1024/SHA224withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT00700, TestSize.Level0)
{
    SignLocalTestCase(HKS_DSA_MT_00700_PARAMS);
}

/**
 * @tc.number    : HksDsaMT00800
 * @tc.name      : HksDsaMT00800
 * @tc.desc      : Test huks sign (1024/SHA224withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT00800, TestSize.Level0)
{
    SignServiceTestCase(HKS_DSA_MT_00800_PARAMS);
}

/**
 * @tc.number    : HksDsaMT00900
 * @tc.name      : HksDsaMT00900
 * @tc.desc      : Test huks Verify (1024/SHA224withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT00900, TestSize.Level0)
{
    VerifyLocalTestCase(HKS_DSA_MT_00900_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01000
 * @tc.name      : HksDsaMT01000
 * @tc.desc      : Test huks Verify (1024/SHA224withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT01000, TestSize.Level0)
{
    VerifyServiceTestCase(HKS_DSA_MT_01000_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01100
 * @tc.name      : HksDsaMT01100
 * @tc.desc      : Test huks generate key (1024/SHA256withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT01100, TestSize.Level0)
{
    GenerateKeyTestCase(HKS_DSA_MT_01100_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01200
 * @tc.name      : HksDsaMT01200
 * @tc.desc      : Test huks sign (1024/SHA256withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT01200, TestSize.Level0)
{
    SignLocalTestCase(HKS_DSA_MT_01200_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01300
 * @tc.name      : HksDsaMT01300
 * @tc.desc      : Test huks sign (1024/SHA256withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT01300, TestSize.Level0)
{
    SignServiceTestCase(HKS_DSA_MT_01300_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01400
 * @tc.name      : HksDsaMT01400
 * @tc.desc      : Test huks Verify (1024/SHA256withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT01400, TestSize.Level0)
{
    VerifyLocalTestCase(HKS_DSA_MT_01400_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01500
 * @tc.name      : HksDsaMT01500
 * @tc.desc      : Test huks Verify (1024/SHA256withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT01500, TestSize.Level0)
{
    VerifyServiceTestCase(HKS_DSA_MT_01500_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01600
 * @tc.name      : HksDsaMT01600
 * @tc.desc      : Test huks generate key (1024/SHA384withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT01600, TestSize.Level0)
{
    GenerateKeyTestCase(HKS_DSA_MT_01600_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01700
 * @tc.name      : HksDsaMT01700
 * @tc.desc      : Test huks sign (1024/SHA384withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT01700, TestSize.Level0)
{
    SignLocalTestCase(HKS_DSA_MT_01700_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01800
 * @tc.name      : HksDsaMT01800
 * @tc.desc      : Test huks sign (1024/SHA384withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT01800, TestSize.Level0)
{
    SignServiceTestCase(HKS_DSA_MT_01800_PARAMS);
}

/**
 * @tc.number    : HksDsaMT01900
 * @tc.name      : HksDsaMT01900
 * @tc.desc      : Test huks Verify (1024/SHA384withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT01900, TestSize.Level0)
{
    VerifyLocalTestCase(HKS_DSA_MT_01900_PARAMS);
}

/**
 * @tc.number    : HksDsaMT02000
 * @tc.name      : HksDsaMT02000
 * @tc.desc      : Test huks Verify (1024/SHA384withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT02000, TestSize.Level0)
{
    VerifyServiceTestCase(HKS_DSA_MT_02000_PARAMS);
}

/**
 * @tc.number    : HksDsaMT02100
 * @tc.name      : HksDsaMT02100
 * @tc.desc      : Test huks generate key (1024/SHA512withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT02100, TestSize.Level0)
{
    GenerateKeyTestCase(HKS_DSA_MT_02100_PARAMS);
}

/**
 * @tc.number    : HksDsaMT02200
 * @tc.name      : HksDsaMT02200
 * @tc.desc      : Test huks sign (1024/SHA512withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT02200, TestSize.Level0)
{
    SignLocalTestCase(HKS_DSA_MT_02200_PARAMS);
}

/**
 * @tc.number    : HksDsaMT02300
 * @tc.name      : HksDsaMT02300
 * @tc.desc      : Test huks sign (1024/SHA512withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT02300, TestSize.Level0)
{
    SignServiceTestCase(HKS_DSA_MT_02300_PARAMS);
}

/**
 * @tc.number    : HksDsaMT02400
 * @tc.name      : HksDsaMT02400
 * @tc.desc      : Test huks Verify (1024/SHA512withDSA/TEMP)
 */
HWTEST_F(HksDsaMt, HksDsaMT02400, TestSize.Level0)
{
    VerifyLocalTestCase(HKS_DSA_MT_02400_PARAMS);
}

/**
 * @tc.number    : HksDsaMT02500
 * @tc.name      : HksDsaMT02500
 * @tc.desc      : Test huks Verify (1024/SHA512withDSA/PERSISTENT)
 */
HWTEST_F(HksDsaMt, HksDsaMT02500, TestSize.Level0)
{
    VerifyServiceTestCase(HKS_DSA_MT_02500_PARAMS);
}
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS
