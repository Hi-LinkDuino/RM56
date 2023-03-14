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

#ifndef FREEZE_DETECTOR_PLUGIN_H
#define FREEZE_DETECTOR_PLUGIN_H

#include <memory>

#include "event.h"
#include "event_loop.h"
#include "event_source.h"
#include "watch_point.h"

namespace OHOS {
namespace HiviewDFX {
const std::string FREEZE_DETECTOR_PLUGIN_NAME = "FreezeDetector";
const std::string FREEZE_DETECTOR_PLUGIN_VERSION = "1.0";

class FreezeDetectorPlugin : public EventListener, public EventSource {
public:
    static const inline std::string COLON = ":";
    static const inline std::string EVENT_DOMAIN = "DOMAIN";
    static const inline std::string EVENT_STRINGID = "STRINGID";
    static const inline std::string EVENT_TIMESTAMP = "TIMESTAMP";
    static const inline std::string EVENT_PID = "PID";
    static const inline std::string EVENT_TID = "TID";
    static const inline std::string EVENT_UID = "UID";
    static const inline std::string EVENT_PACKAGE_NAME = "PACKAGE_NAME";
    static const inline std::string EVENT_PROCESS_NAME = "PROCESS_NAME";
    static const inline std::string EVENT_MSG = "MSG";
    static const inline std::string SYSEVENT_PIPELINE = "SysEventPipeline";

    FreezeDetectorPlugin() : threadLoop_(nullptr) {};
    ~FreezeDetectorPlugin() {};
    bool ReadyToLoad() override;
    bool OnEvent(std::shared_ptr<Event> &event) override;
    void OnLoad() override;
    void OnUnload() override;
    bool CanProcessEvent(std::shared_ptr<Event> event) override;
    void OnUnorderedEvent(const Event& msg) override;
    std::string GetListenerName() override;
    void Recycle(PipelineEvent* event) override {};
    void PauseDispatch(std::weak_ptr<Plugin> plugin) override {};

private:
    std::string RemoveRedundantNewline(const std::string& content);
    WatchPoint MakeWatchPoint(const Event& event);
    void ProcessEvent(WatchPoint watchPoint);
    void AddFreezeListener();
    std::shared_ptr<EventLoop> threadLoop_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_PLUGIN_FREEZE_DETECTOR_H
