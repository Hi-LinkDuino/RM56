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

#ifndef HIVIEW_HICOLLIE_PLUGIN_H
#define HIVIEW_HICOLLIE_PLUGIN_H

#include "event_source.h"
#include "plugin.h"
#include "sys_event.h"

namespace OHOS {
namespace HiviewDFX {
class HiCollieCollector : public EventListener, public EventSource {
public:
    bool ReadyToLoad() override;
    void OnLoad() override;
    void OnUnload() override;
    bool OnEvent(std::shared_ptr<Event> &event) override;
    void OnUnorderedEvent(const Event &event) override;
    bool CanProcessEvent(std::shared_ptr<Event> event) override;
    void Recycle(PipelineEvent* event) override {};
    void PauseDispatch(std::weak_ptr<Plugin> plugin) override {};
    std::string GetListenerName() override;

private:
    static const inline std::string FAULT_LOG_PATH = "/data/log/faultlog/";
    static const inline std::string STRINGID_SERVICE_TIMEOUT = "SERVICE_TIMEOUT"; // timer
    static const inline std::string STRINGID_SERVICE_BLOCK = "SERVICE_BLOCK"; // watchdog
    static const inline std::string EVENT_PID = "PID";
    static const inline std::string EVENT_TGID = "TGID";
    static const inline std::string EVENT_MSG = "MSG";

    void ProcessHiCollieEvent(SysEvent &sysEvent);
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_HICOLLIE_PLUGIN_H
