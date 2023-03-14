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
#include "bundle_plugin_example1.h"

#include "bundle_event_source_example.h"
#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER_PROXY(BundlePluginExample1);
BundlePluginExample1::BundlePluginExample1()
{
    printf("BundlePluginExample1::BundlePluginExample1()\n");
    BundleEventSourceExample::count.insert("BundlePluginExample1");
}

BundlePluginExample1::~BundlePluginExample1()
{
    printf("BundlePluginExample1::~BundlePluginExample1()\n");
    BundleEventSourceExample::count.erase("BundlePluginExample1");
}

bool BundlePluginExample1::CanProcessEvent(std::shared_ptr<Event> event)
{
    printf("BundlePluginExample1 CanProcessEvent.\n");
    return true;
}

bool BundlePluginExample1::OnEvent(std::shared_ptr<Event>& event)
{
    printf("BundlePluginExample1 OnEvent, tid:%d\n", gettid());
    GetHiviewContext()->SetHiviewProperty("BE1_OnEvent", "received : " + event->eventName_, true);
    return true;
}

void BundlePluginExample1::OnLoad()
{
    SetVersion("BundlePluginExample1.0");
    printf("BundlePluginExample1 OnLoad \n");
}

void BundlePluginExample1::OnUnload()
{
    printf("BundlePluginExample1 OnUnload \n");
}
} // namespace HiviewDFX
} // namespace OHOS
