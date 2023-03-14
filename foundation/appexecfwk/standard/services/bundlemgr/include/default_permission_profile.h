/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DEFAULT_PERMISSION_PROFILE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DEFAULT_PERMISSION_PROFILE_H

#include "appexecfwk_errors.h"
#include "default_permission.h"
#include "json_util.h"

namespace OHOS {
namespace AppExecFwk {
class DefaultPermissionProfile {
public:
    /**
     * @brief Transform the jsonObject to defaultPermissions.
     * @param jsonObject Indicates the jsonObject.
     * @param defaultPermissions Indicates the defaultPermissions.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    ErrCode TransformTo(const nlohmann::json &jsonObject, std::vector<DefaultPermission> &defaultPermissions) const;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DEFAULT_PERMISSION_PROFILE_H
