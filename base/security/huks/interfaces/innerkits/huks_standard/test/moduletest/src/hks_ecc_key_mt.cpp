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
#include <securec.h>

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

const TestCaseParams HUKS_ECC_KEY_MT_00100_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_00200_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_00300_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_00400_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_00500_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_00600_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_00700_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_00800_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_00900_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01000_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01100_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01200_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01300_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01400_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01500_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01600_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01700_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01800_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_01900_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_02000_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_02100_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_02200_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_02300_PARAMS = {
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

const TestCaseParams HUKS_ECC_KEY_MT_02400_PARAMS = {
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
}  // namespace

class HksEccKeyMt : public testing::Test {
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

        struct HksParamSet *paramOutSet = nullptr;
        HksInitParamSet(&paramOutSet);
        struct HksParam localKey = {
            .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = {
                .size = HKS_ECC_KEY_SIZE_521,
                .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_521)
            }
        };
        ASSERT_NE(localKey.blob.data, nullptr);
        HksAddParams(paramOutSet, &localKey, 1);

        HksBuildParamSet(&paramOutSet);

        EXPECT_EQ(HksGenerateKey(&authId, paramInSet, paramOutSet), testCaseParams.generateKeyResult);
        HksParam *priParam = nullptr;
        HksGetParam(paramOutSet, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priParam);
        HksBlob priKey = { .size = priParam->blob.size, .data = (uint8_t *)HksMalloc(priParam->blob.size) };
        ASSERT_NE(priKey.data, nullptr);
        (void)memcpy_s(priKey.data, priParam->blob.size, priParam->blob.data, priParam->blob.size);

        HksParam *pubParam = nullptr;
        HksGetParam(paramOutSet, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubParam);
        HksBlob pubKey = { .size = pubParam->blob.size, .data = (uint8_t *)HksMalloc(pubParam->blob.size) };
        ASSERT_NE(pubKey.data, nullptr);
        (void)memcpy_s(pubKey.data, pubParam->blob.size, pubParam->blob.data, pubParam->blob.size);

        HksBlob message = {
            .size = (uint32_t)testCaseParams.hexData.length(),
            .data = (uint8_t *)&testCaseParams.hexData[0],
        };
        HksBlob signature = { .size = ECC_MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(ECC_MESSAGE_SIZE) };
        ASSERT_NE(signature.data, nullptr);

        EXPECT_EQ(EcdsaSign(&priKey, digest, &message, &signature), testCaseParams.signResult);
        EXPECT_EQ(EcdsaVerify(&pubKey, digest, &message, &signature), testCaseParams.verifyResult);

        HksFreeParamSet(&paramInSet);
        HksFree(localKey.blob.data);
        HksFreeParamSet(&paramOutSet);
        HksFree(priKey.data);
        HksFree(pubKey.data);
        HksFree(signature.data);
    }
};

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt00100
 * @tc.name      : HksEccKeyMt00100
 * @tc.desc      : Huks generates ECC224 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-NONE algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt00100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_00100_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt00200
 * @tc.name      : HksEccKeyMt00200
 * @tc.desc      : Huks generates ECC224 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA1 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt00200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_00200_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt00300
 * @tc.name      : HksEccKeyMt00300
 * @tc.desc      : Huks generates ECC224 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA224 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt00300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_00300_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt00400
 * @tc.name      : HksEccKeyMt00400
 * @tc.desc      : Huks generates ECC224 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA256 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt00400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_00400_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt00500
 * @tc.name      : HksEccKeyMt00500
 * @tc.desc      : Huks generates ECC224 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA384 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt00500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_00500_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt00600
 * @tc.name      : HksEccKeyMt00600
 * @tc.desc      : Huks generates ECC224 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA512 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt00600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_00600_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt00700
 * @tc.name      : HksEccKeyMt00700
 * @tc.desc      : Huks generates ECC256 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-NONE algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt00700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_00700_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt00800
 * @tc.name      : HksEccKeyMt00800
 * @tc.desc      : Huks generates ECC256 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA1 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt00800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_00800_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt00900
 * @tc.name      : HksEccKeyMt00900
 * @tc.desc      : Huks generates ECC256 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA224 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt00900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_00900_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01000
 * @tc.name      : HksEccKeyMt01000
 * @tc.desc      : Huks generates ECC256 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA256 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01000_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01100
 * @tc.name      : HksEccKeyMt01100
 * @tc.desc      : Huks generates ECC256 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA384 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01100_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01200
 * @tc.name      : HksEccKeyMt01200
 * @tc.desc      : Huks generates ECC256 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA512 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01200_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01300
 * @tc.name      : HksEccKeyMt01300
 * @tc.desc      : Huks generates ECC384 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-NONE algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01300_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01400
 * @tc.name      : HksEccKeyMt01400
 * @tc.desc      : Huks generates ECC384 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA1 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01400_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01500
 * @tc.name      : HksEccKeyMt01500
 * @tc.desc      : Huks generates ECC384 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA224 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01500_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01600
 * @tc.name      : HksEccKeyMt01600
 * @tc.desc      : Huks generates ECC384 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA256 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01600_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01700
 * @tc.name      : HksEccKeyMt01700
 * @tc.desc      : Huks generates ECC384 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA384 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01700_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01800
 * @tc.name      : HksEccKeyMt01800
 * @tc.desc      : Huks generates ECC384 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA512 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01800_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt01900
 * @tc.name      : HksEccKeyMt01900
 * @tc.desc      : Huks generates ECC521 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-NONE algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt01900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_01900_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt02000
 * @tc.name      : HksEccKeyMt02000
 * @tc.desc      : Huks generates ECC521 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA1 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt02000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_02000_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt02100
 * @tc.name      : HksEccKeyMt02100
 * @tc.desc      : Huks generates ECC521 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA224 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt02100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_02100_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt02200
 * @tc.name      : HksEccKeyMt02200
 * @tc.desc      : Huks generates ECC521 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA256 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt02200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_02200_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt02300
 * @tc.name      : HksEccKeyMt02300
 * @tc.desc      : Huks generates ECC521 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA384 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt02300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_02300_PARAMS);
}

/**
 * @tc.number    : HksEccKeyMt.HksEccKeyMt02400
 * @tc.name      : HksEccKeyMt02400
 * @tc.desc      : Huks generates ECC521 bit key, which can be successfully used for OpenSSL sign/verify using
 * ECC/DIGEST-SHA512 algorithm
 */
HWTEST_F(HksEccKeyMt, HksEccKeyMt02400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_KEY_MT_02400_PARAMS);
}
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS