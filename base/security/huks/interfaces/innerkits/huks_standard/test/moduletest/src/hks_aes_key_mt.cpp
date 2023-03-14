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
#include "openssl_aes_helper.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace MT {
namespace {
struct TestCaseParams {
    std::vector<HksParam> params;
    std::string hexData;
    uint32_t inLenAppend = 0;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode encryptResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode decryptResult = HksErrorCode::HKS_SUCCESS;
};

const uint8_t IV[IV_SIZE] = {0};
const uint8_t AAD_DATA[AAD_SIZE] = {0};
const char TEST_AES_KEY[] = "This is a AES key";

const TestCaseParams HUKS_AES_KEY_MT_00100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = COMPLEMENT_LEN,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_00200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_00300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CTR },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_00400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_00500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_00600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_NONCE, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
        { .tag = HKS_TAG_ASSOCIATED_DATA, .blob = { .size = sizeof(AAD_DATA), .data = (uint8_t*)AAD_DATA } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_00700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = COMPLEMENT_LEN,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_00800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_00900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CTR },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_01000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_01100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_01200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_192 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_NONCE, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
        { .tag = HKS_TAG_ASSOCIATED_DATA, .blob = { .size = sizeof(AAD_DATA), .data = (uint8_t*)AAD_DATA } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_01300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = COMPLEMENT_LEN,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_01400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_01500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CTR },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_01600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_01700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_ECB },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AES_KEY_MT_01800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_NONCE, .blob = { .size = IV_SIZE, .data = (uint8_t*)IV } },
        { .tag = HKS_TAG_ASSOCIATED_DATA, .blob = { .size = sizeof(AAD_DATA), .data = (uint8_t*)AAD_DATA } },
    },
    .hexData = "0123456789abcdef",
    .inLenAppend = 0,
    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksAesKeyMt : public testing::Test {
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
        struct HksBlob authId = { (uint32_t)strlen(TEST_AES_KEY), (uint8_t *)TEST_AES_KEY };

        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);

        HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size());
        HksBuildParamSet(&paramInSet);

        uint32_t blockMode = ReadValueByTag(testCaseParams.params, HKS_TAG_BLOCK_MODE);

        struct HksParamSet *paramSetOut = (struct HksParamSet *)HksMalloc(OUT_PARAMSET_SIZE);
        ASSERT_NE(paramSetOut, nullptr);
        (void)memset_s(paramSetOut, OUT_PARAMSET_SIZE, 0, OUT_PARAMSET_SIZE);
        paramSetOut->paramSetSize = OUT_PARAMSET_SIZE;

        EXPECT_EQ(HksGenerateKey(&authId, paramInSet, paramSetOut), testCaseParams.generateKeyResult);

        HksParam *symmetricParam = NULL;
        HksGetParam(paramSetOut, HKS_TAG_SYMMETRIC_KEY_DATA, &symmetricParam);
        HksBlob symmetricKey = {
            .size = symmetricParam->blob.size,
            .data = (uint8_t *)HksMalloc(symmetricParam->blob.size)
        };
        ASSERT_NE(symmetricKey.data, nullptr);
        (void)memcpy_s(
            symmetricKey.data, symmetricParam->blob.size, symmetricParam->blob.data, symmetricParam->blob.size);

        HksBlob plainText = {
            .size = (uint32_t)testCaseParams.hexData.length(),
            .data = (uint8_t *)&testCaseParams.hexData[0],
        };

        uint32_t inLen = testCaseParams.hexData.length() + COMPLEMENT_LEN;
        HksBlob cipherText = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_NE(cipherText.data, nullptr);
        HksBlob plainTextDecrypt = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_NE(plainTextDecrypt.data, nullptr);
        if (blockMode == HKS_MODE_GCM) {
            HksBlob tagAead = { .size = AAD_SIZE, .data = (uint8_t *)HksMalloc(AAD_SIZE) };
            ASSERT_NE(tagAead.data, nullptr);
            EXPECT_EQ(AesGcmEncrypt(paramInSet, &plainText, &cipherText, &symmetricKey, &tagAead),
                testCaseParams.encryptResult);
            EXPECT_EQ(AesGcmDecrypt(paramInSet, &cipherText, &plainTextDecrypt, &symmetricKey, &tagAead),
                testCaseParams.decryptResult);
            HksFree(tagAead.data);
        } else {
            EXPECT_EQ(AesEncrypt(paramInSet, &plainText, &cipherText, &symmetricKey), testCaseParams.encryptResult);
            EXPECT_EQ(
                AesDecrypt(paramInSet, &cipherText, &plainTextDecrypt, &symmetricKey), testCaseParams.decryptResult);
        }
        EXPECT_EQ(plainTextDecrypt.size, testCaseParams.hexData.length());
        EXPECT_EQ(HksMemCmp(plainText.data, plainTextDecrypt.data, testCaseParams.hexData.length()), 0);

        HksFree(paramSetOut);
        HksFree(symmetricKey.data);
        HksFree(cipherText.data);
        HksFree(plainTextDecrypt.data);
        HksFreeParamSet(&paramInSet);
    }
};

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt00100
 * @tc.name      : HksAesKeyMt00100
 * @tc.desc      : Huks generates aes128 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/CBC/pkcs7padding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt00100, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_00100_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt00200
 * @tc.name      : HksAesKeyMt00200
 * @tc.desc      : Huks generates aes128 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/CBC/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt00200, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_00200_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt00300
 * @tc.name      : HksAesKeyMt00300
 * @tc.desc      : Huks generates aes128 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/CTR/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt00300, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_00300_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt00400
 * @tc.name      : HksAesKeyMt00400
 * @tc.desc      : Huks generates aes128 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/ECB/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt00400, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_00400_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt00500
 * @tc.name      : HksAesKeyMt00500
 * @tc.desc      : Huks generates aes128 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/ECB/pkcs7padding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt00500, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_00500_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt00600
 * @tc.name      : HksAesKeyMt00600
 * @tc.desc      : Huks generates aes128 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/GCM/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt00600, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_00600_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt00700
 * @tc.name      : HksAesKeyMt00700
 * @tc.desc      : Huks generates aes192 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/CBC/pkcs7padding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt00700, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_00700_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt00800
 * @tc.name      : HksAesKeyMt00800
 * @tc.desc      : Huks generates aes192 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/CBC/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt00800, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_00800_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt00900
 * @tc.name      : HksAesKeyMt00900
 * @tc.desc      : Huks generates aes192 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/CTR/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt00900, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_00900_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt01000
 * @tc.name      : HksAesKeyMt01000
 * @tc.desc      : Huks generates aes192 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/ECB/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt01000, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_01000_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt01100
 * @tc.name      : HksAesKeyMt01100
 * @tc.desc      : Huks generates aes192 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/ECB/pkcs7padding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt01100, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_01100_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt01200
 * @tc.name      : HksAesKeyMt01200
 * @tc.desc      : Huks generates aes192 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/GCM/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt01200, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_01200_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt01300
 * @tc.name      : HksAesKeyMt01300
 * @tc.desc      : Huks generates aes256 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/CBC/pkcs7padding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt01300, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_01300_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt01400
 * @tc.name      : HksAesKeyMt01400
 * @tc.desc      : Huks generates aes256 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/CBC/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt01400, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_01400_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt01500
 * @tc.name      : HksAesKeyMt01500
 * @tc.desc      : Huks generates aes256 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/CTR/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt01500, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_01500_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt01600
 * @tc.name      : HksAesKeyMt01600
 * @tc.desc      : Huks generates aes256 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/ECB/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt01600, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_01600_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt01700
 * @tc.name      : HksAesKeyMt01700
 * @tc.desc      : Huks generates aes256 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/ECB/pkcs7padding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt01700, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_01700_PARAMS);
}

/**
 * @tc.number    : HksAesKeyMt.HksAesKeyMt01800
 * @tc.name      : HksAesKeyMt01800
 * @tc.desc      : Huks generates aes256 bit key, which can be successfully used for OpenSSL encryption/decryption using
 * AES/GCM/nopadding algorithm
 */
HWTEST_F(HksAesKeyMt, HksAesKeyMt01800, TestSize.Level0)
{
    RunTestCase(HUKS_AES_KEY_MT_01800_PARAMS);
}
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS