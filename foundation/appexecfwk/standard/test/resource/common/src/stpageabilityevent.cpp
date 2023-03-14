/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "stpageabilityevent.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::EventFwk;

STPageAbilityEvent::STPageAbilityEvent(const std::string &className)
{
    this->className_ = className;
}

bool STPageAbilityEvent::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}

void STPageAbilityEvent::SubscribeEvent(
    std::vector<std::string> eventList, std::shared_ptr<Ability> ability, sptr<AAFwk::IAbilityConnection> stub)
{
    MatchingSkills matchingSkills;
    for (const auto &e : eventList) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<STPageAbilityEventSubscriber>(subscribeInfo, ability, stub);
    CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void STPageAbilityEvent::UnsubscribeEvent()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

std::string STPageAbilityEvent::GetEventDate(const std::string &stateCallbackCount)
{
    return this->className_ + stateCallbackCount;
}

std::string STPageAbilityEvent::GetCallBackPath(const std::string &callBackPath)
{
    this->callBackPath_ += callBackPath;
    return this->callBackPath_;
}

std::string STPageAbilityEvent::GetAbilityStatus(const std::string &abilityStatus)
{
    this->abilityStatus_ += abilityStatus;
    return this->abilityStatus_;
}

int STPageAbilityEvent::GetOnStartCount()
{
    onStartCount_++;
    return onStartCount_;
}

int STPageAbilityEvent::GetOnStopCount()
{
    onStopCount_++;
    return onStopCount_;
}

int STPageAbilityEvent::GetOnActiveCount()
{
    onActiveCount_++;
    return onActiveCount_;
}

int STPageAbilityEvent::GetOnInactiveCount()
{
    onInactiveCount_++;
    return onInactiveCount_;
}

int STPageAbilityEvent::GetOnBackgroundCount()
{
    onBackgroundCount_++;
    return onBackgroundCount_;
}

int STPageAbilityEvent::GetOnForegroundCount()
{
    onForegroundCount_++;
    return onForegroundCount_;
}

int STPageAbilityEvent::GetOnNewWantCount()
{
    onNewWantCount_++;
    return onNewWantCount_;
}

int STPageAbilityEvent::GetOnCommandCount()
{
    onCommandCount_++;
    return onCommandCount_;
}

int STPageAbilityEvent::GetOnConnectCount()
{
    onConnectCount_++;
    return onConnectCount_;
}

int STPageAbilityEvent::GetOnDisconnectCount()
{
    onDisconnectCount_++;
    return onDisconnectCount_;
}

void STPageAbilityEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    APP_LOGI("DataTestPageAEventSubscriber::OnReceiveEvent:event=%{public}s", data.GetWant().GetAction().c_str());
    APP_LOGI("DataTestPageAEventSubscriber::OnReceiveEvent:data=%{public}s", data.GetData().c_str());
    APP_LOGI("DataTestPageAEventSubscriber::OnReceiveEvent:code=%{public}d", data.GetCode());
    auto eventName = data.GetWant().GetAction();
    if (!this->ability_.lock()) {
        APP_LOGI("STPageAbilityEventSubscriber:ability_ is nullptr");
    }
    if (eventName.compare("requ_page_ability_terminate") == 0) {
        std::string target = data.GetData();
        if (target.compare((this->ability_.lock())->GetAbilityName()) == 0) {
            (this->ability_.lock())->TerminateAbility();
        }
    }
    if (eventName.compare("requ_get_process_memory_info") == 0) {
    }
    if (eventName.compare("requ_disconnect_service") == 0) {
        std::string target = data.GetData();
        if (target.compare((this->ability_.lock())->GetAbilityName()) == 0) {
            if (stub_.promote()) {
                (this->ability_.lock())->DisconnectAbility(stub_.promote());
            }
            APP_LOGI("GetMyProcessMemoryInfo:stub_ is nullptr");
        }
    }
    if (eventName.compare("requ_page_ability_terminate_caller") == 0) {
        std::string target = data.GetData();
        if (target.compare((this->ability_.lock())->GetAbilityName()) == 0) {
            int requestCode = data.GetCode();
            (this->ability_.lock())->TerminateAbility(requestCode);
        }
    }
    if (eventName.compare("requ_page_ability_terminate_result") == 0) {
        std::string target = data.GetData();
        int startId = data.GetCode();
        if (target.compare((this->ability_.lock())->GetAbilityName()) == 0) {
            (this->ability_.lock())->TerminateAbilityResult(startId);
        }
    }
}

std::string STPageAbilityEventSubscriber::RunningProcessInfoToString(std::vector<RunningProcessInfo> &infos)
{
    std::string data = "";
    return data;
}

bool STPageAbilityEventSubscriber::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    Want want;
    want.SetAction(eventName);
    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}
}  // namespace AppExecFwk
}  // namespace OHOS