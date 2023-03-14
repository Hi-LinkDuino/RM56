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
#ifndef _AMS_ST_PAGE_ABILITY_EVENT_H_
#define _AMS_ST_PAGE_ABILITY_EVENT_H_

#include <vector>
#include <string>
#include <memory>
#include "common_event.h"
#include "common_event_manager.h"
#include "ability_loader.h"
#include "app_log_wrapper.h"
#include "ability_manager.h"
#include "running_process_info.h"

namespace OHOS {
namespace AppExecFwk {
namespace STEventName {
const std::string g_eventName = "resp_st_page_ability_callback";
const std::string g_eventNameProcessMemory = "resp_st_process_memory_info";
const std::string g_pidEventName = "resp_st_page_ability_pid_callback";
const std::string g_abilityStateInit = ":Init";
const std::string g_abilityStateOnStart = ":OnStart";
const std::string g_abilityStateOnStop = ":OnStop";
const std::string g_abilityStateOnActive = ":OnActive";
const std::string g_abilityStateOnInactive = ":OnInactive";
const std::string g_abilityStateOnBackground = ":OnBackground";
const std::string g_abilityStateOnForeground = ":OnForeground";
const std::string g_abilityStateOnNewWant = ":OnNewWant";
const std::string g_abilityStateOnCommand = ":OnCommand";
const std::string g_abilityStateOnConnect = ":OnConnect";
const std::string g_abilityStateOnDisconnect = ":OnDisconnect";
const std::string g_abilityStateInsert = ":Insert";
const std::string g_abilityStateDelete = ":Delete";
const std::string g_abilityStateUpdate = ":Update";
const std::string g_abilityStateQuery = ":Query";
const std::string g_abilityStateGetFileTypes = ":GetFileTypes";
const std::string g_abilityStateOpenFile = ":OpenFile";
const std::string g_getWantAgentState = ":GetWantAgentFail";
const std::string g_triggerWantAgentState = ":TriggerWantAgentSuccess";
const int g_defeventCode = 0;
const std::vector<std::string> g_eventList = {
    "requ_page_ability_terminate",
    "requ_get_process_memory_info",
    "requ_disconnect_service",
    "requ_page_ability_terminate_caller",
    "requ_page_ability_terminate_result",
};

static constexpr uint32_t LOAD_TIMEOUT = 3000;            // ms
static constexpr uint32_t ACTIVE_TIMEOUT = 5000;          // ms
static constexpr uint32_t INACTIVE_TIMEOUT = 500;         // ms
static constexpr uint32_t BACKGROUND_TIMEOUT = 10000;     // ms
static constexpr uint32_t TERMINATE_TIMEOUT = 10000;      // ms
static constexpr uint32_t CONNECT_TIMEOUT = 500;          // ms
static constexpr uint32_t DISCONNECT_TIMEOUT = 500;       // ms
static constexpr uint32_t COMMAND_TIMEOUT = 5000;         // ms
static constexpr uint32_t SYSTEM_UI_TIMEOUT = 5000;       // ms
static constexpr uint32_t RESTART_TIMEOUT = 5000;         // ms
static constexpr uint32_t RESTART_ABILITY_TIMEOUT = 500;  // ms
}  // namespace STEventName

class STPageAbilityEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    STPageAbilityEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp, std::shared_ptr<Ability> ability,
        sptr<AAFwk::IAbilityConnection> &stub)
        : CommonEventSubscriber(sp), ability_(ability), stub_(stub)
    {}
    ~STPageAbilityEventSubscriber()
    {}
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
    std::string RunningProcessInfoToString(std::vector<RunningProcessInfo> &infos);
    bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);

private:
    std::weak_ptr<Ability> ability_;
    wptr<AAFwk::IAbilityConnection> stub_;
};

class STPageAbilityEvent {
public:
    STPageAbilityEvent() = default;
    explicit STPageAbilityEvent(const std::string &className);
    ~STPageAbilityEvent() = default;

    static bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    void SubscribeEvent(std::vector<std::string> eventList, std::shared_ptr<Ability> ability,
        sptr<AAFwk::IAbilityConnection> stub = nullptr);
    void UnsubscribeEvent();
    std::string GetEventDate(const std::string &stateCallbackCount);
    std::string GetCallBackPath(const std::string &callBackPath);
    std::string GetAbilityStatus(const std::string &abilityStatus);

    int GetOnStartCount();
    int GetOnStopCount();
    int GetOnActiveCount();
    int GetOnInactiveCount();
    int GetOnBackgroundCount();
    int GetOnForegroundCount();
    int GetOnNewWantCount();
    int GetOnCommandCount();
    int GetOnConnectCount();
    int GetOnDisconnectCount();

private:
    std::shared_ptr<STPageAbilityEventSubscriber> subscriber_;
    int onStartCount_ = 0;
    int onStopCount_ = 0;
    int onActiveCount_ = 0;
    int onInactiveCount_ = 0;
    int onBackgroundCount_ = 0;
    int onForegroundCount_ = 0;
    int onNewWantCount_ = 0;
    int onCommandCount_ = 0;
    int onConnectCount_ = 0;
    int onDisconnectCount_ = 0;
    std::string className_;
    std::string callBackPath_;
    std::string abilityStatus_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // _AMS_ST_PAGE_ABILITY_EVENT_H_