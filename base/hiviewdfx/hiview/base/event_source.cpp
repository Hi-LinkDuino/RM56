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
#include "event_source.h"
#include "audit.h"
#include "logger.h"
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-EventSource");
bool EventSource::PublishPipelineEvent(std::shared_ptr<PipelineEvent> event)
{
    HIVIEW_LOGD("EventSource PublishPipelineEvent");
    if (event == nullptr) {
        return false;
    }

    if (Audit::IsEnabled()) {
        auto digest = GetPluginInfo() + Audit::DOMAIN_DELIMITER + event->GetEventInfo();
        Audit::WriteAuditEvent(Audit::StatsEvent::PIPELINE_EVENT_CREATE, event->createTime_, digest);
    }

    for (auto& pipeline : listeners_) {
        if ((pipeline != nullptr) && (pipeline->CanProcessEvent(event))) {
            // one event can only be processed by on pipeline
            pipeline->ProcessEvent(event);
            return true;
        }
    }
    return false;
}

void EventSource::AddPipeline(std::shared_ptr<Pipeline> pipeline)
{
    if (pipeline == nullptr) {
        HIVIEW_LOGW("Add nullptr pipeline to %{public}s.", GetName().c_str());
        return;
    }
    HIVIEW_LOGD("EventSource add pipeline %{public}s.", pipeline->GetName().c_str());
    listeners_.push_back(pipeline);
}

void EventSource::StartEventSource()
{
    // default non-implement
}
} // namespace HiviewDFX
} // namespace OHOS
