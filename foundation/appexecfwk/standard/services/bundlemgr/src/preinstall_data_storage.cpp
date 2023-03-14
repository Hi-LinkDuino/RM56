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

#include "preinstall_data_storage.h"

#include <unistd.h>
#include "app_log_wrapper.h"
#include "bundle_util.h"

using namespace OHOS::DistributedKv;

namespace OHOS {
namespace AppExecFwk {
namespace {
const int32_t MAX_TIMES = 600;             // 1 min
const int32_t SLEEP_INTERVAL = 100 * 1000;  // 100ms
}  // namespace

PreInstallDataStorage::PreInstallDataStorage()
{
    APP_LOGD("PreInstall instance is created");
    TryTwice([this] { return GetKvStore(); });
}

PreInstallDataStorage::~PreInstallDataStorage()
{
    APP_LOGD("PreInstall instance is destroyed");
    dataManager_.CloseKvStore(appId_, kvStorePtr_);
}

Status PreInstallDataStorage::GetKvStore()
{
    Options options = {
        .createIfMissing = true,
        .encrypt = false,
        .autoSync = false,
        .kvStoreType = KvStoreType::SINGLE_VERSION
    };

    Status status = dataManager_.GetSingleKvStore(options, appId_, storeId_, kvStorePtr_);
    if (status != Status::SUCCESS) {
        APP_LOGE("usage get kvStore error: %{public}d", status);
    } else {
        APP_LOGD("usage get kvStore success");
    }
    return status;
}

void PreInstallDataStorage::TryTwice(const std::function<Status()> &func) const
{
    Status status = func();
    if (status == Status::IPC_ERROR) {
        status = func();
        APP_LOGW("distribute database ipc error and try to call again, result = %{public}d", status);
    }
}

bool PreInstallDataStorage::CheckKvStore()
{
    if (kvStorePtr_ != nullptr) {
        return true;
    }
    int32_t tryTimes = MAX_TIMES;
    while (tryTimes > 0) {
        Status status = GetKvStore();
        if (status == Status::SUCCESS && kvStorePtr_ != nullptr) {
            return true;
        }
        APP_LOGD("usage CheckKvStore, Times: %{public}d", tryTimes);
        usleep(SLEEP_INTERVAL);
        tryTimes--;
    }
    return kvStorePtr_ != nullptr;
}

bool PreInstallDataStorage::ResetKvStore()
{
    std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
    kvStorePtr_ = nullptr;
    Status status = GetKvStore();
    if (status == Status::SUCCESS && kvStorePtr_ != nullptr) {
        return true;
    }
    APP_LOGW("usage reset failed");
    return false;
}

void PreInstallDataStorage::SaveEntries(
    const std::vector<Entry> &allEntries, std::vector<PreInstallBundleInfo> &preInstallBundleInfos)
{
    APP_LOGD("PreInstall SaveEntries start.");
    std::map<std::string, PreInstallBundleInfo> updateInfos;
    for (const auto &item : allEntries) {
        PreInstallBundleInfo preInstallBundleInfo;
        nlohmann::json jsonObject = nlohmann::json::parse(item.value.ToString(), nullptr, false);
        if (jsonObject.is_discarded()) {
            APP_LOGE("jsonObject is discarded error key: %{public}s", item.key.ToString().c_str());
            // it's an bad json, delete it
            {
                std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
                kvStorePtr_->Delete(item.key);
            }
            continue;
        }

        if (preInstallBundleInfo.FromJson(jsonObject) != ERR_OK) {
            APP_LOGE("error key: %{private}s", item.key.ToString().c_str());
            // it's an error value, delete it
            {
                std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
                kvStorePtr_->Delete(item.key);
            }
            continue;
        }
        preInstallBundleInfos.emplace_back(preInstallBundleInfo);
        // database update
        std::string key = item.key.ToString();
        if (key != preInstallBundleInfo.GetBundleName()) {
            updateInfos.emplace(key, preInstallBundleInfo);
        }
    }
    if (updateInfos.size() > 0) {
        UpdateDataBase(updateInfos);
    }
    APP_LOGD("PreInstall SaveEntries end");
}

bool PreInstallDataStorage::LoadAllPreInstallBundleInfos(std::vector<PreInstallBundleInfo> &preInstallBundleInfos)
{
    APP_LOGD("load all preInstallBundleInfo data to vector start.");
    {
        std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
        if (!CheckKvStore()) {
            APP_LOGE("kvStore is nullptr");
            return false;
        }
    }

    Status status;
    std::vector<Entry> allEntries;
    TryTwice([this, &status, &allEntries] {
        status = GetEntries(allEntries);
        return status;
    });

    if (status != Status::SUCCESS) {
        APP_LOGE("get entries error: %{public}d", status);
        // KEY_NOT_FOUND means no data in database, no need to report.
        if (status != Status::KEY_NOT_FOUND) {
            const std::string interfaceName = "KvStoreSnapshot::GetEntries()";
        }
        return false;
    }

    SaveEntries(allEntries, preInstallBundleInfos);
    return true;
}

Status PreInstallDataStorage::GetEntries(std::vector<Entry> &allEntries) const
{
    Status status = Status::ERROR;
    Key token;
    // if prefix is empty, get all entries.
    Key allEntryKeyPrefix("");
    if (kvStorePtr_) {
        // sync call GetEntries, the callback will be trigger at once
        status = kvStorePtr_->GetEntries(allEntryKeyPrefix, allEntries);
    }
    APP_LOGD("get all entries status: %{public}d", status);
    return status;
}

bool PreInstallDataStorage::SavePreInstallStorageBundleInfo(const PreInstallBundleInfo &preInstallBundleInfo)
{
    APP_LOGD("save PreInstall bundle data.");
    {
        std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
        if (!CheckKvStore()) {
            APP_LOGE("kvStore is nullptr");
            return false;
        }
    }
    std::string keyInfo = preInstallBundleInfo.GetBundleName();
    std::string valueInfo = preInstallBundleInfo.ToString();
    Key key(keyInfo);
    Value value(valueInfo);
    APP_LOGD("save PreInstallStorageBundleInfo, key: %{public}s value: %{public}s.",
        keyInfo.c_str(), valueInfo.c_str());
    Status status;
    {
        std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
        status = kvStorePtr_->Put(key, value);
        if (status == Status::IPC_ERROR) {
            status = kvStorePtr_->Put(key, value);
            APP_LOGD("distribute database ipc error and try to call again, result = %{public}d", status);
        }
    }

    if (status != Status::SUCCESS) {
        const std::string interfaceName = "kvStorePtr::Put()";
        APP_LOGD("put valLocalAbilityManager::InitializeSaProfilesue to kvStore error: %{public}d", status);
        return false;
    }
    APP_LOGD("put value to kvStore success when save PreInstall bundle data.");
    return true;
}

bool PreInstallDataStorage::DeletePreInstallStorageBundleInfo(const PreInstallBundleInfo &preInstallBundleInfo)
{
    APP_LOGD("delete PreInstall bundle data");
    {
        std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
        if (!CheckKvStore()) {
            APP_LOGE("kvStore is nullptr");
            return false;
        }
    }
    std::string keyInfo = preInstallBundleInfo.GetBundleName();
    Key key(keyInfo);
    APP_LOGD("delete PreInstallStorageBundleInfo, key: %{public}s", keyInfo.c_str());
    Status status;

    {
        std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
        status = kvStorePtr_->Delete(key);
        if (status == Status::IPC_ERROR) {
            status = kvStorePtr_->Delete(key);
            APP_LOGW("distribute database ipc error and try to call again, result = %{public}d", status);
        }
    }

    if (status != Status::SUCCESS) {
        const std::string interfaceName = "kvStorePtr::Delete()";
        APP_LOGE("delete key error: %{public}d", status);
        return false;
    } else {
        APP_LOGD("delete value to kvStore success");
    }
    return true;
}

void PreInstallDataStorage::UpdateDataBase(std::map<std::string, PreInstallBundleInfo>& infos)
{
    APP_LOGD("begin to update preInstall database.");
    for (const auto& item : infos) {
        if (SavePreInstallStorageBundleInfo(item.second)) {
            DeleteOldBundleInfo(item.first);
        }
    }
    APP_LOGD("update preInstall database done.");
}

void PreInstallDataStorage::DeleteOldBundleInfo(const std::string& oldKey)
{
    APP_LOGD("begin to delete old preInstall bundleInfo");
    std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
    if (!CheckKvStore()) {
        APP_LOGE("kvStore is nullptr");
        return;
    }
    Key key(oldKey);
    Status status = kvStorePtr_->Delete(key);
    if (status == Status::IPC_ERROR) {
        status = kvStorePtr_->Delete(key);
        APP_LOGW("distribute database ipc error and try to call again, result = %{public}d", status);
    }
    if (status != Status::SUCCESS) {
        APP_LOGE("delete old bundleInfo failed: %{public}d", status);
        return;
    }
    APP_LOGD("delete old preInstall bundleInfo success");
}
}  // namespace AppExecFwk
}  // namespace OHOS
