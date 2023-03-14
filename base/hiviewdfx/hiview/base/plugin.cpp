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
#include "plugin.h"
#include <functional>
#include "audit.h"
#include "defines.h"
#include "file_util.h"
#include "thread_util.h"
namespace OHOS {
namespace HiviewDFX {
Plugin::~Plugin()
{
    if (handle_ != DynamicModuleDefault) {
        UnloadModule(handle_);
        handle_ = DynamicModuleDefault;
    }
}

bool Plugin::OnEvent(std::shared_ptr<Event>& event __UNUSED)
{
    return true;
}

bool Plugin::CanProcessEvent(std::shared_ptr<Event> event __UNUSED)
{
    return true;
}

bool Plugin::CanProcessMoreEvents()
{
    return true;
}

bool Plugin::OnEventProxy(std::shared_ptr<Event> event)
{
    if (event == nullptr) {
        return false;
    }

    std::shared_ptr<Event> dupEvent = event;
    auto processorSize = dupEvent->GetPendingProcessorSize();
    dupEvent->ResetPendingStatus();
    bool ret = OnEvent(dupEvent);

    if (!dupEvent->IsPipelineEvent()) {
        if (Audit::IsEnabled()) {
            Audit::WriteAuditEvent(Audit::StatsEvent::QUEUE_EVENT_OUT, dupEvent->createTime_,
                std::to_string(Thread::GetTid()));
        }
    } else {
        if ((!dupEvent->HasFinish() && !dupEvent->HasPending()) &&
            (processorSize == dupEvent->GetPendingProcessorSize())) {
            dupEvent->OnContinue();
        }
    }
    return ret;
}

void Plugin::DelayProcessEvent(std::shared_ptr<Event> event, uint64_t delay)
{
    if (workLoop_ == nullptr || event == nullptr) {
        return;
    }

    UpdateTimeByDelay(delay);
    event->OnPending();
    auto task = std::bind(&Plugin::OnEventProxy, this, event);
    workLoop_->AddTimerEvent(nullptr, nullptr, task, delay, false);
    return;
}

void Plugin::AddEventListenerInfo(uint32_t type, const EventListener::EventIdRange& range)
{
    if (context_ == nullptr) {
        return;
    }
    std::set<std::string> eventNames;
    std::set<EventListener::EventIdRange> listenerInfo;
    listenerInfo.insert(range);
    context_->AddListenerInfo(type, shared_from_this(), eventNames, listenerInfo);
}

void Plugin::AddEventListenerInfo(uint32_t type, const std::set<EventListener::EventIdRange> &listenerInfo)
{
    if (context_ == nullptr) {
        return;
    }
    std::set<std::string> eventNames;
    context_->AddListenerInfo(type, shared_from_this(), eventNames, listenerInfo);
}

bool Plugin::GetEventListenerInfo(uint32_t type, std::set<EventListener::EventIdRange> &listenerInfo)
{
    if (context_ == nullptr) {
        return false;
    }
    return context_->GetListenerInfo(type, name_, listenerInfo);
}

void Plugin::AddEventListenerInfo(uint32_t type, const std::string& eventName)
{
    if (context_ == nullptr) {
        return;
    }
    std::set<std::string> eventNames;
    eventNames.insert(eventName);
    std::set<EventListener::EventIdRange> listenerInfo;
    context_->AddListenerInfo(type, shared_from_this(), eventNames, listenerInfo);
}

void Plugin::AddEventListenerInfo(uint32_t type, const std::set<std::string> &eventNames)
{
    if (context_ == nullptr) {
        return;
    }
    std::set<EventListener::EventIdRange> listenerInfo;
    context_->AddListenerInfo(type, shared_from_this(), eventNames, listenerInfo);
}

bool Plugin::GetEventListenerInfo(uint32_t type, std::set<std::string> &eventNames)
{
    if (context_ == nullptr) {
        return false;
    }
    return context_->GetListenerInfo(type, name_, eventNames);
}

std::string Plugin::GetPluginInfo()
{
    return GetName();
}

std::string Plugin::GetHandlerInfo()
{
    return GetName();
}

const std::string& Plugin::GetName()
{
    return name_;
}

const std::string& Plugin::GetVersion()
{
    return version_;
}

void Plugin::SetName(const std::string& name)
{
    name_ = name;
}

void Plugin::SetVersion(const std::string& version)
{
    version_ = version;
}

void Plugin::BindWorkLoop(std::shared_ptr<EventLoop> loop)
{
    workLoop_ = loop;
}

std::shared_ptr<EventLoop> Plugin::GetWorkLoop()
{
    return workLoop_;
}

void Plugin::UpdateActiveTime()
{
    lastActiveTime_ = time(nullptr);
}

void Plugin::UpdateTimeByDelay(time_t delay)
{
    lastActiveTime_ = time(nullptr) + delay;
}
} // namespace HiviewDFX
} // namespace OHOS
