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

#ifndef HKS_CRYPTO_ADAPTER_H
#define HKS_CRYPTO_ADAPTER_H

#include "hks_crypto_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void HksFillKeySpec(const struct HksParamSet *paramSet, struct HksKeySpec *spec);

void HksFillUsageSpec(const struct HksParamSet *paramSet, struct HksUsageSpec *usageSpec);

void HksFreeUsageSpec(struct HksUsageSpec **usageSpec);

void HksFillKeyDerivationParam(const struct HksParamSet *paramSet, struct HksKeyDerivationParam *param);

int32_t HksFillAeadParam(const struct HksParamSet *paramSet, struct HksBlob *inputText, struct HksUsageSpec *usageSpec,
    bool isEncrypt);

int32_t HksFillIvParam(const struct HksParamSet *paramSet, struct HksUsageSpec *usageSpec);

int32_t HksBuildCipherUsageSpec(const struct HksParamSet *paramSet, bool isEncrypt, struct HksBlob *inputText,
    struct HksUsageSpec **outUsageSpec);

int32_t HksGetEncryptAeTag(const struct HksParamSet *paramSet, const struct HksBlob *inData, struct HksBlob *outData,
    struct HksBlob *tagAead);

int32_t HksSetKeyToMaterial(uint32_t alg, bool isPubKey, const struct HksBlob *key, struct HksBlob *keyMaterial);

int32_t HksGetKeyFromMaterial(uint32_t alg, bool isPubKey, const struct HksBlob *keyMaterial, struct HksBlob *key);

int32_t HksFormatKeyFromMaterial(uint32_t alg, const struct HksBlob *keyMaterial, struct HksParamSet *paramSetOut);

#ifdef __cplusplus
}
#endif

#endif