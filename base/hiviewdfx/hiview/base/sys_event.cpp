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
#include "sys_event.h"

#include <chrono>
#include <regex>
#include <sstream>
#include <string>
#include <sys/time.h>

#include "hilog/log.h"
#include "string_util.h"
#include "time_util.h"

namespace OHOS {
namespace HiviewDFX {
static int GetValueFromJson(const std::string& jsonStr, const std::string& expr, std::string& value)
{
    std::smatch result;
    const std::regex pattern(expr);
    if (std::regex_search(jsonStr, result, pattern)) {
        value = result.str(1);
        return 0;
    }
    return -1;
}

static int GetValueFromJson(const std::string& jsonStr, const std::string& expr, int16_t& value)
{
    std::smatch result;
    const std::regex pattern(expr);
    if (std::regex_search(jsonStr, result, pattern)) {
        value = std::atoi(result.str(1).c_str());
        return 0;
    }
    return -1;
}

static int GetValueFromJson(const std::string& jsonStr, const std::string& expr, uint16_t& value)
{
    std::smatch result;
    const std::regex pattern(expr);
    if (std::regex_search(jsonStr, result, pattern)) {
        value = std::atoi(result.str(1).c_str());
        return 0;
    }
    return -1;
}

static int GetValueFromJson(const std::string& jsonStr, const std::string& expr, int32_t& value)
{
    std::smatch result;
    const std::regex pattern(expr);
    if (std::regex_search(jsonStr, result, pattern)) {
        value = std::atoi(result.str(1).c_str());
        return 0;
    }
    return -1;
}

static int GetValueFromJson(const std::string& jsonStr, const std::string& expr, uint64_t& value)
{
    std::smatch result;
    const std::regex pattern(expr);
    if (std::regex_search(jsonStr, result, pattern)) {
        value = std::atoll(result.str(1).c_str());
        return 0;
    }
    return -1;
}

SysEvent::SysEvent(const std::string& sender, PipelineEventProducer* handler, const std::string& jsonStr)
    : PipelineEvent(sender, handler), seq_(0), pid_(0), tid_(0), uid_(0), tz_(0)
{
    messageType_ = Event::MessageType::SYS_EVENT;
    jsonExtraInfo_ = jsonStr;
}

SysEvent::SysEvent(const std::string& sender, PipelineEventProducer* handler, SysEventCreator& sysEventCreator)
    : SysEvent(sender, handler, sysEventCreator.BuildSysEventJson())
{
    PaserJson();
}

SysEvent::~SysEvent()
{
}

int SysEvent::PaserJson()
{
    if (jsonExtraInfo_.empty()) {
        return -1;
    }
    GetValueFromJson(jsonExtraInfo_, R"~("domain_":"([_\w]+)")~", domain_);
    GetValueFromJson(jsonExtraInfo_, R"~("name_":"([\w_]+)")~", eventName_);
    GetValueFromJson(jsonExtraInfo_, R"~("type_":([\d]+))~", what_);
    GetValueFromJson(jsonExtraInfo_, R"~("time_":([\d]+))~", happenTime_);
    GetValueFromJson(jsonExtraInfo_, R"~("tz_":([\d]+))~", tz_);
    GetValueFromJson(jsonExtraInfo_, R"~("pid_":([\d]+))~", pid_);
    GetValueFromJson(jsonExtraInfo_, R"~("tid_":([\d]+))~", tid_);
    GetValueFromJson(jsonExtraInfo_, R"~("uid_":([\d]+))~", uid_);
    GetValueFromJson(jsonExtraInfo_, R"~("traceid_":"([\w]+)")~", traceId_);
    GetValueFromJson(jsonExtraInfo_, R"~("spanid_":"([\w]+)")~", spanId_);
    GetValueFromJson(jsonExtraInfo_, R"~("pspanid_":"([\w]+)")~", parentSpanId_);
    GetValueFromJson(jsonExtraInfo_, R"~("trace_flag_":"([\w]+)")~", traceFlag_);
    if (domain_.empty()) {
        return -1;
    }
    if (eventName_.empty()) {
        return -1;
    }
    if (what_ == 0) {
        return -1;
    }
    if (happenTime_ == 0) {
        return -1;
    }
    return 0;
}

void SysEvent::SetSeq(int64_t seq)
{
    seq_ = seq;
}

int64_t SysEvent::GetSeq() const
{
    return seq_;
}

int32_t SysEvent::GetPid() const
{
    return pid_;
}

int32_t SysEvent::GetTid() const
{
    return tid_;
}

int32_t SysEvent::GetUid() const
{
    return uid_;
}

int16_t SysEvent::GetTz() const
{
    return tz_;
}

std::string SysEvent::GetEventValue(const std::string& key)
{
    std::string value;
    std::string regexStr = "\"" + key + "\":\"([.\\s\\S\\r\\n]*?)\"";
    GetValueFromJson(jsonExtraInfo_, regexStr, value);
    if (!value.empty() && !key.empty()) {
        SetValue(key, value);
    }
    return value;
}

uint64_t SysEvent::GetEventIntValue(const std::string& key)
{
    uint64_t value = 0;
    std::string regexStr = "\"" + key + "\":(\\d+)"; // "PID":PID
    GetValueFromJson(jsonExtraInfo_, regexStr, value);
    return value;
}

void SysEvent::SetEventValue(const std::string& key, int64_t value)
{
    std::smatch keyMatch;
    std::string keyReplace = "\"" + key + "\":" + std::to_string(value);
    std::regex keyReg("\"" + key + "\":([\\d]*?)");
    if (std::regex_search(jsonExtraInfo_, keyMatch, keyReg)) {
        jsonExtraInfo_ = std::regex_replace(jsonExtraInfo_, keyReg, keyReplace);
        return;
    }

    // new key here
    std::regex newReg("\\{([.\\s\\S\\r\\n]*)\\}");
    std::string newReplace = "{$1,\"" + key + "\":" + std::to_string(value) + "}";
    if (std::regex_search(jsonExtraInfo_, keyMatch, newReg)) {
        jsonExtraInfo_ = std::regex_replace(jsonExtraInfo_, newReg, newReplace);
    }
    else {
        jsonExtraInfo_ = "{\"" + key + "\":" + std::to_string(value) + "}";
    }
    return;
}

void SysEvent::SetEventValue(const std::string& key, const std::string& value, bool append)
{
    // fixme, $1 in value may cause error
    std::smatch keyMatch;
    std::string keyReplace;
    if (append) {
        keyReplace = "\"" + key + "\":\"" + value + ",$1\"";
    }
    else {
        keyReplace = "\"" + key + "\":\"" + value + "\"";
    }
    std::regex keyReg("\"" + key + "\":\"([.\\s\\S\\r\\n]*?)\"");
    if (std::regex_search(jsonExtraInfo_, keyMatch, keyReg)) {
        jsonExtraInfo_ = std::regex_replace(jsonExtraInfo_, keyReg, keyReplace);
        return;
    }

    // new key here
    std::regex newReg("\\{([.\\s\\S\\r\\n]*)\\}");
    auto kvStr = "\"" + key + "\":\"" + value + "\"";
    if (std::regex_search(jsonExtraInfo_, keyMatch, newReg)) {
        auto pos = jsonExtraInfo_.find_last_of("}");
        if (pos == std::string::npos) {
            return;
        }
        jsonExtraInfo_.insert(pos, "," + kvStr);
    } else {
        jsonExtraInfo_ = "{" + kvStr + "}";
    }
    return;
}

SysEventCreator::SysEventCreator(const std::string &domain, const std::string &eventName,
    SysEventCreator::EventType type)
{
    jsonStr_ << "{";
    SetKeyValue("domain_", domain);
    SetKeyValue("name_", eventName);
    SetKeyValue("type_", static_cast<int>(type));
    SetKeyValue("time_", TimeUtil::GetMilliseconds());
    SetKeyValue("tz_", TimeUtil::GetTimeZone());
    SetKeyValue("pid_", getpid());
    SetKeyValue("tid_", gettid());
    SetKeyValue("uid_", getuid());
}

std::string SysEventCreator::BuildSysEventJson()
{
    jsonStr_.seekp(-1, std::ios_base::end);
    jsonStr_ << "}";
    return jsonStr_.str();
}

std::string SysEventCreator::EscapeStringValue(const std::string& value)
{
    return StringUtil::EscapeJsonStringValue(value);
}

std::string SysEventCreator::EscapeStringValue(const char* value)
{
    return StringUtil::EscapeJsonStringValue(value);
}

std::string SysEventCreator::EscapeStringValue(char* value)
{
    return StringUtil::EscapeJsonStringValue(value);
}
} // namespace HiviewDFX
} // namespace OHOS
