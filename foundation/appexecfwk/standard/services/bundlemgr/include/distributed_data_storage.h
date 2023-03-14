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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_DISTRIBUTED_DATA_STORAGE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_DISTRIBUTED_DATA_STORAGE_H

#include <map>

#include "distributed_bundle_info.h"

#include "bundle_constants.h"
#include "bundle_info.h"
#include "distributed_kv_data_manager.h"

namespace OHOS {
namespace AppExecFwk {
class DistributedDataStorage {
public:
    DistributedDataStorage();
    ~DistributedDataStorage();
    static std::shared_ptr<DistributedDataStorage> GetInstance();

    bool SaveStorageDistributeInfo(const std::string &bundleName, int32_t userId);
    bool DeleteStorageDistributeInfo(const std::string &bundleName, int32_t userId);
    bool QueryStroageDistributeInfo(const std::string &bundleName, const std::string &networkId,
        DistributedBundleInfo &info);
    bool QueryAllDeviceIds(std::vector<std::string> &deviceIds);
    void UpdateDistributedData(const std::vector<BundleInfo> &bundleInfos);
    void SyncDistributedData(const std::vector<std::string> &deviceList);
    void RemoveDeviceData(const std::string &networkId);

private:
    void DeviceAndNameToKey(const std::string &udid, const std::string &bundleName, std::string &key) const;
    void TryTwice(const std::function<DistributedKv::Status()> &func) const;
    bool CheckKvStore();
    DistributedKv::Status GetKvStore();
    bool GetLocalUdid(std::string &udid);
    DistributedBundleInfo ConvertToDistributedBundleInfo(const BundleInfo &bundleInfo);
    int32_t GetUdidByNetworkId(const std::string &networkId, std::string &udid);
    void CheckToSyncDistributedData();
    bool InnerSaveStorageDistributeInfo(const DistributedBundleInfo distributedBundleInfo);
private:
    static std::recursive_mutex mutex_;
    static std::shared_ptr<DistributedDataStorage> instance_;

    const DistributedKv::AppId appId_ {Constants::APP_ID};
    const DistributedKv::StoreId storeId_ {Constants::DISTRIBUTE_DATA_STORE_ID};
    DistributedKv::DistributedKvDataManager dataManager_;
    std::shared_ptr<DistributedKv::SingleKvStore> kvStorePtr_;
    mutable std::mutex kvStorePtrMutex_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_DISTRIBUTED_DATA_STORAGE_H
