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

#include "hks_api.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "openssl_ecc_helper.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace MT {
namespace {
struct TestCaseParams {
    std::vector<HksParam> params;
    std::string hexData;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode signResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode verifyResult = HksErrorCode::HKS_SUCCESS;
};

const char ECC_KEY[] = "This is a ECC key";

const TestCaseParams HUKS_ECC_SIGN_MT_00100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_00200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_00300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_00400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_00500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_00600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_00700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_00800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_00900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_01900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_02900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_03900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_04000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_04100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_04200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_04300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_04400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_04500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_04600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_04700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_SIGN_MT_04800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};
}  // namespace

class HksEccSignMt : public testing::Test {
protected:
    uint32_t ReadValueByTag(std::vector<HksParam> params, uint32_t needTag)
    {
        for (auto testParams : params) {
            if (testParams.tag == needTag) {
                return testParams.uint32Param;
            }
        }
        return 0;
    }

    void RunTestCase(const TestCaseParams &testCaseParams)
    {
        struct HksBlob authId = { (uint32_t)strlen(ECC_KEY), (uint8_t *)ECC_KEY };

        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);

        HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size());
        HksBuildParamSet(&paramInSet);

        uint32_t digest = ReadValueByTag(testCaseParams.params, HKS_TAG_DIGEST);
        uint32_t storage = ReadValueByTag(testCaseParams.params, HKS_TAG_KEY_STORAGE_FLAG);
        if (storage == HKS_STORAGE_TEMP) {
            authId = { .size = ECC_KEY_SIZE, .data = (uint8_t *)HksMalloc(ECC_KEY_SIZE) };
            ASSERT_NE(authId.data, nullptr);
        }

        struct HksBlob pubKey = { .size = ECC_KEY_SIZE, .data = (uint8_t *)HksMalloc(ECC_KEY_SIZE) };
        ASSERT_NE(pubKey.data, nullptr);
        HksBlob x509Key = { .size = ECC_KEY_SIZE, .data = (uint8_t *)HksMalloc(ECC_KEY_SIZE) };
        ASSERT_NE(x509Key.data, nullptr);

        if (storage == HKS_STORAGE_TEMP) {
            uint32_t keySize = ReadValueByTag(testCaseParams.params, HKS_TAG_KEY_SIZE);
            EXPECT_EQ(EccGenerateKey(keySize, &authId), testCaseParams.generateKeyResult);
            EXPECT_EQ(GetEccPubKey(&authId, &pubKey), ECC_SUCCESS);
        } else if (storage == HKS_STORAGE_PERSISTENT) {
            EXPECT_EQ(HksGenerateKey(&authId, paramInSet, NULL), testCaseParams.generateKeyResult);
            EXPECT_EQ(HksExportPublicKey(&authId, paramInSet, &x509Key), HKS_SUCCESS);
            EXPECT_EQ(X509ToHksBlob(&x509Key, &pubKey), ECC_SUCCESS);
        }

        HksBlob message = {
            .size = (uint32_t)testCaseParams.hexData.length(),
            .data = (uint8_t *)&testCaseParams.hexData[0],
        };
        HksBlob signature = { .size = ECC_MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(ECC_MESSAGE_SIZE) };
        ASSERT_NE(signature.data, nullptr);

        EXPECT_EQ(HksSign(&authId, paramInSet, &message, &signature), testCaseParams.signResult);
        EXPECT_EQ(EcdsaVerify(&pubKey, digest, &message, &signature), testCaseParams.verifyResult);

        if (storage == HKS_STORAGE_TEMP) {
            HksFree(authId.data);
        }
        HksFreeParamSet(&paramInSet);
        HksFree(pubKey.data);
        HksFree(x509Key.data);
        HksFree(signature.data);
    }
};

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt00100
 * @tc.name      : HksEccSignMt00100
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-NONE algorithm, and OpenSSL uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt00100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_00100_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt00200
 * @tc.name      : HksEccSignMt00200
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA1 algorithm, and OpenSSL uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt00200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_00200_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt00300
 * @tc.name      : HksEccSignMt00300
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA224 algorithm, and OpenSSL uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt00300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_00300_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt00400
 * @tc.name      : HksEccSignMt00400
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA256 algorithm, and OpenSSL uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt00400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_00400_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt00500
 * @tc.name      : HksEccSignMt00500
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA384 algorithm, and OpenSSL uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt00500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_00500_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt00600
 * @tc.name      : HksEccSignMt00600
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA512 algorithm, and OpenSSL uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt00600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_00600_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt00700
 * @tc.name      : HksEccSignMt00700
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-NONE algorithm, and OpenSSL uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt00700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_00700_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt00800
 * @tc.name      : HksEccSignMt00800
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA1 algorithm, and OpenSSL uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt00800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_00800_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt00900
 * @tc.name      : HksEccSignMt00900
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA224 algorithm, and OpenSSL uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt00900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_00900_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01000
 * @tc.name      : HksEccSignMt01000
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA256 algorithm, and OpenSSL uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01000_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01100
 * @tc.name      : HksEccSignMt01100
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA384 algorithm, and OpenSSL uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01100_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01200
 * @tc.name      : HksEccSignMt01200
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA512 algorithm, and OpenSSL uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01200_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01300
 * @tc.name      : HksEccSignMt01300
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-NONE algorithm, and OpenSSL uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01300_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01400
 * @tc.name      : HksEccSignMt01400
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA1 algorithm, and OpenSSL uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01400_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01500
 * @tc.name      : HksEccSignMt01500
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA224 algorithm, and OpenSSL uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01500_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01600
 * @tc.name      : HksEccSignMt01600
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA256 algorithm, and OpenSSL uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01600_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01700
 * @tc.name      : HksEccSignMt01700
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA384 algorithm, and OpenSSL uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01700_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01800
 * @tc.name      : HksEccSignMt01800
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA512 algorithm, and OpenSSL uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01800_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt01900
 * @tc.name      : HksEccSignMt01900
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-NONE algorithm, and OpenSSL uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt01900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_01900_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02000
 * @tc.name      : HksEccSignMt02000
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA1 algorithm, and OpenSSL uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02000_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02100
 * @tc.name      : HksEccSignMt02100
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA224 algorithm, and OpenSSL uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02100_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02200
 * @tc.name      : HksEccSignMt02200
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA256 algorithm, and OpenSSL uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02200_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02300
 * @tc.name      : HksEccSignMt02300
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA384 algorithm, and OpenSSL uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02300_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02400
 * @tc.name      : HksEccSignMt02400
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA512 algorithm, and OpenSSL uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02400_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02500
 * @tc.name      : HksEccSignMt02500
 * @tc.desc      : Huks generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-NONE algorithm, and OpenSSL uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02500_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02600
 * @tc.name      : HksEccSignMt02600
 * @tc.desc      : Huks generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA1 algorithm, and OpenSSL uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02600_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02700
 * @tc.name      : HksEccSignMt02700
 * @tc.desc      : Huks generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA224 algorithm, and OpenSSL uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02700_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02800
 * @tc.name      : HksEccSignMt02800
 * @tc.desc      : Huks generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA256 algorithm, and OpenSSL uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02800_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt02900
 * @tc.name      : HksEccSignMt02900
 * @tc.desc      : Huks generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA384 algorithm, and OpenSSL uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt02900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_02900_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03000
 * @tc.name      : HksEccSignMt03000
 * @tc.desc      : Huks generates an ecc224 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA512 algorithm, and OpenSSL uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03000_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03100
 * @tc.name      : HksEccSignMt03100
 * @tc.desc      : Huks generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-NONE algorithm, and OpenSSL uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03100_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03200
 * @tc.name      : HksEccSignMt03200
 * @tc.desc      : Huks generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA1 algorithm, and OpenSSL uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03200_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03300
 * @tc.name      : HksEccSignMt03300
 * @tc.desc      : Huks generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA224 algorithm, and OpenSSL uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03300_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03400
 * @tc.name      : HksEccSignMt03400
 * @tc.desc      : Huks generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA256 algorithm, and OpenSSL uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03400_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03500
 * @tc.name      : HksEccSignMt03500
 * @tc.desc      : Huks generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA384 algorithm, and OpenSSL uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03500_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03600
 * @tc.name      : HksEccSignMt03600
 * @tc.desc      : Huks generates an ecc256 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA512 algorithm, and OpenSSL uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03600_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03700
 * @tc.name      : HksEccSignMt03700
 * @tc.desc      : Huks generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-NONE algorithm, and OpenSSL uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03700_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03800
 * @tc.name      : HksEccSignMt03800
 * @tc.desc      : Huks generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA1 algorithm, and OpenSSL uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03800_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt03900
 * @tc.name      : HksEccSignMt03900
 * @tc.desc      : Huks generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA224 algorithm, and OpenSSL uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt03900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_03900_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt04000
 * @tc.name      : HksEccSignMt04000
 * @tc.desc      : Huks generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA256 algorithm, and OpenSSL uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt04000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_04000_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt04100
 * @tc.name      : HksEccSignMt04100
 * @tc.desc      : Huks generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA384 algorithm, and OpenSSL uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt04100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_04100_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt04200
 * @tc.name      : HksEccSignMt04200
 * @tc.desc      : Huks generates an ecc384 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA512 algorithm, and OpenSSL uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt04200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_04200_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt04300
 * @tc.name      : HksEccSignMt04300
 * @tc.desc      : Huks generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-NONE algorithm, and OpenSSL uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt04300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_04300_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt04400
 * @tc.name      : HksEccSignMt04400
 * @tc.desc      : Huks generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA1 algorithm, and OpenSSL uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt04400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_04400_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt04500
 * @tc.name      : HksEccSignMt04500
 * @tc.desc      : Huks generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA224 algorithm, and OpenSSL uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt04500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_04500_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt04600
 * @tc.name      : HksEccSignMt04600
 * @tc.desc      : Huks generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA256 algorithm, and OpenSSL uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt04600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_04600_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt04700
 * @tc.name      : HksEccSignMt04700
 * @tc.desc      : Huks generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA384 algorithm, and OpenSSL uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt04700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_04700_PARAMS);
}

/**
 * @tc.number    : HksEccSignMt.HksEccSignMt04800
 * @tc.name      : HksEccSignMt04800
 * @tc.desc      : Huks generates an ecc521 bit key, which can be successfully used for huks signing with
 * ECC/DIGEST-SHA512 algorithm, and OpenSSL uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccSignMt, HksEccSignMt04800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_SIGN_MT_04800_PARAMS);
}
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS