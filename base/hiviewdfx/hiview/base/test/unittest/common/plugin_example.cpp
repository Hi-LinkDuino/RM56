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
#include "plugin_example.h"

#include "event_loop.h"
#include "plugin_factory.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER(PluginExample);
bool PluginExample::OnEvent(std::shared_ptr<Event>& event)
{
    if (event != nullptr && event->IsPipelineEvent()) {
            printf(" Repack in plugin Example.\n");
            Event::Repack<PluginExampleEvent, PipelineEvent>(event);
            return true;
    }
    return false;
};

void PluginExample::OnLoad()
{
    printf(" PluginExample OnLoad.\n");
    auto looper = std::make_shared<EventLoop>("PluginExample");
    if (looper != nullptr) {
        looper->StartLoop();
        BindWorkLoop(looper);
        printf(" PluginExample BindWorkLoop.\n");
    }
}
} // namespace HiviewDFX
} // namespace OHOS
