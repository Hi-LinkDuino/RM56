/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "application_info.h"

#include <cerrno>
#include <fcntl.h>
#include <string>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "json_serializer.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
using namespace Constants;

namespace {
    const std::string DEBUG = "debug";
} // namespace

bool CompatibleApplicationInfo::ReadFromParcel(Parcel& parcel)
{
    APP_LOGD("CompatibleApplicationInfo::ReadFromParcel called");
    name = Str16ToStr8(parcel.ReadString16());
    icon = Str16ToStr8(parcel.ReadString16());
    label = Str16ToStr8(parcel.ReadString16());
    description = Str16ToStr8(parcel.ReadString16());
    cpuAbi = Str16ToStr8(parcel.ReadString16());
    process = Str16ToStr8(parcel.ReadString16());
    systemApp = parcel.ReadBool();
    supportedModes = parcel.ReadInt32();
    iconId = parcel.ReadInt32();
    descriptionId = parcel.ReadInt32();
    labelId = parcel.ReadInt32();
    isCompressNativeLibs = parcel.ReadBool();

    int32_t permissionSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissionSize);
    if (permissionSize > Constants::MAX_LIMIT_SIZE) {
        APP_LOGE("permissions size is overflow");
        return false;
    }
    for (auto i = 0; i < permissionSize; i++) {
        std::string permission = Str16ToStr8(parcel.ReadString16());
        if (permission.empty()) {
            APP_LOGE("ReadParcelable<string> failed");
            return false;
        }
        permissions.emplace_back(permission);
    }

    int32_t moduleInfoSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleInfoSize);
    if (moduleInfoSize > Constants::MAX_LIMIT_SIZE) {
        APP_LOGE("module info size is overflow");
        return false;
    }
    for (auto i = 0; i < moduleInfoSize; i++) {
        ModuleInfo moduleInfo;
        std::string moduleName = Str16ToStr8(parcel.ReadString16());
        std::string moduleSourceDir = Str16ToStr8(parcel.ReadString16());
        if (moduleName.empty() || moduleSourceDir.empty()) {
            APP_LOGE("ReadParcelable<string> failed");
            return false;
        }
        moduleInfo.moduleName = moduleName;
        moduleInfo.moduleSourceDir = moduleSourceDir;
        moduleInfos.emplace_back(moduleInfo);
    }
    enabled = parcel.ReadBool();
    debug = parcel.ReadBool();
    return true;
}

CompatibleApplicationInfo* CompatibleApplicationInfo::Unmarshalling(Parcel& parcel)
{
    APP_LOGD("CompatibleApplicationInfo::Unmarshalling called");
    CompatibleApplicationInfo* info = new (std::nothrow) CompatibleApplicationInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool CompatibleApplicationInfo::Marshalling(Parcel& parcel) const
{
    APP_LOGD("CompatibleApplicationInfo::Marshalling called");
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(name));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(icon));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(label));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(description));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(cpuAbi));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(process));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, systemApp);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, supportedModes);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, iconId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, descriptionId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labelId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isCompressNativeLibs);
    const auto permissionSize = static_cast<int32_t>(permissions.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissionSize);
    for (auto i = 0; i < permissionSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(permissions[i]));
    }
    const auto moduleInfoSize = static_cast<int32_t>(moduleInfos.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleInfoSize);
    for (auto i = 0; i < moduleInfoSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleInfos[i].moduleName));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleInfos[i].moduleSourceDir));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, enabled);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, debug);

    return true;
}

void CompatibleApplicationInfo::ConvertToApplicationInfo(ApplicationInfo& applicationInfo) const
{
    APP_LOGD("CompatibleApplicationInfo::ConvertToApplicationInfo called");
    applicationInfo.name = name;
    applicationInfo.icon = icon;
    applicationInfo.label = label;
    applicationInfo.description = description;
    applicationInfo.cpuAbi = cpuAbi;
    applicationInfo.process = process;
    applicationInfo.isSystemApp = systemApp;
    applicationInfo.isCompressNativeLibs = isCompressNativeLibs;
    applicationInfo.iconId = iconId;
    applicationInfo.labelId = labelId;
    applicationInfo.descriptionId = descriptionId;
    applicationInfo.permissions = permissions;
    applicationInfo.moduleInfos = moduleInfos;
    applicationInfo.supportedModes = supportedModes;
    applicationInfo.enabled = debug;
    applicationInfo.entryModuleName = "";
}
} // namespace AppExecFwk
} // namespace OHOS