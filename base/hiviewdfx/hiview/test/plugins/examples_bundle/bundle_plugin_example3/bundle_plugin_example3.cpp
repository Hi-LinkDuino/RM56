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

#include "bundle_plugin_example3.h"

#include "bundle_event_source_example.h"
#include "pipeline.h"
#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER_PROXY_WITH_LOADED(BundlePluginExample3);
BundlePluginExample3::BundlePluginExample3()
{
    printf("BundlePluginExample3::BundlePluginExample3()\n");
    BundleEventSourceExample::count.insert("BundlePluginExample3");
}

BundlePluginExample3::~BundlePluginExample3()
{
    printf("BundlePluginExample3::~BundlePluginExample3()\n");
    BundleEventSourceExample::count.erase("BundlePluginExample3");
}

bool BundlePluginExample3::CanProcessEvent(std::shared_ptr<Event> event)
{
    return true;
}

bool BundlePluginExample3::OnEvent(std::shared_ptr<Event>& event)
{
    printf("BundlePluginExample3 OnEvent 0 tid:%d sender:%s.\n", gettid(), event->sender_.c_str());
    event->SetValue("BundlePluginExample3", "Done");
    return true;
}

void BundlePluginExample3::OnLoad()
{
    SetVersion("BundlePluginExample3.0");
    printf("BundlePluginExample3 OnLoad\n");
    auto ptr = std::static_pointer_cast<BundlePluginExample3>(shared_from_this());
    GetHiviewContext()->RegisterDynamicListenerInfo(ptr);
    AddEventListenerInfo(OHOS::HiviewDFX::Event::MessageType::SYS_EVENT, "testaa");
    AddEventListenerInfo(OHOS::HiviewDFX::Event::MessageType::SYS_EVENT, "testbb");
    const int EVENT_ID_1 = 901000111;
    AddEventListenerInfo(OHOS::HiviewDFX::Event::MessageType::RAW_EVENT, EVENT_ID_1);
}

void BundlePluginExample3::OnUnload()
{
    printf("BundlePluginExample3 OnUnload \n");
}

void BundlePluginExample3::OnEventListeningCallback(const Event &msg)
{
    printf("BundlePluginExample3 OnEventListeningCallback \n");
    GetHiviewContext()->SetHiviewProperty("BE3_Listening", "received : " + msg.eventName_, true);
}
} // namespace HiviewDFX
} // namespace OHOS