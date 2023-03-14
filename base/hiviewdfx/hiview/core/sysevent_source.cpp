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

#include "sysevent_source.h"

#include <memory>

#include "defines.h"
#include "logger.h"
#include "plugin_factory.h"
#include "sys_event.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER(SysEventSource);
DEFINE_LOG_TAG("HiView-SysEventSource");

std::shared_ptr<PipelineEvent> SysEventParser::Parser(const std::string& rawMsg) const
{
    HIVIEW_LOGD("parser raw message size=%{public}d, %{public}s", rawMsg.length(), rawMsg.c_str());
    auto baseEvent = std::make_shared<SysEvent>("SysEventSource", pipeProducer, rawMsg);
    if (baseEvent->PaserJson() != 0) {
        HIVIEW_LOGI("parser sys event error");
        return nullptr;
    }
    HIVIEW_LOGI("parser result domain_=%{public}s eventName_=%{public}s",
        baseEvent->domain_.c_str(), baseEvent->eventName_.c_str());
    return baseEvent;
}

void SysEventReceiver::HandlerEvent(const std::string& rawMsg)
{
    SysEventParser sysEventParser(static_cast<PipelineEventProducer *>(&eventSource));
    std::shared_ptr<PipelineEvent> event = sysEventParser.Parser(rawMsg);
    eventSource.PublishPipelineEvent(event);
}

void SysEventSource::OnLoad()
{
    HIVIEW_LOGI("SysEventSource load ");
}

void SysEventSource::OnUnload()
{
    eventServer.Stop();
    HIVIEW_LOGI("SysEventSource unload");
}

void SysEventSource::StartEventSource()
{
    HIVIEW_LOGI("SysEventSource start");
    std::shared_ptr<EventReceiver> sysEventReceiver = std::make_shared<SysEventReceiver>(*this);
    eventServer.AddReceiver(sysEventReceiver);
    eventServer.Start();
}

void SysEventSource::Recycle(PipelineEvent *event __UNUSED)
{
    HIVIEW_LOGI("recycle resource");
}

void SysEventSource::PauseDispatch(std::weak_ptr<Plugin> plugin)
{
    auto requester = plugin.lock();
    if (requester != nullptr) {
        HIVIEW_LOGI("process pause dispatch event from plugin:%s.\n", requester->GetName().c_str());
    }
}
} // namespace HiviewDFX
} // namespace OHOS