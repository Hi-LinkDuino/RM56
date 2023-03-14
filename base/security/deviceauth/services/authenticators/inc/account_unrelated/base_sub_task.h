/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef BASE_SUB_TASK_H
#define BASE_SUB_TASK_H

#include "json_utils.h"
#include "das_module_defines.h"
#include "das_version_util.h"

typedef struct SubTaskBaseT {
    int (*getTaskType)(const struct SubTaskBaseT *);
    void (*destroyTask)(struct SubTaskBaseT *);
    int (*process)(struct SubTaskBaseT *, const CJson *in, CJson *out, int32_t *status);
    VersionStruct curVersion;
} SubTaskBase;

typedef SubTaskBase *(*CreateSubTaskFunc)(CJson *, CJson *);

#endif
