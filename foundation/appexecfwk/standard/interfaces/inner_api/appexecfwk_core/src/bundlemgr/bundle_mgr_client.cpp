/*
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
#include "bundle_mgr_client.h"

#include "ability_info.h"
#include "app_log_wrapper.h"
#include "bundle_mgr_client_impl.h"
#include "extension_ability_info.h"
#include "hap_module_info.h"

namespace OHOS {
namespace AppExecFwk {
BundleMgrClient::BundleMgrClient()
{
    APP_LOGI("create BundleMgrClient");
    impl_ = std::make_shared<BundleMgrClientImpl>();
}

BundleMgrClient::~BundleMgrClient()
{
    APP_LOGI("destory BundleMgrClient");
}

bool BundleMgrClient::GetBundleNameForUid(const int uid, std::string &bundleName)
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return false;
    }
    return impl_->GetBundleNameForUid(uid, bundleName);
}

bool BundleMgrClient::GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo,
    int32_t userId)
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return false;
    }
    return impl_->GetBundleInfo(bundleName, flag, bundleInfo, userId);
}

bool BundleMgrClient::GetBundlePackInfo(
    const std::string &bundleName, const BundlePackFlag flag, BundlePackInfo &bundlePackInfo)
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return false;
    }
    return impl_->GetBundlePackInfo(bundleName, flag, bundlePackInfo);
}

bool BundleMgrClient::GetHapModuleInfo(const std::string &bundleName, const std::string &hapName,
    HapModuleInfo &hapModuleInfo)
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return false;
    }
    return impl_->GetHapModuleInfo(bundleName, hapName, hapModuleInfo);
}

bool BundleMgrClient::GetResConfigFile(const HapModuleInfo &hapModuleInfo, const std::string &metadataName,
    std::vector<std::string> &profileInfos) const
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return false;
    }
    return impl_->GetResConfigFile(hapModuleInfo, metadataName, profileInfos);
}

bool BundleMgrClient::GetResConfigFile(const ExtensionAbilityInfo &extensionInfo, const std::string &metadataName,
    std::vector<std::string> &profileInfos) const
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return false;
    }
    return impl_->GetResConfigFile(extensionInfo, metadataName, profileInfos);
}

bool BundleMgrClient::GetResConfigFile(const AbilityInfo &abilityInfo, const std::string &metadataName,
    std::vector<std::string> &profileInfos) const
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return false;
    }
    return impl_->GetResConfigFile(abilityInfo, metadataName, profileInfos);
}

std::vector<std::string> BundleMgrClient::GetAccessibleAppCodePaths(int32_t userId)
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        std::vector<std::string> vec;
        return vec;
    }
    return impl_->GetAccessibleAppCodePaths(userId);
}

ErrCode BundleMgrClient::InstallSandboxApp(const std::string &bundleName, int32_t dlpType, int32_t userId,
    int32_t &appIndex)
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_INTERNAL_ERROR;
    }
    return impl_->InstallSandboxApp(bundleName, dlpType, userId, appIndex);
}

ErrCode BundleMgrClient::UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId)
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_INTERNAL_ERROR;
    }
    return impl_->UninstallSandboxApp(bundleName, appIndex, userId);
}

ErrCode BundleMgrClient::GetSandboxBundleInfo(
    const std::string &bundleName, int32_t appIndex, int32_t userId, BundleInfo &info)
{
    if (impl_ == nullptr) {
        APP_LOGE("Bundle mgr client impl is nullptr");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_INTERNAL_ERROR;
    }
    return impl_->GetSandboxBundleInfo(bundleName, appIndex, userId, info);
}
}  // namespace AppExecFwk
}  // namespace OHOS
