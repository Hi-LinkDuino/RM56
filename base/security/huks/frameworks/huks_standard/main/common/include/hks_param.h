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

#ifndef HKS_PARAM_H
#define HKS_PARAM_H

#include "hks_type.h"

#define HKS_PARAM_SET_MAX_SIZE (4 * 1024 * 1024)
#define HKS_DEFAULT_PARAM_SET_SIZE 512
#define HKS_DEFAULT_PARAM_CNT ((uint32_t)(HKS_DEFAULT_PARAM_SET_SIZE / sizeof(struct HksParam)))
#define HKS_TAG_TYPE_MASK (0xF << 28)

#ifdef __cplusplus
extern "C" {
#endif

HKS_API_EXPORT int32_t HksInitParamSet(struct HksParamSet **paramSet);

HKS_API_EXPORT int32_t HksAddParams(struct HksParamSet *paramSet,
    const struct HksParam *params, uint32_t paramCnt);

HKS_API_EXPORT int32_t HksBuildParamSet(struct HksParamSet **paramSet);

HKS_API_EXPORT void HksFreeParamSet(struct HksParamSet **paramSet);

HKS_API_EXPORT int32_t HksGetParamSet(const struct HksParamSet *fromParamSet, uint32_t fromParamSetSize,
    struct HksParamSet **paramSet);

HKS_API_EXPORT int32_t HksGetParam(const struct HksParamSet *paramSet, uint32_t tag, struct HksParam **param);

HKS_API_EXPORT int32_t HksFreshParamSet(struct HksParamSet *paramSet, bool isCopy);

int32_t HksCheckParamSetTag(const struct HksParamSet *paramSet);

int32_t HksCheckParamSet(const struct HksParamSet *paramSet, uint32_t size);

int32_t HksCheckParamMatch(const struct HksParam *baseParam, const struct HksParam *param);

#ifdef __cplusplus
}
#endif

#endif /* HKS_PARAM_H */
