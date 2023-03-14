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

#include "default_permission_profile.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
static const std::string PERMISSIONS_PROFILE_KEY_BUNDLENAME = "bundleName";
static const std::string PERMISSIONS_PROFILE_KEY_PERMISSIONS = "permissions";
static const std::string PERMISSIONS_PROFILE_KEY_NAME = "name";
static const std::string PERMISSIONS_PROFILE_KEY_USER_CANCELLABLE = "userCancellable";
}
thread_local int32_t parseResult;

void from_json(const nlohmann::json &jsonObject, PermissionInfo &permissionInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PERMISSIONS_PROFILE_KEY_NAME,
        permissionInfo.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        PERMISSIONS_PROFILE_KEY_USER_CANCELLABLE,
        permissionInfo.userCancellable,
        JsonType::BOOLEAN,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
}

ErrCode DefaultPermissionProfile::TransformTo(const nlohmann::json &jsonObject,
    std::vector<DefaultPermission> &defaultPermissions) const
{
    if (jsonObject.is_array() && !jsonObject.is_discarded()) {
        for (const auto &object : jsonObject) {
            if (!object.is_object()) {
                APP_LOGE("object is not json object");
                return ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR;
            }
            DefaultPermission defaultPermission;
            const auto &objectEnd = object.end();
            GetValueIfFindKey<std::string>(object,
                objectEnd,
                PERMISSIONS_PROFILE_KEY_BUNDLENAME,
                defaultPermission.bundleName,
                JsonType::STRING,
                true,
                parseResult,
                ArrayType::NOT_ARRAY);
            GetValueIfFindKey<std::vector<PermissionInfo>>(object,
                objectEnd,
                PERMISSIONS_PROFILE_KEY_PERMISSIONS,
                defaultPermission.grantPermission,
                JsonType::ARRAY,
                false,
                parseResult,
                ArrayType::OBJECT);
            if (parseResult != ERR_OK) {
                APP_LOGE("parseResult is %{public}d", parseResult);
                int32_t ret = parseResult;
                // need recover parse result to ERR_OK
                parseResult = ERR_OK;
                return ret;
            }
            defaultPermissions.push_back(defaultPermission);
        }
    }
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS

