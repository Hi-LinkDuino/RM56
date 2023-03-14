/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef GROUP_MANAGER_COMMON_H
#define GROUP_MANAGER_COMMON_H

#include "base_group_manager_task.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TaskFunc)(HcTaskBase *task);

void DestroyGroupManagerTask(HcTaskBase *task);

int32_t BindCallbackToTask(GroupManagerTask *task, const CJson *jsonParams);
int32_t AddReqInfoToJson(int64_t requestId, const char *appId, CJson *jsonParams);
int32_t AddBindParamsToJson(int operationCode, int64_t requestId, const char *appId, CJson *jsonParams);
int32_t InitAndPushGMTask(int32_t osAccountId, int32_t opCode, int64_t reqId, CJson *params, TaskFunc func);

#ifdef __cplusplus
}
#endif
#endif
