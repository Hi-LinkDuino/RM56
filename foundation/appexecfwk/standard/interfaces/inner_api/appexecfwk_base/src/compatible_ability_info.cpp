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

#include "ability_info.h"

#include "nlohmann/json.hpp"

#include "string_ex.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {
using namespace Constants;
bool CompatibleAbilityInfo::ReadFromParcel(Parcel& parcel)
{
    APP_LOGD("CompatibleAbilityInfo::ReadFromParcel called");
    package = Str16ToStr8(parcel.ReadString16());
    name = Str16ToStr8(parcel.ReadString16());
    originalClassName = Str16ToStr8(parcel.ReadString16());
    label = Str16ToStr8(parcel.ReadString16());
    description = Str16ToStr8(parcel.ReadString16());
    iconPath = Str16ToStr8(parcel.ReadString16());
    uri = Str16ToStr8(parcel.ReadString16());
    moduleName = Str16ToStr8(parcel.ReadString16());
    process = Str16ToStr8(parcel.ReadString16());
    targetAbility = Str16ToStr8(parcel.ReadString16());
    appName = Str16ToStr8(parcel.ReadString16());
    privacyUrl = Str16ToStr8(parcel.ReadString16());
    privacyName = Str16ToStr8(parcel.ReadString16());
    downloadUrl = Str16ToStr8(parcel.ReadString16());
    versionName = Str16ToStr8(parcel.ReadString16());
    backgroundModes = parcel.ReadUint32();
    packageSize = parcel.ReadUint32();
    visible = parcel.ReadBool();
    formEnabled = parcel.ReadBool();
    multiUserShared = parcel.ReadBool();

    int32_t typeData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, typeData);
    type = static_cast<AbilityType>(typeData);

    int32_t orientationData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, orientationData);
    orientation = static_cast<DisplayOrientation>(orientationData);

    int32_t launchModeData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, launchModeData);
    launchMode = static_cast<LaunchMode>(launchModeData);

    int32_t permissionsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissionsSize);
    if (permissionsSize > Constants::MAX_LIMIT_SIZE) {
        APP_LOGE("permissions size is overflow");
        return false;
    }
    for (int32_t i = 0; i < permissionsSize; i++) {
        std::string permission = Str16ToStr8(parcel.ReadString16());
        if (permission.empty()) {
            APP_LOGE("ReadParcelable<string> failed");
            return false;
        }
        permissions.emplace_back(permission);
    }

    int32_t deviceTypeSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, deviceTypeSize);
    if (deviceTypeSize > Constants::MAX_DEVICETYPE_SIZE) {
        APP_LOGE("device type size is overflow");
        return false;
    }
    for (auto i = 0; i < deviceTypeSize; i++) {
        std::string deviceType = Str16ToStr8(parcel.ReadString16());
        if (deviceType.empty()) {
            APP_LOGE("ReadParcelable<string> failed");
            return false;
        }
        deviceTypes.emplace_back(deviceType);
    }

    int32_t deviceCapabilitySize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, deviceCapabilitySize);
    if (deviceCapabilitySize > Constants::MAX_LIMIT_SIZE) {
        APP_LOGE("device capability size is overflow");
        return false;
    }
    for (auto i = 0; i < deviceCapabilitySize; i++) {
        std::string deviceCapability = Str16ToStr8(parcel.ReadString16());
        if (deviceCapability.empty()) {
            APP_LOGE("ReadParcelable<string> failed");
            return false;
        }
        deviceCapabilities.emplace_back(deviceCapability);
    }

    supportPipMode = parcel.ReadBool();
    grantPermission = parcel.ReadBool();
    readPermission = Str16ToStr8(parcel.ReadString16());
    writePermission = Str16ToStr8(parcel.ReadString16());
    uriPermissionMode = Str16ToStr8(parcel.ReadString16());
    uriPermissionPath = Str16ToStr8(parcel.ReadString16());
    directLaunch = parcel.ReadBool();
    bundleName = Str16ToStr8(parcel.ReadString16());
    className = Str16ToStr8(parcel.ReadString16());
    deviceId = Str16ToStr8(parcel.ReadString16());
    std::unique_ptr<CompatibleApplicationInfo> appInfo(parcel.ReadParcelable<CompatibleApplicationInfo>());
    if (!appInfo) {
        APP_LOGE("ReadParcelable<CompatibleApplicationInfo> failed");
        return false;
    }
    applicationInfo = *appInfo;
    formEntity = static_cast<uint32_t>(parcel.ReadInt32());
    minFormHeight = parcel.ReadInt32();
    defaultFormHeight = parcel.ReadInt32();
    minFormWidth = parcel.ReadInt32();
    defaultFormWidth = parcel.ReadInt32();
    iconId = parcel.ReadInt32();
    descriptionId = parcel.ReadInt32();
    labelId = parcel.ReadInt32();
    enabled = parcel.ReadBool();
    return true;
}

CompatibleAbilityInfo* CompatibleAbilityInfo::Unmarshalling(Parcel& parcel)
{
    APP_LOGD("CompatibleAbilityInfo::Unmarshalling called");
    CompatibleAbilityInfo* info = new (std::nothrow) CompatibleAbilityInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool CompatibleAbilityInfo::Marshalling(Parcel& parcel) const
{
    APP_LOGD("CompatibleAbilityInfo::Marshalling called");
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(package));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(name));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(originalClassName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(label));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(description));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(iconPath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uri));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(process));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(targetAbility));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(appName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(privacyUrl));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(privacyName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(downloadUrl));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(versionName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, backgroundModes);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, packageSize);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, visible);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, formEnabled);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, multiUserShared);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(type));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(orientation));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(launchMode));
    const auto permissionsSize = static_cast<int32_t>(permissions.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissionsSize);
    for (auto i = 0; i < permissionsSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(permissions[i]));
    }
    const auto deviceTypeSize = static_cast<int32_t>(deviceTypes.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, deviceTypeSize);
    for (auto i = 0; i < deviceTypeSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deviceTypes[i]));
    }
    const auto deviceCapabilitySize = static_cast<int32_t>(deviceCapabilities.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, deviceCapabilitySize);
    for (auto i = 0; i < deviceCapabilitySize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deviceCapabilities[i]));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, supportPipMode);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, grantPermission);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(readPermission));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(writePermission));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uriPermissionMode));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(uriPermissionPath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, directLaunch);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(className));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deviceId));
    APP_LOGE("CompatibleAbilityInfo::Marshalling start to write application info.");
    if (!parcel.WriteParcelable(&applicationInfo)) {
        return false;
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, formEntity);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, minFormHeight);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, defaultFormHeight);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, minFormWidth);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, defaultFormWidth);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, iconId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, descriptionId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labelId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, enabled);
    return true;
}

void CompatibleAbilityInfo::ConvertToAbilityInfo(AbilityInfo& abilityInfo) const
{
    APP_LOGD("CompatibleAbilityInfo::ConvertToAbilityInfo called");
    abilityInfo.package = package;
    abilityInfo.name = name;
    abilityInfo.label = label;
    abilityInfo.description = description;
    abilityInfo.iconPath = iconPath;
    abilityInfo.uri = uri;
    abilityInfo.moduleName = moduleName;
    abilityInfo.process = process;
    abilityInfo.targetAbility = targetAbility;
    abilityInfo.appName = appName;
    abilityInfo.privacyUrl = privacyUrl;
    abilityInfo.privacyName = privacyName;
    abilityInfo.downloadUrl = downloadUrl;
    abilityInfo.versionName = versionName;
    abilityInfo.backgroundModes = backgroundModes;
    abilityInfo.packageSize = packageSize;
    abilityInfo.visible = visible;
    abilityInfo.formEnabled = formEnabled;
    abilityInfo.multiUserShared = multiUserShared;
    abilityInfo.type = type;
    abilityInfo.subType = subType;
    abilityInfo.orientation = orientation;
    abilityInfo.launchMode = launchMode;
    abilityInfo.permissions = permissions;
    abilityInfo.deviceTypes = deviceTypes;
    abilityInfo.deviceCapabilities = deviceCapabilities;
    abilityInfo.supportPipMode = supportPipMode;
    abilityInfo.grantPermission = grantPermission;
    abilityInfo.readPermission = readPermission;
    abilityInfo.writePermission = writePermission;
    abilityInfo.uriPermissionMode = uriPermissionMode;
    abilityInfo.uriPermissionPath = uriPermissionPath;
    abilityInfo.directLaunch = directLaunch;
    abilityInfo.bundleName = bundleName;
    abilityInfo.className = className;
    abilityInfo.originalClassName = originalClassName;
    abilityInfo.deviceId = deviceId;
    ApplicationInfo convertedApplicationInfo;
    applicationInfo.ConvertToApplicationInfo(convertedApplicationInfo);
    abilityInfo.applicationInfo = convertedApplicationInfo;
    abilityInfo.formEntity = formEntity;
    abilityInfo.minFormHeight = minFormHeight;
    abilityInfo.defaultFormHeight = defaultFormHeight;
    abilityInfo.minFormWidth = minFormWidth;
    abilityInfo.defaultFormWidth = defaultFormWidth;
    abilityInfo.iconId = iconId;
    abilityInfo.labelId = labelId;
    abilityInfo.descriptionId = descriptionId;
    abilityInfo.enabled = enabled;
    abilityInfo.originalBundleName = bundleName;
}
} // namespace AppExecFwk
} // namespace OHOS