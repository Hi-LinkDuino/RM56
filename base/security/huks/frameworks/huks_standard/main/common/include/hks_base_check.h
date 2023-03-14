/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HKS_BASE_CHECK_H
#define HKS_BASE_CHECK_H

#include "hks_param.h"
#include "hks_type_inner.h"

enum CheckKeyType {
    HKS_CHECK_TYPE_GEN_KEY,
    HKS_CHECK_TYPE_USE_KEY,
    HKS_CHECK_TYPE_GEN_MAC_KEY,
    HKS_CHECK_TYPE_GEN_DERIVE_KEY,
};

struct Params {
    bool needCheck;
    uint32_t value;
};

struct ParamsValues {
    struct Params keyLen;
    struct Params padding;
    struct Params purpose;
    struct Params digest;
    struct Params mode;
};

struct ParamsValuesChecker {
    enum CheckKeyType checkType;
    struct ParamsValues paramValues;
};

struct ExpectParams {
    bool needCheck;
    const uint32_t *values;
    uint32_t valueCnt;
};

struct ExpectParamsValues {
    const struct ExpectParams keyLen;
    const struct ExpectParams padding;
    const struct ExpectParams purpose;
    const struct ExpectParams digest;
    const struct ExpectParams mode;
};

struct ExpectParamsValuesChecker {
    enum CheckKeyType checkType;
    const struct ExpectParamsValues paramValues;
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksCheckValue(uint32_t inputValue, const uint32_t *expectValues, uint32_t valuesCount);

int32_t HksGetKeySize(uint32_t alg, const struct HksBlob *key, uint32_t *keySize);

int32_t HksCheckGenKeyPurpose(uint32_t alg, uint32_t inputPurpose);

int32_t HksGetInputParmasByAlg(uint32_t alg, enum CheckKeyType checkType, const struct HksParamSet *paramSet,
    struct ParamsValues *inputParams);

int32_t HksCheckFixedParams(uint32_t alg, enum CheckKeyType checkType, const struct ParamsValues *inputParams);

int32_t HksCheckGenKeyMutableParams(uint32_t alg, const struct ParamsValues *inputParams);

int32_t CheckImportMutableParams(uint32_t alg, const struct ParamsValues *params);

int32_t HksCheckSignature(uint32_t cmdId, uint32_t alg, uint32_t keySize, const struct HksBlob *signature);

int32_t HksCheckSignVerifyMutableParams(uint32_t cmdId, uint32_t alg, const struct ParamsValues *inputParams);

int32_t HksCheckCipherMutableParams(uint32_t cmdId, uint32_t alg, const struct ParamsValues *inputParams);

int32_t HksCheckCihperData(uint32_t cmdId, uint32_t alg, const struct ParamsValues *inputParams,
    const struct HksBlob *inData, const struct HksBlob *outData);

int32_t HksCheckCipherMaterialParams(uint32_t alg, const struct ParamsValues *inputParams,
    const struct HksParamSet *paramSet);

#ifdef __cplusplus
}
#endif

#endif /* HKS_BASE_CHECK_H */

