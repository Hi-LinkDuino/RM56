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

#include "event_processor_example5.h"

#include "event_source_example.h"
#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER_PROXY(EventProcessorExample5);
EventProcessorExample5::EventProcessorExample5()
{
    printf("EventProcessorExample5::EventProcessorExample5()\n");
    EventSourceExample::count.insert("EventProcessorExample5");
}

EventProcessorExample5::~EventProcessorExample5()
{
    printf("EventProcessorExample5::~EventProcessorExample5()\n");
    EventSourceExample::count.erase("EventProcessorExample5");
}

bool EventProcessorExample5::CanProcessEvent(std::shared_ptr<Event> event)
{
    printf("EventProcessorExample5 CanProcessEvent true.\n");
    return true;
}

bool EventProcessorExample5::OnEvent(std::shared_ptr<Event>& event)
{
    printf("EventProcessorExample5 cur ref:%ld \n", shared_from_this().use_count());
    return true;
}

void EventProcessorExample5::OnLoad()
{
    printf("EventProcessorExample5 OnLoad \n");
}

void EventProcessorExample5::OnUnload()
{
    printf("EventProcessorExample5 OnUnload \n.");
}
} // namespace HiviewDFX
} // namespace OHOS