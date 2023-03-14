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

#ifndef HAL_PMS_H
#define HAL_PMS_H

#include <stdbool.h>
#include "perm_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

PermissionDef* HalGetPermissionList(unsigned int *length);
const char *HalGetPermissionPath();
int HalGetMaxPermissionSize();
void* HalMalloc(unsigned int size);
void HalFree(void *ptr);
int HalAccess(const char *pathname);
void HalMutexLock();
void HalMutexUnlock();
int HalGetDevUdid(unsigned char *udid, int size);
bool HalIsValidPath(const char *path);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // HAL_PMS_H
