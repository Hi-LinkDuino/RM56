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

#ifndef SYS_EVENT_SOURCE_H
#define SYS_EVENT_SOURCE_H

#include <memory>
#include <string>
#include <vector>

#include "event_server.h"
#include "event_source.h"
#include "pipeline.h"
#include "plugin.h"

namespace OHOS {
namespace HiviewDFX {
class SysEventParser {
public:
    explicit SysEventParser(PipelineEventProducer* producer): pipeProducer(producer) {};
    ~SysEventParser() {};
    std::shared_ptr<PipelineEvent> Parser(const std::string& rawMsg) const;
private:
    PipelineEventProducer* pipeProducer;
};

class SysEventReceiver : public EventReceiver {
public:
    explicit SysEventReceiver(EventSource& source): eventSource(source) {};
    ~SysEventReceiver() override {};
    void HandlerEvent(const std::string& rawMsg) override;
private:
    EventSource& eventSource;
};

class SysEventSource : public EventSource {
public:
    SysEventSource() {};
    ~SysEventSource() {};
    void OnLoad() override;
    void OnUnload() override;
    void StartEventSource() override;
    void Recycle(PipelineEvent *event) override;
    void PauseDispatch(std::weak_ptr<Plugin> plugin) override;

private:
    EventServer eventServer;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // SYS_EVENT_SOURCE_H