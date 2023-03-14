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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_INSTALL_DATA_STORAGE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_INSTALL_DATA_STORAGE_H

#include <mutex>

#include "bundle_constants.h"
#include "distributed_kv_data_manager.h"
#include "pre_install_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {
class PreInstallDataStorage : public std::enable_shared_from_this<PreInstallDataStorage> {
public:
    PreInstallDataStorage();
    ~PreInstallDataStorage();
    /**
     * @brief Save the preInstall bundle data corresponding to the device Id of the bundle name to KvStore.
     * @param preInstallBundleInfo Indicates the PreInstallBundleInfo object to be save.
     * @return Returns true if the data is successfully saved; returns false otherwise.
     */
    bool SavePreInstallStorageBundleInfo(const PreInstallBundleInfo &preInstallBundleInfo);
    /**
     * @brief Obtains the PreInstallBundleInfo objects provided by bundleName.
     * @param preInstallBundleInfos Indicates information about the PreInstallBundleInfo.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool LoadAllPreInstallBundleInfos(std::vector<PreInstallBundleInfo> &preInstallBundleInfos);
    /**
     * @brief Delete the bundle data corresponding to the device Id of the bundle name to KvStore.
     * @param innerBundleInfo Indicates the InnerBundleInfo object to be Delete.
     * @return Returns true if the data is successfully deleted; returns false otherwise.
     */
    bool DeletePreInstallStorageBundleInfo(const PreInstallBundleInfo &preInstallBundleInfo);

private:
    void TryTwice(const std::function<DistributedKv::Status()>& func) const;
    bool CheckKvStore();
    DistributedKv::Status GetKvStore();
    bool ResetKvStore();
    DistributedKv::Status GetEntries(std::vector<DistributedKv::Entry> &allEntries) const;
    void SaveEntries(const std::vector<DistributedKv::Entry> &allEntries,
        std::vector<PreInstallBundleInfo> &preInstallBundleInfos);
    void UpdateDataBase(std::map<std::string, PreInstallBundleInfo>& infos);
    void DeleteOldBundleInfo(const std::string& oldKey);

private:
    const DistributedKv::AppId appId_ { Constants::APP_ID };
    const DistributedKv::StoreId storeId_ { Constants::PRE_INSTALL_DATA_STORE_ID };
    DistributedKv::DistributedKvDataManager dataManager_;
    std::shared_ptr<DistributedKv::SingleKvStore> kvStorePtr_;
    mutable std::mutex kvStorePtrMutex_;
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_INSTALL_DATA_STORAGE_H
