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

#ifndef HKS_TEST_COMMON_H
#define HKS_TEST_COMMON_H

#include <securec.h>
#include "hks_type.h"
#include "hks_param.h"
#include "hks_mem.h"
#include "hks_log.h"
#include "hks_api.h"

#ifdef __cplusplus
extern "C" {
#endif

static const uint32_t TIMES = 4;
static const uint32_t MAX_UPDATE_SIZE = 64;
static const uint32_t MAX_OUTDATA_SIZE = MAX_UPDATE_SIZE * TIMES;

int32_t InitParamSet(struct HksParamSet **paramSet, const struct HksParam *params, uint32_t paramcount);
int32_t TestLessThanMaxSeg(const struct HksBlob *handle, const struct HksParamSet *paramSet,
                           const struct HksBlob *inData, struct HksBlob *outData);
int32_t HksTestUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData);
int32_t TestUpdateLoopFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet,
                             const struct HksBlob *inData, struct HksBlob *outData);
int32_t TestUpdateFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet,
                         const struct HksBlob *inData, struct HksBlob *outData);
int32_t MallocAndCheckBlobData(struct HksBlob *blob, const uint32_t blobSize);
int32_t TestCmpKeyAliasHash(const struct HksBlob *srcData1, const struct HksBlob *srcData2);

#ifdef __cplusplus
}
#endif

#endif // HKS_TEST_COMMON_H