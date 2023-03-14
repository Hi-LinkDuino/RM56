/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "event_logger.h"

#include "securec.h"

#include <sys/epoll.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common_utils.h"
#include "event_source.h"
#include "file_util.h"
#include "parameter_ex.h"
#include "plugin_factory.h"
#include "string_util.h"
#include "sys_event.h"
#include "sys_event_dao.h"

#include "event_log_action.h"
#include "event_logger_config.h"
namespace OHOS {
namespace HiviewDFX {
REGISTER(EventLogger);
DEFINE_LOG_LABEL(0xD002D01, "EventLogger");

bool EventLogger::OnEvent(std::shared_ptr<Event> &onEvent)
{
    if (onEvent == nullptr) {
        HIVIEW_LOGE("event == nullptr");
        return false;
    }

    auto sysEvent = Event::DownCastTo<SysEvent>(onEvent);
    HIVIEW_LOGI("event coming! id:0x%{public}x, eventName:%{public}s",
        sysEvent->eventId_, sysEvent->eventName_.c_str());
    HIVIEW_LOGI("event jsonExtraInfo is %{public}s", sysEvent->jsonExtraInfo_.c_str());

    EventLoggerConfig::EventLoggerConfigData configOut;
    auto logConfig = std::make_unique<EventLoggerConfig>();
    bool existence = logConfig->FindConfigLine(sysEvent->eventId_, sysEvent->eventName_, configOut);
    if (!existence) {
        HIVIEW_LOGE("event: id:0x%{public}x, eventName:%{public}s does not exist in the EventLoggerConfig",
            sysEvent->eventId_,  sysEvent->eventName_.c_str());
        PostEvent(sysEvent);
        return false;
    }

    sysEvent->eventName_ = configOut.name;
    sysEvent->SetValue("eventLog_action", configOut.action);
    sysEvent->SetValue("eventLog_interval", configOut.interval);
    StartLogCollect(sysEvent);

    PostEvent(sysEvent);
    return true;
}

void EventLogger::StartLogCollect(std::shared_ptr<SysEvent> event)
{
    HIVIEW_LOGI("event: id:0x%{public}x, eventName:%{public}s called.",
        event->eventId_,  event->eventName_.c_str());
    if (!JudgmentRateLimiting(event)) {
        return;
    }

    std::string idStr = event->eventName_.empty() ? std::to_string(event->eventId_) : event->eventName_;
    auto timeStr = std::to_string(event->happenTime_);
    uint64_t logTime = std::time(nullptr);
    std::string logFile = idStr + "-" + GetFormatTime(logTime) + ".log";

    int fd = logStore_->CreateLogFile(logFile);
    if (fd < 0) {
        HIVIEW_LOGE("create log file %{public}s failed, %{public}d", logFile.c_str(), fd);
        return;
    }
    WriteCommonHead(fd, event);
    auto eventLogAction = std::make_unique<EventLogAction>(fd, event);
    eventLogAction->Init();
    eventLogAction->CaptureAction();
    close(fd);
    UpdateDB(event, logFile);
}

bool EventLogger::PostEvent(std::shared_ptr<SysEvent> event)
{
    auto eventPtr = std::make_shared<SysEvent>(*(event.get()));
    GetHiviewContext()->PostUnorderedEvent(shared_from_this(), eventPtr);
    return true;
}

bool EventLogger::WriteCommonHead(int fd, std::shared_ptr<SysEvent> event)
{
    long pid = event->GetEventIntValue("PID");
    pid = pid ? pid : event->GetPid();
    long uid = event->GetEventIntValue("UID");
    uid = uid ? uid : event->GetUid();
    FileUtil::SaveStringToFd(fd, event->eventName_ + "\n");
    std::string str = "PID = " + std::to_string(pid);
    FileUtil::SaveStringToFd(fd, str + "\n");
    str = "UID = " + std::to_string(uid);
    FileUtil::SaveStringToFd(fd, str + "\n");
    event->GetEventValue("PACKAGE_NAME");
    event->GetEventValue("PROCESS_NAME");
    event->GetEventValue("PLATFORM");
    event->SetEventValue("MSG", StringUtil::ReplaceStr(event->GetEventValue("MSG"), "\\n", "\n"));
    event->GetEventValue("MSG");

    std::map<std::string, std::string> eventPairs = event->GetKeyValuePairs();
    HIVIEW_LOGD("KeyValuePairs num is %{public}d", eventPairs.size());
    for (auto tmp : eventPairs) {
        HIVIEW_LOGD("KeyValuePairs %{public}s , %{public}s", tmp.first.c_str(), tmp.second.c_str());
        std::string str = tmp.first + " = " + tmp.second + "\n";
        FileUtil::SaveStringToFd(fd, str);
    }
    return true;
}

bool EventLogger::JudgmentRateLimiting(std::shared_ptr<SysEvent> event)
{
    auto interval = event->GetIntValue("eventLog_interval");
    if (interval == 0) {
        return true;
    }

    long pid = event->GetEventIntValue("PID");
    pid = pid ? pid : event->GetPid();
    std::string eventName = event->eventName_;
    std::string eventPid = std::to_string(pid);

    std::time_t now = std::time(0);  
    for (auto it = eventTagTime_.begin(); it != eventTagTime_.end();) {
        if (it->first.find(eventName) != it->first.npos) {
            if ((now - it->second) >= interval) {
                it = eventTagTime_.erase(it);
                continue;
            }
        }
        ++it;
    }

    std::string tagTimeName = eventName + eventPid;
    auto it = eventTagTime_.find(tagTimeName);
    if (it != eventTagTime_.end()) {
        if ((now - it->second) < interval) {
            HIVIEW_LOGE("event: id:0x%{public}d, eventName:%{public}s pid:%{public}s. \
                interval:%{public}ld There's not enough interval",
                event->eventId_, eventName.c_str(), eventPid.c_str(), interval);
            return false;
        }
    }
    eventTagTime_[tagTimeName] = now;
    HIVIEW_LOGI("event: id:0x%{public}d, eventName:%{public}s pid:%{public}s. \
        interval:%{public}ld normal interval",
        event->eventId_, eventName.c_str(), eventPid.c_str(), interval);
    return true;
}

std::string EventLogger::GetFormatTime(uint64_t timestamp) const
{
    struct tm tm;
    time_t ts;
    /* 20: the length of 'YYYYmmddHHMMSS' */
    int strLen = 20;
    ts = timestamp;
    localtime_r(&ts, &tm);
    char buf[strLen];

    (void)memset_s(buf, strLen, 0, strLen);
    strftime(buf, strLen - 1, "%Y%m%d%H%M%S", &tm);
    return std::string(buf, strlen(buf));
}

bool EventLogger::UpdateDB(std::shared_ptr<SysEvent> event, std::string logFile)
{
    HIVIEW_LOGI("call");
    EventStore::SysEventQuery eventQuery = EventStore::SysEventDao::BuildQuery();
    EventStore::ResultSet set = eventQuery.Select( {EventStore::EventCol::TS} )
        .Where(EventStore::EventCol::TS, EventStore::Op::EQ, static_cast<int64_t>(event->happenTime_))
        .And(EventStore::EventCol::DOMAIN, EventStore::Op::EQ, event->domain_)
        .And(EventStore::EventCol::NAME, EventStore::Op::EQ, event->eventName_)
        .Execute();
    if (set.GetErrCode() != 0) {
        HIVIEW_LOGE("failed to get db, error:%{public}d.", set.GetErrCode());
        return false;
    }
    if (set.HasNext()) {
        auto record = set.Next();
        if (record->GetSeq() == event->GetSeq()) {
            HIVIEW_LOGI("Seq match success.");
            if (logFile == "nolog") {
                HIVIEW_LOGI("set info_ with nolog into db.");
                event->SetEventValue(EventStore::EventCol::INFO, "nolog", false);
            } else {
                auto logPath = R"~(logPath:)~" + LOGGER_EVENT_LOG_PATH  + "/" + logFile;
                event->SetEventValue(EventStore::EventCol::INFO, logPath, true);
            }

            auto retCode = EventStore::SysEventDao::Update(event, false);
            if (retCode == 0) {
                return true;
            }
        }
    }
    HIVIEW_LOGE("eventLog LogPath update to DB failed!");
    return false;
}

void EventLogger::OnLoad()
{
    HIVIEW_LOGI("EventLogger OnLoad.");
    SetName("EventLogger");
    SetVersion("1.0");
    logStore_->SetMaxSize(MAX_FOLDER_SIZE);
    logStore_->SetMinKeepingFileNumber(MAX_FILE_NUM);
    logStore_->Init();
    std::shared_ptr<EventLoop> tmp = GetWorkLoop();
    tmp->AddFileDescriptorEventCallback("EventLoggerFd",
        std::static_pointer_cast<EventLogger>(shared_from_this()));
}

void EventLogger::OnUnload()
{
    HIVIEW_LOGD("called");
}

bool EventLogger::CanProcessEvent(std::shared_ptr<Event> event)
{
    if (event == nullptr) {
        return false;
    }
    if (event->eventId_ > EVENT_MAX_ID) {
        return false;
    }
    return true;
}

void EventLogger::CreateAndPublishEvent(std::string& dirPath, std::string& fileName)
{
    HIVIEW_LOGD("called");
    std::shared_ptr<Plugin> sysEventSourcePlugin = GetHiviewContext()->GetPluginByName("SysEventSource");
    std::shared_ptr<EventSource> sysEventSource = std::static_pointer_cast<EventSource>(sysEventSourcePlugin);
    if (dirPath == MONITOR_STACK_LOG_PATH) {
        uint8_t count = 0;
        for (auto& i : MONITOR_STACK_FLIE_NAME) {
            if (fileName.find(i) != fileName.npos) {
                ++count;
                break;
            }
        }
        
        if (count == 0) {
            return;
        }

        SysEventCreator eventCreator("RELIABILITY", "STACK", SysEventCreator::FAULT);
        std::shared_ptr<SysEvent> event = std::make_shared<SysEvent>("eventLogger",
            static_cast<PipelineEventProducer *>(sysEventSource.get()), eventCreator);
        event->domain_ = "RELIABILITY";
        event->SetEventValue("domain_", "RELIABILITY");
        event->eventName_ = "STACK";
        event->SetEventValue("name_", "STACK");

        std::string logPath = dirPath + "/" + fileName;
        if (!FileUtil::FileExists(logPath)) {
            HIVIEW_LOGE("file %{public}s not exist", logPath.c_str());
        }
        std::string tmpStr = R"~(logPath:)~" + logPath;
        event->SetEventValue(EventStore::EventCol::INFO, tmpStr);
        sysEventSource->PublishPipelineEvent(event);
    }
}

bool EventLogger::OnFileDescriptorEvent(int fd, int type)
{
    HIVIEW_LOGD("fd:%{public}d, type:%{public}d, inotifyFd_:%{public}d.\n", fd, type, inotifyFd_);
    const int bufSize = 2048;
    char buffer[bufSize] = {0};
    char *offset = nullptr;
    struct inotify_event *event = nullptr;
    if (inotifyFd_ < 0) {
        HIVIEW_LOGE("Invalid inotify fd:%{public}d", inotifyFd_);
        return false;
    }
    int len = read(inotifyFd_, buffer, bufSize);
    if (len < 0) {
        HIVIEW_LOGE("failed to read event");
        return false;
    }

    offset = buffer;
    event = (struct inotify_event *)buffer;
    while ((reinterpret_cast<char *>(event) - buffer) < len) {
        const auto& it = fileMap_.find(event->wd);
        if (it == fileMap_.end()) {
            continue;
        }

        if (event->name[event->len - 1] != '\0') {
            event->name[event->len - 1] = '\0';
        }
        std::string fileName = std::string(event->name);
        HIVIEW_LOGD("fileName %{public}s event->mask 0x%{pubilc}x",fileName.c_str(), event->mask);
        
        CreateAndPublishEvent(it->second, fileName);

        int tmpLen = sizeof(struct inotify_event) + event->len;
        event = (struct inotify_event *)(offset + tmpLen);
        offset += tmpLen;
    }
    return true;
}

int32_t EventLogger::GetPollFd()
{
    HIVIEW_LOGD("call");
    if (inotifyFd_ > 0) {
        return inotifyFd_;
    }

    inotifyFd_ = inotify_init();
    if (inotifyFd_ == -1) {
        HIVIEW_LOGE("failed to init inotify: %s.\n", strerror(errno));
        return -1;
    }

    for (const std::string& i : MONITOR_LOG_PATH) {
        int wd = inotify_add_watch(inotifyFd_, i.c_str(), IN_CLOSE_WRITE | IN_MOVED_TO);
        if (wd < 0) {
            HIVIEW_LOGE("failed to add watch entry : %s(%s).\n", strerror(errno), i.c_str());
            continue;
        }
        fileMap_[wd] = i;
    }

    if (fileMap_.empty()) {
        close(inotifyFd_);
        inotifyFd_ = -1;
    }
    return inotifyFd_;
}

int32_t EventLogger::GetPollType()
{
    return EPOLLIN;
}
} // namesapce HiviewDFX
} // namespace OHOS
