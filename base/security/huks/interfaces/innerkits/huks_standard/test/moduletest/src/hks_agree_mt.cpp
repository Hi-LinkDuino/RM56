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
#include "hks_config.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "openssl_dh_helper.h"
#include "openssl_ecc_helper.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace MT {
namespace {
struct TestCaseParams {
    std::vector<HksParam> generateKeyParams;
    std::vector<HksParam> agreeKeyParams;
    std::string hexData;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode agreeResult = HksErrorCode::HKS_SUCCESS;
};

const char ALISE_KEY[] = "This is a alise key";
const char BOB_KEY[] = "This is a bob key";
const uint32_t KEY_MEMORY = 4096;

int32_t LocalHksGenerate(const uint32_t keyLen, const struct HksBlob *authId, const struct HksParamSet *paramSetIn,
    struct HksBlob *priKey, struct HksBlob *pubKey)
{
    if (keyLen == 0) {
        return HKS_FAILURE;
    }
    struct HksParamSet *paramOutSet = nullptr;
    HksInitParamSet(&paramOutSet);

    uint8_t *localData = (uint8_t *)HksMalloc(keyLen);
    if (localData == nullptr) {
        HksFreeParamSet(&paramOutSet);
        return HKS_FAILURE;
    }
    struct HksParam localKey = { .tag = HKS_TAG_SYMMETRIC_KEY_DATA, .blob = { .size = keyLen, .data = localData } };
    HksAddParams(paramOutSet, &localKey, 1);
    HksBuildParamSet(&paramOutSet);

    if (HksGenerateKey(authId, paramSetIn, paramOutSet) != HKS_SUCCESS) {
        HksFree(localData);
        HksFreeParamSet(&paramOutSet);
        return HKS_FAILURE;
    }

    HksParam *priParam = nullptr;
    HksGetParam(paramOutSet, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priParam);
    priKey->size = priParam->blob.size;
    (void)memcpy_s(priKey->data, priParam->blob.size, priParam->blob.data, priParam->blob.size);

    HksParam *pubParam = nullptr;
    HksGetParam(paramOutSet, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubParam);
    pubKey->size = pubParam->blob.size;
    (void)memcpy_s(pubKey->data, pubParam->blob.size, pubParam->blob.data, pubParam->blob.size);

    HksFree(localData);
    HksFreeParamSet(&paramOutSet);
    return HKS_SUCCESS;
}

const TestCaseParams HUKS_AGREE_MT_00100_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_00200_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_00300_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_00400_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_00500_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_00600_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_00700_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_00800_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_00900_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_01000_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_256 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_01100_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_384 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_01200_PARAMS = {
    .generateKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_521 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

#ifdef HKS_SUPPORT_DH_C
const TestCaseParams HUKS_AGREE_MT_01300_PARAMS = {
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_2048 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_01400_PARAMS = {
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_3072 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_01500_PARAMS = {
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_4096 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_01600_PARAMS = {
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_2048 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_01700_PARAMS = {
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_3072 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_01800_PARAMS = {
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_4096 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_01900_PARAMS = {
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_2048 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_02000_PARAMS = {
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_3072 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_AGREE_MT_02100_PARAMS = {
    .agreeKeyParams = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_DH },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_DH_KEY_SIZE_4096 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .agreeResult = HKS_SUCCESS,
};
#endif
}  // namespace

class HksAgreeMt : public testing::Test {
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

    void ServiceAgreeScenario(const TestCaseParams &testCaseParams, const struct HksParamSet *generateKeyParams,
        struct HksBlob *agreeKeyAlise, struct HksBlob *agreeKeyBob)
    {
        struct HksParamSet *agreeKeyParams = nullptr;
        HksInitParamSet(&agreeKeyParams);
        HksAddParams(agreeKeyParams, testCaseParams.agreeKeyParams.data(), testCaseParams.agreeKeyParams.size());
        HksBuildParamSet(&agreeKeyParams);
#ifdef HKS_SUPPORT_DH_C
        uint32_t algorithm = ReadValueByTag(testCaseParams.agreeKeyParams, HKS_TAG_ALGORITHM);
#endif
        uint32_t keySize = ReadValueByTag(testCaseParams.agreeKeyParams, HKS_TAG_KEY_SIZE);
        struct HksBlob alise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(alise.data, nullptr);
        struct HksBlob bob = { (uint32_t)strlen(BOB_KEY), (uint8_t *)BOB_KEY };

        HksBlob pubKeyAlise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(pubKeyAlise.data, nullptr);
        HksBlob pubKeyBob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(pubKeyBob.data, nullptr);

        EXPECT_EQ(HksGenerateKey(&bob, generateKeyParams, NULL), testCaseParams.generateKeyResult);

        HksBlob x509KeyAlise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(x509KeyAlise.data, nullptr);
        HksBlob x509KeyBob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(x509KeyBob.data, nullptr);
        EXPECT_EQ(HksExportPublicKey(&bob, generateKeyParams, &x509KeyBob), HKS_SUCCESS);
#ifdef HKS_SUPPORT_DH_C
        if (algorithm == HKS_ALG_ECDH) {
#endif
            EXPECT_EQ(EccGenerateKey(keySize, &alise), testCaseParams.generateKeyResult);
            EXPECT_EQ(X509ToHksBlob(&x509KeyBob, &pubKeyBob), ECC_SUCCESS);
            EXPECT_EQ(EcdhAgreeKey(keySize, &alise, &pubKeyBob, agreeKeyAlise), testCaseParams.agreeResult);

            EXPECT_EQ(GetEccPubKey(&alise, &pubKeyAlise), ECC_SUCCESS);
            EXPECT_EQ(HksBlobToX509(&pubKeyAlise, &x509KeyAlise), ECC_SUCCESS);
#ifdef HKS_SUPPORT_DH_C
        } else if (algorithm == HKS_ALG_DH) {
            EXPECT_EQ(DhGenerateKey(keySize, &alise), testCaseParams.generateKeyResult);
            EXPECT_EQ(DhX509ToHksBlob(&x509KeyBob, &pubKeyBob), DH_SUCCESS);
            EXPECT_EQ(DhAgreeKey(keySize, &alise, &pubKeyBob, agreeKeyAlise), testCaseParams.agreeResult);

            EXPECT_EQ(DhGetDhPubKey(&alise, &pubKeyAlise), DH_SUCCESS);
            EXPECT_EQ(DhHksBlobToX509(&pubKeyAlise, &x509KeyAlise), DH_SUCCESS);
        }
#endif

        EXPECT_EQ(HksAgreeKey(agreeKeyParams, &bob, &x509KeyAlise, agreeKeyBob), testCaseParams.agreeResult);

        HksFree(alise.data);
        HksFree(x509KeyAlise.data);
        HksFree(x509KeyBob.data);
        HksFree(pubKeyAlise.data);
        HksFree(pubKeyBob.data);
        HksFreeParamSet(&agreeKeyParams);
    }

    void LocalAgreeScenario(const TestCaseParams &testCaseParams, const struct HksParamSet *generateKeyParams,
        struct HksBlob *agreeKeyAlise, struct HksBlob *agreeKeyBob, int32_t scenario)
    {
        struct HksParamSet *agreeKeyParams = nullptr;
        HksInitParamSet(&agreeKeyParams);
        HksAddParams(agreeKeyParams, testCaseParams.agreeKeyParams.data(), testCaseParams.agreeKeyParams.size());
        HksBuildParamSet(&agreeKeyParams);
        uint32_t keySize = ReadValueByTag(testCaseParams.agreeKeyParams, HKS_TAG_KEY_SIZE);
        HksBlob pubKeyAlise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(pubKeyAlise.data, nullptr);
        HksBlob pubKeyBob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(pubKeyBob.data, nullptr);
        if (scenario == 0) {
            struct HksBlob alise = { (uint32_t)strlen(ALISE_KEY), (uint8_t *)ALISE_KEY };
            struct HksBlob bob = { (uint32_t)strlen(BOB_KEY), (uint8_t *)BOB_KEY };
            HksBlob priKeyAlise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
            ASSERT_NE(priKeyAlise.data, nullptr);
            HksBlob priKeyBob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
            ASSERT_NE(priKeyBob.data, nullptr);
            EXPECT_EQ(LocalHksGenerate(keySize, &alise, generateKeyParams, &priKeyAlise, &pubKeyAlise),
                testCaseParams.generateKeyResult);
            EXPECT_EQ(LocalHksGenerate(keySize, &bob, generateKeyParams, &priKeyBob, &pubKeyBob),
                testCaseParams.generateKeyResult);
            EXPECT_EQ(EcdhAgreeKey(keySize, &priKeyAlise, &pubKeyBob, agreeKeyAlise), testCaseParams.agreeResult);
            EXPECT_EQ(EcdhAgreeKey(keySize, &priKeyBob, &pubKeyAlise, agreeKeyBob), testCaseParams.agreeResult);
            HksFree(priKeyAlise.data);
            HksFree(priKeyBob.data);
        } else if (scenario == 1) {
            struct HksBlob alise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
            ASSERT_NE(alise.data, nullptr);
            struct HksBlob bob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
            ASSERT_NE(bob.data, nullptr);
            EXPECT_EQ(EccGenerateKey(keySize, &alise), testCaseParams.generateKeyResult);
            EXPECT_EQ(EccGenerateKey(keySize, &bob), testCaseParams.generateKeyResult);
            EXPECT_EQ(GetEccPubKey(&alise, &pubKeyAlise), ECC_SUCCESS);
            EXPECT_EQ(GetEccPubKey(&bob, &pubKeyBob), ECC_SUCCESS);
            EXPECT_EQ(HksAgreeKey(agreeKeyParams, &alise, &pubKeyBob, agreeKeyAlise), testCaseParams.agreeResult);
            EXPECT_EQ(HksAgreeKey(agreeKeyParams, &bob, &pubKeyAlise, agreeKeyBob), testCaseParams.agreeResult);
            HksFree(alise.data);
            HksFree(bob.data);
        }
        HksFreeParamSet(&agreeKeyParams);
        HksFree(pubKeyAlise.data);
        HksFree(pubKeyBob.data);
    }

    void EcdhRunTestCase(const TestCaseParams &testCaseParams, int32_t scenario)
    {
        struct HksParamSet *generateKeyParams = nullptr;
        HksInitParamSet(&generateKeyParams);
        HksAddParams(
            generateKeyParams, testCaseParams.generateKeyParams.data(), testCaseParams.generateKeyParams.size());
        HksBuildParamSet(&generateKeyParams);

        uint32_t storage = ReadValueByTag(testCaseParams.agreeKeyParams, HKS_TAG_KEY_STORAGE_FLAG);

        HksBlob agreeKeyAlise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(agreeKeyAlise.data, nullptr);
        HksBlob agreeKeyBob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(agreeKeyBob.data, nullptr);

        if (storage == HKS_STORAGE_TEMP) {
            LocalAgreeScenario(testCaseParams, generateKeyParams, &agreeKeyAlise, &agreeKeyBob, scenario);
        } else if (storage == HKS_STORAGE_PERSISTENT) {
            ServiceAgreeScenario(testCaseParams, generateKeyParams, &agreeKeyAlise, &agreeKeyBob);
        }
        EXPECT_EQ(agreeKeyAlise.size, agreeKeyBob.size);
        EXPECT_EQ(HksMemCmp(agreeKeyAlise.data, agreeKeyBob.data, agreeKeyAlise.size), HKS_SUCCESS);

        HksFreeParamSet(&generateKeyParams);
        HksFree(agreeKeyAlise.data);
        HksFree(agreeKeyBob.data);
    }

#ifdef HKS_SUPPORT_DH_C
    void DhLocalAgreeScenario(const TestCaseParams &testCaseParams, const struct HksParamSet *paramInSet,
        struct HksBlob *agreeKeyAlise, struct HksBlob *agreeKeyBob, int32_t scenario)
    {
        uint32_t keySize = ReadValueByTag(testCaseParams.agreeKeyParams, HKS_TAG_KEY_SIZE);
        HksBlob pubKeyAlise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(pubKeyAlise.data, nullptr);
        HksBlob pubKeyBob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(pubKeyBob.data, nullptr);
        if (scenario == 0) {
            struct HksBlob alise = { (uint32_t)strlen(ALISE_KEY), (uint8_t *)ALISE_KEY };
            struct HksBlob bob = { (uint32_t)strlen(BOB_KEY), (uint8_t *)BOB_KEY };
            HksBlob priKeyAlise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
            ASSERT_NE(priKeyAlise.data, nullptr);
            HksBlob priKeyBob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
            ASSERT_NE(priKeyBob.data, nullptr);
            EXPECT_EQ(LocalHksGenerate(keySize, &alise, paramInSet, &priKeyAlise, &pubKeyAlise),
                testCaseParams.generateKeyResult);
            EXPECT_EQ(LocalHksGenerate(keySize, &bob, paramInSet, &priKeyBob, &pubKeyBob),
                testCaseParams.generateKeyResult);
            EXPECT_EQ(DhAgreeKey(keySize, &priKeyAlise, &pubKeyBob, agreeKeyAlise), testCaseParams.agreeResult);
            EXPECT_EQ(DhAgreeKey(keySize, &priKeyBob, &pubKeyAlise, agreeKeyBob), testCaseParams.agreeResult);
            HksFree(priKeyAlise.data);
            HksFree(priKeyBob.data);
        } else if (scenario == 1) {
            struct HksBlob alise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
            ASSERT_NE(alise.data, nullptr);
            struct HksBlob bob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
            ASSERT_NE(bob.data, nullptr);
            EXPECT_EQ(DhGenerateKey(keySize, &alise), testCaseParams.generateKeyResult);
            EXPECT_EQ(DhGenerateKey(keySize, &bob), testCaseParams.generateKeyResult);
            EXPECT_EQ(DhGetDhPubKey(&alise, &pubKeyAlise), DH_SUCCESS);
            EXPECT_EQ(DhGetDhPubKey(&bob, &pubKeyBob), DH_SUCCESS);
            EXPECT_EQ(HksAgreeKey(paramInSet, &alise, &pubKeyBob, agreeKeyAlise), testCaseParams.agreeResult);
            EXPECT_EQ(HksAgreeKey(paramInSet, &bob, &pubKeyAlise, agreeKeyBob), testCaseParams.agreeResult);
            HksFree(alise.data);
            HksFree(bob.data);
        }
        HksFree(pubKeyAlise.data);
        HksFree(pubKeyBob.data);
    }

    void DhRunTestCase(const TestCaseParams &testCaseParams, int32_t scenario)
    {
        struct HksParamSet *paramInSet = NULL;
        HksInitParamSet(&paramInSet);
        HksAddParams(paramInSet, testCaseParams.agreeKeyParams.data(), testCaseParams.agreeKeyParams.size());
        HksBuildParamSet(&paramInSet);

        uint32_t storage = ReadValueByTag(testCaseParams.agreeKeyParams, HKS_TAG_KEY_STORAGE_FLAG);

        HksBlob agreeKeyAlise = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(agreeKeyAlise.data, nullptr);
        HksBlob agreeKeyBob = { .size = KEY_MEMORY, .data = (uint8_t *)HksMalloc(KEY_MEMORY) };
        ASSERT_NE(agreeKeyBob.data, nullptr);

        if (storage == HKS_STORAGE_TEMP) {
            DhLocalAgreeScenario(testCaseParams, paramInSet, &agreeKeyAlise, &agreeKeyBob, scenario);
        } else if (storage == HKS_STORAGE_PERSISTENT) {
            ServiceAgreeScenario(testCaseParams, paramInSet, &agreeKeyAlise, &agreeKeyBob);
        }
        EXPECT_EQ(agreeKeyAlise.size, agreeKeyBob.size);
        EXPECT_EQ(HksMemCmp(agreeKeyAlise.data, agreeKeyBob.data, agreeKeyAlise.size), HKS_SUCCESS);

        HksFreeParamSet(&paramInSet);
        HksFree(agreeKeyAlise.data);
        HksFree(agreeKeyBob.data);
    }
#endif
};

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt00100
 * @tc.name      : HksAgreeMt00100
 * @tc.desc      : Both parties use huks to generate an ecc224 bit key, which can be successfully used in OpenSSL to
 * negotiate using the ECDH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt00100, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_00100_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt00200
 * @tc.name      : HksAgreeMt00200
 * @tc.desc      : Both parties use huks to generate an ecc256 bit key, which can be successfully used in OpenSSL to
 * negotiate using the ECDH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt00200, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_00200_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt00300
 * @tc.name      : HksAgreeMt00300
 * @tc.desc      : Both parties use huks to generate an ecc384 bit key, which can be successfully used in OpenSSL to
 * negotiate using the ECDH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt00300, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_00300_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt00400
 * @tc.name      : HksAgreeMt00400
 * @tc.desc      : Both parties use huks to generate an ecc521 bit key, which can be successfully used in OpenSSL to
 * negotiate using the ECDH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt00400, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_00400_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt00500
 * @tc.name      : HksAgreeMt00500
 * @tc.desc      : Both parties use OpenSSL to generate an ecc224 bit key, which can be successfully used in huks to
 * negotiate using the ECDH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt00500, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_00500_PARAMS, 1);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt00600
 * @tc.name      : HksAgreeMt00600
 * @tc.desc      : Both parties use OpenSSL to generate an ecc256 bit key, which can be successfully used in huks to
 * negotiate using the ECDH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt00600, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_00600_PARAMS, 1);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt00700
 * @tc.name      : HksAgreeMt00700
 * @tc.desc      : Both parties use OpenSSL to generate an ecc384 bit key, which can be successfully used in huks to
 * negotiate using the ECDH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt00700, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_00700_PARAMS, 1);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt00800
 * @tc.name      : HksAgreeMt00800
 * @tc.desc      : Both parties use OpenSSL to generate an ecc521 bit key, which can be successfully used in huks to
 * negotiate using the ECDH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt00800, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_00800_PARAMS, 1);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt00900
 * @tc.name      : HksAgreeMt00900
 * @tc.desc      : One party uses the key of ECC224 for openssl negotiation, and the other party uses the key of ECC224
 * for huks negotiation
 */
HWTEST_F(HksAgreeMt, HksAgreeMt00900, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_00900_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01000
 * @tc.name      : HksAgreeMt01000
 * @tc.desc      : One party uses the key of ECC256 for openssl negotiation, and the other party uses the key of ECC256
 * for huks negotiation
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01000, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_01000_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01100
 * @tc.name      : HksAgreeMt01100
 * @tc.desc      : One party uses the key of ECC384 for openssl negotiation, and the other party uses the key of ECC384
 * for huks negotiation
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01100, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_01100_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01200
 * @tc.name      : HksAgreeMt01200
 * @tc.desc      : One party uses the key of ECC521 for openssl negotiation, and the other party uses the key of ECC521
 * for huks negotiation
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01200, TestSize.Level0)
{
    EcdhRunTestCase(HUKS_AGREE_MT_01200_PARAMS, 0);
}

#ifdef HKS_SUPPORT_DH_C
/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01300
 * @tc.name      : HksAgreeMt01300
 * @tc.desc      : Both parties use huks to generate an dh2048 bit key, which can be successfully used in OpenSSL to
 * negotiate using the DH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01300, TestSize.Level0)
{
    DhRunTestCase(HUKS_AGREE_MT_01300_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01400
 * @tc.name      : HksAgreeMt01400
 * @tc.desc      : Both parties use huks to generate an dh3072 bit key, which can be successfully used in OpenSSL to
 * negotiate using the DH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01400, TestSize.Level0)
{
    DhRunTestCase(HUKS_AGREE_MT_01400_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01500
 * @tc.name      : HksAgreeMt01500
 * @tc.desc      : Both parties use huks to generate an dh4096 bit key, which can be successfully used in OpenSSL to
 * negotiate using the DH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01500, TestSize.Level0)
{
    DhRunTestCase(HUKS_AGREE_MT_01500_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01600
 * @tc.name      : HksAgreeMt01600
 * @tc.desc      : Both parties use OpenSSL to generate an dh2048 bit key, which can be successfully used in huks to
 * negotiate using the DH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01600, TestSize.Level0)
{
    DhRunTestCase(HUKS_AGREE_MT_01600_PARAMS, 1);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01700
 * @tc.name      : HksAgreeMt01700
 * @tc.desc      : Both parties use OpenSSL to generate an dh3072 bit key, which can be successfully used in huks to
 * negotiate using the DH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01700, TestSize.Level0)
{
    DhRunTestCase(HUKS_AGREE_MT_01700_PARAMS, 1);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01800
 * @tc.name      : HksAgreeMt01800
 * @tc.desc      : Both parties use OpenSSL to generate an dh4096 bit key, which can be successfully used in huks to
 * negotiate using the DH algorithm
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01800, TestSize.Level0)
{
    DhRunTestCase(HUKS_AGREE_MT_01800_PARAMS, 1);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt01900
 * @tc.name      : HksAgreeMt01900
 * @tc.desc      : One party uses the key of dh2048 for openssl negotiation, and the other party uses the key of dh2048
 * for huks negotiation
 */
HWTEST_F(HksAgreeMt, HksAgreeMt01900, TestSize.Level0)
{
    DhRunTestCase(HUKS_AGREE_MT_01900_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt02000
 * @tc.name      : HksAgreeMt02000
 * @tc.desc      : One party uses the key of dh3072 for openssl negotiation, and the other party uses the key of dh2048
 * for huks negotiation
 */
HWTEST_F(HksAgreeMt, HksAgreeMt02000, TestSize.Level0)
{
    DhRunTestCase(HUKS_AGREE_MT_02000_PARAMS, 0);
}

/**
 * @tc.number    : HksAgreeMt.HksAgreeMt02100
 * @tc.name      : HksAgreeMt02100
 * @tc.desc      : One party uses the key of dh4096 for openssl negotiation, and the other party uses the key of dh2048
 * for huks negotiation
 */
HWTEST_F(HksAgreeMt, HksAgreeMt02100, TestSize.Level0)
{
    DhRunTestCase(HUKS_AGREE_MT_02100_PARAMS, 0);
}
#endif
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS