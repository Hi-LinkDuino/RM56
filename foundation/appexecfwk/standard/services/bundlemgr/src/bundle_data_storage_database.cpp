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

#include "bundle_data_storage_database.h"

#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_exception_handler.h"
#include "bundle_sandbox_exception_handler.h"

#include "kvstore_death_recipient_callback.h"

using namespace OHOS::DistributedKv;

namespace OHOS {
namespace AppExecFwk {
namespace {
const int32_t MAX_TIMES = 600;              // 1min
const int32_t SLEEP_INTERVAL = 100 * 1000;  // 100ms
}  // namespace

BundleDataStorageDatabase::BundleDataStorageDatabase()
{
    APP_LOGI("instance:%{private}p is created", this);
    TryTwice([this] { return GetKvStore(); });
    RegisterKvStoreDeathListener();
}

BundleDataStorageDatabase::~BundleDataStorageDatabase()
{
    APP_LOGI("instance:%{private}p is destroyed", this);
    dataManager_.CloseKvStore(appId_, kvStorePtr_);
}

void BundleDataStorageDatabase::SaveEntries(
    const std::vector<Entry> &allEntries, std::map<std::string, InnerBundleInfo> &infos)
{
    std::map<std::string, InnerBundleInfo> updateInfos;
    for (const auto &item : allEntries) {
        InnerBundleInfo innerBundleInfo;

        nlohmann::json jsonObject = nlohmann::json::parse(item.value.ToString(), nullptr, false);
        if (jsonObject.is_discarded()) {
            APP_LOGE("error key: %{private}s", item.key.ToString().c_str());
            // it's an bad json, delete it
            {
                std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
                kvStorePtr_->Delete(item.key);
            }
            continue;
        }
        if (innerBundleInfo.FromJson(jsonObject) != ERR_OK) {
            APP_LOGE("error key: %{private}s", item.key.ToString().c_str());
            // it's an error value, delete it
            {
                std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
                kvStorePtr_->Delete(item.key);
            }
            continue;
        }
        bool isBundleValid = true;
        auto handler = std::make_shared<BundleExceptionHandler>(shared_from_this());
        handler->HandleInvalidBundle(innerBundleInfo, isBundleValid);
        auto sandboxHandler = std::make_shared<BundleSandboxExceptionHandler>(shared_from_this());
        sandboxHandler->RemoveSandboxApp(innerBundleInfo);
        if (!isBundleValid) {
            continue;
        }
        infos.emplace(innerBundleInfo.GetBundleName(), innerBundleInfo);
        // database update
        std::string key = item.key.ToString();
        if (key != innerBundleInfo.GetBundleName()) {
            updateInfos.emplace(key, innerBundleInfo);
        }
    }
    if (updateInfos.size() > 0) {
        UpdateDataBase(updateInfos);
    }
    APP_LOGD("SaveEntries end");
}

bool BundleDataStorageDatabase::LoadAllData(std::map<std::string, InnerBundleInfo> &infos)
{
    APP_LOGI("load all installed bundle data to map");
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

    bool ret = true;
    if (status != Status::SUCCESS) {
        APP_LOGE("get entries error: %{public}d", status);
        // KEY_NOT_FOUND means no data in database, no need to report.
        if (status != Status::KEY_NOT_FOUND) {
            const std::string interfaceName = "KvStoreSnapshot::GetEntries()";
        }
        ret = false;
    } else {
        SaveEntries(allEntries, infos);
    }
    return ret;
}

bool BundleDataStorageDatabase::SaveStorageBundleInfo(const InnerBundleInfo &innerBundleInfo)
{
    APP_LOGI("save bundle data");
    {
        std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
        if (!CheckKvStore()) {
            APP_LOGE("kvStore is nullptr");
            return false;
        }
    }

    Key key(innerBundleInfo.GetBundleName());
    Value value(innerBundleInfo.ToString());
    Status status;
    {
        std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
        status = kvStorePtr_->Put(key, value);
        if (status == Status::IPC_ERROR) {
            status = kvStorePtr_->Put(key, value);
            APP_LOGW("distribute database ipc error and try to call again, result = %{public}d", status);
        }
    }
    if (status != Status::SUCCESS) {
        const std::string interfaceName = "kvStorePtr::Put()";
        APP_LOGE("put valLocalAbilityManager::InitializeSaProfilesue to kvStore error: %{public}d", status);
        return false;
    }

    APP_LOGI("put value to kvStore success");
    return true;
}

bool BundleDataStorageDatabase::DeleteStorageBundleInfo(const InnerBundleInfo &innerBundleInfo)
{
    APP_LOGI("delete bundle data");
    {
        std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
        if (!CheckKvStore()) {
            APP_LOGE("kvStore is nullptr");
            return false;
        }
    }
    Key key(innerBundleInfo.GetBundleName());
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
        APP_LOGI("delete value to kvStore success");
    }
    return true;
}

void BundleDataStorageDatabase::RegisterKvStoreDeathListener()
{
    APP_LOGI("register kvStore death listener");
    std::shared_ptr<DistributedKv::KvStoreDeathRecipient> callback = std::make_shared<KvStoreDeathRecipientCallback>();
    dataManager_.RegisterKvStoreServiceDeathRecipient(callback);
}

bool BundleDataStorageDatabase::CheckKvStore()
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
        APP_LOGD("CheckKvStore, Times: %{public}d", tryTimes);
        usleep(SLEEP_INTERVAL);
        tryTimes--;
    }
    return kvStorePtr_ != nullptr;
}

Status BundleDataStorageDatabase::GetKvStore()
{
    Options options = {
        .createIfMissing = true,
        .encrypt = false,
        .autoSync = false,
        .kvStoreType = KvStoreType::SINGLE_VERSION
        };

    Status status = dataManager_.GetSingleKvStore(options, appId_, storeId_, kvStorePtr_);
    if (status != Status::SUCCESS) {
        APP_LOGE("return error: %{public}d", status);
    } else {
        APP_LOGI("get kvStore success");
    }
    return status;
}

Status BundleDataStorageDatabase::GetEntries(std::vector<Entry> &allEntries) const
{
    Status status = Status::ERROR;
    Key token;
    // if prefix is empty, get all entries.
    Key allEntryKeyPrefix("");
    if (kvStorePtr_) {
        // sync call GetEntries, the callback will be trigger at once
        status = kvStorePtr_->GetEntries(allEntryKeyPrefix, allEntries);
    }
    APP_LOGI("get all entries status: %{public}d", status);
    return status;
}

void BundleDataStorageDatabase::TryTwice(const std::function<Status()> &func) const
{
    Status status = func();
    if (status == Status::IPC_ERROR) {
        status = func();
        APP_LOGW("distribute database ipc error and try to call again, result = %{public}d", status);
    }
}

bool BundleDataStorageDatabase::ResetKvStore()
{
    std::lock_guard<std::mutex> lock(kvStorePtrMutex_);
    kvStorePtr_ = nullptr;
    Status status = GetKvStore();
    if (status == Status::SUCCESS && kvStorePtr_ != nullptr) {
        return true;
    }
    APP_LOGW("failed");
    return false;
}

void BundleDataStorageDatabase::UpdateDataBase(std::map<std::string, InnerBundleInfo>& infos)
{
    APP_LOGD("begin to update database.");
    for (const auto& item : infos) {
        if (SaveStorageBundleInfo(item.second)) {
            DeleteOldBundleInfo(item.first);
        }
    }
    APP_LOGD("update database done.");
}

void BundleDataStorageDatabase::DeleteOldBundleInfo(const std::string& oldKey)
{
    APP_LOGD("begin to delete old bundleInfo");
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
    APP_LOGD("delete old bundleInfo success");
}
}  // namespace AppExecFwk
}  // namespace OHOS