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

#include "event_processor_example3.h"

#include "pipeline.h"
#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER_PROXY(EventProcessorExample3);
EventProcessorExample3::EventProcessorExample3()
{
    printf("EventProcessorExample3::EventProcessorExample3()\n");
    EventSourceExample::count.insert("EventProcessorExample3");
}

EventProcessorExample3::~EventProcessorExample3()
{
    printf("EventProcessorExample3::~EventProcessorExample3()\n");
    EventSourceExample::count.erase("EventProcessorExample3");
}

bool EventProcessorExample3::CanProcessEvent(std::shared_ptr<Event> event)
{
    printf("EventProcessorExample3 CanProcessEvent: %d.\n", event->eventId_);
    if (event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_CCC) {
        printf("EventProcessorExample3 CanProcessEvent true.\n");
        return true;
    }
    return false;
}

bool EventProcessorExample3::OnEvent(std::shared_ptr<Event>& event)
{
    printf("EventProcessorExample3 OnEvent 0 tid:%d sender:%s.\n", gettid(), event->sender_.c_str());
    if (event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_BBB) {
        printf("EventProcessorExample3 process bbb event.\n");
    }

    if (event->eventId_ == EventSourceExample::PIPELINE_EVENT_ID_AAA) {
        printf("EventProcessorExample3 process aaa event.\n");
    }

    if (GetHiviewContext() != nullptr) {
        GetHiviewContext()->SetHiviewProperty("EPE3_OnEvent", "received : " + event->eventName_, true);
    }
    event->SetValue("EventProcessorExample3", "Done");
    return true;
}

void EventProcessorExample3::OnLoad()
{
    SetVersion("EventProcessorExample3.0");
    printf("EventProcessorExample3 OnLoad\n");
}

void EventProcessorExample3::OnUnload()
{
    printf("EventProcessorExample3 OnUnload \n");
}
} // namespace HiviewDFX
} // namespace OHOS