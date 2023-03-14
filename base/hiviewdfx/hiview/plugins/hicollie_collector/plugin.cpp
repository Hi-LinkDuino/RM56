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

#include "plugin.h"

#include <regex>

#include "file_util.h"
#include "hisysevent.h"
#include "logger.h"
#include "plugin_factory.h"
#include "sys_event_dao.h"

namespace OHOS {
namespace HiviewDFX {
REGISTER(HiCollieCollector);
DEFINE_LOG_TAG("HiCollieCollector");

std::string HiCollieCollector::GetListenerName()
{
    return name_;
}

bool HiCollieCollector::ReadyToLoad()
{
    return true;
}

bool HiCollieCollector::CanProcessEvent(std::shared_ptr<Event> event)
{   
    return false;
}

void HiCollieCollector::OnLoad()
{
    SetName("HiCollieCollector");
    SetVersion("HiCollieCollector 1.0");
    HIVIEW_LOGI("OnLoad.");
    AddListenerInfo(Event::MessageType::SYS_EVENT, STRINGID_SERVICE_TIMEOUT);
    AddListenerInfo(Event::MessageType::SYS_EVENT, STRINGID_SERVICE_BLOCK);
    GetHiviewContext()->RegisterUnorderedEventListener(
        std::static_pointer_cast<HiCollieCollector>(shared_from_this()));
}

void HiCollieCollector::OnUnload()
{
    HIVIEW_LOGI("OnUnload.");
}

bool HiCollieCollector::OnEvent(std::shared_ptr<Event> &event)
{
    return true;
}

void HiCollieCollector::OnUnorderedEvent(const Event &event)
{
    HIVIEW_LOGI("received event domain=%{public}s, stringid=%{public}s.\n",
        event.domain_.c_str(), event.eventName_.c_str());
    if (GetHiviewContext() == nullptr) {
        HIVIEW_LOGE("failed to get context.");
        return;
    }

    if (event.eventName_ != STRINGID_SERVICE_TIMEOUT && event.eventName_ != STRINGID_SERVICE_BLOCK) {
        HIVIEW_LOGE("invalid stringid=%{public}s.\n", event.eventName_.c_str());
        return;
    }

    Event& eventRef = const_cast<Event&>(event);
    SysEvent& sysEvent = static_cast<SysEvent&>(eventRef);
    ProcessHiCollieEvent(sysEvent);
}

void HiCollieCollector::ProcessHiCollieEvent(SysEvent &sysEvent)
{
    std::string path = "";
    std::string info = sysEvent.GetEventValue(EventStore::EventCol::INFO);
    std::regex reg("logPath:([^,]+)");
    std::smatch result;
    if (std::regex_search(info, result, reg)) {
        path = result[1].str();
        std::string desPath = FAULT_LOG_PATH + FileUtil::ExtractFileName(path);
        int fileResult = FileUtil::CopyFile(path, desPath);
        if (fileResult != 0) {
            HIVIEW_LOGE("failed to copy file from %{public}s to %{public}s.\n", path.c_str(), desPath.c_str());
        }
    }

    std::vector<std::string> paths = {path};
    HiSysEvent::Write("RELIABILITY", sysEvent.eventName_ + "_REPORT", HiSysEvent::FAULT,
        "HIVIEW_LOG_FILE_PATHS", paths,
        "PID", sysEvent.GetEventValue(EVENT_PID),
        "TGID", sysEvent.GetEventValue(EVENT_TGID),
        "MSG", sysEvent.GetEventValue(EVENT_MSG));
    HIVIEW_LOGI("send event [%{public}s, %{public}s] msg:%{public}s path:%{public}s",
        "RELIABILITY", sysEvent.eventName_.c_str(), sysEvent.GetEventValue(EVENT_MSG).c_str(), path.c_str());
}
} // namespace HiviewDFX
} // namespace OHOS
