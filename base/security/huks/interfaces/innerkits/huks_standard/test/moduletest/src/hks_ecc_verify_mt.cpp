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
const char PUB_KEY[] = "This is a public key";

const TestCaseParams HUKS_ECC_VERIFY_MT_00100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_00200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_00300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_00400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_00500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_00600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_00700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_00800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_00900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_01900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_02900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_03900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_04000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_04100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_04200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_04300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_04400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_04500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_04600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_04700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_ECC_VERIFY_MT_04800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
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

class HksEccVerifyMt : public testing::Test {
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
        struct HksBlob authId = { .size = ECC_KEY_SIZE, .data = (uint8_t *)HksMalloc(ECC_KEY_SIZE) };
        ASSERT_NE(authId.data, nullptr);

        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);

        HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size());
        HksBuildParamSet(&paramInSet);

        uint32_t digest = ReadValueByTag(testCaseParams.params, HKS_TAG_DIGEST);
        uint32_t storage = ReadValueByTag(testCaseParams.params, HKS_TAG_KEY_STORAGE_FLAG);

        struct HksBlob pubKey = { .size = ECC_KEY_SIZE, .data = (uint8_t *)HksMalloc(ECC_KEY_SIZE) };
        ASSERT_NE(pubKey.data, nullptr);

        uint32_t keySize = ReadValueByTag(testCaseParams.params, HKS_TAG_KEY_SIZE);
        EXPECT_EQ(EccGenerateKey(keySize, &authId), testCaseParams.generateKeyResult);
        EXPECT_EQ(GetEccPubKey(&authId, &pubKey), ECC_SUCCESS);

        HksBlob message = {
            .size = (uint32_t)testCaseParams.hexData.length(),
            .data = (uint8_t *)&testCaseParams.hexData[0],
        };
        HksBlob signature = { .size = ECC_MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(ECC_MESSAGE_SIZE) };
        ASSERT_NE(signature.data, nullptr);

        EXPECT_EQ(EcdsaSign(&authId, digest, &message, &signature), testCaseParams.signResult);

        if (storage == HKS_STORAGE_TEMP) {
            EXPECT_EQ(HksVerify(&pubKey, paramInSet, &message, &signature), testCaseParams.verifyResult);
        } else if (storage == HKS_STORAGE_PERSISTENT) {
            HksBlob x509Key = { .size = ECC_KEY_SIZE, .data = (uint8_t *)HksMalloc(ECC_KEY_SIZE) };
            ASSERT_NE(x509Key.data, nullptr);
            EXPECT_EQ(HksBlobToX509(&pubKey, &x509Key), ECC_SUCCESS);
            struct HksBlob pubId = { (uint32_t)strlen(PUB_KEY), (uint8_t *)PUB_KEY };
            EXPECT_EQ(HksImportKey(&pubId, paramInSet, &x509Key), HKS_SUCCESS);
            EXPECT_EQ(HksVerify(&pubId, paramInSet, &message, &signature), testCaseParams.verifyResult);
            HksFree(x509Key.data);
        }

        HksFree(authId.data);
        HksFreeParamSet(&paramInSet);
        HksFree(pubKey.data);
        HksFree(signature.data);
    }
};

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt00100
 * @tc.name      : HksEccVerifyMt00100
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-NONE algorithm, and huks uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt00100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_00100_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt00200
 * @tc.name      : HksEccVerifyMt00200
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA1 algorithm, and huks uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt00200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_00200_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt00300
 * @tc.name      : HksEccVerifyMt00300
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA224 algorithm, and huks uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt00300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_00300_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt00400
 * @tc.name      : HksEccVerifyMt00400
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA256 algorithm, and huks uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt00400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_00400_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt00500
 * @tc.name      : HksEccVerifyMt00500
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA384 algorithm, and huks uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt00500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_00500_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt00600
 * @tc.name      : HksEccVerifyMt00600
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA512 algorithm, and huks uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt00600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_00600_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt00700
 * @tc.name      : HksEccVerifyMt00700
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-NONE algorithm, and huks uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt00700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_00700_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt00800
 * @tc.name      : HksEccVerifyMt00800
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA1 algorithm, and huks uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt00800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_00800_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt00900
 * @tc.name      : HksEccVerifyMt00900
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA224 algorithm, and huks uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt00900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_00900_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01000
 * @tc.name      : HksEccVerifyMt01000
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA256 algorithm, and huks uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01000_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01100
 * @tc.name      : HksEccVerifyMt01100
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA384 algorithm, and huks uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01100_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01200
 * @tc.name      : HksEccVerifyMt01200
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA512 algorithm, and huks uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01200_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01300
 * @tc.name      : HksEccVerifyMt01300
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-NONE algorithm, and huks uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01300_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01400
 * @tc.name      : HksEccVerifyMt01400
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA1 algorithm, and huks uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01400_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01500
 * @tc.name      : HksEccVerifyMt01500
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA224 algorithm, and huks uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01500_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01600
 * @tc.name      : HksEccVerifyMt01600
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA256 algorithm, and huks uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01600_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01700
 * @tc.name      : HksEccVerifyMt01700
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA384 algorithm, and huks uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01700_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01800
 * @tc.name      : HksEccVerifyMt01800
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA512 algorithm, and huks uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01800_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt01900
 * @tc.name      : HksEccVerifyMt01900
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-NONE algorithm, and huks uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt01900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_01900_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02000
 * @tc.name      : HksEccVerifyMt02000
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA1 algorithm, and huks uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02000_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02100
 * @tc.name      : HksEccVerifyMt02100
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA224 algorithm, and huks uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02100_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02200
 * @tc.name      : HksEccVerifyMt02200
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA256 algorithm, and huks uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02200_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02300
 * @tc.name      : HksEccVerifyMt02300
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA384 algorithm, and huks uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02300_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02400
 * @tc.name      : HksEccVerifyMt02400
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA512 algorithm, and huks uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02400_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02500
 * @tc.name      : HksEccVerifyMt02500
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-NONE algorithm, and huks uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02500_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02600
 * @tc.name      : HksEccVerifyMt02600
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA1 algorithm, and huks uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02600_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02700
 * @tc.name      : HksEccVerifyMt02700
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA224 algorithm, and huks uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02700_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02800
 * @tc.name      : HksEccVerifyMt02800
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA256 algorithm, and huks uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02800_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt02900
 * @tc.name      : HksEccVerifyMt02900
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA384 algorithm, and huks uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt02900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_02900_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03000
 * @tc.name      : HksEccVerifyMt03000
 * @tc.desc      : OpenSSL generates an ecc224 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA512 algorithm, and huks uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03000_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03100
 * @tc.name      : HksEccVerifyMt03100
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-NONE algorithm, and huks uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03100_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03200
 * @tc.name      : HksEccVerifyMt03200
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA1 algorithm, and huks uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03200_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03300
 * @tc.name      : HksEccVerifyMt03300
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA224 algorithm, and huks uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03300_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03400
 * @tc.name      : HksEccVerifyMt03400
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA256 algorithm, and huks uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03400_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03500
 * @tc.name      : HksEccVerifyMt03500
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA384 algorithm, and huks uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03500_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03600
 * @tc.name      : HksEccVerifyMt03600
 * @tc.desc      : OpenSSL generates an ecc256 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA512 algorithm, and huks uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03600_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03700
 * @tc.name      : HksEccVerifyMt03700
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-NONE algorithm, and huks uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03700_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03800
 * @tc.name      : HksEccVerifyMt03800
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA1 algorithm, and huks uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03800_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt03900
 * @tc.name      : HksEccVerifyMt03900
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA224 algorithm, and huks uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt03900, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_03900_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt04000
 * @tc.name      : HksEccVerifyMt04000
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA256 algorithm, and huks uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt04000, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_04000_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt04100
 * @tc.name      : HksEccVerifyMt04100
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA384 algorithm, and huks uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt04100, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_04100_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt04200
 * @tc.name      : HksEccVerifyMt04200
 * @tc.desc      : OpenSSL generates an ecc384 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA512 algorithm, and huks uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt04200, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_04200_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt04300
 * @tc.name      : HksEccVerifyMt04300
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-NONE algorithm, and huks uses ECC/DIGEST-NONE algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt04300, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_04300_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt04400
 * @tc.name      : HksEccVerifyMt04400
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA1 algorithm, and huks uses ECC/DIGEST-SHA1 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt04400, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_04400_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt04500
 * @tc.name      : HksEccVerifyMt04500
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA224 algorithm, and huks uses ECC/DIGEST-SHA224 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt04500, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_04500_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt04600
 * @tc.name      : HksEccVerifyMt04600
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA256 algorithm, and huks uses ECC/DIGEST-SHA256 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt04600, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_04600_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt04700
 * @tc.name      : HksEccVerifyMt04700
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA384 algorithm, and huks uses ECC/DIGEST-SHA384 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt04700, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_04700_PARAMS);
}

/**
 * @tc.number    : HksEccVerifyMt.HksEccVerifyMt04800
 * @tc.name      : HksEccVerifyMt04800
 * @tc.desc      : OpenSSL generates an ecc521 bit key, which can be successfully used for OpenSSL signing with
 * ECC/DIGEST-SHA512 algorithm, and huks uses ECC/DIGEST-SHA512 algorithm for verification
 */
HWTEST_F(HksEccVerifyMt, HksEccVerifyMt04800, TestSize.Level0)
{
    RunTestCase(HUKS_ECC_VERIFY_MT_04800_PARAMS);
}
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS