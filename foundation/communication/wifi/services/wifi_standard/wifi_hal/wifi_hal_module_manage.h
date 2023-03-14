/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WIFI_HAL_MODULE_MANAGE_H
#define OHOS_WIFI_HAL_MODULE_MANAGE_H

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MODULE_NAME_SIZE 32
#define MAX_LINE_SIZE 1024

typedef enum ModuleStatus {
    UNKNOWN = -1, /* when check module status failed */
    ACTIVE = 0,
    INACTIVE = 1,
} ModuleStatus;

typedef struct StModuleInfo {
    char szModuleName[MAX_MODULE_NAME_SIZE];
    int referenceCount;
    pid_t processId;
    struct StModuleInfo *next;
} ModuleInfo;

typedef enum ModuleManageRetCode {
    MM_FAILED = -1,
    MM_SUCCESS = 0,
    MM_REDUCE_REFERENCE = 1,
} ModuleManageRetCode;

/**
 * @Description Starts a specified service.
 *
 * @param moduleName
 * @param modulePropertyName
 * @param startCmd
 * @return ModuleManageRetCode
 */
ModuleManageRetCode StartModule(const char *moduleName, const char *startCmd);

/**
 * @Description Stop a specified service.
 *
 * @param moduleName
 * @param modulePropertyName
 * @return ModuleManageRetCode
 */
ModuleManageRetCode StopModule(const char *moduleName);

#ifdef __cplusplus
}
#endif
#endif