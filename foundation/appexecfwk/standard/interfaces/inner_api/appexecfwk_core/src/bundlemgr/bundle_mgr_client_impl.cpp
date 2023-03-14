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
#include "bundle_mgr_client_impl.h"

#include <cerrno>
#include <fstream>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "system_ability_definition.h"

#ifdef GLOBAL_RESMGR_ENABLE
using namespace OHOS::Global::Resource;
#endif

namespace OHOS {
namespace AppExecFwk {
BundleMgrClientImpl::BundleMgrClientImpl()
{
    APP_LOGI("create bundleMgrClientImpl");
}

BundleMgrClientImpl::~BundleMgrClientImpl()
{
    APP_LOGI("destory bundleMgrClientImpl");
}

bool BundleMgrClientImpl::GetBundleNameForUid(const int uid, std::string &bundleName)
{
    APP_LOGI("GetBundleNameForUid begin");

    ErrCode result = Connect();
    if (result != ERR_OK) {
        APP_LOGE("failed to connect");
        return false;
    }

    return bundleMgr_->GetBundleNameForUid(uid, bundleName);
}

bool BundleMgrClientImpl::GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo,
    int32_t userId)
{
    APP_LOGI("GetBundleInfo begin");

    ErrCode result = Connect();
    if (result != ERR_OK) {
        APP_LOGE("failed to connect");
        return false;
    }

    return bundleMgr_->GetBundleInfo(bundleName, flag, bundleInfo, userId);
}

bool BundleMgrClientImpl::GetBundlePackInfo(
    const std::string &bundleName, const BundlePackFlag flag, BundlePackInfo &bundlePackInfo)
{
    APP_LOGI("enter");
    ErrCode result = Connect();
    if (result != ERR_OK) {
        APP_LOGE("failed to connect");
        return false;
    }
    return bundleMgr_->GetBundlePackInfo(bundleName, flag, bundlePackInfo);
}

bool BundleMgrClientImpl::GetHapModuleInfo(const std::string &bundleName, const std::string &hapName,
    HapModuleInfo &hapModuleInfo)
{
    ErrCode result = Connect();
    if (result != ERR_OK) {
        APP_LOGE("failed to connect");
        return false;
    }

    AbilityInfo info;
    info.bundleName = bundleName;
    info.package = hapName;
    return bundleMgr_->GetHapModuleInfo(info, hapModuleInfo);
}

bool BundleMgrClientImpl::GetResConfigFile(const HapModuleInfo &hapModuleInfo, const std::string &metadataName,
    std::vector<std::string> &profileInfos) const
{
    std::vector<Metadata> data = hapModuleInfo.metadata;
    std::string resourcePath = hapModuleInfo.resourcePath;
    if (!GetResProfileByMetadata(data, metadataName, resourcePath, profileInfos)) {
        APP_LOGE("GetResProfileByMetadata failed");
        return false;
    }
    if (profileInfos.empty()) {
        APP_LOGE("no valid file can be obtained");
        return false;
    }
    return true;
}

bool BundleMgrClientImpl::GetResConfigFile(const ExtensionAbilityInfo &extensionInfo, const std::string &metadataName,
    std::vector<std::string> &profileInfos) const
{
    std::vector<Metadata> data = extensionInfo.metadata;
    std::string resourcePath = extensionInfo.resourcePath;
    if (!GetResProfileByMetadata(data, metadataName, resourcePath, profileInfos)) {
        APP_LOGE("GetResProfileByMetadata failed");
        return false;
    }
    if (profileInfos.empty()) {
        APP_LOGE("no valid file can be obtained");
        return false;
    }
    return true;
}

bool BundleMgrClientImpl::GetResConfigFile(const AbilityInfo &abilityInfo, const std::string &metadataName,
    std::vector<std::string> &profileInfos) const
{
    std::vector<Metadata> data = abilityInfo.metadata;
    std::string resourcePath = abilityInfo.resourcePath;
    if (!GetResProfileByMetadata(data, metadataName, resourcePath, profileInfos)) {
        APP_LOGE("GetResProfileByMetadata failed");
        return false;
    }
    if (profileInfos.empty()) {
        APP_LOGE("no valid file can be obtained");
        return false;
    }
    return true;
}

std::vector<std::string> BundleMgrClientImpl::GetAccessibleAppCodePaths(int32_t userId)
{
    APP_LOGI("GetAccessibleAppCodePaths begin");

    ErrCode result = Connect();
    if (result != ERR_OK) {
        APP_LOGE("failed to connect");
        std::vector<std::string> vec;
        return vec;
    }
    return bundleMgr_->GetAccessibleAppCodePaths(userId);
}

bool BundleMgrClientImpl::GetResProfileByMetadata(const std::vector<Metadata> &metadata,
    const std::string &metadataName, const std ::string &resourcePath, std::vector<std::string> &profileInfos) const
{
#ifdef GLOBAL_RESMGR_ENABLE
    if (metadata.empty()) {
        APP_LOGE("GetResProfileByMetadata failed due to empty metadata");
        return false;
    }
    if (resourcePath.empty()) {
        APP_LOGE("GetResProfileByMetadata failed due to empty resourcePath");
        return false;
    }
    std::shared_ptr<ResourceManager> resMgr = InitResMgr(resourcePath);
    if (resMgr == nullptr) {
        APP_LOGE("GetResProfileByMetadata init resMgr failed");
        return false;
    }

    if (metadataName.empty()) {
        for_each(metadata.begin(), metadata.end(), [this, &resMgr, &profileInfos](const Metadata& data)->void {
            if (!GetResFromResMgr(data.resource, resMgr, profileInfos)) {
                APP_LOGW("GetResFromResMgr failed");
            }
        });
    } else {
        for_each(metadata.begin(), metadata.end(),
            [this, &resMgr, &metadataName, &profileInfos](const Metadata& data)->void {
            if ((metadataName.compare(data.name) == 0) && (!GetResFromResMgr(data.resource, resMgr, profileInfos))) {
                APP_LOGW("GetResFromResMgr failed");
            }
        });
    }

    return true;
#else
    APP_LOGW("GLOBAL_RESMGR_ENABLE is false");
    return false;
#endif
}

#ifdef GLOBAL_RESMGR_ENABLE
std::shared_ptr<ResourceManager> BundleMgrClientImpl::InitResMgr(const std::string &resourcePath) const
{
    APP_LOGD("InitResMgr begin");
    if (resourcePath.empty()) {
        APP_LOGE("InitResMgr failed due to invalid param");
        return nullptr;
    }
    std::shared_ptr<ResourceManager> resMgr(CreateResourceManager());
    if (!resMgr) {
        APP_LOGE("InitResMgr resMgr is nullptr");
        return nullptr;
    }

    std::unique_ptr<ResConfig> resConfig(CreateResConfig());
    if (!resConfig) {
        APP_LOGE("InitResMgr resConfig is nullptr");
        return nullptr;
    }
    resMgr->UpdateResConfig(*resConfig);

    APP_LOGD("resourcePath is %{private}s", resourcePath.c_str());
    if (!resourcePath.empty() && !resMgr->AddResource(resourcePath.c_str())) {
        APP_LOGE("InitResMgr AddResource failed");
        return nullptr;
    }
    return resMgr;
}

bool BundleMgrClientImpl::GetResFromResMgr(const std::string &resName, const std::shared_ptr<ResourceManager> &resMgr,
    std::vector<std::string> &profileInfos) const
{
    APP_LOGD("GetResFromResMgr begin");
    if (resName.empty()) {
        APP_LOGE("GetResFromResMgr res name is empty");
        return false;
    }

    size_t pos = resName.rfind(Constants::PROFILE_FILE_COLON);
    if ((pos == std::string::npos) || (pos == resName.length() - 1)) {
        APP_LOGE("GetResFromResMgr res name is invalid");
        return false;
    }
    std::string profileName = resName.substr(pos + 1);
    std::string resPath;
    if (resMgr->GetProfileByName(profileName.c_str(), resPath) != SUCCESS) {
        APP_LOGE("GetResFromResMgr profileName cannot be found");
        return false;
    }
    APP_LOGD("GetResFromResMgr resPath is %{private}s", resPath.c_str());
    std::string profile;
    if (!TransformFileToJsonString(resPath, profile)) {
        return false;
    }
    profileInfos.emplace_back(profile);
    return true;
}
#endif

bool BundleMgrClientImpl::IsFileExisted(const std::string &filePath, const std::string &suffix) const
{
    if (filePath.empty()) {
        APP_LOGE("the file is not existed due to empty file path");
        return false;
    }

    auto position = filePath.rfind('.');
    if (position == std::string::npos) {
        APP_LOGE("filePath no suffix");
        return false;
    }

    std::string suffixStr = filePath.substr(position);
    if (LowerStr(suffixStr) != suffix) {
        APP_LOGE("file is not json");
        return false;
    }

    if (access(filePath.c_str(), F_OK) != 0) {
        APP_LOGE("can not access the file: %{private}s", filePath.c_str());
        return false;
    }
    return true;
}

bool BundleMgrClientImpl::TransformFileToJsonString(const std::string &resPath, std::string &profile) const
{
    if (!IsFileExisted(resPath, Constants::PROFILE_FILE_SUFFIX)) {
        APP_LOGE("the file is not existed");
        return false;
    }
    std::fstream in;
    char errBuf[256];
    errBuf[0] = '\0';
    in.open(resPath, std::ios_base::in | std::ios_base::binary);
    if (!in.is_open()) {
        strerror_r(errno, errBuf, sizeof(errBuf));
        APP_LOGE("the file cannot be open due to  %{public}s", errBuf);
        return false;
    }
    in.seekg(0, std::ios::end);
    int64_t size = in.tellg();
    if (size <= 0) {
        APP_LOGE("the file is an empty file");
        in.close();
        return false;
    }
    in.seekg(0, std::ios::beg);
    nlohmann::json profileJson = nlohmann::json::parse(in, nullptr, false);
    if (profileJson.is_discarded()) {
        APP_LOGE("bad profile file");
        in.close();
        return false;
    }
    profile = profileJson.dump(Constants::DUMP_INDENT);
    in.close();
    return true;
}

ErrCode BundleMgrClientImpl::InstallSandboxApp(const std::string &bundleName, int32_t dlpType, int32_t userId,
    int32_t &appIndex)
{
    APP_LOGD("InstallSandboxApp begin");
    if (bundleName.empty()) {
        APP_LOGE("InstallSandboxApp bundleName is empty");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }
    ErrCode result = Connect();
    if (result != ERR_OK) {
        APP_LOGE("failed to connect");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_INTERNAL_ERROR;
    }

    return bundleInstaller_->InstallSandboxApp(bundleName, dlpType, userId, appIndex);
}

ErrCode BundleMgrClientImpl::UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId)
{
    APP_LOGD("UninstallSandboxApp begin");
    if (bundleName.empty() || appIndex <= Constants::INITIAL_APP_INDEX) {
        APP_LOGE("UninstallSandboxApp params are invalid");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }
    ErrCode result = Connect();
    if (result != ERR_OK) {
        APP_LOGE("failed to connect");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_INTERNAL_ERROR;
    }

    return bundleInstaller_->UninstallSandboxApp(bundleName, appIndex, userId);
}

ErrCode BundleMgrClientImpl::GetSandboxBundleInfo(
    const std::string &bundleName, int32_t appIndex, int32_t userId, BundleInfo &info)
{
    APP_LOGD("GetSandboxBundleInfo begin");
    if (bundleName.empty() || appIndex <= Constants::INITIAL_APP_INDEX) {
        APP_LOGE("UninstallSandboxApp params are invalid");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }

    ErrCode result = Connect();
    if (result != ERR_OK) {
        APP_LOGE("failed to connect");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_INTERNAL_ERROR;
    }
    return bundleMgr_->GetSandboxBundleInfo(bundleName, appIndex, userId, info);
}

ErrCode BundleMgrClientImpl::Connect()
{
    APP_LOGI("connect begin");
    std::lock_guard<std::mutex> lock(mutex_);
    if (bundleMgr_ == nullptr) {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            APP_LOGE("failed to get system ability manager");
            return ERR_APPEXECFWK_SERVICE_NOT_CONNECTED;
        }

        sptr<IRemoteObject> remoteObject_ = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (remoteObject_ == nullptr || (bundleMgr_ = iface_cast<IBundleMgr>(remoteObject_)) == nullptr) {
            APP_LOGE("failed to get bundle mgr service remote object");
            return ERR_APPEXECFWK_SERVICE_NOT_CONNECTED;
        }
    }

    if (bundleInstaller_ == nullptr) {
        bundleInstaller_ = bundleMgr_->GetBundleInstaller();
        if ((bundleInstaller_ == nullptr) || (bundleInstaller_->AsObject() == nullptr)) {
            APP_LOGE("failed to get bundle installer proxy");
            return ERR_APPEXECFWK_SERVICE_NOT_CONNECTED;
        }
    }
    APP_LOGI("connect end");
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS