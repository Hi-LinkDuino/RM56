/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <iostream>

#include "hks_ability.h"
#include "hks_config.h"
#include "hks_crypto_hal.h"
#include "hks_crypto_hal_common.h"
#include "hks_mem.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace UnitTest {
namespace {
struct TestCaseParams {
    HksKeySpec spec = {0};
    HksUsageSpec usageSpec = {0};
    std::string hexData;
    HksStageType runStage = HksStageType::HKS_STAGE_THREE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode signResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode verifyResult = HksErrorCode::HKS_SUCCESS;
};

const uint32_t SIGNATURE_SIZE = 512;
const uint32_t MAX_PUB_KEY_SIZE = 1044;
const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_001_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_MD5,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_002_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_MD5,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_003_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_MD5,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_004_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_MD5,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_005_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_MD5,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_006_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_MD5,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_007_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_008_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_009_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_010_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_011_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_012_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_013_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_014_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_015_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_016_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_017_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_018_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_019_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_020_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_021_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_022_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_023_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_024_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_025_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_026_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_027_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_028_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_029_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_030_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_031_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_032_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_033_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_034_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_035_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_036_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_037_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_038_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_039_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_040_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PKCS1_V1_5,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_041_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_042_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_043_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_044_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_045_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_046_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_047_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_048_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_049_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_050_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_051_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_052_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_053_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_054_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_055_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_056_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_057_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_058_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_059_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
    .verifyResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#endif
#if defined(_USE_OPENSSL_)
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_060_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_061_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_062_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_063_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_064_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_065_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_MBEDTLS_)
    .signResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
    .verifyResult = HKS_ERROR_CRYPTO_ENGINE_ERROR,
#endif
#if defined(_USE_OPENSSL_)
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_066_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_067_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_068_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_SIGN_069_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_PSS,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
    },
    .hexData = "00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .signResult = HKS_SUCCESS,
    .verifyResult = HKS_SUCCESS,
};
}  // namespace

class HksCryptoHalRsaSign : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    void RunTestCase(const TestCaseParams &testCaseParams) const
    {
        HksBlob key = { .size = 0, .data = nullptr };

        EXPECT_EQ(HksCryptoHalGenerateKey(&testCaseParams.spec, &key), testCaseParams.generateKeyResult);

        uint32_t dataLen = testCaseParams.hexData.length() / HKS_COUNT_OF_HALF;

        HksBlob message = { .size = dataLen, .data = (uint8_t *)HksMalloc(dataLen) };
        ASSERT_NE(message.data, nullptr);
        for (uint32_t ii = 0; ii < dataLen; ii++) {
            message.data[ii] = ReadHex((const uint8_t *)&testCaseParams.hexData[HKS_COUNT_OF_HALF * ii]);
        }

        struct HksBlob* pBlob = nullptr;
        uint8_t hashData[HKS_HMAC_DIGEST_SHA512_LEN] = {0};
        struct HksBlob hash = { HKS_HMAC_DIGEST_SHA512_LEN, hashData };
        if (testCaseParams.runStage == HksStageType::HKS_STAGE_THREE) {
            /* NONEwithECDSA default sha256: ec_pkey_ctrl default md nid */
            struct HksUsageSpec usageSpec = testCaseParams.usageSpec;
            uint32_t inputDigest = usageSpec.digest;
            usageSpec.digest = (inputDigest == HKS_DIGEST_NONE) ? HKS_DIGEST_SHA256 : inputDigest;
            EXPECT_EQ(HksCryptoHalHash(usageSpec.digest, &message, &hash), HKS_SUCCESS);
            pBlob = &hash;
        } else {
            pBlob = &message;
        }

        struct HksBlob signature = { .size = SIGNATURE_SIZE, .data = (uint8_t *)HksMalloc(SIGNATURE_SIZE) };
        ASSERT_NE(signature.data, nullptr);

        EXPECT_EQ(HksCryptoHalSign(&key, &testCaseParams.usageSpec, pBlob, &signature), testCaseParams.signResult);

        struct HksBlob pubKey = { .size = MAX_PUB_KEY_SIZE, .data = (uint8_t *)HksMalloc(MAX_PUB_KEY_SIZE) };
        ASSERT_NE(pubKey.data, nullptr);

        EXPECT_EQ(HksCryptoHalGetPubKey(&key, &pubKey), HKS_SUCCESS);

        EXPECT_EQ(
            HksCryptoHalVerify(&pubKey, &testCaseParams.usageSpec, pBlob, &signature), testCaseParams.verifyResult);

        HksFree(message.data);
        HksFree(signature.data);
        HksFree(pubKey.data);
        HksFree(key.data);
    }
};

void HksCryptoHalRsaSign::SetUpTestCase(void)
{
}

void HksCryptoHalRsaSign::TearDownTestCase(void)
{
}

void HksCryptoHalRsaSign::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalRsaSign::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalRsaSign_001
 * @tc.name      : HksCryptoHalRsaSign_001
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-512-PKCS1PADDING-MD5 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_001, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_002
 * @tc.name      : HksCryptoHalRsaSign_002
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PKCS1PADDING-MD5 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_002, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_003
 * @tc.name      : HksCryptoHalRsaSign_003
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PKCS1PADDING-MD5 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_003, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_003_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_004
 * @tc.name      : HksCryptoHalRsaSign_004
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PKCS1PADDING-MD5 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_004, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_004_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_005
 * @tc.name      : HksCryptoHalRsaSign_005
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PKCS1PADDING-MD5 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_005, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_005_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_006
 * @tc.name      : HksCryptoHalRsaSign_006
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PKCS1PADDING-MD5 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_006, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_006_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_007
 * @tc.name      : HksCryptoHalRsaSign_007
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-512-PKCS1PADDING-NONE key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_007, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_007_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_008
 * @tc.name      : HksCryptoHalRsaSign_008
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PKCS1PADDING-NONE key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_008, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_008_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_009
 * @tc.name      : HksCryptoHalRsaSign_009
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PKCS1PADDING-NONE key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_009, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_009_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_010
 * @tc.name      : HksCryptoHalRsaSign_010
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PKCS1PADDING-NONE key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_010, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_010_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_011
 * @tc.name      : HksCryptoHalRsaSign_011
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PKCS1PADDING-NONE key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_011, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_011_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_012
 * @tc.name      : HksCryptoHalRsaSign_012
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PKCS1PADDING-NONE key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_012, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_012_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_013
 * @tc.name      : HksCryptoHalRsaSign_013
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-512-PKCS1PADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_013, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_013_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_014
 * @tc.name      : HksCryptoHalRsaSign_014
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PKCS1PADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_014, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_014_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_015
 * @tc.name      : HksCryptoHalRsaSign_015
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PKCS1PADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_015, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_015_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_016
 * @tc.name      : HksCryptoHalRsaSign_016
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PKCS1PADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_016, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_016_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_017
 * @tc.name      : HksCryptoHalRsaSign_017
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PKCS1PADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_017, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_017_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_018
 * @tc.name      : HksCryptoHalRsaSign_018
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PKCS1PADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_018, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_018_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_019
 * @tc.name      : HksCryptoHalRsaSign_019
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-512-PKCS1PADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_019, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_019_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_020
 * @tc.name      : HksCryptoHalRsaSign_020
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PKCS1PADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_020, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_020_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_021
 * @tc.name      : HksCryptoHalRsaSign_021
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PKCS1PADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_021, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_021_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_022
 * @tc.name      : HksCryptoHalRsaSign_022
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PKCS1PADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_022, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_022_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_023
 * @tc.name      : HksCryptoHalRsaSign_023
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PKCS1PADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_023, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_023_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_024
 * @tc.name      : HksCryptoHalRsaSign_024
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PKCS1PADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_024, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_024_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_025
 * @tc.name      : HksCryptoHalRsaSign_025
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-512-PKCS1PADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_025, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_025_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_026
 * @tc.name      : HksCryptoHalRsaSign_026
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PKCS1PADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_026, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_026_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_027
 * @tc.name      : HksCryptoHalRsaSign_027
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PKCS1PADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_027, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_027_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_028
 * @tc.name      : HksCryptoHalRsaSign_028
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PKCS1PADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_028, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_028_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_029
 * @tc.name      : HksCryptoHalRsaSign_029
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PKCS1PADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_029, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_029_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_030
 * @tc.name      : HksCryptoHalRsaSign_030
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PKCS1PADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_030, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_030_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_031
 * @tc.name      : HksCryptoHalRsaSign_031
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PKCS1PADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_031, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_031_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_032
 * @tc.name      : HksCryptoHalRsaSign_032
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PKCS1PADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_032, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_032_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_033
 * @tc.name      : HksCryptoHalRsaSign_033
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PKCS1PADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_033, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_033_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_034
 * @tc.name      : HksCryptoHalRsaSign_034
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PKCS1PADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_034, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_034_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_035
 * @tc.name      : HksCryptoHalRsaSign_035
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PKCS1PADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_035, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_035_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_036
 * @tc.name      : HksCryptoHalRsaSign_036
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PKCS1PADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_036, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_036_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_037
 * @tc.name      : HksCryptoHalRsaSign_037
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PKCS1PADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_037, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_037_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_038
 * @tc.name      : HksCryptoHalRsaSign_038
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PKCS1PADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_038, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_038_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_039
 * @tc.name      : HksCryptoHalRsaSign_039
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PKCS1PADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_039, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_039_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_040
 * @tc.name      : HksCryptoHalRsaSign_040
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PKCS1PADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_040, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_040_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_041
 * @tc.name      : HksCryptoHalRsaSign_041
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-512-PSSPADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_041, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_041_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_042
 * @tc.name      : HksCryptoHalRsaSign_042
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PSSPADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_042, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_042_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_043
 * @tc.name      : HksCryptoHalRsaSign_043
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PSSPADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_043, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_043_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_044
 * @tc.name      : HksCryptoHalRsaSign_044
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PSSPADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_044, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_044_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_045
 * @tc.name      : HksCryptoHalRsaSign_045
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PSSPADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_045, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_045_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_046
 * @tc.name      : HksCryptoHalRsaSign_046
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PSSPADDING-SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_046, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_046_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_047
 * @tc.name      : HksCryptoHalRsaSign_047
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-512-PSSPADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_047, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_047_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_048
 * @tc.name      : HksCryptoHalRsaSign_048
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PSSPADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_048, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_048_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_049
 * @tc.name      : HksCryptoHalRsaSign_049
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PSSPADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_049, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_049_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_050
 * @tc.name      : HksCryptoHalRsaSign_050
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PSSPADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_050, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_050_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_051
 * @tc.name      : HksCryptoHalRsaSign_051
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PSSPADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_051, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_051_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_052
 * @tc.name      : HksCryptoHalRsaSign_052
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PSSPADDING-SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_052, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_052_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_053
 * @tc.name      : HksCryptoHalRsaSign_053
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-512-PSSPADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_053, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_053_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_054
 * @tc.name      : HksCryptoHalRsaSign_054
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PSSPADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_054, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_054_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_055
 * @tc.name      : HksCryptoHalRsaSign_055
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PSSPADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_055, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_055_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_056
 * @tc.name      : HksCryptoHalRsaSign_056
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PSSPADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_056, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_056_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_057
 * @tc.name      : HksCryptoHalRsaSign_057
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PSSPADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_057, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_057_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_058
 * @tc.name      : HksCryptoHalRsaSign_058
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PSSPADDING-SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_058, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_058_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_059
 * @tc.name      : HksCryptoHalRsaSign_059
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-512-PSSPADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_059, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_059_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_060
 * @tc.name      : HksCryptoHalRsaSign_060
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PSSPADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_060, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_060_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_061
 * @tc.name      : HksCryptoHalRsaSign_061
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PSSPADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_061, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_061_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_062
 * @tc.name      : HksCryptoHalRsaSign_062
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PSSPADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_062, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_062_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_063
 * @tc.name      : HksCryptoHalRsaSign_063
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PSSPADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_063, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_063_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_064
 * @tc.name      : HksCryptoHalRsaSign_064
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PSSPADDING-SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_064, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_064_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaSign_065
 * @tc.name      : HksCryptoHalRsaSign_065
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-768-PSSPADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_065, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_065_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_066
 * @tc.name      : HksCryptoHalRsaSign_066
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-1024-PSSPADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_066, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_066_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_067
 * @tc.name      : HksCryptoHalRsaSign_067
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-2048-PSSPADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_067, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_067_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaSign_068
 * @tc.name      : HksCryptoHalRsaSign_068
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-3072-PSSPADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_068, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_068_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaSign_069
 * @tc.name      : HksCryptoHalRsaSign_069
 * @tc.desc      : Using HksCryptoHalSign Sign RSA-4096-PSSPADDING-SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaSign, HksCryptoHalRsaSign_069, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_SIGN_069_PARAMS);
}
#endif
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS