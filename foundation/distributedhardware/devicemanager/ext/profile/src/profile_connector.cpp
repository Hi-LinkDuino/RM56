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

#include "profile_connector.h"

#include "dm_constants.h"
#include "dm_log.h"

using namespace OHOS::DeviceProfile;

namespace OHOS {
namespace DistributedHardware {
ProfileConnector::ProfileConnector()
{
    LOGI("ProfileConnector construct");
}

ProfileConnector::~ProfileConnector()
{
    LOGI("ProfileConnector Destructor");
}

int32_t ProfileConnector::RegisterProfileCallback(const std::string &pkgName, const std::string &deviceId,
                                                  IProfileConnectorCallback* callback)
{
    if (pkgName.empty() || deviceId.empty() ||  callback == nullptr) {
        LOGE("Not a reasonable function argument");
        return ERR_DM_INPUT_PARA_INVALID;
    }

    LOGI("register profile callback with pkgName: %s", pkgName.c_str());
    {
        std::lock_guard<std::mutex> mutexLock(callbackMapMutex_);
        if (callbackMap_.find(pkgName) != callbackMap_.end()) {
            LOGE("pkgName: %s already exists in the map", pkgName.c_str());
            return ERR_DM_KEY_ALREADY_EXISTS;
        }
        LOGI("register profile callback pkgName: %s", pkgName.c_str());
        callbackMap_[pkgName] = callback;
    }
    std::list<std::string> serviceIds = { "system", "device", "fakeStorage", "fakeSystem" };
    int32_t ret = SubscribeProfileEvents(serviceIds, deviceId);
    if (ret != DM_OK) {
        LOGE("fail to subscribe profile events");
    }
    return ret;
}

int32_t ProfileConnector::UnRegisterProfileCallback(const std::string &pkgName)
{
    if (pkgName.empty()) {
        LOGE("Not a reasonable function argument");
        return ERR_DM_INPUT_PARA_INVALID;
    }

    LOGI("unregister profile callback with pkgName: %s", pkgName.c_str());
    std::lock_guard<std::mutex> mutexLock(callbackMapMutex_);
    if (callbackMap_.find(pkgName) != callbackMap_.end()) {
        callbackMap_.erase(pkgName);
    }
    return DM_OK;
}

int32_t ProfileConnector::SubscribeProfileEvents(const std::list<std::string> &serviceIds, const std::string &deviceId)
{
    SubscribeInfo eventSync;
    std::list<ProfileEvent> failedEvents;
    std::list<SubscribeInfo> subscribeInfos;
    eventSync.profileEvent = ProfileEvent::EVENT_SYNC_COMPLETED;
    subscribeInfos.emplace_back(eventSync);
    int32_t errCode = DistributedDeviceProfileClient::GetInstance().SubscribeProfileEvents(
        subscribeInfos, shared_from_this(), failedEvents);
    if (errCode != ERR_OK) {
        LOGI("subscribe profile events result: %ud", errCode);
        return ERR_DM_UNSUBSCRIBE_DP_EVENTS;
    }
    return DM_OK;
}

int32_t ProfileConnector::UnSubscribeProfileEvents()
{
    std::list<ProfileEvent> profileEvents;
    std::list<ProfileEvent> failedEvents;
    profileEvents.emplace_back(ProfileEvent::EVENT_SYNC_COMPLETED);
    int32_t errCode = DistributedDeviceProfileClient::GetInstance().UnsubscribeProfileEvents(
        profileEvents, shared_from_this(), failedEvents);
    if (errCode != ERR_OK) {
        LOGI("unSubscribe profile events result:%ud", errCode);
        return ERR_DM_UNSUBSCRIBE_DP_EVENTS;
    }
    return DM_OK;
}

void ProfileConnector::OnSyncCompleted(const SyncResult &syncResults)
{
    std::string deviceId;
    u_int32_t SyncStatus;
    for (auto &iterResult : syncResults) {
        deviceId = iterResult.first;
        SyncStatus = iterResult.second;
    }
    LOGI("ProfileEventCallback::OnSyncCompleted, deviceId = %s", deviceId.c_str());
    std::lock_guard<std::mutex> mutexLock(callbackMapMutex_);
    for (auto &iter : callbackMap_) {
        iter.second->OnProfileComplete(iter.first, deviceId);
    }
}
} // namespace DistributedHardware
} // namespace OHOS
