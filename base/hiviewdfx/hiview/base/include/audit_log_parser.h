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
#ifndef BASE_HIVIEWDFX_HIVIEW_PLUGIN_MANAGEMENT_AUDIT_LOG_PARSER_H
#define BASE_HIVIEWDFX_HIVIEW_PLUGIN_MANAGEMENT_AUDIT_LOG_PARSER_H
#include <list>
#include <map>
#include <string>
#include <vector>
namespace OHOS {
namespace HiviewDFX {
class AuditLogParser {
public:
    struct EventInfo {
        uint64_t eventSerialId = 0;
        bool isPipelineEvent = false;
        std::string sender = "";
        std::string processor = "";
        std::string threadNameOrTid = "";
        std::string digest = "";
        uint64_t inTime = 0;
        uint64_t outTime = 0;
        bool operator<(const EventInfo &obj) const
        {
            return (this->eventSerialId < obj.eventSerialId);
        };

        std::string ToString()
        {
            return "SerialId:" + std::to_string(eventSerialId) + " Sender:" + sender + " Processor:" + processor +
                   " OnThread:" + threadNameOrTid + " IsPipelineEvent:" + std::to_string(isPipelineEvent) +
                   " TimeCost:" + std::to_string((outTime - inTime)) + " Digest:" + digest;
        };
    };

    struct PipelineEventInfo {
        uint64_t eventSerialId = 0;
        uint64_t createTime = 0;
        uint64_t destroyTime = 0;
        std::string creator = "";
        std::string pipeline = "";
        std::string digest = "";
        // only processor, in and out time is valid
        std::list<EventInfo> processChain;
        bool operator<(const PipelineEventInfo &obj) const
        {
            return (this->eventSerialId < obj.eventSerialId);
        };
        std::string ToString()
        {
            std::string ret = "SerialId:" + std::to_string(eventSerialId) + " Creator:" + creator +
                              " Pipeline:" + pipeline + " TimeCost:" + std::to_string((destroyTime - createTime)) +
                              " Digest:" + digest;
            for (auto &processor : processChain) {
                ret.append(" @plugin:");
                ret.append(processor.processor);
                ret.append("@begin:");
                ret.append(std::to_string(processor.inTime));
                ret.append("@end:");
                ret.append(std::to_string(processor.outTime));
                ret.append("@thread:");
                ret.append(processor.threadNameOrTid);
            }
            return ret;
        };

        PipelineEventInfo() : eventSerialId(0), createTime(0), destroyTime(0), creator(""), pipeline(""), digest(""){};
        explicit PipelineEventInfo(const EventInfo &info)
            : eventSerialId(info.eventSerialId), createTime(info.inTime), creator(info.sender), digest(info.digest){};
    };

public:
    AuditLogParser() : logStartTime_(0), logEndTime_(0), parsed_(false){};
    ~AuditLogParser(){};
    // QUEUE_EVENT_IN event format
    // [timestamp] [eventSerialId] [0] [sender] [processor] [thread] [digest]
    // QUEUE_EVENT_OUT event format
    // [timestamp] [eventSerialId] [1]
    // PIPELINE_EVENT_CREATE event format
    // [timestamp] [eventSerialId] [2] [creator] [digest]
    // PIPELINE_EVENT_HANDLE_IN event format
    // [timestamp] [eventSerialId] [3] [processor]
    // PIPELINE_EVENT_HANDLE_OUT event format
    // [timestamp] [eventSerialId] [4] [thread]
    // PIPELINE_EVENT_DONE event format
    // [timestamp] [eventSerialId] [5] [pipeline]
    void StartParse();
    std::list<std::string> GetThreadSummary(const std::string &name = "");
    std::list<std::string> GetPluginSummary(const std::string &name = "");
    std::list<std::string> GetPipelineSummary(const std::string &name = "");
    // From xxx to xxx
    std::string GetAuditLogTimeScope();

private:
    void ParseTimeScope(const std::list<std::string> &logList);
    void ParseEvent(std::string logStr);
    void ParseNormalAuditEvent(int eventType, const std::vector<std::string> &eventElements);
    void ParseNormalEnqueueEvent(const std::vector<std::string> &eventElements);
    void ParseNormalDequeueEvent(const std::vector<std::string> &eventElements);
    void ParsePipelineAuditEvent(int eventType, const std::vector<std::string> &eventElements);
    void ParsePipelineCreateEvent(const std::vector<std::string> &eventElements);
    void ParsePipelineHandleInEvent(const std::vector<std::string> &eventElements);
    void ParsePipelineHandleOutEvent(const std::vector<std::string> &eventElements);
    void ParsePipelineDoneEvent(const std::vector<std::string> &eventElements);

    uint64_t logStartTime_;
    uint64_t logEndTime_;
    bool parsed_;
    std::map<int, std::string> threadNameMap_;
    std::list<EventInfo> eventList_;
    std::list<PipelineEventInfo> pipelineEventList_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // BASE_HIVIEWDFX_HIVIEW_PLUGIN_MANAGEMENT_AUDIT_LOG_PARSER_H