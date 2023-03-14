/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode encryptResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode decryptResult = HksErrorCode::HKS_SUCCESS;
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_001_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff000000000000000000000000000"
                          "0000000000000000000000000000000000000",

    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_002_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData =
        "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff0000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000000000000000000000000",

    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_003_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff000000000000000000000000000"
                          "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                          "00000000000000000000000000000000000000000000000000000000000000000000000000",

    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_004_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData =
        "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff0000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000",

    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_005_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData =
        "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff0000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",

    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_006_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_NONE,
        .digest = HKS_DIGEST_NONE,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData =
        "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff0000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000",

    .generateKeyResult = HKS_SUCCESS,
#if defined(_USE_OPENSSL_)
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
#endif
#if defined(_USE_MBEDTLS_)
    .encryptResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_007_PARAMS = {
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
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_008_PARAMS = {
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
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_009_PARAMS = {
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
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_010_PARAMS = {
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
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_011_PARAMS = {
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
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_012_PARAMS = {
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
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_013_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccdd",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_014_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_015_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_016_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_017_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_018_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA1,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_019_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "001122334455",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_020_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_021_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_022_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_023_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_024_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA224,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_025_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_768,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "001122334455",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_026_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_027_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_028_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_029_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA256,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_030_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_1024,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "001122334455",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_031_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_032_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_033_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA384,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccdd",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_034_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccdd",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_035_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_3072,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccdd",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};

const TestCaseParams HKS_CRYPTO_HAL_RSA_CIPHER_036_PARAMS = {
    .spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_4096,
        .algParam = nullptr,
    },
    .usageSpec = {
        .algType = HKS_ALG_RSA,
        .mode = HKS_MODE_ECB,
        .padding = HKS_PADDING_OAEP,
        .digest = HKS_DIGEST_SHA512,
        .purpose = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
        .algParam = nullptr,
    },
    .hexData = "00112233445566778899aabbccdd",

    .generateKeyResult = HKS_SUCCESS,
    .encryptResult = HKS_SUCCESS,
    .decryptResult = HKS_SUCCESS,
};
}  // namespace

class HksCryptoHalRsaCipher : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    void RunTestCase(const TestCaseParams &testCaseParams) const
    {
        HksBlob key = { .size = 0, .data = nullptr };

        uint32_t inLen = testCaseParams.hexData.length() / HKS_COUNT_OF_HALF;
        uint32_t outLen = HKS_KEY_BYTES(testCaseParams.spec.keyLen);

        HksBlob message = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen + HKS_PADDING_SUPPLENMENT) };
        ASSERT_EQ(message.data == nullptr, false) << "message malloc failed.";
        for (uint32_t ii = 0; ii < inLen; ii++) {
            message.data[ii] = ReadHex((const uint8_t *)&testCaseParams.hexData[HKS_COUNT_OF_HALF * ii]);
        }

        HksBlob cipherText = { .size = outLen, .data = (uint8_t *)HksMalloc(outLen) };
        ASSERT_EQ(cipherText.data == nullptr, false) << "cipherText malloc failed.";

        HksBlob tagAead = { .size = 0, .data = nullptr };

        EXPECT_EQ(HksCryptoHalGenerateKey(&testCaseParams.spec, &key), testCaseParams.generateKeyResult);
        EXPECT_EQ(HksCryptoHalEncrypt(&key, &testCaseParams.usageSpec, &message, &cipherText, &tagAead),
            testCaseParams.encryptResult);
        if (testCaseParams.encryptResult == HKS_SUCCESS) {
            HksBlob inscription = { .size = outLen, .data = (uint8_t *)HksMalloc(outLen) };
            ASSERT_EQ(inscription.data == nullptr, false) << "inscription malloc failed.";
            EXPECT_EQ(HksCryptoHalDecrypt(&key, &testCaseParams.usageSpec, &cipherText, &inscription),
                testCaseParams.decryptResult);
            EXPECT_EQ(inscription.size, message.size);
            EXPECT_EQ(HksMemCmp(message.data, inscription.data, inscription.size), HKS_SUCCESS);
            HksFree(inscription.data);
        }
        HksFree(key.data);
        HksFree(message.data);
        HksFree(cipherText.data);
    }
};

void HksCryptoHalRsaCipher::SetUpTestCase(void)
{
}

void HksCryptoHalRsaCipher::TearDownTestCase(void)
{
}

void HksCryptoHalRsaCipher::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalRsaCipher::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_001
 * @tc.name      : HksCryptoHalRsaCipher_001
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-512-NOPADDING key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_001, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_002
 * @tc.name      : HksCryptoHalRsaCipher_002
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-768-NOPADDING key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_002, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_003
 * @tc.name      : HksCryptoHalRsaCipher_003
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-1024-NOPADDING key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_003, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_003_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_004
 * @tc.name      : HksCryptoHalRsaCipher_004
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-2048-NOPADDING key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_004, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_004_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaCipher_005
 * @tc.name      : HksCryptoHalRsaCipher_005
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-3072-NOPADDING key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_005, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_005_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_006
 * @tc.name      : HksCryptoHalRsaCipher_006
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-4096-NOPADDING key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_006, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_006_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaCipher_007
 * @tc.name      : HksCryptoHalRsaCipher_007
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-512-PKCS1Padding key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_007, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_007_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_008
 * @tc.name      : HksCryptoHalRsaCipher_008
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-768-PKCS1Padding key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_008, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_008_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_009
 * @tc.name      : HksCryptoHalRsaCipher_009
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-1024-PKCS1Padding key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_009, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_009_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_010
 * @tc.name      : HksCryptoHalRsaCipher_010
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-2048-PKCS1Padding key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_010, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_010_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaCipher_011
 * @tc.name      : HksCryptoHalRsaCipher_011
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-3072-PKCS1Padding key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_011, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_011_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_012
 * @tc.name      : HksCryptoHalRsaCipher_012
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-4096-PKCS1Padding key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_012, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_012_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaCipher_013
 * @tc.name      : HksCryptoHalRsaCipher_013
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-512-OAEP_SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_013, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_013_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_014
 * @tc.name      : HksCryptoHalRsaCipher_014
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-768-OAEP_SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_014, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_014_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_015
 * @tc.name      : HksCryptoHalRsaCipher_015
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-1024-OAEP_SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_015, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_015_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_016
 * @tc.name      : HksCryptoHalRsaCipher_016
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-2048-OAEP_SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_016, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_016_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaCipher_017
 * @tc.name      : HksCryptoHalRsaCipher_017
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-3072-OAEP_SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_017, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_017_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_018
 * @tc.name      : HksCryptoHalRsaCipher_018
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-4096-OAEP_SHA1 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_018, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_018_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaCipher_019
 * @tc.name      : HksCryptoHalRsaCipher_019
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-512-OAEP_SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_019, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_019_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_020
 * @tc.name      : HksCryptoHalRsaCipher_020
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-768-OAEP_SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_020, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_020_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_021
 * @tc.name      : HksCryptoHalRsaCipher_021
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-1024-OAEP_SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_021, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_021_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaCipher_022
 * @tc.name      : HksCryptoHalRsaCipher_022
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-2048-OAEP_SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_022, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_022_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_023
 * @tc.name      : HksCryptoHalRsaCipher_023
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-3072-OAEP_SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_023, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_023_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_024
 * @tc.name      : HksCryptoHalRsaCipher_024
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-4096-OAEP_SHA224 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_024, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_024_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaCipher_025
 * @tc.name      : HksCryptoHalRsaCipher_025
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-768-OAEP_SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_025, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_025_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_026
 * @tc.name      : HksCryptoHalRsaCipher_026
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-1024-OAEP_SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_026, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_026_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_027
 * @tc.name      : HksCryptoHalRsaCipher_027
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-2048-OAEP_SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_027, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_027_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaCipher_028
 * @tc.name      : HksCryptoHalRsaCipher_028
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-3072-OAEP_SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_028, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_028_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_029
 * @tc.name      : HksCryptoHalRsaCipher_029
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-4096-OAEP_SHA256 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_029, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_029_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaCipher_030
 * @tc.name      : HksCryptoHalRsaCipher_030
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-1024-OAEP_SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_030, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_030_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaCipher_031
 * @tc.name      : HksCryptoHalRsaCipher_031
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-2048-OAEP_SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_031, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_031_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_032
 * @tc.name      : HksCryptoHalRsaCipher_032
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-3072-OAEP_SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_032, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_032_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_033
 * @tc.name      : HksCryptoHalRsaCipher_033
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-4096-OAEP_SHA384 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_033, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_033_PARAMS);
}
#endif

/**
 * @tc.number    : HksCryptoHalRsaCipher_034
 * @tc.name      : HksCryptoHalRsaCipher_034
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-2048-OAEP_SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_034, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_034_PARAMS);
}

#ifndef CUT_RSA_4096_TEST
/**
 * @tc.number    : HksCryptoHalRsaCipher_035
 * @tc.name      : HksCryptoHalRsaCipher_035
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-3072-OAEP_SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_035, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_035_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalRsaCipher_036
 * @tc.name      : HksCryptoHalRsaCipher_036
 * @tc.desc      : Generate key and Encrypt / Decrypt RSA-4096-OAEP_SHA512 key.
 */
HWTEST_F(HksCryptoHalRsaCipher, HksCryptoHalRsaCipher_036, Function | SmallTest | Level1)
{
    RunTestCase(HKS_CRYPTO_HAL_RSA_CIPHER_036_PARAMS);
}
#endif
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS