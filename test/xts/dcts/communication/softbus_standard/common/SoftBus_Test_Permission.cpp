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

#include "SoftBus_Test_Permission.h"
#include "permission/permission.h"
#include "permission/permission_kit.h"

using namespace std;
using namespace OHOS::Security::Permission;

const static std::string SYSTEM_APP_PERMISSION =
    "com.permission.MANAGE_DISTRIBUTED_PERMISSION";
const static std::string DANGER_APP_PERMISSION =
    "ohos.permission.DISTRIBUTED_DATASYNC";
const static std::string BIND_DISCOVER_SERVICE =
    "com.hwddmp.permission.BIND_DISCOVER_SERVICE";
const static std::string TEST_LABEL = "test label";
const static std::string TEST_DESCRIPTION = "test description";
const static int TEST_LABEL_ID = 9527;
const static int TEST_DESCRIPTION_ID = 9528;

void SoftBus_Test_Permission::AddPermission(const string& pkgName)
{
    std::vector<PermissionDef> permDefList;
    PermissionDef permissionDefAlpha = {.permissionName = SYSTEM_APP_PERMISSION,
                                        .bundleName = pkgName,
                                        .grantMode = GrantMode::SYSTEM_GRANT,
                                        .availableScope = AVAILABLE_SCOPE_ALL,
                                        .label = TEST_LABEL,
                                        .labelId = TEST_LABEL_ID,
                                        .description = TEST_DESCRIPTION,
                                        .descriptionId = TEST_DESCRIPTION_ID};
    PermissionDef permissionDefBeta = {.permissionName = DANGER_APP_PERMISSION,
                                       .bundleName = pkgName,
                                       .grantMode = GrantMode::SYSTEM_GRANT,
                                       .availableScope = AVAILABLE_SCOPE_ALL,
                                       .label = TEST_LABEL,
                                       .labelId = TEST_LABEL_ID,
                                       .description = TEST_DESCRIPTION,
                                       .descriptionId = TEST_DESCRIPTION_ID};
    PermissionDef permissionDefGamma = {.permissionName = BIND_DISCOVER_SERVICE,
                                        .bundleName = pkgName,
                                        .grantMode = GrantMode::SYSTEM_GRANT,
                                        .availableScope = AVAILABLE_SCOPE_ALL,
                                        .label = TEST_LABEL,
                                        .labelId = TEST_LABEL_ID,
                                        .description = TEST_DESCRIPTION,
                                        .descriptionId = TEST_DESCRIPTION_ID};
    permDefList.emplace_back(permissionDefAlpha);
    permDefList.emplace_back(permissionDefBeta);
    permDefList.emplace_back(permissionDefGamma);
    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList;
    permList.push_back(SYSTEM_APP_PERMISSION);
    permList.push_back(DANGER_APP_PERMISSION);
    permList.push_back(BIND_DISCOVER_SERVICE);
    PermissionKit::AddSystemGrantedReqPermissions(pkgName, permList);
    PermissionKit::GrantSystemGrantedPermission(pkgName, SYSTEM_APP_PERMISSION);
    PermissionKit::GrantSystemGrantedPermission(pkgName, DANGER_APP_PERMISSION);
    PermissionKit::GrantSystemGrantedPermission(pkgName, BIND_DISCOVER_SERVICE);
}

void SoftBus_Test_Permission::RemovePermission(const string& pkgName)
{
    PermissionKit::RemoveDefPermissions(pkgName);
    PermissionKit::RemoveSystemGrantedReqPermissions(pkgName);
}
