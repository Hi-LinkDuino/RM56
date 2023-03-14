/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "provision/provision_verify.h"

#include <algorithm>

#include "nlohmann/json.hpp"

#ifndef STANDARD_SYSTEM
#include "ohos_account_kits.h"
#else
#include "parameter.h"
#include "sysparam_errno.h"
#endif // STANDARD_SYSTEM

#include "common/hap_verify_log.h"
#include "init/device_type_manager.h"

using namespace std;
using namespace nlohmann;

namespace {
const string KEY_VERSION_CODE = "version-code";
const string KEY_VERSION_NAME = "version-name";
const string KEY_UUID = "uuid";
const string KEY_TYPE = "type";
const string KEY_APP_DIST_TYPE = "app-distribution-type";
const string KEY_BUNDLE_INFO = "bundle-info";
const string KEY_DEVELOPER_ID = "developer-id";
const string KEY_DEVELOPMENT_CERTIFICATE = "development-certificate";
const string KEY_DISTRIBUTION_CERTIFICATE = "distribution-certificate";
const string KEY_BUNDLE_NAME = "bundle-name";
const string KEY_APL = "apl";
const string KEY_APP_FEATURE = "app-feature";
const string KEY_ACLS = "acls";
const string KEY_ALLOWED_ACLS = "allowed-acls";
const string KEY_PERMISSIONS = "permissions";
const string KEY_RESTRICTED_PERMISSIONS = "restricted-permissions";
const string KEY_RESTRICTED_CAPABILITIES = "restricted-capabilities";
const string KEY_DEBUG_INFO = "debug-info";
const string KEY_DEVICE_ID_TYPE = "device-id-type";
const string KEY_DEVICE_IDS = "device-ids";
const string KEY_ISSUER = "issuer";
const string VALUE_TYPE_RELEASE = "release";
const string VALUE_DIST_TYPE_APP_GALLERY = "app_gallery";
const string VALUE_DIST_TYPE_ENTERPRISE = "enterprise";
const string VALUE_DIST_TYPE_OS_INTEGRATION = "os_integration";
const string VALUE_DEVICE_ID_TYPE_UDID = "udid";

const string GENERIC_BUNDLE_NAME = ".*";

const int32_t MAXIMUM_NUM_DEVICES = 100;
const int32_t VERSION_CODE_TWO = 2;

inline void GetStringIfExist(const json& obj, const string& key, string& out)
{
    if (obj.find(key.c_str()) != obj.end() && obj[key.c_str()].is_string()) {
        obj[key.c_str()].get_to(out);
    }
}

inline void GetInt32IfExist(const json& obj, const string& key, int32_t& out)
{
    if (obj.find(key.c_str()) != obj.end() && obj[key.c_str()].is_number_integer()) {
        obj[key.c_str()].get_to(out);
    }
}

inline void GetStringArrayIfExist(const json& obj, const string& key, vector<string>& out)
{
    if (obj.find(key.c_str()) != obj.end() && obj[key.c_str()].is_array()) {
        for (auto& item : obj[key.c_str()]) {
            if (item.is_string()) {
                out.push_back(item.get<string>());
            }
        }
    }
}

inline bool IsObjectExist(const json& obj, const string& key)
{
    return obj.find(key.c_str()) != obj.end() && obj[key.c_str()].is_object();
}
} // namespace

namespace OHOS {
namespace Security {
namespace Verify {
void ParseType(const json& obj, ProvisionInfo& out)
{
    string type;
    GetStringIfExist(obj, KEY_TYPE, type);
    /* If not release, then it's debug */
    out.type = (type == VALUE_TYPE_RELEASE) ? ProvisionType::RELEASE : ProvisionType::DEBUG;
}

void ParseAppDistType(const json& obj, ProvisionInfo& out)
{
    string distType;
    GetStringIfExist(obj, KEY_APP_DIST_TYPE, distType);
    if (distType == VALUE_DIST_TYPE_APP_GALLERY) {
        out.distributionType = AppDistType::APP_GALLERY;
    } else if (distType == VALUE_DIST_TYPE_ENTERPRISE) {
        out.distributionType = AppDistType::ENTERPRISE;
    } else if (distType == VALUE_DIST_TYPE_OS_INTEGRATION) {
        out.distributionType = AppDistType::OS_INTEGRATION;
    } else {
        out.distributionType = AppDistType::NONE_TYPE;
    }
}

void ParseBundleInfo(const json& obj, ProvisionInfo& out)
{
    if (IsObjectExist(obj, KEY_BUNDLE_INFO)) {
        auto& bundleInfo = obj[KEY_BUNDLE_INFO];
        GetStringIfExist(bundleInfo, KEY_DEVELOPER_ID, out.bundleInfo.developerId);
        GetStringIfExist(bundleInfo, KEY_DEVELOPMENT_CERTIFICATE, out.bundleInfo.developmentCertificate);
        GetStringIfExist(bundleInfo, KEY_DISTRIBUTION_CERTIFICATE, out.bundleInfo.distributionCertificate);
        GetStringIfExist(bundleInfo, KEY_BUNDLE_NAME, out.bundleInfo.bundleName);
        GetStringIfExist(bundleInfo, KEY_APL, out.bundleInfo.apl);
        GetStringIfExist(bundleInfo, KEY_APP_FEATURE, out.bundleInfo.appFeature);
    }
}

void ParseAcls(const json& obj, ProvisionInfo& out)
{
    if (IsObjectExist(obj, KEY_ACLS)) {
        auto& acls = obj[KEY_ACLS];
        GetStringArrayIfExist(acls, KEY_ALLOWED_ACLS, out.acls.allowedAcls);
    }
}

void ParsePermissions(const json& obj, ProvisionInfo& out)
{
    if (IsObjectExist(obj, KEY_PERMISSIONS)) {
        auto& permissions = obj[KEY_PERMISSIONS];
        GetStringArrayIfExist(permissions, KEY_RESTRICTED_PERMISSIONS, out.permissions.restrictedPermissions);
        GetStringArrayIfExist(permissions, KEY_RESTRICTED_CAPABILITIES, out.permissions.restrictedCapabilities);
    }
}

void ParseDebugInfo(const json& obj, ProvisionInfo& out)
{
    if (IsObjectExist(obj, KEY_DEBUG_INFO)) {
        GetStringIfExist(obj[KEY_DEBUG_INFO], KEY_DEVICE_ID_TYPE, out.debugInfo.deviceIdType);
        GetStringArrayIfExist(obj[KEY_DEBUG_INFO], KEY_DEVICE_IDS, out.debugInfo.deviceIds);
    }
}

void from_json(const json& obj, ProvisionInfo& out)
{
    if (!obj.is_object()) {
        return;
    }
    GetInt32IfExist(obj, KEY_VERSION_CODE, out.versionCode);
    GetStringIfExist(obj, KEY_VERSION_NAME, out.versionName);
    GetStringIfExist(obj, KEY_UUID, out.uuid);
    ParseType(obj, out);
    ParseAppDistType(obj, out);
    ParseBundleInfo(obj, out);
    ParseAcls(obj, out);
    ParsePermissions(obj, out);
    ParseDebugInfo(obj, out);
    GetStringIfExist(obj, KEY_ISSUER, out.issuer);
}

#define RETURN_IF_STRING_IS_EMPTY(str, msg) \
    if (str.empty()) {                      \
        HAPVERIFY_LOG_ERROR(LABEL, msg);    \
        return PROVISION_INVALID;           \
    }

#define RETURN_IF_INT_IS_NON_POSITIVE(num, msg) \
    if (num <= 0) {                             \
        HAPVERIFY_LOG_ERROR(LABEL, msg);        \
        return PROVISION_INVALID;               \
    }

AppProvisionVerifyResult ParseProvision(const string& appProvision, ProvisionInfo& info)
{
    json obj = json::parse(appProvision, nullptr, false);
    if (!obj.is_structured()) {
        HAPVERIFY_LOG_ERROR(LABEL, "Parsing appProvision failed. json: %{public}s", appProvision.c_str());
        return PROVISION_INVALID;
    }
    obj.get_to(info);

    RETURN_IF_INT_IS_NON_POSITIVE(info.versionCode, "Tag version code is empty.")
    RETURN_IF_STRING_IS_EMPTY(info.versionName, "Tag version name is empty.")
    RETURN_IF_STRING_IS_EMPTY(info.uuid, "Tag uuid is empty.")
    RETURN_IF_STRING_IS_EMPTY(info.bundleInfo.developerId, "Tag developer-id is empty.")
    if (info.type == ProvisionType::DEBUG) {
        RETURN_IF_STRING_IS_EMPTY(info.bundleInfo.developmentCertificate, "Tag development-certificate is empty.")
    } else if (info.type == ProvisionType::RELEASE) {
        RETURN_IF_INT_IS_NON_POSITIVE(info.distributionType, "Tag app-distribution-type is empty.")
        RETURN_IF_STRING_IS_EMPTY(info.bundleInfo.distributionCertificate, "Tag distribution-certificate is empty.")
    }
    RETURN_IF_STRING_IS_EMPTY(info.bundleInfo.bundleName, "Tag bundle-name is empty.")
    if (info.bundleInfo.bundleName == GENERIC_BUNDLE_NAME) {
        HAPVERIFY_LOG_DEBUG(LABEL, "generic package name: %{public}s, is used.", GENERIC_BUNDLE_NAME.c_str());
    }
    if (info.versionCode >= VERSION_CODE_TWO) {
        RETURN_IF_STRING_IS_EMPTY(info.bundleInfo.apl, "Tag apl is empty.");
    }
    RETURN_IF_STRING_IS_EMPTY(info.bundleInfo.appFeature, "Tag app-feature is empty.")

    return PROVISION_OK;
}

inline bool CheckDeviceID(const std::vector<std::string>& deviceIds, const string& deviceId)
{
    auto iter = find(deviceIds.begin(), deviceIds.end(), deviceId);
    if (iter == deviceIds.end()) {
        DeviceTypeManager& deviceTypeManager = DeviceTypeManager::GetInstance();
        if (!deviceTypeManager.GetDeviceTypeInfo()) {
            HAPVERIFY_LOG_ERROR(LABEL, "current device is not authorized");
            return false;
        }
        HAPVERIFY_LOG_INFO(LABEL, "current device is a debug device");
    }
    return true;
}

AppProvisionVerifyResult CheckDeviceID(ProvisionInfo& info)
{
    // Checking device ids
    if (info.debugInfo.deviceIds.empty()) {
        HAPVERIFY_LOG_ERROR(LABEL, "device-id list is empty.");
        return PROVISION_DEVICE_UNAUTHORIZED;
    }

    if (info.debugInfo.deviceIds.size() > MAXIMUM_NUM_DEVICES) {
        HAPVERIFY_LOG_ERROR(LABEL, "No. of device IDs in list exceed maximum number %{public}d", MAXIMUM_NUM_DEVICES);
        return PROVISION_NUM_DEVICE_EXCEEDED;
    }

    if (info.debugInfo.deviceIdType != VALUE_DEVICE_ID_TYPE_UDID) {
        HAPVERIFY_LOG_ERROR(LABEL, "type of device ID is not supported.");
        return PROVISION_UNSUPPORTED_DEVICE_TYPE;
    }

    string deviceId;
#ifndef STANDARD_SYSTEM
    int32_t ret = OHOS::AccountSA::OhosAccountKits::GetInstance().GetUdid(deviceId);
    if (ret != 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "obtaining current device id failed (%{public}d).", ret);
        return PROVISION_DEVICE_UNAUTHORIZED;
    }
#else
    char udid[DEV_UUID_LEN] = {0};
    int ret = GetDevUdid(udid, sizeof(udid));
    if (ret != EC_SUCCESS) {
        HAPVERIFY_LOG_ERROR(LABEL, "obtaining current device id failed (%{public}d).", static_cast<int>(ret));
        return PROVISION_DEVICE_UNAUTHORIZED;
    }
    deviceId = std::string(udid, sizeof(udid) - 1);
#endif // STANDARD_SYSTEM
    if (deviceId.empty()) {
        HAPVERIFY_LOG_ERROR(LABEL, "device-id of current device is empty.");
        return PROVISION_DEVICE_UNAUTHORIZED;
    }

    if (!CheckDeviceID(info.debugInfo.deviceIds, deviceId)) {
        return PROVISION_DEVICE_UNAUTHORIZED;
    }
    return PROVISION_OK;
}

AppProvisionVerifyResult ParseAndVerify(const string& appProvision, ProvisionInfo& info)
{
    HAPVERIFY_LOG_DEBUG(LABEL, "Enter HarmonyAppProvision Verify");
    AppProvisionVerifyResult ret = ParseProvision(appProvision, info);
    if (ret != PROVISION_OK) {
        return ret;
    }
    if (info.type == ProvisionType::DEBUG) {
        ret = CheckDeviceID(info);
        if (ret != PROVISION_OK) {
            return ret;
        }
    }
    HAPVERIFY_LOG_DEBUG(LABEL, "Leave HarmonyAppProvision Verify");
    return PROVISION_OK;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
