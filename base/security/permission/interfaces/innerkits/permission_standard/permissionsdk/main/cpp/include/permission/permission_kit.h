/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNER_KITS_PERMISSION_PERMISSION_KIT_H
#define INTERFACES_INNER_KITS_PERMISSION_PERMISSION_KIT_H

#include <string>
#include <vector>

#include "permission/permission.h"
#include "permission/permission_def.h"

namespace OHOS {
namespace Security {
namespace Permission {
class PermissionKit {
public:
    static int VerifyPermission(const std::string& bundleName, const std::string& permissionName, int userId);
    static bool CanRequestPermission(const std::string& bundleName, const std::string& permissionName, int userId);

    static int GrantUserGrantedPermission(
        const std::string& bundleName, const std::string& permissionName, int userId);
    static int GrantSystemGrantedPermission(const std::string& bundleName, const std::string& permissionName);

    static int RevokeUserGrantedPermission(
        const std::string& bundleName, const std::string& permissionName, int userId);
    static int RevokeSystemGrantedPermission(const std::string& bundleName, const std::string& permissionName);

    static int AddUserGrantedReqPermissions(
        const std::string& bundleName, const std::vector<std::string>& permList, int userId);
    static int AddSystemGrantedReqPermissions(
        const std::string& bundleName, const std::vector<std::string>& permList);

    static int RemoveUserGrantedReqPermissions(const std::string& bundleName, int userId);
    static int RemoveSystemGrantedReqPermissions(const std::string& bundleName);

    static int AddDefPermissions(const std::vector<PermissionDef>& permList);
    static int RemoveDefPermissions(const std::string& bundleName);
    static int GetDefPermission(const std::string& permissionName, PermissionDef& permissionDefResult);
};
} // namespace Permission
} // namespace Security
} // namespace OHOS

#endif
