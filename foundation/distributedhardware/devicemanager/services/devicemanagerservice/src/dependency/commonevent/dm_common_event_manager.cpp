/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dm_common_event_manager.h"

#include <thread>

#include "dm_constants.h"

using namespace OHOS::EventFwk;

namespace OHOS {
namespace DistributedHardware {
std::mutex DmCommonEventManager::callbackQueueMutex_;
std::mutex DmCommonEventManager::eventSubscriberMutex_;
std::condition_variable DmCommonEventManager::notEmpty_;
std::list<CommomEventCallbackNode> DmCommonEventManager::callbackQueue_;

DmCommonEventManager &DmCommonEventManager::GetInstance()
{
    static DmCommonEventManager instance;
    return instance;
}

DmCommonEventManager::DmCommonEventManager()
{
    std::thread th(DealCallback);
    th.detach();
}

DmCommonEventManager::~DmCommonEventManager()
{
    std::unique_lock<std::mutex> mutexLock(eventSubscriberMutex_);
    for (auto iter = dmEventSubscriber_.begin(); iter != dmEventSubscriber_.end(); iter++) {
        if (!CommonEventManager::UnSubscribeCommonEvent(iter->second)) {
            LOGI("Unsubscribe service event failed: %s", iter->first.c_str());
        }
    }
}

void DmCommonEventManager::DealCallback(void)
{
    while (1) {
        std::unique_lock<std::mutex> callbackQueueMutexLock(callbackQueueMutex_);
        notEmpty_.wait(callbackQueueMutexLock, [] { return !callbackQueue_.empty(); });
        CommomEventCallbackNode node = callbackQueue_.front();
        int32_t input = node.input_;
        CommomEventCallback funcPrt = node.callback_;
        funcPrt(input);
        callbackQueue_.pop_front();
    }
}

bool DmCommonEventManager::SubscribeServiceEvent(const std::string &event, const CommomEventCallback callback)
{
    LOGI("Subscribe event: %s", event.c_str());
    if (dmEventSubscriber_.find(event) != dmEventSubscriber_.end() || callback == nullptr) {
        LOGE("Subscribe event：%s has been exist or callback is nullptr", event.c_str());
        return false;
    }

    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(event);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<EventSubscriber> subscriber =
        std::make_shared<EventSubscriber>(subscriberInfo, callback, event);
    if (subscriber == nullptr) {
        LOGE("subscriber is nullptr %s", event.c_str());
        return false;
    }

    if (!CommonEventManager::SubscribeCommonEvent(subscriber)) {
        LOGE("Subscribe service event failed: %s", event.c_str());
        return false;
    }

    std::unique_lock<std::mutex> mutexLock(eventSubscriberMutex_);
    dmEventSubscriber_[event] = subscriber;
    return true;
}

bool DmCommonEventManager::UnsubscribeServiceEvent(const std::string &event)
{
    LOGI("UnSubscribe event: %s", event.c_str());
    if (dmEventSubscriber_.find(event) == dmEventSubscriber_.end()) {
        LOGE("UnSubscribe event: %s not been exist", event.c_str());
        return false;
    }

    if (!CommonEventManager::UnSubscribeCommonEvent(dmEventSubscriber_[event])) {
        LOGE("Unsubscribe service event failed: %s", event.c_str());
        return false;
    }

    std::unique_lock<std::mutex> mutexLock(eventSubscriberMutex_);
    dmEventSubscriber_.erase(event);
    return true;
}

void DmCommonEventManager::EventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    std::string receiveEvent = data.GetWant().GetAction();
    LOGI("Received event: %s", receiveEvent.c_str());
    if (receiveEvent != event_) {
        LOGE("Received event is error");
        return;
    }

    int32_t userId = data.GetCode();
    if (userId <= 0) {
        LOGE("userId is less zero");
        return;
    }

    std::unique_lock<std::mutex> callbackQueueMutexLock(callbackQueueMutex_);
    if (callbackQueue_.size() > COMMON_CALLBACK_MAX_SIZE) {
        LOGE("event callback Queue is too long");
        return;
    }

    CommomEventCallbackNode node {userId, callback_};
    callbackQueue_.push_back(node);
    notEmpty_.notify_one();
}
} // namespace DistributedHardware
} // namespace OHOS
