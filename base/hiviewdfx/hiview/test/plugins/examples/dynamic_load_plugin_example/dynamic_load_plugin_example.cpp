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

#include "dynamic_load_plugin_example.h"

#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER(DynamicLoadPluginExample);
using namespace std;

bool DynamicLoadPluginExample::OnEvent(std::shared_ptr<Event>& event)
{
    printf("DynamicLoadPluginExample OnEvent xxxx%p\n", event.get());
    event->SetValue("DynamicLoadPluginExample", "Done");
    return true;
}

void DynamicLoadPluginExample::OnLoad()
{
    SetVersion("DynamicLoadPluginExample1.0");
    printf("DynamicLoadPluginExample OnLoad \n");
    AddListenerInfo(Event::PLUGIN_MAINTENANCE);
    auto ptr = std::static_pointer_cast<DynamicLoadPluginExample>(shared_from_this());
    printf("register event listener %p \n", ptr.get());
    GetHiviewContext()->RegisterUnorderedEventListener(ptr);
    AddListenerInfo(OHOS::HiviewDFX::Event::MessageType::SYS_EVENT, "testbb");
    const int EVENT_ID_1 = 901000111;
    AddListenerInfo(OHOS::HiviewDFX::Event::MessageType::RAW_EVENT, EVENT_ID_1);
}

void DynamicLoadPluginExample::OnUnload()
{
    printf("DynamicLoadPluginExample OnUnload \n");
}

void DynamicLoadPluginExample::OnUnorderedEvent(const Event &msg)
{
    printf("DynamicLoadPluginExample OnUnorderedEvent.\n");
    auto cmd = msg.GetValue("DynamicLoadPluginExample");
    if (cmd == "Unload") {
        GetHiviewContext()->RequestUnloadPlugin(shared_from_this());
        printf("DynamicLoadPluginExample Request unload self.\n");
    }
    
    GetHiviewContext()->SetHiviewProperty("DPE_Listening", "received : " + msg.eventName_, true);
}

string DynamicLoadPluginExample::GetListenerName()
{
    printf("DynamicLoadPluginExample GetListenerName \n");
    return "DynamicLoadPluginExample";
}
} // namespace HiviewDFX
} // namespace OHOS
