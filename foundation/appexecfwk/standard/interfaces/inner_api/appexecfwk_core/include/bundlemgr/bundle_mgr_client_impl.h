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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_CLIENT_IMPL_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_CLIENT_IMPL_H

#include "appexecfwk_errors.h"
#include "application_info.h"
#include "bundle_info.h"
#include "bundle_pack_info.h"
#include "bundle_mgr_interface.h"
#include "extension_ability_info.h"
#include "hap_module_info.h"
#ifdef GLOBAL_RESMGR_ENABLE
#include "resource_manager.h"
#endif

namespace OHOS {
namespace AppExecFwk {
class BundleMgrClientImpl {
public:
    BundleMgrClientImpl();
    virtual ~BundleMgrClientImpl();

    bool GetBundleNameForUid(const int uid, std::string &bundleName);
    bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo, int32_t userId);
    bool GetBundlePackInfo(const std::string &bundleName, const BundlePackFlag flag, BundlePackInfo &bundlePackInfo);
    bool GetHapModuleInfo(const std::string &bundleName, const std::string &hapName, HapModuleInfo &hapModuleInfo);
    bool GetResConfigFile(const HapModuleInfo &hapModuleInfo, const std::string &metadataName,
    std::vector<std::string> &profileInfos) const;
    bool GetResConfigFile(const ExtensionAbilityInfo &extensionInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;
    bool GetResConfigFile(const AbilityInfo &abilityInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;
    std::vector<std::string> GetAccessibleAppCodePaths(int32_t userId);
    ErrCode InstallSandboxApp(const std::string &bundleName, int32_t dlpType, int32_t userId, int32_t &appIndex);
    ErrCode UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId);
    ErrCode GetSandboxBundleInfo(const std::string &bundleName, int32_t appIndex, int32_t userId, BundleInfo &info);

private:
    ErrCode Connect();
    bool GetResProfileByMetadata(const std::vector<Metadata> &metadata, const std::string &metadataName,
        const std ::string &resourcePath, std::vector<std::string> &profileInfos) const;
#ifdef GLOBAL_RESMGR_ENABLE
    std::shared_ptr<Global::Resource::ResourceManager> InitResMgr(const std::string &resourcePath) const;
    bool GetResFromResMgr(const std::string &resName, const std::shared_ptr<Global::Resource::ResourceManager> &resMgr,
        std::vector<std::string> &profileInfos) const;
#endif
    bool IsFileExisted(const std::string &filePath, const std::string &suffix) const;
    bool TransformFileToJsonString(const std::string &resPath, std::string &profile) const;

private:
    std::mutex mutex_;
    sptr<IBundleMgr> bundleMgr_;
    sptr<IBundleInstaller> bundleInstaller_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_CLIENT_H