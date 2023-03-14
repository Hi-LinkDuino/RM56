/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef PMS_PMS_H
#define PMS_PMS_H

#include "perm_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int SaveOrUpdatePermissions(const char *identifier, PermissionTrans permissions[], int permNum, enum IsUpdate isUpdate);

int DeletePermissions(const char *identifier);

int IsPermissionValid(const char *permissionName);

int IsPermissionRestricted(const char *permissionName);

int QueryAppCapabilities(const char *identifier, unsigned int **caps, unsigned int *capNum);

int LoadPermissions(const char *identifier, int uid);

int UnLoadPermissions(int uid);

int CheckPermissionStat(int uid, const char *permissionName);

char *QueryPermissionString(const char *identifier, int *errCode);

int QueryPermission(const char *identifier, PermissionSaved **permissions, int *permNum);

int GrantPermission(const char *identifier, const char *permName);

int RevokePermission(const char *identifier, const char *permName);

int UpdatePermissionFlags(const char *identifier, const char *permissionName, const int flags);

int GrantRuntimePermission(int uid, const char *permissionName);

int RevokeRuntimePermission(int uid, const char *permissionName);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // PMS_PMS_H
