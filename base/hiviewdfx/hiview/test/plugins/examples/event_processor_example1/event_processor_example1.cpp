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
#include "event_processor_example1.h"

#include "event_source_example.h"
#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER(EventProcessorExample1);
EventProcessorExample1::EventProcessorExample1()
{
    printf("EventProcessorExample1::EventProcessorExample1()\n");
    EventSourceExample::count.insert("EventProcessorExample1");
}

EventProcessorExample1::~EventProcessorExample1()
{
    printf("EventProcessorExample1::~EventProcessorExample1()\n");
    EventSourceExample::count.erase("EventProcessorExample1");
}

bool EventProcessorExample1::CanProcessEvent(std::shared_ptr<Event> event)
{
    printf("EventProcessorExample1 CanProcessEvent.\n");
    if (event == nullptr) {
        return false;
    }

    if (event->messageType_ == Event::MessageType::FAULT_EVENT &&
        event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_AAA) {
        printf("EventProcessorExample1 CanProcessEvent true.\n");
        return true;
    }
    if (event->messageType_ == Event::MessageType::FAULT_EVENT &&
        event->eventName_ == "testbb") {
        printf("EventProcessorExample1 CanProcessEvent true.\n");
        return true;
    }
    if (event->messageType_ == Event::MessageType::FAULT_EVENT &&
        event->eventName_ == "testcc") {
        printf("EventProcessorExample1 CanProcessEvent true.\n");
        return true;
    }
    printf("EventProcessorExample1 CanProcessEvent false.\n");
    return false;
}

bool EventProcessorExample1::OnEvent(std::shared_ptr<Event>& event)
{
    printf("EventProcessorExample1 OnEvent, tid:%d\n", gettid());
    if (GetHiviewContext() != nullptr) {
        GetHiviewContext()->SetHiviewProperty("EPE1_OnEvent", "received : " + event->eventName_, true);
    }
    processedEventCount_++;
    HandleEvent(event);
    return true;
}

void EventProcessorExample1::HandleEvent(std::shared_ptr<Event>& event)
{
    if (event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_BBB) {
        printf("EventProcessorExample1 process bbb event.\n");
        event->SetValue("Done", GetName());
        if (GetHiviewContext() != nullptr) {
            printf("EventProcessorExample1 PostUnorderedEvent bbb\n ");
            GetHiviewContext()->PostUnorderedEvent(shared_from_this(), event);
        }
    }

    if (event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_AAA) {
        printf("EventProcessorExample1 process aaa event.\n");
    }
    event->SetValue("EventProcessorExample1", "Done");
}

void EventProcessorExample1::OnLoad()
{
    SetVersion("EventProcessorExample1.0");
    printf("EventProcessorExample1 OnLoad \n");
}

void EventProcessorExample1::OnUnload()
{
    printf("EventProcessorExample1 OnUnload \n");
}
} // namespace HiviewDFX
} // namespace OHOS
