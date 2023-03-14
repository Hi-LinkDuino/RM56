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

#include "bundle_plugin_example2.h"

#include "bundle_event_source_example.h"
#include "pipeline.h"
#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER_PROXY_WITH_LOADED(BundlePluginExample2);
BundlePluginExample2::BundlePluginExample2()
{
    printf("BundlePluginExample2::BundlePluginExample2()\n");
    BundleEventSourceExample::count.insert("BundlePluginExample2");
}

BundlePluginExample2::~BundlePluginExample2()
{
    printf("BundlePluginExample2::~BundlePluginExample2()\n");
    BundleEventSourceExample::count.erase("BundlePluginExample2");
}

bool BundlePluginExample2::CanProcessEvent(std::shared_ptr<Event> event)
{
    return true;
}

bool BundlePluginExample2::OnEvent(std::shared_ptr<Event>& event)
{
    printf("EventProcessorExample3 OnEvent 0 tid:%d sender:%s.\n", gettid(), event->sender_.c_str());
    GetHiviewContext()->SetHiviewProperty("BE2_OnEvent", "received : " + event->eventName_, true);
    return true;
}
void BundlePluginExample2::OnLoad()
{
    SetVersion("BundlePluginExample2.0");
    printf("BundlePluginExample2 OnLoad \n");
    GetHiviewContext()->AppendPluginToPipeline(GetName(), "NormalPipeline");
}

void BundlePluginExample2::OnUnload()
{
    printf("BundlePluginExample2 OnUnload \n");
}
} // namespace HiviewDFX
} // namespace OHOS