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

#include "event_handler.h"

#include <unistd.h>
#include "event_handler_utils.h"
#include "hichecker.h"
#include "thread_local_data.h"

DEFINE_HILOG_LABEL("EventHandler");

using namespace OHOS::HiviewDFX;
namespace OHOS {
namespace AppExecFwk {
static constexpr int DATETIME_STRING_LENGTH = 80;

ThreadLocalData<std::weak_ptr<EventHandler>> EventHandler::currentEventHandler;

std::shared_ptr<EventHandler> EventHandler::Current()
{
    const std::weak_ptr<EventHandler> &wp = currentEventHandler;
    return wp.lock();
}

EventHandler::EventHandler(const std::shared_ptr<EventRunner> &runner) : eventRunner_(runner)
{}

EventHandler::~EventHandler()
{
    if (eventRunner_) {
        /*
         * This handler is finishing, need to remove all events belong to it.
         * But events only have weak pointer of this handler,
         * now weak pointer is invalid, so these events become orphans.
         */
        eventRunner_->GetEventQueue()->RemoveOrphan();
    }
}

bool EventHandler::SendEvent(InnerEvent::Pointer &event, int64_t delayTime, Priority priority)
{
    if (!event) {
        HILOGE("SendEvent: Could not send an invalid event");
        return false;
    }

    if (!eventRunner_) {
        HILOGE("SendEvent: MUST Set event runner before sending events");
        return false;
    }

    InnerEvent::TimePoint now = InnerEvent::Clock::now();
    event->SetSendTime(now);

    if (delayTime > 0) {
        event->SetHandleTime(now + std::chrono::milliseconds(delayTime));
    } else {
        event->SetHandleTime(now);
    }

    event->SetOwner(shared_from_this());
    // get traceId from event, if HiTrace::begin has been called, would get a valid trace id.
    auto traceId = event->GetOrCreateTraceId();
    // if traceId is valid, out put trace information
    if (AllowHiTraceOutPut(traceId, event->HasWaiter())) {
        HiTracePointerOutPut(traceId, event, "Send", HiTraceTracepointType::HITRACE_TP_CS);
    }

    eventRunner_->GetEventQueue()->Insert(event, priority);
    return true;
}

bool EventHandler::SendTimingEvent(InnerEvent::Pointer &event, int64_t taskTime, Priority priority)
{
    InnerEvent::TimePoint nowSys = InnerEvent::Clock::now();
    auto epoch = nowSys.time_since_epoch();
    long nowSysTime = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
    int64_t delayTime = taskTime - nowSysTime;
    if (delayTime < 0) {
        HILOGE("SendTimingEvent: SendTime is before now systime, change to 0 delaytime Event");
        return SendEvent(event, 0, priority);
    }

    return SendEvent(event, delayTime, priority);
}

bool EventHandler::SendSyncEvent(InnerEvent::Pointer &event, Priority priority)
{
    if ((!event) || (priority == Priority::IDLE)) {
        HILOGE("SendSyncEvent: Could not send an invalid event or idle event");
        return false;
    }

    if ((!eventRunner_) || (!eventRunner_->IsRunning())) {
        HILOGE("SendSyncEvent: MUST Set a running event runner before sending sync events");
        return false;
    }

    // If send a sync event in same event runner, distribute here.
    if (eventRunner_ == EventRunner::Current()) {
        DistributeEvent(event);
        return true;
    }

    // get traceId from event, if HiTrace::begin has been called, would get a valid trace id.
    auto spanId = event->GetOrCreateTraceId();

    // Create waiter, used to block.
    auto waiter = event->CreateWaiter();
    // Send this event as normal one.
    if (!SendEvent(event, 0, priority)) {
        return false;
    }
    // Wait until event is processed(recycled).
    waiter->Wait();

    if ((spanId) && (spanId->IsValid())) {
        HiTrace::Tracepoint(HiTraceTracepointType::HITRACE_TP_CR, *spanId, "event is processed");
    }

    return true;
}

void EventHandler::RemoveAllEvents()
{
    if (!eventRunner_) {
        HILOGE("RemoveAllEvents: MUST Set event runner before removing all events");
        return;
    }

    eventRunner_->GetEventQueue()->Remove(shared_from_this());
}

void EventHandler::RemoveEvent(uint32_t innerEventId)
{
    if (!eventRunner_) {
        HILOGE("RemoveEvent: MUST Set event runner before removing events by id");
        return;
    }

    eventRunner_->GetEventQueue()->Remove(shared_from_this(), innerEventId);
}

void EventHandler::RemoveEvent(uint32_t innerEventId, int64_t param)
{
    if (!eventRunner_) {
        HILOGE("RemoveEvent: MUST Set event runner before removing events by id and param");
        return;
    }

    eventRunner_->GetEventQueue()->Remove(shared_from_this(), innerEventId, param);
}

void EventHandler::RemoveTask(const std::string &name)
{
    if (!eventRunner_) {
        HILOGE("RemoveTask: MUST Set event runner before removing events by task name");
        return;
    }

    eventRunner_->GetEventQueue()->Remove(shared_from_this(), name);
}

ErrCode EventHandler::AddFileDescriptorListener(
    int32_t fileDescriptor, uint32_t events, const std::shared_ptr<FileDescriptorListener> &listener)
{
    if ((fileDescriptor < 0) || ((events & FILE_DESCRIPTOR_EVENTS_MASK) == 0) || (!listener)) {
        HILOGE("AddFileDescriptorListener(%{public}d, %{public}u, %{public}s): Invalid parameter",
            fileDescriptor,
            events,
            listener ? "valid" : "null");
        return EVENT_HANDLER_ERR_INVALID_PARAM;
    }

    if (!eventRunner_) {
        HILOGE("AddFileDescriptorListener: MUST Set event runner before adding fd listener");
        return EVENT_HANDLER_ERR_NO_EVENT_RUNNER;
    }

    listener->SetOwner(shared_from_this());
    return eventRunner_->GetEventQueue()->AddFileDescriptorListener(fileDescriptor, events, listener);
}

void EventHandler::RemoveAllFileDescriptorListeners()
{
    if (!eventRunner_) {
        HILOGE("RemoveAllFileDescriptorListeners: MUST Set event runner before removing all fd listener");
        return;
    }

    eventRunner_->GetEventQueue()->RemoveFileDescriptorListener(shared_from_this());
}

void EventHandler::RemoveFileDescriptorListener(int32_t fileDescriptor)
{
    if (fileDescriptor < 0) {
        HILOGE("RemoveFileDescriptorListener(%{public}d): Invalid parameter", fileDescriptor);
        return;
    }

    if (!eventRunner_) {
        HILOGE("RemoveFileDescriptorListener: MUST Set event runner before removing fd listener by fd");
        return;
    }

    eventRunner_->GetEventQueue()->RemoveFileDescriptorListener(fileDescriptor);
}

void EventHandler::SetEventRunner(const std::shared_ptr<EventRunner> &runner)
{
    if (eventRunner_ == runner) {
        return;
    }

    if (eventRunner_) {
        HILOGW("SetEventRunner: It is not recommended to change the event runner dynamically");

        // Remove all events and listeners from old event runner.
        RemoveAllEvents();
        RemoveAllFileDescriptorListeners();
    }

    // Switch event runner.
    eventRunner_ = runner;
    return;
}

void EventHandler::DeliveryTimeAction(const InnerEvent::Pointer &event, InnerEvent::TimePoint nowStart)
{
    if (!HiChecker::NeedCheckSlowEvent()) {
        return;
    }
    int64_t deliveryTimeout = eventRunner_->GetDeliveryTimeout();
    if (deliveryTimeout > 0) {
        std::string threadName = eventRunner_->GetRunnerThreadName();
        std::string eventName = GetEventName(event);
        int64_t threadId = gettid();
        std::string threadIdCharacter = std::to_string(threadId);
        std::chrono::duration<double> deliveryTime = nowStart - event->GetSendTime();
        std::string deliveryTimeCharacter = std::to_string((deliveryTime).count());
        std::string deliveryTimeoutCharacter = std::to_string(deliveryTimeout);
        std::string handOutTag = "threadId: " + threadIdCharacter + "," + "threadName: " + threadName + "," +
            "eventName: " + eventName + "," + "deliveryTime: " + deliveryTimeCharacter + "," +
            "deliveryTimeout: " + deliveryTimeoutCharacter;
        if ((nowStart - std::chrono::milliseconds(deliveryTimeout)) > event->GetHandleTime()) {
            HiChecker::NotifySlowEvent(handOutTag);
            if (deliveryTimeoutCallback_) {
                deliveryTimeoutCallback_();
            }
        }
    }
}

void EventHandler::DistributeTimeAction(const InnerEvent::Pointer &event, InnerEvent::TimePoint nowStart)
{
    if (!HiChecker::NeedCheckSlowEvent()) {
        return;
    }
    int64_t distributeTimeout = eventRunner_->GetDistributeTimeout();
    if (distributeTimeout > 0) {
        std::string threadName = eventRunner_->GetRunnerThreadName();
        std::string eventName = GetEventName(event);
        int64_t threadId = gettid();
        std::string threadIdCharacter = std::to_string(threadId);
        InnerEvent::TimePoint nowEnd = InnerEvent::Clock::now();
        std::chrono::duration<double> distributeTime = nowEnd - nowStart;
        std::string distributeTimeCharacter = std::to_string((distributeTime).count());
        std::string distributeTimeoutCharacter = std::to_string(distributeTimeout);
        std::string executeTag = "threadId: " + threadIdCharacter + "," + "threadName: " + threadName + "," +
            "eventName: " + eventName + "," + "distributeTime: " + distributeTimeCharacter + "," +
            "distributeTimeout: " + distributeTimeoutCharacter;
        if ((nowEnd - std::chrono::milliseconds(distributeTimeout)) > nowStart) {
            HiChecker::NotifySlowEvent(executeTag);
            if (distributeTimeoutCallback_) {
                distributeTimeoutCallback_();
            }
        }
    }
}

void EventHandler::DistributeEvent(const InnerEvent::Pointer &event)
{
    if (!event) {
        HILOGE("DistributeEvent: Could not distribute an invalid event");
        return;
    }

    // Save old event handler.
    std::weak_ptr<EventHandler> oldHandler = currentEventHandler;
    // Save current event handler into thread local data.
    currentEventHandler = shared_from_this();

    auto spanId = event->GetTraceId();
    auto traceId = HiTrace::GetId();
    bool allowTraceOutPut = AllowHiTraceOutPut(spanId, event->HasWaiter());
    if (allowTraceOutPut) {
        HiTrace::SetId(*spanId);
        HiTracePointerOutPut(spanId, event, "Receive", HiTraceTracepointType::HITRACE_TP_SR);
    }

    InnerEvent::TimePoint nowStart = InnerEvent::Clock::now();
    DeliveryTimeAction(event, nowStart);

    if (event->HasTask()) {
        // Call task callback directly if contains a task.
        (event->GetTaskCallback())();
    } else {
        // Otherwise let developers to handle it.
        ProcessEvent(event);
    }

    DistributeTimeAction(event, nowStart);

    if (allowTraceOutPut) {
        HiTrace::Tracepoint(HiTraceTracepointType::HITRACE_TP_SS, *spanId, "Event Distribute over");
        HiTrace::ClearId();
        if (traceId.IsValid()) {
            HiTrace::SetId(traceId);
        }
    }

    // Restore current event handler.
    if (oldHandler.expired()) {
        currentEventHandler = nullptr;
    } else {
        currentEventHandler = oldHandler;
    }
}

void EventHandler::Dump(Dumper &dumper)
{
    struct tm curTime = {0};

    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    localtime_r(&tt, &curTime);

    char sysTime[DATETIME_STRING_LENGTH];
    std::strftime(sysTime, sizeof(char) * DATETIME_STRING_LENGTH, "%Y%m%d %I:%M %p", &curTime);
    std::string message =
        dumper.GetTag() + " EventHandler dump begain curTime:" + std::string(sysTime) + LINE_SEPARATOR;
    dumper.Dump(message);

    if (eventRunner_ == nullptr) {
        dumper.Dump(dumper.GetTag() + " event runner uninitialized!" + LINE_SEPARATOR);
    } else {
        eventRunner_->Dump(dumper);
    }
}

bool EventHandler::HasInnerEvent(uint32_t innerEventId)
{
    if (!eventRunner_) {
        HILOGE("event runner uninitialized!");
        return false;
    }
    return eventRunner_->GetEventQueue()->HasInnerEvent(shared_from_this(), innerEventId);
}

bool EventHandler::HasInnerEvent(int64_t param)
{
    if (!eventRunner_) {
        HILOGE("event runner uninitialized!");
        return false;
    }
    return eventRunner_->GetEventQueue()->HasInnerEvent(shared_from_this(), param);
}

std::string EventHandler::GetEventName(const InnerEvent::Pointer &event)
{
    std::string eventName;
    if (!event) {
        return eventName;
    }

    if (event->HasTask()) {
        eventName = event->GetTaskName();
    } else {
        eventName = std::to_string(event->GetInnerEventId());
    }
    return eventName;
}

bool EventHandler::IsIdle()
{
    return eventRunner_->GetEventQueue()->IsIdle();
}

void EventHandler::ProcessEvent(const InnerEvent::Pointer &)
{}
}  // namespace AppExecFwk
}  // namespace OHOS