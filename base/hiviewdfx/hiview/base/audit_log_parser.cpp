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

#include "audit_log_parser.h"
#include "audit.h"
#include "string_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace {
constexpr int LOG_ELEMENT_MIN_COUNT = 3;
constexpr int LOG_ELEMENT_TIMESTAMP_POS = 0;
constexpr int LOG_ELEMENT_SERIAL_ID_POS = 1;
constexpr int LOG_ELEMENT_EVENT_TYPE_POS = 2;
constexpr int LOG_ELEMENT_NORMAL_EVENT_SENDER_POS = 3;
constexpr int LOG_ELEMENT_NORMAL_EVENT_PROCESSOR_POS = 4;
constexpr int LOG_ELEMENT_NORMAL_EVENT_THREAD_POS = 5;
constexpr int LOG_ELEMENT_NORMAL_EVENT_DIGEST_POS = 6;
constexpr int LOG_ELEMENT_PIPE_EVENT_CREATOR_POS = 3;
constexpr int LOG_ELEMENT_PIPE_EVENT_DIGEST_POS = 4;
constexpr int LOG_ELEMENT_PIPE_EVENT_PROCESSOR_POS = 3;
constexpr int LOG_ELEMENT_PIPE_EVENT_THREAD_POS = 3;
constexpr int LOG_ELEMENT_PIPE_EVENT_PIPELINE_POS = 3;
constexpr int QUEUE_EVENT_IN_ELEMENT_SIZE = 7;
constexpr int QUEUE_EVENT_OUT_ELEMENT_SIZE = 3;
constexpr int PIPELINE_EVENT_CREATE_ELEMENT_SIZE = 5;
constexpr int PIPELINE_EVENT_HANDLE_IN_ELEMENT_SIZE = 4;
constexpr int PIPELINE_EVENT_HANDLE_OUT_ELEMENT_SIZE = 4;
constexpr int PIPELINE_EVENT_DONE_ELEMENT_SIZE = 4;

AuditLogParser::EventInfo CreateEventInfo(uint64_t serialId, const std::string& digest,
                                          const std::vector<std::string>& eventElements)
{
    AuditLogParser::EventInfo info;
    info.isPipelineEvent = true;
    info.eventSerialId = serialId;
    StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_TIMESTAMP_POS], info.inTime);
    info.processor = eventElements[LOG_ELEMENT_PIPE_EVENT_PROCESSOR_POS];
    info.digest = digest;
    return info;
}

std::vector<std::string> SplitStr(const std::string& log)
{
    std::vector<std::string> elements;
    std::string sep(1, Audit::DOMAIN_DELIMITER);
    StringUtil::SplitStr(log, sep, elements);
    return elements;
}
} // namespace

void AuditLogParser::StartParse()
{
    std::list<std::string> logList;
    if (!Audit::GetAuditLog(true, logList)) {
        return;
    }

    ParseTimeScope(logList);
    for (auto& log : logList) {
        auto elements = SplitStr(log);
        if (elements.size() < LOG_ELEMENT_MIN_COUNT) {
            continue;
        }

        int eventType = -1;
        StringUtil::ConvertStringTo<int>(elements[LOG_ELEMENT_EVENT_TYPE_POS], eventType);
        if (std::to_string(eventType) != elements[LOG_ELEMENT_EVENT_TYPE_POS]) {
            eventType = -1;
        }
        if (eventType < Audit::StatsEvent::QUEUE_EVENT_IN) {
            continue;
        }

        if (eventType <= Audit::StatsEvent::QUEUE_EVENT_OUT) {
            ParseNormalAuditEvent(eventType, elements);
        } else if (eventType <= Audit::StatsEvent::PIPELINE_EVENT_DONE) {
            ParsePipelineAuditEvent(eventType, elements);
        }
    }
}

std::list<std::string> AuditLogParser::GetThreadSummary(const std::string& name)
{
    std::list<std::string> ret;
    std::string pid = name;
    auto pos = pid.find("@");
    if (pos != std::string::npos) {
        pid = pid.substr(pos + 1);
    }

    for (auto& event : eventList_) {
        if (event.threadNameOrTid.find(pid) != std::string::npos) {
            ret.push_back(event.ToString());
        }
    }

    for (auto& pipeEvent : pipelineEventList_) {
        for (auto& chainEvent : pipeEvent.processChain) {
            if (chainEvent.threadNameOrTid.find(pid) != std::string::npos) {
                ret.push_back(chainEvent.ToString());
            }
        }
    }
    return ret;
}

std::list<std::string> AuditLogParser::GetPluginSummary(const std::string& name)
{
    std::list<std::string> ret;
    for (auto& event : eventList_) {
        if (event.processor.find(name) != std::string::npos) {
            ret.push_back(event.ToString());
        }
    }

    for (auto& pipeEvent : pipelineEventList_) {
        for (auto& chainEvent : pipeEvent.processChain) {
            if (chainEvent.processor.find(name) != std::string::npos) {
                ret.push_back(chainEvent.ToString());
            }
        }
    }
    return ret;
}

std::list<std::string> AuditLogParser::GetPipelineSummary(const std::string& name)
{
    std::list<std::string> ret;
    for (auto& pipeEvent : pipelineEventList_) {
        if (pipeEvent.pipeline.find(name) != std::string::npos) {
            ret.push_back(pipeEvent.ToString());
        }
    }
    return ret;
}

std::string AuditLogParser::GetAuditLogTimeScope()
{
    return "Audit Logs From " + std::to_string(logStartTime_) + " to " + std::to_string(logEndTime_);
}

void AuditLogParser::ParseTimeScope(const std::list<std::string>& logList)
{
    if (logList.empty()) {
        return;
    }

    auto beginLog = SplitStr(*(logList.begin()));
    StringUtil::ConvertStringTo<uint64_t>(beginLog[LOG_ELEMENT_TIMESTAMP_POS], logStartTime_);
    auto endLog = SplitStr(*(logList.rbegin()));
    StringUtil::ConvertStringTo<uint64_t>(endLog[LOG_ELEMENT_TIMESTAMP_POS], logEndTime_);
}

void AuditLogParser::ParseNormalAuditEvent(int eventType, const std::vector<std::string>& eventElements)
{
    if (eventType == Audit::StatsEvent::QUEUE_EVENT_IN && eventElements.size() >= QUEUE_EVENT_IN_ELEMENT_SIZE) {
        ParseNormalEnqueueEvent(eventElements);
        return;
    }

    if (eventType == Audit::StatsEvent::QUEUE_EVENT_OUT && eventElements.size() >= QUEUE_EVENT_OUT_ELEMENT_SIZE) {
        ParseNormalDequeueEvent(eventElements);
    }
}

void AuditLogParser::ParseNormalEnqueueEvent(const std::vector<std::string>& eventElements)
{
    EventInfo info;
    StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_TIMESTAMP_POS], info.inTime);
    StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_SERIAL_ID_POS], info.eventSerialId);
    info.sender = eventElements[LOG_ELEMENT_NORMAL_EVENT_SENDER_POS];
    info.processor = eventElements[LOG_ELEMENT_NORMAL_EVENT_PROCESSOR_POS];
    info.threadNameOrTid = eventElements[LOG_ELEMENT_NORMAL_EVENT_THREAD_POS];
    info.digest = eventElements[LOG_ELEMENT_NORMAL_EVENT_DIGEST_POS];
    eventList_.push_back(std::move(info));
}

void AuditLogParser::ParseNormalDequeueEvent(const std::vector<std::string>& eventElements)
{
    uint64_t eventSerialId = 0;
    StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_SERIAL_ID_POS], eventSerialId);
    for (auto rit = eventList_.rbegin(); rit != eventList_.rend(); ++rit) {
        if (rit->eventSerialId == eventSerialId) {
            StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_TIMESTAMP_POS], rit->outTime);
            return;
        }
    }
}

void AuditLogParser::ParsePipelineAuditEvent(int eventType, const std::vector<std::string>& eventElements)
{
    if (eventType == Audit::StatsEvent::PIPELINE_EVENT_CREATE &&
        eventElements.size() >= PIPELINE_EVENT_CREATE_ELEMENT_SIZE) {
        ParsePipelineCreateEvent(eventElements);
        return;
    }

    if (eventType == Audit::StatsEvent::PIPELINE_EVENT_HANDLE_IN &&
        eventElements.size() >= PIPELINE_EVENT_HANDLE_IN_ELEMENT_SIZE) {
        ParsePipelineHandleInEvent(eventElements);
        return;
    }

    if (eventType == Audit::StatsEvent::PIPELINE_EVENT_HANDLE_OUT &&
        eventElements.size() >= PIPELINE_EVENT_HANDLE_OUT_ELEMENT_SIZE) {
        ParsePipelineHandleOutEvent(eventElements);
        return;
    }

    if (eventType == Audit::StatsEvent::PIPELINE_EVENT_DONE &&
        eventElements.size() >= PIPELINE_EVENT_DONE_ELEMENT_SIZE) {
        ParsePipelineDoneEvent(eventElements);
        return;
    }
}

void AuditLogParser::ParsePipelineCreateEvent(const std::vector<std::string>& eventElements)
{
    PipelineEventInfo info;
    StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_TIMESTAMP_POS], info.createTime);
    StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_SERIAL_ID_POS], info.eventSerialId);
    info.creator = eventElements[LOG_ELEMENT_PIPE_EVENT_CREATOR_POS];
    info.digest = eventElements[LOG_ELEMENT_PIPE_EVENT_DIGEST_POS];
    pipelineEventList_.push_back(std::move(info));
}

void AuditLogParser::ParsePipelineHandleInEvent(const std::vector<std::string>& eventElements)
{
    uint64_t eventSerialId = 0;
    StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_SERIAL_ID_POS], eventSerialId);
    for (auto rit = pipelineEventList_.rbegin(); rit != pipelineEventList_.rend(); ++rit) {
        if (rit->eventSerialId == eventSerialId) {
            rit->processChain.push_back(CreateEventInfo(eventSerialId, rit->digest, eventElements));
            return;
        }
    }

    for (auto rit = eventList_.rbegin(); rit != eventList_.rend(); ++rit) {
        if (rit->eventSerialId == eventSerialId) {
            PipelineEventInfo info(*rit);
            info.processChain.push_back(CreateEventInfo(eventSerialId, info.digest, eventElements));
            pipelineEventList_.push_back(std::move(info));
            return;
        }
    }
}

void AuditLogParser::ParsePipelineHandleOutEvent(const std::vector<std::string>& eventElements)
{
    uint64_t eventSerialId = 0;
    StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_SERIAL_ID_POS], eventSerialId);
    for (auto rit = pipelineEventList_.rbegin(); rit != pipelineEventList_.rend(); ++rit) {
        if (rit->eventSerialId == eventSerialId) {
            auto& info = rit->processChain.back();
            StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_TIMESTAMP_POS], info.outTime);
            info.threadNameOrTid = eventElements[LOG_ELEMENT_PIPE_EVENT_THREAD_POS];
            return;
        }
    }
}

void AuditLogParser::ParsePipelineDoneEvent(const std::vector<std::string>& eventElements)
{
    uint64_t eventSerialId = 0;
    StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_SERIAL_ID_POS], eventSerialId);
    for (auto rit = pipelineEventList_.rbegin(); rit != pipelineEventList_.rend(); ++rit) {
        if (rit->eventSerialId == eventSerialId) {
            StringUtil::ConvertStringTo<uint64_t>(eventElements[LOG_ELEMENT_TIMESTAMP_POS], rit->destroyTime);
            rit->pipeline = eventElements[LOG_ELEMENT_PIPE_EVENT_PIPELINE_POS];
            return;
        }
    }
}
} // namespace HiviewDFX
} // namespace OHOS
