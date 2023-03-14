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

#include "hks_event_observer.h"

#include "bundle_constants.h"
#include "common_event_support.h"
#include "os_account_manager.h"

#include "hks_log.h"
#include "hks_mem.h"
#include "hks_storage.h"
#include "hks_type.h"

static void GetProcessInfo(int userId, int uid, struct HksProcessInfo *processInfo)
{
    uint32_t userSize = sizeof(userId);
    uint8_t *userData = (uint8_t *)HksMalloc(userSize);
    if (userData == nullptr) {
        HKS_LOG_E("user id malloc failed.");
        return;
    }
    (void)memcpy_s(userData, userSize, &userId, userSize);
    processInfo->userId.size = userSize;
    processInfo->userId.data = userData;

    uint32_t uidSize = sizeof(uid);
    uint8_t *uidData = (uint8_t *)HksMalloc(uidSize);
    if (uidData == nullptr) {
        HKS_LOG_E("uid malloc failed.");
        HksFree(userData);
        processInfo->userId.data = nullptr;
        return;
    }
    (void)memcpy_s(uidData, uidSize, &uid, uidSize);
    processInfo->processName.size = uidSize;
    processInfo->processName.data = uidData;
}

static void GetProcessName(int uid, struct HksBlob *processName)
{
    uint32_t uidSize = sizeof(uid);
    uint8_t *uidData = (uint8_t *)HksMalloc(uidSize);
    if (uidData == nullptr) {
        HKS_LOG_E("uid malloc failed.");
        return;
    }
    (void)memcpy_s(uidData, uidSize, &uid, uidSize);
    processName->size = uidSize;
    processName->data = uidData;
}

namespace OHOS {
namespace Security {
namespace Hks {
std::shared_ptr<SystemEventSubscriber> SystemEventObserver::systemEventSubscriber_ = nullptr;

SystemEventSubscriber::SystemEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : OHOS::EventFwk::CommonEventSubscriber(subscriberInfo)
{}

void SystemEventSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    auto want = data.GetWant();
    std::string action = want.GetAction();
    if (action == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        int uid = want.GetIntParam(AppExecFwk::Constants::UID, -1);
        int userId = -1;
        OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(uid, userId);
        HKS_LOG_I("HksService package removed: uid is %d userId is %d", uid, userId);
        struct HksProcessInfo processInfo = { { 0, NULL }, { 0, NULL } };
        GetProcessInfo(userId, uid, &processInfo);
        HksServiceDeleteUIDKeyAliasFile(processInfo);
        HKS_FREE_BLOB(processInfo.userId);
        HKS_FREE_BLOB(processInfo.processName);
    } else if (action == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED) {
        int userId = data.GetCode();
        HKS_LOG_I("HksService user removed: userId is %d", userId);
        struct HksBlob processName = { 0, NULL };
        GetProcessName(userId, &processName);
        HksServiceDeleteUserIDKeyAliasFile(processName);
        HKS_FREE_BLOB(processName);
    }
}

SystemEventObserver::~SystemEventObserver()
{
    if (systemEventSubscriber_ != nullptr) {
        UnSubscribeSystemEvent();
    }
}

bool SystemEventObserver::SubscribeSystemEvent()
{
    OHOS::EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    OHOS::EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    systemEventSubscriber_ = std::make_shared<SystemEventSubscriber>(subscriberInfo);

    if (systemEventSubscriber_ == nullptr) {
        HKS_LOG_E("huks system subscriber nullptr");
        return false;
    }

    return OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(systemEventSubscriber_);
}

bool SystemEventObserver::UnSubscribeSystemEvent()
{
    if (systemEventSubscriber_ == nullptr) {
        HKS_LOG_E("huks system subscriber nullptr");
        return false;
    }
    return OHOS::EventFwk::CommonEventManager::UnSubscribeCommonEvent(systemEventSubscriber_);
}
} // namespace Hks
} // namespace Security
} // namespace OHOS
