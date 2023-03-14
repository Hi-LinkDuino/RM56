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

#ifndef HKS_OPERATION_H
#define HKS_OPERATION_H

#include <stdbool.h>
#include <stdint.h>

#include "hks_type_inner.h"

#include "hks_double_list.h"

struct HksOperation {
    struct DoubleList listHead;
    struct HksBlob processName;
    uint64_t handle;
    bool abortable;
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t CreateOperation(const struct HksBlob *processName, uint64_t operationHandle, bool abortable);

struct HksOperation *QueryOperation(uint64_t operationHandle);

void DeleteOperation(uint64_t operationHandle);

void DeleteSession(const struct HksBlob *processName);

bool DeleteFirstAbortableOperation(void);

uint32_t GetAbortableOperationCount(void);

#ifdef __cplusplus
}
#endif

#endif
