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

#include "event_processor_example4.h"

#include "event.h"
#include "event_source_example.h"
#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER_PROXY_WITH_LOADED(EventProcessorExample4);
EventProcessorExample4::EventProcessorExample4()
{
    printf("EventProcessorExample4::EventProcessorExample4()\n");
    EventSourceExample::count.insert("EventProcessorExample4");
}

EventProcessorExample4::~EventProcessorExample4()
{
    printf("EventProcessorExample4::~EventProcessorExample4()\n");
    EventSourceExample::count.erase("EventProcessorExample4");
}

bool EventProcessorExample4::CanProcessEvent(std::shared_ptr<Event> event)
{
    if (event->messageType_ == Event::MessageType::FAULT_EVENT &&
        event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_BBB) {
        printf("EventProcessorExample4 CanProcessEvent true.\n");
        return true;
    }
    if (event->messageType_ == Event::MessageType::FAULT_EVENT &&
        event->eventName_ == "testbb") {
        printf("EventProcessorExample4 CanProcessEvent true.\n");
        return true;
    }
    return false;
}

bool EventProcessorExample4::OnEvent(std::shared_ptr<Event>& event)
{
    printf("EventProcessorExample4 cur ref:%ld \n", shared_from_this().use_count());
    printf("EventProcessorExample4 OnEvent 0 tid:%d sender:%s.\n", gettid(), event->sender_.c_str());
    if (event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_BBB) {
        printf("EventProcessorExample4 process bbb event.\n");
    }

    if (event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_AAA) {
        printf("EventProcessorExample4 process aaa event.\n");
        event->SetValue("Done", GetName());
        if (GetHiviewContext() != nullptr) {
            printf("EventProcessorExample4 PostUnorderedEvent aaa\n ");
            GetHiviewContext()->PostUnorderedEvent(shared_from_this(), event);
            if (!event->GetValue("Unload").empty()) {
                GetHiviewContext()->RequestUnloadPlugin(shared_from_this());
                printf("EventProcessorExample4 RequestUnloadPlugin aaa\n ");
            }
        }
    }

    if (event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_CCC) {
        printf("EventProcessorExample4 process ccc event.\n");
        event->SetValue("Done", GetName());
        if (GetHiviewContext() != nullptr) {
            printf("EventProcessorExample4 PostUnorderedEvent ccc\n ");
            GetHiviewContext()->PostUnorderedEvent(shared_from_this(), Event::Repack<Event, Event>(event, false));
        }
    }
    if (GetHiviewContext() != nullptr) {
        GetHiviewContext()->SetHiviewProperty("EPE4_OnEvent", "received : " + event->eventName_, true);
    }
    event->SetValue("EventProcessorExample4", "Done");
    return true;
}
void EventProcessorExample4::OnLoad()
{
    SetVersion("EventProcessorExample4.0");
    printf("EventProcessorExample4 OnLoad \n");
    auto ptr = std::static_pointer_cast<EventProcessorExample4>(shared_from_this());
    GetHiviewContext()->RegisterDynamicListenerInfo(ptr);
    AddEventListenerInfo(OHOS::HiviewDFX::Event::MessageType::SYS_EVENT, "testaa");
    AddEventListenerInfo(OHOS::HiviewDFX::Event::MessageType::SYS_EVENT, "testbb");
    AddEventListenerInfo(OHOS::HiviewDFX::Event::MessageType::FAULT_EVENT, EventSourceExample::PIPELINE_EVENT_ID_AAA);
}

void EventProcessorExample4::OnUnload()
{
    printf("EventProcessorExample4 OnUnload \n.");
}

void EventProcessorExample4::OnEventListeningCallback(const Event &msg)
{
    printf("EventProcessorExample4 OnEventListeningCallback.\n");
    GetHiviewContext()->SetHiviewProperty("EPE4_Listening", "received : " + msg.eventName_, true);
}
} // namespace HiviewDFX
} // namespace OHOS